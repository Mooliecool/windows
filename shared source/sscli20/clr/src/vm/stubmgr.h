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
// StubMgr.h
//
// The stub manager exists so that the debugger can accurately step through 
// the myriad stubs & wrappers which exist in the EE, without imposing undue 
// overhead on the stubs themselves.
//
// Each type of stub (except those which the debugger can treat as atomic operations)
// needs to have a stub manager to represent it.  The stub manager is responsible for
// (a) identifying the stub as such, and
// (b) tracing into the stub & reporting what the stub will call.  This
//        report can consist of
//              (i) a managed code address
//              (ii) an unmanaged code address
//              (iii) another stub address
//              (iv) a "frame patch" address - that is, an address in the stub, 
//                      which the debugger can patch. When the patch is hit, the debugger
//                      will query the topmost frame to trace itself.  (Thus this is 
//                      a way of deferring the trace logic to the frame which the stub
//                      will push.)
//
// The set of stub managers is extensible, but should be kept to a reasonable number
// as they are currently linearly searched & queried for each stub.
//

#ifndef __stubmgr_h__
#define __stubmgr_h__

// When 'TraceStub' returns, it gives the address of where the 'target' is for a stub'
// TraceType indicates what this 'target' is
enum TraceType
{
    TRACE_ENTRY_STUB,               // Stub goes to an unmanaged entry stub 
    TRACE_STUB,                     // Stub goes to another stub
    TRACE_UNMANAGED,                // Stub goes to unmanaged code
    TRACE_MANAGED,                  // Stub goes to Jitted code
    TRACE_UNJITTED_METHOD,          // Is the prestub, since there is no code, the address will actually be a MethodDesc*

    TRACE_FRAME_PUSH,               // Don't know where stub goes, stop at address, and then ask the frame that is on the stack
    TRACE_MGR_PUSH,                 // Don't know where stub goes, stop at address then call TraceManager() below to find out 

    TRACE_OTHER                     // We are going somewhere you can't step into (eg. ee helper function)
};

class StubManager;
class SString;

class DebuggerRCThread;

// A TraceDestination describes where code is going to call. This can be used by the Debugger's Step-In functionality
// to skip through stubs and place a patch directly at a call's target.
// TD are supplied by the stubmanagers.
class TraceDestination
{
public:
    friend class DebuggerRCThread;
    
    TraceDestination() { }

#ifdef _DEBUG
    // Get a string representation of this TraceDestination
    // Uses the supplied buffer to store the memory (or may return a string literal).
    // This will also print the TD's arguments.    
    const WCHAR * DbgToString(SString &buffer);
#endif

    // Initialize for unmanaged code.
    // The addr is in unmanaged code. Used for Step-in from managed to native.
    void InitForUnmanaged(TADDR addr)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        this->type = TRACE_UNMANAGED;
        this->address = addr;
        this->stubManager = NULL;        
    }

    // The addr is inside jitted code (eg, there's a JitManaged that will claim it)
    void InitForManaged(TADDR addr)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        this->type = TRACE_MANAGED;
        this->address = addr;
        this->stubManager = NULL;
    }

    // Initialize for an unmanaged entry stub.
    void InitForUnmanagedStub(TADDR addr)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        this->type = TRACE_ENTRY_STUB;
        this->address = addr;
        this->stubManager = NULL;
    }

    // Initialize for a stub.
    void InitForStub(TADDR addr)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        this->type = TRACE_STUB;
        this->address = addr;
        this->stubManager = NULL;
    }

    // Init for a managed unjitted method.
    // This will place an IL patch that will get bound when the debugger gets a Jit complete
    // notification for this method.
    // If pDesc is a wrapper methoddesc, we will unwrap it.
    void InitForUnjittedMethod(MethodDesc * pDesc);

    // Place a patch at the given addr, and then when it's hit,
    // call pStubManager->TraceManager() to get the next TraceDestination.
    void InitForManagerPush(TADDR addr, StubManager * pStubManager)
    {
        STATIC_CONTRACT_SO_TOLERANT;
        this->type = TRACE_MGR_PUSH;
        this->address = addr;
        this->stubManager = pStubManager;
    }

    // Place a patch at the given addr, and then when it's hit
    // call GetThread()->GetFrame()->TraceFrame() to get the next TraceDestination.
    // This address must be safe to run a callstack at.
    void InitForFramePush(TADDR addr)
    {
        this->type = TRACE_FRAME_PUSH;
        this->address = addr;
        this->stubManager = NULL;
    }

    // Nobody recognized the target address. We will not be able to step-in to it.
    // This is ok if the target just calls into mscorwks (such as an Fcall) because
    // there's no managed code to step in to, and we don't support debugging the CLR
    // itself, so there's no native code to step into either.
    void InitForOther(TADDR addr)
    {
        this->type = TRACE_OTHER;
        this->address = addr;
        this->stubManager = NULL;
    }

    // Accessors
    TraceType GetTraceType() { return type; }
    TADDR GetAddress() 
    {
        _ASSERTE(type != TRACE_UNJITTED_METHOD);
        return (TADDR) address; 
    }
    MethodDesc* GetMethodDesc()
    {
        _ASSERTE(type == TRACE_UNJITTED_METHOD);
        return pDesc;
    }    

    StubManager * GetStubManager()
    {
        return stubManager;
    }

    // Expose this b/c DebuggerPatchTable::AddPatchForAddress() needs it.
    // Ideally we'd get rid of this.
    void Bad_SetTraceType(TraceType t)
    {
        this->type = t;
    }
