========================================================================
    WIN32 APPLICATION : CppWin7TaskbarOverlayIcons Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Windows 7 Taskbar introduces Overlay Icons, which makes your application can 
provide contextual status information to the user even if the application's 
window is not shown.  The user doesn't even have to look at the thumbnail 
or the live preview of your app ¨C the taskbar button itself can reveal 
whether you have any interesting status updates.
 
CppWin7TaskbarOverlayIcons example demostrates how to initialize Windows 7 
Taskbar list instance, set and clear Taskbar Overlay Icons using 
ITaskbarList3 related APIs.  .  


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

Microsoft Windows SDK for Windows 7 and .NET Framework 3.5 SP1
http://www.microsoft.com/downloads/details.aspx?FamilyID=c17ba869-9671-4330-a63e-1fd44e0e2505&displaylang=en

Windows 7
http://www.microsoft.com/windows/windows-7/


/////////////////////////////////////////////////////////////////////////////
Creation:

Step 1: Create Main dialog and following the steps in example 
CppWindowsDialog ReadMe.txt.  

Step 2: Set the Main dialog "Application Window" property to True in order 
to let the main dialog has an entry on the taskbar when visible.

Step 3: In targetver.h, comments the original version information and add the
following codes to specify the minimum required platform is Windows 7.

#ifndef NTDDI_VERSION             
#define NTDDI_VERSION NTDDI_WIN7  
#endif

Step 4: In stdafx.h, include necessary header files.

// Windows Header Files:
#include <windowsx.h>

// ATL/WTL
#include <atlbase.h>
#include <atlstr.h>
#include <atltypes.h>

// Header Files for Windows 7 Taskbar features
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>
#include <shlobj.h>
#include <shellapi.h>

Step 5: Open the project's properties from the "project" menu by selecting 
properties. Select "linker - Input" from the tree control on the 
left of your project properties dialog.  Add "shlwapi.lib" in the 
"Additional Dependencies".

Step 6: In InitInstance, add the following codes to set find the ComboBox 
window handle and add items into the ComboBox.

   // Find the ComboBox window handle
   hCombo = FindWindowEx(hWnd, NULL, L"ComboBox", NULL);

   if (hCombo)
   {
	   // Add items into the ComboBox
	   SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Available");
	   SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Away");
	   SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Offline");
   }
   else
   {
	   return FALSE;
   }
   
Step 7: In InitInstance, add the following codes to initialize the Windows 7
Taskbar list instance.

   HRESULT hr;

   // Initialize the Windows 7 Taskbar list instance
   hr = CoCreateInstance(CLSID_TaskbarList, 
	   NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pTaskbarList));

   if (!SUCCEEDED(hr))
   {
	   return FALSE;
   }
   
Step 8: Create function SetOverlayIcons to load an icon and set the icon as 
Taskbar Overlay Icon. For detail, please see .

Step 9: Create function SetOverlayIconsByComboSelectedValue to get the 
selected index of the ComboBox and set the Overlay Icons.

Step 10: Modify the WM_COMMAND message codes to handle ComboBox selected and 
CheckBox clicked messages.


/////////////////////////////////////////////////////////////////////////////
References:

ITaskbarList3 Interface
http://msdn.microsoft.com/en-us/library/dd391692(VS.85).aspx

ITaskbarList3::SetOverlayIcon Method
http://msdn.microsoft.com/en-us/library/dd391696(VS.85).aspx

Combo Box
http://msdn.microsoft.com/en-us/library/bb775792(VS.85).aspx

CBN_SELENDOK Notification
http://msdn.microsoft.com/en-us/library/bb775825(VS.85).aspx

CB_ADDSTRING Message
http://msdn.microsoft.com/en-us/library/bb775828(VS.85).aspx

CB_GETCURSEL Message
http://msdn.microsoft.com/en-us/library/bb775845(VS.85).aspx

CoCreateInstance Function
http://msdn.microsoft.com/en-us/library/ms686615(VS.85).aspx

SendMessage Function
http://msdn.microsoft.com/en-us/library/ms644950(VS.85).aspx

FindWindowEx Function
http://msdn.microsoft.com/en-us/library/ms633500(VS.85).aspx

LoadIcon Function
http://msdn.microsoft.com/en-us/library/ms648072(VS.85).aspx

Windows 7 Taskbar Dynamic Overlay Icons and Progress Bars
http://windowsteamblog.com/blogs/developers/archive/2009/07/28/windows-7-taskbar-dynamic-overlay-icons-and-progress-bars.aspx


/////////////////////////////////////////////////////////////////////////////