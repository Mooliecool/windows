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
/*============================================================
**
** Header: FusionBind.cpp
**
** Purpose: Implements fusion interface
**
** Date:  Dec 1, 1998
**
===========================================================*/

#include "common.h"
#include <stdlib.h>
#include "fusionbind.h"
#include "shimload.h"
#include "timeline.h"
#include "memoryreport.h"
#include "eventtrace.h"

void CodeBaseInfo::ReleaseParent()
{
    WRAPPER_CONTRACT;
    if (m_pParentAssembly) {
        m_pParentAssembly->Release();
        m_pParentAssembly = NULL;
    }
}

FusionBind::~FusionBind()
{
    WRAPPER_CONTRACT;
    if (m_ownedFlags & NAME_OWNED)
        delete [] m_pAssemblyName;
    if (m_ownedFlags & PUBLIC_KEY_OR_TOKEN_OWNED)
        delete [] m_pbPublicKeyOrToken;
    if (m_ownedFlags & CODE_BASE_OWNED)
        delete [] m_CodeInfo.m_pszCodeBase;
    if (m_ownedFlags & LOCALE_OWNED)
        delete [] m_context.szLocale;
}

HRESULT FusionBind::Init(LPCSTR pAssemblyName,
                         AssemblyMetaDataInternal* pContext, 
                         PBYTE pbPublicKeyOrToken, DWORD cbPublicKeyOrToken,
                         DWORD dwFlags)
{
    LEAF_CONTRACT;
    _ASSERTE(pContext);

    m_pAssemblyName = pAssemblyName;
    m_pbPublicKeyOrToken = pbPublicKeyOrToken;
    m_cbPublicKeyOrToken = cbPublicKeyOrToken;
    m_dwFlags = dwFlags;
    m_ownedFlags = 0;

    m_context = *pContext;

    return S_OK;
}

HRESULT FusionBind::Init(LPCSTR pAssemblyDisplayName)
{
    LEAF_CONTRACT;
    m_pAssemblyName = pAssemblyDisplayName;
    // We eagerly parse the name to allow FusionBind::Hash to avoid throwing.
    return ParseName();
}

HRESULT FusionBind::CloneFields(int ownedFlags)
{
    WRAPPER_CONTRACT;
#if _DEBUG
    DWORD hash = Hash();
#endif

    if ((~m_ownedFlags & NAME_OWNED) && (ownedFlags & NAME_OWNED) &&
        m_pAssemblyName) {
        size_t len = strlen(m_pAssemblyName) + 1;
        LPSTR temp = new (nothrow) char [len];
        if (temp == NULL)
            return E_OUTOFMEMORY;
        strcpy_s(temp, len, m_pAssemblyName);
        m_pAssemblyName = temp;
        m_ownedFlags |= NAME_OWNED;
    }

    if ((~m_ownedFlags & PUBLIC_KEY_OR_TOKEN_OWNED) && 
        (ownedFlags & PUBLIC_KEY_OR_TOKEN_OWNED) && m_pbPublicKeyOrToken) {
        BYTE *temp = new (nothrow) BYTE [m_cbPublicKeyOrToken];
        if (temp == NULL)
            return E_OUTOFMEMORY;
        memcpy(temp, m_pbPublicKeyOrToken, m_cbPublicKeyOrToken);
        m_pbPublicKeyOrToken = temp;
        m_ownedFlags |= PUBLIC_KEY_OR_TOKEN_OWNED;
    }

    if ((~m_ownedFlags & CODE_BASE_OWNED) && 
        (ownedFlags & CODE_BASE_OWNED) && m_CodeInfo.m_dwCodeBase > 0) {
        LPWSTR temp = new (nothrow) WCHAR [m_CodeInfo.m_dwCodeBase];
        if (temp == NULL)
            return E_OUTOFMEMORY;
        wcscpy_s(temp, m_CodeInfo.m_dwCodeBase, m_CodeInfo.m_pszCodeBase);
        m_CodeInfo.m_pszCodeBase = temp;
        m_ownedFlags |= CODE_BASE_OWNED;
    }

    if ((~m_ownedFlags & LOCALE_OWNED) && (ownedFlags & LOCALE_OWNED) &&
        m_context.szLocale) {
        size_t len = strlen(m_context.szLocale) + 1;
        LPSTR temp = new (nothrow) char [len];
        if (temp == NULL)
            return E_OUTOFMEMORY;
        strcpy_s(temp, len, m_context.szLocale);
        m_context.szLocale = temp;
        m_ownedFlags |= LOCALE_OWNED;
    }

    _ASSERTE(hash == Hash());

    return S_OK;
}

VOID FusionBind::CloneFieldsToLoaderHeap(int ownedFlags, LoaderHeap *pHeap, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END

#if _DEBUG
    DWORD hash = Hash();
#endif

    if ((~m_ownedFlags & NAME_OWNED) && (ownedFlags & NAME_OWNED) &&
        m_pAssemblyName) {
        size_t len = strlen(m_pAssemblyName) + 1;
        LPSTR temp = (LPSTR)pamTracker->Track( pHeap->AllocMem(len) );
        strcpy_s(temp, len, m_pAssemblyName);
        m_pAssemblyName = temp;
    }

    if ((~m_ownedFlags & PUBLIC_KEY_OR_TOKEN_OWNED) && 
        (ownedFlags & PUBLIC_KEY_OR_TOKEN_OWNED) && m_pbPublicKeyOrToken && m_cbPublicKeyOrToken > 0) {
        BYTE *temp = (BYTE *)pamTracker->Track( pHeap->AllocMem(m_cbPublicKeyOrToken) );
        memcpy(temp, m_pbPublicKeyOrToken, m_cbPublicKeyOrToken);
        m_pbPublicKeyOrToken = temp;
    }

    if ((~m_ownedFlags & CODE_BASE_OWNED) && 
        (ownedFlags & CODE_BASE_OWNED) && m_CodeInfo.m_dwCodeBase > 0) {
        LPWSTR temp = (WCHAR *)pamTracker->Track( pHeap->AllocMem(m_CodeInfo.m_dwCodeBase * sizeof(WCHAR)) );
        wcscpy_s(temp, m_CodeInfo.m_dwCodeBase, m_CodeInfo.m_pszCodeBase);
        m_CodeInfo.m_pszCodeBase = temp;
    }

    if ((~m_ownedFlags & LOCALE_OWNED) && (ownedFlags & LOCALE_OWNED) &&
        m_context.szLocale) {
        size_t len = strlen(m_context.szLocale) + 1;
        LPSTR temp = (char *)pamTracker->Track( pHeap->AllocMem(len) );
        strcpy_s(temp, len, m_context.szLocale);
        m_context.szLocale = temp;
    }

    _ASSERTE(hash == Hash());

}

