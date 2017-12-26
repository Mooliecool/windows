=============================================================================
    DYNAMIC LINK LIBRARY : CppShellExtPropSheetHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates creating a Shell property sheet handler with C++. 

A property sheet extension is a COM object implemented as an in-proc server. 
The property sheet extension must implement the IShellExtInit and 
IShellPropSheetExt interfaces. A property sheet extension is instantiated 
when the user displays the property sheet for an object of a class for which 
the property sheet extension has been registered in the display specifier of 
the class. It enables you to add or replace pages. You can register and 
implement a property sheet handler for a file class, a mounted drive, a 
control panel application, and starting from Windows 7, you can install a 
property sheet handler to devices in Devices and Printers dialog.

The example property sheet handler has the class ID (CLSID): 
    {5C8FA94F-F274-49B9-A5E5-D34C093F7846}

It adds a property sheet page with the title "CppShellExtPropSheetHandler" to 
the Properties dialog of the .cpp file class when one .cpp file is selected 
in the Windows Explorer. The property sheet page displays the name of the 
selected file. It also has a button "Simulate Property Changing" to simulate 
the change of properties that activates the "Apply" button of the property 
sheet. 


/////////////////////////////////////////////////////////////////////////////
Setup and Removal:

A. Setup

If you are going use the Shell extension in a x64 Windows system, please 
configure the Visual C++ project to target 64-bit platforms using project 
configurations (http://msdn.microsoft.com/en-us/library/9yb4317s.aspx). Only 
64-bit extension DLLs can be loaded in the 64-bit Windows Shell. If the 
extension is to be loaded in a 32-bit Windows system, you can use the default 
Win32 project configuration to build the project.

In a command prompt running as administrator, navigate to the folder that 
contains the build result CppShellExtPropSheetHandler.dll and enter the 
command:

    Regsvr32.exe CppShellExtPropSheetHandler.dll

The context menu handler is registered successfully if you see a message box 
saying:

    "DllRegisterServer in CppShellExtPropSheetHandler.dll succeeded."

B. Removal

In a command prompt running as administrator, navigate to the folder that 
contains the build result CppShellExtPropSheetHandler.dll and enter the 
command:

    Regsvr32.exe /u CppShellExtPropSheetHandler.dll

The context menu handler is unregistered successfully if you see a message 
box saying:

    "DllUnregisterServer in CppShellExtPropSheetHandler.dll succeeded."


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the property sheet 
handler code sample.

Step1. If you are going use the Shell extension in a x64 Windows system, 
please configure the Visual C++ project to target 64-bit platforms using 
project configurations (http://msdn.microsoft.com/en-us/library/9yb4317s.aspx). 
Only 64-bit extension DLLs can be loaded in the 64-bit Windows Shell. If the 
extension is to be loaded in a 32-bit Windows system, you can use the default 
Win32 project configuration.

Step2. After you successfully build the sample project in Visual Studio 2010, 
you will get a DLL: CppShellExtPropSheetHandler.dll. Start a command prompt 
as administrator, navigate to the folder that contains the file and enter the 
command:

    Regsvr32.exe CppShellExtPropSheetHandler.dll

The property sheet handler is registered successfully if you see a message 
box saying:

    "DllRegisterServer in CppShellExtPropSheetHandler.dll succeeded."

Step3. Find a .cpp file in the Windows Explorer (e.g. FilePropSheetExt.cpp 
in the sample folder), and open its Properties dialog. You would see the 
"CppShellExtPropSheetHandler" property sheet page in the dialog. The page 
displays the name of the selected file. It also has a button "Simulate 
Property Changing" to simulate the change of properties that activates the 
"Apply" button of the property sheet. 

The "CppShellExtPropSheetHandler" property sheet page is added and displayed 
when the Properties dialog is opened for only one .cpp file. If the 
Properties dialog is opened for more than one file, you will not see the 
property sheet page.

Step4. In the same command prompt, run the command 

    Regsvr32.exe /u CppShellExtPropSheetHandler.dll

to unregister the Shell context menu handler.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Creating and configuring the project

In Visual Studio 2010, create a Visual C++ / Win32 / Win32 Project named 
"CppShellExtPropSheetHandler". In the "Application Settings" page of Win32 
Application Wizard, select the application type as "DLL" and check the "Empty 
project" option. After you click the Finish button, an empty Win32 DLL 
project is created.

-----------------------------------------------------------------------------

B. Implementing a basic Component Object Model (COM) DLL

Shell extension handlers are all in-process COM objects implemented as DLLs. 
Making a basic COM includes implementing DllGetClassObject, DllCanUnloadNow, 
DllRegisterServer, and DllUnregisterServer in (and exporting them from) the 
DLL, adding a COM class with the basic implementation of the IUnknown 
interface, preparing the class factory for your COM class. The relevant files 
in this code sample are:

  dllmain.cpp - implements DllMain and the DllGetClassObject, DllCanUnloadNow, 
    DllRegisterServer, DllUnregisterServer functions that are necessary for a 
    COM DLL. 

  GlobalExportFunctions.def - exports the DllGetClassObject, DllCanUnloadNow, 
    DllRegisterServer, DllUnregisterServer functions from the DLL through the 
    module-definition file. You need to pass the .def file to the linker by 
    configuring the Module Definition File property in the project's Property 
    Pages / Linker / Input property page.

  Reg.h/cpp - defines the reusable helper functions to register or unregister 
    in-process COM components in the registry: 
    RegisterInprocServer, UnregisterInprocServer

  FilePropSheetExt.h/cpp - defines the COM class. You can find the basic 
    implementation of the IUnknown interface in the files.

  ClassFactory.h/cpp - defines the class factory for the COM class. 

-----------------------------------------------------------------------------

C. Implementing the property sheet handler and registering it for a certain 
file class

-----------
Implementing the property sheet handler:

The FilePropSheetExt.h/cpp files define a property sheet handler. The 
property sheet handler must implement the IShellExtInit and IShellPropSheetExt 
interfaces. A property sheet extension is instantiated when the user displays 
the property sheet for an object of a class for which the property sheet 
extension has been registered in the display specifier of the class.

    class FilePropSheetExt : public IShellExtInit, public IShellPropSheetExt
    {
    public:
        // IShellExtInit
        IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT 
            pDataObj, HKEY hKeyProgID);

        // IShellPropSheetExt
        IFACEMETHODIMP AddPages(LPFNADDPROPSHEETPAGE pfnAddPage, LPARAM lParam);
        IFACEMETHODIMP ReplacePage(UINT uPageID, 
            LPFNADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam);
    };

  1. Implementing IShellExtInit

  After the property sheet extension COM object is instantiated, the 
  IShellExtInit::Initialize method is called. IShellExtInit::Initialize 
  supplies the property sheet extension with an IDataObject object that 
  holds one or more file names in CF_HDROP format. You can enumerate the 
  selected files and folders through the IDataObject object. If any value 
  other than S_OK is returned from IShellExtInit::Initialize, the property  
  sheet is not displayed.

  In the code sample, the FileContextMenuExt::Initialize method enumerates 
  the selected files and folders. If only one file is selected, the method 
  stores the file name for later use and returns S_OK to proceed. If more 
  than one file or no file are selected, the method returns E_FAIL to not 
  display the property sheet.

  2. Implementing IShellPropSheetExt

  After IShellExtInit::Initialize returns S_OK, the 
  IShellPropSheetExt::AddPages method is called. The property sheet 
  extension must add the page or pages during this method. A property page is 
  created by filling a PROPSHEETPAGE structure and then passing the structure 
  to the CreatePropertySheetPage function. The property page is then added to 
  the property sheet by calling the callback function passed to 
  IShellPropSheetExt::AddPages in the pfnAddPage parameter. The callback 
  returns a BOOL indicating success or failure. If it fails, we destroy the 
  page by calling DestroyPropertySheetPage.

  While setting up the PROPSHEETPAGE struct, you can specify two call-backs: 

	PROPSHEETPAGE.pfnDlgProc: the dialog procedure of the property page. In 
	the dialog proc, you can, for example, hook WM_INITDIALOG to initialize 
	the dialog, or handle the PSN_APPLY notification if the user clicks the 
	OK or Apply button.
	
	PROPSHEETPAGE.pfnCallback: gets called when the page is created and 
	destroyed. To use this call-back, you must add PSP_USECALLBACK to 
	PROPSHEETPAGE.dwFlags.

  If any value other than S_OK is returned from IShellPropSheetExt::AddPages, 
  the property sheet is not displayed.

  If the property sheet extension is not required to add any pages to the 
  property sheet, it should not call the callback function passed to 
  IShellPropSheetExt::AddPages in the pfnAddPage parameter.

  The IShellPropSheetExt::ReplacePage method is not used.

-----------
Passing the extension object to the property page.

The property sheet extension object is independent from the property page. In 
many cases, it is desirable to be able to use the extension object, or some 
other object, from the property page. To do this, set the lParam member of 
PROPSHEETPAGE structure to the object pointer. The property page can then 
retrieve this value when it processes the WM_INITDIALOG message. For a 
property page, the lParam parameter of the WM_INITDIALOG message is a pointer 
to the PROPSHEETPAGE structure. Retrieve the object pointer by casting the 
lParam of the WM_INITDIALOG message to a PROPSHEETPAGE pointer and then 
retrieving the lParam member of the PROPSHEETPAGE structure.

    INT_PTR CALLBACK FilePropPageDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, 
        LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_INITDIALOG:
            {
                // Get the pointer to the property sheet page object. This is 
                // contained in the LPARAM of the PROPSHEETPAGE structure.
                LPPROPSHEETPAGE pPage = reinterpret_cast<LPPROPSHEETPAGE>(lParam);
                if (pPage != NULL)
                {
                    // Access the property sheet extension from property page.
                    FilePropSheetExt *pExt = reinterpret_cast<FilePropSheetExt *>(pPage->lParam);
                    if (pExt != NULL)
                    {
                        // Access the property sheet extension from property page
                        // ...

                        // Store the object pointer with this particular page dialog.
                        SetProp(hWnd, EXT_POINTER_PROP, static_cast<HANDLE>(pExt));
                    }
                }
                return TRUE;
            }
        }
        ...
    }

