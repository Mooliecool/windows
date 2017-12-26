echo off
set SERVER_NAME=sender

echo ****************
echo Cleanup starting
echo ****************

echo -------------------------
echo del client certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s My -c -n %SERVER_NAME%

%COMSPEC% /c

echo -------------------------
echo del service certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s TrustedPeople -c -n %SERVER_NAME%

%COMSPEC% /c

echo *****************
echo Cleanup completed
echo *****************


