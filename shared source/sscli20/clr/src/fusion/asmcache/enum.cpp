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

#include "asmstrm.h"
#include "util.h"

#include "enum.h"
#include "transprt.h"
#include "cacheutils.h"
#include "lock.h"

extern const WCHAR g_szFindAllMask[];
extern const WCHAR g_szDotDLL[];
extern const WCHAR g_szDotEXE[];

void RemoveDirectoryIfEmpty(LPCWSTR pszCachePath, LPCWSTR pszParentDir)
{
    WCHAR pszDirPath[MAX_PATH+1];

    if( pszCachePath[0] && pszParentDir[0])
    {
        HRESULT hr = StringCchPrintf(pszDirPath, MAX_PATH, L"%ws\\%ws", pszCachePath, pszParentDir);
        if (SUCCEEDED(hr)) {
            WszRemoveDirectory(pszDirPath);
        }
    }
}

// --------------------- CEnumCache implementation --------------------------


// ---------------------------------------------------------------------------
// CEnumRecord  ctor
// ---------------------------------------------------------------------------
CEnumCache::CEnumCache(BOOL bShowAll, LPCWSTR pszCustomPath)
{
    _dwSig = 0x524e4345; /* 'RNCE' */
    _dwColumns = 0;
    _pQry = 0;
    _bShowAll=bShowAll;
    _fAll = FALSE;
    _fAllDone = FALSE;
    _bNeedMutex = FALSE;
    _hParentDir = INVALID_HANDLE_VALUE;
    _hAsmDir = INVALID_HANDLE_VALUE;
    *_wzCachePath = L'\0';
    *_wzParentDir = L'\0';
    _AsmArchType = peI386;       // Default to x86
    _dwCacheType = CACHE_INVALID;
    _dwQueryMask = 0;

    if(pszCustomPath) {
        HRESULT hr = StringCchPrintfW(_wzCustomPath, MAX_PATH, L"%ws", pszCustomPath);
        if (FAILED(hr)) {
            _ASSERTE(!"StringCchPrintf should not fail");
            return;
        }
    }
    else {
        *_wzCustomPath = L'\0';
    }
}

// ---------------------------------------------------------------------------
// CEnumRecord  dtor
// ---------------------------------------------------------------------------
CEnumCache::~CEnumCache()
{
    _dwColumns = 0;

    if(_hParentDir != INVALID_HANDLE_VALUE)
    {
        FindClose(_hParentDir);
        _hParentDir = INVALID_HANDLE_VALUE;
    }

    if(_hAsmDir != INVALID_HANDLE_VALUE)
    {
        FindClose(_hAsmDir);
        _hAsmDir = INVALID_HANDLE_VALUE;
    }

    SAFERELEASE(_pQry);
}

// ---------------------------------------------------------------------------
// CEnumRecord::Init
// ---------------------------------------------------------------------------
HRESULT
CEnumCache::Init(CTransCache* pQry, DWORD dwCmpMask)
{
    HRESULT hr = S_OK;

    _ASSERTE(pQry);

    _bNeedMutex = ((pQry->GetCacheType() & CACHE_DOWNLOAD) && (!_wzCustomPath[0]));

    if(_bNeedMutex)
    {
        if(FAILED(hr = CreateCacheMutex()))
            goto exit;
    }

    hr = Initialize(pQry, dwCmpMask);

exit :
    return hr;
}

