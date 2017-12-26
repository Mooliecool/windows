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
#include "fusconfig.h"
#include "helpers.h"
#include "nodefact.h"
#include "util.h"
#include "mscoree.h"
#include "policy.h"
#include "dbglog.h"
#include "parse.h"
#include "cacheutils.h"
#include "cfgdata.h"
#include "asmimprt.h"

CPolicyConfigPool *g_pConfigPool;

// class CPolicyConfigPool
CPolicyConfigPool::~CPolicyConfigPool()
{
    LISTNODE pos = NULL;
    CPolicyConfig *pConfig = NULL;
    
    for (DWORD i = 0; i < CONFIG_HASH_TABLE_SIZE; i++) {
        pos = _listConfigs[i].GetHeadPosition();
        while(pos) {
            pConfig = _listConfigs[i].GetNext(pos);
            _ASSERTE(pConfig);
            SAFERELEASE(pConfig);
        }
    }

    if (_cs) {
        ClrDeleteCriticalSection(_cs);
    }
}

/* static */
HRESULT CPolicyConfigPool::Create(CPolicyConfigPool **ppConfigPool)
{
    HRESULT hr = S_OK;
    CPolicyConfigPool *pConfigPool = NULL;

    _ASSERTE(ppConfigPool);

    *ppConfigPool = NULL;

    pConfigPool = NEW(CPolicyConfigPool);
    if (!pConfigPool) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pConfigPool->Init();
    if (SUCCEEDED(hr)) {
        *ppConfigPool = pConfigPool;
    }
    
Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pConfigPool);
    }
    return hr;
}

HRESULT CPolicyConfigPool::Init()
{
    _cs = ClrCreateCriticalSection("Fusion: Policy Config Pool", CrstFusionConfigSettings, CRST_REENTRANCY);
    if (!_cs) {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT CPolicyConfigPool::CreatePolicyConfig(
                                        LPCWSTR pwzConfigFile, 
                                        FILETIME *pftLastMod,
                                        DWORD dwWhichConfig,
                                        CDebugLog *pdbglog,
                                        CPolicyConfig **ppConfig)
{
    HRESULT             hr = S_OK;
    DWORD               dwHash = 0;
    LISTNODE            pos = NULL;
    CPolicyConfig       *pConfig = NULL;
    CCriticalSection    cs(_cs);

    _ASSERTE(pwzConfigFile);
    _ASSERTE(pftLastMod);

    if (ppConfig) {
        *ppConfig = NULL;
    }

    dwHash = HashString(pwzConfigFile, 0, CONFIG_HASH_TABLE_SIZE, FALSE);
   
    cs.Lock();

    // look up the table first
    pos = _listConfigs[dwHash].GetHeadPosition();
    while(pos) {
        pConfig = _listConfigs[dwHash].GetNext(pos);
        _ASSERTE(pConfig);

        if (pConfig->IsEqual(pwzConfigFile, pftLastMod)) {
            if (ppConfig) {
                *ppConfig = pConfig;
                pConfig->AddRef();
            }
            goto Exit;
        }
    }

    // not in the table, create one. 

    pConfig = NEW(CPolicyConfig);
    if (!pConfig) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pConfig->Init(pwzConfigFile, pftLastMod, dwWhichConfig, pdbglog);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!_listConfigs[dwHash].AddTail(pConfig)) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }
    pConfig->AddRef();

    *ppConfig = pConfig;

Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pConfig);
    }
    return hr;
}

BOOL CPolicyConfigPool::RemoveConfig(CPolicyConfig *pConfig)
{
    DWORD               dwHash = 0;
    LISTNODE            oldpos = NULL;
    LISTNODE            pos = NULL;
    CPolicyConfig       *pConfigTmp = NULL;
    CCriticalSection    cs(_cs);
    
    _ASSERTE(pConfig);
    _ASSERTE(pConfig->_pwzConfigFile);

    dwHash = HashString(pConfig->_pwzConfigFile, 0, CONFIG_HASH_TABLE_SIZE, FALSE);
   
    cs.Lock();

    pos = _listConfigs[dwHash].GetHeadPosition();
    while(pos) {
        oldpos = pos;
        pConfigTmp = _listConfigs[dwHash].GetNext(pos);
        _ASSERTE(pConfigTmp);

        if (pConfigTmp == pConfig ) {
            _listConfigs[dwHash].RemoveAt(oldpos);
            SAFERELEASE(pConfigTmp);
            return TRUE;
        }
    }

    return FALSE;
}

// class CPolicyConfig
CPolicyConfig::CPolicyConfig()
: _cRef(1)
, _pwzConfigFile(NULL)
, _pNodeFact(NULL)
, _bInConfigPool(TRUE)
{
    memset(&_ftLastMod, 0, sizeof(FILETIME));
}

