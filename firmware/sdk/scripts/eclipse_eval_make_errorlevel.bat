@ECHO OFF
rem This script embeds the make process call within the Eclipse GUI.
rem Eclipse does to evaluate the process returned error code when
rem calling 'make.exe'.
rem Therefore we configure to call this script (instead of make
rem directly), which prints out the error code result
rem (success/failure) of the make call.

%~dp0\..\tools\extern\make\bin\make.exe %*
IF ERRORLEVEL 1 GOTO MAKE_ERROR
ECHO make return: SUCCESS
EXIT /B
:MAKE_ERROR
ECHO make return: FAILURE
EXIT /B
