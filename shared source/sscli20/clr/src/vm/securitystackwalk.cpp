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
** Header:  COMCodeAccessSecurityEngine.cpp
**
**                                             
**
** Purpose:
**
** Date:  March 21, 1998
**
===========================================================*/
#include "common.h"

#include "security.h"
#include "perfcounters.h"
#include "threads.inl"
#include "stackcompressor.h"
#include "crossdomaincalls.h"
#include "appdomain.inl"
#include "appdomainstack.inl"

COUNTER_ONLY(PERF_COUNTER_TIMER_PRECISION g_TotalTimeInSecurityRuntimeChecks = 0);
COUNTER_ONLY(PERF_COUNTER_TIMER_PRECISION g_LastTimeInSecurityRuntimeChecks = 0);
COUNTER_ONLY(UINT32 g_SecurityChecksIterations=0);

bool SecurityStackWalk::IsSpecialRunFrame(MethodDesc* pMeth)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    static MethodDesc* pMethEC_Run = NULL;
    if (pMethEC_Run == NULL)
        pMethEC_Run = g_Mscorlib.GetMethod(METHOD__EXECUTIONCONTEXT__RUN);
    static MethodDesc* pMethSC_Run = NULL;
    if (pMethSC_Run == NULL)
        pMethSC_Run = g_Mscorlib.GetMethod(METHOD__SECURITYCONTEXT__RUN);
    static MethodDesc* pMethCS_Run = NULL;
    if (pMethCS_Run == NULL)
        pMethCS_Run = g_Mscorlib.GetMethod(METHOD__COMPRESSED_STACK__RUN);
    return (pMeth == pMethEC_Run || pMeth == pMethSC_Run || pMeth == pMethCS_Run);
}

