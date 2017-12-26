Option Explicit

REM GLOBAL SCRIPT OBJECTS
Dim wso, fso, logfile, successful , verbose, ScriptName  , interactivelogging, system_architecture, process_architecture

REM Constants 
Const CAPICOM_MEMORY_STORE = 0
Const CAPICOM_LOCAL_MACHINE_STORE = 1
Const CAPICOM_CURRENT_USER_STORE = 2
Const CAPICOM_ACTIVE_DIRECTORY_USER_STORE = 3
Const CAPICOM_SMART_CARD_USER_STORE = 4

Const CAPICOM_STORE_OPEN_READ_ONLY = 0
Const CAPICOM_STORE_OPEN_READ_WRITE = 1
Const CAPICOM_STORE_OPEN_MAXIMUM_ALLOWED = 2
Const CAPICOM_STORE_OPEN_EXISTING_ONLY = 128
Const CAPICOM_STORE_OPEN_INCLUDE_ARCHIVED = 256

const ADS_RIGHT_GENERIC_READ = &h80000000
const ADS_PATH_FILE = 1
const ADS_SD_FORMAT_IID = 1
const ADS_ACETYPE_ACCESS_ALLOWED = 0
const ADS_ACETYPE_ACCESS_DENIED = 1
const ADS_ACETYPE_SYSTEM_AUDIT = 2
const ADS_ACETYPE_ACCESS_ALLOWED_OBJECT = 5
const ADS_ACETYPE_ACCESS_DENIED_OBJECT = 6
const ADS_ACETYPE_SYSTEM_AUDIT_OBJECT = 7
const ADS_ACEFLAG_INHERIT_ACE = 2
Const ForReading = 1
Const ForWriting = 2    
Const ForAppending = 8

set wso = CreateObject("Wscript.Shell")
set fso = CreateObject("Scripting.FileSystemObject")
logfile = "Script Results" & vbcrlf & "----------------------------" & vbcrlf &vbcrlf 
ScriptName = "Script ("& WScript.ScriptName &")"
successful = true
verbose = false
interactivelogging = false

sub PlatformDetect
    On Error resume next
    dim WshProcEnv
    
    Set WshProcEnv = wso.Environment("Process")
    process_architecture= WshProcEnv("PROCESSOR_ARCHITECTURE")
    if process_architecture = "x86" then
        system_architecture= WshProcEnv("PROCESSOR_ARCHITEW6432")
        if system_architecture = ""  then
            system_architecture = "x86"
        end if
    else
        system_architecture = process_architecture
    end if
end sub
PlatformDetect 'Run right away.

sub EnsureNativeScriptEngine
    if NOT system_architecture = process_architecture then 
        wso.popup "This script should be run as a "& system_architecture &" process, but is running as a "& process_architecture &" process " & vbcrlf & vbcrlf & "This is likely when running the script on a 64 bit platform, launched from a 32 bit application. Run the script from the Windows Explorer, or a 64 bit cmd console window"
        WScript.quit 1 
    end if
end sub

sub EnsureX86
	if process_architecture <> "x86" then 
        dim args, i
        for i = 0 to WScript.Arguments.length -1
            args= args+" "+chr(34)+WScript.Arguments(i)+chr(34)
        next
        
        dim Pipe 
        set Pipe = wso.Exec( Replace( LCase(WScript.FullName)  , "system32" ,"syswow64" ) & " " & chr(34) & WScript.ScriptFullName & chr(34) &  args )
        while NOT Pipe.StdOut.AtEndOfStream
            WScript.StdOut.WriteLine Pipe.StdOut.ReadLine()    
        wend
		while Pipe.Status = 0 
		   WScript.Sleep 100
		wend 
		i = Pipe.ExitCode
        WScript.quit 0+i
    end if
end sub

EnsureX86

'keeps a log of the actions performed
sub log(text)
    if interactivelogging then 
        Wscript.echo text 
    else
        logfile = logfile & text & vbcrlf    
    end if
