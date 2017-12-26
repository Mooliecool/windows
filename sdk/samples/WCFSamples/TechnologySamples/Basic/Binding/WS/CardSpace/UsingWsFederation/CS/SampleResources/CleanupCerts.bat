@echo off
setlocal

set PATH=%PATH%;%MSSDK%\bin

set CONTOSO=Contoso
set FABRIKAM=Fabrikam
set CARDSPACE=CardSpace

set CF_SERIAL=61 09 6f db 00 00 00 00 00 02
set CF_THUMB=9b 8f ab 87 39 4f b9 e3 12 be f5 09 43 67 b9 df 35 ff 91 96

set FC_SERIAL=61 09 70 21 00 00 00 00 00 03
set FC_THUMB=54 5c 3b 8e 97 d9 9f d7 5c 75 eb 52 c6 90 83 20 08 8b 4f 50

set CARDSPACE_SERIAL=3d 03 54 a9 1a c3 06 bc 43 f4 2f ea 02 78 d4 9c
set CARDSPACE_THUMB=59 2f 4b 9d 7b 1f e0 90 41 f2 42 0d 9d 60 e0 06 1f 2c 89 56

set VDIR_LOCATION=%SystemDrive%\Inetpub\wwwroot\ServiceModelSamples

REM Set INSTALL_CONTOSO = 1 if using InfoCardSamlSecurityTokenService
set INSTALL_CONTOSO=0
for /f  %%p in (SampleResources\ServiceNames.txt) do (
    if "%%p" == "InfoCardSamlSecurityTokenService" (
        set INSTALL_CONTOSO=1
    )
)

REM Determine which certificates are installed.
set FC_CurrentUser_TrustedPeople=0
set FC_LocalMachine_TrustedPeople=0
set CF_CurrentUser_TrustedPeople=0
set CF_LocalMachine_TrustedPeople=0
set FC_LocalMachine_My=0
set CF_LocalMachine_My=0
set CARDSPACE_LocalMachine_My=0

for /f "delims=" %%l in ('certmgr.exe -all -s -r LocalMachine TrustedPeople') do (

       if /i "%%l" == "   %FC_SERIAL%" (
           set FC_LocalMachine_TrustedPeople=1
       )else if /i "%%l" == "   %CF_SERIAL%" (
           set CF_LocalMachine_TrustedPeople=1
       ) 
)

for /f "delims=" %%l in ('certmgr.exe -all -s -r LocalMachine My') do (

       if /i "%%l" == "   %FC_SERIAL%" (
           set FC_LocalMachine_My=1
       ) else if /i "%%l" == "   %CF_SERIAL%" (
           set CF_LocalMachine_My=1
       ) else if /i "%%l" == "   %CARDSPACE_SERIAL%" (
           set CARDSPACE_LocalMachine_My=1
       )
)

REM Fabrikam public key certs
if %FC_LocalMachine_TrustedPeople% == 1 (
    echo.
    echo Deleting Fabrikam public key certificate...
    echo Deleting LocalMachine TrustedPeople %FABRIKAM%...
    certmgr.exe -del -r LocalMachine -s TrustedPeople -c -n %FABRIKAM% 
)

echo.
echo Deleting CardSpace sample root public certificate....
certmgr.exe -del -r LocalMachine -s AuthRoot -c -n %CARDSPACE% 

if %INSTALL_CONTOSO% == 1 (

    REM Contoso public key certs
    if %CF_LocalMachine_TrustedPeople% == 1 (
        echo.
        echo Deleting Contoso public key certificate...
        echo Deleting LocalMachine TrustedPeople %CONTOSO%...
        certmgr.exe -del -r LocalMachine -s TrustedPeople -c -n %CONTOSO% 
    )

)

REM echo Check for Fabrikam, Contoso, and CardSpace private key certificates.

if %FC_LocalMachine_My% == 1 (
    echo.
    echo Deleting LocalMachine My %FABRIKAM% ...
    certmgr.exe -del -r LocalMachine -s My -c -n %FABRIKAM% 
)

if %INSTALL_CONTOSO% == 1 (

    if %CF_LocalMachine_My% == 1 (
        echo.
        echo Deleting LocalMachine My %CONTOSO% ...
        certmgr.exe -del -r LocalMachine -s My -c -n %CONTOSO% 
    )
)

if %CARDSPACE_LocalMachine_My% == 1 (
    echo.
    echo Deleting LocalMachine My %CARDSPACE% ...
    certmgr.exe -del -r LocalMachine -s My -c -n %CARDSPACE% 
)

echo.
echo Deleting logo files from %VDIR_LOCATION%...
echo.
del %VDIR_LOCATION%\%CONTOSO%.gif
del %VDIR_LOCATION%\%FABRIKAM%.gif
echo.
echo Certificate clean up completed.

endlocal

