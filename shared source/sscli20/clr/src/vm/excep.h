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
// EXCEP.H - Copyright (C) 1998 Microsoft Corporation
//

#ifndef __excep_h__
#define __excep_h__

#include "exceptmacros.h"
#include "comsystem.h"
#include "corerror.h"  // HResults for the COM+ Runtime
#include "corexcep.h"  // Exception codes for the COM+ Runtime
class Thread;

#include "../dlls/mscorrc/resource.h"

#include <excepcpu.h>

// Check if the Win32 Error code is an IO error.
BOOL IsWin32IOError(SCODE scode);

//******************************************************************************
//
//  SwallowUnhandledExceptions
//
//   Consult the EE policy and the app config to determine if the runtime should "swallow" unhandled exceptions.
//   Swallow if: the EEPolicy->UnhandledExceptionPolicy is "eHostDeterminedPolicy"
//           or: the app config value LegacyUnhandledExceptionPolicy() is set.
//
//  Parameters: 
//    none
//
//  Return value:
//    true - the runtime should "swallow" unhandled exceptions
//
inline bool SwallowUnhandledExceptions()
{
    return (eHostDeterminedPolicy == GetEEPolicy()->GetUnhandledExceptionPolicy()) ||
           g_pConfig->LegacyUnhandledExceptionPolicy() ||
           GetCompatibilityFlag(compatSwallowUnhandledExceptions);
}


#if defined(PAL_PORTABLE_SEH) && defined(__GNUC__)

LONG CppEHUnwindFilter(EXCEPTION_POINTERS* ep, LPVOID pv);

#define SEHSetCurrentException(er)

#if !defined(PLATFORM_UNIX)
#define LASTMOMENTUNWIND(target) PAL_DoLastMomentUnwind(target)
#else
#define LASTMOMENTUNWIND(target)
#endif

#define PAL_CPP_EHUNWIND_BEGIN    {                                        \
    INDEBUG(Frame *__pFrame = GetThread()->GetFrame();)                    \
    EXCEPTION_POINTERS ep;                                                 \
    PEXCEPTION_RECORD __caughtRecord = NULL;                               \
    EXCEPTION_REGISTRATION_RECORD __record;                                \
    PAL_GOOD(try) {                                                        \
        __record.Handler = CppEHUnwindFilter;                              \
        __record.pvFilterParameter = (LPVOID)&__record;                    \
        __record.dwFlags = PAL_EXCEPTION_FLAGS_CPPEHUNWIND;                \
        __record.typeOfHandler = PALExceptFilter;                          \
        PAL_TryHelper(&__record);                                          \

#define PAL_CPP_EHUNWIND_END                                               \
        PAL_EndTryHelper(&__record, 0);                                    \
    } PAL_GOOD(catch) (PEXCEPTION_RECORD pRecord) {                        \
        _ASSERTE(GetThread()->GetFrame() == __pFrame);                     \
        __caughtRecord = pRecord;                                          \
    }                                                                      \
    if (__caughtRecord) {                                                  \
        PEXCEPTION_REGISTRATION_RECORD __pBottom =                         \
            PAL_GetBottommostRegistration();                               \
                                                                           \
        ep.ExceptionRecord = (PEXCEPTION_RECORD)__pBottom->ReservedForPAL; \
            ep.ExceptionRecord->ExceptionFlags |= EXCEPTION_UNWINDING;     \
            ep.ContextRecord = NULL;                                       \
        CppEHUnwindFilter(&ep, (LPVOID)&__record);                         \
                                                                           \
        /* The handler returned. We may need a longjmp across jit'd code */\
        /* Also, a nested exception might've happened. Reset the         */\
        /* current exception for the PAL.                                */\
        SEHSetCurrentException(__caughtRecord);                            \
        __pBottom = PAL_GetBottommostRegistration();                       \
        if ((__pBottom != NULL) &&                                         \
            (__pBottom->dwFlags & PAL_EXCEPTION_FLAGS_LONGJMP)) {          \
                                                                           \
        /* Before we longjmp, we need to unwind the managed frames */      \
        /* that we're jumping across */                                    \
            UnwindFrameChain(GetThread(),                                  \
                             (Frame *)__pBottom->pvFilterParameter);       \
            /* if needed, unwind all gcc EH frames past the target ESP */  \
            LASTMOMENTUNWIND(__pBottom);                                   \
            PAL_longjmp(__pBottom->ReservedForPAL, (int)(SIZE_T)__caughtRecord);\
        } else {                                                           \
            PAL_EndTryHelper(&__record, 0);                                \
            UNWIND_CALL(__caughtRecord);                                   \
        }                                                                  \
    } }

