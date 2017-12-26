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
#include "naming.h"
#include "asm.h"
#include "hostasm.h"
#include "actasm.h"
#include "lock.h"
#include "memoryreport.h"

//
// Activated Assembly Node
//

CActivatedAssembly::CActivatedAssembly(IAssemblyName *pName, IUnknown *pAsm)
: _pName(pName)
, _pAsm(pAsm)
{
    _ASSERTE(pAsm && pName);

    _pAsm->AddRef();
    _pName->AddRef();
    _pAsmNI = NULL;
    _bNativeImageProbed = FALSE;
}

CActivatedAssembly::~CActivatedAssembly()
{
    SAFERELEASE(_pAsm);
    SAFERELEASE(_pName);
    SAFERELEASE(_pAsmNI);
}

//
// Load Context
//

CLoadContext::CLoadContext(LOADCTX_TYPE ctxType)
: _ctxType(ctxType)
, _cs(NULL)
, _cRef(1)
{
}

CLoadContext::~CLoadContext()
{
    int                                i;
    LISTNODE                           pos;
    CActivatedAssembly                *pActAsmCur;

    for (i = 0; i < DEPENDENCY_HASH_TABLE_SIZE; i++) {
        pos = _hashDependencies[i].GetHeadPosition();

        while (pos) {
            pActAsmCur = _hashDependencies[i].GetNext(pos);
            _ASSERTE(pActAsmCur);

            SAFEDELETE(pActAsmCur);
        }

        _hashDependencies[i].RemoveAll();
    }

    if (_cs) {
        ClrDeleteCriticalSection(_cs);
    }
}

HRESULT CLoadContext::Init()
{
    HRESULT                              hr = S_OK;
    
    _cs = ClrCreateCriticalSection("Fusion: Load Context", CrstFusionLoadContext, (CrstFlags)(CRST_REENTRANCY | CRST_UNSAFE_ANYMODE));
    if (!_cs) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

Exit:
    return hr;
}

HRESULT CLoadContext::Lock()
{
    HRESULT                              hr = S_OK;

    ClrEnterCriticalSection(_cs);

    return hr;
}

HRESULT CLoadContext::Unlock()
{
    ClrLeaveCriticalSection(_cs);

    return S_OK;
}

HRESULT CLoadContext::Create(CLoadContext **ppLoadContext, LOADCTX_TYPE ctxType)
{
    HRESULT                               hr = S_OK;
    CLoadContext                         *pLoadContext = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionLoadContext");

    if (!ppLoadContext) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppLoadContext = NULL;

    pLoadContext = NEW(CLoadContext(ctxType));
    if (!pLoadContext) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pLoadContext->Init();
    if (FAILED(hr)) {
        goto Exit;
    }

    *ppLoadContext = pLoadContext;
    (*ppLoadContext)->AddRef();

Exit:
    SAFERELEASE(pLoadContext);

    return hr;
}

ULONG CLoadContext::AddRef()
{
    return InterlockedIncrement(&_cRef);
}

