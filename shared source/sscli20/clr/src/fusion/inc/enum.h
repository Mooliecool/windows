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
#ifndef ENUM_H
#define ENUM_H
#include "fusionp.h"
#include "transprt.h"
#include "cacheutils.h"

class CEnumCache {
public:
    CEnumCache(BOOL bShowAll, LPCWSTR pszCustomPath);
    ~CEnumCache();
    HRESULT Init(CTransCache* pQry, DWORD dwCmpMask);
    HRESULT GetNextRecord(CTransCache* pOutRecord);

private:
    HRESULT Initialize(CTransCache* pQry, DWORD dwCmpMask);
    HRESULT GetNextAssemblyDir(CTransCache* pOutRecord);
    HRESULT GetCacheDir();
    HRESULT GetNextCacheDir(CTransCache* pOutRecord);
    HRESULT SetNextCacheType();
    
private:
    DWORD       _dwSig;
    BOOL        _bShowAll; // including non-usable assemblies; meant for scavenger to delete.
    DWORD       _dwColumns;
    DWORD       _dwCmpMask;
    CTransCache*    _pQry;
    BOOL        _fAll;
    BOOL        _fAllDone;
    HANDLE      _hParentDir;
    HANDLE      _hAsmDir;
    WCHAR       _wzCustomPath[MAX_PATH+1];
    WCHAR       _wzCachePath[MAX_PATH+1];
    WCHAR       _wzParentDir[MAX_PATH+1];
    BOOL        _bNeedMutex;
    PEKIND      _AsmArchType;
    CACHE_FLAGS _dwCacheType;
    DWORD       _dwQueryMask;
    WORD        _wRealPA;
};

#endif // ENUM_H
