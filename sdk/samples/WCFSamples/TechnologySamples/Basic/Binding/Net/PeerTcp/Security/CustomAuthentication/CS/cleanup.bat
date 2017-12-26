echo off
set CLIENT_NAME=%1
set ISSUER_NAME=%2
echo ****************
echo Cleanup starting
echo ****************

if "%1" == "" set CLIENT_NAME=peer
if "%2" == "" set ISSUER_NAME=TrustedPeerChannelIssuer

set CLIENT1_NAME=%CLIENT_NAME%1
set CLIENT2_NAME=%CLIENT_NAME%2

echo *****************
echo deleting client certs
echo *****************
certmgr.exe -del -r CurrentUser -s My -c -n %CLIENT1_NAME%
certmgr.exe -del -r CurrentUser -s My -c -n %CLIENT2_NAME%

echo *****************
echo deleting issuer cert
echo *****************
certmgr.exe -del -r CurrentUser -s TrustedPeople -c -n %ISSUER_NAME%
%COMSPEC% /c

echo *****************
echo Cleanup completed
echo *****************


