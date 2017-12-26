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
#include "common.h"
#include "stubmgr.h"
#include "virtualcallstub.h"


#ifdef LOGGING
const char *GetTType( TraceType tt);
void LogTraceDestination(const char * szHint, const void * stubAddr, TraceDestination * pTrace)
{
    if (pTrace->GetTraceType() == TRACE_UNJITTED_METHOD)
    {
        MethodDesc * md = pTrace->GetMethodDesc();
        LOG((LF_CORDB, LL_INFO10000, "'%s' yields '%s' to method 0x%p for input 0x%p.\n",
            szHint, GetTType(pTrace->GetTraceType()),
            md, stubAddr));
    }
    else
    {
        LOG((LF_CORDB, LL_INFO10000, "'%s' yields '%s' to address 0x%p for input 0x%p.\n",
            szHint, GetTType(pTrace->GetTraceType()),
            pTrace->GetAddress(), stubAddr));
    }
}
#endif

#ifdef _DEBUG
// Get a string representation of this TraceDestination
// Uses the supplied buffer to store the memory (or may return a string literal).
const WCHAR * TraceDestination::DbgToString(SString & buffer)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    const WCHAR * pValue = L"unknown";

#ifndef DACCESS_COMPILE  
    if (!StubManager::IsStubLoggingEnabled())
    {
        return L"<unavailable while native-debugging>";
    }
    // Now that we know we're not interop-debugging, we can safely call new.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;  

    
    FAULT_NOT_FATAL();

    EX_TRY
    {
        switch(this->type)
        {
            case TRACE_ENTRY_STUB:
                buffer.Printf("TRACE_ENTRY_STUB(addr=0x%p)", GetAddress());
                pValue = buffer.GetUnicode();
                break;

            case TRACE_STUB:
                buffer.Printf("TRACE_STUB(addr=0x%p)", GetAddress());
                pValue = buffer.GetUnicode();
                break;

            case TRACE_UNMANAGED:
                buffer.Printf("TRACE_UNMANAGED(addr=0x%p)", GetAddress());
                pValue = buffer.GetUnicode();
                break;

            case TRACE_MANAGED:
                buffer.Printf("TRACE_MANAGED(addr=0x%p)", GetAddress());
                pValue = buffer.GetUnicode();
                break;

            case TRACE_UNJITTED_METHOD:
            {
                MethodDesc * md = this->GetMethodDesc();
                buffer.Printf("TRACE_UNJITTED_METHOD(md=0x%p, %s::%s)", md, md->m_pszDebugClassName, md->m_pszDebugMethodName);
                pValue = buffer.GetUnicode();                
            }
                break;

            case TRACE_FRAME_PUSH:
                buffer.Printf("TRACE_FRAME_PUSH(addr=0x%p)", GetAddress());
                pValue = buffer.GetUnicode();                
                break;

            case TRACE_MGR_PUSH:
                buffer.Printf("TRACE_MGR_PUSH(addr=0x%p, sm=%s)", GetAddress(), this->GetStubManager()->DbgGetName());
                pValue = buffer.GetUnicode();
                break;

            case TRACE_OTHER:        
                pValue = L"TRACE_OTHER";
                break;
        }
    }
    EX_CATCH
    {
        pValue = L"(OOM while printing TD)";
    }
    EX_END_CATCH(SwallowAllExceptions);    
#endif            
    return pValue;
}
#endif


