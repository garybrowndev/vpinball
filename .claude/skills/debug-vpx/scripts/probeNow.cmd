@echo off
REM Non-invasive cdb attach to a running VPinballX_BGFX64.exe.
REM Dumps thread 0's full stack with public + private symbols, then detaches.
REM Output: C:\Dumps\probeNow.log on cabinet (Z:\Dumps\probeNow.log on dev).
"C:\tools\Debuggers\x64\cdb.exe" -pv -pn VPinballX_BGFX64.exe -y "SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;C:\Visual Pinball\debug" -lines -c ".echo === T0 ===; ~0 kbn 30; .echo === DONE; qd" > C:\Dumps\probeNow.log 2>&1
echo done
