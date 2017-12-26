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
// ===========================================================================
// File: Prestub.cpp
//
// ===========================================================================
// This file contains the implementation for creating and using prestubs
// ===========================================================================
//

#include "common.h"
#include "vars.hpp"
#include "security.h"
#include "securitydescriptor.h"
#include "eeconfig.h"
#include "dllimport.h"
#include "comdelegate.h"
#include "remoting.h"
#include "dbginterface.h"
#include "listlock.inl"
#include "stubgen.h"
#include "eventtrace.h"
#include "timeline.h"
#include "constrainedexecutionregion.h"
#include "array.h"


#include "mdaassistants.h"


#ifndef DACCESS_COMPILE 

//==========================================================================
// This function is logically part of PreStubWorker(). Throws if there is
// an error making a security stub. Returns pRealStub if no security stub was
// actually needed.
//==========================================================================
Stub *MakeSecurityWorker(MethodDesc *pMD,
                         DWORD dwDeclFlags,
                         UINT_PTR uSecurityStubToken,
                         Stub* pRealStub,
                         LPVOID pRealAddr)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
    }
    CONTRACT_END;

        SecurityStubLinker sl;

    Stub *pStub = NULL;

    LOG((LF_CORDB, LL_INFO10000, "MakeSecurityWorker: Real Stub 0x%x\n", pRealStub));

    pStub = Security::CreateStub(&sl,
                                 pMD,
                                 dwDeclFlags,
                                 uSecurityStubToken,
                                 pRealStub,
                                 pRealAddr);

    RETURN pStub;
}

OBJECTREF GetActiveObject(PrestubMethodFrame *pPFrame)
{
    CONTRACT(OBJECTREF)
    {
        THROWS;
        GC_TRIGGERS;
        //POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

#if _DEBUG 

    // This check is expensive (it accesses metadata), so only do it in a checked build

    BYTE callingConvention = MetaSig::GetCallingConvention(pPFrame->GetModule(),pPFrame->GetFunction()->GetSig());

    if (!isCallConv(callingConvention, IMAGE_CEE_CS_CALLCONV_DEFAULT) &&
        !isCallConv(callingConvention, IMAGE_CEE_CS_CALLCONV_VARARG))
    {
        _ASSERTE(!"Unimplemented calling convention.");
    }
#endif // _DEBUG

    // Now return the this pointer!
    RETURN pPFrame->GetThis();
}

//==========================================================================

void MethodDesc::DoBackpatch(MethodTable *pMT, BOOL bBackpatched /*=FALSE*/)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(!IsAbstract());
        PRECONDITION(!ContainsGenericVariables());
        PRECONDITION(HasStableEntryPoint());
    }
    CONTRACTL_END;


    if (!HasTemporaryEntryPoint())
        return;

    TADDR pTarget = GetStableEntryPoint();
    TADDR pExpected = GetTemporaryEntryPoint();

    if (pExpected == pTarget)
        return;

    if (!bBackpatched)
    {
        Precode* pFuncPtrPrecode = GetDomain()->GetFuncPtrStubs()->Lookup(this);
        if (pFuncPtrPrecode != NULL)
        {
            // If there is a funcptr precode to patch, we are done for this round.
            if (pFuncPtrPrecode->SetTargetInterlocked(GetStableEntryPoint(), FALSE))
                return;
        }

#ifndef HAS_COMPACT_ENTRYPOINTS
        // Patch the fake entrypoint if necessary
        Precode::GetPrecodeFromEntryPoint(pExpected)->SetTargetInterlocked(pTarget, FALSE);
#endif // HAS_COMPACT_ENTRYPOINTS
    }

    if (HasNonVtableSlot())
        return;


    // Backpatch the main slot
    if (pMT->GetSlot(GetSlot()) == (SLOT)pExpected)
    {
        pMT->SetSlot_Log(GetSlot(), (SLOT)pTarget);
        bBackpatched = TRUE;
    }

    // The backpatching on owning methodtable should always produce same results even with races
    _ASSERTE(pMT->GetSlot(GetSlot()) == (SLOT)pTarget || pMT != GetMethodTable());

    if(/*!bBackpatched && */IsMethodImpl())
    {
        MethodImpl::Iterator it(this);
        while (it.IsValid())
        {
            if (pMT->GetSlot(it.GetSlot()) == (SLOT)pExpected)
            {
                pMT->SetSlot_Log(it.GetSlot(), (SLOT)pTarget);
                bBackpatched = TRUE;
            }
            it.Next();
        }
    }
    if (!bBackpatched && IsDuplicate())
    {
        // If this is a duplicate, let's scan the rest of the VTable hunting for other hits.
        unsigned numSlots = GetMethodTable()->GetNumVirtuals();
        for (DWORD i=0; i<numSlots; i++)
        {
            if (pMT->GetSlot(i) == (SLOT)pExpected)
            {
                pMT->SetSlot(i, (SLOT)pTarget);
                bBackpatched = TRUE;
            }
        }
    }
}

//
//
//
#ifdef _MSC_VER 
#pragma optimize("", off)
#endif

void DACNotifyCompilationFinished(MethodDesc *methodDesc)
{
    JITNotifications jn(g_pNotificationTable);
    if (jn.IsActive())
    {
        mdToken t = methodDesc->GetMemberDef();
        Module *modulePtr = methodDesc->GetModule();

        _ASSERTE(modulePtr);

        USHORT jnt = jn.Requested((TADDR) modulePtr, t);
        if (jnt & CLRDATA_METHNOTIFY_GENERATED)
        {
            DACNotify::DoJITNotification(methodDesc);
        }
    }
}

#ifdef _MSC_VER 
#pragma optimize("", on)
#endif


