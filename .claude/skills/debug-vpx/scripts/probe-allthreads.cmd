@echo off
REM Non-invasive attach + every-thread stack dump.
REM Use when narrowing down which thread is faulting/hung.
"C:\tools\Debuggers\x64\cdb.exe" -pv -pn VPinballX_BGFX64.exe -y "SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;C:\Visual Pinball\debug" -lines -c ".echo === ALL_THREADS ===; ~* kbn 30; .echo === LOCKS ===; !locks; .echo === MODS_RELEVANT ===; lm m clr*; lm m mscorlib*; lm m VPinMAME*; lm m B2S*; lm m DmdDevice*; .echo === DONE; qd" > C:\Dumps\probe-allthreads.log 2>&1
echo done
