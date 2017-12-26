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
#include "pcycache.h"
#include "helpers.h"
#include "appctx.h"
#include "lock.h"
#include "cache.h"
#include "naming.h"
#include "memoryreport.h"

extern CRITSEC_COOKIE g_csDownload;

//
// CPolicyMapping 
//

CPolicyMapping::CPolicyMapping()
: _pwzNameSource(NULL)
, _pwzCulture(NULL)
, _pe(peNone)
, _bRetarget(FALSE)
, _pwzNamePolicy(NULL)
, _cbPktPolicy(0)
, _pbPktPolicy(NULL)
, _pePolicy(peInvalid)
, _pBindHistory(NULL)
, _dwPoliciesApplied(0)
{
    memset(_wVersSource, 0, sizeof(_wVersSource));
    memset(_wVersPolicy, 0, sizeof(_wVersPolicy));
    memset(_pbPktSource, 0, sizeof(_pbPktSource));
}

CPolicyMapping::~CPolicyMapping()
{
    SAFEDELETEARRAY(_pwzNameSource);
    SAFEDELETEARRAY(_pwzCulture);
    SAFEDELETEARRAY(_pwzNamePolicy);
    SAFEDELETEARRAY(_pbPktPolicy);
    SAFERELEASE(_pBindHistory);
}

HRESULT CPolicyMapping::Create(IAssemblyName *pNameRefSource, 
                               IAssemblyName *pNameRefPolicy,
                               AsmBindHistoryInfo *pBindHistory, 
                               DWORD dwPoliciesApplied,
                               CPolicyMapping **ppMapping)
{
    HRESULT                                  hr = S_OK;
    CPolicyMapping                          *pMapping = NULL;

    if (!pNameRefSource || !pNameRefPolicy || !ppMapping) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppMapping = NULL;

    pMapping = NEW(CPolicyMapping());
    if (!pMapping) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pMapping->Init(pNameRefSource, pNameRefPolicy, 
                        pBindHistory, dwPoliciesApplied);
    if (FAILED(hr)) {
        goto Exit;
    }

    *ppMapping = pMapping;

Exit:
    return hr;
}