#else // PAL_PORTABLE_SEH && PLATFORM_UNIX
#define PAL_CPP_EHUNWIND_BEGIN      {
#define PAL_CPP_EHUNWIND_END        }
#endif

// Enums
// return values of LookForHandler
enum LFH {
    LFH_NOT_FOUND = 0,
    LFH_FOUND = 1,
};

#include "runtimeexceptionkind.h"

class IJitManager;

//
// ThrowCallbackType is used to pass information to between various functions and the callbacks that they call
// during a managed stack walk.
//
struct ThrowCallbackType
{
    MethodDesc * pFunc;     // the function containing a filter that returned catch indication
    int     dHandler;       // the index of the handler whose filter returned catch indication
    BOOL    bIsUnwind;      // are we currently unwinding an exception
    BOOL    bUnwindStack;   // reset the stack before calling the handler? (Stack overflow only)
    BOOL    bAllowAllocMem; // are we allowed to allocate memory?
    BOOL    bDontCatch;     // can we catch this exception?
    BOOL    bLastChance;    // should we perform last chance handling?
    BOOL    bHandlingSO;    // true if we are handling an SO
    BYTE    *pStack;
    Frame * pTopFrame;
    Frame * pBottomFrame;
    MethodDesc * pProfilerNotify;   // Context for profiler callbacks -- see COMPlusFrameHandler().
    BOOL    bReplaceStack;  // Used to pass info to SaveStackTrace call
    BOOL    bSkipLastElement;// Used to pass info to SaveStackTrace call
#ifdef _DEBUG
    void * pCurrentExceptionRecord;
    void * pPrevExceptionRecord;
#endif

    void Init()
    {
        LEAF_CONTRACT;

        pFunc = NULL;
        dHandler = 0;
        bIsUnwind = FALSE;
        bUnwindStack = FALSE;
        bAllowAllocMem = TRUE;
        bDontCatch = FALSE;
        bLastChance = TRUE;
        bHandlingSO = FALSE;
        pStack = NULL;
        pTopFrame = (Frame *)-1;
        pBottomFrame = (Frame *)-1;
        pProfilerNotify = NULL;
        bReplaceStack = FALSE;
        bSkipLastElement = FALSE;
        
#ifdef _DEBUG
        pCurrentExceptionRecord = 0;
        pPrevExceptionRecord = 0;
#endif
    }
};



struct EE_ILEXCEPTION_CLAUSE;

void InitializeExceptionHandling();
void CLRAddVectoredHandlers(void);
void TerminateExceptionHandling();

// Prototypes
EXTERN_C VOID __stdcall ResetCurrentContext();
#ifdef _DEBUG
void CheckStackBarrier(EXCEPTION_REGISTRATION_RECORD *exRecord);
#endif
EXCEPTION_REGISTRATION_RECORD *FindNestedEstablisherFrame(EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame);
LFH LookForHandler(const EXCEPTION_POINTERS *pExceptionPointers, Thread *pThread, ThrowCallbackType *tct);
StackWalkAction COMPlusThrowCallback (CrawlFrame *pCf, ThrowCallbackType *pData);
void UnwindFrames(Thread *pThread, ThrowCallbackType *tct);