end sub

'keeps a log of the actions performed
sub logerror(text)
    if interactivelogging then 
        Wscript.echo text 
        WScript.Quit 1
    else
        logfile = logfile & vbcrlf &  text & vbcrlf
    end if
    successful = false
end sub

' logs only if verbose is true
sub logverbose(text)
    if verbose then 
        log(text)
    end if
end sub

' logwarning is the same as log verbose
sub logwarning(text)
    logverbose(text)
end sub

'checks to see if this is run as a windowed or console version
function IsCScript()
    IsCScript = instr( 1, WScript.FullName , "CSCRIPT" , VBTextCompare) > 0
end function

'gets  the path this script is in
function GetScriptPath()
	GetScriptPath = left( WScript.ScriptFullName , len (WScript.ScriptFullName) -len( WScript.ScriptName ) )
end function

'gets the path for FindPrivateKey
function GetFindPrivateKeyPath()
on error resume next
	dim path
	dim paths

	path = wso.Environment("Process")("PATH")
	' is it in the path?
	paths = Split(path, ";", -1, 1)
	for each path in paths
		path = path & "\findprivatekey.exe"
		if fso.FileExists(path) then
			GetFindPrivateKeyPath = path
			exit function
		end if
	next 

	' is it in the platform SDK ?
	path = wso.RegRead( "HKCU\Software\Microsoft\Microsoft SDKs\Windows\v6.0\WinSDKWin32Tools\InstallationFolder")
	if err.number<>0  then
		path = ""
	else
		path = path&"bin\findprivatekey.exe"
		
		if fso.FileExists(path) then
       		GetFindPrivateKeyPath = path
			exit function
		end if
		
		path = path&"\bin\findprivatekey.exe"
		if fso.FileExists(path) then
       		GetFindPrivateKeyPath = path
			exit function
		end if
	end if 
	' is it in the ..\bin directory?
	path = fso.GetFolder(GetScriptPath).ParentFolder & "\bin\findprivatekey.exe"
	if fso.FileExists(path) then
		GetFindPrivateKeyPath = path
		exit function
	end if
	
	path = wso.Environment("Process")("ProgramFiles") & "\ServiceModelSampleTools\findprivatekey.exe"
	if fso.FileExists(path) then
		GetFindPrivateKeyPath = path
		exit function
	end if		

	logerror "Unable to find the FINDPRIVATEKEY.EXE utility."
	logerror "Please place it in the path, or install the Windows SDK." 
	finished

end function


'a better Trim function, removes lf's and cr's too!
function TrimEx(text)
	dim rx
	set rx = new regexp
	rx.pattern = "^\s+|\s+$"
	rx.global = true
	rx.ignorecase = true
	TrimEx = rx.replace(text,"")
end function

'call when the script is done, will notify the user
sub finished()
    if IsCScript() then 
        if Not SUCCESSFUL then
            WScript.echo logfile
            WScript.Quit 1
        else
            WScript.echo ScriptName & " executed successfully"
            WScript.Quit
        end if
        
    else
        if Not SUCCESSFUL then
            wso.popup logfile
            WScript.Quit 1
        else
            wso.popup ScriptName & " executed successfully"
            WScript.Quit
        end if
    end if
end sub

' call to setup the script
Sub InitScript(name)
    dim arg
    ScriptName = name
    logfile = ScriptName & vbcrlf & "----------------------------" & vbcrlf &vbcrlf 

    if( WScript.Arguments.Count > 0 ) then 
        ' check for command line args 
        for each arg in WScript.Arguments
            if strcomp( arg , "verbose", vbTextCompare )  then
                verbose = true
            end if
            if strcomp( arg , "debug", vbTextCompare )  then
                interactivelogging= true
            end if
        next 
    end if
end sub

