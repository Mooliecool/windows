=============================================================================
    ACTIVE TEMPLATE LIBRARY : ATLShellExtPropSheetHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

ATLShellExtPropSheetHandler demonstrates making Windows shell property sheet 
handler in VC++/ATL. Property Sheet Handler is called before an object's 
Properties property sheet is displayed. It enables you to add or replace 
pages. You can register and implement a Property Sheet Handler for a file 
class (see FilePropSheetExt), a mounted drive, a control panel application, 
and in Windows 7, you can install property sheet handler to devices in 
Devices and Printers dialog (see Win7DevicePropSheetExt).

ATLShellExtPropSheetHandler exposes these property sheet handlers:

1. FilePropSheetExt

CLSID: 3CDE9BE7-5794-4338-A1BA-5303410D56C1

FilePropSheetExt adds a property page with the subject "All-In-One Code 
Framework" to the property dialogs of all file classes in Windows Explorer. 
The property page displays the name of the first selected file. It also has 
a button "Simulate Property Changing" to simulate the change of properties 
that activates the "Apply" button of the property sheet. 

2. Win7DevicePropSheetExt

CLSID: 387E6235-C3B3-4109-AB21-303EBE6FB5AB

Win7DevicePropSheetExt adds a property page with the subject "All-In-One Code 
Framework" to the property sheet of mouse in Windows 7 Devices and Printers 
dialog. 


/////////////////////////////////////////////////////////////////////////////
Build:

To build ATLShellExtPropSheetHandler, please run Visual Studio as 
administrator because the component needs to be registered into HKCR.


/////////////////////////////////////////////////////////////////////////////
Deployment:

A. Setup

Regsvr32.exe ATLShellExtPropSheetHandler.dll

B. Cleanup

Regsvr32.exe /u ATLShellExtPropSheetHandler.dll


/////////////////////////////////////////////////////////////////////////////
Creation:

A. Creating the project

Step1. Create a Visual C++ / ATL / ATL Project named 
ATLShellExtPropSheetHandler in Visual Studio 2008.

Step2. In the page "Application Settings" of ATL Project Wizard, select the 
server type as Dynamic-link library (DLL). Do not allow merging of proxy/stub 
code. After the creation of the project, remove the proxy project because the 
proxy will never be needed for any extension dlls.

Step3. After the project is created, delete the file 
ATLShellExtPropSheetHandler.rgs from the project. 
ATLShellExtPropSheetHandler.rgs is used to set the AppID of the COM component, 
which is not necessary for shell extension. Remove the following line from 
dllmain.h.

	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ATLSHELLEXTPROPSHEETHANDLER, 
		"{A86133D8-AC65-425D-B03B-E05C4A51951D}")

Last, open Resource View of the project, and delete the registry resource 
IDR_ATLSHELLEXTPROPSHEETHANDLER.

Step4. Include the following header files in stdafx.h:

	#include <comdef.h>
	#include <shlobj.h>

