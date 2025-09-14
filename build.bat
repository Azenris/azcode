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
set binDir=bin
set objDir=obj
set linker=
set links=ws2_32.lib
set flags=-std:c++latest -Zc:preprocessor -Zc:strictStrings -GR-
set warnings=-WX -W4 -wd4189 -wd4201 -wd4324 -wd4505
set includes=-Isrc/
set defines=-D_CRT_SECURE_NO_WARNINGS -D_HAS_EXCEPTIONS=0 -DOS_NAME=\"Windows\"

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
	set binDir=%binDir%/debug
	set defines=%defines% -DDEBUG -D_DEBUG -D_MT -DBUILD_TYPE=\"DEBUG\"
	set flags=%flags% -Z7 -FC -MTd
	if %asanEnabled% == 1 (
		set flags=!flags! -fsanitize=address -Zi -Fd!binDir!/asan_game.pdb
	)
) else (
	set linker=%linker% -SUBSYSTEM:WINDOWS
	set binDir=%binDir%/release
	set defines=%defines% -DNDEBUG -DBUILD_TYPE=\"RELEASE\"
	set flags=%flags% -MT -O2 -Ot -GF
)

if not exist %binDir% ( mkdir "%binDir%" )
if not exist %objDir% ( mkdir "%objDir%" )
del %binDir%/*.pdb > nul 2> nul

:: ----------------------------------------------------
:: UNITY/JUMBO BUILD
:: ----------------------------------------------------

cl -nologo %flags% %warnings% %defines% -Fe%binDir%/%name%.exe -Fo%objDir%/ src/main.cpp %includes% -link %linker% %links% -INCREMENTAL:NO

:build_success
echo Build Success
goto build_end

:build_failed
echo Build Failed
goto build_end

:build_end
endlocal