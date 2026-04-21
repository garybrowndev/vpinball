#!/usr/bin/env python3
"""Bundle one or more VPX tables with backglass + ROMs and push to the S21 at /sdcard/Download/tables/.

Usage:
    python bundle_tables.py "Funhouse" "Black Hole"
    python bundle_tables.py --count 1 "Taxi"
    python bundle_tables.py --no-push "Medieval Madness"

Reads the OneDrive tables + roms libraries, matches table names with a fuzzy scorer
(tolerates misspelling / punctuation), extracts the PinMAME GameName from each .vpx's
embedded VBScript via proper OLE Structured Storage + BIFF parsing (olefile auto-installs
on first run), collects every ROM variant in the matching family, zips it with the
backglass, and pushes via adb. No raw-bytes fallback — OLE failures surface as warnings
so the parser can be patched rather than papered over.
"""

from __future__ import annotations

import argparse
import difflib
import os
import re
import shutil
import struct
import subprocess
import sys
import zipfile
from pathlib import Path

def _ensure_olefile():
    """Import olefile, installing it from pip on first use if missing.

    olefile is the only supported way to read .vpx scripts — no raw-bytes fallback. We
    auto-install so this skill works out of the box on any machine with Python + pip,
    without making the user remember a setup step."""
    try:
        import olefile  # noqa: F401
        return olefile
    except ImportError:
        pass
    print("bundle_tables: installing missing dependency 'olefile' via pip...", file=sys.stderr)
    subprocess.check_call([sys.executable, "-m", "pip", "install", "--quiet", "--disable-pip-version-check", "olefile"])
    import olefile  # noqa: F401
    return olefile


olefile = _ensure_olefile()

# --- paths -------------------------------------------------------------------

ONEDRIVE = Path(os.environ["USERPROFILE"]) / "OneDrive" / "machine backups" / "virtualpin" / "Emulation" / "Visual Pinball"
TABLES_DIR = ONEDRIVE / "Tables"
ROMS_DIR = ONEDRIVE / "roms"
BUNDLE_ROOT = Path(r"C:\code\tmp\bundles")
STAGING_ROOT = BUNDLE_ROOT / "staging"

ADB_DEFAULT = Path(os.environ["LOCALAPPDATA"]) / "Android" / "Sdk" / "platform-tools" / "adb.exe"


def find_adb() -> str:
    if ADB_DEFAULT.exists():
        return str(ADB_DEFAULT)
    # Fallback to PATH
    return "adb"


# --- vpx / backglass / rom discovery -----------------------------------------

# Patterns searched inside the extracted script text. We trust the script boundary (the CODE
# chunk payload) — there's no risk of false matches against stray binary blobs elsewhere in
# the .vpx. Covers: GameName = "x", cGameName = "x", .GameName = "x", Const cGameName = "x",
# ROMNAME = "x", cROMName = "x", Controller.GameName = "x", mixed-case/whitespace variants.
_SCRIPT_GAMENAME_RES = [
    re.compile(r'(?:^|[^A-Za-z_])(?:c?GameName|\.GameName)\s*=\s*"([A-Za-z0-9_]+)"', re.IGNORECASE | re.MULTILINE),
    re.compile(r'(?:^|[^A-Za-z_])(?:c?ROM[_ ]?Name)\s*=\s*"([A-Za-z0-9_]+)"', re.IGNORECASE | re.MULTILINE),
    re.compile(r'Controller\.GameName\s*=\s*"([A-Za-z0-9_]+)"', re.IGNORECASE),
]

# Tag encoding: FourCC as little-endian uint32. "CODE" little-endian = bytes 'C','O','D','E'.
_CODE_TAG = int.from_bytes(b"CODE", "little")
_ENDB_TAG = int.from_bytes(b"ENDB", "little")


