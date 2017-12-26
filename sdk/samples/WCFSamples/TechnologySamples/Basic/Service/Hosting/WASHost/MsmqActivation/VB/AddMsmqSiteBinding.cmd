%windir%\system32\inetsrv\appcmd.exe set site "Default Web Site" -+bindings.[protocol='net.msmq',bindingInformation='localhost']
%windir%\system32\inetsrv\appcmd.exe set app "Default Web Site/servicemodelsamples" /enabledProtocols:http,net.msmq