Link shlwapi.lib and comctl32.lib in the project (Project properties / Linker 
/ Input / Additional Dependencies.

-----------------------------------------------------------------------------

B. Creating Property Sheet Extension Handler for Files

Step1. In Solution Explorer, add a new ATL / ATL Simple Object class to the 
project.

Step2. In ATL Simple Object Wizard, specify the short name as 
FilePropSheetExt, and select the threading model as Apartment (
corresponding to STA). Because the extension will only be used by Explorer, 
so we can change some settings to remove the Automation features: change the 
Interface type to Custom, and change the Aggregation setting to No. When you 
click OK, the wizard creates a class called CFilePropSheetExt that 
contains the basic code for implementing a COM object, and adds this class to 
the project.

Step3. Remove the implementation of IFilePropSheetExt from 
CFilePropSheetExt since we are not implementing our own interface.

	class ATL_NO_VTABLE CFilePropSheetExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFilePropSheetExt, &CLSID_FilePropSheetExt>,
		public IFilePropSheetExt				// Remove this line
    
	BEGIN_COM_MAP(CFilePropSheetExt)
		COM_INTERFACE_ENTRY(IFilePropSheetExt)	// Remove this line
	END_COM_MAP()

Some registry setting of the component can also be removed safely from 
FilePropSheetExt.rgs.

	HKCR
	{
r		ATLShellExtPropSheetHandler.FilePropS.1 = s 'FilePropSheetExt Class'
r		{
r			CLSID = s '{3CDE9BE7-5794-4338-A1BA-5303410D56C1}'
r		}
r		ATLShellExtPropSheetHandler.FilePropShe = s 'FilePropSheetExt Class'
r		{
r			CLSID = s '{3CDE9BE7-5794-4338-A1BA-5303410D56C1}'
r			CurVer = s 'ATLShellExtPropSheetHandler.FilePropS.1'
r		}
		NoRemove CLSID
		{
			ForceRemove {3CDE9BE7-5794-4338-A1BA-5303410D56C1} = 
				s 'FilePropSheetExt Class'
			{
r				ProgID = s 'ATLShellExtPropSheetHandler.FilePropS.1'
r				VersionIndependentProgID = 
r					s 'ATLShellExtPropSheetHandler.FilePropShe'
				InprocServer32 = s '%MODULE%'
				{
					val ThreadingModel = s 'Apartment'
				}
r				'TypeLib' = s '{15EF6A74-CCAB-4D0C-97AF-42397354D554}'
			}
		}
	}

Step4. Register the property sheet handler. First, in the file 
FilePropSheetExt.rgs add the following content under HKCR to tell ATL 
what registry entries to add when the server is registered, and which ones to 
delete when the server is unregistered.

	NoRemove *
	{
		NoRemove shellex
		{
			NoRemove PropertySheetHandlers
			{
				{3CDE9BE7-5794-4338-A1BA-5303410D56C1}
			}
		}
	}

HKCR\* stands for all file extensions. If you want the property sheet handler 
to appears only in .txt files' property dialog, replace the line "NoRemove *" 
with "NoRemove txtfile". {3CDE9BE7-5794-4338-A1BA-5303410D56C1} is the CLSID 
of the property sheet handler component.

Step5. Add the implementation of IShellExtInit and IShellPropSheetExt to 
CFilePropSheetExt. 

	class ATL_NO_VTABLE CFilePropSheetExt :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CFilePropSheetExt, &CLSID_FilePropSheetExt>,
		public IShellExtInit,
		public IShellPropSheetExt

	BEGIN_COM_MAP(CFilePropSheetExt)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IShellPropSheetExt)
	END_COM_MAP()

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IShellPropSheetExt
    IFACEMETHODIMP AddPages(LPFNADDPROPSHEETPAGE, LPARAM);
    IFACEMETHODIMP ReplacePage(UINT, LPFNADDPROPSHEETPAGE, LPARAM) 
	{ return E_NOTIMPL; }

You must implement IShellExtInit when you are writing a handler based on the 
IContextMenu or IShellPropSheetExt interface. When the shell extension is 
loaded, Explorer calls the QueryInterface() function to get a pointer to an 
IShellExtInit interface and invokes its Initialize to give various 
information. pidlFolder is the PIDL of the folder containing the files being 
acted upon. pDataObj is an IDataObject interface pointer through which we 
retrieve the names of the files being worked on.

If IShellExtInit::Initialize() returns S_OK, Explorer queries for the 
interface IShellPropSheetExt. The AddPages method of IShellPropSheetExt is 
the one we'll implement. In AddPages, we can add pages to the property sheet 
by setting up a PROPSHEETPAGE struct to represent the new page, and creating 
the page (CreatePropertySheetPage). If that succeeds, we call the shell's 
callback function which adds the newly-created page to the property sheet. 
The callback returns a BOOL indicating success or failure. If it fails, we 
destroy the page.

	if (!hPage)
	{
		if (!lpfnAddPageProc(hPage, lParam))
			DestroyPropertySheetPage(hPage);
	}

While setting up the PROPSHEETPAGE struct, you can specify two call-backs: 

	PROPSHEETPAGE.pfnDlgProc: the dialog procedure of the property page. In 
	the dialog proc, you can, for example, hook WM_INITDIALOG to initialize 
	the dialog, or handle the PSN_APPLY notification if the user clicks the 
	OK or Apply button.
	
	PROPSHEETPAGE.pfnCallback: gets called when the page is created and 
	destroyed. To use this call-back, you must add PSP_USECALLBACK to 
	PROPSHEETPAGE.dwFlags.