void SecurityStackWalk::CheckPermissionAgainstGrants(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    struct _gc {
        OBJECTREF orCS;
        OBJECTREF orGranted;
        OBJECTREF orRefused;
        OBJECTREF orDemand;
        OBJECTREF orToken;
        OBJECTREF orAssembly;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    gc.orCS = refCS;
    gc.orGranted = refGrants;
    gc.orRefused = refRefused;

    GCPROTECT_BEGIN(gc);

    // Switch into the destination context if necessary.
    ENTER_DOMAIN_PTR(pDomain,ADV_RUNNINGIN)  //have it on the stack
    {
        // Fetch input objects that might originate from a different appdomain,
        // marshalling if necessary.
        gc.orDemand = m_objects.GetObjects(pDomain, &gc.orToken);
        if(pAssembly)
            gc.orAssembly = pAssembly->GetExposedObject();

        PREPARE_NONVIRTUAL_CALLSITE(METHOD__SECURITY_ENGINE__CHECK_HELPER);

        DECLARE_ARGHOLDER_ARRAY(helperArgs, 8);
        helperArgs[ARGNUM_0] = OBJECTREF_TO_ARGHOLDER(gc.orCS);
        helperArgs[ARGNUM_1] = OBJECTREF_TO_ARGHOLDER(gc.orGranted);
        helperArgs[ARGNUM_2] = OBJECTREF_TO_ARGHOLDER(gc.orRefused);
        helperArgs[ARGNUM_3] = OBJECTREF_TO_ARGHOLDER(gc.orDemand);
        helperArgs[ARGNUM_4] = OBJECTREF_TO_ARGHOLDER(gc.orToken);
        helperArgs[ARGNUM_5] = PTR_TO_ARGHOLDER(pMethod);
        helperArgs[ARGNUM_6] = OBJECTREF_TO_ARGHOLDER(gc.orAssembly);
        helperArgs[ARGNUM_7] = DWORD_TO_ARGHOLDER(dclDemand);

        CALL_MANAGED_METHOD_NORET(helperArgs);
        
    }
    END_DOMAIN_TRANSITION;

    GCPROTECT_END();
}


void SecurityStackWalk::CheckSetAgainstGrants(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    struct _gc {
        OBJECTREF orCS;
        OBJECTREF orGranted;
        OBJECTREF orRefused;
        OBJECTREF orDemand;
        OBJECTREF orAssembly;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    gc.orCS = refCS;
    gc.orGranted = refGrants;
    gc.orRefused = refRefused;

    GCPROTECT_BEGIN(gc);

    // Switch into the destination context if necessary.
    ENTER_DOMAIN_PTR(pDomain,ADV_RUNNINGIN) //have it on the stack
    {
        // Fetch input objects that might originate from a different appdomain,
        // marshalling if necessary.
        gc.orDemand = m_objects.GetObject(pDomain);
        if(pAssembly)
            gc.orAssembly = pAssembly->GetExposedObject();

        PREPARE_NONVIRTUAL_CALLSITE(METHOD__SECURITY_ENGINE__CHECK_SET_HELPER);

        DECLARE_ARGHOLDER_ARRAY(helperArgs, 7);
        helperArgs[ARGNUM_0] = OBJECTREF_TO_ARGHOLDER(gc.orCS);
        helperArgs[ARGNUM_1] = OBJECTREF_TO_ARGHOLDER(gc.orGranted);
        helperArgs[ARGNUM_2] = OBJECTREF_TO_ARGHOLDER(gc.orRefused);
        helperArgs[ARGNUM_3] = OBJECTREF_TO_ARGHOLDER(gc.orDemand);
        helperArgs[ARGNUM_4] = PTR_TO_ARGHOLDER(pMethod);
        helperArgs[ARGNUM_5] = OBJECTREF_TO_ARGHOLDER(gc.orAssembly);
        helperArgs[ARGNUM_6] = DWORD_TO_ARGHOLDER(dclDemand);

        CALL_MANAGED_METHOD_NORET(helperArgs);
    }
    END_DOMAIN_TRANSITION;
        
    GCPROTECT_END();
}


void SecurityStackWalk::GetZoneAndOriginGrants(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    Thread *pThread = GetThread();

    struct _gc {
        OBJECTREF orCS;
        OBJECTREF orGranted;
        OBJECTREF orRefused;
        OBJECTREF orZoneList;
        OBJECTREF orOriginList;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    gc.orCS = refCS;
    gc.orGranted = refGrants;
    gc.orRefused = refRefused;

    GCPROTECT_BEGIN(gc);

    // Fetch input objects that might originate from a different appdomain,
    // marshalling if necessary.
    gc.orZoneList = m_objects.GetObjects(pDomain, &gc.orOriginList);

    // Switch into the destination context if necessary.
    ENTER_DOMAIN_PTR(pDomain,ADV_RUNNINGIN) //have it on the stack
    {

        BOOL inProgress = pThread->IsSecurityStackwalkInProgess();

        // We turn security stackwalk in progress off which turns security back
        // on for a thread.  This means that if the managed call throws an exception
        // we are already in the proper state so we don't need to do anything.

        if (inProgress)
            pThread->SetSecurityStackwalkInProgress(FALSE);

        MethodDescCallSite getZoneAndOriginHelper(METHOD__SECURITY_ENGINE__GET_ZONE_AND_ORIGIN_HELPER);

        ARG_SLOT helperArgs[5];

        helperArgs[0] = ObjToArgSlot(gc.orCS);
        helperArgs[1] = ObjToArgSlot(gc.orGranted);
        helperArgs[2] = ObjToArgSlot(gc.orRefused);
        helperArgs[3] = ObjToArgSlot(gc.orZoneList);
        helperArgs[4] = ObjToArgSlot(gc.orOriginList);

        getZoneAndOriginHelper.Call(&(helperArgs[0]));

        if (inProgress)
            pThread->SetSecurityStackwalkInProgress(TRUE);
    }
    END_DOMAIN_TRANSITION;

    GCPROTECT_END();
}

BOOL SecurityStackWalk::CheckPermissionAgainstFrameData(OBJECTREF refFrameData, AppDomain* pDomain, MethodDesc* pMethod)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    BOOL ret = FALSE;

    struct _gc {
        OBJECTREF orFrameData;
        OBJECTREF orDemand;
        OBJECTREF orToken;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    gc.orFrameData = refFrameData;

    GCPROTECT_BEGIN(gc);

    // Fetch input objects that might originate from a different appdomain,
    // marshalling if necessary.
    gc.orDemand = m_objects.GetObjects(pDomain, &gc.orToken);

    // Switch into the destination context if necessary.
    ENTER_DOMAIN_PTR(pDomain,ADV_RUNNINGIN) //have it on the stack
    {
        PREPARE_NONVIRTUAL_CALLSITE(METHOD__SECURITY_RUNTIME__FRAME_DESC_HELPER);

        DECLARE_ARGHOLDER_ARRAY(args, 4);
        args[ARGNUM_0]  = OBJECTREF_TO_ARGHOLDER(gc.orFrameData);    // arg 0
        args[ARGNUM_1]  = OBJECTREF_TO_ARGHOLDER(gc.orDemand);       // arg 1 
        args[ARGNUM_2]  = OBJECTREF_TO_ARGHOLDER(gc.orToken);        // arg 2
        args[ARGNUM_3]  = PTR_TO_ARGHOLDER(pMethod);                              // arg 3 

        CALL_MANAGED_METHOD(ret, Bool, BOOL, args);

    }
    END_DOMAIN_TRANSITION;

    GCPROTECT_END();

    return (ret != FALSE);
}

BOOL SecurityStackWalk::CheckSetAgainstFrameData(OBJECTREF refFrameData, AppDomain* pDomain, MethodDesc* pMethod)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    BOOL ret = FALSE;

    struct _gc {
        OBJECTREF orFrameData;
        OBJECTREF orDemand;
        OBJECTREF orPermSetOut;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    gc.orFrameData = refFrameData;

    GCPROTECT_BEGIN(gc);

    // Fetch input objects that might originate from a different appdomain,
    // marshalling if necessary.
    gc.orDemand = m_objects.GetObject(pDomain);

    // Switch into the destination context if necessary.
    ENTER_DOMAIN_PTR(pDomain,ADV_RUNNINGIN) //have it on the stack
    {
        PREPARE_NONVIRTUAL_CALLSITE(METHOD__SECURITY_RUNTIME__FRAME_DESC_SET_HELPER);

        DECLARE_ARGHOLDER_ARRAY(args, 4);
        args[ARGNUM_0] = OBJECTREF_TO_ARGHOLDER(gc.orFrameData);
        args[ARGNUM_1] = OBJECTREF_TO_ARGHOLDER(gc.orDemand);
        args[ARGNUM_2] = PTR_TO_ARGHOLDER(&gc.orPermSetOut);
        args[ARGNUM_3] = PTR_TO_ARGHOLDER(pMethod);

        CALL_MANAGED_METHOD(ret, Bool, BOOL, args);
        
        if (gc.orPermSetOut != NULL) {
            // Update the cached object.
            m_objects.UpdateObject(pDomain, gc.orPermSetOut);
        }
    }
    END_DOMAIN_TRANSITION;

    GCPROTECT_END();

    return (ret != FALSE);
}


















// -------------------------------------------------------------------------------
//
//                                 DemandStackWalk
//
// -------------------------------------------------------------------------------

class DemandStackWalk : public SecurityStackWalk
{
public:
    enum DemandType
    {
        DT_PERMISSION = 1,
        DT_SET = 2,
        DT_ZONE_AND_URL = 3,
    };

protected:
    Frame*                  m_pCtxTxFrame;
    AppDomain *             m_pPrevAppDomain;
    AppDomain*              m_pSkipAppDomain;
    Assembly *              m_pPrevAssembly;
    StackCrawlMark *        m_pStackMark;
    DemandType              m_eDemandType;
    bool                    m_bHaveFoundStartingFrameYet;
    BOOL                    m_bSkippingRemoting;
    BOOL                    m_bFoundStackMark;
    BOOL                    m_bUnrestrictedOverride;
    DWORD                   m_dwdemandFlags;
    DWORD                   m_adStackIndex;
    AppDomainStack*         m_pThreadADStack;

public:
    DemandStackWalk(SecurityStackWalkType eType, DWORD flags, StackCrawlMark* stackMark, DemandType eDemandType, BOOL bCanUnrestrictedOverride, DWORD demandFlags)
        : SecurityStackWalk(eType, flags)
    {
        m_pCtxTxFrame = NULL;
        m_bSkippingRemoting = FALSE;
        m_pPrevAppDomain = NULL;
        m_pSkipAppDomain = NULL;
        m_pPrevAssembly = NULL;
        m_eDemandType = eDemandType;
        m_bHaveFoundStartingFrameYet = false;
        m_pStackMark = stackMark;
        m_bFoundStackMark = FALSE;
        m_bUnrestrictedOverride = bCanUnrestrictedOverride;
        m_dwdemandFlags = demandFlags;
        m_pThreadADStack = GetThread()->GetAppDomainStackPointer();
        m_pThreadADStack->InitDomainIteration(&m_adStackIndex);
    }

    void DoStackWalk();
    StackWalkAction WalkFrame(CrawlFrame* pCf);

protected:
    bool IsStartingFrame(CrawlFrame* pCf);
    bool IsInternalFrame(MethodDesc* pMeth, CrawlFrame* pCf);
    bool IsSpecialRunFrame(MethodDesc* pMeth)
    {
        return SecurityStackWalk::IsSpecialRunFrame(pMeth);
    }
    void CheckGrant(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly);
    BOOL CheckFrame(OBJECTREF refFrameData, AppDomain* pDomain, MethodDesc* pMethod);

private:
    FORCEINLINE BOOL QuickCheck(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused)
    {
        if (refCS == NULL && refRefused == NULL && refGrants != NULL && m_bUnrestrictedOverride)
        {
            // if we have a FT grant and nothing else, and our demand is for something that FT can satisfy, we're done
            PERMISSIONSETREF permSetRef = (PERMISSIONSETREF)refGrants;
            return permSetRef->IsUnrestricted();
        }
        return FALSE;
    }
    void ProcessAppDomainTransition(AppDomain * pAppDomain, bool bCheckPrevAppDomain);
};

void DemandStackWalk::CheckGrant(OBJECTREF refCS, OBJECTREF refGrants, OBJECTREF refRefused, AppDomain *pDomain, MethodDesc* pMethod, Assembly* pAssembly)
{
    switch(m_eDemandType)
    {
        case DT_PERMISSION:
            // Test early out scenario (quickcheck) before calling into managed code
            if (!QuickCheck(refCS, refGrants, refRefused))
            CheckPermissionAgainstGrants(refCS, refGrants, refRefused, pDomain, pMethod, pAssembly);
            break;

        case DT_SET:
            // Test early out scenario (quickcheck) before calling into managed code
            if (!QuickCheck(refCS, refGrants, refRefused))
            CheckSetAgainstGrants(refCS, refGrants, refRefused, pDomain, pMethod, pAssembly);
            break;

        case DT_ZONE_AND_URL:
            GetZoneAndOriginGrants(refCS, refGrants, refRefused, pDomain, pMethod, pAssembly);
            break;

        default:
            _ASSERTE(!"unexpected demand type");
            break;
    }
}

BOOL DemandStackWalk::CheckFrame(OBJECTREF refFrameData, AppDomain* pDomain, MethodDesc* pMethod)
{
    switch(m_eDemandType)
    {
        case DT_PERMISSION:
            return CheckPermissionAgainstFrameData(refFrameData, pDomain, pMethod);

        case DT_SET:
            return CheckSetAgainstFrameData(refFrameData, pDomain, pMethod);

        case DT_ZONE_AND_URL:
            return TRUE; //Nothing to do here since CS cannot live on a Frame anymore.

        default:
            _ASSERTE(!"unexpected demand type");
    }
    return TRUE;
}

bool DemandStackWalk::IsInternalFrame(MethodDesc* pMeth, CrawlFrame* pCf)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    static MethodDesc* pMethPrivateProcessMessage = NULL;
    if (pMethPrivateProcessMessage == NULL)
        pMethPrivateProcessMessage = g_Mscorlib.FetchMethod(METHOD__STACK_BUILDER_SINK__PRIVATE_PROCESS_MESSAGE);

    if (pMeth == pMethPrivateProcessMessage)
    {
        _ASSERTE(!m_bSkippingRemoting);
        m_bSkippingRemoting = TRUE;
        return true;
    }
    if (!pCf->IsFrameless() && pCf->GetFrame()->GetFrameType() == Frame::TYPE_TP_METHOD_FRAME)
    {
        m_bSkippingRemoting = FALSE;
        return true;
    }
    if (m_bSkippingRemoting)
        return true;

    static MethodTable* pTypeRuntimeMethodInfo = NULL;
    if (pTypeRuntimeMethodInfo == NULL)
        pTypeRuntimeMethodInfo = g_Mscorlib.FetchClass(CLASS__METHOD);
    static MethodTable* pTypeMethodBase = NULL;
    if (pTypeMethodBase == NULL)
        pTypeMethodBase = g_Mscorlib.FetchClass(CLASS__METHOD_BASE);
    static MethodTable* pTypeRuntimeConstructorInfo = NULL;
    if (pTypeRuntimeConstructorInfo == NULL)
        pTypeRuntimeConstructorInfo = g_Mscorlib.FetchClass(CLASS__CONSTRUCTOR);
    static MethodTable* pTypeConstructorInfo = NULL;
    if (pTypeConstructorInfo == NULL)
        pTypeConstructorInfo = g_Mscorlib.FetchClass(CLASS__CONSTRUCTOR_INFO);
    static MethodTable* pTypeRuntimeType = NULL;
    if (pTypeRuntimeType == NULL)
        pTypeRuntimeType = g_Mscorlib.FetchClass(CLASS__CLASS);
    static MethodTable* pTypeType = NULL;
    if (pTypeType == NULL)
        pTypeType = g_Mscorlib.FetchClass(CLASS__TYPE);
    static MethodTable* pTypeRuntimeEventInfo = NULL;
    if (pTypeRuntimeEventInfo == NULL)
        pTypeRuntimeEventInfo = g_Mscorlib.FetchClass(CLASS__EVENT);
    static MethodTable* pTypeEventInfo = NULL;
    if (pTypeEventInfo == NULL)
        pTypeEventInfo = g_Mscorlib.FetchClass(CLASS__EVENT_INFO);
    static MethodTable* pTypeRuntimePropertyInfo = NULL;
    if (pTypeRuntimePropertyInfo == NULL)
        pTypeRuntimePropertyInfo = g_Mscorlib.FetchClass(CLASS__PROPERTY);
    static MethodTable* pTypePropertyInfo = NULL;
    if (pTypePropertyInfo == NULL)
        pTypePropertyInfo = g_Mscorlib.FetchClass(CLASS__PROPERTY_INFO);
    static MethodTable* pTypeActivator = NULL;
    if (pTypeActivator == NULL)
        pTypeActivator = g_Mscorlib.FetchClass(CLASS__ACTIVATOR);
    static MethodTable* pTypeAppDomain = NULL;
    if (pTypeAppDomain == NULL)
        pTypeAppDomain = g_Mscorlib.FetchClass(CLASS__APP_DOMAIN);
    static MethodTable* pTypeAssembly = NULL;
    if (pTypeAssembly == NULL)
        pTypeAssembly = g_Mscorlib.FetchClass(CLASS__ASSEMBLY);

    MethodTable *pMT = pMeth->GetMethodTable();
    if (pMT == pTypeRuntimeMethodInfo ||
        pMT == pTypeMethodBase ||
        pMT == pTypeRuntimeConstructorInfo ||
        pMT == pTypeConstructorInfo ||
        pMT == pTypeRuntimeType ||
        pMT == pTypeType ||
        pMT == pTypeRuntimeEventInfo ||
        pMT == pTypeEventInfo ||
        pMT == pTypeRuntimePropertyInfo ||
        pMT == pTypePropertyInfo ||
        pMT == pTypeActivator ||
        pMT == pTypeAppDomain ||
        pMT == pTypeAssembly)
    {
        return true;
    }

    return false;
}


inline bool IsSecurityStub(CrawlFrame* pCf)
{
    if(pCf->IsFrameless())
        return false;
    if(pCf->GetFrame()->GetVTablePtr() != InterceptorFrame::GetMethodFrameVPtr())
        return false;
    return true;
}


bool DemandStackWalk::IsStartingFrame(CrawlFrame* pCf)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    switch(m_eStackWalkType)
    {
        case SSWT_DECLARATIVE_DEMAND: // Begin after the security stub(s)
            _ASSERTE(m_pStackMark == NULL);
			if (Security::UseNoStubsApproach())
			{
				// no stubs model, skip the current method that has decl sec
              if (m_bFoundStackMark)
                    return true;
                else
                {
                    m_bFoundStackMark = true;
                    return false;            
                }
			}
			else
			{	// old stubs model
            	if (IsSecurityStub(pCf))
            	{
                	m_bFoundStackMark = TRUE;
	                return false;
    	        }
        	    else
            	{
                	if (m_bFoundStackMark)
                    	return true;
	                else
    	                return false;
        	    }
			}

        case SSWT_IMPERATIVE_DEMAND: // Begin where the StackMark says to
        case SSWT_GET_ZONE_AND_URL: // Begin where the StackMark says to
            _ASSERTE(*m_pStackMark == LookForMyCaller || *m_pStackMark == LookForMyCallersCaller);

            // See if we've passed the stack mark yet
            if (!IsInCalleesFrames(pCf->GetRegisterSet(), m_pStackMark))
                return false;

            // Skip the frame after the stack mark as well.
            if(*m_pStackMark == LookForMyCallersCaller && !m_bFoundStackMark)
            {
                m_bFoundStackMark = TRUE;
                return false;
            }

            // Don't start on a declarative security stub
            if (IsSecurityStub(pCf))
                return false;

            return true;

        case SSWT_LATEBOUND_LINKDEMAND: // Begin immediately
        case SSWT_DEMAND_FROM_NATIVE:
            _ASSERTE(m_pStackMark == NULL);
            return true;

        default:
            _ASSERTE(FALSE); // Unexpected stack walk type
            break;
    }
    return true;
}
void DemandStackWalk::ProcessAppDomainTransition(AppDomain* pAppDomain, bool bCheckPrevAppDomain)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    _ASSERTE(pAppDomain != m_pPrevAppDomain);

    if (m_pPrevAppDomain != NULL && bCheckPrevAppDomain)
    {
        // We have not checked the previous AppDomain. Check it now.
        if (m_pSkipAppDomain != m_pPrevAppDomain)
        {
            ApplicationSecurityDescriptor *pSecDesc = 
                m_pPrevAppDomain->GetSecurityDescriptor();

            if (pSecDesc)
            {
                DBG_TRACE_STACKWALK("            Checking appdomain...\n", true);

                if ((!pSecDesc->IsDefaultAppDomain()) && (!pSecDesc->IsFullyTrusted() || !m_bUnrestrictedOverride) &&
                    !pSecDesc->CheckSpecialFlag(m_dwdemandFlags))
                {
                    OBJECTREF orRefused;
                    OBJECTREF orGranted = pSecDesc->GetGrantedPermissionSet(&orRefused);
                    CheckGrant(NULL, orGranted, orRefused, m_pPrevAppDomain, NULL, m_pPrevAssembly);
                }
            }
            else
            {
                DBG_TRACE_STACKWALK("            Skipping appdomain...\n", true);
            }
        }
    }
    // Move the domain index forward 
    m_pThreadADStack->GetNextDomainEntryOnStack(&m_adStackIndex);

    // At the end of the stack walk, do a check on the grants of
    // the m_pPrevAppDomain by the stackwalk caller if needed.
    m_pPrevAppDomain = pAppDomain;

    if ((m_pThreadADStack->GetCurrentDomainEntryOnStack(m_adStackIndex))->HasFlagsOrFullyTrustedWithNoStackModifiers(m_dwdemandFlags))
        m_pSkipAppDomain = pAppDomain;
    else
        m_pSkipAppDomain = NULL;


    
}
StackWalkAction DemandStackWalk::WalkFrame(CrawlFrame* pCf)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Save the CtxTxFrame if this is one
    if (m_pCtxTxFrame == NULL)
    {
        Frame *pFrame = pCf->GetFrame();
        if (SecurityStackWalk::IsContextTransitionFrameWithCS(pFrame))
        {
            m_pCtxTxFrame = pFrame;
        }
    }

    MethodDesc * pFunc = pCf->GetFunction();
    Assembly * pAssem = pCf->GetAssembly();
    // Get the current app domain.
    AppDomain *pAppDomain = pCf->GetAppDomain();
    if (pAppDomain != m_pPrevAppDomain)
        ProcessAppDomainTransition(pAppDomain, m_bHaveFoundStartingFrameYet);
    
    if ((pFunc == NULL && pAssem == NULL) || (pFunc && pFunc->IsILStub()))
        return SWA_CONTINUE; // Not a function

    // Skip until the frame where the stackwalk should begin
    if (!m_bHaveFoundStartingFrameYet)
    {
        if (IsStartingFrame(pCf))
            m_bHaveFoundStartingFrameYet = true;
        else
            return SWA_CONTINUE;
    }

    // Skip internal frames
    if (m_dwFlags & CORSEC_SKIP_INTERNAL_FRAMES)
    {
        if(pFunc && IsInternalFrame(pFunc, pCf))
            return SWA_CONTINUE;
    }

    //
    // Now check the current frame!
    //

    DBG_TRACE_STACKWALK("        Checking granted permissions for current method...\n", true);
    
    // Reached here imples we walked atleast a single frame.
    COUNTER_ONLY(GetPrivatePerfCounters().m_Security.stackWalkDepth++);


    // Get the previous assembly
    Assembly *pPrevAssem = m_pPrevAssembly;


    // Check if we can skip the entire appdomain
    if (m_pSkipAppDomain == pAppDomain)
    {
        return SWA_CONTINUE;
    }
        
    // Keep track of the last module checked. If we have just checked the
    // permissions on the module, we don't need to do it again.
    if (pAssem != pPrevAssem)
    {
        DBG_TRACE_STACKWALK("            Checking grants for current assembly.\n", true);

        // Get the security descriptor for the current assembly and pass it to
        // the interpreted helper.
        AssemblySecurityDescriptor * pSecDesc = pAssem->GetSecurityDescriptor(pAppDomain);
        _ASSERTE(pSecDesc != NULL);

        // We have to check the permissions if we are not fully trusted or
        // we cannot be overrided by full trust.  Plus we always skip checks
        // on system classes.
        if (!pSecDesc->IsSystem() && (!pSecDesc->IsFullyTrusted() || !m_bUnrestrictedOverride) &&
            !pSecDesc->CheckSpecialFlag(m_dwdemandFlags))
        {
            OBJECTREF orRefused;
            OBJECTREF orGranted = pSecDesc->GetGrantedPermissionSet(&orRefused);
            CheckGrant(NULL, orGranted, orRefused, pAppDomain, pFunc, pAssem);
        }

        m_pPrevAssembly = pAssem;
    }
    else
    {
        DBG_TRACE_STACKWALK("            Current assembly same as previous. Skipping check.\n", true);
    }


    // Passed initial check. See if there is security info on this frame.
    OBJECTREF *pFrameObjectSlot = pCf->GetAddrOfSecurityObject();
    if (pFrameObjectSlot != NULL)
    {
        SecurityDeclarative::DoDeclarativeSecurityAtStackWalk(pFunc, pAppDomain, pFrameObjectSlot);
        if (*pFrameObjectSlot != NULL)
        {
            DBG_TRACE_STACKWALK("        + Frame-specific security info found. Checking...\n", false);

            if(!CheckFrame(*pFrameObjectSlot, pAppDomain, pFunc))
            {
                DBG_TRACE_STACKWALK("            Halting stackwalk for assert.\n", false);
                m_dwFlags |= CORSEC_STACKWALK_HALTED;
                return SWA_ABORT;
            }
        }
    }

    DBG_TRACE_STACKWALK("        Check passes for this method.\n", true);

    // If this is a *.Run method, then we need to terminate the stackwalk
    if (pFunc && IsSpecialRunFrame(pFunc))
    {
        DBG_TRACE_STACKWALK("            Halting stackwalk for .Run.\n", false);
        // Dont mark the CORSEC_STACKWALK_HALTED in m_dwFlags because we still need to look at the CS 
        return SWA_ABORT;
    }
    // Passed all the checks, so continue.
    return SWA_CONTINUE;
}

