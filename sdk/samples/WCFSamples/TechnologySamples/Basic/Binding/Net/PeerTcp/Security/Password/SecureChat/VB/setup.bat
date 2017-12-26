echo off
echo ************
echo Client cert setup starting
echo ************
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

 
echo ************
echo making certificates
echo ************
makecert.exe -sr CurrentUser -ss MY -a sha1 -n CN=%CLIENT1_NAME% -sky exchange -pe
makecert.exe -sr CurrentUser -ss MY -a sha1 -n CN=%CLIENT2_NAME% -sky exchange -pe
echo ************
echo ************
certmgr.exe -add -r CurrentUser -s My -c -n %CLIENT1_NAME% -r CurrentUser -s TrustedPeople
certmgr.exe -add -r CurrentUser -s My -c -n %CLIENT2_NAME% -r CurrentUser -s TrustedPeople


%COMSPEC% /c