---
name: status-vpx
description: Print a compact status of Gary's vpinball_ballhistory fork. Centerpiece is four flow rows showing what each branch is missing from its neighbor — `upstream → master`, `master → integration`, `integration → development`, and the flipped `integration ← development` (the PR-back direction). Each non-zero row gets a 2-line LLM-synthesized digest. Plus master local-patches side note, compact origin sync block, working-tree changes, CI status, and any open PRs. Use this whenever Gary asks "where am I", "what's the state of my fork", "show me my branches", "what's going on in my repo", "what's not pushed", "what should I pull", "did the last build pass", "what's CI doing", "give me a status", or runs `/status-vpx`. Read-only — fetches origin and upstream non-destructively.
---

# status-vpx — fork status reporter

Gary maintains a 3-branch fork on top of upstream:

```
upstream/master ─► master (+B2S patch) ─► integration (+Ball History) ─► development (+WIP)
```

This skill produces a single status report. Section order:

1. **Header** — repo name, current HEAD branch, timestamp
2. **`## CI status`** — latest commit per canonical branch with PASS/FAIL rollup (omitted when no runs)
3. **`## Branch stack`** — four flow rows + master local-patches side note + compact origin sync block
4. **`## Working tree`** — `git status` as a table (omitted when clean)
5. **`## Open PRs`** — only when there are open PRs

## The Branch stack section

Four flow rows. The first three read parent → child (top of stack down). The last is **flipped** — `integration ← development` — so integration stays anchored on the left across the rows that involve it. No leading bullet/marker — the arrow inside the row carries the direction.

Raw script output (what the script emits before Claude polishes it):

```
upstream → master               29 commit(s) to bring in (upstream has stuff master hasn't pulled in yet)   [range: master..upstream/master]
master → integration            no new commits
integration → development       no new commits
integration ← development       11 commit(s) to bring in (development has WIP integration hasn't pulled back via PR)   [range: integration..development]
```

Each non-zero row carries a **parenthesized plain-English hint** explaining what the count means in that direction (e.g. "upstream has stuff master hasn't pulled in yet"). These hints are hardcoded in the script orchestration — don't change them at presentation time.

Note on tip selection: when local `master`/`integration`/`development` is strictly behind its `origin/<br>` (fast-forward possible), the script uses `origin/<br>` as that branch's tip in the flow comparisons. This is what catches the "I just merged the PR on GitHub but haven't pulled locally" case — the flow row correctly reads 0 instead of double-counting commits already integrated upstream. The Origin sync table still uses the local ref so its push/pull guidance stays accurate. As a result, ranges shown in `[range: ...]` may name `origin/<br>` rather than the bare local branch name.

After the fenced code block, a **single-line side note** for the local patches `master` carries on top of `upstream`, then a **3-column `Origin sync` table** (`Branch` | `Origin status (garybrowndev/vpinball)` | `What to do`) where the `What to do` column gives plain-English next-step guidance ("pull 2 from origin", "push 1 to origin", "nothing to do"). All emitted by the script verbatim — no Claude polish needed.

### Synthesizing the 2 rich summary lines (Claude's job, every run)

The script does **not** emit commit subjects. For each row that says `N commit(s) to bring in`, Claude must:

1. Run `git log <range> --format='%s' -n 30` using the `[range: ...]` shown on that row.
2. Read the subjects, group them by theme (rendering fixes, build infra, perf, new feature stack, etc.), and write **2 concise rich summary lines** describing the substance of those commits.
3. Insert those 2 lines as indented bullets directly under the row inside the fenced code block.
4. **Strip the `[range: ...]` suffix from the row** in the polished output — it's a hint for Claude, not for Gary. Keep the parenthesized hint (e.g. "(upstream has stuff master hasn't pulled in yet)") — it's part of what Gary sees.

Polished output:

