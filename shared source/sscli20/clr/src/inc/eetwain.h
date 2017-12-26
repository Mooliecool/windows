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
//
// EETwain.h
//
// This file has the definition of ICodeManager and EECodeManager.
//
// ICorJitCompiler compiles the IL of a method to native code, and stores
// auxilliary data called as GCInfo (via ICorJitInfo::allocGCInfo()).
// The data is used by the EE to manage the method's garbage collection,
// exception handling, stack-walking etc.
// This data can be parsed by an ICodeManager corresponding to that
// ICorJitCompiler.
//
// EECodeManager is an implementation of ICodeManager for a default format
// of GCInfo. Various ICorJitCompiler's are free to share this format so that
// they do not need to provide their own implementation of ICodeManager
// (though they are permitted to, if they want).
//
//*****************************************************************************
#ifndef _EETWAIN_H
#define _EETWAIN_H
//*****************************************************************************

#include <daccess.h>
#include "regdisp.h"
#include "corjit.h"     // For NativeVarInfo
#include "stackwalktypes.h"



#if CHECK_APP_DOMAIN_LEAKS
#define CHECK_APP_DOMAIN    GC_CALL_CHECK_APP_DOMAIN
#else
#define CHECK_APP_DOMAIN    0
#endif

struct EHContext;

typedef void (*GCEnumCallback)(
    LPVOID          hCallback,      // callback data
    OBJECTREF*      pObject,        // address of obect-reference we are reporting
    DWORD           flags           // is this a pinned and/or interior pointer
);

/******************************************************************************
  The stackwalker maintains some state on behalf of ICodeManager.
*/

const int CODEMAN_STATE_SIZE = 256;

struct CodeManState
{
    DWORD       dwIsSet; // Is set to 0 by the stackwalk as appropriate
    BYTE        stateBuf[CODEMAN_STATE_SIZE];
};

/******************************************************************************
   These flags are used by some functions, although not all combinations might
   make sense for all functions.
*/

enum ICodeManagerFlags
{
    ActiveStackFrame =  0x0001, // this is the currently active function
    ExecutionAborted =  0x0002, // execution of this function has been aborted
                                    // (i.e. it will not continue execution at the
                                    // current location)
    AbortingCall    =   0x0004, // The current call will never return
    UpdateAllRegs   =   0x0008, // update full register set
    CodeAltered     =   0x0010, // code of that function might be altered
                                    // (e.g. by debugger), need to call EE
                                    // for original code
    SpeculativeStackwalk    = 0x0020, // we're in the middle of a stackwalk seeded
                                    // by an untrusted source (e.g., sampling profiler)
};

//*****************************************************************************
//
// This interface is used by ICodeManager to get information about the
// method whose GCInfo is being processed.
// It is useful so that some information which is available elsewhere does
// not need to be cached in the GCInfo.
// It is similar to corinfo.h - ICorMethodInfo
//

class ICodeInfo
{
public:

    // this function is for debugging only.  It returns the method name
    // and if 'moduleName' is non-null, it sets it to something that will
    // says which method (a class name, or a module name)
    virtual const char*  __stdcall getMethodName(const char **moduleName /* OUT */ ) = 0;

    virtual void         __stdcall getMethodSig(CORINFO_SIG_HANDLE    *phsig,        /* OUT */
                                                DWORD                 *pcbSigSize,   /* OUT */
                                                CORINFO_MODULE_HANDLE *phscope) = 0; /* OUT */

    // Start IP of the method
    virtual LPVOID       __stdcall getStartAddress() = 0;

    // Return the JIT manager of the method
    virtual IJitManager* __stdcall getJitManager() = 0;

    // Are calls to the method synchronized?
    virtual bool         __stdcall IsSynchronized() = 0;

    // Is this method code shared across instantiations of the generic type,
    // with instantiation-specific accesses done via the "this" object?
    virtual bool         __stdcall AcquiresInstMethodTableFromThis() = 0;
    
    // Is this method code shared across instantiations of the generic type/method,
    // with instantiation-specific accesses done via an extra hidden argument?
    virtual bool __stdcall          RequiresInstArg() = 0;
    
};

//*****************************************************************************
//
// ICodeManager is the abstract class that all CodeManagers
// must inherit from.  This will probably need to move into
// cor.h and become a real com interface.
//
//*****************************************************************************

