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
// EXCEPTMACROS.H -
//
// This header file exposes mechanisms to:
//
//    1. Throw COM+ exceptions using the COMPlusThrow() function
//    2. Guard a block of code using EX_TRY, and catch
//       COM+ exceptions using EX_CATCH
//
// from the *unmanaged* portions of the EE. Much of the EE runs
// in a hybrid state where it runs like managed code but the code
// is produced by a classic unmanaged-code C++ compiler.
//
// THROWING A COM+ EXCEPTION
// -------------------------
// To throw a COM+ exception, call the function:
//
//      COMPlusThrow(OBJECTREF pThrowable);
//
// This function does not return. There are also various functions
// that wrap COMPlusThrow for convenience.
//
// COMPlusThrow() must only be called within the scope of a EX_TRY
// block. See below for more information.
//
//
// THROWING A RUNTIME EXCEPTION
// ----------------------------
// COMPlusThrow() is overloaded to take a constant describing
// the common EE-generated exceptions, e.g.
//
//    COMPlusThrow(kOutOfMemoryException);
//
// See rexcep.h for list of constants (prepend "k" to get the actual
// constant name.)
//
// You can also add a descriptive error string as follows:
//
//    - Add a descriptive error string and resource id to
//      COM99\src\dlls\mscorrc\resource.h and mscorrc.rc.
//      Embed "%1", "%2" or "%3" to leave room for runtime string
//      inserts.
//
//    - Pass the resource ID and inserts to COMPlusThrow, i.e.
//
//      COMPlusThrow(kSecurityException,
//                   IDS_CANTREFORMATCDRIVEBECAUSE,
//                   L"Formatting C drive permissions not granted.");
//
//
//
// TO CATCH COMPLUS EXCEPTIONS:
// ----------------------------
//
// Use the following syntax:
//
//      #include "exceptmacros.h"
//
//
//      OBJECTREF pThrownObject;
//
//      EX_TRY {
//          ...guarded code...
//      } EX_CATCH {
//          ...handler...
//      } EX_END_CATCH(SwallowAllExceptions)
//
//
// EX_TRY blocks can be nested.
//
// From within the handler, you can call the GETTHROWABLE() macro to
// obtain the object that was thrown.
//
// CRUCIAL POINTS
// --------------
// In order to call COMPlusThrow(), you *must* be within the scope
// of a EX_TRY block. Under _DEBUG, COMPlusThrow() will assert
// if you call it out of scope. This implies that just about every
// external entrypoint into the EE has to have a EX_TRY, in order
// to convert uncaught COM+ exceptions into some error mechanism
// more understandable to its non-COM+ caller.
//
// Any function that can throw a COM+ exception out to its caller
// has the same requirement. ALL such functions should be tagged
// with THROWS in CONTRACT. Aside from making the code
// self-document its contract, the checked version of this will fire
// an assert if the function is ever called without being in scope.
//
//
// AVOIDING EX_TRY GOTCHAS
// ----------------------------
// EX_TRY/EX_CATCH actually expands into a Win32 SEH
// __try/__except structure. It does a lot of goo under the covers
// to deal with pre-emptive GC settings.
//
//    1. Do not use C++ or SEH try/__try use EX_TRY instead.
//
//    2. Remember that any function marked THROWS
//       has the potential not to return. So be wary of allocating
//       non-gc'd objects around such calls because ensuring cleanup
//       of these things is not simple (you can wrap another EX_TRY
//       around the call to simulate a COM+ "try-finally" but EX_TRY
//       is relatively expensive compared to the real thing.)
//
//

#ifndef __exceptmacros_h__
#define __exceptmacros_h__

struct _EXCEPTION_REGISTRATION_RECORD;
class Thread;
class Frame;
class Exception;
class ZapMonitor;

VOID DECLSPEC_NORETURN RealCOMPlusThrowOM();
VOID DECLSPEC_NORETURN RealCOMPlusThrowSO();

#include <excepcpu.h>
#include "stackprobe.h"



//==========================================================================
// Macros to allow catching exceptions from within the EE. These are lightweight
// handlers that do not install the managed frame handler.
//
//      EE_TRY_FOR_FINALLY {
//          ...<guarded code>...
//      } EE_FINALLY {
//          ...<handler>...
//      } EE_END_FINALLY
//
//      EE_TRY(filter expr) {
//          ...<guarded code>...
//      } EE_CATCH {
//          ...<handler>...
//      }
//==========================================================================

