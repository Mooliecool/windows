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

hostsfile.Close
newhostsfile.Close

REM Remove lines from proxy ignore in the registry.
wso.RegWrite "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings\ProxyOverride", "<local>"

finished

