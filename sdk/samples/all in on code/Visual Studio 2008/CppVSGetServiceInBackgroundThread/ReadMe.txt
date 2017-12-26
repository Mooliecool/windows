========================================================================================
    VISUAL STUDIO EXTENSIBILITY : CppVSGetServiceInBackgroundThread Project Overview
========================================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample demonstrates how to get a service in a background thread.
Services cannot be obtained by means of IServiceProvider.QueryService from a
background thread. If you use QueryService to get a service on the main
thread, and then try to use the service on a background thread, it also will
fail.

To get a service from a background thread, use
CoMarshalInterThreadInterfaceInStream in the IVsPackage.SetSite method to
marshal the service provider into a stream on the main thread. You then can
unmarshal the service provider on a background thread and use it to get the
service. You can unmarshal only once, so cache the interface that you get
back.

Note:
Managed code automatically marshals interfaces between threads, so getting
a service from a background thread does not require special code.


/////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-
867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

If you run this project on a x64 OS, please also config the Debug tab of the
project setting. Set the "Start external program" to 
C:\Program Files(x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe

In VC++ projects, please go to solution folder and open VsSDK.vsprops file. 
Change the VSIntegrationRoot value to:
C:\Program Files(X86)\Microsoft Visual Studio 2008 SDK\VisualStudioIntegration

NOTE: The Package Load Failure Dialog occurs because there is no
      PLK(Package Load Key) Specified in this package. To obtain a PLK, please
      to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info:
      http://msdn.microsoft.com/en-us/library/bb165395.aspx


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppVSGetServiceInBackgroundThread -> CSVSService
CppVSGetServiceInBackgroundThread consumes the global service defined in
CSVSService project.


/////////////////////////////////////////////////////////////////////////////
Build:

CppVSGetServiceInBackgroundThread depends on the CSVSService.  To build and run
CppVSGetServiceInBackgroundThread successfully, please make sure CSVSService is
built and registered rightly.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Visual Studio Integration Package project from the New
Project dialog named CppVSGetServiceInBackgroundThread, choose Visual C++ as
the development language.
Check the Menu Command checkbox to create a menu command, we create the
background thread to consume the service in its handler.

Step2. Generate the type library from the assembly CSVSService.dll created in
the CSVSService project.  Type below command in the Command Prompt:

regasm %CSVSServiceAssemblyPath%\CSVSService.dll /tlb

%CSVSServiceAssemblyPath% is the folder which the assembly locates in.
This command will generate a .tlb file which is the type library of the
services in the %AssemblyPath%.  This is necessary to get the information
about the services and interfaces.
Copy the CSVSService.tlb file to the CppVSGetServiceInBackgroundThread folder.

Step3. The C++ Integration Package project template will generate two
projects: CppVSGetServiceInBackgroundThread and
CppVSGetServiceInBackgroundThreadUI.  Expand the
CppVSGetServiceInBackgroundThread project node, open the Package.h file, it
contains the definition of the package.

Step4. Import the type library at the top of the Package.h file:
#import "CSVSService.tlb" no_namespace named_guids

Step5. Add two private members:

private:
    // Used to marshal IServiceProvider between threads
    CComPtr< IStream > m_pSPStream;
    // IServiceProvider proxy for the background thread
    CComPtr< IServiceProvider > m_pBackgroundSP;

Step6. Modify the command handler, it will create a new background thread:

// Command handler called when the user selects the
// "GetServiceInBackgroundThread" command.
void OnGetServiceInBackgroundThread(
CommandHandler* /*pSender*/,
DWORD /*flags*/,
VARIANT* /*pIn*/,
VARIANT* /*pOut*/)
{
    HANDLE hThread = NULL;
    // Create the background thread which will query for the
    // ICSGlobalService interface will defined in CSVSService project and
    // call its method.
    hThread = (HANDLE)(_beginthreadex(
        NULL,
        0,
        &ThreadProc,
        (LPVOID)this,
        0,
        NULL));
    if (NULL == hThread)
    {
        MessageBox(
            NULL,
            TEXT("Create thread failed!"),
            TEXT("CppVSGetServiceInBackgroundThread"),
            MB_OK);

        return;
    }
    // Wait for the thread end.
    // Because WaitForSingleObject() will break the UI messages handling
    // that hands the Visual Studio, so here we use
    // MsgWaitForMultipleObjects(), it can handle the UI messages at the
    // same time.
    MsgWaitForMultipleObjects(1, &hThread, FALSE, 0, INFINITE);
    CloseHandle(hThread);
}

Step7. Override the SetSite() method to marshal the service provider:

HRESULT STDMETHODCALLTYPE SetSite(IServiceProvider *pSP)
{
    // Marshal the service provider into a stream so that the background
    // thread can retrieve it later.
    CoMarshalInterThreadInterfaceInStream(
        IID_IServiceProvider,
        pSP,
        &m_pSPStream);

    return S_OK;
}

Step8. Add a method to call unmarshal the service provider and consume the
service:

// Call this when your background thread needs to call QueryService.
// The first time through, it unmarshals the interface stored.
HRESULT QueryServiceFromBackgroundThread(
    REFGUID rsid,        // [in] Service ID
    REFIID riid,         // [in] Interface ID
    // [out] Interface pointer of requested service (NULL on error)
    void **ppvObj)
{
    if(!m_pBackgroundSP)
    {
        if(!m_pSPStream)
        {
            return E_UNEXPECTED;
        }

        HRESULT hr = CoGetInterfaceAndReleaseStream(
            m_pSPStream,
            IID_IServiceProvider,
            (void **)&m_pBackgroundSP);
        if(FAILED(hr))
        {
            return hr;
        }

        // The CoGetInterfaceAndReleaseStream has already destroyed the
        // stream.  To avoid double-freeing, the smart wrapper needs to
        // be detached.
        m_pSPStream.Detach();
    }

    return m_pBackgroundSP->QueryService(rsid, riid, ppvObj);
}

Step9. Add the thread procedure:

UINT STDMETHODCALLTYPE ThreadProc(LPVOID pParam)
{
    // Guid of the service SCSGlobalService defined in CSVSService project.
    GUID guidService = {0x81099ee5, 0xd61c, 0x4cd7,
                        {0xbf, 0x44,0xcb, 0x5c, 0x48, 0x66, 0x7b, 0x49}};
    ICSGlobalService *ppvICSGlobalService = NULL;
    // Call the QueryServiceFromBackgroundThread() method to unmarshal the
    // service provider and get the ICSGlobalService interface.
    ((CCppVSGetServiceInBackgroundThreadPackage*)pParam)->
        QueryServiceFromBackgroundThread(
        guidService,
        IID_ICSGlobalService,
        (void**)&ppvICSGlobalService);

    if (NULL == ppvICSGlobalService)
    {
        MessageBox(
            NULL,
            TEXT("Query interface falied!"),
            TEXT("CppVSGetServiceInBackgroundThread"),
            MB_OK);

        return -1;
    }

    // Call the GlobalServiceMethod() method of ICSGlobalService interface.
    ppvICSGlobalService->GlobalServiceMethod();

    return 0;
}


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Services
http://msdn.microsoft.com/en-us/library/bb166389.aspx


/////////////////////////////////////////////////////////////////////////////
