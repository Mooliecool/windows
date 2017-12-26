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

#ifndef __FUSION_CONFIG_H__
#define __FUSION_CONFIG_H__

#include "list.h"
#include "lock.h"
#include "naming.h"
#include "helpers.h"

#define CONFIG_HASH_TABLE_SIZE      17

class CNodeFactory;
class CDebugLog;

// config other than publisher policy
class CPolicyConfig
{
friend class CPolicyConfigPool;

public:
    CPolicyConfig();
    ~CPolicyConfig();

    static HRESULT Create(LPCWSTR pwzConfigFile, 
                          DWORD dwWhichConfig,
                          CDebugLog *pdbglog, 
                          CPolicyConfig **ppConfig);

    static HRESULT Create(BYTE *pbAppConfigBlob, 
                          DWORD cbAppConfigBlob,
                          DWORD dwWhichConfig,
                          CDebugLog *pdbglog, 
                          CPolicyConfig **ppConfig);

    ULONG AddRef();
    ULONG Release();

    BOOL IsEqual(CPolicyConfig *pConfig)
    {
        if (pConfig == this) {
            return TRUE;
        }
        else {
            return IsEqual(pConfig->_pwzConfigFile, &(pConfig->_ftLastMod));
        }
    }

    BOOL IsEqual(LPCWSTR pwzConfigFile, 
                FILETIME *pftLastMod);

    HRESULT GetPolicyVersion(LPCWSTR wzAssemblyName, 
                             LPCWSTR wzPublicKeyToken,
                             LPCWSTR wzCulture, 
                             LPCWSTR wzVersionIn,
                             PEKIND  peIn, 
                             __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                             __inout LPDWORD pdwSizeVer,
                             PEKIND *peOut)
    {
        HRESULT hr = S_OK;
        hr = _pNodeFact->GetPolicyVersion(wzAssemblyName, 
                                          wzPublicKeyToken,
                                          wzCulture,
                                          wzVersionIn,
                                          peIn, 
                                          pwzVersionOut,
                                          pdwSizeVer,
                                          peOut);

        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
        }

        return hr;
                                            
    }

    HRESULT GetSafeMode(LPCWSTR wzAssemblyName, 
                        LPCWSTR wzPublicKeyToken,
                        LPCWSTR wzCulture, 
                        LPCWSTR wzVersionIn,
                        PEKIND  pe,
                        BOOL *pbSafeMode)
    {
        return _pNodeFact->GetSafeMode(wzAssemblyName,
                                       wzPublicKeyToken,
                                       wzCulture,
                                       wzVersionIn,
                                       pe,
                                       pbSafeMode); 
    }

    HRESULT GetCodebaseHint(LPCWSTR pwzAsmName, 
                            LPCWSTR pwzVersion,
                            LPCWSTR pwzPublicKeyToken, 
                            LPCWSTR pwzCulture,
                            PEKIND  pe,
                            LPCWSTR pwzAppBase, 
                            __deref_out LPWSTR *ppwzCodebase)
    {
        return _pNodeFact->GetCodebaseHint(pwzAsmName, 
                                           pwzVersion,
                                           pwzPublicKeyToken, 
                                           pwzCulture,
                                           pe,
                                           pwzAppBase, 
                                           ppwzCodebase);
    }

    HRESULT GetPrivatePath(__deref_out LPWSTR *ppwzPrivatePath)
    {
        return _pNodeFact->GetPrivatePath(ppwzPrivatePath);
    }
    
    HRESULT QualifyAssembly(LPCWSTR pwzDisplayName, 
                            IAssemblyName **ppNameQualified, 
                            CDebugLog *pdbglog)
    {
        return _pNodeFact->QualifyAssembly(pwzDisplayName,
                                           ppNameQualified,
                                           pdbglog); 
    }

    HRESULT HasFrameworkRedirect(BOOL *pbHasFrameworkRedirect) 
    {
        return _pNodeFact->HasFrameworkRedirect(pbHasFrameworkRedirect);
    }

private:

    HRESULT Init(LPCWSTR pwzConfigFile, 
                 FILETIME *pftLastMod, 
                 DWORD dwWhichConfig,
                 CDebugLog *pdbglog);

    HRESULT Init(BYTE *pbAppConfigBlob, 
                 DWORD cbAppConfigBlob, 
                 DWORD dwWhichConfig,
                 CDebugLog *pdbglog);

private:
    LONG            _cRef;
    LPWSTR          _pwzConfigFile;
    FILETIME        _ftLastMod;
    CNodeFactory   *_pNodeFact;
    BOOL            _bInConfigPool;
};

#define REG_KEY_PUBLISHER_POLICY_CACHE  L"Software\\Microsoft\\Fusion\\PublisherPolicy"
#define REG_VAL_PUBLISHER_POLICY_LATEST         L"Latest"
#define REG_VAL_PUBLISHER_POLICY_OLDEST         L"Oldest"
#define REG_VAL_PUBLISHER_POLICY_USAGEMASK      L"UsageMask"
#define REG_VAL_PUBLISHER_POLICY_LEGACYPOLICYTME L"LegacyPolicyTimeStamp"
#define REG_VAL_PUBLISHER_POLICY_CONFIG_FILE    L"ConfigFilePath"

#define PUBLISHER_POLICY_READ_RETRY                 10
#define PUBLISHER_POLICY_READ_RETRY_WAITING_TIME    30

