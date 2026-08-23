---
name: release-vpx
description: Publish a GitHub release of Gary's Ball History fork by dispatching the repo's own prerelease.yml workflow with an explicit commit, then finishing the release (title, body, full-release flag) with gh. Use this whenever Gary says "cut a release", "publish a release", "make a GitHub release", "release the fork", "tag a build", "put out a new build people can download", "release integration", "publish v10.8.1-...", "ship a release of ball history", or runs /release-vpx. Distinct from deploy-vpx (which copies exes onto the VPM cabinet and publishes nothing) and from sync-vpx (which advances the branch stack). Every run resolves and displays the exact branch and commit before publishing, because releasing the wrong branch silently produces a Ball-History release with no Ball History.
---

# release-vpx — publish a fork release

The trilogy plus one:

```
status-vpx  (read: where does the branch stack stand?)
   -> sync-vpx   (advance: rebase master, merge down, rebuild, push, CI green)
      -> deploy-vpx  (ship privately: green CI exes onto the cabinet for real play)
      -> release-vpx (ship publicly: green CI artifacts onto a GitHub release)
```

`deploy-vpx` and `release-vpx` are siblings, not a sequence — both consume the same green CI
run. deploy-vpx puts it on Gary's cabinet; release-vpx puts it on the internet.

## This skill runs the existing workflow — it does not replace it

`.github/workflows/prerelease.yml` already does the hard part, and it is **byte-identical to
upstream's** (`git diff upstream/master..origin/integration -- .github/workflows/prerelease.yml`
is empty). Keep it that way. The workflow:

- derives the tag from `src/core/vpversion.h` + `git rev-list ea558e74..<sha> --count` + short sha
  → e.g. `v10.8.1-5635-73687a207`
- finds the successful `vpinball.yml` run for that exact commit
- downloads every artifact matching `VPinballX_*` and attaches them
- creates the tag and a **prerelease**

This skill supplies what the workflow can't: **the right commit**, **preflight guards**, and the
**finishing steps** (title, body, prerelease → full release). Everything is `gh` and `git` — there
is nothing to build and nothing to commit.

## Settings (Gary's defaults)

| Setting | Default | Notes |
|---|---|---|
| Branch | **`integration`** | CLAUDE.md: integration is the canonical release source. `development` is test-only. |
| Release type | **Full release** (`--prerelease=false`) | Upstream leaves theirs flagged prerelease; Gary promotes his to a full release, as he did by hand in April. |
| Assets | **Whatever the workflow's `VPinballX_*` glob matches** | Upstream parity — BGFX + GL across Windows/macOS/Linux/Android/iOS/Pi. Do not add or trim. |
| DX9 | **Not released** | Upstream doesn't release it either (see Gotcha 2). |
| Notes | Templated from `references/release-body.md` | Parameterised by tag, commit, branch, and what changed since the previous fork release. |

## Phase 0 — Preflight

```powershell
gh auth switch --user garybrowndev     # EMU account can't touch the fork
git fetch origin --quiet
$REPO   = 'garybrowndev/vpinball'
$BRANCH = 'integration'                # override only with Phase 1's warning
$SHA    = (git rev-parse "origin/$BRANCH")
```

Use **`origin/<branch>`, never the local branch** — the release must be something the world can
fetch. A local commit that hasn't been pushed can't be released.

If Gary asks to release anything other than `integration`, say so plainly before continuing:
*"`development` is test-only per CLAUDE.md — the finished-WIP path is the `development → integration`
ship PR, then release integration. Release `development` anyway?"* Get an explicit yes.

## Phase 1 — Guards (all three, every run)

**Guard A — Ball History is actually in this commit.** This is the primary guard, not a nicety:
`prerelease.yml` exists on **master, integration and development**, and `src/core/ballhistory.cpp`
is **absent on master**. A release cut from master builds and publishes cleanly, titles itself
"Ball History Build", and contains zero Ball History. Nothing errors.

```powershell
git cat-file -e "${SHA}:src/core/ballhistory.cpp" 2>$null
if ($LASTEXITCODE -ne 0) { throw "REFUSING: no src/core/ballhistory.cpp at $SHA — this is not a Ball History commit." }
```

**Guard B — a green `vpinball.yml` run exists for this exact commit.** Without one the workflow's
`RUN_ID` comes back empty and the download step fails obscurely half-way through, after the tag
already exists.

```powershell
$run = gh run list --repo $REPO --workflow vpinball.yml --commit $SHA --status success --limit 1 `
         --json databaseId,headSha,createdAt | ConvertFrom-Json
