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

#include "bindhelpers.h"
#include "asm.h"
#include "simplesink.h"
#include "cacheutils.h"
#include "asmimprt.h"
#include "cache.h"
#include "memoryreport.h"
#include "eventtrace.h"
#include "timeline.h"
#include "dbglog.h"
#include "naming.h"
#include "hostasm.h"
#include "ex.h"


IAssemblyName *g_pSystemAssemblyName;
IAssembly *g_pSystemAssembly;
BOOL g_bSystemAssemblyVerified;


HRESULT BindHelper(IAssemblyName *pName, 
                   IApplicationContext *pAppCtx, 
                   LONGLONG llFlags,
                   IUnknown **ppUnk,
                   IFusionBindLog **ppdbglog)
{
    HRESULT hr = S_OK;
    CSimpleFusionBindSink *pSink = NULL;
    IAssemblyNameBinder *pBinder = NULL;

    hr = CSimpleFusionBindSink::Create((void **)ppUnk, &pSink);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pSink->Reset();
    if (FAILED(hr)) {
        goto Exit;
    }

    // remove parent asm hint
    llFlags &= ~ASM_BINDF_PARENT_ASM_HINT;

    hr = pName->QueryInterface(IID_IAssemblyNameBinder, (void **)&pBinder);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pBinder->BindToObject(IID_IAssembly, 
                             pSink,
                             pAppCtx,
                             NULL,
                             llFlags,
                             NULL,
                             0,
                             (void **)ppUnk,
                             NULL);
    if (hr == E_PENDING) {
        pSink->Wait();
        hr = pSink->LastResult();
    }

    if (SUCCEEDED(hr) && *ppUnk) {
        hr = S_OK;
    }

    if (ppdbglog) {
        pSink->GetBindLog(ppdbglog);
    }

Exit:
    SAFERELEASE(pBinder);
    SAFERELEASE(pSink);
    return hr;
}

// DEVOVERRIDE
extern WCHAR g_wzLocalDevOverridePath[MAX_PATH + 1];
extern WCHAR g_wzGlobalDevOverridePath[MAX_PATH + 1];
extern DWORD g_dwDevOverrideFlags;
extern DWORD g_dwDevOverrideEnable;

HRESULT BindToSystem(IAssemblyName *pNameSystem, 
                     LPCWSTR pwzSystemDirectory,
                     IUnknown *pUnk, 
                     IAssembly **ppAsmOut,
                     IAssembly **ppNIAsmOut,
                     IFusionBindLog **ppdbglog)
{
    HRESULT hr = S_OK;
    IAssembly *pAsm = NULL;
    CAssembly *pCAsm = NULL;
    CNativeImageAssembly *pAsmNI = NULL;        
    DWORD dwSize = 0;
    IAssemblyManifestImport *pAsmImport = NULL;
    INativeImageEvaluate *pNIEva = NULL;
    WCHAR wzManifestFilePath[MAX_PATH];
    IAssemblyName *pName = NULL;
    static BOOL bCalled = FALSE;
    CDebugLog *pdbglog = NULL;
    BOOL    bFoundInDevOverride = FALSE;
    
    MEMORY_REPORT_CONTEXT_SCOPE("FusionBindToSystem");

    if (!pNameSystem || !ppAsmOut || CAssemblyName::IsPartial(pNameSystem) || !CAssemblyName::IsStronglyNamed(pNameSystem) || !pwzSystemDirectory){
        return E_INVALIDARG;
    }

    *ppAsmOut = NULL;

    // should only be called once.
    if (bCalled) {
        *ppAsmOut = g_pSystemAssembly;
        if (g_pSystemAssembly) {
            g_pSystemAssembly->AddRef();
            return S_OK;
        }
        else {
            return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
    }

    if (IsLoggingNeeded()) {
        CDebugLog::Create(NULL, pNameSystem, NULL, &pdbglog);
    }

    wzManifestFilePath[0]= L'\0';

    if (pUnk) {
        hr = pUnk->QueryInterface(IID_INativeImageEvaluate, (void **)&pNIEva);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // temporary IAssemblyName for mscorlib
    g_pSystemAssemblyName = pNameSystem;
    g_pSystemAssemblyName->AddRef();


    // we are here because we cannot find the custom assembly,
    // or we are not asked for a custom assembly. 

    if (!bFoundInDevOverride) {
        hr = CAssemblyName::CloneForBind(pNameSystem, &pName);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CreateAssemblyFromCacheLookup(NULL, pName, ppAsmOut, NULL);
        if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            goto Exit;
        }
        
        if (hr == S_OK) {
            DEBUGOUT(pdbglog, 1, ID_FUSLOG_CACHE_LOOKUP_SUCCESS);
        }
        else if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)){
            DEBUGOUT(pdbglog, 1, ID_FUSLOG_ASSEMBLY_LOOKUP_FAILURE);
        }
        
        if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            dwSize = MAX_PATH;
            hr = StringCchPrintf(wzManifestFilePath, MAX_PATH, L"%ws%ws.dll", pwzSystemDirectory, SYSTEM_ASSEMBLY_NAME);
            if (FAILED(hr)) {
                goto Exit;
            }

            DEBUGOUT1(pdbglog, 0, ID_FUSLOG_ATTEMPT_NEW_DOWNLOAD, wzManifestFilePath);

            hr = CreateAssemblyFromManifestFile(wzManifestFilePath, NULL, NULL, ppAsmOut);
            if (FAILED(hr)) {
                DEBUGOUT1(pdbglog, 1, ID_FUSLOG_MANIFEST_EXTRACT_FAILURE, hr);
                goto Exit;
            }
        }
    }


