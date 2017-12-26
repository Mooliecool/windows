=============================================================================
    ACTIVE TEMPLATE LIBRARY : ATLShellExtInfotipHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

ATLShellExtInfotipHandler demonstrates making Windows shell infotip handler 
in VC++/ATL. Infotip Handler provides pop-up text when the user hovers the 
mouse pointer over the object. 

FileInfotipExt is an example of infotip handler for .txt file objects. When 
mouse is placed over a .txt file item in Windows Explorer, shell queries for 
the registry entry

	HKEY_CLASSES_ROOT\.txt\ShellEx\{00021500-0000-0000-C000-000000000046}

It then checks the default value at this key. If it is a valid CLSID, shell 
creates an object of that class. Once the object is created, shell queries 
for IPersistFile and calls its Load method passing the file name of the item 
over which mouse is placed. After that, IQueryInfo is queried for and the 
method GetInfoTip is called. GetInfoTip has an out parameter *ppwszTip which 
recieves the address of the tool tip buffer. Shell finally displays the info 
tip using the value in the tool tip buffer.


/////////////////////////////////////////////////////////////////////////////
Build:

To build ATLShellExtInfotipHandler, please run Visual Studio as administrator 
because the component needs to be registered into HKCR.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvr32.exe ATLShellExtInfotipHandler.dll

B. Cleanup

Regsvr32.exe /u ATLShellExtInfotipHandler.dll


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named 
ATLShellExtInfotipHandler in Visual Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Dynamic-link library (DLL). Do not allow merging of proxy/stub 
code. After the creation of the project, remove the proxy project because the 
proxy will never be needed for any extension dlls.

Step3. After the project is created, delete the file 
ATLShellExtInfotipHandler.rgs from the project. ATLShellExtInfotipHandler.rgs 
is used to set the AppID of the COM component, which is not necessary for 
shell extension. Remove the following line from dllmain.h.

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLSHELLEXTINFOTIPHANDLER, 
		"{3FF4636C-8A2F-4074-8E94-B6DB0D4080C9}")

Last, open Resource View of the project, and delete the registry resource 
IDR_ATLSHELLEXTINFOTIPHANDLER.

Step4. Include the following header files in stdafx.h:

	#include <comdef.h>
	#include <shlobj.h>

