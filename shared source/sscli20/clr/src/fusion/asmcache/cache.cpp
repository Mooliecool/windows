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
#include "cache.h"
#include "naming.h"
#include "asmstrm.h"
#include "util.h"
#include "helpers.h"
#include "transprt.h"
#include "cacheutils.h"
#include "strongname.h"

HRESULT VerifySignatureHelper(CTransCache *pTC, DWORD dwVerifyFlags);

// ---------------------------------------------------------------------------
// CCache  ctor
// ---------------------------------------------------------------------------
CCache::CCache()
: _cRef(1)
,_pwzCachePath(NULL)
{
    _dwSig = 0x48434143; /* 'HCAC' */
}

HRESULT CCache::Init(IApplicationContext *pAppCtx)
{
    HRESULT hr = S_OK;
    DWORD ccCachePath = 0;

    if (pAppCtx)
    {
        hr = pAppCtx->GetAppCacheDirectory(NULL, &ccCachePath);
        
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            // Path not found, but it's ok
            hr = S_OK;
        }
        else
        {
            if (ccCachePath && ccCachePath <= MAX_PATH)
            {
                _pwzCachePath = NEW(WCHAR[ccCachePath]);
                if (!_pwzCachePath)
                {
                    hr = E_OUTOFMEMORY;
                    goto Exit;
                }
                
                hr = pAppCtx->GetAppCacheDirectory(_pwzCachePath, &ccCachePath);
            }
            else
            {
                // Else path too long
                hr = FUSION_E_INVALID_NAME;
            }
        }
    }   
Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CCache  dtor
// ---------------------------------------------------------------------------
CCache::~CCache()
{
    SAFEDELETEARRAY(_pwzCachePath);
}

STDMETHODIMP_(ULONG) CCache::AddRef()
{
    return InterlockedIncrement (&_cRef);
}

STDMETHODIMP_(ULONG) CCache::Release()
{
    ULONG lRet = InterlockedDecrement (&_cRef);
    if (!lRet)
        delete this;
    return lRet;
}

HRESULT CCache::QueryInterface(REFIID iid, void **ppvObj)
{
    HRESULT hr = E_NOINTERFACE;

    // Does not actually implement any interface at all

    return hr;
}    

// ---------------------------------------------------------------------------
// CCache::Create
//---------------------------------------------------------------------------
HRESULT CCache::Create(CCache **ppCache, IApplicationContext *pAppCtx)
{
    HRESULT hr=S_OK;
    CCache *pCache = NULL;
    DWORD   cb = 0;

    // Check to see if an instance of CCache is in pAppCtx.
    // Assume, if it is present, it means if a custom path is
    // also present, that the CCache pointer points to a
    // CCache with the custom path as specified by
    // ACTAG_APP_CUSTOM_CACHE_PATH in pAppCtx.
    // (assumes the custom cache path cannot be not modified)

    if (pAppCtx)
    {
        cb = sizeof(void*);
        hr = pAppCtx->Get(ACTAG_APP_CACHE, (void*)&pCache, &cb, APP_CTX_FLAGS_INTERFACE);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            if (SUCCEEDED(hr))
            {
                _ASSERTE(pCache);
                *ppCache = pCache;
                (*ppCache)->AddRef();

                //Already AddRef-ed in pAppCtx->Get()
            }
            goto exit;
        }
    }

    // Not found, create a new CCache
    pCache = NEW(CCache());
    if (!pCache)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }

    hr = pCache->Init(pAppCtx);
    if (FAILED(hr))
        goto exit;

    if (pAppCtx)
    {
        // Set the new CCache to the AppCtx
        hr = pAppCtx->Set(ACTAG_APP_CACHE, (void*)pCache, sizeof(void*), APP_CTX_FLAGS_INTERFACE);
        if (FAILED(hr))
            goto exit;
    }
    
    *ppCache = pCache;
    (*ppCache)->AddRef();

exit:
    SAFERELEASE(pCache);

    return hr;
}

// ---------------------------------------------------------------------------
// CCache::GetCustomPath
//---------------------------------------------------------------------------