By storing the pointer to the extension object as a property of the dialog 
window, 

    // Store the object pointer with this particular page dialog.
    SetProp(hWnd, EXT_POINTER_PROP, static_cast<HANDLE>(pExt));

we can retrieve and access the extension object in the dialog procedure. 

    // In FilePropPageDlgProc
    FilePropSheetExt *pExt = static_cast<FilePropSheetExt *>(
        GetProp(hWnd, EXT_POINTER_PROP));

Remember to remove the property when the property sheet page is being 
destroied.

    // In FilePropPageDlgProc
    case WM_DESTROY:
        {
            RemoveProp(hWnd, EXT_POINTER_PROP);
            return TRUE;
        }

Be aware that after IShellPropSheetExt::AddPages is called, the property 
sheet will release the property sheet extension object and never use it 
again. This means that the extension object would be deleted before the 
property page is displayed. When the page attempts to access the object 
pointer, the memory will have been freed and the pointer will not be valid. 
To correct this, increment the reference count for the extension object when 
the page is added and then release the object when the property page dialog 
is destroyed:

    IFACEMETHODIMP FilePropSheetExt::AddPages(LPFNADDPROPSHEETPAGE pfnAddPage, 
        LPARAM lParam)
    {
        ...
        HPROPSHEETPAGE hPage = CreatePropertySheetPage(&psp);
        ...
        if (pfnAddPage(hPage, lParam))
        {
            this->AddRef();
        }
        ...
    }

    UINT CALLBACK FilePropPageCallbackProc(HWND hWnd, UINT uMsg, LPPROPSHEETPAGE ppsp)
    {
        switch(uMsg)
        {
        ...
        case PSPCB_RELEASE:
            {
                FilePropSheetExt *pExt = reinterpret_cast<FilePropSheetExt *>(ppsp->lParam);
                if (pExt != NULL)
                {
                    pExt->Release();
                }
            }
            break;
        }
        return FALSE;
    }