static
StackWalkAction CodeAccessCheckStackWalkCB(CrawlFrame* pCf, VOID* pData)
{
    DemandStackWalk *pCBdata = (DemandStackWalk*)pData;
    return pCBdata->WalkFrame(pCf);
}

void DemandStackWalk::DoStackWalk()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Get the current thread.
    Thread *pThread = GetThread();
    _ASSERTE(pThread != NULL);

    // Don't allow recursive security stackwalks. Note that this implies that
    // *no* untrusted code must ever be called during a security stackwalk.
    if (pThread->IsSecurityStackwalkInProgess())
        return;

    // NOTE: Initialize the stack depth. Note that if more that one thread tries
    // to perform stackwalk then these counters gets stomped upon. 
    COUNTER_ONLY(GetPrivatePerfCounters().m_Security.stackWalkDepth = 0);

    // Walk the thread.
    EX_TRY
    {
        pThread->SetSecurityStackwalkInProgress( TRUE );

        DBG_TRACE_STACKWALK("Code-access security check invoked.\n", false);
        // LIGHTUNWIND flag: allow using stackwalk cache for security stackwalks
        pThread->StackWalkFrames(CodeAccessCheckStackWalkCB, this, SKIPFUNCLETS | LIGHTUNWIND);
        DBG_TRACE_STACKWALK("\tCode-access stackwalk completed.\n", false);

        // check the last app domain or CompressedStack at the thread base
        if (((m_dwFlags & CORSEC_STACKWALK_HALTED) == 0) /*&& m_cCheck != 0*/)
        {
            OBJECTREF orCS = pThread->GetCompressedStack();
            AppDomain *pAppDomain = m_pPrevAppDomain;

            if (orCS == NULL)
            {
                // There may have been an AD transition and we shd look at the CB data to see if this is the case
                if (m_pCtxTxFrame != NULL)
                {
                    orCS = (OBJECTREF)SecurityStackWalk::GetCSFromContextTransitionFrame(m_pCtxTxFrame);
                    pAppDomain = m_pCtxTxFrame->GetReturnDomain();
                }
            }
            

            if (orCS != NULL)
            {
                // We have a CS at the thread base - just look at that. Dont look at the last AD
                DBG_TRACE_STACKWALK("\tChoosing CompressedStack check.\n", true);
                DBG_TRACE_STACKWALK("\tChecking CompressedStack...\n", true);
                
                CheckGrant(orCS, NULL, NULL, pAppDomain, NULL, NULL);
                DBG_TRACE_STACKWALK("\tCompressedStack check passed.\n", true);
            }
            else
            {
                // No CS at thread base - must look at the last AD
                DBG_TRACE_STACKWALK("\tChoosing appdomain check.\n", true);

                ApplicationSecurityDescriptor *pSecDesc = pAppDomain->GetSecurityDescriptor();
        
                if (pSecDesc != NULL)
                {
                    // Note: the order of these calls is important since you have to have done a
                    // GetEvidence() on the security descriptor before you check for the
                    // CORSEC_DEFAULT_APPDOMAIN property.  IsFullyTrusted calls Resolve so
                    // we're all good.
                    if ((!pSecDesc->IsDefaultAppDomain()) && (!pSecDesc->IsFullyTrusted() || !m_bUnrestrictedOverride) &&
                        !pSecDesc->CheckSpecialFlag(m_dwdemandFlags))
                    {
                        DBG_TRACE_STACKWALK("\tChecking appdomain...\n", true);
                        OBJECTREF orRefused;
                        OBJECTREF orGranted = pSecDesc->GetGrantedPermissionSet(&orRefused);
                        CheckGrant(NULL, orGranted, orRefused, pAppDomain, NULL, NULL);
                        DBG_TRACE_STACKWALK("\tappdomain check passed.\n", true);
                    }
                }
                else
                {
                    DBG_TRACE_STACKWALK("\tSkipping appdomain check.\n", true);
                }
            }
        }
        else
        {
            DBG_TRACE_STACKWALK("\tSkipping CS/appdomain check.\n", true);
        }

        pThread->SetSecurityStackwalkInProgress( FALSE );
    }
    EX_CATCH
    {
        // We catch exceptions and rethrow like this to ensure that we've
        // established an exception handler on the fs:[0] chain (managed
        // exception handlers won't do this). This in turn guarantees that
        // managed exception filters in any of our callers won't be found,
        // otherwise they could get to execute untrusted code with security
        // turned off.
        pThread->SetSecurityStackwalkInProgress( FALSE );

        EX_RETHROW;
    }
    EX_END_CATCH_UNREACHABLE
    

    DBG_TRACE_STACKWALK("Code-access check passed.\n", false);
}


















