echo off
set SERVER_NAME=localhost

echo ****************
echo Cleanup starting
echo ****************

echo -------------------------
echo del service certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s TrustedPeople -c -n %SERVER_NAME%
certmgr.exe -del -r LocalMachine -s My -c -n %SERVER_NAME%


echo *****************
echo Cleanup completed
echo *****************


