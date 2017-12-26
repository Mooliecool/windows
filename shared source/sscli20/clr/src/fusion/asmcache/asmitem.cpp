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
#include "asm.h"
#include "asmitem.h"
#include "asmstrm.h"
#include "naming.h"
#include "asmimprt.h"
#include "helpers.h"
#include "asmcache.h"
#include "appctx.h"
#include "util.h"
#include "scavenger.h"
#include "cacheutils.h"
#include "policy.h"
#include "lock.h"
#include "fusconfig.h"
#include "strongname.h"


extern CRITSEC_COOKIE g_csInitClb;


// ---------------------------------------------------------------------------
// CAssemblyCacheItem ctor
// ---------------------------------------------------------------------------
CAssemblyCacheItem::CAssemblyCacheItem()
{
    _dwSig           = 0x54494341; /* 'TICA' */
    _cRef            = 1;
    _pName           = NULL;
    _hrError         = S_OK;
    _cStream         = 0;
    _dwAsmSizeInKB   = 0;
    _szDir[0]        = 0;
    _cwDir           = 0;
    _szManifest[0]   = 0;
    _szDestManifest[0] =0;
    _pszAssemblyName = NULL;
    _pManifestImport = NULL;
    _pStreamHashList = NULL;
    _pszUrl          = NULL;
    _pTransCache     = NULL;
    _pCache          = NULL;
    _dwCacheFlags    = CACHE_INVALID;
    _hFile           = INVALID_HANDLE_VALUE;
    _hGACLock        = INVALID_HANDLE_VALUE;
    memset(&_ftLastMod, 0, sizeof(FILETIME));
    _bNeedMutex      = FALSE;
    _bCommitDone     = FALSE;
    _pRefData        = NULL;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem dtor
// ---------------------------------------------------------------------------
CAssemblyCacheItem::~CAssemblyCacheItem()
{
    _ASSERTE (!_cStream);

    if(_pStreamHashList)
        _pStreamHashList->DestroyList();

    SAFERELEASE(_pManifestImport);
    SAFERELEASE(_pName);
    SAFERELEASE(_pTransCache);
    SAFERELEASE(_pCache);
    SAFEDELETEARRAY(_pszUrl);
    SAFEDELETEARRAY(_pszAssemblyName);


    if(_hFile != INVALID_HANDLE_VALUE)
        CloseHandle(_hFile);

    
    // Fix #ASURT 113095 - Cleanup temp dirs if installation is incomplete / fails
    if( ((_hrError != S_OK) || (_bCommitDone == FALSE)) && _szDir[0])
    {
        HRESULT hr;
        hr = RemoveDirectoryAndChildren (_szDir);
    }

    // FusionMoveFile may left the source directory in tmp. Clean it here. 
    WszRemoveDirectory(_szDir);
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::Create
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::Create(IApplicationContext *pAppCtx,
    IAssemblyName *pName, LPCTSTR pszUrl, FILETIME *pftLastMod,
    DWORD dwCacheFlags,    IAssemblyManifestImport *pManImport, 
    LPCWSTR pszAssemblyName, LPCFUSION_INSTALL_REFERENCE pRefData,
    IAssemblyCacheItem **ppAsmItem)
{
    HRESULT               hr       = S_OK;
    CAssemblyCacheItem  *pAsmItem = NULL;


    if (!ppAsmItem) 
    {
        hr = E_INVALIDARG;
        goto exit;
    }

    *ppAsmItem = NULL;

    pAsmItem = NEW(CAssemblyCacheItem);
    if (!pAsmItem) 
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    if (FAILED(hr = pAsmItem->Init(pAppCtx, pName, pszUrl, pftLastMod, 
        (CACHE_FLAGS) dwCacheFlags, pRefData, pManImport)))
        goto exit;
 
    if(pszAssemblyName)
    {
        pAsmItem->_pszAssemblyName = WSTRDupDynamic(pszAssemblyName);
        if (!(pAsmItem->_pszAssemblyName)) {
            hr = E_OUTOFMEMORY;
            goto exit;
        }
    }

    *ppAsmItem = pAsmItem;
    (*ppAsmItem)->AddRef();

exit:
    SAFERELEASE(pAsmItem);

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::Init
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::Init(IApplicationContext *pAppCtx,
    IAssemblyName *pName, LPCTSTR pszUrl,
    FILETIME *pftLastMod, CACHE_FLAGS dwCacheFlags,
    LPCFUSION_INSTALL_REFERENCE pRefData,
    IAssemblyManifestImport *pManifestImport)
{
    HRESULT hr = S_OK;
    LPWSTR pszManifestPath = NULL;
    LPWSTR pszTmp;
    DWORD  cbManifestPath;
    IAssemblyManifestImport *pImport = NULL;

    // Save off cache flags.

    _dwCacheFlags = dwCacheFlags;


    // Create the cache
    if (FAILED(hr = CCache::Create(&_pCache, pAppCtx)))
        goto exit;
    PREFIX_ASSUME(_pCache != NULL);
    
    _bNeedMutex = ((_dwCacheFlags & CACHE_DOWNLOAD) && (_pCache->GetCustomPath() == NULL));

    if(_bNeedMutex)
    {
        if(FAILED(_hrError = CreateCacheMutex()))
            goto exit;
    }

    // If an IAssemblyName passed in, then this will be used
    // to lookup and modify the corresponding cache entry.
    if (pName)
    {
        // Set the assembly name definition.
        SetNameDef(pName);

        if (!pManifestImport) {
            // Retrieve associated cache entry from trans cache.
            hr = _pCache->RetrieveTransCacheEntry(_pName, _dwCacheFlags, &_pTransCache);
            if (FAILED(hr)) {
                goto exit;
            }
            if (hr != S_OK){
                hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
                goto exit;
            }

            // Get full path to manifest.
            pszManifestPath = _pTransCache->_pInfo->pwzPath;

            if (FAILED(hr = CreateAssemblyManifestImport(pszManifestPath, &pImport)))
                goto exit;

            hr = StringCbCopy(_szManifest, sizeof(_szManifest), pszManifestPath);
            if (FAILED(hr)) {
                goto exit;
            }
        }
        else {
            pImport = pManifestImport;
            pImport->AddRef();

            hr = _pCache->TransCacheEntryFromName(_pName, _dwCacheFlags, &_pTransCache);
            if (FAILED(hr)) {
                goto exit;
            }

            cbManifestPath = ARRAYSIZE(_szManifest);
            hr = pImport->GetManifestModulePath(_szManifest, &cbManifestPath);
            if (FAILED(hr)) {
                goto exit;
            }

            _pTransCache->_pInfo->pwzPath = WSTRDupDynamic(_szManifest);
            if (!_pTransCache->_pInfo->pwzPath) {
                hr = E_OUTOFMEMORY;
                goto exit;
            }
            pszManifestPath = _pTransCache->_pInfo->pwzPath;
        }

        // Cache the manifest.
        SetManifestInterface(pImport);

        if((_hFile == INVALID_HANDLE_VALUE) &&
                FAILED(hr = GetManifestFileLock(pszManifestPath, &_hFile)))
        {
            goto exit;
        }

        hr = StringCbCopy(_szDir, sizeof(_szDir), pszManifestPath);
        if (FAILED(hr)) {
            goto exit;
        }

        pszTmp = PathFindFileName(_szDir);
        *(pszTmp-1) = L'\0';
        _cwDir = (DWORD)(pszTmp - _szDir);

        // NOTE - since we have a transport cache entry there is no
        // need to set the url and last modified.
    } 
    else
    {
        // If no IAssemblyName provided, then this cache item will be used
        // to create a new transport cache item.

        // **Note - url and last modified are required if the assembly
        // being comitted is simple. If however it is strong or custom,
        // url and last modified are not required. We can check for 
        // strongly named at this point, but if custom the data will 
        // be set just prior to commit so we cannot enforce this at init.
        

        _ASSERTE(!_pszUrl);

        if(pszUrl)
            _pszUrl = WSTRDupDynamic(pszUrl);

        if(pftLastMod)
            memcpy(&_ftLastMod, pftLastMod, sizeof(FILETIME));

        // Set the manifest import interface if present.
        if (pManifestImport)
            SetManifestInterface(pManifestImport);
    }


exit:
    SAFERELEASE(pImport);

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::SetNameDef
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::SetNameDef(IAssemblyName *pName)
{  
    HRESULT hr = S_OK;

    if(_pName == pName)
        return S_OK;
    
    hr = IsValidAssemblyOnThisPlatform(pName, TRUE);
    if(FAILED(hr)) {
        return hr;
    }

    if(_pName)
        _pName->Release();

    _pName = pName;
    pName->AddRef();
    return S_OK;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::GetNameDef
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::GetNameDef(IAssemblyName **ppName)
{  
    _ASSERTE(ppName && _pName);

    *ppName = _pName;
    
    if(_pName){
        _pName->AddRef();
    }

    return S_OK;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::SetManifestInterface
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::SetManifestInterface(IAssemblyManifestImport *pImport)
{    
    _ASSERTE(!_pManifestImport);

    _pManifestImport = pImport;
    _pManifestImport->AddRef();

    return S_OK;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::GetFileHandle
// ---------------------------------------------------------------------------
HANDLE CAssemblyCacheItem::GetFileHandle()
{
    HANDLE hFile = _hFile;
    _hFile = INVALID_HANDLE_VALUE;
    return hFile;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::IsManifestFileLocked
// ---------------------------------------------------------------------------
BOOL CAssemblyCacheItem::IsManifestFileLocked()
{
    if(_hFile == INVALID_HANDLE_VALUE)
        return FALSE;
    else
        return TRUE;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::GetManifestInterface
// ---------------------------------------------------------------------------
IAssemblyManifestImport* CAssemblyCacheItem::GetManifestInterface()
{
    if (_pManifestImport)
        _pManifestImport->AddRef();
    return _pManifestImport;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::GetManifestPath
// ---------------------------------------------------------------------------
LPTSTR CAssemblyCacheItem::GetManifestPath()
{
    return _szDestManifest;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::StreamDone
// ---------------------------------------------------------------------------
void CAssemblyCacheItem::StreamDone (HRESULT hr)
{    
    _ASSERTE (_cStream);
    if (hr != S_OK)
        _hrError = hr;
    InterlockedDecrement (&_cStream);
}


// ---------------------------------------------------------------------------
// CAssemblyCacheItem::CreateCacheDir
// ---------------------------------------------------------------------------

HRESULT CAssemblyCacheItem::CreateCacheDir( 
    /* [in]  */  LPCOLESTR pszCustomPath,
    /* [in] */ LPCOLESTR pszName)
{
    _cwDir = MAX_PATH;
    _hrError = GetAssemblyStagingPath (pszCustomPath, _dwCacheFlags, 0, _szDir, &_cwDir);
    if (FAILED(_hrError)) {
        return _hrError;
    }

    // Compose with stream name, checking for path overflow.
    DWORD cwName = lstrlenW(pszName) + 1;
    if (_cwDir + cwName > MAX_PATH) // includes backslash
    {
        _hrError = HRESULT_FROM_WIN32(FUSION_E_INVALID_NAME);
        return _hrError;
    }

    _hrError = S_OK;

    return S_OK;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::CreateStream
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyCacheItem::CreateStream( 
        /* [in] */ DWORD dwFlags,                         // For general API flags
        /* [in] */ LPCWSTR pszName,                       // Name of the stream to be passed in
        /* [in] */ DWORD dwFormat,                        // format of the file to be streamed in.
        /* [in] */ DWORD dwFormatFlags,                   // format-specific flags
        /* [out] */ IStream **ppStream,
        /* [in, optional] */ ULARGE_INTEGER *puliMaxSize) // Max size of the Stream.
{
    BEGIN_ENTRYPOINT_NOTHROW;

    TCHAR szPath[MAX_PATH];
    CAssemblyStream* pstm = NULL;

    if(!pszName || !*pszName || !ppStream) {
        _hrError = E_INVALIDARG;
        goto exit;
    }

    *ppStream = NULL;

    // Do not allow path hackery.
    // need to validate this will result in a relative path within asmcache dir.
    // For now don't allow "\" in path; collapse the path before doing this.
    if (StrChr(pszName, DIR_SEPARATOR_CHAR))
    {
        _hrError = E_INVALIDARG;
        goto exit;
    }

    // Empty directory indicates - create a cache directory.
    if ( !_szDir[0] )
    {
        if (FAILED(_hrError = CreateCacheDir((LPOLESTR) _pCache->GetCustomPath(), (LPOLESTR) pszName)))
            goto exit;
    }
    // Dir exists - ensure final file path from name
    // does not exceed MAX_PATH chars.
    else
    {        
        DWORD cwName; 
        cwName = lstrlenW(pszName) + 1;
        if (_cwDir + cwName > MAX_PATH) // includes backslash
        {
            _hrError = FUSION_E_INVALID_NAME;
            goto exit;
        }
    }

    // Construct the stream object.
    pstm = NEW(CAssemblyStream(this));
    if (!pstm)
    {
        _hrError = E_OUTOFMEMORY;
        goto exit;
    }

    InterlockedIncrement (&_cStream);

    // Append trailing slash to path.
    _hrError = StringCbCopy(szPath, sizeof(szPath), _szDir);
    if (FAILED(_hrError)) {
        goto exit;
    }

    _hrError = PathAddBackslashWrap(szPath, MAX_PATH);
    if (FAILED(_hrError)) {
        goto exit;
    }

    // Generate cache file name
    switch (dwFormat)
    {
        case STREAM_FORMAT_COMPLIB_MANIFEST:
        {
            if((_dwCacheFlags & CACHE_DOWNLOAD) && (_pszUrl) 
                )
            {
                // for download cache get the manifest name from URL;
                // this will get around the name mangling done by IE cache.
                LPWSTR pszTemp = NULL;

                pszTemp = StrRChr(_pszUrl, NULL, URL_DIR_SEPERATOR_CHAR);
                if(pszTemp && (lstrlenW(pszTemp) > 1))
                {
                    if(lstrlenW(szPath) + lstrlenW(pszTemp)  >= MAX_PATH)
                    {
                        _hrError = FUSION_E_INVALID_NAME;
                        goto exit;
                    }

                    DWORD dwLen = lstrlenW(szPath);

                    _hrError = StringCbCopy(szPath+dwLen, sizeof(szPath) - dwLen * sizeof(WCHAR), pszTemp + 1);
                    if (FAILED(_hrError)) {
                        goto exit;
                    }
                    break;
                }
            }

            // Use passed in module name since we can't do
            // integrity checking to determine real name.
            _hrError = StringCbCat(szPath, sizeof(szPath), pszName);
            if (FAILED(_hrError)) {
                goto exit;
            }
            
            break;
        }

        case STREAM_FORMAT_COMPLIB_MODULE:
        {
            // Create a random filename since we will
            // do integrity checking later from which
            // we will determine the correct name.
            TCHAR*  pszFileName;
            pszFileName = szPath + lstrlenW(szPath);

            #define RANDOM_NAME_SIZE 8

            if (lstrlenW(szPath) + RANDOM_NAME_SIZE + 1 >= MAX_PATH) {
                _hrError = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
                goto exit;
            }

            // Loop until we get a unique file name.
            int i;

            for (i = 0; i < MAX_RANDOM_ATTEMPTS; i++)
            {
                BOOL        fExists = FALSE;
                
                GetRandomName (pszFileName, RANDOM_NAME_SIZE);

                _hrError = CheckFileExistence(szPath, &fExists, NULL);
                if(FAILED(_hrError)) {
                    goto exit;
                }
                else if(!fExists) {
                    _hrError = S_OK;
                    break;
                }
            }

            if (i >= MAX_RANDOM_ATTEMPTS) {
                _hrError = E_UNEXPECTED;
                goto exit;
            }

            break;
        }

        default:
            // Unhandle case
            _ASSERTE(!"Unexpected stream format");
            break;
    } // end switch

    // this creates Asm hierarchy (if required)
    if (FAILED(_hrError = CreateFilePathHierarchy(szPath)))
        goto exit;

    // Initialize stream object.
    if (FAILED(_hrError = pstm->Init ((LPOLESTR) szPath, dwFormat)))
        goto exit;

    // Record the manifest file path.
    switch(dwFormat)
    {
        case STREAM_FORMAT_COMPLIB_MANIFEST:
            _hrError = StringCbCopy(_szManifest, sizeof(_szManifest), szPath);
    }
    
    *ppStream = (IStream*) pstm;

exit:
    if (!SUCCEEDED(_hrError))
        SAFERELEASE(pstm);
    END_ENTRYPOINT_NOTHROW;

    return _hrError;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::CompareInputToDef
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::CompareInputToDef()
{
    HRESULT hr = S_OK;

    IAssemblyName *pName = NULL;

    if (FAILED(hr = CreateAssemblyNameObject(&pName, _pszAssemblyName, CANOF_PARSE_DISPLAY_NAME, 0)))
        goto exit;

    hr = _pName->IsEqual(pName, ASM_CMPF_DEFAULT);

exit:

   SAFERELEASE(pName);
   return hr;

}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::VerifyDuplicate
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::VerifyDuplicate(DWORD dwVerifyFlags, CTransCache *pTC)
{
    HRESULT hr = S_OK;
    IAssemblyName *pName = NULL;
    IAssemblyManifestImport           *pManifestImport=NULL;

    // we found a duplicate now do VerifySignature && def-def matching 
    if (CAssemblyName::IsStronglyNamed(_pName) && (_pCache->GetCustomPath() == NULL))
    {
        BOOL fWasVerified;
        if (!VerifySignature(_szDestManifest, &(fWasVerified = FALSE), dwVerifyFlags))
        {
            hr = FUSION_E_SIGNATURE_CHECK_FAILED;
            goto exit;
        }
    }

    if(FAILED(hr = GetFusionInfo(pTC, _szDestManifest)))
        goto exit;


    if(!pTC->_pInfo->pwzCodebaseURL || FusionCompareStringI(pTC->_pInfo->pwzCodebaseURL, _pszUrl))
    {
        hr = E_FAIL;
        goto exit;
    }

    if(_pCache->GetCustomPath() == NULL)
    {
        // ref-def matching in non-XSP case only
        if (FAILED(hr = CreateAssemblyManifestImport(_szDestManifest, &pManifestImport)))
            goto exit;

        // Get the read-only name def.
        if (FAILED(hr = pManifestImport->GetAssemblyNameDef(&pName)))
            goto exit;

        _ASSERTE(pName);

        hr = _pName->IsEqual(pName, ASM_CMPF_DEFAULT);
    }

exit:

   SAFERELEASE(pManifestImport);
   SAFERELEASE(pName);
   return hr;

}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::MoveAssemblyToFinalLocation
// ---------------------------------------------------------------------------
extern CRITSEC_COOKIE g_csSingleUse;
HRESULT CAssemblyCacheItem::MoveAssemblyToFinalLocation(DWORD dwFlags, DWORD dwVerifyFlags, ULONG *pulDisposition)
{
    HRESULT                 hr = S_OK;
    CTransCache             *pTransCache = NULL;
    CMutex                  cCacheMutex(_bNeedMutex ? g_hCacheMutex : INVALID_HANDLE_VALUE);
    CCriticalSection        cCacheCritSec(g_csSingleUse);
    WCHAR                   wzCacheLocation[MAX_PATH+1];
    WCHAR                   wzFullPath[MAX_PATH+1];
    WCHAR                   wzManifestFileName[MAX_PATH+1];
    WCHAR                   szAsmTextName[MAX_PATH+1];
    WCHAR                   szSubDirName[MAX_PATH+1];
    DWORD                   dwSize = 0;
    int                     iNewer = 0;
    BOOL                    bEntryFound = FALSE;
    BOOL                    bReplaceBits = FALSE;
    BOOL                    bNeedNewDir = FALSE;
    BOOL                    bIsPolicy = FALSE;
    BOOL                    fHasManifest = FALSE;
    BOOL                    fExists = TRUE;
    BOOL                    bCustom = (_pCache->GetCustomPath() != NULL);


    dwSize = MAX_PATH;
    hr = CreateAssemblyDirPath(_pCache->GetCustomPath(), _dwCacheFlags, 0, wzCacheLocation, &dwSize);
    if(FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCopy(wzFullPath, sizeof(wzFullPath), wzCacheLocation);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = PathAddBackslashWrap(wzFullPath, MAX_PATH);
    if(FAILED(hr)) {
        goto exit;
    }

    hr = GetCacheDirsFromName(_pName, _dwCacheFlags, szAsmTextName, 
        ARRAYSIZE(szAsmTextName), szSubDirName, ARRAYSIZE(szSubDirName), _pCache->GetCustomPath());
    if(FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCopy(wzManifestFileName, sizeof(wzManifestFileName), PathFindFileName(_szManifest));
    if (FAILED(hr)) {
        goto exit;
    }

    if(!FusionCompareStringNI(wzManifestFileName, POLICY_ASSEMBLY_PREFIX, lstrlenW(POLICY_ASSEMBLY_PREFIX))) {
        bIsPolicy = TRUE;
    }   
    
    if( (lstrlenW(wzFullPath) + lstrlenW(szAsmTextName) + lstrlenW(szSubDirName) + 
                lstrlenW(wzManifestFileName) + 4 /* (3) slashes, 1 null */ ) >= MAX_PATH )
    {
        hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
        goto exit;
    }

    hr = StringCbCat(wzFullPath, sizeof(wzFullPath), szAsmTextName);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = PathAddBackslashWrap(wzFullPath, MAX_PATH);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCat(wzFullPath, sizeof(wzFullPath), szSubDirName);
    if (FAILED(hr)) {
        goto exit;
    }

    if(_pTransCache)
    {
        // This seems to be incremental download. nothing to move.
        hr = S_OK;
        goto exit;
    }

    if( IsGacType(_dwCacheFlags)) {
        WCHAR       wzFileName[MAX_PATH + 1];

        // wzManifestFileName is MAX_PATH+1, so we can do a blind copy.
        
        hr = StringCbCopy(wzFileName, sizeof(wzFileName), wzManifestFileName);
        if (FAILED(hr)) {
            goto exit;
        }

        PathRemoveExtension(wzFileName);

        if(FusionCompareStringI(wzFileName, szAsmTextName) )
        {
            // manifest file name should be "asseblyname.dll" (or .exe ??)
            hr = FUSION_E_INVALID_NAME;
            goto exit;
        }
    }

    if (bCustom) {
        hr = cCacheCritSec.Lock();
    }
    else {    
        hr = cCacheMutex.Lock();
    }
    if (FAILED(hr)) {
        goto exit;
    }

    if(_dwCacheFlags == CACHE_DOWNLOAD) {
        dwSize = MAX_PATH;
        hr = GetRealAssemblyDir(wzFullPath, &fExists, wzFullPath, &dwSize);
        if (FAILED(hr)) {
            goto exit;
        }

        hr = S_OK;
    }

    hr = StringCchPrintf(_szDestManifest, MAX_PATH, L"%ws\\%ws", wzFullPath, wzManifestFileName);
    if (FAILED(hr)) {
        goto exit;
    }

    if (_dwCacheFlags != CACHE_DOWNLOAD || fExists) {
        // Create a transcache entry from name.
        if (FAILED(hr = _pCache->TransCacheEntryFromName(_pName, _dwCacheFlags, &pTransCache)))
            goto exit;

        // See if this assembly already exists.
        hr = pTransCache->Retrieve();
        if(FAILED(hr)) {
            goto exit;
        }
        else if (hr == S_OK) {
            hr = ValidateAssembly(_szDestManifest, _pName);
            if (hr == S_OK) {
                bEntryFound = TRUE;
            }
        }

        if(hr != S_OK) {
            hr = RemoveOldBits(_dwCacheFlags, pTransCache, fHasManifest, &bNeedNewDir);
            if (FAILED(hr)) {
                goto exit;
            }
        }
    }

    if(bEntryFound)
    {
        if(_dwCacheFlags & CACHE_DOWNLOAD)
        {
            hr = VerifyDuplicate(dwVerifyFlags, pTransCache);
            if(hr != S_OK)
                bReplaceBits = TRUE;
        }
        else if(IsGacType(_dwCacheFlags))
        {
            // always replace the bits
            bReplaceBits = TRUE;
        }
    }

    if(bEntryFound)
    {
        if( bReplaceBits 
            || (!iNewer && (dwFlags & IASSEMBLYCACHEITEM_COMMIT_FLAG_REFRESH)) // same file-version but still refresh
            || (dwFlags & IASSEMBLYCACHEITEM_COMMIT_FLAG_FORCE_REFRESH))  // don't care about file-versions! just overwrite
        {
            // We will replace it here. 
            // Update the flag so that ngen index knows bits are replaced.
            bReplaceBits = TRUE;
            
            hr = RemoveOldBits(_dwCacheFlags, pTransCache, fHasManifest, &bNeedNewDir);
            if (FAILED(hr)) {
                goto exit;
            }
        }
        else
        {
            SAFERELEASE(pTransCache);

            if (FAILED(hr = _pCache->CreateTransCacheEntry(_dwCacheFlags, &pTransCache)))
                goto exit;

            if(FAILED(hr = GetFusionInfo(pTransCache, wzFullPath)))
                goto exit;


            pTransCache->_pInfo->pwzPath = WSTRDupDynamic(_szDestManifest);

            _pTransCache = pTransCache;
            pTransCache->AddRef();

            if(_dwCacheFlags & CACHE_DOWNLOAD)
            {
                if(FAILED(hr = GetManifestFileLock(_szDestManifest, &_hFile)))
                    goto exit;

                hr = DB_E_DUPLICATE;
            }
            else
                hr = S_FALSE;

            goto exit;
        }
    }

    if (bNeedNewDir) 
    {
        DWORD dwPathLen = lstrlenW(wzFullPath);
        LPWSTR pwzTmp = wzFullPath + lstrlenW(wzFullPath); // end of wzFullPath
        WORD i;

        if ((dwPathLen + MAX_PATH_MODIFIER_LENGTH) > MAX_PATH) {
            hr = HRESULT_FROM_WIN32(ERROR_BUFFER_OVERFLOW);
            goto exit;
        }

        for (i=0; i < 65535; i++)
        {
            fExists = FALSE;
            
            hr = StringCchPrintf(pwzTmp, MAX_PATH_MODIFIER_LENGTH, L"_%d", i);
            if (FAILED(hr)) {
                goto exit;
            }

            hr = CheckFileExistence(wzFullPath, &fExists, NULL);
            if(FAILED(hr)) {
                goto exit;
            }
            else if(!fExists) {
                break;
            }
        }

        // fail after so many tries, let's fail.
        if (i >= 65535) {
            hr = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);
            goto exit;
        }
    }
        
    if(FAILED(hr = CreateFilePathHierarchy(wzFullPath)))
    {
        goto exit;
    }


    // Install an assembly is not going into the GAC
    // or the assembly doesn't have a Win32 manifest,
    // then install it
    if(!IsGacType(_dwCacheFlags) || !fHasManifest ) {
        hr = FusionMoveDirectory(_szDir, wzFullPath);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    hr = StringCbCopy(_szManifest, sizeof(_szManifest), wzFullPath);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = PathAddBackslashWrap(_szManifest, MAX_PATH);
    if (FAILED(hr)) {
        goto exit;
    }

    hr = StringCbCat(_szManifest, sizeof(_szManifest), wzManifestFileName);
    if (FAILED(hr)) {
        goto exit;
    }

    if (bNeedNewDir) {
        // We change where the assembly will go. 
        // Let's update _szDestManifest
        hr = StringCchPrintf(_szDestManifest, MAX_PATH, L"%ws\\%ws", wzFullPath, wzManifestFileName);
        if (FAILED(hr)) {
            goto exit;
        }
    }

    hr = S_OK;

    if(_dwCacheFlags & CACHE_DOWNLOAD) {
        if (FAILED(hr = GetManifestFileLock(_szDestManifest, &_hFile))) {
            goto exit;
        }
    }

    if(_pCache->GetCustomPath()) // delete older version of this assembly 
        FlushOldAssembly(_pCache->GetCustomPath(), wzFullPath, wzManifestFileName, FALSE);


exit:
    if(pulDisposition) {
        if(hr == S_OK) {
            if(!bEntryFound)
                *pulDisposition = IASSEMBLYCACHEITEM_COMMIT_DISPOSITION_INSTALLED;
            else
                *pulDisposition = IASSEMBLYCACHEITEM_COMMIT_DISPOSITION_REFRESHED;
        }
        else if(hr == S_FALSE || hr == DB_E_DUPLICATE) {
            *pulDisposition = IASSEMBLYCACHEITEM_COMMIT_DISPOSITION_ALREADY_INSTALLED;
        }
    }

    SAFERELEASE(pTransCache);

    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::AbortItem
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyCacheItem::AbortItem()
{
    BEGIN_ENTRYPOINT_NOTHROW;
    END_ENTRYPOINT_NOTHROW;

    return S_OK;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::Commit
// ---------------------------------------------------------------------------
STDMETHODIMP CAssemblyCacheItem::Commit(
    /* [in] */ DWORD dwFlags,
    /* [out, optional] */ ULONG *pulDisposition)
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    hr = LegacyCommit(dwFlags, pulDisposition);
    END_ENTRYPOINT_NOTHROW;

    return hr;
}

STDMETHODIMP CAssemblyCacheItem::LegacyCommit(
    /* [in] */ DWORD dwFlags,
    /* [out, optional] */ ULONG *pulDisposition)
{
    BOOL bDownLoadComplete = TRUE;
    CMutex  cCacheMutex(_bNeedMutex ? g_hCacheMutex : INVALID_HANDLE_VALUE);

    if(pulDisposition) {
        *pulDisposition = 0;
    }

    DWORD dwVerifyFlags = SN_INFLAG_INSTALL;
    if( IsGacType(_dwCacheFlags) || IsZapType(_dwCacheFlags)) {
        dwVerifyFlags |= SN_INFLAG_ADMIN_ACCESS;
    }
    else {
        _ASSERTE(_dwCacheFlags & CACHE_DOWNLOAD);
        dwVerifyFlags |= SN_INFLAG_USER_ACCESS;
    }
    
    // Check to make sure there are no errors.
    _ASSERTE(!_cStream);

    if (_cStream)
        _hrError = E_UNEXPECTED;

    if (!_pName)
        _hrError = COR_E_MISSINGMANIFESTRESOURCE;
    if (FAILED(_hrError))
        goto exit;


    if(_bNeedMutex && _pTransCache)
    { // take mutex if we are modifing existing bits(incremental download).
        if(FAILED(_hrError = cCacheMutex.Lock()))
            goto exit;
    }

    // Commit the assembly to the index.    
    if (FAILED(_hrError = CModuleHashNode::DoIntegrityCheck
        (_pStreamHashList, _pManifestImport, &bDownLoadComplete )))
        goto exit;

    if(_bNeedMutex && _pTransCache)
    {
        if(FAILED(_hrError = cCacheMutex.Unlock()))
            goto exit;
    }

    // check if all modules are in for GAC.
    if( IsGacType(_dwCacheFlags) && (!bDownLoadComplete))
    {
        _hrError = FUSION_E_ASM_MODULE_MISSING;
        goto exit;
    }

    // for GAC check if DisplayName passed-in matches with manifest-def
    if( _pszAssemblyName && IsGacType(_dwCacheFlags))
    {
        _hrError = CompareInputToDef();

        if(_hrError != S_OK)
        {
            _hrError = FUSION_E_INVALID_NAME;
            goto exit;
        }
    }

    // Verify signature if strongly named assembly
    if (CAssemblyName::IsStronglyNamed(_pName))
    {
        BOOL fWasVerified;
        if (!VerifySignature(_szManifest, &(fWasVerified = FALSE), dwVerifyFlags))
        {
            _hrError = FUSION_E_SIGNATURE_CHECK_FAILED;
            goto exit;
        }

    }

    // we are done with using ManifestImport. 
    // also releasing this helps un-lock assembly, needed for move.
    SAFERELEASE(_pManifestImport);

    if(!_pTransCache) // this asm is being added first time and not incremental download
    {
        // ** Create a transport cache entry **
        // For trans cache insertion we require codebase and last mod
    
        // Codebase
        _pName->SetProperty(ASM_NAME_CODEBASE_URL, (LPWSTR) _pszUrl, 
            _pszUrl ? (DWORD)((lstrlenW(_pszUrl) + 1) * sizeof(WCHAR)) : 0);

        // Codebase last modified time.
        _pName->SetProperty(ASM_NAME_CODEBASE_LASTMOD, &_ftLastMod, 
            sizeof(FILETIME));

        CAssemblyName           *pCName = static_cast<CAssemblyName*>(_pName); // dynamic_cast
        PEKIND                  dwAsmImageType = peNone;
        DWORD                   dwFileSizeLow = 0;
        DWORD                   dwSize = sizeof(PEKIND);

        // Only new Gac can be architecture specific
        if(!pCName->IsLegacyAssembly() && IsGacType(_dwCacheFlags)) {
            _hrError = _pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE) &(dwAsmImageType), &(dwSize));
            if(FAILED(_hrError)) {
                goto exit;
            }

            // Must have an Architecture
            if(!dwSize) {
                _ASSERTE(!"None legacy Assembly must have processor architecture");
                _hrError = FUSION_E_INVALID_NAME;
                goto exit;
            }

            MapAssemblyArchitectureToCacheType(dwAsmImageType, &_dwCacheFlags);
        }

        {
            if(FAILED(_hrError = StoreFusionInfo(_pName, _szDir, _dwCacheFlags, &dwFileSizeLow)))
            {
                goto exit;
            }
            else
            {
                AddStreamSize(dwFileSizeLow, 0); // add size of auxilary file to asm.
            }
        }
    }

    if( (_hrError = MoveAssemblyToFinalLocation(dwFlags, dwVerifyFlags, pulDisposition)) != S_OK)
        goto exit;

    if(_hrError == S_OK) {
        if((_dwCacheFlags & CACHE_DOWNLOAD) && (_pCache->GetCustomPath() == NULL) )
        {
            DoScavengingIfRequired(FALSE);

            if(FAILED(_hrError = cCacheMutex.Lock()))
                goto exit;

            SetDownLoadUsage( TRUE, _dwAsmSizeInKB );

            if(FAILED(_hrError = cCacheMutex.Unlock())) {
                goto exit;
            }
        }

    }

    CleanupTempDir(_dwCacheFlags, _pCache->GetCustomPath());

exit:
    _bCommitDone = TRUE;        // Set final commit flag
    return _hrError;
}

//
// IUnknown boilerplate...
//

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::QI
// ---------------------------------------------------------------------------
STDMETHODIMP
CAssemblyCacheItem::QueryInterface(REFIID riid, void** ppvObj)
{
    HRESULT hr = S_OK;
    BEGIN_ENTRYPOINT_NOTHROW;

    if (!ppvObj) 
        IfFailGo(E_POINTER);
    
    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_IAssemblyCacheItem)
       )
    {
        *ppvObj = static_cast<IAssemblyCacheItem*> (this);
        AddRef();
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

// Serialize access to _cRef even though this object is rental model
// w.r.t. to the client, but there may be multiple child objects which
// which can call concurrently.

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::AddRef
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyCacheItem::AddRef()
{
    ULONG refCount = 0;
    
    BEGIN_ENTRYPOINT_VOIDRET;
    refCount = InterlockedIncrement (&_cRef);
    END_ENTRYPOINT_VOIDRET;

    return refCount;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::Release
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CAssemblyCacheItem::Release()
{
    ULONG lRet = 0;
    BEGIN_CLEANUP_ENTRYPOINT;

    lRet = InterlockedDecrement (&_cRef);
    if (!lRet)
        delete this;

    END_CLEANUP_ENTRYPOINT;

    return lRet;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::LockStreamHashList
// ---------------------------------------------------------------------------
HRESULT CAssemblyCacheItem::AddToStreamHashList(CModuleHashNode *pModuleHashNode)
{
    HRESULT                                 hr = S_OK;
    CCriticalSection                        cs(g_csInitClb);
    
    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }
    pModuleHashNode->AddToList(&_pStreamHashList);
    
    cs.Unlock();

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CAssemblyCacheItem::AddStreamSize
// ---------------------------------------------------------------------------
void CAssemblyCacheItem::AddStreamSize(ULONG dwFileSizeLow, ULONG dwFileSizeHigh)
{    
    static ULONG dwKBMask = (1023); // 1024-1
    ULONG   dwFileSizeInKB = dwFileSizeLow >> 10 ; // strip of 10 LSB bits to convert from bytes to KB.

    if(dwKBMask & dwFileSizeLow)
        dwFileSizeInKB++; // Round up to the next KB.

    if(dwFileSizeHigh)
    {
        dwFileSizeInKB += (dwFileSizeHigh * (1 << 22) );
    }

    _dwAsmSizeInKB += dwFileSizeInKB;
}

HRESULT CAssemblyCacheItem::RemoveOldBits(CACHE_FLAGS dwAsmCacheType, CTransCache *pTransCache, BOOL fRemoveWinSxS, BOOL *pbNeedNewDir)
{
    HRESULT hr = S_OK;

    hr = CScavenger::DeleteAssembly(dwAsmCacheType, _pCache->GetCustomPath(),
                            _szDestManifest, TRUE);

    if (hr == HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION) ||
        hr == HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED) )
    {
        // Will try to copy assembly to new directory only at ASP.Net case.
        if ((_pCache->GetCustomPath() != NULL) && (dwAsmCacheType & CACHE_DOWNLOAD)) {
            *pbNeedNewDir = TRUE; 
            hr = S_OK;
        }
    }

    if(FAILED(hr)) {
        goto Exit;
    }


Exit:
    return hr;
}

HRESULT CAssemblyCacheItem::GetRealAssemblyDir(
    LPCWSTR pwzAsmDir, 
    __out LPBOOL pbExists, 
    __out_ecount_opt(*pdwSize) LPWSTR pwzRealAsmDir, 
    __inout LPDWORD pdwSize)
{
    HRESULT hr = S_OK;
    WCHAR   wzBuf[MAX_PATH];
    DWORD   dwSize = 0;
    CTransCache             *pTransCache = NULL;
    INT     i = 0;

    hr = StringCbCopy(wzBuf, sizeof(wzBuf), pwzAsmDir);
    if (FAILED(hr)) {
        goto exit;
    }

    while(TRUE) {
        hr = CheckFileExistence(wzBuf, pbExists, NULL);
        if (FAILED(hr)) {
            goto exit;
        }

        if (*pbExists) {
            // if we have collision, make sure it is the same codebase.    
            if (FAILED(hr = _pCache->CreateTransCacheEntry(_dwCacheFlags, &pTransCache)))
                goto exit;

            if(FAILED(hr = GetFusionInfo(pTransCache, wzBuf)))
                goto exit;

            if(!pTransCache->_pInfo->pwzCodebaseURL) {
                hr = StringCbCat(wzBuf, sizeof(wzBuf), L"\\1");
                if (FAILED(hr)) {
                    goto exit;
                }

                CScavenger::DeleteAssembly(_dwCacheFlags, _pCache->GetCustomPath(), wzBuf, TRUE);
            }
            else if(!FusionCompareStringI(pTransCache->_pInfo->pwzCodebaseURL, _pszUrl)) {
                // path exists, and it is the same codebase, we like it. 
                break;
            }
            SAFERELEASE(pTransCache);

            // we are here because the current asm dir is no good. Need to find a new one.
            if (i >= 65535) {
                hr = HRESULT_FROM_WIN32(ERROR_SHARING_VIOLATION);
                goto exit;
            }
            hr = StringCchPrintf(wzBuf, ARRAYSIZE(wzBuf), L"%ws_%d", pwzAsmDir, i++);
            if (FAILED(hr)) {
                goto exit;
            }
        }
        else {
            // if not exist, this is what we will use
            break;
        }
    }

    dwSize = lstrlenW(wzBuf) + 1;

    if (*pdwSize < dwSize) {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    else {
        memcpy(pwzRealAsmDir, wzBuf, dwSize * sizeof(WCHAR));
        hr = S_OK;
    }
    *pdwSize = dwSize;
        
exit:
    SAFERELEASE(pTransCache);
    return hr;
}