HRESULT FusionBind::Init(IAssemblyName *pName)
{
    WRAPPER_CONTRACT;
    _ASSERTE(pName);

    HRESULT hr;
   
    // Fill out info from name, if we have it.

    DWORD cbSize = 0;
    hr=pName->GetProperty(ASM_NAME_NAME, NULL, &cbSize);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        hr=S_OK;
        CQuickBytes qb;
        LPWSTR pwName = (LPWSTR) qb.AllocNoThrow(cbSize);
        if (!pwName)
            return E_OUTOFMEMORY;

        IfFailRet(pName->GetProperty(ASM_NAME_NAME, pwName, &cbSize));

        cbSize = WszWideCharToMultiByte(CP_UTF8, 0, pwName, -1, NULL, 0, NULL, NULL);
        if(cbSize == 0)
            IfFailRet(HRESULT_FROM_GetLastError());

        m_pAssemblyName = new (nothrow) char[cbSize];
        if (!m_pAssemblyName)
            return E_OUTOFMEMORY;

        m_ownedFlags |= NAME_OWNED;
        cbSize=WszWideCharToMultiByte(CP_UTF8, 0, pwName, -1, (LPSTR) m_pAssemblyName, cbSize, NULL, NULL);
        if(cbSize == 0)
            IfFailRet(HRESULT_FROM_GetLastError());

    }
    IfFailRet(hr);

    // Note: cascade checks so we don't set lower priority version #'s if higher ones are missing
    cbSize = sizeof(m_context.usMajorVersion);
    hr=pName->GetProperty(ASM_NAME_MAJOR_VERSION, &m_context.usMajorVersion, &cbSize);

    if (hr!=S_OK || !cbSize)
        m_context.usMajorVersion = (USHORT) -1;
    else {
        cbSize = sizeof(m_context.usMinorVersion);
        hr=pName->GetProperty(ASM_NAME_MINOR_VERSION, &m_context.usMinorVersion, &cbSize);
    }

    if (hr!=S_OK || !cbSize)
        m_context.usMinorVersion = (USHORT) -1;
    else {
        cbSize = sizeof(m_context.usBuildNumber);
        pName->GetProperty(ASM_NAME_BUILD_NUMBER, &m_context.usBuildNumber, &cbSize);
    }

    if (hr!=S_OK || !cbSize)
        m_context.usBuildNumber = (USHORT) -1;
    else {
        cbSize = sizeof(m_context.usRevisionNumber);
        pName->GetProperty(ASM_NAME_REVISION_NUMBER, &m_context.usRevisionNumber, &cbSize);
    }

    if (hr!=S_OK || !cbSize)
        m_context.usRevisionNumber = (USHORT) -1;

    if (hr==E_INVALIDARG)
        hr=S_FALSE;

    IfFailRet(hr);

    cbSize = 0;
    hr=pName->GetProperty(ASM_NAME_CULTURE, NULL, &cbSize);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        hr=S_OK;
        LPWSTR pwName = (LPWSTR) alloca(cbSize);
        IfFailRet(pName->GetProperty(ASM_NAME_CULTURE, pwName, &cbSize));

        cbSize = WszWideCharToMultiByte(CP_UTF8, 0, pwName, -1, NULL, 0, NULL, NULL);
        if(cbSize == 0)
            IfFailRet(HRESULT_FROM_GetLastError());

        m_context.szLocale = new (nothrow) char [cbSize];
        if (!m_context.szLocale)
            return E_OUTOFMEMORY;
        m_ownedFlags |= LOCALE_OWNED;
        cbSize=WszWideCharToMultiByte(CP_UTF8, 0, pwName, -1, (LPSTR) m_context.szLocale, cbSize, NULL, NULL);        
        if(cbSize == 0)
            IfFailRet(HRESULT_FROM_GetLastError());
    }

    IfFailRet(hr);

    m_dwFlags = 0;

    cbSize = 0;
    hr=pName->GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, NULL, &cbSize);
    if (hr== HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        m_pbPublicKeyOrToken = new (nothrow) BYTE[cbSize];
        if (m_pbPublicKeyOrToken == NULL)
            return E_OUTOFMEMORY;
        m_cbPublicKeyOrToken = cbSize;
        m_ownedFlags |= PUBLIC_KEY_OR_TOKEN_OWNED;           
        IfFailRet(pName->GetProperty(ASM_NAME_PUBLIC_KEY_TOKEN, m_pbPublicKeyOrToken, &cbSize));
    }
    else {
        if (hr!=E_INVALIDARG)
            IfFailRet(hr);
        hr=pName->GetProperty(ASM_NAME_PUBLIC_KEY, NULL, &cbSize);
        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
            hr=S_OK;
            m_pbPublicKeyOrToken = new (nothrow) BYTE[cbSize];
            if (m_pbPublicKeyOrToken == NULL)
                return E_OUTOFMEMORY;
            m_cbPublicKeyOrToken = cbSize;
            m_dwFlags |= afPublicKey;
            m_ownedFlags |= PUBLIC_KEY_OR_TOKEN_OWNED;           
            IfFailRet(pName->GetProperty(ASM_NAME_PUBLIC_KEY, m_pbPublicKeyOrToken, &cbSize));
        }
        else {
            IfFailRet(hr);
            hr= pName->GetProperty(ASM_NAME_NULL_PUBLIC_KEY, NULL, &cbSize);
            if (hr!=S_OK)
                hr=pName->GetProperty(ASM_NAME_NULL_PUBLIC_KEY_TOKEN, NULL, &cbSize);
            if ( hr == S_OK ) {
                m_pbPublicKeyOrToken = new (nothrow) BYTE[0];
                if (m_pbPublicKeyOrToken == NULL)
                    return E_OUTOFMEMORY;
                m_cbPublicKeyOrToken = 0;
                m_ownedFlags |= PUBLIC_KEY_OR_TOKEN_OWNED;           
            }
            if (hr==E_INVALIDARG)
                hr=S_FALSE;
            IfFailRet(hr);
            
        }
    }

    // Recover the afRetargetable flag
    BOOL bRetarget;
    cbSize = sizeof(bRetarget);
    hr = pName->GetProperty(ASM_NAME_RETARGET, &bRetarget, &cbSize);
    if (hr == S_OK && cbSize != 0 && bRetarget)
        m_dwFlags |= afRetargetable;

    // Recover the Processor Architecture flags
    PEKIND peKind;
    cbSize = sizeof(PEKIND);
    hr = pName->GetProperty(ASM_NAME_ARCHITECTURE, &peKind, &cbSize);
    if ((hr == S_OK) && (cbSize != 0) && (peKind <= 4) && (peKind >= 1))
        m_dwFlags |= (((DWORD)peKind) << 4);

    cbSize = 0;
    hr=pName->GetProperty(ASM_NAME_CODEBASE_URL, NULL, &cbSize);
    if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) {
        m_CodeInfo.m_pszCodeBase = new (nothrow) WCHAR [ cbSize/sizeof(WCHAR) ];
        if (m_CodeInfo.m_pszCodeBase == NULL)
            return E_OUTOFMEMORY;
        m_CodeInfo.m_dwCodeBase = cbSize/sizeof(WCHAR);
        m_ownedFlags |= CODE_BASE_OWNED;
        IfFailRet(pName->GetProperty(ASM_NAME_CODEBASE_URL, 
                                    (void*)m_CodeInfo.m_pszCodeBase, &cbSize));
    }
    else
        IfFailRet(hr);

    return S_OK;
}

