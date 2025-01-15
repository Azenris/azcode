@echo OFF
cls

setlocal EnableDelayedExpansion

for /F %%a in ('echo prompt $E ^| cmd') do set "ESC=%%a"

SET mypath=%~dp0
pushd %mypath%\final\debug\

call:run_test arithmetic 16
call:run_test functions 16
call:run_test objects 21
call:run_test printing 0


popd
exit /b

:: ----------------------------------------------

:run_test
azcode.exe %mypath%example\%1.aas > NUL
if %ERRORLEVEL% == %2 (
	echo !ESC![7m[Success]!ESC![0m : %1
) else (
	echo !ESC![101;93m[ Failed]!ESC![0m : %1
)
exit /b