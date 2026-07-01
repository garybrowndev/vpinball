---
name: debug-vpx
description: Be Gary's hands-on dev/debug assistant for VPinballX on the VPM cabinet — build, deploy, launch (optionally under cdb), keep cdb attached for the whole session, and iterate. Drives the full inner-loop: Gary plays/tests on the cabinet, comes back saying "this crashed", "X doesn't remember Y", "I want it to do Z instead", "what's the value of foo when I press flipper?", and Claude reads the attached debugger state if useful, edits source, rebuilds, redeploys, relaunches, and the cycle continues. Use this whenever Gary mentions debugging VPX, fixing a crash, fixing a hang, watching a variable, tracking a state-machine bug, adding or changing a behavior in VPX/Ball-History, asking what something does at runtime, or any "let's iterate on this on the cabinet" workflow. Triggers on phrases like "let's debug", "let's do a cycle", "X is crashing", "the cabinet hangs", "X doesn't remember", "I want X to happen", "make it do Y", "find the bug", "what's the value of", "watch this variable", "Black Pyramid does X", or any reproduction recipe involving Black Pyramid or another VPM-backed table.
---

# debug-vpx — VPinballX inner-loop assistant

This is the workflow Claude uses to be Gary's hands-on dev partner for VPX on the VPM cabinet (`virtualpin` / `192.168.1.31`). The pattern is:

> Build → deploy → launch (often under cdb) → Gary plays / tests → Gary reports something (crash, missing behavior, desired change, "what's the value of X when Y") → Claude observes via attached debugger if useful, edits source, rebuilds, redeploys, relaunches → Gary tests again → loop.

It covers four overlapping kinds of work, all using the same plumbing:

| Mode | What Gary says | What Claude does |
|---|---|---|
| **Crash** | "VPX crashed when I hit X" | Read the cdb crash log or WER dump → identify file:line → propose fix → rebuild → redeploy |
| **Hang** | "It hung after I did Y" | Non-invasive cdb attach → dump all threads → find the blocked one → identify root cause → fix |
| **Behavior bug** | "X doesn't remember Z", "Y fires twice", "the trainer skips a step" | Read source for the relevant state machine → set conditional breakpoint to log the state at runtime if needed → identify the logic error → fix |
| **Feature work** | "I want pressing F to do G", "add a counter for H", "make Trainer do I" | Find the right code site → write the change → rebuild → redeploy → Gary tries it → iterate |

Same scripts, same cdb session, same deploy step. The distinction is *what input drives Claude's edit* — a crash log, a watched variable, a piece of code, or a spec from Gary.

## Cabinet quick-reference

