=============================================================================
   ACTIVE TEMPLATE LIBRARY : ATLShellExtContextMenuHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates creating a Shell context menu handler with C++. 

A context menu handler is a shell extension handler that adds commands to an 
existing context menu. Context menu handlers are associated with a particular 
file class and are called any time a context menu is displayed for a member 
of the class. While you can add items to a file class context menu with the 
registry, the items will be the same for all members of the class. By 
implementing and registering such a handler, you can dynamically add items to 
an object's context menu, customized for the particular object.

Context menu handler is the most powerful but also the most complicated method 
to implement. It is strongly encouraged that you implement a context menu 
using one of the static verb methods if applicable:
http://msdn.microsoft.com/en-us/library/dd758091.aspx


/////////////////////////////////////////////////////////////////////////////
Build:

To build ATLShellExtContextMenuHandler, please run Visual Studio as 
administrator because the component needs to be registered into HKCR.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvr32.exe ATLShellExtContextMenuHandler.dll

B. Cleanup

Regsvr32.exe /u ATLShellExtContextMenuHandler.dll


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named 
ATLShellExtContextMenuHandler in Visual Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Dynamic-link library (DLL). Do not allow merging of proxy/stub 
code. After the creation of the project, remove the proxy project because the 
proxy will never be needed for any extension dlls.

Step3. After the project is created, delete the file 
ATLShellExtContextMenuHandler.rgs from the project. 
ATLShellExtContextMenuHandler.rgs is used to set the AppID of the COM 
component, which is not necessary for shell extension. Remove the following 
line from dllmain.h.

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLSHELLEXTCONTEXTMENUHANDLER, 
		"{DEF5FF96-EE4F-4592-BA4A-3AB4C1B3FBC4}")

Last, open Resource View of the project, and delete the registry resource 
IDR_ATLSHELLEXTCONTEXTMENUHANDLER.

Step4. Include the following header files in stdafx.h:

	#include <comdef.h>
	#include <shlobj.h>

Link shlwapi.lib and comctl32.lib in the project (Project properties / Linker 
/ Input / Additional Dependencies.

-----------------------------------------------------------------------------

B. Creating Context Menu Extension Handler for Files

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as 
FileContextMenuExt, and select the threading model as Apartment (
corresponding to STA). Because the extension will only be used by Explorer, 
so we can change some settings to remove the Automation features: change the 
Interface type to Custom, and change the Aggregation setting to No. When you 
click OK, the wizard creates a class called CFileContextMenuExt that 
contains the basic code for implementing a COM object, and adds this class to 
the project.

Step3. Remove the implementation of IFileContextMenuExt from 
CFileContextMenuExt since we are not implementing our own interface.

	class ATL_NO_VTABLE CFileContextMenuExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileContextMenuExt, &CLSID_FileContextMenuExt>,
		public IFileContextMenuExt					// Remove this line
    
	BEGIN_COM_MAP(CFileContextMenuExt)
		COM_INTERFACE_ENTRY(IFileContextMenuExt)	// Remove this line
	END_COM_MAP()

Some registry setting of the component can also be removed safely from 
FileContextMenuExt.rgs.

	HKCR
	{
r		ATLShellExtContextMenuHandler.FileCon.1 = s 'FileContextMenuExt Class'
r		{
r			CLSID = s '{6ECB0C29-A701-4892-A234-667933E1CC91}'
r		}
r		ATLShellExtContextMenuHandler.FileConte = s 'FileContextMenuExt Class'
r		{
r			CLSID = s '{6ECB0C29-A701-4892-A234-667933E1CC91}'
r			CurVer = s 'ATLShellExtContextMenuHandler.FileCon.1'
r		}
		NoRemove CLSID
		{
			ForceRemove {6ECB0C29-A701-4892-A234-667933E1CC91} = 
				s 'FileContextMenuExt Class'
			{
r				ProgID = s 'ATLShellExtContextMenuHandler.FileCon.1'
r				VersionIndependentProgID = 
r					s 'ATLShellExtContextMenuHandler.FileConte'
				InprocServer32 = s '%MODULE%'
				{
					val ThreadingModel = s 'Apartment'
				}
r				'TypeLib' = s '{DEF5FF96-EE4F-4592-BA4A-3AB4C1B3FBC4}'
			}
		}
	}

Step4. Register the context menu handler. First, in the file 
FileContextMenuExt.rgs add the following content under HKCR to tell ATL 
what registry entries to add when the server is registered, and which ones to 
delete when the server is unregistered.

	NoRemove *
	{
		NoRemove shellex
		{
			NoRemove ContextMenuHandlers
			{
				{6ECB0C29-A701-4892-A234-667933E1CC91}
			}
		}
	}

HKCR\* stands for all file extensions. If you want the context menu handler 
to appears only in .txt files' property dialog, replace the line "NoRemove *" 
with "NoRemove txtfile". {6ECB0C29-A701-4892-A234-667933E1CC91} is the CLSID 
of the context menu handler component.

Step5. Add the implementation of IShellExtInit and IContextMenu to 
CFileContextMenuExt.

	class ATL_NO_VTABLE CFileContextMenuExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileContextMenuExt, &CLSID_FileContextMenuExt>,
		public IShellExtInit, 
		public IContextMenu

	BEGIN_COM_MAP(CFileContextMenuExt)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()
	
	// IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IContextMenu
	IFACEMETHODIMP GetCommandString(UINT, UINT, UINT*, LPSTR, UINT);
	IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO);
	IFACEMETHODIMP QueryContextMenu(HMENU, UINT, UINT, UINT, UINT);


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Initializing Shell Extensions
http://msdn.microsoft.com/en-us/library/cc144105.aspx

MSDN: Creating Context Menu Handlers
http://msdn.microsoft.com/en-us/library/bb776881.aspx

MSDN: Implementing the Context Menu COM Object
http://msdn.microsoft.com/en-us/library/ms677106.aspx

MSDN: Extending Shortcut Menus
http://msdn.microsoft.com/en-us/library/cc144101(VS.85).aspx

The Complete Idiot's Guide to Writing Shell Extensions
http://www.codeproject.com/KB/shell/shellextguide1.aspx
http://www.codeproject.com/KB/shell/shellextguide2.aspx
http://www.codeproject.com/KB/shell/shellextguide7.aspx

How to Use Submenus in a Context Menu Shell Extension
http://www.codeproject.com/KB/shell/ctxextsubmenu.aspx


/////////////////////////////////////////////////////////////////////////////