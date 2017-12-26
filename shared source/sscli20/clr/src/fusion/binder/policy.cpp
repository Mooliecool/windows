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
//  Policy Services
//

#include "mscorcfg.h"
#include "fusionp.h"
#include "naming.h"
#include "policy.h"
#include "helpers.h"
#include "parse.h"
#include "dbglog.h"
#include "util.h"
#include "asm.h"
#include "adlmgr.h"
#include "xmlparser.h"
#include "nodefact.h"
#include "fstream.h"
#include "mstream.h"
#include "helpers.h"
#include "memoryreport.h"
#include "eventtrace.h"
#include "timeline.h"
#include "asmidmgr.h"
#include "hostpcymgr.h"

extern DWORD    g_dwConfigForceUnification;

HRESULT PrepQueryMatchData(IAssemblyName *pName, 
                           __out_ecount(*pdwSizeName) LPWSTR pwzAsmName,
                           __inout LPDWORD pdwSizeName,
                           __out_ecount(*pdwSizeVer) LPWSTR pwzAsmVersion, 
                           __inout LPDWORD pdwSizeVer,
                           __out_ecount(*pdwSizePKT) LPWSTR pwzPublicKeyToken, 
                           __inout LPDWORD pdwSizePKT,
                           __out_ecount_opt(pdwSizeCulture) LPWSTR pwzCulture,
                           __inout_opt LPDWORD pdwSizeCulture,
                           __out_opt PEKIND   *pe
                           )
{
    HRESULT                                     hr = S_OK;
    DWORD                                       dwSize;
    DWORD                                       dwVerHigh;
    DWORD                                       dwVerLow;
    CAssemblyName                              *pCName = NULL;

    _ASSERTE(pName);

    // Assembly Name
    if (pwzAsmName && pdwSizeName) { 
        hr = pName->GetName(pdwSizeName, pwzAsmName);
        if (FAILED(hr)) {
            goto Exit;
        }

        _ASSERTE(*pdwSizeName);
    }

    // Assembly Version
    if (pwzAsmVersion && pdwSizeVer) {
        hr = pName->GetVersion(&dwVerHigh, &dwVerLow);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCchPrintf(pwzAsmVersion, *pdwSizeVer, L"%hu.%hu.%hu.%hu",
                   HIWORD(dwVerHigh), LOWORD(dwVerHigh),
                   HIWORD(dwVerLow), LOWORD(dwVerLow));
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // Assembly Public Key Token
    if (pwzPublicKeyToken && pdwSizePKT) {
        pCName = static_cast<CAssemblyName*>(pName); // dynamic_cast
        _ASSERTE(pCName);
       
        dwSize = *pdwSizePKT * sizeof(WCHAR); 
        hr = pCName->GetPublicKeyToken(&dwSize, (LPBYTE)pwzPublicKeyToken, TRUE);
        if (FAILED(hr)) {
            goto Exit;
        }
        *pdwSizePKT = dwSize / sizeof(WCHAR);
    }

    // Assembly Language
    if (pwzCulture && pdwSizeCulture) {
        dwSize = *pdwSizeCulture * sizeof(WCHAR);
        hr = pName->GetProperty(ASM_NAME_CULTURE, pwzCulture, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
        *pdwSizeCulture = dwSize / sizeof(WCHAR);
    }

    // PE
    if (pe) {
        dwSize = sizeof(PEKIND);
        hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)pe, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
Exit:
    return hr;    
}

HRESULT GetPublisherPolicyFilePath(LPCWSTR pwzAsmName, 
                                   LPCWSTR pwzPublicKeyToken,
                                   LPCWSTR pwzCulture, 
                                   PEKIND peIn, 
                                   WORD wVerMajor,
                                   WORD wVerMinor, 
                                   __out_ecount_opt(*pdwSize) LPWSTR pwzPublisherCfg,
                                   __inout LPDWORD pdwSize,
                                   __out PEKIND *peOut)
{
    HRESULT                                   hr = S_OK;
    DWORD                                     dwLen = 0;
    WCHAR                                     wzPolicyAsmName[MAX_PATH];
    UINT                                      uiSize;
    BYTE                                      abProp[PUBLIC_KEY_TOKEN_LEN];
    IAssemblyName                            *pName = NULL;
    IAssemblyName                            *pNameGlobal = NULL;
    CTransCache                              *pTransCache = NULL;
    TRANSCACHEINFO                           *pInfo = NULL;
    IAssembly                                *pAsm = NULL;
    IAssemblyModuleImport                    *pModImport = NULL;
    LPWSTR pszManifestPath=NULL;
    
    _ASSERTE(pwzAsmName);
    _ASSERTE(pwzPublicKeyToken);
    _ASSERTE(pwzPublisherCfg);
    _ASSERTE(pdwSize);
    _ASSERTE(peOut);

    // Build name of policy assembly

    hr = StringCchPrintf(wzPolicyAsmName, MAX_PATH, L"%ws%d.%d.%ws", POLICY_ASSEMBLY_PREFIX,
               wVerMajor, wVerMinor, pwzAsmName);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Create policy name reference object

    hr = CreateAssemblyNameObject(&pName, wzPolicyAsmName, 0, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    uiSize = PUBLIC_KEY_TOKEN_LEN;
    CParseUtils::UnicodeHexToBin(pwzPublicKeyToken, uiSize * sizeof(WCHAR), abProp);

    hr = pName->SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, abProp, uiSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pwzCulture) {
        uiSize = (lstrlenW(pwzCulture) + 1) * sizeof(WCHAR);

        hr = pName->SetProperty(ASM_NAME_CULTURE, (void *)pwzCulture, uiSize);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = pName->SetProperty(ASM_NAME_CULTURE, L"", sizeof(L""));
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    if (peIn != peInvalid) {
        hr = pName->SetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&peIn, sizeof(PEKIND));
        if (FAILED(hr)) {
            goto Exit;
        }
    }
   
    hr = CCache::GetGlobalMax(pName, &pNameGlobal, &pTransCache);
    if (FAILED(hr) || hr == S_FALSE) {
        goto Exit;
    }

    SAFERELEASE(pName);

    pInfo = (TRANSCACHEINFO *)pTransCache->_pInfo;
    _ASSERTE(pInfo);

    pszManifestPath = pTransCache->_pInfo->pwzPath;

    hr = CreateAssemblyFromManifestFile(pszManifestPath, NULL, NULL, &pAsm);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (peIn == peInvalid) {
        hr = pAsm->GetAssemblyNameDef(&pName);
        if (FAILED(hr)) {
            goto Exit;
        }
        dwLen = sizeof(PEKIND);
        hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)peOut, &dwLen);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        *peOut = peIn;
    }
    
    hr = pAsm->GetNextAssemblyModule(0, &pModImport);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pModImport->GetModulePath(pwzPublisherCfg, pdwSize);
    if (FAILED(hr)) {
        goto Exit;
    }
Exit:
    SAFEDELETE(pTransCache);

    SAFERELEASE(pName);
    SAFERELEASE(pNameGlobal);
    SAFERELEASE(pAsm);
    SAFERELEASE(pModImport);

    return hr;
}