Exit:

    if (pdbglog) {
        pdbglog->SetResultCode(FUSION_BIND_LOG_CATEGORY_DEFAULT, hr);
        DUMPDEBUGLOG(pdbglog, g_dwLogLevel);
        DUMPDEBUGLOGNGEN(pdbglog, g_dwLogLevel);
        if (ppdbglog) {
            *ppdbglog = pdbglog;
            pdbglog->AddRef();
        }
    }

    SAFERELEASE(g_pSystemAssemblyName);

    if (SUCCEEDED(hr)) {
        pCAsm = static_cast<CAssembly *>(*ppAsmOut); // dynamic_cast
        pCAsm->SetIsSystemAssembly(TRUE);

        g_pSystemAssembly = *ppAsmOut;
        g_pSystemAssembly->AddRef();
        g_pSystemAssembly->GetAssemblyNameDef(&g_pSystemAssemblyName);
    }

    if (FAILED(hr)) {
        SAFERELEASE(*ppAsmOut);
    }

    SAFERELEASE(pdbglog);
    SAFERELEASE(pAsmImport);
    SAFERELEASE(pAsm);
    SAFERELEASE(pAsmNI);
    SAFERELEASE(pNIEva);
    SAFERELEASE(pName);

    bCalled = TRUE;
    return hr;
}

