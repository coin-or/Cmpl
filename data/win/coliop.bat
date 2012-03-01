@echo off


echo Starting Coliop and setting environment

set PATH=%PATH%;%PROGRAMFILES%\Cmpl\bin;%PROGRAMFILES%\Cmpl\Frameworks

cd %PROGRAMFILES%\Cmpl\Coliop3

java -jar coliop.jar



exit

@echo on