class VpxParseError(Exception):
    """Raised when a .vpx can't be parsed as OLE or its script can't be located. We surface
    this rather than silently falling back — if OLE fails on a table we haven't seen before,
    that's a real bug to investigate (see the self-healing section in SKILL.md), not
    something to paper over with a less-accurate regex."""


def _extract_vpx_script(vpx: Path) -> str:
    """Open the .vpx as OLE Structured Storage, walk the BIFF records in GameStg/GameData,
    and return the embedded VBScript text.

    Format (matches BiffReader::AsObject + AsScript in src/utils/BiffReader.cpp):
      Stream = series of records. Each record:
        uint32 size     — total bytes of this record INCLUDING the tag, before payload data.
        uint32 tag      — FourCC (e.g. b'CODE', b'NAME', b'ENDB').
        (size - 4) bytes of payload.
      CODE payload:
        int32 cchar     — length of script text in bytes.
        cchar bytes of script text (may be null-terminated).
    Raises VpxParseError if the script can't be located.
    """
    if not olefile.isOleFile(str(vpx)):
        raise VpxParseError(f"{vpx.name}: not an OLE file (unexpected VPX format)")
    try:
        with olefile.OleFileIO(str(vpx)) as ole:
            if not ole.exists("GameStg/GameData"):
                raise VpxParseError(f"{vpx.name}: missing GameStg/GameData stream")
            data = ole.openstream("GameStg/GameData").read()
    except (OSError, IOError) as e:
        raise VpxParseError(f"{vpx.name}: OLE read failed: {e}") from e

    pos = 0
    end = len(data)
    while pos + 8 <= end:
        (rec_size,) = struct.unpack_from("<I", data, pos)
        if rec_size < 4 or pos + 4 + rec_size > end:
            raise VpxParseError(f"{vpx.name}: malformed BIFF record at offset {pos} (size={rec_size}, stream={end}B)")
        (tag,) = struct.unpack_from("<I", data, pos + 4)
        if tag == _ENDB_TAG:
            raise VpxParseError(f"{vpx.name}: hit ENDB without finding CODE chunk")
        if tag == _CODE_TAG:
            if pos + 8 + 4 > end:
                raise VpxParseError(f"{vpx.name}: truncated CODE record (no cchar field)")
            (cchar,) = struct.unpack_from("<i", data, pos + 8)
            if cchar <= 0 or pos + 8 + 4 + cchar > end:
                raise VpxParseError(f"{vpx.name}: invalid CODE cchar={cchar}")
            raw = data[pos + 8 + 4 : pos + 8 + 4 + cchar]
            return raw.rstrip(b"\x00").decode("utf-8", errors="replace")
        pos += 4 + rec_size  # skip to next record header
    raise VpxParseError(f"{vpx.name}: CODE chunk not found in {end}B stream")


def sniff_rom_name(vpx: Path) -> str | None:
    """Return the PinMAME GameName / ROM name referenced by this table's script.

    OLE-only — no raw-bytes fallback. If OLE parsing fails, VpxParseError is raised so the
    caller surfaces the real problem instead of quietly regressing to a weaker match.
    Returns None only if the script parses cleanly but contains no recognizable ROM reference
    (which is normal for pure-EM tables)."""
    script = _extract_vpx_script(vpx)
    for pattern in _SCRIPT_GAMENAME_RES:
        m = pattern.search(script)
        if m:
            return m.group(1)
    return None


def rom_family_prefix(game_name: str) -> str:
    """From 'taf_l7' return 'taf'. Handles names without underscore by returning them whole."""
    return game_name.split("_", 1)[0]


_TOKEN_RE = re.compile(r"[a-z0-9]+")

MATCH_THRESHOLD = 0.45  # filename scores below this never make the candidate list