// ********************************************************************
//                  README!!
// ********************************************************************

// MakeJitWorker is the thread safe way to invoke the JIT compiler
// If multiple threads get in here for the same pMD, ALL of them
// MUST return the SAME value for pstub.
//
// This function creates a DeadlockAware list of methods being jitted
// which prevents us from trying to JIT the same method more that once. </STRIP>

TADDR MethodDesc::MakeJitWorker(COR_ILMETHOD_DECODER* ILHeader, DWORD flags)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    BOOL fIsILStub = IsILStub();

    LOG((LF_JIT, LL_INFO1000000,
         "MakeJitWorker(" FMT_ADDR ", %s) for %s:%s\n",
         DBG_ADDR(this),
         fIsILStub               ? " TRUE" : "FALSE",
         GetClass()->GetDebugClassName(),
         m_pszDebugMethodName));

    TADDR pCode = NULL;

    {
        // Enter the global lock which protects the list of all functions being JITd
        ListLockHolder pJitLock (GetDomain()->GetJitLock());

        // It is possible that another thread stepped in before we entered the global lock for the first time.
        pCode = GetNativeCode();
        if (pCode != NULL)
            goto Done;

        const char *description = "jit lock";
        INDEBUG(description = m_pszDebugMethodName;)
        ListLockEntryHolder pEntry(ListLockEntry::Find(pJitLock, this, description));

        // We have an entry now, we can release the global lock
        pJitLock.Release();

        // Take the entry lock
        {
            ListLockEntryLockHolder pEntryLock(pEntry, FALSE);

            if (pEntryLock.DeadlockAwareAcquire())
            {
                if (pEntry->m_hrResultCode == S_FALSE)
                {
                    // Nobody has jitted the method yet
                    g_IBCLogger.LogMethodCodeAccess(this);
                }
                else
                {


                }
            }
            else
            {
                // Taking this lock would cause a deadlock (presumably because we
                // are involved in a class constructor circular dependency.)  For
                // instance, another thread may be waiting to run the class constructor
                // that we are jitting, but is currently jitting this function.
                //
                // To remedy this, we want to go ahead and do the jitting anyway.
                // The other threads contending for the lock will then notice that
                // the jit finished while they were running class constructors, and abort their
                // current jit effort.
                //
                // We don't have to do anything special right here since we
                // can check IsJitted() to detect this case later.
                //
                // Note that at this point we don't have the lock, but that's OK because the
                // thread which does have the lock is blocked waiting for us.
            }

            // It is possible that another thread stepped in before we entered the lock.
            pCode = GetNativeCode();
            if (pCode != NULL)
                goto Done;



#ifdef PROFILING_SUPPORTED 
            // If profiling, need to give a chance for a tool to examine and modify
            // the IL before it gets to the JIT.  This allows one to add probe calls for
            // things like code coverage, performance, or whatever.
            if (CORProfilerTrackJITInfo() && !IsNoMetadata())
            {

                {
                    PROFILER_CALL;
                    g_profControlBlock.pProfInterface->JITCompilationStarted((ThreadID) GetThread(),
                                                                                (CodeID) this,
                                                                                TRUE);
                }
                // The profiler may have changed the code on the callback.  Need to
                // pick up the new code.  Note that you have to be fully trusted in
                // this mode and the code will not be verified.
                COR_ILMETHOD *pilHeader = GetILHeader();
                new (ILHeader) COR_ILMETHOD_DECODER(pilHeader, GetMDImport(), NULL);
            }
#endif // PROFILING_SUPPORTED

            {
                TADDR pOtherCode = NULL;

                EX_TRY
                {
                    pCode = UnsafeJitFunction(this, ILHeader, flags);
                }
                EX_CATCH
                {
                    // This catches cases where we aborted the JIT because of a deadlock cycle that another function broke by
                    // jitting our function. We swallow the kSynchronizationLockException.
                    pOtherCode = GetNativeCode();
                    if (pOtherCode == NULL)
                    {
                        pEntry->m_hrResultCode = E_FAIL;
                        EX_RETHROW;
                    }
                }
                EX_END_CATCH(RethrowTerminalExceptions)

                if (pOtherCode != NULL)
                {
                    // Somebody finished jitting recursively while we were jitting the method.
                    // Just use their method & leak the one we finished. (Normally we hope
                    // not to finish our JIT in this case, as we will abort early if we notice
                    // a reentrant jit has occurred.  But we may not catch every place so we
                    // do a definitive final check here.
                    pCode = pOtherCode;
                    goto Done;
                }
            }
            _ASSERTE(pCode != NULL);


            {
                BOOL fSuccess = FALSE;

                //
                // Set the native code address via interlocked operation to ensure
                // that there is only one winning version of the native code.
                //
                // This also avoid races with profiler overriding ngened code 
                // (see matching SetNativeCodeInterlocked done after JITCachedFunctionSearchStarted)
                //
                if (!HasPrecode())
                {
                    fSuccess = SetStableEntryPointInterlocked(pCode);
                }
                else
                {
                    fSuccess = GetPrecode()->SetNativeCodeInterlocked(pCode);
                }

                if (!fSuccess)
                {
                    // Somebody beat us to it
                    pCode = GetNativeCode();
                    goto Done;
                }
            }


            // We succeeded in jitting the code, and our jitted code is the one that's going to run now.
            pEntry->m_hrResultCode = S_OK;

#ifdef PROFILING_SUPPORTED 
            if (CORProfilerTrackJITInfo() && !IsNoMetadata())
            {
                PROFILER_CALL;
                g_profControlBlock.pProfInterface->
                    JITCompilationFinished((ThreadID) GetThread(), (CodeID) this,
                                            pEntry->m_hrResultCode, TRUE);
            }
#endif // PROFILING_SUPPORTED

            if (!fIsILStub)
            {
                // The notification will only occur if someone has registered for this method.
                DACNotifyCompilationFinished(this);
            }
        }
    }

