=============================================================================
   ACTIVE TEMPLATE LIBRARY : ATLShellExtDragAndDropHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

ATLShellExtDragAndDropHandler demonstrates making Windows shell drag-and-drop 
handler in VC++/ATL. When a user right-clicks a Shell object to drag an 
object, a context menu is displayed when the user attempts to drop the object. 
A drag-and-drop handler is a context menu handler that can add items to this 
context menu.

FileDragAndDropExt is an example of drag-and-drop handler for file objects. 
After the setup of the handler, when you right-click any files to drag the 
files to a directory or the desktop, a context menu with "All-In-One Code 
Framework" menu item will be displayed. Clicking the menu item prompts a 
message box that shows the files being dragged and the target location that 
the files are dropped to.


/////////////////////////////////////////////////////////////////////////////
Build:

To build ATLShellExtDragAndDropHandler, please run Visual Studio as 
administrator because the component needs to be registered into HKCR.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvr32.exe ATLShellExtDragAndDropHandler.dll

B. Cleanup

Regsvr32.exe /u ATLShellExtDragAndDropHandler.dll


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named 
ATLShellExtDragAndDropHandler in Visual Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Dynamic-link library (DLL). Do not allow merging of proxy/stub 
code. After the creation of the project, remove the proxy project because the 
proxy will never be needed for any extension dlls.

Step3. After the project is created, delete the file 
ATLShellExtDragAndDropHandler.rgs from the project. 
ATLShellExtDragAndDropHandler.rgs is used to set the AppID of the COM 
component, which is not necessary for shell extension. Remove the following 
line from dllmain.h.

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLSHELLEXTDRAGANDDROPHANDLER, 
		"{F063E076-4260-4D10-8FB8-6B59DECAFAD2}")

Last, open Resource View of the project, and delete the registry resource 
IDR_ATLSHELLEXTDRAGANDDROPHANDLER.

Step4. Include the following header files in stdafx.h:

	#include <comdef.h>
	#include <shlobj.h>

Link shlwapi.lib and comctl32.lib in the project (Project properties / Linker 
/ Input / Additional Dependencies.

-----------------------------------------------------------------------------

B. Creating Drag-and-Drop Extension Handler for Files

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as 
FileDragAndDropExt, and select the threading model as Apartment (
corresponding to STA). Because the extension will only be used by Explorer, 
so we can change some settings to remove the Automation features: change the 
Interface type to Custom, and change the Aggregation setting to No. When you 
click OK, the wizard creates a class called CFileDragAndDropExt that contains 
the basic code for implementing a COM object, and adds this class to the 
project.

Step3. Remove the implementation of IFileDragAndDropExt from 
CFileDragAndDropExt since we are not implementing our own interface.

	class ATL_NO_VTABLE CFileDragAndDropExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileDragAndDropExt, &CLSID_FileDragAndDropExt>,
		public IFileDragAndDropExt					// Remove this line

	BEGIN_COM_MAP(CFileDragAndDropExt)
		COM_INTERFACE_ENTRY(IFileDragAndDropExt)	// Remove this line
	END_COM_MAP()

Some registry setting of the component can also be removed safely from 
FileDragAndDropExt.rgs.

	HKCR
	{
r		ATLShellExtDragAndDropHandler.FileDra.1 = s 'FileDragAndDropExt Class'
r		{
r			CLSID = s '{42A553E9-5587-41AF-BCC0-ADA388CFCCDA}'
r		}
r		ATLShellExtDragAndDropHandler.FileDragA = s 'FileDragAndDropExt Class'
r		{
r			CLSID = s '{42A553E9-5587-41AF-BCC0-ADA388CFCCDA}'
r			CurVer = s 'ATLShellExtDragAndDropHandler.FileDra.1'
r		}
		NoRemove CLSID
		{
			ForceRemove {42A553E9-5587-41AF-BCC0-ADA388CFCCDA} = 
				s 'FileDragAndDropExt Class'
			{
r				ProgID = s 'ATLShellExtDragAndDropHandler.FileDra.1'
r				VersionIndependentProgID = 
r					s 'ATLShellExtDragAndDropHandler.FileDragA'
				InprocServer32 = s '%MODULE%'
				{
					val ThreadingModel = s 'Apartment'
				}
r				'TypeLib' = s '{27060220-9651-4446-B7A3-CACE0E0EA53D}'
			}
		}
	}

Step4. Register the drag-and-drop handler. First, in the file 
FileDragAndDropExt.rgs add the following content under HKCR to tell ATL 
what registry entries to add when the server is registered, and which ones to 
delete when the server is unregistered.

	NoRemove Directory
	{
		NoRemove shellex
		{
			NoRemove DragDropHandlers
			{
				{42A553E9-5587-41AF-BCC0-ADA388CFCCDA}
			}
		}
	}
	NoRemove Folder
	{
		NoRemove shellex
		{
			NoRemove DragDropHandlers
			{
				{42A553E9-5587-41AF-BCC0-ADA388CFCCDA}
			}
		}
	}
	NoRemove Drive
	{
		NoRemove shellex
		{
			NoRemove DragDropHandlers
			{
				{42A553E9-5587-41AF-BCC0-ADA388CFCCDA}
			}
		}
	}

Drag-and-drop handlers are typically registered under the following subkey:

	HKEY_CLASSES_ROOT\Directory\shellex\DragDropHandlers\

In some cases, you also need to register under HKCR\Folder to handle drops on 
the desktop, and HKCR\Drive to handle drops in root directories. 
{42A553E9-5587-41AF-BCC0-ADA388CFCCDA} is the CLSID of the drag-and-drop 
handler component.

Step5. Add the implementation of IShellExtInit and IContextMenu to 
CFileDragAndDropExt. 

	class ATL_NO_VTABLE CFileDragAndDropExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileDragAndDropExt, &CLSID_FileDragAndDropExt>,
		public IShellExtInit, 
		public IContextMenu

	BEGIN_COM_MAP(CFileDragAndDropExt)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()	

	// IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IContextMenu
	IFACEMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT)
	{ return E_NOTIMPL; }
	IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
	IFACEMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);