def _score_filename(query: str, filename: str) -> float:
    """Return a 0..1 match score for `query` against `filename`.

    Tiered to prefer strong signals (all query words appear as whole tokens in the
    filename) over weaker ones (fuzzy substring matches on a single token). The
    tiering is what keeps 'Viking' from pulling 'NFL ... Vikings' above the real
    'Viking (Bally 1980)' — whole-token hits beat plural/substring hits.
    """
    q = query.lower().strip()
    f = filename.lower()
    q_tokens = _TOKEN_RE.findall(q)
    f_tokens = _TOKEN_RE.findall(f)
    if not q_tokens or not f_tokens:
        return 0.0

    # Tier 1: every query token is a whole-word token in the filename.
    if all(qt in f_tokens for qt in q_tokens):
        return 0.95 if q.replace(" ", "") in f.replace(" ", "") else 0.90

    # Tier 2: full query is a substring of the filename.
    if q in f:
        return 0.85

    # Tier 3: every query token has a fuzzy match in the filename tokens (ratio >= 0.8).
    best_per_qt = [max((difflib.SequenceMatcher(None, qt, ft).ratio() for ft in f_tokens), default=0.0) for qt in q_tokens]
    strong_hits = [r for r in best_per_qt if r >= 0.80]
    if len(strong_hits) == len(q_tokens):
        return 0.60 + 0.10 * (sum(strong_hits) / len(strong_hits))

    # Tier 4: at least half the query tokens fuzzy-hit. Lower-confidence signal.
    need = max(1, len(q_tokens) // 2)
    if len(strong_hits) >= need:
        return 0.45 + 0.05 * (sum(strong_hits) / len(strong_hits))

    # Tier 5: fall back to full-string sequence ratio (rarely useful, but non-zero).
    return difflib.SequenceMatcher(None, q, f).ratio() * 0.40


def find_tables(name: str, count: int) -> list[Path]:
    """Find .vpx files matching the user's query, tolerating misspelling and punctuation.

    Scores every .vpx with `_score_filename`, drops anything below MATCH_THRESHOLD, then
    returns the top `count` by (score DESC, mtime DESC). Substring-only is deliberately
    avoided because it's too permissive (e.g. 'Viking' substring-matches 'NFL Vikings')
    and too strict (e.g. 'Surfin Safari' misses "Surf 'n Safari")."""
    all_vpx = list(TABLES_DIR.glob("*.vpx"))
    scored = [(p, _score_filename(name, p.name)) for p in all_vpx]
    scored = [(p, s) for p, s in scored if s >= MATCH_THRESHOLD]
    scored.sort(key=lambda ps: (ps[1], ps[0].stat().st_mtime), reverse=True)
    return [p for p, _s in scored[:count]]


def score_tables(name: str, count: int) -> list[tuple[Path, float]]:
    """Same as find_tables but returns (path, score) tuples so the plan output
    can surface the match quality for the caller to sanity-check."""
    all_vpx = list(TABLES_DIR.glob("*.vpx"))
    scored = [(p, _score_filename(name, p.name)) for p in all_vpx]
    scored = [(p, s) for p, s in scored if s >= MATCH_THRESHOLD]
    scored.sort(key=lambda ps: (ps[1], ps[0].stat().st_mtime), reverse=True)
    return scored[:count]


def find_backglass(vpx: Path) -> Path | None:
    """Matching .directb2s is the one with the EXACT same stem as the .vpx."""
    candidate = vpx.with_suffix(".directb2s")
    return candidate if candidate.exists() else None


def find_roms(family: str) -> list[Path]:
    """Return deduped list of ROM zips matching the PinMAME family.

    Matches:
      - `{family}*.zip`   — bare name (flashgdn.zip), versioned (taf_l7.zip), and `g` variants (tafg_*.zip).
      - `{family[:-1]}[a-z]*.zip` (for families >=6 chars only) — catches root-letter-swap PinMAME
        variants like flashgdn -> flashgda/flashgdf/flashgdp/flashgdv, or vikingb -> viking.
        Guarded by length >=6 to avoid false positives on short prefixes like `t2` or `ij`.
    """
    patterns = [f"{family}*.zip"]
    if len(family) >= 6:
        patterns.append(f"{family[:-1]}*.zip")
    seen: dict[str, Path] = {}
    for pattern in patterns:
        for p in ROMS_DIR.rglob(pattern):
            seen.setdefault(p.name, p)
    return sorted(seen.values(), key=lambda p: p.name.lower())


# --- label derivation --------------------------------------------------------

_SANITIZE_RE = re.compile(r"[^A-Za-z0-9]+")


def sanitize(text: str) -> str:
    return _SANITIZE_RE.sub("", text)


def derive_label(vpx: Path, query: str) -> tuple[str, str]:
    """Return (sanitized_base, version_tag).

    base  = the user's query, sanitized (e.g. 'AddamsFamily')
    tag   = the filename chunk AFTER the year parens, sanitized (e.g. 'JPsv600' from
            'The Addams Family (Bally 1992) JP's v600.vpx'). Empty if no chunk."""
    base = sanitize(query)
    stem = vpx.stem
    # Everything after the last ')'
    if ")" in stem:
        tail = stem.split(")")[-1]
    else:
        tail = stem
    tag = sanitize(tail)
    return base, tag


# --- staging + zipping -------------------------------------------------------

def stage_bundle(vpx: Path, label: str, rom_family: str | None) -> Path:
    """Copy table + backglass + rom family into a flat staging folder. Return the folder."""
    stage = STAGING_ROOT / label
    if stage.exists():
        shutil.rmtree(stage)
    stage.mkdir(parents=True)

    shutil.copy2(vpx, stage / vpx.name)

    bg = find_backglass(vpx)
    bg_found = bool(bg)
    if bg:
        shutil.copy2(bg, stage / bg.name)

    rom_count = 0
    if rom_family:
        roms = find_roms(rom_family)
        if roms:
            roms_dir = stage / "pinmame" / "roms"
            roms_dir.mkdir(parents=True)
            for r in roms:
                shutil.copy2(r, roms_dir / r.name)
            rom_count = len(roms)

    # Stash summary
    (stage / "_bundle_info.txt").write_text(
        f"vpx: {vpx.name}\n"
        f"mtime: {vpx.stat().st_mtime}\n"
        f"backglass: {bg.name if bg else '(none)'}\n"
        f"rom_family: {rom_family or '(none)'}\n"
        f"rom_count: {rom_count}\n",
        encoding="utf-8",
    )
    return stage


def zip_bundle(stage: Path, label: str) -> Path:
    out = BUNDLE_ROOT / f"{label}.zip"
    if out.exists():
        out.unlink()
    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED, compresslevel=6) as zf:
        for f in stage.rglob("*"):
            if f.is_file() and f.name != "_bundle_info.txt":
                zf.write(f, f.relative_to(stage))
    return out