HRESULT ApplyPolicy(
        /* in */    IAssemblyName *pNameSource,     // name before policy
        /* in */    IApplicationContext *pIAppCtx,  // contains various setting
        /* in */    CDebugLog *pdbglog,             // fuslog
        /* out */   IAssemblyName **ppNamePolicy,   // name after policy
        /* out */   __deref_opt_out_opt LPWSTR *ppwzPolicyCodebase,     // policy codebase hint
        /* out */   AsmBindHistoryInfo **ppHistInfo,  // history logging
        /* out */   DWORD *pdwPolicyApplied)    // binding result info
{
    HRESULT                              hr = S_OK;
    DWORD                                dwSize = 0;
    CApplicationContext                 *pAppCtx = static_cast<CApplicationContext *>(pIAppCtx);

    BOOL                                 bDisallowApplyPublisherPolicy = FALSE;
    BOOL                                 bDisallowAppBindingRedirects = FALSE;
    BOOL                                 bUnifyFXAssemblies = TRUE;
    BOOL                                 bForceUnifyFX = g_dwConfigForceUnification;
    LPWSTR                               wzAppBase = NULL;

    CAppCtxPolicyConfigs                *pConfig = NULL;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionPolicy");

    _ASSERTE(pNameSource && ppNamePolicy && pAppCtx && pdwPolicyApplied);

    _ASSERTE(!*ppNamePolicy);
    
    *ppNamePolicy = NULL;
    *pdwPolicyApplied = ePolicyLevelNone;

    if (ppwzPolicyCodebase) {
        *ppwzPolicyCodebase = NULL;
    }

    ETWTraceStartup trace(ETW_TYPE_STARTUP_APPLY_POLICY);
    TIMELINE_AUTO(STARTUP, "ApplyPolicy");

    if (!CAssemblyName::IsStronglyNamed(pNameSource) 
        || CAssemblyName::IsPartial(pNameSource)) {

        DEBUGOUT(pdbglog, 0, ID_FUSLOG_POLICY_NOT_APPLIED);
        hr = CAssemblyName::CloneForBind(pNameSource, ppNamePolicy);
        return hr;
    }

    hr = pAppCtx->ReadConfigSettings(pdbglog, &pConfig);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = 0;
    hr = pAppCtx->Get(ACTAG_DISABLE_FX_ASM_UNIFICATION, NULL, &dwSize, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        bUnifyFXAssemblies = FALSE;
    }

    dwSize = 0;
    hr = pAppCtx->Get(ACTAG_DISALLOW_APP_BINDING_REDIRECTS, NULL, &dwSize, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        bDisallowAppBindingRedirects = TRUE;
    }
    
    dwSize = 0;
    hr = pAppCtx->Get(ACTAG_DISALLOW_APPLYPUBLISHERPOLICY, NULL, &dwSize, 0);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        bDisallowApplyPublisherPolicy = TRUE;
    }

    wzAppBase = NEW(WCHAR[MAX_URL_LENGTH+1]);
    if (!wzAppBase) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    dwSize = MAX_URL_LENGTH * sizeof(WCHAR);
    hr = pAppCtx->Get(ACTAG_APP_BASE_URL, wzAppBase, &dwSize, 0);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = S_OK;

    hr = pConfig->GetPolicyInfo(pNameSource, 
                                bDisallowAppBindingRedirects, 
                                bDisallowApplyPublisherPolicy, 
                                bUnifyFXAssemblies,
                                bForceUnifyFX,
                                wzAppBase,
                                pdbglog, 
                                ppNamePolicy, 
                                ppwzPolicyCodebase,
                                ppHistInfo, 
                                pdwPolicyApplied);