ULONG CLoadContext::Release()
{
    LONG              lRef = InterlockedDecrement(&_cRef);

    if (!lRef) {
        delete this;
    }

    return lRef;
}
HRESULT CLoadContext::CheckActivated(IAssemblyName *pName, IUnknown **ppAsm, CDebugLog *pdbglog)
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dwHash = 0;
    DWORD                                       dwDisplayFlags = ASM_DISPLAYF_CULTURE;
    DWORD                                       dwCmpFlags = ASM_CMPF_NAME|ASM_CMPF_CULTURE|ASM_CMPF_PUBLIC_KEY_TOKEN;
    LISTNODE                                    pos;
    CActivatedAssembly                         *pActAsm;
    CCriticalSection                            cs(_cs);

    _ASSERTE(pName && ppAsm);
    _ASSERTE(!CAssemblyName::IsPartial(pName));

    *ppAsm = NULL;

    if (CAssemblyName::IsStronglyNamed(pName)) {
        dwDisplayFlags |= (ASM_DISPLAYF_PUBLIC_KEY_TOKEN | ASM_DISPLAYF_VERSION);
        dwCmpFlags |= ASM_CMPF_VERSION;
    }

    CAssemblyName::GetHash(pName, dwDisplayFlags, DEPENDENCY_HASH_TABLE_SIZE, &dwHash);

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _hashDependencies[dwHash].GetHeadPosition();
    while (pos) {
        pActAsm = _hashDependencies[dwHash].GetNext(pos);
        _ASSERTE(pActAsm);
        
        // first not compare PA
        hr = pName->IsEqual(pActAsm->_pName, dwCmpFlags);
        if (FAILED(hr)) {
           goto Exit;
        }
        
       if (hr == S_OK) {
            // now compare PA

            hr = pName->IsEqual(pActAsm->_pName, ASM_CMPF_ARCHITECTURE);
            if (FAILED(hr)) {
               goto Exit;
            }

            if (hr == S_OK) {
                // Found activated assembly.
            
                *ppAsm = pActAsm->_pAsm;
                (*ppAsm)->AddRef();

                cs.Unlock();
                goto Exit;
            }
            else {
                // find something in load context, everything else matches except PA. 
                // we should fail this bind. 
                cs.Unlock();
                hr = FUSION_E_REF_DEF_MISMATCH;
                DEBUGOUT(pdbglog, 0, ID_FUSLOG_LOAD_CONTEXT_PA_MISMATCH);
                goto Exit;
            }
            
        }
    }

    cs.Unlock();

    // Did not find matching activated assembly in this load context

    hr = S_FALSE;

Exit:
    return hr;
}

//
// CLoadContext::AddActivation tries to add pAsm into the given load context.
// In the event of a race, and the two assemblies being added are for the
// exact same name definition, then hr==S_FALSE will be returned, and
// ppAsmActivated will point to the already-activated assembly.
//

HRESULT CLoadContext::AddActivation(IUnknown *pAsm, IUnknown **ppAsmActivated)
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dwHash = 0;
    DWORD                                       dwDisplayFlags = ASM_DISPLAYF_CULTURE;
    CCriticalSection                            cs(_cs);
    CActivatedAssembly                         *pActAsm;
    IAssemblyName                              *pName = NULL;
    CActivatedAssembly                         *pActAsmCur;
    LISTNODE                                    pos;
    IAssembly                                  *pIAsm = NULL;
    CAssembly                                  *pCAsm = NULL; 
    IHostAssembly                              *pIHostAsm = NULL; 
    CHostAssembly                              *pCHostAsm = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionLoadContext");

    _ASSERTE(pAsm && ppAsmActivated);

    hr = pAsm->QueryInterface(IID_IAssembly, (void **)&pIAsm);
    if (SUCCEEDED(hr)) {
        pCAsm = static_cast<CAssembly *>(pIAsm);
        _ASSERTE(pCAsm);

        hr = pIAsm->GetAssemblyNameDef(&pName);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = pAsm->QueryInterface(IID_IHostAssembly, (void **)&pIHostAsm);
        if (FAILED(hr)) {
            goto Exit;
        }
        pCHostAsm = static_cast<CHostAssembly *>(pIHostAsm);
        _ASSERTE(pCHostAsm);

        hr = pIHostAsm->GetAssemblyNameDef(&pName);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    _ASSERTE(!CAssemblyName::IsPartial(pName));

    if (CAssemblyName::IsStronglyNamed(pName)) {
        dwDisplayFlags |= (ASM_DISPLAYF_PUBLIC_KEY_TOKEN | ASM_DISPLAYF_VERSION);
    }
    
    CAssemblyName::GetHash(pName,dwDisplayFlags, DEPENDENCY_HASH_TABLE_SIZE, &dwHash);

    // Create activated assembly node, and put the node into the table

    pActAsm = NEW(CActivatedAssembly(pName, pAsm));
    if (!pActAsm) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    
    hr = cs.Lock();
    if (FAILED(hr)) {
        SAFEDELETE(pActAsm);
        goto Exit;
    }

    // We should be able to just blindly add to the tail of this dependency
    // list, but just for sanity sake, make sure we don't already have
    // something with the same identity. If we do, then it means there must
    // have been two different downloads for the same name going on that didn't
    // get piggybacked into the same download object, before completion.

    pos = _hashDependencies[dwHash].GetHeadPosition();
    while (pos) {
        
        pActAsmCur = _hashDependencies[dwHash].GetNext(pos);
        _ASSERTE(pActAsmCur);

        if (pName->IsEqual(pActAsmCur->_pName, ASM_CMPF_DEFAULT) == S_OK) {
            // We must have hit a race adding to the load context. Return
            // the already-activated assembly.
            
            *ppAsmActivated = pActAsmCur->_pAsm;
            (*ppAsmActivated)->AddRef();

            SAFEDELETE(pActAsm);
            cs.Unlock();

            hr = S_FALSE;

            goto Exit;
        }
    }

    if (!_hashDependencies[dwHash].AddTail(pActAsm))
    {
        hr = E_OUTOFMEMORY;
        SAFEDELETE(pActAsm);
        cs.Unlock();
        goto Exit;
    }

    if (pCAsm) {
        pCAsm->SetLoadContext(this);
    }
    else {
        _ASSERTE(pCHostAsm);
        pCHostAsm->SetLoadContext(this);
    }
    
    cs.Unlock();

Exit:
    SAFERELEASE(pName);

    SAFERELEASE(pIAsm);
    SAFERELEASE(pIHostAsm);

    return hr;
}

