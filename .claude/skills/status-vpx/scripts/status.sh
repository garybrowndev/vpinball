#!/usr/bin/env bash
# status-vpx — print a structured status of the vpinball_ballhistory repo.
# Read-only. Runs from anywhere inside the repo.

set -u

cd "$(git rev-parse --show-toplevel 2>/dev/null)" || {
  echo "ERROR: not inside a git repo" >&2
  exit 1
}

# Always have an up-to-date view of remotes — but never fetch destructively.
# Prune stale remote-tracking refs so the report doesn't lie.
git fetch --quiet --prune origin 2>/dev/null || true
git fetch --quiet --prune upstream 2>/dev/null || true

hr() { printf -- '----------------------------------------\n'; }

# ---- header ----
echo "Repo:    $(git rev-parse --show-toplevel)"
echo "Date:    $(date '+%Y-%m-%d %H:%M:%S')"
echo "Branch:  $(git rev-parse --abbrev-ref HEAD)"
hr

# ---- working-tree state ----
echo "## Working tree"
if [ -z "$(git status --porcelain)" ]; then
  echo "  clean"
else
  git status --short | sed 's/^/  /'
fi
hr

# ---- remotes ----
echo "## Remotes"
git remote -v | awk '!seen[$1$3]++ {printf "  %-10s %s (%s)\n", $1, $2, $3}'
hr

# ---- key branches: ahead/behind tracking ----
echo "## Key branches"
printf "  %-14s %-44s %s\n" "BRANCH" "TIP" "AHEAD/BEHIND vs upstream/origin"

for br in master integration development; do
  if ! git show-ref --verify --quiet "refs/heads/$br"; then
    printf "  %-14s %s\n" "$br" "(missing locally)"
    continue
  fi
  tip=$(git log -1 --format='%h %s' "$br" 2>/dev/null | cut -c1-42)

  vs_upstream=""
  if git show-ref --verify --quiet "refs/remotes/upstream/master"; then
    a=$(git rev-list --count "upstream/master..$br" 2>/dev/null || echo "?")
    b=$(git rev-list --count "$br..upstream/master" 2>/dev/null || echo "?")
    vs_upstream="upstream/master: +$a / -$b"
  fi

  vs_origin=""
  if git show-ref --verify --quiet "refs/remotes/origin/$br"; then
    a=$(git rev-list --count "origin/$br..$br" 2>/dev/null || echo "?")
    b=$(git rev-list --count "$br..origin/$br" 2>/dev/null || echo "?")
    vs_origin="origin/$br: +$a / -$b"
  fi

  printf "  %-14s %-44s %s | %s\n" "$br" "$tip" "$vs_upstream" "$vs_origin"
done
hr

# ---- master patch list (commits master carries on top of upstream) ----
echo "## Patches master carries on top of upstream/master"
if git show-ref --verify --quiet "refs/remotes/upstream/master" \
   && git show-ref --verify --quiet "refs/heads/master"; then
  count=$(git rev-list --count upstream/master..master 2>/dev/null || echo 0)
  if [ "$count" = "0" ]; then
    echo "  (none — master is at upstream/master)"
  else
    git log --format='  %h %s' upstream/master..master
  fi
else
  echo "  (cannot compute — upstream/master or master missing)"
fi
hr

# ---- ball history patches: integration on top of master ----
echo "## Patches integration carries on top of master"
if git show-ref --verify --quiet "refs/heads/master" \
   && git show-ref --verify --quiet "refs/heads/integration"; then
  count=$(git rev-list --count master..integration 2>/dev/null || echo 0)
  if [ "$count" = "0" ]; then
    echo "  (none — integration is at master)"
  else
    echo "  ($count commits — showing first 15)"
    git log --format='  %h %s' master..integration | head -15
  fi
fi
hr

# ---- development on top of integration ----
echo "## Patches development carries on top of integration"
if git show-ref --verify --quiet "refs/heads/integration" \
   && git show-ref --verify --quiet "refs/heads/development"; then
  count=$(git rev-list --count integration..development 2>/dev/null || echo 0)
  if [ "$count" = "0" ]; then
    echo "  (none — development is at integration)"
  else
    echo "  ($count commits — showing first 15)"
    git log --format='  %h %s' integration..development | head -15
  fi
fi
hr

# ---- other local branches (everything not in the canonical 3) ----
echo "## Other local branches"
others=$(git for-each-ref --format='%(refname:short)' refs/heads/ \
  | grep -vE '^(master|integration|development)$' || true)
if [ -z "$others" ]; then
  echo "  (none)"
else
  for br in $others; do
    tip=$(git log -1 --format='%h %s' "$br" 2>/dev/null | cut -c1-60)
    printf "  %-32s %s\n" "$br" "$tip"
  done
fi
hr

# ---- open PRs (gh) ----
# WSL bash doesn't always see Windows PATH; try gh, gh.exe, and known winget link.
echo "## Open PRs"
GH=""
for cand in gh gh.exe "/c/Users/$USER/AppData/Local/Microsoft/WinGet/Links/gh.exe" "/mnt/c/Users/gary.brown/AppData/Local/Microsoft/WinGet/Links/gh.exe"; do
  if command -v "$cand" >/dev/null 2>&1 || [ -x "$cand" ]; then
    GH="$cand"
    break
  fi
done

if [ -n "$GH" ]; then
  prs_origin=$("$GH" pr list --repo garybrowndev/vpinball --state open --json number,title,headRefName,baseRefName 2>/dev/null || echo "")
  if [ -z "$prs_origin" ] || [ "$prs_origin" = "[]" ]; then
    echo "  origin (garybrowndev/vpinball): none"
  else
    echo "  origin (garybrowndev/vpinball):"
    echo "$prs_origin" | python -c 'import json,sys
for p in json.load(sys.stdin):
    print(f"    #{p[\"number\"]} {p[\"title\"]}  ({p[\"headRefName\"]} -> {p[\"baseRefName\"]})")'
  fi
else
  echo "  (gh CLI not on PATH in this shell — skipping)"
fi
hr

echo "## Workflow reminder"
echo "  upstream sync : fetch upstream, rebase master onto upstream/master, force-push origin master"
echo "                  (master carries B2S compat patch — ff-only WILL fail; rebase is expected)"
echo "  ship to integ : merge master into integration, rebuild, test"
echo "  ship to dev   : merge integration into development"
echo "  ship feature  : PR development -> integration on GitHub"
echo "  branch state  : master = upstream + B2S patch ; integration = master + Ball History ; development = integration + WIP"
