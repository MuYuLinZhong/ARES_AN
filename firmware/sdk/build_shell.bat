@echo off
:: create the actual environment
@CALL .\tool_config\build_shell\environment.bat

:: start the command shell, set the CMD window title to tell the user that this shell is specially 
:: made for building the Firmware.
start "Smack Firmware Build Shell" 