// -------------------------------------------------------------------------------
//
//                                 AssertStackWalk
//
// -------------------------------------------------------------------------------

class AssertStackWalk : public SecurityStackWalk
{
protected:
    StackCrawlMark *        m_pStackMark;
    bool                    m_bHaveFoundStartingFrameYet;
    INT_PTR                 m_cCheck;
    BOOL                    m_bUnrestrictedOverride;

public:
    OBJECTREF*              m_pSecurityObject;
    AppDomain*              m_pSecurityObjectDomain;

    AssertStackWalk(SecurityStackWalkType eType, DWORD dwFlags, StackCrawlMark* stackMark, BOOL bCanUnrestrictedOverride)
        : SecurityStackWalk(eType, dwFlags)
    {
        m_pStackMark = stackMark;
        m_bHaveFoundStartingFrameYet = false;
        m_cCheck = 1;
        m_pSecurityObject = NULL;
        m_pSecurityObjectDomain = NULL;
        m_bUnrestrictedOverride = bCanUnrestrictedOverride;
    }

    void DoStackWalk();
    StackWalkAction WalkFrame(CrawlFrame* pCf);
};

StackWalkAction AssertStackWalk::WalkFrame(CrawlFrame* pCf)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    DBG_TRACE_METHOD(pCf);

    MethodDesc * pFunc = pCf->GetFunction();
    _ASSERTE(pFunc != NULL); // we requested functions only!
    _ASSERTE(m_eStackWalkType == SSWT_IMPERATIVE_ASSERT);
    _ASSERTE(*m_pStackMark == LookForMyCaller);
    _ASSERTE((m_dwFlags & CORSEC_SKIP_INTERNAL_FRAMES) == 0);

    // Skip until we pass the StackMark
    if (!m_bHaveFoundStartingFrameYet)
    {
        if (IsInCalleesFrames(pCf->GetRegisterSet(), m_pStackMark))
            m_bHaveFoundStartingFrameYet = true;
        else
            return SWA_CONTINUE;
    }

    // Check if we've visited the maximum number of frames
    if (m_cCheck >= 0)
    {
        if (m_cCheck == 0)
        {
            m_dwFlags |= CORSEC_STACKWALK_HALTED;
            return SWA_ABORT;
        }
        else
            --m_cCheck;
    }

    // Reached here imples we walked atleast a single frame.
    COUNTER_ONLY(GetPrivatePerfCounters().m_Security.stackWalkDepth++);

    DBG_TRACE_STACKWALK("            Checking grants for current assembly.\n", true);

    // Get the security descriptor for the current assembly and pass it to
    // the interpreted helper.
    // Get the current assembly
    Assembly *pAssem = pFunc->GetModule()->GetAssembly();
    AppDomain *pAppDomain = pCf->GetAppDomain();
    AssemblySecurityDescriptor * pSecDesc = pAssem->GetSecurityDescriptor(pAppDomain);
    _ASSERTE(pSecDesc != NULL);
     
    
    if (SecurityTransparent::IsMethodTransparent(pFunc))
    {
        // Transparent method can't have the permission to Assert
        COMPlusThrow(kInvalidOperationException,L"InvalidOperation_AssertTransparentCode");
    }

    if (!pSecDesc->IsSystem() && (!pSecDesc->IsFullyTrusted() || !m_bUnrestrictedOverride))
    {
        OBJECTREF orRefused;
        OBJECTREF orGranted = pSecDesc->GetGrantedPermissionSet(&orRefused);
        CheckPermissionAgainstGrants(NULL, orGranted, orRefused, pAppDomain, pFunc, pAssem);  
    }

    // Passed initial check. See if there is security info on this frame.
    m_pSecurityObject = pCf->GetAddrOfSecurityObject();
    m_pSecurityObjectDomain = pAppDomain;

    DBG_TRACE_STACKWALK("        Check Immediate passes for this method.\n", true);

    // Passed all the checks, so continue.
    return SWA_ABORT;
}

static
StackWalkAction CheckNReturnSOStackWalkCB(CrawlFrame* pCf, VOID* pData)
{
    AssertStackWalk *pCBdata = (AssertStackWalk*)pData;
    return pCBdata->WalkFrame(pCf);
}

FCIMPL5(Object*, SecurityStackWalk::CheckNReturnSO, Object* permTokenUNSAFE, Object* permUNSAFE, StackCrawlMark* stackMark, INT32 unrestrictedOverride, INT32 create)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    OBJECTREF refRetVal = NULL;
    OBJECTREF permToken = (OBJECTREF) permTokenUNSAFE;
    OBJECTREF perm      = (OBJECTREF) permUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_2(Frame::FRAME_ATTR_RETURNOBJ, permToken, perm);
    
    //-[autocvtpro]-------------------------------------------------------

    if (!SecurityPolicy::IsSecurityOff())
    {
        _ASSERTE((permToken != NULL) && (perm != NULL));

        // Track perfmon counters. Runtime security checkes.
        IncrementSecurityPerfCounter();

#if defined(ENABLE_PERF_COUNTERS)
        // Perf Counter "%Time in Runtime check" support
        PERF_COUNTER_TIMER_PRECISION _startPerfCounterTimer = GET_CYCLE_COUNT();
#endif

        // Initialize callback data.
        DWORD dwFlags = 0;
        AssertStackWalk walkData(SSWT_IMPERATIVE_ASSERT, dwFlags, stackMark, unrestrictedOverride);
        walkData.m_objects.SetObjects(perm, permToken);

        // Protect the object references in the callback data.
        GCPROTECT_BEGIN(walkData.m_objects.m_sGC);

        walkData.DoStackWalk();

        GCPROTECT_END();

#if defined(ENABLE_PERF_COUNTERS)
        // Accumulate the counter
        PERF_COUNTER_TIMER_PRECISION _stopPerfCounterTimer = GET_CYCLE_COUNT();
        g_TotalTimeInSecurityRuntimeChecks += _stopPerfCounterTimer - _startPerfCounterTimer;

        // Report the accumulated counter only after NUM_OF_TERATIONS
        if (g_SecurityChecksIterations++ > PERF_COUNTER_NUM_OF_ITERATIONS)
        {
            GetPrivatePerfCounters().m_Security.timeRTchecks = g_TotalTimeInSecurityRuntimeChecks;
            GetPrivatePerfCounters().m_Security.timeRTchecksBase = (_stopPerfCounterTimer - g_LastTimeInSecurityRuntimeChecks);
            
            g_TotalTimeInSecurityRuntimeChecks = 0;
            g_LastTimeInSecurityRuntimeChecks = _stopPerfCounterTimer;
            g_SecurityChecksIterations = 0;
        }
#endif // #if defined(ENABLE_PERF_COUNTERS)

        if (walkData.m_pSecurityObject == NULL)
        {
            goto lExit;
        }

        // Is security object frame in a different context?
        Thread *pThread = GetThread();
        bool fSwitchContext = walkData.m_pSecurityObjectDomain != pThread->GetDomain();

        if (create && *walkData.m_pSecurityObject == NULL)
        {
            // If necessary, shift to correct context to allocate security object.
            ENTER_DOMAIN_PTR(walkData.m_pSecurityObjectDomain,ADV_RUNNINGIN) //on the stack
            {
                static MethodTable* pMethFrameSecDesc = NULL;
                if (pMethFrameSecDesc == NULL)
                    pMethFrameSecDesc = g_Mscorlib.GetClass(CLASS__FRAME_SECURITY_DESCRIPTOR);

                *walkData.m_pSecurityObject = AllocateObject(pMethFrameSecDesc);
            }
            END_DOMAIN_TRANSITION;
        }

        // If we found or created a security object in a different context, make a
        // copy in the current context.
        if (fSwitchContext && *walkData.m_pSecurityObject != NULL)
            refRetVal = AppDomainHelper::CrossContextCopyFrom(walkData.m_pSecurityObjectDomain, 
                                                                    walkData.m_pSecurityObject);
        else
            refRetVal = *walkData.m_pSecurityObject;

    }

