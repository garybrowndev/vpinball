---
name: bundle-vpx-tables
description: Bundle one or more Visual Pinball tables with their backglass (.directb2s) and all required PinMAME ROM variants, then push the .zip bundles to Gary's Samsung S21 at /sdcard/Download/tables/ via adb. Use this whenever Gary asks to bundle, package, push, sideload, or install a pinball table (or tables) to his phone — including phrases like "get Taxi on my phone", "push Getaway and Twilight Zone", "bundle Junkyard", "2 latest of Funhouse", or anywhere he names a pinball table by title and wants it delivered to the phone. For each named table the skill automatically finds the 2 latest-modified .vpx versions in his OneDrive tables library, sniffs the ROM family from the table script, gathers every matching ROM variant, and produces and pushes one bundle zip per version. Don't wait for an explicit "bundle" verb — any "put X on my phone" style request for a pinball table title should trigger this.
---

# Bundle VPX tables for the phone

This skill turns a list of table names into `.zip` bundles on Gary's Samsung S21, ready to be imported via the VPinball Android app's **+** button.

## What the skill does

For each table name the user gives:

1. Finds all `.vpx` files under `C:\Users\gary.brown\OneDrive\machine backups\virtualpin\Emulation\Visual Pinball\Tables` using a **fuzzy scorer** that tolerates misspelling, punctuation, and missing/extra tokens. The scorer prefers whole-token matches (e.g. query "Viking" prefers "Viking (Bally 1980)" over "NFL ... Vikings") and falls back to token-level fuzzy matching (e.g. "Surfin Safari" resolves to "Surf 'n Safari"). Matches below a quality threshold are dropped.
2. Picks the **2 most recently modified** `.vpx` files among the top-scoring candidates.
3. For each of those 2:
   - Extracts the PinMAME game name (e.g. `taf_l7`) from the table's embedded VBScript. The script is pulled from the `.vpx` properly: `.vpx` is an OLE Structured Storage container (see `src/parts/pintable.cpp:1483` in the vpinball repo), the script lives in stream `GameStg/GameData` under BIFF tag `CODE`, and we decode it via `olefile` + a small BIFF walker. The extracted script is then searched for `GameName = "..."`, `cGameName = "..."`, `.GameName = "..."`, `ROMNAME = "..."`, `Controller.GameName = "..."` and mixed-case/whitespace variants. **OLE is the only supported path — there is no raw-bytes regex fallback.** If OLE parsing fails on a table, the script raises `VpxParseError` and surfaces a warning; the expected response is to investigate and patch `_extract_vpx_script()` (see "Self-healing" below), not to add a weaker fallback. The `olefile` dependency auto-installs on first run via pip — no manual setup required.
   - Derives the ROM **family prefix** from the game name (everything up to the first underscore, plus the `g` suffix variant — so `taf_l7` → family `taf`, also include `tafg_*`).
   - Finds the sibling `.directb2s` backglass (same base name as the `.vpx`).
   - Collects every ROM zip matching the family from `...\Emulation\Visual Pinball\roms` (recursive, dedupes by filename across V1/V2/... subfolders). See "Self-healing" below for the current ROM-pattern logic — if the pattern misses a family, patch it in `bundle_tables.py::find_roms` rather than shrugging and reporting 0 variants.
