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
#ifndef CACHE_H
#define CACHE_H


#include "transprt.h"
#include "appctx.h"


// ---------------------------------------------------------------------------
// CCache
// cache class
// ---------------------------------------------------------------------------
class CCache : IUnknown
{
    friend class CAssemblyEnum;
    friend class CScavenger;
    friend class CAssemblyCacheRegenerator;
    friend class CUnifiedStore;
    friend class CTransCache;

public:
    // ctor, dtor
    CCache();
    ~CCache();

    // IUnknown methods, implemented only for the Release mechanism in CAppCtx
    // RefCount is used though

    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    static HRESULT Create(CCache **ppCache, IApplicationContext *pAppCtx);

    // Return the custom path or NULL if none
    LPCWSTR GetCustomPath();

    // Trans cache apis  ***********************************************************

    // Release global transport cache database.
    static VOID ReleaseTransCacheDatabase(DWORD dwCacheId);
    
    // Retrieves transport cache entry from transport cache.
    HRESULT RetrieveTransCacheEntry(IAssemblyName *pName, 
        DWORD dwFlags, CTransCache **ppTransCache);

    // get trans cache entry from naming object.
    HRESULT TransCacheEntryFromName(IAssemblyName *pName, 
        DWORD dwFlags, CTransCache **ppTransCache);

    // Retrieves assembly in global cache with maximum
    // revision/build number based on name passed in.
    static HRESULT GetGlobalMax(IAssemblyName *pName, 
        IAssemblyName **ppNameGlobal, CTransCache **ppTransCache);

    // get assembly name object from transcache entry.
    static HRESULT NameFromTransCacheEntry(
        CTransCache *pTC, IAssemblyName **ppName);

    // Determines whether to create new or reuse DB opened with the custom path
    HRESULT CreateTransCacheEntry(DWORD dwCacheId, CTransCache **ppTransCache);

protected:
        
    // Determines cache index from name and flags.
    static HRESULT ResolveCacheIndex(IAssemblyName *pName, 
        DWORD dwFlags, LPDWORD pdwCacheId);

private:
    HRESULT Init(IApplicationContext *pAppCtx);

private:

    DWORD   _dwSig;

    // RefCount
    LONG    _cRef;


    // Custom cache path, if specified
    LPWSTR  _pwzCachePath;
};

STDAPI NukeDownloadedCache();

STDAPI DeleteAssemblyFromTransportCache( LPCTSTR lpszCmdLine, DWORD *pDelCount );

#endif // CACHE_H