LPCWSTR CCache::GetCustomPath()
{
    // Return custom cache path if present
    return (_pwzCachePath) ? _pwzCachePath : NULL;
}


//------------------- Transport Cache APIs ------------------------------------

// ---------------------------------------------------------------------------
// CCache::RetrievTransCacheEntry
// Retrieves transport entry from transport cache
//---------------------------------------------------------------------------
HRESULT CCache::RetrieveTransCacheEntry(IAssemblyName *pName,
    DWORD dwFlags, CTransCache **ppTransCache)
{
    HRESULT hr;
    DWORD dwCmpMask = 0;
    DWORD dwVerifyFlags;
    CTransCache *pTransCache = NULL;
    CTransCache *pTransCacheMax = NULL;

    if (IsGacType(dwFlags) || IsZapType(dwFlags)) {
        dwVerifyFlags = SN_INFLAG_ADMIN_ACCESS;
    }
    else {
        _ASSERTE(dwFlags & CACHE_DOWNLOAD);
        dwVerifyFlags = SN_INFLAG_USER_ACCESS;
    }
    
    // Fully specified - direct lookup.
    // Partial - enum global cache only.

    // If fully specified, do direct lookup.
    if (!(CAssemblyName::IsPartial(pName, &dwCmpMask)))
    {    
        // Create a transcache entry from name.
        if (FAILED(hr = TransCacheEntryFromName(pName, dwFlags, &pTransCache)))
            goto exit;
    
        // Retrieve this record from the database.
        if (FAILED(hr = pTransCache->Retrieve()))
            goto exit;

    }
    // Ref is partial - enum global cache.
    else
    {
        // Should only be in here if enumerating global cache or retrieving custom assembly.
        _ASSERTE (IsGacType(dwFlags) ||  IsZapType(dwFlags) || (dwCmpMask & ASM_CMPF_CUSTOM));
                
        // Create a transcache entry from name.
        if (FAILED(hr = TransCacheEntryFromName(pName, dwFlags, &pTransCache)))
            goto exit;

        // Retrieve the max entry.
        if (FAILED(hr = pTransCache->Retrieve(&pTransCacheMax, dwCmpMask)))
            goto exit;            
    }
    
exit:

    // Failure.
    if (FAILED(hr) || (hr == S_FALSE))    
    {
        SAFERELEASE(pTransCache);
        SAFERELEASE(pTransCacheMax);
    }
    // Success.
    else
    {
        if (pTransCacheMax)
        {
            *ppTransCache = pTransCacheMax;
            SAFERELEASE(pTransCache);
        }
        else
        {
            *ppTransCache = pTransCache;
        }
    }
    
    return hr;    
}

// ---------------------------------------------------------------------------
// CCache::GetGlobalMax
//---------------------------------------------------------------------------
HRESULT CCache::GetGlobalMax(IAssemblyName *pName, 
    IAssemblyName **ppNameGlobal, CTransCache **ppTransCache)
{
    HRESULT         hr = NOERROR;
    DWORD           dwCmpMask = 0;
    BOOL            fIsPartial = FALSE;
    CTransCache    *pTransCache = NULL, *pTransCacheMax = NULL;
    IAssemblyName  *pNameGlobal = NULL;
    CCache         *pCache = NULL;

    if (FAILED(hr = Create(&pCache, NULL)))
        goto exit;
        
    // Create query trans cache object.
    if (FAILED(hr = pCache->TransCacheEntryFromName(
        pName, CACHE_GAC, &pTransCache)))
        goto exit;

    // For both full and partially specified, mask off version.
    fIsPartial = CAssemblyName::IsPartial(pName, &dwCmpMask);
    if (!fIsPartial)
        dwCmpMask = ASM_CMPF_NAME | ASM_CMPF_CULTURE | ASM_CMPF_PUBLIC_KEY_TOKEN | ASM_CMPF_CUSTOM;
    else            
        dwCmpMask &= ~(ASM_CMPF_MAJOR_VERSION | ASM_CMPF_MINOR_VERSION | ASM_CMPF_REVISION_NUMBER | ASM_CMPF_BUILD_NUMBER);

    // Retrieve the max entry.
    if (FAILED(hr = pTransCache->Retrieve(&pTransCacheMax, dwCmpMask)))
        goto exit;            

    // Found a match.
    if (hr == S_OK)
    {
        // If version matches or exceeds, return.
        ULONGLONG ullVer = 0;

        hr = CAssemblyName::GetVersion(pName, FALSE, &ullVer);
        if (FAILED(hr)) {
            goto exit;
        }

        if (pTransCacheMax->GetVersion() 
            >= ullVer)
        {
            hr = S_OK;
            if (FAILED(hr = CCache::NameFromTransCacheEntry(
                pTransCacheMax, &pNameGlobal)))
                goto exit;
        }
        else
        {
            hr = S_FALSE;
        }
    }

exit:

    if (hr == S_OK)
    {
        *ppTransCache = pTransCacheMax;
        *ppNameGlobal = pNameGlobal;
    }
    else
    {
        SAFERELEASE(pTransCacheMax);
        SAFERELEASE(pNameGlobal);
    }

    SAFERELEASE(pTransCache);
    SAFERELEASE(pCache);

    return hr;
}