void UnwindFrameChain(Thread *pThread, LPVOID pvLimitSP);
DWORD MapWin32FaultToCOMPlusException(EXCEPTION_RECORD *pExceptionRecord);
DWORD ComputeEnclosingHandlerNestingLevel(IJitManager *pIJM, METHODTOKEN mdTok, SIZE_T offsNat);
DWORD ComputeEnclosingHandlerNestingLevel_DEPRECATED(IJitManager *pIJM, METHODTOKEN mdTok, SIZE_T offsNat);
BOOL IsException(MethodTable *pMT);
BOOL IsExceptionOfType(RuntimeExceptionKind reKind, OBJECTREF *pThrowable);
BOOL IsExceptionOfType(RuntimeExceptionKind reKind, Exception *pException);
BOOL IsAsyncThreadException(OBJECTREF *pThrowable);
BOOL IsUncatchable(OBJECTREF *pThrowable);
VOID FixupOnRethrow(Thread *pCurThread, EXCEPTION_POINTERS *pExceptionPointers);
BOOL UpdateCurrentThrowable(PEXCEPTION_RECORD pExceptionRecord);
BOOL IsStackOverflowException(Thread* pThread, EXCEPTION_RECORD* pExceptionRecord);
void WrapNonCompliantException(OBJECTREF *ppThrowable);
OBJECTREF PossiblyUnwrapThrowable(OBJECTREF throwable, Assembly *pAssembly);

#ifdef _DEBUG
// C++ EH cracking material gleaned from the debugger:
// (DO NOT USE THIS KNOWLEDGE IN NON-DEBUG CODE!!!)
void *DebugGetCxxException(EXCEPTION_RECORD* pExceptionRecord);
#endif


#ifdef _DEBUG_IMPL
BOOL IsValidClause(EE_ILEXCEPTION_CLAUSE *EHClause);
BOOL IsCOMPlusExceptionHandlerInstalled();
#endif

void InstallUnhandledExceptionFilter();
void UninstallUnhandledExceptionFilter();

LONG __stdcall COMUnhandledExceptionFilter(EXCEPTION_POINTERS *pExceptionInfo);


//////////////
// A list of places where we might have unhandled exceptions or other serious faults. These can be used as a mask in
// DbgJITDebuggerLaunchSetting to help control when we decide to ask the user about whether or not to launch a debugger.
//
enum UnhandledExceptionLocation
    {
    ProcessWideHandler    = 0x000001,
    ManagedThread         = 0x000002, // Does not terminate the application. CLR swallows the unhandled exception.
    ThreadPoolThread      = 0x000004, // ditto.
    FinalizerThread       = 0x000008, // ditto.
    FatalStackOverflow    = 0x000010,
    SystemNotification    = 0x000020, // CLR will swallow after the notification occurs
    FatalExecutionEngineException = 0x000040,
    ClassInitUnhandledException   = 0x000080, // Does not terminate the application. CLR transforms this into TypeInitializationException

    MaximumLocationValue  = 0x800000, // This is the maximum location value you're allowed to use. (Max 24 bits allowed.)

    // This is a mask of all the locations that the debugger will attach to by default.
    DefaultDebuggerAttach = ProcessWideHandler |
                            FatalStackOverflow |
                            FatalExecutionEngineException
};

LONG ThreadBaseExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID _location);
LONG ThreadBaseExceptionSwallowingFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID _location);
LONG ThreadBaseExceptionAppDomainFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID _location);

// Filter for calls out from the 'vm' to native code, if there's a possibility of SEH exceptions
// in the native code.
LONG CallOutFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID pv);


void DECLSPEC_NORETURN RaiseDeadLockException();

void STDMETHODCALLTYPE DefaultCatchHandler(PEXCEPTION_POINTERS pExceptionInfo,
                                           OBJECTREF *Throwable = NULL,
                                           BOOL useLastThrownObject = FALSE,
                                           BOOL isTerminating = FALSE,
                                           BOOL isThreadBaseFilter = FALSE,
                                           BOOL sendAppDomainEvents = TRUE);