Step6. Pass the extension object to the property page.

The property sheet extension object is independent from the property page. 
In many cases, it is desirable to be able to use the extension object, or 
some other object, from the property page. To do this, set the lParam member 
of PROPSHEETPAGE structure to the object pointer. The property page can then 
retrieve this value when it processes the WM_INITDIALOG message. For a 
property page, the lParam parameter of the WM_INITDIALOG message is a pointer 
to the PROPSHEETPAGE structure. Retrieve the object pointer by casting the 
lParam of the WM_INITDIALOG message to a PROPSHEETPAGE pointer and then 
retrieving the lParam member of the PROPSHEETPAGE structure.

	// In the message handler of WM_INITDIALOG
	LPPROPSHEETPAGE pPage = (LPPROPSHEETPAGE)lParam;
	if (pPage)
	{
		CFilePropSheetExt* pPropSheetExt = (CFilePropSheetExt*)
			pPage->lParam;
		if (pPropSheetExt)
		{
			// Access the property sheet extension from property page
		}
	}

Be aware that after IShellPropSheetExt::AddPages is called, the property 
sheet will release the property sheet extension object and never use it 
again. This means that the extension object would be deleted before the 
property page is displayed. When the page attempts to access the object 
pointer, the memory will have been freed and the pointer will not be valid. 
To correct this, increment the reference count for the extension object when 
the page is added and then release the object when the property page dialog 
is destroyed:

	// In CFilePropSheetExt::AddPages
	hPage = CreatePropertySheetPage (&psp);
	if (hPage)
	{
		if (lpfnAddPageProc(hPage, lParam))
		{
            this->AddRef();
		}
		else
		{
			DestroyPropertySheetPage(hPage);
			return E_FAIL;
		}
	}

	// In PropPageCallbackProc
	switch(uMsg)
    {
	case PSPCB_RELEASE:
        {
			CFilePropSheetExt *pPropSheetExt = (CFilePropSheetExt*)ppsp->lParam;
			if(pPropSheetExt)
			{
				pPropSheetExt->Release();
			}
		}
		break;
    }

-----------------------------------------------------------------------------

C. Creating Property Sheet Extension Handler for Windows 7 Devices and 
Printers Folder

Devices and Printers Folder - Extensibility Guide
http://www.microsoft.com/whdc/device/DeviceExperience/DevPrintFolder-Ext.mspx

Win7DevicePropSheetExt demonstrates a property sheet handler for Windows 7 
mouse device. The steps of creating the extension handler for devices in 
Windows 7 Devices and Printers Folder are similar to the steps for files, 
except step 4.

Step4. Register the property sheet handler. In the file 
Win7DevicePropSheetExt.rgs add the following content under HKCR.

	NoRemove DeviceDisplayObject
    {
		NoRemove HardwareId
		{
			NoRemove HID_DEVICE_SYSTEM_MOUSE
			{
				NoRemove shellex
				{
					NoRemove PropertySheetHandlers
					{
						{387E6235-C3B3-4109-AB21-303EBE6FB5AB}
					}
				}
			}
		}
	}

This installs the property sheet handler to devices with hardware ID 
HID_DEVICE_SYSTEM_MOUSE. 


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Initializing Shell Extensions
http://msdn.microsoft.com/en-us/library/cc144105.aspx

MSDN: Creating Property Sheet Handlers
http://msdn.microsoft.com/en-us/library/bb776850.aspx

MSDN: Implementing the Property Page COM Object
http://msdn.microsoft.com/en-us/library/ms677109.aspx

The Complete Idiot's Guide to Writing Shell Extensions - Part V
http://www.codeproject.com/KB/shell/shellextguide5.aspx

Devices and Printers Folder - Extensibility Guide
http://www.microsoft.com/whdc/device/DeviceExperience/DevPrintFolder-Ext.mspx


/////////////////////////////////////////////////////////////////////////////