class ICodeManager
{
    VPTR_BASE_VTABLE_CLASS(ICodeManager)

public:

/*
    Last chance for the runtime support to do fixups in the context
    before execution continues inside a filter, catch handler, or fault/finally
*/

enum ContextType
{
    FILTER_CONTEXT,
    CATCH_CONTEXT,
    FINALLY_CONTEXT
};

/* Type of funclet corresponding to a shadow stack-pointer */

enum
{
    SHADOW_SP_IN_FILTER = 0x1,
    SHADOW_SP_FILTER_DONE = 0x2,
    SHADOW_SP_BITS = 0x3
};

#ifndef DACCESS_COMPILE

virtual void FixContext(ContextType     ctxType,
                        EHContext      *ctx,
                        LPVOID          methodInfoPtr,
                        LPVOID          methodStart,
                        DWORD           nestingLevel,
                        OBJECTREF       thrownObject,
                        CodeManState   *pState,
                        size_t       ** ppShadowSP,             // OUT
                        size_t       ** ppEndRegion) = 0;       // OUT


/*
    Gets the ambient stack pointer value at the given nesting level within
    the method.
*/
virtual TADDR GetAmbientSP(EHContext      *ctx,
                           LPVOID          methodInfoPtr,
                           DWORD           nestingLevel,
                           ICodeInfo      *pCodeInfo,
                           CodeManState   *pState) = 0;

#endif // #ifndef DACCESS_COMPILE

/*
    Unwind the current stack frame, i.e. update the virtual register
    set in pContext. This will be similar to the state after the function
    returns back to caller (IP points to after the call, Frame and Stack
    pointer has been reset, callee-saved registers restored
    (if UpdateAllRegs), callee-UNsaved registers are trashed)
    Returns success of operation.
*/
virtual bool UnwindStackFrame(PREGDISPLAY     pContext,
                              LPVOID          methodInfoPtr,
                              ICodeInfo      *pCodeInfo,
                              unsigned        flags,
                              CodeManState   *pState,
                              StackwalkCacheUnwindInfo  *pUnwindInfo) = 0;

/*
    Is the function currently at a "GC safe point" ?
    Can call EnumGcRefs() successfully
*/
virtual bool IsGcSafe(PREGDISPLAY     pContext,
                      LPVOID          methodInfoPtr,
                      ICodeInfo      *pCodeInfo,
                      unsigned        flags) = 0;


/*
    Enumerate all live object references in that function using
    the virtual register set. Same reference location cannot be enumerated
    multiple times (but all differenct references pointing to the same
    object have to be individually enumerated).
    Returns success of operation.
*/
virtual bool EnumGcRefs(PREGDISPLAY     pContext,
                        LPVOID          methodInfoPtr,
                        ICodeInfo      *pCodeInfo,
                        unsigned        curOffs,
                        unsigned        flags,
                        GCEnumCallback  pCallback,
                        LPVOID          hCallBack) = 0;

/*
    Return the address of the local security object reference
    (if available).
*/
virtual OBJECTREF* GetAddrOfSecurityObject(CrawlFrame *pCF) = 0;

/*
    For a non-static method, "this" pointer is passed in as argument 0.
    However, if there is a "ldarga 0" or "starg 0" in the IL, 
    JIT will create a copy of arg0 and redirect all "ldarg(a) 0" and "starg 0" to this copy.
    (See Compiler::lvaArg0Var for more details.)
    
    The following method returns the original "this" argument, i.e. the one that is passed in,
    if it is a non-static method AND the object is still alive. 
    Returns NULL in all other cases.
*/
virtual OBJECTREF GetInstance(PREGDISPLAY     pContext,
                              LPVOID          methodInfoPtr,
                              ICodeInfo *     pCodeInfo,
                              unsigned        relOffset) = 0;

/*
    Returns the extra argument passed to to shared generic code if it is still alive.
    Returns NULL in all other cases.
*/
virtual void *GetParamTypeArg(PREGDISPLAY     pContext,
                              LPVOID          methodInfoPtr,
                              ICodeInfo *     pCodeInfo,
                              unsigned        relOffset) = 0;

/*
    Returns the offset of the GuardStack cookie if it exists.
    Returns NULL if there is no cookie.
*/
virtual void * GetGSCookieAddr(PREGDISPLAY     pContext,
                               LPVOID          methodInfoPtr,
                               ICodeInfo     * pCodeInfo,
                               unsigned        relOffset,
                               CodeManState  * pState) = 0;

/*
  Returns true if the given IP is in the given method's prolog or an epilog.
*/
virtual bool IsInPrologOrEpilog(DWORD  relPCOffset,
                                LPVOID methodInfoPtr,
                                size_t* prologSize) = 0;

/*
  Returns true if the given IP is in the synchronized region of the method (valid for synchronized methods only)
*/
virtual bool IsInSynchronizedRegion(
                DWORD           relOffset,
                LPVOID          methodInfoPtr,
                unsigned        flags) = 0;

/*
  Returns the size of a given function as reported in the GC info (does
  not take procedure splitting into account).  For the actual size of
  the hot region call IJitManager::JitTokenToMethodHotSize.
*/
virtual size_t GetFunctionSize(LPVOID methodInfoPtr) = 0;

/*
  Returns the size of the frame (barring localloc)
*/
virtual unsigned int GetFrameSize(LPVOID methodInfoPtr) = 0;

#ifndef DACCESS_COMPILE

/* Debugger API */

virtual const BYTE*     GetFinallyReturnAddr(PREGDISPLAY pReg)=0;

virtual BOOL            IsInFilter(void *methodInfoPtr,
                                   unsigned offset,
                                   PCONTEXT pCtx,
                                   DWORD curNestLevel) = 0;

virtual BOOL            LeaveFinally(void *methodInfoPtr,
                                     unsigned offset,
                                     PCONTEXT pCtx) = 0;

virtual void            LeaveCatch(void *methodInfoPtr,
                                   unsigned offset,
                                   PCONTEXT pCtx)=0;


#endif // #ifndef DACCESS_COMPILE


#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags) = 0;
#endif
};

