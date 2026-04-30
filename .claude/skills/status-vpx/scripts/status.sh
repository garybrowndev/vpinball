#!/usr/bin/env bash
# status-vpx — minimal repo status. Centerpiece is the "Branch stack"
# section: a single ASCII visualization (in a fenced code block) walking
# upstream → master → integration → development with edge-diff bullets and
# origin sync state on each node. Other sections (working tree, CI, PRs)
# are omitted when empty. Output is markdown. The caller MUST paste this
# output unfenced — the Branch stack diagram has its own internal fence.

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

cur_branch=$(git rev-parse --abbrev-ref HEAD)

origin_state() {
  local br="$1"
  git show-ref --verify --quiet "refs/remotes/origin/$br" || { echo "no origin"; return; }
  local a b
  a=$(git rev-list --count "origin/$br..$br" 2>/dev/null || echo 0)
  b=$(git rev-list --count "$br..origin/$br" 2>/dev/null || echo 0)
  if [ "$a" = "0" ] && [ "$b" = "0" ]; then echo "in sync ✓"; return; fi
  local s=""
  # ▲ = local has commits origin doesn't (push direction)
  # ▼ = origin has commits local doesn't (pull direction)
  [ "$a" -gt 0 ] && s="$s ▲ $a unpushed"
  [ "$b" -gt 0 ] && s="$s ▼ $b unpulled"
  echo "${s# }"
}

# Plain-English action line for an origin/<branch> state. Same args as origin_state.
origin_action() {
  local br="$1"
  git show-ref --verify --quiet "refs/remotes/origin/$br" || { echo "—"; return; }
  local a b
  a=$(git rev-list --count "origin/$br..$br" 2>/dev/null || echo 0)
  b=$(git rev-list --count "$br..origin/$br" 2>/dev/null || echo 0)
  if [ "$a" = "0" ] && [ "$b" = "0" ]; then echo "nothing to do"; return; fi
  local pull="" push=""
  [ "$b" -gt 0 ] && pull="pull $b from origin"
  [ "$a" -gt 0 ] && push="push $a to origin"
  if [ -n "$pull" ] && [ -n "$push" ]; then echo "$pull, then $push"
  else echo "${pull}${push}"
  fi
}

# ---- header ----
echo "**vpinball_ballhistory** · HEAD: \`$cur_branch\` · $(date '+%Y-%m-%d %H:%M')"
echo ""

# ---- branch stack ----
# One unified visualization: the patch stack from upstream root down to HEAD.
# Each edge between branches lists what differs (with up to 2 commit subjects);
# each branch node shows its sync state with origin and any unpushed/unpulled
# commit subjects below it.

ahead_count() { git rev-list --count "$1..$2" 2>/dev/null || echo 0; }

# Hardcoded 2-bullet semantic labels for stack-layer ahead portions.
# Args: stack-pair-key. Echoes 2 lines (one per bullet).
get_stack_bullets() {
  case "$1" in
    "master/upstream")
      echo "B2S compat stub for PinUp-Popper backglass discovery"
      echo "2 local fixes — .NET COM hang, InputManager device-ID collision"
      ;;
    "integration/master")
      echo "Ball History feature stack — physics replay + trainer mode"
      echo "Build/debug/skill tooling and bundled assets"
      ;;
    "development/integration")
      echo "Active WIP — small fixes and experiments"
      echo "Awaiting verification before merging into integration"
      ;;
  esac
}

# Compute all branch comparison counts up-front.
have_um=0; um_a=0; um_b=0
if git show-ref --verify --quiet refs/remotes/upstream/master \
   && git show-ref --verify --quiet refs/heads/master; then
  have_um=1
  um_a=$(ahead_count upstream/master master)
  um_b=$(ahead_count master upstream/master)
fi

have_om=0; om_a=0; om_b=0
if git show-ref --verify --quiet refs/heads/master \
   && git show-ref --verify --quiet refs/remotes/origin/master; then
  have_om=1
  om_a=$(ahead_count origin/master master)
  om_b=$(ahead_count master origin/master)
fi

have_mi=0; mi_a=0; mi_b=0
if git show-ref --verify --quiet refs/heads/master \
   && git show-ref --verify --quiet refs/heads/integration; then
  have_mi=1
  mi_a=$(ahead_count master integration)
  mi_b=$(ahead_count integration master)
fi

have_oi=0; oi_a=0; oi_b=0
if git show-ref --verify --quiet refs/heads/integration \
   && git show-ref --verify --quiet refs/remotes/origin/integration; then
  have_oi=1
  oi_a=$(ahead_count origin/integration integration)
  oi_b=$(ahead_count integration origin/integration)
