---
name: sync-vpx
description: Run the full sync round on Gary's vpinball_ballhistory fork — downhill then uphill. Downhill (Phases 1-5) fetches upstream, rebases master replaying its fork-local patches, merges master→integration resolving Ball-History conflicts, refreshes fork deps, CMake-builds, and verifies on the cabinet before merging integration→development. Uphill (Phase 6) then ships anything development holds that integration lacks back up via PR, so a sync never strands development's own commits. This is the WRITE companion to the read-only status-vpx. Use this whenever Gary says "sync from upstream", "update my fork", "pull in upstream", "bring upstream down", "rebase master onto upstream", "merge master into integration", "do the whole merge round", "get up to date from upstream", "go through that round (pull, build, merge, build, master/integration/development)", "ship development back to integration", "push dev back up", or status-vpx reported "N commits to bring in" on the upstream→master row or "Ship flow" on the integration←development row. Drives every step with human checkpoints before each push, at every conflict, and before merging the ship PR — it never force-pushes or merges silently. It never merges integration→master; that direction is fork-only and deliberate. Do NOT use this for a read-only status check (that's status-vpx) or for building/debugging a single config on the cabinet (that's debug-vpx).
---

# sync-vpx — the upstream→down sync round

This is the WRITE counterpart to `status-vpx`. Where status-vpx only *reports* the
branch stack, this skill *advances* it: it walks upstream changes down through the
3-branch fork, resolving conflicts and rebuilding along the way.

```
upstream/master ─► master (+local patches) ─► integration (+Ball History) ─► development (+WIP)
                                                    ▲                              │
                                                    └──────── ship (PR) ───────────┘
```

The round flows **downhill** first (upstream → development, Phases 1–5), then makes one
**uphill** pass (development → integration, Phase 6) to ship back anything development
holds that integration lacks. Uphill is always a PR, never a direct merge.

`integration → master` is **not** part of the round in either direction — see Phase 6.

## Why this is a guided skill, not a one-shot script

Three of the steps are genuinely risky and need human judgment:

- **Rebasing master force-pushes to origin.** master isn't pure upstream — it carries
  fork-local patches that get *replayed* on top of the new upstream tip. A bad replay
  silently drops a patch the cabinet needs to boot. Never assume the count; enumerate.
- **Merging master→integration is where upstream collides with Ball History.** The
  conflicts land in a known set of integration-point files and must be resolved
  upstream-wins-then-reapply, not blindly.
- **The build must pass and Gary must play-test on the cabinet** before the changes
  flow into development. Merging down a broken integration poisons the active branch.

So every push and every conflict is a **checkpoint**: stop, show Gary what happened,
wait for an explicit go. Gary's standing rule is review-before-push — honor it here
especially, because these operations rewrite origin history.

## Phase 0 — Preflight (always)

Run the read-only preview first so you and Gary agree on scope before touching anything:

```bash
bash .claude/skills/sync-vpx/scripts/sync-plan.sh
```

It fetches origin+upstream (non-destructive) and reports: how many commits upstream is
ahead, the local patches that will replay, the working-tree state, the active `gh`
account, which upstream commits touch Ball-History integration files (the likely-conflict
preview), and the **Phase 6 ship backlog** (`integration..development`). Note it reports
the ship backlog even when upstream is 0 ahead — "upstream is current" and "development
has unshipped work" are independent conditions. Read it out and confirm:

1. **Working tree is clean.** Uncommitted work + a rebase/merge round is how you lose
   changes. If dirty, stop and ask Gary to commit/stash first.
2. **Scope.** A full round goes all the way to development. Gary may want to stop after
   master, or after integration. Ask if it's not obvious.
3. **`gh` account.** Pulling fork deps in Phase 3 needs the personal account active
   (`gh auth switch --user garybrowndev`). The `gary-brown_bplogix` EMU account can't
   read `garybrowndev/vpinball`. Flag if it's on the wrong one — but don't switch yet
   (only Phase 3 needs it; switch back after).

If upstream is 0 ahead of master, the fork is already current — say so and stop.

## Phase 1 — master ← upstream (rebase + force-push)  ⛔ CHECKPOINT

master carries fork-local patches on top of upstream. **Never hardcode the count or match
them by hash** — SHAs are replayed on every rebase, and the set shrinks when a patch lands
upstream (the Kalman idle-start tilt fix did exactly that on 2026-08-23, going from two
patches to one). Enumerate the live set every time:

```bash
git log upstream/master..master --oneline --no-merges
```

As of 2026-08-24 that is exactly one:

- Restore B2SBackglassServer discovery under modern -Play path (the B2S compat stub —
  without it the cabinet's PinUp-Popper/B2S setup won't boot tables)

If `git rebase` reports `skipped previously applied commit <sha>`, that patch has landed
upstream — confirm by diffing its body against upstream's version, then drop it from the
table in `CLAUDE.md` and from `get_stack_bullets()` in
`.claude/skills/status-vpx/scripts/status.sh`.

```bash
git checkout master
git rebase upstream/master      # replays the local patches on top of the new upstream tip
```

- **If conflicts:** resolve them keeping *both* intents — upstream's new code AND the
  local patch's purpose. These patches are small and targeted; a conflict usually means
  upstream refactored the area the patch touches. Re-read the patch's diff
  (`git show <sha>`) to understand what it's protecting, then port that intent into the
  new code. `git rebase --abort` returns to safety if it goes sideways.
- **After a clean rebase, verify both patches survived — by content, not by count.**
  `git log upstream/master..master --oneline --no-merges` should still list both, but
  a bare count won't catch a patch that replayed into upstream's refactored code and lost
  half its hunks. Compare each patch's added/removed lines against its pre-rebase version:

  ```bash
  # old SHA -> new SHA for each patch; strip line numbers/blob hashes and diff the bodies
  a=$(git show <oldSha> --format='' | grep -E '^[+-]' | grep -vE '^(\+\+\+|---)')
  b=$(git show <newSha> --format='' | grep -E '^[+-]' | grep -vE '^(\+\+\+|---)')
  [ "$a" = "$b" ] && echo IDENTICAL || echo DIFFERS
  ```

  Get the old SHAs from `git reflog` (the pre-rebase tip is still there). If one vanished
  or differs unexpectedly, stop and investigate before pushing.

⛔ **CHECKPOINT — do not push without Gary's OK.** This force-pushes and rewrites origin
master history:

```bash
git push --force-with-lease origin master
```

Use `--force-with-lease` (not `--force`) so the push aborts if origin moved under you.

⏳ **Wait for master CI before moving on.** A rebase can compile locally yet break the
fork's CI build (different toolchain/flags), and a red master poisons everything
downstream. Watch the run to completion per **CI gating** below. If it fails, the rebase
likely mis-replayed a patch into upstream's refactored code — fix on master, re-push,
re-wait. **Do not start Phase 2 on a red master.**

## Phase 2 — integration ← master (merge + resolve Ball-History conflicts)  ⛔ CHECKPOINT

This is the conflict-heavy phase: upstream's changes meet the Ball History feature.

```bash
git checkout integration
git merge master
```

