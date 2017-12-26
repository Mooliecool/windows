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
#ifndef ASMIMPRT_H
#define ASMIMPRT_H

#include "fusionp.h"


HRESULT
CreateAssemblyManifestImport(        
    LPCTSTR szManifestFilePath,
    LPASSEMBLY_MANIFEST_IMPORT *ppImport);

HRESULT
CreateAssemblyManifestImport(
    LPCTSTR szManifestFilePath,
    IMetaDataAssemblyImport *pMDImport,
    LPASSEMBLY_MANIFEST_IMPORT *ppImport);


class CAssemblyManifestImport : public IAssemblyManifestImport 
{
public:
    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    STDMETHOD(GetAssemblyNameDef)( 
        /* out */ LPASSEMBLYNAME *ppName);

    STDMETHOD(GetNextAssemblyNameRef)( 
        /* in  */ DWORD nIndex,
        /* out */ LPASSEMBLYNAME *ppName);

    STDMETHOD(GetNextAssemblyModule)( 
        /* in  */ DWORD nIndex,
        /* out */ LPASSEMBLY_MODULE_IMPORT *ppImport);
        
    STDMETHOD(GetModuleByName)( 
        /* in  */ LPCOLESTR pszModuleName,
        /* out */ LPASSEMBLY_MODULE_IMPORT *ppImport);

    STDMETHOD(GetManifestModulePath)( 
        /* out     */ __out_ecount_opt(*pccModulePath) LPOLESTR  pszModulePath,
        /* in, out */ LPDWORD   pccModulePath);

    CAssemblyManifestImport();
    ~CAssemblyManifestImport();

    HRESULT Init(LPCTSTR szManifestFilePath);
    HRESULT Init(LPCTSTR szManifestFilePath, IMetaDataAssemblyImport *pMDImport);

    HRESULT SetManifestModulePath(LPCWSTR pszModulePath);

    BOOL IsInGAC() { return IsGacType(_dwCacheType); }

    HRESULT GetInternalMDImport(IMetaDataAssemblyImport **ppMDImport) 
    {
        *ppMDImport = _pMDImport;
        if (_pMDImport) {
            _pMDImport->AddRef();
        }

        return S_OK;
    }

private:
    HRESULT CopyMetaData();
    HRESULT CopyNameDef(IMetaDataAssemblyImport *pMDImport);
    HRESULT CopyModuleRefs(IMetaDataAssemblyImport *pMDImport);

    HRESULT InternalSetManifestModulePath(LPCWSTR pwzModulePath);

private:
    DWORD                    _dwSig;
    LONG                     _cRef;
    LPTSTR                   _pszManifestFilePath;
    DWORD                    _dwCacheType;
    BOOL                     _isDll;
    LPASSEMBLYNAME           _pName;
    IMetaDataAssemblyImport *_pMDImport;
    IAssemblyModuleImport   **_ppModules;
    DWORD                     _dwNumModules;
    mdAssemblyRef           *_rAssemblyRefTokens;
    DWORD                    _cAssemblyRefTokens;
};

#endif // ASMIMPRT_H

