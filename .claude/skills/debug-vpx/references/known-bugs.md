# Known bugs — the patterns to recognize

## 1. PE `SizeOfStackReserve = 0` → CLR threadpool init divide-by-zero

**Symptom**: Debug build hangs forever when loading any VPinMAME-backed table that uses a .NET COM server (B2S.Server, FlexDMD, DmdDevice). Release works fine. Process consumes ~480 MB and stays steady — no progress, no crash.

**Diagnostic stack (Debug, hung)**:
- Thread 0 (main): blocked in `VPinMAME64+0x1cf9f9 → KERNELBASE!WaitForMultipleObjects(INFINITE)`, called from `Player::Player → FireVoidEvent(Init) → vbscript → CLR Dispatch_Invoke → managed code → VPinMAME COM method`
- VPinMAME-spawned worker thread: stuck in `clr!EESleepEx ← clr!ThreadpoolMgr::EnsureInitialized` (or with public symbols, the function address near `clr!GetDefaultMaxLimitWorkerThreads+0x51`)

**The smoking gun**: launch under cdb with `sxe c0000094` (break on integer divide-by-zero). Debug fires the exception immediately at:
```
clr!GetDefaultMaxLimitWorkerThreads+0x51:
  div  rax, qword ptr [rsp+20h]    ; [rsp+20h] = stackReserveSize from PE = 0
```
Release does NOT fire it. **The exact difference between Debug and Release.**

**Root cause**: `make/vpx-core.vcxitems` had `<StackReserveSize>0</StackReserveSize>` in all three Debug configs. Linker writes the literal 0 into PE header `SizeOfStackReserve`. CLR reads that value via `Thread::GetProcessDefaultStackSize`, computes `MaxThreads = (TotalPhysMem/2) / 0` → divide-by-zero → SEH catches → retry forever.

**Fix**: set Debug configs to `<StackReserveSize>1048576</StackReserveSize>` and `<StackCommitSize>4096</StackCommitSize>` (matching Release defaults). Verify with `dumpbin /headers`: should show `100000` and `1000`.

**Verification command**:
```powershell
& 'C:\...\dumpbin.exe' /headers '.build\bin\vpx\Debug_BGFX-x64\VPinballX_BGFX64.exe' | Select-String 'stack reserve|stack commit'
```

Shipped fix: commit `14237196d` on master.

---

## 2. ClearDraws UAF on table close

**Symptom**: Closing a table (not exiting the app) fires "Pure Virtual Function Call" + hex stack dump; crashes after 3 iterations.

**Diagnostic**: invasive cdb attach to running VPX, set conditional bp inside `BallHistory::UnInit` at `ballhistory.cpp:930` (the `ClearDraws(player)` call). Step into. Watch `drawnBall.second->m_dwRef` — if it shows `0xDDDDDDDD`, the object was already freed (MSVC Debug CRT dead-land fill).

**Root cause**: `ClearDraws` was calling `Release()` more than once per draw-slot object (the same object lived in multiple slots and got released N times instead of 1).

**Fix**: track release-once via a set; only `Release()` when removing the slot's primary owner. Shipped on integration as commit `7cd18ad36`.

---

## 3. PsExec → black-window UI

**Symptom**: PsExec-launched VPX shows editor + File Open dialog with title bar and borders, but ALL-BLACK client area.

**Cause**: PsExec's session-1 token-acquisition path produces a token whose logon SID lacks full DACL access on `winsta0\default`. DirectComposition's swap-chain init fails silently with that DACL state. UI windows render their non-client chrome but the client area is broken.

**Fix**: use PAExec (drop-in replacement, same CLI). PAExec uses a different token-acquisition path that produces a token with the right DACL.

**Don't waste time** retrying PsExec flag variants (`-h`, `-i N`, schtasks, `start ""` shim). All fail with the same symptom.

---

## 4. Misleading symbol offsets without public CLR PDBs

**Symptom**: stack frames like `clr!ThreadpoolMgr::EnsureInitialized+0x239160` (huge offset). The offset is so large it can't be inside that function — `EnsureInitialized` is at most a few hundred bytes.

**Cause**: cdb falls back to the nearest **exported** symbol when it can't find the real function. The real code is somewhere else entirely; the displayed name is misleading.

**Fix**: pull public symbols from Microsoft's symbol server with `_NT_SYMBOL_PATH=SRV*C:\symcache*https://msdl.microsoft.com/download/symbols`. After symbols load, the same address resolves to its actual function (often something like `clr!GetDefaultMaxLimitWorkerThreads+0x51`).

**Cost**: ~2 min on first fetch. Cached locally afterward. Always do this before trusting any large offset.

---

## 5. VBScript Invoke from worker thread → access violation

**Symptom**: dispatch FireDispID on a worker thread (e.g. to keep main STA pumping during a long event handler). VBScript crashes with `vbscript+0x1f46a` access violation reading `[rax+38h]` where `rax=0`.

**Cause**: VBScript's IDispatch is strictly STA-bound. Calling it from a worker thread without proper COM marshalling violates threading rules and corrupts engine state.

**Fix**: don't dispatch FireDispID off-thread. If you need main STA to be responsive during a long call, the right pattern is `CoMarshalInterThreadInterfaceInStream` + worker calls the marshalled proxy. But VBScript may reject the marshal — it's not designed for this.

Lesson: don't try to "fix" STA-bound objects with worker threads. Find the cause of the original block instead.

---

## How to add a new entry here

When you ship a fix, append:
1. Symptom (1-2 sentences, what the user sees)
2. Diagnostic stack (the 3-5 frames that pointed to it)
3. Smoking gun (the cdb command / breakpoint that proved it)
4. Root cause (1 paragraph)
5. Fix (file:line, what changed)
6. Verification command(s)
7. Shipped commit SHA

This file is the institutional memory. Read it BEFORE diving into a new investigation — many "new" bugs turn out to be patterns we've already shipped fixes for.
