echo off
set CLIENT1_NAME=peer1
set CLIENT2_NAME=peer2

echo ****************
echo Cleanup starting
echo ****************

echo -------------------------
echo del client certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s My -c -n %CLIENT1_NAME%
certmgr.exe -del -r CurrentUser -s My -c -n %CLIENT2_NAME%

%COMSPEC% /c

echo -------------------------
echo del service certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s TrustedPeople -c -n %CLIENT1_NAME%
certmgr.exe -del -r CurrentUser -s TrustedPeople -c -n %CLIENT2_NAME%

%COMSPEC% /c

echo *****************
echo Cleanup completed
echo *****************


