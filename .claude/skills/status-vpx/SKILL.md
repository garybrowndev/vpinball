---
name: status-vpx
description: Print a structured status of Gary's vpinball_ballhistory repo — branch positions (master vs upstream, integration vs master, development vs integration), the extra patches master carries on top of upstream, working-tree state, open PRs on garybrowndev/vpinball, and a one-line reminder of his fork-development workflow. Use this whenever Gary asks "what's the state of my branches", "what's going on in my repo", "where am I", "where's master / integration / development", "what fixes do I have on top of upstream", "what PRs do I have open", "give me a status", "/status-vpx", or any variant where he wants the current shape of his vpinball fork at a glance. Read-only — runs `git fetch --prune` against origin and upstream and prints; never modifies branches or pushes.
---

# status-vpx — vpinball_ballhistory repo status reporter

Gary works in this repo across three long-lived branches with a specific topology:

```
upstream/master   ──► master   ──► integration   ──► development
   (vpinball)      (+B2S patch)   (+ Ball History)    (+ WIP)
```

When he asks "what's the state of things" he wants a quick read of:

1. Where each branch is relative to the next layer up/down
2. What patches `master` carries on top of `upstream/master` (today: the B2S compat stub `3ea227d80`)
3. Ball History commits riding on `integration` over `master`
4. WIP commits on `development` over `integration`
5. Working tree dirty? Open PRs?

This skill runs the bundled status script and presents the output to Gary unmodified — it's a status reporter, not an interpreter. If something looks off (e.g. master has unexpected extra patches, or integration is way behind master), call it out briefly *after* the report.

## Run the script

```bash
bash .claude/skills/status-vpx/scripts/status.sh
```

The script:

- `git fetch --prune` against `origin` and `upstream` so the report isn't stale (non-destructive)
- Prints sections in this order: header → working-tree → remotes → key-branch ahead/behind → patches master carries on upstream → patches integration carries on master → patches development carries on integration → other local branches → open PRs (via `gh`) → workflow reminder

If the script fails (e.g. `upstream` remote not configured on this clone), surface the error to Gary rather than silently working around it — his repos always have both remotes.

## After running

1. Pipe the script output verbatim to Gary as a fenced block
2. If anything jumps out as anomalous, add 1-3 lines after the block flagging it. Examples:
   - `master` has more than 1 patch on top of `upstream/master` and the extras don't look like the B2S stub → ask what they are
   - `integration` is *behind* `master` → he probably hasn't done the post-upstream-sync merge yet
   - `development` has many commits behind `integration` → maybe stale
   - Working tree is dirty with files that look like accidental edits (e.g. `.build/` artifacts, `third-party/` overwrites)
3. Don't propose actions unless he asks. He uses this command to *see* state, not to be told what to do.

## Why the script and not inline git commands

The git ahead/behind math is fiddly (have to verify each remote-tracking ref exists before counting, handle the missing-upstream case, format the `gh pr list` JSON). A bundled script is faster, deterministic, and the same script can be wired into a status-line or polled by a separate agent later if Gary wants.

## Editing tips

- The "Workflow reminder" tail block at the end of the script is a memory aid for new Claude sessions that have just started in this repo. Keep it terse.
- The patches-on-master section is the most important diagnostic — if Gary ever ends up with surprise commits there it's a sign of a botched upstream rebase. Don't trim it.
- `gh pr list --repo garybrowndev/vpinball` is hardcoded; if the fork ever moves, update both places.