Done:
    // We must have a code by now.
    _ASSERTE(pCode != NULL);

    LOG((LF_CORDB, LL_EVERYTHING, "MethodDesc::MakeJitWorker finished. Stub is" FMT_ADDR "\n",
         DBG_ADDR(pCode)));

    return pCode;
}

//==========================================================================
// This function is logically part of PreStubWorker().
//==========================================================================
Stub *MakeStubWorker(MethodDesc *pMD)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(!pMD->IsRestored() || !pMD->IsIntrospectionOnly());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Note: this should be kept idempotent ... in the sense that
    // if multiple threads get in here for the same pMD
    // it should not matter whose stub finally gets used. This applies
    // to all the helper functions this calls!

    Stub *pstub = NULL;
    CPUSTUBLINKER sl;

    if (pMD->IsInterface() && !pMD->IsStatic())
    {
        // Forward through transparent proxy
        CRemotingServices::GenerateCheckForProxy(&sl);

        // CheckForProxy never returns
        pstub = sl.Link(pMD->GetDomain()->GetStubHeap());
    }
    else
    if (pMD->IsArray())
    {
        LOG((LF_LOADER, LL_INFO1000, "Made ARRAY stub for method '%s.%s'\n",
             pMD->GetClass()->GetDebugClassName(),
             pMD->GetName()));

        pstub = GenerateArrayOpStub(&sl, (ArrayMethodDesc*)pMD);
        _ASSERTE(pstub != 0);   // Should handle all cases now.
    }
    else if (pMD->IsNDirect())
    {
        if (!Security::CanCallUnmanagedCode(pMD->GetModule()->GetSecurityDescriptor()))
            Security::ThrowSecurityException(g_SecurityPermissionClassName, SPFLAGSUNMANAGEDCODE);

        LOG((LF_LOADER, LL_INFO1000, "Made NDirect stub for method '%s.%s'\n",
             pMD->GetClass()->GetDebugClassName(),
             pMD->GetName()));

        pstub = NDirect::GetNDirectMethodStub(&sl, (NDirectMethodDesc*)pMD);

        if (pstub == NULL)
            COMPlusThrowOM();
    }
    else if (pMD->IsEEImpl())
    {
        LOG((LF_LOADER, LL_INFO1000, "Made EEImpl stub for method '%s'\n",
             pMD->GetName()));

        _ASSERTE(pMD->GetMethodTable()->IsAnyDelegateClass());
        pstub = COMDelegate::GetInvokeMethodStub((EEImplMethodDesc*)pMD);

    }
    else
    {
        // This is a method type we don't handle yet
        _ASSERTE(!"Unknown Method Type");
    }

    PREFIX_ASSUME(pstub != NULL);

    if (!pstub->IsMulticastDelegate())
    {
        pstub->SetRequiresMethodDescCallingConvention();
    }

    RETURN pstub;
}





/* Make a stub that for a value class method that expects a BOXed this poitner */

Stub * MakeUnboxingStubWorker(MethodDesc *pMD)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Note: this should be kept idempotent ... in the sense that
    // if multiple threads get in here for the same pMD
    // it should not matter whose stuff finally gets used.

    Stub *pstub = NULL;

    _ASSERTE (pMD->GetMethodTable()->IsValueClass());
    _ASSERTE(!pMD->ContainsGenericVariables());
    MethodDesc *pUnboxedMD = pMD->GetWrappedMethodDesc();

    _ASSERTE(pUnboxedMD != NULL && pUnboxedMD != pMD);

    CPUSTUBLINKER sl;
    sl.EmitUnboxMethodStub(pUnboxedMD);
    pstub = sl.Link(pMD->GetDomain()->GetStubHeap());

    RETURN pstub;
}

#if defined(FEATURE_SHARE_GENERIC_CODE) 
Stub * MakeInstantiatingStubWorker(MethodDesc *pMD)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(pMD->IsInstantiatingStub());
        PRECONDITION(!pMD->RequiresInstArg());
        PRECONDITION(!pMD->IsSharedByGenericMethodInstantiations());
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    // Note: this should be kept idempotent ... in the sense that
    // if multiple threads get in here for the same pMD
    // it should not matter whose stuff finally gets used.

    MethodDesc *pSharedMD = NULL;
    void* extraArg = NULL;

    // It's an instantiated generic method
    // Fetch the shared code associated with this instantiation
    //   pSharedMD = pMD->GetWrappedMethodDesc();
    pSharedMD = MethodDesc::FindOrCreateAssociatedMethodDesc(pMD,
                                                             pMD->GetMethodTable(),
                                                             FALSE, /* no unboxing entrypoint */
                                                             pMD->GetNumGenericMethodArgs(),
                                                             pMD->GetMethodInstantiation(),
                                                             TRUE /* get shared code */ );
    _ASSERTE(pSharedMD != NULL && pSharedMD != pMD);

    if (pMD->HasMethodInstantiation())
    {
        extraArg = pMD;
    }
    else
    {
        // It's a per-instantiation static method
        extraArg = pMD->GetMethodTable();
    }
    Stub *pstub = NULL;

    CPUSTUBLINKER sl;
    _ASSERTE(pSharedMD != NULL && pSharedMD != pMD);
    sl.EmitInstantiatingMethodStub(pSharedMD, extraArg);

    pstub = sl.Link(pMD->GetDomain()->GetStubHeap());

    RETURN pstub;
}
#endif // defined(FEATURE_SHARE_GENERIC_CODE)