```
upstream → master               29 commit(s) to bring in (upstream has stuff master hasn't pulled in yet)
       BGFX render-loop overhaul — per-frame VSync, tonemapper exposure, latency/stability fixes
       UI & build hygiene — InGameUI display-size handling, joystick naming, dmdutil/dof bumps
master → integration            no new commits
integration → development       no new commits
integration ← development       11 commit(s) to bring in (development has WIP integration hasn't pulled back via PR)
       Trainer polish — result-hold Time freeze, status panel layout, async pass/fail sounds
       Rendering & infra — runtime Material for DrawLine, corridor XY rotation, perf cuts
```

Rules for the summary lines:
- **Two lines, no more.** If there are 50 commits, distill them into 2 themed buckets.
- **Each line max 120 characters.** Hard cap. If you can't fit it, tighten the wording — don't wrap, don't truncate mid-word, don't drop the cap. Why: keeps the fenced block from soft-wrapping in narrow terminals and keeps the digest scannable at a glance.
- **No raw subjects, no truncation marks, no `_(+N more)_` tags.** This is a digest, not a sample.
- **Concrete and specific.** "Bug fixes and improvements" is useless — name the subsystems and the kinds of changes.
- **Skip noise.** Merge commits, version bumps, whitespace cleanups — fold into the theme or drop.
- **Don't touch the master local-patches side note.** It comes from `get_stack_bullets()` as a single line emitted by the script — leave it verbatim.

### Working-tree "What changed" column (Claude's job, every run)

The Working tree table emits 3 columns: `Status | File | What changed`, where the third column is a `_[describe]_` placeholder. For each row, Claude must replace the placeholder with **one short sentence** describing what changed in that file.

How to do this efficiently:
- Untracked files (`??`): just describe what the file is for based on path/extension (e.g. ".jks signing keystore for Android release builds"). Don't read the file.
- Modified files: run `git diff <file>` (or `git diff --cached <file>` for staged) and write a 1-sentence summary of the actual diff. Keep it concrete — name the function, section, or behavior touched.
- For tiny edits (renamed var, comment fix), say so: "Renamed X → Y" / "Updated comment near Z".
- For bigger edits, summarize the most substantive change in one phrase. Don't list everything — pick the headline.

The column should make Gary glance at his working tree and instantly remember what he was doing in each file without opening it.

### Arrow rules (one each, no exceptions)

- **Stack flows use `→` for downward (parent→child), `←` only on the last row** so `integration` stays the left column. Arrow points from source toward target.
- **Origin sync uses `▲ unpushed` / `▼ unpulled` / `✓ in sync`.** Push goes up to origin, pull comes down — both real.

### What fills each line

- **Flow row count** — `git rev-list --count <target>..<source>`.
- **Flow row summary (2 lines)** — synthesized by Claude from `git log <range>` at presentation time. See above.
- **Side-note bullets** — hardcoded labels from `get_stack_bullets()` for the `master/upstream` key. Edit those when the master patch set shifts.
- **Origin sync** — short one-liner from `origin_state()` per branch.

## Run it

```bash
bash .claude/skills/status-vpx/scripts/status.sh
```

The script:

- runs `git fetch --quiet --prune` against `origin` and `upstream` (non-destructive)
- needs `gh` CLI for the CI and PR sections (skipped gracefully if missing) and `python3` for JSON parsing
- omits any section that has nothing to report

## How to present the output

The script emits markdown directly. **Paste it into your reply unfenced** — don't wrap the whole thing in a ``` fence or the tables become raw `|` text. The Branch stack rows are *internally* fenced by the script (so column alignment renders correctly in monospace); leave that fence alone.

Synthesize the 2-line digests per the rules above, then emit the rest verbatim.

## Editing tips

- **Master local-patch labels** live in `get_stack_bullets()` near the top of the script (the `master/upstream` case — 2 lines). Edit when the master patch set shifts.
- **Flow-row column width** is the `w=32` default in `emit_flow`; bump if branch names get longer.
- **CI section** lists only the canonical 3 branches (+ current HEAD if non-canonical). Old feature branches won't appear.
- All `gh` invocations go through the `$GH` resolver at the top of the script, which tries `gh`, `gh.exe`, and a couple of WinGet paths so this works from WSL bash.
- If the fork ever moves off `garybrowndev/vpinball`, both `gh pr list` and `gh run list` reference it.
