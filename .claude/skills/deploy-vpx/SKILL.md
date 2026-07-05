---
name: deploy-vpx
description: Ship the fork's GREEN CI builds to Gary's VPM cabinet — the third leg of the status-vpx -> sync-vpx -> deploy-vpx trilogy. Pulls the 3 windows-x64 Release app exes (BGFX/GL/DX9) from a green fork CI run, verifies each has the real Ball History impl compiled in (not the no-op stub), closes VPX on the cabinet, and copies them to Z:\visual pinball\ as the _BH exes the cabinet actually runs. Use this whenever Gary says "deploy to the VPM", "push the build(s) to the cabinet", "update my VPM/cabinet", "get my cabinet up to date", "ship this to virtualpin", "propagate the fix to GL/DX", "redeploy all renderers", or after a sync-vpx round finishes green and the freshly-built branch needs to land on the cabinet for real play-testing. This is the RELEASE deploy for gameplay — distinct from debug-vpx, which deploys a local Debug build under cdb for the crash/hang inner loop.
---

# deploy-vpx — ship green CI builds to the VPM cabinet

The trilogy:

```
status-vpx  (read: where does the branch stack stand?)
   -> sync-vpx  (advance: rebase master, merge down, rebuild, push, CI green)
      -> deploy-vpx  (ship: pull the green CI exes onto the cabinet for real play)
```

`deploy-vpx` is the last leg: it takes a **green** fork CI run and lands its Windows exes on the
cabinet so Gary can actually play them. It does **not** build anything locally — the CI already
built (and, on x64, the exes are the same bits CI validated). Its job is: pull the right artifacts,
prove Ball History is really in them, and copy them onto the cabinet safely.

## deploy-vpx vs debug-vpx — don't confuse them

