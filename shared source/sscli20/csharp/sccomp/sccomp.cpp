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
// File: sccomp.cpp
//
// Defines the entry point for the DLL application.
// ===========================================================================

#include "stdafx.h"

HINSTANCE hModuleMessages = 0; // Module error messages are loaded from.
LANGID uiLangId = (LANGID)-1;

////////////////////////////////////////////////////////////////////////////////
// GetMesageDll -- find and load the message DLL. Returns 0 if the message
// DLL could not be located or loaded. The message DLL should be either in a
// subdirectory whose name is a language id, or in the current directory. We have
// a complex set of rules to figure out which language ids to try.
HINSTANCE STDMETHODCALLTYPE GetMessageDll_Impl ()
{
    return GetMessageDllWorker(hModuleMessages, (LANGID) -1, MESSAGE_DLLA, &uiLangId);
}

// Some ATL goo
BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_CSCompilerFactory, CFactory)
END_OBJECT_MAP()

CComModule & GetATLModule() 
{
    return _Module;
}

////////////////////////////////////////////////////////////////////////////////
// CreateCompilerFactory

HRESULT STDMETHODCALLTYPE CreateCompilerFactory_Impl (ICSCompilerFactory **ppFactory)
{
    HRESULT                 hr = E_OUTOFMEMORY;
    CComObject<CFactory>    *pObj;

    if (SUCCEEDED (hr = CComObject<CFactory>::CreateInstance (&pObj)))
    {
        if (FAILED (hr = pObj->QueryInterface (IID_ICSCompilerFactory, (void **)ppFactory)))
        {
            delete pObj;
        }
    }

    return hr;
}


HINSTANCE STDMETHODCALLTYPE GetMessageDll()
{
    return GetMessageDll_Impl();
}

HRESULT STDMETHODCALLTYPE CreateCompilerFactory (ICSCompilerFactory **ppFactory)
{
    return CreateCompilerFactory_Impl(ppFactory);
}

HRESULT STDMETHODCALLTYPE InMemoryCompile(LPCWSTR * pszText, const COMPILEPARAMS * params)
{
    return InMemoryCompile_Impl(pszText, params);
}

CComModule  _Module;
#include "dllmain.cpp"
