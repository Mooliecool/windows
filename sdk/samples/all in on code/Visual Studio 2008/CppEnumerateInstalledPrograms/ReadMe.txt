=============================================================================
    CONSOLE APPLICATION : CppEnumerateInstalledPrograms Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

You need to enumerate these keys:
HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall
And
HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Uninstall
And the other-bitness versions of the same. (i.e., 32-bit version if you are 
a 64-bit app, or 64-bit version if you are a 32-bit app.)

http://www.codeproject.com/KB/system/installed_programs.aspx


Actually if you want to know what shows up in the Programs and Features CPL, you can just ask the Progarms and Features CPL to give you a list of its items. No need to try to emulate it.

var oShell = new ActiveXObject("Shell.Application");
var oFolder = oShell.NameSpace("::{21EC2020-3AEA-1069-A2DD-08002B30309D}\\::{7B81BE6A-CE2B-4676-A29E-EB907A5126C5}");
var oItems = oFolder.Items();
for (i = 0; i < oItems.Count; i++) {
 var oItem = oItems.Item(i);
 WScript.StdOut.WriteLine(oItem.Name);
}

How did I know the magic namespace path? I didn't. I asked the shell to tell me.

var oShell = new ActiveXObject("Shell.Application");
var oFolder = oShell.NameSpace(3); // 3 = control panel
var oItems = oFolder.Items();
for (i = 0; i < oItems.Count; i++) {
 var oItem = oItems.Item(i);
 if (oItem.Name == "Programs and Features") WScript.StdOut.WriteLine(oItem.Path);
}


/////////////////////////////////////////////////////////////////////////////
References:




/////////////////////////////////////////////////////////////////////////////