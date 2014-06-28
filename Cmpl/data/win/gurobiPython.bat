@ECHO OFF

if "%GUROBI_HOME%"=="" (
  echo.
  echo Gurobi installer changes have not taken effect yet.
  echo Please restart your machine before continuing.
  echo.
  set /p JUNK= [Hit ENTER to exit]
  exit
)

set PYTHONSTARTUP=%GUROBI_HOME%\lib\gurobi.py

cd \Program Files\Cmpl\bin

set ARGS=%1
shift

:start
if not "%1" == "" (
  set ARGS=%ARGS% %1
  shift
  goto start
)

echo %ARGS%

"%GUROBI_HOME%\python27\bin\python" gurobiCmpl.py %ARGS%