HRESULT
CEnumCache::Initialize(CTransCache* pQry, DWORD dwCmpMask)
{
    HRESULT         hr = S_OK;
    CMutex          cCacheMutex(_bNeedMutex ? g_hCacheMutex : INVALID_HANDLE_VALUE);

    _pQry = pQry;
    _pQry->AddRef();

    _dwQueryMask = dwCmpMask;
    _dwCacheType = _pQry->GetCacheType();
    _wRealPA = GetRealProcessorType();

    _dwCacheType = CACHE_GAC_MSIL;

    _dwColumns = _pQry->MapCacheMaskToQueryCols(_dwQueryMask);

    if(!_dwColumns) {
        _fAll = TRUE;        // Do whole table scan
    }

    for(;;) {
        hr = GetCacheDir();
        if(hr != S_FALSE) {
            break;
        }

        // DB_S_NOTFOUND || _fAllDone means no more 
        // cache's to look at
        hr = SetNextCacheType();
        if((hr == S_FALSE) || _fAllDone) {
            goto exit;
        }
    }

exit:

    if(FAILED(hr))
        _fAllDone = TRUE;

    return hr;
}

// ---------------------------------------------------------------------------
// CEnumRecord::GetNextRecord
// ---------------------------------------------------------------------------
HRESULT
CEnumCache::GetNextRecord(CTransCache* pOutRecord)
{
    HRESULT hr = S_FALSE;

    WIN32_FIND_DATA FindFileData;
    CMutex  cCacheMutex(_bNeedMutex ? g_hCacheMutex : INVALID_HANDLE_VALUE);
    
    if( !pOutRecord ) {
        hr = E_INVALIDARG;
        goto exit;
    }

    if(_fAllDone) {
        hr = S_FALSE;
        goto exit;
    }

    hr = cCacheMutex.Lock();
    if(FAILED(hr)) {
        goto exit;
    }

    if(_hParentDir == INVALID_HANDLE_VALUE) {

        // Get the next assembly
        hr = GetNextAssemblyDir(pOutRecord);
        if(hr != S_FALSE) {
            goto exit;
        }

        // No assembly, get next cache
        hr = GetNextCacheDir(pOutRecord);
        goto exit;
    }
    else {
        _ASSERTE(lstrlenW(_wzParentDir));
        hr = StringCbCopy(FindFileData.cFileName, sizeof(FindFileData.cFileName), _wzParentDir);
        if (FAILED(hr)) {
            goto exit;
        }

        do {
            // skip directories    
            if (!FusionCompareStringI(FindFileData.cFileName, L"."))
                continue;
            if (!FusionCompareStringI(FindFileData.cFileName, L".."))
                continue;

            hr = StringCbCopy(_wzParentDir, sizeof(_wzParentDir), FindFileData.cFileName );
            if (FAILED(hr)) {
                goto exit;
            }

            hr = GetNextAssemblyDir(pOutRecord);
            if(FAILED(hr))
                goto exit;
            if(hr == S_OK)
                goto exit;

            RemoveDirectoryIfEmpty( _wzCachePath, _wzParentDir);

        } while(WszFindNextFile(_hParentDir, &FindFileData)); // while

        // No more assemblies, go to next cache dir
        hr = GetNextCacheDir(pOutRecord);
    }

exit:

    return hr;
}