HRESULT ExplicitBind(
        LPCWSTR wzFilePath, IApplicationContext *pAppCtx, 
        DWORD dwFlags, IUnknown *pUnkNIEva, 
        IAssembly **ppAsmOut,
        IAssembly **ppNIAsmOut,
        IFusionBindLog **ppdbglog)
{
    HRESULT hr = S_OK;
    WCHAR wzManifestFilePath[MAX_PATH];
    LPWSTR pwzCodebase = NULL;
    DWORD dwSize = MAX_PATH;
    FILETIME ftLastMod;
    IAssemblyManifestImport *pAsmImport = NULL;
    IAssembly *pAsm = NULL;
    IAssemblyName *pName = NULL;
    IAssemblyName *pNameCopy = NULL;
    LPWSTR pwzConfigString = NULL;
    CLoadContext *pLoadContext = NULL;
    IUnknown *pAsmActivated = NULL;
    INativeImageEvaluate *pNIEva = NULL;
    CDebugLog *pdbglog = NULL;
    CAssembly *pCAsm = NULL;
    IUnknown *pUnk = NULL;
    CApplicationContext *pCAppCtx = static_cast<CApplicationContext *>(pAppCtx);
    CNativeImageAssembly *pAsmNI = NULL;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_EXPLICITBIND);
    TIMELINE_AUTO(STARTUP, "ExplicitBind");

    if (!wzFilePath || !pAppCtx || !ppAsmOut) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (IsLoggingNeeded()) {
        CDebugLog::Create(pAppCtx, NULL, wzFilePath, &pdbglog);
        DEBUGOUT1(pdbglog, 0, (dwFlags == EXPLICITBIND_FLAGS_NON_BINDABLE)?ID_FUSLOG_EXPLICIT_BIND_IJW:ID_FUSLOG_EXPLICIT_BIND_EXE, wzFilePath);
    }

    if (pUnkNIEva) {
        hr = pUnkNIEva->QueryInterface(IID_INativeImageEvaluate, (void **)pNIEva);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
    hr = CreateAssemblyManifestImport(wzFilePath, &pAsmImport);
    if (FAILED(hr)) {
        goto Exit;
    }
   
    hr = pAsmImport->GetAssemblyNameDef(&pName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (dwFlags == EXPLICITBIND_FLAGS_NON_BINDABLE) {
        hr = pName->Clone(&pNameCopy);
        if (FAILED(hr)) {
            goto Exit;
        }

        pNameCopy->SetProperty(ASM_NAME_ARCHITECTURE, NULL, 0);
    }
    else {
        pNameCopy = pName;
        pNameCopy->AddRef();
    }

    if (dwFlags == EXPLICITBIND_FLAGS_NON_BINDABLE) {
        // bind to the assembly
        //
        // Intentionally ignore error here. 
        // If the failure is not transient, we will cache the failure
        // so the assembly won't never be loaded in the app context.
        // If the failure is transient, likely we will fail below.
        // Either way, we should be good to use the input assembly.
        // 
        BindHelper(pNameCopy, pAppCtx, ASM_BINDF_DO_NOT_PROBE_NATIVE_IMAGE, &pUnk, NULL);

        if (pUnk) {
            pUnk->QueryInterface(IID_IAssembly, (void **)&pAsm);
            SAFERELEASE(pUnk);
        }
            
        hr = S_OK;
        if (pAsm) {
            // we found the assembly through binding.
            dwSize = MAX_PATH;
            hr = pAsm->GetManifestModulePath(wzManifestFilePath, &dwSize);
            if (FAILED(hr)) {
                goto Exit;
            }

            if (FusionCompareStringI(wzFilePath, wzManifestFilePath)) {
                // binding results in different file path. 
                // Cannot use it.
                DEBUGOUT1(pdbglog, 0, ID_FUSLOG_EXPLICIT_BIND_IJW_PATH_DIFF, wzManifestFilePath);
                SAFERELEASE(pAsm);
            }
            else {
                DEBUGOUT(pdbglog, 0, ID_FUSLOG_EXPLICIT_BIND_IJW_PATH_MATCH);
            }
        }
        else {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_EXPLICIT_BIND_IJW_NOT_FOUND);
        }
    }
    else {
        dwSize = 0;
        hr = pAppCtx->Get(ACTAG_APP_CFG_DOWNLOAD_ATTEMPTED, NULL, &dwSize, 0);
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            hr = DownloadAppCfg(pAppCtx, NULL, NULL, NULL, FALSE);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
        hr = S_OK;
    }

    if (!pAsm) {
        hr = GetFileLastModified(wzFilePath, &ftLastMod);
        if (FAILED(hr)) {
            goto Exit;
        }

        pwzCodebase = NEW(WCHAR[MAX_URL_LENGTH]);
        if (!pwzCodebase) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        dwSize = MAX_URL_LENGTH;
        hr = UrlCanonicalizeUnescape(wzFilePath, pwzCodebase, &dwSize, 0);
        if (FAILED(hr)) {
            goto Exit;
        }
            
        hr = CreateAssemblyFromManifestImport(pAsmImport, pwzCodebase, &ftLastMod, &pAsm);
        if (FAILED(hr)) {
            goto Exit;
        }
        pCAsm = static_cast<CAssembly *>(pAsm);
        pCAsm->SetAssemblyLocation(ASMLOC_RUN_FROM_SOURCE);
        pCAsm->SetApplicationContextID(pAppCtx);
    }

    _ASSERTE(pAsm);

    if (dwFlags == EXPLICITBIND_FLAGS_EXE) {
        hr = pCAppCtx->GetLoadContext(&pLoadContext, LOADCTX_TYPE_DEFAULT);
        if (FAILED(hr)) {
            goto Exit;
        }

        _ASSERTE(pLoadContext);

        hr = pLoadContext->AddActivation(pAsm, &pAsmActivated);
        if (FAILED(hr)) {
            goto Exit;
        }
        else if (hr == S_FALSE) {
            if (pdbglog && IsLoggingNeeded()) {
                IAssembly *pAsmTmp = NULL;
                if (SUCCEEDED(pAsmActivated->QueryInterface(IID_IAssembly, (void **)&pAsmTmp))) {
                    dwSize = MAX_PATH;
                    wzManifestFilePath[0] = L'\0';
                    pAsmTmp->GetManifestModulePath(wzManifestFilePath, &dwSize);
                    SAFERELEASE(pAsmTmp);
                }

                DEBUGOUT1(pdbglog, 0, ID_FUSLOG_EXPLICIT_BIND_EXE_LOADCONTEXT_COLLISION, wzManifestFilePath[0]?wzManifestFilePath:L"HostAssembly");
            }
            _ASSERTE(!"Some assembly with the same name as the executable is loaded!");
            hr = E_UNEXPECTED;
            goto Exit;
        }
    }


    *ppAsmOut = pAsm;
    (*ppAsmOut)->AddRef();

    if (ppNIAsmOut) {
        *ppNIAsmOut = pAsmNI;
        if (*ppNIAsmOut) {
            (*ppNIAsmOut)->AddRef();
        }
    }

Exit:
    if ( pdbglog) {
        pdbglog->SetResultCode(FUSION_BIND_LOG_CATEGORY_DEFAULT, hr);
        DUMPDEBUGLOG(pdbglog, g_dwLogLevel);
        DUMPDEBUGLOGNGEN(pdbglog, g_dwLogLevel);
        if (ppdbglog) {
            *ppdbglog = pdbglog;
            pdbglog->AddRef();
        }
    }

    SAFERELEASE(pdbglog);
    SAFERELEASE(pAsm);
    SAFERELEASE(pAsmNI);
    SAFERELEASE(pName);
    SAFERELEASE(pNameCopy);
    SAFERELEASE(pAsmImport);
    SAFEDELETEARRAY(pwzConfigString);
    SAFEDELETEARRAY(pwzCodebase);
    SAFERELEASE(pAsmActivated);
    SAFERELEASE(pLoadContext);
    SAFERELEASE(pNIEva);
    SAFERELEASE(pUnk);
    return hr;
}

