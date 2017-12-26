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
** Header:  Com Callable wrapper  classes
**
**                                                     </STRIP>
===========================================================*/

#ifndef _COMCALLABLEWRAPPER_H
#define _COMCALLABLEWRAPPER_H


#include "corffi.h"

class ComCallWrapper : public IManagedInstanceWrapper
{
protected:
    inline static PVOID GetComCallWrapperVPtr()
    {
        CONTRACT (PVOID)
        {
            WRAPPER(THROWS);
            WRAPPER(GC_TRIGGERS);
            MODE_ANY;
            POSTCONDITION(CheckPointer(RETVAL));
        }
        CONTRACT_END;
        
        ComCallWrapper boilerplate;
        RETURN (PVOID&)boilerplate;
    }

public:
    ComCallWrapper()
    {
        LEAF_CONTRACT;
        
        m_cRef = 1;
        m_hThis = NULL;
        m_pContext = NULL;
    }

    ~ComCallWrapper()
    {
        WRAPPER_CONTRACT;
        
        // NICE: A bad things will happen if the ComCallWrapper is called or
        // destroyed after the unload of AppDomain it belongs to.
        // It might be ok: AppDomain unloading is pretty much like DLL unloading,
        // and nobody is surprised that you crash if you call an object
        // in unloaded DLL ... but it would be nice to handle this case gracefully.
        if (m_hThis) {
            DestroyHandle(m_hThis);
        }
    }

    void OnADUnload()
    {
        LEAF_CONTRACT;

        m_hThis = NULL;
    }

#ifndef DACCESS_COMPILE
    void Init(OBJECTREF* poref)
    {
        CONTRACTL
        {
            WRAPPER(THROWS);
            WRAPPER(GC_TRIGGERS);
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(poref));
            PRECONDITION(CheckPointer(*poref));
        }
        CONTRACTL_END;

        Context *pContext = GetCurrentContext();
        AppDomain *pDomain = pContext->GetDomain();

        m_hThis = pDomain->CreateHandle(*poref);
        m_dwDomainId = pDomain->GetId();
        m_pContext = pContext;
    }
#endif // DACCESS_COMPILE

    // IUnknown interface
    STDMETHODIMP QueryInterface(REFIID riid, void **ppInterface);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IManagedInstanceWrapper interface implementation
    STDMETHODIMP InvokeByName(LPCWSTR MemberName, INT32 BindingFlags, 
        INT32 ArgCount, VARIANT *ArgList, VARIANT *pRetVal);

    static VOID InvokeByNameCallback(LPVOID ptr);

    // Get wrapper from IP
    inline static ComCallWrapper* GetWrapperFromIP(IUnknown* pUnk)
    {
        CONTRACT (ComCallWrapper*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pUnk));
            POSTCONDITION(CheckPointer(RETVAL));
        }
        CONTRACT_END;
        
        return (ComCallWrapper*)pUnk;
    }

    inline static ComCallWrapper* GetCCWFromIUnknown(IUnknown* pUnk)
    {
        CONTRACT (ComCallWrapper*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            PRECONDITION(CheckPointer(pUnk));
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        if (*(PVOID*)pUnk != GetComCallWrapperVPtr())
            RETURN NULL;

        RETURN GetWrapperFromIP(pUnk);
    }

    inline OBJECTREF GetObjectRef()
    {
        CONTRACT (OBJECTREF)
        {
            WRAPPER(THROWS);
            WRAPPER(GC_TRIGGERS);
            MODE_COOPERATIVE;
            PRECONDITION(CheckPointer(m_hThis));
        }
        CONTRACT_END;
        
        RETURN ObjectFromHandle(m_hThis);
    }

    inline ADID GetDomainID()
    {
        WRAPPER_CONTRACT;
        
        return m_dwDomainId;
    }

protected:
    LONG            m_cRef;
    OBJECTHANDLE    m_hThis;
    ADID            m_dwDomainId;
    Context*        m_pContext;

public:
    struct InvokeByNameArgs
    {
        ComCallWrapper* pThis;
        LPCWSTR MemberName;
        INT32 BindingFlags;
        INT32 ArgCount;
        VARIANT *ArgList;
        VARIANT *pRetVal;
    };
};


#endif // _COMCALLABLEWRAPPER_H
