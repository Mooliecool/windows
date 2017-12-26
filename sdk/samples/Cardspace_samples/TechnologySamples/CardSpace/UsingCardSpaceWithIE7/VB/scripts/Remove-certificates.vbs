'use common library
ExecuteGlobal CreateObject("Scripting.FileSystemObject").OpenTextFile( left( WScript.ScriptFullName , len (WScript.ScriptFullName) -len( WScript.ScriptName ) ) & "\vb-script-library.vbs",1).ReadAll()
InitScript("Certificate Cleanup Script")

'global variables 
DIM IISAccount, rootstore

'the certificate path 
function GetCertPath( )
	GetCertPath= fso.GetFolder(GetScriptPath).ParentFolder & "\certs\"
end function

' test for CAPICOM library.
sub VerifyCAPICOM()
dim store
    on error resume next
    set store = CreateObject("Capicom.Store")
    if err.number <> 0 then
		logerror "It would appear that the CAPICOM library is not installed." 
		logerror "Please run the install-capicom.bat script to install CAPICOM." 
		finished 
	end if
end sub

REM SCRIPT STARTS HERE
VerifyCAPICOM 

DeleteCertificate "root", "localMachine" , "adatum" 

' install leaf certs
DeleteCertificate "My", "localMachine" , "www.contoso.com" 
DeleteCertificate "My", "localMachine" , "www.fabrikam.com" 
DeleteCertificate "My", "localMachine" , "www.woodgrovebank.com" 
DeleteCertificate "My", "localMachine" , "www.adatum.com" 

finished