# --- adb push ----------------------------------------------------------------

def adb_push(adb: str, local: Path, phone_dir: str) -> tuple[bool, str]:
    """mkdir -p the target dir, then push. Returns (ok, combined_output)."""
    env = os.environ.copy()
    env["MSYS_NO_PATHCONV"] = "1"  # avoid Git Bash mangling /sdcard/...
    mkdir_cmd = [adb, "shell", "mkdir", "-p", phone_dir]
    push_cmd = [adb, "push", str(local), f"{phone_dir.rstrip('/')}/{local.name}"]
    parts: list[str] = []
    try:
        r = subprocess.run(mkdir_cmd, env=env, capture_output=True, text=True, timeout=30)
        parts.append(f"$ {' '.join(mkdir_cmd)}\n{r.stdout}{r.stderr}".strip())
        r = subprocess.run(push_cmd, env=env, capture_output=True, text=True, timeout=600)
        parts.append(f"$ {' '.join(push_cmd)}\n{r.stdout}{r.stderr}".strip())
        return r.returncode == 0, "\n".join(parts)
    except subprocess.SubprocessError as e:
        return False, f"adb error: {e}"


# --- main flow ---------------------------------------------------------------

def process_name(
    query: str,
    count: int,
    do_push: bool,
    phone_dir: str,
    adb: str,
) -> list[dict]:
    """Bundle (and optionally push) the `count` latest VPXes matching `query`.
    Returns one result dict per version handled."""
    results: list[dict] = []
    matches = find_tables(query, count)
    if not matches:
        return [{"query": query, "status": "NO_MATCH"}]

    base, _ = derive_label(matches[0], query)
    # Pre-compute tags to detect duplicates and apply v1/v2 fallback
    tags = [derive_label(v, query)[1] for v in matches]
    used_labels: set[str] = set()

    for idx, vpx in enumerate(matches):
        tag = tags[idx]
        # If this tag is a duplicate of another one, fall back to index-based
        if tag == "" or tags.count(tag) > 1:
            tag = f"v{idx + 1}"
        label = f"{base}-{tag}" if tag else base
        # Guarantee uniqueness in this run
        suffix = 1
        original_label = label
        while label in used_labels:
            suffix += 1
            label = f"{original_label}{suffix}"
        used_labels.add(label)

        try:
            game = sniff_rom_name(vpx)
        except VpxParseError as e:
            # Surface loudly — this is the self-healing trigger: investigate and patch
            # _extract_vpx_script rather than silently continuing with no ROM.
            print(f"WARN: {e}  (skipping ROM bundle for this version; fix the parser per SKILL.md self-healing section)", file=sys.stderr)
            game = None
        family = rom_family_prefix(game) if game else None
        stage = stage_bundle(vpx, label, family)
        zpath = zip_bundle(stage, label)

        roms = find_roms(family) if family else []
        bg = find_backglass(vpx)

        result = {
            "query": query,
            "vpx": str(vpx),
            "vpx_name": vpx.name,
            "mtime": vpx.stat().st_mtime,
            "rom_name": game,
            "rom_family": family,
            "rom_count": len(roms),
            "has_backglass": bg is not None,
            "label": label,
            "zip_path": str(zpath),
            "zip_size": zpath.stat().st_size,
            "pushed": False,
            "push_log": "",
            "status": "OK",
        }

        if do_push:
            ok, log = adb_push(adb, zpath, phone_dir)
            result["pushed"] = ok
            result["push_log"] = log
            if not ok:
                result["status"] = "PUSH_FAILED"

        results.append(result)
    return results


