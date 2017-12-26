========================================================================
  WINDOWS FORMS APPLICATION : CSWin7TaskbarThumbnail Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Thumbnail toolbars are another productivity feature that gives us the 
ability to do more without actually switching to another application's 
window.  A thumbnail toolbar is essentially a mini remote-control that is 
displayed when you hover over the application's taskbar button, right 
beneath the application’s thumbnail. 
 
CSWin7TaskbarThumbnail example demostrates how to set Taskbar Thumbnail
previews, change Thumbnail previews order, set Thumbnail toolbars using 
Taskbar related APIs in Windows API Code Pack. .
 
 
/////////////////////////////////////////////////////////////////////////////
Creation:

Thumbnail Preview:

1. In form load event, check the Windows version.  If the current system is 
   not Windows 7 or Windows Server 2008 R2,exit the process.
   (TaskbarManager.IsPlatformSupported, Application.Exit)
   
2. Create add and remove Thumbnail previews buttons event handler to add and 
   remove the Thumbnial previews which are PictureBox images.
   (TaskbarManager.Instance.TabbedThumbnail.GetThumbnailPreview, 
   TaskbarManager.Instance.TabbedThumbnail.AddThumbnailPreview,
   TaskbarManager.Instance.TabbedThumbnail.SetActiveTab,
   TaskbarManager.Instance.TabbedThumbnail.RemoveThumbnailPreview)
   
3. Create change Thumbnail previews order button event handler to modify the
   Thumbnail previews order.
   (TaskbarManager.Instance.TabbedThumbnail.SetTabOrder)
   

Thumbnail Toolbar Buttons:

1. Create the TabControl selected index changed event handler to create 
   Thumbnail toolbar buttons, clear Thumbnail previews when the user 
   switches the tab to the Thumbnail Toolbar Button page, and make the 
   Thumbnail Toolbar buttons invisible when the user switches to the Thumbnail
   Preview page.
   (TaskbarManager.Instance.TabbedThumbnail.GetThumbnailPreview, 
   TaskbarManager.Instance.TabbedThumbnail.RemoveThumbnailPreview)
   
2. Create a enum type to represent the Thumbnail toolbar buttons.

3. Create a method CreateToolbarButtons to initialize the Thumbnail toolbar
   buttons. 
   (TaskbarManager.Instance.ThumbnailToolbars.AddButtons)
   
4. Create a method ChangeVisibility to change the certain Thumbnail button's 
   visible and enable status based on the current selected index in the 
   imageListView.
   
5. Create a method ShowList to update the selected index when the certain 
   Thumbnail toolbar button is clicked.
   
6. Create event handlers for Thumbnail toolbar buttons First, Previous, 
   Next, and Last.
   
7. Create imageListView selected index changed event handler to retrieve 
   images from Resource based on the selected index of the imageListView.


/////////////////////////////////////////////////////////////////////////////
References:

Windows® API Code Pack for Microsoft® .NET Framework
http://code.msdn.microsoft.com/WindowsAPICodePack

Inside Windows 7 Introducing The Taskbar APIs
http://msdn.microsoft.com/en-us/magazine/dd942846.aspx

Welcome to the Windows 7 Taskbar
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/01/25/welcome-to-the-windows-7-taskbar.aspx

Windows 7 Training Kit For Developers
http://www.microsoft.com/downloads/details.aspx?familyid=1C333F06-FADB-4D93-9C80-402621C600E7&displaylang=en

Windows 7 Taskbar: Thumbnail Toolbars
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/02/26/windows-7-taskbar-thumbnail-toolbars.aspx

Windows 7 Taskbar: Tabbed Thumbnails and Previews in Native Code
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/08/12/windows-7-taskbar-tabbed-thumbnails-and-previews-in-native-code.aspx

/////////////////////////////////////////////////////////////////////////////