BOOL COMPlusIsMonitorException(EXCEPTION_POINTERS *pExceptionInfo);
BOOL COMPlusIsMonitorException(EXCEPTION_RECORD *pExceptionRecord,
                               CONTEXT *pContext);

void ReplaceExceptionContextRecord(CONTEXT *pTarget, CONTEXT *pSource);

// Localization helper function
void ResMgrGetString(LPCWSTR wszResourceName, STRINGREF * ppMessage);

// externs

//==========================================================================
// Various routines to throw COM+ objects.
//==========================================================================

//==========================================================================
// Throw an undecorated runtime exception with a specific string parameter
// that won't be localized.  If possible, try using
// COMPlusThrow(reKind, LPCWSTR wszResourceName) instead.
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrowNonLocalized(RuntimeExceptionKind reKind, LPCWSTR wszTag);

//==========================================================================
// Throw an object.
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrow(OBJECTREF pThrowable);

//==========================================================================
// Throw an undecorated runtime exception.
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrow(RuntimeExceptionKind reKind);

//==========================================================================
// Throw an undecorated runtime exception with a localized message.  Given
// a resource name, the ResourceManager will find the correct paired string
// in our .resources file.
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrow(RuntimeExceptionKind reKind, LPCWSTR wszResourceName);

//==========================================================================
// Throw a decorated runtime exception.
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrow(RuntimeExceptionKind  reKind, UINT resID, LPCWSTR wszArg1 = NULL, 
                                        LPCWSTR wszArg2 = NULL, LPCWSTR wszArg3 = NULL, LPCWSTR wszArg4 = NULL, 
                                        LPCWSTR wszArg5 = NULL, LPCWSTR wszArg6 = NULL, LPCWSTR wszArg7 = NULL, 
                                        LPCWSTR wszArg8 = NULL, LPCWSTR wszArg9 = NULL, LPCWSTR wszArg10 = NULL);


//==========================================================================
// Throw a runtime exception based on an HResult. Note that for the version 
// of RealCOMPlusThrowHR that takes a resource ID, the HRESULT will be 
// passed as the first substitution string (%1).
//==========================================================================

enum tagGetErrorInfo
{
    kGetErrorInfo
};

VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr, IErrorInfo* pErrInfo);
VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr, tagGetErrorInfo);
VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr);
VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr, UINT resID, LPCWSTR wszArg1 = NULL, LPCWSTR wszArg2 = NULL, 
                                          LPCWSTR wszArg3 = NULL, LPCWSTR wszArg4 = NULL, LPCWSTR wszArg5 = NULL, 
                                          LPCWSTR wszArg6 = NULL, LPCWSTR wszArg7 = NULL, LPCWSTR wszArg8 = NULL, 
                                          LPCWSTR wszArg9 = NULL);


//==========================================================================
// Throw a runtime exception based on the last Win32 error (GetLastError())
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrowWin32();
VOID DECLSPEC_NORETURN RealCOMPlusThrowWin32(HRESULT hr);


//==========================================================================
// Create an exception object
// Note that this may not succeed due to problems creating the exception
// object. On failure, it will set pInitException to the value of
// pInnerException, and will set pThrowable to the exception that got thrown
// while trying to create the TypeInitializationException object, which
// could be due to other type init issues, OOM, thread abort, etc.
// pInnerException (may be NULL) and pInitException and are IN params.
// pThrowable is an OUT param.
//==========================================================================
void CreateTypeInitializationExceptionObject(LPCWSTR pTypeThatFailed,
                                             OBJECTREF *pInnerException, 
                                             OBJECTREF *pInitException,
                                             OBJECTREF *pThrowable);

//==========================================================================
// Examine an exception object
//==========================================================================

ULONG GetExceptionMessage(OBJECTREF throwable,
                          __inout_ecount(bufferLength) LPWSTR buffer,
                          ULONG bufferLength);
void GetExceptionMessage(OBJECTREF throwable, SString &result);
HRESULT GetExceptionHResult(OBJECTREF throwable);
DWORD GetExceptionXCode(OBJECTREF throwable);

void ExceptionPreserveStackTrace(OBJECTREF throwable);