HRESULT FusionBind::Init(FusionBind *pSpec, BOOL bClone/*=TRUE*/)
{
    WRAPPER_CONTRACT;
    m_CodeInfo.m_pszCodeBase = pSpec->m_CodeInfo.m_pszCodeBase;
    m_CodeInfo.m_dwCodeBase = pSpec->m_CodeInfo.m_dwCodeBase;

    {
        GCX_PREEMP();
        m_CodeInfo.SetParentAssembly(pSpec->m_CodeInfo.GetParentAssembly());
    }

    HRESULT hr;
    hr = Init(pSpec->m_pAssemblyName, 
              &pSpec->m_context,
              pSpec->m_pbPublicKeyOrToken, 
              pSpec->m_cbPublicKeyOrToken,
              pSpec->m_dwFlags);

    if (SUCCEEDED(hr) && bClone)
    {
        CONTRACT_VIOLATION(GCViolation);
        hr = CloneFields(pSpec->m_ownedFlags);
        //
        //
        //_ASSERTE(CompareEx(pSpec));
    }

    return hr;
}

HRESULT FusionBind::ParseName()
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        GC_NOTRIGGER;
        NOTHROW;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    if (!m_pAssemblyName)
        return S_OK;

    TIMELINE_START(FUSIONBIND, ("ParseName %s", m_pAssemblyName));

    LPWSTR pwName;
    NonVMComHolder<IAssemblyName> pName;

    CQuickBytes qb;
    long pwNameLen = WszMultiByteToWideChar(CP_UTF8, 0, m_pAssemblyName, -1, 0, 0);

    HRESULT hr;
    if(pwNameLen == 0)
        IfFailGo(HRESULT_FROM_GetLastError());

    pwName = (LPWSTR) qb.AllocNoThrow(pwNameLen*sizeof(WCHAR));
    if (!pwName)
        IfFailGo(E_OUTOFMEMORY);

    if (!WszMultiByteToWideChar(CP_UTF8, 0, m_pAssemblyName, -1, pwName, pwNameLen))
        IfFailGo(HRESULT_FROM_GetLastError());

    IfFailGo(CreateAssemblyNameObject(&pName, pwName, CANOF_PARSE_DISPLAY_NAME, NULL));

    if (m_ownedFlags & NAME_OWNED)
        delete [] m_pAssemblyName;
    m_pAssemblyName = NULL;

    hr = Init(pName);

 ErrExit:
    TIMELINE_END(FUSIONBIND, ("ParseName %s", m_pAssemblyName));
    return hr;
}


void FusionBind::SetCodeBase(LPCWSTR szCodeBase, DWORD dwCodeBase)
{
    LEAF_CONTRACT;
    _ASSERTE(szCodeBase == 0 || wcslen(szCodeBase) + 1 == dwCodeBase);     // length includes terminator 
    m_CodeInfo.m_pszCodeBase = szCodeBase;
    m_CodeInfo.m_dwCodeBase = dwCodeBase;
}

