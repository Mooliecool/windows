echo off
echo ************
echo Client cert setup starting
echo ************
set SERVER_NAMe=sender


echo ****************
echo Cleanup starting
echo ****************

echo -------------------------
echo del client certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s MY -c -n %SERVER_NAME%
%COMSPEC% /c

echo -------------------------
echo del service certs
echo -------------------------
certmgr.exe -del -r CurrentUser -s TrustedPeople -c -n %SERVER_NAME%
%COMSPEC% /c

echo *****************
echo Cleanup completed
echo *****************

 
echo ************
echo making certificates
echo ************
makecert.exe -sr CurrentUser -ss MY -a sha1 -n CN=%SERVER_NAME% -sky exchange -pe
echo ************
echo ************
certmgr.exe -add -r CurrentUser -s My -c -n %SERVER_NAME% -r CurrentUser -s TrustedPeople


%COMSPEC% /c