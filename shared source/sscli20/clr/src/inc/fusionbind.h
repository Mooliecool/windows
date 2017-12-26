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
** Header:  FusionBind.hpp
**
** Purpose: Implements FusionBind (loader domain) architecture
**
** Date:  Oct 26, 1998
**
===========================================================*/
#ifndef _FUSIONBIND_H
#define _FUSIONBIND_H

#include <fusion.h>
#include <fusionpriv.h>
#include "metadata.h"
#include "fusionsink.h"
#include "utilcode.h"
#include "loaderheap.h"
#include "fusionsetup.h"
#include "sstring.h"
#include "ex.h"


class CodeBaseInfo
{
    IAssembly* m_pParentAssembly;      // The assembly that has the reference.
public:
    LPCWSTR m_pszCodeBase;             // URL to the code
    DWORD   m_dwCodeBase;              // length of URL INCLUDING NULL TERMINATOR!
    LOADCTX_TYPE m_fParentLoadContext; // m_pParentAssembly->GetFusionLoadContext()
    
    CodeBaseInfo() :
        m_pParentAssembly(NULL),
        m_pszCodeBase(NULL),
        m_dwCodeBase(0),
        m_fParentLoadContext(LOADCTX_TYPE_DEFAULT)
    {
        LEAF_CONTRACT;
    }

    ~CodeBaseInfo()
    {
        WRAPPER_CONTRACT;
        ReleaseParent();
    }

    // Note: the hint and parent assembly cannot both be set. The Parent Assembly takes
    // precedence. The parent assembly provides the context in which to bind. Fusion
    // has two context's per ApplicationContext, one for normal binds and one for 
    // where-ref binds. The number-one rule for normal binds is order will not affect
    // which assemblies are loaded. The where-ref binds is completely dependent on 
    // order. Where-ref binds to not influence normal binds but where-refs can bind to
    // assemblies in the normal context.
    
    void SetParentAssembly(IAssembly* pAssembly)
    {
        CONTRACTL
        {
            INSTANCE_CHECK;
            NOTHROW;
#ifdef MODE_PREEMPTIVE
            MODE_PREEMPTIVE;
#endif
            GC_NOTRIGGER;
        }
        CONTRACTL_END;

        if(m_pParentAssembly)
            ReleaseParent();
        m_pParentAssembly = pAssembly;
        if(m_pParentAssembly) {
            m_pParentAssembly->AddRef();
            m_fParentLoadContext = m_pParentAssembly->GetFusionLoadContext();
        }
        else
            m_fParentLoadContext = LOADCTX_TYPE_DEFAULT;
    }

    IAssembly* GetParentAssembly()
    {
        LEAF_CONTRACT;
        return m_pParentAssembly;
    }

    void ReleaseParent();
};

class FusionBind
{
protected:
    AssemblyMetaDataInternal    m_context;
    LPCSTR                      m_pAssemblyName; 
    PBYTE                       m_pbPublicKeyOrToken;
    DWORD                       m_cbPublicKeyOrToken;
    DWORD                       m_dwFlags;
    CodeBaseInfo                m_CodeInfo;
    int                         m_ownedFlags;

public:
    enum 
    {
        NAME_OWNED                  = 0x01,
        PUBLIC_KEY_OR_TOKEN_OWNED   = 0x02,
        CODE_BASE_OWNED             = 0x04,
        LOCALE_OWNED                = 0x08,
        ALL_OWNED                   = 0xFF,
    };

    FusionBind()
    {
        LEAF_CONTRACT;
        ZeroMemory(this, sizeof(*this));
    }
    ~FusionBind();

    HRESULT Init(LPCSTR pAssemblyDisplayName);
    HRESULT Init(LPCSTR pAssemblyName,
                 AssemblyMetaDataInternal* pContext, 
                 PBYTE pbPublicKeyOrToken, DWORD cbPublicKeyOrToken,
                 DWORD dwFlags);
    HRESULT Init(IAssemblyName *pName);
    HRESULT Init(FusionBind *pSpec, BOOL bClone=TRUE);

    HRESULT CloneFields(int flags);
    VOID    CloneFieldsToLoaderHeap(int flags, LoaderHeap *pHeap, AllocMemTracker *pamTracker);

    HRESULT ParseName();

    void SetCodeBase(LPCWSTR szCodeBase, DWORD dwCodeBase);
    
    DWORD Hash();
    BOOL CompareEx(FusionBind *pSpec);

    void GetFileOrDisplayName(DWORD flags, SString &result);

    //****************************************************************************************
    //

    static HRESULT GetVersion(__out_ecount(*pdwVersion) LPWSTR pVersion, __inout DWORD* pdwVersion);
    
    HRESULT EmitToken(IMetaDataAssemblyEmit *pEmitter, mdAssemblyRef *pToken, BOOL fUsePublicKeyToken = TRUE);