// __GotException will only be FALSE if got all the way through the code
// guarded by the try, otherwise it will be TRUE, so we know if we got into the
// finally from an exception or not. In which case need to reset the GC state back
// to what it was for the finally to run in that state.

#define EE_TRY_FOR_FINALLY                                          \
    {                                                               \
        BOOL __fGCDisabled = GetThread()->PreemptiveGCDisabled();   \
        BOOL __GotException = TRUE;                                 \
        PAL_TRY {

#define GOT_EXCEPTION() __GotException

#define EE_FINALLY                                                      \
        __GotException = FALSE;                                         \
    } PAL_FINALLY {                                                     \
        if (__GotException) {                                           \
            if (__fGCDisabled != GetThread()->PreemptiveGCDisabled())   \
                if (__fGCDisabled)                                      \
                    GetThread()->DisablePreemptiveGC();                 \
                else                                                    \
                    GetThread()->EnablePreemptiveGC();                  \
        }

#define EE_END_FINALLY                                                  \
        }                                                               \
        PAL_ENDTRY                                                      \
    }




#define GETTHROWABLE()              GET_THROWABLE()


//==========================================================================
// Helpful macros to declare exception handlers, their implementaiton,
// and to call them.
//==========================================================================

#define _EXCEPTION_HANDLER_DECL(funcname)                                                               \
    EXCEPTION_DISPOSITION __cdecl _##funcname(EXCEPTION_RECORD *pExceptionRecord,                       \
                                              struct _EXCEPTION_REGISTRATION_RECORD *pEstablisherFrame, \
                                              CONTEXT *pContext,                                        \
                                              DISPATCHER_CONTEXT *pDispatcherContext)

#define EXCEPTION_HANDLER_DECL(funcname)                                \
    LONG funcname(EXCEPTION_POINTERS *pExceptionPointers, PVOID pv);    \
    _EXCEPTION_HANDLER_DECL(funcname)

#define EXCEPTION_HANDLER_IMPL(funcname)                                    \
    _EXCEPTION_HANDLER_DECL(funcname);                                      \
                                                                            \
    LONG funcname(EXCEPTION_POINTERS *pExceptionPointers,                   \
                  PVOID pv)                                                 \
        { WRAPPER_CONTRACT;                                                 \
          return _##funcname(pExceptionPointers->ExceptionRecord,           \
                             (struct _EXCEPTION_REGISTRATION_RECORD*)pv,    \
                             pExceptionPointers->ContextRecord,             \
                             NULL); }                                       \
                                                                            \
    _EXCEPTION_HANDLER_DECL(funcname)

#define EXCEPTION_HANDLER_FWD(funcname) \
    _##funcname(pExceptionRecord, pEstablisherFrame, pContext, pDispatcherContext)


#if defined(PAL_PORTABLE_SEH) && defined(__GNUC__)

#if defined(PLATFORM_UNIX)

extern "C" void SEHUnwind(LPCONTEXT contextRecord);
#define UNWIND_CALL(er) SEHUnwind(NULL)

#else

extern "C" PALAPI void DoUnwind(DWORD ExceptionCode, PEXCEPTION_RECORD er);
#define UNWIND_CALL(er) DoUnwind(((PEXCEPTION_RECORD)er)->ExceptionCode, (PEXCEPTION_RECORD)er)

#endif

#define INSTALL_PAL_LONGJMP_TARGET {                    \
    PAL_EXCEPTION_REGISTRATION __unwindRecord;          \
    __unwindRecord.typeOfHandler = PALExcept;           \
    __unwindRecord.Handler = EXCEPTION_CONTINUE_SEARCH; \
    __unwindRecord.pvFilterParameter =                  \
        GetThread()->GetFrame();                        \
    __unwindRecord.dwFlags = PAL_EXCEPTION_FLAGS_LONGJMP;\
    PAL_TryHelper(&__unwindRecord);                     \
    int __retv = PAL_setjmp(__unwindRecord.ReservedForPAL);\
    if (__retv == 0) { DEBUG_ASSURE_NO_RETURN_BEGIN

#define UNINSTALL_PAL_LONGJMP_TARGET                    \
        DEBUG_ASSURE_NO_RETURN_END                      \
        PAL_EndTryHelper(&__unwindRecord, 0);           \
    } else {                                            \
        UNWIND_CALL(__retv);                            \
        UNREACHABLE();                                  \
    } }

#else // PAL_PORTABLE_SEH && PLATFORM_UNIX

