@echo off

echo Starting Coliop and setting environment

set progPath=%programfiles%
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" set progPath=%programfiles(x86)%

if exist "%progPath%\Cmpl\bin\cmpl.exe" goto RUN

echo Please install Cmpl into folder : %progPath%\Cmpl
pause
goto END
 
:RUN


set oldPath=%PATH%
set PATH=%progPath%\Cmpl;%PATH%


cd %progPath%\Cmpl\Coliop3

IF EXIST %windir%\system32\java.exe (
	java -jar coliop.jar
) ELSE IF EXIST %windir%\SysWOW64\java.exe (
	%windir%\SysWOW64\java.exe -jar coliop.jar 
) ELSE (
	echo "Sorry, Can't find java!"
	pause
)

:END
set PATH=%oldPath%
rem exit

@echo on