void TraceDestination::InitForUnjittedMethod(MethodDesc * pDesc)
{
    CONTRACTL
    {
        GC_NOTRIGGER;
        NOTHROW;
        MODE_ANY;

        PRECONDITION(CheckPointer(pDesc));
    }
    CONTRACTL_END;

    _ASSERTE(pDesc->SanityCheck());

    {
        // If this is a wrapper stub, then find the real method that it will go to and patch that.
        // This is more than just a convenience - converted wrapper MD to real MD is required for correct behavior.
        // Wrapper MDs look like unjitted MethodDescs. So when the debugger patches one, 
        // it won't actually bind + apply the patch (it'll wait for the jit-complete instead).
        // But if the wrapper MD is for prejitted code, then we'll never get the Jit-complete.
        // Thus it'll miss the patch completely.
        if (pDesc->IsWrapperStub())
        {
            MethodDesc * pNewDesc = NULL;

            FAULT_NOT_FATAL();


#ifndef DACCESS_COMPILE                        
            EX_TRY  
            {    
                pNewDesc = pDesc->GetExistingWrappedMethodDesc();
            }
            EX_CATCH
            {
                // In case of an error, we'll just stick w/ the original method desc.
            } EX_END_CATCH(SwallowAllExceptions)
#else
#endif

            if (pNewDesc != NULL)
            {
                pDesc = pNewDesc;

                LOG((LF_CORDB, LL_INFO10000, "TD::UnjittedMethod: wrapper md: %p --> %p", pDesc, pNewDesc));

            }
        }
    }


    this->type = TRACE_UNJITTED_METHOD;
    this->pDesc = pDesc;
    this->stubManager = NULL;
}


// Initialize statics.
#ifdef _DEBUG
SString * StubManager::s_pDbgStubManagerLog = NULL; 
CrstStatic StubManager::s_DbgLogCrst;

#endif

SPTR_IMPL(StubManager, StubManager, g_pFirstManager);

CrstStatic StubManager::s_StubManagerListCrst;

BOOL   StubManager::s_fTargetsDirty = FALSE;
DWORD  StubManager::s_dwNumTargets = 0;
TADDR* StubManager::s_pVirtualTraceCallTargets = NULL;

//-----------------------------------------------------------
// For perf reasons, the stub managers are now kept in a two
// tier system: all stub managers but the VirtualStubManagers
// are in the first tier. A VirtualStubManagerManager takes
// care of all VirtualStubManagers, and is iterated last of
// all. It does a smarter job of looking up the owning
// manager for virtual stubs, checking the current and shared
// appdomains before checking the remaining managers.
//
// Thus, this iterator will run the regular list until it
// hits the end, then it will check the VSMM, then it will
// end.
//-----------------------------------------------------------
class StubManagerIterator
{
  public:
    StubManagerIterator();
    ~StubManagerIterator();

    void Reset();
    BOOL Next();
    PTR_StubManager Current();

  protected:
    enum SMI_State
    {
        SMI_START,
        SMI_NORMAL,
        SMI_VIRTUALCALLSTUBMANAGER,
        SMI_END
    };

    SMI_State               m_state;
    PTR_StubManager m_pCurMgr;
    SimpleReadLockHolder    m_lh;
};

//-----------------------------------------------------------
// Ctor
//-----------------------------------------------------------
StubManagerIterator::StubManagerIterator()
{
    WRAPPER_CONTRACT;

    Reset();
}

void StubManagerIterator::Reset()
{
    LEAF_CONTRACT;
    m_pCurMgr = NULL;
    m_state = SMI_START;
}

//-----------------------------------------------------------
// Ctor
//-----------------------------------------------------------
StubManagerIterator::~StubManagerIterator()
{
    LEAF_CONTRACT;
}