    LPCSTR GetName() { LEAF_CONTRACT; return m_pAssemblyName; }
    AssemblyMetaDataInternal *GetContext() { LEAF_CONTRACT; return &m_context; }
    CodeBaseInfo* GetCodeBase() { LEAF_CONTRACT; return &m_CodeInfo; }
    BOOL IsStrongNamed() { LEAF_CONTRACT; return m_cbPublicKeyOrToken; }

    //****************************************************************************************
    //
    HRESULT LoadAssembly(IApplicationContext *pFusionContext, 
                         FusionSink *pSink,
                         IAssembly** ppIAssembly,
                         IHostAssembly** ppIHostAssembly,
                         IAssembly** ppNativeFusionAssembly,
                         BOOL fForIntrospectionOnly);
    
    
    //****************************************************************************************
    //
    // Creates a fusion context for the application domain. All ApplicationContext properties
    // must be set in the AppDomain store prior to this call. Any changes or additions to the
    // AppDomain store are ignored.
    static HRESULT CreateFusionContext(LPCWSTR pzName, IApplicationContext** ppFusionContext);


    //****************************************************************************************
    //
    // Loads an environmental value into the fusion context
    static HRESULT AddEnvironmentProperty(__in LPCWSTR variable, 
                                          __in LPCWSTR pProperty, 
                                          IApplicationContext* pFusionContext);
    
    //****************************************************************************************
    //
    // Creates and loads an assembly based on the name and context.
    HRESULT CreateFusionName(IAssemblyName **ppName, BOOL fIncludeCodeBase = TRUE);

    //****************************************************************************************
    //
    static HRESULT SetupFusionContext(LPCWSTR szAppBase,
                                      LPCWSTR szPrivateBin,
                                      IApplicationContext** ppFusionContext);

    // Starts remote load of an assembly. The thread is parked on 
    // an event waiting for fusion to report success or failure.
    static HRESULT RemoteLoad(IApplicationContext * pFusionContext, 
                              FusionSink* pSink, 
                              IAssemblyName *pName, 
                              IAssembly *pParentAssembly,
                              LPCWSTR pCodeBase,
                              IAssembly** ppIAssembly,
                              IHostAssembly** ppIHostAssembly,
                              IAssembly** ppNativeFusionAssembly,
                              BOOL fForIntrospectionOnly);

    static HRESULT RemoteLoadModule(IApplicationContext * pFusionContext, 
                                    IAssemblyModuleImport* pModule, 
                                    FusionSink *pSink,
                                    IAssemblyModuleImport** pResult);

    static BOOL VerifyBindingStringW(LPCWSTR pwStr) {
        WRAPPER_CONTRACT;
        if (wcschr(pwStr, '\\') ||
            wcschr(pwStr, '/') ||
            wcschr(pwStr, ':') ||
            (RunningOnWin95() && ContainsUnmappableChars(pwStr, CP_OEMCP)))
            return FALSE;

        return TRUE;
    }

    static HRESULT VerifyBindingString(LPCSTR pName) {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            INJECT_FAULT(return E_OUTOFMEMORY;);
        }
        CONTRACTL_END;