#ifndef FJITONLY

//*****************************************************************************
//
// EECodeManager is the EE's implementation of the ICodeManager which
// supports the default format of GCInfo.
//
//*****************************************************************************

struct hdrInfo;

class EECodeManager : public ICodeManager {

    VPTR_VTABLE_CLASS(EECodeManager, ICodeManager)

public:


#ifndef DACCESS_COMPILE

/*
    Last chance for the runtime support to do fixups in the context
    before execution continues inside a filter, catch handler, or finally
*/
virtual
void FixContext(ContextType     ctxType,
                EHContext      *ctx,
                LPVOID          methodInfoPtr,
                LPVOID          methodStart,
                DWORD           nestingLevel,
                OBJECTREF       thrownObject,
                CodeManState   *pState,
                size_t       ** ppShadowSP,             // OUT
                size_t       ** ppEndRegion);           // OUT


/*
    Gets the ambient stack pointer value at the given nesting level within
    the method.
*/
virtual
TADDR GetAmbientSP(EHContext      *ctx,
                   LPVOID          methodInfoPtr,
                   DWORD           nestingLevel,
                   ICodeInfo      *pCodeInfo,
                   CodeManState   *pState);

#endif // #ifndef DACCESS_COMPILE

/*
    Unwind the current stack frame, i.e. update the virtual register
    set in pContext. This will be similar to the state after the function
    returns back to caller (IP points to after the call, Frame and Stack
    pointer has been reset, callee-saved registers restored
    (if UpdateAllRegs), callee-UNsaved registers are trashed)
    Returns success of operation.
*/
virtual
bool UnwindStackFrame(
                PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo      *pCodeInfo,
                unsigned        flags,
                CodeManState   *pState,
                StackwalkCacheUnwindInfo  *pUnwindInfo);

enum QuickUnwindFlag
{
    UnwindCurrentStackFrame,
    EnsureCallerStackFrameIsValid
};

/*
  *  Light unwind the current stack frame, using provided cache entry.
  *  only pPC and Esp of pContext are updated. And pEbp if necessary.
  */

static
void QuickUnwindStackFrame(
             PREGDISPLAY pRD,
             StackwalkCacheEntry *pCacheEntry,
             QuickUnwindFlag flag);

/*
    Is the function currently at a "GC safe point" ?
    Can call EnumGcRefs() successfully
*/
virtual
bool IsGcSafe(  PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo      *pCodeInfo,
                unsigned        flags);


/*
    Enumerate all live object references in that function using
    the virtual register set. Same reference location cannot be enumerated
    multiple times (but all differenct references pointing to the same
    object have to be individually enumerated).
    Returns success of operation.
*/
virtual
bool EnumGcRefs(PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo      *pCodeInfo,
                unsigned        pcOffset,
                unsigned        flags,
                GCEnumCallback  pCallback,
                LPVOID          hCallBack);

/*
   Return the address of the local security object reference
   using data that was previously cached before in UnwindStackFrame
   using StackwalkCacheUnwindInfo
*/
static OBJECTREF* GetAddrOfSecurityObjectFromCachedInfo(
        PREGDISPLAY pRD,
        StackwalkCacheUnwindInfo * stackwalkCacheUnwindInfo);

virtual
OBJECTREF* GetAddrOfSecurityObject(CrawlFrame *pCF);

virtual
OBJECTREF GetInstance(
                PREGDISPLAY     pContext,
                LPVOID          methodInfoPtr,
                ICodeInfo *     pCodeInfo,
                unsigned        relOffset);

/*
    Returns the extra argument passed to to shared generic code if it is still alive.
    Returns NULL in all other cases.
*/
virtual
void *GetParamTypeArg(PREGDISPLAY     pContext,
                      LPVOID          methodInfoPtr,
                      ICodeInfo *     pCodeInfo,
                      unsigned        relOffset);


/*
    Returns the offset of the GuardStack cookie if it exists.
    Returns NULL if there is no cookie.
*/
virtual
void * GetGSCookieAddr(PREGDISPLAY     pContext,
                       LPVOID          methodInfoPtr,
                       ICodeInfo     * pCodeInfo,
                       unsigned        relOffset,
                       CodeManState  * pState);



/*
  Returns true if the given IP is in the given method's prolog or an epilog.
*/
virtual
bool IsInPrologOrEpilog(
                DWORD           relOffset,
                LPVOID          methodInfoPtr,
                size_t*         prologSize);

/*
  Returns true if the given IP is in the synchronized region of the method (valid for synchronized functions only)
*/
virtual
bool IsInSynchronizedRegion(
                DWORD           relOffset,
                LPVOID          methodInfoPtr,
                unsigned        flags);

/*
  Returns the size of a given function.
*/
virtual
size_t GetFunctionSize(
                LPVOID          methodInfoPtr);

/*
  Returns the size of the frame (barring localloc)
*/
virtual
unsigned int GetFrameSize(
                LPVOID          methodInfoPtr);

#ifndef DACCESS_COMPILE

virtual const BYTE* GetFinallyReturnAddr(PREGDISPLAY pReg);
virtual BOOL LeaveFinally(void *methodInfoPtr,
                          unsigned offset,
                          PCONTEXT pCtx);
virtual BOOL IsInFilter(void *methodInfoPtr,
                        unsigned offset,
                        PCONTEXT pCtx,
                          DWORD curNestLevel);
virtual void LeaveCatch(void *methodInfoPtr,
                         unsigned offset,
                         PCONTEXT pCtx);


#endif // #ifndef DACCESS_COMPILE

#ifndef _X86_
    static void EnsureCallerContextIsValid( PREGDISPLAY pRD, StackwalkCacheEntry* pCacheEntry, ICodeInfo* pCodeInfo = NULL );
    static size_t GetCallerSp( PREGDISPLAY  pRD );
#endif

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

};