function AddToPath( currentPath  , pathToAdd)
	if InStr( 1,currentPath , pathToAdd  , 1 ) = 0 then 
        if NOT currentPath = "" then 
		    currentPath = currentPath & ";"  & pathToAdd
        else
            currentPath = pathToAdd
        end if
	end if 
	AddToPath = currentPath
end function

'gets the IIS User Account
function GetIISUser()
    Dim colItems, objItem, WinVersion, Computer_name 
    Set colItems = GetObject("winmgmts:\\localhost\root\cimv2").ExecQuery("Select * from Win32_OperatingSystem",,48)
    For Each objItem in colItems
        WinVersion = Left(objItem.Version,3)
        Computer_Name = objItem.CSName
    Next

    if WinVersion = "5.1" then 
        '5.1  XP
        GetIISUser =  Computer_Name & "\ASPNET"
    else 
        'Windows 2003 Server
        GetIISUser = "NT AUTHORITY\NETWORK SERVICE"
    end if
end function

'Adds a READ ACL for a user on a file
sub AddReadACL( resource, user )
    dim oADsSecurityUtility, oSD, oDacl, oAce
    
    set oADsSecurityUtility = CreateObject("ADsSecurityUtility")
    set oSD = oADsSecurityUtility.GetSecurityDescriptor(resource, ADS_PATH_FILE, ADS_SD_FORMAT_IID)
    set oDacl = oSD.DiscretionaryAcl

    set oAce = CreateObject("AccessControlEntry")
    oAce.Trustee = user
    oAce.AccessMask = ADS_RIGHT_GENERIC_READ

    oAce.AceType = ADS_ACETYPE_ACCESS_ALLOWED
    oAce.AceFlags = ADS_ACEFLAG_INHERIT_ACE + 1
    oDacl.AddAce(oAce)
    oSD.DiscretionaryAcl = oDacl
    oADsSecurityUtility.SetSecurityDescriptor resource , ADS_PATH_FILE, oSD, ADS_SD_FORMAT_IID
end sub

'Removes the ACL for a user on a file
sub RemoveACL( resource, user )
    dim oADsSecurityUtility, oSD, oDacl, oAce
    set oADsSecurityUtility = CreateObject("ADsSecurityUtility")

    set oSD = oADsSecurityUtility.GetSecurityDescriptor(resource, ADS_PATH_FILE, ADS_SD_FORMAT_IID)
    set oDacl = oSD.DiscretionaryAcl

    For Each Ace in oDacl
        If Ace.trustee = user then 
           oDacl.RemoveAce Ace 
        End If
    Next

    oSD.DiscretionaryAcl = oDacl
    oADsSecurityUtility.SetSecurityDescriptor resource , ADS_PATH_FILE, oSD, ADS_SD_FORMAT_IID
end sub

'Gets the path to a private key store file
function GetKeyStoreFileFromThumbprint(storename, storelocation, thumbprint)
    dim objScriptExec 
    Set objScriptExec = wso.Exec(GetFindPrivateKeyPath & " " & storename & " " & storelocation & " -t "& thumbprint & " -a")
    GetKeyStoreFileFromThumbprint = TrimEx( objScriptExec.StdOut.ReadAll )
end function

'Gets the path to a private key store file
function GetKeyStoreFile(storename, storelocation, certname )
    dim thumbprint , filename
    thumbprint = GetCertificateThumbprint( storename , storelocation , certname )
    GetKeyStoreFile = GetKeyStoreFileFromThumbprint(  storename , storelocation, thumbprint )
end function

'opens a certificate store
function OpenStore( storename, storelocation  )
    on error resume next 
    dim whichstore, store 
    if strcomp( storelocation, "localMachine", vbTextCompare )  then 
        whichstore = CAPICOM_CURRENT_USER_STORE
    else
        whichstore = CAPICOM_LOCAL_MACHINE_STORE
    end if 
    
    set store = CreateObject( "CAPICOM.store" )

    store.Open whichstore, storename, CAPICOM_STORE_OPEN_MAXIMUM_ALLOWED OR CAPICOM_STORE_OPEN_EXISTING_ONLY    
    if err.number<>0  then
       logerror "[ERROR] OpenStore( " & storename & ", " & storelocation& "): unable to find open store"
       set OpenStore = Nothing
       exit function
    end if 
    set OpenStore = store
