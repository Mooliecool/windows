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
#include "fusionp.h"
#include "asmcache.h"
#include "asmitem.h"
#include "naming.h"
#include "appctx.h"
#include "helpers.h"
#include "asm.h"
#include "asmimprt.h"
#include "policy.h"
#include "scavenger.h"
#include "util.h"
#include "cache.h"
#include "cacheutils.h"


#include "dlwrap.h"

extern BOOL g_bRunningOnNT;

STDAPI CreateAssemblyCache(IAssemblyCache **ppAsmCache,
                           DWORD dwReserved)
{
    HRESULT                       hr = S_OK;

    SO_NOT_MAINLINE_FUNCTION;
    
    if (!ppAsmCache) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppAsmCache = NEW(CAssemblyCache);

    if (!*ppAsmCache) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// ValidateAssembly
// ---------------------------------------------------------------------------
HRESULT ValidateAssembly(LPCTSTR pszManifestFilePath, IAssemblyName *pName)
{
    HRESULT                    hr = S_OK;
    BYTE                       abCurHash[MAX_HASH_LEN];
    BYTE                       abFileHash[MAX_HASH_LEN];
    DWORD                      cbModHash;
    DWORD                      cbFileHash;
    DWORD                      dwAlgId;
    WCHAR                      wzDir[MAX_PATH+1];
    LPWSTR                     pwzTmp = NULL;
    WCHAR                      wzModName[MAX_PATH+1];
    WCHAR                      wzModPath[MAX_PATH+1];
    DWORD                      idx = 0;
    DWORD                      cbLen=0;
    IAssemblyManifestImport   *pManifestImport=NULL;
    IAssemblyModuleImport     *pCurModImport = NULL;
    BOOL                       bExists;

    SO_NOT_MAINLINE_FUNCTION;
    
    if (FAILED(hr = CreateAssemblyManifestImport((LPTSTR)pszManifestFilePath, &pManifestImport))) 
    {
        goto exit;
    }

    // Integrity checking
    // Walk all modules to make sure they are there (and are valid)

    hr = StringCbCopy(wzDir, sizeof(wzDir), pszManifestFilePath);
    if (FAILED(hr)) {
        goto exit;
    }

    pwzTmp = PathFindFileName(wzDir);
    *pwzTmp = L'\0';

    while (SUCCEEDED(hr = pManifestImport->GetNextAssemblyModule(idx++, &pCurModImport)))
    {
        cbLen = MAX_PATH;
        if (FAILED(hr = pCurModImport->GetModuleName(wzModName, &cbLen)))
            goto exit;

        hr = StringCchPrintf(wzModPath, MAX_PATH, L"%s%s", wzDir, wzModName);
        if (FAILED(hr)) {
            goto exit;
        }
        
        hr = CheckFileExistence(wzModPath, &bExists, NULL);
        if (FAILED(hr)) {
            goto exit;
        }
        else if (!bExists) {
            hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
            goto exit;
        }

        // Get the hash of this module from manifest
        if(FAILED(hr = pCurModImport->GetHashAlgId(&dwAlgId)))
            goto exit;

        cbModHash = MAX_HASH_LEN; 
        if(FAILED(hr = pCurModImport->GetHashValue(abCurHash, &cbModHash)))
            goto exit;

        cbFileHash = MAX_HASH_LEN;
        if(FAILED(hr = GetHash(wzModPath, (ALG_ID)dwAlgId, abFileHash, &cbFileHash)))
            goto exit;

        if ((cbModHash != cbFileHash) || !CompareHashs(cbModHash, abCurHash, abFileHash)) 
        {
            hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
            goto exit;
        }

        SAFERELEASE(pCurModImport);
    }

    if (hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) 
    {
        hr = S_OK;
    }

exit:

    SAFERELEASE(pManifestImport);
    SAFERELEASE(pCurModImport);

    return hr;
}


// ---------------------------------------------------------------------------
// CopyAssemblyFile
// ---------------------------------------------------------------------------
HRESULT CopyAssemblyFile
    (IAssemblyCacheItem *pasm, LPCOLESTR pszSrcFile, DWORD dwFormat)
{
    HRESULT hr;
    IStream* pstm    = NULL;
    HANDLE hf        = INVALID_HANDLE_VALUE;
    LPBYTE pBuf      = NULL;
    DWORD cbBuf      = 0x4000;
    DWORD cbRootPath = 0;
    TCHAR *pszName   = NULL;
    
    SO_NOT_MAINLINE_FUNCTION;
    
    // Find root path length
    pszName = PathFindFileName(pszSrcFile);

    cbRootPath = (DWORD) (pszName - pszSrcFile);
    _ASSERTE(cbRootPath < MAX_PATH);
    
    hr = pasm->CreateStream (0, pszSrcFile+cbRootPath, 
        dwFormat, 0, &pstm, NULL);

    if (hr != S_OK)
        goto exit;

    pBuf = NEW(BYTE[cbBuf]);
    if (!pBuf)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }
    
    DWORD dwWritten, cbRead;
    hf = WszCreateFile (pszSrcFile, GENERIC_READ, FILE_SHARE_READ,
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hf == INVALID_HANDLE_VALUE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto exit;
    }

    while (ReadFile (hf, pBuf, cbBuf, &cbRead, NULL) && cbRead)
    {
        hr = pstm->Write (pBuf, cbRead, &dwWritten);
        if (hr != S_OK)
            goto exit;
    }

    hr = pstm->Commit(0);
    if (hr != S_OK)
        goto exit;

exit:

    SAFERELEASE(pstm);
    SAFEDELETEARRAY(pBuf);

    if (hf != INVALID_HANDLE_VALUE)
    {
        CloseHandle (hf);
    }
    return hr;
}