// ---------------------------------------------------------------------------
// CEnumCache::GetNextAssemblyDir
// ---------------------------------------------------------------------------
HRESULT
CEnumCache::GetNextAssemblyDir(CTransCache* pOutRecord)
{
    HRESULT hr = S_FALSE;
    DWORD   dwCmpResult = 0;
    BOOL    fIsMatch = FALSE;
    BOOL    fFound = FALSE;
    WIN32_FIND_DATA FindFileData;
    WCHAR   wzFullSearchPath[MAX_PATH+1];
    DWORD   dwAttr=0;
    WCHAR   wzFullPath[MAX_PATH+1];
    DWORD   dwLen = 0;
    BOOL    fExists = FALSE;

    if(!pOutRecord) {
        hr = E_INVALIDARG;
        goto exit;
    }

    if(_fAllDone) {
        hr = S_FALSE;
        goto exit;
    }

    _ASSERTE(lstrlenW(_wzParentDir));

    if(_hAsmDir == INVALID_HANDLE_VALUE)
    {
        if( (lstrlenW(_wzCachePath) + lstrlenW(_wzParentDir) + lstrlenW(g_szFindAllMask) + 4) >= MAX_PATH)
        {
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }

        hr = StringCbCopy(wzFullSearchPath, sizeof(wzFullSearchPath), _wzCachePath);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = PathAddBackslashWrap(wzFullSearchPath, MAX_PATH);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = StringCbCat(wzFullSearchPath, sizeof(wzFullSearchPath), _wzParentDir);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = CheckFileExistence(wzFullSearchPath, &fExists, &dwAttr);
        if(FAILED(hr)) {
            goto exit;
        }
        else if(!fExists || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)) {
            hr = S_FALSE;
            goto exit;
        }

        hr = StringCbCat(wzFullSearchPath, sizeof(wzFullSearchPath), g_szFindAllMask);
        if (FAILED(hr)) {
            goto exit;
        }
        
        _hAsmDir = WszFindFirstFile(wzFullSearchPath, &FindFileData);

        if(_hAsmDir == INVALID_HANDLE_VALUE)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }

        fFound = TRUE;
    }
    else
    {   
        if(WszFindNextFile(_hAsmDir, &FindFileData))
            fFound = TRUE;
    }

    do
    {
        if(!fFound)
            break;

        if (!FusionCompareStringI(FindFileData.cFileName, L"."))
                continue;

        if (!FusionCompareStringI(FindFileData.cFileName, L".."))
                continue;

        if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            continue;

        _AsmArchType = peNone;
        if(!(_dwCacheType & CACHE_DOWNLOAD)) {

            _AsmArchType = peMSIL;
        }

        pOutRecord->SetCacheType(_dwCacheType);

        hr = ParseDirName(pOutRecord, _AsmArchType, _wzParentDir, FindFileData.cFileName );
        if(hr != S_OK)
        {
            pOutRecord->CleanInfo(TRUE);
            continue;
        }

        if( (lstrlenW(_wzCachePath) + lstrlenW(_wzParentDir) + lstrlenW(FindFileData.cFileName) + 4) >= MAX_PATH)
        {
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }

        hr = StringCbCopy(wzFullPath, sizeof(wzFullPath), _wzCachePath);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = PathAddBackslashWrap(wzFullPath, MAX_PATH);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = StringCbCat(wzFullPath, sizeof(wzFullPath), _wzParentDir);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = PathAddBackslashWrap(wzFullPath, MAX_PATH);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = StringCbCat(wzFullPath, sizeof(wzFullPath), FindFileData.cFileName);
        if (FAILED(hr)) {
            goto exit;
        }

        // only look for fusion info for download cache. 
        if (!IsGacType(_dwCacheType)) {
            hr = GetFusionInfo( pOutRecord, wzFullPath);
            if((hr != S_OK) && !_bShowAll)
            {
                pOutRecord->CleanInfo(TRUE);
                continue;
            }
        }

        hr = PathAddBackslashWrap(wzFullPath, MAX_PATH);
        if (FAILED(hr)) {
            goto exit;
        }

        if(_dwCacheType & CACHE_DOWNLOAD)
        {
            if(!pOutRecord->_pInfo->pwzName)
            {
                if(_bShowAll)
                    goto Done;
                else
                    continue;
            }

            hr = StringCbCat(wzFullPath, sizeof(wzFullPath), pOutRecord->_pInfo->pwzName);
            if (FAILED(hr)) {
                goto exit;
            }
        }
        else
        {
            hr = StringCbCat(wzFullPath, sizeof(wzFullPath), _wzParentDir);
            if (FAILED(hr)) {
                goto exit;
            }
        }

        dwLen  = lstrlenW(wzFullPath);

        if( (dwLen + lstrlenW(g_szDotDLL)) >= MAX_PATH) {
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }

        hr = StringCbCat(wzFullPath, sizeof(wzFullPath), g_szDotDLL);
        if (FAILED(hr)) {
            goto exit;
        }

        fExists = FALSE;

        // Look for AsmName.dll
        hr = CheckFileExistence(wzFullPath, &fExists, NULL);
        if(FAILED(hr)) {
            goto exit;
        }
        else if(!fExists) {
            // Look for AsmName.exe
            hr = StringCbCopy(wzFullPath+dwLen, sizeof(wzFullPath) - dwLen * sizeof(WCHAR), g_szDotEXE);
            if (FAILED(hr)) {
                goto exit;
            }

            hr = CheckFileExistence(wzFullPath, &fExists, NULL);
            if(FAILED(hr)) {
                goto exit;
            }
            else if(!fExists) {
                pOutRecord->CleanInfo(TRUE);
                continue;
            }
        }
        
        if(!_bShowAll)
        {
            fIsMatch = _pQry->IsMatch(pOutRecord, _dwQueryMask, &dwCmpResult);                
            if(!fIsMatch)
            {
                pOutRecord->CleanInfo(TRUE);
                continue;
            }
        }

Done:
        // Delete the old path and replace it with the fully
        // qualified path + filename
        SAFEDELETEARRAY(pOutRecord->_pInfo->pwzPath);
        pOutRecord->_pInfo->pwzPath = WSTRDupDynamic(wzFullPath);
        if (!pOutRecord->_pInfo->pwzPath) {
            hr = E_OUTOFMEMORY;
            goto exit;
        }

        hr = S_OK;
        goto exit;

    }while(WszFindNextFile(_hAsmDir, &FindFileData)); // while 

    if( GetLastError() != ERROR_NO_MORE_FILES)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    hr = S_FALSE;
    FindClose(_hAsmDir);
    _hAsmDir = INVALID_HANDLE_VALUE;

