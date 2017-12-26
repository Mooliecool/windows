@echo off
setlocal

makecert.exe -a sha1 -n CN=BookStoreService.com -sr LocalMachine -ss My -sky exchange -sk BookStoreService
certmgr.exe -add -c -n BookStoreService -s -r localMachine My -s -r localMachine TrustedPeople

makecert.exe -a sha1 -n CN=BookStoreSTS.com -sr LocalMachine -ss My -sky exchange -sk BookStoreSTS
certmgr.exe -add -c -n BookStoreSTS.com -s -r localMachine My -s -r localMachine TrustedPeople

makecert.exe -a sha1 -n CN=HomeRealmSTS.com -sr LocalMachine -ss My -sky exchange -sk HomeRealmSTS
certmgr.exe -add -c -n HomeRealmSTS.com -s -r localMachine My -s -r localMachine TrustedPeople

for /F "delims=" %%i in ('"%ProgramFiles%\ServiceModelSampleTools\FindPrivateKey.exe" My LocalMachine -n CN^=BookStoreService.com -a') do set PRIVATE_KEY_FILE=%%i
echo %PRIVATE_KEY_FILE%
set WP_ACCOUNT=NT AUTHORITY\NETWORK SERVICE
(ver | findstr /C:"5.1") && set WP_ACCOUNT=%COMPUTERNAME%\ASPNET
echo Y|cacls.exe "%PRIVATE_KEY_FILE%" /E /G "%WP_ACCOUNT%":R

for /F "delims=" %%i in ('"%ProgramFiles%\ServiceModelSampleTools\FindPrivateKey.exe" My LocalMachine -n CN^=BookStoreSTS.com -a') do set PRIVATE_KEY_FILE=%%i
echo %PRIVATE_KEY_FILE%
set WP_ACCOUNT=NT AUTHORITY\NETWORK SERVICE
(ver | findstr /C:"5.1") && set WP_ACCOUNT=%COMPUTERNAME%\ASPNET
echo Y|cacls.exe "%PRIVATE_KEY_FILE%" /E /G "%WP_ACCOUNT%":R

for /F "delims=" %%i in ('"%ProgramFiles%\ServiceModelSampleTools\FindPrivateKey.exe" My LocalMachine -n CN^=HomeRealmSTS.com -a') do set PRIVATE_KEY_FILE=%%i
echo %PRIVATE_KEY_FILE%
set WP_ACCOUNT=NT AUTHORITY\NETWORK SERVICE
(ver | findstr /C:"5.1") && set WP_ACCOUNT=%COMPUTERNAME%\ASPNET
echo Y|cacls.exe "%PRIVATE_KEY_FILE%" /E /G "%WP_ACCOUNT%":R

iisreset
