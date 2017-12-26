========================================================================
    COMPONENT OBJECT MODEL : CppDllCOMServer Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

COM is one of the most popular words in Windows world, there are lots of
technologies are based on it, such as: ActiveX control, WMI, even the super
star CLR is also based on the COM. 
This sample demonstrates how to create an in-process COM component by the raw
interfaces from Win32 DLL project, describes the fundamental concepts
involved.

1) An Cpp STA Simple Object short-named CppSimpleObjectSTA.

   Program ID: CppDllCOMServer.CppSimpleObjectSTA
   CLSID_CppSimpleObjectSTA: 3739576F-F27B-4857-9E3E-8BAAA2A030B9
   IID_ICppSimpleObjectSTA: 0AE6650F-C9D2-46b2-80C8-7FE10654CC93
   LIBID_CppDllCOMServerLib: D180D63C-6728-42ce-B953-885CB6E57F01

   Methods:
   // HelloWorld returns a BSTR "HelloWorld"
   HRESULT HelloWorld([out,retval] BSTR* pRet);
   // GetProcessThreadID outputs the running process ID and thread ID
   HRESULT GetProcessThreadID([out] LONG* pdwProcessId, [out] LONG* pdwThreadId);

2) Use the IDL(Interface Definition Language) to define the interface and the
   type library.
   Interface is a set of methods, the component should expose some interfaces
   for using from outside.
   Type library contains the information of the component.  We can use COM
   component by importing the type library into our project, and implement
   the early-binding in Automation with its support.
   IDL is a general way to define the interface, it will generate the
   language relative files.  After compiled it, it will generate five files
   in VC++ project:

   IDLFileName_h.h   -- Contains the definition of interfaces.
   IDLFileName_i.c   -- Contains the GUIDs, including CLSID, IID.
   IDLFileName_p.c   -- Implementation of proxy/stub.
   dlldata.c         -- Contains the entry functions of proxy/stub and some
                        data structs of the proxy class factory.
   ComponentName.tlb -- Type library.

3) IUnknown interface.
   Every COM interface must be inherited from the IUnknown interface directly
   or indirectly, we need to implement the three methods of it at first:

   // Increase the reference count for an interface on an object.
   STDMETHODIMP_(DWORD) AddRef();
   // Decrease the reference count for an interface on an object.
   STDMETHODIMP_(DWORD) Release();
   // Query to the interface the component supported.
   STDMETHODIMP QueryInterface(REFIID riid, void** ppv);

4) Class factory.
   Class factory is respondsible to the creation of the component which
   implements the interface.  A class factory object is inherited from the
   IClassObject interface, we can get this object from the client by the
   CoGetClassObject() method, then create the component by its method
   CreateInstance().

5) Global exposed functions.
   The global exposed functions are used to register/unregister the
   component, create the class factory object and check the status.
   They are:

   // Create the class factory and query to the specific interface.
   EXTERN_C STDMETHODIMP DllGetClassObject(const CLSID&clsid,
                                           const IID& iid,
                                           void **ppv);
   // Register the component to the registry.
   EXTERN_C STDMETHODIMP DllRegisterServer();
   // Unregister the component from the registry.
   EXTERN_C STDMETHODIMP DllUnregisterServer();
   // Check whether we can unload the component from the memory.
   EXTERN_C STDMETHODIMP DllCanUnloadNow(void);


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppCOMClient -> CppDllCOMServer

Currently CppCOMClient project is used to consume the ATL in-process
component SimpleObject using C++ and raw COM API.
You can simply modify the LIBID to "D180D63C-6728-42ce-B953-885CB6E57F01" in
the "ImportDirective.cpp" file to import the type library, and modify the
progID to "CppDllCOMServer.CppSimpleObjectSTA" in the "RawAPI.cpp" file to
create the CppSimpleObjectSTA component, then consume the two same methods
HelloWorld() and GetProcessThreadID().
And please remove the code of consuming property FloatProperty because we
haven't implement that property in CppDllCOMServer project.

/////////////////////////////////////////////////////////////////////////////
Deployment:

Run cmd.exe as Administrator, type below commands to register/unregiser the
component.

A. Register   -- DllRegisterServer() getting called.
Regsvr32.exe %ComponentPath%\CppDllCOMServer.dll

B. Unregister -- DllUnregisterServer() getting called.
Regsvr32.exe /u %ComponentPath%\CppDllCOMServer.dll

%ComponentPath% is the path of directory your latest DLL generated in.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Win32 DLL project named "CppDllCOMServer" from the wizard in
       VS2008, accept the default settings and click "Finish".

Step2. Add an IDL file into the project to define the interface.
       We defined a ICppSimpleObjectSTA interface which has two methods:
       HelloWorld(), GetProcessThreadID().

// Define ICppSimpleObjectSTA interface
[
    object,
    uuid(0AE6650F-C9D2-46b2-80C8-7FE10654CC93),
    helpstring("ICppSimpleObjectSTA Interface")
]
interface ICppSimpleObjectSTA : IUnknown
{
    HRESULT HelloWorld([out,retval] BSTR* pRet);
    HRESULT GetProcessThreadID([out] LONG* pdwProcessId, [out] LONG* pdwThreadId);
};

Step3. Add a class into the project to implement the ICppSimpleObjectSTA interface:

class CppSimpleObjectSTA : public ICppSimpleObjectSTA
{
public:
    // Default constructor and distructor.
    CppSimpleObjectSTA();
    virtual ~CppSimpleObjectSTA();

    // IUnknown methods.
    STDMETHODIMP_(DWORD) AddRef();
    STDMETHODIMP_(DWORD) Release();
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv);

    // ICppSimpleObjectSTA methods.
    STDMETHODIMP HelloWorld(BSTR* pRet);
    STDMETHODIMP GetProcessThreadID(LONG* pdwProcessId, LONG* pdwThreadId);

private:
    // Reference count of component.
    DWORD m_refCount;
};

Step4. Add a class into the project to implement the IClassFactory interface:

class CppSimpleObjectSTAFactory : public IClassFactory
{
public:
    // Default constructor and distructor.
    CppSimpleObjectSTAFactory();
    virtual ~CppSimpleObjectSTAFactory();

    // IUnknown methods.
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(REFIID riid,void** ppv);

    // IClassFactory methods.
    STDMETHODIMP LockServer(BOOL fLock);
    STDMETHODIMP CreateInstance(LPUNKNOWN pUnkOuter,REFIID riid,void** ppv);

private:
    // Reference count of class factory.
    ULONG m_refCount;
};

Step5. Implement the four global exposed functions.

Step6. Add a DEF file into the project to export the global exposed
       functions:

LIBRARY "CppCOMServer"
EXPORTS
DllGetClassObject   PRIVATE
DllCanUnloadNow     PRIVATE
DllRegisterServer   PRIVATE
DllUnregisterServer PRIVATE

// Standard self-registration table.
// There are two places we need to specify in the registry:
// 1. HKEY_CLASSES_ROOT\CLSID\{CLSID of Component}
// 2. HKEY_CLASSES_ROOT\ProgID of Component
// ProgID is a easy way to get the CLSID to create the COM component, we can
// get the CLSID from the ProgID through the method GetClsIdFromProgid().

Step7. Compile the project.

/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Component Object Model (COM)
http://msdn.microsoft.com/en-us/library/ms680573(VS.85).aspx

http://www.codeproject.com/KB/COM/simplecomserver.aspx


/////////////////////////////////////////////////////////////////////////////
