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


if not [%2] == [] (
	set cmplPort=%2
) else (

	set cmplPort="8008"
	
	set filename=%progPath%\Cmpl\cmplServer\cmplServer.opt
	
	for /f "eol=# tokens=1-3" %%a in ('findstr "cmplServerPort =" "%filename%"') do (
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

	
%PythonBin% "%progPath%"\Cmpl\cmplServer\cmplServer\cmplServerHandler.py  %cmplServerCommand%  %cmplPort% 
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