**Conflict resolution strategy** (from hard-won experience — see CLAUDE.md "Upstream
merge strategy for Ball History conflicts"):

1. **Take upstream's version for the conflict itself**, then **re-apply the Ball History
   change on top** as a clear, separate edit. Don't hand-blend the two inside the
   conflict markers — that produces un-reviewable Frankenstein hunks.
2. Prefer landing the re-applied Ball-History integration as its **own follow-up
   commit(s)** after the merge commit, so the merge stays "accept upstream" and the
   re-integration is reviewable in isolation.
3. **Integration-point files that almost always need re-integration** (check these even
   if git didn't flag a conflict — upstream may have moved the API out from under them):
   - `src/core/player.{h,cpp}` — owns the `BallHistory` instance + game-loop hooks
   - `src/ui/live/LiveUI.cpp` — calls `BallHistory::Process/ProcessKeys/ProcessMouse`
   - `src/input/InputManager.cpp` — registers the V (menu) and R (recall) key actions
   - `src/physics/hitball.h` — `friend struct BallHistory` for `m_oldpos`
   - `src/parts/pintable.{h,cpp}` — `AddPart`/`RemovePart` lifecycle
   - build files (`make/*`, CMake templates) if upstream restructured the build
4. The **Upstream API Changes table in CLAUDE.md** is the cheat-sheet for what upstream
   renamed (e.g. `m_vhitables.push_back` → `AddPart`, `PinInput` → `InputManager`). When
   a re-integration won't compile, that table usually says why.

⛔ **CHECKPOINT** — show Gary the conflict resolution and the re-integration commits
before pushing:

```bash
git push origin integration
```

⏳ **Wait for integration CI — this gate is load-bearing, not optional.** Phase 3 pulls
the third-party deps from *this very run's artifacts*, which only exist once the run
**succeeds**. So you must watch it to completion (per **CI gating** below) and capture its
run id. If it fails, the merge/re-integration is broken — read the failing logs, fix on
integration, re-push, re-wait. Never download deps from a failed or still-running build.

## Phase 3 — refresh fork deps + local CMake build

After an upstream merge the third-party dep SHAs bump, so the old `dev-third-party` zips
no longer match. **Only the fork's own CI run for the new integration commit has matching
deps** (upstream's won't — the merged tree has different dep SHAs).

1. **Use the integration CI run you already watched to green in Phase 2** (its artifacts
   are now ready) — download + merge its deps (`gh` must be on `garybrowndev`):
   ```powershell
   gh auth switch --user garybrowndev
   $RUN = <the integration CI run id captured in Phase 2>   # already confirmed successful
   gh run download $RUN --repo garybrowndev/vpinball -n "VPinballX-<VER>-<sha>-dev-third-party-windows-x64-Release.zip" --dir $env:TEMP\vpx-deps
   robocopy "$env:TEMP\vpx-deps\build-libs"   third-party\build-libs   /E
   robocopy "$env:TEMP\vpx-deps\include"      third-party\include      /E
   robocopy "$env:TEMP\vpx-deps\runtime-libs" third-party\runtime-libs /E
   git checkout -- third-party/   # revert tracked files; gitignored binaries keep the fresh versions
   ```
2. **Build via CMake** (NOT `create_vs_solution.bat` — that path is deprecated/broken;
   see CLAUDE.md). Cap parallelism at `/m:9` on Gary's 12-core laptop to avoid C1076:
   ```powershell
   cmake -G "Visual Studio 17 2022" -A x64 -B build -DRENDERER=BGFX -DPLATFORM=windows -DARCH=x64  # single tracked root CMakeLists; no template copy. Reconfigures itself after first run
   cmake --build build --config Release -- /m:9 /p:CL_MPCount=9
   ```
   Output: `build/Release/VPinballX_BGFX64.exe`.
3. **If a plugin post-build fails on a missing DLL** (e.g. `libserialport64-0.dll`), the
   deps are stale → the run you pulled from doesn't match the commit. Refresh from the
   correct run.

### ⚠️ If upstream bumped bgfx, check the embedded shaders BEFORE building

BGFX shaders are **not** compiled by the build — they are pre-compiled blobs checked into
`src/shaders/bgfx_*.h`, regenerated only by the `vpinball-bgfx-shaders` workflow, which is
**`workflow_dispatch`-only**. So upstream can move `BGFX_PATCH_SHA` without regenerating
them, and nothing in CI catches it: the build succeeds and only *running a table* fails,
with `Fatal Error: shader compilation failed!` at `RenderDevice.cpp` and every shader
failing at once in `vpinball.log`. This happened on 2026-08-23 (`42ec8999f`) and cost a
full cycle plus a broken cabinet.

If the round's diff touches `platforms/config.sh` or `src/shaders/bgfx/shaderc.exe`, run
this two-second check before building:

```powershell
# what the CURRENT shaderc emits
cd src/shaders/bgfx
./shaderc.exe -f vs_imgui.sc -o $env:TEMP\v.bin --type vertex --platform windows -p s_5_0 -O 3 --varyingdef varying.def.sc -i . --define NOSTEREO
# first 4 bytes are "VSH" + version; compare against the checked-in blob's version byte
```

Mismatch (e.g. tool emits `0x0c`, blobs are `0x0b`) means the shaders are stale. Fix by
regenerating them yourself — `src/shaders/bgfx/shaders.ps1` with the in-tree `shaderc.exe`,
plus `dxcompiler.dll` + `dxil.dll` pulled from bgfx at `BGFX_PATCH_SHA` for the `s_6_0`
step (those two DLLs are **not** checked in; fetch, use, then delete them). Commit the
regenerated headers with a note that upstream's version supersedes ours in the next merge.
4. Switch `gh` back to `gary-brown_bplogix` once deps are pulled (keeps the default EMU
   account active for everything else).

## Phase 4 — deploy + play-test  ⛔ CHECKPOINT (the real gate)

A build that compiles is not a build that works. Before anything flows into development,
the merged integration must run on the cabinet and Gary must confirm it.

- Hand the deploy/launch/iterate loop to the **`debug-vpx`** skill (it owns the cabinet
  plumbing — preflight-kill, PAExec launch, cdb attach). At minimum: deploy
  `build/Release/VPinballX_BGFX64.exe` → cabinet `Z:\visual pinball\VPinballX_BGFX64_BH.exe`
  (close VPX on the cabinet first or the copy fails "file in use").
- Tell Gary exactly what to smoke-test: load a VPM-backed table (Black Pyramid), confirm
  it boots (B2S patch survived the rebase), then exercise Ball History — press V for the
  menu, R for recall — to confirm the re-integration is wired.

⛔ **Do not proceed to Phase 5 until Gary plays it and says it's good.** This is the
whole point of building at integration before merging down.

## Phase 5 — development ← integration (merge + push)  ⛔ CHECKPOINT

Once Gary confirms integration is healthy, bring it into the active branch:

```bash
git checkout development
git merge integration
```

Usually a clean fast-forward / no-conflict merge (development = integration + WIP, and
WIP rarely overlaps the upstream changes). If development has its own WIP that conflicts,
resolve favoring the WIP's intent and flag it to Gary.

⛔ **CHECKPOINT** before:

```bash
git push origin development
```

⏳ **Wait for development CI to go green** (per **CI gating** below) — this is the
round's final confirmation that the active branch is healthy. If it fails here but
integration was green, the WIP on development conflicts with the upstream changes; surface
the failing logs and fix on development.

## Phase 6 — integration ← development (the ship flow)  ⛔ CHECKPOINT

**This is the one uphill leg of the round, and it runs on every round.** After the
downhill flow lands, development may still hold commits integration has never seen — WIP
that predates this round, or fixes made *during* it (docs corrections, tooling, anything
committed to development while working). Leaving them stranded is how integration's copy
of `CLAUDE.md`/skills silently goes stale, which matters because **integration is the
canonical deploy-to-cabinet branch**.

```bash
git log integration..development --oneline --no-merges   # what is unshipped
git diff --stat integration development                  # what would change
```

If that is empty, say so and move to postflight. If not, ship it **as a PR, never a
direct merge** — the PR is the review gate:

```bash
gh pr create --repo garybrowndev/vpinball --base integration --head development \
   --title "<what it does>" --body "<why, and what verified it>"
```

⛔ **CHECKPOINT — get Gary's OK before merging.** Then merge (prefer a **merge commit**,
matching how integration has historically taken changes), pull it locally, and ⏳ **wait
for integration CI to go green** so both branches end the round verified at identical
content.

Judgement on scope: a docs/tooling-only PR needs no rebuild. If the unshipped commits
touch source, treat it like Phase 3–4 — build and have Gary play-test before merging,
because integration is what the cabinet actually runs.

> ⚠️ **Never merge `integration → master` as part of a round.** `master` is upstream plus
> the fork-local patches, nothing else. Something only goes that way when Gary explicitly
> wants a fork-only fix that is *not* going upstream — a conscious decision, never a
> step in this skill.

## Phase 7 — postflight

Run `status-vpx` to confirm the round landed clean: all four flow rows should read "no new
commits" — including `integration ← development`, which Phase 6 should have just drained —
and origin sync should be ✓ across the board. Summarize for Gary what came down from
upstream (the themes from the 42-or-however-many commits) so he knows what changed in the
build he just tested.

## CI gating (wait for checks, handle failures)

Every push in this round kicks off a GitHub Actions build on `garybrowndev/vpinball`.
Two of those builds are gates you must not run past: integration CI (Phase 2 — its
artifacts ARE the Phase 3 deps) and master CI (Phase 1 — a red master poisons the merge
down). The pattern is the same each time — **don't passively wait; first poll for the run
to register, then block on it to completion, then branch on the result.**

**Step 1 — find the run for the commit you just pushed.** The run takes a few seconds to
appear, so poll until the run whose `headSha` matches your pushed commit shows up:

```powershell
$sha = (git rev-parse HEAD)
$run = $null
while (-not $run) {
  $run = gh run list --repo garybrowndev/vpinball --branch <branch> --limit 10 `
    --json databaseId,headSha,status,conclusion,createdAt |
    ConvertFrom-Json | Where-Object { $_.headSha -eq $sha } | Select-Object -First 1
  if (-not $run) { Start-Sleep 10 }
}
```

**Step 2 — block on it with `--exit-status`** so a failed run is a non-zero exit you can
branch on (it streams job progress as it goes):

```powershell
gh run watch $run.databaseId --repo garybrowndev/vpinball --exit-status
$ciOk = ($LASTEXITCODE -eq 0)
```

A full fork build runs on the order of tens of minutes — **never wait on it silently.**
While the watch blocks, post an **interim CI status report every ~5 minutes** until the run
concludes, each one carrying: **wall-clock time, elapsed since the wait started, an
approximate ETA, and a per-job rollup** (which jobs are done vs in-progress — especially the
`Build VPinballX-windows-x64-Release` job, since its artifacts are the Phase 3 deps). Gary
asked for this explicitly: a big build going dark for 30 minutes is worse than a few
heartbeat lines.

Implement the cadence with a **self-scheduled ~270–300s poll** running *alongside* the
blocking `gh run watch`, not instead of it — the watch gives the definitive
completion/exit-status signal, the periodic poll gives Gary the heartbeat:

```powershell
gh run view <run-id> --repo garybrowndev/vpinball --json status,conclusion,createdAt,jobs
```

Use `ScheduleWakeup` (≈270s — just under the 5-min cache window so the wake-up reads warm
context) to re-enter and emit the next status line, then reschedule while
`status == in_progress`; stop once `conclusion` is populated (the watch will also have
fired). Don't go tighter than ~5 minutes — sub-minute polling is noise and churns the
prompt cache for no benefit; ~5 minutes is the sweet spot for a build this long.

**Step 3 — branch on the result:**

- **Green** → proceed. For the Phase 2 integration run, **keep `$run.databaseId`** — it's
  the `$RUN` you feed to the Phase 3 dep download (its artifacts are now ready).
- **Red** → stop. Pull the failing step's logs and diagnose before touching anything else:
  ```powershell
  gh run view $run.databaseId --repo garybrowndev/vpinball --log-failed
  ```
  Map the failure back to the phase: a master failure (Phase 1) usually means a local
  patch mis-replayed into upstream's refactor; an integration failure (Phase 2) means the
  Ball-History re-integration is broken (often an upstream API rename — check CLAUDE.md's
  API-changes table). Fix on that branch, re-push, re-run this CI-gating loop. **Never
  advance the round on a red build.**

**If `gh` is unavailable or the repo has no Actions runs** (script's `$GH` resolver comes
up empty), CI can't be the gate — say so explicitly, and lean on the **local CMake build
in Phase 3** as the proxy gate instead. Don't silently skip the check and imply it passed.

## Checkpoint discipline (the one rule that matters)

Never run a `push` (especially the Phase 1 force-push) or finalize a conflict resolution
without showing Gary the result and getting an explicit go. The cost of a wrong
force-push to master is the cabinet failing to boot tables; the cost of asking is one
extra message. Always ask.

## Rollback escape hatches

- Mid-rebase gone wrong: `git rebase --abort` (returns master to pre-rebase tip).
- Mid-merge gone wrong: `git merge --abort` (returns the branch to pre-merge tip).
- Already committed a bad merge but not pushed: `git reset --hard ORIG_HEAD` (or use the
  reflog — `git reflog` shows every tip the branch has had).
- Force-pushed master then realized the rebase dropped a patch: the patch's SHA is still
  in `git reflog` / `git log --all` — cherry-pick it back, then re-push.

## Partial rounds

Gary often wants only part of the round (e.g. "just get master current"). Each phase is a
clean stopping point. Do the phases Gary asked for, push at the checkpoint, and report
where the stack now stands — don't auto-continue to the next phase without confirming.

## Relationship to the other skills

- **status-vpx** — read-only "where am I". Run it before (Phase 0) and after (Phase 7).
  It emits a "▶ Next" hint pointing here when upstream is ahead of master, and a
  "▶ Ship flow" hint when development is ahead of integration — the latter is now
  handled inside this skill as Phase 6 rather than being left to the reader.
- **debug-vpx** — the build/deploy/cabinet inner-loop. Phase 4 delegates to it.
- **The ship flow (development → integration)** used to live outside this skill. It is
  now **Phase 6** and runs on every round, so a sync no longer leaves development's own
  commits stranded. If Gary asks to "ship" *without* syncing, run Phase 6 on its own —
  it is self-contained (`git log integration..development`, then a PR).