#define INSTALL_PAL_LONGJMP_TARGET { DEBUG_ASSURE_NO_RETURN_BEGIN
#define UNINSTALL_PAL_LONGJMP_TARGET DEBUG_ASSURE_NO_RETURN_END }

#endif // PAL_PORTABLE_SEH && PLATFORM_UNIX

//==========================================================================
// Declares a COM+ frame handler that can be used to make sure that
// exceptions that should be handled from within managed code
// are handled within and don't leak out to give other handlers a
// chance at them.
//==========================================================================
#define INSTALL_COMPLUS_EXCEPTION_HANDLER()                                     \
    INSTALL_COMPLUS_EXCEPTION_HANDLEREX((GetThread()))                          

#define INSTALL_COMPLUS_EXCEPTION_HANDLEREX(pCurThread)                         \
{                                                                               \
    DECLARE_CPFH_EH_RECORD(pCurThread);                                         \
    INTERNAL_INSTALL_COMPLUS_EXCEPTION_HANDLEREX_HELPER()                  

#define INSTALL_COMPLUS_EXCEPTION_HANDLEREX_NO_DECLARE()                        \
{                                                                               \
    INTERNAL_INSTALL_COMPLUS_EXCEPTION_HANDLEREX_HELPER()                       

#define INTERNAL_INSTALL_COMPLUS_EXCEPTION_HANDLEREX_HELPER()                   \
    INSTALL_PAL_LONGJMP_TARGET;                                                 \
    INSTALL_EXCEPTION_HANDLING_RECORD(&(___pExRecord->m_ExReg));                \
    /* work around unreachable code warning */                                  \
    if (true) {                                                                 

#define UNINSTALL_COMPLUS_EXCEPTION_HANDLER()                                   \
    }                                                                           \
    UNINSTALL_EXCEPTION_HANDLING_RECORD(&(___pExRecord->m_ExReg));              \
    UNINSTALL_PAL_LONGJMP_TARGET;                                               \
}                                                                               

#ifdef _X86_

#define INSTALL_NESTED_EXCEPTION_HANDLER(frame)                                                                       \
   NestedHandlerExRecord *__pNestedHandlerExRecord = (NestedHandlerExRecord*) _alloca(sizeof(NestedHandlerExRecord)); \
   __pNestedHandlerExRecord->m_handlerInfo.m_hThrowable = NULL;                                                       \
   __pNestedHandlerExRecord->Init(0, (PEXCEPTION_ROUTINE)COMPlusNestedExceptionHandler, frame);                       \
   INSTALL_EXCEPTION_HANDLING_RECORD(&(__pNestedHandlerExRecord->m_ExReg));

#define UNINSTALL_NESTED_EXCEPTION_HANDLER()                                                                          \
   UNINSTALL_EXCEPTION_HANDLING_RECORD(&(__pNestedHandlerExRecord->m_ExReg));

#else // _X86_

#define INSTALL_NESTED_EXCEPTION_HANDLER(frame)
#define UNINSTALL_NESTED_EXCEPTION_HANDLER()

#endif // _x86_

#ifdef _X86_
EXCEPTION_HANDLER_DECL(CLRVectoredExceptionHandlerShimX86);

#if defined(FEATURE_PAL) && defined(__GNUC__)
#define ALLOC_EXCEPTION_REGISTRATION_RECORD __state.m_RecordBytes.AllocThrows(sizeof(EXCEPTION_REGISTRATION_RECORD))
#else
#define ALLOC_EXCEPTION_REGISTRATION_RECORD _alloca(sizeof(EXCEPTION_REGISTRATION_RECORD))
#endif

#define INSTALL_DOWNLEVEL_VECTORED_HANDLER(__state)                                                              \
    __state.m_pDownlevelVectoredHandlerRecord =                                                                  \
        (PEXCEPTION_REGISTRATION_RECORD) ALLOC_EXCEPTION_REGISTRATION_RECORD;                                    \
    __state.m_pDownlevelVectoredHandlerRecord->Handler = (PEXCEPTION_ROUTINE)CLRVectoredExceptionHandlerShimX86; \
    INSTALL_EXCEPTION_HANDLING_RECORD(__state.m_pDownlevelVectoredHandlerRecord);                                \

#else // _X86_

#define INSTALL_DOWNLEVEL_VECTORED_HANDLER(__state)

#endif // _X86_


LONG WINAPI CLRVectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo);
LONG WINAPI CLRVectoredContinueHandler(PEXCEPTION_POINTERS pExceptionInfo);
BOOL COMPlusIsMonitorException(struct _EXCEPTION_POINTERS *pExceptionInfo);