lExit: ;
    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(refRetVal);
}
FCIMPLEND


void AssertStackWalk::DoStackWalk()
{
    // Get the current thread.
    Thread *pThread = GetThread();
    _ASSERTE(pThread != NULL);

    // NOTE: Initialize the stack depth. Note that if more that one thread tries
    // to perform stackwalk then these counters gets stomped upon. 
    COUNTER_ONLY(GetPrivatePerfCounters().m_Security.stackWalkDepth = 0);

    // Walk the thread.
    DBG_TRACE_STACKWALK("Code-access security check immediate invoked.\n", false);
    // LIGHTUNWIND flag: allow using stackwalk cache for security stackwalks
    pThread->StackWalkFrames(CheckNReturnSOStackWalkCB, this, FUNCTIONSONLY | SKIPFUNCLETS | LIGHTUNWIND);

    DBG_TRACE_STACKWALK("\tCode-access stackwalk completed.\n", false);
}


















// -------------------------------------------------------------------------------
//
//                                 CountOverridesStackWalk
//
// -------------------------------------------------------------------------------

typedef struct _SkipFunctionsData
{
    INT32           cSkipFunctions;
    StackCrawlMark* pStackMark;
    BOOL            bUseStackMark;
    BOOL            bFoundCaller;
    MethodDesc*     pFunction;
    OBJECTREF*      pSecurityObject;
    AppDomain*      pSecurityObjectAppDomain;
} SkipFunctionsData;

static StackWalkAction SkipFunctionsCB(CrawlFrame* pCf, VOID* pData)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;
    SkipFunctionsData *skipData = (SkipFunctionsData*)pData;
    _ASSERTE(skipData != NULL);

    MethodDesc *pFunc = pCf->GetFunction();

#ifdef _DEBUG
    // Get the interesting info now, so we can get a trace
    // while debugging...
    OBJECTREF  *pSecObj;
    pSecObj = pCf->GetAddrOfSecurityObject();
#endif

    _ASSERTE(skipData->bUseStackMark && "you must specify a stackmark");

    // First check if the walk has skipped the required frames. The check
    // here is between the address of a local variable (the stack mark) and a
    // pointer to the EIP for a frame (which is actually the pointer to the
    // return address to the function from the previous frame). So we'll
    // actually notice which frame the stack mark was in one frame later. This
    // is fine for our purposes since we're always looking for the frame of the
    // caller of the method that actually created the stack mark.
    if ((skipData->pStackMark != NULL) &&
        !IsInCalleesFrames(pCf->GetRegisterSet(), skipData->pStackMark))
        return SWA_CONTINUE;

    skipData->pFunction                 = pFunc;
    skipData->pSecurityObject           = pCf->GetAddrOfSecurityObject();
    skipData->pSecurityObjectAppDomain  = pCf->GetAppDomain();
    return SWA_ABORT; // This actually indicates success.
}

// Version of the above method that looks for a stack mark (the address of a
// local variable in a frame called by the target frame).
BOOL SecurityStackWalk::SkipAndFindFunctionInfo(StackCrawlMark* stackMark, MethodDesc ** ppFunc, OBJECTREF ** ppObj, AppDomain ** ppAppDomain)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;
    _ASSERTE(ppFunc != NULL || ppObj != NULL || !"Why was this function called?!");

    SkipFunctionsData walkData;
    walkData.pStackMark = stackMark;
    walkData.bUseStackMark = TRUE;
    walkData.bFoundCaller = FALSE;
    walkData.pFunction = NULL;
    walkData.pSecurityObject = NULL;
    // LIGHTUNWIND flag: allow using stackwalk cache for security stackwalks
    StackWalkAction action = GetThread()->StackWalkFrames(SkipFunctionsCB, &walkData, FUNCTIONSONLY | SKIPFUNCLETS | LIGHTUNWIND);
    if (action == SWA_ABORT)
    {
        if (ppFunc != NULL)
            *ppFunc = walkData.pFunction;
        if (ppObj != NULL)
        {
            *ppObj = walkData.pSecurityObject;
            if (ppAppDomain != NULL)
                *ppAppDomain = walkData.pSecurityObjectAppDomain;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}























// -------------------------------------------------------------------------------
//
//                                 CountOverridesStackWalk
//
// -------------------------------------------------------------------------------

class CountOverridesStackWalk
{
public:
    DWORD           numOverrides; // Can be removed
    DWORD           numAsserts; // Can be removed
    DWORD           numDomainOverrides;
    DWORD           numDomainAsserts;
    AppDomain*      prev_AppDomain;
    Frame*          pCtxTxFrame;
    DWORD           adStackIndex;

    CountOverridesStackWalk()
    {
        numOverrides = 0;
        numAsserts = 0;
        numDomainAsserts = 0;
        numDomainOverrides = 0;
        prev_AppDomain = NULL;
        pCtxTxFrame = NULL;
        GetThread()->InitDomainIteration(&adStackIndex);
    }
    bool IsSpecialRunFrame(MethodDesc* pMeth)
    {
        return SecurityStackWalk::IsSpecialRunFrame(pMeth);
    }
};

static 
StackWalkAction UpdateOverridesCountCB(CrawlFrame* pCf, void *pData)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    DBG_TRACE_METHOD(pCf);

    CountOverridesStackWalk *pCBdata = static_cast<CountOverridesStackWalk *>(pData);


    // First check if the walk has skipped the required frames. The check
    // here is between the address of a local variable (the stack mark) and a
    // pointer to the EIP for a frame (which is actually the pointer to the
    // return address to the function from the previous frame). So we'll
    // actually notice which frame the stack mark was in one frame later. This
    // is fine for our purposes since we're always looking for the frame of the
    // caller (or the caller's caller) of the method that actually created the
    // stack mark.

    // Save the CtxTxFrame if this is one
    if (pCBdata->pCtxTxFrame == NULL)
    {
        Frame *pFrame = pCf->GetFrame();
        if (SecurityStackWalk::IsContextTransitionFrameWithCS(pFrame))
        {
            pCBdata->pCtxTxFrame = pFrame;
        }
    }

    MethodDesc* pMeth = pCf->GetFunction();

    if ((pMeth == NULL || pMeth->IsILStub()) &&
        !IsSecurityStub(pCf))
        return SWA_CONTINUE; // not a function frame and not a security stub.
                             // Since we were just looking for CtxTransitionFrames, resume the stackwalk...
        



    AppDomain* pAppDomain = pCf->GetAppDomain();
    if (pCBdata->prev_AppDomain == NULL)
    {
        pCBdata->prev_AppDomain = pAppDomain; //innermost AD
    }
    else if (pCBdata->prev_AppDomain != pAppDomain)
    {
        // AppDomain Transition
        // Update the values in the ADStack for the current AD
        Thread *t = GetThread();
        t->GetNextDomainOnStack(&pCBdata->adStackIndex, NULL, NULL);
        t->UpdateDomainOnStack(pCBdata->adStackIndex, pCBdata->numDomainAsserts, pCBdata->numDomainOverrides);

        // Update CBdata values
        pCBdata->numAsserts+= pCBdata->numDomainAsserts;
        pCBdata->numOverrides += pCBdata->numDomainOverrides;
        pCBdata->numDomainAsserts = 0;
        pCBdata->numDomainOverrides = 0;
        pCBdata->prev_AppDomain = pAppDomain;
        
    }
    // Get the security object for this function...
    OBJECTREF* pRefSecDesc = pCf->GetAddrOfSecurityObject();
    if (pRefSecDesc != NULL)
        SecurityDeclarative::DoDeclarativeSecurityAtStackWalk(pMeth, pAppDomain, pRefSecDesc);

    if (pRefSecDesc == NULL || *pRefSecDesc == NULL)
    {
        DBG_TRACE_STACKWALK("       No SecurityDescriptor on this frame. Skipping.\n", true);
        return SWA_CONTINUE;
    }
    
    FRAMESECDESCREF refFSD = *((FRAMESECDESCREF*)pRefSecDesc);
    
    INT32       ret = refFSD->GetOverridesCount();
    pCBdata->numDomainAsserts+= refFSD->GetAssertCount();

    if (ret > 0)
    {
        DBG_TRACE_STACKWALK("       SecurityDescriptor with overrides FOUND.\n", false);
        pCBdata->numDomainOverrides += ret;
    }
    else
    {
        DBG_TRACE_STACKWALK("       SecurityDescriptor with no override found.\n", false);
    }
    // If this is a *.Run method, 
    // or if it has a CompressedStack then we need to terminate the stackwalk
    if (pCBdata->IsSpecialRunFrame(pMeth))
    {
        DBG_TRACE_STACKWALK("            Halting stackwalk for .Run.\n", false);
        return SWA_ABORT;
    }
    
    return SWA_CONTINUE;
}

