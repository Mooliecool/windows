/****************************** Module Header ******************************\
* Module Name:  Package.h
* Project:      CppVSGetServiceInBackgroundThread
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to get a service in a background thread.
* Services cannot be obtained by means of IServiceProvider.QueryService from
* a background thread. If you use QueryService to get a service on the main
* thread, and then try to use the service on a background thread, it also
* will fail.

* To get a service from a background thread, use
* CoMarshalInterThreadInterfaceInStream in the IVsPackage.SetSite method to
* marshal the service provider into a stream on the main thread. You then can
* unmarshal the service provider on a background thread and use it to get the
* service. You can unmarshal only once, so cache the interface that you get
* back.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/10/2009 05:00 PM Wesley Yao Created
\***************************************************************************/

#pragma once

#include <atlstr.h>
#include <VSLCommandTarget.h>


#include "resource.h"       // main symbols
#include "Guids.h"
#include "..\CppVSGetServiceInBackgroundThreadUI\Resource.h"

#include "..\CppVSGetServiceInBackgroundThreadUI\CommandIds.h"

// Step1. Import the type library at the top of the Package.h file:

// Import the type library which contains the definition of the
// ICSGlobalService interface.
#import "CSVSService.tlb" no_namespace named_guids


using namespace VSL;

UINT STDMETHODCALLTYPE ThreadProc(LPVOID pParam);

class ATL_NO_VTABLE CCppVSGetServiceInBackgroundThreadPackage : 
    // CComObjectRootEx and CComCoClass are used to implement a non-thread
    // safe COM object, and a partial implementation for IUknown (the COM
    // map below provides the rest).
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CCppVSGetServiceInBackgroundThreadPackage,
                       &CLSID_CppVSGetServiceInBackgroundThread>,
    // Provides the implementation for IVsPackage to make this COM object
    // into a VS Package.
    public IVsPackageImpl<CCppVSGetServiceInBackgroundThreadPackage,
                          &CLSID_CppVSGetServiceInBackgroundThread>,
    public IOleCommandTargetImpl<CCppVSGetServiceInBackgroundThreadPackage>,
    // Provides consumers of this object with the ability to determine which
    // interfaces support extended error information.
    public ATL::ISupportErrorInfoImpl<&__uuidof(IVsPackage)>
{
public:

    // Provides a portion of the implementation of IUnknown, in particular
    // the list of interfaces the CCppVSGetServiceInBackgroundThreadPackage
    // object will support via QueryInterface.
    BEGIN_COM_MAP(CCppVSGetServiceInBackgroundThreadPackage)
        COM_INTERFACE_ENTRY(IVsPackage)
        COM_INTERFACE_ENTRY(IOleCommandTarget)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    // COM objects typically should not be cloned, and this prevents cloning
    // by declaring the copy constructor and assignment operator private
    // (NOTE:  this macro includes the declaration of a private section, so
    // everything following this macro and preceding a public or protected 
    // section will be private).
    VSL_DECLARE_NOT_COPYABLE(CCppVSGetServiceInBackgroundThreadPackage)

public:
    CCppVSGetServiceInBackgroundThreadPackage()
    {
    }

    ~CCppVSGetServiceInBackgroundThreadPackage()
    {
    }


    // This function provides the error information if it is not possible to
    // load the UI dll. It is for this reason that the resource
    // IDS_E_BADINSTALL must be defined inside this dll's resources.
    static const LoadUILibrary::ExtendedErrorInfo&
        GetLoadUILibraryErrorInfo()
    {
        static LoadUILibrary::ExtendedErrorInfo errorInfo(IDS_E_BADINSTALL);

        return errorInfo;
    }

    // NOTE - the arguments passed to these macros cannot have names longer
    // then 30 characters.

    // Begins the registry map.  All usage of VSL_REGISTRY_MAP_*_ENTRY macros
    // need to be with-in the beginning and end of the map.  The single
    // parameter is the Resource ID for the .rgs file corresponding to this
    // package.
    VSL_BEGIN_REGISTRY_MAP(IDR_REGISTRYSCRIPT)
        // This provides the value for the
        // %CLSID_CppVSGetServiceInBackgroundThread% token in the .rgs file.
        VSL_REGISTRY_MAP_GUID_ENTRY_EX(CLSID_CppVSGetServiceInBackgroundThread,
                                       CLSID_Package)

        // Resource ID for the PLK
        VSL_REGISTRY_MAP_NUMBER_ENTRY(IDS_PACKAGE_LOAD_KEY)
        // Entries used for the Help/About dialog.
        VSL_REGISTRY_RESOURCEID_ENTRY(IDS_OFFICIALNAME)
        VSL_REGISTRY_RESOURCEID_ENTRY(IDS_PRODUCTDETAILS)


        // Ends the registry map.  All usage of VSL_REGISTRY_MAP_*_ENTRY
        // macros need to be with-in the beginning and end of the map.
        VSL_END_REGISTRY_MAP()


        // Definition of the commands handled by this package.
        VSL_BEGIN_COMMAND_MAP()

        VSL_COMMAND_MAP_ENTRY(CLSID_CppVSGetServiceInBackgroundThreadCmdSet,
                              cmdidGetServiceInBackgroundThread,
                              NULL,
                              CommandHandler::ExecHandler(&OnGetServiceInBackgroundThread))

        VSL_END_VSCOMMAND_MAP()

    // Step3. Modify the command handler, it will create a new background thread:

    // Command handler called when the user selects the "GetServiceInBackgroundThread"
    // command.
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

    // Step4. Override the SetSite() method to marshal the service provider:

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

    // Step5. Add a method to call unmarshal the service provider and consume the
    // service:

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

// Step2. Add two private members:

private:
    // Used to marshal IServiceProvider between threads
    CComPtr< IStream > m_pSPStream;
    // IServiceProvider proxy for the background thread
    CComPtr< IServiceProvider > m_pBackgroundSP;
};

// This exposes CCppVSGetServiceInBackgroundThreadPackage for instantiation via
// DllGetClassObject; however, an instance cannot be created by
// CoCreateInstance, as CCppVSGetServiceInBackgroundThreadPackage is
// specifically registered with VS, not the system in general.
OBJECT_ENTRY_AUTO(CLSID_CppVSGetServiceInBackgroundThread,
                  CCppVSGetServiceInBackgroundThreadPackage)

// Step6. Add the thread procedure:

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
