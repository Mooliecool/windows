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
** Header:  AppDomain.i
**
** Purpose: Implements AppDomain (loader domain) architecture
** inline functions
**
** Date:  June 27, 2000
**
===========================================================*/
#ifndef _APPDOMAIN_I
#define _APPDOMAIN_I

#ifndef DACCESS_COMPILE

#include "appdomain.hpp"

inline void AppDomain::SetUnloadInProgress(AppDomain *pThis)
{
    WRAPPER_CONTRACT;

    SystemDomain::System()->SetUnloadInProgress(pThis);
}

inline void AppDomain::SetUnloadComplete(AppDomain *pThis)
{
    GCX_COOP();

    SystemDomain::System()->SetUnloadComplete();
}

inline  void AppDomain::EnterContext(Thread* pThread, Context* pCtx,ContextTransitionFrame *pFrame)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pThread));
        PRECONDITION(CheckPointer(pCtx));
        PRECONDITION(CheckPointer(pFrame));
        PRECONDITION(pCtx->GetDomain()==this);
    }
    CONTRACTL_END;
    pThread->EnterContextRestricted(pCtx,pFrame);
};


inline AppDomainFromIDHolder::AppDomainFromIDHolder(ADID id, BOOL bUnsafePoint, SyncType synctype)
{
    WRAPPER_CONTRACT;
#ifdef _DEBUG
    m_bAcquired=false;   
    m_type=synctype;
    
#endif
    Assign(id, bUnsafePoint);
}

inline AppDomainFromIDHolder::AppDomainFromIDHolder(SyncType synctype)
{
    LEAF_CONTRACT;
    m_pDomain=NULL;
#ifdef _DEBUG
    m_bAcquired=false;
    m_type=synctype;
#endif
}

inline AppDomainFromIDHolder::~AppDomainFromIDHolder()
{
    WRAPPER_CONTRACT;
#ifdef _DEBUG
    if(m_bAcquired)
        Release();
#endif    
}

inline void AppDomainFromIDHolder::Release()
{
    //do not use real contract here!
    WRAPPER_CONTRACT;
#ifdef _DEBUG
    if(m_bAcquired)
    {
        if (m_type==SyncType_GC)
            m_pDomain=NULL;
        else
        if (m_type==SyncType_ADLock)
            SystemDomain::m_SystemDomainCrst.SetCantLeave(FALSE);
        else
        {
            _ASSERTE(!"Unknown type");        
        }
        m_pDomain=NULL;
        m_bAcquired=FALSE;
    }
#endif
}

inline void AppDomainFromIDHolder::Assign(ADID id, BOOL bUnsafePoint)
{
    //do not use real contract here!
    WRAPPER_CONTRACT;
    TESTHOOKCALL(AppDomainCanBeUnloaded(id.m_dwId, bUnsafePoint));
#ifdef _DEBUG
    m_bChecked=FALSE;
    if (m_type==SyncType_GC)
    {
        _ASSERTE(GetThread()->PreemptiveGCDisabled());
    }
    else
    if (m_type==SyncType_ADLock)    
    {
        _ASSERTE(SystemDomain::m_SystemDomainCrst.OwnedByCurrentThread());
        SystemDomain::m_SystemDomainCrst.SetCantLeave(TRUE);
    }
    else
    {
        _ASSERT(!"NI");
    }

    m_bAcquired=TRUE;
 #endif
    m_pDomain=SystemDomain::GetAppDomainAtId(id);

}



inline void AppDomainFromIDHolder::ThrowIfUnloaded()
{
    STATIC_CONTRACT_THROWS;
    if (IsUnloaded())
    {
        COMPlusThrow(kAppDomainUnloadedException);
    }
#ifdef _DEBUG
    m_bChecked=TRUE;
#endif
}

inline AppDomain* AppDomainFromIDHolder::operator ->()
{
    LEAF_CONTRACT;
    _ASSERTE(m_bChecked && m_bAcquired);    
    return m_pDomain;
}

inline DomainAssembly* AppDomain::FindDomainAssembly(Assembly* assembly)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(assembly));
    }
    CONTRACTL_END;
    return assembly->FindDomainAssembly(this);    
};

inline BOOL AppDomain::IsRunningIn(Thread* pThread)
{
    WRAPPER_CONTRACT;
    if (IsDefaultDomain()) 
        return TRUE;
    return pThread->IsRunningIn(this, NULL)!=NULL;
}



inline void AppDomain::AddMemoryPressure()
{
    WRAPPER_CONTRACT;
    m_MemoryPressure=EstimateSize();
    GCInterface::AddMemoryPressure(m_MemoryPressure);
}

inline void AppDomain::RemoveMemoryPressure()
{
    WRAPPER_CONTRACT;

    GCInterface::RemoveMemoryPressure(m_MemoryPressure);
}

#endif // DACCESS_COMPILE



#endif  // _APPDOMAIN_I




