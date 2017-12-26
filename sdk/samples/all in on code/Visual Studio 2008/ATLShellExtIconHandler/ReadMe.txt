========================================================================
    ACTIVE TEMPLATE LIBRARY : ATLShellExtIconHandler Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

An shell icon handler is a type of Shell extension handler that allows you to 
dynamically assign icons to the members of a file class and replace the 
file's default icon with a custom icon on a file-by-file basis. Every time a 
file from the class is displayed, the Shell queries the handler for the 
appropriate icon. For instance, an icon handler can assign different icons to 
different members of the class, or vary the icon based on the current state 
of the file. 

FileIconExt in ATLShellExtIconHandler demonstrates a shell icon handler for 
.cfx files. After installing the icon handler, "C#.cfx" files show Sample C# 
icon, "VB.cfx" files show Sample VB icon, "C++.cfx" files show Sample C++ 
icon, and other .cfx files show a Sample icon in Microsoft Windows Explorer. 


/////////////////////////////////////////////////////////////////////////////
Build:

To build ATLShellExtIconHandler, please run Visual Studio as administrator 
because the component needs to be registered into HKCR.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvr32.exe ATLShellExtIconHandler.dll

B. Cleanup

Regsvr32.exe /u ATLShellExtIconHandler.dll


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named ATLShellExtIconHandler 
in Visual Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Dynamic-link library (DLL). Do not allow merging of proxy/stub 
code. After the creation of the project, remove the proxy project because the 
proxy will never be needed for any extension dlls.

Step3. After the project is created, delete the file 
ATLShellExtIconHandler.rgs from the project. ATLShellExtIconHandler.rgs is 
used to set the AppID of the COM component, which is not necessary for shell 
extension. Remove the following line from dllmain.h.

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLSHELLEXTICONHANDLER, 
		"{C5220BC1-5560-469A-85ED-013A927590EF}")

Last, open Resource View of the project, and delete the registry resource 
IDR_ATLSHELLEXTICONHANDLER.

Step4. Include the following header files in stdafx.h:

	#include <comdef.h>
	#include <shlobj.h>

