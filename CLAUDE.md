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

- `master` — tracks upstream `vpinball/vpinball`. Pull upstream changes here first.
- `integration` — `master` + Ball History changes merged together. This is the build/test branch.
- `development` — active Ball History development work.
- Remotes: `origin` = `garybrowndev/vpinball`, `upstream` = `vpinball/vpinball`

### Sync workflow (upstream -> development)
1. `git fetch upstream`
2. Checkout `master`, merge `upstream/master` (fast-forward), push to origin
3. Checkout `integration`, merge `master` — resolve conflicts here (upstream vs Ball History)
4. **Download fresh third-party deps** if upstream added new plugins/libraries
5. **Re-run `make/create_vs_solution.bat`** to regenerate VS project files
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
- **UI** — rendered via ImGui overlay (`PrintScreenRecord` handles on-screen text/menus). Uses default ImGui font (custom fonts crash in ImGui 1.92+ due to atlas rebuild). Activated in-game by pressing "C"
- **Input** — C/R keys registered as InputManager actions (`InputManager.cpp`). Flipper/plunger keys dispatched via ImGui in `LiveUI.cpp`.
- **Settings persistence** — trainer configurations saved/loaded via `CSimpleIni` (.ini files)
- **Rendering** — draws fake balls, intersection circles, lines using the VPinball parts system (`CComObject<Ball>`, `CComObject<Light>`, `CComObject<Rubber>`)

### Key Integration Points

- `Player` class (`src/core/player.h`) — owns the `BallHistory` instance; constructor inits, destructor uninits, `ApplyPlayingState` resets trainer timing
- `LiveUI` (`src/ui/live/LiveUI.cpp`) — calls `BallHistory::Process`, `ProcessKeys`, `ProcessMouse` each frame during `RenderUI`
- `InputManager` (`src/input/InputManager.cpp`) — registers C and R key actions that call `BallHistory::ProcessKeys`
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
- **Ball History debug logger** (`BHLog` class in `ballhistory.h`): writes to `BallHistory/ballhistory_debug.log` in Debug builds. Call `BHLOG(fmt, ...)` and `BHLOG_FLUSH()`.
- **Direct file writes** for quick diagnostics: `{ FILE* _f = nullptr; fopen_s(&_f, "C:\\path\\log.log", "a"); if (_f) { fprintf(_f, "msg\n"); fclose(_f); } }`
- **strncpy_s assertion** (`def.h:919`): upstream's custom `strncpy_s` asserts on truncation. Changed to log instead of assert.

## Code Style

- `.clang-format` is present: WebKit-based style, Allman braces, 3-space indent, 190-column limit, tabs=3 spaces
- Member variables use `m_` prefix (e.g., `m_Position`, `m_BallHistoryRecords`)
- Enums use `TypeName_Value` naming (e.g., `ModeStateType_Config`, `MenuStateType_Root_SelectMode`)
- Static constants defined as `static const` class members
- COM objects used for rendered elements (`CComObject<Ball>`, `CComObject<Light>`, `CComObject<Rubber>`)

## Known Issues

- **Ball frozen after pause/unpause**: After Ball History pauses (SetControl) and unpauses, the ball doesn't move despite `IsPlaying()` returning true and Ball History not overwriting state. Physics engine has the ball registered (`m_vmover`). Under investigation — suspected physics timing issue after `SetPlayState` cycle. See commits on `integration` branch.
- **RenderDevice pending buffer assertion**: Debug-only assertion on exit after Ball History use (`m_pendingSharedIndexBuffers.empty()`). Mitigated by `SubmitRenderFrame()` in destructor but may still fire in some exit paths.