| | **deploy-vpx** (this skill) | **debug-vpx** |
|---|---|---|
| Source | **CI Release artifacts** (green run) | **Local Debug build** (`cmake --build --config Debug`) |
| What | All 3 renderers: BGFX + GL + DX9 | Usually just BGFX exe + PDB |
| Where on cabinet | `Z:\visual pinball\*_BH.exe` (the real play exes) | `Z:\Visual Pinball\debug\` (the debug sandbox) |
| Purpose | Real gameplay / play-test the shipped build | cdb-attached crash/hang/behavior inner loop |

If Gary wants to **debug a crash**, that's debug-vpx. If he wants his **cabinet running the latest
finished build**, that's deploy-vpx.

## What "up to date" means on the cabinet

The cabinet keeps three renderer builds of the fork, each as an `_BH` exe, plus stock-upstream
`_orig` backups. The default launcher `VPinballX.exe` is a **symlink** to whichever `_BH` renderer
is the current default (normally BGFX). deploy-vpx refreshes the three `_BH` exes; it leaves the
`_orig` backups and the symlink alone unless asked.

| Renderer | CI artifact prefix | exe inside zip | Cabinet deploy name |
|---|---|---|---|
| BGFX (default) | `VPinballX_BGFX-…-windows-x64-Release.zip` | `VPinballX_BGFX64.exe` | `VPinballX_BGFX64_BH.exe` |
| GL | `VPinballX_GL-…-windows-x64-Release.zip` | `VPinballX_GL64.exe` | `VPinballX_GL64_BH.exe` |
| DX9 | `VPinballX-…-windows-x64-Release.zip` (bare prefix) | `VPinballX64.exe` | `VPinballX64_BH.exe` |

The `_orig` trio (`*_orig.exe`) are stock-upstream fallbacks — a known-good boot path if a fork
build misbehaves. **Never overwrite `_orig`.**

## Cabinet plumbing (shared with debug-vpx)

Same cabinet, same access as debug-vpx — see `debug-vpx/references/cabinet-setup.md` for the
one-time provisioning. The essentials:

- Host `virtualpin` (DNS flaky → fallback IP `192.168.1.31`). C$ admin share mapped to `Z:\`.
- **Always use `Z:\...`** from the dev box (faster + cleaner than UNC). If `Z:` dropped after a
  cabinet reboot: `net use Z: \\virtualpin\c$ /persistent:yes`.
- **Close VPX before copying** — a running VPX locks the exe and the copy fails "file in use".
  Reuse debug-vpx's killer: `paexec \\virtualpin -d cmd /c 'C:\Dumps\preflight-kill.cmd'`.
- **PAExec, never PsExec**, for any GUI launch on the cabinet (PsExec → black windows).
- `gh` must be on the **personal** account for fork artifacts: `gh auth switch --user garybrowndev`
  (the EMU account `gary-brown_bplogix` can't read the fork).

## The deploy round

### Phase 0 — Preflight (always) ⛔ know your source is GREEN

deploy-vpx ships **only** from a green run — a red or still-running build has no trustworthy
artifacts, and shipping a broken exe means the cabinet won't boot tables. Confirm with Gary:

1. **Which branch/commit?** **Default = `integration`** — it is the canonical release/deploy source,
   and the standing `_BH` builds the cabinet runs for real gameplay always come from a green
   `integration` CI run. `development` is **test/debug only**: deploy it during an iteration loop
   (play-test gate, debug-vpx) but never leave development as the standing cabinet build — finished
   WIP must graduate `development → integration` (the ship PR) first, then deploy `integration`. A
   specific `-RunId` if deploying an exact known-green run. If asked to "deploy development" for
   anything other than a test/iteration, confirm that's really intended before shipping it as `_BH`.
2. **Is that run green?** The helper below refuses anything not `completed + success`. Never
   hand-wave this — if CI is still running, wait (watch it per sync-vpx's CI-gating pattern).
3. **gh account** on `garybrowndev`, **cabinet reachable**, **`Z:` mapped**.

### Phase 1 — Stage + verify (read-only preview)

Run the bundled helper as a **dry run** — it resolves the green run, downloads the three
windows-x64 Release app exes to a fresh temp dir, verifies each has the real Ball History impl,
and prints the deploy plan. It copies nothing:

```powershell
pwsh .claude/skills/deploy-vpx/scripts/deploy-plan.ps1 -Branch integration
# or a specific run:
pwsh .claude/skills/deploy-vpx/scripts/deploy-plan.ps1 -RunId 28481960501
```

**The Ball History verification is load-bearing, not a formality.** Each exe is scanned for the
`Trainer` / `RunRecord` strings that only exist in the real implementation. When upstream's build
restructure silently dropped the `__BALLHISTORY_WIN32__` define, every build compiled Ball History
down to a no-op stub — the exe looked normal until you pressed **V** and nothing happened. The
script **refuses to deploy a stub** for exactly this reason. If it flags a stub: the fix is in the
build (confirm `__BALLHISTORY_WIN32__` is defined in `CMakeLists.txt`), not in deploy — stop and
rebuild.

### Phase 2 — Checkpoint ⛔ review before the copy

Deploying overwrites the exes the cabinet runs — outward-facing and not trivially reversible. Show
Gary the plan (which run, which three exes, sizes, BH-verified, target paths) and get an explicit
go, per his standing review-before-ship rule. The dry-run/`-Deploy` split exists to make this
checkpoint natural.

### Phase 3 — Close VPX + deploy

Close VPX on the cabinet (or the copy fails "file in use"), then deploy. Reuse debug-vpx's
preflight-kill, then re-run the helper with `-Deploy`:

```powershell
& 'C:\tools\sysinternals\paexec.exe' \\virtualpin -d cmd /c 'C:\Dumps\preflight-kill.cmd'
Start-Sleep 4
pwsh .claude/skills/deploy-vpx/scripts/deploy-plan.ps1 -Branch integration -Deploy
```

`-Deploy` re-verifies green + BH, then copies each exe to its `_BH` name and prints the landed
size + timestamp so the copy carries its own receipt.

> If VPX isn't running, the copy just works — preflight-kill is a cheap no-op guard, not mandatory.

### Phase 4 — Confirm + play-test ⛔ the real gate

A copied exe isn't a working exe until Gary runs it. Tell him exactly what to check:

- Launch a table (the default `VPinballX.exe` → BGFX `_BH`; or launch a specific renderer's exe).
- Press **V** → the Ball History menu should open (confirms BH is really in the build).
- Navigate the menu with the **flippers** and select with the **plunger/launch** (confirms input
  works with his actual mapping — keyboard *or* joystick).

Only after Gary confirms is the deploy done. If he wants a **different default renderer**, repoint
the launcher symlink (it targets the cabinet-local path):

```powershell
# from the dev box, operating on the cabinet's C: via Z:
Remove-Item 'Z:\visual pinball\VPinballX.exe' -Force
New-Item -ItemType SymbolicLink -Path 'Z:\visual pinball\VPinballX.exe' -Target 'C:\Visual Pinball\VPinballX_GL64_BH.exe'
```

(Target is the cabinet-local `C:\Visual Pinball\...` path, not `Z:\` — the symlink is resolved on
the cabinet.)

## Common shapes

- **"Deploy the latest integration build"** → Phase 0 confirm green → `deploy-plan.ps1 -Branch integration`
  (dry run) → review → `-Deploy` → play-test. Usually all three renderers at once.
- **"Propagate the fix to GL/DX"** → same, all three; the point is closing the gap where BGFX was
  fixed but GL/DX still ran an older/stubbed build.
- **"Just BGFX for a quick test"** → you can deploy a single locally-built Release exe by hand
  (`Copy-Item build\Release\VPinballX_BGFX64.exe 'Z:\visual pinball\VPinballX_BGFX64_BH.exe'`) — but
  prefer the CI path for anything Gary will keep, so all three stay in lockstep on the same commit.

## Gotchas (earned the hard way)

1. **Never deploy a non-green run.** Red/in-progress artifacts are untrustworthy; a bad exe means
   the cabinet won't boot tables. The helper enforces this.
2. **Verify Ball History is compiled in.** A missing `__BALLHISTORY_WIN32__` yields a stub that
   looks fine until you press V. The `Trainer`/`RunRecord` string check is the guard.
3. **Close VPX first** or the copy fails "file in use". preflight-kill (from debug-vpx) handles it.
4. **PAExec `-d -i 1`, never PsExec**, for any GUI launch on the cabinet.
5. **Don't `Remove-Item` a shared temp/deploy path with a glob** — the sandbox blocks it and a
   stale glob once matched a live `.ini` and destroyed it. The helper stages into a fresh
   per-run dir instead.
6. **`Z:` drops after a cabinet reboot** — remap `net use Z: \\virtualpin\c$ /persistent:yes`.
7. **gh on the personal account** or fork artifact downloads 401.
8. **Leave `_orig` alone.** Those are the stock-upstream escape hatch.

## Relationship to the other skills

- **status-vpx** — read-only "where am I"; run it first if unsure whether there's a green build to
  ship.
- **sync-vpx** — the branch-advancing round that *produces* the green build deploy-vpx ships. A full
  session is often `sync-vpx` (ends CI-green) → `deploy-vpx` (lands it on the cabinet).
- **debug-vpx** — the Debug/cdb inner loop; owns the cabinet plumbing (preflight-kill, PAExec,
  `cabinet-setup.md`) that this skill reuses. Reach for debug-vpx to *diagnose*, deploy-vpx to
  *ship*.