//=============================================================================
// This function generates the real code for a method and installs it into
// the methoddesc. Usually ***BUT NOT ALWAYS***, this function runs only once
// per methoddesc. In addition to installing the new code, this function
// returns a pointer to the new code for the prestub's convenience.
//=============================================================================
extern "C" const BYTE * __stdcall PreStubWorker(PrestubMethodFrame *pPFrame)
{

    DWORD dwLastError = GetLastError();

    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_ENTRY_POINT;
    const BYTE* pbRetVal = NULL;

    INSTALL_UNWIND_AND_CONTINUE_HANDLER;


    ETWTraceStartup trace(ETW_TYPE_STARTUP_PRESTUBWORKER);
    TIMELINE_START(STARTUP_PRESTUB, ("PreStubWorker"));

    MethodDesc *pMD = pPFrame->GetFunction();

    // Make sure the method table is restored, and method instantiation if present
    pMD->CheckRestore();

    // A secondary layer of defense against executing code in inspection-only assembly.
    // This should already have been taken care of by not allowing inspection assemblies
    // to be activated. However, this is a very inexpensive piece of insurance in the name
    // of security.
    if (pMD->IsIntrospectionOnly())
    {
        _ASSERTE(!"A ReflectionOnly assembly reached the prestub. This should not have happened.");
        COMPlusThrow(kExecutionEngineException, IDS_EE_CODEEXECUTION_IN_INTROSPECTIVE_ASSEMBLY);
    }

    CONSISTENCY_CHECK(GetAppDomain()->CheckCanExecuteManagedCode(pMD));

    // Note this is redundant with the above check but we do it anyway for safety
    //
    // This has been disabled so we have a better chance of catching these.  Note that this check is
    // NOT sufficient for domain neutral and ngen cases.
    //
    // pMD->EnsureActive();

    MethodTable *pDispatchingMT = NULL;

    if (pMD->IsVtableMethod())
    {
        OBJECTREF curobj = GetActiveObject(pPFrame);

        if (curobj != NULL) // Check for virtual function called non-virtually on a NULL object
        {
            pDispatchingMT = curobj->GetMethodTable();

            if (CTPMethodTable::IsTPMethodTable(pDispatchingMT))
            {
                // If its a __TransparentProxy, get the server object
                pDispatchingMT = CTPMethodTable::GetClassBeingProxied(curobj).AsMethodTable();
            }

            // For value types, the only virtual methods are interface implementations.
            // Thus pDispatching == pMT because there
            // is no inheritance in value types.  Note the BoxedEntryPointStubs are shared
            // between all sharable generic instantiations, so the == test is on
            // canonical method tables.
#ifdef _DEBUG 
            MethodTable *pMDMT = pMD->GetMethodTable(); // put this here to see what the MT is in debug mode
            _ASSERTE(!pMD->GetClass()->IsValueClass() ||
                     (pMD->IsUnboxingStub() && (pDispatchingMT->GetCanonicalMethodTable() == pMDMT->GetCanonicalMethodTable())));
#endif // _DEBUG
        }
    }

    pbRetVal = pMD->DoPrestub(pDispatchingMT);

    TIMELINE_END(STARTUP_PRESTUB, ("PreStubWorker"));

    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;


    POSTCONDITION(CheckPointer(pbRetVal));

    SetLastError(dwLastError);

    return pbRetVal;
}

Stub* InsertUnmanagedCodeCheckSecurityStub(MethodDesc* pMD, Stub* pStub)
{
    CONTRACT(Stub*)
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pStub));
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    mdTypeDef cl;

    {
        cl = pMD->GetMethodTable()->GetCl();
    }

    // We may need to perform a runtime security check (in which case we'll
    // indirect through yet another stub). The check is disabled if the
    // interface we're calling through is marked with a runtime check
    // suppression attribute.
    if (Security::IsSecurityOn() &&
        pMD->GetMDImport()->GetCustomAttributeByName(cl,
                                                     COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                     NULL,
                                                     NULL) == S_FALSE)
    {
        LOG((LF_CORDB, LL_INFO10000,
             "::PSW: Placing security interceptor before delegate dllimport stub " FMT_ADDR "\n",
             DBG_ADDR(pStub)));

        LPVOID stubEntryPoint = (LPVOID) pStub->GetEntryPoint();
        UINT_PTR uSecurityStubTokenThis = Security::GetSecurityStubToken(pMD, DECLSEC_UNMNGD_ACCESS_DEMAND);

        // Replace existing stub with a security stub that calls the Com Interop stub
        Stub* pSecurityStub = MakeSecurityWorker(pMD, DECLSEC_UNMNGD_ACCESS_DEMAND, uSecurityStubTokenThis, pStub, stubEntryPoint);

        if ( pSecurityStub == NULL)
        {
            // no security stub was created (possibly empty demand)
            LOG((LF_CORDB, LL_INFO10000,
                 "::PSW: no security interceptor stub needed\n"));
        }
        else
        {
            LOG((LF_CORDB, LL_INFO10000,
             "::PSW security interceptor stub " FMT_ADDR "\n", DBG_ADDR(pSecurityStub)));


            pStub = pSecurityStub;
        }
    }

    RETURN pStub;
}

#ifdef _DEBUG 
//
static void TestSEHGuardPageRestoreOverflow()
{
}

static void TestSEHGuardPageRestore()
{
        PAL_TRY
        {
            TestSEHGuardPageRestoreOverflow();
        }
        PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
            _ASSERTE(!"Got first overflow.");
        }
        PAL_ENDTRY;

        PAL_TRY
        {
            TestSEHGuardPageRestoreOverflow();
        }
        PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
        {
            // If you get two asserts, then it works!
            _ASSERTE(!"Got second overflow.");
        }
        PAL_ENDTRY;
}
#endif // _DEBUG

