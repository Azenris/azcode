@echo OFF
cls

:: Run vcvars64 to setup enviroment
if not defined VSCMD_ARG_TGT_ARCH (
	call "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat"
)

:: done after the enviroment setup so the VSCMD_ARG_TGT_ARCH variable is set globally
setlocal EnableDelayedExpansion

:: ----------------------------------------------------
:: OPTIONS
:: ----------------------------------------------------
set debugMode=1
set asanEnabled=0
set name=azcode
set outputDir=final
set buildDir=build
set linker=
set flags=-std:c++20 -Zc:preprocessor -Zc:strictStrings -GR-
set warnings=-WX -W4 -wd4189 -wd4201 -wd4324 -wd4505
set includes=-Isrc/
set defines=-D_CRT_SECURE_NO_WARNINGS -D_HAS_EXCEPTIONS=0

if not "%1" == "" (
	if "%1" == "DEBUG" (
		set debugMode=1
	)
	if "%1" == "RELEASE" (
		set debugMode=0
	)
)

:: ----------------------------------------------------
:: 
:: ----------------------------------------------------
if %debugMode% == 1 (
	set linker=%linker% -SUBSYSTEM:CONSOLE
	set outputDir=%outputDir%/debug
	set defines=%defines% -DDEBUG -D_DEBUG -D_MT -DBUILD_TYPE=\"DEBUG\"
	set flags=%flags% -Z7 -FC -MTd
	if %asanEnabled% == 1 (
		set flags=!flags! -fsanitize=address -Zi -Fd!outputDir!/asan_game.pdb
	)
) else (
	set linker=%linker% -SUBSYSTEM:WINDOWS
	set outputDir=%outputDir%/release
	set defines=%defines% -DNDEBUG -DBUILD_TYPE=\"RELEASE\"
	set flags=%flags% -MT -O2 -Ot -GF
)

if not exist %outputDir% ( mkdir "%outputDir%" )
if not exist %buildDir% ( mkdir "%buildDir%" )
del %outputDir%/*.pdb > nul 2> nul

:: ----------------------------------------------------
:: 
:: ----------------------------------------------------

cl -nologo %flags% %warnings% %defines% -Fe%outputDir%/%name%.exe -Fo%buildDir%/ src/main.cpp %includes% -INCREMENTAL:NO

:build_success
echo Build Success
goto build_end

:build_failed
echo Build Failed
goto build_end

:build_end
endlocal