CPolicyConfig::~CPolicyConfig()
{
    SAFEDELETEARRAY(_pwzConfigFile);
    SAFERELEASE(_pNodeFact);
}

ULONG CPolicyConfig::AddRef()
{
    ULONG lRef = 0;
    
    if (_bInConfigPool) {
        g_pConfigPool->Lock();
    }

    lRef = InterlockedIncrement(&_cRef);
    
    if (_bInConfigPool) {
        g_pConfigPool->Unlock();
    }

    return lRef;
}

ULONG CPolicyConfig::Release()
{
    ULONG lRef = 0;

    if (_bInConfigPool) {
        g_pConfigPool->Lock();
        lRef = InterlockedDecrement((LONG*) &_cRef);
        if (lRef == 1) {
            // well, only reference left will be the copy in global pool. Release it as well.
            
            // prevent delete this object in g_pConfigPool->RemoveConfig.
            _cRef += 2;
            BOOL bRemoved;
            bRemoved = g_pConfigPool->RemoveConfig(this);
            _ASSERTE(bRemoved == TRUE);
            lRef = _cRef - 2;
            _ASSERTE(lRef == 0);
        }
        g_pConfigPool->Unlock();
    }
    else {
        lRef = InterlockedDecrement((LONG*)&_cRef);
    }

    if (lRef == 0) {
        delete this;
    }
    
    return lRef;
}

/* static */
HRESULT CPolicyConfig::Create(LPCWSTR pwzConfigFile, 
                              DWORD dwWhichConfig,
                              CDebugLog *pdbglog, 
                              CPolicyConfig **ppConfig)
{
    HRESULT hr = S_OK;
    FILETIME ftLastMod;

    _ASSERTE(pwzConfigFile);
    _ASSERTE(pwzConfigFile[0]);
    _ASSERTE(ppConfig);

    *ppConfig = NULL;

    hr = GetFileLastModified(pwzConfigFile, &ftLastMod);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = InitializeConfigPool();
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = g_pConfigPool->CreatePolicyConfig(pwzConfigFile, &ftLastMod, dwWhichConfig, pdbglog, ppConfig);
    if (FAILED(hr)) {
        goto Exit;
    }
    
Exit:
    return hr;
}
/* static */
HRESULT CPolicyConfig::Create(BYTE *pbAppConfigBlob, 
                          DWORD cbAppConfigBlob,
                          DWORD dwWhichConfig,
                          CDebugLog *pdbglog, 
                          CPolicyConfig **ppConfig)
{
    HRESULT hr = S_OK;
    CPolicyConfig *pConfig = NULL;

    pConfig = NEW(CPolicyConfig);
    if (!pConfig) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pConfig->Init(pbAppConfigBlob, cbAppConfigBlob, dwWhichConfig, pdbglog);
    if (FAILED(hr)) {
        SAFERELEASE(pConfig);
        goto Exit;
    }

    *ppConfig = pConfig;

Exit:
    return hr;
}

HRESULT CPolicyConfig::Init(LPCWSTR pwzConfigFile, 
                            FILETIME *pftLastMod, 
                            DWORD dwWhichConfig,
                            CDebugLog *pdbglog)
{
    HRESULT hr = S_OK;
    BOOL bProcessLinkConfig = (dwWhichConfig == ePolicyLevelApp);
    CNodeFactory::ParseCtl parseCtrl = (dwWhichConfig == ePolicyLevelAdmin)? CNodeFactory::stopAfterRuntimeSection : CNodeFactory::parseAll;

    memcpy(&_ftLastMod, pftLastMod, sizeof(FILETIME));

    _pwzConfigFile = WSTRDupDynamic(pwzConfigFile);
    if (!_pwzConfigFile) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    // CLR_STARTUP_OPT:
    // The machine.config file can be very large.  We cannot afford
    // to parse all of it at CLR startup time.
    //
    // Accordingly, we instruct ParseXML to stop parsing the
    // machine.config file when it sees the end of the
    // <runtime>...</runtime> section that holds our data (if any).
    //
    // By construction, this section is now placed near the top
    // of machine.config.
    // 
    hr = ParseXML(&_pNodeFact, _pwzConfigFile, pdbglog, 
                  bProcessLinkConfig, parseCtrl);

Exit:
    return hr;
}

HRESULT CPolicyConfig::Init(BYTE *pbAppConfigBlob, 
                 DWORD cbAppConfigBlob, 
                 DWORD dwWhichConfig,
                 CDebugLog *pdbglog)
{
    _bInConfigPool = FALSE;
    return ParseXML(&_pNodeFact, pbAppConfigBlob, cbAppConfigBlob, pdbglog);
}

