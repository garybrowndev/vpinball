@echo off
REM Launch debug VPX under cdb with first-chance crash filters.
REM Catches AVs, integer divide-by-zero, stack overflows.
REM Symbol path includes Microsoft public symbols (CRITICAL for CLR/Win32 frames).
cd /d "C:\Visual Pinball\debug"
"C:\tools\Debuggers\x64\cdb.exe" -g -G -lines -y "SRV*C:\symcache*https://msdl.microsoft.com/download/symbols;C:\Visual Pinball\debug" -logo C:\Dumps\vpx-under-cdb.log -c "sxe av; sxe c0000005; sxe sov; sxe c0000094; g" "C:\Visual Pinball\debug\VPinballX_BGFX64.exe" -Play "D:\OneDrive\machine backups\virtualpin\Emulation\Visual Pinball\tables\Black Pyramid (Bally 1984) v600.vpx"