You must implement IShellExtInit when you are writing a handler based on the 
IContextMenu or IShellPropSheetExt interface. When the shell extension is 
loaded, Explorer calls the QueryInterface() function to get a pointer to an 
IShellExtInit interface and invokes its Initialize to give various 
information. pidlFolder is the PIDL of the folder where the files were 
dropped. pDataObj is an IDataObject interface pointer through which we 
retrieve the names of the files being dragged.

The remainder of the operation takes place through the handler's IContextMenu 
interface. The Shell first calls IContextMenu::QueryContextMenu. It passes in 
an HMENU handle that the method can use to add items to the context menu. 

	InsertMenu(hMenu, indexMenu, MF_STRING | MF_BYPOSITION, idCmdFirst + 
		IDM_SAMPLE, _T("&All-In-One Code Framework"));

If the user selects one of the commands, IContextMenu::GetCommandString is 
called to retrieve the Help string that will be displayed on the Windows 
Explorer status bar. In this example, we do not need the help string or verb 
string for the command, so we simply return E_NOTIMPL in GetCommandString.  

If the user clicks one of the handler's items, the Shell calls 
IContextMenu::InvokeCommand. The handler can then execute the appropriate 
command according to the command's verb string (in the high-word of 
lpcmi->lpVerb) or its identifier offset (in the low-word of lpcmi->lpVerb). 
In this example, because we did not implement IContextMenu::GetCommandString 
to specify any verb for the command, the high-word of lpcmi->lpVerb must be 
NULL. The low-word should contain the command's identifier offset. If the  
offset equals the value specified when we created the menu item, then execute  
the command.

	if (NULL != HIWORD(lpcmi->lpVerb))
	{
		return E_INVALIDARG;
	}
	if (LOWORD(lpcmi->lpVerb) == IDM_SAMPLE)
	{
		OnSample(lpcmi->hwnd);
	}
	else
	{
		return E_FAIL;
	}


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating Drag-and-Drop Handlers
http://msdn.microsoft.com/en-us/library/bb776881.aspx

The Complete Idiot's Guide to Writing Shell Extensions - Part IV
http://www.codeproject.com/KB/shell/shellextguide4.aspx


/////////////////////////////////////////////////////////////////////////////