BOOL CPolicyConfig::IsEqual(LPCWSTR pwzConfigFile, FILETIME *pftLastMod)
{
    _ASSERTE(pwzConfigFile);
    _ASSERTE(pftLastMod);

    if (memcmp(&_ftLastMod, pftLastMod, sizeof(FILETIME))) {
        return FALSE;
    }

    return !FusionCompareStringI(_pwzConfigFile, pwzConfigFile);
}

// class CAppCtxPolicyConfigs
CAppCtxPolicyConfigs::CAppCtxPolicyConfigs()
: _cRef(1)
, _pAppConfig(NULL)
, _pHostConfig(NULL)
, _pAdminConfig(NULL)
, _pPubPolicy(NULL)
{
}

CAppCtxPolicyConfigs::~CAppCtxPolicyConfigs()
{
    SAFERELEASE(_pAppConfig);
    SAFERELEASE(_pHostConfig);
    SAFERELEASE(_pAdminConfig);
    SAFERELEASE(_pPubPolicy);
}

ULONG CAppCtxPolicyConfigs::AddRef()
{
    return InterlockedIncrement((LONG*) &_cRef);
}

ULONG CAppCtxPolicyConfigs::Release()
{
    ULONG lRes = InterlockedDecrement((LONG*) &_cRef);

    if (lRes == 0){ 
        delete this;
    }

    return lRes;
}

/* static */
HRESULT CAppCtxPolicyConfigs::Create(CAppCtxPolicyConfigs **ppConfig)
{
    HRESULT hr = S_OK;
    CAppCtxPolicyConfigs *pConfig = NULL;

    _ASSERTE(ppConfig);

    *ppConfig = NULL;

    pConfig = NEW(CAppCtxPolicyConfigs);
    if (!pConfig) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pConfig->Init();
    if (SUCCEEDED(hr)) {
        *ppConfig = pConfig;
    }
    
Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pConfig);
    }
    return hr;
}

HRESULT CAppCtxPolicyConfigs::Init()
{
    HRESULT hr = S_OK;

    hr = CPublisherPolicy::Create(TRUE, &_pPubPolicy);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    return hr;
}

void CAppCtxPolicyConfigs::SetPolicyConfig(DWORD dwWhichConfig, CPolicyConfig *pConfig)
{
    _ASSERTE(pConfig);

    switch (dwWhichConfig) {
        case ePolicyLevelApp:
            _ASSERTE(!_pAppConfig);
            _pAppConfig = pConfig;
            if (_pAppConfig) {
                _pAppConfig->AddRef();
            }
            return;
        case ePolicyLevelHost:
            _ASSERTE(!_pHostConfig);
            _pHostConfig = pConfig;
            if (_pHostConfig) {
                _pHostConfig->AddRef();
            }
            return;
        case ePolicyLevelAdmin:
            _ASSERTE(!_pAdminConfig);
            _pAdminConfig = pConfig;
            if (_pAdminConfig) {
                _pAdminConfig->AddRef();
            }
            return;
        default:
            _ASSERTE(!"Unknown input config type!");
            return;
    }
}

HRESULT CAppCtxPolicyConfigs::GetPolicyConfig(DWORD dwWhichConfig, CPolicyConfig **ppConfig)
{
    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    _ASSERTE(ppConfig);

    switch (dwWhichConfig) {
        case ePolicyLevelApp:
            if (!_pAppConfig) {
                return hr;
            }
            *ppConfig = _pAppConfig;
            _pAppConfig->AddRef();
            return S_OK;
        case ePolicyLevelHost:
            if (!_pHostConfig) {
                return hr;
            }
            *ppConfig = _pHostConfig;
            _pHostConfig->AddRef();
            return S_OK;
        case ePolicyLevelAdmin:
            if (!_pAdminConfig) {
                return hr;
            }
            *ppConfig = _pAdminConfig;
            _pAdminConfig->AddRef();
            return S_OK;
        default:
            _ASSERTE(!"Unknown input config type!");
            return hr;
    }
}

BOOL CAppCtxPolicyConfigs::IsEqual(CAppCtxPolicyConfigs *pAppCtxConfigs)
{
    BOOL bEqual = FALSE;

    if ((!_pAppConfig && pAppCtxConfigs->_pAppConfig) || (_pAppConfig && !pAppCtxConfigs->_pAppConfig)) {
        bEqual = FALSE;
    }

    if (bEqual) {
        bEqual = _pAppConfig->IsEqual(pAppCtxConfigs->_pAppConfig);
    }

    if (bEqual) {
        if ((!_pHostConfig && pAppCtxConfigs->_pHostConfig) || (_pHostConfig && !pAppCtxConfigs->_pHostConfig)) {
            bEqual = FALSE;
        }
    }

    if (bEqual) {
        bEqual = _pHostConfig->IsEqual(pAppCtxConfigs->_pHostConfig);
    }

    if (bEqual) {
        if ((!_pPubPolicy && pAppCtxConfigs->_pPubPolicy) || (_pPubPolicy && !pAppCtxConfigs->_pPubPolicy)) {
            bEqual = FALSE;
        }
    }

    if (bEqual) {
        bEqual = _pPubPolicy->IsEqual(pAppCtxConfigs->_pPubPolicy);
    }

    if (bEqual) {
        if ((!_pAdminConfig && pAppCtxConfigs->_pAdminConfig) || (_pAdminConfig && !pAppCtxConfigs->_pAdminConfig)) {
            bEqual = FALSE;
        }
    }

    if (bEqual) {
        bEqual = _pAdminConfig->IsEqual(pAppCtxConfigs->_pAdminConfig);
    }

    return bEqual;
}