//-----------------------------------------------------------
// Move to the next element. Iterators are created at
// start-1, so must call Next before using Current
//-----------------------------------------------------------
BOOL StubManagerIterator::Next()
{
    LEAF_CONTRACT;

    do {
        if (m_state == SMI_START) {
            m_state = SMI_NORMAL;
            m_pCurMgr = (PTR_StubManager) StubManager::g_pFirstManager;
        }
        else if (m_state == SMI_NORMAL) {
            if (m_pCurMgr != NULL) {
                m_pCurMgr = m_pCurMgr->m_pNextManager;
            }
            else {
                // If we've reached the end of the regular list of stub managers, then we
                // set the VirtualCallStubManagerManager is the current item (effectively
                // forcing it to always be the last manager checked).
                m_state = SMI_VIRTUALCALLSTUBMANAGER;
                VirtualCallStubManagerManager *pVCSMMgr = VirtualCallStubManagerManager::GlobalManager();
                m_pCurMgr = PTR_StubManager(pVCSMMgr);
#ifndef DACCESS_COMPILE
                m_lh.Assign(&pVCSMMgr->m_RWLock);
#endif
            }
        }
        else if (m_state == SMI_VIRTUALCALLSTUBMANAGER) {
            m_state = SMI_END;
            m_pCurMgr = NULL;
#ifndef DACCESS_COMPILE
            m_lh.Clear();
#endif
        }
    } while (m_state != SMI_END && m_pCurMgr == NULL);

    CONSISTENCY_CHECK(m_state == SMI_END || m_pCurMgr != NULL);
    return (m_state != SMI_END);
}

//-----------------------------------------------------------
// Get the current contents of the iterator
//-----------------------------------------------------------
PTR_StubManager StubManagerIterator::Current()
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(m_state != SMI_START);
    CONSISTENCY_CHECK(m_state != SMI_END);
    CONSISTENCY_CHECK(CheckPointer(m_pCurMgr));

    return m_pCurMgr;
}

#ifndef DACCESS_COMPILE
//-----------------------------------------------------------
//-----------------------------------------------------------
StubManager::StubManager()
  : m_pNextManager(NULL)
{
    LEAF_CONTRACT;
}

//-----------------------------------------------------------
//-----------------------------------------------------------
StubManager::~StubManager()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
    } CONTRACTL_END;

    UnlinkStubManager(this);
}
#endif // #ifndef DACCESS_COMPILE

#ifdef _DEBUG_IMPL
BOOL StubManager::IsSingleOwner(TADDR stubAddress, StubManager * pOwner)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    // ensure this stubmanager owns it.
    _ASSERTE(pOwner != NULL);

    // ensure nobody else does.
    bool ownerFound = false;
    int count = 0;
    StubManagerIterator it;
    while (it.Next())
    {
        // Callers would have iterated till pOwner.
        if (!ownerFound && it.Current() != pOwner)
            continue;

        if (it.Current() == pOwner)
            ownerFound = true;
            
        if (it.Current()->CheckIsStub_Worker(stubAddress))
        {
            // If you hit this assert, you can tell what 2 stub managers are conflicting by inspecting their vtable.
            CONSISTENCY_CHECK_MSGF((it.Current() == pOwner), ("Stub at 0x%p is owner by multiple managers (0x%p, 0x%p)",
                (void*) stubAddress, pOwner, it.Current()));            
            count++;
        }
        else
        {
            _ASSERTE(it.Current() != pOwner);
        }
    }

    _ASSERTE(ownerFound);
    
    // We expect pOwner to be the only one to own this stub.
    return (count == 1);
}
#endif

//-----------------------------------------------------------
//-----------------------------------------------------------
BOOL StubManager::CheckIsStub_Worker(TADDR stubStartAddress)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    if (stubStartAddress == NULL)
    {
        return FALSE;
    }

    BOOL fIsStub = FALSE;

    CONTRACT_VIOLATION(SOToleranceViolation);

#ifdef DACCESS_COMPILE
    PAL_TRY
#else    
    EX_TRY
#endif    
    {

#ifndef DACCESS_COMPILE    
        // Use CheckIsStub_Internal may AV. That's ok. 
        AVInRuntimeImplOkayHolder AVOkay(TRUE);
#endif

        // Make a Polymorphic call to derived stub manager.
        // Try to see if this address is for a stub. If the address is
        // completely bogus, then this might fault, so we protect it
        // with SEH.
        fIsStub = CheckIsStub_Internal(stubStartAddress);
    }
#ifdef DACCESS_COMPILE
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
#else
    EX_CATCH
