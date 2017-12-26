@echo off
setlocal

set CONTOSO=Contoso
set FABRIKAM=Fabrikam

set CF_LOGO_FILE=SampleResources\contoso.gif
set FC_LOGO_FILE=SampleResources\fabrikam.gif

set CF_SUBJECT=CN=Contoso
set CF_SERIAL=61 09 6f db 00 00 00 00 00 02
set CF_THUMB=9b 8f ab 87 39 4f b9 e3 12 be f5 09 43 67 b9 df 35 ff 91 96

set FC_SUBJECT=CN=Fabrikam
set FC_SERIAL=61 09 70 21 00 00 00 00 00 03
set FC_THUMB=54 5c 3b 8e 97 d9 9f d7 5c 75 eb 52 c6 90 83 20 08 8b 4f 50

set CF_PFX_FILE=SampleResources\Contoso-Fabrikam.pfx
set FC_PFX_FILE=SampleResources\Fabrikam-Contoso.pfx
set PFX_PASSWORD=xyz

set CF_CER_FILE=SampleResources\PublicKeyCertificates\Contoso-Fabrikam-Public.cer
set FC_CER_FILE=SampleResources\PublicKeyCertificates\Fabrikam-Contoso-Public.cer
set CA_CER_FILE=SampleResources\PublicKeyCertificates\CardSpace-Sample-Public.cer

set VDIR_LOCATION=%SystemDrive%\Inetpub\wwwroot\ServiceModelSamples

set WP_ACCOUNT=NT AUTHORITY\NETWORK SERVICE
(ver | findstr /C:"5.1") && set WP_ACCOUNT=%COMPUTERNAME%\ASPNET

REM set path for findPrivateKey
set PATH=%PATH%;%ProgramFiles%\ServiceModelSampleTools

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

for /f "delims=" %%l in ('certmgr.exe -all -s -r CurrentUser TrustedPeople') do (

       if /i "%%l" == "   %FC_SERIAL%" (
           set FC_CurrentUser_TrustedPeople=1
       )else if /i "%%l" == "   %CF_SERIAL%" (
            set CF_CurrentUser_TrustedPeople=1
       ) 
)

for /f "delims=" %%l in ('certmgr.exe -all -s -r LocalMachine TrustedPeople') do (

       if /i "%%l" == "   %FC_SERIAL%" (
           set FC_LocalMachine_TrustedPeople=1
       ) else if /i "%%l" == "   %CF_SERIAL%" (
           set CF_LocalMachine_TrustedPeople=1
       ) 
)

for /f "delims=" %%l in ('certmgr.exe -all -s -r LocalMachine My') do (

       if /i "%%l" == "   %FC_SERIAL%" (
           set FC_LocalMachine_My=1
       ) else if /i "%%l" == "   %CF_SERIAL%" (
           set CF_LocalMachine_My=1
       )
)

REM Add FABRIKAM to LocalMachine and CurrentUser TrustedPeople
if NOT %FC_LocalMachine_TrustedPeople% == 1 (

    REM Adding to LocalMachine TrustedPeople also adds to CurrentUser TrustedPeople
    REM So delete cert.
    if %FC_CurrentUser_TrustedPeople% == 1 (
        echo.
        echo Delete %FABRIKAM% from CurrentUser TrustedPeople ...
        certmgr.exe -del -c -n %FABRIKAM% -s -r CurrentUser TrustedPeople   
    )
    
    REM Add cert.
    echo.
    echo Import %FABRIKAM% to LocalMachine and CurrentUser TrustedPeople ...
    certmgr.exe -add -c %FC_CER_FILE% -s -r LocalMachine TrustedPeople
    certmgr.exe -add -c %CA_CER_FILE% -s -r LocalMachine AuthRoot
    
) 

if %INSTALL_CONTOSO% == 1 (

    REM Add CONTOSO to LocalMachine 
    if NOT %CF_LocalMachine_TrustedPeople% == 1  (

        REM Adding to LocalMachine TrustedPeople also adds to CurrentUser TrustedPeople
        REM So delete cert.
        if %CF_CurrentUser_TrustedPeople% == 1 (
            echo.
            echo Delete %CONTOSO% from CurrentUser TrustedPeople ...
            certmgr.exe -del -c -n %CONTOSO% -s -r CurrentUser TrustedPeople   
        )

        REM Add cert.
        echo.
        echo Import %CONTOSO% to LocalMachine and CurrentUser TrustedPeople ...
        certmgr.exe -add -c %CF_CER_FILE% -s -r LocalMachine TrustedPeople

    ) 
)