Exit:
    SAFERELEASE(pConfig);
    SAFEDELETEARRAY(wzAppBase);
    return hr;
}

HRESULT ParseXML(
    CNodeFactory **ppNodeFactory,
    LPCWSTR wzFileName,
    CDebugLog *pdbglog,
    BOOL bProcessLinkedConfigurations,
    CNodeFactory::ParseCtl parseCtl)
{
    HRESULT                                  hr = S_OK;
    CFileStream                             *pStream = NULL;
    CNodeFactory                            *pNF = NULL;
    IXMLParser                              *pXMLParser = NULL;

    ETWTraceStartup trace(ETW_TYPE_STARTUP_PARSEXML);
    TIMELINE_AUTO(STARTUP, "ParseXML");

    if (!ppNodeFactory || !wzFileName) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    pNF = NEW(CNodeFactory(pdbglog, parseCtl));
    if (!pNF) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = ParseXML(pNF, wzFileName, pdbglog);
    if (FAILED(hr)) {
        DEBUGOUT1(pdbglog, 0, ID_FUSLOG_XML_PARSE_ERROR_FILE, wzFileName);

        // Delete the old node factory (ie. all traces of anything that did
        // get parsed), and create a empty node factory.

        SAFERELEASE(pNF);

        pNF = NEW(CNodeFactory(pdbglog, parseCtl));
        if (!pNF) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }
        
        hr = S_FALSE;

        // Fall through, and hand back the empty node factory.
    }

    if (bProcessLinkedConfigurations) {
        hr = pNF->ProcessLinkedConfiguration(wzFileName);
        if (hr != S_OK) {
            // Error processing linked configurations

            hr = S_FALSE;
        }
    }

    pNF->ReleaseLogObject();
    *ppNodeFactory = pNF;
    (*ppNodeFactory)->AddRef();

