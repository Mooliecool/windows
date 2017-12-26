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
#include "transprt.h"
#include "util.h"
#include "cache.h"
#include "scavenger.h"
#include "cacheutils.h"
#include "enum.h"
#include "strongname.h"

#define HIGH_WORD_MASK 0xffff0000
#define LOW_WORD_MASK 0x0000ffff

HRESULT VerifySignatureHelper(CTransCache *pTC, DWORD dwVerifyFlags);



// ---------------------------------------------------------------------------
// CTransCache  ctor
// ---------------------------------------------------------------------------
CTransCache::CTransCache()
{
    _cRef        = 1;
    _dwSig = 0x534e5254; /* 'SNRT' */
    _dwCacheType = CACHE_INVALID;
    _pCache = NULL;
    _pInfo = NULL;
}

HRESULT CTransCache::Init(DWORD dwCacheId, CCache *pCache)
{
    HRESULT hr = S_OK;

    _dwCacheType = (CACHE_FLAGS) dwCacheId;


    _pInfo = NEW(TRANSCACHEINFO);
    if (!_pInfo) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    memset(_pInfo, 0, sizeof(TRANSCACHEINFO));

    if (pCache)
    {
        _pCache = pCache;
        pCache->AddRef();
    }

Exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CTransCache  dtor
//---------------------------------------------------------------------------
CTransCache::~CTransCache()
{
    CleanInfo();
    SAFEDELETE(_pInfo);
    SAFERELEASE(_pCache);
}

LONG CTransCache::AddRef()
{
    return InterlockedIncrement (&_cRef);
}

LONG CTransCache::Release()
{
    ULONG lRet = InterlockedDecrement (&_cRef);
    if (!lRet)
        delete this;
    return lRet;
}

LPWSTR CTransCache::GetCustomPath()
{
    return (_pCache == NULL) ? NULL : (LPWSTR)_pCache->GetCustomPath();
}

void CTransCache::SetCacheType(CACHE_FLAGS cf)
{
    _dwCacheType = cf;
}

CACHE_FLAGS CTransCache::GetCacheType()
{
    return _dwCacheType;
}

// ---------------------------------------------------------------------------
// CTransCache::GetVersion
//---------------------------------------------------------------------------
ULONGLONG CTransCache::GetVersion()
{
    ULONGLONG ullVer = 0;
    ullVer = _pInfo->wVers[0];
    ullVer = (ullVer<< (sizeof(WORD) * 8)) | (_pInfo->wVers[1]);
    ullVer = (ullVer<< (sizeof(WORD) * 8)) | (_pInfo->wVers[2]);
    ullVer = (ullVer<< (sizeof(WORD) * 8)) | (_pInfo->wVers[3]);
    return ullVer;
}

// ---------------------------------------------------------------------------
// CTransCache::Create
//---------------------------------------------------------------------------
HRESULT CTransCache::Create(CTransCache **ppTransCache, DWORD dwCacheId)
{
    return CTransCache::Create(ppTransCache, dwCacheId, NULL);
}

// ---------------------------------------------------------------------------
// CTransCache::Create
//---------------------------------------------------------------------------
HRESULT CTransCache::Create(CTransCache **ppTransCache, DWORD dwCacheId, CCache *pCache)
{
    HRESULT hr=S_OK;
    CTransCache *pTransCache = NULL;

    pTransCache = NEW(CTransCache());
    if (!pTransCache)
    {
        hr = E_OUTOFMEMORY;
        goto exit;
    }
    
    hr = pTransCache->Init(dwCacheId, pCache);
    if (FAILED(hr)) {
        SAFEDELETE(pTransCache);
        goto exit;
    }
        
    *ppTransCache = pTransCache;

exit:
    return hr;
}

// ---------------------------------------------------------------------------
// CTransCache::Retrieve
//---------------------------------------------------------------------------
HRESULT CTransCache::Retrieve()
{
    return RetrieveFromFileStore(this);    
}

// ---------------------------------------------------------------------------
// CTransCache::Retrieve(CTransCache **, DWORD)
//---------------------------------------------------------------------------
HRESULT CTransCache::Retrieve(CTransCache **ppTransCache, DWORD dwCmpMask)
{
    HRESULT       hr = S_OK;
    DWORD        dwQueryMask = 0;
    CTransCache  *pTC = NULL, *pTCMax = NULL;
    CEnumCache   enumR(FALSE, NULL);
    
    // Map name mask to cache mask
    dwQueryMask = MapNameMaskToCacheMask(dwCmpMask);

    // Create an enumerator based on this entry.
    if (FAILED(hr = enumR.Init(this,  dwQueryMask)))
    {
        goto exit;
    }
    
    // Enum over cache.
    while(hr == S_OK)
    {
        // Create a transcache entry for output.
        if (FAILED(hr = Create(&pTC, (DWORD) _dwCacheType, _pCache)))
            goto exit;

        // Enumerate next entry.
        hr = enumR.GetNextRecord(pTC);
                
        // If the version is greater, 
        // save off max.
        if (hr == S_OK && pTC->GetVersion() >= GetVersion())
        {
            SAFERELEASE(pTCMax);
            pTCMax = pTC;
        }
        else
        {
            // Otherwise, release allocated transcache.
            SAFERELEASE(pTC)
        }
    }

exit:
    if (SUCCEEDED(hr))
    {
        if (pTCMax)
        {
            *ppTransCache = pTCMax;
            hr = S_OK;
        }
        else
        {
            hr = S_FALSE;
        }
    }
    return hr;
}


// ---------------------------------------------------------------------------
// CTransCache::CloneInfo
// Returns shallow copy of info pointer.
//---------------------------------------------------------------------------
TRANSCACHEINFO* CTransCache::CloneInfo()
{
    TRANSCACHEINFO *pClone = NULL;

    if (!_pInfo) {
        _ASSERTE(!"_pInfo is NULL!");
        goto exit;
    }
    
    pClone = NEW(TRANSCACHEINFO);
    if(!pClone)
        goto exit;
    memcpy(pClone, _pInfo, sizeof(TRANSCACHEINFO));

exit:
    return pClone;
}

// ---------------------------------------------------------------------------
// CTransCache::CleanInfo
// Deallocates TRANSCACHEINFO struct members.
//---------------------------------------------------------------------------
VOID CTransCache::CleanInfo(BOOL fFree)
{
    if(_pInfo)
    {
        if (fFree)
        {
            // Delete member ptrs.
            SAFEDELETEARRAY(_pInfo->pwzName);
            SAFEDELETEARRAY(_pInfo->blobPKT.pBlobData);
            SAFEDELETEARRAY(_pInfo->blobSignature.pBlobData);
            SAFEDELETEARRAY(_pInfo->blobMVID.pBlobData);
            SAFEDELETEARRAY(_pInfo->pwzCodebaseURL);
            SAFEDELETEARRAY(_pInfo->pwzPath);
            SAFEDELETEARRAY(_pInfo->blobPK.pBlobData);
            SAFEDELETEARRAY(_pInfo->pwzCulture);
            SAFEDELETEARRAY(_pInfo->pwzPathModifier);
        }
        
        // Zero out entire struct.
        memset(_pInfo, 0, sizeof(TRANSCACHEINFO));
    }
}

// ---------------------------------------------------------------------------
// CTransCache::IsMatch
//---------------------------------------------------------------------------
BOOL CTransCache::IsMatch(CTransCache *pRec, DWORD dwCmpMaskIn, LPDWORD pdwCmpMaskOut)
{
    BOOL fRet = TRUE;
    DWORD dwVerifyFlags;
    TRANSCACHEINFO*    pSource = NULL;
    TRANSCACHEINFO*    pTarget = NULL;


    dwVerifyFlags = SN_INFLAG_USER_ACCESS;
    
    // invalid params
    if( !pRec || !pRec->_pInfo || !pdwCmpMaskOut )
    {
        fRet = FALSE;
        goto exit;
    }

    if(!dwCmpMaskIn) // match all
        goto exit;

    
    // compare source(this object) with target(incoming object) 
    pSource = (TRANSCACHEINFO*)_pInfo;
    pTarget = (TRANSCACHEINFO*)(pRec->_pInfo);

    if(IsGacType(_dwCacheType) || IsZapType(_dwCacheType))
    {
        // Name
        if (dwCmpMaskIn & TCF_STRONG_PARTIAL_NAME)
        {
            if(pSource->pwzName && pTarget->pwzName
                && FusionCompareString(pSource->pwzName, pTarget->pwzName))
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_NAME;
        }

        // Culture
        if (dwCmpMaskIn & TCF_STRONG_PARTIAL_CULTURE)
        {
            if(pSource->pwzCulture && pTarget->pwzCulture
                && FusionCompareStringI(pSource->pwzCulture, pTarget->pwzCulture))
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_CULTURE;
        }        

        // PublicKeyToken
        if (dwCmpMaskIn & TCF_STRONG_PARTIAL_PUBLIC_KEY_TOKEN)
        {
            // Check if ptrs are different.
            if ((DWORD_PTR)(pSource->blobPKT.pBlobData) ^
                (DWORD_PTR)(pTarget->blobPKT.pBlobData))
            {
                // ptrs are different
                if (!((DWORD_PTR)pSource->blobPKT.pBlobData &&
                    (DWORD_PTR)pTarget->blobPKT.pBlobData) || // only one is NULL
                    ((pSource->blobPKT.cbSize != pTarget->blobPKT.cbSize) ||
                        (memcmp(pSource->blobPKT.pBlobData,
                            pTarget->blobPKT.pBlobData,
                            pSource->blobPKT.cbSize)))) // must both be non-NULL
                {
                    fRet = FALSE;
                    goto exit;
                }
                *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_PUBLIC_KEY_TOKEN;
            }            
        }

        // Major Version
        if (dwCmpMaskIn & TCF_STRONG_PARTIAL_MAJOR_VERSION)
        {
            if((pSource->wVers[0]) != (pTarget->wVers[0])) 
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_MAJOR_VERSION;
        }            

        // Minor Version
        if (dwCmpMaskIn & TCF_STRONG_PARTIAL_MINOR_VERSION)
        {
            if((pSource->wVers[1]) != (pTarget->wVers[1])) 
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_MINOR_VERSION;
        }            

        // Build number.
        if (dwCmpMaskIn & TCF_STRONG_PARTIAL_BUILD_NUMBER)
        {
            if((pSource->wVers[2]) != (pTarget->wVers[2]))
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_BUILD_NUMBER;
        }            

        // Revision number.
        if (dwCmpMaskIn & TCF_STRONG_PARTIAL_REVISION_NUMBER)
        {
            if((pSource->wVers[3]) != (pTarget->wVers[3]))
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_REVISION_NUMBER;
        }

        // Processor Architecture
        if(dwCmpMaskIn & TCF_STRONG_PARTIAL_ASSEMBLY_ARCHITECTURE) {
            if(pSource->dwAsmImageType != pTarget->dwAsmImageType) {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_STRONG_PARTIAL_ASSEMBLY_ARCHITECTURE;
        }
    }     
    else
    if(_dwCacheType & CACHE_DOWNLOAD) 
    {
        if (dwCmpMaskIn & TCF_SIMPLE_PARTIAL_CODEBASE_URL)
        {
            // column 1    
            if( FusionCompareStringI(pSource->pwzCodebaseURL, pTarget->pwzCodebaseURL) )
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_SIMPLE_PARTIAL_CODEBASE_URL;
        }            

        if (dwCmpMaskIn & TCF_SIMPLE_PARTIAL_CODEBASE_LAST_MODIFIED)
        {
            if(memcmp(&(pSource->ftLastModified), &(pTarget->ftLastModified), 
                  sizeof(FILETIME)))
            {
                fRet = FALSE;
                goto exit;
            }
            *pdwCmpMaskOut |= TCF_SIMPLE_PARTIAL_CODEBASE_LAST_MODIFIED;
        }
    }
    else {
        // invalid index
        _ASSERTE(!"Invalid Cache Flags!");
        goto exit;
    }


exit:
    return fRet;
}

// ---------------------------------------------------------------------------
// CTransCache::MapNameMaskToCacheMask
//---------------------------------------------------------------------------
DWORD CTransCache::MapNameMaskToCacheMask(DWORD dwNameMask)
{
    DWORD dwCacheMask = 0;

    if(IsGacType(_dwCacheType) || IsZapType(_dwCacheType)) {
        if(dwNameMask & ASM_CMPF_NAME)
            dwCacheMask |= TCF_STRONG_PARTIAL_NAME;
        if(dwNameMask & ASM_CMPF_CULTURE)
            dwCacheMask |= TCF_STRONG_PARTIAL_CULTURE;
        if(dwNameMask & ASM_CMPF_PUBLIC_KEY_TOKEN)
            dwCacheMask |= TCF_STRONG_PARTIAL_PUBLIC_KEY_TOKEN;
        if(dwNameMask & ASM_CMPF_MAJOR_VERSION)
            dwCacheMask |= TCF_STRONG_PARTIAL_MAJOR_VERSION;
        if(dwNameMask & ASM_CMPF_MINOR_VERSION)
            dwCacheMask |= TCF_STRONG_PARTIAL_MINOR_VERSION;
        if(dwNameMask & ASM_CMPF_REVISION_NUMBER)
            dwCacheMask |= TCF_STRONG_PARTIAL_REVISION_NUMBER;
        if(dwNameMask & ASM_CMPF_BUILD_NUMBER)
            dwCacheMask |= TCF_STRONG_PARTIAL_BUILD_NUMBER;
        if(dwNameMask & ASM_CMPF_CUSTOM)
            dwCacheMask |= TCF_STRONG_PARTIAL_CUSTOM;
        if(dwNameMask & ASM_CMPF_ARCHITECTURE)
            dwCacheMask |= TCF_STRONG_PARTIAL_ASSEMBLY_ARCHITECTURE;
    }
    else {
        //_ASSERTE(!"Invalid name mask!");
    }

    return dwCacheMask;
}
// ---------------------------------------------------------------------------
// CTransCache::MapCacheMaskToQueryCols
//---------------------------------------------------------------------------
DWORD CTransCache::MapCacheMaskToQueryCols(DWORD dwMask)
{
    DWORD rFlags[] = {TCF_STRONG_PARTIAL_NAME, TCF_STRONG_PARTIAL_CULTURE,
        TCF_STRONG_PARTIAL_PUBLIC_KEY_TOKEN, TCF_STRONG_PARTIAL_MAJOR_VERSION,
        TCF_STRONG_PARTIAL_MINOR_VERSION, TCF_STRONG_PARTIAL_BUILD_NUMBER,
        TCF_STRONG_PARTIAL_REVISION_NUMBER, TCF_STRONG_PARTIAL_ASSEMBLY_ARCHITECTURE};

    DWORD nCols = 0;

    if(IsGacType(_dwCacheType) || IsZapType(_dwCacheType)) {
        for (DWORD i = 0; i < ARRAYSIZE(rFlags); i++) {
            if(dwMask & rFlags[i]) {
                // Name, Cul, PKT
                if (i < 3) {
                    nCols++;
                }
                // VerMaj AND VerMin
                else if ((i == 3) && (dwMask & rFlags[4])) {
                    nCols++;
                }
                // RevNo AND BuildNo
                else if ((i == 5) && (dwMask & rFlags[6])) {
                    nCols++;
                }
                // Processor Architecture
                else if(i == 7) {
                    nCols++;
                }
            }
            else
                break;
        }
    }
    else if(_dwCacheType & CACHE_DOWNLOAD) {
        if (dwMask & TCF_SIMPLE_PARTIAL_CODEBASE_URL)
        {
            nCols++;
            if(dwMask & TCF_SIMPLE_PARTIAL_CODEBASE_LAST_MODIFIED)
            {
                nCols++;
            }
        }
    }
    else {
        // Invalid cache type
        _ASSERTE(!"Invalid cache flags");
    }

    return nCols;
}