if (-not $run) { throw "REFUSING: no successful vpinball.yml run for $SHA. Wait for CI (sync-vpx's CI-gating pattern) or pick a commit that has one." }
```

**Guard C — the tag isn't already taken.**

```powershell
gh release view $TAG --repo $REPO 2>$null   # must fail; a hit means this commit was already released
```

**Optional — binary Ball History check.** Guard A proves BH is in the *source*. It does not prove
`__BALLHISTORY_WIN32__` survived the build (when upstream's build restructure once dropped that
define, every exe compiled BH down to a silent no-op stub). If Gary hasn't already run `deploy-vpx`
on this same run — which performs exactly this check — borrow it before publishing:

```powershell
pwsh .claude/skills/deploy-vpx/scripts/deploy-plan.ps1 -RunId $run.databaseId   # dry run, copies nothing
```

It refuses on a stub. A public release deserves at least the same bar as a cabinet copy.

## Phase 2 — Preview the tag and body  ⛔ CHECKPOINT

Reproduce the workflow's tag math locally so Gary sees it before anything is dispatched:

```powershell
$v = (git show "${SHA}:src/core/vpversion.h")
$maj = [regex]::Match($v,'VP_VERSION_MAJOR\s+(\d+)').Groups[1].Value
$min = [regex]::Match($v,'VP_VERSION_MINOR\s+(\d+)').Groups[1].Value
$rev = [regex]::Match($v,'VP_VERSION_REV\s+(\d+)').Groups[1].Value
$n   = (git rev-list "ea558e7417f6f06fe567d34f0e33792a141b8e64..$SHA" --count)
$TAG = "v$maj.$min.$rev-$n-$(git rev-parse --short $SHA)"
```

Build the body from `references/release-body.md`, filling `{{TAG}}`, `{{SHA}}`, `{{BRANCH}}` and
`{{CHANGES}}`. For `{{CHANGES}}`, range from the **previous fork release** — `gh release list
--repo $REPO --limit 5` — **not** `git tag`, which is polluted with upstream's tags (upstream's
`v10.8.1-5436-af26b2d93` sorts newer than Gary's April tag but isn't his):

```powershell
$prev = (gh release list --repo $REPO --limit 5 --json tagName -q '.[0].tagName')
git log "$prev..$SHA" --format='%s' --no-merges
```

Distil those subjects into 3–6 themed bullets — same digest discipline as status-vpx's flow-row
summaries. No raw subject dumps.

⛔ **Show Gary and wait for an explicit go.** Publishing is public and awkward to undo. Show:

- **branch + full commit** (the thing that goes wrong silently — say it out loud every run)
- predicted **tag**
- the **green run id** the assets come from
- the rendered **body**
- release type (full release unless he said otherwise)

## Phase 3 — Dispatch and watch

**Always pass `-f sha=` explicitly.** This is what closes the branch hazard: `workflow_dispatch`
runs against whichever branch the GitHub UI dropdown had selected, and with a blank `sha` the
workflow falls back to `${GITHUB_SHA}` — that branch's tip. With an explicit sha, every consumer
in the workflow (`checkout ref:`, the version math, `gh run list --commit=`, and the release's
`commit:`) uses it, and the dropdown becomes irrelevant.

```powershell
gh workflow run prerelease.yml --repo $REPO --ref $BRANCH -f sha=$SHA
```

Then wait for it using **sync-vpx's CI-gating pattern**: poll until the run registers, block on
`gh run watch <id> --exit-status`, and post a heartbeat (wall-clock, elapsed, ETA) every ~5 minutes
alongside the watch. This run only repackages existing artifacts, so it's minutes not tens of
minutes — but never go dark.

**Expect a wall of scary-looking warnings — they are cosmetic.** The log emits one line per
artifact reading:

```
! Artifact is a directory:artifacts/VPinballX_GL-<ver>-windows-x64-Release.zip/. Directories can not be uploaded to a release.
```

That is **not** a failure. `download-artifact` puts each artifact in its own directory named after
the artifact (and the artifact names end in `.zip`/`.tar.gz`, which makes the directories *look*
like files). ncipollo's `artifacts/**/*` glob matches both the directory entries — skipped, one
warning each — and the real files inside them, which upload fine. A verified run emitted 22 of
these warnings and attached all 22 assets. **Judge the run by the asset count in Phase 4, never by
these lines**; rolling back over them would discard a perfectly good release.

If it genuinely fails, **nothing was published if it died before the release step** — check with
`gh release list`. If a tag was created, use the rollback below before retrying.

## Phase 4 — Finish the release  ⛔ CHECKPOINT

The workflow leaves a bare prerelease named `v<tag>` with no body. Resolve the **actual** tag from
the API rather than trusting Phase 2's prediction (see Gotcha 3):

```powershell
$TAG = (gh release list --repo $REPO --limit 1 --json tagName -q '.[0].tagName')
gh release edit $TAG --repo $REPO `
  --title "Ball History $TAG" `
  --notes-file $bodyFile `
  --prerelease=false `
  --latest
