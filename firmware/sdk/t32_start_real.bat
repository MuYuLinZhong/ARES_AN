@echo off
:: If this batch file is not working:
:: - make sure you installed T32 software
::
:: Command line syntax:
:: start_t32_real.bat [optional: <BUILD_DIR> <ROM_ELF_FILE> <NVM_ELF_FILE>]
:: 
:: Example:
:: start_t32_real.bat smack_rom\build image\image_rom.elf

:: Detect bit-ness of OS
reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | findstr /i "x86" > NUL && set OS=32BIT || set OS=64BIT

if %OS%==32BIT set T32_DIR=c:\T32\bin\windows
if %OS%==64BIT set T32_DIR=c:\T32\bin\windows64

echo %OS% OS detected. Expecting T32 being installed in %T32_DIR%

set SMACK_T32_TOOL_CONFIG=%CD%\tool_config\t32\real
set SMACK_T32_PERFILES_PATH=%CD%\tools\intern\Lauterbach
set SMACK_BUILD_DIR=%CD%\smack_rom\build
set SMACK_NVM_BUILD_DIR=%CD%\smack_sl\build

if not "%1"=="" set SMACK_BUILD_DIR=%CD%\%1%

:: concatenate all per files
echo Concatenating peripheral files for Lauterbach...
::del /q %SMACK_BUILD_DIR%\smack.per
echo. > %SMACK_BUILD_DIR%\smack.per
for /r "%SMACK_T32_PERFILES_PATH%" %%F in (*.per) do type "%%F" >>%SMACK_BUILD_DIR%\smack.per
echo done.

:: create T32 config file
echo OS= >%SMACK_BUILD_DIR%\config.t32
echo ID=T32 >>%SMACK_BUILD_DIR%\config.t32
:: new line is mandatory - otherwise it will not work!
echo.  >>%SMACK_BUILD_DIR%\config.t32
echo PBI=>>%SMACK_BUILD_DIR%\config.t32
echo USB>>%SMACK_BUILD_DIR%\config.t32
:: new line is mandatory - otherwise it will not work!
echo.  >>%SMACK_BUILD_DIR%\config.t32
::echo PRINTER=WINDOWS>>%SMACK_BUILD_DIR%\config.t32
::echo.  >>%SMACK_BUILD_DIR%\config.t32
::echo.  >>%SMACK_BUILD_DIR%\config.t32
:: new line is mandatory - otherwise it will not work!
echo.  >>%SMACK_BUILD_DIR%\config.t32
echo RCL=NETASSIST>>%SMACK_BUILD_DIR%\config.t32
echo PACKLEN=1024>>%SMACK_BUILD_DIR%\config.t32
echo PORT=20000>>%SMACK_BUILD_DIR%\config.t32
:: new line is mandatory - otherwise it will not work!
echo.  >>%SMACK_BUILD_DIR%\config.t32

set CONFIG_FILE=%SMACK_BUILD_DIR%\config.t32
set STARTUP_SCRIPT=%SMACK_T32_TOOL_CONFIG%\startup.s
set USER_STARTUP_SCRIPT=%CD%\t32_user_startup.s
set PAR_ROM_ELF_FILE=%SMACK_BUILD_DIR%\image\image_rom.elf
set PAR_DPARAM_ELF_FILE=%SMACK_BUILD_DIR%\dparams\default_dparam.elf
set PAR_NVM_ELF_FILE=%SMACK_NVM_BUILD_DIR%\image\image_nvm.elf
set PAR_WINDOW_SCRIPT=%SMACK_T32_TOOL_CONFIG%\windows.cmm
set PAR_WINDOW_MENU_SCRIPT=%SMACK_T32_TOOL_CONFIG%\windows_menu.cmm
set PER_FILE=%SMACK_BUILD_DIR%\smack.per
set NVM_BINARY_FILE=%SMACK_T32_TOOL_CONFIG%\image_ram.bin

if not "%2"=="" set PAR_ROM_ELF_FILE=%SMACK_BUILD_DIR%\%2%
if not "%3"=="" set PAR_NVM_ELF_FILE=%SMACK_BUILD_DIR%\%3%

:: change drive and path to where T32 is installed
%T32_DIR:~0,2%
cd %T32_DIR%

:: start T32
start t32marm.exe -c %CONFIG_FILE% -s %STARTUP_SCRIPT% %PAR_ROM_ELF_FILE% %PAR_NVM_ELF_FILE% %PAR_WINDOW_SCRIPT% %PAR_WINDOW_MENU_SCRIPT% %USER_STARTUP_SCRIPT% %PER_FILE% %NVM_BINARY_FILE% 0 %PAR_DPARAM_ELF_FILE%