fi

have_id=0; id_a=0; id_b=0
if git show-ref --verify --quiet refs/heads/integration \
   && git show-ref --verify --quiet refs/heads/development; then
  have_id=1
  id_a=$(ahead_count integration development)
  id_b=$(ahead_count development integration)
fi

have_od=0; od_a=0; od_b=0
if git show-ref --verify --quiet refs/heads/development \
   && git show-ref --verify --quiet refs/remotes/origin/development; then
  have_od=1
  od_a=$(ahead_count origin/development development)
  od_b=$(ahead_count development origin/development)
fi

# Helper: read up-to-2 bullet lines from a function/range invocation into b1/b2.
read_two() {
  # Reads stdin into globals __b1 / __b2
  __b1=""; __b2=""
  local line i=0
  while IFS= read -r line; do
    i=$((i+1))
    if [ $i = 1 ]; then __b1="$line"
    elif [ $i = 2 ]; then __b2="$line"
    fi
  done
}

# Print one flow row: "<left> <arrow> <right>   N commits to bring in (hint)",
# or "no new commits" when count is 0. The parenthesized hint explains in plain
# English what the count means for that direction (e.g. "upstream has stuff
# master hasn't pulled in yet"). The `[range: ...]` suffix is a hint for Claude
# to git log against when synthesizing the 2-line digest at presentation time.
# Args: left_label, arrow, right_label, count, range, hint, label_width
emit_flow() {
  local left="$1" arrow="$2" right="$3" n="$4" range="$5" hint="$6" w="${7:-32}"
  local pair
  pair=$(printf "%s %s %s" "$left" "$arrow" "$right")
  if [ "$n" = "0" ]; then
    printf "%-${w}s  no new commits\n" "$pair"
    return
  fi
  printf "%-${w}s  %s commit(s) to bring in (%s)   [range: %s]\n" "$pair" "$n" "$hint" "$range"
}

emit_ci_status() {
  [ -z "$GH" ] && return
  local runs
  runs=$("$GH" run list --repo garybrowndev/vpinball --limit 60 \
    --json status,conclusion,headBranch,headSha,name,displayTitle,createdAt 2>/dev/null || echo "")
  [ -z "$runs" ] || [ "$runs" = "[]" ] && return
  local branches_filter="master,integration,development"
  case "$cur_branch" in
    master|integration|development) ;;
    *) branches_filter="$branches_filter,$cur_branch" ;;
  esac
  echo "## CI status"
  echo ""
  echo "$runs" | BRANCHES="$branches_filter" python3 -c '
import json, sys, os, datetime as dt
from collections import defaultdict

allowed = set(os.environ.get("BRANCHES","").split(","))

def rel(ts):
    if not ts: return ""
    try: then = dt.datetime.fromisoformat(ts.replace("Z","+00:00"))
    except Exception: return ts
    diff = (dt.datetime.now(dt.timezone.utc) - then).total_seconds()
    if diff < 60:    return f"{int(diff)}s ago"
    if diff < 3600:  return f"{int(diff/60)}m ago"
    if diff < 86400: return f"{int(diff/3600)}h ago"
    return f"{int(diff/86400)}d ago"

runs = json.load(sys.stdin)
branch_latest = {}
for r in runs:
    br = r.get("headBranch") or ""
    if br not in allowed: continue
    ts = r.get("createdAt") or ""
    sha = (r.get("headSha") or "")[:7]
    cur = branch_latest.get(br)
    if cur is None or ts > cur[0]:
        branch_latest[br] = (ts, sha, r.get("displayTitle") or "")

groups = defaultdict(list)
for r in runs:
    br = r.get("headBranch") or ""
    sha = (r.get("headSha") or "")[:7]
    if br in branch_latest and sha == branch_latest[br][1]:
        groups[br].append(r)

order = ["master","integration","development"]
for b in branch_latest:
    if b not in order: order.append(b)

if not groups:
    print("_no recent runs on tracked branches_")
else:
    print("| Branch | Result | Commit | Title | When |")
    print("|---|---|---|---|---|")
    for br in order:
        if br not in groups: continue
        rs = groups[br]
        total  = len(rs)
        pass_n = sum(1 for r in rs if r.get("conclusion") == "success")
        fail_n = sum(1 for r in rs if r.get("conclusion") == "failure")
        pend_n = sum(1 for r in rs if r.get("status") in ("in_progress","queued"))
        if fail_n:            result = f"**{fail_n}/{total} FAIL**"
        elif pend_n:          result = f"{pend_n}/{total} RUN"
        elif pass_n == total: result = f"{total}/{total} ✓"
        else:                 result = f"{pass_n}/{total} ?"
        ts, sha, title = branch_latest[br]
        title = (title or "")[:50]
        print(f"| {br} | {result} | `{sha}` | {title} | {rel(ts)} |")
