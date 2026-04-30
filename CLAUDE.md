# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a fork of [Visual Pinball](https://github.com/vpinball/vpinball) (VPinballX) that adds the **Ball History** feature — a system for recording, replaying, and training with ball state history during gameplay. The fork is maintained at `garybrowndev/vpinball`. Only Windows x64 with DirectX is tested for Ball History development.

## Build Instructions (Windows / Visual Studio)

### Quick Build (no CMake)
1. Download precompiled third-party dependencies from [upstream GitHub Actions](https://github.com/vpinball/vpinball/actions) — find a successful `vpinball` workflow run on master, download both:
   - `VPinballX-<VERSION>-dev-third-party-windows-x64-Debug.zip`
   - `VPinballX-<VERSION>-dev-third-party-windows-x64-Release.zip`
2. Extract Debug first, then Release, both to `./third-party`, overwriting existing files
3. Revert any git changes caused by the overwrite (`git checkout -- third-party/`)
4. Run `make/create_vs_solution.bat`, select `2022` (must re-run after upstream merge)
5. Open `.build/vsproject/VisualPinball.sln` in Visual Studio 2022
6. Set `vpx` as the Startup Project
7. Build for Debug or Release / x64

### Third-party deps via CLI
```bash
gh run download <RUN_ID> --repo vpinball/vpinball -n "VPinballX-...-dev-third-party-windows-x64-Debug.zip" --dir /tmp/vpx-deps
gh run download <RUN_ID> --repo vpinball/vpinball -n "VPinballX-...-dev-third-party-windows-x64-Release.zip" --dir /tmp/vpx-deps-release
cp -rf /tmp/vpx-deps/* third-party/
cp -rf /tmp/vpx-deps-release/* third-party/
git checkout -- third-party/
```

### Build tips
- After upstream merges, re-run `make/create_vs_solution.bat` to regenerate VS project files
- When changing headers (especially `def.h`, `stdafx.h`, `ballhistory.h`), delete ALL `.obj` files: `rm -rf .build/obj/vpx/Debug-x64/*.obj`
- Force relink by also deleting the exe: `rm -f .build/bin/vpx/Debug-x64/VPinballX64.exe`
- Build just vpx without plugins: add `-p:BuildProjectReferences=false` to MSBuild
- MSBuild from CLI: `"/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe" ".build/vsproject/vpx.vcxproj" -p:Configuration=Debug -p:Platform=x64 -m`

### CMake Build (windows-x64)
```
platforms/windows-x64/external.sh
cp make/CMakeLists_bgfx-windows-x64.txt CMakeLists.txt
cmake -G "Visual Studio 17 2022" -A x64 -B build
cmake --build build --config Release
```

### Running Tests
Build and run the `vpx-test` project in the VS solution (`.build/vsproject/vpx-test.vcxproj`). Test sources are in `tests/`.

### Running VPinball from CLI
```bash
# Always launch with the Example table pre-loaded for faster debugging:
start .build/bin/vpx/Debug-x64/VPinballX64.exe "C:\code\Pinball\vpinball_ballhistory\Example.vpx"
```
Press F5 to play, ESC to return to editor, Q to quit from play mode.
When debugging Ball History, always launch with a table loaded — starting empty wastes time navigating menus.

## Branch Strategy & Update Workflow

- `master` — tracks upstream `vpinball/vpinball` **plus one cherry-picked patch** (see "Master is not pure upstream" below).
- `integration` — `master` + Ball History changes merged together. This is the build/test branch.
- `development` — active Ball History development work.
- Remotes: `origin` = `garybrowndev/vpinball`, `upstream` = `vpinball/vpinball`

### ⚠️ Master is not pure upstream — carries one patch

`origin/master` intentionally carries the **B2S compat stub** commit (`3ea227d80` — "Restore B2SBackglassServer discovery under modern -Play path") cherry-picked on top of upstream. This is the single patch required for any stock (non-Ball-History) build to work with PinUp-Popper + B2SBackglassServer on the cabinet, since upstream rejected the fix (PR vpinball/vpinball#2529) but we still need the cabinet to boot tables.

Consequence: `git merge --ff-only upstream/master` will fail on master after a fetch. Use one of:

```bash
# Rebase (preferred — clean single-patch history)
git fetch upstream
git checkout master
git rebase upstream/master       # replays the B2S patch on top of new upstream tip
git push --force-with-lease origin master

# Or merge-commit (preserves history, accumulates merge commits)
git checkout master
git merge upstream/master
git push origin master
```

If the B2S patch ever lands upstream or a replacement solution arrives, drop the cherry-pick and restore `git merge --ff-only` flow.

### Sync workflow (upstream -> development)
1. `git fetch upstream`
2. Checkout `master`, **rebase** onto `upstream/master` (not ff-only — see above), force-push to origin
3. Checkout `integration`, merge `master` — resolve conflicts here (upstream vs Ball History)
4. **Download fresh third-party deps** if upstream added new plugins/libraries
5. **Re-run `make/create_vs_solution.bat`** to regenerate VS project files (required after every branch switch between master and integration — different `make/vpx-core.vcxitems` templates)
6. Build and test integration
7. Merge `integration` into `development`

### Ship workflow (development -> integration)
1. Create PR from `development` to `integration` on GitHub
2. Merge PR after local build verification

### Upstream merge strategy for Ball History conflicts
When upstream changes conflict with Ball History integration points:
1. Accept upstream version for all conflicts in the merge commit
2. Re-apply Ball History changes in separate commits (reviewable)
3. Key files that always need re-integration: `player.h/cpp`, `LiveUI.cpp`, `InputManager.cpp`, build files

## Configuration

- **VPinball settings**: `C:\Users\<user>\AppData\Roaming\VPinballX\10.8\VPinballX.ini`
- **Ball History settings**: `<exe folder>/BallHistory/` (per-table .ini files)
- **In-game settings UI**: Press F12 while playing (replaces old Video Options dialog)
- **Ball History debug log**: `<exe folder>/BallHistory/ballhistory_debug.log` (Debug builds only)

### Cabinet table-rotation universal fix (`ViewCabRotation = 90.000000`)

In Gary's `VPinballX.ini` `[TableOverride]` section, `ViewCabRotation = 90.000000` is the universal cabinet-orientation fix. Empirically verified across a 500-table audit — produces the correct landscape orientation for **both** of the rotation regimes that exist in the wild:

- **Legacy-mode tables** (~98% of tables, ROTF=270 baked in `.vpx`)
- **Window-mode tables with the broken-author bug** (ROTF=0 baked, e.g. White Water v.1.2, 4 Queens, World Poker Tour, Space Station VPW, Junior, Defender VPW, Central Park)

**Why one value fixes both modes** (`src/renderer/ViewSetup.cpp`):
- Window mode (`GetRotation`, line 302-306) auto-adds `270°` for landscape viewports before applying the user value: setting 90 → effective `(270 + 90) % 360 = 0°` (no rotation, table fills landscape directly because Window mode already maps physical screen cm to playfield).
- Legacy mode (line 309-310, plus `MatrixScale(mSceneScaleX, -mSceneScaleY, -mSceneScaleZ)` Y/Z-axis flip in `ComputeMVP` line 447) applies the user value through a flipped coordinate system, so 90° in legacy is not the geometric inverse of 270° in window — the combined transforms produce equivalent visual orientations on a landscape display.

**Practical implication**: when triaging a table that renders "wrong orientation" on the cabinet, **first** assume the global override already covers it. Per-table sidecar `.ini` files (`<table>.ini` next to `<table>.vpx` with `[TableOverride] ViewCabRotation = X`) are only needed for genuine outliers — not the routine ROTF=0 author bug.

**Limitation**: only verified on Gary's landscape cabinet (4K landscape physical, BGSet=1, ScreenWidth=95.9 / Height=53.9 cm). Different cabinet geometries (portrait monitor, head-tracking, VR) may need different values.

**Diagnosing future "wrong orientation" reports**: extract `GameStg/GameData` stream from the `.vpx` (it's an OLE compound document — 7-Zip lists/extracts streams directly) and binary-grep for the 4-byte tags. The full set is in `src/parts/pintable.cpp:1225-1227`, but the ones to check first are:

| Tag (Cabinet view setup) | Meaning | Type |
|---|---|---|
| `VSM1` | Cabinet view layout mode (0=LEGACY, 1=CAMERA, 2=WINDOW) | int32 |
| `ROTF` | Cabinet viewport rotation in degrees | float |
| `SCFX/SCFY/SCFZ` | Cabinet scene scale | float |
| `XLFX/XLFY/XLFZ` | Cabinet camera position | float |
| `LAYF` | Cabinet layback | float |
| `FOVF` | Cabinet field of view | float |

## Architecture

### Source Layout (`src/`)

| Directory | Purpose |
|-----------|---------|
| `core/` | Application core: main entry point, player loop, settings, pin table, undo, and **Ball History** |
| `physics/` | Physics engine, collision detection, quad trees, ball/flipper/plunger hit objects |
| `renderer/` | Rendering abstraction (DirectX/OpenGL/bgfx), shaders, textures, render targets, VR |
| `parts/` | Pinball table elements (Ball, Bumper, Flipper, Gate, Kicker, Light, Ramp, Rubber, Spinner, Surface, Trigger, etc.) |
| `ui/` | Editor UI, debugger, live UI, resource files, dialog properties |
| `input/` | Input handling (InputManager with SDL, replaces old PinInput/DirectInput) |
| `math/` | Vectors, matrices, mesh math, bounding boxes |
| `audio/` | Audio playback (miniaudio, replaces old SDL_mixer) |
| `utils/` | Crash handling, logging, file I/O, memory utilities, timers |
| `assets/` | Bundled assets including example tables |
| `shaders/` | HLSL/GLSL shader source |
| `plugins/` | Plugin interface implementation |

### Ball History Feature (`src/core/ballhistory.h` / `.cpp`)

This is the primary area of active development (~10,800 lines combined). Key concepts:

- **`BallHistory` struct** — central controller integrated into the `Player` game loop. Has two operational modes:
  - **Normal mode** (`NormalOptions`) — record/replay ball positions, manage auto-control vertices, recall previous ball states
  - **Trainer mode** (`TrainerOptions`) — configurable training sessions with ball start/pass/fail locations, corridors, physics variance, difficulty settings, run tracking with results
- **`BallHistoryState`** — snapshot of a single ball's physics state (position, velocity, angular momentum, orientation)
- **`BallHistoryRecord`** — timestamped collection of `BallHistoryState` for all tracked balls
- **Platform guard** — `#ifdef __BALLHISTORY_WIN32__` wraps the full implementation; non-Windows platforms get a no-op stub
- **`EnumAssignKeys` enum** — defined in `ballhistory.h` (was previously in `pininput.h` which upstream removed). Ball History is the sole consumer.
- **UI** — rendered via ImGui overlay (`PrintScreenRecord` handles on-screen text/menus). Uses default ImGui font (custom fonts crash in ImGui 1.92+ due to atlas rebuild). Activated in-game by pressing "V". (Originally C, moved to V because C collides with `Manual Ball Control` in some table scripts — `Example.vpx` toggles its ball-control flag on `keycode = 46` which is C; see commit history.)
- **Input** — V (menu) and R (recall) keys registered as InputManager actions (`InputManager.cpp`). Flipper/plunger keys dispatched via ImGui in `LiveUI.cpp`.
- **Settings persistence** — trainer configurations saved/loaded via `CSimpleIni` (.ini files)
- **Rendering** — draws fake balls, intersection circles, lines using the VPinball parts system (`CComObject<Ball>`, `CComObject<Light>`, `CComObject<Rubber>`)

### Key Integration Points

- `Player` class (`src/core/player.h`) — owns the `BallHistory` instance; constructor inits, destructor uninits, `ApplyPlayingState` resets trainer timing
- `LiveUI` (`src/ui/live/LiveUI.cpp`) — calls `BallHistory::Process`, `ProcessKeys`, `ProcessMouse` each frame during `RenderUI`
- `InputManager` (`src/input/InputManager.cpp`) — registers V (menu) and R (recall) key actions that call `BallHistory::ProcessKeys`
- `HitBall` (`src/physics/hitball.h`) — the physics ball object whose state gets captured. Has `friend struct BallHistory` for `m_oldpos` access.
- `PinTable` (`src/parts/pintable.h`) — provides table metadata, part management (`AddPart`/`RemovePart`), and element access via `GetParts()`

### Upstream API Changes (important for Ball History maintenance)

These upstream changes affect Ball History and must be maintained during merges:

| Old API | New API | Ball History impact |
|---------|---------|-------------------|
| `m_vhitables.push_back()` | `m_ptable->AddPart()` / `RemovePart()` | Parts need `m_wzName` set. `RenderRelease()` before `RemovePart()`. `AddPart` only once. |
| `PinInput` / `pininput.h` | `InputManager` / `InputManager.h` | Register key actions via `AddAction()`. `EnumAssignKeys` moved to `ballhistory.h`. |
| `Ball::Init(table, ...)` | `Ball::Init(x, y, false, true)` | No PinTable parameter. Set `m_ptable` via `AddPart`. |
| `m_vedit` (public) | `GetParts()` | Use public accessor. |
| `m_BG_current_set` | `GetViewSetup()` | Use accessor method. |
| `m_filename` (string) | `m_filename` (filesystem::path) | Use `.string().c_str()` for C APIs. |
| `m_vball` (`vector<HitBall*>`) | `m_vball` (`vector<Ball*>`) | Access HitBall via `ball->m_hitBall`. |
| `ImGui::PushFont(font)` | Works but fonts must be added BEFORE atlas build | `PrintScreenRecord::UnInit` must NOT null font pointers. Use default font. |
| `GetFocusHWnd()` | `m_playfieldWnd->GetNativeHWND()` | For mouse/cursor operations. |
| `DEBUG_REFCOUNT_TRIGGER` | Remove from preprocessor defines | Causes assertion dialogs. Fix in `vpx-core.vcxitems`. |

### Rendering Object Lifecycle (Ball History)

When Ball History creates visual objects (balls, lines, circles):
1. `CComObject<T>::CreateInstance(&obj)` + `AddRef()`
2. `obj->Init(...)` 
3. `obj->m_wzName = L"uniqueName"` (required by `AddPart`)
4. `m_ptable->AddPart(obj)` (sets `m_ptable`, calls `AddRef`)
5. `obj->RenderSetup(renderDevice)` (MUST be after `AddPart`)

When destroying:
1. `obj->RenderRelease()` (MUST be before `RemovePart`)
2. `m_ptable->RemovePart(obj)` (calls `Release`)
3. Clear from map

After `UnInit`, call `m_renderer->m_renderDevice->SubmitRenderFrame()` to flush pending GPU resources.

### Other Key Directories

- `make/` — Visual Studio solution/project templates, CMake files per platform, build scripts
- `platforms/` — per-platform external dependency build scripts
- `plugins/` — runtime plugin implementations (PinMAME, DMD, DOF, B2S, etc.)
- `standalone/` — standalone builds for iOS, Android, macOS (not Ball History target)
- `third-party/` — precompiled external dependencies (gitignored, must be downloaded)
- `scripts/` — VBScript files for various pinball machine ROM emulation
- `tables/` — sample pinball table files

## Debugging

- **Crash handler** produces resolved x64 stack traces in `crash.txt` (fixed `IMAGE_FILE_MACHINE_AMD64` in `StackTrace.cpp`, rewrote `WriteCallStack` in `CrashHandler.cpp`). Always check `crash.txt` first.
- **Ball History debug logger** (`BHLog` class in `ballhistory.h`): writes to `<exe-folder>/BallHistory/logs/ballhistory_debug.log` in Debug builds. Call `BHLOG(fmt, ...)`. Each call auto-flushes; there is NO `BHLOG_FLUSH` macro.
- **Canonical log filenames** are declared as `static constexpr const char*` members on `BHLog` (e.g., `BHLog::LogFile_Debug`). Add new log filenames there, never hardcode paths.
- **Log folder auto-create**: `BHLog::SetLogFolder()` creates the `logs/` subfolder. Called from `BallHistory::Init` after `GetSettingsFolderPath()`.
- **Direct file writes for transient diagnostics** should also use `<exe-folder>/BallHistory/logs/` via `BHLog::GetLogPath("name.log")` — do NOT hardcode paths.

### Interactive debugging with Visual Studio (preferred when `crash.txt` is missing or stack is unresolved)

`crash.txt` comes from our SEH-based crash handler, which **does not catch** `_purecall` (pure-virtual call), `abort()`, CRT assertion dialogs, or any native dialog that doesn't go through an unhandled SEH exception. Release builds also ship without PDBs, so even when we DO get a stack, symbols may be raw hex. Attach Visual Studio in those cases — the payoff on today's ClearDraws UAF was huge.

**Attach the pre-built exe under VS (no sln needed):**

```bash
# From Git Bash. The DOUBLE slash on //DebugExe is REQUIRED — Git Bash MSYS
# rewrites single-slash args starting with `/` to Unix paths (the crash we
# hit: VS tried to open "C:/Program Files/Git/DebugExe"). The double slash
# escapes the rewrite.
cmd //c start "" "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" //DebugExe "C:\path\to\VPinballX_GL64.exe"
```

VS opens with the exe as a solution-less debug target. Press **F5** to run; VS breaks on any access violation or assert with a full resolved stack.

**Set conditional breakpoints for teardown-only bugs.** A breakpoint in `ClearDraws` hits every frame during gameplay. But `BallHistory::UnInit` is called **only** from `Player::~Player()` (`player.cpp:980`) — breakpoint at `ballhistory.cpp:930` (the `ClearDraws(player)` inside UnInit) fires exclusively during teardown. Apply the same pattern for any function reachable from both normal path and teardown: find a caller that's teardown-exclusive and put the breakpoint there, then **F11** (Step Into).

**Inspect refcount and pointer identity at the breakpoint.**
- Watch `drawnBall.second->m_dwRef` to see COM refcount each step
- `0xDDDDDDDD` (= signed -572662307) is MSVC Debug CRT's **dead-land** fill — confirms object was deleted
- `0xFD`, `0xFE`, `0xCD`, `0xFEEEFEEE` are other MSVC poison patterns; know the list
- Expanding `this->m_DrawnBalls` in Locals shows all entries — lets you compare the crashing entry against its siblings

**Debug vs Release PDBs.** Only Debug configs currently emit PDBs (confirmed: `.build/bin/vpx/{Debug,Debug_BGFX,Debug_GL}-x64/*.pdb` exist; Release_GL/Release_BGFX don't). To get a resolved stack on a Release-only bug, either reproduce in the matching Debug config (bug class is often shared — today's I3 and I4 had the same root cause) or add `GenerateDebugInformation` to the Release linker settings and rebuild.

### TODO: WinDBG-driven debugging for fully automated captures

Better than VS when the agent (not the user) drives the debugger: user launches the app + reproduces the crash, agent scripts breakpoints and logs stacks to a file. Requires installing Debugging Tools for Windows (cdb.exe/windbg.exe) — NOT currently installed on Gary's machine (only `vsjitdebugger.exe` is present). Set up before the next teardown-bug session.

## Debugging Lessons Learned (expensive mistakes to not repeat)

### 1. Specific numeric values in a symptom point at VBScript, not physics

When debugging "trainer ball freezes on first run," I saw the ball's velocity getting set to **exactly `-0.01`** every physics step. I spent many hours assuming it was a contact-solver friction bug, a physics-thread race, or a timing issue, and wrote hundreds of lines of deferred-state / gravity-kick machinery that all turned out to be unnecessary.

**The actual cause**: `Example.vpx`'s embedded VBScript has a "Manual Ball Control" feature bound to keycode 46 (C). Pressing C toggles its `EnableBallControl` flag. A table timer then sets `ball.velx = 0; ball.vely = BCyveloffset (= -0.01)` every tick. My Ball History menu key was ALSO C, so opening the menu silently enabled the script's control — ball froze.

**The fix was one line**: `SDL_SCANCODE_C` → `SDL_SCANCODE_V` in `InputManager.cpp`.

**Lesson: if a physics symptom's value is a specific literal (like `-0.01`), grep the table's script/config for that constant FIRST.** If the table script uses it, the problem is interaction with the script — not the physics engine. This single check would have saved the entire multi-session debugging saga.

### 2. Win32 `PlaySound` with `SND_SYNC` blocks the main thread

VPinball's `BallHistory::PlaySound(UINT, bool async = false)` wraps `::PlaySound` with `SND_RESOURCE | (async ? SND_ASYNC : SND_SYNC)`. The default is sync, which **blocks rendering until the sound finishes** (often ~500ms). All trainer end-of-run pass/fail sounds were sync → rendering froze → visuals changes (e.g., the trainer ball-teleport to next start) happened during the frozen window, so users saw "sound then suddenly both ball-teleport and visuals appear." Pass `true` (async) for end-of-run sounds.

### 3. Win32 `PlaySound` with `SND_ASYNC` has only ONE sound slot

A new `PlaySound` call cancels whatever is currently playing. If pass-sound fires async and the next frame's countdown-beep fires async, **pass-sound gets cut off after ~16ms**. Solution: delay the subsequent sound. We use a **result-hold period** (`m_ResultDisplayEndTimeMs`, ~1.5s) where the trainer sits on the pass/fail frame — no next-countdown kicks in yet, so no competing sound. See `ProcessModeTrainer` top-of-function handler.

### 4. Ball locks need tracking vectors so they can be undone on interruption

`m_lockedInKicker = true` freezes a ball. Multiple trainer phases (countdown ball hold, result-hold) use this. But `m_ControlVBalls` is **filtered to non-locked balls** — once we lock a ball, it's no longer in `m_ControlVBalls`, so we've lost the reference.

If the user opens the menu (interrupts trainer) mid-lock, nothing ever unlocks the ball → it stays stuck forever. Fix: track all balls we lock in `m_TrainerLockedBalls` vector. On `SetControl(true)` (menu opens), iterate the vector and unlock.

### 5. `MenuStateType_Trainer_Results` is misleadingly named

Despite the name, this state spans **both** the active run AND the post-run results screen. Two guards in `Process()` and `ResetTrainerRunStartTime()` check `m_MenuState == Trainer_Results` to prevent external events (ball changed, focus lost) from calling `Init()` mid-trainer. See comments in the code. Rename candidate: `Trainer_Active` or `Trainer_RunningOrResults`.

### 6. `m_WasControlled` one-time restore path

`Process()` at the end of the branch runs a "one-time restore" when `m_Control` flips from true → false AND `m_WasControlled` was true. The restore calls `UpdateBallState` which warps the ball to the last-viewed history record. This is wrong for a freshly-started trainer run, so the trainer start/resume paths set `m_WasControlled = false` immediately after `ToggleControl()`. Arguably cleaner to move this check into the restore block itself.

### 7. Don't misdiagnose — trust boundary evidence before writing fix code

I wrote two "Fix Trainer ball freeze" commits before finding the actual C-key cause. Both commits contained a mix of:
- **Real bug fixes** (kept): `Init()` removal from trainer setup (countdown loop), `ResetTrainerRunStartTime` guard, ball-lock during countdown, use of `player.m_vball` instead of filtered `m_ControlVBalls`
- **Misdiagnosis code** (reverted): `m_trainerApplyState` deferred-state machinery, 100-step gravity kick in `HitBall::UpdateVelocities`, `ClearDraws` at wrong place causing corridor flicker

Use `git rebase -i` to drop misdiagnosis commits and re-apply only the real fixes as one clean commit. The trainer-debug-wip branch preserved the misdiagnosis work before nuking it from main history.

### 8. Debug instrumentation should go on a side branch

When adding extensive debug logging, file writes, trace counters, etc., commit them to a `-wip` branch before reverting from main. That way if the diagnosis turns out wrong, the code is discoverable for next time.

## Code Style

- `.clang-format` is present: WebKit-based style, Allman braces, 3-space indent, 190-column limit, tabs=3 spaces
- Member variables use `m_` prefix (e.g., `m_Position`, `m_BallHistoryRecords`)
- Enums use `TypeName_Value` naming (e.g., `ModeStateType_Config`, `MenuStateType_Root_SelectMode`)
- Static constants defined as `static const` class members
- COM objects used for rendered elements (`CComObject<Ball>`, `CComObject<Light>`, `CComObject<Rubber>`)

## Known Issues

- **RenderDevice pending buffer assertion**: Debug-only assertion on exit after Ball History use (`m_pendingSharedIndexBuffers.empty()`). Mitigated by `SubmitRenderFrame()` in destructor but may still fire in some exit paths.
- **Trainer key collision with table scripts**: The V key is now the default for the Ball History menu (was C until the Example.vpx Manual Ball Control script collision was diagnosed). Other tables MAY still use V — audit table scripts if a new collision is suspected. Signal for a collision: specific ball velocity values keep getting written every physics step. Grep the table script for the value.

## Trainer phase timeline (for quick reference)

A trainer run flows through these phases in `ProcessModeTrainer`:

1. **Result hold** (top-of-function): if `m_ResultDisplayEndTimeMs != 0` and we're inside the window, draw visuals + lock balls via `m_TrainerLockedBalls`, return early. Skips all phase logic while pass/fail sound plays and visuals are shown at the result location.
2. **Phase 1 — countdown**: `runElapsedTimeMs < countdown`. Draw trainer visuals, lock balls at start position, play countdown beeps. Track locked balls in `m_TrainerLockedBalls`.
3. **Phase 2 — active run**: transition fires `m_SetupBallStarts` block once — clears visuals, unlocks balls, sets pos/vel/angmom from `RunRecord`, clears `m_TrainerLockedBalls`. Physics runs. Pass/fail detection ongoing.
4. **Pass/fail/timeout**: sets `m_ResultDisplayEndTimeMs = currentTimeMs + ResultDisplayDurationMs`, plays async sound, advances `m_CurrentRunRecord`. Next frame goes back to phase 1 (result hold).

All pass/fail/timeout sites use **async** (`PlaySound(id, true)`) — never sync — because sync blocks rendering.
