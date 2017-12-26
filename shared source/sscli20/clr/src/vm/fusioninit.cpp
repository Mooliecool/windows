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

#include "common.h"
#include "clrhost.h"
#include "helpers.h"
#include "dbglog.h"
#include "adl.h"
#include "cacheutils.h"
#include "actasm.h"
#include "naming.h"
#include "policy.h"


#define DEVOVERRIDE_PATH                       L".local\\"
#define REG_KEY_IMAGE_FILE_EXECUTION_OPTIONS   L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options"
#define REG_VAL_DEVOVERRIDE_PATH               L"DevOverridePath"
#define REG_VAL_DEVOVERRIDE_ENABLE             L"DevOverrideEnable"
#define REG_VAL_FUSION_FORCE_UNIFICATION       L"OnlyUseLatestCLR" 
#define REG_VAL_USE_LEGACY_IDENTITY_FORMAT     L"UseLegacyIdentityFormat"

// Registry / Environment variable
#define BINDING_CONFIGURATION                  L"BindingConfiguration"

extern HMODULE g_pMSCorEE;
extern BOOL g_bRunningOnNT;
extern BOOL g_bRunningOnNT51OrHigher;
extern BOOL g_bRunningOnNT6OrHigher;

WCHAR                         g_szWindowsDir[MAX_PATH+1];
WCHAR                         g_FusionDllPath[MAX_PATH+1];
HINSTANCE                     g_hInst = NULL;
HMODULE                       g_hMSCorEE = NULL;
DWORD                         g_dwLogInMemory;
DWORD                         g_dwLogLevel;
DWORD                         g_dwForceLog;
DWORD                         g_dwLogFailures;
DWORD                         g_dwLogResourceBinds;
DWORD                         g_dwDevOverrideEnable;
WORD                          g_wProcessorArchitecture;     // Default to 32 bit
BOOL                          g_fWow64Process;          // Wow64 Process
PEKIND                        g_peKindProcess;
List<CAssemblyDownload *>    *g_pDownloadList;
BOOL                          g_bLogToWininet;
WCHAR                         g_wzCustomLogPath[MAX_PATH];
DWORD                         g_dwConfigForceUnification;
DWORD                         g_dwUseLegacyIdentityFormat;

CRITSEC_COOKIE                g_csInitClb;
CRITSEC_COOKIE                g_csConfigSettings;
CRITSEC_COOKIE                g_csSingleUse;
CRITSEC_COOKIE                g_csDownload;
CRITSEC_COOKIE                g_csBindLog;
LCID                          g_lcid;


// Note: g_wzEXEPath is ...\clix.exe most of the time on Rotor. 
//  Use ACTAG_APP_NAME and ACTAG_APP_BASE_URL as appropriate
WCHAR                         g_wzEXEPath[MAX_PATH+1];

HSATELLITE g_hSatelliteInst;


WCHAR g_wzLocalDevOverridePath[MAX_PATH + 1];
WCHAR g_wzGlobalDevOverridePath[MAX_PATH + 1];
DWORD g_dwDevOverrideFlags;

extern BOOL OnUnicodeSystem(void);

HRESULT GetScavengerQuotasFromReg(DWORD *pdwZapQuotaInGAC,
                                  DWORD *pdwDownloadQuotaAdmin,
                                  DWORD *pdwDownloadQuotaUser);

HRESULT CheckTestRootCertificate(BOOL *pbTestRootInstalled);
HRESULT SetupDevOverride(LPCWSTR pwzBindingConfigDevOverridePath);

static DWORD GetConfigDWORD(LPCWSTR wzName, DWORD dwDefault)
{
    WRAPPER_CONTRACT;
    WCHAR wzValue[16];
    DWORD dwValue;

    if (PAL_FetchConfigurationString(TRUE, wzName, wzValue, sizeof(wzValue) / sizeof(WCHAR)))
    {
        LPWSTR pEnd;
        dwValue = wcstol(wzValue, &pEnd, 16);   // treat it has hex
        if (pEnd != wzValue)                    // success
            return dwValue;
    }

    return dwDefault;
}

