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
// File: symbinder.cpp
// 
// ===========================================================================

#include "pch.h"
#include "symbinder.h"

/* ------------------------------------------------------------------------- *
 * SymBinder class
 * ------------------------------------------------------------------------- */

extern "C"
HRESULT SymBinder_Release(ISymUnmanagedBinder *pBinder)
{
    if (pBinder)
        pBinder->Release();;
    return NOERROR;
}

HRESULT
SymBinder::QueryInterface(
    REFIID riid,
    void **ppvObject
    )
{
    HRESULT hr = S_OK;

    _ASSERTE(IsValidIID(riid));
    _ASSERTE(IsValidWritePtr(ppvObject, void*));

    IfFalseGo( ppvObject, E_INVALIDARG );

    if (riid == IID_ISymUnmanagedBinder)
    {
        *ppvObject = (ISymUnmanagedBinder*) this;
    }
    else if (riid == IID_IUnknown)
    {
        *ppvObject = (IUnknown*)this;
    }
    else
    {
        *ppvObject = NULL;
        hr = E_NOINTERFACE;
    }

    if (*ppvObject)
    {
        AddRef();
    }

ErrExit:
    
    return hr;
}

HRESULT
SymBinder::NewSymBinder(
    REFCLSID clsid,
    void** ppObj
    )
{
    HRESULT hr = S_OK;
    SymBinder* pSymBinder = NULL;

    _ASSERTE(IsValidCLSID(clsid));
    _ASSERTE(IsValidWritePtr(ppObj, IUnknown*));

    if (clsid != IID_ISymUnmanagedBinder)
        return (E_UNEXPECTED);

    IfFalseGo( ppObj, E_INVALIDARG );
    
    *ppObj = NULL;
    
    IfNullGo( pSymBinder = NEW(SymBinder()) );
    *ppObj = pSymBinder;
    pSymBinder->AddRef();
    pSymBinder = NULL;
    
ErrExit:

    RELEASE( pSymBinder );
    
    return hr;
}

//-----------------------------------------------------------
// GetReaderForFile
//-----------------------------------------------------------
extern "C"
HRESULT SymBinder_GetReaderForFile(
    ISymUnmanagedBinder *pBinder,                  // SymBinder
    IUnknown *importer,             // IMetaDataImporter
    const WCHAR *fileName,          // File we're looking symbols for
    const WCHAR *searchPath,        // Search path for file
    ISymUnmanagedReader **ppRetVal) // Out: SymReader for file
{
    HRESULT hr = NOERROR;
    IfFalseGo(pBinder, E_INVALIDARG);
    hr = pBinder->GetReaderForFile(importer, fileName, searchPath, ppRetVal);
ErrExit:
    return hr;
}
HRESULT
SymBinder::GetReaderForFile(
    IUnknown *importer,             // IMetaDataImporter
    const WCHAR *fileName,          // File we're looking symbols for
    const WCHAR *searchPath,        // Search path for file
    ISymUnmanagedReader **ppRetVal) // Out: SymReader for file
{

    HRESULT hr = S_OK;
    ISymUnmanagedReader *pSymReader = NULL;
    IfFalseGo( ppRetVal && fileName && fileName[0] != '\0', E_INVALIDARG );

    // Init Out parameter
    *ppRetVal = NULL;

    IfFailGo(PAL_CoCreateInstance(CLSID_CorSymReader_SxS,
                                  IID_ISymUnmanagedReader,
                                  (void**)&pSymReader));
                                            
    IfFailGo(pSymReader->Initialize(importer, fileName, searchPath, NULL));

    // Transfer ownership to the out parameter 
    *ppRetVal = pSymReader;
    pSymReader = NULL;

ErrExit:
    RELEASE(pSymReader);
    return hr;
}

HRESULT
SymBinder::GetReaderFromStream(
    IUnknown *importer,
    IStream *pStream,
    ISymUnmanagedReader **ppRetVal
    )
{
    HRESULT hr = S_OK;
    ISymUnmanagedReader *pSymReader = NULL;
    IfFalseGo( ppRetVal && importer && pStream, E_INVALIDARG );

    // Init Out parameter
    *ppRetVal = NULL;

    IfFailGo(PAL_CoCreateInstance(CLSID_CorSymReader_SxS,
                                  IID_ISymUnmanagedReader,
                                  (void**)&pSymReader));
                                            
    IfFailGo(pSymReader->Initialize(importer, NULL, NULL, pStream));

    // Transfer ownership to the out parameter 
    *ppRetVal = pSymReader;
    pSymReader = NULL;

ErrExit:
    RELEASE(pSymReader);
    return hr;
}