exit :
    return hr;
}

// ---------------------------------------------------------------------------
// CEnumCache::GetCacheDir
// ---------------------------------------------------------------------------
HRESULT CEnumCache::GetCacheDir()
{
    HRESULT     hr = S_OK;
    DWORD       cchRequired = 0;
    DWORD       dwSize = 0;
    WCHAR       wzFullSearchPath[MAX_PATH+1];
    WIN32_FIND_DATA FindFileData;
    LPWSTR      pwzCustomPath = lstrlenW(_wzCustomPath) ?  _wzCustomPath : NULL;
    CMutex      cCacheMutex(_bNeedMutex ? g_hCacheMutex : INVALID_HANDLE_VALUE);
    BOOL        fExists = FALSE;

    if(_hParentDir != INVALID_HANDLE_VALUE) {
        FindClose(_hParentDir);

        _hParentDir = INVALID_HANDLE_VALUE;
        _wzCachePath[0] = L'\0';
        _wzParentDir[0] = L'\0';
    }

    cchRequired = MAX_PATH;
    hr = CreateAssemblyDirPath(pwzCustomPath, _dwCacheType, 0, _wzCachePath, &cchRequired);
    if(FAILED(hr)) {
        goto exit;
    }
    
    if(FAILED(hr = cCacheMutex.Lock()))
        goto exit;

    hr = CheckFileExistence(_wzCachePath, &fExists, NULL);
    if (FAILED(hr)) {
        goto exit;
    }
    else if(!fExists) {
        hr = S_FALSE;

        if((_dwCacheType & CACHE_GAC) || (_dwCacheType & CACHE_ZAP)) {
            _fAllDone = TRUE;
        }

        goto exit;
    }

    dwSize = ARRAYSIZE(_wzParentDir) - 1;
    if(FAILED(hr = GetAssemblyParentDir( (CTransCache*) _pQry, _wzParentDir, &dwSize, _wzCustomPath)))
        goto exit;

    cchRequired = lstrlenW(_wzCachePath) + lstrlenW(_wzParentDir) + lstrlenW(g_szFindAllMask) + 1; // extra chars for "\" etc

    if (cchRequired >= MAX_PATH)
    {
        hr = FUSION_E_INVALID_NAME;
        goto exit;
    }

    hr = StringCbCopy(wzFullSearchPath, sizeof(wzFullSearchPath), _wzCachePath);
    if (FAILED(hr)) {
        goto exit;
    }

    // Find all assemblies
    if(_fAll) {
        hr = StringCbCat(wzFullSearchPath, sizeof(wzFullSearchPath), g_szFindAllMask);
        if (FAILED(hr)) {
            goto exit;
        }
        
        _hParentDir = WszFindFirstFile(wzFullSearchPath, &FindFileData);

        if(_hParentDir == INVALID_HANDLE_VALUE) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }

        hr = StringCbCat(_wzParentDir, sizeof(_wzParentDir), FindFileData.cFileName);
        if (FAILED(hr)) {
            goto exit;
        }
    }
    else {
        // Partial lookup on given assembly name
        if (!lstrlenW(_wzParentDir)) {
            hr = S_FALSE;

            if((_dwCacheType & CACHE_GAC) || (_dwCacheType & CACHE_ZAP)) {
                _fAllDone = TRUE;
            }
            goto exit;
        }
            
        hr = PathAddBackslashWrap(wzFullSearchPath, MAX_PATH);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = StringCbCat(wzFullSearchPath, sizeof(wzFullSearchPath), _wzParentDir);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = CheckFileExistence(wzFullSearchPath, &fExists, NULL);
        if (FAILED(hr)) {
            goto exit;
        }
        else if(!fExists) {
            hr = S_FALSE;
            if((_dwCacheType & CACHE_GAC) || (_dwCacheType & CACHE_ZAP)) {
                _fAllDone = TRUE;
            }

            goto exit;
        }
    }

    hr = S_OK;