// Separated out the body of PreStubWorker for the case where we don't have a frame.
//
// Note that pDispatchingMT may not actually be the MT that is indirected through.
// If a virtual method is called non-virtually, pMT will be used to indirect through
const BYTE * MethodDesc::DoPrestub(MethodTable *pDispatchingMT)
{
    CONTRACT(const BYTE *)
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        POSTCONDITION(CheckPointer(RETVAL));
    }
    CONTRACT_END;

    Stub *pStub = NULL;
    TADDR pCode = NULL;

    Thread *pThread = GetThread();

    MethodTable *pMT = GetMethodTable();

   // Running a prestub on the methods causes us to access its
    // MethodTable. Record this fact for the profiling data.
    if (g_IBCLogger.InstrEnabled())
    {
        g_IBCLogger.LogMethodDescAccess(this);
        g_IBCLogger.LogMethodCodeAccess(this);
    }

    /**************************   DEBUG CHECKS  *************************/
    /*-----------------------------------------------------------------
    // Halt if needed, GC stress, check the sharing count etc.
    */

#ifdef _DEBUG 
    static unsigned ctr = 0;
    ctr++;

    if (g_pConfig->ShouldPrestubHalt(this))
    {
        _ASSERTE(!"PreStubHalt");
    }

    LOG((LF_CLASSLOADER, LL_INFO10000, "In PreStubWorker for %s::%s\n",
                m_pszDebugClassName, m_pszDebugMethodName));

    // This is a nice place to test out having some fatal EE errors. We do this only in a checked build, and only
    // under the InjectFatalError key.
    if (g_pConfig->InjectFatalError() == 1)
    {
        EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE);
    }
    else if (g_pConfig->InjectFatalError() == 2)
    {
        EEPOLICY_HANDLE_FATAL_ERROR(COR_E_STACKOVERFLOW);
    }
    else if (g_pConfig->InjectFatalError() == 3)
    {
        TestSEHGuardPageRestore();
    }

    // Are we using the prestub more than we expect to?
    if ((m_iPrestubCalls > m_iPermittedPrestubCalls) &&
        (m_iPrestubCalls >= 0) && (m_iPermittedPrestubCalls >= 0) && // protect against overflow
        !IsSharedByGenericInstantiations() &&
        !MayBeRemotingIntercepted())
    {
        ThreadStoreLockHolder tsLock(TRUE);

        // Relax rules for multi-threaded case since multiple threads can hit
        // the prestub of an as-yet-unjitted method
        LONG threadCount = ThreadStore::s_pThreadStore->MaxThreadCountInEE();

        // We don't increment m_iPrestubCalls on the finalizer thread, so we can ignore it
        // This lets us enforce that m_iPrestubCalls is <= 1 for simple single-threaded apps
        _ASSERTE(threadCount > 1); threadCount--;

        LONG iPrestubCalls = m_iPrestubCalls;
        LONG iPermittedPrestubCalls = m_iPermittedPrestubCalls;

        if ((iPrestubCalls >= 0) && (iPermittedPrestubCalls >= 0)) // protect against overflow
        {
            // Stress logging if we are getting close
            if (iPrestubCalls > iPermittedPrestubCalls)
            {
                STRESS_LOG4(LF_CLASSLOADER, LL_INFO10000,
                    "Prestub called multiple times on %pM (limit: %d (* %d threads)) called %d times.",
                    this, iPermittedPrestubCalls, threadCount, iPrestubCalls);
            }

            // All threads can race about every patched slot in the worst case, thus the multiplication
            CONSISTENCY_CHECK_MSGF(((UINT64)iPrestubCalls) <= ((UINT64)iPermittedPrestubCalls * (UINT64)threadCount),
                ("Prestub called multiple times on %s::%s (limit: %d (* %d threads)) called %d times.",
                m_pszDebugClassName, m_pszDebugMethodName, iPermittedPrestubCalls, threadCount, iPrestubCalls));
        }
    }

    // Useful to test GC with the prestub on the call stack
    if (g_pConfig->ShouldPrestubGC(this))
    {
        GCHeap::GetGCHeap()->GarbageCollect(-1);
    }


#endif // _DEBUG

    STRESS_LOG1(LF_CLASSLOADER, LL_INFO10000, "Prestubworker: method %pM\n", this);


#ifdef STRESS_HEAP 
        // Force a GC on every jit if the stress level is high enough
    if (g_pConfig->GetGCStressLevel() != 0
#ifdef _DEBUG 
        && !g_pConfig->FastGCStressLevel()
#endif
        )
        GCHeap::GetGCHeap()->StressHeap();
