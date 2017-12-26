========================================================================
 WINDOWS FORMS APPLICATION : VBWin7TaskbarProgressBar Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Windows 7 Taskbar introduces Taskbar Progress Bar, which makes your 
application can provide contextual status information to the user even if 
the application’s window is not shown.  The user doesn’t even have to 
look at the thumbnail or the live preview of your app – the taskbar button 
itself can reveal whether you have any interesting status updates..
 
VBWin7TaskbarProgressBar example demostrates how to set Taskbar Progress
Bar state (normal, pause, indeterminate, error) and value, and flash window.
 
 
/////////////////////////////////////////////////////////////////////////////
Creation:

1. In form load event, check the Windows version.  If the current system is 
   not Windows 7 or Windows Server 2008 R2,exit the process.
   (TaskbarManager.IsPlatformSupported, Application.Exit)
   
2. Create method UpdateProgress to update WinForm ProgressBar value, set 
   Taskbar button Progress Bar state and value.
   (TaskbarManager.Instance.SetProgressState, 
   TaskbarManager.Instance.SetProgressValue)
   
3. Create Start, Pause, Indeterminate, Error buttons to set the Taskbar
   button Progress Bar state.
   (TaskbarProgressBarState.Normal, TaskbarProgressBarState.Paused, 
    TaskbarProgressBarState.Indeterminate,  TaskbarProgressBarState.Error)
    
4. P/Invoke the Windows API FlashWindow to make the Taskbar button Progress
   Bar flash several times.
   (FlashWindow)


/////////////////////////////////////////////////////////////////////////////
References:

Windows® API Code Pack for Microsoft® .NET Framework
http://code.msdn.microsoft.com/WindowsAPICodePack

FlashWindow Function
http://msdn.microsoft.com/en-us/library/ms679346(VS.85).aspx

Welcome to the Windows 7 Taskbar
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/01/25/welcome-to-the-windows-7-taskbar.aspx

Windows 7 Taskbar: Overlay Icons and Progress Bars
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/02/16/windows-7-taskbar-overlay-icons-and-progress-bars.aspx

Windows 7 Training Kit For Developers
http://www.microsoft.com/downloads/details.aspx?familyid=1C333F06-FADB-4D93-9C80-402621C600E7&displaylang=en

Windows 7 Taskbar Dynamic Overlay Icons and Progress Bars
http://windowsteamblog.com/blogs/developers/archive/2009/07/28/windows-7-taskbar-dynamic-overlay-icons-and-progress-bars.aspx

/////////////////////////////////////////////////////////////////////////////