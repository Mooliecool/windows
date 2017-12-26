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
// ===========================================================================
// File: ildbsymbols.cpp
// 
// ===========================================================================

#include "pch.h"

#include "classfactory.h"

// This map contains the list of coclasses which are exported from this module.
const COCLASS_REGISTER g_CoClasses[] =
{
//  pClsid                      szProgID            pfnCreateObject
    { &CLSID_CorSymReader_SxS,    L"CorSymReader",    SymReader::NewSymReader},
    { &CLSID_CorSymWriter_SxS,    L"CorSymWriter",    SymWriter::NewSymWriter},
    { &CLSID_CorSymBinder_SxS,    L"CorSymBinder",    SymBinder::NewSymBinder},
    { NULL,                       NULL,               NULL }
};

HINSTANCE g_hInstance = NULL;

extern "C"
BOOL WINAPI DllMain(HINSTANCE hmod, DWORD dwReason, PVOID pvReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            g_hInstance = hmod;

            DisableThreadLibraryCalls(hmod);

            break;
        }
        case DLL_PROCESS_DETACH:
        {

            g_hInstance = NULL;

            break;
        }
        default:
            break;
    }

    return TRUE;
}

extern "C" STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppvObject)
{
    CClassFactory *pClassFactory;      // To create class factory object.
    const COCLASS_REGISTER *pCoClass;   // Loop control.
    HRESULT     hr = CLASS_E_CLASSNOTAVAILABLE;

    _ASSERTE(IsValidCLSID(rclsid));
    _ASSERTE(IsValidIID(riid));
    _ASSERTE(IsValidWritePtr(ppvObject, void*));

    if (ppvObject)
    {
        *ppvObject = NULL;

        // Scan for the right one.
        for (pCoClass=g_CoClasses;  pCoClass->pClsid;  pCoClass++)
        {
            if (*pCoClass->pClsid == rclsid)
            {
                // Allocate the new factory object.
                pClassFactory = NEW(CClassFactory(pCoClass));
                if (!pClassFactory)
                    return (E_OUTOFMEMORY);

                // Pick the v-table based on the caller's request.
                hr = pClassFactory->QueryInterface(riid, ppvObject);

                // Always release the local reference, if QI failed it will be
                // the only one and the object gets freed.
                pClassFactory->Release();
                break;
            }
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}


extern "C" STDAPI DllCanUnloadNow(void)
{
    return S_OK;
}

/* ------------------------------------------------------------------------- *
 * CClassFactory class
 * ------------------------------------------------------------------------- */
extern "C"
HRESULT ClassFactory_Release(
    IClassFactory *pClassFactory)
{
    if (pClassFactory)
        pClassFactory->Release();
    return NOERROR;
}

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
extern "C"
HRESULT ClassFactory_CreateInstance(
    IClassFactory   *pClassFactory,
    REFIID          riid,
    void            **ppvObject)
{
    HRESULT hr = NOERROR;
    IfFalseGo(pClassFactory, E_INVALIDARG);
    hr = pClassFactory->CreateInstance(NULL, riid, ppvObject);
ErrExit:
    return hr;
}
HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance( 
    IUnknown    *pUnkOuter,
    REFIID      riid,
    void        **ppvObject)
{
    HRESULT     hr;

    _ASSERTE(IsValidIID(riid));
    _ASSERTE(IsValidWritePtr(ppvObject, void*));

    // Avoid confusion.
    *ppvObject = NULL;
    _ASSERTE(m_pCoClass);

    // Aggregation is not supported by these objects.
    if (pUnkOuter)
        return (CLASS_E_NOAGGREGATION);

    // Ask the object to create an instance of itself, and check the iid.
    hr = (*m_pCoClass->pfnCreateObject)(riid, ppvObject);
    return (hr);
}

HRESULT STDMETHODCALLTYPE CClassFactory::LockServer( 
    BOOL        fLock)
{
    return (S_OK);
}
