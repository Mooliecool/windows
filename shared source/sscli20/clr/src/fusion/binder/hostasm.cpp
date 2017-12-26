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
#include "asmimprt.h"
#include "hostasm.h"
#include "hostmodimprt.h"
#include "imprthelpers.h"
#include "naming.h"
#include "util.h"

HRESULT CreateHostAssemblyFromStream(
        IStream        *pAsmStream,
        IStream        *pDebugStream, 
        UINT64          AssemblyId,
        UINT64          AssemblyContext,
        AssemblyBindInfo *pBindInfo,
        IHostAssembly **ppHostAsm)

{
    HRESULT         hr          = S_OK;
    CHostAssembly  *pHostAsm    = NULL;

    _ASSERTE(ppHostAsm);
    *ppHostAsm = NULL;

    pHostAsm = NEW(CHostAssembly);
    if (!pHostAsm) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pHostAsm->Init(pAsmStream, pDebugStream, AssemblyId, AssemblyContext, pBindInfo);
    if (FAILED(hr)) {
        SAFERELEASE(pHostAsm);
        goto Exit;
    }

    *ppHostAsm = pHostAsm;

Exit:
    return hr;
}

CHostAssembly::CHostAssembly()
:_cRef(1)
,_pImport(NULL)
,_pName(NULL)
,_pLoadContext(NULL)
,_pDebugStream(NULL)
,_AsmId(0)
,_pAppCtx(NULL)
,_AsmContext(0)
,_rAssemblyRefTokens(NULL)
,_cAssemblyRefTokens((DWORD)(-1))
{
    _dwSig = 0x53414843; // "SAHC"
    memset((void *)&_bindInfo, 0, sizeof(AssemblyBindInfo));
}

CHostAssembly::~CHostAssembly()
{
    SAFERELEASE(_pImport);
    SAFERELEASE(_pName);
    SAFERELEASE(_pLoadContext);
    SAFERELEASE(_pDebugStream);

    SAFEDELETEARRAY(_bindInfo.lpReferencedIdentity);
    SAFEDELETEARRAY(_bindInfo.lpPostPolicyIdentity);
    SAFEDELETEARRAY(_rAssemblyRefTokens);

    LISTNODE pos;
    IHostAssemblyModuleImport *pImport;

    pos = _listModules.GetHeadPosition();
    while (pos) {
        pImport = _listModules.GetNext(pos);
        SAFERELEASE(pImport);
    }

    _listModules.RemoveAll();
}

HRESULT CHostAssembly::Init(
        IStream          *pAsmStream,
        IStream          *pDebugStream, 
        UINT64            AssemblyId,
        UINT64            AssemblyContext,
        AssemblyBindInfo *pBindInfo)
{
    HRESULT hr = S_OK;

    _ASSERTE(pAsmStream);
    _ASSERTE(pBindInfo);

    _pDebugStream = pDebugStream;
    if (_pDebugStream) {
        _pDebugStream->AddRef();
    }

    _AsmId = AssemblyId;
    _AsmContext = AssemblyContext;

    _bindInfo.dwAppDomainId = pBindInfo->dwAppDomainId;
    _bindInfo.ePolicyLevel = pBindInfo->ePolicyLevel;
    _bindInfo.lpReferencedIdentity = WSTRDupDynamic(pBindInfo->lpReferencedIdentity);
    if (!_bindInfo.lpReferencedIdentity) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    _bindInfo.lpPostPolicyIdentity = WSTRDupDynamic(pBindInfo->lpPostPolicyIdentity);
    if (!_bindInfo.lpPostPolicyIdentity) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = CopyMetaData(pAsmStream);

Exit:
    return hr;
}

STDMETHODIMP
CHostAssembly::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) {
        return E_POINTER;
    }

    *ppv = NULL;

    if (riid==IID_IHostAssembly || riid == IID_IUnknown) {
        *ppv = (IHostAssembly *)this;
        AddRef();
    }
    else {
        return E_NOINTERFACE;
    }

    return S_OK;
} 

