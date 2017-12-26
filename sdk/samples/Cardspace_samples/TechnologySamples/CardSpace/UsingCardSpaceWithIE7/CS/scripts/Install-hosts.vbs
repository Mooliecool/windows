'use common library
ExecuteGlobal CreateObject("Scripting.FileSystemObject").OpenTextFile( left( WScript.ScriptFullName , len (WScript.ScriptFullName) -len( WScript.ScriptName ) ) & "\vb-script-library.vbs",1).ReadAll()
InitScript("Hosts file installation")

REM GLOBAL SCRIPT OBJECTS

REM Set error handler to continue.
on error resume next 
hostsfilename = wso.ExpandEnvironmentStrings("%windir%") & "\System32\drivers\etc\hosts"
newhostsfilename = hostsfilename & ".new"
backuphostsfilename = hostsfilename & "."&month(date)&"-"&day(date)&"-"&year(date)&"-" & hour(time)& minute(time)&".bak"

REM verify the hosts file exists.
if not fso.FileExists(hostsfilename) then
        logerror "unable to locate the hosts file at ("& hostsfilename &")"
        finished
end if        

fso.CopyFile hostsfilename, backuphostsfilename
if err.number <> 0 or NOT fso.FileExists(backuphostsfilename) then 
        logerror "unable to copy the existing hosts ("& hostsfilename &") file to ("& backuphostsfilename &")"
        finished
else
    log "copied the original hosts ("& hostsfilename &") file to ("& backuphostsfilename &")"
end if



Set hostsfile = fso.OpenTextFile(backuphostsfilename,ForReading)
if err.number <> 0 then 
        logerror "unable to open the hosts file at ("& backuphostsfilename &")"
        finished
end if

Set newhostsfile = fso.OpenTextFile(hostsfilename,ForWriting)
if err.number <> 0 then 
        logerror "unable to open the new hosts file for writing at ("& hostsfilename &")"
        finished
end if

While Not hostsfile.AtEndOfStream
    text = hostsfile.ReadLine
    if InStr( 1, text , "fabrikam"  , 1 ) OR InStr( 1, text , "contoso"  , 1 ) OR InStr( 1, text , "adatum"  , 1 )OR InStr( 1, text , "woodgrovebank"  , 1 )  then
        ' line already exists, drop it.
        log "Dropping the line ("& text &") from the hosts file."
    else
        newhostsfile.WriteLine text
    end if 
Wend

newhostsfile.WriteLine "127.0.0.1       www.adatum.com" 
newhostsfile.WriteLine "127.0.0.1       adatum.com" 
newhostsfile.WriteLine "127.0.0.1       www.contoso.com" 
newhostsfile.WriteLine "127.0.0.1       contoso.com" 
newhostsfile.WriteLine "127.0.0.1       www.fabrikam.com" 
newhostsfile.WriteLine "127.0.0.1       fabrikam.com" 
newhostsfile.WriteLine "127.0.0.1       www.woodgrovebank.com" 
newhostsfile.WriteLine "127.0.0.1       woodgrovebank.com" 
log "Adding sample lines"

hostsfile.Close
newhostsfile.Close

REM Add lines to proxy ignore in the registry.
REM and try to detect if the user has a proxy.
dim proxybypass
dim proxyenable
dim iesettings

proxybypass = wso.RegRead( "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyOverride")

if err.number<>0  then
    proxybypass = ""
end if 
proxybypass = AddToPath( proxybypass , "fabrikam.com" )
proxybypass = AddToPath( proxybypass , "contoso.com" )
proxybypass = AddToPath( proxybypass , "adatum.com" )
proxybypass = AddToPath( proxybypass , "woodgrovebank.com" )

proxybypass = AddToPath( proxybypass , "www.fabrikam.com" )
proxybypass = AddToPath( proxybypass , "www.contoso.com" )
proxybypass = AddToPath( proxybypass , "www.adatum.com" )
proxybypass = AddToPath( proxybypass , "www.woodgrovebank.com" )
proxybypass = AddToPath( proxybypass , "<local>" )

wso.RegWrite "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyOverride", proxybypass

proxyenable = wso.RegRead( "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyEnable")

if proxyenable = 0 then
    proxyautohurl = wso.RegRead( "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\AutoConfigURL")
    if( proxyautohurl <> "" ) then
        if wso.popup( "It appears that you are using an automatic proxy detection url:" &vbcrlf&vbcrlf& "    " & proxyautohurl & vbcrlf&vbcrlf& "You may need to disable auto detection,and set the Internet Explorer Proxy manually in order for the sample websites to work." &vbcrlf&vbcrlf& "Do you wish to access the Internet Explorer Settings Now?" , , "Note" , 4 ) =6 then 
            wso.Exec("RunDll32.exe shell32.dll,Control_RunDLL inetcpl.cpl,,4" )
        end if
    else
        iesettings = wso.RegRead( "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\Connections\DefaultConnectionSettings")
        if( iesettings(8) AND 8  ) = 8  then
            if wso.popup( "It appears that you are using automatic proxy detection." & vbcrlf & "If you do use a proxy, you may need to disable auto detection,and set the Internet Explorer Proxy manually in order for the sample websites to work." &vbcrlf&vbcrlf& "If you do not use a proxy, you can ignore this message." &vbcrlf&vbcrlf& "Do you wish to access the Internet Explorer Settings Now?" , , "Note" , 4 ) = 6 then 
                wso.Exec("RunDll32.exe shell32.dll,Control_RunDLL inetcpl.cpl,,4" )
            end if
        end if
    end if
end if

finished