exit:

    if(FAILED(hr))
        _fAllDone = TRUE;

    return hr;
}

// ---------------------------------------------------------------------------
// CEnumCache::GetNextCacheDir
// ---------------------------------------------------------------------------
HRESULT CEnumCache::GetNextCacheDir(CTransCache* pOutRecord)
{
    HRESULT     hr = S_FALSE;
    
    // No more assembly dirs, go to next cache if 64 bit
    if(_dwCacheType & CACHE_DOWNLOAD) {
        _fAllDone = TRUE;
        if( GetLastError() != ERROR_NO_MORE_FILES) {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
        goto exit;
    }

    // Architecture specific comparison?
    if(_dwQueryMask & CTransCache::TCF_STRONG_PARTIAL_ASSEMBLY_ARCHITECTURE) {
        _fAllDone = TRUE;
        if( GetLastError() != ERROR_NO_MORE_FILES) {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
        goto exit;
    }

    for(;;) {
        hr = SetNextCacheType();

        // No more cache's
        if(hr == S_FALSE || _fAllDone)  {
            hr = S_FALSE;
            goto exit;
        }

        // Get cache dir
        hr = GetCacheDir();
        if(FAILED(hr)) {
            goto exit;
        }

        // Not there, continue on
        if(hr == S_FALSE) {
            continue;
        }

        // Found a new record, return it
        hr = GetNextRecord(pOutRecord);
        goto exit;
    }

exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CEnumCache::SetNextCacheType
// ---------------------------------------------------------------------------
HRESULT CEnumCache::SetNextCacheType()
{
    HRESULT     hr = S_OK;
    
    if(_dwCacheType & CACHE_GAC_64)
        _dwCacheType = CACHE_GAC_32;
    else if(_dwCacheType & CACHE_GAC_32)
        _dwCacheType = CACHE_GAC_MSIL;
    else if(_dwCacheType & CACHE_GAC_MSIL)
        _dwCacheType = CACHE_GAC;
    else {
        hr = S_FALSE;
        _fAllDone = TRUE;
    }

    return hr;
}

