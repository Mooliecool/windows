@echo off
setlocal

set SamplesVRootDir=%SystemDrive%\inetpub\wwwroot\FederationSample
mkdir %SamplesVRootDir%

echo Creating virtual and file system directories...

echo.
echo FederationSample
Scripts\SetUpVDir.vbs "FederationSample" %SamplesVRootDir%

echo.
for /f  %%p in (Scripts\ServiceNames.txt) do (

    echo %%p

    mkdir %SamplesVRootDir%\%%p
    mkdir %SamplesVRootDir%\%%p\bin

    Scripts\SetUpVDir.vbs FederationSample/%%p %SamplesVRootDir%\%%p
    
    )
)

echo.
call Scripts\SetUpCerts.bat

echo Done.
pause