Link shlwapi.lib and comctl32.lib in the project (Project properties / Linker 
/ Input / Additional Dependencies.

-----------------------------------------------------------------------------

B. Creating Infotip Extension Handler for Files

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as 
FileInfotipExt, and select the threading model as Apartment (corresponding to 
STA). Because the extension will only be used by Explorer, so we can change 
some settings to remove the Automation features: change the Interface type to 
Custom, and change the Aggregation setting to No. When you click OK, the 
wizard creates a class called CFileInfotipExt that contains the basic code 
for implementing a COM object, and adds this class to the project.

Step3. Remove the implementation of IFilePropSheetExt from 
CFilePropSheetExt since we are not implementing our own interface.

	class ATL_NO_VTABLE CFileInfotipExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileInfotipExt, &CLSID_FileInfotipExt>,
		public IFileInfotipExt					// Remove this line
    
	BEGIN_COM_MAP(CFileInfotipExt)
		COM_INTERFACE_ENTRY(IFileInfotipExt)	// Remove this line
	END_COM_MAP()

Some registry setting of the component can also be removed safely from 
FileInfotipExt.rgs.

	HKCR
	{
r		ATLShellExtInfotipHandler.FileInfotip.1 = s 'FileInfotipExt Class'
r		{
r			CLSID = s '{869DEFF7-C2CE-4918-B205-EF354C551E47}'
r		}
r		ATLShellExtInfotipHandler.FileInfotipEx = s 'FileInfotipExt Class'
r		{
r			CLSID = s '{869DEFF7-C2CE-4918-B205-EF354C551E47}'
r			CurVer = s 'ATLShellExtInfotipHandler.FileInfotip.1'
r		}
		NoRemove CLSID
		{
			ForceRemove {869DEFF7-C2CE-4918-B205-EF354C551E47} = 
				s 'FileInfotipExt Class'
			{
r				ProgID = s 'ATLShellExtInfotipHandler.FileInfotip.1'
r				VersionIndependentProgID = 
r					s 'ATLShellExtInfotipHandler.FileInfotipEx'
				InprocServer32 = s '%MODULE%'
				{
					val ThreadingModel = s 'Apartment'
				}
r				'TypeLib' = s '{52A14351-F64F-4BA0-AF62-95ED373845E9}'
			}
		}
	}

Step4. Register the infotip handler. First, in the file FileInfotipExt.rgs 
add the following content under HKCR to tell ATL what registry entries to add 
when the server is registered, and which ones to delete when the server is 
unregistered.

	NoRemove .txt
	{
		NoRemove shellex
		{
			{00021500-0000-0000-C000-000000000046} = 
				s '{869DEFF7-C2CE-4918-B205-EF354C551E47}'
		}
	}

HKCR\.txt means that the infotip handler applies to all .txt files. 
{869DEFF7-C2CE-4918-B205-EF354C551E47} is the CLSID of the infotip handler 
component. {00021500-0000-0000-C000-000000000046} is the GUID of IQueryInfo, 
and it stands for toolip handlers under the shellex registry key. When mouse 
is placed over a file item displayed in explorer, shell queries for the 
registry entry 

	HKEY_CLASSES_ROOT\.<file extension>\ShellEx\{00021500-0000-0000-C000-000000000046}

Shell then checks the default value present at this key. If the value is a 
valid CLSID, shell creates an object of the class. 

Step5. Add the implementation of IPersistFile and IQueryInfo to 
CFileInfotipExt. 

	class ATL_NO_VTABLE CFileInfotipExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileInfotipExt, &CLSID_FileInfotipExt>,
		public IPersistFile, 
		public IQueryInfo
		
	BEGIN_COM_MAP(CFileInfotipExt)
		COM_INTERFACE_ENTRY(IPersistFile)
		COM_INTERFACE_ENTRY(IQueryInfo)
	END_COM_MAP()

	// IPersistFile
	IFACEMETHODIMP GetClassID(LPCLSID)      { return E_NOTIMPL; }
	IFACEMETHODIMP IsDirty()                { return E_NOTIMPL; }
	IFACEMETHODIMP Load(LPCOLESTR, DWORD);
	IFACEMETHODIMP Save(LPCOLESTR, BOOL)    { return E_NOTIMPL; }
	IFACEMETHODIMP SaveCompleted(LPCOLESTR) { return E_NOTIMPL; }
	IFACEMETHODIMP GetCurFile(LPOLESTR*)    { return E_NOTIMPL; }

	// IQueryInfo
	IFACEMETHODIMP GetInfoFlags(DWORD*)     { return E_NOTIMPL; }
	IFACEMETHODIMP GetInfoTip(DWORD, LPWSTR*);

Shell queries the extension for IPersistFile and calls its Load method 
passing the file name of the item over which mouse is placed. 
IPersistFile::Load opens the specified file and initializes an object from 
the file contents. We can get the absolute path of the file in this method.

After that, IQueryInfo is queried for and the method GetInfoTip is called. 
GetInfoTip has an out parameter *ppwszTip of type LPWSTR  which recieves the 
address of the tool tip buffer. Please note that the memory pointed by 
*ppwszTip must be allocated by calling CoTaskMemAlloc. Shell knows to call 
CoTaskMemFree to free the memory when the info tip is no longer needed.

	*ppwszTip = (LPWSTR)CoTaskMemAlloc(512 * sizeof(WCHAR));
	swprintf_s(*ppwszTip, 512, L"All-In-One Code Framework Shell Extension " \
		L"Example\nATLShellExtInfotipHandler - FileInfotipExt\nFile name: %s", 
		m_szFileName);


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Initializing Shell Extensions
http://msdn.microsoft.com/en-us/library/cc144105.aspx

MSDN: IQueryInfo Interface
http://msdn.microsoft.com/en-us/library/bb761359.aspx

The Complete Idiot's Guide to Writing Shell Extensions - Part III
http://www.codeproject.com/KB/shell/ShellExtGuide3.aspx


/////////////////////////////////////////////////////////////////////////////