#endif    
    {
        LOG((LF_CORDB, LL_INFO10000, "D::GASTSI: exception indicated addr is bad.\n"));

        fIsStub = FALSE;
    }
#ifdef DACCESS_COMPILE
    PAL_ENDTRY
#else
    EX_END_CATCH(SwallowAllExceptions);        
#endif    

    //END_SO_INTOLERANT_CODE;
    
    return fIsStub;
}

//-----------------------------------------------------------
// stubAddress may be an invalid address.
//-----------------------------------------------------------
BOOL StubManager::IsStub(TADDR stubAddress)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    StubManagerIterator it;
    while (it.Next())
    {
        if (it.Current()->CheckIsStub_Worker(stubAddress))
        {
            _ASSERTE_IMPL(IsSingleOwner(stubAddress, it.Current()));
            return TRUE;
        }
    }

    return FALSE;
}

//-----------------------------------------------------------
// Given an address, figure out a TraceDestination describing where
// the instructions at that address will eventually transfer execution to.
//-----------------------------------------------------------
BOOL StubManager::TraceStub(TADDR stubStartAddress, TraceDestination *trace)
{
    LEAF_CONTRACT;

    StubManagerIterator it;
    while (it.Next())
    {
        StubManager * pCurrent = it.Current();
        if (pCurrent->CheckIsStub_Worker((TADDR) stubStartAddress))
        {
            LOG((LF_CORDB, LL_INFO10000,
                 "StubManager::TraceStub: addr 0x%p claimed by mgr "
                 "0x%p.\n", stubStartAddress, pCurrent));

            _ASSERTE_IMPL(IsSingleOwner((TADDR) stubStartAddress, pCurrent));                

            BOOL fValid = pCurrent->DoTraceStub((const BYTE*) stubStartAddress, trace);
#ifdef _DEBUG
            if (IsStubLoggingEnabled())
            {
            DbgWriteLog("Doing TraceStub for Address 0x%p, claimed by '%s' (0x%p)\n", stubStartAddress, pCurrent->DbgGetName(), pCurrent);            
            if (fValid)
            {
                SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
                FAULT_NOT_FATAL();
                SString buffer;
                DbgWriteLog("  td=%S\n", trace->DbgToString(buffer));
            }
            else
            {
                DbgWriteLog("  stubmanager returned false. Does not expect to call managed code\n");
                
            }
            } // logging
#endif
            return fValid;
        }
    }

    if (ExecutionManager::FindCodeMan((SLOT)stubStartAddress) != NULL)
    {
        trace->InitForManaged((TADDR) stubStartAddress);

#ifdef _DEBUG
        DbgWriteLog("Doing TraceStub for Address 0x%p is jitted code claimed by codemanager\n", stubStartAddress);
#endif        

        LOG((LF_CORDB, LL_INFO10000,
             "StubManager::TraceStub: addr 0x%p is managed code\n",
             stubStartAddress));

        return TRUE;
    }

    LOG((LF_CORDB, LL_INFO10000,
         "StubManager::TraceStub: addr 0x%p unknown. TRACE_OTHER...\n",
         stubStartAddress));

#ifdef _DEBUG
    DbgWriteLog("Doing TraceStub for Address 0x%p is unknown!!!\n", stubStartAddress);
#endif            

    trace->InitForOther((TADDR) stubStartAddress);
    return FALSE;
}

