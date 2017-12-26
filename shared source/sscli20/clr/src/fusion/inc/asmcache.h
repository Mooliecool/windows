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
#ifndef _ASMCACHE_
#define _ASMCACHE_

#include <windows.h>
#include <winerror.h>
#include "fusionp.h"
#include "cache.h"


#define STREAM_FORMAT_MANIFEST STREAM_FORMAT_COMPLIB_MANIFEST
#define STREAM_FORMAT_MODULE   STREAM_FORMAT_COMPLIB_MODULE

class CTransCache;
class CDebugLog;

// Top-level apis used internally by fusion.
HRESULT CopyAssemblyFile
    (IAssemblyCacheItem *pasm, LPCOLESTR pszSrcFile, DWORD dwFlags);

HRESULT ValidateAssembly(LPCTSTR pszManifestFilePath, IAssemblyName *pName);

// CAssemblyCache declaration.
class CAssemblyCache : public IAssemblyCache
{
public:

    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    // IAssemblyCache methods
    STDMETHOD (UninstallAssembly)(
        /* [in] */ DWORD dwFlags,
        /* [in] */ LPCWSTR pszAssemblyName, 
        /* [in] */ LPCFUSION_INSTALL_REFERENCE pInfo, 
        /* [out, optional] */ ULONG *pulDisposition
        );
 
    STDMETHOD (QueryAssemblyInfo)(
        /* [in] */ DWORD dwFlags,
        /* [in] */ LPCWSTR pszAssemblyName,
        /* [in, out] */ ASSEMBLY_INFO *pAsmInfo
        );
 
    STDMETHOD (CreateAssemblyCacheItem)( 
        /* [in] */ DWORD dwFlags,
        /* [in] */ PVOID pvReserved,
        /* [out] */ IAssemblyCacheItem **ppAsmItem,
        /* [in, optional] */ LPCWSTR pszAssemblyName  // uncanonicalized, comma separted name=value pairs.
        );

    STDMETHOD (InstallAssembly)( // if you use this, fusion will do the streaming & commit.
        /* [in] */ DWORD dwFlags,
        /* [in] */ LPCWSTR pszManifestFilePath,
        /* [in] */ LPCFUSION_INSTALL_REFERENCE pInfo
        );


    STDMETHOD(CreateAssemblyScavenger) (
        /* [out] */ IUnknown **ppAsmScavenger
    );

    // IAssemblyNativeImageCache methods

    CAssemblyCache();
    ~CAssemblyCache();
private :
    DWORD       _dwSig;
    LONG        _cRef;
};
                       
#endif // _ASMCACHE_