/*--------------------- CAssemblyCache defines -----------------------------*/


CAssemblyCache::CAssemblyCache()
{
    _dwSig = 0x434d5341; /* 'CMSA' */
    _cRef = 1;
}

CAssemblyCache::~CAssemblyCache()
{

}

extern const WCHAR g_szDotDLL[];
extern const WCHAR g_szDotEXE[];

STDMETHODIMP CAssemblyCache::InstallAssembly( // if you use this, fusion will do the streaming & commit.
        /* [in] */ DWORD dwFlags,
        /* [in] */ LPCWSTR pszManifestFilePath, 
        /* [in] */ LPCFUSION_INSTALL_REFERENCE pRefData)
{
    HRESULT                     hr=S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    LPWSTR                      szFullCodebase=NULL;
    LPWSTR                      szFullManifestFilePath=NULL;
    DWORD                       dwLen=0;
    DWORD                       dwIdx = 0;
    WCHAR                       wzDir[MAX_PATH+1];
    WCHAR                       wzModPath[MAX_PATH+1];
    WCHAR                       wzModName[MAX_PATH+1];
    LPWSTR                      pwzTmp = NULL;

    IAssemblyManifestImport     *pManifestImport=NULL;
    IAssemblyModuleImport       *pModImport = NULL;
    CAssemblyCacheItem          *pAsmItem    = NULL;
    FILETIME                    ftLastModTime;
    DWORD                       dwCommitFlags = 0;
    BOOL                        bExists;
   
    LPWSTR pszExt = NULL;
    
    if(!pszManifestFilePath || !*pszManifestFilePath) {
        hr = E_INVALIDARG;
        goto exit;
    }

    pszExt = PathFindExtension(pszManifestFilePath);
    if(!(*pszExt)) {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
        goto exit;
    }
    if(FusionCompareStringI(pszExt, g_szDotDLL) && FusionCompareStringI(pszExt, g_szDotEXE)) {
        hr = HRESULT_FROM_WIN32(ERROR_INVALID_NAME);
        goto exit;
    }


    hr = CheckFileExistence(pszManifestFilePath, &bExists, NULL);
    if (FAILED(hr)) {
        goto exit;
    }
    else if (!bExists) {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        goto exit;
    }

    szFullCodebase = NEW(WCHAR[MAX_URL_LENGTH*2+2]);
    if (!szFullCodebase)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    szFullManifestFilePath = szFullCodebase + MAX_URL_LENGTH +1;

    if (PathIsRelative(pszManifestFilePath))
    {
        // szPath is relative! Combine this with the CWD
        // Canonicalize codebase with CWD if needed.
        WCHAR szCurrentDir[MAX_PATH+1];

        if (!WszGetCurrentDirectory(MAX_PATH, szCurrentDir)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            goto exit;
        }

        if(!IsPathSeparator(szCurrentDir[lstrlenW(szCurrentDir)-1])) {
            // Add trailing backslash
            hr = PathAddBackslashWrap(szCurrentDir, MAX_PATH);
            if (FAILED(hr)) {
                goto exit;
            }
        }

        // Build the codebase
        dwLen = MAX_URL_LENGTH;
        hr = UrlCombineUnescape(szCurrentDir, pszManifestFilePath, szFullCodebase, &dwLen, 0);
        if(FAILED(hr)) {
            goto exit;
        }

        // Build the install path from the codebase
        dwLen = MAX_URL_LENGTH;
        hr = PathCreateFromUrlWrap(szFullCodebase, szFullManifestFilePath, &dwLen, 0);
        if(FAILED(hr)) {
            goto exit;
        }
    }
    else {
        dwLen = MAX_URL_LENGTH;
        hr = UrlCanonicalizeUnescape(pszManifestFilePath, szFullCodebase, &dwLen, 0);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = StringCchCopy(szFullManifestFilePath, MAX_URL_LENGTH, pszManifestFilePath);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    hr = CreateAssemblyManifestImport(szFullManifestFilePath, &pManifestImport);
    if(FAILED(hr)) {
        goto exit;
    }

    if(FAILED(hr = GetFileLastModified(szFullManifestFilePath, &ftLastModTime)))
        goto exit;

    hr = StringCbCopy(wzDir, sizeof(wzDir), szFullManifestFilePath);
    if (FAILED(hr)) {
        goto exit;
    }

    pwzTmp = PathFindFileName(wzDir);

    *pwzTmp = L'\0';

    // Create the assembly cache item.
    if (FAILED(hr = CAssemblyCacheItem::Create(NULL, NULL, (LPTSTR) szFullCodebase, 
        &ftLastModTime, CACHE_GAC, pManifestImport, NULL, pRefData,
        (IAssemblyCacheItem**) &pAsmItem)))
        goto exit;

    // Copy to cache.
    if (FAILED(hr = CopyAssemblyFile (pAsmItem, szFullManifestFilePath, 
        STREAM_FORMAT_MANIFEST)))
        goto exit;

    while (SUCCEEDED(hr = pManifestImport->GetNextAssemblyModule(dwIdx++, &pModImport))) 
    {
        dwLen = MAX_PATH;
        hr = pModImport->GetModuleName(wzModName, &dwLen);

        if(FAILED(hr)) {
            goto exit;
        }

        hr = StringCchPrintf(wzModPath, MAX_PATH, L"%s%s", wzDir, wzModName);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = CheckFileExistence(wzModPath, &bExists, NULL);
        if (FAILED(hr)) {
            goto exit;
        }
        else if (!bExists) {
            hr = FUSION_E_ASM_MODULE_MISSING;
            goto exit;
        }

        // Copy to cache.
        if(FAILED(hr = CopyAssemblyFile (pAsmItem, wzModPath, 0))) {
            goto exit;
        }

        SAFERELEASE(pModImport);
    }

    // don't enforce this flag for now. i.e always replace bits.
    // if(dwFlags & IASSEMBLYCACHE_INSTALL_FLAG_REFRESH)
    {
        dwCommitFlags |= IASSEMBLYCACHEITEM_COMMIT_FLAG_REFRESH; 
    }

    if(dwFlags & IASSEMBLYCACHE_INSTALL_FLAG_FORCE_REFRESH)
    {
        dwCommitFlags |= IASSEMBLYCACHEITEM_COMMIT_FLAG_FORCE_REFRESH; 
    }

    //  Do a force install. This will delete the existing entry(if any)
    if (FAILED(hr = pAsmItem->Commit(dwCommitFlags, NULL)))
    {
        goto exit;        
    }


exit:

    SAFERELEASE(pAsmItem);
    SAFERELEASE(pModImport);
    SAFERELEASE(pManifestImport);
    SAFEDELETEARRAY(szFullCodebase);

END_ENTRYPOINT_NOTHROW
    
    return hr;
}

STDMETHODIMP CAssemblyCache::UninstallAssembly(
        /* [in] */ DWORD dwFlags,
        /* [in] */ LPCWSTR pszAssemblyName, 
        /* [in] */ LPCFUSION_INSTALL_REFERENCE pRefData, 
        /* [out, optional] */ ULONG *pulDisposition)
{
    HRESULT         hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    IAssemblyName   *pName = NULL;
    IAssemblyName   *pNameQuery = NULL;
    CTransCache     *pTransCache = NULL;
    CCache          *pCache = NULL;
    BOOL            bHasActiveRefs = FALSE;
    BOOL            bRefNotFound = FALSE;
    ULONG           uDisposition = 0;

    if (!pszAssemblyName || !pszAssemblyName[0]) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = CreateAssemblyNameObject(&pNameQuery, pszAssemblyName, CANOF_PARSE_DISPLAY_NAME, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (CAssemblyName::IsCustom(pNameQuery)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = IsValidAssemblyOnThisPlatform(pNameQuery, TRUE);
    if (hr == HRESULT_FROM_WIN32(ERROR_BAD_FORMAT)) {
        hr = S_FALSE;
        goto Exit;
    }


    hr = CCache::Create(&pCache, NULL);
    if(FAILED(hr)) {
        goto Exit;
    }

    hr = pCache->RetrieveTransCacheEntry(pNameQuery, CACHE_GAC, &pTransCache);
    if (hr != S_OK) {
        goto Exit;
    }

    hr = CCache::NameFromTransCacheEntry(pTransCache, &pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Uninstall it from the Fusion CLR cache
    hr = CScavenger::DeleteAssembly(pTransCache->GetCacheType(), NULL, 
                                pTransCache->_pInfo->pwzPath, TRUE);

    if(FAILED(hr)) {
        goto Exit;
    }


    CleanupTempDir(CACHE_GAC, NULL);

Exit:
    uDisposition = 0;

    if(bRefNotFound)
    {
        uDisposition |= IASSEMBLYCACHE_UNINSTALL_DISPOSITION_REFERENCE_NOT_FOUND;
        hr = S_FALSE;
    }
    else if(bHasActiveRefs)
    {
        uDisposition |= IASSEMBLYCACHE_UNINSTALL_DISPOSITION_HAS_INSTALL_REFERENCES;
        hr = S_FALSE;
    }
    else if(hr == S_OK)
    {
        uDisposition |= IASSEMBLYCACHE_UNINSTALL_DISPOSITION_UNINSTALLED;
    }
    else if(hr == S_FALSE)
    {
        uDisposition |= IASSEMBLYCACHE_UNINSTALL_DISPOSITION_ALREADY_UNINSTALLED;
    }
    else if(hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION))
    {
        uDisposition |= IASSEMBLYCACHE_UNINSTALL_DISPOSITION_STILL_IN_USE;
    }

    if (pulDisposition) {
        *pulDisposition = uDisposition;
    }

    SAFERELEASE(pTransCache);
    SAFERELEASE(pCache);
    SAFERELEASE(pName);
    SAFERELEASE(pNameQuery);

    END_ENTRYPOINT_NOTHROW;
    
    return hr;
}

STDMETHODIMP CAssemblyCache::QueryAssemblyInfo(
        /* [in] */ DWORD dwFlags,
        /* [in] */ LPCWSTR pszAssemblyName,
        /* [in, out] */ ASSEMBLY_INFO *pAsmInfo)
{
    HRESULT           hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    LPTSTR            pszPath = NULL;
    DWORD             cbPath = 0;
    IAssemblyName     *pName = NULL;
    CAssemblyName     *pCName = NULL;
    CTransCache       *pTransCache = NULL;
    CCache            *pCache = NULL;
    BOOL              bExists;

    if (pAsmInfo) {
        // Handle legacy cases where the structure size wasn't enforced
        if(pAsmInfo->cbAssemblyInfo == 0) {
            pAsmInfo->cbAssemblyInfo = sizeof(ASSEMBLY_INFO);
        }
    
        // Validate size
        if(pAsmInfo->cbAssemblyInfo != sizeof(ASSEMBLY_INFO)) {
            hr = E_INVALIDARG;
            goto exit;
        }
    }


    if (FAILED(hr = CreateAssemblyNameObject(&pName, pszAssemblyName, CANOF_PARSE_DISPLAY_NAME, 0))) {
        goto exit;
    }

    hr = IsValidAssemblyOnThisPlatform(pName, TRUE);
    if (hr == HRESULT_FROM_WIN32(ERROR_BAD_FORMAT)) {
        hr = E_INVALIDARG;
        goto exit;
    }

    if (CAssemblyName::IsCustom(pName)) {
        hr = E_INVALIDARG;
        goto exit;
    }

    if (FAILED(hr = CCache::Create(&pCache, NULL))) {
        goto exit;
    }

    hr = pCache->RetrieveTransCacheEntry(pName, CACHE_GAC, &pTransCache);
    if((hr != S_OK)) {
        goto exit;
    }

    pszPath = pTransCache->_pInfo->pwzPath;
    pCName = static_cast<CAssemblyName*> (pName);

    hr = CheckFileExistence(pszPath, &bExists, NULL);
    if (FAILED(hr)) {
        goto exit;
    }
    else if (!bExists) {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        goto exit;
    }


    // Check Asm hash
    if ( dwFlags & QUERYASMINFO_FLAG_VALIDATE) {
        hr = ValidateAssembly(pszPath, pName);
    }

    if(pAsmInfo && SUCCEEDED(hr))
    {
        LPWSTR  szPath = NULL;
        
        szPath = pAsmInfo->pszCurrentAssemblyPathBuf;

       // if requested return the assembly path in cache.
        cbPath = lstrlenW(pszPath);

        if(szPath && (pAsmInfo->cchBuf > cbPath)) {
            hr = StringCchCopy(szPath, pAsmInfo->cchBuf, pszPath);
            if (FAILED(hr)) {
                goto exit;
            }
        }
        else {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        }

        pAsmInfo->cchBuf =  cbPath+1;
        pAsmInfo->dwAssemblyFlags = ASSEMBLYINFO_FLAG_INSTALLED;

        if ((dwFlags & QUERYASMINFO_FLAG_GETSIZE) /*&& SUCCEEDED(hr) */)
        {
            HRESULT     hrTmp = S_OK;
            
            hrTmp = GetAssemblyKBSize(pTransCache->_pInfo->pwzPath, &(pTransCache->_pInfo->dwKBSize), NULL, NULL);
            if(SUCCEEDED(hrTmp)) {
                pAsmInfo->uliAssemblySizeInKB.QuadPart = pTransCache->_pInfo->dwKBSize;
            }
            else if(SUCCEEDED(hr)) {
                hr = hrTmp;     // Return this error up only if everything else failed
            }
        }
    }

exit:

    if (hr == S_FALSE) {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    SAFERELEASE(pTransCache);
    SAFERELEASE(pCache);
    SAFERELEASE(pName);
    
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

STDMETHODIMP   CAssemblyCache::CreateAssemblyCacheItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ PVOID pvReserved,
        /* [out] */ IAssemblyCacheItem **ppAsmItem,
        /* [in, optional] */ LPCWSTR pszAssemblyName)  // uncanonicalized, comma separted name=value pairs.
{
    HRESULT hr = S_OK;
    
    BEGIN_ENTRYPOINT_NOTHROW;

    if(!ppAsmItem)
        IfFailGo(E_INVALIDARG);

    hr = CAssemblyCacheItem::Create(NULL, NULL, NULL, NULL, CACHE_GAC, NULL, pszAssemblyName, NULL, ppAsmItem);

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;

}


STDMETHODIMP  CAssemblyCache::CreateAssemblyScavenger(
        /* [out] */ IUnknown **ppAsmScavenger )
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;

    if(!ppAsmScavenger)
        IfFailGo(E_INVALIDARG);


    hr = CreateScavenger( ppAsmScavenger );

ErrExit:
    END_ENTRYPOINT_NOTHROW;
    return hr;
}

//
// IUnknown boilerplate...
//

STDMETHODIMP
CAssemblyCache::QueryInterface(REFIID riid, void** ppvObj)
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;


    if (!ppvObj) 
        IfFailGo(E_POINTER);
    
    
    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IAssemblyCache)
       )
    {
        *ppvObj = static_cast<IAssemblyCache*> (this);
        AddRef();
        goto ErrExit;
    }
    else
    {
        *ppvObj = NULL;
        IfFailGo(E_NOINTERFACE);
    }

ErrExit:
    END_ENTRYPOINT_NOTHROW;

    return hr;

}

STDMETHODIMP_(ULONG)
CAssemblyCache::AddRef()
{
    return InterlockedIncrement (&_cRef);
}

STDMETHODIMP_(ULONG)
CAssemblyCache::Release()
{
    BEGIN_CLEANUP_ENTRYPOINT
    ULONG lRet = InterlockedDecrement (&_cRef);
    if (!lRet)
        delete this;
    END_CLEANUP_ENTRYPOINT
    return lRet;
}