DWORD FusionBind::Hash()
{
    CONTRACTL {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    // Hash fields.
    DWORD hash = 0;

    if (m_pAssemblyName)
        hash ^= HashStringA(m_pAssemblyName);
    hash = _rotl(hash, 4);

    hash ^= HashBytes(m_pbPublicKeyOrToken, m_cbPublicKeyOrToken);
    hash = _rotl(hash, 4);
        
    hash ^= m_dwFlags;
    hash = _rotl(hash, 4);

    if (m_CodeInfo.m_pszCodeBase)
        hash ^= HashString(m_CodeInfo.m_pszCodeBase);
    hash = _rotl(hash, 4);

    hash ^= m_context.usMajorVersion;
    hash = _rotl(hash, 8);

    if (m_context.usMajorVersion != (USHORT) -1) {
        hash ^= m_context.usMinorVersion;
        hash = _rotl(hash, 8);
        
        if (m_context.usMinorVersion != (USHORT) -1) {
            hash ^= m_context.usBuildNumber;
            hash = _rotl(hash, 8);
        
            if (m_context.usBuildNumber != (USHORT) -1) {
                hash ^= m_context.usRevisionNumber;
                hash = _rotl(hash, 8);
            }
        }
    }

    if (m_context.szLocale)
        hash ^= HashStringA(m_context.szLocale);
    hash = _rotl(hash, 4);

    hash ^= (m_CodeInfo.m_fParentLoadContext == LOADCTX_TYPE_LOADFROM);

    return hash;
}


BOOL FusionBind::CompareEx(FusionBind *pSpec)
{
    WRAPPER_CONTRACT;

    // Compare fields
    BOOL fIsInLoadFromContext = (m_CodeInfo.m_fParentLoadContext == LOADCTX_TYPE_LOADFROM);
    BOOL fSpecIsInLoadFromContext = (pSpec->m_CodeInfo.m_fParentLoadContext == LOADCTX_TYPE_LOADFROM);
    if (fIsInLoadFromContext != fSpecIsInLoadFromContext)
        return FALSE;

    if (m_pAssemblyName != pSpec->m_pAssemblyName
        && (m_pAssemblyName == NULL || pSpec->m_pAssemblyName == NULL
            || strcmp(m_pAssemblyName, pSpec->m_pAssemblyName)))
        return FALSE;

    if (m_cbPublicKeyOrToken != pSpec->m_cbPublicKeyOrToken
        || memcmp(m_pbPublicKeyOrToken, pSpec->m_pbPublicKeyOrToken, m_cbPublicKeyOrToken))
        return FALSE;

    if (m_dwFlags != pSpec->m_dwFlags)
        return FALSE;

    if (m_CodeInfo.m_pszCodeBase != pSpec->m_CodeInfo.m_pszCodeBase
        && (m_CodeInfo.m_pszCodeBase == NULL || pSpec->m_CodeInfo.m_pszCodeBase == NULL
            || wcscmp(m_CodeInfo.m_pszCodeBase, pSpec->m_CodeInfo.m_pszCodeBase)))
        return FALSE;

    if (m_context.usMajorVersion != pSpec->m_context.usMajorVersion)
        return FALSE;

    if (m_context.usMajorVersion != (USHORT) -1) {
        if (m_context.usMinorVersion != pSpec->m_context.usMinorVersion)
            return FALSE;

        if (m_context.usMinorVersion != (USHORT) -1) {
            if (m_context.usBuildNumber != pSpec->m_context.usBuildNumber)
                return FALSE;
            
            if (m_context.usBuildNumber != (USHORT) -1) {
                if (m_context.usRevisionNumber != pSpec->m_context.usRevisionNumber)
                    return FALSE;
            }
        }
    }

    if (m_context.szLocale != pSpec->m_context.szLocale
        && (m_context.szLocale == NULL || pSpec->m_context.szLocale == NULL
            || strcmp(m_context.szLocale, pSpec->m_context.szLocale)))
        return FALSE;

    // Post-policy load-neither binds can be picked up by nobody except their own parent assembly.
    if (m_CodeInfo.m_fParentLoadContext == LOADCTX_TYPE_UNKNOWN
        && pSpec->m_CodeInfo.m_fParentLoadContext == LOADCTX_TYPE_UNKNOWN
        && m_CodeInfo.GetParentAssembly() != pSpec->m_CodeInfo.GetParentAssembly())
        return FALSE;

    return TRUE;
}

void FusionBind::GetFileOrDisplayName(DWORD flags, SString &result)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        INJECT_FAULT(ThrowOutOfMemory());
        PRECONDITION(CheckValue(result));
        PRECONDITION(result.IsEmpty());
    }
    CONTRACTL_END;

    if (m_pAssemblyName != NULL) {
        NonVMComHolder<IAssemblyName> pFusionName;
        IfFailThrow(CreateFusionName(&pFusionName));

        GetAssemblyNameDisplayName(pFusionName, result, flags);
    }
    else
        result.Set(m_CodeInfo.m_pszCodeBase);
}