class CPublisherPolicy
{
public:
    CPublisherPolicy();
    ~CPublisherPolicy();

    ULONG AddRef();
    ULONG Release();
    
    static HRESULT Create(BOOL bForBind, CPublisherPolicy **ppPolicy);

    BOOL IsEqual(CPublisherPolicy *pPubPolicy)
    {
        if (!_isValid || !pPubPolicy->_isValid) {
            return FALSE;
        }

        return _version == pPubPolicy->_version;
    }
    
    HRESULT GetPolicyVersion(CDebugLog *pdbglog,
                             LPCWSTR wzAssemblyName, 
                             LPCWSTR wzPublicKeyToken,
                             LPCWSTR wzCulture, 
                             LPCWSTR wzVersionIn,
                             PEKIND  peIn,
                             __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                             __inout LPDWORD pdwSizeVer,
                             __deref_out LPWSTR *ppwzCodebaseHint,
                             PEKIND *peOut)
    {
            return GetPolicyVersionFromDisk(pdbglog, wzAssemblyName,
                                    wzPublicKeyToken, wzCulture, wzVersionIn, peIn,
                                    pwzVersionOut, pdwSizeVer, ppwzCodebaseHint, peOut);
    }


private:
    HRESULT Init(BOOL bForBind);

    HRESULT GetPolicyVersionFromDisk(
                             CDebugLog *pdbglog,
                             LPCWSTR wzAssemblyName, 
                             LPCWSTR wzPublicKeyToken,
                             LPCWSTR wzCulture, 
                             LPCWSTR wzVersionIn,
                             PEKIND  peIn,
                             __out_ecount(*pdwSizeVer) LPWSTR  pwzVersionOut,
                             __inout LPDWORD pdwSizeVer,
                             __deref_out LPWSTR *ppwzCodebaseHint,
                             PEKIND *peOut);   
private:
    LONG            _cRef;
    DWORD           _version;
    BOOL            _isValid;
    FILETIME        _ftLegacyPubPol;    // time stamp of legacy publisher policy
};

class CPolicyConfigPool
{
public:    
    CPolicyConfigPool() {};
    ~CPolicyConfigPool();

    static HRESULT Create(CPolicyConfigPool **ppConfigPool);

    HRESULT CreatePolicyConfig( LPCWSTR pwzConfigFile, FILETIME *pftLastMod, 
                                DWORD dwWhichConfig, CDebugLog *pdbglog, CPolicyConfig **ppConfig);
    BOOL RemoveConfig(CPolicyConfig *pConfig);

    void Lock() { ClrEnterCriticalSection(_cs); }
    void Unlock() { ClrLeaveCriticalSection(_cs); }

private:
    HRESULT Init();

private:
    CRITSEC_COOKIE        _cs;
    List<CPolicyConfig *> _listConfigs[CONFIG_HASH_TABLE_SIZE];
};

// aggregation of all the policy configs in an app ctx
class CAppCtxPolicyConfigs
{
public:
    CAppCtxPolicyConfigs();
    ~CAppCtxPolicyConfigs();

    ULONG AddRef();
    ULONG Release();

    static HRESULT Create(CAppCtxPolicyConfigs **ppConfig);
    
    void SetPolicyConfig(DWORD dwWhichConfig, CPolicyConfig *pConfig);
    HRESULT GetPolicyConfig(DWORD dwWhichConfig, CPolicyConfig **ppConfig);

    BOOL IsEqual(CAppCtxPolicyConfigs *pAppConfig);

    HRESULT GetPolicyInfo(IAssemblyName *pNameSource, 
                          BOOL bDisallowAppBindingRedirects,
                          BOOL bDisallowApplyPublisherPolicy,  
                          BOOL bUnifyFx,
                          BOOL bForceUnifyFX,
                          LPCWSTR pwzAppBase,
                          CDebugLog *pdbglog,
                          IAssemblyName **ppNamePolicy,
                          __deref_opt_out_opt LPWSTR *ppwzPolicyCodebase,
                          AsmBindHistoryInfo **ppHistInfo,
                          DWORD *pdwPolicyApplied);

    HRESULT QualifyAssembly(LPCWSTR pwzDisplayName, 
                            IAssemblyName **ppNameQualified, 
                            CDebugLog *pdbglog)
    {
        if (!_pAppConfig) {
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
        else {
            return _pAppConfig->QualifyAssembly(pwzDisplayName,
                                                ppNameQualified,
                                                pdbglog); 
        }
    }

    HRESULT GetAppCfgCodebaseHint(IAssemblyName *pName, 
                                  LPCWSTR pwzAppBase,
                                  __deref_out LPWSTR *ppwzCodebaseHint);

    void SetHasFrameworkRedirect(BOOL bHasRedirect) {_bHasFrameworkRedirect = bHasRedirect;}
    BOOL HasFrameworkRedirect() {return _bHasFrameworkRedirect;}

private:
    HRESULT Init();

private:
    LONG                 _cRef;
    CPolicyConfig       *_pAppConfig;
    CPolicyConfig       *_pHostConfig;
    CPolicyConfig       *_pAdminConfig;
    CPublisherPolicy    *_pPubPolicy;
    BOOL                 _bHasFrameworkRedirect;
    // And two implicit configs: retarget/fx.
};

extern CPolicyConfigPool *g_pConfigPool;

HRESULT InitializeConfigPool();

#endif