// AddRef and Release are copied from CAssembly.
// Maybe some change is needed
STDMETHODIMP_(ULONG)
CHostAssembly::AddRef()
{
    HRESULT                             hr;
    LONG                                lRef = -1;
    
    if (_pLoadContext) {
        hr = _pLoadContext->Lock();
        if (hr == S_OK) {
            lRef = InterlockedIncrement((LONG*) &_cRef);
            _pLoadContext->Unlock();
        }
    }
    else {
        lRef = InterlockedIncrement((LONG *)&_cRef);
    }

    return lRef;
}

// ---------------------------------------------------------------------------
// CAssembly::Release
// ---------------------------------------------------------------------------
STDMETHODIMP_(ULONG)
CHostAssembly::Release()
{
    LONG                                      lRef = -1;

    if (_pLoadContext) {
        _pLoadContext->Lock();
        lRef = InterlockedDecrement(&_cRef);
        if (lRef == 1) {
            // There is a circular reference count between the 
            // load context and the CAssembly (CAssembly holds 
            // back pointer to load context, and load context 
            // holds on to activated node, which contains a ref 
            // to the CAssembly). When release causes the ref 
            // count to go to 1, we know the only ref count 
            // left is from the load context (as long as nobody 
            // messed up the ref counting). Thus, at this time, 
            // we need to remove ourselves from the load context 
            // list, which will in turn, cause a release of this 
            // object, so it is properly destroyed.

            // In RemoveActication we
            // have to QI ourself to get the assembly name def.
            // That will increase the ref count to 2. When we release
            // the QI, this logic kicks in again, causing an infinite loop.
            // We have to add some ref here to protect against this.
            //
            // We need to add 2 ref because we will do two release
            // in RemoveActivation, one for the QI, one for the copy
            // in load context.
            InterlockedIncrement(&_cRef);
            InterlockedIncrement(&_cRef);
            _pLoadContext->RemoveActivation((IHostAssembly *)this);
            InterlockedDecrement(&_cRef);
            lRef = InterlockedDecrement(&_cRef);
            _ASSERTE(lRef == 0);
        }
        _pLoadContext->Unlock();
    }
    else {
        lRef = InterlockedDecrement(&_cRef);

    }
    
    if (!lRef) {
        delete this;
    }

    return lRef;
}

STDMETHODIMP
CHostAssembly::GetAssemblyNameDef(IAssemblyName **ppName)
{
    HRESULT hr = S_OK;
    
    _ASSERTE(_pName);

    if (!ppName) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppName = _pName;
    if (*ppName) {
        (*ppName)->AddRef();
    }

Exit:
    return hr;
}

STDMETHODIMP
CHostAssembly::GetNextAssemblyNameRef(DWORD nIndex, IAssemblyName **ppName)
{
    HRESULT     hr = S_OK;
    mdAssembly  *rAssemblyRefTokens = NULL;
    DWORD        cAssemblyRefTokens = 0;
    mdAssemblyRef    mdmar;

    if (_cAssemblyRefTokens == (DWORD)(-1)) {
        IfFailGo(GetAssemblyRefTokens(_pImport, &rAssemblyRefTokens, &cAssemblyRefTokens));

        if (InterlockedCompareExchangePointer((void **)&_rAssemblyRefTokens, rAssemblyRefTokens, NULL)) {
            SAFEDELETEARRAY(rAssemblyRefTokens);
        }

        rAssemblyRefTokens = NULL;

        _cAssemblyRefTokens = cAssemblyRefTokens;
    } 

    _ASSERTE(_cAssemblyRefTokens != (DWORD)(-1));

    // Verify the index passed in. 
    if (nIndex >= _cAssemblyRefTokens)
    {
        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
        goto ErrExit;
    }

    // Reference indexed dep assembly ref token.
    mdmar = _rAssemblyRefTokens[nIndex];
    hr = GetAssemblyNameRefFromMDImport(_pImport, mdmar, ASM_IMPORT_NAME_HASH_VALUE, ppName);
        
ErrExit:        
    if (FAILED(hr)) {
        SAFEDELETEARRAY(rAssemblyRefTokens);
    }
    return hr;
}