HRESULT FusionBind::CreateFusionName(IAssemblyName **ppName,
                                     BOOL fIncludeCodeBase/*=TRUE*/)
{
    WRAPPER_CONTRACT;

    TIMELINE_AUTO(FUSIONBIND, ("CreateFusionName %s", m_pAssemblyName));

    MEMORY_REPORT_CONTEXT_SCOPE("FusionName");

    HRESULT hr;
    IAssemblyName *pFusionAssemblyName = NULL;
    LPWSTR pwAssemblyName = NULL;
    CQuickBytes qb;

    NonVMComHolder< IAssemblyName > holder(NULL);

    if (m_pAssemblyName) {
        long pwNameLen = WszMultiByteToWideChar(CP_UTF8, 0, m_pAssemblyName, -1, 0, 0);
        if(pwNameLen==0)
            IfFailGo(HRESULT_FROM_GetLastError());
        pwAssemblyName = (LPWSTR) qb.AllocNoThrow(pwNameLen*sizeof(WCHAR));
        if (!pwAssemblyName)
            IfFailGo(E_OUTOFMEMORY);
        if (!WszMultiByteToWideChar(CP_UTF8, 0, m_pAssemblyName, -1, pwAssemblyName, pwNameLen))
            IfFailGo(HRESULT_FROM_GetLastError());
    }

    IfFailGo(CreateAssemblyNameObject(&pFusionAssemblyName, pwAssemblyName, 0, NULL));

    holder = pFusionAssemblyName;

    if (m_context.usMajorVersion != (USHORT) -1) {
        IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_MAJOR_VERSION, 
                                                  &m_context.usMajorVersion, 
                                                  sizeof(USHORT)));
        
        if (m_context.usMinorVersion != (USHORT) -1) {
            IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_MINOR_VERSION, 
                                                      &m_context.usMinorVersion, 
                                                      sizeof(USHORT)));
            
            if (m_context.usBuildNumber != (USHORT) -1) {
                IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_BUILD_NUMBER, 
                                                          &m_context.usBuildNumber, 
                                                          sizeof(USHORT)));
                
                if (m_context.usRevisionNumber != (USHORT) -1)
                    IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_REVISION_NUMBER, 
                                                              &m_context.usRevisionNumber, 
                                                              sizeof(USHORT)));
            }
        }
    }
    
    if (m_context.szLocale) {
        MAKE_WIDEPTR_FROMUTF8(pwLocale,m_context.szLocale);
        
        IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_CULTURE, 
                                                  pwLocale, 
                                                  (DWORD)(wcslen(pwLocale) + 1) * sizeof (WCHAR)));
    }
    
    if (m_pbPublicKeyOrToken) {
        if (m_cbPublicKeyOrToken) {
            if(m_dwFlags & afPublicKey) {
                IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_PUBLIC_KEY,
                                                          m_pbPublicKeyOrToken, m_cbPublicKeyOrToken));
            }
            else {
                    IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_PUBLIC_KEY_TOKEN,
                                                              m_pbPublicKeyOrToken, m_cbPublicKeyOrToken));
            }
        }
        else {
            IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_NULL_PUBLIC_KEY_TOKEN,
                                                      NULL, 0));
        }
    }

    // See if the assembly[ref] is retargetable (ie, for a generic assembly).
    if (IsAfRetargetable(m_dwFlags)) {
        BOOL bTrue = TRUE;
        IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_RETARGET, 
                                                  &bTrue, sizeof(bTrue)));
    }

    // Set the Processor Architecture (if any)
    {
        DWORD dwPEkind = (DWORD)PAIndex(m_dwFlags);
        if((dwPEkind >= 1) && (dwPEkind <= 4))
        {
            PEKIND peKind = (PEKIND)dwPEkind;
            IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_ARCHITECTURE, 
                                                      &peKind, sizeof(peKind)));
        }
    }

    if (fIncludeCodeBase && m_CodeInfo.m_dwCodeBase > 0) {
        IfFailGo(pFusionAssemblyName->SetProperty(ASM_NAME_CODEBASE_URL,
                                                  (void*)m_CodeInfo.m_pszCodeBase, 
                                                  m_CodeInfo.m_dwCodeBase*sizeof(WCHAR)));
    }

    *ppName = pFusionAssemblyName;

    TIMELINE_END(FUSIONBIND, ("CreateFusionName %s", m_pAssemblyName));

    holder.SuppressRelease();
    return S_OK;

 ErrExit:

    TIMELINE_END(FUSIONBIND, ("CreateFusionName %s", m_pAssemblyName));

    return hr;
}

HRESULT FusionBind::EmitToken(IMetaDataAssemblyEmit *pEmit, 
                              mdAssemblyRef *pToken,
                              BOOL fUsePublicKeyToken)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        MODE_ANY;
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    HRESULT hr;
    ASSEMBLYMETADATA AMD;

    AMD.usMajorVersion = m_context.usMajorVersion;
    AMD.usMinorVersion = m_context.usMinorVersion;
    AMD.usBuildNumber = m_context.usBuildNumber;
    AMD.usRevisionNumber = m_context.usRevisionNumber;

    if (m_context.szLocale) {
        AMD.cbLocale = MultiByteToWideChar(CP_ACP, 0, m_context.szLocale, -1, NULL, 0);
        if(AMD.cbLocale==0)
            IfFailRet(HRESULT_FROM_GetLastError());
        AMD.szLocale = (LPWSTR) alloca(AMD.cbLocale * sizeof(WCHAR) );
        if(MultiByteToWideChar(CP_ACP, 0, m_context.szLocale, -1, AMD.szLocale, AMD.cbLocale)==0)
            IfFailRet(HRESULT_FROM_GetLastError());
    }
    else {
        AMD.cbLocale = 0;
        AMD.szLocale = NULL;
    }

    long pwNameLen = WszMultiByteToWideChar(CP_UTF8, 0, m_pAssemblyName, -1, 0, 0);
    if(pwNameLen==0)
        return HRESULT_FROM_GetLastError();
    CQuickBytes qb;
    LPWSTR pwName = (LPWSTR) qb.AllocNoThrow(pwNameLen*sizeof(WCHAR));
    if (!pwName)
        return E_OUTOFMEMORY;

    if (!WszMultiByteToWideChar(CP_UTF8, 0, m_pAssemblyName, -1, pwName, pwNameLen))
        return HRESULT_FROM_GetLastError();

    // If we've been asked to emit a public key token in the reference but we've
    // been given a public key then we need to generate the token now.
    PBYTE pbPublicKeyOrToken = m_pbPublicKeyOrToken;
    DWORD cbPublicKeyOrToken = m_cbPublicKeyOrToken;
    DWORD dwFlags = m_dwFlags;

    if (cbPublicKeyOrToken && fUsePublicKeyToken && IsAfPublicKey(m_dwFlags)) {
        PBYTE pbPublicKeyToken;
        DWORD cbPublicKeyToken;
        if (!StrongNameTokenFromPublicKey(pbPublicKeyOrToken,
                                          cbPublicKeyOrToken,
                                          &pbPublicKeyToken,
                                          &cbPublicKeyToken))
            return StrongNameErrorInfo();

        pbPublicKeyOrToken = pbPublicKeyToken;
        cbPublicKeyOrToken = cbPublicKeyToken;
        dwFlags &= ~afPublicKey;
    }

    hr = pEmit->DefineAssemblyRef(pbPublicKeyOrToken, cbPublicKeyOrToken,
                                  pwName,
                                  &AMD,
                                  NULL, 0,
                                  dwFlags, pToken);

    if (pbPublicKeyOrToken != m_pbPublicKeyOrToken)
        StrongNameFreeBuffer(pbPublicKeyOrToken);

    return hr;
}