BOOL InitFusionCriticalSections()
{
    WRAPPER_CONTRACT;
    BOOL fRet = FALSE;

    g_csInitClb = ClrCreateCriticalSection("Fusion: Comp Lib (global)", CrstFusionClb, CRST_REENTRANCY);
    if (!g_csInitClb) {
        goto Exit;
    }

    g_csDownload = ClrCreateCriticalSection("Fusion: Download (global)", CrstFusionDownload, CRST_REENTRANCY);
    if (!g_csDownload) {
        goto Exit;
    }

    g_csBindLog = ClrCreateCriticalSection("Fusion: Bind Log (global)", CrstFusionLog, CRST_DEFAULT);
    if (!g_csBindLog) {
        goto Exit;
    }

    g_csSingleUse = ClrCreateCriticalSection("Fusion: Single Use (global)", CrstFusionSingleUse, CRST_DEFAULT);
    if (!g_csSingleUse) {
        goto Exit;
    }

    g_csConfigSettings = ClrCreateCriticalSection("Fusion: Config Settings (global)", CrstFusionConfigSettings, CRST_DEFAULT);
    if (!g_csConfigSettings) {
        goto Exit;
    }

    fRet = TRUE;
    
Exit:
    return fRet;
}

// ensure that the symbol will be exported properly
extern "C" HRESULT STDMETHODCALLTYPE InitializeFusion();

HRESULT STDMETHODCALLTYPE InitializeFusion()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY;);
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("InitializeFusion");

    HRESULT             hr = S_OK;
    static BOOL         bInitialized = FALSE;
    LPWSTR              pwzBindingConfigAssemblyStorePath = NULL;
    LPWSTR              pwzBindingConfigDevOverridePath = NULL;
    CNodeFactory        *pNodeFact = NULL;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    
    if (bInitialized) {
        hr = S_OK;
        goto Exit;
    }

    g_hInst = g_pMSCorEE;

    LCID lcid;
    lcid = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
    g_lcid = lcid;


    PAL_GetPALDirectory(g_FusionDllPath, MAX_PATH);
    hr = StringCbCat(g_FusionDllPath, sizeof(g_FusionDllPath), MSCOREE_SHIM_W);
    if (FAILED(hr)) {
        goto Exit;
    }

    if (!InitFusionCriticalSections()) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }


#define _GetConfigDWORD(name, default) GetConfigDWORD(name, default)


    hr = SetRootCachePath(pwzBindingConfigAssemblyStorePath);
    if(FAILED(hr)) {
        goto Exit;
    }

    GetScavengerQuotasFromReg(NULL, NULL, NULL);

    // Machine level logging settings
    g_dwLogInMemory = _GetConfigDWORD(REG_VAL_FUSION_LOG_ENABLE, 0);
    g_dwLogLevel = _GetConfigDWORD(REG_VAL_FUSION_LOG_LEVEL, 1);
    g_dwForceLog = _GetConfigDWORD(REG_VAL_FUSION_LOG_FORCE, 0);
    g_dwLogFailures = _GetConfigDWORD(REG_VAL_FUSION_LOG_FAILURES, 0);

    g_dwLogResourceBinds = _GetConfigDWORD(REG_VAL_FUSION_LOG_RESOURCE_BINDS, 0);
    g_dwConfigForceUnification = REGUTIL::GetConfigDWORD(REG_VAL_FUSION_FORCE_UNIFICATION, 0);

    
    g_pDownloadList = new (nothrow) List<CAssemblyDownload *>;
    if (!g_pDownloadList) {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    bInitialized = TRUE;

Exit:
    SAFEDELETEARRAY(pwzBindingConfigAssemblyStorePath);
    SAFEDELETEARRAY(pwzBindingConfigDevOverridePath);
    SAFERELEASE(pNodeFact);

    END_SO_INTOLERANT_CODE;
    
    return hr;
}

HRESULT SetupDevOverride(LPCWSTR pwzBindingConfigDevOverridePath)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    HRESULT         hr = S_OK;

    g_wzLocalDevOverridePath[0] = L'\0';
    g_wzGlobalDevOverridePath[0] = L'\0';
    g_dwDevOverrideFlags = 0;

    g_dwDevOverrideEnable = _GetConfigDWORD(REG_VAL_DEVOVERRIDE_ENABLE, 0);

    if (g_dwDevOverrideEnable != 0) {
        if (PAL_FetchConfigurationString(TRUE, REG_VAL_DEVOVERRIDE_PATH, g_wzGlobalDevOverridePath, MAX_PATH) &&
            lstrlenW(g_wzGlobalDevOverridePath)) {
            g_dwDevOverrideFlags |= DEVOVERRIDE_GLOBAL;
        }
    }

    return hr;
}


