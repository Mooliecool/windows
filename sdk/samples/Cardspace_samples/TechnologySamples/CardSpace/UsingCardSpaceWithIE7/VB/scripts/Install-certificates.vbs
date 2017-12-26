'use common library
ExecuteGlobal CreateObject("Scripting.FileSystemObject").OpenTextFile( left( WScript.ScriptFullName , len (WScript.ScriptFullName) -len( WScript.ScriptName ) ) & "\vb-script-library.vbs",1).ReadAll()
InitScript("Certificate Installation Script")

'global variables 
DIM IISAccount, rootstore

'the certificate path 
function GetCertPath( )
	GetCertPath= fso.GetFolder(GetScriptPath).ParentFolder & "\certificates\"
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

sub VerifyIIS()
    on error resume next
    dim iis_installed

    iis_installed = wso.RegRead( "HKLM\Software\Microsoft\InetStp\PathWWWRoot")
    if err.number <> 0 OR iis_installed = "" then
         wso.popup( "It would appear that IIS is not installed." &vbcrlf& "Please install IIS prior to installing the CardSpace samples." )
         logerror "It would appear that IIS is not installed." &vbcrlf& "Please install IIS prior to installing the CardSpace samples." 
        finished 
    end if
end sub


REM SCRIPT STARTS HERE
VerifyCAPICOM 
VerifyIIS

dim filename
IISAccount = GetIISUser()

if wso.popup( "Note: this script installs the Certificates for the CardSpace samples."&vbcrlf&"It modifies ACLs on it's own certificate's private key files, in order that ASP.NET running under IIS can access the files. "&vbcrlf&vbcrlf&"This process could take a few minutes"&vbcrlf&vbcrlf&"WARNING: *These certificates are for testing only and should not be installed on a production machine*" , , "Note" , 49 ) = 2 then 
    logerror "Installation cancelled at user request"
    finished
end if

DeleteCertificate "root", "localMachine" , "adatum" 

' install root CA cert
set rootstore = OpenStore( "root", "localMachine" )
log "Installing Adatum Root CA public certificate from " & GetCertPath & "adatum.sst"
rootstore.Load GetCertPath & "ADATUM.sst"
rootstore.Close

' install leaf certs
DeleteCertificate "My", "localMachine" , "www.contoso.com" 
DeleteCertificate "My", "localMachine" , "www.fabrikam.com" 
DeleteCertificate "My", "localMachine" , "www.woodgrovebank.com" 
DeleteCertificate "My", "localMachine" , "www.adatum.com" 

log "Installing Adatum Website Certificate from " & GetCertPath & "www.adatum.com.pfx"
InstallCertificate "My", "localMachine" , GetCertPath & "www.adatum.com.pfx"
filename = GetKeyStoreFile( "My","localMachine", "www.adatum.com" )
AddReadACL filename , IISAccount 

log "Installing Fabrikam Website Certificate from " & GetCertPath & "www.fabrikam.com.pfx"
InstallCertificate "My", "localMachine" , GetCertPath & "www.fabrikam.com.pfx"
filename = GetKeyStoreFile( "My","localMachine", "www.fabrikam.com" )
AddReadACL filename , IISAccount 

log "Installing Contoso Website Certificate from " & GetCertPath & "www.contoso.com.pfx"
InstallCertificate "My", "localMachine" , GetCertPath & "www.contoso.com.pfx"
filename = GetKeyStoreFile( "My","localMachine", "www.contoso.com" )
AddReadACL filename , IISAccount 

log "Installing WoodgroveBank Website Certificate from " & GetCertPath & "www.WoodgroveBank.com.pfx"
InstallCertificate "My", "localMachine" , GetCertPath & "www.WoodgroveBank.com.pfx"
filename = GetKeyStoreFile( "My","localMachine", "www.woodgrovebank.com" )
AddReadACL filename , IISAccount 

finished