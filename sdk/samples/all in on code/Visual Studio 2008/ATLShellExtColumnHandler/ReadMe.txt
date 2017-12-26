=============================================================================
    ACTIVE TEMPLATE LIBRARY : ATLShellExtColumnHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The Details view in the Microsoft Windows Windows Explorer normally displays 
several standard columns. Each column lists information, such as the file 
size or type, for each file in the current folder. By implementing and 
registering a column handler, you can make custom columns available for 
display on Windows 2000, Windows XP and Windows 2003. 

Note: Support for column handler (IColumnProvider) has been removed from 
Windows Vista. So the sample does not work on Windows Vista, and later 
operating systems. The new property system should be used in its place. See 
Property System (http://msdn.microsoft.com/en-us/library/bb776859.aspx) for 
conceptual materials that explain the use of the new system.

FileColumnExt in ATLShellExtColumnHandler demonstrates a column handler that 
provides three columns:

	1. Sample C1 - display the file path when enabled
	2. Sample C2 - display the file size when enabled
	3. Title (Predefined column) - display the file name when enabled

The three columns process only .cfx files.


/////////////////////////////////////////////////////////////////////////////
Build:

To build ATLShellExtColumnHandler, please run Visual Studio as administrator 
because the component needs to be registered into HKCR.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvr32.exe ATLShellExtColumnHandler.dll

B. Cleanup

Regsvr32.exe /u ATLShellExtColumnHandler.dll


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named ATLShellExtColumnHandler 
in Visual Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Dynamic-link library (DLL). Do not allow merging of proxy/stub 
code. After the creation of the project, remove the proxy project because the 
proxy will never be needed for any extension dlls.

Step3. After the project is created, delete the file 
ATLShellExtColumnHandler.rgs from the project. ATLShellExtColumnHandler.rgs 
is used to set the AppID of the COM component, which is not necessary for 
shell extension. Remove the following line from dllmain.h.

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLSHELLEXTCOLUMNHANDLER, 
		"{20092C2A-8317-4CA9-A65E-567B39E5E0AD}")

Last, open Resource View of the project, and delete the registry resource 
IDR_ATLSHELLEXTCOLUMNHANDLER.

Step4. Include the following header files in stdafx.h:

	#include <comdef.h>
	#include <shlobj.h>

Link shlwapi.lib and comctl32.lib in the project (Project properties / Linker 
/ Input / Additional Dependencies.

-----------------------------------------------------------------------------

B. Creating Column Extension Handler for Files

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as FileColumnExt, 
and select the threading model as Apartment (corresponding to STA). Because 
the extension will only be used by Explorer, so we can change some settings 
to remove the Automation features: change the Interface type to Custom, and 
change the Aggregation setting to No. When you click OK, the wizard creates a 
class called CFileDragAndDropExt that contains the basic code for 
implementing a COM object, and adds this class to the project.

Step3. Remove the implementation of IFileColumnExt from CFileColumnExt since 
we are not implementing our own interface.

	class ATL_NO_VTABLE CFileColumnExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileColumnExt, &CLSID_FileColumnExt>,
		public IFileColumnExt						// Remove this line

	BEGIN_COM_MAP(CFileColumnExt)
		COM_INTERFACE_ENTRY(IFileColumnExt)			// Remove this line
	END_COM_MAP()

Some registry setting of the component can also be removed safely from 
FileColumnExt.rgs.

	HKCR
	{
r		ATLShellExtColumnHandler.FileColumnEx.1 = s 'FileColumnExt Class'
r		{
r			CLSID = s '{69019B90-52D8-4F93-91A3-5378CDC9C290}'
r		}
r		ATLShellExtColumnHandler.FileColumnExt = s 'FileColumnExt Class'
r		{
r			CLSID = s '{69019B90-52D8-4F93-91A3-5378CDC9C290}'
r			CurVer = s 'ATLShellExtColumnHandler.FileColumnEx.1'
r		}
		NoRemove CLSID
		{
			ForceRemove {69019B90-52D8-4F93-91A3-5378CDC9C290} = 
				s 'FileColumnExt Class'
			{
r				ProgID = s 'ATLShellExtColumnHandler.FileColumnEx.1'
r				VersionIndependentProgID = s 'ATLShellExtColumnHandler.FileColumnExt'
				InprocServer32 = s '%MODULE%'
				{
					val ThreadingModel = s 'Apartment'
				}
r				'TypeLib' = s '{23FEBFA0-BBEC-4BA9-BD3C-B6F689AAAC57}'
			}
		}
	}

Step4. Register the column handler. First, in the file FileColumnExt.rgs add 
the following content under HKCR to tell ATL what registry entries to add 
when the server is registered, and which ones to delete when the server is 
unregistered. Column handlers are registered under the following subkey. 

	HKEY_CLASSES_ROOT\Folder\shellex\ColumnHandlers

Create a subkey of ColumnHandlers named with the string form of the handler's 
class identifier (CLSID) GUID.

	HKCR
	{
		NoRemove Folder
		{
			NoRemove Shellex
			{
				NoRemove ColumnHandlers
				{
					ForceRemove {69019B90-52D8-4F93-91A3-5378CDC9C290} = s 'FileColumnExt'
				}
			}
		}
	}

Step5. Add the implementation of IColumnProvider to CFileColumnExt.

	class ATL_NO_VTABLE CFileColumnExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileColumnExt, &CLSID_FileColumnExt>,
		public IColumnProvider

	BEGIN_COM_MAP(CFileColumnExt)
		COM_INTERFACE_ENTRY_IID(IID_IColumnProvider, IColumnProvider)
	END_COM_MAP()

	// IColumnProvider
	IFACEMETHODIMP Initialize(LPCSHCOLUMNINIT psci);
	IFACEMETHODIMP GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci);
	IFACEMETHODIMP GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, 
		VARIANT* pvarData);


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating Column Handlers
http://msdn.microsoft.com/en-us/library/bb776831(VS.85).aspx

The Complete Idiot's Guide to Writing Shell Extensions - Part VIII
http://www.codeproject.com/KB/shell/shellextguide8.aspx


/////////////////////////////////////////////////////////////////////////////