HRESULT FusionBind::LoadAssembly(IApplicationContext* pFusionContext,
                                 FusionSink *pSink,
                                 IAssembly** ppIAssembly,
                                 IHostAssembly** ppIHostAssembly,
                                 IAssembly** ppNativeFusionAssembly,
                                 BOOL fForIntrospectionOnly)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        MODE_PREEMPTIVE;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

    HRESULT hr = E_FAIL;

    NonVMComHolder<IAssembly> pIAssembly(NULL);
    NonVMComHolder<IAssembly> pNativeFusionAssembly(NULL);    
    NonVMComHolder<IHostAssembly> pIHostAssembly(NULL);
    NonVMComHolder<IAssemblyName> pSpecName;
    NonVMComHolder<IAssemblyName> pCodeBaseName;

    TIMELINE_START(FUSIONBIND, ("LoadAssembly %s", m_pAssemblyName));

    // Make sure we don't have malformed names
    
    if (m_pAssemblyName)
        IfFailGo(VerifyBindingString(m_pAssemblyName));

    if (m_context.szLocale)
        IfFailGo(VerifyBindingString(m_context.szLocale));

    // If we have assembly name info, first bind using that
    if (m_pAssemblyName != NULL) {
        IfFailGo(CreateFusionName(&pSpecName, FALSE));

        hr = RemoteLoad(pFusionContext, pSink, 
                        pSpecName, m_CodeInfo.GetParentAssembly(), NULL, 
                        &pIAssembly, &pIHostAssembly, &pNativeFusionAssembly, fForIntrospectionOnly);
    }
    
    // Now, bind using the codebase.
    if (FAILED(hr) && m_CodeInfo.m_dwCodeBase > 0) {
        // No resolution by code base for SQL-hosted environment, except for introspection
        if((!fForIntrospectionOnly) && CorHost2::IsLoadFromBlocked())
        {
            hr = FUSION_E_LOADFROM_BLOCKED;
            goto ErrExit;
        }
        IfFailGo(CreateAssemblyNameObject(&pCodeBaseName, NULL, 0, NULL));

        IfFailGo(pCodeBaseName->SetProperty(ASM_NAME_CODEBASE_URL,
                                            (void*)m_CodeInfo.m_pszCodeBase, 
                                            m_CodeInfo.m_dwCodeBase*sizeof(WCHAR)));

        // Note that we cannot bind a native image using a codebase, as it will
        // always be in the LoadFrom context which does not support native images.

        pSink->Reset();
        hr = RemoteLoad(pFusionContext, pSink, 
                        pCodeBaseName, NULL, m_CodeInfo.m_pszCodeBase,
                        &pIAssembly, &pIHostAssembly, &pNativeFusionAssembly, fForIntrospectionOnly);

        // If we had both name info and codebase, make sure they are consistent.
        if (SUCCEEDED(hr) && m_pAssemblyName != NULL) {

            NonVMComHolder<IAssemblyName> pPolicyRefName(NULL);
            if (!fForIntrospectionOnly) {
                // Get post-policy ref, because we'll be comparing
                // it against a post-policy def
                HRESULT policyHr = PreBindAssembly(pFusionContext,
                                                   pSpecName,
                                                   NULL, // pAsmParent
                                                   &pPolicyRefName,
                                                   NULL);  // pvReserved
                if (FAILED(policyHr) && (policyHr != FUSION_E_REF_DEF_MISMATCH) &&
                    (policyHr != E_INVALIDARG)) // partial ref
                    IfFailGo(policyHr);
            }

            NonVMComHolder<IAssemblyName> pBoundName;
            if (pIAssembly.IsNull())
                IfFailGo(pIHostAssembly->GetAssemblyNameDef(&pBoundName));
            else
                IfFailGo(pIAssembly->GetAssemblyNameDef(&pBoundName));

            // Order matters: Ref->IsEqual(Def)
            HRESULT equalHr;
            if (pPolicyRefName)
                equalHr = pPolicyRefName->IsEqual(pBoundName, ASM_CMPF_DEFAULT);
            else
                equalHr = pSpecName->IsEqual(pBoundName, ASM_CMPF_DEFAULT);
            if (equalHr != S_OK)
                IfFailGo(FUSION_E_REF_DEF_MISMATCH);
        }
    }

    // We should have found an assembly by now.
    IfFailGo(hr);

    


    if (SUCCEEDED(hr)) {
        if (pIAssembly.IsNull())
            *ppIHostAssembly = pIHostAssembly.Extract();
        else
            *ppIAssembly = pIAssembly.Extract();
        if (ppNativeFusionAssembly) {
            *ppNativeFusionAssembly = pNativeFusionAssembly.Extract();
        }
    }

 ErrExit:
    TIMELINE_END(FUSIONBIND, ("LoadAssembly %s", m_pAssemblyName));
    return hr;
}


