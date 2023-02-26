@set ENGINE=%TEMP%\VadymChan
@IF NOT EXIST %ENGINE% MKDIR "%ENGINE%"
@set SIFILE=%ENGINE%\VadymChan.SysInfo.txt
@echo Generating System Info report in %SIFILE%
dxdiag.exe /t "%SIFILE%"
start explorer "%ENGINE%"
exit

