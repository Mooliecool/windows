@echo off
iisreset /stop
mkdir %SystemDrive%\inetpub\wwwroot\XAMLActivation
mkdir %SystemDrive%\inetpub\wwwroot\XAMLActivation\bin
copy bin\service.dll /y %SystemDrive%\inetpub\wwwroot\XAMLActivation\bin\
copy Web.config /y %SystemDrive%\inetpub\wwwroot\XAMLActivation\
copy service.xamlx /y %SystemDrive%\inetpub\wwwroot\XAMLActivation\

VER | FINDSTR /c:" 6." >NUL
IF NOT ERRORLEVEL 1 (
    IF EXIST %windir%\system32\inetsrv\appcmd.exe (
        %windir%\system32\inetsrv\appcmd.exe add app /site.name:"Default Web Site" /path:/XAMLActivation /physicalPath:%systemdrive%\inetpub\wwwroot\XAMLActivation
	%windir%\system32\inetsrv\appcmd.exe set apppool "DefaultAppPool" /managedRuntimeVersion:v4.0
    ) ELSE (
        Echo "Could not find %windir%\system32\inetsrv\appcmd.exe.  Please ensure IIS is installed.  See 'Internet Information Service (IIS) Hosting Instructions' in the WCF samples Setup Instructions."
    )
) ELSE (
    IF EXIST %SystemDrive%\inetpub\adminscripts\adsutil.vbs (
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs CREATE w3svc/1/root/XAMLActivation "IIsWebVirtualDir"
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs SET w3svc/1/root/XAMLActivation/Path %SystemDrive%\inetpub\wwwroot\XAMLActivation
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs SET w3svc/1/root/XAMLActivation/AppRoot "w3svc/1/Root/XAMLActivation"
        cscript.exe %SystemDrive%\inetpub\adminscripts\adsutil.vbs APPCREATEPOOLPROC w3svc/1/root/XAMLActivation
    ) ELSE (
        Echo "Could not find %SystemDrive%\inetpub\adminscripts\adsutil.vbs.  Please ensure IIS is installed.  See 'Internet Information Service (IIS) Hosting Instructions' in the WCF samples Setup Instructions."
    )
    
)
iisreset /start