//------------------- Cache Utility APIs -------------------------------------


// ---------------------------------------------------------------------------
// CCache::TransCacheEntryFromName
// create transport entry from name
//---------------------------------------------------------------------------
HRESULT CCache::TransCacheEntryFromName(IAssemblyName *pName, 
    DWORD dwFlags, CTransCache **ppTransCache)
{    
    HRESULT hr;
    DWORD cb, dwCacheId = 0;
    TRANSCACHEINFO *pTCInfo = 0;
    CTransCache *pTransCache = NULL;

    _ASSERTE(pName);

    // Get the correct cache index.
    if(FAILED(hr = ResolveCacheIndex(pName, dwFlags, &dwCacheId)))
        goto exit;

    // Construct new CTransCache object.
    if(FAILED(hr = CreateTransCacheEntry(dwCacheId, &pTransCache)))
        goto exit;

    // Cast base info ptr to TRANSCACHEINFO ptr
    pTCInfo = (TRANSCACHEINFO*) pTransCache->_pInfo;
        
    // Downcased text name from target
    hr = NameObjGetWrapper(pName, ASM_NAME_NAME, (LPBYTE*) &pTCInfo->pwzName, &(cb = 0));

    if (SUCCEEDED(hr)) {
        for (DWORD i=0; i<4; i++) {
            cb = sizeof(WORD);
            hr = pName->GetProperty(ASM_NAME_MAJOR_VERSION+i, &(pTCInfo->wVers[i]), &cb);
            if (FAILED(hr)) {
                goto exit;
            }
        }
    }

    if (SUCCEEDED(hr)) {
        hr = NameObjGetWrapper(pName, ASM_NAME_CULTURE, (LPBYTE*) &pTCInfo->pwzCulture, &cb);
    }

    if (SUCCEEDED(hr)) {
        hr = NameObjGetWrapper(pName, ASM_NAME_PUBLIC_KEY_TOKEN, &pTCInfo->blobPKT.pBlobData, &pTCInfo->blobPKT.cbSize);
    }

    if (SUCCEEDED(hr)) {
        hr = NameObjGetWrapper(pName, ASM_NAME_SIGNATURE_BLOB, &pTCInfo->blobSignature.pBlobData, &pTCInfo->blobSignature.cbSize);
    }

    if (SUCCEEDED(hr)) {
        hr = NameObjGetWrapper(pName, ASM_NAME_MVID, &pTCInfo->blobMVID.pBlobData, &pTCInfo->blobMVID.cbSize);
    }
    
    if (SUCCEEDED(hr)) {
        hr = NameObjGetWrapper(pName, ASM_NAME_CODEBASE_URL, (LPBYTE*) &pTCInfo->pwzCodebaseURL, &(cb = 0));
    }

    if (SUCCEEDED(hr)) {
        hr = pName->GetProperty(ASM_NAME_CODEBASE_LASTMOD, &pTCInfo->ftLastModified, &(cb = sizeof(FILETIME)));
    }

    if(SUCCEEDED(hr)) {
        CAssemblyName   *pCName = static_cast<CAssemblyName*>(pName); // dynamic_cast
        DWORD           dwSize = 0;

        // Returns S_OK if no modifier exists
        hr = pCName->GetPathModifier(NULL, &dwSize);
        
        if(hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            pTCInfo->pwzPathModifier = NEW(WCHAR[dwSize]);
            if(!pTCInfo->pwzPathModifier) {
                hr = E_OUTOFMEMORY;
                goto exit;
            }

            hr = pCName->GetPathModifier(pTCInfo->pwzPathModifier, &dwSize);
        }
    }

    if (SUCCEEDED(hr)) {
        if(pTCInfo->pwzName && (lstrlenW(pTCInfo->pwzName) >= MAX_PATH) )
            hr = FUSION_E_INVALID_NAME;  // name is too long; this is an error.
    }

    if (SUCCEEDED(hr)) {
        CAssemblyName   *pCName = static_cast<CAssemblyName*>(pName); // dynamic_cast
        pTCInfo->fLegacyAssembly = pCName->IsLegacyAssembly();

        hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE) &pTCInfo->dwAsmImageType, &(cb = sizeof(DWORD)));
     }

