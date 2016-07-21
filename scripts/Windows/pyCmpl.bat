@echo off

if exist "%CmplProgPath%bin\cmpl.exe" goto RUN

echo Please install Cmpl into folder : %CmplProgPath%\bin
pause
goto END
 
:RUN

set oldPath=%PATH%
set PATH=%CmplProgPath%;%CmplProgPath%pyCmpl\scripts\Windows\;%PATH%

set PythonBin="%CmplProgPath%"pypy\pypy.exe

set PYTHONPATH=%PYTHONPATH%;%CmplProgPath%pyCmpl\lib;%CmplProgPath%cmplServer
set PYTHONSTARTUP=%CmplProgPath%pyCmpl\lib\pyCmplShell.py
set CMPLBINARY=%CmplProgPath%bin\cmpl.exe

%PythonBin% "%1" "%2" "%3" "%4" "%5" "%6" "%7" "%8" "%9" "%10"


set PATH=%oldPath%

@echo on