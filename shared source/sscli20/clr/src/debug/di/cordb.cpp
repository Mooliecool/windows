// ==++==
//
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
//
// ==--==
//*****************************************************************************
// CorDB.cpp
//
// Dll* routines for entry points, and support for COM framework.  The class
// factory and other routines live in this module.
//
//*****************************************************************************
#include "stdafx.h"
#include "classfactory.h"
#include "corsym.h"
#include "contract.h"

// Helper function returns the instance handle of this module.
HINSTANCE GetModuleInst();

//********** Globals. *********************************************************
HINSTANCE       g_hInst;


//
//
//
//
//





STDAPI CreateCordbObject(int iDebuggerVersion, IUnknown ** ppCordb)
{
    if (ppCordb == NULL)
    {
        return E_INVALIDARG;
    }
    if (iDebuggerVersion != CorDebugVersion_2_0)
    {
        return E_INVALIDARG;
    }

    return Cordb::CreateObject((CorDebugInterfaceVersion)iDebuggerVersion, IID_ICorDebug, (void **) ppCordb);
}




//*****************************************************************************
// The main dll entry point for this module.  This routine is called by the
// OS when the dll gets loaded.  Control is simply deferred to the main code.
//*****************************************************************************
BOOL WINAPI DbgDllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _DEBUG
    static int BreakOnDILoad = -1;
    if (BreakOnDILoad == -1)
        BreakOnDILoad = REGUTIL::GetConfigDWORD(L"BreakOnDILoad", 0);

    if (BreakOnDILoad)
    {
        _ASSERTE(!"DI Loaded");
    }
#endif

    // Save off the instance handle for later use.
    switch (dwReason)
    {

        case DLL_PROCESS_ATTACH:
        {
            g_hInst = hInstance;

            // Init the Win32 wrappers.
            OnUnicodeSystem();


#ifdef RSCONTRACTS
            // alloc a TLS slot
            DbgRSThread::s_TlsSlot = TlsAlloc();
            _ASSERTE(DbgRSThread::s_TlsSlot != TLS_OUT_OF_INDEXES);
#endif
        }
        break;

        case DLL_THREAD_DETACH:
        {
#ifdef STRESS_LOG
            StressLog::ThreadDetach((ThreadStressLog*) ClrFlsGetValue(TlsIdx_StressLog));
#endif

#ifdef RSCONTRACTS
            // DbgRSThread are lazily created when we call GetThread(),
            // So we don't need to do anything in DLL_THREAD_ATTACH,
            // But this is our only chance to destroy the thread object.
            DbgRSThread * p = DbgRSThread::GetThread();

            p->Destroy();
#endif
        }
        break;

        case DLL_PROCESS_DETACH:
        {
#ifdef RSCONTRACTS
            TlsFree(DbgRSThread::s_TlsSlot);
            DbgRSThread::s_TlsSlot = TLS_OUT_OF_INDEXES;
#endif

        }
        break;
    }

    return TRUE;
}

// The obsolete v1 CLSID - see comment above for details.
static const GUID CLSID_CorDebug_V1 = {0x6fef44d0,0x39e7,0x4c77, { 0xbe,0x8e,0xc9,0xf8,0xcf,0x98,0x86,0x30}};

//*****************************************************************************
// Called by COM to get a class factory for a given CLSID.  If it is one we
// support, instantiate a class factory object and prepare for create instance.
//*****************************************************************************
STDAPI DllGetClassObjectInternal(               // Return code.
    REFCLSID    rclsid,                 // The class to desired.
    REFIID      riid,                   // Interface wanted on class factory.
    LPVOID FAR  *ppv)                   // Return interface pointer here.
{
    HRESULT         hr;
    CClassFactory   *pClassFactory;         // To create class factory object.
    PFN_CREATE_OBJ  pfnCreateObject = NULL;


    if (rclsid == CLSID_CorpubPublish)
    {
        pfnCreateObject = CorpubPublish::CreateObject;
    }
    else
    if(rclsid == CLSID_CorDebug_V1)
    {
        if (0) // if (IsSingleCLR())
        {
            // Don't allow creating backwards objects until we ensure that the v2.0 Right-side
            // is backwards compat. This may involve using CordbProcess::SupportsVersion to conditionally
            // emulate old behavior.
            // If emulating V1.0, QIs for V2.0 interfaces should fail.
            _ASSERTE(!"Ensure that V2.0 RS is backwards compat");
            pfnCreateObject = Cordb::CreateObjectV1;
        }
    }

    if (pfnCreateObject == NULL)
        return (CLASS_E_CLASSNOTAVAILABLE);

    // Allocate the new factory object.
    pClassFactory = new (nothrow) CClassFactory(pfnCreateObject);
    if (!pClassFactory)
        return (E_OUTOFMEMORY);

    // Pick the v-table based on the caller's request.
    hr = pClassFactory->QueryInterface(riid, ppv);

    // Always release the local reference, if QI failed it will be
    // the only one and the object gets freed.
    pClassFactory->Release();

    return S_OK;
}



//*****************************************************************************
//
//********** Class factory code.
//
//*****************************************************************************


//*****************************************************************************
// QueryInterface is called to pick a v-table on the co-class.
//*****************************************************************************
HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(
    REFIID      riid,
    void        **ppvObject)
{
    HRESULT     hr;

    // Avoid confusion.
    *ppvObject = NULL;

    // Pick the right v-table based on the IID passed in.
    if (riid == IID_IUnknown)
        *ppvObject = (IUnknown *) this;
    else if (riid == IID_IClassFactory)
        *ppvObject = (IClassFactory *) this;

    // If successful, add a reference for out pointer and return.
    if (*ppvObject)
    {
        hr = S_OK;
        AddRef();
    }
    else
        hr = E_NOINTERFACE;
    return (hr);
}


//*****************************************************************************
// CreateInstance is called to create a new instance of the coclass for which
// this class was created in the first place.  The returned pointer is the
// v-table matching the IID if there.
//*****************************************************************************
HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(
    IUnknown    *pUnkOuter,
    REFIID      riid,
    void        **ppvObject)
{
    HRESULT     hr;

    // Avoid confusion.
    *ppvObject = NULL;
    _ASSERTE(m_pfnCreateObject);

    // Aggregation is not supported by these objects.
    if (pUnkOuter)
        return (CLASS_E_NOAGGREGATION);

    // Ask the object to create an instance of itself, and check the iid.
    hr = (*m_pfnCreateObject)(riid, ppvObject);
    return (hr);
}


HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(
    BOOL        fLock)
{
    return (S_OK);
}





//*****************************************************************************
// This helper provides access to the instance handle of the loaded image.
//*****************************************************************************
HINSTANCE GetModuleInst()
{
    return g_hInst;
}