VOID SecurityStackWalk::UpdateOverridesCount()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (SecurityPolicy::IsSecurityOff())
        return;

    //
    // Initialize the callback data on the stack.
    //

    CountOverridesStackWalk walkData;

    // Get the current thread that we're to walk.
    Thread * t = GetThread();

    
    // Don't allow recursive security stackwalks. Note that this implies that
    // *no* untrusted code must ever be called during a security stackwalk.
    if (t->IsSecurityStackwalkInProgess())
        return;
    
    EX_TRY
    {
        t->SetSecurityStackwalkInProgress( TRUE );

        //
        // Begin the stack walk
        //
        DBG_TRACE_STACKWALK(" Update Overrides Count invoked .\n", false);
        // LIGHTUNWIND flag: allow using stackwalk cache for security stackwalks
        t->StackWalkFrames(UpdateOverridesCountCB, &walkData, SKIPFUNCLETS | LIGHTUNWIND);

        COMPRESSEDSTACKREF csRef = (COMPRESSEDSTACKREF)t->GetCompressedStack();

        // There may have been an AD transition and we shd look at the CB data to see if this is the case
        if (csRef == NULL && walkData.pCtxTxFrame != NULL)
        {
            csRef = SecurityStackWalk::GetCSFromContextTransitionFrame(walkData.pCtxTxFrame);
        }

        // Use CS if found
        if (csRef != NULL)
        {

            walkData.numDomainOverrides += StackCompressor::GetCSInnerAppDomainOverridesCount(csRef);
            walkData.numDomainAsserts += StackCompressor::GetCSInnerAppDomainAssertCount(csRef);
        }
        t->GetNextDomainOnStack(&walkData.adStackIndex, NULL, NULL);
        t->UpdateDomainOnStack(walkData.adStackIndex, walkData.numDomainAsserts, walkData.numDomainOverrides);
        walkData.numAsserts += walkData.numDomainAsserts;
        walkData.numOverrides += walkData.numDomainOverrides;

        t->SetSecurityStackwalkInProgress( FALSE );
    }
    EX_CATCH
    {
        // We catch exceptions and rethrow like this to ensure that we've
        // established an exception handler on the fs:[0] chain (managed
        // exception handlers won't do this). This in turn guarantees that
        // managed exception filters in any of our callers won't be found,
        // otherwise they could get to execute untrusted code with security
        // turned off.
        t->SetSecurityStackwalkInProgress( FALSE );

        EX_RETHROW;
    }
    EX_END_CATCH_UNREACHABLE


    

}



























// -------------------------------------------------------------------------------
//
//                                 COMCodeAccessSecurityEngine
//
// -------------------------------------------------------------------------------

COMPRESSEDSTACKREF SecurityStackWalk::GetCSFromContextTransitionFrame(Frame *pFrame)
{
    EXECUTIONCONTEXTREF ecRef = NULL;

    if (pFrame != NULL)
        ecRef = (EXECUTIONCONTEXTREF)pFrame->GetReturnExecutionContext();
    if (ecRef != NULL)
        return (ecRef->GetCompressedStack());
    
    return NULL;
}



//-----------------------------------------------------------+
// Helper used to check a demand set against a provided grant
// and possibly denied set. Grant and denied set might be from
// another domain.
//-----------------------------------------------------------+
void SecurityStackWalk::CheckSetHelper(OBJECTREF *prefDemand,
                                                 OBJECTREF *prefGrant,
                                                 OBJECTREF *prefRefused,
                                                 AppDomain *pGrantDomain,
                                                 MethodDesc *pMethod,
                                                 OBJECTREF *pAssembly,
                                                 CorDeclSecurity action)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsProtectedByGCFrame (prefDemand));
        PRECONDITION(IsProtectedByGCFrame (prefGrant));
        PRECONDITION(IsProtectedByGCFrame (prefRefused));
        PRECONDITION(IsProtectedByGCFrame (pAssembly));
    } CONTRACTL_END;

    // We might need to marshal the grant and denied sets into the current
    // domain.
    if (pGrantDomain != GetAppDomain())
    {
        *prefGrant = AppDomainHelper::CrossContextCopyFrom(pGrantDomain, prefGrant);
        if (*prefRefused != NULL)
            *prefRefused = AppDomainHelper::CrossContextCopyFrom(pGrantDomain, prefRefused);
    }

    MethodDescCallSite checkSetHelper(METHOD__SECURITY_ENGINE__CHECK_SET_HELPER);

    ARG_SLOT args[] = {
        ObjToArgSlot(NULL),
        ObjToArgSlot(*prefGrant),
        ObjToArgSlot(*prefRefused),
        ObjToArgSlot(*prefDemand),
        PtrToArgSlot(pMethod),
        ObjToArgSlot(*pAssembly),
        (ARG_SLOT)action
    };

    checkSetHelper.Call(args);
}





FCIMPL0(FC_BOOL_RET, SecurityStackWalk::FCallQuickCheckForAllDemands)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    FC_RETURN_BOOL(QuickCheckForAllDemands(0));

}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, SecurityStackWalk::FCallAllDomainsHomogeneousWithNoStackModifiers)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    Thread* t = GetThread();
    FC_RETURN_BOOL(t->AllDomainsHomogeneousWithNoStackModifiers());

}
FCIMPLEND

//-----------------------------------------------------------
// Native implementation for code-access security check.
// Checks that callers on the stack have the permission
// specified in the arguments or checks for unrestricted
// access if the permission is null.
//-----------------------------------------------------------
FCIMPL3(void, SecurityStackWalk::Check, Object* permOrPermSetUNSAFE, StackCrawlMark* stackMark, CLR_BOOL isPermSet)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    if (QuickCheckForAllDemands(0))
        return;
    
    
    FC_CALL_INNER(CheckFramed(permOrPermSetUNSAFE, stackMark, isPermSet));
}
FCIMPLEND
    
FC_INNER_RET SecurityStackWalk::CheckFramed(Object* permOrPermSetUNSAFE, 
                                            StackCrawlMark* stackMark, 
                                            CLR_BOOL isPermSet)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    FC_INNER_PROLOG(SecurityStackWalk::Check);

    OBJECTREF permOrPermSet      = (OBJECTREF) permOrPermSetUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_CAPTURE_DEPTH_2, permOrPermSet);

    Check_PLS_SW(isPermSet, SSWT_IMPERATIVE_DEMAND, &permOrPermSet, stackMark);

    HELPER_METHOD_FRAME_END();

    FC_INNER_EPILOG();
}


void SecurityStackWalk::Check_PLS_SW(BOOL isPermSet,
                                     SecurityStackWalkType eType, 
                                     OBJECTREF* permOrPermSet, 
                                     StackCrawlMark* stackMark)
{
    
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (!PreCheck(permOrPermSet, isPermSet))
    {
        Check_StackWalk(eType, permOrPermSet, stackMark, isPermSet);
    }



}
void SecurityStackWalk::Check_PLS_SW_GC( BOOL isPermSet,
                                         SecurityStackWalkType eType, 
                                         OBJECTREF permOrPermSet, 
                                         StackCrawlMark* stackMark)
{
    WRAPPER_CONTRACT;
    GCPROTECT_BEGIN(permOrPermSet);
    Check_PLS_SW(isPermSet, eType, &permOrPermSet, stackMark);
    GCPROTECT_END();
}

void SecurityStackWalk::Check_StackWalk(SecurityStackWalkType eType, 
                                        OBJECTREF* pPermOrPermSet, 
                                        StackCrawlMark* stackMark, 
                                        BOOL isPermSet)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(*pPermOrPermSet != NULL);
    } CONTRACTL_END;

#if defined(ENABLE_PERF_COUNTERS)
    // Perf Counter "%Time in Runtime check" support
    PERF_COUNTER_TIMER_PRECISION _startPerfCounterTimer = GET_CYCLE_COUNT();
#endif

    if (GetThread()->GetOverridesCount() != 0)
    {
        // First let's make sure the overrides count is OK.
        UpdateOverridesCount();
        // Once the overrides count has been fixes, let's see if we really need to stackwalk
        // This is an additional cost if we do need to walk, but can remove an unnecessary SW otherwise.
        // Pick your poison.
        if (QuickCheckForAllDemands(0))
            return; // 
    }
    // Initialize callback data.
    DWORD dwFlags = 0;
    BOOL exactFlag = FALSE;
    BOOL unrestrictedOverride = FALSE;
    DWORD demandFlags = GetPermissionSpecialFlags(pPermOrPermSet,&exactFlag, &unrestrictedOverride);
    DemandStackWalk walkData(eType, dwFlags, stackMark, (isPermSet?DemandStackWalk::DT_SET:DemandStackWalk::DT_PERMISSION), unrestrictedOverride,demandFlags);
    walkData.m_objects.SetObject(*pPermOrPermSet);

    // Protect the object references in the callback data.
    GCPROTECT_BEGIN(walkData.m_objects.m_sGC);

    walkData.DoStackWalk();

    GCPROTECT_END();