Exit:
    SAFERELEASE(pStream);
    SAFERELEASE(pNF);
    SAFERELEASE(pXMLParser);

    return hr;
}

HRESULT ParseXML(
    CNodeFactory *pNodeFactory,
    LPCWSTR wzFileName,
    CDebugLog *pdbglog)
{
    HRESULT                                  hr = S_OK;
    CFileStream                             *pStream = NULL;
    IXMLParser                              *pXMLParser = NULL;

    _ASSERTE(pNodeFactory && wzFileName);
    
    hr = GetXMLObject(&pXMLParser);
    if (FAILED(hr)) {
        goto Exit;
    }

    pStream = NEW(CFileStream);
    if (!pStream) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pStream->OpenForRead(wzFileName);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pXMLParser->SetFactory(pNodeFactory);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pXMLParser->SetInput(pStream);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pXMLParser->Run(-1);
    if (FAILED(hr)) {
        DEBUGOUT1(pdbglog, 0, ID_FUSLOG_XML_PARSE_ERROR_FILE, wzFileName);
        goto Exit;
    }

Exit:
    SAFERELEASE(pStream);
    SAFERELEASE(pXMLParser);

    return hr;
}

HRESULT ParseXML(CNodeFactory **ppNodeFactory, LPVOID lpMemory, ULONG cbSize, CDebugLog *pdbglog)
{
    HRESULT                                  hr = S_OK;
    CMemoryStream                           *pStream = NULL;
    CNodeFactory                            *pNF = NULL;
    IXMLParser                              *pXMLParser = NULL;

    if (!ppNodeFactory || !lpMemory) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    *ppNodeFactory = NULL;

    hr = GetXMLObject(&pXMLParser);
    if (FAILED(hr)) {
        goto Exit;
    }

    pStream = NEW(CMemoryStream);
    if (!pStream) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    // CMemoryStream::Init(LPVOID lpStart, ULONG cbSize, BOOL bReadOnly)
    hr = pStream->Init(lpMemory, cbSize, TRUE);
    if (FAILED(hr)) {
        goto Exit;
    }

    pNF = NEW(CNodeFactory(pdbglog));
    if (!pNF) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pXMLParser->SetFactory(pNF);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pXMLParser->SetInput(pStream);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pXMLParser->Run(-1);
    if (FAILED(hr)) {
        DEBUGOUT(pdbglog, 0, ID_FUSLOG_XML_PARSE_ERROR_MEMORY);
        goto Exit;
    }

    *ppNodeFactory = pNF;
    (*ppNodeFactory)->AddRef();

Exit:
    SAFERELEASE(pStream);
    SAFERELEASE(pNF);
    SAFERELEASE(pXMLParser);

    return hr;
}

