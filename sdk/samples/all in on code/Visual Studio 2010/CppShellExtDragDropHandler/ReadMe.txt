=============================================================================
    DYNAMIC LINK LIBRARY : CppShellExtDragDropHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates creating a Shell drag-and-drop handler with C++. 

When a user right-clicks a Shell object to drag an object, a context menu is 
displayed when the user attempts to drop the object. A drag-and-drop handler 
is a context menu handler that can add items to this context menu.

The example drag-and-drop handler has the class ID (CLSID): 
    {F574437A-F944-4350-B7E9-95B6C7008029}

It adds the menu item "Create hard link here" to the context menu. When you 
right-click a file and drag the file to a directory or a drive or the 
desktop, a context menu will be displayed with the "Create hard link here" 
menu item. By clicking the menu item, the handler will create a hard link for 
the dragged file in the dropped location. The name of the link is "Hard link 
to <source file name>". 

(A hard link is the file system representation of a file by which more than 
one path references a single file in the same volume. Any changes to that 
file are instantly visible to applications that access it through the hard 
links that reference it. For more information about hard links, refer to the 
MSDN documentation: http://msdn.microsoft.com/en-us/library/aa365006.aspx.)


/////////////////////////////////////////////////////////////////////////////
Setup and Removal:

A. Setup

If you are going to use the Shell extension in a x64 Windows system, please 
configure the Visual C++ project to target 64-bit platforms using project 
configurations (http://msdn.microsoft.com/en-us/library/9yb4317s.aspx). Only 
64-bit extension DLLs can be loaded in the 64-bit Windows Shell. If the 
extension is to be loaded in a 32-bit Windows system, you can use the default 
Win32 project configuration to build the project.

In a command prompt running as administrator, navigate to the folder that 
contains the build result CppShellExtDragDropHandler.dll and enter the 
command:

    Regsvr32.exe CppShellExtDragDropHandler.dll

The drag-and-drop handler is registered successfully if you see a message box 
saying:

    "DllRegisterServer in CppShellExtDragDropHandler.dll succeeded."

B. Removal

In a command prompt running as administrator, navigate to the folder that 
contains the build result CppShellExtDragDropHandler.dll and enter the 
command:

    Regsvr32.exe /u CppShellExtDragDropHandler.dll

The drag-and-drop handler is unregistered successfully if you see a message 
box saying:

    "DllUnregisterServer in CppShellExtDragDropHandler.dll succeeded."


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the drag-and-drop 
handler code sample.

Step1. If you are going to use the Shell extension in a x64 Windows system, 
please configure the Visual C++ project to target 64-bit platforms using 
project configurations (http://msdn.microsoft.com/en-us/library/9yb4317s.aspx). 
Only 64-bit extension DLLs can be loaded in the 64-bit Windows Shell. If the 
extension is to be loaded in a 32-bit Windows system, you can use the default 
Win32 project configuration.

Step2. After you successfully build the sample project in Visual Studio 2010, 
you will get a DLL: CppShellExtDragDropHandler.dll. Start a command prompt 
as administrator, navigate to the folder that contains the file and enter the 
command:

    Regsvr32.exe CppShellExtDragDropHandler.dll

The drag-and-drop handler is registered successfully if you see a message 
box saying:

    "DllRegisterServer in CppShellExtDragDropHandler.dll succeeded."

Step3. Find a file in the Windows Explorer (e.g. ReadMe.txt in the sample 
folder), right-click the file and drag it to a directory in the same volume. 
A context menu will be displayed with the "Create hard link here" menu item. 
By clicking the menu item, the handler will create a hard link for the 
dragged file in the dropped location. The name of the link is "Hard link 
to <source file name>" (e.g. "Hard link to ReadMe.txt"). Any changes to the 
source file are instantly visible to applications that access it through the 
hard link that reference it. If there is already a file with the same name in 
dropped location, you will see an error message "There is already a file with 
the same name in this location." prompted by the Windows Shell.

Step4. In the same command prompt, run the command 

    Regsvr32.exe /u CppShellExtDragDropHandler.dll

to unregister the Shell drag-and-drop handler.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Creating and configuring the project

In Visual Studio 2010, create a Visual C++ / Win32 / Win32 Project named 
"CppShellExtDragDropHandler". In the "Application Settings" page of Win32 
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

  FileDragDropExt.h/cpp - defines the COM class. You can find the basic 
    implementation of the IUnknown interface in the files.

  ClassFactory.h/cpp - defines the class factory for the COM class. 

-----------------------------------------------------------------------------

C. Implementing the drag-and-drop handler and registering it.

-----------
Implementing the drag-and-drop handler:

A drag-and-drop handler is a context menu handler that can add items to this 
context menu. The basic procedure for implementing a drag-and-drop handler is 
the same as for conventional context menu handlers.

The FileDragDropExt.h/cpp files define a drag-and-drop handler. The 
drag-and-drop handler must implement the IShellExtInit and IContextMenu 
interfaces. 

    class FileDragDropExt : public IShellExtInit, public IContextMenu
    {
    public:
        // IShellExtInit
        IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT 
            pDataObj, HKEY hKeyProgID);

        // IContextMenu
        IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, 
            UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
        IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
        IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, 
            UINT *pwReserved, LPSTR pszName, UINT cchMax);
    };

  1. Implementing IShellExtInit

  After the drag-and-drop extension COM object is instantiated, the 
  IShellExtInit::Initialize method is called. Among the parameters of the 
  method, pidlFolder is the PIDL of the folder where the files were dropped. 
  you can get the directory from the PIDL by calling SHGetPathFromIDList.

  pDataObj is an IDataObject interface pointer through which we retrieve the 
  names of the files being dragged. 
  
  If any value other than S_OK is returned from IShellExtInit::Initialize, 
  the drag-and-drop menu item will not be used.

  In the code sample, the FileDragDropExt::Initialize method retrieves the 
  target directory and saves the directory path in the member variable:
  m_szTargetDir.
  
    // Get the directory where the file is dropped to.
    if (!SHGetPathFromIDList(pidlFolder, this->m_szTargetDir))
    {
		return E_FAIL;
    }

  It also enumerates the selected files and folders. If only *one* file is 
  dragged, and the file is not a directory considering hard links do not work 
  for directories, the method stores the file name for later use and returns 
  S_OK to proceed. 

  2. Implementing IContextMenu

  After IShellExtInit::Initialize returns S_OK, the 
  IContextMenu::QueryContextMenu method is called to obtain the menu item or 
  items that the drag-and-drop extension will add. The QueryContextMenu 
  implementation is fairly straightforward. The drag-and-drop extension adds 
  its menu items using the InsertMenuItem or similar functions. The menu 
  command identifiers must be greater than or equal to idCmdFirst and must be 
  less than idCmdLast. QueryContextMenu must return the greatest numeric 
  identifier added to the menu plus one. The best way to assign menu command 
  identifiers is to start at zero and work up in sequence. If the drag-and-
  drop extension does not need to add any items to the menu, it should simply 
  return zero from QueryContextMenu.

  In this code sample, we insert the menu item "Create hard link here".

  IContextMenu::GetCommandString is called to retrieve textual data for the 
  menu item, such as help text to be displayed for the menu item. In this 
  drag-and-drop handler example, we do not need the help string or verb 
  string for the command, so we simply return E_NOTIMPL from GetCommandString.  

  IContextMenu::InvokeCommand is called when one of the menu items installed 
  by the extension is selected. The handler establishes a hard link between 
  the dragged file and a new file in the dropped location in response to the 
  "Create hard link here" command. 

-----------
Registering the handler:

The CLSID of the handler is declared at the beginning of dllmain.cpp.

// {F574437A-F944-4350-B7E9-95B6C7008029}
const CLSID CLSID_FileDragDropExt = 
{ 0xF574437A, 0xF944, 0x4350, { 0xB7, 0xE9, 0x95, 0xB6, 0xC7, 0x00, 0x80, 0x29 } };

When you write your own handler, you must create a new CLSID by using the 
"Create GUID" tool in the Tools menu, and specify the CLSID value here.

Drag-and-drop handlers are typically registered under the following subkey:

    HKEY_CLASSES_ROOT\Directory\shellex\DragDropHandlers\

In some cases, you also need to register it under HKCR\Folder to handle drops 
on the desktop, and HKCR\Drive to handle drops in root directories. 

The registration of the drag-and-drop handler is implemented in the 
DllRegisterServer function of dllmain.cpp. DllRegisterServer first calls the 
RegisterInprocServer function in Reg.h/cpp to register the COM component. 
Next, it calls RegisterShellExtDragDropHandler to register the drag-and-drop 
handler under HKEY_CLASSES_ROOT\Directory\shellex\DragDropHandlers\, 
HKEY_CLASSES_ROOT\Folder\shellex\DragDropHandlers\, and 
HKEY_CLASSES_ROOT\Drive\shellex\DragDropHandlers\.

The following keys and values are added in the registration process of the 
sample handler. 

    HKCR
    {
        NoRemove CLSID
        {
            ForceRemove {F574437A-F944-4350-B7E9-95B6C7008029} = 
                s 'CppShellExtDragDropHandler.FileDragDropExt Class'
            {
                InprocServer32 = s '<Path of CppShellExtDragDropHandler.DLL file>'
                {
                    val ThreadingModel = s 'Apartment'
                }
            }
        }
        NoRemove Directory
        {
            NoRemove shellex
            {
                NoRemove DragDropHandlers
                {
                    {F574437A-F944-4350-B7E9-95B6C7008029} = 
                        s 'CppShellExtDragDropHandler.FileDragDropExt'
                }
            }
        }
        NoRemove Folder
        {
            NoRemove shellex
            {
                NoRemove DragDropHandlers
                {
                    {F574437A-F944-4350-B7E9-95B6C7008029} = 
                        s 'CppShellExtDragDropHandler.FileDragDropExt'
                }
            }
        }
        NoRemove Drive
        {
            NoRemove shellex
            {
                NoRemove DragDropHandlers
                {
                    {F574437A-F944-4350-B7E9-95B6C7008029} = 
                        s 'CppShellExtDragDropHandler.FileDragDropExt'
                }
            }
        }
    }

The unregistration is implemented in the DllUnregisterServer function of 
dllmain.cpp. It removes the HKCR\CLSID\{<CLSID>} key and the {<CLSID>} key 
under HKCR\Directory\shellex\ContextMenuHandlers, 
HKCR\Folder\shellex\ContextMenuHandlers, and 
HKCR\Drive\shellex\ContextMenuHandlers.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Creating Drag-and-Drop Handlers
http://msdn.microsoft.com/en-us/library/bb776881.aspx#dragdrop

The Complete Idiot's Guide to Writing Shell Extensions - Part IV
http://www.codeproject.com/KB/shell/shellextguide4.aspx


/////////////////////////////////////////////////////////////////////////////