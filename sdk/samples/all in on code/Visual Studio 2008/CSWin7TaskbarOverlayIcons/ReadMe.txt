========================================================================
 WINDOWS FORMS APPLICATION : CSWin7TaskbarOverlayIcons Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Windows 7 Taskbar introduces Overlay Icons, which makes your application can 
provide contextual status information to the user even if the application’s 
window is not shown.  The user doesn’t even have to look at the thumbnail 
or the live preview of your app – the taskbar button itself can reveal 
whether you have any interesting status updates..
 
CSWin7TaskbarOverlayIcons example demostrates how to set and clear Taskbar
Overlay Icons using Taskbar related APIs in Windows API Code Pack.
 
 
/////////////////////////////////////////////////////////////////////////////
Creation:

1. In form load event, check the Windows version.  If the current system is 
   not Windows 7 or Windows Server 2008 R2,exit the process.
   (TaskbarManager.IsPlatformSupported, Application.Exit)
   
2. Create method ShowOrHideOverlayIcon to Show, hide and modify Taskbar 
   button Overlay Icons.
   (TaskbarManager.Instance.SetOverlayIcon)


/////////////////////////////////////////////////////////////////////////////
References:

Windows® API Code Pack for Microsoft® .NET Framework
http://code.msdn.microsoft.com/WindowsAPICodePack

Welcome to the Windows 7 Taskbar
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/01/25/welcome-to-the-windows-7-taskbar.aspx

Windows 7 Taskbar: Overlay Icons and Progress Bars
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/02/16/windows-7-taskbar-overlay-icons-and-progress-bars.aspx

Windows 7 Training Kit For Developers
http://www.microsoft.com/downloads/details.aspx?familyid=1C333F06-FADB-4D93-9C80-402621C600E7&displaylang=en

Windows 7 Taskbar Dynamic Overlay Icons and Progress Bars
http://windowsteamblog.com/blogs/developers/archive/2009/07/28/windows-7-taskbar-dynamic-overlay-icons-and-progress-bars.aspx

/////////////////////////////////////////////////////////////////////////////