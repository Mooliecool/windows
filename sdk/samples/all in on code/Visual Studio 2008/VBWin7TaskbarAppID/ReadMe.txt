========================================================================
    WINDOWS FORMS APPLICATION : VBWin7TaskbarAppID Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Application User Model IDs (AppUserModelIDs) are used extensively by the 
taskbar in Windows 7 and later systems to associate processes, files, and 
windows with a particular application. In some cases, it is sufficient to 
rely on the internal AppUserModelID assigned to a process by the system. 
However, an application that owns multiple processes or an application that 
is running in a host process might need to explicitly identify itself so 
that it can group its otherwise disparate windows under a single taskbar 
button and control the contents of that application's Jump List.
 
VBWin7TaskbarAppID example demostrates how to set process level Application
User Model IDs (AppUserModelIDs or AppIDs) and modify the AppIDs for a 
specific window using Taskbar related APIs in Windows API Code Pack.
 
 
/////////////////////////////////////////////////////////////////////////////
Creation:

1. In form load event, check the Windows version.  If the current system is 
   Windows 7 or Windows Server 2008 R2, set the process level AppID.  
   Otherwise exit the process.
   (TaskbarManager.Instance.ApplicationID, TaskbarManager.IsPlatformSupported, 
   Windows API SetCurrentProcessExplicitAppUserModelID, Application.Exit)
   
2. In open SubForm button, create a SubForm instance, add it into a List, and
   set specific AppID for the SubForm.
   (TaskbarManager.Instance.SetApplicationIdForSpecificWindow, Windows API
   SHGetPropertyStoreForWindow)
   
3. In set button, set all the SubForms a specific AppID.
   (TaskbarManager.Instance.SetApplicationIdForSpecificWindow, Windows API
   SHGetPropertyStoreForWindow)
   
4. In reset button, reset all the SubForms to the MainForm's AppID.
   (TaskbarManager.Instance.SetApplicationIdForSpecificWindow, Windows API
   SHGetPropertyStoreForWindow)


/////////////////////////////////////////////////////////////////////////////
References:

Application User Model IDs (AppUserModelIDs)
http://msdn.microsoft.com/en-us/library/dd378459(VS.85).aspx

SetCurrentProcessExplicitAppUserModelID Function
http://msdn.microsoft.com/en-us/library/dd378422(VS.85).aspx

SHGetPropertyStoreForWindow Function
http://msdn.microsoft.com/en-us/library/dd378430(VS.85).aspx

Windows® API Code Pack for Microsoft® .NET Framework
http://code.msdn.microsoft.com/WindowsAPICodePack

Windows 7 Training Kit For Developers
http://www.microsoft.com/downloads/details.aspx?familyid=1C333F06-FADB-4D93-9C80-402621C600E7&displaylang=en

Windows 7 Taskbar: Application Id
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/02/15/windows-7-taskbar-application-id.aspx

Developing for the Windows 7 Taskbar – Application ID
http://windowsteamblog.com/blogs/developers/archive/2009/06/18/developing-for-the-windows-7-taskbar-application-id.aspx

Developing for the Windows 7 Taskbar – Application ID – Part 2
http://windowsteamblog.com/blogs/developers/archive/2009/06/19/developing-for-the-windows-7-taskbar-application-id-part-2.aspx

/////////////////////////////////////////////////////////////////////////////