#endif // STRESS_HEAP



    // HACK: This is to handle a punted work item dealing with a skipped module constructor
    //       due to appdomain unload. Basically shared code was JITted in domain A, and then
    //       this caused a link to another shared module with a module CCTOR, which was skipped
    //       or aborted in another appdomain we were trying to propagate the activation to.
    //
    //       Note that this is not a fix, but that it just minimizes the window in which the
    //       issue can occur. </STRIP>
    if (pThread->IsAbortRequested())
    {
        pThread->HandleThreadAbort();
    }

    /**************************   CLASS CONSTRUCTOR   ********************/

    if (!pMT->IsSharedByGenericInstantiations() /*&&
        !pMT->IsDomainNeutral()*/)
    {
        pMT->CheckRunClassInitThrowing();
    }

    /**************************   BACKPATCHING   *************************/
    // See if the addr of code has changed from the pre-stub

    if (!IsPointingToPrestub())
    {
        LOG((LF_CLASSLOADER, LL_INFO10000,
                "    In PreStubWorker, method already jitted, backpatching call point\n"));

        DoBackpatch((pDispatchingMT != NULL) ? pDispatchingMT : pMT);

#ifdef _DEBUG 
        IncPrestubCalls();
#endif // _DEBUG

        RETURN (BYTE*)GetStableEntryPoint();
    }

    // record if remoting needs to intercept this call
    BOOL  fRemotingIntercepted = IsRemotingInterceptedViaPrestub();
    BOOL fSecurityIntercepted = Security::IsSecurityOn() && PrestubMayInsertSecurityStub();
    
    /**************************   CODE CREATION  *************************/
    if (IsUnboxingStub())
    {
        pStub = MakeUnboxingStubWorker(this);
    }
#if defined(FEATURE_SHARE_GENERIC_CODE) 
    else if (IsInstantiatingStub())
    {
        pStub = MakeInstantiatingStubWorker(this);
    }
#endif // defined(FEATURE_SHARE_GENERIC_CODE)
    else if (IsInterface() && !IsStatic())
    {
        pStub = MakeStubWorker(this);
    }
    else if (IsIL() || IsNoMetadata())
    {
        // remember if we need to backpatch the MethodTable slot
        BOOL  fBackpatch           = !fSecurityIntercepted
                                    && !fRemotingIntercepted
                                    && !IsEnCMethod();


        if (pCode == NULL)
        {
            NewHolder<COR_ILMETHOD_DECODER> pHeader(NULL);
            // Get the information on the method
            if (!IsNoMetadata())
            {
                COR_ILMETHOD* ilHeader = GetILHeader();
                if(ilHeader == NULL)
                {
                    COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_IL);
                }

                COR_ILMETHOD_DECODER::DecoderStatus status = COR_ILMETHOD_DECODER::FORMAT_ERROR;

                {
                    // Decoder ctor can AV on a malformed method header
                    AVInRuntimeImplOkayHolder AVOkay(TRUE);
                    pHeader = new COR_ILMETHOD_DECODER(ilHeader, GetMDImport(), &status);
                    if(pHeader == NULL)
                        status = COR_ILMETHOD_DECODER::FORMAT_ERROR;
                }

                if (status == COR_ILMETHOD_DECODER::VERIFICATION_ERROR &&
                    Security::CanSkipVerification(GetModule()->GetDomainAssembly()))
                {
                    status = COR_ILMETHOD_DECODER::SUCCESS;
                }

                if (status != COR_ILMETHOD_DECODER::SUCCESS ||
                    !GetModule()->CheckIL(GetRVA(), pHeader->GetHeaderSize()))
                {
                    if (status == COR_ILMETHOD_DECODER::VERIFICATION_ERROR)
                    {
                        // Throw a verification HR
                        COMPlusThrowHR(COR_E_VERIFICATION);
                    }
                    else
                    {
                        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_IL);
                    }
                }

            } // end if (!IsNoMetadata())

            // JIT it
            LOG((LF_CLASSLOADER, LL_INFO1000000,
                    "    In PreStubWorker, calling MakeJitWorker\n"));

            // Create the precode eagerly if it is going to be needed later.
            if (!fBackpatch)
            {
                GetOrCreatePrecode();
            }

            pCode = MakeJitWorker(pHeader, 0);

        } // end if (pCode == NULL)
    } // end else if (IsIL() || IsNoMetadata())
    else if (IsFCall())
    {
        BOOL fBackpatch = !fRemotingIntercepted && !RequiresPrestub();

        // Create the precode eagerly if it is going to be needed later.
        if (!fBackpatch)
        {
            GetOrCreatePrecode();
        }

        // Get the fcall implementation
        pCode = (TADDR)ECall::GetFCallImpl(this);

        if (pCode == NULL)
            ECall::ReportMissingFCall(this);

        if (!HasPrecode())
        {
            SetStableEntryPointInterlocked(pCode);
        }
        else
        {
            // should be set by CacheFCallImpl in ecall.cpp
            _ASSERTE(GetPrecode()->GetNativeCode() == pCode);
        }
    }
    else //!IsUnboxingStub() && !IsInstantiatingStub() && !IsIL() && !IsFCall() case
    {
        // do all the other stubs.
        pStub = MakeStubWorker(this);

        // If this was a delegate to unmanaged code, and we don't have security permissions, then throw.
        if ( GetMethodTable()->IsAnyDelegateClass() &&
            ((DelegateEEClass*)GetMethodTable()->GetClass())->m_pMLStub != NULL)
        {
            pStub = InsertUnmanagedCodeCheckSecurityStub(this, pStub);
        }
    }

    /**************************   POSTJIT *************************/

    _ASSERTE(pCode == NULL || pCode == GetNativeCode());

    // At this point we must have either a pointer to managed code or to a stub. All of the above code
    // should have thrown an exception if it couldn't make a stub.
    _ASSERTE((pStub != NULL) ^ (pCode != NULL));

    /**************************   SECURITY   *************************/

    // Lets check to see if we need declarative security on this stub, If we have
    // security checks on this method or class then we need to add an intermediate
    // stub that performs declarative checks prior to calling the real stub.
     // record if security needs to intercept this call (also depends on whether we plan to use stubs for declarative security)
    
    if (Security::UseNoStubsApproach())
    {
        // for normal JIT methods, we don't build stubs, 
        // for COM Interop and p/invokes we will continue to use stubs.
        if (IsNDirect() || IsComPlusCall())
        {
            fSecurityIntercepted = IsInterceptedForDeclSecurity();
        }
    }   
    if (fSecurityIntercepted)
    {
        DWORD dwSecurityFlags = GetSecurityFlagsDuringPreStub();
        _ASSERTE(dwSecurityFlags && " Expected some runtime security action");
        UINT_PTR uSecurityStubToken = Security::GetSecurityStubToken(this, dwSecurityFlags);

        LOG((LF_CORDB, LL_INFO10000,
             "::PSW: Placing security interceptor before real stub 0x%08x\n",
             pStub));

        Stub *pSecurityStub = MakeSecurityWorker(this, dwSecurityFlags, uSecurityStubToken, 
            pStub, (pStub != NULL) ? (LPVOID)pStub->GetEntryPoint() : (LPVOID)pCode);

        if (pSecurityStub != NULL)
        {
            LOG((LF_CORDB, LL_INFO10000,
                "::PSW security interceptor stub 0x%08x\n",pSecurityStub));

            pStub = pSecurityStub;
            pCode = NULL;
        }
    }