def human_size(n: int) -> str:
    size = float(n)
    for unit in ("B", "KB", "MB", "GB"):
        if size < 1024:
            return f"{size:.0f} B" if unit == "B" else f"{size:.1f} {unit}"
        size /= 1024
    return f"{size:.1f} TB"


def plan_for_names(names: list[str], count: int) -> list[dict]:
    """Return the list of versions that WOULD be bundled, without touching disk."""
    out: list[dict] = []
    for name in names:
        scored = score_tables(name, count)
        if not scored:
            out.append({"query": name, "status": "NO_MATCH"})
            continue
        for vpx, score in scored:
            base, tag = derive_label(vpx, name)
            try:
                game = sniff_rom_name(vpx)
            except VpxParseError as e:
                print(f"WARN: {e}  (plan continues but this table will have no ROMs; fix the parser per SKILL.md self-healing section)", file=sys.stderr)
                game = None
            family = rom_family_prefix(game) if game else None
            roms = find_roms(family) if family else []
            bg = find_backglass(vpx)
            import datetime
            mtime = datetime.datetime.fromtimestamp(vpx.stat().st_mtime).strftime("%Y-%m-%d %H:%M")
            out.append({
                "query": name,
                "vpx_name": vpx.name,
                "vpx_size": vpx.stat().st_size,
                "mtime_human": mtime,
                "match_score": score,
                "rom_name": game,
                "rom_family": family,
                "rom_count": len(roms),
                "has_backglass": bg is not None,
                "derived_tag": tag or "(none)",
                "status": "OK",
            })
    return out