// compare two assemblies for equality. 
// For IAssembly, two assemblies are equal only if they have the same path. 
// For IHostAssembly, two assemblies are equal only if they have the same module id. 
HRESULT CompareAssemblies(IUnknown *pUnk1, IUnknown *pUnk2, BOOL *pbIsEqual)
{
    IAssembly *pAsm1 = NULL;
    IHostAssembly *pHostAsm1 = NULL;
    IAssembly *pAsm2 = NULL;
    IHostAssembly *pHostAsm2 = NULL;
    WCHAR   wzFilePath1[MAX_PATH];
    WCHAR   wzFilePath2[MAX_PATH];
    UINT64  AssemblyId1;
    UINT64  AssemblyId2;
    DWORD   dwSize;
    HRESULT hr = S_OK;

    hr = pUnk1->QueryInterface(IID_IAssembly, (void **)&pAsm1);
    if (hr == E_NOINTERFACE) {
        hr = pUnk1->QueryInterface(IID_IHostAssembly, (void **)&pHostAsm1);
    }

    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pUnk2->QueryInterface(IID_IAssembly, (void **)&pAsm2);
    if (hr == E_NOINTERFACE) {
        hr = pUnk2->QueryInterface(IID_IHostAssembly, (void **)&pHostAsm2);
    }

    if (FAILED(hr)) {
        goto Exit;
    }

    if ((pAsm1 && !pAsm2) ||
        (!pAsm1 && pAsm2)) {
        *pbIsEqual = FALSE;
        goto Exit;
    }

    if (pAsm1) {
        dwSize = ARRAYSIZE(wzFilePath1);
        hr = pAsm1->GetManifestModulePath(wzFilePath1, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }

        dwSize = ARRAYSIZE(wzFilePath2);
        hr = pAsm2->GetManifestModulePath(wzFilePath2, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
       
        *pbIsEqual = (FusionCompareStringI(wzFilePath1, wzFilePath2) == 0);
    }
    else {
        _ASSERTE(pHostAsm1);
        pHostAsm1->GetAssemblyId(&AssemblyId1);
        if (FAILED(hr)) {
            goto Exit;
        }

        pHostAsm2->GetAssemblyId(&AssemblyId2);
        if (FAILED(hr)) {
            goto Exit;
        }

        *pbIsEqual = (AssemblyId1 == AssemblyId2);
    }

Exit:
    SAFERELEASE(pAsm1);
    SAFERELEASE(pHostAsm1);
    SAFERELEASE(pAsm2);
    SAFERELEASE(pHostAsm2);

    return hr;
}

HRESULT HostStoreAssemblyLookup(IAssemblyName *pNameRefSource,
                                IAssemblyName *pNameRefPolicy, 
                                DWORD dwPolicyApplied,
                                IApplicationContext *pAppCtx,
                                CDebugLog *pdbglog,
                                IHostAssembly **ppHostAsm)
{
    HRESULT     hr  = S_OK;
    AssemblyBindInfo bindInfo;
    DWORD       dwSize = 0;
    UINT64      id;
    UINT64      context;
    DWORD       dwAppDomainId = 0;
    PEKIND      rgPEKind[] = {g_peKindProcess, peMSIL, peNone};
    PEKIND      pe;
    BOOL        bHasPE = FALSE;

    DWORD       dwDisplayFlags = ASM_DISPLAYF_FULL;

    DWORD       cbValue;
    IHostAssembly *pHostAsm = NULL;
    IAssemblyName *pHostNameDef = NULL;
    IAssembly   *pAsm = NULL;
    IAssemblyName *pNameDef = NULL;
    IStream    *pAsmStream = NULL;
    IStream    *pDebugStream = NULL;
    IAssemblyName *pNameClone = NULL;

    WORD        wVer = 0;
    DWORD       i;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionAssemblyDownloadMgr");
    
    _ASSERTE(pNameRefPolicy && ppHostAsm);

    *ppHostAsm = NULL;
    memset(&bindInfo, 0, sizeof(AssemblyBindInfo));

    if (! g_pHostAssemblyStore ) {
        DEBUGOUT(pdbglog, 0, ID_FUSLOG_HOST_STORE_NOT_AVAILABLE);
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        goto Exit;
    }
    
    if (CAssemblyName::IsPartial(pNameRefSource, NULL)) {
        // never ask host for partial name binding. 
        DEBUGOUT(pdbglog, 0, ID_FUSLOG_HOST_STORE_PARTIAL_NAME_SKIP);
        hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        goto Exit;
    }

    dwSize = sizeof(PEKIND);
    hr = pNameRefPolicy->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&pe, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (dwSize != 0) {
        _ASSERTE(pe != peInvalid);
        bHasPE = TRUE;
        rgPEKind[0] = pe;
    }

    memset((void *)&bindInfo, 0, sizeof(AssemblyBindInfo));

    cbValue = sizeof(dwAppDomainId);
    hr = pAppCtx->Get(ACTAG_APP_DOMAIN_ID, &dwAppDomainId, &cbValue, 0);
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
        goto Exit;
    }
    hr = S_OK;

    bindInfo.dwAppDomainId = dwAppDomainId;
    bindInfo.ePolicyLevel = dwPolicyApplied;

    // we need to return 0.0.0.0 for simply named assembly
    if (!CAssemblyName::IsStronglyNamed(pNameRefSource)) {
        hr = pNameRefSource->Clone(&pNameClone);
        if (FAILED(hr)) {
            goto Exit;
        }
        for (i = 0; i < 4; i++) {
            hr = pNameClone->SetProperty(ASM_NAME_MAJOR_VERSION+i, (LPBYTE)&wVer, sizeof(WORD)); 
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }
    else {
        pNameClone = pNameRefSource;
        pNameClone->AddRef();
    }
            
    dwSize = 0;
    hr = pNameClone->GetDisplayName(NULL, &dwSize, dwDisplayFlags);
    if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        goto Exit;
    }
    
    bindInfo.lpReferencedIdentity = NEW(WCHAR[dwSize]);
    if (!bindInfo.lpReferencedIdentity) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pNameClone->GetDisplayName((LPOLESTR)bindInfo.lpReferencedIdentity, &dwSize, dwDisplayFlags);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CanonicalizeIdentity((LPWSTR)bindInfo.lpReferencedIdentity);
    if (FAILED(hr)) {
        goto Exit;
    }

    SAFERELEASE(pNameClone);
    if (!CAssemblyName::IsStronglyNamed(pNameRefPolicy)) {
        hr = pNameRefPolicy->Clone(&pNameClone);
        if (FAILED(hr)) {
            goto Exit;
        }
        for (i = 0; i < 4; i++) {
            hr = pNameClone->SetProperty(ASM_NAME_MAJOR_VERSION+i, (LPBYTE)&wVer, sizeof(WORD));
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }
    else {
        pNameClone = pNameRefPolicy;
        pNameClone->AddRef();
    }

    for (i = 0; bHasPE ? (i<1):(i<(int)ARRAYSIZE(rgPEKind)); i++) {
        SAFEDELETEARRAY(bindInfo.lpPostPolicyIdentity);

        hr = pNameClone->SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&(rgPEKind[i]), sizeof(PEKIND));
        if (FAILED(hr)) {
            goto Exit;
        }

        dwSize = 0;
        hr = pNameClone->GetDisplayName(NULL, &dwSize, dwDisplayFlags);
        if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            goto Exit;
        }
        
        bindInfo.lpPostPolicyIdentity = NEW(WCHAR[dwSize]);
        if (!bindInfo.lpPostPolicyIdentity) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = pNameClone->GetDisplayName((LPOLESTR)bindInfo.lpPostPolicyIdentity, &dwSize, dwDisplayFlags);
        if (FAILED(hr)) {
            goto Exit;
        }


        hr = CanonicalizeIdentity((LPWSTR)bindInfo.lpPostPolicyIdentity);
        if (FAILED(hr)) {
            goto Exit;
        }

        DEBUGOUT1(pdbglog, 0, ID_FUSLOG_HOST_STORE_RESOLVE_TRY, bindInfo.lpPostPolicyIdentity);

        {
            REMOVE_STACK_GUARD;
            hr = g_pHostAssemblyStore->ProvideAssembly(
                                        &bindInfo,
                                        &id,
                                        &context,
                                        &pAsmStream,
                                        &pDebugStream);
        }

        if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            continue;
        }
        else if (FAILED(hr)) {
            goto Exit;
        }

        if (!pAsmStream) {
            // host gives us invalid information, treat it as hard failure
            // no good hr. Suzanne suggests COR_E_FILELOAD
            hr = COR_E_FILELOAD;
            goto Exit;
        }

        DEBUGOUT2(pdbglog, 0, ID_FUSLOG_HOST_RESULT, id, context);
        DEBUGOUT(pdbglog, 0, ID_FUSLOG_HOST_NO_DEBUG_STREAM);

        hr = CreateHostAssemblyFromStream(
                        pAsmStream,
                        pDebugStream,
                        id,
                        context,
                        &bindInfo,
                        &pHostAsm);
        if (FAILED(hr)){
            goto Exit;
        }

        hr = pHostAsm->GetAssemblyNameDef(&pHostNameDef);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (CAssemblyName::IsStronglyNamed(pHostNameDef)) {
            hr = CreateAssemblyFromCacheLookup(pAppCtx, pHostNameDef, &pAsm, pdbglog);
            if (FAILED(hr) && (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))) {
                goto Exit;
            }
                
            hr = S_OK;

            if (pAsm) {
                hr = pAsm->GetAssemblyNameDef(&pNameDef);
                if (FAILED(hr)) {
                    goto Exit;
                }

                hr = pNameDef->IsEqual(pHostNameDef, ASM_CMPF_MVID);
                if (FAILED(hr)){
                    goto Exit;
                }

                if (hr != S_OK) {
                    DEBUGOUT(pdbglog, 0, ID_FUSLOG_HOST_GAC_ASM_MISMATCH);
                    hr = FUSION_E_HOST_GAC_ASM_MISMATCH;
                    goto Exit;
                }
            }
        }
        
        if (!bHasPE) {
            hr = pNameRefPolicy->SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&(rgPEKind[i]), sizeof(PEKIND));
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        *ppHostAsm = pHostAsm;
        (*ppHostAsm)->AddRef();
        goto Exit;
    }

    // Not found.

    DEBUGOUT(pdbglog, 0, ID_FUSLOG_HOST_STORE_RESOLVE_FAILED);
    hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        