#ifndef HAS_REMOTING_PRECODE
    /**************************   REMOTING   *************************/

    // check for MarshalByRef scenarios ... we need to intercept
    // Non-virtual calls on MarshalByRef types
    if (fRemotingIntercepted)
    {
        // let us setup a remoting stub to intercept all the calls
        Stub *pRemotingStub = CRemotingServices::GetStubForNonVirtualMethod(this, 
            (pStub != NULL) ? (LPVOID)pStub->GetEntryPoint() : (LPVOID)pCode, pStub);
        
        if (pRemotingStub != NULL)
        {
            pStub = pRemotingStub;
            pCode = NULL;
        }
    }
#endif // HAS_REMOTING_PRECODE

    _ASSERTE((pStub != NULL) ^ (pCode != NULL));

    if (pCode != NULL)
    {
        if (HasPrecode())
            GetPrecode()->SetTargetInterlocked(pCode, FALSE);
    }
    else
    {
        if (!GetOrCreatePrecode()->SetTargetInterlocked((TADDR)pStub->GetEntryPoint()))
        {
            pStub->DecRef();
        }
        else
        if (pStub->HasExternalEntryPoint())
        {
            // If the Stub wraps code that is outside of the Stub allocation, then we
            // need to free the Stub allocation now.
            pStub->DecRef();
        }
    }

    _ASSERTE(!IsPointingToPrestub());
    _ASSERTE(HasStableEntryPoint());

    // Interfaces are never backpatched
    if (!pMT->IsInterface())
    {
        // Make sure to backpatch the MethodTable that owns this MethodDesc
        if (pDispatchingMT != pMT)
            DoBackpatch(pMT, TRUE);

        if (pDispatchingMT != NULL)
            DoBackpatch(pDispatchingMT, TRUE);
    }

#ifdef _DEBUG
    IncPrestubCalls();
#endif // _DEBUG

    RETURN (BYTE*)GetStableEntryPoint();
}

BOOL MethodDesc::PrestubMayInsertStub(CORINFO_ACCESS_FLAGS accessFlags/*=CORINFO_ACCESS_ANY*/)
{
    WRAPPER_CONTRACT;

    return
        // The remoting interception is not necessary if we are calling on the same thisptr
        (!(accessFlags & CORINFO_ACCESS_THIS) && IsRemotingInterceptedViaPrestub())
        || (Security::IsSecurityOn() && PrestubMayInsertSecurityStub());
}

BOOL MethodDesc::PrestubMayProduceStub(CORINFO_ACCESS_FLAGS accessFlags/*=CORINFO_ACCESS_ANY*/)
{
    WRAPPER_CONTRACT;

    return IsWrapperStub() || PrestubMayInsertStub(accessFlags);
}

BOOL MethodDesc::PrestubMayInsertSecurityStub()
{
    WRAPPER_CONTRACT;

        // We do not insert a security stub around the shared code method descriptor
    // for instantiated generic methods, i.e. anything which requires a hidden
    // instantiation argument.  Instead we insert it around the instantiating stubs
    // and ensure that we call the instantiating stubs directly.
    return Security::UseNoStubsApproach() ? FALSE : (IsInterceptedForDeclSecurity() != 0 && !RequiresInstArg());
}

BOOL MethodDesc::RequiresPrestub()
{
    WRAPPER_CONTRACT;

    // Certain method descs may appear as directly callable since they
    // do not require method-desc calling convention, but they still require 
    // prestub for other reasons (e.g. to guarantee uniqueness of entrypoints)

    return 
        // Unboxing and instantiating stubs are always generated at runtime
        IsWrapperStub() ||
        // Interfaces always go through prestub for com and remoting interception
        IsInterface() ||
        // Fake ctors share one implementation that has to be wrapped by prestub
        (IsFCall() && ECall::IsSharedFCallImpl((TADDR)ECall::GetFCallImpl(this)));
}

BOOL MethodDesc::RequiresStableEntryPoint(BOOL fEstimateForChunk /*=FALSE*/)
{
    WRAPPER_CONTRACT;

    // Create precodes for edit and continue to make methods updateable
    if (IsEnCMethod() || IsEnCAddedMethod())
        return TRUE;

    // Precreate precodes for LCG methods so we do not leak memory when the method descs are recycled
    if (IsLCGMethod())
        return TRUE;

    if (fEstimateForChunk)
    {
        // Note that the estimate for the entire chunk does. We just make a best guess based
        // on the method table of the chunk.
        if (IsInterface())
            return TRUE;
    }
    else
    {       
        // Wrapper stubs are stored in generic dictionary that's not backpatched
        if (IsWrapperStub())
            return TRUE;

        if ((IsInterface() && !IsStatic()) || IsComPlusCall())
            return TRUE;
    }

    return FALSE;
}