private:
    TraceType                       type;               // The kind of code the stub is going to
    TADDR                           address;            // Where the stub is going    
    StubManager                     *stubManager;       // The manager that claims this stub
    MethodDesc                      *pDesc;
};

// For logging
#ifdef LOGGING
    void LogTraceDestination(const char * szHint, const void * stubAddr, TraceDestination * pTrace);
    #define LOG_TRACE_DESTINATION(_tracedestination, stubAddr, _stHint)  LogTraceDestination(_stHint, stubAddr, _tracedestination)
#else
    #define LOG_TRACE_DESTINATION(_tracedestination, stubAddr, _stHint)    
#endif




typedef VPTR(class StubManager) PTR_StubManager;

class StubManager
{
    friend class StubManagerIterator;
    VPTR_BASE_VTABLE_CLASS(StubManager)
    
  public:
    // Startup and shutdown the global stubmanager service.
    static void InitializeStubManagers();
    static void TerminateStubManagers();

    // Does any sub manager recognise this EIP?
    static BOOL IsStub(TADDR stubAddress);
        
    // Look for stubAddress, if found return TRUE, and set 'trace' to 
    static BOOL TraceStub(TADDR tubAddress, TraceDestination *trace);
    
    static bool GetVirtualTraceCallTargets(TADDR** ppTargets, DWORD* pdwNumTargets);

    // if 'trace' indicates TRACE_STUB, keep calling TraceStub on 'trace', until you get out of all stubs
    // returns true if successfull
    static BOOL FollowTrace(TraceDestination *trace);
    
    // Get the methodDesc associated with a stub.
    static MethodDesc *MethodDescFromEntry(const BYTE *stubStartAddress, MethodTable*pMT);
            
#ifdef DACCESS_COMPILE
    static void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    static void AddStubManager(StubManager *mgr);

    // NOTE: Very important when using this. It is not thread safe, except in this very
    //       limited scenario: the thread must have the runtime suspended.
    static void UnlinkStubManager(StubManager *mgr);
    
#ifndef DACCESS_COMPILE
    StubManager();
    virtual ~StubManager();
#endif


#ifdef _DEBUG
    // Debug helper to help identify stub-managers. Make it pure to force stub managers to implement it.
    virtual const char * DbgGetName() = 0;
#endif

    // Only Stubmanagers that return 'TRACE_MGR_PUSH' as a trace type need to implement this function
    // Fills in 'trace' (the target), and 'pRetAddr' (the method that called the stub) (this is needed
    // as a 'fall back' so that the debugger can at least stop when the stub returns.  
    virtual BOOL TraceManager(Thread *thread, TraceDestination *trace,
                              CONTEXT *pContext, BYTE **pRetAddr)
    {
        LEAF_CONTRACT;

        _ASSERTE(!"Default impl of TraceManager should never be called!");
        return FALSE;
    }