HRESULT CPolicyMapping::Init(IAssemblyName *pNameRefSource, 
                             IAssemblyName *pNameRefPolicy,
                             AsmBindHistoryInfo *pBindHistory,
                             DWORD dwPoliciesApplied)
{
    HRESULT         hr = S_OK;
    DWORD           dwSize = 0;
    DWORD           i;

    _ASSERTE(pNameRefSource && pNameRefPolicy);
    _ASSERTE(CAssemblyName::IsStronglyNamed(pNameRefSource) && !CAssemblyName::IsPartial(pNameRefSource));

    _dwPoliciesApplied = dwPoliciesApplied;

    _pBindHistory = pBindHistory;
    if (_pBindHistory) {
        _pBindHistory->AddRef();
    }

    // copy source name
    hr = NameObjGetWrapper(pNameRefSource, ASM_NAME_NAME, (LPBYTE *)&_pwzNameSource, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    // copy source culture
    hr = NameObjGetWrapper(pNameRefSource, ASM_NAME_CULTURE, (LPBYTE *)&_pwzCulture, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    // copy source public key token
    dwSize = sizeof(_pbPktSource);
    hr = pNameRefSource->GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, _pbPktSource, &dwSize);
    if (FAILED(hr)) {
        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            hr = FUSION_E_INVALID_NAME;
        }
        goto Exit;
    }

    // copy source versions. 
    for (i = 0 ; i < 4; i++) {
        dwSize = sizeof(WORD);
        hr = pNameRefSource->GetProperty(i + ASM_NAME_MAJOR_VERSION, (LPVOID)&_wVersSource[i], &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // copy policy versions
    for (i = 0 ; i < 4; i++) {
        dwSize = sizeof(WORD);
        hr = pNameRefPolicy->GetProperty(i + ASM_NAME_MAJOR_VERSION, (LPVOID)&_wVersPolicy[i], &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // copy source PA. 
    dwSize = sizeof(PEKIND);
    hr = pNameRefPolicy->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&_pe, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    // copy policy PA. 
    dwSize = sizeof(PEKIND);
    hr = pNameRefPolicy->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&_pePolicy, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    // copy retarget. 
    dwSize = sizeof(BOOL);
    hr = pNameRefPolicy->GetProperty(ASM_NAME_RETARGET, (LPBYTE)&_bRetarget, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (dwPoliciesApplied & ePolicyLevelRetargetable) {
        // we will need to store name and pkt as well for retargetable redirect.
        hr = NameObjGetWrapper(pNameRefPolicy, ASM_NAME_NAME, (LPBYTE*)&_pwzNamePolicy, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        hr = NameObjGetWrapper(pNameRefPolicy, ASM_NAME_PUBLIC_KEY_TOKEN, &_pbPktPolicy, &_cbPktPolicy);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
     
Exit:
    return hr;
}

HRESULT CPolicyMapping::IsMatchingSource(IAssemblyName *pName)
{
    HRESULT hr = S_OK;
    LPWSTR  pwzNameSource = NULL;
    LPWSTR  pwzCulture = NULL;
    WORD    wVers[4];
    BYTE    pbPkt[PUBLIC_KEY_TOKEN_LEN];
    DWORD   dwSize;
    PEKIND  pe = peNone;
    BOOL    bRetarget = FALSE;
    DWORD   i;

    memset(pbPkt, 0, sizeof(pbPkt));
    memset(wVers, 0, sizeof(wVers));

    // copy source name
    hr = NameObjGetWrapper(pName, ASM_NAME_NAME, (LPBYTE *)&pwzNameSource, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    _ASSERTE(pwzNameSource);

    // copy source culture
    hr = NameObjGetWrapper(pName, ASM_NAME_CULTURE, (LPBYTE *)&pwzCulture, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    _ASSERTE(pwzCulture);

    // copy source public key token
    dwSize = sizeof(pbPkt);
    hr = pName->GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, pbPkt, &dwSize);
    if (FAILED(hr)) {
        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            hr = FUSION_E_INVALID_NAME;
        }
        goto Exit;
    }

    _ASSERTE(dwSize);

    // copy source versions. 
    for (i = 0 ; i < 4; i++) {
        dwSize = sizeof(WORD);
        hr = pName->GetProperty(i + ASM_NAME_MAJOR_VERSION, (LPVOID)&wVers[i], &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
    dwSize = sizeof(PEKIND);
    hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&pe, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    // copy retarget. 
    dwSize = sizeof(BOOL);
    hr = pName->GetProperty(ASM_NAME_RETARGET, (LPBYTE)&bRetarget, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = S_OK;

    // now do comparison. 
     
    // name
    if (FusionCompareStringI(pwzNameSource, _pwzNameSource)) {
        hr = S_FALSE;
    }

    // culture
    if (hr == S_OK) {
        if (FusionCompareStringI(pwzCulture, _pwzCulture)) {
            hr = S_FALSE;
        }       
    }

    // pkt
    if (hr == S_OK) {
        if (memcmp(pbPkt, _pbPktSource, sizeof(pbPkt))) {
            hr = S_FALSE;
        }       
    }

    // version
    if (hr == S_OK) {
        for (i=0; i<4 && (hr==S_OK);i++) {
            if (wVers[i] != _wVersSource[i]) {
                hr = S_FALSE;
            }
        }
    }

    // PA
    if (hr == S_OK) {
        if (pe != _pe) {
            hr = S_FALSE;
        }
    }

    // retarget
    if (hr == S_OK) {
        if (bRetarget != _bRetarget) {
            hr = S_FALSE;
        }
    }   

Exit:
    SAFEDELETEARRAY(pwzNameSource);
    SAFEDELETEARRAY(pwzCulture);
    return hr;
}

//
// CPolicyCache
//

CPolicyCache::CPolicyCache()
: _cRef(1)
, _cs(NULL)
{
}

CPolicyCache::~CPolicyCache()
{
    LISTNODE                                  pos = NULL;
    CPolicyMapping                           *pMapping = NULL;
    int                                       i;

    for (i = 0; i < POLICY_CACHE_SIZE; i++) {
        pos = _listMappings[i].GetHeadPosition();

        while (pos) {
            pMapping = _listMappings[i].GetNext(pos);
            SAFEDELETE(pMapping);
        }

        _listMappings[i].RemoveAll();
    }

    if (_cs) {
        ClrDeleteCriticalSection(_cs);
    }
}

HRESULT CPolicyCache::Init()
{
    HRESULT                          hr = S_OK;

    _cs = ClrCreateCriticalSection("Fusion: CPolicyCache", CrstFusionPcyCache, CRST_DEFAULT);
    if (!_cs) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CPolicyCache::Create(CPolicyCache **ppPolicyCache)
{
    HRESULT                                   hr = S_OK;
    CPolicyCache                             *pPolicyCache = NULL;

    _ASSERTE(ppPolicyCache);

    pPolicyCache = NEW(CPolicyCache);
    if (!pPolicyCache) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pPolicyCache->Init();
    if (FAILED(hr)) {
        goto Exit;
    }

    *ppPolicyCache = pPolicyCache;
    (*ppPolicyCache)->AddRef();

Exit:
    SAFERELEASE(pPolicyCache);
    return hr;
}

HRESULT CPolicyCache::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT                                    hr = S_OK;

    if (!ppv) 
        return E_POINTER;
    
    *ppv = NULL;

    if (IsEqualIID(riid, IID_IUnknown)) {
        *ppv = static_cast<IUnknown *>(this);
    }
    else {
        hr = E_NOINTERFACE;
    }

    if (*ppv) {
        AddRef();
    }

    return hr;
}

STDMETHODIMP_(ULONG) CPolicyCache::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

STDMETHODIMP_(ULONG) CPolicyCache::Release()
{
    ULONG                    ulRef = InterlockedDecrement(&_cRef);

    if (!ulRef) {
        delete this;
    }

    return ulRef;
}

HRESULT CPolicyCache::LookupPolicy(IAssemblyName *pNameRefSource,
                                   IAssemblyName **ppNameRefPolicy,
                                   AsmBindHistoryInfo **ppBindHistory,
                                   DWORD *pdwPolicyApplied)
{
    HRESULT                                    hr = S_OK;
    CPolicyMapping                            *pMapping = NULL;
    LISTNODE                                   pos = NULL;
    DWORD                                      dwHash = 0;
    CCriticalSection                           cs(_cs);
    DWORD                                      dwFlags = ASM_DISPLAYF_VERSION
                                                      |  ASM_DISPLAYF_CULTURE
                                                      |  ASM_DISPLAYF_PUBLIC_KEY_TOKEN
                                                      |  ASM_DISPLAYF_RETARGET;
    IAssemblyName                             *pName = NULL;  
    DWORD                                      i; 

    if (!pNameRefSource || !ppNameRefPolicy) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    _ASSERTE(!*ppNameRefPolicy);

    *ppNameRefPolicy = NULL;
    if (ppBindHistory) {
        *ppBindHistory = NULL;
    }
    
    CAssemblyName::GetHash(pNameRefSource, dwFlags, POLICY_CACHE_SIZE, &dwHash);

    // Lookup the assembly

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _listMappings[dwHash].GetHeadPosition();
    while (pos) {
        pMapping = _listMappings[dwHash].GetNext(pos);
        _ASSERTE(pMapping);

        hr = pMapping->IsMatchingSource(pNameRefSource);
        if (FAILED(hr)) {
            cs.Unlock();
            goto Exit;
        }

        if (hr == S_OK) {
            if (ppBindHistory) {
                *ppBindHistory = pMapping->_pBindHistory;
                if (*ppBindHistory) {
                    (*ppBindHistory)->AddRef();
                }
            }

            hr = pNameRefSource->Clone(&pName);
            if (FAILED(hr)) {
                cs.Unlock();
                goto Exit;
            }
              
            for (i = 0; i < 4; i++) {
                hr = pName->SetProperty(i + ASM_NAME_MAJOR_VERSION, (LPVOID)&(pMapping->_wVersPolicy[i]), sizeof(WORD));
                if (FAILED(hr)) {
                    cs.Unlock();
                    goto Exit;
                }
            }

            if (pMapping->_pePolicy != peInvalid) {
                hr = pName->SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&(pMapping->_pePolicy), sizeof(PEKIND));
                if (FAILED(hr)) {
                    goto Exit;
                }
            }

            if (pMapping->_dwPoliciesApplied & ePolicyLevelRetargetable) {
                hr = pName->SetProperty(ASM_NAME_NAME, (LPVOID)(pMapping->_pwzNamePolicy), (lstrlenW(pMapping->_pwzNamePolicy) + 1) * sizeof(WCHAR));
                if (FAILED(hr)) {
                    cs.Unlock();
                    goto Exit;
                }

                hr = pName->SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, (LPVOID)(pMapping->_pbPktPolicy), pMapping->_cbPktPolicy);
                if (FAILED(hr)) {
                    cs.Unlock();
                    goto Exit;
                }
            }

            *ppNameRefPolicy = pName;
            (*ppNameRefPolicy)->AddRef();

            if (pdwPolicyApplied) {
                *pdwPolicyApplied = pMapping->_dwPoliciesApplied;
            }

            cs.Unlock();
            goto Exit;
        }
    }

    cs.Unlock();

    // Missed in policy cache

    hr = S_FALSE;

Exit:
    SAFERELEASE(pName);

    return hr;
}                                   

HRESULT CPolicyCache::InsertPolicy(IAssemblyName *pNameRefSource,
                                   IAssemblyName *pNameRefPolicy,
                                   AsmBindHistoryInfo *pBindHistory,
                                   DWORD dwPolicyApplied)
{
    HRESULT                                     hr = S_OK;
    CPolicyMapping                             *pMapping = NULL;
    DWORD                                       dwHash = 0;
    CCriticalSection                            cs(_cs);
    DWORD                                      dwFlags = ASM_DISPLAYF_VERSION
                                                      |  ASM_DISPLAYF_CULTURE
                                                      |  ASM_DISPLAYF_PUBLIC_KEY_TOKEN
                                                      |  ASM_DISPLAYF_RETARGET;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionPolicyCache");

    if (!pNameRefSource || !pNameRefPolicy) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    CAssemblyName::GetHash(pNameRefSource, dwFlags, POLICY_CACHE_SIZE, &dwHash);

    // Clone off the name mappings and add it to the hash table

    hr = CPolicyMapping::Create(pNameRefSource, pNameRefPolicy, pBindHistory, dwPolicyApplied, &pMapping);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (!_listMappings[dwHash].AddTail(pMapping))
    {
        hr = E_OUTOFMEMORY;
        SAFEDELETE(pMapping);
        cs.Unlock();
        goto Exit;
    }

    cs.Unlock();

Exit:
    return hr;
}

//
// PreparePolicyCache
//

HRESULT PreparePolicyCache(IApplicationContext *pAppCtx, CPolicyCache **ppPolicyCache)
{
    HRESULT                               hr = S_OK;
    DWORD                                 dwSize;
    CPolicyCache                         *pPolicyCache = NULL;
    CCriticalSection                      cs(g_csDownload);

    MEMORY_REPORT_CONTEXT_SCOPE("FusionPolicyCache");

    if (!pAppCtx) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = sizeof(pPolicyCache);
    hr = pAppCtx->Get(ACTAG_APP_POLICY_CACHE, &pPolicyCache, &dwSize, APP_CTX_FLAGS_INTERFACE);
    if (hr == S_OK) {
        // Already setup
    } 
    else {
        hr = CPolicyCache::Create(&pPolicyCache);
        if (FAILED(hr)) {
            cs.Unlock();
            goto Exit;
        }

        hr = pAppCtx->Set(ACTAG_APP_POLICY_CACHE, pPolicyCache, sizeof(pPolicyCache),
                          APP_CTX_FLAGS_INTERFACE);
        if (FAILED(hr)) {
            cs.Unlock();
            goto Exit;
        }
    }

    if (ppPolicyCache) {
        *ppPolicyCache = pPolicyCache;
        (*ppPolicyCache)->AddRef();
    }

    cs.Unlock();

Exit:
    SAFERELEASE(pPolicyCache);

    return hr;
}
