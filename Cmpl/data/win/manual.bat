@echo off
set progPath=%~dp0
set manual="%progPath%..\doc\CMPL.pdf"
rundll32 url.dll,FileProtocolHandler %manual%
@echo on

