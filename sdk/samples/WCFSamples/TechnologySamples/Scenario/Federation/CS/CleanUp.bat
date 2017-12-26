@echo off
setlocal

REM Clean up virtual directories of projects in the sample
REM Delete folders from wwwroot
echo.
echo Stopping IIS in order to delete sample folders from wwwroot...

iisreset /stop

echo.
echo Deleting sample virtual directories from IIS, and
echo Deleting sample files from wwwroot...

for /f  %%p in (Scripts\ServiceNames.txt) do (

Scripts\CleanUpVdir.vbs FederationSample/%%p
)

Scripts\CleanUpVdir.vbs FederationSample

iisreset /start

REM Remove certs from store
echo.
call Scripts/CleanUpCerts.bat

echo.
pause