| Item | Value |
|---|---|
| Hostname | `virtualpin` (DNS sometimes flaky — fallback IP `192.168.1.31`) |
| C$ admin share | `\\virtualpin\c$` mounted as `Z:\` on the dev box |
| D$ share | `\\virtualpin\d$` (Black Pyramid lives here, no Z-mapping) |
| Debug deploy folder | `C:\Visual Pinball\debug\` (cabinet) ≡ `Z:\Visual Pinball\debug\` (dev) |
| Stock release exe | `C:\Visual Pinball\VPinballX_BGFX64_orig.exe` (working reference) |
| WER dumps + cdb logs | `C:\Dumps\` (cabinet) ≡ `Z:\Dumps\` (dev) |
| Symbol cache | `C:\symcache\` (cabinet and dev) |
| cdb on cabinet | `C:\tools\Debuggers\x64\cdb.exe` |
| cdb on dev | `C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe` |
| PAExec on dev | `C:\tools\sysinternals\paexec.exe` |
| Black Pyramid | `D:\OneDrive\machine backups\virtualpin\Emulation\Visual Pinball\tables\Black Pyramid (Bally 1984) v600.vpx` |

**Always use `Z:\`** for file ops from the dev box (Read, Write, Glob, Grep, Copy-Item) — faster + cleaner than UNC. Inside batch files that **run on the cabinet**, keep `C:\...`.

**Never use PsExec to launch a GUI app on the cabinet** — produces black-window UI (DComp swap-chain init fails on a token/desktop DACL mismatch). PAExec is the drop-in fix.

## Session shape: setup once, iterate often

Most sessions follow this rhythm:

1. **Once per session — provision** (skip if cabinet is already provisioned and the deploy folder is current):
   - Build the Debug config via CMake (BGFX flavor; outputs to `build/Debug/`)
   - Deploy exe + pdb to `Z:\Visual Pinball\debug\`
   - Push the bundled scripts (`scripts/*`) to `Z:\Dumps\` if any are missing or stale (see "Bundled scripts" section)

2. **Once per session — launch with cdb attached** (most useful default — catches whatever happens, lets you peek at runtime state when needed):
   - `preflight-kill` to clean cabinet state
   - Launch via `launch-bp-cdb.cmd` with first-chance crash filters

3. **Loop, many times — Gary tests, reports, Claude responds:**
   - Gary plays / tests something specific (e.g. "hits flipper, watches Trainer state")
   - Gary reports back: crash / hang / weird behavior / feature request / "what's the value of X"
   - Claude does ONE of the four mode reactions below
   - Rebuild + redeploy + (optionally) relaunch under cdb → Gary tests again

The "always-attached cdb" stance is the key idea. It costs almost nothing (one extra invocation at launch) and means when something unexpected happens, the debugger is already there to catch and report.

## The loop in detail

### Step 1 (once per session): Build and deploy

Build via **CMake** — the old `create_vs_solution.bat` / VS-solution path is deprecated and no longer compiles merged trees (upstream's import-cleanup broke the PCH; see CLAUDE.md). Upstream ships a single tracked root `CMakeLists.txt` selected by cache vars (the old `make/CMakeLists_<flavor>-*.txt` templates were removed) — configure once per clean `build/` dir; after that CMake reconfigures itself on each build:
```powershell
cmake -G "Visual Studio 17 2022" -A x64 -B build -DRENDERER=BGFX -DPLATFORM=windows -DARCH=x64
```

Debug builds need the **Debug-flavor** prebuilt deps present (download the `dev-third-party-windows-x64-Debug` artifact alongside Release — see CLAUDE.md step 1). Only Debug configs emit the PDBs cdb needs for resolved stacks.

Build the Debug config, capping parallelism at `/m:9` (75% of Gary's 12 cores — full parallel pegs the laptop and trips C1076 heap-limit errors):
```powershell
cmake --build build --config Debug --target vpinball -- /m:9 /p:CL_MPCount=9 | Select-Object -Last 5
```

Sanity-check the PE header (catches the stack-reserve=0 class of bug — see `references/known-bugs.md`):
```powershell
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64\dumpbin.exe' /headers 'build\Debug\VPinballX_BGFX64.exe' | Select-String 'stack reserve|stack commit'
```
Expected: `100000` and `1000`.

Deploy:
```powershell
Copy-Item 'build\Debug\VPinballX_BGFX64.exe' 'Z:\Visual Pinball\debug\VPinballX_BGFX64.exe' -Force
Copy-Item 'build\Debug\VPinballX_BGFX64.pdb' 'Z:\Visual Pinball\debug\VPinballX_BGFX64.pdb' -Force
```

If copy fails with "file in use", the cabinet still has VPX or cdb running — preflight-kill first (next step) and retry.

### Step 2 (once per session): Launch under cdb

```powershell
& 'C:\tools\sysinternals\paexec.exe' \\virtualpin -d cmd /c 'C:\Dumps\preflight-kill.cmd'
Start-Sleep 4
& 'C:\tools\sysinternals\paexec.exe' \\virtualpin -d -i 1 cmd /c 'C:\Dumps\launch-bp-cdb.cmd'
```

`launch-bp-cdb.cmd` runs cdb with first-chance filters for AVs (`c0000005`), integer divide-by-zero (`c0000094`), and stack overflow. Output streams to `C:\Dumps\vpx-under-cdb.log` (= `Z:\Dumps\vpx-under-cdb.log` from dev).

For pure feature work where you don't expect crashes, you can use `launch-bp-direct.cmd` instead (no debugger overhead, faster startup). But the default should be **cdb-attached** — the overhead is tiny and it's much better to be ready for an unexpected crash than to have to restart the whole cycle when one happens.

### Step 3 (loop): Gary tests + reports

Gary plays Black Pyramid (the canonical VPM-backed test table — covers SS+B2S+DmdDevice path) or whatever table is relevant. Tell him concretely what to do: "load the table, press flipper", "open Ball History menu, switch to Trainer mode, press R three times", etc.

When Gary comes back, identify which mode applies — crash / hang / behavior / feature — and respond accordingly.

### Step 4 (loop, mode-specific): Observe / read state if needed

**Crash**: read `Z:\Dumps\vpx-under-cdb.log`. The cdb session already broke on the first-chance exception. Look at:
- The exception line (e.g. `Access violation - code c0000005`)
- The faulting instruction + register dump
- The stack at fault (frame 0 = where it crashed; trace upward to find the calling code in our source)
- If you need more: spawn another cdb non-invasive attach via `Z:\Dumps\probe-allthreads.cmd` to dump every thread.

**Hang**: process is alive but unresponsive. Use `Z:\Dumps\probeNow.cmd` for thread 0, or `probe-allthreads.cmd` for all threads. Look for who's stuck and on what (typically `WaitForSingleObject`, `WaitForMultipleObjects`, or polling `Sleep`). If thread 0's stack contains `Player::MultithreadedGameLoop` then it's NOT hung — the game loop is running normally and the "hang" is a frontend / focus issue. Re-investigate.

**Behavior bug** ("X doesn't remember Y", "fires twice", state-machine wrong): if the bug is reproducible via a specific keypress/event, attach a watch breakpoint that prints state on every hit without stopping execution. Drop a custom breakpoint script in `Z:\Dumps\watch.txt` like:
```
bu MyDLL!MyClass::MyMethod ".echo === HIT ===; ? @$tid; dv /V /t; gc"
g
```
Then attach with that script:
```cmd
"C:\tools\Debuggers\x64\cdb.exe" -p <pid> -lines -y "..." -c "$<C:\Dumps\watch.txt"
```
Each hit prints the local variables (`dv`) and continues (`gc`). After Gary repros the bug, the cdb log has a series of state snapshots you can read to see exactly what diverged. See `references/cdb-script-syntax.md` for the full bp/dv command grammar.

**Feature work**: no debugger interaction needed unless you want to verify a specific code path. Skip to step 5.

### Step 5 (loop): Edit source, rebuild, redeploy

Make the change locally. Re-run the `cmake --build` command from Step 1 — CMake/MSBuild tracks dependencies, so source-only edits relink incrementally and fast (often <30s); no `create_vs_solution.bat` regen, no `.obj` deletion ritual. Only re-`Copy-Item` the CMakeLists if you switch flavor (BGFX↔GL). Redeploy the exe + pdb (Copy-Item the same way).

If the running cdb session is still attached and the binary hasn't changed yet on disk: tell Gary to close VPX (or run `preflight-kill`), then redeploy + relaunch (Step 2).

If there's NO running session (cdb already exited because crash), the file isn't locked and you can copy directly.

### Step 6 (loop): Tell Gary what to test next

Be specific. "Press flipper again — Trainer should now reset between runs" is better than "try it". Include the exact key, the exact sequence, and the exact expected outcome.

Then back to Step 3.

## Bundled scripts (in `scripts/`)

These live in this skill folder and should be pushed to `Z:\Dumps\` on first use of the skill in a new session:

```powershell
Copy-Item '.claude\skills\debug-vpx\scripts\*' 'Z:\Dumps\' -Force
```

| Script | When to use |
|---|---|
| `preflight-kill.cmd` | Always, before any deploy. Kills VPX/B2S/DMD/cdb. |
| `launch-bp-direct.cmd` | Pure feature work, no debugger needed |
| `launch-bp-cdb.cmd` | **Default launch** — cdb attached, first-chance crash filters armed |
| `probeNow.cmd` | Quick thread-0 stack dump on a running process (non-invasive) |
| `probe-allthreads.cmd` | All-threads stack dump + key module list (non-invasive) |
| `cdb-bp-template.txt` | Starting point for custom watch/break scripts — copy and edit |

## Reference docs (in `references/`)

Read these on demand:

- `cdb-script-syntax.md` — quoting rules for `-c "$<file"`, `bu` command syntax for watchpoints, MASM expression pitfalls, why `-g` breaks `-c` execution, the cdb command cookbook
- `known-bugs.md` — catalog of bugs we've shipped fixes for, with the diagnostic stack that pointed to each. **Read this BEFORE diving into a fresh investigation** — many "new" bugs are repeats of patterns we've already solved.
- `symbol-setup.md` — `_NT_SYMBOL_PATH` patterns, why public Microsoft symbols matter for CLR/Win32 frames (huge offsets like `clr!Foo+0x239160` are a red flag indicating missing symbols)
- `cabinet-setup.md` — one-time cabinet provisioning (Debugging Tools install, Z:\ mapping, WER LocalDumps registry, PAExec install). Skip unless rebuilding the cabinet from scratch.

## Gotchas (the pain we earned)

1. **PsExec → black windows** for any GUI launch. Use PAExec. Stop trying PsExec flag variants.
2. **PAExec `-i 1` is required** for the GUI to render on the cabinet's display (without `-i 1`: session-0 invisible-window land).
3. **PAExec `-d`** = don't wait. Combine `-d -i 1` for fire-and-forget GUI launches.
4. **Always preflight-kill before redeploy.** File locks on the exe/pdb will silently fail your Copy-Item.
5. **Build cap `-m:9`.** Full parallel pegs the laptop and trips C1076.
6. **Debug PE header `SizeOfStackReserve = 0` will hang VPinMAME tables.** Always verify `dumpbin /headers` shows `100000` after build (see `known-bugs.md` for the full story).
7. **Pull public CLR/Win32 symbols.** A `+0x239160` offset means you're NOT in the named function — you're somewhere else and that's just the nearest export. Public symbols fix this.
8. **`-g -G` skips `-c` execution.** When using `-c "$<file"` to load a breakpoint script, drop `-g` so cdb stops at the initial breakpoint where `-c` runs.
9. **DNS for `virtualpin` is flaky.** Fall back to `192.168.1.31`.
10. **VBScript objects are STA-bound.** Calling them from a worker thread crashes with access violation. Don't dispatch FireDispID off-thread.
11. **First-chance exceptions are silent without a debugger.** A crash/divide-by-zero/AV that CLR's SEH catches and retries leaves no log entry, no dump file — just a "hang". This is why the default launch is **always under cdb with `sxe` filters armed**. Without that, you'll spend hours guessing.
12. **`bu Foo "..."` quoting.** Inside the cdb command string, semicolons separate commands and the outer string can't contain raw double-quotes that aren't paired. Easiest path: put the bp script in a file and load via `$<file`. Saves three layers of shell-escape pain (PowerShell → PAExec → cmd → cdb).

## Verification — when to declare a fix complete

A fix is real when:

1. Clean preflight + clean build + clean deploy + launch produces a healthy state
2. `probeNow.cmd` shows thread 0 in `Player::MultithreadedGameLoop` (the steady-state game loop)
3. `dumpbin /headers` on the deployed exe still shows expected values
4. **Gary plays through the original repro on the cabinet without recurrence**
5. (For behavior fixes) the watch breakpoint shows the corrected state at the previously-broken codepath

Don't declare done until #4 — Gary's hands-on confirmation. Stack analysis and code inspection are necessary but not sufficient.

## Adding to the institutional memory

When a fix ships, append a new entry to `references/known-bugs.md`:
- Symptom (what Gary saw)
- Diagnostic stack (the 3-5 frames or watch-points that pointed to it)
- Smoking gun (the cdb command / breakpoint that proved it)
- Root cause (1 paragraph)
- Fix (file:line, what changed)
- Verification command(s)
- Shipped commit SHA

This is the institutional memory. Future sessions read it first to recognize patterns we've already solved.