exit:
    if (SUCCEEDED(hr))
    {
        *ppTransCache = pTransCache;
    }
    else
    {
        SAFERELEASE(pTransCache);
    }
    return hr;
}


// ---------------------------------------------------------------------------
// CCache::NameFromTransCacheEntry
// convert target assembly name from name res entry
//---------------------------------------------------------------------------
HRESULT CCache::NameFromTransCacheEntry(
    CTransCache         *pTransCache,   
    IAssemblyName      **ppName
)
{
    HRESULT hr;
    TRANSCACHEINFO *pTCInfo = NULL;

    LPBYTE pbPublicKeyToken, pbSignature, pbMVID;
    DWORD  cbPublicKeyToken, cbSignature, cbMVID;

    // IAssemblyName target to be returned.
    IAssemblyName *pNameFinal = NULL;
    
    pTCInfo = (TRANSCACHEINFO*) pTransCache->_pInfo;

    // Currently this function is only called during enuming
    // the global cache so we expect an PublicKeyToken to be present.
    // BUT THIS IS NO LONGER TRUE - THE TRANSPORT CACHE CAN BE
    // INDEPENDENTLY ENUMERATED BUT AM LEAVING IN _ASSERTE AS COMMENT.
    // _ASSERTE(pTCInfo->blobPKT.cbSize);
    
    pbPublicKeyToken = pTCInfo->blobPKT.pBlobData;
    cbPublicKeyToken = pTCInfo->blobPKT.cbSize;

    pbSignature = pTCInfo->blobSignature.pBlobData;
    cbSignature = pTCInfo->blobSignature.cbSize;

    pbMVID = pTCInfo->blobMVID.pBlobData;
    cbMVID = pTCInfo->blobMVID.cbSize;

    // Create final name on text name and set properties.
    if (FAILED(hr = CreateAssemblyNameObject(&pNameFinal, pTCInfo->pwzName, NULL, 0)))
        goto exit;

    if(FAILED(hr = pNameFinal->SetProperty(cbPublicKeyToken ? 
        ASM_NAME_PUBLIC_KEY_TOKEN : ASM_NAME_NULL_PUBLIC_KEY_TOKEN,
        pbPublicKeyToken, cbPublicKeyToken)))
        goto exit;

    for (DWORD i=0; i<4; i++) {
        if(FAILED(hr = pNameFinal->SetProperty(ASM_NAME_MAJOR_VERSION + i, 
                 &(pTCInfo->wVers[i]), sizeof(WORD))))
            goto exit;
    }

    // Culture
    if(pTCInfo->pwzCulture)
    {
        if(FAILED(hr = pNameFinal->SetProperty(ASM_NAME_CULTURE,
            pTCInfo->pwzCulture, (lstrlenW(pTCInfo->pwzCulture) +1) * sizeof(TCHAR))))
            goto exit;
    }

    if(pbSignature)
    {
        // Signature blob
        if(FAILED(hr = pNameFinal->SetProperty(ASM_NAME_SIGNATURE_BLOB, pbSignature, cbSignature)))
            goto exit;
    }

    if(pbMVID)
    {
        // MVID
        if(FAILED(hr = pNameFinal->SetProperty(ASM_NAME_MVID, pbMVID, cbMVID)))
            goto exit;
    }

    // For GAC's, we must have both runtime version 
    // and architecture
    if(IsGacType(pTransCache->GetCacheType())) {
        if(!pTCInfo->fLegacyAssembly) {
            if(pTCInfo->dwAsmImageType) {
                // Assembly Image Type
                hr = pNameFinal->SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE) &pTCInfo->dwAsmImageType, sizeof(pTCInfo->dwAsmImageType));
                if(FAILED(hr)) {
                    goto exit;
                }
            }
            else {
                _ASSERTE(!"Non Legacy Assembly must have processor architecture.");
            }
        }
    }

    if(pTCInfo->pwzCodebaseURL)
    {
        // Codebase url
        if(FAILED(hr = pNameFinal->SetProperty(ASM_NAME_CODEBASE_URL,
             pTCInfo->pwzCodebaseURL, pTCInfo->pwzCodebaseURL ? 
                (DWORD)((lstrlenW(pTCInfo->pwzCodebaseURL) +1) * sizeof(TCHAR)) : 0)))
                goto exit;
    }

    // Codebase url last modified filetime
    if(FAILED(hr = pNameFinal->SetProperty(ASM_NAME_CODEBASE_LASTMOD,
             &pTCInfo->ftLastModified, sizeof(FILETIME))))
        goto exit;

    // Path modifier for ASP.NET installations
    if(pTCInfo->pwzPathModifier) {
        CAssemblyName   *pCName = static_cast<CAssemblyName*>(pNameFinal); // dynamic_cast

        hr = pCName->SetPathModifier(pTCInfo->pwzPathModifier);
        if(FAILED(hr)) {
            goto exit;
        }
    }

    // We're done and can hand out the target name.
    hr = S_OK;       