HRESULT CLoadContext::RemoveActivation(IUnknown *pAsm)
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dwDisplayFlags = ASM_DISPLAYF_CULTURE;
    LISTNODE                                    pos;
    LISTNODE                                    oldpos;
    CCriticalSection                            cs(_cs);
    CActivatedAssembly                         *pActAsm;
    IAssemblyName                              *pName = NULL;
    DWORD                                       dwHash = 0;
    IAssembly                                  *pIAsm = NULL;
    IHostAssembly                              *pIHostAsm = NULL; 

    // By removing an activation, we may be losing the last ref count
    // on ourselves. Make sure the object is still alive, by doing a
    // manual addref/release around this block.

    AddRef(); 

    _ASSERTE(pAsm);

    hr = pAsm->QueryInterface(IID_IAssembly, (void **)&pIAsm);
    if (SUCCEEDED(hr)) {
        hr = pIAsm->GetAssemblyNameDef(&pName);
        SAFERELEASE(pIAsm);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = pAsm->QueryInterface(IID_IHostAssembly, (void **)&pIHostAsm);
        if (FAILED(hr)) {
            goto Exit;
        }
        hr = pIHostAsm->GetAssemblyNameDef(&pName);
        SAFERELEASE(pIHostAsm);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (CAssemblyName::IsStronglyNamed(pName)) {
        dwDisplayFlags |= (ASM_DISPLAYF_PUBLIC_KEY_TOKEN | ASM_DISPLAYF_VERSION);
    }

    CAssemblyName::GetHash(pName, dwDisplayFlags, DEPENDENCY_HASH_TABLE_SIZE, &dwHash);

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _hashDependencies[dwHash].GetHeadPosition();
    while (pos) {
        oldpos = pos;
        pActAsm = _hashDependencies[dwHash].GetNext(pos);
        _ASSERTE(pActAsm);

        if (pActAsm->_pAsm == pAsm) {

            // Found activated assembly.

            _hashDependencies[dwHash].RemoveAt(oldpos);

            // Leave critical section before deleting the activate
            // assembly node, because deleting the node causes the
            // pAssembly to be released, causing us to call the runtime
            // back to release the metadata import. This can't happen
            // while we hold a critical section, because we may deadlock
            // (issue with what GC mode we may be running in).

            cs.Unlock();
            SAFEDELETE(pActAsm);

            goto Exit;
        }
    }

    cs.Unlock();

    // Not found

    hr = S_FALSE;
    _ASSERTE(!"Assembly not found in activation context!");

Exit:
    SAFERELEASE(pName);

    Release();

    return hr;
}

LOADCTX_TYPE CLoadContext::GetContextType()
{
    return _ctxType;
}

