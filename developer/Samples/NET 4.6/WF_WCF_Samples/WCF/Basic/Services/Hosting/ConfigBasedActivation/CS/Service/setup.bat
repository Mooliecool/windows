@echo off
iisreset /stop

VER | FINDSTR /c:" 6." >NUL
IF NOT ERRORLEVEL 1 (
    IF EXIST %windir%\system32\inetsrv\appcmd.exe (
        %windir%\system32\inetsrv\appcmd.exe add app /site.name:"Default Web Site" /path:/servicemodelsamples /physicalPath:%systemdrive%\inetpub\wwwroot\servicemodelsamples
        %windir%\system32\inetsrv\appcmd.exe set apppool "DefaultAppPool" /managedRuntimeVersion:v4.0
    ) ELSE (
        Echo "Could not find %windir%\system32\inetsrv\appcmd.exe.  Please ensure IIS is installed.  See 'Internet Information Service (IIS) Hosting Instructions' in the WCF samples Setup Instructions."
    )
) ELSE (
    IF EXIST %SystemDrive%\inetpub\adminscripts\adsutil.vbs (
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs CREATE w3svc/1/root/servicemodelsamples "IIsWebVirtualDir"
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs SET w3svc/1/root/servicemodelsamples/Path %SystemDrive%\inetpub\wwwroot\servicemodelsamples
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs SET w3svc/1/root/servicemodelsamples/AppRoot "w3svc/1/Root/servicemodelsamples"
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs APPCREATEPOOLPROC w3svc/1/root/servicemodelsamples
    ) ELSE (
        Echo "Could not find %SystemDrive%\inetpub\adminscripts\adsutil.vbs.  Please ensure IIS is installed.  See 'Internet Information Service (IIS) Hosting Instructions' in the WCF samples Setup Instructions."
    )
    
)
iisreset /start