/*****************************************************************************
 */

enum regNum
{
        REGI_EAX, REGI_ECX, REGI_EDX, REGI_EBX,
        REGI_ESP, REGI_EBP, REGI_ESI, REGI_EDI,
        REGI_COUNT,
        REGI_NA = REGI_COUNT
};

/*****************************************************************************
 Register masks
 */

enum RegMask
{
    RM_EAX = 0x01,
    RM_ECX = 0x02,
    RM_EDX = 0x04,
    RM_EBX = 0x08,
    RM_ESP = 0x10,
    RM_EBP = 0x20,
    RM_ESI = 0x40,
    RM_EDI = 0x80,

    RM_NONE = 0x00,
    RM_ALL = (RM_EAX|RM_ECX|RM_EDX|RM_EBX|RM_ESP|RM_EBP|RM_ESI|RM_EDI),
    RM_CALLEE_SAVED = (RM_EBP|RM_EBX|RM_ESI|RM_EDI),
    RM_CALLEE_TRASHED = (RM_ALL & ~RM_CALLEE_SAVED),
};

/*****************************************************************************
 *
 *  Helper to extract basic info from a method info block.
 */


typedef unsigned __int64 ptrArgTP;

inline BOOL nonZero(const ptrArgTP& arg) {LEAF_CONTRACT;  return(arg != 0); }
inline BOOL intersect(const ptrArgTP arg1, const ptrArgTP arg2) {
    LEAF_CONTRACT;

    return ((arg1 & arg2) != 0);
}
inline void setDiff(ptrArgTP& target, const ptrArgTP& arg) {
    LEAF_CONTRACT;

    target &= ~arg;
}