HRESULT CLoadContext::SetNativeImage(IAssembly *pAsm, 
                                     CNativeImageAssembly *pAsmNI,
                                     CNativeImageAssembly **ppAsmNIActivation)
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dwDisplayFlags = ASM_DISPLAYF_CULTURE;
    LISTNODE                                    pos;
    CCriticalSection                            cs(_cs);
    CActivatedAssembly                         *pActAsm;
    IAssemblyName                              *pName = NULL;
    DWORD                                       dwHash = 0;

    _ASSERTE(pAsm);

    hr = pAsm->GetAssemblyNameDef(&pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (CAssemblyName::IsStronglyNamed(pName)) {
        dwDisplayFlags |= (ASM_DISPLAYF_PUBLIC_KEY_TOKEN | ASM_DISPLAYF_VERSION);
    }

    CAssemblyName::GetHash(pName, dwDisplayFlags, DEPENDENCY_HASH_TABLE_SIZE, &dwHash);

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _hashDependencies[dwHash].GetHeadPosition();
    while (pos) {
        pActAsm = _hashDependencies[dwHash].GetNext(pos);
        _ASSERTE(pActAsm);

        if (pActAsm->_pAsm == pAsm) {
            if (!pAsmNI) {
                // remove native image
                SAFERELEASE(pActAsm->_pAsmNI);
                pActAsm->_bNativeImageProbed = TRUE;
                hr = S_OK;
            }
            else {
                if (!pActAsm->_pAsmNI) {
                    // set native image
                    pActAsm->_pAsmNI = pAsmNI;
                    pActAsm->_pAsmNI->AddRef();
                    pActAsm->_pAsmNI->SetLoadContext(this);
                    pActAsm->_bNativeImageProbed = TRUE;
                    hr = S_OK;
                }
                else {
                    // there is already a native image in load context. 
                    // Re-use the one in load context.
                    if (ppAsmNIActivation) {
                        *ppAsmNIActivation = pActAsm->_pAsmNI;
                        (*ppAsmNIActivation)->AddRef();
                    }
                    hr = S_FALSE;
                }
            }
            goto Exit;
        }
    }

    cs.Unlock();

    // Not found

    hr = E_UNEXPECTED;
    _ASSERTE(!"Assembly not found in activation context!");

Exit:
    SAFERELEASE(pName);

    return hr;

}

HRESULT CLoadContext::GetNativeImage(IAssembly *pAsm, 
                                    CNativeImageAssembly **ppAsmNI, 
                                    LPBOOL pbNativeImageProbed)
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dwDisplayFlags = ASM_DISPLAYF_CULTURE;
    LISTNODE                                    pos;
    CCriticalSection                            cs(_cs);
    CActivatedAssembly                         *pActAsm;
    IAssemblyName                              *pName = NULL;
    DWORD                                       dwHash = 0;

    _ASSERTE(pAsm);

    hr = pAsm->GetAssemblyNameDef(&pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (CAssemblyName::IsStronglyNamed(pName)) {
        dwDisplayFlags |= (ASM_DISPLAYF_PUBLIC_KEY_TOKEN | ASM_DISPLAYF_VERSION);
    }

    CAssemblyName::GetHash(pName, dwDisplayFlags, DEPENDENCY_HASH_TABLE_SIZE, &dwHash);

    hr = cs.Lock();
    if (FAILED(hr)) {
        goto Exit;
    }

    pos = _hashDependencies[dwHash].GetHeadPosition();
    while (pos) {
        pActAsm = _hashDependencies[dwHash].GetNext(pos);
        _ASSERTE(pActAsm);

        if (pActAsm->_pAsm == pAsm) {
            if (pbNativeImageProbed) {
                *pbNativeImageProbed = pActAsm->_bNativeImageProbed;
            }
            
            if (ppAsmNI) {
                *ppAsmNI = pActAsm->_pAsmNI;
                if (*ppAsmNI) {
                    (*ppAsmNI)->AddRef();
                }
            }
            hr = S_OK;
            goto Exit;
        }
    }

    cs.Unlock();

    // Not found

    hr = E_UNEXPECTED;
    _ASSERTE(!"Assembly not found in activation context!");

Exit:
    SAFERELEASE(pName);

    return hr;

}
