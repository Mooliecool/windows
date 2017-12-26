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
#ifndef _ASMITEM_
#define _ASMITEM_

#include <windows.h>
#include <winerror.h>
#include "fusionp.h"
#include "cache.h"
#include "asmstrm.h"
#include "fusion.h"
#include "asmint.h"

class CAssemblyCacheItem : public IAssemblyCacheItem
{
public:

    // IUnknown methods
    STDMETHODIMP            QueryInterface(REFIID riid,void ** ppv);
    STDMETHODIMP_(ULONG)    AddRef();
    STDMETHODIMP_(ULONG)    Release();

    STDMETHOD (CreateStream)(
        /* [in] */ DWORD dwFlags,                         // For general API flags
        /* [in] */ LPCWSTR pszStreamName,                 // Name of the stream to be passed in
        /* [in] */ DWORD dwFormat,                        // format of the file to be streamed in.
        /* [in] */ DWORD dwFormatFlags,                   // format-specific flags
        /* [out] */ IStream **ppIStream,
        /* [in, optional] */ ULARGE_INTEGER *puliMaxSize  // Max size of the Stream.
        );
 
    STDMETHOD (Commit)(
        /* [in] */ DWORD dwFlags, // For general API flags like IASSEMBLYCACHEITEM _COMMIT_FLAG_REFRESH 
        /* [out, optional] */ ULONG *pulDisposition); 

    STDMETHOD (AbortItem)(); // If you have created IAssemblyCacheItem and don't plan to use it, its good idea to call AbortItem before releasing it.

    CAssemblyCacheItem();
    ~CAssemblyCacheItem();

    HANDLE GetFileHandle();
    BOOL IsManifestFileLocked();

    static HRESULT Create(IApplicationContext *pAppCtx,
        IAssemblyName *pName, 
        LPCTSTR pszUrl, 
        FILETIME *pftLastMod, 
        DWORD dwCacheFlags,
        IAssemblyManifestImport *pManImport,
        LPCWSTR pszAssemblyName,
        LPCFUSION_INSTALL_REFERENCE pRefData,
        IAssemblyCacheItem **ppAsmItem);

    HRESULT Init(IApplicationContext *pAppCtx,
        IAssemblyName *pName, 
        LPCTSTR pszUrl,
        FILETIME *pftLastMod, 
        CACHE_FLAGS dwCacheFlags,
        LPCFUSION_INSTALL_REFERENCE pRefData,
        IAssemblyManifestImport *pManImport);

    void StreamDone (HRESULT);

    void AddStreamSize(DWORD dwFileSizeLow, DWORD dwFileSizeHigh);

    HRESULT AddToStreamHashList(CModuleHashNode *);

    HRESULT MoveAssemblyToFinalLocation(DWORD dwFlags, DWORD dwVerifyFlags, ULONG *pulDisposition);
    LPTSTR GetManifestPath();
    CTransCache *GetTransCacheEntry();

    HRESULT SetManifestInterface(IAssemblyManifestImport *pImport);
    IAssemblyManifestImport* GetManifestInterface();   

    HRESULT SetNameDef(IAssemblyName *pName);
    HRESULT GetNameDef(IAssemblyName **ppName);

    HRESULT CompareInputToDef();

    HRESULT VerifyDuplicate(DWORD dwVerifyFlags, CTransCache *pTC);

    STDMETHOD (LegacyCommit)(
        /* [in] */ DWORD dwFlags, // For general API flags like IASSEMBLYCACHEITEM _COMMIT_FLAG_REFRESH 
        /* [out, optional] */ ULONG *pulDisposition); 

private:
    HRESULT CreateCacheDir( 
        /* [in]  */  LPCOLESTR pszCustomPath,
        /* [in]  */  LPCOLESTR pszName);

    HRESULT RemoveOldBits(CACHE_FLAGS dwAsmCacheType, CTransCache *pTransCache, BOOL fRemoveWinSxs, BOOL *pbNeedNewDir);


    HRESULT GetRealAssemblyDir(
                LPCWSTR pwzAsmDir, 
                __in LPBOOL pbExists, 
                __out_ecount_opt(*pdwSize) LPWSTR pwzRealAsmDir, 
                __inout LPDWORD pdwSize);

private:
    DWORD                    _dwSig;
    LONG                     _cRef;                 // refcount
    HRESULT                  _hrError;              // error for rollback to check
    IAssemblyName*           _pName;                // assembly name object
    LONG                     _cStream;              // child refcount
    LONG                     _dwAsmSizeInKB;        // Size of Asm in KB, downloded in this round.
    TCHAR                    _szDir[MAX_PATH];      // assembly item directory
    DWORD                    _cwDir;                // path size including null
    TCHAR                    _szManifest[MAX_PATH]; // full path to manifest
    LPWSTR                   _pszAssemblyName;      // Display name of the assembly from Installer; has to match the def.
    IAssemblyManifestImport *_pManifestImport;      // Interface to Manifest.
    CModuleHashNode         *_pStreamHashList;      // Linked List of Modules hashes for integrity check
    LPTSTR                   _pszUrl;               // Codebase
    FILETIME                 _ftLastMod;            // Last mod time of Codebase.
    CTransCache             *_pTransCache;          // associated trans cache entry.
    CACHE_FLAGS              _dwCacheFlags;         // Cache_Flag
    CCache                  *_pCache;
    HANDLE                   _hFile;
    BOOL                     _bNeedMutex;
    BOOL                     _bCommitDone;          // Final commit flag, controls cleanup
    TCHAR                    _szDestManifest[MAX_PATH]; // full path to manifest
    LPCFUSION_INSTALL_REFERENCE _pRefData;
    HANDLE                   _hGACLock;
};

#endif // ASMITEM_H