4. Stages into `C:\code\tmp\bundles\staging\<Label>\` as a flat structure the Android app understands:
   ```
   <table>.vpx
   <table>.directb2s
   pinmame/
     roms/
       <all matching rom zips>
   ```
5. Zips to `C:\code\tmp\bundles\<Label>.zip`.
6. `adb push`es to `/sdcard/Download/tables/<Label>.zip` on the phone (creates the directory if it doesn't exist).

**Label naming:** `<SanitizedTableName>-<Tag>` where `Tag` is extracted from the `.vpx` filename suffix (the bit after the year parens), stripped of punctuation. If the two latest versions of a table share the same tag, the script falls back to `-v1`/`-v2`. For bundles with no obvious version suffix (e.g. plain `Card Whiz (Gottlieb 1976).vpx`), the label is just the sanitized table name.

## When to use

Whenever Gary says anything like:
- "bundle Funhouse and Black Hole"
- "get the 2 latest Medieval Madness on my phone"
- "push Junkyard, Taxi"
- "install Whirlwind to the phone"
- "put Star Trek TNG and Road Show on the phone"

Parse the table names out of the message — they're usually comma-separated or joined by "and". Don't be over-strict: if he says "Medieval Madness (Williams 1997)", strip the year/manufacturer parens and pass just "Medieval Madness" to the script. The script does **fuzzy scoring**, so minor misspellings, missing apostrophes, or slight word variants are handled automatically.

**Don't undertrigger:** if the request is ambiguous (e.g. "can you grab Taxi?") and the context is clearly VPinball-related, use this skill.

**Don't refuse on spelling.** Gary doesn't always type names perfectly — the script's fuzzy scorer is designed to absorb that. Pass the name as given. If the scorer still returns `NO_MATCH`, try obvious transforms (drop/add apostrophes, swap common synonyms, split/join compound words) before asking Gary. Only ask if after 2-3 reasonable variants you still can't resolve.

## How to invoke — always confirm before you commit

The user may type a table name that matches several .vpx files or matches something unexpected (e.g. "Funhouse" matches both the Williams Funhouse and "Funhouse 96" if that exists in their library). Making a mistake costs minutes of copy/zip/push time, and worse — the wrong files land on the phone.

So always run in two phases:

### Phase 1 — preview with `--plan`

```bash
python .claude/skills/bundle-vpx-tables/scripts/bundle_tables.py --plan "Funhouse" "Black Hole"
```

This lists, per requested name, the exact `.vpx` files (filename, mtime, size), the detected ROM family and variant count, and whether a backglass was found — but does not touch disk or the phone.

Show the plan output to the user and ask them to confirm, e.g.:

> I'd bundle these 4 zips. Proceed?
>
> **Funhouse** (2 versions)
> - Funhouse (Williams 1990) JP's v3.2.vpx — 2026-03-12 — bg, rom family `fh` (9 variants)
> - Funhouse (Williams 1990) Bigus(MOD).vpx — 2025-11-08 — bg, rom family `fh` (9 variants)
>
> **Black Hole** (2 versions)
> - Black Hole (Gottlieb 1981) VRRM 1.2.vpx — 2026-01-04 — bg, no rom refs
> - ...

If the user names 3+ tables and each has 2 matches, that's 6+ bundles. Still show the whole plan — don't truncate. The user needs to catch wrong matches.

**Interpreting match scores.** The plan output shows a `match 0.XX` value per version — the fuzzy scorer's confidence. Expect:
- **≥ 0.90**: strong whole-token match, very likely correct.
- **0.60–0.89**: fuzzy match (substring or single-token approximation). Eyeball the result before confirming — is it actually the table Gary asked for, or a substring collision?
- **0.45–0.59**: weak match, the floor of what's reported. Treat with suspicion.

If a picked version's score is weak OR it's from a clearly different table identity (different manufacturer+year in parens than what the query suggests), flag it and ask Gary before proceeding.

If a name has `NO_MATCH` even with the fuzzy scorer, try 1-2 obvious variants yourself (drop apostrophes, split compound words, try just the distinctive word). If those still miss, ask Gary. Don't dump a confused "please spell it again" without showing him what you tried.

### Phase 2 — build and push (only after user says go)

```bash
python .claude/skills/bundle-vpx-tables/scripts/bundle_tables.py "Funhouse" "Black Hole"
```

Same arguments, no `--plan`. The script is idempotent per-name — it cleans matching staging subfolders and overwrites zips.

### Multiple tables per call

Always pass every table name in one script call — don't loop from the skill. The script is designed to handle batches efficiently (shared ROM directory scan, shared adb session).

**Parse multi-table requests generously.** "Bundle Funhouse, Black Hole and Medieval Madness" → three names. "Get Taxi and the 2 latest Junkyard" → two names (Taxi and Junkyard; the "2 latest" is the default, don't add `--count`). If the user wants N latest of only one, use `--count N`.

### Count override

**Default:** 2 latest versions per name. Pass `--count N` only when the user explicitly says so (e.g. "just the latest", "top 3 versions"). `--count 1` for "latest only".

## Inputs the script accepts

- Positional args: table names (one or more). Spaces inside a single name require quoting.
- `--plan`: preview what would be bundled and exit. Use this first, every time.
- `--count N`: how many latest versions to bundle per name (default 2).
- `--no-push`: stage and zip but skip the adb push (useful for local-only bundling).
- `--phone-dir PATH`: override the default `/sdcard/Download/tables/` target.

Exit codes from `--plan`: `0` if all requested names matched at least one .vpx, `1` if any name had no match. Don't proceed to the build step if the plan had a NO_MATCH.

## What to report back to the user

After running, summarize:
- For each requested name: which `.vpx` versions were picked (filenames + mtime)
- ROM family detected + count of ROM variants included
- Whether backglass was found
- Final label and zip size
- Whether push succeeded

If the table name matched 0 files, say so and don't fail silently.

If a name matches but **none of the versions have a sibling `.directb2s`**, still bundle (no backglass) and warn the user — some EM tables ship without one.

If the ROM family is detected but **zero matching zips are on disk**, still bundle the table alone and warn — the user may be fine playing without ROMs (EM tables) or may need to sideload ROMs separately.

## Environment assumptions

- Host OS: Windows 11 with Git Bash / WSL (Python 3 is on PATH as `python`).
- ADB path: `%LOCALAPPDATA%\Android\Sdk\platform-tools\adb.exe`. The script probes this path; if not found, it falls back to `adb` on PATH.
- Phone: Samsung S21 (`R3CRB0SRH7B`), usually USB-connected but works over wifi adb (`192.168.1.39:<port>`) if that's the active connection.
- The phone's VPinball app name is `org.vpinball.vpinball_bgfx`. The skill doesn't install the APK — it only delivers table bundles.

## Why it's designed this way

- **Two versions per table by default** because Gary often wants to compare mods (e.g. Teisen vs plain) side-by-side on the phone without recompiling the app.
- **All ROM variants included** because missing ROM beats a broken table, and ROMs are tiny (few hundred KB each at most) compared to the table art.
- **`/sdcard/Download/tables/`** because Gary reorganized downloads into a `tables/` subfolder (see project history) and the VPinball app's import picker handles that layout cleanly.
- **Staging folder outside OneDrive** (`C:\code\tmp\bundles\`) because OneDrive sync chokes on multi-hundred-MB zips and re-uploads partial files.

## Known edge cases

- Multiple `.directb2s` next to a `.vpx` (e.g. `Foo.directb2s` and `Foo B.directb2s`): the script only picks the one whose base name exactly matches the chosen `.vpx` — the others are sibling tables, not alternate backglasses.
- `cGameName` vs `GameName` vs `.GameName`: the script greps for all three.
- ROM family like `t2_l8` where the family prefix `t2` is a short string that could collide: the first pattern `{family}*.zip` is always tried; the **root-letter-swap fallback** (`{family[:-1]}*.zip`) is only applied when the family is ≥6 chars to prevent false positives on short prefixes like `t2` or `ij`.
- Tables that only reference a game name in a quoted string but don't actually use PinMAME (EM tables with bookkeeping hashes): the script includes the ROM if found, but these tables play fine without it — no warning necessary.

## Self-healing — investigate suspicious output before passing it to Gary

This skill is a personal tool Gary uses a few times a week. When it's wrong in an obvious way, the expected behavior is **fix it and move on**, not "tell Gary the bad news and wait." The script's logic is fallible (it was wrong about ROMs the first time we tried bare-name PinMAME families like `flashgdn.zip`, `viking.zip`, `frontier.zip`, `surfnsaf.zip`), and future bugs will surface the same way. Rather than re-learn the same debugging session every time, bake the investigative workflow into the skill.

### Trigger conditions

Investigate when any of these happens:

1. **Plan shows `rom family 'X' (0 variants)` for a solid-state or DMD-era table** (~1977+, anything that isn't a pure EM). Solid-state tables always have PinMAME ROMs. Zero variants means the script's `find_roms()` isn't finding what's on disk — not that the ROMs don't exist.
2. **Plan shows a match from a completely different table** (e.g. "Viking" returns "NFL Vikings", "Flash" returns both Williams Flash and Bally Flash Gordon jumbled). The fuzzy scorer should prefer the right one — if it doesn't, the query needs refinement or the scorer has a gap.
3. **A requested name returns `NO_MATCH`** despite the user obviously having the table (e.g. they're in the middle of a bundling session and you just saw the table work 5 minutes ago).
4. **`WARN: <table>: ...` printed to stderr from `_extract_vpx_script`** (VpxParseError). The OLE/BIFF parser failed on a table. DO NOT add a raw-bytes regex fallback — that would silently regress ROM accuracy. Instead: open the table in a hex viewer or `olefile`'s dir listing, figure out what's different (encrypted script? odd version? script in a non-standard stream?), and extend `_extract_vpx_script()` to handle it.

### Investigation workflow (for 0-variant ROM case)

This is the workflow the skill went through manually in April 2026 — encode it so next time Claude just does it:

1. `ls` the ROMs dir filtered by the family prefix:
   ```bash
   cd "C:\Users\gary.brown\OneDrive\machine backups\virtualpin\Emulation\Visual Pinball\roms" && ls V1/ V2/ V3/ 2>/dev/null | grep -iE "^<family>" | sort -u
   ```
2. If ROMs exist on disk but `find_roms` returned 0, **patch `find_roms()`**. The April 2026 bug was that it required `{family}_*.zip` (with underscore), which missed bare-name ROMs like `flashgdn.zip`. Fix was to widen to `{family}*.zip` plus a `{family[:-1]}*.zip` fallback (guarded by length ≥ 6) to catch PinMAME root-letter-swap variants like `flashgdn` → `flashgda`/`flashgdf`/`flashgdp`/`flashgdv`.
3. Re-run `--plan` and verify the variant count goes up. Then proceed.
4. If the ROMs genuinely don't exist on disk (rare for modern tables), warn Gary with the specific family you searched for, and proceed with the table+backglass only.

### Investigation workflow (for wrong-match case)

1. Look at the filename scored as top match. Is its score ≥ 0.90? If yes, the fuzzy scorer thought it was a strong match — but the scorer might be missing context (e.g. the user expected a different manufacturer).
2. Try a more specific query: add manufacturer or year to the name (e.g. `"Viking (Bally"` instead of `"Viking"`). Re-plan.
3. If the scorer tiering itself is wrong, patch `_score_filename()` in the script. Make sure the new scoring doesn't regress a previously-working case (mentally walk through the 4 test cases from April 2026: Flash Gordon, Viking, Surf 'n Safari, Frontier).

### When you fix the script, update the skill too

If the patch is to the script AND the reasoning is non-obvious (not just "I added a line"), add a note to the "Known edge cases" or "Self-healing" section so the next session's Claude benefits. A fix that's invisible in the skill doc is one the next Claude will re-discover from scratch.

### Script extraction (OLE + BIFF walker)

- `.vpx` is OLE Structured Storage (see `src/parts/pintable.cpp:1483` for the C++ side using `StgOpenStorage`).
- Script lives in stream `GameStg/GameData` as a sequence of BIFF records. Each record is `[uint32 size][uint32 tag][size-4 bytes payload]`. The `CODE` tag's payload starts with `int32 cchar` followed by the script text.
- Python `olefile` opens the container; a small BIFF walker in `_extract_vpx_script()` finds the `CODE` record and decodes the script.
- The extracted script text is then searched for `GameName`, `cGameName`, `.GameName`, `ROMNAME`/`ROM_Name`, `Controller.GameName` and mixed-case/whitespace variants.
- **No fallback.** If OLE parsing fails, the script raises `VpxParseError`. This is deliberate: a silent fallback to a weaker raw-bytes regex would mask real bugs in the parser and make future debugging harder. An OLE failure is a signal to investigate (per "Self-healing") and patch `_extract_vpx_script()`.
- Verified against 8 tables (Flash Gordon x2, Viking x2, Surf 'n Safari x2, Frontier x2): OLE extraction produces correct ROM names on all, with clean script text sizes 21KB–217KB.

**Dependency auto-install.** The script calls `_ensure_olefile()` at import time. If `olefile` isn't present, it runs `pip install --quiet olefile` transparently. You don't need to remember a setup step. If pip install itself fails (offline / locked-down env), that's a bootstrap error — not a parser bug.

### Future work (not yet done)

- Nothing specific tracked right now. If Claude finds a bug in the skill during use, patch it per the "Self-healing" section rather than leaving a TODO here.