#define MAX_PTRARG_OFS  (sizeof(ptrArgTP) * 8)

struct hdrInfo
{
    unsigned int        methodSize;     // native code bytes
    unsigned int        argSize;        // in bytes
    unsigned int        stackSize;      /* including callee saved registers */
    unsigned int        rawStkSize;     /* excluding callee saved registers */

    unsigned int        prologSize;

    // Size of the epilogs in the method.
    // For methods which use CEE_JMP, some epilogs may end with a "ret" instruction
    // and some may end with a "jmp". The epilogSize reported should be for the 
    // epilog with the smallest size.
    unsigned int        epilogSize;

    unsigned char       epilogCnt;
    bool                epilogEnd;      // is the epilog at the end of the method
    
    bool                ebpFrame;       // locals and arguments addressed relative to EBP
    bool                doubleAlign;    // is the stack double-aligned? locals addressed relative to ESP, and arguments relative to EBP
    bool                interruptible;  // intr. at all times (excluding prolog/epilog), not just call sites

    bool                securityCheck;  // has a slot for security object
    bool                handlers;       // has callable handlers
    bool                localloc;       // uses localloc
    bool                editNcontinue;  // has been compiled in EnC mode
    bool                varargs;        // is this a varargs routine
    bool                profCallbacks;  // does the method have Enter-Leave callbacks
    bool                isSpeculativeStackWalk; // is the stackwalk seeded by an untrusted source (e.g., sampling profiler)?

    // These always includes EBP for EBP-frames and double-aligned-frames
    RegMask             savedRegMask:8; // which callee-saved regs are saved on stack

    // Count of the callee-saved registers, excluding the frame pointer.
    // This does not include EBP for EBP-frames and double-aligned-frames.
    unsigned int        savedRegsCountExclFP;

    unsigned int        untrackedCnt;
    unsigned int        varPtrTableSize;
    unsigned int        argTabOffset;   // INVALID_ARGTAB_OFFSET if argtab must be reached by stepping through ptr tables
    unsigned int        gsCookieOffset; // INVALID_GS_COOKIE_OFFSET if there is no GuardStack cookie

    unsigned int        syncStartOffset; // start/end code offset of the protected region in synchronized methods.
    unsigned int        syncEndOffset;   // INVALID_SYNC_OFFSET if there not synchronized method
    unsigned int        syncEpilogStart; // The start of the epilog. Synchronized methods are guaranteed to have no more than one epilog.

    enum { NOT_IN_PROLOG = -1, NOT_IN_EPILOG = -1 };
    
    int                 prologOffs;     // NOT_IN_PROLOG if not in prolog
    int                 epilogOffs;     // NOT_IN_EPILOG if not in epilog. It is never 0

    //
    // Results passed back from scanArgRegTable
    //
    regNum              thisPtrResult;  // register holding "this"
    RegMask             regMaskResult;  // registers currently holding GC ptrs
    RegMask            iregMaskResult;  // iptr qualifier for regMaskResult
    ptrArgTP            argMaskResult;  // pending arguments mask
    ptrArgTP           iargMaskResult;  // iptr qualifier for argMaskResult
    unsigned            argHnumResult;
    const BYTE *         argTabResult;  // Table of encoded offsets of pending ptr args
    unsigned              argTabBytes;  // Number of bytes in argTabResult[]
};

/*****************************************************************************
  How the stackwalkers buffer will be interpreted
*/

struct CodeManStateBuf
{
    DWORD       hdrInfoSize;
    hdrInfo     hdrInfoBody;
};

#endif // !FJITONLY

//*****************************************************************************
#endif // _EETWAIN_H
//*****************************************************************************