end function

'Gets the Thumbprint of a certificate using the CN name 
function GetCertificateThumbprint( storename, storelocation ,certname )
    on error resume next
    Dim store, certificate, subj, comm     
    set store = OpenStore( storename , storelocation )
    
    for each certificate in store.Certificates
        subj = certificate.SubjectName
        comm = ucase(split(subj,",")(0))
 
        if comm = "CN=" & ucase(certname) then
            GetCertificateThumbprint  = certificate.thumbprint
            store.close
            exit Function
        end if
    next
    store.close
    logerror "[ERROR] GetCertificateThumbprint( " & storename & ", " & storelocation& " ," & certname& " ): unable to find matching certificate"
end function

sub DeleteCertificate( storename, storelocation ,certname )
    on error resume next
    Dim certificate, store, subj, comm 
    set store = OpenStore( storename , storelocation )
    
    for each certificate in store.Certificates
        subj = certificate.SubjectName
        comm = ucase(split(subj,",")(0))
     
        if comm = "CN=" & ucase(certname) then
            log "Processing Certificate: " & certname
            
            if certificate.HasPrivateKey then
                log "   - private key removed"
                certificate.PrivateKey.Delete
            end if
            store.Remove certificate
            log "   - certificate removed"
            store.close
            exit sub
        end if
    next
    store.close
    logwarning "[WARNING] deletecert( " & storename & ", " & storelocation& " ," & certname& " ): unable to find matching certificate"
end sub

' installs a certificate into the store
sub InstallCertificate(storename, storelocation ,filename)
    dim store 
    set store = OpenStore( storeName, storelocation )
    store.Load filename 
    store.Close
    logverbose "[NOTE] Installed " & filename & " into " & storename & ":" & storelocation
end sub

Sub CreateVirtualWebDir(DirName,DirPath)
	Dim webSite, vRoot, vDir
	On Error Resume Next
	
	set webSite = findWeb("localhost", "Default Web Site")
	if IsObject(webSite) then
		set vRoot = webSite.GetObject("IIsWebVirtualDir", "Root")
		
		If (Err <> 0) Then
			logerror "Unable to access root for " & webSite.ADsPath
            finished
		Else
            'Create the new virtual directory
            Set vDir = vRoot.Create("IIsWebVirtualDir",DirName)
            If (Err <> 0) Then
                logerror "Unable to create " & vRoot.ADsPath & "/" & DirName &"."
                finished
            Else
                'Set the new virtual directory path
                vDir.AccessRead = true
                vDir.AuthFlags = 1
                vDir.Path = DirPath
                vDir.AppCreate True
                vDir.AccessFlags = 5
                If (Err <> 0) Then
                    logerror "Unable to bind path " & DirPath & " to " & vRoot.ADsPath & "/" & DirName & ". Path may be invalid."
                    finished
                Else
                    'Save the changes
                    vDir.SetInfo
                    If (Err <> 0) Then
                        logerror "Unable to save configuration for " &  vRoot.ADsPath &"/" & DirName &"."
                        finished
                    Else
                        log "Web virtual directory " & vRoot.ADsPath & "/" & DirName & " created successfully."
                    End If
                End If
            End If
            Err = 0
		End If
	else
		logerror "Unable to find 'Default Web Site' on localhost"
        finished
	End if
End Sub

