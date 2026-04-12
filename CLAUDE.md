# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a fork of [Visual Pinball](https://github.com/vpinball/vpinball) (VPinballX) that adds the **Ball History** feature — a system for recording, replaying, and training with ball state history during gameplay. The fork is maintained at `garybrowndev/vpinball`. Only Windows x64 with DirectX is tested for Ball History development.

## Build Instructions (Windows / Visual Studio)

### Quick Build (no CMake)
1. Download precompiled third-party dependencies (`VPinballX-<VERSION>-<BUILD>-<HASH>-dev-third-party-windows-x64-Debug.zip` and the Release variant) from [GitHub Actions](https://github.com/vpinball/vpinball/actions)
2. Extract both to `./third-party`, overwriting existing files
3. Revert any git changes caused by the overwrite
4. Run `make/create_vs_solution.bat`, select `2022`
5. Open `.build/vsproject/VisualPinball.sln` in Visual Studio 2022
6. Set `vpx` as the Startup Project
7. Build for Debug or Release / x64

### CMake Build (windows-x64)
```
platforms/windows-x64/external.sh
cp make/CMakeLists_bgfx-windows-x64.txt CMakeLists.txt
cmake -G "Visual Studio 17 2022" -A x64 -B build
cmake --build build --config Release
```

### Running Tests
Build and run the `vpx-test` project in the VS solution (`.build/vsproject/vpx-test.vcxproj`). Test sources are in `tests/`.

## Branch Strategy & Update Workflow

- `master` — tracks upstream `vpinball/vpinball`. Pull upstream changes here first.
- `integration` — `master` + Ball History changes merged together. This is the build/test branch.
- `development` — active Ball History development work.

### Sync workflow (upstream -> development)
1. Pull upstream into `master`
2. Merge `master` into `integration`, verify it builds and works
3. Merge `integration` into `development` to pick up latest upstream

### Ship workflow (development -> integration)
1. Push Ball History changes from `development` into `integration`
2. Build and test from `integration` (it has latest upstream + your changes)

## Architecture

### Source Layout (`src/`)

| Directory | Purpose |
|-----------|---------|
| `core/` | Application core: main entry point, player loop, settings, pin table, undo, and **Ball History** |
| `physics/` | Physics engine, collision detection, quad trees, ball/flipper/plunger hit objects |
| `renderer/` | Rendering abstraction (DirectX/OpenGL/bgfx), shaders, textures, render targets, VR |
| `parts/` | Pinball table elements (Ball, Bumper, Flipper, Gate, Kicker, Light, Ramp, Rubber, Spinner, Surface, Trigger, etc.) |
| `ui/` | Editor UI, debugger, live UI, resource files, dialog properties |
| `input/` | Input handling (DirectInput keyboard/mouse/joystick, XInput, SDL, OpenPinDev) |
| `math/` | Vectors, matrices, mesh math, bounding boxes |
| `audio/` | Audio playback |
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
- **UI** — rendered via ImGui overlay (`PrintScreenRecord` handles on-screen text/menus). Activated in-game by pressing "C"
- **Settings persistence** — trainer configurations saved/loaded via `CSimpleIni` (.ini files)
- **Rendering** — draws fake balls, intersection circles, lines, and corridors using the VPinball parts system (`Ball`, `Light`, `Rubber` COM objects)

### Key Integration Points

- `Player` class (`src/core/player.h`) — owns the `BallHistory` instance, calls `Init`, `Process`, `ProcessKeys`, `ProcessMouse` each frame
- `HitBall` (`src/physics/hitball.h`) — the physics ball object whose state gets captured
- `PinTable` (`src/parts/pintable.h`) — provides table metadata and element access
- Table parts (`Kicker`, `Flipper`, etc.) — Ball History interacts with kickers for ball creation and flippers for strength/friction variance

### Other Key Directories

- `make/` — Visual Studio solution/project templates, CMake files per platform, build scripts
- `platforms/` — per-platform external dependency build scripts
- `plugins/` — runtime plugin implementations (PinMAME, DMD, DOF, B2S, etc.)
- `standalone/` — standalone builds for iOS, Android, macOS (not Ball History target)
- `third-party/` — precompiled external dependencies (gitignored, must be downloaded)
- `scripts/` — VBScript files for various pinball machine ROM emulation
- `tables/` — sample pinball table files

## Code Style

- `.clang-format` is present: WebKit-based style, Allman braces, 3-space indent, 190-column limit, tabs=3 spaces
- Member variables use `m_` prefix (e.g., `m_Position`, `m_BallHistoryRecords`)
- Enums use `TypeName_Value` naming (e.g., `ModeStateType_Config`, `MenuStateType_Root_SelectMode`)
- Static constants defined as `static const` class members
- COM objects used for rendered elements (`CComObject<Ball>`, `CComObject<Light>`, `CComObject<Rubber>`)