inline LONG CheckException(EXCEPTION_POINTERS* pExceptionPointers, PVOID pv)
{
    WRAPPER_CONTRACT;

    LONG result = CLRVectoredExceptionHandler(pExceptionPointers);
    if (result != EXCEPTION_EXECUTE_HANDLER)
        return result;

#ifdef _DEBUG_IMPL
    _ASSERTE(!"Unexpected Exception");
#else
    FreeBuildDebugBreak();
#endif

    // Set the debugger to break on AV and return a value of EXCEPTION_CONTINUE_EXECUTION (-1)
    // here and you will bounce back to the point of the AV.
    return EXCEPTION_EXECUTE_HANDLER;

}

//==========================================================================
// Installs a handler to unwind exception frames, but not catch the exception
//==========================================================================
VOID DECLSPEC_NORETURN RaiseTheException(OBJECTREF throwable, BOOL rethrow);
VOID DECLSPEC_NORETURN RaiseTheExceptionInternalOnly(OBJECTREF throwable, BOOL rethrow, BOOL fForStackOverflow = FALSE);

#ifdef DACCESS_COMPILE

#define INSTALL_UNWIND_AND_CONTINUE_HANDLER
#define UNINSTALL_UNWIND_AND_CONTINUE_HANDLER

#define INSTALL_UNWIND_AND_CONTINUE_HANDLER_FOR_INTEROP
#define UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_FOR_INTEROP


#else // DACCESS_COMPILE

void UnwindAndContinueRethrowHelperInsideCatch(Frame* pEntryFrame, Exception* pException);
VOID DECLSPEC_NORETURN UnwindAndContinueRethrowHelperAfterCatch(Frame* pEntryFrame, Exception* pException);

// This is to allow other code to access the thread object obtained in IUACH
#define IUACH_THREAD_OBJECT __pUnCThread

#define INSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND_NO_PROBE(pThread)           \
    {                                                                                       \
        Exception* __pUnCException  = NULL;                                                 \
        Thread*    IUACH_THREAD_OBJECT = pThread;                                           \
        Frame*     __pUnCEntryFrame = __pUnCThread->GetFrame();                             \
        bool       __fExceptionCatched = false;                                             \
                                                                                            \
        /* work around unreachable code warning */                                          \
        if (true) PAL_CPP_TRY {                                                             \
            DEBUG_ASSURE_NO_RETURN_BEGIN                                                    \

#define INSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PROBE(pThread)                               \
    PAL_CPP_EHUNWIND_BEGIN;                                                                 \
    INSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND_NO_PROBE(pThread);              \

#define INSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND                             \
    INSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND_NO_PROBE(GetThread());          \
    /* The purpose of the INSTALL_UNWIND_AND_CONTINUE_HANDLER is to translate an exception to a managed */ \
    /* exception before it hits managed code.  The transition to SO_INTOLERANT code does not logically belong here. */ \
    /* However, we don't want to miss any probe points and the intersection between a probe point and installing */ \
    /* an  INSTALL_UNWIND_AND_CONTINUE_HANDLER is very high.  The probes are very cheap, so we can tolerate */ \
    /* those few places where we are probing and don't need to. */ \
    /* Ideally, we would instead have an encompassing ENTER_SO_INTOLERANT_CODE macro that would */ \
    /* include INSTALL_UNWIND_AND_CONTINUE_HANDLER */                                       \
    BEGIN_SO_INTOLERANT_CODE(__pUnCThread);                                                 \

#define INSTALL_UNWIND_AND_CONTINUE_HANDLER                                                 \
    PAL_CPP_EHUNWIND_BEGIN;                                                                 \
    INSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND;                                \


#define UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND_NO_PROBE                  \
            DEBUG_ASSURE_NO_RETURN_END                                                      \
        }                                                                                   \
        PAL_CPP_CATCH_EXCEPTION (__pException)                                              \
        {                                                                                   \
            CONSISTENCY_CHECK(NULL != __pException);                                        \
            __pUnCException = __pException;                                                 \
            UnwindAndContinueRethrowHelperInsideCatch(__pUnCEntryFrame, __pUnCException);   \
            __fExceptionCatched = true;                                                     \
        }                                                                                   \
        PAL_CPP_ENDTRY                                                                      \
        if (__fExceptionCatched)                                                            \
        {                                                                                   \
            UnwindAndContinueRethrowHelperAfterCatch(__pUnCEntryFrame, __pUnCException);    \
        }                                                                                   \
    }                                                                                       \