exit:
    if (SUCCEEDED(hr))
    {
        *ppName = pNameFinal;
    }
    else
    {
        SAFERELEASE(pNameFinal);
    }
    return hr;
}


// ---------------------------------------------------------------------------
// CCache::ResolveCacheIndex
//---------------------------------------------------------------------------
HRESULT CCache::ResolveCacheIndex(IAssemblyName *pName, 
    DWORD dwFlags, LPDWORD pdwCacheId)
{
    HRESULT hr = S_OK;
    DWORD   dwCmpMask = 0;
    BOOL    fIsPartial = FALSE;

    _ASSERTE(pdwCacheId);
    
    // Resolve index from flag and name.    
    if(dwFlags & CACHE_DOWNLOAD)
    {
        *pdwCacheId = dwFlags;
    }
    else if(IsGacType(dwFlags))
    {
        fIsPartial = CAssemblyName::IsPartial(pName, &dwCmpMask);


        // Name can be strongly named, or indeterminate ref or custom.
        if (! (CAssemblyName::IsStronglyNamed(pName) || (fIsPartial && !(dwCmpMask & ASM_CMPF_PUBLIC_KEY_TOKEN))) )
        {
            hr = FUSION_E_PRIVATE_ASM_DISALLOWED;
            goto exit;
        }

        *pdwCacheId = dwFlags;
    }
    else if(IsZapType(dwFlags))
    {
        fIsPartial = CAssemblyName::IsPartial(pName, &dwCmpMask);

        *pdwCacheId = dwFlags;
    }
    // Raw index passed in which is mirrored back.
    else
    {
        *pdwCacheId = dwFlags;
        hr = E_INVALIDARG;
    }

exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CCache::CreateTransCacheEntry
//---------------------------------------------------------------------------
HRESULT CCache::CreateTransCacheEntry(DWORD dwCacheId, CTransCache **ppTransCache)
{
    HRESULT hr = S_OK;

    if (FAILED(hr = CTransCache::Create(ppTransCache, dwCacheId, this)))
        goto exit;

exit:
    return hr;
}

