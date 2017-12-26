@echo off
setlocal

set STARTDIR=%cd%
cd %~dp0%

REM Clean up virtual directories of projects in the sample
REM Delete folders from wwwroot
echo.
echo Stopping IIS in order to delete sample folders from wwwroot...

iisreset /stop

echo.
echo Deleting sample virtual directories from IIS, and
echo Deleting sample files from wwwroot...

for /f  %%p in (SampleResources\ServiceNames.txt) do (

SampleResources\CleanUpVdir.vbs ServiceModelSamples/%%p
rd /s /q %SystemDir%\inetpub\wwwroot\ServiceModelSamples\%%p
)

iisreset /start

REM Remove certs from store and delete cert logo files from wwwroot
echo.
call SampleResources/CleanUpCerts.bat

echo.
pause