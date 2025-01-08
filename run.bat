@echo OFF
cls
SET mypath=%~dp0
SET testFile=input.aas
pushd %mypath%\final\debug\
azcode.exe %~dp0%testFile%
popd