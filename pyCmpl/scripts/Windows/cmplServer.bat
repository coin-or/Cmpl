@echo off

set PATH=%PROGRAMFILES%\Cmpl\bin;%PROGRAMFILES%\Cmpl\Frameworks;%PATH%


set PythonBin="%PROGRAMFILES%"\Cmpl\Thirdparty\pypy\pypy.exe

set PYTHONPATH=%PROGRAMFILES%\Cmpl\pyCmpl
set PYTHONSTARTUP=%PROGRAMFILES%\Cmpl\pyCmpl\pyCmpl\pyCmplShell.py
set CMPLBINARY=%PROGRAMFILES%\Cmpl\bin\cmpl.exe

set cmplPort="8008"
if not [%2] == [] set cmplPort=%2

set cmplServerCommand="nonsens"
if not [%1] == [] set cmplServerCommand=%1


if not "%cmplServerCommand%" == "-start" if not "%cmplServerCommand%" == "-stop" if not "%cmplServerCommand%" == "-isRunning" goto usage 

if "%cmplServerCommand%" == "-start" echo "CmplServer has started .... "  

%PythonBin% "%PYTHONPATH%"\pyCmpl\cmplServerHandler.py  %cmplServerCommand%  %cmplPort% 
goto end


:usage
echo "Usage: cmplServer < start | stop >  [<port>]"  
echo "-start       - start cmplServer"
echo "-stop        - stop cmplServer "
echo "-isRunning   - tests whether cmplServer is running or not"
echo "port         - define cmplServer's port (default 8008)"

:end



rem @echo on