//-----------------------------------------------------------
// Get a list of address to put a patch on for virtual trace call from each stub manager.
// NOTE: this function is not thread safe.  You need to call it under a lock.
//-----------------------------------------------------------
// static 
bool StubManager::GetVirtualTraceCallTargets(TADDR** ppTargets, DWORD* pdwNumTargets)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    CrstHolder ch(&s_StubManagerListCrst);

    if (s_fTargetsDirty)
    {
        s_fTargetsDirty = FALSE;

        if (s_pVirtualTraceCallTargets != NULL)
        {
            delete [] s_pVirtualTraceCallTargets;
            s_dwNumTargets = 0;
            s_pVirtualTraceCallTargets = NULL;
        }

        // Make sure everything is cleaned up.
        _ASSERTE(s_dwNumTargets == 0);
        _ASSERTE(s_pVirtualTraceCallTargets == NULL);

        // Ask each individual stub manager how many targets it has.
        StubManagerIterator it;
        while (it.Next())
        {
            StubManager* pCurrent = it.Current();
            pCurrent->GetVirtualTraceCallTarget(NULL, &s_dwNumTargets);
        }

        if (s_dwNumTargets > 0)
        {
            s_pVirtualTraceCallTargets = new (nothrow) TADDR[s_dwNumTargets];

            // We need to reset the number of targets in either case.
            s_dwNumTargets = 0;

            if (s_pVirtualTraceCallTargets == NULL)
            {
                // We can't do much here.
            }
            else
            {
                it.Reset();
                while (it.Next())
                {
                    StubManager* pCurrent = it.Current();
                    pCurrent->GetVirtualTraceCallTarget(s_pVirtualTraceCallTargets, &s_dwNumTargets);
                }
            }
        }
    }

    if (ppTargets)
    {
        *ppTargets = s_pVirtualTraceCallTargets;
    }
    if (pdwNumTargets)
    {
        *pdwNumTargets = s_dwNumTargets;
    }
    return true;
}

//-----------------------------------------------------------
//-----------------------------------------------------------
BOOL StubManager::FollowTrace(TraceDestination *trace)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    while (trace->GetTraceType() == TRACE_STUB)
    {
        LOG((LF_CORDB, LL_INFO10000,
             "StubManager::FollowTrace: TRACE_STUB for 0x%p\n",
             trace->GetAddress()));
        
        if (!TraceStub(trace->GetAddress(), trace))
        {
            //
            // No stub manager claimed it - it must be an EE helper or something.
            // 

            trace->InitForOther(trace->GetAddress());
        }
    }

    LOG_TRACE_DESTINATION(trace, NULL, "StubManager::FollowTrace");
    
    return trace->GetTraceType() != TRACE_OTHER;
}

#ifndef DACCESS_COMPILE

//-----------------------------------------------------------
//-----------------------------------------------------------
void StubManager::AddStubManager(StubManager *mgr)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(CheckPointer(g_pFirstManager, NULL_OK));
    CONSISTENCY_CHECK(CheckPointer(mgr));

    GCX_COOP_NO_THREAD_BROKEN();

    CrstHolder ch(&s_StubManagerListCrst);

    if (g_pFirstManager == NULL)
    {
        g_pFirstManager = mgr;
    }
    else
    {
        mgr->m_pNextManager = g_pFirstManager;
        g_pFirstManager = mgr;
    }

    s_fTargetsDirty = TRUE;

    LOG((LF_CORDB, LL_EVERYTHING, "StubManager::AddStubManager - 0x%p (vptr %x%p)\n", mgr, (*(PVOID*)mgr)));
}

//-----------------------------------------------------------
// NOTE: The runtime MUST be suspended to use this in a
//       truly safe manner.
//-----------------------------------------------------------
void StubManager::UnlinkStubManager(StubManager *mgr)
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(CheckPointer(g_pFirstManager, NULL_OK));
    CONSISTENCY_CHECK(CheckPointer(mgr));

    CrstHolder ch(&s_StubManagerListCrst);

    StubManager **m = &g_pFirstManager;
    while (*m != NULL) 
    {
        if (*m == mgr) 
        {
            *m = (*m)->m_pNextManager;
            s_fTargetsDirty = TRUE;
            return;
        }
        m = &(*m)->m_pNextManager;
    }
}

#endif // #ifndef DACCESS_COMPILE

