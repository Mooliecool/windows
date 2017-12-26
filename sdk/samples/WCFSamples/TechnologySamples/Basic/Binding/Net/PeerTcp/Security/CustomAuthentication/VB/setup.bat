echo off
echo ************
echo Client cert setup starting
echo ************



set CLIENT_NAME=%1
set ISSUER_NAME=%2

if "%1" == "" set CLIENT_NAME=peer
if "%2" == "" set ISSUER_NAME=TrustedPeerChannelIssuer

set CLIENT1_NAME=%CLIENT_NAME%1
set CLIENT2_NAME=%CLIENT_NAME%2

set ISSUER_NAME_OPTION=-in %ISSUER_NAME%
set CLIENT_NAME_OPTION=-n CN=%CLIENT_NAME%
set ISSUER_STORE_OPTION=-is TrustedPeople
set ISSUER_LOCATION_OPTION=-ir CurrentUser

set FILENAME=%ISSUER_NAME%.cer

echo -------------------------
echo deleting old certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s My -c -n %CLIENT1_NAME%
certmgr.exe -del -r CurrentUser -s My -c -n %CLIENT2_NAME%
certmgr.exe -del -r CurrentUser -s TrustedPeople -c -n %ISSUER_NAME% 
%COMSPEC% /c

 
echo ************
echo making issuer certificate
echo ************
makecert.exe -sr CurrentUser -ss TrustedPeople -a sha1 -n CN=%ISSUER_NAME% -sky exchange -pe -r
certmgr.exe -put -r CurrentUser -s TrustedPeople -c -n %ISSUER_NAME% %FILENAME%

echo ************
echo making client certificates
echo ************
makecert.exe %ISSUER_LOCATION_OPTION% %ISSUER_STORE_OPTION% %ISSUER_NAME_OPTION% -sr CurrentUser -ss MY -a sha1 -n CN=%CLIENT1_NAME% -sky exchange -pe
makecert.exe %ISSUER_LOCATION_OPTION% %ISSUER_STORE_OPTION% %ISSUER_NAME_OPTION% -sr CurrentUser -ss MY -a sha1 -n CN=%CLIENT2_NAME% -sky exchange -pe

%COMSPEC% /c