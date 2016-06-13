@echo off

set oldPath=%PATH%

set CmplProgPath=%~dp0
if exist "%CmplProgPath%"bin\cmpl.exe goto RUN

echo Please install Cmpl into folder : %progPath%\bin
pause
goto END
 
:RUN

set oldPath=%PATH%
set PATH=%CmplProgPath%;%CmplProgPath%pyCmpl\scripts\Windows\;%PATH%

echo Exit with the command exit
cd %HOMEPATH%\Documents


echo ""
echo "###########################################################################"
echo "# Welcome to CmplShell                                                    #" 
echo "# Run cmpl (pyCmpl) to solve a Cmpl (pyCmpl) problem.                     #" 
echo "# To start or stop CmplServer please use cmplServer <-start|-stop> [port] #"
echo "# Type exit to leave the Cmpl environment                                 #"
echo "###########################################################################"
echo ""

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