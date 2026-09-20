@echo off
REM Launch debug VPX under cdb with first-chance crash filters.
REM Catches AVs, integer divide-by-zero, stack overflows.
REM Symbol path includes Microsoft public symbols (CRITICAL for CLR/Win32 frames).
REM NOTE: no -g here, on purpose. -g skips the initial breakpoint, which is the only place
REM -c runs -- with it, the sxe filters below are NEVER armed and you silently get a plain
REM attach instead of first-chance capture. Verified 2026-09-20: the log had no "0:000> sxe"
REM echo. The trailing "g" inside -c is what resumes execution. See gotcha #8 in SKILL.md.
cd /d "C:\Visual Pinball\debug"
"C:\tools\Debuggers\x64\cdb.exe" -G -lines -y "SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;C:\Visual Pinball\debug" -logo C:\Dumps\vpx-under-cdb.log -c "sxe av; sxe c0000005; sxe sov; sxe c0000094; g" "C:\Visual Pinball\debug\VPinballX_BGFX64.exe" -Play "D:\OneDrive\machine backups\virtualpin\Emulation\Visual Pinball\tables\Black Pyramid (Bally 1984) v600.vpx"
