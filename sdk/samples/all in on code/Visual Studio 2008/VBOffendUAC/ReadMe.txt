========================================================================
    WINFORM APPLICATION : VBOffendUAC Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

VBOffendUAC demonstrates various tasks requiring administrative privilege.  
It writes to file system and registry, writes to .ini files in protected 
locations, restarts services, explicitly checks for admin rights, etc. 


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Checking for Administative Privilege at Startup

In MainForm_Load, PInvoke the IsUserAnAdmin API to determine if the 
application is currently run as administrator. 

2. File Operations

Add buttons and textboxes to the main form to demonstrate creating, deleting 
directories and writing to and reading from files that are located in the UAC 
protected locations.

3. Registry Operations

Add buttons and combo boxes to the main form to demonstate creating, opening 
and deleting registry key. 

4. INI File Operatins

The INI file format is a de facto standard for configuration files. The use 
of the "INI file" has been deprecated in Windows and corresponding data 
should be placed in the registry. WritePrivateProfileString, 
GetPrivateProfileString, WriteProfileString and GetProfileString manipulate 
.ini files in C:\Windows directory, which is UAC protected. The Write 
operations require administrative privilege.

5. Token

Call IsUserAnAdmin to explicitly checks for admin rights.

6. Privilege

Enable or disable privileges in the current process token by calling 
AdjustTokenPrivileges.

7. Other Operations

Restarting a Windows Service by using the .NET ServiceController class 
requires administative privilege.


/////////////////////////////////////////////////////////////////////////////
References:

"LUA Bug" demo app
http://blogs.msdn.com/aaron_margosis/archive/2008/11/07/lua-bug-demo-app.aspx

Stock Viewer Shim Demo Application 
http://blogs.msdn.com/cjacks/archive/2008/01/03/stock-viewer-shim-demo-application.aspx


/////////////////////////////////////////////////////////////////////////////