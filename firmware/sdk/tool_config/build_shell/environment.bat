@echo off 

:: set the path to all tools which are supposed to be used in the command shell
@SET PATH=%cd%\tools\extern\make\bin;%PATH%
@SET PATH=%cd%\tools\extern\git_bin;%PATH%
rem @SET PATH=%PATH%;%cd%\tools\extern\python2.7.9.10
rem @SET PATH=%PATH%;%cd%\tools\extern\gcc\bin
@SET PATH=%cd%\tools\extern\gcc-arm-none-eabi\bin;%PATH%
rem @SET PATH=%PATH%;%cd%\tools\extern\pclint8.00
rem @SET PATH=%PATH%;C:\sofit\test_doxy2prisma\ant\bin
rem @SET PATH=%PATH%;C:\Windows\system32
