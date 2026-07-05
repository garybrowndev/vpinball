# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a fork of [Visual Pinball](https://github.com/vpinball/vpinball) (VPinballX) that adds the **Ball History** feature — a system for recording, replaying, and training with ball state history during gameplay. The fork is maintained at `garybrowndev/vpinball`. Only Windows x64 with DirectX is tested for Ball History development.

## Build Instructions (Windows / Visual Studio)

### Primary build: CMake + prebuilt deps (matches upstream CI)

Upstream builds Windows via **CMake** (`cmake -G "Visual Studio …" && cmake --build`). The old hand-maintained VS-solution path (`create_vs_solution.bat`) **broke after upstream's "import cleanup"** restructured `main.h` — see the deprecation note below. Use CMake going forward. VS**2022** is fine for the main build (only `external.sh` wants VS2026).

1. **Get matching prebuilt third-party deps — do NOT run `external.sh`.** Download the `dev-third-party-windows-x64` Release (+Debug only if you also build Debug) artifact from a CI run that matches your code. **Best source: our own fork's run for the current commit** — it builds against the exact merged dep SHAs (after a Ball-History+upstream merge the upstream artifacts won't match). The artifact extracts to `build-libs/ include/ runtime-libs/`; merge those into `third-party/`:
   ```powershell
   # gh must be on the personal account for fork access:  gh auth switch --user garybrowndev
   $RUN = <fork 'vpinball' workflow run id for the current commit>   # e.g. the development-branch run
   gh run download $RUN --repo garybrowndev/vpinball -n "VPinballX-<VER>-<sha>-dev-third-party-windows-x64-Release.zip" --dir $env:TEMP\vpx-deps
   robocopy "$env:TEMP\vpx-deps\build-libs"   third-party\build-libs   /E
   robocopy "$env:TEMP\vpx-deps\include"      third-party\include      /E
   robocopy "$env:TEMP\vpx-deps\runtime-libs" third-party\runtime-libs /E
   git checkout -- third-party/   # revert tracked-file changes; gitignored binaries keep the fresh versions
   ```
2. **Configure** (one-time per clean build dir; reconfigures automatically after that). Upstream now
   ships a **single tracked root `CMakeLists.txt`** selected by cache vars — there is **no template to
   copy** (the old `make/CMakeLists_<flavor>-<platform>-<arch>.txt` files were **removed upstream**; only
   the per-plugin `make/CMakeLists_plugin_*.txt` and `make/CMakeLists_sources.txt` remain):
   ```powershell
   cmake -G "Visual Studio 17 2022" -A x64 -B build -DRENDERER=BGFX -DPLATFORM=windows -DARCH=x64
   ```
   - `RENDERER` = `BGFX` (default) | `GL` | `DX9`. `PLATFORM`/`ARCH` auto-detect from the host, so on a
     Windows x64 box `cmake -G "Visual Studio 17 2022" -A x64 -B build` alone yields a BGFX build — but
     passing the vars explicitly is clearer and is **required** to select GL/DX9.
   - Switch renderer by configuring a **separate build dir** with a different `-DRENDERER` (e.g.
     `-B buildgl -DRENDERER=GL`) — no file copy, no repo-root mutation.
3. **Build** (cap file-parallelism to avoid C1076 heap errors):
   ```powershell
   cmake --build build --config Release -- /m:9 /p:CL_MPCount=9                       # full: vpx + all plugins
   cmake --build build --config Release --target vpinball -- /m:9 /p:CL_MPCount=9      # just the exe (faster; skips plugin post-build DLL copies)
   ```
   Output exe by renderer (CMake `RUNTIME_OUTPUT_NAME`): **BGFX → `build/Release/VPinballX_BGFX64.exe`**,
   GL → `VPinballX_GL64.exe`, DX9 → `VPinballX64.exe`; plugins land in `build/Release/plugins/`.

- **Deploy convention**: `build/Release/VPinballX_BGFX64.exe` → cabinet `Z:\visual pinball\VPinballX_BGFX64_BH.exe` (rename adds `_BH`). **Close VPX on the cabinet first** or the copy fails with "file in use".
- **After an upstream merge**: re-download fresh `dev-third-party` deps (versions bump); CMake reconfigures itself from the tracked root `CMakeLists.txt` — no template copy, no `create_vs_solution.bat`. If a plugin post-build fails on a missing DLL (e.g. `libserialport64-0.dll`), your deps are stale → refresh them.
- **Never run `platforms/windows-x64/external.sh`** unless rebuilding all 12 deps from source: it needs **VS2026 + MSYS2 (`/c/msys64` UCRT64) + nasm**. The prebuilt-deps download replaces it.