echo.
echo Copying logo files to %VDIR_LOCATION%\ ...
xcopy /Y %CF_LOGO_FILE% %VDIR_LOCATION%\
xcopy /Y %FC_LOGO_FILE% %VDIR_LOCATION%\

set XP=0
(ver | findstr /C:"5.1") && set XP=1
if "%XP%"=="1" (
goto SETKEYPERMS
)

REM Import server certificates on Windows 2003

if %INSTALL_CONTOSO% == 1 (

    if NOT %CF_LocalMachine_My% == 1 (
        echo.
        echo Importing %CF_PFX_FILE% to LocalMachine/My store ...
        certutil -importpfx -p %PFX_PASSWORD% %CF_PFX_FILE%
    )
)
if NOT %FC_LocalMachine_My% == 1 (
    echo.
    echo Importing %FC_PFX_FILE% to LocalMachine/My store ...
    certutil -importpfx -p %PFX_PASSWORD% %FC_PFX_FILE%
)

:SETKEYPERMS


echo.
echo Searching for server certificates in LocalMachine/My(Personal) store...

REM Look for server certificate private key files
set FC_PRIVATE_KEY_FILE=
set CF_PRIVATE_KEY_FILE=

for /f "delims=" %%l in ('certmgr.exe -all -s -r localmachine my') do (

       if /i "%%l" == "   %CF_SERIAL%" ( 
       for /F "delims=" %%i in ('FindPrivateKey.exe My LocalMachine -t "%CF_THUMB%" -a') do set CF_PRIVATE_KEY_FILE=%%i
       ) else if /i "%%l" == "   %FC_SERIAL%" (
           for /F "delims=" %%i in ('FindPrivateKey.exe My LocalMachine -t "%FC_THUMB%" -a') do set FC_PRIVATE_KEY_FILE=%%i
       )
)


REM Need to restart IIS if we set ACLs on server keys
set restartiis=0

if %INSTALL_CONTOSO% == 1 (

    if not "" == "%CF_PRIVATE_KEY_FILE%"  (
        echo.
        echo Giving "%WP_ACCOUNT%" read permissions on %CF_SUBJECT% private key...
        echo.
        echo Y|cacls.exe "%CF_PRIVATE_KEY_FILE%" /E /G "%WP_ACCOUNT%":R

        set restartiis=1
    )
)

if not "" == "%FC_PRIVATE_KEY_FILE%" (
    echo.
    echo Giving "%WP_ACCOUNT%" read permissions on %FC_SUBJECT% private key...
    echo.
    echo Y|cacls.exe "%FC_PRIVATE_KEY_FILE%" /E /G "%WP_ACCOUNT%":R

    set restartiis=1
)

if %restartiis% == 1 (
    echo.
    echo Restarting IIS because a certificate private key ACL was set...

    iisreset
)

set XP=0
(ver | findstr /C:"5.1") && set XP=1
if NOT "%XP%"=="1" (
goto end
)

REM Show instructions for adding server certificates
set showInstr=0

if %INSTALL_CONTOSO% == 1 (

    if "" == "%CF_PRIVATE_KEY_FILE%" (
        echo.
        echo ********************************
        echo MANUAL SET UP TASK NOT COMPLETE:
        echo ********************************
        echo Import %CF_PFX_FILE% 
        echo into LocalMachine/My ^(Personal^) certificate store.

        set showInstr=1
    )
)

if "" ==  "%FC_PRIVATE_KEY_FILE%" (
    echo.
    echo ********************************
    echo MANUAL SET UP TASK NOT COMPLETE:
    echo ********************************
    echo Import %FC_PFX_FILE% 
    echo into LocalMachine/My ^(Personal^) certificate store.
    
    set showInstr=1
)

if %showInstr% == 1 (
    echo.
    echo ********************************
    echo MANUAL SET UP INSTRUCTIONS:
    echo ********************************
    echo Use the MMC Console Certificates Snap-in to install the 
    echo required server certificates into the 
    echo LocalMachine/My ^(Personal^) certificate store.
    echo.
    echo The password for the PFX files is "%PFX_PASSWORD%".
    echo.
    echo Then run this script again to give the ASPNET user
    echo read permission on the private key^(s^).
    echo.
    echo SEE ALSO:
    echo See this sample's readme.xml for detailed instructions.
    echo.
    pause
)



:end
endlocal
