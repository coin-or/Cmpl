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

set PYTHONPATH=%progPath%\Cmpl\pyCmpl
set PYTHONSTARTUP=%progPath%\Cmpl\pyCmpl\pyCmpl\pyCmplShell.py
set CMPLBINARY=%progPath%\Cmpl\bin\cmpl.exe

set cmplPort="8008"
if not [%2] == [] set cmplPort=%2

set cmplServerCommand=""
if not [%1] == [] set cmplServerCommand=%1


if not "%cmplServerCommand%" == "-start" if not "%cmplServerCommand%" == "-stop" if not "%cmplServerCommand%" == "-isRunning" goto usage 

if "%cmplServerCommand%" == "-start" echo "Starting CmplServer ... Please stop it with cmplServer -stop [<port>]"
	
%PythonBin% "%PYTHONPATH%"\pyCmpl\cmplServerHandler.py  %cmplServerCommand%  %cmplPort% 
goto end


:usage
echo "Usage: cmplServer < start | stop >  [<port>]" 
echo "-start       - start cmplServer"
echo "-stop        - stop cmplServer "
echo "-isRunning   - tests whether cmplServer is running or not"
echo "port         - define cmplServer's port (default 8008)"

:end

set PATH=%oldPath%

@echo on