#if defined(ENABLE_PERF_COUNTERS)
    // Accumulate the counter
    PERF_COUNTER_TIMER_PRECISION _stopPerfCounterTimer = GET_CYCLE_COUNT();
    g_TotalTimeInSecurityRuntimeChecks += _stopPerfCounterTimer - _startPerfCounterTimer;

    // Report the accumulated counter only after NUM_OF_TERATIONS
    if (g_SecurityChecksIterations++ > PERF_COUNTER_NUM_OF_ITERATIONS)
    {
        GetPrivatePerfCounters().m_Security.timeRTchecks = g_TotalTimeInSecurityRuntimeChecks;
        GetPrivatePerfCounters().m_Security.timeRTchecksBase = (_stopPerfCounterTimer - g_LastTimeInSecurityRuntimeChecks);
        
        g_TotalTimeInSecurityRuntimeChecks = 0;
        g_LastTimeInSecurityRuntimeChecks = _stopPerfCounterTimer;
        g_SecurityChecksIterations = 0;
    }
#endif // #if defined(ENABLE_PERF_COUNTERS)
}

FCIMPL3(void, SecurityStackWalk::GetZoneAndOrigin, Object* pZoneListUNSAFE, Object* pOriginListUNSAFE, StackCrawlMark* stackMark)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    OBJECTREF zoneList    = (OBJECTREF) pZoneListUNSAFE;
    OBJECTREF originList  = (OBJECTREF) pOriginListUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_2(zoneList, originList);

    // Initialize callback data.
    DWORD dwFlags = 0;
    DemandStackWalk walkData(SSWT_GET_ZONE_AND_URL, dwFlags, stackMark, DemandStackWalk::DT_ZONE_AND_URL, FALSE, 0);
    walkData.m_objects.SetObjects(zoneList, originList);

    GCPROTECT_BEGIN(walkData.m_objects.m_sGC);

    walkData.DoStackWalk();

    GCPROTECT_END();

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

FCIMPL1(VOID, SecurityStackWalk::FcallDestroyDelayedCompressedStack, void *compressedStack) 
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    _ASSERTE( SecurityPolicy::IsSecurityOn() && "This function should only be called with security on" );

    StackCompressor::Destroy(compressedStack);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

//
// This method checks a few special demands in case we can 
// avoid looking at the real PLS object.
//

DWORD SecurityStackWalk::GetPermissionSpecialFlags (OBJECTREF* orDemand, BOOL *pfExactFlag, BOOL* pUnrestrictedOverride)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    static MethodTable* pMethPermissionSet = NULL;
    if (pMethPermissionSet == NULL)
        pMethPermissionSet = g_Mscorlib.GetClass(CLASS__PERMISSION_SET);
    static MethodTable* pMethUIPermission = NULL;
    if (pMethUIPermission == NULL)
        pMethUIPermission = g_Mscorlib.GetClass(CLASS__UI_PERMISSION);
    static MethodTable* pMethEnvPermission = NULL;
    if (pMethEnvPermission == NULL)
        pMethEnvPermission = g_Mscorlib.GetClass(CLASS__ENVIRONMENT_PERMISSION);
    static MethodTable* pMethFileDialogPermission = NULL;
    if (pMethFileDialogPermission == NULL)
        pMethFileDialogPermission = g_Mscorlib.GetClass(CLASS__FILEDIALOG_PERMISSION);
    static MethodTable* pMethFileIOPermission = NULL;
    if (pMethFileIOPermission == NULL)
        pMethFileIOPermission = g_Mscorlib.GetClass(CLASS__FILEIO_PERMISSION);
    static MethodTable* pMethReflectionPermission = NULL;
    if (pMethReflectionPermission == NULL)
        pMethReflectionPermission = g_Mscorlib.GetClass(CLASS__REFLECTION_PERMISSION);
    static MethodTable* pMethSecurityPermission = NULL;
    if (pMethSecurityPermission == NULL)
        pMethSecurityPermission = g_Mscorlib.GetClass(CLASS__SECURITY_PERMISSION);
    static MethodTable* pMethIUnrestrictedPermission = NULL;
    if (pMethIUnrestrictedPermission == NULL)
        pMethIUnrestrictedPermission = g_Mscorlib.GetClass(CLASS__IUNRESTRICTED_PERMISSION);

    DWORD dwSecurityPermissionFlags = 0, dwReflectionPermissionFlags = 0;
    MethodTable* pMeth = (*orDemand)->GetMethodTable();
    if (pMeth == pMethPermissionSet) {
        PERMISSIONSETREF permSet = (PERMISSIONSETREF) *orDemand;
        
        if (pUnrestrictedOverride != NULL)
            *pUnrestrictedOverride = permSet->CanUnrestrictedOverride();
        
        if (permSet->IsUnrestricted()) {
            *pfExactFlag = TRUE;
            return (1 << SECURITY_FULL_TRUST);
        }
        TOKENBASEDSETREF tokenBasedSet = (TOKENBASEDSETREF) permSet->GetTokenBasedSet();
        if (tokenBasedSet != NULL && tokenBasedSet->GetNumElements() == 1 && tokenBasedSet->GetPermSet() != NULL)
            pMeth = (tokenBasedSet->GetPermSet())->GetMethodTable();

        if (pMeth == pMethReflectionPermission)
            dwReflectionPermissionFlags = ((REFLECTIONPERMISSIONREF) tokenBasedSet->GetPermSet())->GetFlags();
        else if (pMeth == pMethSecurityPermission)
            dwSecurityPermissionFlags = ((SECURITYPERMISSIONREF) tokenBasedSet->GetPermSet())->GetFlags();
    }
    else {
        // We're dealing with a CodeAccessPermission
        if (pUnrestrictedOverride != NULL && pMeth->CanCastToNonVariantInterface(pMethIUnrestrictedPermission))
            *pUnrestrictedOverride = TRUE;
        
        if (pMeth == pMethReflectionPermission)
            dwReflectionPermissionFlags = ((REFLECTIONPERMISSIONREF) (*orDemand))->GetFlags();
        else if (pMeth == pMethSecurityPermission)
            dwSecurityPermissionFlags = ((SECURITYPERMISSIONREF) (*orDemand))->GetFlags();
    }

    if (pMeth == pMethUIPermission)
        return (1 << UI_PERMISSION);
    if (pMeth == pMethEnvPermission)
        return (1 << ENVIRONMENT_PERMISSION);
    if (pMeth == pMethFileDialogPermission)
        return (1 << FILEDIALOG_PERMISSION);
    if (pMeth == pMethFileIOPermission)
        return (1 << FILEIO_PERMISSION);
    if (pMeth == pMethReflectionPermission) {
        switch (dwReflectionPermissionFlags) {
        case REFLECTION_PERMISSION_TYPEINFO:
            *pfExactFlag = TRUE;
            return (1 << REFLECTION_TYPE_INFO);
        case REFLECTION_PERMISSION_MEMBERACCESS:
            *pfExactFlag = TRUE;
            return (1 << REFLECTION_MEMBER_ACCESS);
        case REFLECTION_PERMISSION_REFLECTIONEMIT:
            *pfExactFlag = TRUE;
            return (1 << REFLECTION_EMIT);
        default:
            return (1 << REFLECTION_PERMISSION);
        }
    }
    if (pMeth == pMethSecurityPermission) {
        switch (dwSecurityPermissionFlags) {
        case SECURITY_PERMISSION_ASSERTION:
            *pfExactFlag = TRUE;
            return (1 << SECURITY_ASSERT);
        case SECURITY_PERMISSION_UNMANAGEDCODE:
            *pfExactFlag = TRUE;
            return (1 << SECURITY_UNMANAGED_CODE);
        case SECURITY_PERMISSION_SKIPVERIFICATION:
            *pfExactFlag = TRUE;
            return (1 << SECURITY_SKIP_VER);
        case SECURITY_PERMISSION_SERIALIZATIONFORMATTER:
            *pfExactFlag = TRUE;
            return (1 << SECURITY_SERIALIZATION);
        case SECURITY_PERMISSION_BINDINGREDIRECTS:
            *pfExactFlag = TRUE;
            return (1 << SECURITY_BINDING_REDIRECTS);
        case SECURITY_PERMISSION_CONTROLEVIDENCE:
            *pfExactFlag = TRUE;
            return (1 << SECURITY_CONTROL_EVIDENCE);
        case SECURITY_PERMISSION_CONTROLPRINCIPAL:
            *pfExactFlag = TRUE;
            return (1 << SECURITY_CONTROL_PRINCIPAL);
        default:
            return (1 << SECURITY_PERMISSION);
        }
    }

    return 0;
}

// check is a stackwalk is needed to evaluate the demand
BOOL SecurityStackWalk::PreCheck (OBJECTREF* orDemand, BOOL fDemandSet)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    // Track perfmon counters. Runtime security checks.
    IncrementSecurityPerfCounter();

    Thread* pThread = GetThread();
    // The PLS optimization does not support overrides.
    if (pThread->GetOverridesCount() > 0)
        return FALSE;

    if (ContainsIdentityPermissions(*orDemand)) 
    {
        // Fail the PreCheck early when the demand contains identity permissions.
        return FALSE;
    }
    else 
    {
        BOOL fExactFlag = FALSE;
        DWORD dwDemandSpecialFlags = GetPermissionSpecialFlags(orDemand, &fExactFlag, NULL);

        if (SecurityStackWalk::HasFlagsOrFullyTrustedIgnoreMode(dwDemandSpecialFlags))
            return TRUE; // Ignore mode here because we have checked above for identity perms
        else if (fExactFlag)
            return FALSE;

        // If we know there is only one AppDomain, then there is
        // no need to walk the AppDomainStack structure.
        if (pThread->GetNumAppDomainsOnThread() == 1)
            return GetAppDomain()->GetSecurityDescriptor()->CheckPLS(orDemand, dwDemandSpecialFlags, fDemandSet);

        // Walk all AppDomains in the stack and check the PLS on each one of them
        DWORD dwAppDomainIndex = 0;
        pThread->InitDomainIteration(&dwAppDomainIndex);
        _ASSERT(SystemDomain::System() && "SystemDomain not yet created!");
        while (dwAppDomainIndex != 0) {
            AppDomainFromIDHolder appDomain(pThread->GetNextDomainOnStack(&dwAppDomainIndex, NULL, NULL), FALSE);
            if (appDomain.IsUnloaded())
                // appdomain has been unloaded, so we can just continue on the loop
                continue;

            ApplicationSecurityDescriptor* pAppSecDesc = appDomain->GetSecurityDescriptor();
            appDomain.Release();

            if (!pAppSecDesc->CheckPLS(orDemand, dwDemandSpecialFlags, fDemandSet))
                return FALSE;
        }
        return TRUE;
   }
}

