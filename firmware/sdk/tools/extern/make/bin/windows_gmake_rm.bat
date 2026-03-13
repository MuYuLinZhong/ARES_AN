@echo off

Setlocal EnableDelayedExpansion

:: Replace forward slashes with backslashes
set param=%1
set file=!param:/=\!

echo !file!

:: Remove directory including subdirectories and do it quietly (no confirmation)
if exist !file! (
    echo !file!
    del /F /Q !file!
)