#define UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PROBE                                      \
    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND_NO_PROBE;                     \
    PAL_CPP_EHUNWIND_END;                                                                   \

#define UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND                           \
    END_SO_INTOLERANT_CODE;                                                                 \
    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND_NO_PROBE;                     \

#define UNINSTALL_UNWIND_AND_CONTINUE_HANDLER                                               \
    UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_NO_PAL_CPP_EHUNWIND;                              \
    PAL_CPP_EHUNWIND_END;                                                                   \


//
//
//
#define INSTALL_UNWIND_AND_CONTINUE_HANDLER_FOR_INTEROP                                     \
    {                                                                                       \
        PAL_CPP_EHUNWIND_BEGIN;                                                             \
        Exception   *__pUnCException = NULL;                                                \
        SIZE_T      __ESP = (SIZE_T)GetSP();                                                \
        INDEBUG(Frame *__pUnCEntryFrame = GetThread()->GetFrame());                         \
                                                                                            \
        /* work around unreachable code warning */                                          \
        if (true) PAL_CPP_TRY {                                                             \
            DEBUG_ASSURE_NO_RETURN_BEGIN                                                    \


#define UNINSTALL_UNWIND_AND_CONTINUE_HANDLER_FOR_INTEROP                                   \
            DEBUG_ASSURE_NO_RETURN_END                                                      \
        }                                                                                   \
        PAL_CPP_CATCH_EXCEPTION (__pException)                                              \
        {                                                                                   \
            CONSISTENCY_CHECK(NULL != __pException);                                        \
            __pUnCException = __pException;                                                 \
            _ASSERTE(__pUnCEntryFrame == GetThread()->FindFrame(__ESP, NULL));              \
            UnwindAndContinueRethrowHelperInsideCatch(GetThread()->FindFrame(__ESP, NULL), __pUnCException);   \
        }                                                                                   \
        PAL_CPP_ENDTRY                                                                      \
        if (__pUnCException != NULL)                                                        \
        {                                                                                   \
            UnwindAndContinueRethrowHelperAfterCatch(GetThread()->FindFrame(__ESP, NULL), __pUnCException);    \
        }                                                                                   \
        PAL_CPP_EHUNWIND_END;                                                               \
    }                                                                                       \

#endif // DACCESS_COMPILE


#define ENCLOSE_IN_EXCEPTION_HANDLER( func ) \
    { \
        struct exception_handler_wrapper \
        { \
            static void wrap() \
            { \
                INSTALL_UNWIND_AND_CONTINUE_HANDLER; \
                func(); \
                UNINSTALL_UNWIND_AND_CONTINUE_HANDLER; \
            } \
        }; \
    \
        exception_handler_wrapper::wrap(); \
    }


//==========================================================================
// Declares that a function can throw a COM+ exception.
//==========================================================================

#define CANNOTTHROWCOMPLUSEXCEPTION() ANNOTATION_NOTHROW
#define MAYBE_CANNOTTHROWCOMPLUSEXCEPTION(fCond)
#define BEGINCANNOTTHROWCOMPLUSEXCEPTION_SEH() ANNOTATION_NOTHROW
#define ENDCANNOTTHROWCOMPLUSEXCEPTION_SEH()

#define COMPlusThrow                        RealCOMPlusThrow
#define COMPlusThrowNonLocalized            RealCOMPlusThrowNonLocalized
#define COMPlusThrowHR                      RealCOMPlusThrowHR
#define COMPlusThrowWin32                   RealCOMPlusThrowWin32
#define COMPlusThrowOM                      RealCOMPlusThrowOM
#define COMPlusThrowSO                      RealCOMPlusThrowSO
#define COMPlusThrowArithmetic              RealCOMPlusThrowArithmetic
#define COMPlusThrowArgumentNull            RealCOMPlusThrowArgumentNull
#define COMPlusThrowArgumentOutOfRange      RealCOMPlusThrowArgumentOutOfRange
#define COMPlusThrowArgumentException       RealCOMPlusThrowArgumentException
#define COMPlusThrowInvalidCastException    RealCOMPlusThrowInvalidCastException