HRESULT CAppCtxPolicyConfigs::GetPolicyInfo(IAssemblyName *pNameSource,
                                            BOOL bDisallowAppBindingRedirects,
                                            BOOL bDisallowApplyPublisherPolicy, 
                                            BOOL bUnifyFx,
                                            BOOL bForceUnifyFX,
                                            LPCWSTR pwzAppBase,
                                            CDebugLog *pdbglog,
                                            IAssemblyName **ppNamePolicy,
                                            __deref_opt_out_opt LPWSTR *ppwzPolicyCodebase,
                                            AsmBindHistoryInfo **ppHistInfo,
                                            DWORD *pdwPolicyApplied)
{
    HRESULT                              hr = S_OK;
    IAssemblyName                       *pNamePolicy = NULL;
    WCHAR                                wzAsmName[MAX_PATH];
    DWORD                                dwSizeName;
    WCHAR                                wzAsmVersion[MAX_VERSION_DISPLAY_SIZE+1];
    DWORD                                dwSizeVer;
    WCHAR                                wzPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    DWORD                                dwSizePKT;
    WCHAR                                wzCulture[MAX_CULTURE_SIZE];
    DWORD                                dwSizeCulture;
    WCHAR                                wzVerHostCfg[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR                                wzVerAppCfg[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR                                wzVerFxCfg[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR                                wzVerPublisherCfg[MAX_VERSION_DISPLAY_SIZE+1];
    WCHAR                                wzVerAdminCfg[MAX_VERSION_DISPLAY_SIZE+1];
    BOOL                                 bSafeMode = FALSE;
    WORD                                 wVerMajor;
    WORD                                 wVerMinor;
    WORD                                 wVerRev;
    WORD                                 wVerBld;
    DWORD                                dwSize;
    PEKIND                               pe = peInvalid;
    BOOL                                 bPELocked = FALSE;

    LPWSTR                               pwzPublisherPolicyCodebase = NULL;

    WCHAR                                wzNameRtg[MAX_PATH];
    WCHAR                                wzPktRtg[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    WCHAR                                wzVerRtg[MAX_VERSION_DISPLAY_SIZE+1];

    BOOL                                 bRetarget = FALSE;

    DWORD                                ePolicyApplied = ePolicyLevelNone;

    AsmBindHistoryInfo                  *pHistInfo = NULL;

    dwSizeName = ARRAYSIZE(wzAsmName);
    dwSizeVer = ARRAYSIZE(wzAsmVersion);
    dwSizePKT = ARRAYSIZE(wzPublicKeyToken);
    dwSizeCulture = ARRAYSIZE(wzCulture);

    wzAsmName[0] = L'\0';
    wzAsmVersion[0] = L'\0';
    wzCulture[0] = L'\0';
    wzPublicKeyToken[0] = L'\0';

    hr = PrepQueryMatchData(pNameSource, wzAsmName, &dwSizeName, 
                            wzAsmVersion, &dwSizeVer, 
                            wzPublicKeyToken, &dwSizePKT, 
                            wzCulture, &dwSizeCulture, &pe);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = CAssemblyName::CloneForBind(pNameSource, &pNamePolicy);
    if (FAILED(hr)) {
        goto Exit;
    }

    // pNameSource->Clone(pNamePolicy) accidentally copies the retarget
    // flag to pNamePolicy. pNamePolicy should not carry that flag
    // since it contains the "retargeted" reference. Clean it here.
    pNamePolicy->SetProperty(ASM_NAME_RETARGET, NULL, 0);

    // Now we can start applying policies.
    dwSize = sizeof(BOOL);
    hr = pNameSource->GetProperty(ASM_NAME_RETARGET, &bRetarget, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    // ASM_NAME_RETARGET is not set. Treat it as FALSE
    if (dwSize == 0) {
        bRetarget = FALSE;
    }

    {
        hr = StringCbCopy(wzPktRtg, sizeof(wzPktRtg), wzPublicKeyToken);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCbCopy(wzVerRtg, sizeof(wzVerRtg), wzAsmVersion);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCbCopy(wzNameRtg, sizeof(wzNameRtg), wzAsmName);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // Get app.cfg policy

    if (_pAppConfig) {
        // We've parsed the app.cfg before.
        if (bDisallowAppBindingRedirects) {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_DISALLOW_APP_BINDING_REDIRECTS);

            hr = StringCbCopy(wzVerAppCfg, sizeof(wzVerAppCfg), wzVerRtg);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
        else {
            dwSize = ARRAYSIZE(wzVerAppCfg);
            hr = _pAppConfig->GetPolicyVersion(wzNameRtg, wzPktRtg, wzCulture, wzVerRtg, pe, wzVerAppCfg, &dwSize, &pe);
            if (FAILED(hr)) {
                goto Exit;
            }
            
            if (hr == S_OK) {
                ePolicyApplied |= ePolicyLevelApp;

                DEBUGOUT2(pdbglog, 0, ID_FUSLOG_APP_CFG_REDIRECT, wzVerRtg, wzVerAppCfg);

                if (pe != peInvalid) {
                    bPELocked = TRUE;
                    DEBUGOUT1(pdbglog, 0, ID_FUSLOG_PROCESSOR_ARCHITECTURE_LOCKED, ProcessorArchitectureFromPEKIND(pe));
                }
            }

            hr = _pAppConfig->GetSafeMode(wzNameRtg, wzPktRtg, wzCulture, wzVerRtg, pe, &bSafeMode);
            if (FAILED(hr)) {
                goto Exit;
            }
        }
    }
    else {
        hr = StringCbCopy(wzVerAppCfg, sizeof(wzVerAppCfg), wzVerRtg);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    {
        hr = StringCbCopy(wzVerFxCfg, sizeof(wzVerFxCfg), wzVerAppCfg);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
    if (bDisallowApplyPublisherPolicy) {
        DEBUGOUT(pdbglog, 0, ID_FUSLOG_DISALLOW_APPLY_PUB_POLICY);
        bSafeMode = FALSE;
    }

    if (bSafeMode) {
        DEBUGOUT(pdbglog, 0, ID_FUSLOG_APP_CFG_SAFE_MODE);

        // We are in safe mode, so treat this like there is no publisher.cfg
        hr = StringCbCopy(wzVerPublisherCfg, sizeof(wzVerPublisherCfg), wzVerFxCfg);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        dwSize = ARRAYSIZE(wzVerPublisherCfg);
        hr = _pPubPolicy->GetPolicyVersion(pdbglog, wzNameRtg, wzPktRtg, wzCulture, wzVerFxCfg, pe, 
                                           wzVerPublisherCfg, &dwSize, &pwzPublisherPolicyCodebase, &pe); 
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_OK) {
            ePolicyApplied |= ePolicyLevelPublisher;
            DEBUGOUT2(pdbglog, 0, ID_FUSLOG_PUB_CFG_REDIRECT, wzVerFxCfg, wzVerPublisherCfg);
            if (!bPELocked && pe != peInvalid) {
                bPELocked = TRUE;
                DEBUGOUT1(pdbglog, 0, ID_FUSLOG_PROCESSOR_ARCHITECTURE_LOCKED, ProcessorArchitectureFromPEKIND(pe));
            }           
        }
    }
    
    // Get host config policy
    // Skip host policy if framework unification has been applied and g_dwConfigForceUnification is set
    if (_pHostConfig && !(bForceUnifyFX && (ePolicyApplied & ePolicyUnifiedToCLR))) {
        dwSize = ARRAYSIZE(wzVerHostCfg);
        hr = _pHostConfig->GetPolicyVersion(wzNameRtg, wzPktRtg, wzCulture,
                                          wzVerPublisherCfg, pe, wzVerHostCfg, &dwSize, &pe);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_OK) {
            ePolicyApplied |= ePolicyLevelHost;
            DEBUGOUT2(pdbglog, 0, ID_FUSLOG_HOST_CFG_REDIRECT, wzVerPublisherCfg, wzVerHostCfg);

            if (!bPELocked && pe != peInvalid) {
                bPELocked = TRUE;
                DEBUGOUT1(pdbglog, 0, ID_FUSLOG_PROCESSOR_ARCHITECTURE_LOCKED, ProcessorArchitectureFromPEKIND(pe));
            }                      
        }
    }
    else {
        hr = StringCbCopy(wzVerHostCfg, sizeof(wzVerHostCfg), wzVerPublisherCfg);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    
    // Apply admin policy
    if (_pAdminConfig) {
        dwSize = ARRAYSIZE(wzVerAdminCfg);
        hr = _pAdminConfig->GetPolicyVersion(wzNameRtg, wzPktRtg, wzCulture,
                                          wzVerHostCfg, pe, wzVerAdminCfg, &dwSize, &pe);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_OK) {
            ePolicyApplied |= ePolicyLevelAdmin;
            DEBUGOUT2(pdbglog, 0, ID_FUSLOG_MACHINE_CFG_REDIRECT, wzVerHostCfg, wzVerAdminCfg);
            if (!bPELocked && pe != peInvalid) {
                bPELocked = TRUE;
                DEBUGOUT1(pdbglog, 0, ID_FUSLOG_PROCESSOR_ARCHITECTURE_LOCKED, ProcessorArchitectureFromPEKIND(pe));
            }                      
        }
    }
    else {
        hr = StringCbCopy(wzVerAdminCfg, sizeof(wzVerAdminCfg), wzVerHostCfg);
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // Set the post-policy version

    hr = VersionFromString(wzVerAdminCfg, &wVerMajor, &wVerMinor, &wVerBld, &wVerRev);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pNamePolicy->SetProperty(ASM_NAME_MAJOR_VERSION, &wVerMajor, sizeof(WORD));
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pNamePolicy->SetProperty(ASM_NAME_MINOR_VERSION, &wVerMinor, sizeof(WORD)); 
    if (FAILED(hr)) {
        goto Exit;
    }
    
    hr = pNamePolicy->SetProperty(ASM_NAME_REVISION_NUMBER, &wVerRev, sizeof(WORD));
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pNamePolicy->SetProperty(ASM_NAME_BUILD_NUMBER, &wVerBld, sizeof(WORD));
    if (FAILED(hr)) {
        goto Exit;
    }

    if (pe != peInvalid) {
        hr = pNamePolicy->SetProperty(ASM_NAME_ARCHITECTURE, &pe, sizeof(PEKIND));
        if (FAILED(hr)) {
            goto Exit;
        }
    }

    // Get the codebase hint from the right cfg file
    
    // machine config has final say on codebase.
    if (_pAdminConfig && ppwzPolicyCodebase) {
        hr = _pAdminConfig->GetCodebaseHint(wzNameRtg, wzVerAdminCfg,
                                          wzPublicKeyToken, wzCulture, pe, 
                                          NULL, ppwzPolicyCodebase);
        if (FAILED(hr)) {
            goto Exit;
        }
        else if (*ppwzPolicyCodebase) {
            DEBUGOUT1(pdbglog, 0, ID_FUSLOG_ADMIN_CFG_POLICY_CODEBASE, *ppwzPolicyCodebase);
        }
    }

    if (ppwzPolicyCodebase && !*ppwzPolicyCodebase && (ePolicyApplied != ePolicyLevelNone)) {
        // There was a redirect, and machine config does not have codebase hint.
        if (ePolicyApplied & ePolicyLevelHost) {
            hr = _pHostConfig->GetCodebaseHint(wzNameRtg, wzVerAdminCfg,
                                              wzPktRtg, wzCulture, pe, 
                                              NULL, ppwzPolicyCodebase);
        }
        else if (ePolicyApplied & ePolicyLevelPublisher) {
            *ppwzPolicyCodebase = pwzPublisherPolicyCodebase;
            pwzPublisherPolicyCodebase = NULL;
        }
        else if (ePolicyApplied & ePolicyLevelApp) {
            hr = _pAppConfig->GetCodebaseHint(wzNameRtg, wzVerAdminCfg,
                                              wzPktRtg, wzCulture, pe,
                                              pwzAppBase, ppwzPolicyCodebase);
        }
        if (FAILED(hr)) {
            goto Exit;
        }
        else if (*ppwzPolicyCodebase) {
            DEBUGOUT1(pdbglog, 0, ID_FUSLOG_POLICY_CODEBASE, *ppwzPolicyCodebase);
        }
    }

    // Populate the bind history

    if (ppHistInfo) {

        _ASSERTE(!*ppHistInfo);

        pHistInfo = NEW(AsmBindHistoryInfo());
        if (!pHistInfo) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = StringCbCopy(pHistInfo->wzVerReference, sizeof(pHistInfo->wzVerReference), wzAsmVersion);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCbCopy(pHistInfo->wzVerAppCfg, sizeof(pHistInfo->wzVerAppCfg), wzVerAppCfg);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        hr = StringCbCopy(pHistInfo->wzVerPublisherCfg, sizeof(pHistInfo->wzVerPublisherCfg), wzVerPublisherCfg);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = StringCbCopy(pHistInfo->wzVerAdminCfg, sizeof(pHistInfo->wzVerAdminCfg), wzVerAdminCfg);
        if (FAILED(hr)) {
            goto Exit;
        }

        pHistInfo->pwzAsmName = WSTRDupDynamic(wzNameRtg);
        if (!pHistInfo->pwzAsmName)
        {
            SAFERELEASE(pHistInfo);
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = StringCbCopy(pHistInfo->wzPublicKeyToken, sizeof(pHistInfo->wzPublicKeyToken), wzPktRtg);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (wzCulture && wzCulture[0] && FusionCompareStringI(wzCulture, CFG_CULTURE_NEUTRAL)) {
            pHistInfo->pwzCulture = WSTRDupDynamic(wzCulture);
            if (!pHistInfo->pwzCulture)
            {
                SAFERELEASE(pHistInfo);
                hr = E_OUTOFMEMORY;
                goto Exit;
            }
        }

        *ppHistInfo = pHistInfo;
    }

    // Done. Return policy reference.

    *ppNamePolicy = pNamePolicy;
    (*ppNamePolicy)->AddRef();

    *pdwPolicyApplied = ePolicyApplied;

    if (pdbglog && IsLoggingNeeded()) {

        _ASSERTE(pNamePolicy);

        // Log the post-policy reference
        dwSize = ARRAYSIZE(wzNameRtg);
        pNamePolicy->GetDisplayName(wzNameRtg, &dwSize, ASM_DISPLAYF_FULL);
        DEBUGOUT1(pdbglog, 0, ID_FUSLOG_POST_POLICY_REFERENCE, wzNameRtg);
    }

Exit:

    SAFEDELETEARRAY(pwzPublisherPolicyCodebase);

    if (FAILED(hr)) {
        DEBUGOUT1(pdbglog, 1, ID_FUSLOG_APPLY_POLICY_FAILED, hr);
    }

    SAFERELEASE(pNamePolicy);

    return hr;
}

HRESULT CAppCtxPolicyConfigs::GetAppCfgCodebaseHint(IAssemblyName *pName, 
                                                    LPCWSTR pwzAppBase,
                                                    __deref_out LPWSTR *ppwzCodebaseHint)
{
    HRESULT             hr = S_OK;
    WCHAR               wzName[MAX_PATH];
    WCHAR               wzPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    WCHAR               wzCulture[MAX_CULTURE_SIZE];
    WCHAR               wzVersion[MAX_VERSION_DISPLAY_SIZE + 1];
    DWORD               dwVerHigh = 0;
    DWORD               dwVerLow = 0;
    DWORD               dwSize;
    CAssemblyName       *pCName= NULL;
    PEKIND              pe = peInvalid;

    if (!_pAppConfig) {
        return S_FALSE;
    }

    wzName[0] = L'\0';
    wzPublicKeyToken[0] = L'\0';
    wzCulture[0] = L'\0';

    pCName= static_cast<CAssemblyName *>(pName); // dynamic_cast
    _ASSERTE(pCName);

    dwSize = MAX_PATH;
    hr = pName->GetName(&dwSize, wzName);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!dwSize) {
        // Where-ref bind. No hint possible
        hr = S_FALSE;
        goto Exit;
    }

    dwSize = ARRAYSIZE(wzPublicKeyToken) * sizeof(WCHAR);
    hr = pCName->GetPublicKeyToken(&dwSize, (LPBYTE)wzPublicKeyToken , TRUE);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = pName->GetVersion(&dwVerHigh, &dwVerLow);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = StringCchPrintfW(wzVersion, MAX_VERSION_DISPLAY_SIZE + 1, L"%d.%d.%d.%d",
               HIWORD(dwVerHigh), LOWORD(dwVerHigh), HIWORD(dwVerLow),
               LOWORD(dwVerLow));
    if (FAILED(hr)) {
        goto Exit;
    }


    dwSize = ARRAYSIZE(wzCulture) * sizeof(WCHAR);
    hr = pName->GetProperty(ASM_NAME_CULTURE, wzCulture, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    dwSize = sizeof(PEKIND);
    hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, (LPBYTE)&pe, &dwSize);
    if (FAILED(hr)) {
        goto Exit;
    }

    hr = _pAppConfig->GetCodebaseHint(wzName, wzVersion, wzPublicKeyToken[0]?wzPublicKeyToken:NULL,
                                    wzCulture, pe, pwzAppBase, ppwzCodebaseHint);
    if (FAILED(hr)) {
        goto Exit;
    }                                    
    
Exit:
    return hr;
}


// Format of publisher policy index
//HKLM\Software\Microsoft\Fusion\PublisherPolicy<CachePath_CLRVersion>
//    Latest
//    Oldest
//    UsageMask
//    <PolicyAssemblyName.Culture.PublicKeyToken>
//        <ProcessorArchitecture>
//            <PolicyAssemblyVersion>
//                <Id>
//                  ConfigFilePath  filePath
//    IndexN  

// class CPublisherPolicy
CPublisherPolicy::CPublisherPolicy()
: _cRef(1)
, _version(0)
{
    memset(&_ftLegacyPubPol, 0, sizeof(FILETIME));
}

CPublisherPolicy::~CPublisherPolicy()
{
}

ULONG CPublisherPolicy::AddRef()
{
    return InterlockedIncrement((LONG*) &_cRef);
}

ULONG CPublisherPolicy::Release()
{
    ULONG lRes = InterlockedDecrement((LONG*) &_cRef);

    if (lRes == 0){ 
        delete this;
    }

    return lRes;
}

/* static */
HRESULT CPublisherPolicy::Create(BOOL bForBind, CPublisherPolicy **ppConfig)
{
    HRESULT hr = S_OK;
    CPublisherPolicy *pConfig = NULL;

    _ASSERTE(ppConfig);

    *ppConfig = NULL;

    pConfig = NEW(CPublisherPolicy);
    if (!pConfig) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    hr = pConfig->Init(bForBind);
    if (SUCCEEDED(hr)) {
        *ppConfig = pConfig;
    }
    
Exit:
    if (FAILED(hr)) {
        SAFEDELETE(pConfig);
    }
    return hr;
}

HRESULT CPublisherPolicy::Init(BOOL bForBind)
{
    return S_OK;
}


HRESULT CPublisherPolicy::GetPolicyVersionFromDisk(
                             CDebugLog *pdbglog,
                             LPCWSTR wzAssemblyName, 
                             LPCWSTR wzPublicKeyToken,
                             LPCWSTR wzCulture, 
                             LPCWSTR wzVersionIn,
                             PEKIND  peIn,
                             __out_ecount_opt(*pdwSizeVer) LPWSTR  pwzVersionOut,
                             LPDWORD pdwSizeVer,
                             __deref_opt_out_opt LPWSTR *ppwzCodebaseHint,
                             __out_opt PEKIND *peOut )
{
    HRESULT hr = S_OK;
    WORD    wMajor = 0;
    WORD    wMinor = 0;
    WORD    wRev = 0;
    WORD    wBld = 0;
    WCHAR   wzCfgPath[MAX_PATH];
    DWORD   dwSize;
    CNodeFactory *pNodeFact = NULL;

    // Extract version

    hr = VersionFromString(wzVersionIn, &wMajor, &wMinor, &wBld, &wRev);
    if (FAILED(hr)) {
        goto Exit;
    }

    // Find publisher policy file

    dwSize = MAX_PATH;
    hr = GetPublisherPolicyFilePath(wzAssemblyName, wzPublicKeyToken, wzCulture, peIn, 
                                    wMajor, wMinor, wzCfgPath, &dwSize, peOut);
    if (FAILED(hr)) {
        if (hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) {
            DEBUGOUT(pdbglog, 0, ID_FUSLOG_PUBLISHER_POLICY_CONFIG_MISSING);
        }
        goto Exit;
    }

    if (hr == S_FALSE) {
        // No publisher policy file
        DEBUGOUT(pdbglog, 0, ID_FUSLOG_PUB_CFG_MISSING);

        dwSize = lstrlenW(wzVersionIn) + 1;
        if (*pdwSizeVer < dwSize) {
            hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
        }
        else {
            hr = StringCchCopy(pwzVersionOut, *pdwSizeVer, wzVersionIn);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        hr = S_FALSE;
        *pdwSizeVer = dwSize;
        goto Exit;
    }
    else {
        DEBUGOUT1(pdbglog, 0, ID_FUSLOG_PUB_CFG_FOUND, wzCfgPath);

        hr = ParseXML(&pNodeFact, wzCfgPath, pdbglog, FALSE);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = pNodeFact->GetPolicyVersion(wzAssemblyName,wzPublicKeyToken, wzCulture,
                                         wzVersionIn, *peOut, pwzVersionOut, pdwSizeVer, peOut);
        if (FAILED(hr)) {
            goto Exit;
        }
    
        if (hr == S_OK) {
            hr = pNodeFact->GetCodebaseHint(wzAssemblyName, pwzVersionOut, wzPublicKeyToken, wzCulture, *peOut,
                                       NULL, ppwzCodebaseHint);
            if (FAILED(hr)) {
                goto Exit;
            }

            // reset hr to S_OK indicate we have a redirect.
            hr = S_OK;
        }
    }

Exit:
    SAFERELEASE(pNodeFact);
    return hr;
}


HRESULT InitializeConfigPool()
{
    HRESULT hr = S_OK;
    CPolicyConfigPool *pConfigPool = NULL;

    if (g_pConfigPool) {
        return S_OK;
    }

    hr = CPolicyConfigPool::Create(&pConfigPool);
    if (FAILED(hr)) {
        goto Exit;
    }

    // try to update the global pointer
    if (InterlockedCompareExchangePointer((void **)&g_pConfigPool, pConfigPool, NULL)) {
        // oops, lost the update
        SAFEDELETE(pConfigPool);
    }
    
Exit:
    return hr;
}