-----------
Registering the handler for a certain file class:

The CLSID of the handler is declared at the beginning of dllmain.cpp.

// {5C8FA94F-F274-49B9-A5E5-D34C093F7846}
const CLSID CLSID_FilePropSheetExt = 
{ 0x5C8FA94F, 0xF274, 0x49B9, { 0xA5, 0xE5, 0xD3, 0x4C, 0x09, 0x3F, 0x78, 0x46 } };

When you write your own handler, you must create a new CLSID by using the 
"Create GUID" tool in the Tools menu, and specify the CLSID value here.

Property sheet handlers can be associated with a file class by 
registering them under the following registry key.

    HKEY_CLASSES_ROOT\<File Type>\shellex\PropertySheetHandlers

The registration of the property sheet handler is implemented in the 
DllRegisterServer function of dllmain.cpp. DllRegisterServer first calls the 
RegisterInprocServer function in Reg.h/cpp to register the COM component. 
Next, it calls RegisterShellExtPropSheetHandler to associate the handler 
with a certain file type. If the file type starts with '.', try to read the 
default value of the HKCR\<File Type> key which may contain the Program ID to 
which the file type is linked. If the default value is not empty, use the 
Program ID as the file type to proceed the registration. 

For example, this code sample associates the handler with '.cpp' files. 
HKCR\.cpp has the default value 'VisualStudio.cpp.10.0' by default when 
Visual Studio 2010 is installed, so we proceed to register the handler under 
HKCR\VisualStudio.cpp.10.0\ instead of under HKCR\.cpp. The following keys 
and values are added in the registration process of the sample handler. 

    HKCR
    {
        NoRemove CLSID
        {
            ForceRemove {5C8FA94F-F274-49B9-A5E5-D34C093F7846} = 
                s 'CppShellExtPropSheetHandler.FilePropSheetExt Class'
            {
                InprocServer32 = s '<Path of CppShellExtPropSheetHandler.DLL file>'
                {
                    val ThreadingModel = s 'Apartment'
                }
            }
        }
        NoRemove .cpp = s 'VisualStudio.cpp.10.0'
        NoRemove VisualStudio.cpp.10.0
        {
            NoRemove shellex
            {
                NoRemove PropertySheetHandlers
                {
                    {5C8FA94F-F274-49B9-A5E5-D34C093F7846} = 
                        s 'CppShellExtPropSheetHandler.FilePropSheetExt'
                }
            }
        }
    }

The unregistration is implemented in the DllUnregisterServer function of 
dllmain.cpp. It removes the HKCR\CLSID\{<CLSID>} key and the {<CLSID>} key 
under HKCR\<File Type>\shellex\PropertySheetHandlers.


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
