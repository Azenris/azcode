@echo OFF
cls
SET mypath=%~dp0
SET testFile=input.txt
pushd %mypath%\final\debug\
azcode.exe %~dp0%testFile%
popd