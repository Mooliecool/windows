@echo off
setlocal

set STARTDIR=%cd%
cd %~dp0%

set CardSpaceServiceName="idsvc"

set ACCOUNT_DB_FILE=AccountDB.xml
set ACCOUNTDB_PROJ=ProvisioningService

set SamplesVRootDir=%SystemDrive%\inetpub\wwwroot\servicemodelsamples

set WP_ACCOUNT=NT AUTHORITY\NETWORK SERVICE
(ver | findstr /C:"5.1") && set WP_ACCOUNT=%COMPUTERNAME%\ASPNET

if defined USERDOMAIN (
    set USER_ACCOUNT=%USERDOMAIN%\%USERNAME%
) else (
    set USER_ACCOUNT=%COMPUTERNAME%\%USERNAME%
)

mkdir %SamplesVRootDir%
mkdir %SamplesVRootDir%\bin

echo Creating virtual and file system directories...

echo.
echo ServiceModelSamples
SampleResources\SetUpVdir.vbs "ServiceModelSamples" %SamplesVRootDir%

echo.
for /f  %%p in (SampleResources\ServiceNames.txt) do (

    echo %%p

    mkdir %SamplesVRootDir%\%%p
    mkdir %SamplesVRootDir%\%%p\bin

    SampleResources\SetUpVdir.vbs ServiceModelSamples/%%p %SamplesVRootDir%\%%p
    
    if "%%p" == "%ACCOUNTDB_PROJ%" (

       if Not Exist "%SamplesVRootDir%\%%p\bin\%ACCOUNT_DB_FILE%" (
            echo.
            echo Creating file "%SamplesVRootDir%\%%p\bin\%ACCOUNT_DB_FILE%" ...
            copy "SampleResources\%ACCOUNT_DB_FILE%" "%SamplesVRootDir%\%%p\bin\%ACCOUNT_DB_FILE%"
        )

        echo.
        echo Set permissions on "%SamplesVRootDir%\%%p\bin\%ACCOUNT_DB_FILE%" ...
        echo.
        echo Y|cacls.exe "%SamplesVRootDir%\%%p\bin\%ACCOUNT_DB_FILE%" /G "Administrators:F" "%USER_ACCOUNT%":F "%WP_ACCOUNT%":C 
    )
)

echo.
call SampleResources\SetUpCerts.bat

echo.
REM Start CardSpace service if it is not running
for /f "delims=" %%i in ('net start ^| find /c /i %CardSpaceServiceName%') do ( 
    if %%i == 0 net start %CardSpaceServiceName%
)

pause