//==========================================================================
// Create an exception object for an HRESULT
//==========================================================================

void GetExceptionForHR(HRESULT hr, IErrorInfo* pErrInfo, OBJECTREF* pProtectedThrowable);
void GetExceptionForHR(HRESULT hr, OBJECTREF* pProtectedThrowable);

//==========================================================================
// Throw an ArithmeticException
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrowArithmetic();

//==========================================================================
// Throw an ArgumentNullException
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentNull(LPCWSTR argName, LPCWSTR wszResourceName);

VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentNull(LPCWSTR argName);

//==========================================================================
// Throw an ArgumentOutOfRangeException
//==========================================================================

VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentOutOfRange(LPCWSTR argName, LPCWSTR wszResourceName);

//==========================================================================
// Throw an ArgumentException
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentException(LPCWSTR argName, LPCWSTR wszResourceName);

//==========================================================================
// Throw an InvalidCastException
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowInvalidCastException(TypeHandle thCastFrom, TypeHandle thCastTo);

#include "eexcp.h"
#include "exinfo.h"

struct FrameHandlerExRecord 
{
    EXCEPTION_REGISTRATION_RECORD m_ExReg;
    // to preserve the state between CPFH_RealFirstPassHandler and COMPlusAfterUnwind
    ThrowCallbackType m_tct;
    Frame *m_pEntryFrame;
    Frame *GetCurrFrame() 
    {
        LEAF_CONTRACT;
        return m_pEntryFrame;
    }
};

struct NestedHandlerExRecord : public FrameHandlerExRecord 
{
    ExInfo m_handlerInfo;
    BOOL   m_ActiveForUnwind;
    NestedHandlerExRecord() : m_handlerInfo() {LEAF_CONTRACT;}
    void Init(EXCEPTION_REGISTRATION_RECORD *pNext, PEXCEPTION_ROUTINE pFrameHandler, Frame *pEntryFrame)
    {
        WRAPPER_CONTRACT;

        m_ExReg.Next=pNext;
        m_ExReg.Handler=pFrameHandler;
        m_pEntryFrame=pEntryFrame;
        m_handlerInfo.Init();
        m_ActiveForUnwind = FALSE;
    }
};


//-------------------------------------------------------------------------------
// This simply tests to see if the exception object is a subclass of
// the descriminating class specified in the exception clause.
//-------------------------------------------------------------------------------
extern "C" BOOL ExceptionIsOfRightType(TypeHandle clauseType, TypeHandle thrownType);

//==========================================================================
// The stuff below is what works "behind the scenes" of the public macros.
//==========================================================================

EXTERN_C LPVOID __stdcall COMPlusEndCatch( Thread *pCurThread, CONTEXT *pCtx, void *pSEH = NULL );
EXTERN_C LPVOID __fastcall COMPlusCheckForAbort(LPVOID retAddress, LPVOID esp, LPVOID ebp);

BOOL        IsThreadHijackedForThreadStop(Thread* pThread, EXCEPTION_RECORD* pExceptionRecord);
void        AdjustContextForThreadStop(Thread* pThread, CONTEXT* pContext);
OBJECTREF   CreateCOMPlusExceptionObject(Thread* pThread, EXCEPTION_RECORD* pExceptionRecord, BOOL bAsynchronousThreadStop);


#define PAL_EXCEPTION_FLAGS_UnwindCallback      0x40000000
#define PAL_EXCEPTION_FLAGS_ContextTransition   0x20000000
#define PAL_EXCEPTION_FLAGS_All                 0xFFFF0000


EXCEPTION_HANDLER_DECL(COMPlusFrameHandler);
EXCEPTION_HANDLER_DECL(COMPlusNestedExceptionHandler);

// Pop off any SEH handlers we have registered below pTargetSP
VOID __cdecl PopSEHRecords(LPVOID pTargetSP);
VOID PopNestedExceptionRecords(LPVOID pTargetSP);
VOID PopNestedExceptionRecords(LPVOID pTargetSP, CONTEXT *pCtx, void *pSEH);

