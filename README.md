# Ball History for Visual Pinball

A fork of [Visual Pinball](https://github.com/vpinball/vpinball) (VPinballX) that adds the **Ball History** feature — a system for recording, replaying, and training with ball state during gameplay. Useful for analysing what happened on a given shot, recreating tricky situations, and drilling specific shots over and over to improve pinball skill.

The fork is maintained at [`garybrowndev/vpinball`](https://github.com/garybrowndev/vpinball). All three **Windows x64** rendering backends — DirectX, OpenGL, and BGFX — are regularly built and tested in both Debug and Release for Ball History development. Non-Windows platforms compile (the feature is `#ifdef`-guarded out) but are not actively exercised.

> Looking for the upstream Visual Pinball README? [Jump to the original ↓](#visual-pinball)

> **AI-assisted development.** This fork is built with heavy use of AI tooling — primarily [Claude Code](https://www.claude.com/product/claude-code) — for everything from Ball History feature work to upstream-merge conflict resolution, debugging, and documentation (including this README). Contributors are **encouraged** to use Claude Code (or equivalent) during active development. The repo ships with a detailed [`CLAUDE.md`](CLAUDE.md) (build recipes, upstream-API change tracking, debugging lessons, trainer phase timeline) that is consumed by Claude Code automatically as project context — keep it up to date when you change conventions, and it pays for itself on the next session.

## What Ball History does

Ball History runs as an in-game overlay on top of any VPX table. While playing, press **`V`** to open the Ball History menu. The feature has two operational modes:

### Normal mode — record / replay / recall
- Continuously snapshots every ball's full physics state (position, velocity, angular momentum, orientation) into timestamped `BallHistoryRecord`s.
- Press **`R`** during play to recall a previous ball state — useful for rewinding to "right before that drain" without restarting the table.
- Take manual control of one or more balls: pause the simulation, scrub through history, drag balls to new positions on the playfield, and resume play from the new state.
- Auto-control vertices: place waypoints on the playfield and have the system steer a ball through them — handy for exercising specific shot lines repeatably.

### Trainer mode — drill specific shots
- Define a **start location** (where the ball spawns, with optional X/Y/Z offset and initial velocity), a **pass corridor** (geometry the ball must travel through to count as a pass), and a **fail zone** (region that ends the run as a fail).
- Configure runs per session, countdown delay between runs, ball-hold during countdown, physics variance per run (so you're not drilling against a deterministic target), gameplay difficulty (separate from variance), and a per-run timeout.
- The trainer plays a result sound (pass / fail / timeout) and holds the result on screen for a configurable result-display window before starting the next countdown.
- Per-run results are tracked and shown in a status panel (Previous / Current / Remaining) with overall pass/fail tallies for the session.
- Trainer configurations are saved per-table as `.ini` files and reload automatically when you re-open the same table.

### How it's drawn
- The menu, status panel, and per-run text are rendered via ImGui overlay in `LiveUI`.
- Physical visualisations on the playfield (fake balls, the corridor walls, the fail-zone intersection circle, vertex labels) reuse VPinball's own parts system — `CComObject<Ball>`, `CComObject<Light>`, `CComObject<Rubber>` — so they shade and rotate correctly with whatever camera/cabinet view the table is using.

## Quick start (try it out)

1. Grab the latest build from `garybrowndev/vpinball` (or build it yourself — see below).
2. Launch any VPX table and press **`F5`** to start play.
3. Press **`V`** to open the Ball History menu.
4. Read the menus / descriptions and figure it out — Normal mode is at the top, Trainer mode is in its own submenu.
5. Press **`R`** during play to recall the most-recent history snapshot. (Other keys — flippers, plunger, etc. — pass through to ImGui as normal while the menu is open.)

DM [@garybrowndev](https://www.github.com/garybrowndev) with feedback or issues.

## Build (Windows / Visual Studio 2022)

### Quick build (no CMake)

1. Sync the code (`git pull`, etc.).
2. Download precompiled third-party deps from [upstream GitHub Actions](https://github.com/vpinball/vpinball/actions) — find a successful `vpinball` workflow run on master, download both:
   - `VPinballX-<VERSION>-dev-third-party-windows-x64-Debug.zip`
   - `VPinballX-<VERSION>-dev-third-party-windows-x64-Release.zip`
3. Extract Debug first, then Release, both into `./third-party/`, overwriting existing files.
4. Revert any git changes caused by the overwrite: `git checkout -- third-party/`.
5. Run `make/create_vs_solution.bat`, choose `2022`. (Re-run this any time you switch between `master` and `integration`/`development`, or after merging upstream — different `vpx-core.vcxitems` templates are used per branch.)
6. Open `.build/vsproject/VisualPinball.sln` in Visual Studio 2022.
7. Set **`vpx`** as the Startup Project.
8. Build for Debug or Release / x64.

### Third-party deps via GitHub CLI

```bash
gh run download <RUN_ID> --repo vpinball/vpinball -n "VPinballX-...-dev-third-party-windows-x64-Debug.zip"   --dir /tmp/vpx-deps
gh run download <RUN_ID> --repo vpinball/vpinball -n "VPinballX-...-dev-third-party-windows-x64-Release.zip" --dir /tmp/vpx-deps-release
cp -rf /tmp/vpx-deps/*         third-party/
cp -rf /tmp/vpx-deps-release/* third-party/
git checkout -- third-party/
```

### Build tips

- After upstream merges, **always** re-run `make/create_vs_solution.bat`.
- When changing widely-included headers (`def.h`, `stdafx.h`, `ballhistory.h`), force a full rebuild by deleting all `.obj` files: `rm -rf .build/obj/vpx/Debug-x64/*.obj`. To force a relink, also delete the exe.
- Build only `vpx` (skip plugin projects): add `-p:BuildProjectReferences=false` to MSBuild.
- MSBuild from CLI:
  ```bash
  "/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" \
    ".build/vsproject/vpx.vcxproj" -p:Configuration=Debug -p:Platform=x64 -m
  ```

### CMake build (windows-x64)

```bash
platforms/windows-x64/external.sh
cp make/CMakeLists_bgfx-windows-x64.txt CMakeLists.txt
cmake -G "Visual Studio 17 2022" -A x64 -B build
cmake --build build --config Release
```

### Run from CLI with a table preloaded

```bash
start .build/bin/vpx/Debug-x64/VPinballX64.exe "C:\path\to\Example.vpx"
```

Press **F5** to play, **ESC** for editor, **Q** to quit play mode. Always launch with a table loaded when iterating on Ball History — starting empty wastes time navigating menus.

### Tests

Build and run the **`vpx-test`** project from the VS solution (`.build/vsproject/vpx-test.vcxproj`). Test sources are in `tests/`.

## Branch strategy

- **`master`** — tracks upstream `vpinball/vpinball` **plus one cherry-picked patch** (the B2S compat stub — see below).
- **`integration`** — `master` merged with Ball History changes. Build / test branch.
- **`development`** — active Ball History work. PRs land here first, then ship to `integration` after local verification.
- **Remotes**: `origin` = `garybrowndev/vpinball`, `upstream` = `vpinball/vpinball`.

### Master is not pure upstream — carries one patch

`origin/master` intentionally carries the **B2S compat stub** (`Restore B2SBackglassServer discovery under modern -Play path`) cherry-picked on top of upstream. This is the one patch required for any stock (non-Ball-History) build to work with PinUp-Popper + B2SBackglassServer on a cabinet — upstream rejected the fix ([PR #2529](https://github.com/vpinball/vpinball/pull/2529)) but the cabinet still needs to boot tables.

Consequence: `git merge --ff-only upstream/master` will fail on master after a fetch. Instead:

```bash
# Rebase (preferred — clean single-patch history)
git fetch upstream
git checkout master
git rebase upstream/master
git push --force-with-lease origin master
```

If the patch ever lands upstream or is replaced, drop the cherry-pick and restore `git merge --ff-only`.

### Sync workflow (upstream → development)

1. `git fetch upstream`
2. Checkout `master`, **rebase** onto `upstream/master`, force-push to origin.
3. Checkout `integration`, merge `master` — resolve upstream-vs-Ball-History conflicts here.
4. Download fresh third-party deps if upstream added new plugins or libraries.
5. Re-run `make/create_vs_solution.bat`.
6. Build and test integration.
7. Merge `integration` into `development`.

When upstream changes conflict with Ball History integration points, the convention is: accept upstream for all conflicts in the merge commit, then re-apply Ball History changes in separate (reviewable) commits. Files that always need re-integration: `player.h/cpp`, `LiveUI.cpp`, `InputManager.cpp`, build files.

## Configuration

- **VPinball settings**: `C:\Users\<user>\AppData\Roaming\VPinballX\10.8\VPinballX.ini`
- **Ball History settings**: `<exe folder>/BallHistory/` (per-table `.ini` files)
- **In-game settings UI**: press **F12** while playing (replaces the old Video Options dialog)
- **Ball History debug log**: `<exe folder>/BallHistory/logs/ballhistory_debug.log` (Debug builds only)

### Cabinet table-rotation universal fix

In a cabinet `VPinballX.ini` `[TableOverride]` section, `ViewCabRotation = 90.000000` is the universal cabinet-orientation fix on a landscape cabinet. Empirically verified across a 500-table audit — produces correct landscape orientation for both rotation regimes seen in the wild (legacy-mode tables with ROTF=270 baked in, and window-mode tables with the ROTF=0 author bug). Works because window mode auto-adds 270° before applying the user value, and legacy mode applies it through a Y/Z-flipped coordinate system; both paths converge on the same visual orientation.

Per-table sidecar `.ini` files (`<table>.ini` next to `<table>.vpx`) are only needed for genuine outliers, not the routine ROTF=0 author bug.

## Source layout

| Directory | Purpose |
|-----------|---------|
| `src/core/` | Application core — main entry, player loop, settings, pin table, undo, **Ball History** |
| `src/physics/` | Physics engine, collision, quad trees, ball/flipper/plunger hit objects |
| `src/renderer/` | Rendering abstraction (DirectX / OpenGL / bgfx), shaders, render targets, VR |
| `src/parts/` | Pinball table elements (Ball, Bumper, Flipper, Gate, Kicker, Light, Ramp, Rubber, Spinner, Surface, Trigger, …) |
| `src/ui/` | Editor UI, debugger, live UI, dialog properties |
| `src/input/` | InputManager (SDL-based, replaces old PinInput / DirectInput) |
| `src/audio/` | Audio playback (miniaudio, replaces old SDL_mixer) |
| `src/math/`, `src/utils/`, `src/assets/`, `src/shaders/`, `src/plugins/` | Math, utilities, bundled assets, shaders, plugin host |
| `make/` | VS solution / project templates, CMake files per platform, build scripts |
| `platforms/` | Per-platform external dependency build scripts |
| `plugins/` | Runtime plugin implementations (PinMAME, DMD, DOF, B2S, …) |
| `standalone/` | Standalone builds (iOS, Android, macOS — not Ball History targets) |
| `third-party/` | Precompiled external dependencies (gitignored — must be downloaded) |

### Ball History feature (`src/core/ballhistory.h` / `.cpp`)

The primary area of active development (~10,800 lines combined). Key types:

- **`BallHistory`** — central controller integrated into the `Player` game loop. Owns `NormalOptions` and `TrainerOptions`.
- **`BallHistoryState`** — snapshot of one ball's physics state.
- **`BallHistoryRecord`** — timestamped collection of `BallHistoryState` for all tracked balls.
- **`BHLog`** — Debug-build logger; writes to `<exe>/BallHistory/logs/ballhistory_debug.log` via `BHLOG(fmt, ...)`. Release builds get a no-op stub.
- **`EnumAssignKeys`** — defined in `ballhistory.h` (was previously in `pininput.h`, which upstream removed). Ball History is the sole consumer.
- **Platform guard** — the full implementation is wrapped in `#ifdef __BALLHISTORY_WIN32__`; non-Windows platforms get a no-op stub so cross-platform builds still link.

### Key integration points

- **`Player`** (`src/core/player.h`) — owns the `BallHistory` instance; constructor inits, destructor uninits, `ApplyPlayingState` resets trainer timing.
- **`LiveUI`** (`src/ui/live/LiveUI.cpp`) — calls `BallHistory::Process` / `ProcessKeys` / `ProcessMouse` each frame during `RenderUI`.
- **`InputManager`** (`src/input/InputManager.cpp`) — registers `V` (menu) and `R` (recall) actions that dispatch to `BallHistory::ProcessKeys`.
- **`HitBall`** (`src/physics/hitball.h`) — has `friend struct BallHistory` for `m_oldpos` access.
- **`PinTable`** (`src/parts/pintable.h`) — provides part management (`AddPart` / `RemovePart`) and element access via `GetParts()`.

### Rendering object lifecycle (Ball History → VPX parts)

Creating a visual (fake ball, line, intersection circle):

1. `CComObject<T>::CreateInstance(&obj); obj->AddRef();`
2. `obj->Init(...);`
3. `obj->m_wzName = L"uniqueName";` (required by `AddPart`)
4. `m_ptable->AddPart(obj);` (sets `m_ptable`, calls `AddRef`)
5. `obj->RenderSetup(renderDevice);` (must be after `AddPart`)

Destroying:

1. `obj->RenderRelease();` (must be before `RemovePart`)
2. `m_ptable->RemovePart(obj);` (calls `Release`)
3. Clear from the tracking map.

After `UnInit`, call `m_renderer->m_renderDevice->SubmitRenderFrame()` to flush pending GPU resources.

## Debugging

- **Crash handler** produces resolved x64 stack traces in `crash.txt` (the upstream stack code was patched to use `IMAGE_FILE_MACHINE_AMD64` and a rewritten `WriteCallStack`). Always check `crash.txt` first.
- **`BHLog`** writes to `<exe>/BallHistory/logs/ballhistory_debug.log` in Debug builds. Each `BHLOG(...)` call auto-flushes. Canonical log filenames are `static constexpr` members on `BHLog` — add new ones there rather than hardcoding paths.
- **Visual Studio attach** — useful when `crash.txt` is missing (the SEH crash handler doesn't catch `_purecall`, `abort()`, or CRT assertion dialogs). From Git Bash:
  ```bash
  cmd //c start "" "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" //DebugExe "C:\path\to\VPinballX64.exe"
  ```
  The double slash on `//DebugExe` is required — Git Bash MSYS rewrites single-slash args to Unix paths and breaks the launch.
- **Conditional breakpoints for teardown bugs** — many UAFs only fire in `BallHistory::UnInit`, which is called only from `Player::~Player()`. Breakpoint at `ballhistory.cpp` `UnInit` to fire exclusively during teardown rather than every frame.
- **Refcount inspection** — watch `m_dwRef` on COM objects; `0xDDDDDDDD` is MSVC Debug CRT's dead-land fill and confirms a delete-then-use bug.

### Known issues

- **RenderDevice pending buffer assertion** — Debug-only assertion on exit after Ball History use (`m_pendingSharedIndexBuffers.empty()`). Mitigated by `SubmitRenderFrame()` in destructor; may still fire in some exit paths.
- **Trainer key collisions with table scripts** — the menu key was moved from `C` to `V` after `Example.vpx`'s VBScript "Manual Ball Control" was found to bind to `C` (keycode 46) and silently freeze the ball when the menu opened. If a new collision is suspected on another table, signal to look for: a specific ball velocity literal being written every physics step. Grep the table script for that constant first — it's almost certainly the table, not the physics engine.

## Code style

- `.clang-format` is present: WebKit base, Allman braces, 3-space indent, 190-column limit, tabs = 3 spaces.
- Member variables use `m_` prefix (e.g., `m_Position`, `m_BallHistoryRecords`).
- Enums use `TypeName_Value` naming (e.g., `ModeStateType_Config`, `MenuStateType_Root_SelectMode`).
- Static constants defined as `static const` class members.
- COM objects used for rendered elements (`CComObject<Ball>`, `CComObject<Light>`, `CComObject<Rubber>`).

## Further reading

See [`CLAUDE.md`](CLAUDE.md) for the full developer guide — upstream API change tracking, a trainer-phase timeline reference, and a catalogue of debugging lessons learned the expensive way (one symptom value pointing at a VBScript collision rather than the physics engine, sync-vs-async sound stalls in Win32 `PlaySound`, etc.).

---

Original readme for forked repo below...

# Visual Pinball

*An open source pinball table editor and simulator.*

This project was started by Randy Davis, open sourced in 2010 and continued by the Visual Pinball development team. This is the official repository.

## Features

- Simulates pinball table physics and renders the table with DirectX, OpenGL or [bgfx](https://bkaradzic.github.io/bgfx/overview.html)
- Simple editor to (re-)create any kind of pinball table
- Live editing of most content within the rendered viewport
- Table logic (and game rules) can be controlled via Visual Basic Script
- Over 1050 real/unique pinball machines from ~100 manufacturers, plus over 550 original creations were rebuilt/designed using the Visual Pinball X editor (over 3000 if one counts all released tables, incl. MODs and different variants), and even more when including its predecessor versions (Visual Pinball 9.X)
- Emulation of real pinball machines via [PinMAME](https://github.com/vpinball/pinmame) is possible via Visual Basic Script (Visual PinMAME), or via the libPinMAME-API/plugin
- Supports configurable camera views (e.g. to allow for correct display in virtual pinball cabinets)
- Support for Tablet/Touch input, Joypads, or specialized pinball controllers
- Support for Stereo3D output
- Support for Head tracking via BAM
- Support for VR/XR HMD rendering (including [PUP](https://www.nailbuster.com/wikipinup), [B2S](https://github.com/vpinball/b2s-backglass) backglass and DMD output support)
- Support for WCG/HDR rendering (for now only via the BGFX (D3D11/12) build)
- Support for Windows (x86), Linux (x86/Arm, incl. RaspberryPi and RK3588), macOS, iOS/tvOS, Android (the latter builds are also available via the respective app stores for free)
- Plugin system to drive/fuel all kinds of displays (DMD, backglass, etc), add custom/dynamically-changed content (PUP, Serum, etc), direct output framework (DOF), sensors, and much more (WIP)

## Download

All releases are available on the [releases page](https://github.com/vpinball/vpinball/releases).

## Documentation

Documentation is currently sparse. Check the [docs](docs) directory for various guides and references.

## How to build

Build instructions are available in the [make directory README](make/README.md).

