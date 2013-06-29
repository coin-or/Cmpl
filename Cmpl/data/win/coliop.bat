@echo off

echo Starting Coliop and setting environment

set PATH=%PROGRAMFILES%\Cmpl\bin;%PROGRAMFILES%\Cmpl\Frameworks;%PATH%

cd %PROGRAMFILES%\Cmpl\Coliop3

IF EXIST %windir%\system32\java.exe (
	java -jar coliop.jar
) ELSE IF EXIST %windir%\SysWOW64\java.exe (
	%windir%\SysWOW64\java.exe -jar coliop.jar 
) ELSE (
	echo "Sorry, Can't find java!"
	pause
)

exit

@echo on