    // If pTargets is NULL, then the stub managers just need to increment the counter.
    // Otherwise fill in the array using the counter as the index.
    virtual void GetVirtualTraceCallTarget(TADDR* pTargets, DWORD* pdwNumTargets)
    {
        LEAF_CONTRACT;

        return;
    }

    // The worker for IsStub. This calls CheckIsStub_Internal, but wraps it w/ 
    // a try-catch.
    BOOL CheckIsStub_Worker(TADDR stubStartAddress);



#ifdef _DEBUG
public:
    //-----------------------------------------------------------------------------
    // Debugging Stubmanager bugs is very painful. You need to figure out
    // how you go to where you got and which stub-manager is at fault.
    // To help with this, we track a rolling log so that we can give very
    // informative asserts. this log is not thread-safe, but we really only expect
    // a single stub-manager usage at a time.
    //
    // A stub manager for a step-in operation may be used across 
    // both the helper thread and then the managed thread doing the step-in.
    // These threads will coordinate to have exclusive access (helper will only access
    // when stopped; and managed thread will only access when running).
    //
    // It's also possible (but rare) for a single thread to have multiple step-in operations.
    // Since that's so rare, no present need to expand our logging to support it.    
    //-----------------------------------------------------------------------------


    static bool IsStubLoggingEnabled();

    // Call to reset the log. This is used at the start of a new step-operation.    
    static void DbgBeginLog(TADDR addrCallInstruction, TADDR addrCallTarget);
    static void DbgFinishLog();
    
    static void DbgWriteLog(const CHAR *format, ...);
    
    // Get the log as a string.
    static void DbgGetLog(SString * pStringOut);

protected:
    // Implement log as a SString.
    static SString * s_pDbgStubManagerLog;

    static CrstStatic s_DbgLogCrst;

#endif

        
protected:

    // Each stubmanaged implements this. 
    // This may throw, AV, etc depending on the implementation. This should not 
    // be called directly unless you know exactly what you're doing.
    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress) = 0;

    // The worker for TraceStub
    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace) = 0;

    // The worker for MethodDescFromEntry
    virtual MethodDesc *Entry2MethodDesc(const BYTE *stubStartAddress, MethodTable *pMT) = 0;

#ifdef _DEBUG_IMPL
    static BOOL IsSingleOwner(TADDR stubAddress, StubManager * pOwner);
#endif

#ifdef DACCESS_COMPILE
    virtual void DoEnumMemoryRegions(CLRDataEnumMemoryFlags flags);

  protected:
    // This is used by DAC to provide more information on who owns a stub.
    virtual LPCWSTR GetStubManagerName(TADDR addr) = 0;

  public:
    static LPCWSTR FindStubManagerName(TADDR addr);
#endif

private:
    SPTR_DECL(StubManager, g_pFirstManager);
    PTR_StubManager m_pNextManager;

    static CrstStatic s_StubManagerListCrst;

    static BOOL   s_fTargetsDirty;
    static DWORD  s_dwNumTargets;
    static TADDR* s_pVirtualTraceCallTargets;
};

//-----------------------------------------------------------
// Stub manager for the prestub.  Although there is just one, it has
// unique behavior so it gets its own stub manager.
//-----------------------------------------------------------
class ThePreStubManager : public StubManager
{
    VPTR_VTABLE_CLASS(ThePreStubManager, StubManager)
    
  public:
#ifndef DACCESS_COMPILE
    ThePreStubManager() { LEAF_CONTRACT; }
#endif

#ifdef _DEBUG
    virtual const char * DbgGetName() { return "ThePreStubManager"; }
#endif

    virtual BOOL CheckIsStub_Internal(TADDR stubStartAddress);

    virtual BOOL DoTraceStub(const BYTE *stubStartAddress, TraceDestination *trace);
    
    virtual MethodDesc *Entry2MethodDesc(const BYTE *stubStartAddress, MethodTable *pMT)
    {
        LEAF_CONTRACT;
        return NULL;
    }

    static ThePreStubManager *g_pManager;

#ifndef DACCESS_COMPILE
    static void Init(void);
#endif

#ifdef DACCESS_COMPILE
  protected:
    virtual LPCWSTR GetStubManagerName(TADDR addr)
        { LEAF_CONTRACT; return L"ThePreStub"; }
#endif
};

#endif