// Misc functions to access and update the SEH chain. Be very, very careful about updating the SEH chain,
// especially with RemoveSEHRecordOutOfOrder. Frankly, if you think you need to use one of these function, please
// consult with the owner of the exception system.
PEXCEPTION_REGISTRATION_RECORD GetCurrentSEHRecord();
VOID SetCurrentSEHRecord(EXCEPTION_REGISTRATION_RECORD *pSEH);
VOID RemoveSEHRecordOutOfOrder(EXCEPTION_REGISTRATION_RECORD *pEHR);


#define STACK_OVERWRITE_BARRIER_SIZE 20
#define STACK_OVERWRITE_BARRIER_VALUE 0xabcdefab

#ifdef _DEBUG
struct FrameHandlerExRecordWithBarrier {
    DWORD m_StackOverwriteBarrier[STACK_OVERWRITE_BARRIER_SIZE];
    FrameHandlerExRecord m_ExRecord;
};

void VerifyValidTransitionFromManagedCode(Thread *pThread, CrawlFrame *pCF);
#endif

#define BOOTUP_EXCEPTION_COMPLUS  0xC0020002

void COMPlusThrowBoot(HRESULT hr);


//==========================================================================
// Used by the classloader to record a managed exception object to explain
// why a classload got botched.
//
// - Can be called with gc enabled or disabled.
//   This allows a catch-all error path to post a generic catchall error
//   message w/out bonking more specific error messages posted by inner functions.
//==========================================================================
VOID DECLSPEC_NORETURN ThrowTypeLoadException(LPCUTF8 pNameSpace, LPCUTF8 pTypeName,
                           LPCWSTR pAssemblyName, LPCUTF8 pMessageArg,
                           UINT resIDWhy);

VOID DECLSPEC_NORETURN ThrowTypeLoadException(LPCWSTR pFullTypeName,
                                              LPCWSTR pAssemblyName,
                                              LPCUTF8 pMessageArg,
                                              UINT resIDWhy);

VOID DECLSPEC_NORETURN ThrowFieldLayoutError(mdTypeDef cl,                // cl of the NStruct being loaded
                           Module* pModule,             // Module that defines the scope, loader and heap (for allocate FieldMarshalers)
                           DWORD   dwOffset,            // Field offset
                           DWORD   dwID);

UINT GetResourceIDForFileLoadExceptionHR(HRESULT hr);

FCDECL1(StringObject*, GetTypeLoadExceptionMessage, UINT32 resId);
FCDECL1(StringObject*, GetFileLoadExceptionMessage, UINT32 hr);
FCDECL1(StringObject*, FileLoadException_GetMessageForHR, UINT32 hresult);
FCDECL1(Object*, MissingMemberException_FormatSignature, I1Array* pPersistedSigUNSAFE);
FCDECL1(Object*, GetResourceFromDefault, StringObject* key);

#define EXCEPTION_NONCONTINUABLE 0x1    // Noncontinuable exception
#define EXCEPTION_UNWINDING 0x2         // Unwind is in progress
#define EXCEPTION_EXIT_UNWIND 0x4       // Exit unwind is in progress
#define EXCEPTION_STACK_INVALID 0x8     // Stack out of limits or unaligned
#define EXCEPTION_NESTED_CALL 0x10      // Nested exception handler call
#define EXCEPTION_TARGET_UNWIND 0x20    // Target unwind in progress
#define EXCEPTION_COLLIDED_UNWIND 0x40  // Collided exception handler call

#define EXCEPTION_UNWIND (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND | \
                          EXCEPTION_TARGET_UNWIND | EXCEPTION_COLLIDED_UNWIND)

#define IS_UNWINDING(Flag) ((Flag & EXCEPTION_UNWIND) != 0)

//#include "CodeMan.h"

class EHRangeTreeNode;
class EHRangeTree;

typedef CUnorderedArray<EHRangeTreeNode *, 7> EH_CLAUSE_UNORDERED_ARRAY;

