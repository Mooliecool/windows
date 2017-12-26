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

#include "hostpcymgr.h"
#include "policy.h"
#include "helpers.h"
#include "naming.h"
#include "fusconfig.h"

CHostBindingPolicyManager CHostBindingPolicyManager::_HostPolicyManager;

STDMETHODIMP
CHostBindingPolicyManager::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) 
        return E_POINTER;

    if (   IsEqualIID(riid, IID_IUnknown)
        || IsEqualIID(riid, IID_ICLRHostBindingPolicyManager))
    {
        *ppv = static_cast<ICLRHostBindingPolicyManager*> (this);
        return S_OK;
    }
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
}

ULONG CHostBindingPolicyManager::AddRef() 
{
    return 1;
}

ULONG CHostBindingPolicyManager::Release()
{
    return 1;
}

HRESULT CHostBindingPolicyManager::ModifyApplicationPolicy(
        LPCWSTR     pwzSourceAssemblyIdentity,
        LPCWSTR     pwzTargetAssemblyIdentity,
        BYTE        *pbApplicationPolicy,
        DWORD       cbAppPolicySize,
        DWORD       dwModifyPolicyFlags,
        BYTE        *pbNewApplicationPolicy,
        DWORD       *pcbNewAppPolicySize)
{
    HRESULT         hr = S_OK;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW;) 
        
    IAssemblyName   *pNameSource = NULL;
    IAssemblyName   *pNameTarget = NULL;
    CNodeFactory    *pNodeFact = NULL;

    DWORD           dwCmpFlagsNoVersion = ASM_CMPF_NAME | ASM_CMPF_CULTURE | ASM_CMPF_PUBLIC_KEY_TOKEN | ASM_CMPF_ARCHITECTURE;
    ULONGLONG       ulVer1 = 0;
    ULONGLONG       ulVer2 = 0;

    WORD    wVers[4];
    WCHAR   wzVersion[MAX_VERSION_DISPLAY_SIZE+1];

    WCHAR       wzAsmName[MAX_PATH];
    DWORD       dwSizeName;
    WCHAR       wzAsmVersion[MAX_VERSION_DISPLAY_SIZE+1];
    DWORD       dwSizeVer;
    WCHAR       wzPublicKeyToken[PUBLIC_KEY_TOKEN_DISPLAY_LEN+1];
    DWORD       dwSizePKT;
    WCHAR       wzCulture[MAX_CULTURE_SIZE];
    DWORD       dwSizeCulture;    
    PEKIND      pe = peNone;

    DWORD       dwSize = 0;

    BOOL        bNeedChange = TRUE;

    if (!pwzSourceAssemblyIdentity || !pcbNewAppPolicySize) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if ((!pwzTargetAssemblyIdentity || !pwzTargetAssemblyIdentity[0]) && (dwModifyPolicyFlags != HOST_BINDING_POLICY_MODIFY_REMOVE)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (cbAppPolicySize && !pbApplicationPolicy) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (dwModifyPolicyFlags >= HOST_BINDING_POLICY_MODIFY_MAX) {
        hr = E_INVALIDARG;
        goto Exit;
    }
    
    hr = CreateAssemblyNameObject(&pNameSource, pwzSourceAssemblyIdentity, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }
    
    if (CAssemblyName::IsPartial(pNameSource)) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (dwModifyPolicyFlags != HOST_BINDING_POLICY_MODIFY_REMOVE) {
        // modify case. Need to do some validation
        hr = CreateAssemblyNameObject(&pNameTarget, pwzTargetAssemblyIdentity, CANOF_PARSE_DISPLAY_NAME, NULL);
        if (FAILED(hr)) {
            goto Exit;
        }
        
        if (CAssemblyName::IsPartial(pNameTarget)) {
            hr = E_INVALIDARG;
            goto Exit;
        }

        if (!CAssemblyName::IsStronglyNamed(pNameTarget)) {
            // simply named modification. Nothing. 
            bNeedChange = FALSE;
        }

        hr = pNameSource->IsEqual(pNameTarget, dwCmpFlagsNoVersion);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (hr == S_FALSE) {
            // redirect name/culture/publickeytoken/architecture is not supported. 
            hr = E_INVALIDARG;
            goto Exit;
        }

        if (bNeedChange) {
            // You can redirect from any version to any other higher version, but no lower version. 
            hr = CAssemblyName::GetVersion(pNameSource, TRUE, &ulVer1);
            if (FAILED(hr)) {
                goto Exit;
            }
                    
            hr = CAssemblyName::GetVersion(pNameTarget, TRUE, &ulVer2);
            if (FAILED(hr)) {
                goto Exit;
            }
            
            if (ulVer1 == ulVer2 || !CAssemblyName::IsStronglyNamed(pNameSource)) {
                // same version or simply name assembly? This is a no-op. 
                if (*pcbNewAppPolicySize >= cbAppPolicySize)  {
                    memcpy(pbNewApplicationPolicy, pbApplicationPolicy, cbAppPolicySize);
                }
                else {
                    hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
                }
                *pcbNewAppPolicySize = cbAppPolicySize;
                goto Exit;
            }
            else {
                if (ulVer2 < ulVer1) {
                    // new version lower than old version. 
                    hr = E_INVALIDARG;
                    goto Exit;
                }
            }
        }
    }
    
    if (cbAppPolicySize) {
        hr = ParseXML(&pNodeFact, pbApplicationPolicy, cbAppPolicySize, NULL);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        pNodeFact = NEW(CNodeFactory(NULL));
        if (!pNodeFact) {
            goto Exit;
        }
    }

    if (bNeedChange) {
        if (pNameTarget) {
            for (DWORD i = 0; i < 4; i++) {
                dwSize = sizeof(WORD);
                hr = pNameTarget->GetProperty(ASM_NAME_MAJOR_VERSION+i, (LPBYTE)(&wVers[i]), &dwSize);
                if (FAILED(hr)) {
                    goto Exit;
                }
            }

            hr = StringCchPrintf(wzVersion, ARRAYSIZE(wzVersion), L"%hu.%hu.%hu.%hu", wVers[0], wVers[1], wVers[2], wVers[3]);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

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

        if (dwModifyPolicyFlags == HOST_BINDING_POLICY_MODIFY_REMOVE) {
            hr = pNodeFact->RemovePolicy(wzAsmName, wzAsmVersion, wzCulture[0]?wzCulture:NULL, wzPublicKeyToken, pe);
        }
        else {
            hr = pNodeFact->AddPolicy(wzAsmName, wzAsmVersion, wzCulture[0]?wzCulture:NULL, wzPublicKeyToken, pe, wzVersion, dwModifyPolicyFlags);
        }
        if (FAILED(hr)) {
            goto Exit;
        
        }
    }

    hr = pNodeFact->OutputToUTF8(pcbNewAppPolicySize, pbNewApplicationPolicy);
    if (FAILED(hr)) {
        goto Exit;
    }

Exit:
    SAFERELEASE(pNameSource);
    SAFERELEASE(pNameTarget);
    SAFERELEASE(pNodeFact);

    END_SO_INTOLERANT_CODE;
    
    return hr;
}

#define MACHINE_CONFIG_SUFFIX   L"config\\machine.config"

HRESULT CHostBindingPolicyManager::EvaluatePolicy(
        LPCWSTR pwzReferenceIdentity,
        BYTE    *pbApplicationPolicy,
        DWORD   cbAppPolicySize, 
        __out_ecount_opt(*pcchPostPolicyReferenceIdentity) LPWSTR  pwzPostPolicyReferenceIdentity,
        __inout DWORD   *pcchPostPolicyReferenceIdentity,
        DWORD   *pdwPoliciesApplied)
{      
    HRESULT hr = S_OK;

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW;) 
            
    IAssemblyName *pNameRef = NULL;
    IAssemblyName *pNamePolicy = NULL;
    DWORD       dwSize = 0;
    DWORD       dwDispFlags = ASM_DISPLAYF_FULL;
    CAppCtxPolicyConfigs *pAppCtxConfig = NULL;
    CPolicyConfig *pAppConfig = NULL;
    CPolicyConfig *pAdminConfig = NULL;
    LPWSTR      pwzAdminConfig = NULL;

    if (!pwzReferenceIdentity || !pcchPostPolicyReferenceIdentity) {
        hr = E_INVALIDARG;
        goto Exit;
    }

    if (pdwPoliciesApplied) {
        *pdwPoliciesApplied = ePolicyLevelNone;
    }

    hr = CreateAssemblyNameObject(&pNameRef, pwzReferenceIdentity, CANOF_PARSE_DISPLAY_NAME, NULL);
    if (FAILED(hr)) {
        goto Exit;
    }

    // don't do anything for simply named assembly 
    if (CAssemblyName::IsStronglyNamed(pNameRef)) {
        if (CAssemblyName::IsPartial(pNameRef)) {
            hr = E_INVALIDARG;
            goto Exit;
        }

        if (cbAppPolicySize && pbApplicationPolicy) {
            hr = CPolicyConfig::Create(pbApplicationPolicy, cbAppPolicySize, ePolicyLevelApp, NULL, &pAppConfig);
            if (FAILED(hr)) {
                goto Exit;
            }
        }

        pwzAdminConfig = NEW(WCHAR[MAX_PATH]);
        if (!pwzAdminConfig) {
            hr = E_OUTOFMEMORY;
            goto Exit;
        }

        hr = GetCORSystemDirectory(pwzAdminConfig, MAX_PATH, &dwSize);
        if (FAILED(hr)) {
            goto Exit;
        }

        if (dwSize + ARRAYSIZE(MACHINE_CONFIG_SUFFIX) >= MAX_PATH) {
            // machine.config's path longer than MAX_PATH
            // We can handle this, but there is no point now. 
            hr = E_UNEXPECTED;
            goto Exit;
        }

        hr = StringCchCat(pwzAdminConfig, MAX_PATH, MACHINE_CONFIG_SUFFIX);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = CPolicyConfig::Create(pwzAdminConfig, ePolicyLevelAdmin, NULL, &pAdminConfig);
        if (FAILED(hr)) {
            if (hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) && hr != HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)) {
                goto Exit;
            }
        }

        hr = S_OK;

        hr = CAppCtxPolicyConfigs::Create(&pAppCtxConfig);
        if (FAILED(hr)) {
            goto Exit;
        }
               
        if (pAppConfig) {
            pAppCtxConfig->SetPolicyConfig(ePolicyLevelApp, pAppConfig);
        }

        if (pAdminConfig) {
            pAppCtxConfig->SetPolicyConfig(ePolicyLevelAdmin, pAdminConfig);
        }

        hr = pAppCtxConfig->GetPolicyInfo(pNameRef, FALSE, FALSE, TRUE, FALSE, NULL, NULL, &pNamePolicy, NULL, NULL, pdwPoliciesApplied);
        if (FAILED(hr)) {
            goto Exit;
        }

        hr = pNamePolicy->GetDisplayName(pwzPostPolicyReferenceIdentity, pcchPostPolicyReferenceIdentity, dwDispFlags);
        goto Exit;
    }

    // if we are here, either there is no policy, or policy does not apply. 
    hr = S_OK;
    dwSize = lstrlenW(pwzReferenceIdentity) + 1;
    if (*pcchPostPolicyReferenceIdentity >= dwSize) {
        hr = StringCchCopy(pwzPostPolicyReferenceIdentity, *pcchPostPolicyReferenceIdentity, pwzReferenceIdentity);
        if (FAILED(hr)) {
            goto Exit;
        }
    }
    else {
        hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }
    *pcchPostPolicyReferenceIdentity = dwSize;
        
Exit:
    if (SUCCEEDED(hr) && pwzPostPolicyReferenceIdentity) {
        hr = CanonicalizeIdentity(pwzPostPolicyReferenceIdentity);
    }
    SAFERELEASE(pNameRef);
    SAFERELEASE(pNamePolicy);
    SAFERELEASE(pAppConfig);
    SAFERELEASE(pAdminConfig);
    SAFERELEASE(pAppCtxConfig);
    SAFEDELETEARRAY(pwzAdminConfig);
    
    END_SO_INTOLERANT_CODE;

    return hr;
}