STDMETHODIMP
CHostAssembly::GetNextAssemblyModule(
        DWORD nIndex, 
        IHostAssemblyModuleImport **ppModImport)
{
    HRESULT                                    hr = S_OK;
    LISTNODE                                   pos;
    DWORD                                      dwCount;
    DWORD                                      i;
    IHostAssemblyModuleImport                 *pImport = NULL;

    if (!ppModImport) {
        return E_INVALIDARG;
    }

    dwCount = _listModules.GetCount();
    if (nIndex >= dwCount) {
        hr = HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS);
        goto Exit;
    }

    pos = _listModules.GetHeadPosition();
    for (i = 0; i <= nIndex; i++) {
        pImport = _listModules.GetNext(pos);
    }

    _ASSERTE(pImport);

    *ppModImport = pImport;
    (*ppModImport)->AddRef();

Exit:
    return hr;
}

STDMETHODIMP
CHostAssembly::GetModuleByName(
        LPCOLESTR pszModuleName,
        IHostAssemblyModuleImport **ppModImport)
{
    HRESULT                hr;
    DWORD                  dwIdx      = 0;
    IHostAssemblyModuleImport *pModImport = NULL;

    if (!pszModuleName || !ppModImport) {
        return E_INVALIDARG;
    }
    
    // Enumerate the modules in this manifest.
    while (SUCCEEDED(hr = GetNextAssemblyModule(dwIdx++, &pModImport)))
    {
        TCHAR szModName[MAX_PATH];
        DWORD ccModName;
        ccModName = MAX_PATH;
        if (FAILED(hr = pModImport->GetModuleName(szModName, &ccModName)))
            goto exit;
            
        // Compare module name against given.
        if (!FusionCompareStringI(szModName, pszModuleName))
        {
            // Found the module
            break;
        }   
        SAFERELEASE(pModImport);
    }

    if (SUCCEEDED(hr)) {
        *ppModImport = pModImport;
        (*ppModImport)->AddRef();
    }

exit:
    SAFERELEASE(pModImport);
    return hr;    
}

STDMETHODIMP 
CHostAssembly::GetAssemblyId(
        /* out */  UINT64  *pAssemblyId)
{
    HRESULT hr = S_OK;

    if (!pAssemblyId) {
        return E_INVALIDARG;
    }

    *pAssemblyId = _AsmId;
    return hr;
}

STDMETHODIMP 
CHostAssembly::GetAssemblyContext(
        /* out */  UINT64  *pAsmContext)
{
    HRESULT hr = S_OK;

    if (!pAsmContext) {
        return E_INVALIDARG;
    }

    *pAsmContext = _AsmContext;
    return hr;
}

STDMETHODIMP
CHostAssembly::GetAssemblyStream(
        IStream **ppStreamAsm)
{
    HRESULT hr = S_OK;
    UINT64  AsmId = 0;
    UINT64  AsmContext = 0;
    IStream *pDebugStream = NULL;

    if (!ppStreamAsm ) {
        return E_INVALIDARG;
    }

    *ppStreamAsm = NULL;

    if (! g_pHostAssemblyStore ) {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto Exit;
    }

    {
        REMOVE_STACK_GUARD;
        hr = g_pHostAssemblyStore->ProvideAssembly(
                            &_bindInfo,
                            &AsmId,
                            &AsmContext,
                            ppStreamAsm,
                            &pDebugStream);
    }

    if (FAILED(hr)) {
        goto Exit;
    }

    if (!*ppStreamAsm) {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto Exit;
    }

Exit:
    SAFERELEASE(pDebugStream);
    return hr;
}

