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
** Header: DelegateInfo.h
**       
**
** Purpose: Native methods on System.ThreadPool
**          and its inner classes
**
** Date:  August, 1999
** 
===========================================================*/
#ifndef DELEGATE_INFO
#define DELEGATE_INFO

#include "security.h"
#include "threadpool.h"


struct DelegateInfo;
typedef DelegateInfo* DelegateInfoPtr;

struct DelegateInfo
{
    ADID            m_appDomainId;
    OBJECTHANDLE    m_stateHandle;
    OBJECTHANDLE    m_eventHandle;
    OBJECTHANDLE    m_registeredWaitHandle;
    DWORD           m_overridesCount;
    BOOL            m_hasSecurityInfo;
    



    void SetThreadSecurityInfo( Thread* thread, StackCrawlMark* stackMark )
    {
        CONTRACTL {
            THROWS;
            GC_TRIGGERS;
            MODE_COOPERATIVE; 
            INJECT_FAULT(COMPlusThrowOM());
        }
        CONTRACTL_END;


    }

    void Release()
    {
        CONTRACTL {
            // m_compressedStack->Release() can actually throw today because it has got a call
            // to new down the stack. However that is recent and the semantic of that api is such
            // it should not throw. I am expecting clenup of that function to take care of that
            // so I am adding this comment to make sure the issue is document.
            // Remove this comment once that work is done
            NOTHROW;
            GC_TRIGGERS;
            MODE_COOPERATIVE; 
            FORBID_FAULT;
        }
        CONTRACTL_END;


        AppDomainFromIDHolder ad(m_appDomainId, FALSE);
        if (!ad.IsUnloaded())
        {
            DestroyHandle(m_stateHandle);
            if (m_eventHandle)
            	 DestroyHandle(m_eventHandle);
            if (m_registeredWaitHandle)
               DestroyHandle(m_registeredWaitHandle);
        }

    }

    static DelegateInfo  *MakeDelegateInfo(AppDomain *pAppDomain,
                                           OBJECTREF *state,
                                           OBJECTREF *waitEvent,
                                           OBJECTREF *registeredWaitObject);
};





#endif // DELEGATE_INFO
