=============================================================================
    ACTIVE TEMPLATE LIBRARY : ATLShellExtIconOverlayHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

Icon overlays are small images placed at the lower-left corner of the icon 
that represents a Shell object. They are normally added to an object's icon 
to provide some extra information. For instance, a commonly used icon overlay 
is the small arrow that indicates that the icon represents a link, rather 
than the actual file or folder. In addition to the standard icon overlays 
that are provided by the system, you can request custom icon overlays for 
specified Shell objects by implementing and registering an icon overlay 
handler.

FileIconOverlayExt in ATLShellExtIconOverlayHandler demonstrates a shell icon 
overlay handler for files with "Sample" in the file paths. After installing 
the icon overlay handler, any file with "Sample" in its file path, e.g. 
"D:\Sample.txt", shows an icon overlay in Windows Explorer. 


/////////////////////////////////////////////////////////////////////////////
Build:

To build ATLShellExtIconOverlayHandler, please run Visual Studio as 
dministrator because the component needs to be registered into HKCR.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvr32.exe ATLShellExtIconOverlayHandler.dll

B. Cleanup

Regsvr32.exe /u ATLShellExtIconOverlayHandler.dll


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named 
ATLShellExtIconOverlayHandler in Visual Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Dynamic-link library (DLL). Do not allow merging of proxy/stub 
code. After the creation of the project, remove the proxy project because the 
proxy will never be needed for any extension dlls.

Step3. After the project is created, delete the file 
ATLShellExtIconOverlayHandler.rgs from the project. 
ATLShellExtIconOverlayHandler.rgs is used to set the AppID of the COM 
component, which is not necessary for shell extension. Remove the following 
line from dllmain.h.

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLSHELLEXTICONOVERLAYHANDLER, 
		"{2C120B84-1B64-41DF-A6C9-58CA609DDA10}")

Last, open Resource View of the project, and delete the registry resource 
IDR_ATLSHELLEXTICONOVERLAYHANDLER.

Step4. Include the following header files in stdafx.h:

	#include <comdef.h>
	#include <shlobj.h>

