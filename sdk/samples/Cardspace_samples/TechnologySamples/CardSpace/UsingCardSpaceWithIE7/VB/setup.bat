@echo off
if NOT "%1" == "" goto normal
call "%~fp0" CAPICOM.DLL 
goto fin

:normal
setlocal
cd %~dp0

REM INSTALL CAPICOM 

REM Check on the SDK path first
set CAPICOM_FULLPATH=%~$SDKTOOLS:1
if exist "%CAPICOM_FULLPATH%" goto REGISTER_FULLPATH

REM Check on the path next
set CAPICOM_FULLPATH=%~$PATH:1
if exist "%CAPICOM_FULLPATH%" goto REGISTER_FULLPATH

REM One last guess for the SDK, is it in the default install directory
set SDKTOOLS=%PROGRAMFILES%\Microsoft SDKs\Windows\v6.0
if exist "%SDKTOOLS%\capicom.dll" goto REGISTER_SDK
set SDKTOOLS=

set SDKTOOLS=%PROGRAMFILES%\Microsoft SDKs\Windows\v6.0\bin\x64
if exist "%SDKTOOLS%\capicom.dll" goto REGISTER_SDK
set SDKTOOLS=

set SDKTOOLS=%PROGRAMFILES%\Microsoft SDKs\Windows\v6.0\bin
if exist "%SDKTOOLS%\capicom.dll" goto REGISTER_SDK
set SDKTOOLS=

REM Use the local one if it is here.
if exist "bin\capicom.dll" goto REGISTER_LOCAL

echo // An Error has ocurred during the installation of the CAPICOM library.
echo // The Platform SDK does not appear to be installed.
echo // 
echo // If you have installed the Platform SDK, ensure that this script
echo // run from the CMD Shell shortcut, in the start menu, under
echo // Microsoft Windows SDK.
goto fin


:REGISTER_FULLPATH
echo Registering SDK copy of CAPICOM.DLL
regsvr32 /u /s %CAPICOM_FULLPATH%
regsvr32 /s %CAPICOM_FULLPATH%
goto install_certificates

:REGISTER_LOCAL
echo Registering local copy of CAPICOM.DLL
regsvr32 /u /s "bin\capicom.dll" 
regsvr32 /s "bin\capicom.dll" 
goto install_certificates

:REGISTER_SDK
echo Registering SDK copy of CAPICOM.DLL
regsvr32 /u /s "%SDKTOOLS%\capicom.dll" 
regsvr32 /s "%SDKTOOLS%\capicom.dll" 
goto install_certificates

:install_certificates
REM INSTALL CERTIFICATES
cscript scripts\install-certificates.vbs
if ERRORLEVEL 1 goto failcert

REM INSTALL HOSTS
cscript scripts\install-hosts.vbs
if ERRORLEVEL 1 goto failhosts

REM INSTALL WEBSITES
cscript scripts\install-website.vbs
if ERRORLEVEL 1 goto failweb

goto fin


:failcert
echo The certificate installation script has failed.
goto fin

:failweb
echo The website installation script has failed.
goto fin

:failhosts
echo The hosts installation script has failed.
goto fin
:fin