STDMETHODIMP
CHostAssembly::GetAssemblyDebugStream(
        IStream **ppDebugStream)
{
    HRESULT hr = S_OK;
    UINT64  AsmId = 0;
    UINT64  AsmContext = 0;
    IStream *pAsmStream = NULL;

    if (!ppDebugStream) {
        return E_INVALIDARG;
    }

    *ppDebugStream = NULL;

    *ppDebugStream= (IStream*)InterlockedExchangePointer((LPVOID *)&_pDebugStream, 0);
    if (!*ppDebugStream) {
        if (! g_pHostAssemblyStore ) {
            hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            goto Exit;
        }

        {
            REMOVE_STACK_GUARD;
            hr = g_pHostAssemblyStore->ProvideAssembly(
                                &_bindInfo,
                                &AsmId,
                                &AsmContext,
                                &pAsmStream,
                                ppDebugStream);
        }
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (!*ppDebugStream) {
        hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        goto Exit;
    }

Exit:
    SAFERELEASE(pAsmStream);
    return hr;
}

LOADCTX_TYPE CHostAssembly::GetFusionLoadContext()
{
    if (!_pLoadContext) {
        return LOADCTX_TYPE_UNKNOWN;
    }

    return _pLoadContext->GetContextType();
}

HRESULT CHostAssembly::SetLoadContext(CLoadContext *pLoadContext)
{
    _ASSERTE(!_pLoadContext);

    _pLoadContext = pLoadContext;
    if (_pLoadContext) {
        _pLoadContext->AddRef();
    }

    return S_OK;
}

HRESULT CHostAssembly::CopyMetaData(IStream *pStream)
{
    HRESULT hr = S_OK;
    HCORENUM      hEnum = 0;
    mdFile        mdf;
    TCHAR         szModuleName[MAX_PATH];
    DWORD         ccModuleName = MAX_PATH;
    const VOID    *pvHashValue = NULL;
    DWORD         cbHashValue = 0;
    DWORD         dwFlags = 0;
    mdFile        rAssemblyModuleTokens[ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE];
    DWORD         cAssemblyModuleTokens = ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE;
    DWORD         i;
    IHostAssemblyModuleImport  *pImport = NULL;
    ModuleBindInfo modBindInfo;
    DWORD         dwImportFlags = ASM_IMPORT_NAME_HASH_ALGID 
                                | ASM_IMPORT_NAME_SIGNATURE_BLOB 
                                | ASM_IMPORT_NAME_MVID 
                                | ASM_IMPORT_NAME_ARCHITECTURE 
                                | ASM_IMPORT_NAME_PE_RUNTIME;

    memset(&modBindInfo, 0, sizeof(ModuleBindInfo));
            
    hr = CreateMetaDataImport(pStream, _AsmId, &_pImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = GetAssemblyNameDefFromMDImport(_pImport, dwImportFlags, FALSE, &_pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    while (cAssemblyModuleTokens > 0) {
        hr = _pImport->EnumFiles(&hEnum, rAssemblyModuleTokens,
                                  ASM_MANIFEST_IMPORT_DEFAULT_ARRAY_SIZE,
                                  &cAssemblyModuleTokens);
        if (FAILED(hr)) {
            goto Exit;
        }
    
        for (i = 0; i < cAssemblyModuleTokens; i++) {
            mdf = rAssemblyModuleTokens[i];
        
            hr = _pImport->GetFileProps(
                mdf,            // [IN] The File for which to get the properties.
                szModuleName,   // [OUT] Buffer to fill with name.
                MAX_PATH,       // [IN] Size of buffer in wide chars.
                &ccModuleName,  // [OUT] Actual # of wide chars in name.
                &pvHashValue,   // [OUT] Pointer to the Hash Value Blob.
                &cbHashValue,   // [OUT] Count of bytes in the Hash Value Blob.
                &dwFlags);      // [OUT] Flags.
            if (FAILED(hr)) {
                goto Exit;
            }

            if (hr == CLDB_S_TRUNCATION) {
                // Cannot have a name greater than MAX_PATH
                hr = FUSION_E_ASM_MODULE_MISSING;
                goto Exit;
            }

            modBindInfo.dwAppDomainId = _bindInfo.dwAppDomainId;
            modBindInfo.lpAssemblyIdentity = _bindInfo.lpPostPolicyIdentity;
            modBindInfo.lpModuleName = szModuleName;

            hr = CreateHostAssemblyModuleImport(
                        &modBindInfo,
                        NULL,               // pStream
                        NULL,               // pDebugStream
                        0,                  // dwModuleId
                        &pImport);
    
            if (!_listModules.AddTail(pImport))
            {
                hr = E_OUTOFMEMORY;
                SAFERELEASE(pImport);
                goto Exit;
            }

            pImport = NULL;
        }
    }

    hr = S_OK;

Exit:
    return hr;
}