### Build tips (CMake)
- **Header changes rebuild automatically** — CMake/MSBuild tracks header dependencies, so the old VS-solution "delete all `.obj`" ritual isn't needed. For a fully clean build, delete the `build/` dir and re-run the configure step.
- **Build just the exe** (faster; also dodges plugin post-build DLL-copy failures when deps are slightly stale): `cmake --build build --config Release --target vpinball -- /m:9 /p:CL_MPCount=9`. The cabinet keeps its existing plugins.
- **Cap file parallelism** with `/p:CL_MPCount=9` (75% of 12 cores) to avoid C1076 heap-limit errors (`/m:9` caps project-level parallelism).
- **GL/DX9 instead of BGFX**: configure a separate build dir with `-DRENDERER=GL` (or `DX9`), e.g. `cmake -G "Visual Studio 17 2022" -A x64 -B buildgl -DRENDERER=GL -DPLATFORM=windows -DARCH=x64`. Caveat: VS2022 builds **BGFX** cleanly, but **GL/DX9 may fail locally with DXGI header errors** — CI (VS2026 generator) builds them fine, so pull the GL/DX exes from a green CI run (see `deploy-vpx`) rather than fighting the local toolchain.
- **Fork vs upstream deps**: on pure `master` the `dev-third-party` zips from `vpinball/vpinball` match; on `integration`/`development` (Ball History + upstream merged) only **our fork's** run for that commit has matching dep SHAs — pull from `garybrowndev/vpinball` (CMake build, step 1).
- **gh account**: writes/artifact pulls on the fork need the personal account active — `gh auth switch --user garybrowndev` (the `gary-brown_bplogix` EMU account can't access `garybrowndev/vpinball`).

> _Legacy (deprecated VS-solution path only — see note below): `create_vs_solution.bat` regen after merges, `MSBuild …VisualPinball.sln -t:vpx -p:Configuration=Release_BGFX`, `-p:BuildProjectReferences=false`. These no longer build merged trees._

### ⚠️ Deprecated: VS-solution "Quick Build" (`create_vs_solution.bat`) — broken after upstream import-cleanup

The old flow (download deps → `make/create_vs_solution.bat` → open `.build/vsproject/VisualPinball.sln` in VS2022 → build `vpx`) **no longer compiles** on any tree that includes upstream's "Import cleanup" (the −210-line `main.h` rewrite, ~mid-2026). That path builds its precompiled header through `main.h`, which used to front-load every include; upstream moved them to point-of-use, so the PCH no longer supplies core types and the build dies in the part headers with a cascade (`FRect3D` undefined → `IScriptable`/`CWinApp` undefined, etc.). Restoring it means reconstructing the entire old `main.h` include order, which fights upstream — so we switched to CMake (above). The `make/*.vcxproj` / `vpx-core.vcxitems` files and `create_vs_solution.bat` are kept only for historical reference / non-merged branches.

### Running Tests
Build and run the `vpx-test` project in the VS solution (`.build/vsproject/vpx-test.vcxproj`). Test sources are in `tests/`.

### Running VPinball from CLI
```bash
# Always launch with the Example table pre-loaded for faster debugging (CMake output path):
start build\Release\VPinballX_BGFX64.exe "C:\code\Pinball\vpinball_ballhistory\Example.vpx"
```
Press F5 to play, ESC to return to editor, Q to quit from play mode.
When debugging Ball History, always launch with a table loaded — starting empty wastes time navigating menus.

## Android x86_64 emulator build (pioneer port — fork-local only)

A working x86_64 Android cross-compile path lives on branch `android-x86_64-discovery`. **Upstream has no x86_64 path** — CI is arm64-only — so this is fork-local. Never PR upstream; the changes are scoped to give Gary an emulator dev workflow on his Windows x86_64 box.

### Why this path exists

The Android emulator on a Windows/Linux **x86_64 host** can no longer run an arm64 system image: Google removed the `berberis` ARM-translation layer ~2026-04-22. After that removal the only AVD that boots on Gary's box is `VPinball_Pixel7_x86` (x86_64), and an APK only installs if its native libs match the AVD ABI — so `arm64-v8a` APKs are rejected. The **only** path to emulator-based dev is a full x86_64 cross-compile of VPinball + every third-party lib + BGFX.

The arm64 build path (`platforms/android-arm64-v8a/`) for the Samsung S21 is untouched and still primary.

### Build sequence (WSL Ubuntu — Windows is unsupported by the bash scripts)

```bash
export ANDROID_NDK_HOME=/mnt/c/Users/gary.brown/AppData/Local/Android/Sdk/ndk/28.2.13676358
export ANDROID_NDK=$ANDROID_NDK_HOME
export ANDROID_NDK_ROOT=$ANDROID_NDK_HOME

# 1. Build all 12 third-party libs for x86_64 (long — ~30-60 min)
BUILD_TYPE=Debug ./platforms/android-x86_64/external.sh

# 2. Apply BGFX emulator workaround patches (see Three Walls below)
python3 ./platforms/android-x86_64/_patch_bgfx_debugname.py
python3 ./platforms/android-x86_64/_patch_skip_swapchain_recreate.py

# 3. Rebuild BGFX in Release config with the patches, overlay onto third-party/
./platforms/android-x86_64/_rebuild_bgfx_release.sh

# 4. Cross-compile libvpinball.so for x86_64
./platforms/android-x86_64/_build_libvpinball.sh
```

Then from PowerShell (WSL gradle hits build-tools/35.0.0 corruption from Gary's duplicate SDK dir):

```powershell
cd standalone\android
$env:ABI = "x86_64"
.\gradlew assembleMobileDebug
```

The `ABI` env var (default `arm64-v8a`) drives `build.gradle.kts`'s jniLibs path, build dir, and `abiFilters`. Omit it for the S21 build; set `x86_64` for emulator. **Never re-introduce the literal hardcode** — both archs share this file.

Also: `make/CMakeLists_bgfx_lib.txt:21` is `set(CMAKE_ANDROID_ARCH_ABI ${ARCH})`. With the arm64 external.sh passing `-DARCH=arm64-v8a` (multiple call sites), this reduces to the original hardcode. Provably no-op for arm64.

### The three emulator graphics walls and their fixes

The Android emulator's `gfxstream` + `vulkan.ranchu.so` drivers implement *just enough* Vulkan/GLES for typical Android apps. BGFX exercises paths the emulator hasn't validated. Three distinct crashes hit during the pioneer work:

| Wall | Symptom | Root cause | Fix |
|---|---|---|---|
| **GL backend** | `bgfx::gl::GlContext::create+8317` SIGABRT under `-gpu host` and `-gpu swiftshader_indirect` | Emulator GLES rejects the EGL config BGFX wants | Don't use GL — set `GfxBackend = Vulkan` in `VPinballX.ini` |
| **Vulkan + debug-utils** | `vk_common_SetDebugUtilsObjectNameEXT+35` SIGABRT in `vulkan.ranchu.so` | BGFX's `setDebugObjectName` template (gated by `BGFX_CONFIG_DEBUG_OBJECT_NAME`, defaults on with `BGFX_CONFIG_DEBUG`) hits a debug-helper extension the emulator's Vulkan driver doesn't implement properly. Building BGFX in `Release` does NOT fully skip the calls | `_patch_bgfx_debugname.py` rewrites the template body to `BX_UNUSED(...)` — full no-op even in Release |
| **Vulkan swapchain recreate** | `bgfx::vk::SwapChainVK::createSwapChain+1156` → `libvulkan.so CreateSwapchainKHR+2184` SIGSEGV during the first-frame resolution update | `vulkan.ranchu.so` returns null swapchain handle when BGFX recreates the swapchain on resolution-change | `_patch_skip_swapchain_recreate.py` short-circuits `recreateSurface = false; recreateSwapchain = false` in `SwapChainVK::update`. **Plus** `~/.android/advancedFeatures.ini` flips `VulkanNativeSwapchain = on` to route Vulkan via ANGLE → host NVIDIA driver, bypassing `vulkan.ranchu.so` entirely |

### Required emulator host flags (`~/.android/advancedFeatures.ini`)

```ini
Vulkan = on
GLDirectMem = on
VulkanSnapshots = off
VulkanNativeSwapchain = on
```

`VulkanNativeSwapchain = on` is the load-bearing one. Without it the host GPU isn't reachable via ANGLE and walls 2/3 re-emerge.

### Run sequence

```powershell
# AVD launch
$env:ANDROID_HOME = "C:\Users\gary.brown\AppData\Local\Android\Sdk"
& "$env:ANDROID_HOME\emulator\emulator.exe" -avd VPinball_Pixel7_x86 -gpu host -no-snapshot

# Install + run
adb install -r standalone\android\app\build\outputs\apk\mobile\debug\VPinballX_BGFX-*-mobile-debug.apk
adb shell am start -n org.vpinball.vpinball_bgfx/org.vpinball.app.VPinballActivity
```

In-app, set Vulkan + 1080×2400 + FullScreen in `VPinballX.ini` before running the table. The ralph loop in `_research/android-x86_64/_ralph_iteration.sh` documents the full sequence end-to-end.

### Troubleshooting: emulator gets slow after a long session

After ~1 hour of use — especially if there were any native crashes (SIGSEGV/SIGABRT/SIGFPE) — frame times degrade dramatically (16ms → 200-2700ms per `dumpsys gfxinfo`). Cause: emulator state accumulates leaked GPU resources, surface/swapchain refcount imbalances, fragmented heap, and stuck tombstone-collection threads. Stopping gradle daemons and shutting down WSL does **not** help (Windows reallocates immediately). The fix:

1. Kill QEMU/emulator: `& adb emu kill; Stop-Process -Name qemu*,emulator* -Force`
2. Relaunch fresh (`-no-snapshot`, ~30 sec to boot)
3. **Do NOT `adb install -r` the APK** — this wipes BGFX's compiled-shader cache and you'll pay for re-JIT on first encounter. Just relaunch the existing install with `am start`.

If slowness persists after a clean restart with cache preserved, then suspect host pressure or BGFX/Vulkan path issues. Otherwise, restarting on this loop is normal hygiene during long debug sessions.

### Research/debug helpers (local-only, not committed)

`_research/android-x86_64/` (gitignored) holds:
- `_ralph_iteration.sh` — the deterministic build → install → drive UI → screenshot loop used to fix bugs without manual taps
- `_switch_gpu.ps1` — kill emulator, relaunch with a different `-gpu` mode, wait for boot
- `_drive_app.sh`, `_dump_manifest.sh`, `_apk_check.sh`, `_arch_check.sh`, `_set_backend.sh`, `_set_vulkan.sh` — assorted runtime/debug helpers
- `seed/` (tables.json + blankTable.vpx pre-seeded into app data) and `ralph_out/` (per-iteration screenshots/logcat) — research artifacts

Don't promote any of these to `platforms/android-x86_64/` — the rule is: **only files the build pipeline strictly needs go there.** Everything else stays research-local.

## Branch Strategy & Update Workflow

- `master` — tracks upstream `vpinball/vpinball` **plus one cherry-picked patch** (see "Master is not pure upstream" below).
- `integration` — `master` + Ball History changes merged together. This is the build/test branch **and the canonical deploy-to-cabinet source**: the standing `_BH` builds on the VPM come from a green `integration` CI run. Finished WIP graduates here via the ship PR (below) before it becomes the cabinet's real build.
- `development` — active Ball History development work. **Test/debug only** — development builds may be deployed to the cabinet during an iteration loop (play-test gate, debug-vpx), but development is never the standing cabinet build. Real gameplay runs `integration`.
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

### Cabinet ini recovery recipe (known-good baseline as of 2026-05-14)

Minimum config needed to recover a working cabinet from a blank ini. Everything else can stay at code defaults; VPX repopulates auto-state on first launch.

```ini
[Player]
; --- View mode + autofit (the unlock) ---
BGSet                = 1                  ; Cabinet view set
CabinetAutofitMode   = 2                  ; Fit Screen — autofit derives per-table values
FullScreen           = 1

; --- Display target ---
PlayfieldDisplay     = 1
PlayfieldWidth       = 3840
PlayfieldHeight      = 2160
RefreshRate          = 144
GfxBackend           = Direct3D11
SyncMode             = 1                  ; Hardware V-Sync. NOT 3 — Frame Pacing stutters.
MaxFramerate         = -1.0               ; Uncapped; V-Sync drives cadence

; --- Physical cabinet geometry (autofit math reads these) ---
ScreenWidth          = 95.889999          ; cm — measured TV display area
ScreenHeight         = 53.939999
ScreenInclination    = 0.0
ScreenPlayerX        = 0.0
ScreenPlayerY        = 0.0
ScreenPlayerZ        = 70.0               ; cm eye height above playfield bottom edge

; --- Anti-stretch + max quality push (Gary's RTX-class GPU) ---
BallAntiStretch      = 1
AAFactor             = 1.0                ; 100% (no supersample); 1.5 was too soft, 2.0 broke 144Hz
MSAASamples          = 3                  ; 8 samples — max
FXAA                 = 7                  ; Quality FAAA — max
Sharpen              = 2                  ; Bilateral CAS — max
SSRefl               = 1                  ; Additive screen-space reflection — max

[TableOverride]
ViewCabMode          = 2                  ; Window mode (required for autofit)
ViewCabRotation      = 0.0                ; Post-Vincent autofit fix — see history below
ViewCabWindowTopScale = 1.0               ; Calibrate per-cabinet via F12; ~0.7 on Gary's rig
```

Other quality knobs (`PFReflection`, `AlphaRampAccuracy`, `MaxTexDimension`, `DynamicAO`, `ForceAnisotropicFiltering`) are already at max in their **code defaults** — no ini override needed.

### `ViewCabWindowTopScale` — fork-local autofit calibration knob (added 2026-05-13)

Multiplier applied to autofit-computed `WindowTopZOfs` so the top-glass-plane bias on Gary's cabinet can be trimmed **proportionally across all tables**. Default `1.0` = no-op. `0.7` trims every table's top by 30%, preserving per-table variation (short tables get small trim, tall tables get large trim — what an offset can't do).

- Property defs in `src/core/Settings_properties.inl` (DT/FSS/Cab triple at lines ~684, ~705, ~726).
- Multiplier applied in `src/renderer/ViewSetup.cpp` `ApplyTableOverrideSettings` (line 185-186).
- F12 → Point Of View slider in `src/ui/live/ingameui/PointOfViewSettingsPage.cpp` (visible only in autofit modes; setter applies incremental ratio `mWindowTopZOfs *= (v/prev)` — **does NOT** call `SetWindowAutofit` because that clobbers every other slider's transient state).
- Page's `SaveMode` upgraded from `Table` to `Both` so changes can be saved globally OR per-table.

Fork-local feature — upstream-worthy PR but not yet filed.

### Cabinet rotation: post-Vincent regime change (April 29, 2026)

**Current rule**: `ViewCabRotation = 0.0` in `[TableOverride]` is correct for Gary's landscape cabinet under post-Vincent-fix builds.

Upstream commit `89a84494c "Fix cabinet autofit on landscape display"` (Vincent, April 29) changed `SetWindowAutofit` to derive landscape rotation itself — removing the `mViewportRotation = GetRotation(1080*aspect, 1080)` line and replacing with `mViewportRotation = 0.f`. Plus an axis-swap in the vertical-offset dichotomy search (`-bottomFlipper.x` for landscape vs `bottomFlipper.y` for portrait).

**Symptom of getting this wrong**: with `ViewCabRotation = 90` (the old empirically-verified value) on a post-fix build, the table renders **sideways**. Rollback to a pre-fix build OR change ini to `0.0` — both fix it.

**Historical context** (pre-Vincent regime, kept for reference only — no longer the current rule):

> `ViewCabRotation = 90.000000` was the universal cabinet-orientation fix, empirically verified across a 500-table audit. Produced the correct landscape orientation for both Legacy-mode tables (ROTF=270 baked in `.vpx`) and Window-mode tables with the broken-author bug (ROTF=0 baked, e.g. White Water v1.2, 4 Queens, World Poker Tour, Space Station VPW). Worked because Window mode's `GetRotation` auto-adds 270° for landscape viewports: `(270 + 90) % 360 = 0` effective — no rotation, table fills landscape directly.

**Practical implication**: when triaging a "wrong orientation" report, first check whether the build is pre- or post-Vincent. If post-Vincent, use 0. If pre-Vincent, use 90. Per-table sidecar `.ini` files are only needed for genuine outliers.

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