class EHRangeTreeNode
{
public:
    EHRangeTree                *m_pTree;
    EE_ILEXCEPTION_CLAUSE      *m_clause;

    EHRangeTreeNode            *m_pContainedBy;
    EH_CLAUSE_UNORDERED_ARRAY   m_containees;

private:
    // A node can represent a range or a single offset.
    // A node representing a range can either be the root node, which
    // contains everything and has a NULL m_clause, or it can be
    // a node mapping to an EH clause.
    DWORD                       m_offset;
    bool                        m_fIsRange;
    bool                        m_fIsRoot;

public:
    EHRangeTreeNode(void);
    EHRangeTreeNode(DWORD offset, bool fIsRange = false);
    void CommonCtor(DWORD offset, bool fIsRange);

    bool IsRange();
    void MarkAsRange();

    bool IsRoot();
    void MarkAsRoot(DWORD offset);

    DWORD GetOffset();
    DWORD GetTryStart();
    DWORD GetTryEnd();
    DWORD GetHandlerStart();
    DWORD GetHandlerEnd();
    DWORD GetFilterStart();

    // These four functions may actually be called via FindContainer() while we are building the tree
    // structure, in which case we shouldn't really check the tree recursively because the result is unreliable.
    // Thus, they check m_pTree->m_fInitializing to see if they should call themselves recursively.
    // Also, FindContainer() has extra logic to work around this boot-strapping problem.
    bool Contains(EHRangeTreeNode* pNode);
    bool TryContains(EHRangeTreeNode* pNode);
    bool HandlerContains(EHRangeTreeNode* pNode);
    bool FilterContains(EHRangeTreeNode* pNode);

    // These are simple wrappers around the previous four.
    bool Contains(DWORD offset);
    bool TryContains(DWORD offset);
    bool HandlerContains(DWORD offset);
    bool FilterContains(DWORD offset);

    EHRangeTreeNode* GetContainer();

    HRESULT AddNode(EHRangeTreeNode *pNode);
} ;

class EHRangeTree
{
    unsigned                m_EHCount;
    EHRangeTreeNode        *m_rgNodes;
    EE_ILEXCEPTION_CLAUSE  *m_rgClauses;
    BOOL                    m_isNative; // else it's IL

    // We can get the EH info either from
    // the runtime, in runtime data structures, or from
    // the on-disk image, which we'll examine using the
    // COR_ILMETHOD_DECODERs.  Except for the implicit
    // 'root' node, we'll want to iterate through the rest
    // w/o caring which one it is.
    union TypeFields
    {
        // if which == EHRTT_JIT_MANAGER
        struct _JitManager
        {
            IJitManager     *pIJM;
            METHODTOKEN      methodToken;

            void             *pEnumState; //EH_CLAUSE_ENUMERATOR
            } JitManager;

        // if which == EHRTT_ON_DISK
        struct _OnDisk
        {
            const COR_ILMETHOD_SECT_EH  *sectEH;
        } OnDisk;
    };

    struct EHRT_InternalIterator
    {
        enum Type
        {
            EHRTT_JIT_MANAGER, //from the runtime
            EHRTT_ON_DISK, // we'll be using a COR_ILMETHOD_DECODER
        };

        enum Type which;
        union TypeFields tf;
    };

public:

    EHRangeTreeNode        *m_root; // This is a sentinel, NOT an actual
                                    // Exception Handler!
    HRESULT                 m_hrInit; // Ctor fills this out.

    bool                    m_fInitializing;

    EHRangeTree(COR_ILMETHOD_DECODER *pMethodDecoder);
    EHRangeTree(IJitManager* pIJM,
                METHODTOKEN methodToken,
                DWORD methodSize);
    void CommonCtor(EHRT_InternalIterator *pii,
                          DWORD methodSize);

    ~EHRangeTree();

    EHRangeTreeNode *FindContainer(EHRangeTreeNode *pNodeCur);
    EHRangeTreeNode *FindMostSpecificContainer(DWORD addr);
    EHRangeTreeNode *FindNextMostSpecificContainer(EHRangeTreeNode *pNodeCur,
                                                   DWORD addr);