Sub DeleteVirtualWebDir(DirName)
    Dim webSite, vRoot
	On Error Resume Next

    set webSite = findWeb("localhost", "Default Web Site")
	if IsObject(webSite) then
		set vRoot = webSite.GetObject("IIsWebVirtualDir", "Root")
		
		If (Err <> 0) Then
			logerror "Unable to access root for " & webSite.ADsPath
            finished
		Else
            vRoot.Delete "IIsWebVirtualDir",DirName
            if Err.number <> 0 then 
                log "Directory " & DirName & " not found, skipping delete"
                else
                log "Deleted directory "& DirName
            end if
            err.clear
    	End If
	else
		logerror "Unable to find 'Default Web Site' on localhost"
        finished
	End if
End Sub

function getBinding(bindstr)

	Dim one, two, ia, ip, hn
	
	one=Instr(bindstr,":")
	two=Instr((one+1),bindstr,":")
	
	ia=Mid(bindstr,1,(one-1))
	ip=Mid(bindstr,(one+1),((two-one)-1))
	hn=Mid(bindstr,(two+1))
	
	getBinding=Array(ia,ip,hn)
end function

Function findWeb(computer, webname)
	On Error Resume Next

	Dim websvc, site
	dim webinfo
	Dim aBinding, binding

	set websvc = GetObject("IIS://"&computer&"/W3svc")
	if (Err <> 0) then
		exit function
	end if
	' First try to open the webname.
	set site = websvc.GetObject("IIsWebServer", webname)
	if (Err = 0) and (not isNull(site)) then
		if (site.class = "IIsWebServer") then
			' Here we found a site that is a web server.
			set findWeb = site
			exit function
		end if
	end if
	err.clear
	for each site in websvc
		if site.class = "IIsWebServer" then
			'
			' First, check to see if the ServerComment
			' matches
			'
			If site.ServerComment = webname Then
				set findWeb = site
				exit function
			End If
			aBinding=site.ServerBindings
			if (IsArray(aBinding)) then
				if aBinding(0) = "" then
					binding = Null
				else
					binding = getBinding(aBinding(0))
				end if
			else 
				if aBinding = "" then
					binding = Null
				else
					binding = getBinding(aBinding)
				end if
			end if
			if IsArray(binding) then
				if (binding(2) = webname) or (binding(0) = webname) then
					set findWeb = site
					exit function
				End If
			end if 
		end if
	next
End Function

function toBinaryArray(txt)
    dim dom, element
    Set dom = CreateObject("Microsoft.XMLDOM") 
    Set element = dom.createElement("tmp") 
    element.DataType = "bin.hex" 
    element.Text = txt
    toBinaryArray = element.NodeTypedValue
end function

sub SetSSLSite( certificatename )
    SetMetabaseProperty "SSLCertHash" , Array( toBinaryArray(  GetCertificateThumbprint ( "My" , "localmachine", certificatename ) ) )
    SetMetabaseProperty "SSLStoreName" , "MY"
    SetMetabaseProperty "securebindings" , Array( ":443:" )
end sub

sub ClearSSLSite( )
    ClearMetabaseProperty "SSLCertHash" 
    ClearMetabaseProperty "SSLStoreName" 
    ClearMetabaseProperty "securebindings" 
end sub

Sub SetMetabaseProperty(propname , value )
    Dim webSite, vRoot 

    set webSite = findWeb("localhost", "Default Web Site")
	if IsObject(webSite) then
		set vRoot = webSite.GetObject("IIsWebVirtualDir", "Root")
        webSite.Put propname, value
        webSite.setInfo
		If (Err <> 0) Then
			logerror "Error" & err.description
            finished
		end if
    else
		logerror "Unable to find 'Default Web Site' on localhost"
        finished
	End if
End Sub

Sub ClearMetabaseProperty(propname)
    Dim webSite, vRoot 

    set webSite = findWeb("localhost", "Default Web Site")
	if IsObject(webSite) then
		set vRoot = webSite.GetObject("IIsWebVirtualDir", "Root")
        webSite.PutEx 1 , propname, nothing
        webSite.setInfo
		If (Err <> 0) Then
			logerror "Error" & err.description
            finished
		end if
    else
		logerror "Unable to find 'Default Web Site' on localhost"
        finished
	End if
End Sub