//-----------------------------------------------------------
//-----------------------------------------------------------
MethodDesc *StubManager::MethodDescFromEntry(const BYTE *stubStartAddress, MethodTable *pMT)
{
    LEAF_CONTRACT;
    StubManagerIterator it;
    while (it.Next())
    {        
        MethodDesc *pMD = it.Current()->Entry2MethodDesc(stubStartAddress, pMT);
        if (pMD)
        {
            _ASSERTE_IMPL(IsSingleOwner((TADDR) stubStartAddress, it.Current()));
            return pMD;
        }
    }
    return NULL;
}

#ifdef DACCESS_COMPILE

//-----------------------------------------------------------
//-----------------------------------------------------------
void
StubManager::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // Report the global list head.
    DacEnumMemoryRegion(DacGlobalBase() +
                        g_dacGlobals.StubManager__g_pFirstManager,
                        sizeof(TADDR));

    //
    // Report the list contents.
    //
    
    StubManagerIterator it;
    while (it.Next())
    {
        it.Current()->DoEnumMemoryRegions(flags);
    }
}

//-----------------------------------------------------------
//-----------------------------------------------------------
void
StubManager::DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_VTHIS();
    EMEM_OUT(("MEM: %p StubManager base\n", PTR_HOST_TO_TADDR(this)));
}

//-----------------------------------------------------------
//-----------------------------------------------------------
LPCWSTR StubManager::FindStubManagerName(TADDR addr)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;
    
    StubManagerIterator it;
    while (it.Next())
    {
        if (it.Current()->CheckIsStub_Worker(addr))
        {
            _ASSERTE_IMPL(IsSingleOwner(addr, it.Current()));
            return it.Current()->GetStubManagerName(addr);
        }
    }

    return L"Unknown";
}

#endif // #ifdef DACCESS_COMPILE

//-----------------------------------------------------------
// Initialize the global stub manager service.
//-----------------------------------------------------------
void StubManager::InitializeStubManagers()
{
#if !defined(DACCESS_COMPILE)

#if defined(_DEBUG)
    s_DbgLogCrst.Init("StubManagerLogLock", CrstDebuggerHeapLock, (CrstFlags)(CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));    
#endif
    s_StubManagerListCrst.Init("StubManagerListLock", CrstDebuggerHeapLock, (CrstFlags)(CRST_UNSAFE_ANYMODE | CRST_DEBUGGER_THREAD));

#endif // !DACCESS_COMPILE
}

//-----------------------------------------------------------
// Terminate the global stub manager service.
//-----------------------------------------------------------
void StubManager::TerminateStubManagers()
{
#if !defined(DACCESS_COMPILE)

#if defined(_DEBUG)
    DbgFinishLog();
    s_DbgLogCrst.Destroy();
#endif

    s_StubManagerListCrst.Destroy();
#endif // !DACCESS_COMPILE

    if (s_pVirtualTraceCallTargets != NULL)
    {
        delete [] s_pVirtualTraceCallTargets;
    }
}

#ifdef _DEBUG

//-----------------------------------------------------------
// Should stub-manager logging be enabled?
//-----------------------------------------------------------
bool StubManager::IsStubLoggingEnabled()
{
    // Our current logging impl uses SString, which uses new(), which can't be called
    // on the helper thread. (B/c it may deadlock. See SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE)

    // We avoid this by just not logging when native-debugging.
    if (IsDebuggerPresent())
    {
        return false;
    }

    return true;
}