#endif // !DACCESS_COMPILE


BOOL MethodDesc::MayBeRemotingIntercepted()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    MethodTable *pMT = GetMethodTable();
    return
        !IsStatic()
        && ((g_pObjectClass == pMT
                && (this->GetSlot() != g_pObjectCtorMD->GetSlot())        // the .ctor doesn't do anything, so let's speed up object allocation by not adding the remoting stub
                && (this->GetSlot() != g_pObjectFinalizerMD->GetSlot()))  // finalizer doesn't do anything either
         || pMT->IsMarshaledByRef());
}

BOOL MethodDesc::IsRemotingInterceptedViaPrestub()
{
    WRAPPER_CONTRACT;
    // We do not insert a remoting stub around the shared code method descriptor
    // for instantiated generic methods, i.e. anything which requires a hidden
    // instantiation argument.  Instead we insert it around the instantiating stubs
    // and ensure that we call the instantiating stubs directly.
    return MayBeRemotingIntercepted() && !IsVtableMethod() && !RequiresInstArg();
}

BOOL MethodDesc::IsRemotingInterceptedViaVirtualDispatch()
{
    WRAPPER_CONTRACT;
    return MayBeRemotingIntercepted() && IsVtableMethod();
}

BOOL MethodDesc::ComputeMayHaveNativeCode(BOOL fEstimateForChunk /*=FALSE*/)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(IsRestored());
    }
    CONTRACTL_END

    // This code flow of this method should roughly match the code flow of MethodDesc::DoPrestub.

    switch (GetClassification())
    {
    case mcIL:              // IsIL() case. Handled below.
        break;
    case mcFCall:           // FCalls always have native code.
        return TRUE;
    case mcNDirect:         // NDirect never have native code (note that the NDirect method
        return FALSE;       //  does not appear as having a native code even for stubs as IL)
    case mcEEImpl:          // Runtime provided implementation. No native code.
        return FALSE;
    case mcArray:           // Runtime provided implementation. No native code.
        return FALSE;
    case mcInstantiated:    // IsIL() case. Handled below.
        break;     
    case mcDynamic:         // LCG or stub-as-il.
        return TRUE; 
    default:
        _ASSERTE(!"Unknown classification");
    }

    _ASSERTE(IsIL());

    if (fEstimateForChunk)
    {
        // Note that the estimate for the entire chunk does. We just make a best guess based
        // on the methodtable of the chunk.
        MethodTable* pMT = GetMethodTable();
        if (pMT->IsInterface() || pMT->ContainsGenericVariables())
        {
            return FALSE;
        }
    }
    else
    {
        if ((IsInterface() && !IsStatic()) || IsWrapperStub() || ContainsGenericVariables() || IsAbstract())
        {
            return FALSE;
        }
    }

    return TRUE;
}

//==========================================================================
// The following code manages the PreStub. All method stubs initially
// use the prestub. Note that method's do not IncRef the prestub as they
// do their regular stubs. This PreStub is permanent.
//==========================================================================
GPTR_IMPL(Stub, g_preStub);
GPTR_IMPL(Stub, g_UMThunkPreStub);

ThePreStubManager   *ThePreStubManager::g_pManager = NULL;

#ifndef DACCESS_COMPILE 

void ThePreStubManager::Init(void)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    //
    // Add the prestub manager
    //

    g_pManager = new ThePreStubManager();

    StubManager::AddStubManager(g_pManager);
}

//-----------------------------------------------------------
// Initialize the prestub.
//-----------------------------------------------------------
void InitPreStubManager(void)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    g_preStub        = GeneratePrestub();
    g_UMThunkPreStub = GenerateUMThunkPrestub();


    ThePreStubManager::Init();
}

#endif // !DACCESS_COMPILE

//-----------------------------------------------------------
// Access the prestub (NO incref.)
//-----------------------------------------------------------
Stub *ThePreStub()
{
    LEAF_CONTRACT;
    return g_preStub;
}

Stub *TheUMThunkPreStub()
{
    LEAF_CONTRACT;
    return g_UMThunkPreStub;
}


#ifndef DACCESS_COMPILE 

void CallDefaultConstructor(OBJECTREF ref)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    MethodTable *pMT = ref->GetTrueMethodTable();

    PREFIX_ASSUME(pMT != NULL);

    if (!pMT->HasDefaultConstructor())
    {
        SString ctorMethodName(SString::Utf8, COR_CTOR_METHOD_NAME);
        COMPlusThrowNonLocalized(kMissingMethodException, ctorMethodName.GetUnicode());
    }

    g_IBCLogger.LogEEClassAndMethodTableAccess(pMT->GetClass());

    GCPROTECT_BEGIN (ref);
    
    MethodDesc *pMD = pMT->GetDefaultConstructor();

    static MetaSig *sig = NULL;
    if (sig == NULL)
    {
        // Allocate a metasig to use for all default constructors.
        void *tempSpace = SystemDomain::System()->GetHighFrequencyHeap()->AllocMem(sizeof(MetaSig));
        PCCOR_SIGNATURE pSig;
        DWORD cbSigSize;
        gsig_IM_RetVoid.GetBinarySig(&pSig, &cbSigSize);
        sig = new (tempSpace) MetaSig(pSig, cbSigSize, SystemDomain::SystemModule(), NULL, NULL);
    }

    MethodDescCallSite ctor(pMD, sig);    

    ARG_SLOT arg = ObjToArgSlot(ref);

    ctor.Call(&arg);
    GCPROTECT_END ();
}

#endif // !DACCESS_COMPILE
