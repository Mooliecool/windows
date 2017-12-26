=============================================================================
      DYNAMIC LINK LIBRARY : CppShellExtInfotipHandler Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The code sample demonstrates creating a Shell infotip handler with C++. An 
infotip handler is a shell extension handler that provides pop-up text when 
the user hovers the mouse pointer over the object. It is the most flexible 
way to customize infotips. The alternative way is to specify either a fixed 
string or a list of certain file properties to be displayed (See the Infotip 
Customization section in http://msdn.microsoft.com/en-us/library/cc144067.aspx)

The example infotip handler has the class ID (CLSID): 
    {A67511FE-371A-498D-9372-A27FDA58BE60}

It customizes the infotips of .cpp file objects. When you hover your mouse 
pointer over a .cpp file object in the Windows Explorer, you will see an 
infotip with the text:

    File: <File path, e.g. D:\Test.cpp>
    Lines: <Line number, e.g. 123 or N/A>
    - Infotip displayed by CppShellExtInfotipHandler


/////////////////////////////////////////////////////////////////////////////
Setup and Removal:

A. Setup

If you are going to use the Shell extension in a x64 Windows system, please 
configure the Visual C++ project to target 64-bit platforms using project 
configurations (http://msdn.microsoft.com/en-us/library/9yb4317s.aspx). Only 
64-bit extension DLLs can be loaded in the 64-bit Windows Shell. 

If the extension is to be loaded in a 32-bit Windows system, you can use the 
default Win32 project configuration to build the project.

In a command prompt running as administrator, navigate to the folder that 
contains the build result CppShellExtInfotipHandler.dll and enter the command:

    Regsvr32.exe CppShellExtInfotipHandler.dll

The infotip handler is registered successfully if you see a message box 
saying:

    "DllRegisterServer in CppShellExtInfotipHandler.dll succeeded."

B. Removal

In a command prompt running as administrator, navigate to the folder that 
contains the build result CppShellExtInfotipHandler.dll and enter the command:

    Regsvr32.exe /u CppShellExtInfotipHandler.dll

The infotip handler is unregistered successfully if you see a message box 
saying:

    "DllUnregisterServer in CppShellExtInfotipHandler.dll succeeded."


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the infotip handler code 
sample.

Step1. If you are going to use the Shell extension in a x64 Windows system, 
please configure the Visual C++ project to target 64-bit platforms using 
project configurations (http://msdn.microsoft.com/en-us/library/9yb4317s.aspx). 
Only 64-bit extension DLLs can be loaded in the 64-bit Windows Shell. 

If the extension is to be loaded in a 32-bit Windows system, you can use the 
default Win32 project configuration.

Step2. After you successfully build the sample project in Visual Studio 2010, 
you will get a DLL: CppShellExtInfotipHandler.dll. Start a command prompt as 
administrator, navigate to the folder that contains the file and enter the 
command:

    Regsvr32.exe CppShellExtInfotipHandler.dll

The infotip handler is registered successfully if you see a message box saying:

    "DllRegisterServer in CppShellExtInfotipHandler.dll succeeded."

Step3. Find a .cpp file in the Windows Explorer (e.g. FileInfotipExt.cpp in 
the sample folder), and hover the mouse pointer over it. you will see an 
infotip with the text:

    File: <File path, e.g. D:\CppShellExtInfotipHandler\FileInfotipExt.cpp>
    Lines: <Line number, e.g. 123 or N/A>
    - Infotip displayed by CppShellExtInfotipHandler

Step4. In the same command prompt, run the command 

    Regsvr32.exe /u CppShellExtInfotipHandler.dll

to unregister the Shell infotip handler.


/////////////////////////////////////////////////////////////////////////////
Implementation:

A. Creating and configuring the project

In Visual Studio 2010, create a Visual C++ / Win32 / Win32 Project named 
"CppShellExtInfotipHandler". In the "Application Settings" page of Win32 
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

  FileInfotipExt.h/cpp - defines the COM class. You can find the basic 
    implementation of the IUnknown interface in the files.

  ClassFactory.h/cpp - defines the class factory for the COM class. 

-----------------------------------------------------------------------------

C. Implementing the infotip handler and registering it for a certain file 
class

-----------
Implementing the infotip handler:

The FileInfotipExt.h/cpp files define an infotip handler. Infotip handlers 
must implement the IQueryInfo interface to create text for the tooltip, and 
implement the IPersistFile interface for initialization.

    class FileInfotipExt : public IPersistFile, public IQueryInfo
    {
    public:
        // IPersistFile
        IFACEMETHODIMP GetClassID(CLSID *pClassID);
        IFACEMETHODIMP IsDirty(void);
        IFACEMETHODIMP Load(LPCOLESTR pszFileName, DWORD dwMode);
        IFACEMETHODIMP Save(LPCOLESTR pszFileName, BOOL fRemember);
        IFACEMETHODIMP SaveCompleted(LPCOLESTR pszFileName);
	    IFACEMETHODIMP GetCurFile(LPOLESTR *ppszFileName);

        // IQueryInfo
        IFACEMETHODIMP GetInfoTip(DWORD dwFlags, LPWSTR *ppwszTip);
        IFACEMETHODIMP GetInfoFlags(DWORD *pdwFlags);
    };
	
  1. Implementing IPersistFile

  The Shell queries the extension for IPersistFile and calls its Load method 
  passing the file name of the item over which mouse is placed. 
  IPersistFile::Load opens the specified file and initializes the wanted data. 
  In this code sample, we save the absolute path of the file.

    IFACEMETHODIMP FileInfotipExt::Load(LPCOLESTR pszFileName, DWORD dwMode)
    {
        // pszFileName contains the absolute path of the file to be opened.
        return StringCchCopy(
            m_szSelectedFile, 
            ARRAYSIZE(m_szSelectedFile), 
            pszFileName);
    }

  2. Implementing IQueryInfo

  After IPersistFile is queried, the Shell queries the IQueryInfo interface 
  and the GetInfoTip method is called. GetInfoTip has an out parameter 
  ppwszTip of the type LPWSTR * which recieves the address of the tool tip 
  buffer. Please note that the memory pointed by *ppwszTip must be allocated 
  by calling CoTaskMemAlloc. Shell knows to call CoTaskMemFree to free the 
  memory when the info tip is no longer needed.

  In this code sample, the example infotip is composed of the file path and 
  the count of text lines.

    const int cch = MAX_PATH + 512;
    *ppwszTip = static_cast<LPWSTR>(CoTaskMemAlloc(cch * sizeof(wchar_t)));
    if (*ppwszTip == NULL)
    {
        return E_OUTOFMEMORY;
    }

    // Prepare the text of the infotip. The example infotip is composed of 
    // the file path and the count of code lines.
    wchar_t szLineNum[50];
    ...

    HRESULT hr = StringCchPrintf(*ppwszTip, cch, 
        L"File: %s\nLines: %s\n- Infotip displayed by CppShellExtInfotipHandler", 
        m_szSelectedFile, szLineNum);
    if (FAILED(hr))
    {
        CoTaskMemFree(*ppwszTip);
    }

The IQueryInfo::GetInfoFlags method is not currently used. We simply return 
E_NOTIMPL from the method.

-----------
Registering the handler for a certain file class:

The CLSID of the handler is declared at the beginning of dllmain.cpp.

// {A67511FE-371A-498D-9372-A27FDA58BE60}
const CLSID CLSID_FileInfotipExt = 
{ 0xA67511FE, 0x371A, 0x498D, { 0x93, 0x72, 0xA2, 0x7F, 0xDA, 0x58, 0xBE, 0x60 } };

When you write your own handler, you must create a new CLSID by using the 
"Create GUID" tool in the Tools menu, and specify the CLSID value here.

Infotip handlers can be associated with a file class. The handlers are 
registered under the following subkey.

    HKEY_CLASSES_ROOT\<File Type>\shellex\{00021500-0000-0000-C000-000000000046}

The registration of the infotip handler is implemented in the 
DllRegisterServer function of dllmain.cpp. DllRegisterServer first calls the 
RegisterInprocServer function in Reg.h/cpp to register the COM component. 
Next, it calls RegisterShellExtInfotipHandler to associate the handler with a 
certain file type. If the file type starts with '.', it tries to read the 
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
            ForceRemove {A67511FE-371A-498D-9372-A27FDA58BE60} = 
                s 'CppShellExtInfotipHandler.FileInfotipExt Class'
            {
                InprocServer32 = s '<Path of CppShellExtInfotipHandler.DLL file>'
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
                {00021500-0000-0000-C000-000000000046} = 
                    s '{A67511FE-371A-498D-9372-A27FDA58BE60}'
            }
        }
    }

The unregistration is implemented in the DllUnregisterServer function of 
dllmain.cpp. It removes the HKCR\CLSID\{<CLSID>} key and the 
HKCR\<File Type>\shellex\{00021500-0000-0000-C000-000000000046} key.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Initializing Shell Extensions
http://msdn.microsoft.com/en-us/library/cc144105.aspx

MSDN: IQueryInfo Interface
http://msdn.microsoft.com/en-us/library/bb761359.aspx

The Complete Idiot's Guide to Writing Shell Extensions - Part III
http://www.codeproject.com/KB/shell/ShellExtGuide3.aspx


/////////////////////////////////////////////////////////////////////////////