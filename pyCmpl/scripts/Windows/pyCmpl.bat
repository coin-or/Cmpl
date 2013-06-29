@echo off

set PythonBin="%PROGRAMFILES%"\Cmpl\Thirdparty\pypy\pypy.exe

set PYTHONPATH=%PROGRAMFILES%\Cmpl\pyCmpl
set PYTHONSTARTUP=%PROGRAMFILES%\Cmpl\pyCmpl\pyCmpl\pyCmplShell.py
set CMPLBINARY=%PROGRAMFILES%\Cmpl\bin\cmpl.exe

%PythonBin% %1