/* static */
HRESULT FusionBind::RemoteLoad(IApplicationContext* pFusionContext,
                               FusionSink *pSink,
                               IAssemblyName *pName,
                               IAssembly *pParentAssembly,
                               LPCWSTR pCodeBase,
                               IAssembly** ppIAssembly,
                               IHostAssembly** ppIHostAssembly,
                               IAssembly** ppNativeFusionAssembly,
                               BOOL    fForIntrospectionOnly)
{
    CONTRACTL
    {
        NOTHROW;
        MODE_PREEMPTIVE;
        // The resulting IP must be held so the assembly will not be scavenged.
        PRECONDITION(CheckPointer(ppIAssembly));
        PRECONDITION(CheckPointer(ppIHostAssembly));

        PRECONDITION(CheckPointer(pName));
        INJECT_FAULT(return E_OUTOFMEMORY;);
    } CONTRACTL_END;

    TIMELINE_START(FUSIONBIND, ("RemoteLoad"));

    MEMORY_REPORT_CONTEXT_SCOPE("FusionBinding");
    ETWTraceStartup trace(ETW_TYPE_STARTUP_FUSION_BINDING);

    HRESULT hr;
    ASM_BIND_FLAGS dwFlags = ASM_BINDF_NONE;
    DWORD dwReserved = 0;
    LPVOID pReserved = NULL;

    NonVMComHolder< IUnknown > pSinkIUnknown(NULL);
    NonVMComHolder< IAssemblyNameBinder> pBinder(NULL);
    *ppNativeFusionAssembly=NULL;

    if(pParentAssembly != NULL) {
        // Only use a parent assembly hint when the parent assembly has a load context.
        // Assemblies in anonymous context are not dicoverable by loader's binding rules,
        // thus loader can't find their dependencies. 
        // Loader will only try to locate dependencies in default load context.
        if (pParentAssembly->GetFusionLoadContext() != LOADCTX_TYPE_UNKNOWN) {
            dwReserved = sizeof(IAssembly*);
            pReserved = (LPVOID) pParentAssembly;
            dwFlags = ASM_BINDF_PARENT_ASM_HINT;
        }
    }
    
    IfFailRet(pSink->AssemblyResetEvent());
    IfFailRet(pSink->QueryInterface(IID_IUnknown, (void**)&pSinkIUnknown));
    IUnknown *pFusionAssembly=NULL;
    IUnknown  *pNativeAssembly=NULL;
    BOOL fCached = TRUE;


    if (fForIntrospectionOnly)
    {
        dwFlags = (ASM_BIND_FLAGS)(dwFlags | ASM_BINDF_INSPECTION_ONLY);
    }

    IfFailRet(pName->QueryInterface(IID_IAssemblyNameBinder, (void **)&pBinder));
    {
        // In SQL, this can call back into the runtime
        CONTRACT_VIOLATION(ThrowsViolation);
        hr = pBinder->BindToObject(IID_IAssembly,
                                 pSinkIUnknown,
                                 pFusionContext,
                                 pCodeBase,
                                 dwFlags,
                                 pReserved,
                                 dwReserved,
                                 (void**) &pFusionAssembly,
                                 (void**)&pNativeAssembly);
    }
    
    if(hr == E_PENDING) {
        // If there is an assembly IP then we were successful.
        hr = pSink->Wait();
        if (SUCCEEDED(hr))
            hr = pSink->LastResult();
        if(SUCCEEDED(hr)) {
            if(pSink->m_punk) {
                if (pSink->m_pNIunk)
                    pNativeAssembly=pSink->m_pNIunk;
                pFusionAssembly = pSink->m_punk;
                fCached = FALSE;
            }
            else
                hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
    }

    if (SUCCEEDED(hr)) {
        // Keep a handle to ensure it does not disappear from the cache
        // and allow access to modules associated with the assembly.
        hr = pFusionAssembly->QueryInterface(IID_IAssembly, 
                                             (void**) ppIAssembly);
        if (hr == E_NOINTERFACE) // IStream assembly
            hr = pFusionAssembly->QueryInterface(IID_IHostAssembly, 
                                                 (void**) ppIHostAssembly);
        if (SUCCEEDED(hr) && pNativeAssembly)
            hr=pNativeAssembly->QueryInterface(IID_IAssembly,
                                            (void**)ppNativeFusionAssembly);

        if (fCached)
        {
            pFusionAssembly->Release();
            if(pNativeAssembly)
                pNativeAssembly->Release();
        }
    }

    TIMELINE_END(FUSIONBIND, ("RemoteLoad"));
    return hr;
}

/* static */
HRESULT FusionBind::RemoteLoadModule(IApplicationContext * pFusionContext, 
                                     IAssemblyModuleImport* pModule, 
                                     FusionSink *pSink,
                                     IAssemblyModuleImport** pResult)
{
    
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION(CheckPointer(pFusionContext));
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(pSink));
        PRECONDITION(CheckPointer(pResult));
        INJECT_FAULT(return E_OUTOFMEMORY;);
    } CONTRACTL_END;

    TIMELINE_START(FUSIONBIND, ("RemoteLoadModule"));

    MEMORY_REPORT_CONTEXT_SCOPE("FusionBinding");
    ETWTraceStartup trace(ETW_TYPE_STARTUP_FUSION_BINDING);

    HRESULT hr;
    IfFailGo(pSink->AssemblyResetEvent());
    hr = pModule->BindToObject(pSink,
                               pFusionContext,
                               ASM_BINDF_NONE,
                               (void**) pResult);
    if(hr == E_PENDING) {
        // If there is an assembly IP then we were successful.
        hr = pSink->Wait();
        if (SUCCEEDED(hr))
            hr = pSink->LastResult();
        if (SUCCEEDED(hr)) {
            if(pSink->m_punk)
                hr = pSink->m_punk->QueryInterface(IID_IAssemblyModuleImport, 
                                                   (void**) pResult);
            else
                hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
    }

 ErrExit:
    TIMELINE_END(FUSIONBIND, ("RemoteLoadModule"));
    return hr;
}