HRESULT IsMatchingVersion(LPCWSTR wzVerCfg, LPCWSTR wzVerSource)
{
    HRESULT                         hr = S_OK;
    BOOL                            bMatch = FALSE;
    BOOL                            bAnchor = FALSE;
    LPWSTR                          wzVer = NULL;
    LPWSTR                          wzPos = NULL;
    ULONGLONG                       ullVer = 0;
    ULONGLONG                       ullVerLow = 0;
    ULONGLONG                       ullVerHigh = 0;

    _ASSERTE(wzVerCfg && wzVerSource);

    if (!FusionCompareString(wzVerCfg, wzVerSource)) {
        // Exact match
        bMatch = TRUE;
        goto Exit;
    }

    // See if wzVerCfg contains a range

    wzVer = WSTRDupDynamic(wzVerCfg);
    if (!wzVer) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    wzPos = wzVer;

    while (*wzPos) {
        if (*wzPos == L'-') {
            *wzPos++ = L'\0';
            bAnchor = TRUE;
            break;
        }

        wzPos++;
    }

    if (!bAnchor) {
        goto Exit;
    }

    TrimString(wzVer);

    hr = GetVersionFromString(wzVer, &ullVerLow);
    if (FAILED(hr)) {
        goto Exit;
    }

    TrimString(wzPos);

    if (!*wzPos) {
        goto Exit;
    }

    hr = GetVersionFromString(wzPos, &ullVerHigh);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = GetVersionFromString(wzVerSource, &ullVer);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (ullVer >= ullVerLow && ullVer <= ullVerHigh) {
        bMatch = TRUE;
    }

Exit:
    SAFEDELETEARRAY(wzVer);
    if (SUCCEEDED(hr)) {
        if (bMatch) {
            hr = S_OK;
        }
        else {
            hr = S_FALSE;
        }
    }
    
    return hr;
}

HRESULT GetVersionFromString(LPCWSTR wzVersionIn, ULONGLONG *pullVer)
{
    HRESULT     hr = S_OK;
    WORD        wVerMajor = 0;
    WORD        wVerMinor = 0;
    WORD        wVerRev = 0;
    WORD        wVerBld = 0;
    DWORD       dwVerHigh = 0;
    DWORD       dwVerLow = 0;

    _ASSERTE(wzVersionIn && pullVer);

    hr = VersionFromString(wzVersionIn, &wVerMajor, &wVerMinor, &wVerBld, &wVerRev);
    if(FAILED(hr)) {
        goto Exit;
    }

    dwVerHigh = (((DWORD)wVerMajor << 16) & 0xFFFF0000);
    dwVerHigh |= ((DWORD)(wVerMinor) & 0x0000FFFF);

    dwVerLow = (((DWORD)wVerBld << 16) & 0xFFFF0000);
    dwVerLow |= ((DWORD)(wVerRev) & 0x0000FFFF);

    *pullVer = (((ULONGLONG)dwVerHigh << 32) & UI64(0xFFFFFFFF00000000)) | (dwVerLow & 0xFFFFFFFF);

Exit:
    return hr;
}

extern "C" HRESULT STDMETHODCALLTYPE InitializeFusion();

STDAPI GetCLRIdentityManager(REFIID riid, IUnknown **ppManager)
{
    HRESULT hr = S_OK;

    BEGIN_ENTRYPOINT_NOTHROW;


    if (!ppManager) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (IsEqualIID(riid, IID_ICLRAssemblyIdentityManager)) {
        *ppManager = CAssemblyIdentityManager::GetSingletonIdMgr();
        (*ppManager)->AddRef();
    }
    else if (IsEqualIID(riid, IID_ICLRHostBindingPolicyManager)) {
        *ppManager = CHostBindingPolicyManager::GetSingletonPolicyMgr();
        (*ppManager)->AddRef();
    }
    else {
        hr = E_NOINTERFACE;
        goto Exit;
    }

Exit:
    END_ENTRYPOINT_NOTHROW;

    return hr;
}
