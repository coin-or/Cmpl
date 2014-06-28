@echo off



set progPath=%programfiles%
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" set progPath=%programfiles(x86)%

if exist "%progPath%\Cmpl\bin\cmpl.exe" goto RUN

echo Please install Cmpl into folder : %progPath%\Cmpl
pause
goto END
 
:RUN

set oldPath=%PATH%
set PATH=%progPath%\Cmpl\bin;%progPath%\Cmpl\Frameworks;%progPath%\Cmpl\pyCmpl\scripts\Windows\;%PATH%



set PythonBin="%progPath%"\Cmpl\Thirdparty\pypy\pypy.exe

set PYTHONPATH=%PYTHONPATH%;%progPath%\Cmpl\pyCmpl\lib;%progPath%\Cmpl\cmplServer
set PYTHONSTARTUP=%progPath%\Cmpl\pyCmpl\lib\pyCmplShell.py
set CMPLBINARY=%progPath%\Cmpl\bin\cmpl.exe

%PythonBin% "%1" "%2" "%3" "%4" "%5" "%6" "%7" "%8" "%9" "%10"


set PATH=%oldPath%

@echo on