    BOOL isNative(); // FALSE ==> It's IL

    BOOL isAtStartOfCatch(DWORD offset);
} ;

HRESULT SetIPFromSrcToDst(Thread *pThread,
                          IJitManager* pIJM,
                          METHODTOKEN MethodToken,
                          SLOT addrStart,       // base address of method
                          DWORD offFrom,        // native offset
                          DWORD offTo,          // native offset
                          bool fCanSetIPOnly,   // if true, don't do any real work
                          PREGDISPLAY pReg,
                          PCONTEXT pCtx,
                          DWORD methodSize,
                          void *firstExceptionHandler,
                          void *pDji,
                          EHRangeTree *pEHRT);

BOOL IsInFirstFrameOfHandler(Thread *pThread,
                             IJitManager *pJitManager,
                             METHODTOKEN MethodToken,
                             DWORD offSet);

//==========================================================================
// Handy helper functions
//==========================================================================
LONG FilterAccessViolation(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam);

bool IsInstrModifyFault(PEXCEPTION_POINTERS pExceptionInfo);

bool IsContinuableException(Thread *pThread);

bool IsInterceptableException(Thread *pThread);

#ifdef DEBUGGING_SUPPORTED
bool CheckThreadExceptionStateForInterception();
EXCEPTION_DISPOSITION ClrDebuggerDoUnwindAndIntercept(EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame,
                                                      EXCEPTION_RECORD *pExceptionRecord);
#endif // DEBUGGING_SUPPORTED

#ifdef _X86_
void CPFH_AdjustContextForThreadSuspensionRace(CONTEXT *pContext, Thread *pThread);
void CPFH_AdjustContextForInducedStackOverflow(CONTEXT *pContext, Thread *pThread);
#endif // _X86_

bool IsGcMarker(DWORD exceptionCode, CONTEXT *pContext);

void InitSavedExceptionInfo();

bool ShouldHandleManagedFault(
                        EXCEPTION_RECORD*               pExceptionRecord,
                        CONTEXT*                        pContext,
                        EXCEPTION_REGISTRATION_RECORD*  pEstablisherFrame,
                        Thread*                         pThread);

void HandleManagedFault(EXCEPTION_RECORD*               pExceptionRecord,
                        CONTEXT*                        pContext,
                        EXCEPTION_REGISTRATION_RECORD*  pEstablisherFrame,
                        Thread*                         pThread);

//------------------------------------------------------------------------------
// Error reporting (unhandled exception, fatal error, user breakpoint
class TypeOfReportedError
{
public:
    enum Type {INVALID, UnhandledException, FatalError, UserBreakpoint, NativeThreadUnhandledException, NativeBreakpoint};

    TypeOfReportedError(Type t) : m_type(t) {}

    BOOL IsUnhandledException() { return (m_type == UnhandledException) || (m_type == NativeThreadUnhandledException); }
    BOOL IsFatalError() { return (m_type == FatalError); }
    BOOL IsUserBreakpoint() {return (m_type == UserBreakpoint); }
    BOOL IsBreakpoint() {return (m_type == UserBreakpoint) || (m_type == NativeBreakpoint); }
    BOOL IsException() { return IsUnhandledException() || (m_type == NativeBreakpoint); }

    Type GetType() { return m_type; }
    void SetType(Type t) { m_type = t; }

private:
    Type m_type;
};

LONG WatsonLastChance(
    Thread              *pThread,
    EXCEPTION_POINTERS  *pExceptionInfo,
    TypeOfReportedError tore);

bool DebugIsEECxxException(EXCEPTION_RECORD* pExceptionRecord);


inline void CopyOSContext(CONTEXT* pDest, CONTEXT* pSrc)
{
    SIZE_T cbReadOnlyPost = 0;

    memcpyNoGCRefs(pDest, pSrc, sizeof(CONTEXT) - cbReadOnlyPost);
}

#endif // __excep_h__
