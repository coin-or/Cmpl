@echo off

if exist "%CmplProgPath%"bin\cmpl.exe goto RUN

echo Please install Cmpl into folder : %progPath%\bin
pause
goto END
 

:RUN
set oldWd=%CD%
cd  "%CmplProgPath%"

set oldPath=%PATH%
set PATH=%CmplProgPath%;%CmplProgPath%pyCmpl\scripts\Windows\;%PATH%

set PythonBin="%CmplProgPath%"pypy\pypy.exe
set serverHandler="%CmplProgPath%cmplServer\cmplServer\cmplServerHandler.py"

set PYTHONPATH=%PYTHONPATH%;%CmplProgPath%pyCmpl\lib;%CmplProgPath%cmplServer
set PYTHONSTARTUP=%CmplProgPath%pyCmpl\lib\pyCmplShell.py
set CMPLBINARY=%progPath%bin\cmpl.exe


if not [%2] == [] (
	set cmplPort=%2
) else (

	set cmplPort="8008"

	for /f "eol=# tokens=1-3" %%a in ('findstr "cmplServerPort =" "cmplServer\cmplServer.opt"') do (
	if "%%a"=="cmplServerPort" (
		set cmplPort=%%c
	)
)

)

set cmplServerCommand=""
if not [%1] == [] set cmplServerCommand=%1


if not "%cmplServerCommand%" == "-start" if not "%cmplServerCommand%" == "-startInGrid" if not "%cmplServerCommand%" == "-startScheduler" if not "%cmplServerCommand%" == "-stop" if not "%cmplServerCommand%" == "-status" goto usage 

if "%cmplServerCommand%" == "-start" echo "Starting CmplServer  ... Please stop it with cmplServer -stop  %cmplPort% in another cmplShell terminal"
if "%cmplServerCommand%" == "-startInGrid" echo "Starting CmplServer in CmplGrid ... Please stop it with cmplServer -stop  %cmplPort% in another cmplShell terminal"
if "%cmplServerCommand%" == "-startScheduler" echo "Starting CmplGridScheduler ... Please stop it with cmplServer -stop  %cmplPort% in another cmplShell terminal"
	
%PythonBin% %serverHandler%  %cmplServerCommand%  %cmplPort% 
cd %oldWd%
goto end

:usage
echo "Usage: cmplServer <command>  [<port>] [-showLog] "
echo "command:"
echo "   -start           - starts as single CMPLServer "
echo "   -startInGrid     - starts CMPLServer and connects to CMPLGrid"	
echo "   -startScheduler  - starts as CMPLGridScheduler"
echo "   -stop            - stops CMPLServer or CMPLGridScheduler "
echo "   -status          - returns the status of the CMPLServer or CMPLGridScheduler "
echo "port                - defines CMPLServer's or CMPLGridScheduler's port (default in cmplServer.opt: %cmplPort%)"


:end

set PATH=%oldPath%

@echo on