Link shlwapi.lib and comctl32.lib in the project (Project properties / Linker 
/ Input / Additional Dependencies.

-----------------------------------------------------------------------------

B. Creating Icon Handler for Files

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as FileIconExt, 
and select the threading model as Apartment (corresponding to STA). Because 
the extension will only be used by Explorer, so we can change some settings 
to remove the Automation features: change the Interface type to Custom, and 
change the Aggregation setting to No. When you click OK, the wizard creates 
a class called CFileIconExt that contains the basic code for implementing a 
COM object, and adds this class to the project.

Step3. Remove the implementation of IFileIconExt from CFileIconExt since we 
are not implementing our own interface.

	class ATL_NO_VTABLE CFileIconExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileIconExt, &CLSID_FileIconExt>,
		public IFileIconExt							// Remove this line

	BEGIN_COM_MAP(CFileIconExt)
		COM_INTERFACE_ENTRY(IFileIconExt)			// Remove this line
	END_COM_MAP()

Some registry setting of the component can also be removed safely from 
FileIconExt.rgs.

	HKCR
	{
r		ATLShellExtIconHandler.FileIconExt.1 = s 'FileIconExt Class'
r		{
r			CLSID = s '{5F2C6E67-3871-4016-9A89-66012AE62B6C}'
r		}
r		ATLShellExtIconHandler.FileIconExt = s 'FileIconExt Class'
r		{
r			CLSID = s '{5F2C6E67-3871-4016-9A89-66012AE62B6C}'
r			CurVer = s 'ATLShellExtIconHandler.FileIconExt.1'
r		}
		NoRemove CLSID
		{
			ForceRemove {5F2C6E67-3871-4016-9A89-66012AE62B6C} = s 'FileIconExt Class'
			{
r				ProgID = s 'ATLShellExtIconHandler.FileIconExt.1'
r				VersionIndependentProgID = s 'ATLShellExtIconHandler.FileIconExt'
				InprocServer32 = s '%MODULE%'
				{
					val ThreadingModel = s 'Apartment'
				}
r				'TypeLib' = s '{41F392DC-25EB-4A91-A01E-C3F5FBC258B0}'
			}
		}
	}

Step4. Register the icon handler. First, in the file FileIconExt.rgs add the 
following content under HKCR to tell ATL what registry entries to add when 
the server is registered, and which ones to delete when the server is 
unregistered.

	HKCR
	{
		NoRemove .cfx
		{
			NoRemove DefaultIcon = s '%%1'
			NoRemove ShellEx
			{
				ForceRemove IconHandler = 
					s '{5F2C6E67-3871-4016-9A89-66012AE62B6C}'
			}
		}
	}

When you statically register an icon for a file class, you create a 
DefaultIcon subkey under the file class. Its default value is set to the file 
that contains the icon. To register an icon handler, you must still have the 
DefaultIcon subkey, but its default value must be set to "%1". Add an 
IconHandler subkey to the Shellex subkey of the .cfx subkey, and set its 
default value to the string form of the handler's class identifier (CLSID) 
GUID. There can be only one icon handler for a particular file type, so the 
GUID is stored as a value in the IconHandler key, instead of in a subkey 
under IconHandler.

Step5. Add the implementation of IPersistFile and IExtractIcon to 
CFileIconExt. Icon handlers must export two interfaces in addition to 
IUnknown: IPersistFile and IExtractIcon.

	class ATL_NO_VTABLE CFileIconExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileIconExt, &CLSID_FileIconExt>,
		public IPersistFile,
		public IExtractIcon

	BEGIN_COM_MAP(CFileIconExt)
		COM_INTERFACE_ENTRY(IPersistFile)
		COM_INTERFACE_ENTRY(IExtractIcon)
	END_COM_MAP()

	// IPersistFile
	IFACEMETHODIMP GetClassID(LPCLSID)      { return E_NOTIMPL; }
	IFACEMETHODIMP IsDirty()                { return E_NOTIMPL; }
	IFACEMETHODIMP Load(LPCOLESTR, DWORD);
	IFACEMETHODIMP Save(LPCOLESTR, BOOL)    { return E_NOTIMPL; }
	IFACEMETHODIMP SaveCompleted(LPCOLESTR) { return E_NOTIMPL; }
	IFACEMETHODIMP GetCurFile(LPOLESTR*)    { return E_NOTIMPL; }

	// IExtractIcon
	IFACEMETHODIMP GetIconLocation(UINT, LPTSTR, UINT, int*, UINT*);
	IFACEMETHODIMP Extract(LPCTSTR, UINT, HICON*, HICON*, UINT);

The Shell initializes the handler through its IPersistFile interface. It uses 
this interface to request the handler's CLSID and provides it with the file's 
name:

	// pszFileName contains the absolute path of the file to be opened
	wcscpy_s(m_szFileName, MAX_PATH, pszFileName);

The rest of the operation takes place through the IExtractIcon interface. The 
Shell uses the handler's IExtractIcon interface to request the appropriate 
icon. The interface has two methods: GetIconLocation and Extract, and 
gives two options of providing icon information to the Shell.

Option1. 
GetIconLocation() can return the icon file name (szIconFile) and the 
zero-based index of the icon if there is more than one icon in the file.
Extract() does not have to do anything but return S_FALSE to tell the Shell 
to extract the icon itself. When using this option, the Shell normally 
attempts to load the icon from its cache. The Shell keeps an icon cache which 
holds recently-used icons. 

	IFACEMETHODIMP CFileIconExt::GetIconLocation(
		UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
	{
		// The icons are in this shell extension DLL, so get the full path of the 
		// module and return through the szIconFile parameter.
		GetModuleFileName(_AtlBaseModule.GetResourceInstance(), szIconFile, 
			MAX_PATH);

		// Determine which icon to use, depending on the file name.

		// Get the file name from the file path
		TCHAR szFileName[_MAX_FNAME];
		_tsplitpath_s(m_szFileName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, 
			NULL, 0);

		// Check the file name and set piIndex to the correct index of icon
		if (_tcscmp(szFileName, _T("C#")) == 0)
		{
			// C#.cfx -> the 2nd icon
			*piIndex = 1;
		}
		...
		else
		{
			// Other file name.cfx -> the 1st icon
			*piIndex = 0;
		}

		// Set pwFlags to 0 to get the default behavior from the Shell, i.e.
		// the Shell will attempt to load the icon from its cache.
		*pwFlags = 0;

		return S_OK;
	}

	IFACEMETHODIMP CFileIconExt::Extract(LPCTSTR pszFile, UINT nIconIndex, 
		HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize)
	{
		// Because a file name is provided in GetIconLocation, the handler can 
		// return S_FALSE to have the Shell extract the icon by itself.
		return S_FALSE;
	}

Option2.
To prevent the loading of a cached icon, set the GIL_DONTCACHE flag in the 
pwFlags parameter of GetIconLocation(). This tells the Shell not to look in 
the icon cache. The Shell will always call Extract(), which is then 
responsible for loading the icons and returning handles to those icons that 
the Shell will show. The advantage of this option is that you do not have to 
worry about keeping your icons' resource IDs in order. The downside is that 
it bypasses the Shell's icon cache, which conceivably might slow down file 
browsing a bit if you go into a directory with a ton of .cfx files.

	IFACEMETHODIMP CFileIconExt::GetIconLocation(
		UINT uFlags, LPTSTR szIconFile, UINT cchMax, int* piIndex, UINT* pwFlags)
	{
		// Tell the shell not to look at the icon file name or index, and do not 
		// check the icon cache.
		*pwFlags = GIL_NOTFILENAME | GIL_DONTCACHE;

		return S_OK;
	}

	IFACEMETHODIMP CFileIconExt::Extract(LPCTSTR pszFile, UINT nIconIndex, 
										 HICON* phiconLarge, HICON* phiconSmall, 
										 UINT nIconSize)
	{
		// Determine which icon to use, depending on the file name.
		UINT uIconID;

		// Get the file name from the file path
		TCHAR szFileName[_MAX_FNAME];
		_tsplitpath_s(m_szFileName, NULL, 0, NULL, 0, szFileName, _MAX_FNAME, 
			NULL, 0);

		// Check the file name and set piIndex to the correct icon id
		if (_tcscmp(szFileName, _T("C#")) == 0)
		{
			// C#.cfx -> IDI_SAMPLECS
			uIconID = IDI_SAMPLECS;
		}
		...
		else
		{
			// Other file name.cfx -> IDI_SAMPLE
			uIconID = IDI_SAMPLE;
		}

		// Determine the icon sizes
		WORD wSmallIconSize = HIWORD(nIconSize);
		WORD wLargeIconSize = LOWORD(nIconSize);

		// Load the icons
		if (NULL != phiconLarge)
		{
			*phiconLarge = (HICON)LoadImage(
				_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(uIconID), 
				IMAGE_ICON, wLargeIconSize, wLargeIconSize, LR_DEFAULTCOLOR);
		}
		if (NULL != phiconSmall)
		{
			*phiconSmall = (HICON)LoadImage(
				_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(uIconID), 
				IMAGE_ICON, wSmallIconSize, wSmallIconSize, LR_DEFAULTCOLOR);
		}

		return S_OK;
	}


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating Icon Handlers
http://msdn.microsoft.com/en-us/library/bb776857(VS.85).aspx

The Complete Idiot's Guide to Writing Shell Extensions - Part IX
http://www.codeproject.com/KB/shell/shellextguide9.aspx


/////////////////////////////////////////////////////////////////////////////