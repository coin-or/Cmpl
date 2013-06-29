@echo off

set PATH=%PROGRAMFILES%\Cmpl\bin;%PROGRAMFILES%\Cmpl\Frameworks;%PROGRAMFILES%\Cmpl\pyCmpl\scripts\Windows\;%PATH%

echo Exit with the command exit

cd %HOMEPATH%\Documents

:START

set /p cmplCommand="%CD% Cmpl:":

if "%cmplCommand%"=="exit" (
	goto END
)

call %cmplCommand%

goto START

:END

@echo on