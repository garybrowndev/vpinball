@echo off
REM VPM preflight: kill anything that could pollute a fresh debug run.
REM Run on the VPM cabinet via PAExec before each launch.
taskkill /F /IM VPinballX_BGFX64.exe       >nul 2>&1
taskkill /F /IM VPinballX_BGFX64_orig.exe  >nul 2>&1
taskkill /F /IM VPinballX_BGFX64_control.exe >nul 2>&1
taskkill /F /IM VPinballX64.exe            >nul 2>&1
taskkill /F /IM VPinballX64_orig.exe       >nul 2>&1
taskkill /F /IM VPinballX_GL64.exe         >nul 2>&1
taskkill /F /IM VPinballX_GL64_orig.exe    >nul 2>&1
taskkill /F /IM cdb.exe                    >nul 2>&1
taskkill /F /IM windbg.exe                 >nul 2>&1
taskkill /F /IM B2SBackglassServer.exe     >nul 2>&1
taskkill /F /IM B2SBackglassServerEXE.exe  >nul 2>&1
taskkill /F /IM DmdDevice.exe              >nul 2>&1
taskkill /F /IM DmdDevice64.exe            >nul 2>&1
taskkill /F /IM dmdext.exe                 >nul 2>&1
taskkill /F /IM PinUpPlayer.exe            >nul 2>&1
taskkill /F /IM PinUpMenu.exe              >nul 2>&1
taskkill /F /IM PuPDOFLink.exe             >nul 2>&1
echo PREFLIGHT_DONE > C:\Dumps\preflight.log
tasklist | findstr /I "VPinball B2S Dmd PinUp" >> C:\Dumps\preflight.log 2>&1
