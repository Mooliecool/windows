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
#ifndef TRANSCACHE_H
#define TRANSCACHE_H

#include <fusionp.h>
#include "helpers.h"

class CCache;

// ---------------------------------------------------------------------------
// CTransCache
// ---------------------------------------------------------------------------
class CTransCache
{
public:

	// IINDEX_TRANSCACHE_STRONGNAME_PARTIAL 
	enum StrongPartialFlags
	{
	    TCF_STRONG_PARTIAL_NAME                     = 0x1,
	    TCF_STRONG_PARTIAL_CULTURE                  = 0x2,
	    TCF_STRONG_PARTIAL_PUBLIC_KEY_TOKEN         = 0x4,
	    TCF_STRONG_PARTIAL_MAJOR_VERSION            = 0x8,
	    TCF_STRONG_PARTIAL_MINOR_VERSION            = 0x10,
	    TCF_STRONG_PARTIAL_BUILD_NUMBER             = 0x20,
	    TCF_STRONG_PARTIAL_REVISION_NUMBER          = 0x40,
	    TCF_STRONG_PARTIAL_CUSTOM                   = 0x80,
	    TCF_STRONG_PARTIAL_ASSEMBLY_ARCHITECTURE    = 0x100
	};

	// IINDEX_TRANSCACHE_SIMPLENAME_PARTIAL 
	enum SimplePartialFlags
	{
	    TCF_SIMPLE_PARTIAL_CODEBASE_URL           = 0x1,
	    TCF_SIMPLE_PARTIAL_CODEBASE_LAST_MODIFIED = 0x2
	};

    // ctor, dtor
    CTransCache();
    ~CTransCache();

    // RefCount
    LONG AddRef();
    LONG Release();

    static HRESULT Create(CTransCache **ppTransCache, DWORD dwCacheId);
    static HRESULT Create(CTransCache **ppTransCache, DWORD dwCacheId, CCache *pCache);
    // Deallocates info.
    VOID CleanInfo(BOOL fFree = TRUE);

    // apply global QFE
    HRESULT ApplyQFEPolicy(CTransCache **ppOutTransCache);    
    HRESULT Retrieve();
    HRESULT Retrieve(CTransCache **pTransCache, DWORD dwCmpMask);
    BOOL IsMatch(CTransCache *pRec, DWORD dwCmpMaskIn, LPDWORD pdwCmpMaskOut);
    TRANSCACHEINFO* CloneInfo();

    void SetCacheType(CACHE_FLAGS cf);
    CACHE_FLAGS GetCacheType();
    LPWSTR GetCustomPath();


    
    DWORD MapNameMaskToCacheMask(DWORD dwMask);
    DWORD MapCacheMaskToQueryCols(DWORD dwMask);
    BOOL            IsSimpleName();
    BOOL            IsPurgable();
    ULONGLONG       GetVersion();
    HRESULT UpdateDiskUsage(int dwDiskUsageInKB);

private:
    HRESULT Init(DWORD dwCacheId, CCache *pCache);

protected:

    DWORD _dwSig;
    
public:
    // RefCount
    LONG        _cRef;

    TRANSCACHEINFO*   _pInfo;

    CACHE_FLAGS     _dwCacheType;

private:
    CCache          *_pCache;
};

#endif // TRANSCACHE_H