//-----------------------------------------------------------+
// Unmanaged version of CodeAccessSecurityEngine.Demand() in BCL
// Any change there may have to be propagated here
// This call has to be virtual, unlike DemandSet
//-----------------------------------------------------------+
void
SecurityStackWalk::Demand(SecurityStackWalkType eType, OBJECTREF demand)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (QuickCheckForAllDemands(0))
        return;

    Check_PLS_SW_GC(FALSE, eType, demand, NULL);
}

//-----------------------------------------------------------+
// Special case of Demand(). This remembers the result of the 
// previous demand, and reuses it if new assemblies have not
// been added since then
//-----------------------------------------------------------+
void SecurityStackWalk::SpecialDemand(SecurityStackWalkType eType, DWORD whatPermission, StackCrawlMark* stackMark)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (QuickCheckForAllDemands(1<<whatPermission))
    {
        // Track perfmon counters. Runtime security checks.
        IncrementSecurityPerfCounter();
        return;
    }
    OBJECTREF demand = NULL;
    GCPROTECT_BEGIN(demand);

    SecurityDeclarative::GetPermissionInstance(&demand, whatPermission);
    Check_PLS_SW(FALSE, eType, &demand, stackMark);

    GCPROTECT_END();

}

// Do a demand for a special permission type
FCIMPL2(void, SecurityStackWalk::FcallSpecialDemand, DWORD whatPermission, StackCrawlMark* stackMark)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();

    SpecialDemand(SSWT_IMPERATIVE_DEMAND, whatPermission, stackMark);

    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

//-----------------------------------------------------------+
// Unmanaged version of PermissionSet.Demand()
//-----------------------------------------------------------+
void SecurityStackWalk::DemandSet(SecurityStackWalkType eType, OBJECTREF demand)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;


    // Though the PermissionSet may contain non-CAS permissions, we are considering it as a CAS permission set only
    // at this point and so it's safe to check for the FT and if FTmeansFT and return
    if (QuickCheckForAllDemands(0))
        return;

    // Do further checks (PLS/SW) only if this set contains CAS perms
    if(((PERMISSIONSETREF)demand)->CheckedForNonCas() && !((PERMISSIONSETREF)demand)->ContainsCas())
        return;

    Check_PLS_SW_GC(TRUE, eType, demand, NULL);
}


void SecurityStackWalk::DemandSet(SecurityStackWalkType eType, DWORD dwSetIndex, DWORD dwAction)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;


    // Though the PermissionSet may contain non-CAS permissions, we are considering it as a CAS permission set only
    // at this point and so it's safe to check for the FT and if FTmeansFT and return
    if (QuickCheckForAllDemands(0))
        return;

    OBJECTREF refPermSet = SecurityAttributes::GetPermissionSet(dwSetIndex, dwAction);
    if(refPermSet != NULL)
    {
        // Do further checks (PLS/SW) only if this set contains CAS perms
        if(((PERMISSIONSETREF)refPermSet)->CheckedForNonCas() && !((PERMISSIONSETREF)refPermSet)->ContainsCas())
            return;
        
        Check_PLS_SW_GC(TRUE, eType, refPermSet, NULL);
        
    }
}



//-----------------------------------------------------------+
// L I N K /I N H E R I T A N C E T I M E   C H E C K
//-----------------------------------------------------------+
void SecurityStackWalk::LinkOrInheritanceCheck(AssemblySecurityDescriptor *pSecDesc, OBJECTREF refDemands, Assembly* pAssembly, CorDeclSecurity action)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (SecurityPolicy::IsSecurityOff())
        return;

    // MSCORLIB is not subject to inheritance checks
    if (pAssembly->IsSystem())
        return;

    struct _gc {
        OBJECTREF refDemands;
        OBJECTREF refExposedAssemblyObject;
        OBJECTREF refRefused;
        OBJECTREF refGranted;
    } gc;
    ZeroMemory(&gc, sizeof(gc));
    
    gc.refDemands = refDemands;
    gc.refExposedAssemblyObject = NULL;

    GCPROTECT_BEGIN(gc);

    // We only do LinkDemands if the assembly is not fully trusted or if the demand contains permissions that don't implement IUnrestricted
    if (!pAssembly->GetSecurityDescriptor()->IsFullyTrusted() || ContainsIdentityPermissions(gc.refDemands))
    {


        if (pAssembly) 
            gc.refExposedAssemblyObject = pAssembly->GetExposedObject();
        if (pSecDesc->IsFullyTrusted())
        {
            MethodDescCallSite lazyCheckSetHelper(METHOD__SECURITY_ENGINE__LAZY_CHECK_SET_HELPER);
            ARG_SLOT ilargs[5];
            ilargs[0] = ObjToArgSlot(gc.refDemands);
            ilargs[1] = PtrToArgSlot(pSecDesc);
            ilargs[2] = PtrToArgSlot(NULL);
            ilargs[3] = ObjToArgSlot(gc.refExposedAssemblyObject);
            ilargs[4] = (ARG_SLOT)action;
            lazyCheckSetHelper.Call(ilargs);
        }
        else
        {
            MethodDescCallSite checkSetHelper(METHOD__SECURITY_ENGINE__CHECK_SET_HELPER);
            gc.refGranted = pSecDesc->GetGrantedPermissionSet(&(gc.refRefused));
            ARG_SLOT ilargs[7];
            ilargs[0] = ObjToArgSlot(NULL);
            ilargs[1] = ObjToArgSlot(gc.refGranted);
            ilargs[2] = ObjToArgSlot(gc.refRefused);
            ilargs[3] = ObjToArgSlot(gc.refDemands);
            ilargs[4] = PtrToArgSlot(NULL);
            ilargs[5] = ObjToArgSlot(gc.refExposedAssemblyObject);
            ilargs[6] = (ARG_SLOT)action;
            checkSetHelper.Call(ilargs);
        }
    }
    GCPROTECT_END();
}


//-----------------------------------------------------------+
// S T A C K   C O M P R E S S I O N FCALLS
//-----------------------------------------------------------+




FCIMPL1(Object*, SecurityStackWalk::EcallGetDelayedCompressedStack, StackCrawlMark* stackMark)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    OBJECTREF rv = NULL;

    HELPER_METHOD_FRAME_BEGIN_RET_0();
    //-[autocvtpro]-------------------------------------------------------

    rv = StackCompressor::GetCompressedStack( stackMark );

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(rv);

}
FCIMPLEND

//-----------------------------------------------------------
// Warning!! This is passing out a reference to the permissions
// for the assembly. It must be deep copied before passing it out
//-----------------------------------------------------------

FCIMPL3(void, SecurityStackWalk::GetGrantedPermissionSet, void* pSecDescIn, OBJECTREF* ppGranted, OBJECTREF* ppDenied)
{
    CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS); // FCALLS with HELPER frames have issues with GC_TRIGGERS
        MODE_COOPERATIVE;
        SO_TOLERANT;        
    } CONTRACTL_END;

    HELPER_METHOD_FRAME_BEGIN_0();
    //-[autocvtpro]-------------------------------------------------------

    AssemblySecurityDescriptor * pSecDesc = (AssemblySecurityDescriptor*) pSecDescIn;
    _ASSERTE(pSecDesc != NULL);

    OBJECTREF token = pSecDesc->GetGrantedPermissionSet(ppDenied);
    *((OBJECTREF*)ppGranted) = token;

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
}
FCIMPLEND

/* static */
BOOL SecurityStackWalk::ContainsIdentityPermissions(OBJECTREF refPerm)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END

    if (Security::DoesFullTrustMeanFullTrust())
        return FALSE;

    BOOL containsIdentityPermission = TRUE;

    GCPROTECT_BEGIN(refPerm);

    static MethodTable* pMethPermissionSet = NULL;
    if (pMethPermissionSet == NULL)
        pMethPermissionSet = g_Mscorlib.GetClass(CLASS__PERMISSION_SET);
    static MethodTable* pMethIUnrestrictedPermission = NULL;
    if (pMethIUnrestrictedPermission == NULL)
        pMethIUnrestrictedPermission = g_Mscorlib.GetClass(CLASS__IUNRESTRICTED_PERMISSION);

    if (refPerm->GetMethodTable() == pMethPermissionSet)
    {
        // We're dealing with a PermissionSet
        // Looking up the field using the unmanaged layout of the managed class
        PERMISSIONSETREF refUnmgdPset = (PERMISSIONSETREF)refPerm; 
        BOOL canUnrestrictedOverride = refUnmgdPset->CanUnrestrictedOverride();

        // The value of 'canUnrestrictedOverride is only valid if we have decoded all permissions.
        _ASSERTE(refUnmgdPset->AllPermissionsDecoded());

        containsIdentityPermission = !canUnrestrictedOverride;
    }
    else
    {
        // We're dealing with a CodeAccessPermission
        MethodTable *pPermission = refPerm->GetMethodTable();
        if (pPermission->CanCastToNonVariantInterface(pMethIUnrestrictedPermission))
        {
            containsIdentityPermission = FALSE;
        }
        else
            containsIdentityPermission = TRUE;
    }

    GCPROTECT_END();
    return containsIdentityPermission;
}
