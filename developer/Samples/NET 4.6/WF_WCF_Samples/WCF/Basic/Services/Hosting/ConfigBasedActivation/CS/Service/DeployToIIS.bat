@echo off
iisreset /stop
mkdir %SystemDrive%\inetpub\wwwroot\ConfigBasedActivation\bin
VER | FINDSTR /c:" 6.0." >NUL
IF NOT ERRORLEVEL 1 (
    IF EXIST %windir%\system32\inetsrv\appcmd.exe (
        %windir%\system32\inetsrv\appcmd.exe add app /site.name:"Default Web Site" /path:/ConfigBasedActivation /physicalPath:%systemdrive%\inetpub\wwwroot\ConfigBasedActivation
    ) ELSE (
        Echo "Could not find %windir%\system32\inetsrv\appcmd.exe.  Please ensure IIS is installed.  See 'Internet Information Service (IIS) Hosting Instructions' in the WCF samples Setup Instructions."
    )
) ELSE (
    IF EXIST %SystemDrive%\inetpub\adminscripts\adsutil.vbs (
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs CREATE w3svc/1/root/ConfigBasedActivation "IIsWebVirtualDir"
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs SET w3svc/1/root/ConfigBasedActivation/Path %SystemDrive%\inetpub\wwwroot\ConfigBasedActivation
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs SET w3svc/1/root/ConfigBasedActivation/AppRoot "w3svc/1/Root/ConfigBasedActivation"
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs APPCREATEPOOLPROC w3svc/1/root/ConfigBasedActivation
    ) ELSE (
        Echo "Could not find %SystemDrive%\inetpub\adminscripts\adsutil.vbs.  Please ensure IIS is installed.  See 'Internet Information Service (IIS) Hosting Instructions' in the WCF samples Setup Instructions."
    )
    
)
iisreset /start
copy bin\service.dll /y %SystemDrive%\inetpub\wwwroot\ConfigBasedActivation\bin\
copy Web.config /y %SystemDrive%\inetpub\wwwroot\ConfigBasedActivation\