/* static */
HRESULT FusionBind::AddEnvironmentProperty(__in LPCWSTR variable, 
                                           __in LPCWSTR pProperty, 
                                           IApplicationContext* pFusionContext)
{
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION(CheckPointer(pProperty));
        PRECONDITION(CheckPointer(variable));
        PRECONDITION(CheckPointer(pFusionContext));
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    DWORD size = _MAX_PATH;
    WCHAR rcValue[_MAX_PATH];    // Buffer for the directory.
    WCHAR *pValue = &(rcValue[0]);
    size = WszGetEnvironmentVariable(variable, pValue, size);
    if(size > _MAX_PATH) {
        pValue = (WCHAR*) _alloca(size * sizeof(WCHAR));
        size = WszGetEnvironmentVariable(variable, pValue, size);
        size++; // Add in the null terminator
    }

    if(size)
        return pFusionContext->Set(pProperty,
                                   pValue,
                                   size * sizeof(WCHAR),
                                   0);
    else 
        return S_FALSE; // no variable found
}

// Fusion uses a context class to drive resolution of assemblies.
// Each application has properties that can be pushed into the
// fusion context (see fusionp.h). The public api is part of
// application domains.
/* static */
HRESULT FusionBind::SetupFusionContext(LPCWSTR szAppBase,
                                       LPCWSTR szPrivateBin,
                                       IApplicationContext** ppFusionContext)
{
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION(CheckPointer(ppFusionContext));
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    TIMELINE_START_SAFE(FUSIONBIND, ("SetupFusionContext %S", szAppBase));
    HRESULT hr;
    NonVMComHolder <IApplicationContext> pFusionContext;

    MEMORY_REPORT_CONTEXT_SCOPE("FusionContext");

    LPCWSTR pBase;
    // if the appbase is null then use the current directory
    if (szAppBase == NULL) {
        pBase = (LPCWSTR) _alloca(_MAX_PATH * sizeof(WCHAR));
        if(!WszGetCurrentDirectory(_MAX_PATH, (LPWSTR) pBase))
            IfFailGo(HRESULT_FROM_GetLastError());
    }
    else
        pBase = szAppBase;


    IfFailGo(CreateFusionContext(pBase, &pFusionContext));
        
    IfFailGo((pFusionContext)->Set(ACTAG_APP_BASE_URL,
                                   (void*) pBase,
                                   (DWORD)(wcslen(pBase) + 1) * sizeof(WCHAR),
                                   0));
        
    if (szPrivateBin)
        IfFailGo((pFusionContext)->Set(ACTAG_APP_PRIVATE_BINPATH,
                                       (void*) szPrivateBin,
                                       (DWORD)(wcslen(szPrivateBin) + 1) * sizeof(WCHAR),
                                       0));
    else
        IfFailGo(AddEnvironmentProperty(APPENV_RELATIVEPATH, ACTAG_APP_PRIVATE_BINPATH, pFusionContext));

    *ppFusionContext=pFusionContext;
    pFusionContext.SuppressRelease();
    
ErrExit:    
    TIMELINE_END(FUSIONBIND, ("SetupFusionContext %S", szAppBase));

    return hr;
}

/* static */
HRESULT FusionBind::CreateFusionContext(LPCWSTR pzName, IApplicationContext** ppFusionContext)
{
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION(CheckPointer(ppFusionContext));
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    TIMELINE_START(FUSIONBIND, ("CreateFusionContext %S", pzName));

    MEMORY_REPORT_CONTEXT_SCOPE("FusionContext");

    // This is a file name not a namespace
    LPCWSTR contextName = NULL;

    if(pzName) {
        contextName = wcsrchr( pzName, L'\\' );
        if(contextName)
            contextName++;
        else
            contextName = pzName;
    }
    // We go off and create a fusion context for this application domain.
    // Note, once it is made it can not be modified.
    NonVMComHolder<IAssemblyName> pFusionAssemblyName;
    HRESULT hr = CreateAssemblyNameObject(&pFusionAssemblyName, contextName, 0, NULL);

    if(SUCCEEDED(hr))
        hr = CreateApplicationContext(pFusionAssemblyName, ppFusionContext);
    
    if(pzName)
        TIMELINE_END(FUSIONBIND, ("CreateFusionContext %S", pzName));
    else
        TIMELINE_END(FUSIONBIND, ("CreateFusionContext <unknown>"));

    return hr;
}

/* static */
HRESULT FusionBind::GetVersion(__out_ecount(*pdwVersion) LPWSTR pVersion, __inout DWORD* pdwVersion)
{
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION(CheckPointer(pdwVersion));
        PRECONDITION(pdwVersion>0 && CheckPointer(pVersion));
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    HRESULT hr;
    WCHAR pCORSystem[_MAX_PATH];
    DWORD dwCORSystem = _MAX_PATH;
    
    pCORSystem[0] = L'\0';
    hr = GetInternalSystemDirectory(pCORSystem, &dwCORSystem);
    if(FAILED(hr)) return hr;

    if(dwCORSystem == 0) 
        return E_FAIL;

    dwCORSystem--; // remove the null character
    if(dwCORSystem && pCORSystem[dwCORSystem-1] == L'\\')
        dwCORSystem--; // and the trailing slash if it exists

    if (dwCORSystem==0)
        return E_FAIL;

    WCHAR* pSeparator;
    WCHAR* pTail = pCORSystem + dwCORSystem;

    for(pSeparator = pCORSystem+dwCORSystem-1; pSeparator > pCORSystem && *pSeparator != L'\\';pSeparator--);

    if(*pSeparator == L'\\')
        pSeparator++;
    
    DWORD lgth = (DWORD)(pTail - pSeparator);
    if(lgth > *pdwVersion) {
        *pdwVersion = lgth+1;
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    while(pSeparator < pTail) 
        *pVersion++ = *pSeparator++;

    *pVersion = L'\0';

    return S_OK;
}
