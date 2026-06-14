#!/usr/bin/env bash
# sync-plan.sh — READ-ONLY preview of the upstream→down sync round.
# Touches nothing: fetches (non-destructive) and reports what a full sync-vpx
# round would do. Run this in Phase 0 before any rebase/merge/push.
#
# Reports: upstream-ahead count, the local patches master will replay, working
# tree state, active gh account, and which incoming upstream commits touch
# Ball-History integration files (the likely-conflict preview for the
# master→integration merge).

set -u

cd "$(git rev-parse --show-toplevel 2>/dev/null)" || {
  echo "ERROR: not inside a git repo" >&2
  exit 1
}

git fetch --quiet --prune origin 2>/dev/null || true
git fetch --quiet --prune upstream 2>/dev/null || true

GH=""
for c in gh gh.exe \
         "/c/Users/$USER/AppData/Local/Microsoft/WinGet/Links/gh.exe" \
         "/mnt/c/Users/gary.brown/AppData/Local/Microsoft/WinGet/Links/gh.exe"; do
  if command -v "$c" >/dev/null 2>&1 || [ -x "$c" ]; then GH="$c"; break; fi
done

echo "# sync-vpx — round preview ($(date '+%Y-%m-%d %H:%M'))"
echo ""

# ---- 0. working tree must be clean ----
dirty=$(git status --porcelain | wc -l | tr -d ' ')
cur=$(git rev-parse --abbrev-ref HEAD)
echo "## Preflight"
echo "- HEAD: \`$cur\`"
if [ "$dirty" = "0" ]; then
  echo "- Working tree: clean ✓"
else
  echo "- Working tree: **$dirty uncommitted change(s)** ⚠ — commit or stash before the round (a rebase/merge with a dirty tree loses work)."
fi

# active gh account (Phase 3 dep pull needs garybrowndev). gh prints one block
# per account; the active one has "Active account: true" under its "Logged in
# ... account NAME" line. Pair them with awk to print just the active name.
if [ -n "$GH" ]; then
  acct=$("$GH" auth status 2>&1 | awk '
    /account [^ ]+ \(keyring\)/ { name=$0; sub(/.*account /,"",name); sub(/ .*/,"",name) }
    /Active account: true/      { print name; exit }')
  if [ "$acct" = "garybrowndev" ]; then
    echo "- Active \`gh\` account: \`$acct\` ✓ (ready for Phase 3 fork dep pull)"
  else
    echo "- Active \`gh\` account: \`${acct:-unknown}\` — Phase 3 dep pull needs \`garybrowndev\` (\`gh auth switch --user garybrowndev\`)"
  fi
fi
echo ""

# ---- 1. upstream → master ----
if git show-ref --verify --quiet refs/remotes/upstream/master \
   && git show-ref --verify --quiet refs/heads/master; then
  n=$(git rev-list --count master..upstream/master 2>/dev/null || echo 0)
  echo "## Phase 1 — master ← upstream"
  if [ "$n" = "0" ]; then
    echo "- **0 commits** — fork is already current with upstream. Nothing to sync. ✓"
    echo ""
    echo "_(Stop here unless you specifically want to re-merge down through integration/development.)_"
    exit 0
  fi
  echo "- **$n commit(s)** to pull in (range \`master..upstream/master\`)."
  echo "- Local patches that will REPLAY on top (must all survive the rebase):"
  git log upstream/master..master --oneline --no-merges | sed 's/^/    - /'
  echo ""
fi

# ---- 2. conflict-risk preview for master → integration ----
echo "## Phase 2 — master → integration (conflict-risk preview)"
echo "Incoming upstream commits that touch Ball-History integration files —"
echo "expect to re-integrate these by hand:"
echo ""
risk=0
for f in \
  src/core/player.cpp src/core/player.h \
  src/ui/live/LiveUI.cpp \
  src/input/InputManager.cpp \
  src/physics/hitball.h \
  src/parts/pintable.cpp src/parts/pintable.h ; do
  c=$(git rev-list --count master..upstream/master -- "$f" 2>/dev/null || echo 0)
  if [ "$c" != "0" ]; then
    echo "- \`$f\` — $c upstream commit(s) touch it ⚠"
    risk=$((risk+1))
  fi
done
if [ "$risk" = "0" ]; then
  echo "- None of the known integration-point files were touched upstream — merge should be light. ✓"
  echo "  _(Still re-check that the Ball-History API calls compile against any upstream renames.)_"
fi
echo ""

# ---- 3-5. reminders ----
echo "## Phases 3–5 — build, verify, merge down"
echo "- Phase 3: refresh fork deps for the new commit (gh on \`garybrowndev\`) → CMake build (\`/m:9\`)."
echo "- Phase 4: deploy to cabinet via debug-vpx → Gary play-tests (B2S boot + Ball History V/R). ⛔ gate."
echo "- Phase 5: merge integration → development → push. ⛔ checkpoint before push."
echo ""
echo "_Every push (esp. the Phase 1 \`--force-with-lease\` on master) is a checkpoint — get Gary's OK first._"