'
  echo ""
}

# Section order: CI status → Branch stack (rows + side note + origin sync) → Working tree → Open PRs.
emit_ci_status

echo "## Branch stack"
echo ""
echo "_Rows show \`source → target\` (last row flipped to keep integration anchored left). HEAD: \`$cur_branch\`._"
echo ""
echo '```'

# The 4 flows. Arrow points from source toward target. First three rows
# read left-to-right (parent → child); the last is flipped (integration ← development)
# so the integration column lines up vertically across rows where it matters.
# Each non-zero row gets a parenthesized plain-English hint explaining the count.
[ $have_um = 1 ] && emit_flow "upstream"    "→" "master"      "$um_b" "master..upstream/master"      "upstream has stuff master hasn't pulled in yet"
[ $have_mi = 1 ] && emit_flow "master"      "→" "integration" "$mi_b" "integration..master"          "master has stuff integration hasn't merged in yet"
[ $have_id = 1 ] && emit_flow "integration" "→" "development" "$id_b" "development..integration"     "integration has stuff development hasn't merged in yet"
[ $have_id = 1 ] && emit_flow "integration" "←" "development" "$id_a" "integration..development"     "development has WIP integration hasn't pulled back via PR"

echo '```'
echo ""

# Side note: master's local patches on top of upstream — single line.
if [ $have_um = 1 ] && [ "$um_a" != "0" ]; then
  read_two < <(get_stack_bullets "master/upstream")
  echo "_Side note — master carries **$um_a** local patch(es) on top of upstream so things build: ${__b1}; ${__b2}._"
  echo ""
fi

# Origin sync state as a table — caption folded into the "Status" header.
echo "| Branch | Origin status (\`garybrowndev/vpinball\`) | What to do |"
echo "|---|---|---|"
[ $have_om = 1 ] && echo "| \`master\` | $(origin_state master) | $(origin_action master) |"
[ $have_oi = 1 ] && echo "| \`integration\` | $(origin_state integration) | $(origin_action integration) |"
[ $have_od = 1 ] && echo "| \`development\` | $(origin_state development) | $(origin_action development) |"
echo ""

case "$cur_branch" in
  master|integration|development) ;;
  *) echo "_HEAD is on \`$cur_branch\` — off the canonical 3 branches._"; echo "" ;;
esac

# ---- working tree ----
dirty=$(git status --porcelain | wc -l | tr -d ' ')
if [ "$dirty" != "0" ]; then
  echo "## Working tree — $dirty change(s)"
  echo ""
  echo "| Status | File | What changed |"
  echo "|---|---|---|"
  git status --porcelain | while IFS= read -r line; do
    st=$(echo "${line:0:2}" | sed 's/ /·/g')
    f="${line:3}"
    echo "| \`$st\` | $f | _[describe]_ |"
  done
  echo ""
fi

# ---- open PRs ----
if [ -n "$GH" ]; then
  prs=$("$GH" pr list --repo garybrowndev/vpinball --state open \
    --json number,title,headRefName,baseRefName,isDraft,statusCheckRollup 2>/dev/null || echo "")
  if [ -n "$prs" ] && [ "$prs" != "[]" ]; then
    echo "## Open PRs"
    echo ""
    echo "| # | Title | Branch → Base | Draft | Checks |"
    echo "|---|---|---|---|---|"
    echo "$prs" | python3 -c '
import json, sys
def roll(checks):
    if not checks: return "—"
    p=f=pe=0
    for c in checks:
        s=(c.get("conclusion") or c.get("state") or "").upper()
        if s=="SUCCESS": p+=1
        elif s in ("FAILURE","ERROR","TIMED_OUT"): f+=1
        else: pe+=1
    parts=[]
    if p:  parts.append(f"✓{p}")
    if f:  parts.append(f"✗{f}")
    if pe: parts.append(f"…{pe}")
    return " ".join(parts) or "—"
for p in json.load(sys.stdin):
    num   = p["number"]
    title = (p.get("title") or "")[:50]
    head  = p.get("headRefName") or ""
    base  = p.get("baseRefName") or ""
    draft = "yes" if p.get("isDraft") else "no"
    chk   = roll(p.get("statusCheckRollup"))
    print(f"| #{num} | {title} | `{head}` → `{base}` | {draft} | {chk} |")
'
    echo ""
  fi
fi

