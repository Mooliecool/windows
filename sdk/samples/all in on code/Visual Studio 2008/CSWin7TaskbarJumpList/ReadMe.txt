========================================================================
  WINDOWS FORMS APPLICATION : CSWin7TaskbarJumpList Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Jump List feature is designed to provide you with quick access to the 
documents and tasks associated with your applications. You can think of 
Jump Lists like little application-specific Start menus. Jump Lists can be 
found on the application icons that appear on the Taskbar when an 
application is running or on the Start menu in the recently opened programs
section. Jump Lists can also be found on the icons of applications that 
have been specifically pinned to the Taskbar or the Start menu.
 
CSWin7TaskbarJumpList example demostrates how to set register Jump List
file handle, add items into Recent/Frequent known categories, add/remove
user tasks, and add items/links into custom categories in Windows 7 Taskbar
Jump List using Taskbar related APIs in Windows API Code Pack.  
 
 
/////////////////////////////////////////////////////////////////////////////
Creation:

1. In the form load event, check the Windows version.  if it is Windows 7 or 
   Windows Server 2008 R2, set the AppID and update the UI, otherwise exit 
   the process.
   (TaskbarManager.IsPlatformSupported, TaskbarManager.Instance.ApplicationId,
   Application.Exit)

2. Create static HelperMethods class and the static helper methods to handle
   Admin session check, restart application to elevate the user session, 
   register/unregister application ID and file handle, validate file name, 
   and create files under system temp folder.
   
3. Create register/unregister file handle button events handler to register/
   unregister application ID and file handle.
   
4. Create RadioButton checked event handler to modify the known category to 
   display in the Jump List.
   (JumpList.KnownCategoryToDisplay) 
   
5. Create OpenFileDialog to open .txt file, so that the file will be
   displayed in Recent/Frequent known category.
   
6. Create addTaskButton and clearTaskButton click event handler to add/remove 
   user tasks (notepad.exe, mspaint.exe, and calc.exe) into the Jump List.
   (JumpList.AddUserTasks)
   
7. Create event handlers to create and add custom category, shell item, and 
   shell link. 
   (JumpList.AddCustomCategories, JumpListCustomCategory.AddJumpListItems)


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

Developing for the Windows 7 Taskbar – Jump into Jump Lists – Part 1
http://windowsteamblog.com/blogs/developers/archive/2009/06/22/developing-for-the-windows-7-taskbar-jump-into-jump-lists-part-1.aspx

Developing for the Windows 7 Taskbar – Jump into Jump Lists – Part 2
http://windowsteamblog.com/blogs/developers/archive/2009/06/25/developing-for-the-windows-7-taskbar-jump-into-jump-lists-part-2.aspx

Developing for the Windows 7 Taskbar – Jump into Jump Lists – Part 3
http://windowsteamblog.com/blogs/developers/archive/2009/07/02/developing-for-the-windows-7-taskbar-jump-into-jump-lists-part-3.aspx

/////////////////////////////////////////////////////////////////////////////