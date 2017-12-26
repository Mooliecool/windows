========================================================================
    WIN32 APPLICATION : CppWin7TaskbarAppID Project Overview
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
 
CppWin7TaskbarAppID example demostrates how to set process level Application
User Model IDs (AppUserModelIDs or AppIDs) and modify the AppIDs for a 
specific window using native C++.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

Microsoft Windows SDK for Windows 7 and .NET Framework 3.5 SP1
http://www.microsoft.com/downloads/details.aspx?FamilyID=c17ba869-9671-4330-a63e-1fd44e0e2505&displaylang=en

Windows 7
http://www.microsoft.com/windows/windows-7/


/////////////////////////////////////////////////////////////////////////////
Creation:

Step 1: Create Main dialog and Sub Modeless dialog following the steps in 
example CppWindowsDialog ReadMe.txt.  

Step 2: Named the Sub Modeless dialog as IDD_SUBDIALOG.  Open its properties
window, set "Application Window" property to True in order to let the Sub 
dialog has an entry on the taskbar when visible.

Step 3: In targetver.h, comments the original version information and add the
following codes to specify the minimum required platform is Windows 7.

#ifndef NTDDI_VERSION             
#define NTDDI_VERSION NTDDI_WIN7  
#endif

Step 4: In stdafx.h, include necessary header files.

// Windows Header Files:
#include <windowsx.h>

// Header Files for Windows 7 Taskbar features
#include <shobjidl.h>
#include <propkey.h>
#include <propvarutil.h>
#include <shlobj.h>
#include <shellapi.h>

Step 5: Open the project's properties from the "project" menu by selecting 
properties. Select "linker - Input" from the tree control on the 
left of your project properties dialog.  Add "shlwapi.lib" in the 
"Additional Dependencies" and "shlwapi.dll;" in the "Delay Loaded DLLs".

Step 6: In InitInstance, add the following codes to set process-level AppID

   // Set process-level AppID
   HRESULT hr = SetCurrentProcessExplicitAppUserModelID(c_rgszAppID[0]);

   if (!SUCCEEDED(hr))
   {
	   return FALSE;
   }

   // Set Window caption
   if (!SetWindowText(hWnd, c_rgszAppID[0]))
   {
	   return FALSE;
   }
   
Step 7: Create function SetAppIDForSpecificWindow to set AppID for a specific
window.  For detail, please see the Developing for the Windows 7 Taskbar - 
Application ID in References section.

Step 8: Modify the function OnCommand to create Sub Modeless dialog, set a 
specific AppID for the Sub dialog, modify Main dialog AppID.  


/////////////////////////////////////////////////////////////////////////////
References:

Application User Model IDs (AppUserModelIDs)
http://msdn.microsoft.com/en-us/library/dd378459(VS.85).aspx

SetCurrentProcessExplicitAppUserModelID Function
http://msdn.microsoft.com/en-us/library/dd378422(VS.85).aspx

SHGetPropertyStoreForWindow Function
http://msdn.microsoft.com/en-us/library/dd378430(VS.85).aspx

SetWindowText Function
http://msdn.microsoft.com/en-us/library/ms633546(VS.85).aspx

InitPropVariantFromString Function
http://msdn.microsoft.com/en-us/library/bb762305(VS.85).aspx

IPropertyStore Interface
http://msdn.microsoft.com/en-us/library/bb761474(VS.85).aspx

PropVariantInit Macro
http://msdn.microsoft.com/en-us/library/aa380293(VS.85).aspx

PropVariantClear Function
http://msdn.microsoft.com/en-us/library/aa380073(VS.85).aspx

Windows 7 Taskbar: Application Id
http://blogs.microsoft.co.il/blogs/sasha/archive/2009/02/15/windows-7-taskbar-application-id.aspx

Developing for the Windows 7 Taskbar ¨C Application ID
http://windowsteamblog.com/blogs/developers/archive/2009/06/18/developing-for-the-windows-7-taskbar-application-id.aspx

Developing for the Windows 7 Taskbar ¨C Application ID ¨C Part 2
http://windowsteamblog.com/blogs/developers/archive/2009/06/19/developing-for-the-windows-7-taskbar-application-id-part-2.aspx


/////////////////////////////////////////////////////////////////////////////
