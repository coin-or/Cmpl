@echo off

set oldPath=%PATH%
set progPath=%programfiles%
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" set progPath=%programfiles(x86)%

if exist "%progPath%\Cmpl\bin\cmpl.exe" goto RUN

echo Please install Cmpl into folder : %progPath%\Cmpl
pause
goto END
 
:RUN

set oldPath=%PATH%
set PATH=%progPath%\Cmpl\;%progPath%\Cmpl\Frameworks;%progPath%\Cmpl\pyCmpl\scripts\Windows\;%PATH%
echo Exit with the command exit
cd %HOMEPATH%\Documents

:START
	set /p cmplCommand="Cmpl: ":
	set cmplCmd=%cmplCommand: =#%
	if "%cmplCmd%"=="exit" (
		goto END
	)
	call %cmplCommand%
	@echo off
	goto START

:END
set PATH=%oldPath%

@echo on