```

Verify it landed, and paste the result to Gary. **`isLatest` is not a valid `--json` field for
`gh release view`** (it errors out — the field list has `isDraft`/`isPrerelease` but no
`isLatest`); read the Latest label off `gh release list` instead:

```powershell
gh release view $TAG --repo $REPO --json tagName,name,isPrerelease,isDraft,targetCommitish,assets `
  -q '"\(.tagName)  prerelease=\(.isPrerelease)  draft=\(.isDraft)  commit=\(.targetCommitish)  assets=\(.assets|length)"'
gh release list --repo $REPO --limit 3    # confirms the Latest label
```

Also confirm nothing uploaded empty — a truncated upload is the failure mode the Phase 3 warnings
would mask:

```powershell
$sz = gh release view $TAG --repo $REPO --json assets -q '.assets[].size'
"zero-byte assets: " + (($sz | Where-Object { [int]$_ -eq 0 }).Count)
```

⛔ Confirm the asset count and the commit match what Phase 2 previewed before calling it done.

## Rollback

A release is undoable while nobody has downloaded it. Deleting the release alone leaves the tag
behind and the next attempt at the same commit collides — always clean up both:

```powershell
gh release delete $TAG --repo $REPO --cleanup-tag --yes
```

## Gotchas (earned the hard way)

1. **Releasing the wrong branch is silent, not loud.** `prerelease.yml` is dispatchable from
   master, master has no `ballhistory.cpp`, and the resulting release is titled "Ball History
   Build" with zero Ball History in it. Guard A plus an explicit `-f sha=` are the two things
   preventing this. Never dispatch with a blank `sha`.
2. **DX9 is not in releases, and that is upstream's behaviour too.** CI builds
   `VPinballX-<ver>-<sha>-windows-x64-Release.zip`, but the workflow's `VPinballX_*` glob (leading
   underscore) never matches it. Upstream's own latest release has zero DX9 assets. Don't "fix"
   the glob: dropping the underscore also sweeps in the five `VPinballX-…-dev-third-party-…` deps
   zips, which share the bare prefix and must never ship. Gary deploys DX9 to the cabinet via
   deploy-vpx instead.
3. **The predicted tag can differ from the real one in short-sha length.** `git rev-parse --short`
   auto-widens as the object count grows — April's tag used 7 chars, current commits abbreviate to
   9. Phase 2's tag is a *preview*; Phase 4 reads the real tag back from `gh release list`.
4. **Don't use `git tag` to find the previous release.** Local tags include upstream's, which
   interleave with Gary's by date. Use `gh release list --repo garybrowndev/vpinball`.
5. **`gh` must be on `garybrowndev`.** The `gary-brown_bplogix` EMU account can't touch the fork —
   dispatch and artifact reads 404/401.
6. **Never edit `prerelease.yml`.** It's byte-identical to upstream's; keeping it that way means
   it never conflicts during a sync round. Everything this skill needs is achievable from `gh`.
7. **The April body claimed DX9 was attached and it wasn't.** The template in
   `references/release-body.md` has that line corrected — don't paste the old body forward.
8. **`integration` and `development` are often the same commit.** When they are, "release
   integration" and "release development" resolve identically and the distinction is moot — but
   still resolve from `origin/integration` so it stays correct when they diverge. Expect the
   workflow's `--limit=1` run lookup to sometimes pick a run whose `headBranch` is `development`:
   same commit, same tree, identical bits, and the release's `commit:` still comes from our
   explicit `-f sha=`. Say so at the checkpoint rather than treating it as a fault.
9. **"Artifact is a directory … can not be uploaded" is noise, not failure.** One line per
   artifact, on a run that attaches everything correctly. See Phase 3. Judge by asset count.
10. **`isLatest` is not a `gh release view --json` field.** `--latest` works as an *edit* flag, but
    verifying the Latest label needs `gh release list`. Asking for `isLatest` exits 1 with
    "Unknown JSON field".

## Relationship to the other skills

- **status-vpx** — read-only "where am I". Run it first if unsure whether there's a green build to
  release.
- **sync-vpx** — produces the green integration build this skill publishes.
- **deploy-vpx** — the sibling ship path (cabinet, private). Owns the Ball-History-stub binary
  check that Phase 1 borrows. A common session is sync-vpx → deploy-vpx (play-test) → release-vpx.
- **The ship PR (`development → integration`)** is the prerequisite when the work being released
  still lives on development. Release integration, not development.