/* Non-VM exception helpers to be rerouted inside the VM directory:
ThrowHR
ThrowWin32
ThrowLastError       -->ThrowWin32(GetLastError())
ThrowOutOfMemory        COMPlusThrowOM defers to this
ThrowStackOverflow      COMPlusThrowSO defers to this
ThrowMessage            ThrowHR(E_FAIL, Message)

*/

/* Ideally we could make these defines.  But the sources in the VM directory
   won't build with them as defines.  @todo: go through VM directory and
   eliminate calls to the non-VM style functions.

#define ThrowHR             COMPlusThrowHR
#define ThrowWin32          COMPlusThrowWin32
#define ThrowLastError()    COMPlusThrowWin32(GetLastError())
#define ThrowMessage        "Don't use this in the VM directory"

*/

//======================================================
// Used when we're entering the EE from unmanaged code
// and we can assert that the gc state is cooperative.
//
// If an exception is thrown through this transition
// handler, it will clean up the EE appropriately.  See
// the definition of COMPlusCooperativeTransitionHandler
// for the details.
//======================================================

void COMPlusCooperativeTransitionHandler(Frame* pFrame);

#define COOPERATIVE_TRANSITION_BEGIN() COOPERATIVE_TRANSITION_BEGIN_EX(GetThread())

#define COOPERATIVE_TRANSITION_BEGIN_EX(pThread)    \
  {                                                 \
    _ASSERTE(GetThread() && GetThread()->PreemptiveGCDisabled() == TRUE); \
    BEGIN_SO_INTOLERANT_CODE(pThread); \
    CoopTransitionHolder __CoopTransition(pThread->m_pFrame);             \
    DEBUG_ASSURE_NO_RETURN_BEGIN

#define COOPERATIVE_TRANSITION_END()        \
    DEBUG_ASSURE_NO_RETURN_END              \
    __CoopTransition.SuppressRelease();     \
    END_SO_INTOLERANT_CODE; \
  }

extern LONG UserBreakpointFilter(EXCEPTION_POINTERS *ep);
extern LONG DefaultCatchFilter(EXCEPTION_POINTERS *ep, LPVOID pv);
extern LONG DefaultCatchNoSwallowFilter(EXCEPTION_POINTERS *ep, LPVOID pv);


// the only valid parameter for DefaultCatchFilter
#define COMPLUS_EXCEPTION_EXECUTE_HANDLER   (PVOID)EXCEPTION_EXECUTE_HANDLER



VOID ThrowBadFormatWorker(__in_z char *cond, UINT resID, const SString & imageName);

template <typename T>
VOID GetPathForErrorMessagesT(T *pImgObj, SString &result)
{
    if (pImgObj)
    {
        pImgObj->GetPathForErrorMessages(result);
    }
    else
    {
        result = L"";
    }
}


// Worker macro for throwing BadImageFormat exceptions.
//
//     resID:     resource ID in mscorrc.rc. Message may not have substitutions. resID is permitted (but not encouraged) to be 0.
//     imgObj:    one of Module* or PEFile* or PEImage* (must support GetPathForErrorMessages method.)
//
#define IfFailThrowBF(hresult, resID, imgObj)   \
    do                                          \
        {                                       \
            if (FAILED(hresult))                \
                THROW_BAD_FORMAT(resID, imgObj); \
        }                                       \
    while(0)


#define THROW_BAD_FORMAT(resID, imgObj) THROW_BAD_FORMAT_MAYBE(FALSE, resID, imgObj)


// Conditional version of THROW_BAD_FORMAT. Do not use for new callsites. This is really meant to be a drop-in replacement
// for the obsolete BAD_FORMAT_ASSERT.

#define THROW_BAD_FORMAT_MAYBE(cond, resID, imgObj)             \
    do                                                          \
        {                                                       \
            if (!(cond))                                        \
            {                                                   \
                SString _tmpStr;                                \
                GetPathForErrorMessagesT((imgObj), _tmpStr);    \
                ThrowBadFormatWorker(#cond, (resID), _tmpStr);  \
            }                                                   \
        }                                                       \
   while(0)


// Same as above, but allows you to specify your own HRESULT
#define THROW_HR_ERROR_WITH_INFO(hr, imgObj) \
    do                                                          \
        {                                                       \
            SString _tmpStr;                                \
            GetPathForErrorMessagesT((imgObj), _tmpStr);    \
            RealCOMPlusThrowHR(hr, hr, _tmpStr.GetUnicode(), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);  \
        }                                                       \
   while(0)



#endif // __exceptmacros_h__
