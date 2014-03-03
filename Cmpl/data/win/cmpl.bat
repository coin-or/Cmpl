@echo off
SETLOCAL enabledelayedexpansion

set progPath=%programfiles%
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" set progPath=%programfiles(x86)%

if exist "%progPath%\Cmpl\bin\cmpl.exe" goto RUN

echo Please install Cmpl into folder : %progPath%\Cmpl
pause
goto END
 
:RUN

set oldPath=%PATH%
set PATH=%progPath%\Cmpl\bin;%progPath%\Cmpl\Frameworks;%progPath%\Cmpl\pyCmpl\scripts\Windows\;%PATH%

set idx=0
set isRemote=0
set cArgList=
set cmplFile=
set cmplUrl=
set nextCmplFile=0
set nextCmplUrl=0
set cmplFileFound=0
set cmplJava=0

FOR %%x IN (%*) DO (

	set /a idx=!idx!+1

	set mArg=%%x
	set mArg=!mArg: =#!
	
	if !nextCmplFile!==1 (
		set cmplFile=%%x
		set cmplFileFound=1
		set nextCmplFile=0
	)

	if !nextCmplUrl!==1 (
		if not !CmplJava!==1 ( 
			set cmplUrl=%%x
			set isRemote=1
		)
		set nextCmplUrl=0
	)
	
	if "!mArg!"=="-cmplUrl" set nextCmplUrl=1

	if not "!mArg~0,1!"=="-" (
		if "!idx!"=="1" (
			set cmplFile=%%x
			set cmplFileFound=1
			
		) else (
			if "!mArg!"=="-cmplJava" (
				set nextCmplFile=1 
				set cmplJava=1
				set cmplUrl=""

			) else (
				set cArgList=!cArgList! %%x
			)
		)
	) else ( 
		set cArgList=!cArgList! %%x 
	)

)

if !cmplJava!==1 goto RUN1

if not "!cmplUrl!"=="" goto RUN1

set cmplFile=!cmplFile: =#!

FOR /f %%i IN ("%cmplFile%") DO (
	set filepath=%%~dpi
	set filename=%%~nxi
)

set cmplFile=!cmplFile:#= !


if %cmplFileFound%==1 ( 
	set currPath="%CD%"
	set filePath=!filePath:#= !
	cd !filePath!
	for /f "eol=# tokens=2-3" %%a in ('findstr "%%args cmplUrl" "!filename!"') do (
		if "%%a"=="-cmplUrl" (
			set cmplUrl=%%b
			set isRemote=1	 
		)
	)
	cd !currPath!
)


:RUN1

if %isRemote%==1 (
	set PythonBin="%progPath%"\Cmpl\Thirdparty\pypy\pypy.exe

	set PYTHONPATH=!progPath!\Cmpl\pyCmpl
	set PYTHONSTARTUP=!progPath!\Cmpl\pyCmpl\pyCmpl\pyCmplShell.py
	set CMPLBINARY=!progPath!\Cmpl\bin\cmpl.exe

	!pythonBin! "%progPath%"\Cmpl\pyCmpl\pyCmpl\cmplOnServer.py !cmplFile! !cmplUrl! !cArgList!
) else (
	"%progPath%"\Cmpl\bin\cmpl !cmplFile! !cArgList!
)



:END
set PATH=%oldPath%

@echo on