Link shlwapi.lib and comctl32.lib in the project (Project properties / Linker 
/ Input / Additional Dependencies.

-----------------------------------------------------------------------------

B. Creating Icon Overlay Handler for Files

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as 
FileIconOverlayExt, and select the threading model as Apartment 
(corresponding to STA). Because the extension will only be used by Explorer, 
so we can change some settings to remove the Automation features: change the 
Interface type to Custom, and change the Aggregation setting to No. When you 
click OK, the wizard creates a class called CFileIconOverlayExt that contains 
the basic code for implementing a COM object, and adds this class to the 
project.

Step3. Remove the implementation of IFileIconOverlayExt from 
CFileIconOverlayExt since we are not implementing our own interface.

	class ATL_NO_VTABLE CFileIconOverlayExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileIconOverlayExt, &CLSID_FileIconOverlayExt>,
		public IFileIconOverlayExt					// Remove this line
		
	BEGIN_COM_MAP(CFileIconOverlayExt)
		COM_INTERFACE_ENTRY(IFileIconOverlayExt)	// Remove this line
	END_COM_MAP()

Some registry setting of the component can also be removed safely from 
FileIconOverlayExt.rgs.

	HKCR
	{
r		ATLShellExtIconOverlayHandler.FileIco.1 = s 'FileIconOverlayExt Class'
r		{
r			CLSID = s '{B110C7D3-BE8C-43D5-83FD-DC1AAC7605A2}'
r		}
r		ATLShellExtIconOverlayHandler.FileIconO = s 'FileIconOverlayExt Class'
r		{
r			CLSID = s '{B110C7D3-BE8C-43D5-83FD-DC1AAC7605A2}'
r			CurVer = s 'ATLShellExtIconOverlayHandler.FileIco.1'
r		}
		NoRemove CLSID
		{
			ForceRemove {B110C7D3-BE8C-43D5-83FD-DC1AAC7605A2} = 
				s 'FileIconOverlayExt Class'
			{
r				ProgID = s 'ATLShellExtIconOverlayHandler.FileIco.1'
r				VersionIndependentProgID = 
r					s 'ATLShellExtIconOverlayHandler.FileIconO'
				InprocServer32 = s '%MODULE%'
				{
					val ThreadingModel = s 'Apartment'
				}
r				'TypeLib' = s '{26E7F59B-B172-4952-944E-EC15689CD8FB}'
			}
		}
	}

Step4. Register the icon overlay handler. First, in the file 
FileIconOverlayExt.rgs add the following content under HKCR to tell ATL what 
registry entries to add when the server is registered, and which ones to 
delete when the server is unregistered. In addition to normal COM 
registration, you must create a subkey for the handler under the 
ShellIconOverlayIdentifiers key:

	HKEY_LOCAL_MACHINE/Software/Microsoft/Windows/CurrentVersion/Explorer/
		ShellIconOverlayIdentifiers

Set the default value of the subkey to the string form of the object's CLSID.

	HKLM
	{
		NoRemove Software
		{
			NoRemove Microsoft
			{
				NoRemove Windows
				{
					NoRemove CurrentVersion
					{
						NoRemove Explorer
						{
							NoRemove ShellIconOverlayIdentifiers
							{
								ForceRemove SampleATLShellExtIconOverlayHandler = 
									s '{B110C7D3-BE8C-43D5-83FD-DC1AAC7605A2}'
							}
						}
					}
				}
			}
		}
	}

Step5. Add the implementation of IShellIconOverlayIdentifier to 
CFileIconOverlayExt. 

	class ATL_NO_VTABLE CFileIconOverlayExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFileIconOverlayExt, &CLSID_FileIconOverlayExt>,
		public IShellIconOverlayIdentifier

	BEGIN_COM_MAP(CFileIconOverlayExt)
		COM_INTERFACE_ENTRY(IShellIconOverlayIdentifier)
	END_COM_MAP()

	// IShellIconOverlayIdentifier
	IFACEMETHODIMP GetOverlayInfo(LPWSTR, int, int*, DWORD*);
	IFACEMETHODIMP GetPriority(int*);
	IFACEMETHODIMP IsMemberOf(LPCWSTR, DWORD);

When the Shell starts up, it initializes all icon overlay handlers by calling 
two of their IShellIconOverlayIdentifier methods:

	1.
	The Shell calls GetOverlayInfo to request the location of the handler's 
	icon overlay. The icon overlay handler returns the name of the file 
	containing the overlay image, and its index within that file. The Shell 
	then adds the icon overlay to the system image list. 

	IFACEMETHODIMP CFileIconOverlayExt::GetOverlayInfo(
		LPWSTR pwszIconFile, int cchMax, int* pIndex, DWORD* pdwFlags)
	{
		// Get the module's full path
		GetModuleFileNameW(_AtlBaseModule.GetModuleInstance(), pwszIconFile, 
			cchMax);

		// Use the first icon in the resource
		*pIndex = 0;

		*pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;d

		return S_OK;
	}
	
	2.
	The Shell calls GetPriority to determine the icon overlay's priority. The 
	priority value is a number from zero to 100, with 100 indicating the 
	lowest priority. If more than one icon overlay is requested for a 
	particular file, the Shell uses this value to help it determine which 
	icon overlay to display. 
	
	IFACEMETHODIMP CFileIconOverlayExt::GetPriority(int* pPriority)
	{
		// Request the highest priority 
		*pPriority = 0;

		return S_OK;
	}

Note   Once the image has been loaded into the system image list during 
initialization, it cannot be changed. After initialization, the file name and 
index are used only to identify the icon overlay. The system will not load a 
new icon overlay. 

Before painting an object's icon, the Shell passes the object's name to each 
icon overlay handler's IsMemberOf method. An icon overlay handler is normally 
associated with a particular group of files. For example, the icon overlay 
handler might request an overlay for all members of a file class, such as all 
files with an .myp file name extension. If a handler wants to have its icon 
overlay displayed, it returns S_OK. The Shell then calls the handler's 
GetOverlayInfo method to determine which icon to display.

Although only one icon overlay can be displayed, it is possible for an object 
to have icon overlays requested by more than one handler. In that case, the 
Shell resolves the conflict by displaying the highest priority icon overlay. 
The Shell resolves many such conflicts with an internal set of rules. If 
these rules are not sufficient, the Shell compares the priority values that 
were specified by the handlers' GetPriority method during initialization.

	IFACEMETHODIMP CFileIconOverlayExt::IsMemberOf(LPCWSTR pwszPath, 
												   DWORD dwAttrib)
	{
		// If the file name contains "Sample", add the overlay
		if (wcsstr(pwszPath, L"Sample") != 0)
			return S_OK;
		else
			return S_FALSE;
	}


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating Icon Overlay Handlers
http://msdn.microsoft.com/en-us/library/bb776858(VS.85).aspx

How to overlay an icon over existing shell objects in 3 easy steps
http://www.codeproject.com/KB/shell/overlayicon.aspx


/////////////////////////////////////////////////////////////////////////////