        DWORD dwStrLen = WszMultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, pName, -1, NULL, NULL);
        CQuickBytes qb;
        LPWSTR pwStr = (LPWSTR) qb.AllocNoThrow(dwStrLen*sizeof(WCHAR));
        if (!pwStr)
            return E_OUTOFMEMORY;
        
        if(!WszMultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, pName, -1, pwStr, dwStrLen))
            return HRESULT_FROM_GetLastError();

        if (VerifyBindingStringW(pwStr))
            return S_OK;
        else
            return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }

    static void GetAssemblyManifestModulePath(IAssembly *pFusionAssembly, SString &result)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(ThrowOutOfMemory());
        }
        CONTRACTL_END;

        DWORD dwSize = 0;
        LPWSTR buffer = NULL;
        COUNT_T allocation = result.GetUnicodeAllocation();
        if (allocation > 0) {
            // pass in the buffer if we got one
            dwSize = allocation + 1;
            buffer = result.OpenUnicodeBuffer(allocation);
        }
        HRESULT hr = pFusionAssembly->GetManifestModulePath(buffer, &dwSize);
        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        {
            if (buffer != NULL) 
                result.CloseBuffer(0);
            buffer = result.OpenUnicodeBuffer(dwSize-1);
            hr = pFusionAssembly->GetManifestModulePath(buffer, &dwSize);
        }
        if (buffer != NULL)
            result.CloseBuffer((SUCCEEDED(hr) && dwSize >= 1) ? (dwSize-1) : 0);
        IfFailThrow(hr);
    }

    static void GetAssemblyNameDisplayName(IAssemblyName *pName, SString &result, DWORD flags)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(ThrowOutOfMemory());
        }
        CONTRACTL_END;

        DWORD dwSize = 0;
        LPWSTR buffer = NULL;
        COUNT_T allocation = result.GetUnicodeAllocation();
        if (allocation > 0) {
            // pass in the buffer if we got one
            dwSize = allocation + 1;
            buffer = result.OpenUnicodeBuffer(allocation);
        }
        HRESULT hr = pName->GetDisplayName(buffer, &dwSize, flags);
        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        {
            if (buffer != NULL) 
                result.CloseBuffer(0);
            buffer = result.OpenUnicodeBuffer(dwSize-1);
            hr = pName->GetDisplayName(buffer, &dwSize, flags);
        }
        if (buffer != NULL)
            result.CloseBuffer((SUCCEEDED(hr) && dwSize >= 1) ? (dwSize-1) : 0);
        IfFailThrow(hr);
    }

    static BOOL GetAssemblyNameStringProperty(IAssemblyName *pName, DWORD property, SString &result)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(ThrowOutOfMemory());
        }
        CONTRACTL_END;

        DWORD dwSize = 0;
        LPWSTR buffer = NULL;
        COUNT_T allocation = result.GetUnicodeAllocation();
        if (allocation > 0) {
            // pass in the buffer if we got one
            dwSize = (allocation + 1) * sizeof(WCHAR);
            buffer = result.OpenUnicodeBuffer(allocation);
        }
        HRESULT hr = pName->GetProperty(property, (LPVOID)buffer, &dwSize);
        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        {
            if (buffer != NULL) 
                result.CloseBuffer(0);
            buffer = result.OpenUnicodeBuffer(dwSize/sizeof(WCHAR) - 1);
            hr = pName->GetProperty(property, (LPVOID)buffer, &dwSize);
        }
        if (buffer != NULL)
            result.CloseBuffer((SUCCEEDED(hr) && dwSize >= sizeof(WCHAR)) ? (dwSize/sizeof(WCHAR)-1) : 0);
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            return FALSE;
        }
        IfFailThrow(hr);
              
        return TRUE;
    }

    static BOOL GetApplicationContextStringProperty(IApplicationContext *pContext, 
                                                    LPCWSTR property, SString &result)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(ThrowOutOfMemory());
        }
        CONTRACTL_END;

        DWORD dwSize = 0;
        LPWSTR buffer = NULL;
        COUNT_T allocation = result.GetUnicodeAllocation();
        if (allocation > 0) {
            // pass in the buffer if we got one
            dwSize = (allocation + 1) * sizeof(WCHAR);
            buffer = result.OpenUnicodeBuffer(allocation);
        }
        HRESULT hr = pContext->Get(property, (LPVOID)buffer, &dwSize, 0);
        if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
        {
            if (buffer != NULL) 
                result.CloseBuffer(0);
            buffer = result.OpenUnicodeBuffer(dwSize/sizeof(WCHAR) - 1);
            hr = pContext->Get(property, (LPVOID)buffer, &dwSize, 0);
        }
        if (buffer != NULL)
            result.CloseBuffer((SUCCEEDED(hr) && dwSize >= sizeof(WCHAR)) ? (dwSize/sizeof(WCHAR)-1) : 0);
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            return FALSE;
        }
        IfFailThrow(hr);

        return TRUE;
    }

    static BOOL GetApplicationContextDWORDProperty(IApplicationContext *pContext, 
                                                   LPCWSTR property, DWORD *result)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(return E_OUTOFMEMORY;);
        }
        CONTRACTL_END;

        DWORD dwSize = sizeof(DWORD);
        HRESULT hr = pContext->Get(property, result, &dwSize, 0);
        if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
            return FALSE;

        IfFailThrow(hr);
        
        return TRUE;
    }

    static void SetApplicationContextStringProperty(IApplicationContext *pContext, LPCWSTR property, 
                                                    SString &value)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(ThrowOutOfMemory());
        }
        CONTRACTL_END;

        IfFailThrow(pContext->Set(property, (void *) value.GetUnicode(), 
                                  (value.GetCount()+1)*sizeof(WCHAR), 0));
    }

    static void SetApplicationContextDWORDProperty(IApplicationContext *pContext, LPCWSTR property, 
                                                   DWORD value)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(ThrowOutOfMemory());
        }
        CONTRACTL_END;

        IfFailThrow(pContext->Set(property, &value, sizeof(value), 0));
    }

    static BOOL DeleteFusionAssembly(IAssemblyName *name)
    {
        CONTRACTL
        {
            THROWS;
            INJECT_FAULT(ThrowOutOfMemory());
        }
        CONTRACTL_END;

        return FALSE;
    }
    void SetPAFlags(DWORD dwPAIndex)
    {
        m_dwFlags &= ~afPA_FullMask;
        m_dwFlags |= PAFlag(dwPAIndex);
    }
};

#endif