def print_plan(plan: list[dict]) -> None:
    """Human-readable output of what would be bundled — formatted for caller to show user."""
    by_query: dict[str, list[dict]] = {}
    for p in plan:
        by_query.setdefault(p["query"], []).append(p)
    print("=== Plan ===")
    any_match = False
    for query, items in by_query.items():
        if items and items[0].get("status") == "NO_MATCH":
            print(f"\n[{query}] NO MATCH in {TABLES_DIR}")
            continue
        any_match = True
        print(f"\n[{query}] - {len(items)} version(s) to bundle:")
        for p in items:
            bg = "bg" if p["has_backglass"] else "NO-bg"
            if p["rom_family"]:
                rom = f"rom family '{p['rom_family']}' ({p['rom_count']} variant" + ("s" if p["rom_count"] != 1 else "") + ")"
            else:
                rom = "no rom refs"
            score = p.get("match_score")
            score_str = f"match {score:.2f}" if score is not None else ""
            print(f"  - {p['vpx_name']}")
            print(f"      mtime: {p['mtime_human']}  size: {human_size(p['vpx_size'])}  [{bg}, {rom}, {score_str}]")
    if not any_match:
        print("\n(No matches anywhere — nothing to bundle.)")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("names", nargs="+", help="Table name(s) to match (case-insensitive substring)")
    ap.add_argument("--count", type=int, default=2, help="Latest N versions per name (default 2)")
    ap.add_argument("--plan", action="store_true",
                    help="Preview only: print what would be bundled and exit (no staging, no zip, no push)")
    ap.add_argument("--no-push", action="store_true", help="Stage and zip but skip the adb push")
    ap.add_argument("--phone-dir", default="/sdcard/Download/tables", help="Target dir on phone")
    args = ap.parse_args()

    if not TABLES_DIR.exists():
        print(f"ERROR: tables dir not found: {TABLES_DIR}", file=sys.stderr)
        return 2

    if args.plan:
        plan = plan_for_names(args.names, args.count)
        print_plan(plan)
        # Non-zero exit if any name had NO_MATCH, so the caller can detect it
        return 1 if any(p.get("status") == "NO_MATCH" for p in plan) else 0

    BUNDLE_ROOT.mkdir(parents=True, exist_ok=True)
    STAGING_ROOT.mkdir(parents=True, exist_ok=True)
    adb = find_adb()

    all_results: list[dict] = []
    for name in args.names:
        all_results.extend(process_name(name, args.count, not args.no_push, args.phone_dir, adb))

    # Summary
    print("\n=== Summary ===")
    for r in all_results:
        if r["status"] == "NO_MATCH":
            print(f"- {r['query']}: NO MATCH in {TABLES_DIR}")
            continue
        bg = "bg" if r["has_backglass"] else "NO-bg"
        roms = f"{r['rom_count']} rom" + ("s" if r["rom_count"] != 1 else "")
        if r["rom_count"] == 0 and r["rom_family"]:
            roms = f"rom family '{r['rom_family']}' detected but 0 zips on disk"
        elif not r["rom_family"]:
            roms = "no rom refs (EM?)"
        push = ""
        if r["pushed"]:
            push = " -> pushed to phone"
        elif r["status"] == "PUSH_FAILED":
            push = " -> PUSH FAILED (see log below)"
        print(f"- {r['label']}  ({bg}, {roms}, {human_size(r['zip_size'])}){push}")
        print(f"    from: {r['vpx_name']}")

    # Dump push logs for failures
    failed = [r for r in all_results if r["status"] == "PUSH_FAILED"]
    if failed:
        print("\n=== Push failures ===")
        for r in failed:
            print(f"\n{r['label']}:\n{r['push_log']}")
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
