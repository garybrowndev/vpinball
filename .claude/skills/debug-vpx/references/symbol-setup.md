# Symbol setup

## Why it matters

CLR, OS, and Win32 frames will appear in basically every VPX debug stack — VPinMAME's COM dispatch goes through `clr.dll`, `mscorlib`, `combase`, `oleaut32`, etc. Without public symbols for those modules, you get either no symbol resolution or **misleading symbols** (nearest-exported-name with huge offsets, see `cdb-script-syntax.md`).

A stack like:
```
clr!ThreadpoolMgr::EnsureInitialized+0x239160
```
is **not** in `EnsureInitialized` — that offset is far outside any reasonable function size. cdb fell back to the nearest export. The real function is somewhere else. Public symbols fix this immediately.

## The path

```
SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;<private_pdb_dir>
```

Components:
- `SRV*` — symbol-server directive
- `C:\symcache` — local cache (downloaded PDBs land here, ~50-200 MB after warm-up)
- `https://msdl.microsoft.com/download/symbols` — Microsoft's public symbol server
- `<private_pdb_dir>` — e.g. `C:\Visual Pinball\debug` (where VPinballX_BGFX64.pdb lives)

Set per-session, not globally:

**On the cabinet, in a `.cmd` script for cdb**:
```cmd
"C:\tools\Debuggers\x64\cdb.exe" -y "SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;C:\Visual Pinball\debug" ...
```

**On the dev machine** (if running cdb-remote):
```powershell
$env:_NT_SYMBOL_PATH = "SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;C:\code\Pinball\vpinball_ballhistory\development\build\Debug"
```

DO NOT set `_NT_SYMBOL_PATH` system-wide — leaks into unrelated processes and causes them to do unexpected network fetches.

## First-fetch cost

~2 minutes on first attach to populate `C:\symcache\`. Subsequent attaches resolve instantly. The cache survives reboots.

If you're impatient, you can pre-warm by running:
```cmd
"C:\tools\Debuggers\x64\symchk.exe" /r C:\Windows\Microsoft.NET\Framework64\v4.0.30319 /s "SRV*C:\symcache*https://msdl.microsoft.com/download/symbols"
```
This walks every DLL in the .NET 4.0 install and fetches matching PDBs. Takes a while but front-loads the cost.

## Verifying symbols are actually loaded

In cdb:
```
lm vm clr
```
Should show `pdb symbols (private)` or `pdb symbols (public)` for clr.dll. If you see `(deferred)`, run `.reload /f clr` to force-load. If you see `(no symbols)`, the symbol path is broken or the network fetch failed.

To force a re-fetch (clear cache for one module):
```
.reload /f clr.dll
```

## Common failure modes

**Network fetch silently fails behind a corporate proxy**: set `HTTP_PROXY` env var or download manually.

**Cache directory not writable**: cdb falls back to no symbols silently. Make sure `C:\symcache` exists and is writable by the cdb-running account.

**Wrong PDB version for the loaded DLL**: cdb refuses to load mismatched PDBs. Symbol-server fetches the version-matched PDB automatically; the failure mode is usually "no PDB on server for this build" (rare for shipping CLR/OS DLLs, common for nightly builds).