Exit:
    SAFEDELETEARRAY(bindInfo.lpReferencedIdentity);
    SAFEDELETEARRAY(bindInfo.lpPostPolicyIdentity);
    SAFERELEASE(pAsmStream);
    SAFERELEASE(pDebugStream);
    SAFERELEASE(pAsm);
    SAFERELEASE(pHostAsm);
    SAFERELEASE(pHostNameDef);
    SAFERELEASE(pNameDef);
    SAFERELEASE(pNameClone);
    
    if (*ppHostAsm) {
        CHostAssembly *pCHostAsm = static_cast<CHostAssembly *>(*ppHostAsm);
        pCHostAsm->SetApplicationContextID(pAppCtx);
    }

    // revert accidental changes to pNameRefPolicy
    if (!bHasPE && FAILED(hr)) {
        pNameRefPolicy->SetProperty(ASM_NAME_ARCHITECTURE, NULL, 0);
    }

    return hr;
}

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        CBindingInput *pBindInput,
                        CBindingOutput *pBindOutput)
{
    return pBindCache->AddBindingResult(pBindInput, pBindOutput);
}

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        IAssemblyName *pName, 
                        LPCWSTR pwzCodebase, 
                        LOADCTX_TYPE ctxType, 
                        LPCWSTR pwzProbingBase,
                        CBindingOutput *pBindOutput)
{
    HRESULT hr = S_OK;
    CBindingInput *pBindInput = NULL;

    hr = CBindingInput::Create(pName, pwzCodebase, ctxType, pwzProbingBase, &pBindInput);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CacheBindResult(pBindCache, pBindInput, pBindOutput);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    SAFERELEASE(pBindInput);
    return hr;
}

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        CBindingInput *pBindInput,
                        IAssemblyName *pName, 
                        LOADCTX_TYPE ctxType, 
                        HRESULT hrResult)
{
    HRESULT hr = S_OK;
    CBindingOutput *pBindOutput = NULL;

    if (FAILED(hrResult) && (!Exception::IsTransient(hrResult))) {
        hr = CBindingOutput::Create(pName, ctxType, hrResult, &pBindOutput);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CacheBindResult(pBindCache, pBindInput, pBindOutput);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    SAFERELEASE(pBindOutput);
    return hr;
}

HRESULT CacheBindResult(CBindingResultCache *pBindCache,
                        IAssemblyName *pNameRef, 
                        LPCWSTR pwzCodebase, 
                        LOADCTX_TYPE ctxTypeIn, 
                        LPCWSTR pwzProbingBase,
                        IAssemblyName *pNameDef, 
                        LOADCTX_TYPE ctxTypeOut, 
                        HRESULT hrResult)
{
    HRESULT hr = S_OK;
    CBindingOutput *pBindOutput = NULL;

    if (FAILED(hrResult) && (!Exception::IsTransient(hrResult))) {
        hr = CBindingOutput::Create(pNameDef, ctxTypeOut, hrResult, &pBindOutput);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CacheBindResult(pBindCache, 
                            pNameRef,
                            pwzCodebase,
                            ctxTypeIn,
                            pwzProbingBase,
                            pBindOutput);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

Exit:
    SAFERELEASE(pBindOutput);
    return hr;
}

STDAPI CreateHistoryReader(LPCWSTR wzFilePath, IHistoryReader **ppHistoryReader)
{
    return E_NOTIMPL;
}


STDAPI GetHistoryFileDirectory(__out_ecount_opt(*pdwSize)LPWSTR wzDir, __inout DWORD *pdwSize)
{
    return E_NOTIMPL;
}

STDAPI LookupHistoryAssembly(LPCWSTR pwzFilePath, FILETIME *pftActivationDate,
                             LPCWSTR pwzAsmName, LPCWSTR pwzPublicKeyToken,
                             LPCWSTR pwzCulture, LPCWSTR pwzVerRef,
                             IHistoryAssembly **ppHistAsm)
{
    return E_NOTIMPL;
}
