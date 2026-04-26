# Cabinet one-time setup

You only need this if rebuilding the VPM cabinet from a clean Windows install. Normal debug cycles don't touch any of this.

## 1. Tools to install

### On dev machine

- **Debugging Tools for Windows** (cdb.exe, windbg.exe). Comes with the Windows 10/11 SDK. During SDK setup, select ONLY "Debugging Tools for Windows" — uncheck the rest unless you need them. Lands at `C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\`.
- **PAExec**. Direct download: `https://www.poweradmin.com/paexec/paexec.exe`. Drop into `C:\tools\sysinternals\paexec.exe`. (winget has no PAExec manifest — direct download is the path.)
- **PsExec** (optional fallback for non-GUI commands): `winget install Microsoft.Sysinternals.PsTools` lands it in `%LOCALAPPDATA%\Microsoft\WinGet\Links\`.

### On cabinet

- Copy cdb + DLLs from dev's Windows Kits Debuggers folder to `C:\tools\Debuggers\x64\` on the cabinet (UNC copy works fine). Specifically: `cdb.exe`, `dbgeng.dll`, `dbghelp.dll`, `dbgcore.dll`, `DbgModel.dll`, `dbgsrv.exe`, `srcsrv.dll`, `symsrv.dll`, `symsrv.yes`, all `api-ms-win-*.dll` in that folder. Easier: just robocopy the whole folder.

## 2. SMB drive mapping

On the dev machine, map the cabinet's `C$` admin share to `Z:\`. Persistent mapping — doesn't survive reboots otherwise:

```cmd
net use Z: \\virtualpin\c$ /persistent:yes
```

(Or via `Map Network Drive` in Explorer with "Reconnect at sign-in" checked.)

After mapping, all file ops from dev should use `Z:\...` paths instead of UNC `\\virtualpin\c$\...`. Faster and avoids quoting/permission quirks.

## 3. Folder structure on cabinet

Create on the cabinet (do this from dev via Z:\):

```
C:\Dumps\                       (Z:\Dumps\)         WER local dumps + scratch logs
C:\symcache\                                        cdb symbol cache
C:\tools\Debuggers\x64\         (Z:\tools\...)      cdb + DLLs (see above)
C:\Visual Pinball\debug\        (Z:\Visual Pinball\debug\)  Debug exe + PDB + Debug CRT DLLs
```

The `C:\Visual Pinball\` parent already exists from a normal VPX install; just add the `debug\` subfolder.

## 4. Windows Error Reporting LocalDumps

Configure WER to write full crash dumps to `C:\Dumps\` for the VPX exes. Push from dev as a `.cmd` and execute via PAExec (registry write requires admin):

```cmd
@echo off
reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpFolder /t REG_EXPAND_SZ /d "C:\Dumps" /f
reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpCount  /t REG_DWORD /d 20 /f
reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /v DumpType   /t REG_DWORD /d 2  /f
for %%E in (VPinballX_BGFX64.exe VPinballX_GL64.exe VPinballX64.exe) do (
  reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%%E" /v DumpFolder /t REG_EXPAND_SZ /d "C:\Dumps" /f
  reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%%E" /v DumpCount  /t REG_DWORD /d 20 /f
  reg add "HKLM\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%%E" /v DumpType   /t REG_DWORD /d 2  /f
)
```

`DumpType=2` = full memory dump (large but complete; lets you walk the heap and see strings). Use `DumpType=1` for mini-dumps if disk space is tight, but you lose a lot.

After this, any unhandled crash in those exes lands a `.dmp` in `C:\Dumps\` automatically.

## 5. Network reachability

```powershell
Test-NetConnection -ComputerName virtualpin -CommonTCPPort SMB
```

Should succeed. If DNS for `virtualpin` fails (sometimes happens), fall back to the IP `192.168.1.31`. SMB still resolves the hostname when DNS doesn't, but tools like `Test-NetConnection` use DNS first.

## 6. Verifying

From dev:
```powershell
Test-Path 'Z:\tools\Debuggers\x64\cdb.exe'              # True
Test-Path 'Z:\Visual Pinball\debug'                     # True
Test-Path 'Z:\Dumps'                                    # True
Test-Path 'C:\tools\sysinternals\paexec.exe'            # True
```

Trial run:
```powershell
& 'C:\tools\sysinternals\paexec.exe' \\virtualpin -d cmd /c 'echo HELLO > C:\Dumps\hello.log'
Start-Sleep 3
Get-Content 'Z:\Dumps\hello.log'   # should print "HELLO "
```

If that round-trips, the cabinet is provisioned and ready for debug cycles.