//-----------------------------------------------------------
// Call to reset the log. This is used at the start of a new step-operation.
// pThread is the managed thread doing the stepping. 
// It should either be the current thread or the helper thread.
//-----------------------------------------------------------
void StubManager::DbgBeginLog(TADDR addrCallInstruction, TADDR addrCallTarget)
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;


    // We can't call new() if another thread holds the heap lock and is then suspended by
    // an interop-debugging. Since this is debug-only logging code, we'll just skip
    // it under those cases.
    if (!IsStubLoggingEnabled())
    {
        return;
    }
    // Now that we know we're not interop-debugging, we can safely call new.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    FAULT_NOT_FATAL();

    {
        CrstHolder ch(&s_DbgLogCrst);
        EX_TRY
        {
            if (s_pDbgStubManagerLog == NULL)
            {
                s_pDbgStubManagerLog = new SString();
            }
            s_pDbgStubManagerLog->Clear();
        }
        EX_CATCH
        {
            DbgFinishLog();
        }
        EX_END_CATCH(SwallowAllExceptions);                
    }

    DbgWriteLog("Beginning Step-in. IP after Call instruction is at 0x%p, call target is at 0x%p\n", 
        addrCallInstruction, addrCallTarget);
#endif        
}

//-----------------------------------------------------------
// Finish logging for this thread.
// pThread is the managed thread doing the stepping. 
// It should either be the current thread or the helper thread.
//-----------------------------------------------------------
void StubManager::DbgFinishLog()
{
#ifndef DACCESS_COMPILE
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    CrstHolder ch(&s_DbgLogCrst);

    // Since this is just a tool for debugging, we don't care if we call new.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;    
    FAULT_NOT_FATAL();
    
    delete s_pDbgStubManagerLog;
    s_pDbgStubManagerLog = NULL;

       
#endif    
}


//-----------------------------------------------------------
// Write an arbitrary string to the log.
//-----------------------------------------------------------
void StubManager::DbgWriteLog(const CHAR *format, ...)
{
#ifndef DACCESS_COMPILE                        
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;


    if (!IsStubLoggingEnabled())
    {
        return;
    }

    // Since this is just a tool for debugging, we don't care if we call new.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    FAULT_NOT_FATAL();

    CrstHolder ch(&s_DbgLogCrst);

    if (s_pDbgStubManagerLog == NULL)
    {
        return;
    }

    // Suppress asserts about lossy encoding conversion in SString::Printf
    BOOL fEntered = CHECK::EnterAssert();

    EX_TRY
    {
        va_list args;
        va_start(args, format);
        s_pDbgStubManagerLog->AppendVPrintf(format, args);
        va_end(args); 
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);  

    if (fEntered) CHECK::LeaveAssert();
#endif
}



//-----------------------------------------------------------
// Get the log as a string.
//-----------------------------------------------------------
void StubManager::DbgGetLog(SString * pStringOut)
{
#ifndef DACCESS_COMPILE                        
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(pStringOut));
    }
    CONTRACTL_END;

    if (!IsStubLoggingEnabled())
    {
        return;
    }

    // Since this is just a tool for debugging, we don't care if we call new.
    SUPPRESS_ALLOCATION_ASSERTS_IN_THIS_SCOPE;
    FAULT_NOT_FATAL();

    CrstHolder ch(&s_DbgLogCrst);

    if (s_pDbgStubManagerLog == NULL)
    {
        return;
    }
    
    EX_TRY
    {
        pStringOut->Set(*s_pDbgStubManagerLog);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);    
#endif    
}


#endif // _DEBUG



#if !defined(DACCESS_COMPILE)
#endif // DACCCESS_COMPILE

//-----------------------------------------------------------
//-----------------------------------------------------------
BOOL ThePreStubManager::DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;

        PRECONDITION(CheckPointer(stubStartAddress));
        PRECONDITION(CheckPointer(trace));
    }
    CONTRACTL_END;
    
    //
    // We cannot tell where the stub will end up
    // until after the prestub worker has been run.
    //
    
    _ASSERTE(Stub::RecoverStub((TADDR)stubStartAddress) == ThePreStub());
    trace->InitForFramePush(ThePreStub()->GetPatchAddress());

    return TRUE;
}

//-----------------------------------------------------------
BOOL ThePreStubManager::CheckIsStub_Internal(TADDR stubStartAddress)
{
    LEAF_CONTRACT;
    return stubStartAddress == (TADDR)ThePreStub()->GetEntryPoint();
}
