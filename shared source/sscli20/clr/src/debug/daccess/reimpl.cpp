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
//*****************************************************************************
// File: reimpl.cpp
//
// Data-access-specific reimplementations of standard code.
//
//*****************************************************************************

#include "stdafx.h"

extern "C"
{
GVAL_DECL(DWORD, gThreadTLSIndex);
GVAL_DECL(DWORD, gAppDomainTLSIndex);
};

Thread* __stdcall
DacGetThread(ULONG32 osThread)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    CLRDATA_ADDRESS value;

    if ((osThread != 0 || g_dacImpl->m_target->
            GetCurrentThreadID(&osThread) == S_OK) 
        && g_dacImpl->m_target->
            GetTLSValue(osThread, gThreadTLSIndex, &value) == S_OK
        && value != NULL)
    {
        return PTR_Thread((TADDR)value);
    }

    //
    // There may not be TLS, so fall back on scanning the
    // whole thread store to see if there's a matching thread.
    //

    if (!ThreadStore::s_pThreadStore)
    {
        return NULL;
    }

    Thread* thread = ThreadStore::s_pThreadStore->m_ThreadList.GetHead();
    while (thread)
    {
        if (thread->GetOSThreadId() == osThread)
        {
            return thread;
        }

        thread = ThreadStore::s_pThreadStore->m_ThreadList.GetNext(thread);
    }

    return NULL;
}

Thread* __stdcall
DacFailGetThread(void);

Thread* __stdcall
DacFailGetThread(void)
{
    // In dac mode it's unlikely that the thread calling dac
    // is actually the same "current thread" that the runtime cares
    // about.  Fail all queries of the current thread by
    // the runtime code to catch any inadvertent usage.
    DacError(E_UNEXPECTED);
    return NULL;
}

#if USE_INDIRECT_GET_THREAD_APPDOMAIN
Thread* (__stdcall *GetThread)(void) = DacFailGetThread;
#else
EXTERN_C Thread* GetThread()
{
    DacError(E_UNEXPECTED);
    return NULL;
}
#endif

AppDomain* __stdcall
DacGetAppDomain(void)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    HRESULT status;
    ULONG32 curOsThread;

    if ((status = g_dacImpl->m_target->
         GetCurrentThreadID(&curOsThread)) != S_OK)
    {
        DacError(status);
        UNREACHABLE();
    }

    CLRDATA_ADDRESS value;

    if ((status = g_dacImpl->m_target->
         GetTLSValue(curOsThread, gAppDomainTLSIndex, &value)) != S_OK)
    {
        DacError(status);
        UNREACHABLE();
    }

    return PTR_AppDomain((TADDR)value);
}

AppDomain* __stdcall
DacFailGetAppDomain(void);

AppDomain* __stdcall
DacFailGetAppDomain(void)
{
    // In dac mode it's unlikely that the thread calling dac
    // is actually the same "current thread" that the runtime cares
    // about.  Fail all queries of the current thread by
    // the runtime code to catch any inadvertent usage.
    DacError(E_UNEXPECTED);
    return NULL;
}

#if USE_INDIRECT_GET_THREAD_APPDOMAIN
AppDomain* (__stdcall *GetAppDomain)(void) = DacFailGetAppDomain;
#else
EXTERN_C AppDomain* GetAppDomain()
{
    DacError(E_UNEXPECTED);
    return NULL;
}
#endif

BOOL
DacGetThreadContext(Thread* thread, CONTEXT* context)
{
    if (!g_dacImpl)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    // XXX drewb - How do you retrieve the context for
    // a Thread that's not running?
    if (!thread->GetOSThreadId() ||
        thread->GetOSThreadId() == 0xbaadf00d)
    {
        DacError(E_UNEXPECTED);
        UNREACHABLE();
    }

    ULONG32 contextFlags;

    contextFlags = context->ContextFlags;

    HRESULT status =
        g_dacImpl->m_target->
        GetThreadContext(thread->GetOSThreadId(), contextFlags, 
                         sizeof(*context), (PBYTE)context);
    if (status != S_OK)
    {
        DacError(status);
        UNREACHABLE();
    }

    return TRUE;
}

BOOL (*EEGetThreadContext)(Thread *pThread, CONTEXT *pContext) =
    DacGetThreadContext;

//----------------------------------------------------------------------------
//
// Stolen from dlls\mscoroc\mscoroc.cpp.
//
//----------------------------------------------------------------------------

#include <corperm.h>


HRESULT STDMETHODCALLTYPE
TranslateSecurityAttributes(CORSEC_ATTRSET    *pPset,
                            BYTE          **ppbOutput,
                            DWORD          *pcbOutput,
                            BYTE          **ppbNonCasOutput,
                            DWORD          *pcbNonCasOutput,
                            DWORD          *pdwErrorIndex)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE
GroupSecurityAttributesByAction(CORSEC_ATTRSET /*OUT*/rPermSets[],
                                      COR_SECATTR rSecAttrs[],
                                      ULONG cSecAttrs,
                                      mdToken tkObj,
                                      ULONG *pulErrorAttr,
                                      CMiniMdRW* pMiniMd,
                                      IMDInternalImport* pInternalImport)
{
    return E_NOTIMPL;
}

HRESULT AttributeSetToBlob(CORSEC_ATTRSET* pPset, BYTE* pBuffer, DWORD* pCount, IMetaDataAssemblyImport *pImport, DWORD dwAction)
{
    return E_NOTIMPL;
}

mdAssemblyRef DefineAssemblyRefForImportedTypeLib(
    void        *pAssembly,             // Assembly importing the typelib.
    void        *pvModule,              // Module importing the typelib.
    IUnknown    *pIMeta,                // IMetaData* from import module.
    IUnknown    *pIUnk,                 // IUnknown to referenced Assembly.
    BSTR        *pwzNamespace,          // The namespace of the resolved assembly.
    BSTR        *pwzAsmName,            // The name of the resolved assembly.
    Assembly    **AssemblyRef)          // The resolved assembly.
{
    return 0;
}

mdAssemblyRef DefineAssemblyRefForExportedAssembly(
    LPCWSTR     pszFullName,            // The full name of the assembly.
    IUnknown    *pIMeta)                // Metadata emit interface.
{
    return 0;
}

HRESULT STDMETHODCALLTYPE
GetAssembliesByName(LPCWSTR  szAppBase,
                    LPCWSTR  szPrivateBin,
                    LPCWSTR  szAssemblyName,
                    IUnknown *ppIUnk[],
                    ULONG    cMax,
                    ULONG    *pcAssemblies)
{
    return E_NOTIMPL;
}

//----------------------------------------------------------------------------
//
// Strong name functionality.
//
//----------------------------------------------------------------------------

#include "../../vm/strongname.cpp"
