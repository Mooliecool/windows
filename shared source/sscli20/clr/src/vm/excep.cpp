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
/*  EXCEP.CPP:
 *
 */

#include "common.h"

#include "frames.h"
#include "threads.h"
#include "excep.h"
#include "object.h"
#include "comstring.h"
#include "field.h"
#include "dbginterface.h"
#include "cgensys.h"
#include "gcscan.h"
#include "comutilnative.h"
#include "comsystem.h"
#include "siginfo.hpp"
#include "gc.h"
#include "eedbginterfaceimpl.h" //so we can clearexception in RealCOMPlusThrow
#include "perfcounters.h"
#include "dllimportcallback.h"
#include "stackwalk.h" //for CrawlFrame, in SetIPFromSrcToDst
#include "shimload.h"
#include "eeconfig.h"
#include "virtualcallstub.h"




// Disable the "initialization of static local vars is no thread safe" error
#ifdef _MSC_VER
#pragma warning(disable : 4640)
#endif


#ifndef DACCESS_COMPILE


#define SZ_UNHANDLED_EXCEPTION L"Unhandled Exception:"
#define SZ_UNHANDLED_EXCEPTION_CHARLEN ((sizeof(SZ_UNHANDLED_EXCEPTION) / sizeof(WCHAR)))


typedef struct {
    OBJECTREF pThrowable;
    STRINGREF s1;
    OBJECTREF pTmpThrowable;
} ProtectArgsStruct;

PEXCEPTION_REGISTRATION_RECORD GetCurrentSEHRecord();
BOOL IsUnmanagedToManagedSEHHandler(EXCEPTION_REGISTRATION_RECORD*);
VOID DECLSPEC_NORETURN RealCOMPlusThrow(OBJECTREF throwable, BOOL rethrow);
VOID DECLSPEC_NORETURN RealCOMPlusThrow(OBJECTREF throwable);
VOID DECLSPEC_NORETURN RealCOMPlusThrow(RuntimeExceptionKind reKind);


BOOL NotifyAppDomainsOfUnhandledException(
    PEXCEPTION_POINTERS pExceptionPointers,
    OBJECTREF   *pThrowableIn,
    BOOL        useLastThrownObject,
    BOOL        isTerminating);


void COMPlusThrowBoot(HRESULT hr)
{
    STATIC_CONTRACT_THROWS;

    _ASSERTE(g_fEEShutDown >= ShutDown_Finalize2 || !"This should not be called unless we are in the last phase of shutdown!");
    ULONG_PTR arg = hr;
    RaiseException(BOOTUP_EXCEPTION_COMPLUS, EXCEPTION_NONCONTINUABLE, 1, &arg);
}


//-------------------------------------------------------------------------------
// This simply tests to see if the exception object is a subclass of
// the descriminating class specified in the exception clause.
//-------------------------------------------------------------------------------
BOOL ExceptionIsOfRightType(TypeHandle clauseType, TypeHandle thrownType)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    // if not resolved to, then it wasn't loaded and couldn't have been thrown
    if (clauseType.IsNull())
        return FALSE;

    if (clauseType == thrownType)
        return TRUE;

    // now look for parent match
    TypeHandle superType = thrownType;
    while (!superType.IsNull()) {
        if (superType == clauseType) {
            break;
        }
        superType = superType.GetParent();
    }

    return !superType.IsNull();
}

//===========================================================================
// Gets the message text from an exception
//===========================================================================
ULONG GetExceptionMessage(OBJECTREF throwable,
                          __inout_ecount(bufferLength) LPWSTR buffer,
                          ULONG bufferLength)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

    // Prefast buffer sanity check.  Don't call the API with a zero length buffer.
    if (bufferLength == 0)
    {
        _ASSERTE(bufferLength > 0);
        return 0;
    }

    StackSString result;
    GetExceptionMessage(throwable, result);

    ULONG length = result.GetCount();
    LPCWSTR chars = result.GetUnicode();

    if (length < bufferLength)
    {
        wcsncpy_s(buffer, bufferLength, chars, length);
    }
    else
    {
        wcsncpy_s(buffer, bufferLength, chars, bufferLength-1);
    }

    return length;
}

//-----------------------------------------------------------------------------
// Given an object, get the "message" from it.  If the object is an Exception
//  call Exception.InternalToString, otherwise, call Object.ToString
//-----------------------------------------------------------------------------
void GetExceptionMessage(OBJECTREF throwable, SString &result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

    // If there is no object, there is no message.
    if (throwable == NULL)
        return;

    // Assume we're calling Exception.InternalToString() ...
    BinderMethodID sigID = METHOD__EXCEPTION__INTERNAL_TO_STRING;

    // ... but if it isn't an exception, call Object.ToString().
    _ASSERTE(IsException(throwable->GetMethodTable()));        // what is the pathway here?
    if (!IsException(throwable->GetMethodTable()))
    {
        sigID = METHOD__OBJECT__TO_STRING;
    }

    // Return value.
    STRINGREF pString = NULL;

    GCPROTECT_BEGIN(throwable);

    // Get the MethodDesc on which we'll call.
    MethodDescCallSite toString(sigID, &throwable);

    // Make the call.
    ARG_SLOT arg[1] = {ObjToArgSlot(throwable)};
    pString = toString.Call_RetSTRINGREF(arg);

    GCPROTECT_END();

    // If call returned NULL (not empty), oh well, no message.
    if (pString != NULL)
        pString->GetSString(result);
} // void GetExceptionMessage()

HRESULT GetExceptionHResult(OBJECTREF throwable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(throwable != NULL);
    }
    CONTRACTL_END;

    HRESULT hr = E_FAIL;

    GCPROTECT_BEGIN(throwable)
    {
        // Since any object can be thrown in managed code, not only instances of System.Exception subclasses
        // we need to check to see if we are dealing with an exception before attempting to retrieve
        // the HRESULT field. If we are not dealing with an exception, then we will simply return E_FAIL.
        _ASSERTE(IsException(throwable->GetMethodTable()));        // what is the pathway here?
        if (IsException(throwable->GetMethodTable()))
        {
            hr = ((EXCEPTIONREF)throwable)->GetHResult();
        }
    }
    GCPROTECT_END();

    return hr;
} // HRESULT GetExceptionHResult()

DWORD GetExceptionXCode(OBJECTREF throwable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(throwable != NULL);
    }
    CONTRACTL_END;

    HRESULT hr = E_FAIL;

    GCPROTECT_BEGIN(throwable)
    {
        // Since any object can be thrown in managed code, not only instances of System.Exception subclasses
        // we need to check to see if we are dealing with an exception before attempting to retrieve
        // the HRESULT field. If we are not dealing with an exception, then we will simply return E_FAIL.
        _ASSERTE(IsException(throwable->GetMethodTable()));        // what is the pathway here?
        if (IsException(throwable->GetMethodTable()))
        {
            hr = ((EXCEPTIONREF)throwable)->GetXCode();
        }
    }
    GCPROTECT_END();

    return hr;
} // DWORD GetExceptionXCode()

//------------------------------------------------------------------------------
// This function will extract some information from an Access Violation SEH
//  exception, and store it in the System.AccessViolationException object.
// - the faulting instruction's IP.
// - the target address of the faulting instruction.
// - a code indicating attempted read vs write
//------------------------------------------------------------------------------
void SetExceptionAVParameters(              // No return.
    OBJECTREF throwable,                    // The object into which to set the values.
    EXCEPTION_RECORD *pExceptionRecord)     // The SEH exception information.
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(throwable != NULL);
    }
    CONTRACTL_END;

    GCPROTECT_BEGIN(throwable)
    {
        // This should only be called for AccessViolationException
        _ASSERTE(g_Mscorlib.GetException(kAccessViolationException) == throwable->GetMethodTable());

        FieldDesc *pFD_ip = g_Mscorlib.GetField(FIELD__ACCESS_VIOLATION_EXCEPTION__IP);
        FieldDesc *pFD_target = g_Mscorlib.GetField(FIELD__ACCESS_VIOLATION_EXCEPTION__TARGET);
        FieldDesc *pFD_access = g_Mscorlib.GetField(FIELD__ACCESS_VIOLATION_EXCEPTION__ACCESSTYPE);

        _ASSERTE(pFD_ip->GetFieldType() == ELEMENT_TYPE_I);
        _ASSERTE(pFD_target->GetFieldType() == ELEMENT_TYPE_I);
        _ASSERTE(pFD_access->GetFieldType() == ELEMENT_TYPE_I4);
        
        void *ip     = pExceptionRecord->ExceptionAddress;
        void *target = (void*)(pExceptionRecord->ExceptionInformation[1]);
        DWORD access = (DWORD)(pExceptionRecord->ExceptionInformation[0]);

        pFD_ip->SetValuePtr(throwable, ip);
        pFD_target->SetValuePtr(throwable, target);
        pFD_access->SetValue32(throwable, access);

    }
    GCPROTECT_END();
    
} // void SetExceptionAVParameters()

//------------------------------------------------------------------------------
// This will call InternalPreserveStackTrace (if the throwable derives from
//  System.Exception), to copy the stack trace to the _remoteStackTraceString.
// Doing so allows the stack trace of an exception caught by the runtime, and
//  rethrown with COMPlusThrow(OBJECTREF thowable), to be preserved.  Otherwise
//  the exception handling code may clear the stack trace.  (Generally, we see
//  the stack trace preserved on win32 and cleared on win64.)
//------------------------------------------------------------------------------
void ExceptionPreserveStackTrace(   // No return.
    OBJECTREF throwable)            // Object about to be thrown.
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory());
    }
    CONTRACTL_END;

    // If there is no object, there is no stack trace to save.
    if (throwable == NULL)
        return;

    GCPROTECT_BEGIN(throwable);

    // Make sure it is derived from System.Exception, that it is not one of the
    //  preallocated exception objects, and that it has a stack trace to save.
    if (IsException(throwable->GetMethodTable()) &&
        !CLRException::IsPreallocatedExceptionObject(throwable))
    {
        LOG((LF_EH, LL_INFO1000, "ExceptionPreserveStackTrace called\n"));

        // We're calling Exception.InternalPreserveStackTrace() ...
        BinderMethodID sigID = METHOD__EXCEPTION__INTERNAL_PRESERVE_STACK_TRACE;


        // Get the MethodDesc on which we'll call.
        MethodDescCallSite preserveStackTrace(sigID, &throwable);

        // Make the call.
        ARG_SLOT arg[1] = {ObjToArgSlot(throwable)};
        preserveStackTrace.Call(arg);
    }

    GCPROTECT_END();

} // void ExceptionPreserveStackTrace()

BOOL IsException(MethodTable *pMT) {
    STATIC_CONTRACT_WRAPPER;
    return pMT->CanCastToClass(g_pExceptionClass);
} // BOOL IsException()


// We have to cache the MethodTable and FieldDesc for wrapped non-compliant exceptions the first
// time we wrap, because we cannot tolerate a GC when it comes time to detect and unwrap one.

static MethodTable *pMT_RuntimeWrappedException;
static FieldDesc   *pFD_WrappedException;


// Non-compliant exceptions are immediately wrapped in a RuntimeWrappedException instance.  The entire
// exception system can now ignore the possibility of these cases except:
//
// 1) JIT_Throw, which must wrap via this API
// 2) Calls to Filters & Catch handlers, which must unwrap based on whether the assembly is on the legacy
//    plan.
//
void WrapNonCompliantException(OBJECTREF *ppThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(IsProtectedByGCFrame(ppThrowable));
    }
    CONTRACTL_END;

    _ASSERTE(!IsException((*ppThrowable)->GetMethodTable()));

    EX_TRY
    {
        // idempotent operations, so the race condition is okay.
        if (pMT_RuntimeWrappedException == NULL)
            pMT_RuntimeWrappedException = g_Mscorlib.GetException(kRuntimeWrappedException);

        if (pFD_WrappedException == NULL)
            pFD_WrappedException = g_Mscorlib.GetField(FIELD__RUNTIME_WRAPPED_EXCEPTION__WRAPPED_EXCEPTION);

        OBJECTREF orWrapper = AllocateObject(g_Mscorlib.GetException(kRuntimeWrappedException));

        GCPROTECT_BEGIN(orWrapper);

        MethodDescCallSite ctor(METHOD__RUNTIME_WRAPPED_EXCEPTION__OBJ_CTOR, &orWrapper);

        ARG_SLOT args[] =
        {
            ObjToArgSlot(orWrapper),
            ObjToArgSlot(*ppThrowable)
        };

        ctor.Call(args);

        *ppThrowable = orWrapper;

        GCPROTECT_END();
    }
    EX_CATCH
    {
        // If we took an exception while binding, or running the constructor of the RuntimeWrappedException
        // instance, we know that this new exception is CLS compliant.  In fact, it's likely to be
        // OutOfMemoryException, StackOverflowException or ThreadAbortException.
        OBJECTREF orReplacement = GET_THROWABLE();

        _ASSERTE(IsException(orReplacement->GetMethodTable()));

        *ppThrowable = orReplacement;

    } EX_END_CATCH(SwallowAllExceptions);
}


// Before presenting an exception object to a handler (filter or catch, not finally or fault), it
// may be necessary to turn it back into a non-compliant exception.  This is conditioned on an
// assembly level setting.
OBJECTREF PossiblyUnwrapThrowable(OBJECTREF throwable, Assembly *pAssembly)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pAssembly));
    }
    CONTRACTL_END;

    if ((throwable != NULL) &&
        (throwable->GetMethodTable() == pMT_RuntimeWrappedException) &&
        (!pAssembly->IsRuntimeWrapExceptions()))
    {
        // We already created the instance, fetched the field.  We know it is
        // not marshal by ref, or any of the other cases that might trigger GC.
        ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

        throwable = pFD_WrappedException->GetRefValue(throwable);
    }

    return throwable;
}


// This is used by a holder in CreateTypeInitializationExceptionObject to
// reset the state as appropriate.
void ResetTypeInitializationExceptionState(BOOL isAlreadyCreating)
{
    if (!isAlreadyCreating)
        GetThread()->ResetIsCreatingTypeInitException();
}

void CreateTypeInitializationExceptionObject(LPCWSTR pTypeThatFailed,
                                             OBJECTREF *pInnerException,
                                             OBJECTREF *pInitException,
                                             OBJECTREF *pThrowable)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pInnerException, NULL_OK));
        PRECONDITION(CheckPointer(pInitException));
        PRECONDITION(CheckPointer(pThrowable));
        PRECONDITION(IsProtectedByGCFrame(pInnerException));
        PRECONDITION(IsProtectedByGCFrame(pInitException));
        PRECONDITION(IsProtectedByGCFrame(pThrowable));
        PRECONDITION(CheckPointer(GetThread()));
    } CONTRACTL_END;

    Thread *pThread  = GetThread();
    *pThrowable = NULL;

    // This will make sure to put the thread back to its original state if something
    // throws out of this function (like an OOM exception or something)
    Holder< BOOL, DoNothing< BOOL >, ResetTypeInitializationExceptionState, FALSE, NoNull< BOOL > >
        isAlreadyCreating(pThread->IsCreatingTypeInitException());

    EX_TRY {
        // This will contain the type of exception we want to create. Read comment below
        // on why we'd want to create an exception other than TypeInitException
        MethodTable *pMT;
        BinderMethodID methodID;

        // If we are already in the midst of creating a TypeInitializationException object,
        // and we get here, it means there was an exception thrown while initializing the
        // TypeInitializationException type itself, or one of the types used by its class
        // constructor. In this case, we're going to back down and use a SystemException
        // object in its place. It is *KNOWN* that both these exception types have identical
        // .ctor sigs "void instance (string, exception)" so both can be used interchangeably
        // in the code that follows.
        if (!isAlreadyCreating.GetValue()) {
            pThread->SetIsCreatingTypeInitException();
            pMT = g_Mscorlib.GetException(kTypeInitializationException);
            methodID = METHOD__TYPE_INIT_EXCEPTION__STR_EX_CTOR;
        }
        else {
            _ASSERTE(pInnerException != NULL);
            _ASSERTE(*pInnerException != NULL);
            *pThrowable = *pInnerException;
            *pInitException = *pInnerException;
            goto ErrExit;
        }

        // Allocate the exception object
        *pThrowable = AllocateObject(pMT);

        MethodDescCallSite ctor(methodID, pThrowable);

        // Since the inner exception object in the .ctor is of type Exception, make sure
        // that the object we're passed in derives from Exception. If not, pass NULL.
        BOOL isException = FALSE;
        if (pInnerException != NULL)
            isException = IsException((*pInnerException)->GetMethodTable());

        _ASSERTE(isException);      // What pathway can give us non-compliant exceptions?

        STRINGREF sType = COMString::NewString(pTypeThatFailed);

        // If the inner object derives from exception, set it as the third argument.
        ARG_SLOT args[] = { ObjToArgSlot(*pThrowable),
                            ObjToArgSlot(sType),
                            ObjToArgSlot(isException ? *pInnerException : NULL) };

        // Call the .ctor
        ctor.Call(args);

        // On success, set the init exception.
        *pInitException = *pThrowable;
    }
    EX_CATCH {
        // If calling the constructor fails, then we'll call ourselves again, and this time
        // through we will try and create an EEException object. If that fails, then the
        // else block of this will be executed.
        if (!isAlreadyCreating.GetValue()) {
            CreateTypeInitializationExceptionObject(pTypeThatFailed, pInnerException, pInitException, pThrowable);
        }

        else {
            // If we're recursing, then we should be calling ourselves from DoRunClassInitThrowing,
            // in which case we're guaranteed that we're passing in all three arguments.
            *pInitException = pInnerException ? *pInnerException : NULL;
            *pThrowable = GET_THROWABLE();
        }
    } EX_END_CATCH(SwallowAllExceptions);

    CONSISTENCY_CHECK(*pInitException != NULL || !pInnerException);
 ErrExit:
    ;
}

// ==========================================================================
// ComputeEnclosingHandlerNestingLevel
//
//  This is code factored out of COMPlusThrowCallback to figure out
//  what the number of nested exception handlers is.
// ==========================================================================
DWORD ComputeEnclosingHandlerNestingLevel(IJitManager *pIJM,
                                          METHODTOKEN mdTok,
                                          SIZE_T offsNat)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    // Determine the nesting level of EHClause. Just walk the table
    // again, and find out how many handlers enclose it
    DWORD nestingLevel = 0;
    EH_CLAUSE_ENUMERATOR pEnumState2;
    EE_ILEXCEPTION_CLAUSE EHClause2, *EHClausePtr;
    unsigned EHCount2 = pIJM->InitializeEHEnumeration(mdTok, &pEnumState2);

    for (unsigned j=0; j<EHCount2; j++)
    {
        EHClausePtr = pIJM->GetNextEHClause(mdTok,&pEnumState2,&EHClause2);
        _ASSERTE(EHClausePtr->HandlerEndPC != (DWORD) -1);

        if ((offsNat > EHClausePtr->HandlerStartPC) &&
            (offsNat < EHClausePtr->HandlerEndPC))
        {
                nestingLevel++;
        }
    }

    return nestingLevel;
}

//
//
DWORD ComputeEnclosingHandlerNestingLevel_DEPRECATED(IJitManager *pIJM,
                                  METHODTOKEN mdTok,
                                          SIZE_T offsNat)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    DWORD nestingLevel = 0;
    EH_CLAUSE_ENUMERATOR pEnumState2;
    unsigned EHCount2 = pIJM->InitializeEHEnumeration(mdTok, &pEnumState2);

    if (EHCount2 > 1)
    {
        nestingLevel = ComputeEnclosingHandlerNestingLevel(pIJM, mdTok, offsNat);
    }

    return nestingLevel;
}


// ******************************* EHRangeTreeNode ************************** //
EHRangeTreeNode::EHRangeTreeNode(void)
{
    WRAPPER_CONTRACT;
    CommonCtor(0, false);
}

EHRangeTreeNode::EHRangeTreeNode(DWORD offset, bool fIsRange /* = false */)
{
    WRAPPER_CONTRACT;
    CommonCtor(offset, fIsRange);
}

void EHRangeTreeNode::CommonCtor(DWORD offset, bool fIsRange)
{
    LEAF_CONTRACT;

    m_pTree = NULL;
    m_clause = NULL;

    m_pContainedBy = NULL;

    m_offset   = offset;
    m_fIsRange = fIsRange;
    m_fIsRoot  = false;      // must set this flag explicitly
}

inline bool EHRangeTreeNode::IsRange()
{
    // Please see the header file for an explanation of this assertion.
    _ASSERTE(m_fIsRoot || m_clause != NULL || !m_fIsRange);
    return m_fIsRange;
}

void EHRangeTreeNode::MarkAsRange()
{
    m_offset   = 0;
    m_fIsRange = true;
    m_fIsRoot  = false;
}

inline bool EHRangeTreeNode::IsRoot()
{
    // Please see the header file for an explanation of this assertion.
    _ASSERTE(m_fIsRoot || m_clause != NULL || !m_fIsRange);
    return m_fIsRoot;
}

void EHRangeTreeNode::MarkAsRoot(DWORD offset)
{
    m_offset   = offset;
    m_fIsRange = true;
    m_fIsRoot  = true;
}

inline DWORD EHRangeTreeNode::GetOffset()
{
    _ASSERTE(m_clause == NULL);
    _ASSERTE(IsRoot() || !IsRange());
    return m_offset;
}

inline DWORD EHRangeTreeNode::GetTryStart()
{
    _ASSERTE(IsRange());
    _ASSERTE(!IsRoot());
    if (IsRoot())
    {
        return 0;
    }
    else
    {
        return m_clause->TryStartPC;
    }
}

inline DWORD EHRangeTreeNode::GetTryEnd()
{
    _ASSERTE(IsRange());
    _ASSERTE(!IsRoot());
    if (IsRoot())
    {
        return GetOffset();
    }
    else
    {
        return m_clause->TryEndPC;
    }
}

inline DWORD EHRangeTreeNode::GetHandlerStart()
{
    _ASSERTE(IsRange());
    _ASSERTE(!IsRoot());
    if (IsRoot())
    {
        return 0;
    }
    else
    {
        return m_clause->HandlerStartPC;
    }
}

inline DWORD EHRangeTreeNode::GetHandlerEnd()
{
    _ASSERTE(IsRange());
    _ASSERTE(!IsRoot());
    if (IsRoot())
    {
        return GetOffset();
    }
    else
    {
        return m_clause->HandlerEndPC;
    }
}

inline DWORD EHRangeTreeNode::GetFilterStart()
{
    _ASSERTE(IsRange());
    _ASSERTE(!IsRoot());
    if (IsRoot())
    {
        return 0;
    }
    else
    {
        return m_clause->FilterOffset;
    }
}

bool EHRangeTreeNode::Contains(DWORD offset)
{
    WRAPPER_CONTRACT;

    EHRangeTreeNode node(offset);
    return Contains(&node);
}

bool EHRangeTreeNode::TryContains(DWORD offset)
{
    WRAPPER_CONTRACT;

    EHRangeTreeNode node(offset);
    return TryContains(&node);
}

bool EHRangeTreeNode::HandlerContains(DWORD offset)
{
    WRAPPER_CONTRACT;

    EHRangeTreeNode node(offset);
    return HandlerContains(&node);
}

bool EHRangeTreeNode::FilterContains(DWORD offset)
{
    WRAPPER_CONTRACT;

    EHRangeTreeNode node(offset);
    return FilterContains(&node);
}

bool EHRangeTreeNode::Contains(EHRangeTreeNode* pNode)
{
    LEAF_CONTRACT;

    // If we are checking a range of address, then we should check the end address inclusively.
    if (pNode->IsRoot())
    {
        // No node contains the root node.
        return false;
    }
    else if (this->IsRoot())
    {
        return (pNode->IsRange() ?
                  (pNode->GetTryEnd() <= this->GetOffset()) && (pNode->GetHandlerEnd() <= this->GetOffset())
                : (pNode->GetOffset() < this->GetOffset()) );
    }
    else
    {
        return (this->TryContains(pNode) || this->HandlerContains(pNode) || this->FilterContains(pNode));
    }
}

bool EHRangeTreeNode::TryContains(EHRangeTreeNode* pNode)
{
    LEAF_CONTRACT;

    _ASSERTE(this->IsRange());

    if (pNode->IsRoot())
    {
        // No node contains the root node.
        return false;
    }
    else if (this->IsRoot())
    {
        // We will only get here from GetTcf() to determine if an address is in a try clause.
        // In this case we want to return false.
        return false;
    }
    else
    {
        DWORD tryStart = this->GetTryStart();
        DWORD tryEnd   = this->GetTryEnd();

        // If we are checking a range of address, then we should check the end address inclusively.
        if (pNode->IsRange())
        {
            DWORD start = pNode->GetTryStart();
            DWORD end   = pNode->GetTryEnd();

            if (start == tryStart && end == tryEnd)
            {
                return false;
            }
            else if (start == end)
            {
                // This is effectively a single offset.
                if ((tryStart <= start) && (end < tryEnd))
                {
                    return true;
                }
            }
            else if ((tryStart <= start) && (end <= tryEnd))
            {
                return true;
            }
        }
        else
        {
            DWORD offset = pNode->GetOffset();
            if ((tryStart <= offset) && (offset < tryEnd))
            {
                return true;
            }
        }
    }


    return false;
}

bool EHRangeTreeNode::HandlerContains(EHRangeTreeNode* pNode)
{
    LEAF_CONTRACT;

    _ASSERTE(this->IsRange());

    if (pNode->IsRoot())
    {
        // No node contains the root node.
        return false;
    }
    else if (this->IsRoot())
    {
        // We will only get here from GetTcf() to determine if an address is in a try clause.
        // In this case we want to return false.
        return false;
    }
    else
    {
        DWORD handlerStart = this->GetHandlerStart();
        DWORD handlerEnd   = this->GetHandlerEnd();

        // If we are checking a range of address, then we should check the end address inclusively.
        if (pNode->IsRange())
        {
            DWORD start = pNode->GetTryStart();
            DWORD end   = pNode->GetTryEnd();

            if (start == handlerStart && end == handlerEnd)
            {
                return false;
            }
            else if ((handlerStart <= start) && (end <= handlerEnd))
            {
                return true;
            }
        }
        else
        {
            DWORD offset = pNode->GetOffset();
            if ((handlerStart <= offset) && (offset < handlerEnd))
            {
                return true;
            }
        }
    }


    return false;
}

bool EHRangeTreeNode::FilterContains(EHRangeTreeNode* pNode)
{
    LEAF_CONTRACT;

    _ASSERTE(this->IsRange());

    if (pNode->IsRoot())
    {
        // No node contains the root node.
        return false;
    }
    else if (this->IsRoot() || !IsFilterHandler(this->m_clause))
    {
        // We will only get here from GetTcf() to determine if an address is in a try clause.
        // In this case we want to return false.
        return false;
    }
    else
    {
        DWORD filterStart = this->GetFilterStart();
        DWORD filterEnd   = this->GetHandlerStart();

        // If we are checking a range of address, then we should check the end address inclusively.
        if (pNode->IsRange())
        {
            DWORD start = pNode->GetTryStart();
            DWORD end   = pNode->GetTryEnd();

            if (start == filterStart && end == filterEnd)
            {
                return false;
            }
            else if ((filterStart <= start) && (end <= filterEnd))
            {
                return true;
            }
        }
        else
        {
            DWORD offset = pNode->GetOffset();
            if ((filterStart <= offset) && (offset < filterEnd))
            {
                return true;
            }
        }
    }


    return false;
}

EHRangeTreeNode* EHRangeTreeNode::GetContainer()
{
    return m_pContainedBy;
}

HRESULT EHRangeTreeNode::AddNode(EHRangeTreeNode *pNode)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY;);
        PRECONDITION(pNode != NULL);
    }
    CONTRACTL_END;

    EHRangeTreeNode **ppEH = m_containees.Append();

    if (ppEH == NULL)
        return E_OUTOFMEMORY;

    (*ppEH) = pNode;
    return S_OK;
}

// ******************************* EHRangeTree ************************** //
EHRangeTree::EHRangeTree(COR_ILMETHOD_DECODER *pMethodDecoder)
{
    WRAPPER_CONTRACT;


    LOG((LF_CORDB, LL_INFO10000, "EHRT::ERHT: on disk!\n"));

    _ASSERTE(pMethodDecoder!=NULL);
    m_EHCount = 0xFFFFFFFF;
    m_isNative = FALSE;

    // !!! THIS ISN"T ON THE HEAP - it's only a covienient packaging for
    // the core constructor method, so don't save pointers to it !!!
    EHRT_InternalIterator ii;

    ii.which = EHRT_InternalIterator::EHRTT_ON_DISK;

    if(pMethodDecoder->EH == NULL)
    {
        m_EHCount = 0;
    }
    else
    {
        const COR_ILMETHOD_SECT_EH *sectEH = pMethodDecoder->EH;
        m_EHCount = sectEH->EHCount();
        ii.tf.OnDisk.sectEH = sectEH;
    }

    DWORD methodSize = pMethodDecoder->GetCodeSize();
    CommonCtor(&ii, methodSize);
}

EHRangeTree::EHRangeTree(IJitManager* pIJM,
                         METHODTOKEN methodToken,
                         DWORD methodSize)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORDB, LL_INFO10000, "EHRT::ERHT: already loaded!\n"));

    m_EHCount = 0xFFFFFFFF;
    m_isNative = TRUE;

    // !!! THIS ISN"T ON THE HEAP - it's only a covienient packaging for
    // the core constructor method, so don't save pointers to it !!!
    EHRT_InternalIterator ii;
    ii.which = EHRT_InternalIterator::EHRTT_JIT_MANAGER;
    ii.tf.JitManager.pIJM = pIJM;
    ii.tf.JitManager.methodToken = methodToken;

    m_EHCount = pIJM->InitializeEHEnumeration(methodToken,
                 (EH_CLAUSE_ENUMERATOR*)&ii.tf.JitManager.pEnumState);

    CommonCtor(&ii, methodSize);
}

EHRangeTree::~EHRangeTree()
{
    LEAF_CONTRACT;

    if (m_rgNodes != NULL)
        delete [] m_rgNodes;

    if (m_rgClauses != NULL)
        delete [] m_rgClauses;
} //Dtor

// Before calling this, m_EHCount must be filled in.
void EHRangeTree::CommonCtor(EHRT_InternalIterator *pii,
                             DWORD methodSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(m_EHCount != 0xFFFFFFFF);

    ULONG i = 0;

    m_rgClauses = NULL;
    m_rgNodes = NULL;
    m_root = NULL;
    m_hrInit = S_OK;
    m_fInitializing = true;

    EE_ILEXCEPTION_CLAUSE  *pEHClause = NULL;

    if (m_EHCount > 0)
    {
        m_rgClauses = new (nothrow) EE_ILEXCEPTION_CLAUSE[m_EHCount];
        if (m_rgClauses == NULL)
        {
           m_hrInit = E_OUTOFMEMORY;
           goto LError;
        }
    }

    LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: m_ehcount:0x%x, m_rgClauses:0%x\n",
         m_EHCount, m_rgClauses));

    m_rgNodes = new (nothrow) EHRangeTreeNode[m_EHCount+1];
    if (m_rgNodes == NULL)
    {
       m_hrInit = E_OUTOFMEMORY;
       goto LError;
    }

    //this contains everything, even stuff on the last IP
    m_root = &(m_rgNodes[m_EHCount]);
    m_root->MarkAsRoot(methodSize + 1);

    LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: rgNodes:0x%x\n", m_rgNodes));

    if (m_EHCount ==0)
    {
        LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: About to leave!\n"));
        goto LSuccess;
    }

    LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: Sticking around!\n"));

    pEHClause = NULL;
    EHRangeTreeNode *pNodeCur;

    // First, load all the EH clauses into the object.
    for (i = 0; i < m_EHCount; i++)
    {
        LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: i:0x%x!\n", i));

        switch(pii->which)
        {
            case EHRT_InternalIterator::EHRTT_JIT_MANAGER:
            {
                LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: EHRTT_JIT_MANAGER\n", i));

                pEHClause = pii->tf.JitManager.pIJM->GetNextEHClause(
                                        pii->tf.JitManager.methodToken,
                 (EH_CLAUSE_ENUMERATOR*)&(pii->tf.JitManager.pEnumState),
                                        &(m_rgClauses[i]) );

                LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: EHRTT_JIT_MANAGER got clause\n", i));

                // What's actually
                // happening is that the JIT ignores m_rgClauses[i], and simply
                // hands us back a pointer to their internal data structure.
                // So copy it over, THEN muck with it.
                m_rgClauses[i] = (*pEHClause); //bitwise copy
                pEHClause = &(m_rgClauses[i]);

                LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: clause 0x%x,"
                    "addrof:0x%x\n", i, &(m_rgClauses[i]) ));

                break;
            }

            case EHRT_InternalIterator::EHRTT_ON_DISK:
            {
                LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: EHRTT_ON_DISK\n"));

                COR_ILMETHOD_SECT_EH_CLAUSE_FAT clause;
                const COR_ILMETHOD_SECT_EH_CLAUSE_FAT *pClause;
                pClause = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)pii->tf.OnDisk.sectEH->EHClause(i, &clause);

                LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: EHRTT_ON_DISK got clause\n"));

                // Convert between data structures.
                pEHClause = &(m_rgClauses[i]);  // DON'T DELETE THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                pEHClause->Flags = pClause->GetFlags();
                pEHClause->TryStartPC = pClause->GetTryOffset();
                pEHClause->TryEndPC = pClause->GetTryOffset()+pClause->GetTryLength();
                pEHClause->HandlerStartPC = pClause->GetHandlerOffset();
                pEHClause->HandlerEndPC = pClause->GetHandlerOffset()+pClause->GetHandlerLength();

                LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: post disk get\n"));

                break;
            }
#ifdef _DEBUG
            default:
            {
                _ASSERTE( !"Debugger is trying to analyze an unknown EH format!");
            }
#endif //_DEBUG
        }

        LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: got da clause!\n"));

        _ASSERTE(pEHClause->HandlerEndPC != (DWORD) -1);

        pNodeCur = &(m_rgNodes[i]);

        pNodeCur->m_pTree = this;
        pNodeCur->m_clause = pEHClause;

        // Since the filter doesn't have a start/end FilterPC, the only
        // way we can know the size of the filter is if it's located
        // immediately prior to it's handler.  We assume that this is,
        // and if it isn't, we're so amazingly hosed that we can't
        // continue
        if (pEHClause->Flags == COR_ILEXCEPTION_CLAUSE_FILTER &&
            (pEHClause->FilterOffset >= pEHClause->HandlerStartPC ||
             pEHClause->FilterOffset < pEHClause->TryEndPC))
        {
            m_hrInit = CORDBG_E_SET_IP_IMPOSSIBLE;
            goto LError;
        }

        pNodeCur->MarkAsRange();
    }

    LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: about to do the second pass\n"));


    // Second, for each EH, find it's most limited, containing clause
    // On WIN64, we have duplicate clauses.  There are two types of duplicate clauses.
    //
    // The first type is described in ExceptionHandling.cpp.  This type doesn't add additional information to the
    // EH tree structure.  For example, if an offset is in the try region of a duplicate clause of this type,
    // then some clause which comes before the duplicate clause should contain the offset in its handler region.
    // Therefore, even though this type of duplicate clauses are added to the EH tree, they should never be used.
    //
    // The second type is what's called the protected clause.  These clauses are used to mark the cloned finally
    // region.  They have an empty try region.  Here's an example:
    //
    // // C# code
    // try
    // {
    //     A
    // }
    // finally
    // {
    //     B
    // }
    //
    // // jitted code
    // parent
    // -------
    // A
    // B'
    // -------
    //
    // funclet
    // -------
    // B
    // -------
    //
    // A protected clause covers the B' region in the parent method.  In essence you can think of the method as
    // having two try/finally regions, and that's exactly how protected clauses are handled in the EH tree.
    // They are added to the EH tree just like any other EH clauses.
    for (i = 0; i < m_EHCount; i++)
    {
        LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: SP:0x%x\n", i));

        pNodeCur = &(m_rgNodes[i]);

        EHRangeTreeNode *pNodeCandidate = NULL;
        pNodeCandidate = FindContainer(pNodeCur);
        _ASSERTE(pNodeCandidate != NULL);

        pNodeCur->m_pContainedBy = pNodeCandidate;

        LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: SP: about to add to tree\n"));

        HRESULT hr = pNodeCandidate->AddNode(pNodeCur);
        if (FAILED(hr))
        {
            m_hrInit = hr;
            goto LError;
        }
    }

LSuccess:
    m_fInitializing = false;
    return;

LError:
    LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: LError - something went wrong!\n"));

    if (m_rgClauses != NULL)
    {
        delete [] m_rgClauses;
        m_rgClauses = NULL;
    }

    if (m_rgNodes != NULL)
    {
        delete [] m_rgNodes;
        m_rgNodes = NULL;
    }

    m_fInitializing = false;

    LOG((LF_CORDB, LL_INFO10000, "EHRT::CC: Falling off of LError!\n"));
} // Ctor Core


EHRangeTreeNode *EHRangeTree::FindContainer(EHRangeTreeNode *pNodeSearch)
{
    LEAF_CONTRACT;

    EHRangeTreeNode *pNodeCandidate = NULL;

    // Examine the root, too.
    for (ULONG iInner = 0; iInner < m_EHCount+1; iInner++)
    {
        EHRangeTreeNode *pNodeCur = &(m_rgNodes[iInner]);

        // Check if the current node contains the node we are searching for.
        if ((pNodeSearch != pNodeCur) &&
            pNodeCur->Contains(pNodeSearch))
        {
            // Update the candidate node if it is NULL or if it contains the current node
            // (i.e. the current node is more specific than the candidate node).
            if ((pNodeCandidate == NULL) ||
                pNodeCandidate->Contains(pNodeCur))
            {
                pNodeCandidate = pNodeCur;
            }
        }
    }

    return pNodeCandidate;
}

EHRangeTreeNode *EHRangeTree::FindMostSpecificContainer(DWORD addr)
{
    WRAPPER_CONTRACT;

    EHRangeTreeNode node(addr);
    return FindContainer(&node);
}

EHRangeTreeNode *EHRangeTree::FindNextMostSpecificContainer(EHRangeTreeNode *pNodeSearch, DWORD addr)
{
    WRAPPER_CONTRACT;

    _ASSERTE(!m_fInitializing);

    EHRangeTreeNode **rgpNodes = pNodeSearch->m_containees.Table();

    if (NULL == rgpNodes)
        return pNodeSearch;

    // It's possible that no subrange contains the desired address, so
    // keep a reasonable default around.
    EHRangeTreeNode *pNodeCandidate = pNodeSearch;

    USHORT cSubRanges = pNodeSearch->m_containees.Count();
    EHRangeTreeNode **ppNodeCur = pNodeSearch->m_containees.Table();

    for (int i = 0; i < cSubRanges; i++, ppNodeCur++)
    {
        if ((*ppNodeCur)->Contains(addr) &&
            pNodeCandidate->Contains((*ppNodeCur)))
        {
            pNodeCandidate = (*ppNodeCur);
        }
    }

    return pNodeCandidate;
}

BOOL EHRangeTree::isNative()
{
    LEAF_CONTRACT;

    return m_isNative;
}

BOOL EHRangeTree::isAtStartOfCatch(DWORD offset)
{
    LEAF_CONTRACT;

    if (NULL != m_rgNodes && m_EHCount != 0)
    {
        for(unsigned i = 0; i < m_EHCount;i++)
        {
            if (m_rgNodes[i].m_clause->HandlerStartPC == offset &&
                (!IsFilterHandler(m_rgNodes[i].m_clause) && !IsFaultOrFinally(m_rgNodes[i].m_clause)))
                return TRUE;
        }
    }

    return FALSE;
}

enum TRY_CATCH_FINALLY
{
    TCF_NONE= 0,
    TCF_TRY,
    TCF_FILTER,
    TCF_CATCH,
    TCF_FINALLY,
    TCF_COUNT, //count of all elements, not an element itself
};

#ifdef LOGGING
char *TCFStringFromConst(TRY_CATCH_FINALLY tcf)
{
    LEAF_CONTRACT;

    switch( tcf )
    {
        case TCF_NONE:
            return "TCFS_NONE";
            break;
        case TCF_TRY:
            return "TCFS_TRY";
            break;
        case TCF_FILTER:
            return "TCF_FILTER";
            break;
        case TCF_CATCH:
            return "TCFS_CATCH";
            break;
        case TCF_FINALLY:
            return "TCFS_FINALLY";
            break;
        case TCF_COUNT:
            return "TCFS_COUNT";
            break;
        default:
            return "INVALID TCFS VALUE";
            break;
    }
}
#endif //LOGGING

// We're unwinding if we'll return to the EE's code.  Otherwise
// we'll return to someplace in the current code.  Anywhere outside
// this function is "EE code".
bool FinallyIsUnwinding(EHRangeTreeNode *pNode,
                        ICodeManager* pEECM,
                        PREGDISPLAY pReg,
                        SLOT addrStart)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    const BYTE *pbRetAddr = pEECM->GetFinallyReturnAddr(pReg);

    if (pbRetAddr < (const BYTE *)addrStart)
        return true;

    DWORD offset = (DWORD)(size_t)(pbRetAddr - addrStart);
    EHRangeTreeNode *pRoot = pNode->m_pTree->m_root;

    if (!pRoot->Contains(offset))
        return true;
    else
        return false;
}

BOOL LeaveCatch(ICodeManager* pEECM,
                Thread *pThread,
                CONTEXT *pCtx,
                void *firstException,
                void *methodInfoPtr,
                unsigned offset)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // We can assert these things here, and skip a call
    // to COMPlusCheckForAbort later.

            // If no abort has been requested,
    _ASSERTE((pThread->GetThrowable() != NULL) ||
            // or if there is a pending exception.
            (!pThread->IsAbortRequested()) );

    LPVOID esp = ::COMPlusEndCatch(pThread, pCtx, firstException);

    // Do JIT-specific work
    pEECM->LeaveCatch(methodInfoPtr, offset, pCtx);

    SetSP(pCtx, esp);
    return TRUE;
}


TRY_CATCH_FINALLY GetTcf(EHRangeTreeNode *pNode,
                         ICodeManager* pEECM,
                         void *methodInfoPtr,
                         unsigned offset,
                         PCONTEXT pCtx)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    _ASSERTE(pNode->IsRange() && !pNode->IsRoot());

    TRY_CATCH_FINALLY tcf;

    if (!pNode->Contains(offset))
    {
        tcf = TCF_NONE;
    }
    else if (pNode->TryContains(offset))
    {
        tcf = TCF_TRY;
    }
    else if (pNode->FilterContains(offset))
    {
        tcf = TCF_FILTER;
    }
    else
    {
        _ASSERTE(pNode->HandlerContains(offset));
        if (IsFaultOrFinally(pNode->m_clause))
            tcf = TCF_FINALLY;
        else
            tcf = TCF_CATCH;
    }

    return tcf;
}

const DWORD bEnter = 0x01;
const DWORD bLeave = 0x02;

HRESULT IsLegalTransition(Thread *pThread,
                          bool fCanSetIPOnly,
                          DWORD fEnter,
                          EHRangeTreeNode *pNode,
                          DWORD offFrom,
                          DWORD offTo,
                          ICodeManager* pEECM,
                          PREGDISPLAY pReg,
                          SLOT addrStart,
                          void *firstException,
                          void *methodInfoPtr,
                          PCONTEXT pCtx)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifdef _DEBUG
    if (fEnter & bEnter)
    {
        _ASSERTE(pNode->Contains(offTo));
    }
    if (fEnter & bLeave)
    {
        _ASSERTE(pNode->Contains(offFrom));
    }
#endif //_DEBUG

    // First, figure out where we're coming from/going to
    TRY_CATCH_FINALLY tcfFrom = GetTcf(pNode,
                                       pEECM,
                                       methodInfoPtr,
                                       offFrom,
                                       pCtx);

    TRY_CATCH_FINALLY tcfTo =  GetTcf(pNode,
                                      pEECM,
                                      methodInfoPtr,
                                      offTo,
                                      pCtx);

    LOG((LF_CORDB, LL_INFO10000, "ILT: from %s to %s\n",
        TCFStringFromConst(tcfFrom),
        TCFStringFromConst(tcfTo)));

    switch(tcfFrom)
    {
        case TCF_NONE:
        case TCF_TRY:
        {
            switch(tcfTo)
            {
                case TCF_NONE:
                case TCF_TRY:
                {
                    return S_OK;
                    break;
                }

                case TCF_FILTER:
                {
                    return CORDBG_E_CANT_SETIP_INTO_OR_OUT_OF_FILTER;
                    break;
                }

                case TCF_CATCH:
                {
                    return CORDBG_E_CANT_SET_IP_INTO_CATCH;
                    break;
                }

                case TCF_FINALLY:
                {
                    return CORDBG_E_CANT_SET_IP_INTO_FINALLY;
                    break;
                }
                default:
                    break;
            }
            break;
        }

        case TCF_FILTER:
        {
            switch(tcfTo)
            {
                case TCF_NONE:
                case TCF_TRY:
                case TCF_CATCH:
                case TCF_FINALLY:
                {
                    return CORDBG_E_CANT_SETIP_INTO_OR_OUT_OF_FILTER;
                    break;
                }
                case TCF_FILTER:
                {
                    return S_OK;
                    break;
                }
                default:
                    break;

            }
            break;
        }

        case TCF_CATCH:
        {
            switch(tcfTo)
            {
                case TCF_NONE:
                case TCF_TRY:
                {
                    CONTEXT *pFilterCtx = pThread->GetFilterContext();
                    if (pFilterCtx == NULL)
                        return CORDBG_E_SET_IP_IMPOSSIBLE;

                    if (!fCanSetIPOnly)
                    {
                        if (!LeaveCatch(pEECM,
                                        pThread,
                                        pFilterCtx,
                                        firstException,
                                        methodInfoPtr,
                                        offFrom))
                            return E_FAIL;
                    }
                    return S_OK;
                    break;
                }

                case TCF_FILTER:
                {
                    return CORDBG_E_CANT_SETIP_INTO_OR_OUT_OF_FILTER;
                    break;
                }

                case TCF_CATCH:
                {
                    return S_OK;
                    break;
                }

                case TCF_FINALLY:
                {
                    return CORDBG_E_CANT_SET_IP_INTO_FINALLY;
                    break;
                }
                default:
                    break;
            }
            break;
        }

        case TCF_FINALLY:
        {
            switch(tcfTo)
            {
                case TCF_NONE:
                case TCF_TRY:
                {
                    if (!FinallyIsUnwinding(pNode, pEECM, pReg, addrStart))
                    {
                        CONTEXT *pFilterCtx = pThread->GetFilterContext();
                        if (pFilterCtx == NULL)
                            return CORDBG_E_SET_IP_IMPOSSIBLE;

                        if (!fCanSetIPOnly)
                        {
                            if (!pEECM->LeaveFinally(methodInfoPtr,
                                                     offFrom,
                                                     pFilterCtx))
                                return E_FAIL;
                        }
                        return S_OK;
                    }
                    else
                    {
                        return CORDBG_E_CANT_SET_IP_OUT_OF_FINALLY;
                    }

                    break;
                }

                case TCF_FILTER:
                {
                    return CORDBG_E_CANT_SETIP_INTO_OR_OUT_OF_FILTER;
                    break;
                }

                case TCF_CATCH:
                {
                    return CORDBG_E_CANT_SET_IP_INTO_CATCH;
                    break;
                }

                case TCF_FINALLY:
                {
                    return S_OK;
                    break;
                }
                default:
                    break;
            }
            break;
        }
       break;
       default:
        break;
    }

    _ASSERTE( !"IsLegalTransition: We should never reach this point!" );

    return CORDBG_E_SET_IP_IMPOSSIBLE;
}

HRESULT DestinationIsValid(void *pDjiToken,
                           DWORD offTo,
                           EHRangeTree *pEHRT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    // We'll add a call to the DebugInterface that takes this
    // & tells us if the destination is a stack empty point.
//    DebuggerJitInfo *pDji = (DebuggerJitInfo *)pDjiToken;

    if (pEHRT->isAtStartOfCatch(offTo))
        return CORDBG_S_BAD_START_SEQUENCE_POINT;
    else
        return S_OK;
} // HRESULT DestinationIsValid()

// We want to keep the 'worst' HRESULT - if one has failed (..._E_...) & the
// other hasn't, take the failing one.  If they've both/neither failed, then
// it doesn't matter which we take.
// Note that this macro favors retaining the first argument
#define WORST_HR(hr1,hr2) (FAILED(hr1)?hr1:hr2)
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
                          EHRangeTree *pEHRT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END;

    LPVOID          methodInfoPtr;
    HRESULT         hr = S_OK;
    HRESULT         hrReturn = S_OK;
    PBYTE           EipReal = (PBYTE)GetControlPC(pReg);
    bool            fCheckOnly = true;

    // Make sure that the start point is GC safe
    SetControlPC(pReg, (LPVOID)(addrStart+offFrom));
    IJitManager* pEEJM = ExecutionManager::FindJitMan((PBYTE)GetControlPC(pReg));
    _ASSERTE(pEEJM);
    _ASSERTE(pEEJM == pIJM);
    PREFIX_ASSUME(pEEJM != NULL);

    methodInfoPtr = pEEJM->GetGCInfo(MethodToken);

    ICodeManager * pEECM = pEEJM->GetCodeManager();

    // Make sure that the end point is GC safe
    SetControlPC(pReg, (LPVOID)(addrStart + offTo));
    IJitManager* pEEJMDup;
    pEEJMDup = ExecutionManager::FindJitMan((PBYTE)GetControlPC(pReg));
    _ASSERTE(pEEJMDup == pEEJM);

    // Undo this here so stack traces, etc, don't look weird
    SetControlPC(pReg, (LPVOID)EipReal);

    methodInfoPtr = pEEJM->GetGCInfo(MethodToken);

    ICodeManager * pEECMDup;
    pEECMDup = pEEJM->GetCodeManager();
    _ASSERTE(pEECMDup == pEECM);

    EECodeInfo codeInfo(MethodToken, pEEJM);
    EECodeInfo codeInfoDup(MethodToken, pEEJM);
    EECodeInfo *pCodeInfo    = &codeInfo;
    EECodeInfo *pCodeInfoDup = &codeInfoDup;

    // Do both checks here so compiler doesn't complain about skipping
    // initialization b/c of goto.
    if (fCanSetIPOnly && !pEECM->IsGcSafe(pReg, methodInfoPtr, pCodeInfo, 0))
    {
        hrReturn = WORST_HR(hrReturn, CORDBG_E_SET_IP_IMPOSSIBLE);
    }

    if (fCanSetIPOnly && !pEECM->IsGcSafe(pReg, methodInfoPtr, pCodeInfoDup, 0))
    {
        hrReturn = WORST_HR(hrReturn, CORDBG_E_SET_IP_IMPOSSIBLE);
    }

    if ((hr = DestinationIsValid(pDji, offTo, pEHRT)) != S_OK
        && fCanSetIPOnly)
    {
        hrReturn = WORST_HR(hrReturn,hr);
    }

    // The basic approach is this:  We'll start with the most specific (smallest)
    // EHClause that contains the starting address.  We'll 'back out', to larger
    // and larger ranges, until we either find an EHClause that contains both
    // the from and to addresses, or until we reach the root EHRangeTreeNode,
    // which contains all addresses within it.  At each step, we check/do work
    // that the various transitions (from inside to outside a catch, etc).
    // At that point, we do the reverse process  - we go from the EHClause that
    // encompasses both from and to, and narrow down to the smallest EHClause that
    // encompasses the to point.  We use our nifty data structure to manage
    // the tree structure inherent in this process.
    //
    // NOTE:  We do this process twice, once to check that we're not doing an
    //        overall illegal transition, such as ultimately set the IP into
    //        a catch, which is never allowed.  We're doing this because VS
    //        calls SetIP without calling CanSetIP first, and so we should be able
    //        to return an error code and have the stack in the same condition
    //        as the start of the call, and so we shouldn't back out of clauses
    //        or move into them until we're sure that can be done.

retryForCommit:

    EHRangeTreeNode *node;
    EHRangeTreeNode *nodeNext;
    node = pEHRT->FindMostSpecificContainer(offFrom);

    while (!node->Contains(offTo))
    {
        hr = IsLegalTransition(pThread,
                               fCheckOnly,
                               bLeave,
                               node,
                               offFrom,
                               offTo,
                               pEECM,
                               pReg,
                               addrStart,
                               firstExceptionHandler,
                               methodInfoPtr,
                               pCtx);

        if (FAILED(hr))
        {
            hrReturn = WORST_HR(hrReturn,hr);
        }

        node = node->GetContainer();
        // m_root prevents node from ever being NULL.
    }

    if (node != pEHRT->m_root)
    {
        hr = IsLegalTransition(pThread,
                               fCheckOnly,
                               bEnter|bLeave,
                               node,
                               offFrom,
                               offTo,
                               pEECM,
                               pReg,
                               addrStart,
                               firstExceptionHandler,
                               methodInfoPtr,
                               pCtx);

        if (FAILED(hr))
        {
            hrReturn = WORST_HR(hrReturn,hr);
        }
    }

    nodeNext = pEHRT->FindNextMostSpecificContainer(node,
                                                    offTo);

    while(nodeNext != node)
    {
        hr = IsLegalTransition(pThread,
                               fCheckOnly,
                               bEnter,
                               nodeNext,
                               offFrom,
                               offTo,
                               pEECM,
                               pReg,
                               addrStart,
                               firstExceptionHandler,
                               methodInfoPtr,
                               pCtx);

        if (FAILED(hr))
        {
            hrReturn = WORST_HR(hrReturn, hr);
        }

        node = nodeNext;
        nodeNext = pEHRT->FindNextMostSpecificContainer(node,
                                                        offTo);
    }

    // If it was the intention to actually set the IP and the above transition checks succeeded,
    // then go back and do it all again but this time widen and narrow the thread's actual scope
    if (!fCanSetIPOnly && fCheckOnly && SUCCEEDED(hrReturn))
    {
        fCheckOnly = false;
        goto retryForCommit;
    }

    return hrReturn;
} // HRESULT SetIPFromSrcToDst()

// This function should only be called if the thread is suspended and sitting in jitted code
BOOL IsInFirstFrameOfHandler(Thread *pThread, IJitManager *pJitManager, METHODTOKEN MethodToken, DWORD offset)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    // if don't have a throwable the aren't processing an exception
    if (IsHandleNullUnchecked(pThread->GetThrowableAsHandle()))
        return FALSE;

    EH_CLAUSE_ENUMERATOR pEnumState;
    unsigned EHCount = pJitManager->InitializeEHEnumeration(MethodToken, &pEnumState);

    if (EHCount == 0)
        return FALSE;

    EE_ILEXCEPTION_CLAUSE EHClause, *EHClausePtr;

    for(ULONG i=0; i < EHCount; i++)
    {
         EHClausePtr = pJitManager->GetNextEHClause(MethodToken, &pEnumState, &EHClause);
         _ASSERTE(IsValidClause(EHClausePtr));

        if ( offset >= EHClausePtr->HandlerStartPC && offset < EHClausePtr->HandlerEndPC)
            return TRUE;

        // check if it's in the filter itself if we're not in the handler
        if (IsFilterHandler(EHClausePtr) && offset >= EHClausePtr->FilterOffset && offset < EHClausePtr->HandlerStartPC)
            return TRUE;
    }
    return FALSE;
} // BOOL IsInFirstFrameOfHandler()



//******************************************************************************
// LookForHandler -- search for a function that will handle the exception.
//******************************************************************************
LFH LookForHandler(                         // LFH return types
    const EXCEPTION_POINTERS *pExceptionPointers, // The ExceptionRecord and ExceptionContext
    Thread      *pThread,                   // Thread on which to look (always current?)
    ThrowCallbackType *tct)                 // Structure to pass back to callback functions.
{
    // We don't want to use a runtime contract here since this codepath is used during
    // the processing of a hard SO. Contracts use a significant amount of stack
    // which we can't afford for those cases.
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    // Make sure that the stack depth counter is set to zero.
    COUNTER_ONLY(GetPrivatePerfCounters().m_Excep.cThrowToCatchStackDepth=0);

    // go through to find if anyone handles the exception
    StackWalkAction action = pThread->StackWalkFrames((PSTACKWALKFRAMESCALLBACK)COMPlusThrowCallback,
                                 tct,
                                 0,     //can't use FUNCTIONSONLY because the callback uses non-function frames to stop the walk
                                 tct->pBottomFrame);

    // If someone handles it, the action will be SWA_ABORT with pFunc and dHandler indicating the
        // function and handler that is handling the exception. Debugger can put a hook in here.
    if (action == SWA_ABORT && tct->pFunc != NULL)
        return LFH_FOUND;

    // nobody is handling it
    return LFH_NOT_FOUND;
} // LFH LookForHandler()

StackWalkAction COMPlusUnwindCallback (CrawlFrame *pCf, ThrowCallbackType *pData);

//******************************************************************************
//  UnwindFrames
//******************************************************************************
void UnwindFrames(                      // No return value.
    Thread      *pThread,               // Thread to unwind.
    ThrowCallbackType *tct)             // Structure to pass back to callback function.
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    // Make sure that the stack depth counter is set ro zero.
    COUNTER_ONLY(GetPrivatePerfCounters().m_Excep.cThrowToCatchStackDepth=0);

    if (pThread->IsExceptionInProgress())
    {
        pThread->GetExceptionState()->GetFlags()->SetUnwindHasStarted();
    }

    #ifdef DEBUGGING_SUPPORTED
        //
        // If a debugger is attached, notify it that unwinding is going on.
        //
        if (CORDebuggerAttached())
        {
            g_pDebugInterface->ManagedExceptionUnwindBegin(pThread);
        }
    #endif // DEBUGGING_SUPPORTED

    LOG((LF_EH, LL_INFO1000, "UnwindFrames: going to: pFunc:%#X, pStack:%#X\n",
        tct->pFunc, tct->pStack));

    pThread->StackWalkFrames((PSTACKWALKFRAMESCALLBACK)COMPlusUnwindCallback,
                             tct,
                             POPFRAMES | (tct->pFunc ? FUNCTIONSONLY : 0),  // can only use FUNCTIONSONLY here if
                                                                            // know we will stop
                             tct->pBottomFrame);
} // void UnwindFrames()


void StackTraceInfo::SaveStackTrace(BOOL bAllowAllocMem, OBJECTHANDLE hThrowable, BOOL bReplaceStack, BOOL bSkipLastElement)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END;

    // Do not save stacktrace to preallocated exception.  These are shared.
    if (CLRException::IsPreallocatedExceptionHandle(hThrowable))
    {
        return;
    }

    LOG((LF_EH, LL_INFO1000, "StackTraceInfo::SaveStackTrace (%p), alloc = %d, replace = %d, skiplast = %d\n", this, bAllowAllocMem, bReplaceStack, bSkipLastElement));

    // if have bSkipLastElement, must also keep the stack
    _ASSERTE(! bSkipLastElement || ! bReplaceStack);

    bool         fSuccess = false;
    MethodTable* pMT      = ObjectFromHandle(hThrowable)->GetTrueMethodTable();

    if (bAllowAllocMem && m_dFrameCount != 0)
    {
        EX_TRY
        {
            // Only save stack trace info on exceptions
            _ASSERTE(IsException(pMT));     // what is the pathway here?
            if (!IsException(pMT))
            {
                fSuccess = true;
            }
            else
            {
                // If the stack trace contains DynamicMethodDescs, we need to save the corrosponding
                // System.Resolver objects in the Exception._dynamicMethods field. Failing to do that
                // will cause an AV in the runtime when we try to visit those MethodDescs in the
                // Exception._stackTrace field, because they have been recycled or destroyed.
                unsigned    iNumDynamics      = 0;
                unsigned    iCurDynamic       = 0;

                // How many DynamicMethodDescs do we need to keep alive?
                for (unsigned iElement=0; iElement < m_dFrameCount; iElement++)
                {
                    _ASSERTE(m_pStackTrace[iElement].pFunc);

                    if (m_pStackTrace[iElement].pFunc->IsLCGMethod())
                    {
                        iNumDynamics++;
                    }
                }

                struct _gc
                {
                    StackTraceArray stackTrace;
                    PTRARRAYREF dynamicMethodsArray; // Object array of Managed Resolvers
                    PTRARRAYREF pOrigDynamicArray;

                    _gc()
                        : stackTrace()
                        , dynamicMethodsArray(static_cast<PTRArray *>(NULL))
                        , pOrigDynamicArray(static_cast<PTRArray *>(NULL))
                    {}
                };

                _gc gc;
                GCPROTECT_BEGIN(gc);

                if (bReplaceStack)
                {
                    // nuke previous info
                    gc.stackTrace.Append(m_pStackTrace, m_pStackTrace + m_dFrameCount);

                    if (iNumDynamics)
                    {
                        gc.dynamicMethodsArray = (PTRARRAYREF)AllocateObjectArray(iNumDynamics, g_pObjectClass);
                    }
                }
                else
                {
                    // append to previous info

                    ((EXCEPTIONREF)ObjectFromHandle(hThrowable))->GetStackTrace(gc.stackTrace);

                    if (bSkipLastElement && gc.stackTrace.Size() != 0)
                        gc.stackTrace.AppendSkipLast(m_pStackTrace, m_pStackTrace + m_dFrameCount);
                    else
                        gc.stackTrace.Append(m_pStackTrace, m_pStackTrace + m_dFrameCount);

                    //////////////////////////////

                    unsigned   cOrigDynamic = 0;    // number of objects in the old array
                    gc.pOrigDynamicArray = ((EXCEPTIONREF)ObjectFromHandle(hThrowable))->GetDynamicMethods();
                    if (gc.pOrigDynamicArray != NULL)
                    {
                        cOrigDynamic = gc.pOrigDynamicArray->GetNumComponents();
                    }

                    // Create an object array if we have new and/or old entries.
                    if ((cOrigDynamic + iNumDynamics) > 0)
                    {
                        gc.dynamicMethodsArray = (PTRARRAYREF)AllocateObjectArray(cOrigDynamic + iNumDynamics,
                                                                                  g_pObjectClass);

                        _ASSERTE(!(cOrigDynamic && !gc.pOrigDynamicArray));

                        // Copy previous entries if there are any, and update iCurDynamic to point
                        // to the following index.
                        if (cOrigDynamic && (gc.pOrigDynamicArray != NULL))
                        {
                            memcpyGCRefs (gc.dynamicMethodsArray->GetDataPtr(),
                                          gc.pOrigDynamicArray->GetDataPtr(),
                                          cOrigDynamic * sizeof(Object *));

                            iCurDynamic += cOrigDynamic;
                        }
                    }
                }

                // Update _dynamicMethods field
                if (iNumDynamics)
                {
                    for (unsigned i=0; i < m_dFrameCount; i++)
                    {
                        _ASSERTE(m_pStackTrace[i].pFunc);

                        if (m_pStackTrace[i].pFunc->IsLCGMethod())
                        {
                            // We need to append the corrosponding System.Resolver for
                            // this DynamicMethodDesc to keep it alive.
                            DynamicMethodDesc *pDMD = (DynamicMethodDesc *) m_pStackTrace[i].pFunc;
                            OBJECTREF pResolver = ObjectFromHandle(pDMD->GetLCGMethodResolver()->m_managedResolver);

                            _ASSERTE(pResolver != NULL);

                            // Store it.
                            gc.dynamicMethodsArray->SetAt (iCurDynamic++, pResolver);
                        }
                    }
                }

                MemoryBarrier();  // make sure stackTrace changes are visible before we commit it into the Exception object
                ((EXCEPTIONREF)ObjectFromHandle(hThrowable))->SetStackTrace(gc.stackTrace);
                ((EXCEPTIONREF)ObjectFromHandle(hThrowable))->SetDynamicMethods(gc.dynamicMethodsArray);

                // Update _stackTraceString field.
                ((EXCEPTIONREF)ObjectFromHandle(hThrowable))->SetStackTraceString(NULL);
                fSuccess = true;

                GCPROTECT_END();    // gc
            }
        }
        EX_CATCH
        {
        }
        EX_END_CATCH(SwallowAllExceptions)
    }

    ClearStackTrace();

    if (!fSuccess)
    {
        _ASSERTE(IsException(pMT));         // what is the pathway here?
        if (bReplaceStack && IsException(pMT))
            ((EXCEPTIONREF)ObjectFromHandle(hThrowable))->ClearStackTraceForThrow();
    }
}

// Copy a context record, being careful about whether or not the target
// is large enough to support CONTEXT_EXTENDED_REGISTERS.
//
// NOTE: this function can ONLY be used when a filter function will return
// EXCEPTION_CONTINUE_EXECUTION.  On AMD64, replacing the CONTEXT in any other
// situation may break exception unwinding.
//
// NOTE: this function MUST be used on AMD64.  During exception handling,
// parts of the CONTEXT struct must not be modified.


// High 2 bytes are machine type.  Low 2 bytes are register subset.
#define CONTEXT_EXTENDED_BIT (CONTEXT_EXTENDED_REGISTERS & 0xffff)

VOID
ReplaceExceptionContextRecord(CONTEXT *pTarget, CONTEXT *pSource)
{
    LEAF_CONTRACT;

    _ASSERTE(pTarget);
    _ASSERTE(pSource);


#ifdef CONTEXT_EXTENDED_REGISTERS

    if (pSource->ContextFlags & CONTEXT_EXTENDED_BIT)
    {
        if (pTarget->ContextFlags & CONTEXT_EXTENDED_BIT)
        {   // Source and Target have EXTENDED bit set.
            *pTarget = *pSource;
        }
        else
        {   // Source has but Target doesn't have EXTENDED bit set.  (Target is shorter than Source.)
            //  Copy non-extended part of the struct, and reset the bit on the Target, as it was.
            memcpy(pTarget, pSource, offsetof(CONTEXT, ExtendedRegisters));
            pTarget->ContextFlags &= ~CONTEXT_EXTENDED_BIT;  // Target was short.  Reset the extended bit.
        }
    }
    else
    {   // Source does not have EXTENDED bit.  Copy only non-extended part of the struct.
        memcpy(pTarget, pSource, offsetof(CONTEXT, ExtendedRegisters));
    }
    STRESS_LOG3(LF_SYNC, LL_INFO1000, "ReSet thread context EIP = %p ESP = %p EBP = %p\n",
        GetIP((CONTEXT*)pTarget), GetSP((CONTEXT*)pTarget), GetFP((CONTEXT*)pTarget));

#else // !CONTEXT_EXTENDED_REGISTERS

    // Everything that's left
    *pTarget = *pSource;

#endif // !CONTEXT_EXTENDED_REGISTERS
}

VOID FixupOnRethrow(Thread* pCurThread, EXCEPTION_POINTERS* pExceptionPointers)
{
    WRAPPER_CONTRACT;

    ThreadExceptionState* pExState = pCurThread->GetExceptionState();

    // Don't allow rethrow of a STATUS_STACK_OVERFLOW -- it's a new throw of the COM+ exception.
    if (pExState->GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        return;
    }

    // For COMPLUS exceptions, we don't need the original context for our rethrow.
    if (pExState->GetExceptionCode() != EXCEPTION_COMPLUS)
    {
        _ASSERTE(pExState->GetExceptionRecord());

        // don't copy parm args as have already supplied them on the throw
        memcpy((void*)pExceptionPointers->ExceptionRecord,
               (void*)pExState->GetExceptionRecord(),
               offsetof(EXCEPTION_RECORD, ExceptionInformation));

    }

    pExState->GetFlags()->SetIsRethrown();
}

#if defined(PAL_PORTABLE_SEH) && defined(__GNUC__)

BOOL ComPlusFrameSEH(EXCEPTION_REGISTRATION_RECORD* pReg);

LONG CppEHUnwindFilter(EXCEPTION_POINTERS* ep, LPVOID pv)
{
    PEXCEPTION_REGISTRATION_RECORD pReg = ((PEXCEPTION_REGISTRATION_RECORD)pv)->Next;

    if (pReg == NULL) {
        // We're the topmost handler in the chain, but we don't actually handle anything ourselves
        return EXCEPTION_CONTINUE_SEARCH;
    }

    if (!ComPlusFrameSEH(pReg)) {
        // This means that we installed a CPP_EHUNWIND block
        // somewhere deep in unmanaged code, instead of at
        // a managed/unmanaged boundary
        // We hope this is just benign and continue
        return EXCEPTION_CONTINUE_SEARCH;
    }

    BOOL bUnwinding = ep->ExceptionRecord->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND);

#if _DEBUG
    if (bUnwinding) {
        PPAL_EXCEPTION_REGISTRATION pBottom = PAL_GetBottommostRegistration();
        _ASSERTE(pBottom->Next == pReg);
        _ASSERTE(pBottom->dwFlags & PAL_EXCEPTION_FLAGS_CPPEHUNWIND);
    }
#endif

    while (!(pReg->dwFlags & PAL_EXCEPTION_FLAGS_LONGJMP)) {
        _ASSERTE(ComPlusFrameSEH(pReg));
        if (bUnwinding) {
            pReg->dwFlags &= ~PAL_EXCEPTION_FLAGS_FILTERCALLED;
            PAL_SetBottommostRegistration(pReg);
        }
        else {
            pReg->dwFlags |= PAL_EXCEPTION_FLAGS_FILTERCALLED;
        }

        LONG filterResult = pReg->Handler(ep, pReg->pvFilterParameter);

        if (filterResult != EXCEPTION_CONTINUE_SEARCH) {
            _ASSERTE(!bUnwinding);
            return filterResult;
        }

        pReg = pReg->Next;
    }

    if (bUnwinding)
        PAL_SetBottommostRegistration(pReg);

    return EXCEPTION_CONTINUE_SEARCH;
}

#endif

LONG RaiseExceptionFilter(EXCEPTION_POINTERS* ep, LPVOID pv)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;

    BOOL* pIsRethrown = (BOOL*)pv;

    if (1 == *pIsRethrown)
    {
        // need to reset the EH info back to the original thrown exception
        FixupOnRethrow(GetThread(), ep);
    }
    else
    {
        CONSISTENCY_CHECK((2 == *pIsRethrown) || (0 == *pIsRethrown));
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

//==========================================================================
// Throw an object.
//==========================================================================
VOID DECLSPEC_NORETURN RaiseTheException(OBJECTREF throwable, BOOL rethrow)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    LOG((LF_EH, LL_INFO100, "RealCOMPlusThrow throwing %s\n",
        throwable->GetTrueMethodTable()->GetDebugClassName()));

    if (throwable == NULL)
    {
        _ASSERTE(!"RealCOMPlusThrow(OBJECTREF) called with NULL argument. Somebody forgot to post an exception!");
        EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE);
    }

    if (g_CLRPolicyRequested &&
        throwable->GetMethodTable() == g_pOutOfMemoryExceptionClass)
    {
        // We depends on UNINSTALL_UNWIND_AND_CONTINUE_HANDLER to handle out of memory escalation.
        // We should throw c++ exception instead.
        ThrowOutOfMemory();
    }
    else if (throwable == CLRException::GetPreallocatedStackOverflowException())
    {
        ThrowStackOverflow();
    }

    RaiseTheExceptionInternalOnly(throwable,rethrow);
}

ULONG_PTR GetHRFromThrowable(OBJECTREF throwable)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    ULONG_PTR    hr  = E_FAIL;
    MethodTable *pMT = throwable->GetTrueMethodTable();

    // Only Exception objects have a HResult field
    // So don't fetch the field unless we have an exception

    _ASSERTE(IsException(pMT));     // what is the pathway here?
    if (IsException(pMT))
    {
        hr = ((EXCEPTIONREF)throwable)->GetHResult();
    }

    return hr;
}


VOID DECLSPEC_NORETURN RaiseTheExceptionInternalOnly(OBJECTREF throwable, BOOL rethrow, BOOL fForStackOverflow)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;

    STRESS_LOG3(LF_EH, LL_INFO100, "******* MANAGED EXCEPTION THROWN: Object thrown: %p MT %pT rethrow %d\n",
                OBJECTREFToObject(throwable), (throwable!=0)?throwable->GetMethodTable():0, rethrow);

#ifdef STRESS_LOG
    // Any object could have been thrown, but System.Exception objects have useful information for the stress log
    if (throwable == CLRException::GetPreallocatedStackOverflowException())
    {
        // if are handling an SO, don't try to get all that other goop.  It isn't there anyway,
        // and it could cause us to take another SO.
        STRESS_LOG1(LF_EH, LL_INFO100, "Exception HRESULT = 0x%x \n", COR_E_STACKOVERFLOW);
    }
    else if (throwable != 0)
    {
        _ASSERTE(IsException(throwable->GetMethodTable()));

        int hr = ((EXCEPTIONREF)throwable)->GetHResult();
        STRINGREF message = ((EXCEPTIONREF)throwable)->GetMessage();
        OBJECTREF innerEH = ((EXCEPTIONREF)throwable)->GetInnerException();

        STRESS_LOG4(LF_EH, LL_INFO100, "Exception HRESULT = 0x%x Message String 0x%p (db will display) InnerException %p MT %pT\n",
            hr, OBJECTREFToObject(message), OBJECTREFToObject(innerEH), (innerEH!=0)?innerEH->GetMethodTable():0);
    }
#endif

    Thread* pThread = GetThread();
    _ASSERTE(pThread);

    // Make sure that the object being thrown belongs in the current appdomain.
    #if defined(_DEBUG)
    if (throwable != NULL)
    {
        GCPROTECT_BEGIN(throwable);
        if (!CLRException::IsPreallocatedExceptionObject(throwable))
            _ASSERTE(throwable->CheckAppDomain(GetAppDomain()));
        GCPROTECT_END();
    }
    else
    {   // throwable is NULL -- that shouldn't happen
        _ASSERTE(throwable != NULL);
    }
    #endif

    if (pThread->IsRudeAbortInitiated())
    {
        // Nobody should be able to swallow rude thread abort.
        throwable = CLRException::GetPreallocatedRudeThreadAbortException();
    }

    ThreadExceptionState* pExState = pThread->GetExceptionState();


    rethrow = rethrow ? 1 : 0; // normalize because we use it as a count in RaiseExceptionFilter

    // raise
    PAL_TRY
    {
        //_ASSERTE(! rethrow || pExState->m_pExceptionRecord);
        ULONG_PTR *args = NULL;
        ULONG argCount = 0;
        ULONG flags = 0;
        ULONG code = 0;

        // Always save the current object in the handle so on rethrow we can reuse it. This is important as it
        // contains stack trace info.
        //
        // Note: we use SafeSetLastThrownObject, which will try to set the throwable and if there are any problems,
        // it will set the throwable to something appropiate (like OOM exception) and return the new
        // exception. Thus, the user's exception object can be replaced here.
        throwable = pThread->SafeSetLastThrownObject(throwable);

        if (!rethrow ||
            (pExState->GetExceptionCode() == EXCEPTION_COMPLUS) ||
            (pExState->GetExceptionCode() == STATUS_STACK_OVERFLOW))
        {
            ULONG_PTR hr = GetHRFromThrowable(throwable);

            args = &hr;
            argCount = 1;
            flags = EXCEPTION_NONCONTINUABLE;
            code = EXCEPTION_COMPLUS;
        }
        else
        {
            // Exception code should be consistent.
            _ASSERTE((DWORD)(pExState->GetExceptionRecord()->ExceptionCode) == pExState->GetExceptionCode());

            args     = pExState->GetExceptionRecord()->ExceptionInformation;
            argCount = pExState->GetExceptionRecord()->NumberParameters;
            flags    = pExState->GetExceptionRecord()->ExceptionFlags;
            code     = pExState->GetExceptionRecord()->ExceptionCode;

            flags |= EXCEPTION_NONCONTINUABLE;
        }

        if (pThread->IsAbortInitiated () && IsExceptionOfType(kThreadAbortException,&throwable))
        {
            pThread->ResetPreparingAbort();

            if (pThread->GetFrame() == FRAME_TOP)
            {
                // There is no more managed code on stack.
                pThread->EEResetAbort(Thread::TAR_ALL);
            }
        }

        // Can't access the exception object when are in pre-emptive, so find out before
        // if its an SO.
        BOOL fIsStackOverflow = IsExceptionOfType(kStackOverflowException, &throwable);

        if (fIsStackOverflow || fForStackOverflow)
        {
            // Don't probe if we're already handling an SO.  Just throw the exception.
            RaiseException(code, flags, argCount, args);
        }

        // Probe for sufficient stack.
        PUSH_STACK_PROBE_FOR_THROW(pThread);

        // enable preemptive mode before call into OS
        pThread->EnablePreemptiveGC();

#ifndef STACK_GUARDS_DEBUG
        // In non-debug, we can just raise the exception once we've probed.
        RaiseException(code, flags, argCount, args);

#else
        // In a debug build, we need to unwind our probe structure off the stack.
        BaseStackGuard *pThrowGuard = NULL;
        // Stach away the address of the guard we just pushed above in PUSH_STACK_PROBE_FOR_THROW
        SAVE_ADDRESS_OF_STACK_PROBE_FOR_THROW(pThrowGuard);
        PAL_TRY
        {
            RaiseException(code, flags, argCount, args);
        }
        PAL_FINALLY
        {
            // pop the guard that we pushed above in PUSH_STACK_PROBE_FOR_THROW
            POP_STACK_PROBE_FOR_THROW(pThrowGuard);
        }
        PAL_ENDTRY
#endif
    }
    PAL_EXCEPT_FILTER (RaiseExceptionFilter, &rethrow)
    {
    }
    PAL_ENDTRY
    _ASSERTE(!"Cannot continue after COM+ exception");      // Debugger can bring you here.
    EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE);
}


// INSTALL_COMPLUS_EXCEPTION_HANDLER has a filter, so must put the call in a separate fcn
static VOID DECLSPEC_NORETURN RealCOMPlusThrowWorker(OBJECTREF throwable, BOOL rethrow) {
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;



    if (g_CLRPolicyRequested &&
        throwable->GetMethodTable() == g_pOutOfMemoryExceptionClass)
    {
        // We depends on UNINSTALL_UNWIND_AND_CONTINUE_HANDLER to handle out of memory escalation.
        // We should throw c++ exception instead.
        ThrowOutOfMemory();
    }
    else if (throwable == CLRException::GetPreallocatedStackOverflowException())
    {
        ThrowStackOverflow();
    }

    INSTALL_COMPLUS_EXCEPTION_HANDLER();
    RaiseTheException(throwable, rethrow);
    UNINSTALL_COMPLUS_EXCEPTION_HANDLER();
}


VOID DECLSPEC_NORETURN RealCOMPlusThrow(OBJECTREF throwable, BOOL rethrow) {
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;
    GCPROTECT_BEGIN(throwable);

    _ASSERTE(IsException(throwable->GetMethodTable()));

    // This may look a bit odd, but there is an explaination.  The rethrow boolean
    //  means that an actual RaiseException(EXCEPTION_COMPLUS,...) is being re-thrown,
    //  and that the exception context saved on the Thread object should replace
    //  the exception context from the upcoming RaiseException().  There is logic
    //  in the stack trace code to preserve MOST of the stack trace, but to drop the
    //  last element of the stack trace (has to do with having the address of the rethrow
    //  instead of the address of the original call in the stack trace.  That is
    //  controversial itself, but we won't get into that here.)
    // However, if this is not re-raising that original exception, but rather a new
    //  os exception for what may be an existing exception object, it is generally
    //  a good thing to preserve the stack trace.
    if (!rethrow)
    {
        ExceptionPreserveStackTrace(throwable);
    }

    RealCOMPlusThrowWorker(throwable, rethrow);

    GCPROTECT_END();
}

VOID DECLSPEC_NORETURN RealCOMPlusThrow(OBJECTREF throwable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    RealCOMPlusThrow(throwable, FALSE);
}

//==========================================================================
// Throw an undecorated runtime exception.
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrow(RuntimeExceptionKind reKind)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE((reKind != kExecutionEngineException) ||
             !"Don't throw an ExecutionEngineException without a specific message!");

    EX_THROW(EEException, (reKind));
}

//==========================================================================
// Throw a decorated runtime exception.
// Try using RealCOMPlusThrow(reKind, wszResourceName) instead.
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowNonLocalized(RuntimeExceptionKind reKind, LPCWSTR wszTag)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EEMessageException, (reKind, IDS_EE_GENERIC, wszTag));
}

//==========================================================================
// Throw a decorated runtime exception with a localized message.
// Queries the ResourceManager for a corresponding resource value.
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrow(RuntimeExceptionKind reKind, LPCWSTR wszResourceName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
        PRECONDITION(CheckPointer(wszResourceName));
    }
    CONTRACTL_END;

    //
    // For some reason, the compiler complains about unreachable code if
    // we don't split the new from the throw.  So we're left with this
    // unnecessarily verbose syntax.
    //

    EX_THROW(EEResourceException, (reKind, wszResourceName));
}

// this function finds the managed callback to get a resource
// string from the then current local domain and calls it
// this could be a lot of work
STRINGREF GetResourceStringFromManaged(STRINGREF key)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(key != NULL);
    }
    CONTRACTL_END;

    struct xx {
        STRINGREF key;
        STRINGREF ret;
    } gc;

    gc.key = key;
    gc.ret = NULL;

    // The standard probe isn't good enough here. It's possible that we only have ~14 pages of stack
    // left. By the time we transition to the default domain and start fetching this resource string,
    // another 12 page probe could fail.
    // This failing probe would cause us to unload the default appdomain, which would cause us
    // to take down the process.

    // Instead, let's probe for a lots more stack to make sure that doesn' happen.

    // We need to have enough stack to survive 2 more probes... the original entrypoint back
    // into mscorwks after we go into managed code, and a "large" probe that protects the GC

    INTERIOR_STACK_PROBE_FOR(GetThread(), DEFAULT_ENTRY_PROBE_AMOUNT * 2);
    GCPROTECT_BEGIN(gc);

    MethodDescCallSite getResourceStringLocal(METHOD__ENVIRONMENT__GET_RESOURCE_STRING_LOCAL);

    // Call Environment::GetResourceStringLocal(String name).  Returns String value (or maybe null)

    ENTER_DOMAIN_PTR(SystemDomain::System()->DefaultDomain(),ADV_DEFAULTAD);

    // Don't need to GCPROTECT pArgs, since it's not used after the function call.

    ARG_SLOT pArgs[1] = { ObjToArgSlot(gc.key) };
    gc.ret = getResourceStringLocal.Call_RetSTRINGREF(pArgs);

    END_DOMAIN_TRANSITION;

    GCPROTECT_END();

    END_INTERIOR_STACK_PROBE;


    return gc.ret;
}


// This function does poentially a LOT of work (loading possibly 50 classes).
// The return value is an un-GC-protected string ref, or possibly NULL.
void ResMgrGetString(LPCWSTR wszResourceName, STRINGREF * ppMessage)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    _ASSERTE(ppMessage != NULL);

    if (wszResourceName == NULL || *wszResourceName == L'\0')
    {
        ppMessage = NULL;
        return;
    }

    // this function never looks at name again after
    // calling the helper so no need to GCPROTECT it
    STRINGREF name = COMString::NewString(wszResourceName);

    if (wszResourceName != NULL)
    {
        STRINGREF value = GetResourceStringFromManaged(name);

        _ASSERTE(value!=NULL || !"Resource string lookup failed - possible misspelling or .resources missing or out of date?");
        *ppMessage = value;
    }
}


// GetResourceFromDefault
// transition to the default domain and get a resource there
FCIMPL1(Object*, GetResourceFromDefault, StringObject* keyUnsafe)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_TRIGGERS);
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    STRINGREF ret = NULL;
    STRINGREF key = (STRINGREF)keyUnsafe;

    HELPER_METHOD_FRAME_BEGIN_RET_2(ret, key);

    ret = GetResourceStringFromManaged(key);

    HELPER_METHOD_FRAME_END();

    return OBJECTREFToObject(ret);
}
FCIMPLEND

//==========================================================================
// Throw a decorated runtime exception.
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrow(RuntimeExceptionKind  reKind, UINT resID, LPCWSTR wszArg1,
                                        LPCWSTR wszArg2, LPCWSTR wszArg3, LPCWSTR wszArg4,
                                        LPCWSTR wszArg5, LPCWSTR wszArg6, LPCWSTR wszArg7,
                                        LPCWSTR wszArg8, LPCWSTR wszArg9, LPCWSTR wszArg10)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EEMessageException,
        (reKind, resID, wszArg1, wszArg2, wszArg3, wszArg4, wszArg5, wszArg6, wszArg7, wszArg8, wszArg9, wszArg10));
}


void FreeExceptionData(ExceptionData *pedata)
{
    LEAF_CONTRACT;

    _ASSERTE(pedata != NULL);

    // This test may no longer be necessary.  Remove at own peril.
    Thread *pThread = GetThread();
    if (!pThread)
        return;

    if (pedata->bstrSource)
        SysFreeString(pedata->bstrSource);
    if (pedata->bstrDescription)
        SysFreeString(pedata->bstrDescription);
    if (pedata->bstrHelpFile)
        SysFreeString(pedata->bstrHelpFile);
}


void GetExceptionForHR(HRESULT hr, IErrorInfo* pErrInfo, OBJECTREF* pProtectedThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    // Initialize
    *pProtectedThrowable = NULL;


    // If we made it here and we don't have an exception object, we didn't have a valid IErrorInfo
    // so we'll create an exception based solely on the hresult.
    if ((*pProtectedThrowable) == NULL)
    {
        EEMessageException ex(hr);
        (*pProtectedThrowable) = ex.GetThrowable();
    }
}


void GetExceptionForHR(HRESULT hr, OBJECTREF* pProtectedThrowable)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;        // because of IErrorInfo
        MODE_ANY;
    }
    CONTRACTL_END;

    // Get an IErrorInfo if one is available.
    IErrorInfo *pErrInfo = NULL;
    if (SafeGetErrorInfo(&pErrInfo) != S_OK)
        pErrInfo = NULL;

    GetExceptionForHR(hr, pErrInfo, pProtectedThrowable);
}


//==========================================================================
// Throw a runtime exception based on an HResult
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr, IErrorInfo* pErrInfo )
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;        // because of IErrorInfo
        MODE_ANY;
    }
    CONTRACTL_END;


    if (pErrInfo == NULL)
    {
        EX_THROW(EEMessageException, (hr));
    }
    else
    {
        EX_THROW(EECOMException, (hr, pErrInfo));
    }
}

VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;


    // ! COMPlusThrowHR(hr) no longer snags the IErrorInfo off the TLS (Too many places
    // ! call this routine where no IErrorInfo was set by the prior call.)
    // !
    // ! If you actually want to pull IErrorInfo off the TLS, call
    // !
    // ! COMPlusThrowHR(hr, kGetErrorInfo)

    RealCOMPlusThrowHR(hr, (IErrorInfo*)NULL);
}


VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr, tagGetErrorInfo)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    // Get an IErrorInfo if one is available.
    IErrorInfo *pErrInfo = NULL;
    if (SafeGetErrorInfo(&pErrInfo) != S_OK)
        pErrInfo = NULL;

    // Throw the exception.
    RealCOMPlusThrowHR(hr, pErrInfo);
}



VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr, UINT resID, LPCWSTR wszArg1,
                                          LPCWSTR wszArg2, LPCWSTR wszArg3, LPCWSTR wszArg4,
                                          LPCWSTR wszArg5, LPCWSTR wszArg6, LPCWSTR wszArg7,
                                          LPCWSTR wszArg8, LPCWSTR wszArg9)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EEMessageException,
        (hr, resID, wszArg1, wszArg2, wszArg3, wszArg4, wszArg5, wszArg6, wszArg7, wszArg8, wszArg9));
}


//==========================================================================
// Throw a runtime exception based on the last Win32 error (GetLastError())
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowWin32()
{

// before we do anything else...
    DWORD   err = ::GetLastError();

    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    RealCOMPlusThrowWin32(HRESULT_FROM_WIN32(err));
} // VOID DECLSPEC_NORETURN RealCOMPlusThrowWin32()

//==========================================================================
// Throw a runtime exception based on the last Win32 error (GetLastError())
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowWin32(HRESULT hr)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
}
    CONTRACTL_END;

    // Force to ApplicationException for compatability with previous versions.  We would
    //  prefer a "Win32Exception" here.
    EX_THROW(EEMessageException, (kApplicationException, hr, 0 /* resid*/,
                                  NULL /* szArg1 */, NULL /* szArg2 */, NULL /* szArg3 */, NULL /* szArg4 */,
                                  NULL /* szArg5 */, NULL /* szArg6 */, NULL /* szArg7 */, NULL /* szArg8 */,
                                  NULL /* szArg9 */, NULL /* szArg10 */));
} // VOID DECLSPEC_NORETURN RealCOMPlusThrowWin32()


//==========================================================================
// Throw an OutOfMemoryError
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowOM()
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    ThrowOutOfMemory();
}

//==========================================================================
// Throw a StackOverflowError
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowSO()
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        SO_TOLERANT;
        MODE_ANY;
    }
    CONTRACTL_END;

    static int breakOnSO = -1;

    if (breakOnSO == -1)
        breakOnSO = REGUTIL::GetConfigDWORD(L"BreakOnSO", 0);

    if (breakOnSO != 0)
    {
#ifdef _DEBUG
        _ASSERTE(!"SO occured");
#else
        if (REGUTIL::GetConfigDWORD(L"EHGolden", 0))
            DebugBreak();
#endif
    }

    ThrowStackOverflow();
}

//==========================================================================
// Throw an ArithmeticException
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowArithmetic()
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    RealCOMPlusThrow(kArithmeticException);
}

//==========================================================================
// Throw an ArgumentNullException
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentNull(LPCWSTR argName, LPCWSTR wszResourceName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
        PRECONDITION(CheckPointer(wszResourceName));
    }
    CONTRACTL_END;

    EX_THROW(EEArgumentException, (kArgumentNullException, argName, wszResourceName));
}


VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentNull(LPCWSTR argName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EEArgumentException, (kArgumentNullException, argName, L"ArgumentNull_Generic"));
}


//==========================================================================
// Throw an ArgumentOutOfRangeException
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentOutOfRange(LPCWSTR argName, LPCWSTR wszResourceName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EEArgumentException, (kArgumentOutOfRangeException, argName, wszResourceName));
}

//==========================================================================
// Throw an ArgumentException
//==========================================================================
VOID DECLSPEC_NORETURN RealCOMPlusThrowArgumentException(LPCWSTR argName, LPCWSTR wszResourceName)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EEArgumentException, (kArgumentException, argName, wszResourceName));
}

//==========================================================================
// Throw an InvalidCastException
//==========================================================================
VOID RealCOMPlusThrowInvalidCastException(TypeHandle thCastFrom, TypeHandle thCastTo)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    } CONTRACTL_END;

    // Use an InlineSString with a size of MAX_CLASSNAME_LENGTH + 1 to prevent
    // TypeHandle::GetName from having to allocate a new block of memory. This
    // significantly improves the performance of throwing an InvalidCastException.
    InlineSString<MAX_CLASSNAME_LENGTH + 1> strCastFromName;
    InlineSString<MAX_CLASSNAME_LENGTH + 1> strCastToName;

    thCastTo.GetName(strCastToName);

    if (thCastFrom.IsThunking())
    {
        COMPlusThrow(kInvalidCastException, IDS_EE_CANNOTCASTPROXY, strCastToName.GetUnicode());
    }
    else
    {
        thCastFrom.GetName(strCastFromName);
        COMPlusThrow(kInvalidCastException, IDS_EE_CANNOTCAST, strCastFromName.GetUnicode(), strCastToName.GetUnicode());
    }
}

//
// Maps a Win32 fault to a COM+ Exception enumeration code
//
// Returns 0xFFFFFFFF if it cannot be mapped.
//
DWORD MapWin32FaultToCOMPlusException(EXCEPTION_RECORD *pExceptionRecord)
{
    LEAF_CONTRACT;

    switch (pExceptionRecord->ExceptionCode)
    {
        case STATUS_FLOAT_INEXACT_RESULT:
        case STATUS_FLOAT_INVALID_OPERATION:
        case STATUS_FLOAT_STACK_CHECK:
        case STATUS_FLOAT_UNDERFLOW:
            return (DWORD) kArithmeticException;
        case STATUS_FLOAT_OVERFLOW:
        case STATUS_INTEGER_OVERFLOW:
            return (DWORD) kOverflowException;

        case STATUS_FLOAT_DIVIDE_BY_ZERO:
        case STATUS_INTEGER_DIVIDE_BY_ZERO:
            return (DWORD) kDivideByZeroException;

        case STATUS_FLOAT_DENORMAL_OPERAND:
            return (DWORD) kFormatException;

        case STATUS_ACCESS_VIOLATION:
            {
                // We have a config key, InsecurelyTreatAVsAsNullReference, that ensures we always translate to
                // NullReferenceException instead of doing the new AV translation logic.
                if ((g_pConfig != NULL) && !g_pConfig->LegacyNullReferenceExceptionPolicy() &&
                    !GetCompatibilityFlag(compatNullReferenceExceptionOnAV) )
                {

                    // If the IP of the AV is not in managed code, then its an AccessViolationException.
                    if (!ExecutionManager::FindJitMan((PBYTE)pExceptionRecord->ExceptionAddress))
                    {
                        return (DWORD) kAccessViolationException;
                    }

                    // If the address accessed is above 64k, then its an AccessViolationException. Note: Win9x is a
                    // little different... it never gives you the proper address of the read or write that caused
                    // the fault. It always gives -1, so we can't use it as part of the decision... just give
                    // NullReferenceException instead.
                    if (RunningOnWinNT() && (pExceptionRecord->ExceptionInformation[1] > (64 * 1024)))
                    {
                        return (DWORD) kAccessViolationException;
                    }
                }

            return (DWORD) kNullReferenceException;
            }

        case STATUS_ARRAY_BOUNDS_EXCEEDED:
            return (DWORD) kIndexOutOfRangeException;

        case STATUS_NO_MEMORY:
            return (DWORD) kOutOfMemoryException;

        case STATUS_STACK_OVERFLOW:
            return (DWORD) kStackOverflowException;

#ifdef ALIGN_ACCESS
        case STATUS_DATATYPE_MISALIGNMENT:
            return (DWORD) kDataMisalignedException;
#endif // ALIGN_ACCESS

        default:
            return kSEHException;
    }
}

#ifdef _DEBUG
// check if anyone has written to the stack above the handler which would wipe out the EH registration
void CheckStackBarrier(EXCEPTION_REGISTRATION_RECORD *exRecord)
{
    LEAF_CONTRACT;

    if (exRecord->Handler != (PEXCEPTION_ROUTINE)COMPlusFrameHandler)
        return;

    DWORD *stackOverwriteBarrier = (DWORD *)((BYTE*)exRecord - offsetof(FrameHandlerExRecordWithBarrier, m_ExRecord));
    for (int i =0; i < STACK_OVERWRITE_BARRIER_SIZE; i++) {
        if (*(stackOverwriteBarrier+i) != STACK_OVERWRITE_BARRIER_VALUE) {
            // to debug this error, you must determine who erroneously overwrote the stack
            _ASSERTE(!"Fatal error: the stack has been overwritten");
        }
    }
}
#endif // _DEBUG

//-------------------------------------------------------------------------
// A marker for unmanaged -> EE transition when we know we're in cooperative
// gc mode.  As we leave the EE, we fix a few things:
//
//      - the gc state must be set back to co-operative
//      - the COM+ frame chain must be rewound to what it was on entry
//      - ExInfo()->m_pSearchBoundary must be adjusted
//        if we popped the frame that is identified as begnning the next
//        crawl.
//-------------------------------------------------------------------------

void COMPlusCooperativeTransitionHandler(Frame* pFrame)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_EH, LL_INFO1000, "COMPlusCooprativeTransitionHandler unwinding\n"));

    Thread* pThread = GetThread();

    // Restore us to cooperative gc mode.
    if (!pThread->PreemptiveGCDisabled())
        pThread->DisablePreemptiveGC();

    // Pop the frame chain.
    UnwindFrameChain(pThread, pFrame);
    CONSISTENCY_CHECK(pFrame == pThread->GetFrame());

    // An exception is being thrown through here.  The COM+ exception
    // info keeps a pointer to a frame that is used by the next
    // COM+ Exception Handler as the starting point of its crawl.
    // We may have popped this marker -- in which case, we need to
    // update it to the current frame.
    //
    ThreadExceptionState* pExState = pThread->GetExceptionState();
    Frame*  pSearchBoundary = NULL;

    if (pThread->IsExceptionInProgress())
    {
        pSearchBoundary = pExState->m_currentExInfo.m_pSearchBoundary;
    }

    if (pSearchBoundary && pSearchBoundary < pFrame)
    {
        LOG((LF_EH, LL_INFO1000, "\tpExInfo->m_pSearchBoundary = %08x\n", (void*)pFrame));
        pExState->m_currentExInfo.m_pSearchBoundary = pFrame;
    }
}



void StackTraceInfo::Init()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    LOG((LF_EH, LL_INFO10000, "StackTraceInfo::Init (%p)\n", this));

    m_pStackTrace = NULL;
    m_cStackTrace = 0;
    m_dFrameCount = 0;
}

void StackTraceInfo::FreeStackTrace()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    if (m_pStackTrace)
    {
        delete [] m_pStackTrace;
        m_pStackTrace = NULL;
        m_cStackTrace = 0;
        m_dFrameCount = 0;
    }
}

BOOL StackTraceInfo::IsEmpty()
{
    LEAF_CONTRACT;

    return 0 == m_dFrameCount;
}

void StackTraceInfo::ClearStackTrace()
{
    LEAF_CONTRACT;

    LOG((LF_EH, LL_INFO1000, "StackTraceInfo::ClearStackTrace (%p)\n", this));
    m_dFrameCount = 0;
}

// allocate stack trace info. As each function is found in the stack crawl, it will be added
// to this list. If the list is too small, it is reallocated.
void StackTraceInfo::AllocateStackTrace()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_FORBID_FAULT;

    LOG((LF_EH, LL_INFO1000, "StackTraceInfo::AllocateStackTrace (%p)\n", this));

    if (!m_pStackTrace)
    {
#ifdef _DEBUG
        unsigned int allocSize = 2;    // make small to exercise realloc
#else
        unsigned int allocSize = 30;
#endif

        SCAN_IGNORE_FAULT;
        m_pStackTrace = new (nothrow) StackTraceElement[allocSize];

        if (m_pStackTrace != NULL)
        {
            // Remember how much we allocated.
            m_cStackTrace = allocSize;
        }
        else
        {
            m_cStackTrace = 0;
        }
    }
}

//
// Returns true if it appended the element, false otherwise.
//
BOOL StackTraceInfo::AppendElement(BOOL bAllowAllocMem, UINT_PTR currentIP, UINT_PTR currentSP, MethodDesc* pFunc, CrawlFrame* pCf)
{
    CONTRACTL
    {
        GC_TRIGGERS;
        THROWS;
    }
    CONTRACTL_END

    LOG((LF_EH, LL_INFO10000, "StackTraceInfo::AppendElement (%p), IP = %p, SP = %p, %s::%s\n", this, currentIP, currentSP, pFunc ? pFunc->m_pszDebugClassName : "", pFunc ? pFunc->m_pszDebugMethodName : "" ));
    BOOL bRetVal = FALSE;

    // Save this function in the stack trace array, which we only build on the first pass. We'll try to expand the
    // stack trace array if we don't have enough room. Note that we only try to expand if we're allowed to allocate
    // memory (bAllowAllocMem).
    if (bAllowAllocMem && (m_dFrameCount >= m_cStackTrace))
    {
        FAULT_NOT_FATAL(); // a fault from new here is okay, since the rest of the code is okay if we don't have
                           // room to save functions in the stack trace.
        StackTraceElement* pTempElement = new (nothrow) StackTraceElement[m_cStackTrace*2];

        if (pTempElement != NULL)
        {
            memcpy(pTempElement, m_pStackTrace, m_cStackTrace * sizeof(StackTraceElement));
            delete [] m_pStackTrace;
            m_pStackTrace = pTempElement;
            m_cStackTrace *= 2;
        }
    }

    // Add the function to the stack trace array if there's room.
    if (m_dFrameCount < m_cStackTrace)
    {
        StackTraceElement* pStackTraceElem;

        // If we get in here, we'd better have a stack trace array.
        CONSISTENCY_CHECK(m_pStackTrace != NULL);

        pStackTraceElem = &(m_pStackTrace[m_dFrameCount]);

        pStackTraceElem->pFunc = pFunc;

        // For method code that's shared between instantiations, we get the exact owner from the this pointer
        pStackTraceElem->pExactGenericArgsToken = pCf->GetExactGenericArgsToken();

        pStackTraceElem->ip = currentIP;
        pStackTraceElem->sp = currentSP;

        if (!(pCf->HasFaulted() || pCf->IsIPadjusted()) && pStackTraceElem->ip != 0)
        {
            pStackTraceElem->ip -= 1;
        }

        ++m_dFrameCount;
        bRetVal = TRUE;
        COUNTER_ONLY(GetPrivatePerfCounters().m_Excep.cThrowToCatchStackDepth++);
    }

#if defined(PRESERVE_WATSON_ACROSS_CONTEXTS) // Watson isn't implemented on Rotor or DAC
    {
        Thread *pThread = GetThread();
        _ASSERTE(pThread);
        ThreadExceptionState *pExState = pThread->GetExceptionState();

        // Make sure that "this" StackTraceInfo is the one on the ThreadExceptionState.
        DEBUG_STMT(pExState->AssertStackTraceInfo(this));

        // Adjust the IP if necessary.
        UINT_PTR adjustedIp = currentIP;
        if (!(pCf->HasFaulted() || pCf->IsIPadjusted()) && adjustedIp != 0)
        {
            adjustedIp -= 1;
        }

        // And save it to the ExceptionState.
        LOG((LF_EH, LL_INFO1000, "StackTraceInfo::AppendElement - Capture IP for Watson (%p, %p)\n", this, adjustedIp));
        pExState->SaveIpForUnhandledInfo(adjustedIp);

    }
#endif // rotor or dac


    return bRetVal;
}

void StackTraceInfo::GetLeafFrameInfo(StackTraceElement* pStackTraceElement)
{
    LEAF_CONTRACT;

    if (NULL == m_pStackTrace)
    {
        return;
    }
    _ASSERTE(NULL != pStackTraceElement);

    *pStackTraceElement = m_pStackTrace[0];
}


void UnwindFrameChain(Thread* pThread, LPVOID pvLimitSP)
{
    CONTRACTL
    {
        NOTHROW;
        DISABLED(GC_TRIGGERS);  // some Frames' ExceptionUnwind methods trigger  :(
        MODE_ANY;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CONTRACT_VIOLATION(SOToleranceViolation);

    Frame* pFrame = pThread->m_pFrame;
    if (((LPVOID)pFrame) < pvLimitSP)
    {
        GCX_COOP_THREAD_EXISTS(pThread);

        //
        // call ExceptionUnwind with the Frame chain intact
        //
        pFrame = pThread->NotifyFrameChainOfExceptionUnwind(pFrame, pvLimitSP);

        //
        // now pop the frames off by trimming the Frame chain
        //
        pThread->SetFrame(pFrame);
    }
}

BOOL IsExceptionOfType(RuntimeExceptionKind reKind, Exception *pException)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_FORBID_FAULT;

      if (pException->IsType(reKind))
        return TRUE;

    if (pException->IsType(CLRException::GetType()))
    {

        OBJECTREF Throwable=((CLRException*)pException)->GetThrowable();
        GCX_FORBID();
        if (IsExceptionOfType(reKind, &Throwable))
            return TRUE;
    }
    return FALSE;

};
BOOL IsExceptionOfType(RuntimeExceptionKind reKind, OBJECTREF *pThrowable)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(pThrowableAvailable(pThrowable));

    if (*pThrowable == NULL)
        return FALSE;

    MethodTable *pThrowableMT = (*pThrowable)->GetTrueMethodTable();

    return g_Mscorlib.IsException(pThrowableMT, reKind);
}

BOOL IsAsyncThreadException(OBJECTREF *pThrowable) {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_FORBID_FAULT;

    if (  (GetThread() && GetThread()->IsRudeAbort() && GetThread()->IsRudeAbortInitiated())
        ||IsExceptionOfType(kThreadAbortException, pThrowable)
        ||IsExceptionOfType(kThreadInterruptedException, pThrowable)) {
        return TRUE;
    } else {
    return FALSE;
    }
}

BOOL IsUncatchable(OBJECTREF *pThrowable)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        FORBID_FAULT;
    } CONTRACTL_END;

    _ASSERTE(pThrowable != NULL);

    Thread *pThread = GetThread();

    if (pThread)
    {
        if (pThread->IsAbortInitiated())
            return TRUE;

        if (OBJECTREFToObject(*pThrowable)->GetMethodTable() == g_pExecutionEngineExceptionClass)
        return TRUE;
    }
        return FALSE;
    }

BOOL IsStackOverflowException(Thread* pThread, EXCEPTION_RECORD* pExceptionRecord)
{
    if (STATUS_STACK_OVERFLOW == pExceptionRecord->ExceptionCode)
    {
        return true;
    }

    if (EXCEPTION_COMPLUS == pExceptionRecord->ExceptionCode &&
         pThread->IsLastThrownObjectStackOverflowException())
    {
        return true;
    }

    // We cannot take a dependency on this API in the retail build, so ensure that there is 
    // no case where a C++ exception leaks this far.
    _ASSERTE(DebugGetCxxException(pExceptionRecord) != (void*)Exception::GetSOException());
    
    return false;
}


#ifdef _DEBUG
BOOL IsValidClause(EE_ILEXCEPTION_CLAUSE *EHClause)
{
    LEAF_CONTRACT;

    if (EHClause->TryStartPC > EHClause->TryEndPC)
        return FALSE;
    return TRUE;
}
#endif


#ifdef DEBUGGING_SUPPORTED
static LONG NotifyDebuggerLastChance(Thread *pThread,
                                     EXCEPTION_POINTERS *pExceptionInfo,
                                     BOOL jitAttachRequested,
                                     BOOL useManagedDebugger)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    LONG retval = EXCEPTION_CONTINUE_SEARCH;

    // Debugger does func-evals inside this call, which may take nested exceptions. We need a nested exception
    // handler to allow this.
    INSTALL_NESTED_EXCEPTION_HANDLER(pThread->GetFrame());

    EXCEPTION_POINTERS dummy;
    dummy.ExceptionRecord = NULL;
    dummy.ContextRecord = NULL;
    
    if (NULL == pExceptionInfo)
    {
        pExceptionInfo = &dummy;
    }
    else if (NULL != pExceptionInfo->ExceptionRecord && NULL == pExceptionInfo->ContextRecord)
    {
        // In a soft stack overflow, we have an exception record but not a  context record. 
        // Debugger::LastChanceManagedException requires that both ExceptionRecord and 
        // ContextRecord be valid or both be NULL.
        pExceptionInfo = &dummy;
    }

    if  (g_pDebugInterface && g_pDebugInterface->LastChanceManagedException(pExceptionInfo->ExceptionRecord,
                                                                            pExceptionInfo->ContextRecord,
                                                                            pThread,
                                                                            jitAttachRequested, useManagedDebugger) == ExceptionContinueExecution)
    {
        retval = EXCEPTION_CONTINUE_EXECUTION;
    }

    UNINSTALL_NESTED_EXCEPTION_HANDLER();

    return retval;
}

//------------------------------------------------------------------------------
LONG WatsonLastChance(                  // EXCEPTION_CONTINUE_SEARCH, _CONTINUE_EXECUTION
    Thread              *pThread,       // Thread object.
    EXCEPTION_POINTERS  *pExceptionInfo,// Information about reported exception.
    TypeOfReportedError tore)           // Just what kind of error is reported?
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    // We don't want to launch Watson if a debugger is already attached to
    // the process.
    BOOL shouldNotifyDebugger = FALSE;  // Assume we won't debug.
    BOOL useManagedDebugger = TRUE;     // Assume, if we debug, we debug managed.
    BOOL jitAttachRequested = !CORDebuggerAttached(); // Launch debugger if not already running.

    if (!CORDebuggerAttached())
    {
        LOG((LF_EH, LL_INFO100, "WatsonLastChance: Debugger not attached at sp %p ...\n", GetCurrentSP()));
    }
    else
    {
        LOG((LF_EH, LL_INFO100, "WatsonLastChance: Debugger already attached at sp %p ...\n", GetCurrentSP()));
        shouldNotifyDebugger = TRUE;
    }

    // If no debugger interface, we can't notify the debugger.
    if (g_pDebugInterface == NULL)
    {
        LOG((LF_EH, LL_INFO100, "WatsonLastChance: No debugger interface.  Returning EXCEPTION_CONTINUE_SEARCH\n"));
        return EXCEPTION_CONTINUE_SEARCH;
    }


    //
    // When a managed debugger is attached (or will be attaching), we need
    // to disable the OS GPF dialog. If we don't, an unhandled managed
    // exception will launch the OS watson dialog even when the debugger is
    // attached
    //
    // So, unless we want to launch a native debugger, turn off the dialog.
    //
    // If we do want to launch a native debugger, do not turn off the dialog

    if (shouldNotifyDebugger)
    {
        LOG((LF_EH, LL_INFO100, "WatsonLastChance: Notifying debugger\n"));
        _ASSERTE(CORDebuggerAttached() || jitAttachRequested);

        switch (tore.GetType())
        {
        case TypeOfReportedError::FatalError:
            // If no MDA support, then fall through to the Unhandled exception case...


        case TypeOfReportedError::UnhandledException:
        case TypeOfReportedError::NativeBreakpoint:

            return NotifyDebuggerLastChance(pThread, pExceptionInfo, jitAttachRequested, useManagedDebugger);


        case TypeOfReportedError::UserBreakpoint:

            g_pDebugInterface->LaunchDebuggerForUser();

            return EXCEPTION_CONTINUE_EXECUTION;

        case TypeOfReportedError::NativeThreadUnhandledException:
            // Can't do much with a native thread.
            // Return CONTINUE_SEARCH to launch debugger.
            return EXCEPTION_CONTINUE_SEARCH;

        default:
            _ASSERTE(!"Unknown case in WatsonLastChance");
            return EXCEPTION_CONTINUE_SEARCH;
        }

    }

    return EXCEPTION_CONTINUE_SEARCH;
} // LONG WatsonLastChance()

bool CheckThreadExceptionStateForInterception()
{
    Thread* pThread = GetThread();

    if (pThread == NULL)
    {
        return false;
    }

    if (!pThread->IsExceptionInProgress())
    {
        return false;
    }

    return true;
}
#endif

//===========================================================================================
//
// UNHANDLED EXCEPTION HANDLING
//

void DECLSPEC_NORETURN RaiseDeadLockException() {
    STATIC_CONTRACT_THROWS;

    CHECK_LOCAL_STATIC_VAR(static SString s);
    static volatile BOOL fReady = 0;
    static SpinLock initLock;
    if (!fReady)
    {
        WCHAR name[256];
        HRESULT hr = S_OK;
        {
            FAULT_NOT_FATAL();
            hr = UtilLoadStringRC(IDS_EE_THREAD_DEADLOCK_VICTIM, name, sizeof(name)/sizeof(WCHAR), 1);
            }
        initLock.Init(LOCK_TYPE_DEFAULT);
        SpinLockHolder slh(&initLock);
        if (!fReady)
            {
                if (SUCCEEDED(hr))
                {
                    s.Set(name);
                fReady = 1;
                }
                else
                {
                    ThrowHR(hr);
                }
            }
        }
    ThrowHR(HOST_E_DEADLOCK, s);
}

//
//
//
//
bool ExceptionIsAlwaysSwallowed(EXCEPTION_POINTERS *pExceptionInfo)
{
    bool isSwallowed = false;

    // The exception code must be ours, if it is one of our Exceptions.
    if (EXCEPTION_COMPLUS == pExceptionInfo->ExceptionRecord->ExceptionCode)
    {
        // Our exception code.  Get the current exception from the thread.
        Thread *pThread = GetThread();
        if (pThread)
        {
            OBJECTREF throwable;

            GCX_COOP();
            if ((throwable = pThread->GetThrowable()) == NULL)
            {
                throwable = pThread->LastThrownObject();
            }
            isSwallowed = IsExceptionOfType(kThreadAbortException, &throwable) ||
                          IsExceptionOfType(kAppDomainUnloadedException, &throwable);
        }
    }

    return isSwallowed;
} // BOOL ExceptionIsAlwaysSwallowed()

//
// UserBreakpointFilter is used to ensure that we get a popup on user breakpoints (DebugBreak(), hard-coded int 3,
// etc.) as soon as possible.
//
LONG UserBreakpointFilter(EXCEPTION_POINTERS* pEP)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;


    // Otherwise, we termintate the process.
    TerminateProcess(GetCurrentProcess(), STATUS_BREAKPOINT);

    // Shouldn't get here ...
    return EXCEPTION_CONTINUE_EXECUTION;
} // LONG UserBreakpointFilter()

//******************************************************************************
//
//  DefaultCatchFilter
//
//    The old default except filter (v1.0/v1.1) .  For user breakpoints, call out to UserBreakpointFilter()
//     but otherwise return EXCEPTION_EXECUTE_HANDLER, to swallow the exception.
//
//  Parameters:
//    pExceptionInfo    EXCEPTION_POINTERS for current exception
//    pv                A constant as an INT_PTR.  Must be COMPLUS_EXCEPTION_EXECUTE_HANDLER.
//
//  Returns:
//    EXCEPTION_EXECUTE_HANDLER     Generally returns this to swallow the exception.
//
// IMPORTANT!! READ ME!!
//
// This filter is very similar to DefaultCatchNoSwallowFilter, except when unhandled
//  exception policy/config dictate swallowing the exception.
// If you make any changes to this function, look to see if the other one also needs
//  the same change.
//
LONG DefaultCatchFilter(EXCEPTION_POINTERS *ep, PVOID pv)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    //

    _ASSERTE(pv == COMPLUS_EXCEPTION_EXECUTE_HANDLER);


    PEXCEPTION_RECORD er = ep->ExceptionRecord;
    DWORD code = er->ExceptionCode;

    if (code == STATUS_SINGLE_STEP || code == STATUS_BREAKPOINT)
    {
        return UserBreakpointFilter(ep);
    }

    // return EXCEPTION_EXECUTE_HANDLER to swallow the exception.
    return EXCEPTION_EXECUTE_HANDLER;
} // LONG DefaultCatchFilter()


//******************************************************************************
//
//  DefaultCatchNoSwallowFilter
//
//    The new default except filter (v2.0).  For user breakpoints, call out to UserBreakpointFilter().
//     Otherwise consults host policy and config file to return EXECUTE_HANDLER / CONTINUE_SEARCH.
//
//  Parameters:
//    pExceptionInfo    EXCEPTION_POINTERS for current exception
//    pv                A constant as an INT_PTR.  Must be COMPLUS_EXCEPTION_EXECUTE_HANDLER.
//
//  Returns:
//    EXCEPTION_CONTINUE_SEARCH     Generally returns this to let the exception go unhandled.
//    EXCEPTION_EXECUTE_HANDLER     May return this to swallow the exception.
//
// IMPORTANT!! READ ME!!
//
// This filter is very similar to DefaultCatchFilter, except when unhandled
//  exception policy/config dictate swallowing the exception.
// If you make any changes to this function, look to see if the other one also needs
//  the same change.
//
LONG DefaultCatchNoSwallowFilter(EXCEPTION_POINTERS *ep, PVOID pv)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    // the only valid parameter for DefaultCatchFilter so far
    _ASSERTE(pv == COMPLUS_EXCEPTION_EXECUTE_HANDLER);


    PEXCEPTION_RECORD er = ep->ExceptionRecord;
    DWORD code = er->ExceptionCode;

    if (code == STATUS_SINGLE_STEP || code == STATUS_BREAKPOINT)
    {
        return UserBreakpointFilter(ep);
    }

    // If host policy or config file says "swallow"...
    if (SwallowUnhandledExceptions())
    {   // ...return EXCEPTION_EXECUTE_HANDLER to swallow the exception.
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // If the exception is of a type that is always swallowed (ThreadAbort, AppDomainUnload)...
    if (ExceptionIsAlwaysSwallowed(ep))
    {   // ...return EXCEPTION_EXECUTE_HANDLER to swallow the exception.
        return EXCEPTION_EXECUTE_HANDLER;
    }

    // Otherwise, continue search. i.e. let the exception go unhandled (at least for now).
    return EXCEPTION_CONTINUE_SEARCH;
} // LONG DefaultCatchNoSwallowFilter()


// We keep a pointer to the previous unhandled exception filter.  After we install, we use
// this to call the previous guy.  When we un-install, we put them back.  Putting them back
// is a bug -- we have no guarantee that the DLL unload order matches the DLL load order -- we
// may in fact be putting back a pointer to a DLL that has been unloaded.
//

// initialize to -1 because NULL won't detect difference between us not having installed our handler
// yet and having installed it but the original handler was NULL.
static LPTOP_LEVEL_EXCEPTION_FILTER g_pOriginalUnhandledExceptionFilter = (LPTOP_LEVEL_EXCEPTION_FILTER)-1;
#define FILTER_NOT_INSTALLED (LPTOP_LEVEL_EXCEPTION_FILTER) -1

void InstallUnhandledExceptionFilter() {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_FORBID_FAULT;

    if (g_pOriginalUnhandledExceptionFilter == FILTER_NOT_INSTALLED) {
        g_pOriginalUnhandledExceptionFilter =
              SetUnhandledExceptionFilter(COMUnhandledExceptionFilter);
        // make sure is set (ie. is not our special value to indicate unset)
    }
    _ASSERTE(g_pOriginalUnhandledExceptionFilter != FILTER_NOT_INSTALLED);
}

void UninstallUnhandledExceptionFilter() {
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_FORBID_FAULT;

    if (g_pOriginalUnhandledExceptionFilter != FILTER_NOT_INSTALLED) {
        SetUnhandledExceptionFilter(g_pOriginalUnhandledExceptionFilter);
        g_pOriginalUnhandledExceptionFilter = FILTER_NOT_INSTALLED;
    }
}

//
// Update the current throwable on the thread if necessary. If we're looking at one of our exceptions, and if the
// current throwable on the thread is NULL, then we'll set it to something more useful based on the
// LastThrownObject.
//
BOOL UpdateCurrentThrowable(PEXCEPTION_RECORD pExceptionRecord)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_ANY;
    STATIC_CONTRACT_GC_TRIGGERS;

    BOOL useLastThrownObject = FALSE;

    Thread* pThread = GetThread();

    // GetThrowable needs cooperative.
    GCX_COOP();

    if ((pThread->GetThrowable() == NULL) && (pThread->LastThrownObject() != NULL))
    {
        // If GetThrowable is NULL and LastThrownObject is not, use lastThrownObject.
        //  In current (June 05) implementation, this is only used to pass to
        //  NotifyAppDomainsOfUnhandledException, which needs to get a throwable
        //  from somewhere, with which to notify the AppDomains.
        useLastThrownObject = TRUE;
        
        if (pExceptionRecord->ExceptionCode == EXCEPTION_COMPLUS)
        {
            OBJECTREF oThrowable = pThread->LastThrownObject();

            LOG((LF_EH, LL_INFO100, "UpdateCurrentThrowable: setting throwable to %s\n", (oThrowable == NULL) ? "NULL" : oThrowable->GetTrueMethodTable()->GetDebugClassName()));
            pThread->SafeSetThrowables(oThrowable);
        }
    }

    return useLastThrownObject;
}

//
// COMUnhandledExceptionFilter is used to catch all unhandled exceptions.
// The debugger will either handle the exception, attach a debugger, or
// notify an existing attached debugger.
//


LONG SaveIPFilter(EXCEPTION_POINTERS* ep, LPVOID pv)
{
    WRAPPER_CONTRACT;
    *(SLOT*)pv = (SLOT)GetIP(ep->ContextRecord);
    return DefaultCatchFilter(ep, COMPLUS_EXCEPTION_EXECUTE_HANDLER);
}


//------------------------------------------------------------------------------
// Description
//   Does not call any previous UnhandledExceptionFilter.  The assumption is that
//    either it is inappropriate to call it (because we have elected to rip the
//    process without transitioning completely to the base of the thread), or
//    the caller has already consulted the previously installed UnhandledExceptionFilter.
//
//    So we know we are ripping and Watson is appropriate.
//
//    **** Note*****
//    This is a stack-sensitive function if we have an unhandled SO.
//    Do not allocate more than a few bytes on the stack or we risk taking an
//    AV while trying to throw up Watson.

// Parameters
//    pExceptionInfo -- information about the exception that caused the error.
//           If the error is not the result of an exception, pass NULL for this
//           parameter
//
// Returns
//   EXCEPTION_CONTINUE_SEARCH -- we've done anything we will with the exception.
//      As far as the runtime is concerned, the process is doomed.
//   EXCEPTION_CONTINUE_EXECUTION -- means a debugger "caught" the exception and
//      wants to continue running.
//------------------------------------------------------------------------------
static LONG InternalUnhandledExceptionFilter_Worker(
    EXCEPTION_POINTERS *pExceptionInfo)     // Information about the exception
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

#ifdef _DEBUG
    static int fBreakOnUEF = -1;
    if (fBreakOnUEF==-1) fBreakOnUEF = REGUTIL::GetConfigDWORD(L"BreakOnUEF", 0);
    _ASSERTE(!fBreakOnUEF);
#endif

    LONG    retval = EXCEPTION_CONTINUE_SEARCH;   // Result of UEF filter.
    STRESS_LOG2(LF_EH, LL_INFO10, "In InternalUnhandledExceptionFilter_Worker, Exception = %x, sp = %p\n",
                                    pExceptionInfo->ExceptionRecord->ExceptionCode, GetCurrentSP());

    // If we can't enter the EE, done.
    if (g_fForbidEnterEE)
    {
        LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: g_fForbidEnterEE is TRUE\n"));
        return EXCEPTION_CONTINUE_SEARCH;
    }


    if (GetEEPolicy()->GetActionOnFailure(FAIL_FatalRuntime) == eDisableRuntime)
    {
        ETaskType type = ::GetCurrentTaskType();
        if (type != TT_UNKNOWN && type != TT_USER)
        {
            LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: calling EEPolicy::HandleFatalError\n"));
            EEPolicy::HandleFatalError(COR_E_EXECUTIONENGINE, (UINT_PTR)GetIP(pExceptionInfo->ContextRecord));
        }
    }

    // We don't do anything when this is called from an unmanaged thread.
    Thread *pThread = GetThread();

#ifdef _DEBUG
    static bool bBreakOnUncaught = false;
    static int fBreakOnUncaught = 0;

    if (!bBreakOnUncaught)
    {
        fBreakOnUncaught = REGUTIL::GetConfigDWORD(L"BreakOnUncaughtException", 0);
        bBreakOnUncaught = true;
    }
    if (fBreakOnUncaught != 0)
    {
        if (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_STACK_OVERFLOW)
        {
            // if we've got an uncaught SO, we don't have enough stack to pop a debug break.  So instead,
            // loop infinitely and we can attach a debugger at that point and break in.
            LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: Infinite loop on uncaught SO\n"));
            for ( ;; )
            {
            }
        }
        else
        {
            LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: ASSERTING on uncaught\n"));
            _ASSERTE(!"BreakOnUnCaughtException");
        }
    }
#endif

#ifdef _DEBUG_ADUNLOAD
    printf("%x InternalUnhandledExceptionFilter_Worker: Called for %x\n",
           ((pThread == NULL) ? NULL : pThread->GetThreadId()), pExceptionInfo->ExceptionRecord->ExceptionCode);
    fflush(stdout);
#endif

    // This shouldn't be possible, but MSVC re-installs us... for now, just bail if this happens.
    if (g_fNoExceptions)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }


    LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: Handling\n"));

    SLOT ExceptionEIP = 0;

    PAL_TRY
    {
        // Is this a particular kind of exception that we'd like to ignore?
        BOOL fIgnore = ((pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) ||
                        (pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP));

        // If fIgnore, then this is some sort of breakpoint, not a "normal" unhandled exception.  But, the
        //  breakpoint is due to an int3 or debugger step instruction, not due to calling Debugger.Break()
        TypeOfReportedError tore = fIgnore ? TypeOfReportedError::NativeBreakpoint : TypeOfReportedError::UnhandledException;

        //
        // If this exception is on a thread without managed code, then report this as a NativeThreadUnhandledException
        //
        // The thread object may exist if there was once managed code on the stack, but if the exception never
        // bubbled thru managed code, ie no managed code is on its stack, then this is a native unhandled exception
        //
        // Ignore breakpoints and single-step.
        if (!fIgnore)
        {   // Possibly interesting exception.  Is there no Thread at all?  Or, is there a Thread,
            //  but with no exception at all on it?
            if ((pThread == NULL) || 
                (pThread->IsThrowableNull() && pThread->IsLastThrownObjectNull()) )
            {   // Whatever this exception is, we don't know about it.  Treat as Native.
                tore = TypeOfReportedError::NativeThreadUnhandledException;
            }
        }

        // If there is no throwable on the thread, go ahead and update from the last thrown exception if possible.
        // Note: don't do this for exceptions that we're going to ignore below anyway...
        BOOL useLastThrownObject = FALSE;
        if (!fIgnore && (pThread != NULL))
        {
            useLastThrownObject = UpdateCurrentThrowable(pExceptionInfo->ExceptionRecord);
        }

#ifdef DEBUGGING_SUPPORTED
        LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: Notifying Debugger...\n"));

        //
        // We don't want the managed debugger to try to "intercept" breakpoints
        // or singlestep exceptions.
        if ((pThread != NULL) && pThread->IsExceptionInProgress() && fIgnore)
        {
            pThread->GetExceptionState()->GetFlags()->SetDebuggerInterceptNotPossible();
        }

        if (pThread != NULL)
        {
            // Send notifications to the AppDomains.
            NotifyAppDomainsOfUnhandledException(pExceptionInfo, NULL, useLastThrownObject, TRUE /*isTerminating*/);
        }

        // Launch Watson and see if we want to debug the process
        //
        // Note that we need to do this before "ignoring" exceptions like
        // breakpoints and single step exceptions
        //

        LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: Launching Watson at sp %p ...\n", GetCurrentSP()));

        if (WatsonLastChance(pThread, pExceptionInfo, tore) == EXCEPTION_CONTINUE_EXECUTION)
        {
            LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: debugger ==> EXCEPTION_CONTINUE_EXECUTION\n"));
            retval = EXCEPTION_CONTINUE_EXECUTION;
            goto lDone;
        }

        LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: ... returned.\n"));
#endif // DEBUGGING_SUPPORTED


        //
        // Except for notifying debugger, ignore exception if unmanaged, or
        // if it's a debugger-generated exception or user breakpoint exception.
        //
        if (tore.GetType() == TypeOfReportedError::NativeThreadUnhandledException)
        {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        if (fIgnore)
        {
            LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker, ignoring the exception\n"));
            retval = EXCEPTION_CONTINUE_SEARCH;
            goto lDone;
        }

        LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter_Worker: Calling DefaultCatchHandler\n"));


        // Call our default catch handler to do the managed unhandled exception work.
        DefaultCatchHandler(pExceptionInfo, NULL, useLastThrownObject,
            TRUE /*isTerminating*/, FALSE /*isThreadBaseFIlter*/, FALSE /*sendAppDomainEvents*/);

lDone: ;
    }
    PAL_EXCEPT_FILTER (SaveIPFilter, &ExceptionEIP)
    {
        // Should never get here.
#ifdef _DEBUG
        char buffer[200];
        sprintf_s(buffer, 200, "\nInternal error: Uncaught exception was thrown from IP = %p in UnhandledExceptionFilter_Worker on thread 0x%08x\n",
                ExceptionEIP, ((GetThread() == NULL) ? NULL : GetThread()->GetThreadId()));
        PrintToStdErrA(buffer);
        _ASSERTE(!"Unexpected exception in UnhandledExceptionFilter_Worker");
#endif
        EEPOLICY_HANDLE_FATAL_ERROR(COR_E_EXECUTIONENGINE)
    }
    PAL_ENDTRY

    return retval;
} // LONG InternalUnhandledExceptionFilter_Worker()



//------------------------------------------------------------------------------
// Description
//   Calls any previous UnhandledExceptionFilter.  If that filter doesn't
//    handle the exception (ie, it returns EXCEPTION_CONTINUE_SEARCH), then
//    delegate to InternalUnhandledExceptionFilter_Worker for Watson, etc.
//
// Parameters
//    pExceptionInfo -- information about the exception that caused the error.
//           If the error is not the result of an exception, pass NULL for this
//           parameter
//
// Returns
//   EXCEPTION_CONTINUE_SEARCH -- we've done anything we will with the exception.
//      As far as the runtime is concerned, the process is doomed.
//   EXCEPTION_CONTINUE_EXECUTION -- means a debugger "caught" the exception and
//      wants to continue running.
//------------------------------------------------------------------------------
LONG InternalUnhandledExceptionFilter(
    EXCEPTION_POINTERS *pExceptionInfo)     // Information about the exception
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;
    // We don't need to be SO-robust for an unhandled exception
    SO_NOT_MAINLINE_FUNCTION;

    LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter: at sp %p.\n", GetCurrentSP()));

    LONG    retval = EXCEPTION_CONTINUE_SEARCH;   // Result of UEF filter.

    // If there is another UEF, use it.
    if (g_pOriginalUnhandledExceptionFilter != FILTER_NOT_INSTALLED
        && g_pOriginalUnhandledExceptionFilter != NULL)
    {
        LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter: calling previoius filter.\n"));
        retval = g_pOriginalUnhandledExceptionFilter(pExceptionInfo);
        LOG((LF_EH, LL_INFO100, "InternalUnhandledExceptionFilter: previoius filter returned %d.\n", retval));
    }

    // Keep looking, or done?
    if (retval != EXCEPTION_CONTINUE_SEARCH)
    {   // done.
        return retval;
    }

    return InternalUnhandledExceptionFilter_Worker(pExceptionInfo);
} // LONG InternalUnhandledExceptionFilter()



//------------------------------------------------------------------------------
// Description
//   The actual UEF.  Defers to InternalUnhandledExceptionFilter.
//
// Parameters
//   pExceptionInfo -- information about the exception
//
// Returns
//   the result of calling InternalUnhandledExceptionFilter
//------------------------------------------------------------------------------
LONG __stdcall COMUnhandledExceptionFilter(     // EXCEPTION_CONTINUE_SEARCH or EXCEPTION_CONTINUE_EXECUTION
    EXCEPTION_POINTERS *pExceptionInfo)         // Information about the exception.
{
    WRAPPER_CONTRACT;
    return InternalUnhandledExceptionFilter(pExceptionInfo);
} // LONG __stdcall COMUnhandledExceptionFilter()


void PrintStackTraceToStdout();

static SString GetExceptionMessageWrapper(Thread* pThread, OBJECTREF throwable)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_GC_TRIGGERS;

    StackSString result;

    INSTALL_NESTED_EXCEPTION_HANDLER(pThread->GetFrame());
    GetExceptionMessage(throwable, result);
    UNINSTALL_NESTED_EXCEPTION_HANDLER();

    return result;
}


void STDMETHODCALLTYPE
DefaultCatchHandlerExceptionMessageWorker(Thread* pThread,
                                          OBJECTREF throwable,
                                          __inout_ecount(buf_size) WCHAR *buf,
                                          const int buf_size)
{
    if (throwable != NULL)
    {
        PrintToStdErrA("\n");

        if (FAILED(UtilLoadStringRC(IDS_EE_UNHANDLED_EXCEPTION, buf, buf_size)))
        {
            wcsncpy_s(buf, buf_size, SZ_UNHANDLED_EXCEPTION, SZ_UNHANDLED_EXCEPTION_CHARLEN);
        }

        PrintToStdErrW(buf);
        PrintToStdErrA(" ");

        SString message = GetExceptionMessageWrapper(pThread, throwable);

        if (!message.IsEmpty())
        {
            NPrintToStdErrW(message, message.GetCount());
        }

        PrintToStdErrA("\n");
    }
}

//******************************************************************************
// DefaultCatchHandler -- common processing for otherwise uncaught exceptions.
//******************************************************************************
void STDMETHODCALLTYPE
DefaultCatchHandler(PEXCEPTION_POINTERS pExceptionPointers,
                    OBJECTREF *pThrowableIn,
                    BOOL useLastThrownObject,
                    BOOL isTerminating,
                    BOOL isThreadBaseFilter,
                    BOOL sendAppDomainEvents)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_EH, LL_INFO10, "In DefaultCatchHandler\n"));

#if defined(_DEBUG)
    static bool bHaveInitialized_BreakOnUncaught = false;
    enum BreakOnUncaughtAction {
        breakOnNone     =   0,          // Default.
        breakOnAll      =   1,          // Always break.
        breakSelective  =   2,          // Break on exceptions application can catch,
                                        //  but not ThreadAbort, AppdomainUnload
        breakOnMax      =   2
    };
    static DWORD breakOnUncaught = breakOnNone;

    if (!bHaveInitialized_BreakOnUncaught)
    {
        breakOnUncaught = REGUTIL::GetConfigDWORD(L"BreakOnUncaughtException", breakOnNone);
        if (breakOnUncaught > breakOnMax)
        {   // Could turn it off completely, or turn into legal value.  Since it is debug code, be accommodating.
            breakOnUncaught = breakOnAll;
        }
        bHaveInitialized_BreakOnUncaught = true;
    }

    if (breakOnUncaught == breakOnAll)
    {
        _ASSERTE(!"BreakOnUnCaughtException");
    }

    int suppressSelectiveBreak = false; // to filter for the case where breakOnUncaught == "2"
#endif

    Thread *pThread = GetThread();

    if (!pThread)
    {
        _ASSERTE(g_fEEShutDown);
        return;
    }

    _ASSERTE(pThread);

    ThreadPreventAsyncHolder prevAsync(TRUE);

    GCX_COOP();

    OBJECTREF throwable;

    if (pThrowableAvailable(pThrowableIn))
    {
        throwable = *pThrowableIn;
    }
    else if (useLastThrownObject)
    {
        throwable = pThread->LastThrownObject();
    }
    else
    {
        throwable = pThread->GetThrowable();
    }

    // If we've got no managed object, then we can't send an event or print a message, so we just return.
    if (throwable == NULL)
    {
#ifdef LOGGING
        if (!pThread->IsRudeAbortInitiated())
        {
            LOG((LF_EH, LL_INFO10, "Unhandled exception, throwable == NULL\n"));
        }
#endif

        return;
    }

#ifdef _DEBUG
    DWORD unbreakableLockCount = 0;
    // Do not care about lock check for unhandled exception.
    while (pThread->HasUnbreakableLock())
    {
        pThread->DecUnbreakableLockCount();
        unbreakableLockCount ++;
    }
    BOOL fOwnsSpinLock = pThread->HasThreadStateNC(Thread::TSNC_OwnsSpinLock);
    if (fOwnsSpinLock)
    {
        pThread->ResetThreadStateNC(Thread::TSNC_OwnsSpinLock);
    }
#endif

    GCPROTECT_BEGIN(throwable);
    //BOOL IsStackOverflow = (throwable->GetTrueMethodTable() == g_pStackOverflowExceptionClass);
    BOOL IsOutOfMemory = (throwable->GetTrueMethodTable() == g_pOutOfMemoryExceptionClass);

    // Notify the AppDomain that we have taken an unhandled exception.  Can't notify of stack overflow -- guard
    // page is not yet reset.
    BOOL SentEvent = FALSE;

#ifdef DEBUGGING_SUPPORTED
    if ((pThread->IsExceptionInProgress()) && pThread->GetExceptionState()->GetFlags()->DebuggerInterceptInfo())
    {
        // The debugger wants to intercept this exception.  It may return in a failure case, in which case we want
        // to continue thru this path.
        ClrDebuggerDoUnwindAndIntercept(EXCEPTION_CHAIN_END, pExceptionPointers->ExceptionRecord);
    }
#endif // DEBUGGING_SUPPORTED

    // Send up the unhandled exception appdomain event.
    if (sendAppDomainEvents)
    {
        SentEvent = NotifyAppDomainsOfUnhandledException(pExceptionPointers, &throwable, useLastThrownObject, isTerminating);
    }

    const int buf_size = 128;
    WCHAR buf[buf_size];

    // See detailed explanation of this flag in threads.cpp.  But the basic idea is that we already
    // reported the exception in the AppDomain where it went unhandled, so we don't need to report
    // it at the process level.
    // Print the unhandled exception message.
    if (!pThread->HasThreadStateNC(Thread::TSNC_AppDomainContainUnhandled))
    {
        EX_TRY
        {
            EX_TRY
            {
                // If this isn't ThreadAbortException, we want to print a stack trace to indicate why this thread abruptly
                // terminated. Exceptions kill threads rarely enough that an uncached name check is reasonable.
                BOOL        dump = TRUE;

                if (/*IsStackOverflow ||*/
                    IsOutOfMemory)
                {
                    // We have to be very careful.  If we walk off the end of the stack, the process will just
                    // die. e.g. IsAsyncThreadException() and Exception.ToString both consume too much stack -- and can't
                    // be called here.
                    dump = FALSE;
                    PrintToStdErrA("\n");

                    if (FAILED(UtilLoadStringRC(IDS_EE_UNHANDLED_EXCEPTION, buf, buf_size)))
                    {
                        wcsncpy_s(buf, COUNTOF(buf), SZ_UNHANDLED_EXCEPTION, SZ_UNHANDLED_EXCEPTION_CHARLEN);
                    }

                    PrintToStdErrW(buf);

                    if (IsOutOfMemory)
                    {
                        PrintToStdErrA(" OutOfMemoryException.\n");
                    }
                    else
                    {
                        PrintToStdErrA(" StackOverflowException.\n");
                    }
                }
                else if (!CanRunManagedCode(FALSE))
                {
                    // Well, if we can't enter the runtime, we very well can't get the exception message.
                    dump = FALSE;
                }
                else if (SentEvent || IsAsyncThreadException(&throwable))
                {
                    // We don't print anything on async exceptions, like ThreadAbort.
                    dump = FALSE;
                    INDEBUG(suppressSelectiveBreak=TRUE);
                }
                else if (isThreadBaseFilter && IsExceptionOfType(kAppDomainUnloadedException, &throwable))
                {
                    // AppdomainUnloadedException is also a special case.
                    dump = FALSE;
                    INDEBUG(suppressSelectiveBreak=TRUE);
                }

                // Finally, should we print the message?
                if (dump)
                {
                    // this is stack heavy because of the CQuickWSTRBase, so we break it out
                    // and don't have to carry the weight through our other code paths.
                    DefaultCatchHandlerExceptionMessageWorker(pThread, throwable, buf, buf_size);
                }
            }
            EX_CATCH
            {
                LOG((LF_EH, LL_INFO10, "Exception occurred while processing uncaught exception\n"));
                UtilLoadStringRC(IDS_EE_EXCEPTION_TOSTRING_FAILED, buf, buf_size);
                PrintToStdErrA("\n   ");
                PrintToStdErrW(buf);
                PrintToStdErrA("\n");
            }
            EX_END_CATCH(SwallowAllExceptions);
        }
        EX_CATCH
        {   // If we got here, we can't even print the localized error message.  Print non-localized.
            LOG((LF_EH, LL_INFO10, "Exception occurred while logging processing uncaught exception\n"));
            PrintToStdErrA("\n   Error: Can't print exception string because Exception.ToString() failed.\n");
        }
        EX_END_CATCH(SwallowAllExceptions);
    }

#if defined(_DEBUG)
    if ((breakOnUncaught == breakSelective) && !suppressSelectiveBreak)
    {
        _ASSERTE(!"BreakOnUnCaughtException");
    }
#endif // defined(_DEBUG)

    FlushLogging();     // Flush any logging output
    GCPROTECT_END();

#ifdef _DEBUG
    // Do not care about lock check for unhandled exception.
    while (unbreakableLockCount)
    {
        pThread->IncUnbreakableLockCount();
        unbreakableLockCount --;
    }
    if (fOwnsSpinLock)
    {
        pThread->SetThreadStateNC(Thread::TSNC_OwnsSpinLock);
    }
#endif
} // DefaultCatchHandler()

//******************************************************************************
// NotifyAppDomainsOfUnhandledException -- common processing for otherwise uncaught exceptions.
//******************************************************************************
BOOL NotifyAppDomainsOfUnhandledException(
    PEXCEPTION_POINTERS pExceptionPointers,
    OBJECTREF   *pThrowableIn,
    BOOL        useLastThrownObject,
    BOOL        isTerminating)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

#ifdef _DEBUG
    static int fBreakOnNotify = -1;
    if (fBreakOnNotify==-1) fBreakOnNotify = REGUTIL::GetConfigDWORD(L"BreakOnNotify", 0);
    _ASSERTE(!fBreakOnNotify);
#endif

    BOOL SentEvent = FALSE;

    LOG((LF_EH, LL_INFO10, "In NotifyAppDomainsOfUnhandledException\n"));

    Thread *pThread = GetThread();

    if (!pThread)
    {
        _ASSERTE(g_fEEShutDown);
        return FALSE;
    }

    // See detailed explanation of this flag in threads.cpp.  But the basic idea is that we already
    // reported the exception in the AppDomain where it went unhandled, so we don't need to report
    // it at the process level.
    if (pThread->HasThreadStateNC(Thread::TSNC_AppDomainContainUnhandled))
        return FALSE;

    ThreadPreventAsyncHolder prevAsync(TRUE);

    GCX_COOP();

    OBJECTREF throwable;

    if (pThrowableAvailable(pThrowableIn))
    {
        throwable = *pThrowableIn;
    }
    else if (useLastThrownObject)
    {
        throwable = pThread->LastThrownObject();
    }
    else
    {
        throwable = pThread->GetThrowable();
    }

    // If we've got no managed object, then we can't send an event, so we just return.
    if (throwable == NULL)
    {
        return FALSE;
    }

#ifdef _DEBUG
    DWORD unbreakableLockCount = 0;
    // Do not care about lock check for unhandled exception.
    while (pThread->HasUnbreakableLock())
    {
        pThread->DecUnbreakableLockCount();
        unbreakableLockCount ++;
    }
    BOOL fOwnsSpinLock = pThread->HasThreadStateNC(Thread::TSNC_OwnsSpinLock);
    if (fOwnsSpinLock)
    {
        pThread->ResetThreadStateNC(Thread::TSNC_OwnsSpinLock);
    }
#endif

    GCPROTECT_BEGIN(throwable);
    //BOOL IsStackOverflow = (throwable->GetTrueMethodTable() == g_pStackOverflowExceptionClass);

    // Notify the AppDomain that we have taken an unhandled exception.  Can't notify of stack overflow -- guard
    // page is not yet reset.

    // Send up the unhandled exception appdomain event.
    //
    // If we can't run managed code, we can't deliver the event. Nor do we attempt to delieve the event in stack
    // overflow or OOM conditions.
    if (/*!IsStackOverflow &&*/
        CanRunManagedCode(FALSE))
    {

        // x86 only

        INSTALL_NESTED_EXCEPTION_HANDLER(pThread->GetFrame());

        // This guy will never throw, but it will need a spot to store
        // any nested exceptions it might find.
        SentEvent = AppDomain::OnUnhandledException(&throwable, isTerminating);

        UNINSTALL_NESTED_EXCEPTION_HANDLER();


    }

    GCPROTECT_END();

#ifdef _DEBUG
    // Do not care about lock check for unhandled exception.
    while (unbreakableLockCount)
    {
        pThread->IncUnbreakableLockCount();
        unbreakableLockCount --;
    }
    if (fOwnsSpinLock)
    {
        pThread->SetThreadStateNC(Thread::TSNC_OwnsSpinLock);
    }
#endif

    return SentEvent;

} // NotifyAppDomainsOfUnhandledException()


BOOL COMPlusIsMonitorException(EXCEPTION_POINTERS *pExceptionInfo)
{
    WRAPPER_CONTRACT;
    return COMPlusIsMonitorException(pExceptionInfo->ExceptionRecord,
                                     pExceptionInfo->ContextRecord);
}

BOOL COMPlusIsMonitorException(EXCEPTION_RECORD *pExceptionRecord,
                               CONTEXT *pContext)
{
    WRAPPER_CONTRACT;
    return false;
}


//******************************************************************************
//
//  ThreadBaseExceptionFilter_Worker
//
//    The return from the function can be EXCEPTION_CONTINUE_SEARCH to let an
//     exception go unhandled.  This is the default behaviour (starting in v2.0),
//     but can be overridden by hosts or by config file.
//    When the behaviour is overridden, the return will be EXCEPTION_EXECUTE_HANDLER
//     to swallow the exception.
//    Note that some exceptions are always swallowed: ThreadAbort, and AppDomainUnload.
//
//  Parameters:
//    pExceptionInfo    EXCEPTION_POINTERS for current exception
//    _location         A constant as an INT_PTR.  Tells the context from whence called.
//    swallowing        Are we swallowing unhandled exceptions based on policy?
//
//  Returns:
//    EXCEPTION_CONTINUE_SEARCH     Generally returns this to let the exception go unhandled.
//    EXCEPTION_EXECUTE_HANDLER     May return this to swallow the exception.
//
static LONG ThreadBaseExceptionFilter_Worker(PEXCEPTION_POINTERS pExceptionInfo,
                                             PVOID _location,
                                             BOOL swallowing)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    LOG((LF_EH, LL_INFO100, "ThreadBaseExceptionFilter_Worker: Enter\n"));

    UnhandledExceptionLocation location = (UnhandledExceptionLocation)(INT_PTR)_location;


    _ASSERTE(!g_fNoExceptions);

    Thread* pThread = GetThread();
    _ASSERTE(pThread);

#ifdef _DEBUG
    if (REGUTIL::GetConfigDWORD(L"BreakOnUncaughtException", 0) &&
        !(swallowing && (SwallowUnhandledExceptions() || ExceptionIsAlwaysSwallowed(pExceptionInfo))) &&
        !(location == ClassInitUnhandledException && pThread->IsRudeAbortInitiated()))
        _ASSERTE(!"BreakOnUnCaughtException");
#endif

    BOOL doDefault =  ((location != ClassInitUnhandledException) &&
                       (pExceptionInfo->ExceptionRecord->ExceptionCode != STATUS_BREAKPOINT) &&
                       (pExceptionInfo->ExceptionRecord->ExceptionCode != STATUS_SINGLE_STEP));

    if (swallowing)
    {
        // The default handling for versions v1.0 and v1.1 was to swallow unhandled exceptions.
        //  With v2.0, the default is to let them go unhandled.  Hosts & config files can modify the default
        //  to retain the v1.1 behaviour.
        // Should we swallow this exception, or let it continue up and be unhandled?
        if (!SwallowUnhandledExceptions())
        {
            // No, don't swallow unhandled exceptions...

            // ...except if the exception is of a type that is always swallowed (ThreadAbort, AppDomainUnload)...
            if (ExceptionIsAlwaysSwallowed(pExceptionInfo))
            {   // ...return EXCEPTION_EXECUTE_HANDLER to swallow the exception anyway.
                return EXCEPTION_EXECUTE_HANDLER;
            }

            #ifdef _DEBUG
            if (REGUTIL::GetConfigDWORD(L"BreakOnUncaughtException", 0))
                _ASSERTE(!"BreakOnUnCaughtException");
            #endif

            // ...so, continue search. i.e. let the exception go unhandled.
            pThread->GetExceptionState()->MaybeEagerlyCaptureUnhandledInfoForWatson();
            return EXCEPTION_CONTINUE_SEARCH;
        }
    }

#ifdef DEBUGGING_SUPPORTED
    // If there's a debugger (and not doing a thread abort), give the debugger a shot at the exception.
    // If the debugger is going to try to continue the exception, it will return ContinueException (which
    // we see here as EXCEPTION_CONTINUE_EXECUTION).
    if (!pThread->IsAbortRequested())
    {
        if (CORDebuggerAttached())
        {
            if (NotifyDebuggerLastChance(pThread, pExceptionInfo, FALSE, TRUE) == EXCEPTION_CONTINUE_EXECUTION)
            {
                LOG((LF_EH, LL_INFO100, "ThreadBaseExceptionFilter_Worker: EXCEPTION_CONTINUE_EXECUTION\n"));
                return EXCEPTION_CONTINUE_EXECUTION;
            }
        }
    }
#endif // DEBUGGING_SUPPORTED

    // Do default handling, but ignore breakpoint exceptions and class init exceptions
    if (doDefault)
    {
        LOG((LF_EH, LL_INFO100, "ThreadBaseExceptionFilter_Worker: Calling DefaultCatchHandler\n"));

        BOOL useLastThrownObject = UpdateCurrentThrowable(pExceptionInfo->ExceptionRecord);

        DefaultCatchHandler(pExceptionInfo,
                            NULL,
                            useLastThrownObject,
                            FALSE,
                            location == ManagedThread || location == ThreadPoolThread || location == FinalizerThread);
    }

    // If we're not swallowing the exception...
    if (!swallowing)
    {   // Capture Watson buckets before letting the exception go unhandled.
        pThread->GetExceptionState()->MaybeEagerlyCaptureUnhandledInfoForWatson();
    }


    // Return EXCEPTION_EXECUTE_HANDLER to swallow the exception.
    return (swallowing
            ? EXCEPTION_EXECUTE_HANDLER
            : EXCEPTION_CONTINUE_SEARCH);
} // LONG ThreadBaseExceptionFilter_Worker()


//    This is the filter for new managed threads, for threadpool threads, and for
//     running finalizer methods.
LONG ThreadBaseExceptionSwallowingFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID _location)
{
    return ThreadBaseExceptionFilter_Worker(pExceptionInfo, _location, /*swallowing=*/true);
}

//    This was the filter for new managed threads in v1.0 and v1.1.  Now used
//     for delegate invoke, various things in the thread pool, and the
//     class init handler.
LONG ThreadBaseExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID _location)
{
    return ThreadBaseExceptionFilter_Worker(pExceptionInfo, _location, /*swallowing=*/false);
}


//    This is the filter that we install when transitioning an AppDomain at the base of a managed
//     thread.  Nothing interesting will get swallowed after us.  So we never decide to continue
//     the search.  Instead, we let it go unhandled and get the Watson report and debugging
//     experience before the AD transition has an opportunity to catch/rethrow and lose all the
//     relevant information.
LONG ThreadBaseExceptionAppDomainFilter(EXCEPTION_POINTERS *pExceptionInfo, PVOID _location)
{
    LONG ret = ThreadBaseExceptionSwallowingFilter(pExceptionInfo, _location);

    if (ret != EXCEPTION_CONTINUE_SEARCH)
        return ret;

    return InternalUnhandledExceptionFilter_Worker(pExceptionInfo);
}


// Filter for calls out from the 'vm' to native code, if there's a possibility of SEH exceptions
// in the native code.
LONG CallOutFilter(PEXCEPTION_POINTERS pExceptionInfo, PVOID pv)
{
    BOOL *pOneShot = (BOOL *) pv;

    _ASSERTE(pOneShot && (*pOneShot == TRUE || *pOneShot == FALSE));

    if (*pOneShot == TRUE)
    {
        *pOneShot = FALSE;

        // Replace whatever SEH exception is in flight, with an SEHException derived from
        // CLRException.  But if the exception already looks like one of ours, let it
        // blow past since LastThrownObject should already represent it.
        if ((pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_COMPLUS) && 
            (pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_MSVC))
            PAL_CPP_THROW(SEHException *, new SEHException(pExceptionInfo->ExceptionRecord,
                                                           pExceptionInfo->ContextRecord));
    }
    return EXCEPTION_CONTINUE_SEARCH;
}


//=========================================================================
// Used by the classloader to record a managed exception object to explain
// why a classload got botched.
//
// - Can be called with gc enabled or disabled.
//   This allows a catch-all error path to post a generic catchall error
//   message w/out bonking more specific error messages posted by inner functions.
//==========================================================================
VOID DECLSPEC_NORETURN ThrowTypeLoadException(LPCUTF8 pszNameSpace,
                                              LPCUTF8 pTypeName,
                                              LPCWSTR pAssemblyName,
                                              LPCUTF8 pMessageArg,
                                              UINT resIDWhy)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EETypeLoadException, (pszNameSpace, pTypeName, pAssemblyName, pMessageArg, resIDWhy));
}


//==========================================================================
// Used by the classloader to record a managed exception object to explain
// why a classload got botched.
//
// - Can be called with gc enabled or disabled.
//   This allows a catch-all error path to post a generic catchall error
//   message w/out bonking more specific error messages posted by inner functions.
//==========================================================================
VOID DECLSPEC_NORETURN ThrowTypeLoadException(LPCWSTR pFullTypeName,
                                              LPCWSTR pAssemblyName,
                                              LPCUTF8 pMessageArg,
                                              UINT resIDWhy)
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    EX_THROW(EETypeLoadException, (pFullTypeName, pAssemblyName, pMessageArg, resIDWhy));
}


//==========================================================================
// Used by the classloader to post illegal layout
//==========================================================================
VOID DECLSPEC_NORETURN ThrowFieldLayoutError(mdTypeDef cl,                // cl of the NStruct being loaded
                           Module* pModule,             // Module that defines the scope, loader and heap (for allocate FieldMarshalers)
                           DWORD   dwOffset,            // Offset of field
                           DWORD   dwID)                // Message id
{
    CONTRACTL
    {
        THROWS;
        DISABLED(GC_NOTRIGGER);  // Must sanitize first pass handling to enable this
        MODE_ANY;
    }
    CONTRACTL_END;

    IMDInternalImport *pInternalImport = pModule->GetMDImport();    // Internal interface for the NStruct being loaded.


    LPCUTF8 pszName, pszNamespace;
    pInternalImport->GetNameOfTypeDef(cl, &pszName, &pszNamespace);

    CHAR offsetBuf[16];
    sprintf_s(offsetBuf, COUNTOF(offsetBuf), "%d", dwOffset);
    offsetBuf[COUNTOF(offsetBuf) - 1] = '\0';

    pModule->GetAssembly()->ThrowTypeLoadException(pszNamespace,
                                                   pszName,
                                                   offsetBuf,
                                                   dwID);
}

//==========================================================================
// Convert the format string used by sprintf to the format used by String.Format.
// Using the managed formatting routine avoids bogus access violations
// that happen for long strings in Win32's FormatMessage.
//
// Note: This is not general purpose routine. It handles only cases found
// in TypeLoadException and FileLoadException.
//==========================================================================
static STRINGREF GetManagedFormatStringForResourceID(UINT32 resId)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    StackSString temp;
    StackSString converted;
    temp.LoadResource(resId, FALSE);

    SString::Iterator itr = temp.Begin();
    while (*itr)
    {
        WCHAR c = *itr++;
        switch (c) {
        case '%':
            {
                WCHAR fmt = *itr++;
                if (fmt >= '1' && fmt <= '9') {
                    converted.Append(L"{");
                    converted.Append(fmt - 1); // the managed args start at 0
                    converted.Append(L"}");
                }
                else
                if (fmt == '%') {
                    converted.Append(L"%");
                }
                else {
                    _ASSERTE(!"Unexpected formating string: %s");
                }
            }
            break;
        case '{':
            converted.Append(L"{{");
            break;
        case '}':
            converted.Append(L"}}");
            break;
        default:
            converted.Append(c);
            break;
        }
    }

    return COMString::NewString(converted);
}

//==========================================================================
// Private helper for TypeLoadException.
//==========================================================================

FCIMPL1(StringObject*, GetTypeLoadExceptionMessage, UINT32 resId)
{
    CONTRACTL
    {
        SO_TOLERANT;
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);
    //-[autocvtpro]-------------------------------------------------------

    refRetVal = GetManagedFormatStringForResourceID(resId ? resId : IDS_CLASSLOAD_GENERAL);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return STRINGREFToObject(refRetVal);
}
FCIMPLEND

UINT GetResourceIDForFileLoadExceptionHR(HRESULT hr)
{
    switch ((HRESULT)hr) {

    case CTL_E_FILENOTFOUND:
        hr = IDS_EE_FILE_NOT_FOUND;
        break;

    case IDS_EE_PROC_NOT_FOUND:
    case IDS_EE_PATH_TOO_LONG:
    case INET_E_OBJECT_NOT_FOUND:
    case INET_E_DATA_NOT_AVAILABLE:
    case INET_E_DOWNLOAD_FAILURE:
    case INET_E_UNKNOWN_PROTOCOL:
    case IDS_INET_E_SECURITY_PROBLEM:
    case IDS_EE_BAD_USER_PROFILE:
    case IDS_EE_ALREADY_EXISTS:
    case IDS_EE_REFLECTIONONLY_LOADFAILURE:
    case IDS_CLASSLOAD_32BITCLRLOADING64BITASSEMBLY:
       break;

    case MK_E_SYNTAX:
        hr = FUSION_E_INVALID_NAME;
        break;

    case INET_E_CONNECTION_TIMEOUT:
        hr = IDS_INET_E_CONNECTION_TIMEOUT;
        break;

    case INET_E_CANNOT_CONNECT:
        hr = IDS_INET_E_CANNOT_CONNECT;
        break;

    case INET_E_RESOURCE_NOT_FOUND:
        hr = IDS_INET_E_RESOURCE_NOT_FOUND;
        break;

    case NTE_BAD_HASH:
    case NTE_BAD_LEN:
    case NTE_BAD_KEY:
    case NTE_BAD_DATA:
    case NTE_BAD_ALGID:
    case NTE_BAD_FLAGS:
    case NTE_BAD_HASH_STATE:
    case NTE_BAD_UID:
    case NTE_FAIL:
    case NTE_BAD_TYPE:
    case NTE_BAD_VER:
    case NTE_BAD_SIGNATURE:
    case NTE_SIGNATURE_FILE_BAD:
    case CRYPT_E_HASH_VALUE:
        hr = IDS_EE_HASH_VAL_FAILED;
        break;

    default:
        hr = IDS_EE_FILELOAD_ERROR_GENERIC;
        break;

    }

    return (UINT) hr;
}


//==========================================================================
// Private helper for FileLoadException and FileNotFoundException.
//==========================================================================

FCIMPL1(StringObject*, GetFileLoadExceptionMessage, UINT32 hr)
{
    CONTRACTL
    {
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    STRINGREF refRetVal = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);
    //-[autocvtpro]-------------------------------------------------------

    refRetVal = GetManagedFormatStringForResourceID(GetResourceIDForFileLoadExceptionHR(hr));

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return STRINGREFToObject(refRetVal);
}
FCIMPLEND


//==========================================================================
// Private helper for FileLoadException and FileNotFoundException.
//==========================================================================
FCIMPL1(StringObject*, FileLoadException_GetMessageForHR, UINT32 hresult)
{
    StringObject* rv = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);

    BOOL bNoGeekStuff = FALSE;
    switch (hresult)
    {
        // These errors often come us as a result of cockpit errors rather than app errors - as long
        // as the message is reasonably clear, we can live without the hex code stuff.
        case COR_E_FILENOTFOUND:
        case HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND):
        case HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND):
        case HRESULT_FROM_WIN32(ERROR_INVALID_NAME):
        case HRESULT_FROM_WIN32(ERROR_BAD_NET_NAME):
        case HRESULT_FROM_WIN32(ERROR_BAD_NETPATH):
        case HRESULT_FROM_WIN32(ERROR_DLL_NOT_FOUND):
        case CTL_E_FILENOTFOUND:
        case COR_E_DLLNOTFOUND:
        case COR_E_PATHTOOLONG:
        case E_ACCESSDENIED:
        case COR_E_BADIMAGEFORMAT:
        case COR_E_NEWER_RUNTIME:
        case COR_E_ASSEMBLYEXPECTED:
            bNoGeekStuff = TRUE;
            break;
    }


    SString s;
    GetHRMsg((HRESULT)hresult, s, bNoGeekStuff);
    *((STRINGREF*)&rv) = COMString::NewString(s.GetUnicode());

    HELPER_METHOD_FRAME_END();

    return rv;
}
FCIMPLEND


#define ValidateSigBytes(_size) do { if ((_size) > csig) COMPlusThrow(kArgumentException, L"Argument_BadSigFormat"); csig -= (_size); } while (false)

//==========================================================================
// Unparses an individual type.
//==========================================================================
const BYTE *UnparseType(const BYTE *pType, DWORD& csig, StubLinker *psl)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(ThrowOutOfMemory();); // Emitting data to the StubLinker can throw OOM.
    }
    CONTRACTL_END;

    LPCUTF8 pName = NULL;

    ValidateSigBytes(sizeof(BYTE));
    switch ( (CorElementType) *(pType++) ) {
        case ELEMENT_TYPE_VOID:
            psl->EmitUtf8("void");
            break;

        case ELEMENT_TYPE_BOOLEAN:
            psl->EmitUtf8("boolean");
            break;

        case ELEMENT_TYPE_CHAR:
            psl->EmitUtf8("char");
            break;

        case ELEMENT_TYPE_U1:
            psl->EmitUtf8("unsigned ");
            //fallthru
        case ELEMENT_TYPE_I1:
            psl->EmitUtf8("byte");
            break;

        case ELEMENT_TYPE_U2:
            psl->EmitUtf8("unsigned ");
            //fallthru
        case ELEMENT_TYPE_I2:
            psl->EmitUtf8("short");
            break;

        case ELEMENT_TYPE_U4:
            psl->EmitUtf8("unsigned ");
            //fallthru
        case ELEMENT_TYPE_I4:
            psl->EmitUtf8("int");
            break;

        case ELEMENT_TYPE_I:
            psl->EmitUtf8("native int");
            break;
        case ELEMENT_TYPE_U:
            psl->EmitUtf8("native unsigned");
            break;

        case ELEMENT_TYPE_U8:
            psl->EmitUtf8("unsigned ");
            //fallthru
        case ELEMENT_TYPE_I8:
            psl->EmitUtf8("long");
            break;


        case ELEMENT_TYPE_R4:
            psl->EmitUtf8("float");
            break;

        case ELEMENT_TYPE_R8:
            psl->EmitUtf8("double");
            break;

        case ELEMENT_TYPE_STRING:
            psl->EmitUtf8(g_StringName);
            break;

        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_OBJECT:
            psl->EmitUtf8(g_ObjectName);
            break;

        case ELEMENT_TYPE_PTR:
            pType = UnparseType(pType, csig, psl);
            psl->EmitUtf8("*");
            break;

        case ELEMENT_TYPE_BYREF:
            pType = UnparseType(pType, csig, psl);
            psl->EmitUtf8("&");
            break;

        case ELEMENT_TYPE_VALUETYPE:
        case ELEMENT_TYPE_CLASS:
            pName = (LPCUTF8)pType;
            while (true) {
                ValidateSigBytes(sizeof(CHAR));
                if (*(pType++) == '\0')
                    break;
            }
            psl->EmitUtf8(pName);
            break;

        case ELEMENT_TYPE_SZARRAY:
            {
                pType = UnparseType(pType, csig, psl);
                psl->EmitUtf8("[]");
            }
            break;

        case ELEMENT_TYPE_ARRAY:
            {
                pType = UnparseType(pType, csig, psl);
                ValidateSigBytes(sizeof(DWORD));
                DWORD rank = GET_UNALIGNED_VAL32(pType);
                pType += sizeof(DWORD);
                if (rank)
                {
                    ValidateSigBytes(sizeof(UINT32));
                    UINT32 nsizes = GET_UNALIGNED_VAL32(pType); // Get # of sizes
                    ValidateSigBytes(nsizes * sizeof(UINT32));
                    pType += 4 + nsizes*4;
                    ValidateSigBytes(sizeof(UINT32));
                    UINT32 nlbounds = GET_UNALIGNED_VAL32(pType); // Get # of lower bounds
                    ValidateSigBytes(nlbounds * sizeof(UINT32));
                    pType += 4 + nlbounds*4;


                    while (rank--) {
                        psl->EmitUtf8("[]");
                    }

}

            }
            break;

        case ELEMENT_TYPE_TYPEDBYREF:
            psl->EmitUtf8("&");
            break;

        case ELEMENT_TYPE_FNPTR:
            psl->EmitUtf8("ftnptr");
            break;

        default:
            psl->EmitUtf8("?");
            break;
    }

    return pType;
    }



//==========================================================================
// Helper for MissingMemberException.
//==========================================================================
static STRINGREF MissingMemberException_FormatSignature_Internal(I1ARRAYREF* ppPersistedSig)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END;

    STRINGREF pString = NULL;

    DWORD csig = 0;
    const BYTE *psig = 0;
    StubLinker *psl = NULL;
    StubHolder<Stub> pstub;

    if ((*ppPersistedSig) != NULL)
        csig = (*ppPersistedSig)->GetNumComponents();

    if (csig == 0)
    {
        return COMString::NewString("Unknown signature");
    }

    psig = (const BYTE*)_alloca(csig);
    CopyMemory((BYTE*)psig,
               (*ppPersistedSig)->GetDirectPointerToNonObjectElements(),
               csig);

    StubLinker sl;
    psl = &sl;
    pstub = NULL;

    ValidateSigBytes(sizeof(UINT32));
    UINT32 cconv = GET_UNALIGNED_VAL32(psig);
    psig += 4;

    if (cconv == IMAGE_CEE_CS_CALLCONV_FIELD) {
        psig = UnparseType(psig, csig, psl);
    } else {
        ValidateSigBytes(sizeof(UINT32));
        UINT32 nargs = GET_UNALIGNED_VAL32(psig);
        psig += 4;

        // Unparse return type
        psig = UnparseType(psig, csig, psl);
        psl->EmitUtf8("(");
        while (nargs--) {
            psig = UnparseType(psig, csig, psl);
            if (nargs) {
                psl->EmitUtf8(", ");
            }
        }
        psl->EmitUtf8(")");
    }
    psl->Emit8('\0');
    pstub = psl->Link();
    pString = COMString::NewString( (LPCUTF8)(pstub->GetEntryPoint()) );

    return pString;
}

FCIMPL1(Object*, MissingMemberException_FormatSignature, I1Array* pPersistedSigUNSAFE)
{
    CONTRACTL
    {
        SO_TOLERANT;
        DISABLED(GC_TRIGGERS);
        THROWS;
        MODE_COOPERATIVE;
    }
    CONTRACTL_END;

    STRINGREF pString = NULL;
    I1ARRAYREF pPersistedSig = (I1ARRAYREF) pPersistedSigUNSAFE;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_1(Frame::FRAME_ATTR_RETURNOBJ, pPersistedSig);
    //-[autocvtpro]-------------------------------------------------------

    pString = MissingMemberException_FormatSignature_Internal(&pPersistedSig);

    //-[autocvtepi]-------------------------------------------------------
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(pString);
    }
FCIMPLEND

// Check if the Win32 Error code is an IO error.
BOOL IsWin32IOError(SCODE scode)
{
    LEAF_CONTRACT;

    switch (scode)
    {
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_TOO_MANY_OPEN_FILES:
    case ERROR_ACCESS_DENIED:
    case ERROR_INVALID_HANDLE:
    case ERROR_INVALID_DRIVE:
    case ERROR_WRITE_PROTECT:
    case ERROR_NOT_READY:
    case ERROR_WRITE_FAULT:
    case ERROR_SHARING_VIOLATION:
    case ERROR_LOCK_VIOLATION:
    case ERROR_SHARING_BUFFER_EXCEEDED:
    case ERROR_HANDLE_DISK_FULL:
    case ERROR_BAD_NETPATH:
    case ERROR_DEV_NOT_EXIST:
    case ERROR_FILE_EXISTS:
    case ERROR_CANNOT_MAKE:
    case ERROR_NET_WRITE_FAULT:
    case ERROR_DRIVE_LOCKED:
    case ERROR_OPEN_FAILED:
    case ERROR_BUFFER_OVERFLOW:
    case ERROR_DISK_FULL:
    case ERROR_INVALID_NAME:
    case ERROR_FILENAME_EXCED_RANGE:
    case ERROR_IO_DEVICE:
    case ERROR_DISK_OPERATION_FAILED:
        return TRUE;

    default:
        return FALSE;
    }
}


// Check if there is a pending exception or the thread is already aborting. Returns 0 if yes.
// Otherwise, sets the thread up for generating an abort and returns address of ThrowControlForThread
LPVOID __fastcall COMPlusCheckForAbort(LPVOID retAddress, LPVOID esp, LPVOID ebp)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    Thread* pThread = GetThread();

    if ((!pThread->IsAbortRequested()) ||         // if no abort has been requested
        (!pThread->IsRudeAbort() &&
        (pThread->GetThrowable() != NULL)) )  // or if there is a pending exception
    {
        return 0;
    }

    // else we must produce an abort
    if ((pThread->GetThrowable() == NULL) &&
        (pThread->IsAbortInitiated()))
    {
        // Oops, we just swallowed an abort, must restart the process
        pThread->ResetAbortInitiated();
    }

    // Question: Should we also check for (pThread->m_PreventAsync == 0)

    SetIP(pThread->m_OSContext, retAddress);
    SetSP(pThread->m_OSContext, esp);
    SetFP(pThread->m_OSContext, ebp);   // this indicates that when we reach ThrowControlForThread, ebp will already be correct
    pThread->SetThrowControlForThread(Thread::InducedThreadStop);
    return (LPVOID) GetEEFuncEntryPoint(THROW_CONTROL_FOR_THREAD_FUNCTION);

}


BOOL IsThreadHijackedForThreadStop(Thread* pThread, EXCEPTION_RECORD* pExceptionRecord)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    if (pExceptionRecord->ExceptionCode == EXCEPTION_COMPLUS)
    {
        if (pThread->ThrewControlForThread() == Thread::InducedThreadStop)
        {
            LOG((LF_EH, LL_INFO100, "Asynchronous Thread Stop or Abort\n"));
            return TRUE;
        }
    }
    return FALSE;
}


// We sometimes move a thread's execution so it will throw an exception for us.
// But then we have to treat the exception as if it came from the instruction
// the thread was originally running.
//
// NOTE: This code depends on the fact that there are no register-based data dependencies
// between a try block and a catch, fault, or finally block.  If there were, then we need
// to preserve more of the register context.

void AdjustContextForThreadStop(Thread* pThread,
                                CONTEXT* pContext)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;

    _ASSERTE(pThread->m_OSContext);

    SetIP(pContext, GetIP(pThread->m_OSContext));
    SetSP(pContext, GetSP(pThread->m_OSContext));

    if (GetFP(pThread->m_OSContext) != 0)  // ebp = 0 implies that we got here with the right values for ebp
    {
        SetFP(pContext, GetFP(pThread->m_OSContext));
    }

    // We might have been interrupted execution at a point where the jit has roots in
    // registers.  We just need to store a "safe" value in here so that the collector
    // doesn't trap.  We're not going to use these objects after the exception.
    //
    // Only callee saved registers are going to be reported by the faulting excepiton frame.
#if defined(_X86_)
    // Ebx,esi,edi are important.  Eax,ecx,edx are not.
    pContext->Ebx = 0;
    pContext->Edi = 0;
    pContext->Esi = 0;
#elif defined(_PPC_)
    ZeroMemory(&pContext->Gpr13, sizeof(ULONG) * NUM_CALLEESAVED_REGISTERS);
#else
    PORTABILITY_ASSERT("AdjustContextForThreadStop");
#endif


    pThread->ResetThrowControlForThread();

    // Should never get here if we're already throwing an exception.
    _ASSERTE(!pThread->IsExceptionInProgress() || pThread->IsRudeAbort());

    // Should never get here if we're already abort initiated.
    _ASSERTE(!pThread->IsAbortInitiated() || pThread->IsRudeAbort());

    if (pThread->IsAbortRequested())
    {
        pThread->SetAbortInitiated();    // to prevent duplicate aborts
    }
}


// Create a COM+ exception , stick it in the thread.
OBJECTREF
CreateCOMPlusExceptionObject(Thread *pThread, EXCEPTION_RECORD *pExceptionRecord, BOOL bAsynchronousThreadStop)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(GetThread() == pThread);

    DWORD exceptionCode = pExceptionRecord->ExceptionCode;
    OBJECTREF result = 0;

    DWORD COMPlusExceptionCode = (bAsynchronousThreadStop
                                    ? kThreadAbortException
                                    : MapWin32FaultToCOMPlusException(pExceptionRecord));

    if (exceptionCode == STATUS_NO_MEMORY)
    {
        result = CLRException::GetBestOutOfMemoryException();
    }
    else if (IsStackOverflowException(pThread, pExceptionRecord))
    {
        result = CLRException::GetPreallocatedStackOverflowException();
    }
    else if (bAsynchronousThreadStop && pThread->IsAbortRequested() && pThread->IsRudeAbort())
    {
        result = CLRException::GetPreallocatedRudeThreadAbortException();
    }
    else
    {
        EX_TRY
        {
            // We need to disable the backout stack validation at this point since CreateThrowable can
            // take arbitrarily large amounts of stack for different exception types; however we know
            // for a fact that we will never go through this code path if the exception is a stack
            // overflow exception since we already handled that case above with the pre-allocated SO exception.
            DISABLE_BACKOUT_STACK_VALIDATION;

            FAULT_NOT_FATAL();

            ThreadPreventAsyncHolder preventAsync(TRUE);
            ResetProcessorStateHolder procState;

            INSTALL_UNWIND_AND_CONTINUE_HANDLER;

            GCPROTECT_BEGIN(result)

            EEException e((RuntimeExceptionKind)COMPlusExceptionCode);
            result = e.CreateThrowable();

            // EEException is "one size fits all".  But AV needs some more information.
            if (COMPlusExceptionCode == kAccessViolationException)
            {
                SetExceptionAVParameters(result, pExceptionRecord);
            }

            GCPROTECT_END();

            UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;
        }
        EX_CATCH
        {
            // If we get an exception trying to build the managed exception object, then go ahead and return the
            // thrown object as the result of this function. This is preferable to letting the exception try to
            // percolate up through the EH code, and it effectively replaces the thrown exception with this
            // exception.
            result = GET_THROWABLE();
        }
        EX_END_CATCH(SwallowAllExceptions);
    }

    return result;
}

LONG FilterAccessViolation(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;


    if (pExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_EXECUTE_HANDLER;

    return EXCEPTION_CONTINUE_SEARCH;
}

/*
 * IsContinuableException
 *
 * Returns whether this is an exception the EE knows how to intercept and continue from.
 *
 * Parameters:
 *   pThread - The thread the exception occured on.
 *
 * Returns:
 *   TRUE if the exception on the thread is interceptable or not.
 *
 */
bool IsInterceptableException(Thread *pThread)
{
    CONTRACTL
    {
        MODE_ANY;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    return ((pThread != NULL)                       &&
            (!pThread->IsAbortRequested())          &&
            (pThread->IsExceptionInProgress())      &&
            (!pThread->IsThrowableNull())

#ifdef DEBUGGING_SUPPORTED
            &&
            pThread->GetExceptionState()->IsDebuggerInterceptable()
#endif

            );
}



// Return true if the access violation is well formed (has two info parameters
// at the end)
static inline BOOL
IsWellFormedAV(EXCEPTION_RECORD *pExceptionRecord)
{
    LEAF_CONTRACT;

    #define NUM_AV_PARAMS 2

    if (pExceptionRecord->NumberParameters == NUM_AV_PARAMS)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// Some page faults are handled by the GC.
static inline BOOL
IsGcFault(EXCEPTION_RECORD* pExceptionRecord)
{
    WRAPPER_CONTRACT;

    //get the fault address and hand it to GC.
    void* f_address = (void*)pExceptionRecord->ExceptionInformation [1];

    BOOL retval = FALSE;

    if ( GCHeap::GetGCHeap()->HandlePageFault (f_address) )
    {
        retval = TRUE;
    }

    return retval;
}

// Some page faults are handled by perf monitors.
static inline BOOL
IsMonitorFault(EXCEPTION_RECORD* pExceptionRecord, CONTEXT* pContext)
{
    WRAPPER_CONTRACT;

    return COMPlusIsMonitorException(pExceptionRecord, pContext);
}

static inline BOOL
IsDebuggerFault(EXCEPTION_RECORD *pExceptionRecord,
                CONTEXT *pContext,
                DWORD exceptionCode,
                Thread *pThread)
{
#ifdef DEBUGGING_SUPPORTED
    WRAPPER_CONTRACT;

    SO_NOT_MAINLINE_FUNCTION;


    if ((exceptionCode != EXCEPTION_COMPLUS) &&
        (GetThread() != NULL) &&
        (g_pDebugInterface != NULL) &&
        g_pDebugInterface->FirstChanceNativeException(pExceptionRecord,
                                                      pContext,
                                                      exceptionCode,
                                                      pThread))
    {
        LOG((LF_EH | LF_CORDB, LL_INFO1000, "IsDebuggerFault - it's the debugger's fault\n"));
        return true;
    }
#else
    LEAF_CONTRACT;
#endif // DEBUGGING_SUPPORTED
    return false;
}


// Returns TRUE if caller should resume execution.
BOOL
AdjustContextForWriteBarrier(
        EXCEPTION_RECORD *pExceptionRecord,
        CONTEXT *pContext)
{
    WRAPPER_CONTRACT;

#ifdef _X86_

    void* f_IP = GetIP(pContext);

    if (f_IP >= (void *) JIT_WriteBarrierStart && f_IP <= (void *) JIT_WriteBarrierLast ||
        f_IP >= (void *) JIT_WriteBarrier_Buf_Start && f_IP <= (void *) JIT_WriteBarrier_Buf_Last)
    {
        // set the exception IP to be the instruction that called the write barrier
        void* callsite = GetAdjustedCallAddress(GetSP(pContext));
        pExceptionRecord->ExceptionAddress = callsite;
        SetIP(pContext, callsite);

        // put ESP back to what it was before the call.
        SetSP(pContext, (BYTE*)GetSP(pContext) + sizeof(void*));
    }

    return FALSE;

#else

    PORTABILITY_WARNING("AdjustContextForWriteBarrier() not implemented on this platform");
    return FALSE;

#endif
}

// Returns TRUE if caller should resume execution.
BOOL
AdjustContextForVirtualStub(
        EXCEPTION_RECORD *pExceptionRecord,
        CONTEXT *pContext)
{
    WRAPPER_CONTRACT;

    Thread * pThread = GetThread();

    // We may not have a managed thread object. Example is an AV on the helper thread.
    // (perhaps during StubManager::IsStub)
    if (pThread == NULL)
    {
        return false;
    }

#if defined(_X86_)

    TADDR f_IP = (TADDR)GetIP(pContext);

    VirtualCallStubManager::StubKind sk;
    VirtualCallStubManager *pMgr = VirtualCallStubManager::FindStubManager(f_IP, &sk);

    if (sk == VirtualCallStubManager::SK_DISPATCH)
    {
        if (*PTR_WORD(f_IP) != X86_INSTR_CMP_IND_ECX_IMM32)
        {
            _ASSERTE(!"AV in DispatchStub at unknown instruction");
            return FALSE;
        }
    }
    else
    if (sk == VirtualCallStubManager::SK_RESOLVE)
    {
        if (*PTR_WORD(f_IP) != X86_INSTR_MOV_EAX_ECX_IND)
        {
            _ASSERTE(!"AV in ResolveStub at unknown instruction");
            return FALSE;
        }

        SetSP(pContext, (BYTE*)GetSP(pContext) + sizeof(void*)); // rollback push eax
    }
    else
    {
        return FALSE;
    }

    void* callsite = GetAdjustedCallAddress(GetSP(pContext));
    pExceptionRecord->ExceptionAddress = callsite;
    SetIP(pContext, callsite);

    // put ESP back to what it was before the call.
    SetSP(pContext, (BYTE*)GetSP(pContext) + sizeof(void*));

    return TRUE;

#else

    PORTABILITY_WARNING("AdjustContextForVirtualStub() not implemented on this platform");
    return FALSE;

#endif
}


struct SavedExceptionInfo
{
    EXCEPTION_RECORD m_ExceptionRecord;
    CONTEXT m_ExceptionContext;
    CrstStatic m_Crst;

    void SaveExceptionRecord(EXCEPTION_RECORD *pExceptionRecord)
    {
        LEAF_CONTRACT;
        size_t erSize = offsetof(EXCEPTION_RECORD, ExceptionInformation[pExceptionRecord->NumberParameters]);
        memcpy(&m_ExceptionRecord, pExceptionRecord, erSize);

    }

    void SaveContext(CONTEXT *pContext)
    {
        LEAF_CONTRACT;
#ifdef CONTEXT_EXTENDED_REGISTERS

        size_t contextSize = offsetof(CONTEXT, ExtendedRegisters);
        if ((pContext->ContextFlags & CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS)
            contextSize += sizeof(pContext->ExtendedRegisters);
        memcpy(&m_ExceptionContext, pContext, contextSize);

#else // !CONTEXT_EXTENDED_REGISTERS

        size_t contextSize = sizeof(CONTEXT);
        memcpy(&m_ExceptionContext, pContext, contextSize);

#endif // !CONTEXT_EXTENDED_REGISTERS
    }

    void Enter()
    {
        WRAPPER_CONTRACT;
        m_Crst.Enter();
    }

    void Leave()
    {
        WRAPPER_CONTRACT;
        m_Crst.Leave();
    }

    void Init()
    {
        WRAPPER_CONTRACT;
        m_Crst.Init("SavedExceptionInfo lock", CrstDummy, CRST_UNSAFE_ANYMODE);
    }
};

SavedExceptionInfo g_SavedExceptionInfo;  // Globals are guaranteed zero-init;

void InitSavedExceptionInfo()
{
    g_SavedExceptionInfo.Init();
}

EXTERN_C VOID FixContextForFaultingExceptionFrame (
        EXCEPTION_RECORD* pExceptionRecord,
        CONTEXT *pContextRecord)
{
    WRAPPER_CONTRACT;
#if defined(USE_FEF)

    // isn't this already correct and the memcpy is extraneous?

    // don't copy parm args as have already supplied them on the throw
    memcpy((void*) pExceptionRecord,
           (void*) &g_SavedExceptionInfo.m_ExceptionRecord,
           offsetof(EXCEPTION_RECORD, ExceptionInformation)
          );

    ReplaceExceptionContextRecord(pContextRecord, &g_SavedExceptionInfo.m_ExceptionContext);

    g_SavedExceptionInfo.Leave();

    GetThread()->ResetThreadStateNC(Thread::TSNC_DebuggerIsManagedException);
#endif // USE_FEF
}


#if defined(USE_FEF)    // the function can not appear if it is never referenced -- rotor build error
static LONG LinkFrameAndThrowFilter(EXCEPTION_POINTERS* ep, LPVOID pv)
{
    WRAPPER_CONTRACT;
    if (++(*(int*)pv) == 1)
        FixContextForFaultingExceptionFrame(ep->ExceptionRecord, ep->ContextRecord);

    return EXCEPTION_CONTINUE_SEARCH;
}
#endif // USE_FEF


EXTERN_C VOID __fastcall
LinkFrameAndThrow(FaultingExceptionFrame* pFrame)
{
    WRAPPER_CONTRACT;
#if defined(USE_FEF)

    // It's possible for our filter to be called more than once if some other first-pass
    // handler lets an exception out.  We need to make sure we only fix the context for
    // the first exception we see.  Filter_count takes care of that.
    int filter_count = 0;

    *(TADDR*)pFrame = FaultingExceptionFrame::GetMethodFrameVPtr();
    *pFrame->GetGSCookiePtr() = GetProcessGSCookie();

    pFrame->Push();

    ULONG       argcount = g_SavedExceptionInfo.m_ExceptionRecord.NumberParameters;
    ULONG       flags    = g_SavedExceptionInfo.m_ExceptionRecord.ExceptionFlags;
    ULONG       code     = g_SavedExceptionInfo.m_ExceptionRecord.ExceptionCode;
    ULONG_PTR*  args     = &g_SavedExceptionInfo.m_ExceptionRecord.ExceptionInformation[0];

    GetThread()->SetThreadStateNC(Thread::TSNC_DebuggerIsManagedException);

    PAL_CPP_EHUNWIND_BEGIN;

    PAL_TRY
    {
        RaiseException(code, flags, argcount, args);
    }
    PAL_EXCEPT_FILTER(LinkFrameAndThrowFilter, &filter_count)
    {
    }
    PAL_ENDTRY

    PAL_CPP_EHUNWIND_END;
#endif // USE_FEF
}

//
// Init a new frame
//
void FaultingExceptionFrame::Init(CONTEXT *pContext)
{
    WRAPPER_CONTRACT;
#if defined(_X86_)
    CalleeSavedRegisters *pRegs = GetCalleeSavedRegisters();
    pRegs->ebp = pContext->Ebp;
    pRegs->ebx = pContext->Ebx;
    pRegs->esi = pContext->Esi;
    pRegs->edi = pContext->Edi;
    m_ReturnAddress = (TADDR)::GetIP(pContext);
    m_Esp = (DWORD)(size_t)GetSP(pContext);
#elif defined(_PPC_)
    CalleeSavedRegisters *pRegs = GetCalleeSavedRegisters();
    pRegs->cr = pContext->Cr;
    memcpy(pRegs->r, &pContext->Gpr13, NUM_CALLEESAVED_REGISTERS * sizeof(INT32));
    memcpy(pRegs->f, &pContext->Fpr14, NUM_FLOAT_CALLEESAVED_REGISTERS * sizeof(DOUBLE));

    // InitAndLink is called in a state where m_Link doesn't coincide with the linkage area on stack
    // 0 is a safe value for the return address in this case
    m_ReturnAddress = 0;
#else
    PORTABILITY_ASSERT("FaultingExceptionFrame::InitAndLink");
#endif
}

//
// Init and Link in a new frame
//
void FaultingExceptionFrame::InitAndLink(CONTEXT *pContext)
{
    WRAPPER_CONTRACT;

    Init(pContext);

    Push();
}


void SetNakedThrowHelperArgRegistersInContext(CONTEXT* pContext,
                                              EXCEPTION_RECORD* pExceptionRecord
                                              )
{
    // Lock will be released by the throw helper.
#ifdef _X86_
    pContext->Ecx = (DWORD)(size_t)GetIP(pContext); // ECX gets original IP.
#elif _PPC_
    pContext->Gpr0 = (DWORD)(size_t)GetIP(pContext);
#else
    PORTABILITY_WARNING("NakedThrowHelper argument not defined");
#endif
}


EXTERN_C VOID __stdcall NakedThrowHelper(VOID);

bool ShouldHandleManagedFault(
                        EXCEPTION_RECORD*               pExceptionRecord,
                        CONTEXT*                        pContext,
                        EXCEPTION_REGISTRATION_RECORD*  pEstablisherFrame,
                        Thread*                         pThread)
{
    WRAPPER_CONTRACT;

    //
    //


    DWORD exceptionCode = pExceptionRecord->ExceptionCode;
    if (   exceptionCode == EXCEPTION_COMPLUS
        || exceptionCode == STATUS_BREAKPOINT
        || exceptionCode == STATUS_SINGLE_STEP)
    {
        return false;
    }

#ifdef _DEBUG
    if (exceptionCode == STATUS_CLR_GCCOVER_CODE)
        return false;
#endif // _DEBUG

    // If there's any frame below the ESP of the exception, then we can forget it.
    if (pThread->m_pFrame < GetSP(pContext))
        return false;

    // If we're a subsequent handler forget it.
    EXCEPTION_REGISTRATION_RECORD* pBottomMostHandler = pThread->GetExceptionState()->m_currentExInfo.m_pBottomMostHandler;
    if (pBottomMostHandler != NULL && pEstablisherFrame > pBottomMostHandler)
    {
        return false;
    }

    // If it's not a fault in jitted code, forget it.
    ICodeManager* pCodeMan = ExecutionManager::FindCodeMan((SLOT)GetIP(pContext));
    if (!pCodeMan)
        return false;

    // caller should call HandleManagedFault and resume execution.
    return true;
}


void HandleManagedFault(EXCEPTION_RECORD*               pExceptionRecord,
                        CONTEXT*                        pContext,
                        EXCEPTION_REGISTRATION_RECORD*  pEstablisherFrame,
                        Thread*                         pThread)
{
    WRAPPER_CONTRACT;

    // Ok.  Now we have a brand new fault in jitted code.
    g_SavedExceptionInfo.Enter();
    g_SavedExceptionInfo.SaveExceptionRecord(pExceptionRecord);
    g_SavedExceptionInfo.SaveContext(pContext);

    SetNakedThrowHelperArgRegistersInContext(pContext, pExceptionRecord);
    SetIP(pContext, GetEEFuncEntryPoint(NakedThrowHelper));
}

LONG WINAPI CLRVectoredExceptionHandlerPhase2(PEXCEPTION_POINTERS pExceptionInfo);

typedef enum VEH_ACTION
{
    VEH_NO_ACTION = 0,
    VEH_EXECUTE_HANDLE_MANAGED_EXCEPTION,
    VEH_CONTINUE_EXECUTION,
    VEH_CONTINUE_SEARCH,
    VEH_EXECUTE_HANDLER
};


VEH_ACTION WINAPI CLRVectoredExceptionHandlerPhase3(PEXCEPTION_POINTERS pExceptionInfo);

LONG WINAPI CLRVectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    // It is not safe to execute code inside VM after we shutdown EE.  One example is DiablePreemptiveGC
    // will block forever.
    if (g_fForbidEnterEE)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    //
    // DO NOT USE CONTRACTS HERE AS THIS ROUTINE MAY NEVER RETURN.  You can use
    // static contracts, but currently this is all WRAPPER_CONTRACT.
    //


    //
    //        READ THIS!
    //
    //
    // You cannot put any code in here that allocates during an out-of-memory handling.
    // This routine runs before *any* other handlers, including __try.  Thus, if you
    // allocate anything in this routine then it will throw out-of-memory and end up
    // right back here.
    //
    // There are various things that allocate that you may not expect to allocate.  One
    // instance of this is STRESS_LOG.  It allocates the log buffer if the thread does
    // not already have one allocated.  Thus, if we OOM during the setting up of the
    // thread, the log buffer will not be allocated and this will try to do so.  Thus,
    // all STRESS_LOGs in here need to be after you have guaranteed the allocation has
    // already occured.
    //


    return CLRVectoredExceptionHandlerPhase2(pExceptionInfo);
}


LONG WINAPI CLRVectoredExceptionHandlerPhase2(PEXCEPTION_POINTERS pExceptionInfo)
{
    //
    // DO NOT USE CONTRACTS HERE AS THIS ROUTINE MAY NEVER RETURN.  You can use
    // static contracts, but currently this is all WRAPPER_CONTRACT.
    //

    //
    //        READ THIS!
    //
    //
    // You cannot put any code in here that allocates during an out-of-memory handling.
    // This routine runs before *any* other handlers, including __try.  Thus, if you
    // allocate anything in this routine then it will throw out-of-memory and end up
    // right back here.
    //
    // There are various things that allocate that you may not expect to allocate.  One
    // instance of this is STRESS_LOG.  It allocates the log buffer if the thread does
    // not already have one allocated.  Thus, if we OOM during the setting up of the
    // thread, the log buffer will not be allocated and this will try to do so.  Thus,
    // all STRESS_LOGs in here need to be after you have guaranteed the allocation has
    // already occured.
    //

    PEXCEPTION_RECORD pExceptionRecord  = pExceptionInfo->ExceptionRecord;
    VEH_ACTION action;

    {
        MAYBE_FAULT_FORBID_NO_ALLOC((pExceptionRecord->ExceptionCode == STATUS_NO_MEMORY));
        CantAllocHolder caHolder;

        action = CLRVectoredExceptionHandlerPhase3(pExceptionInfo);
    }

    if (action == VEH_CONTINUE_EXECUTION)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    if (action == VEH_CONTINUE_SEARCH)
        {
        return EXCEPTION_CONTINUE_SEARCH;
        }

    if (action == VEH_EXECUTE_HANDLER)
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }



    //
    // In OOM situations, this call better not fault.
    //
    {
        MAYBE_FAULT_FORBID_NO_ALLOC((pExceptionRecord->ExceptionCode == STATUS_NO_MEMORY));
        CantAllocHolder caHolder;

        //
        if (IsDebuggerFault(pExceptionRecord, pExceptionInfo->ContextRecord, pExceptionRecord->ExceptionCode, GetThread()))
        {
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

    //

    if ((pExceptionRecord->ExceptionCode == STATUS_BREAKPOINT) ||
        (pExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP))
    {

        // The breakpoint was from managed or the runtime.  Handle it.  Or,
        //  this may be a Rotor build.
        return UserBreakpointFilter(pExceptionInfo);
    }


    return EXCEPTION_EXECUTE_HANDLER;
}

/*
 * CLRVectoredExceptionHandlerPhase3
 *
 * This routine does some basic processing on the exception, making decisions about common
 * exception types and whether to continue them or not.  It has side-effects, in that it may
 * adjust the context in the exception.
 *
 * Parameters:
 *    pExceptionInfo - pointer to the exception
 *
 * Returns:
 *    VEH_NO_ACTION - This indicates that Phase3 has no specific action to take and that further
 *       processing of this exception should continue.
 *    VEH_EXECUTE_HANDLE_MANAGED_EXCEPTION - This indicates that the caller should call HandleMandagedException
 *       immediately.
 *    VEH_CONTINUE_EXECUTION - Caller should return EXCEPTION_CONTINUE_EXECUTION.
 *    VEH_CONTINUE_SEARCH - Caller should return EXCEPTION_CONTINUE_SEARCH;
 *    VEH_EXECUTE_HANDLER - Caller should return EXCEPTION_EXECUTE_HANDLER.
 *
 *   Note that in all cases the context in the exception may have been adjusted.
 *
 */

VEH_ACTION WINAPI CLRVectoredExceptionHandlerPhase3(PEXCEPTION_POINTERS pExceptionInfo)
{
    //
    // DO NOT USE CONTRACTS HERE AS THIS ROUTINE MAY NEVER RETURN.  You can use
    // static contracts, but currently this is all WRAPPER_CONTRACT.
    //

    //
    //        READ THIS!
    //
    //
    // You cannot put any code in here that allocates during an out-of-memory handling.
    // This routine runs before *any* other handlers, including __try.  Thus, if you
    // allocate anything in this routine then it will throw out-of-memory and end up
    // right back here.
    //
    // There are various things that allocate that you may not expect to allocate.  One
    // instance of this is STRESS_LOG.  It allocates the log buffer if the thread does
    // not already have one allocated.  Thus, if we OOM during the setting up of the
    // thread, the log buffer will not be allocated and this will try to do so.  Thus,
    // all STRESS_LOGs in here need to be after you have guaranteed the allocation has
    // already occured.
    //

    // Handle special cases which are common amongst all filters.
    PEXCEPTION_RECORD pExceptionRecord  = pExceptionInfo->ExceptionRecord;
    PCONTEXT          pContext          = pExceptionInfo->ContextRecord;
    DWORD             exceptionCode     = pExceptionRecord->ExceptionCode;


        GCX_NOTRIGGER();


        if (IsInstrModifyFault(pExceptionInfo))
        {
        return VEH_CONTINUE_EXECUTION;;
        }



        // Some other parts of the EE use exceptions in their own nefarious ways.  We do some up-front processing
        // here to fix up the exception if needed.
        if (exceptionCode == STATUS_ACCESS_VIOLATION)
        {
            if (IsWellFormedAV(pExceptionRecord))
            {
                if (GCHeap::IsGCHeapInitialized () && IsGcFault(pExceptionRecord))
                {
                return VEH_CONTINUE_EXECUTION;;
                }

                if (AdjustContextForWriteBarrier(pExceptionRecord, pContext))
                {
                    // On x86, AdjustContextForWriteBarrier simply backs up AV's
                    // in write barrier helpers into the calling frame, so that
                    // the subsequent logic here sees a managed fault.
                    //
                // On 64-bit, some additional work is required..
                }
            else if (AdjustContextForVirtualStub(pExceptionRecord, pContext))
                {
                }

            }
        }
    else if (exceptionCode == BOOTUP_EXCEPTION_COMPLUS)
        {
            // Don't handle a boot exception
        return VEH_CONTINUE_SEARCH;
        }

    return VEH_NO_ACTION;
}

#define DBG_PRINTEXCEPTION_C             ((DWORD) 0x40010006L)
#define EXCEPTION_VISUALCPP_DEBUGGER        ((DWORD) (1<<30 | 0x6D<<16 | 5000))


LONG WINAPI CLRVectoredExceptionHandlerShim(PEXCEPTION_POINTERS pExceptionInfo)
{
    //
    // HandleManagedFault will take a Crst that causes an unbalanced
    // notrigger scope, and this contract will whack the thread's
    // ClrDebugState to what it was on entry in the dtor, which causes
    // us to assert when we finally release the Crst later on.
    //
//    CONTRACTL
//    {
//        NOTHROW;
//        GC_NOTRIGGER;
//        MODE_ANY;
//    }
//    CONTRACTL_END;

    //
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    //
    // o This function should not call functions that acquire
    //   synchronization objects or allocate memory, because this
    //   can cause problems.  <-- quoteth MSDN  -- probably for
    //   the same reason as we cannot use LOG(); we'll recurse
    //   into a stack overflow.
    //
    // o You cannot use LOG() in here because that will trigger an
    //   exception which will cause infinite recursion with this
    //   function.  We work around this by ignoring all non-error
    //   exception codes, which serves as the base of the recursion.
    //   That way, we can LOG() from the rest of the function
    //
    // The same goes for any function called by this
    // function.
    //
    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING
    //
    DWORD dwCode = pExceptionInfo->ExceptionRecord->ExceptionCode;
    if (dwCode == DBG_PRINTEXCEPTION_C || dwCode == EXCEPTION_VISUALCPP_DEBUGGER)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // We must preserve this so that GCStress=4 eh processing doesnt kill last error.
    // Note that even GetThread below can affect the LastError.
    DWORD dwLastError = GetLastError();

    LONG result = EXCEPTION_CONTINUE_SEARCH;

    // If we cannot obtain a Thread object, then we have no business processing any
    // exceptions on this thread.  Indeed, even checking to see if the faulting
    // address is in JITted code is problematic if we have no Thread object, since
    // this thread will bypass all our locks.
    Thread *pThread = GetThread();

    if (pThread)
    {
        result = CLRVectoredExceptionHandler(pExceptionInfo);
        if (EXCEPTION_EXECUTE_HANDLER == result)
        {
            result = EXCEPTION_CONTINUE_SEARCH;
        }

        Frame* pNewBoundary = (Frame*)GetSP(pExceptionInfo->ContextRecord);
        pThread->GetExceptionState()->m_currentExInfo.m_pSearchBoundary = pNewBoundary;
        STRESS_LOG1(LF_EH, LL_INFO1000, "CLRVectoredExceptionHandlerShim: returning %d\n", result);

    }

    SetLastError(dwLastError);

    return result;
}

//
LONG WINAPI CLRVectoredContinueHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
    // It is not safe to execute code inside VM after we shutdown EE.  One example is DiablePreemptiveGC
    // will block forever.
    if (!g_fForbidEnterEE)
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    PEXCEPTION_RECORD pExceptionRecord  = pExceptionInfo->ExceptionRecord;
    PCONTEXT          pContext          = pExceptionInfo->ContextRecord;
    DWORD             exceptionCode     = pExceptionRecord->ExceptionCode;

    //
    //        READ THIS!
    //
    //
    // You cannot put any code in here that allocates during an out-of-memory handling.
    // This routine runs before *any* other handlers, including __try.  Thus, if you
    // allocate anything in this routine then it will throw out-of-memory and end up
    // right back here.
    //
    // There are various things that allocate that you may not expect to allocate.  One
    // instance of this is STRESS_LOG.  It allocates the log buffer if the thread does
    // not already have one allocated.  Thus, if we OOM during the setting up of the
    // thread, the log buffer will not be allocated and this will try to do so.  Thus,
    // all STRESS_LOGs in here need to be after you have guaranteed the allocation has
    // already occured.
    //

    if (exceptionCode == DBG_PRINTEXCEPTION_C || exceptionCode == EXCEPTION_VISUALCPP_DEBUGGER ||
        (pExceptionRecord->ExceptionFlags & EXCEPTION_STACK_INVALID))
    {
    // Note: you cannot use LOG() in here because that will trigger an exception which will cause infinite
    //  recursion with this function. We work around this by ignoring the exception that OutputDebugString uses,
    //  which serves as the base of the recursion. That way, we can LOG() from the rest of the function.
    // If the EXCEPTION_STACK_INVALID bit is set, we can not trust the stack.  Don't do anything.
        return EXCEPTION_CONTINUE_SEARCH;
    }

    MAYBE_FAULT_FORBID_NO_ALLOC((pExceptionRecord->ExceptionCode == STATUS_NO_MEMORY));

    // We must preserve this so that GCStress=4 eh processing doesnt kill last error.
    DWORD dwLastError = GetLastError();

    WRAPPER_CONTRACT;

    Thread *pThread = NULL;

    // First things first: only do this work if we're on a managed thread.
    pThread = GetThread();

    if (pThread != NULL)
    {
        CantAllocHolder caHolder;
        STRESS_LOG4(LF_EH, LL_INFO100, "In CLRVectoredContinueHandler, Exception = %x, Context = %p, IP = %p SP = %p\n",
                    exceptionCode, pContext, GetIP(pContext), GetSP(pContext));

        STRESS_LOG1(LF_EH, LL_INFO1000000, "CLRVectoredContinueHandler: SP at original fault was 0x%p\n", GetSP(pContext));

        // Since we're continuing from an exception, we take this opprotunity to make sure that we've removed any
        // FS:0 and Frame entries that are below the original ESP that the exception occured at. Do the Frames
        // first.
        {
            // NOTE: we can't use GCX_COOP() here... that's an object on the stack, which means C++ EH to remove
            // it, and we're going to mess with the FS:0 chain on x86 right below here...
            BOOL disabled = pThread->PreemptiveGCDisabled();

            if (!disabled)
            {
                pThread->DisablePreemptiveGC();
            }

            Frame *pFrame = pThread->GetFrame();

            while ((pFrame != FRAME_TOP) && (pFrame < GetSP(pContext)))
            {

                pFrame->Pop();
                pFrame = pThread->GetFrame();
            }

            if (pFrame != pThread->GetFrame())
            {
                STRESS_LOG2(LF_EH, LL_INFO1000000, "CLRVectoredContinueHandler: update Frame from 0x%p to 0x%p\n",
                     pThread->GetFrame(), pFrame);

                pThread->SetFrame(pFrame);
            }

            if (!disabled)
            {
                pThread->EnablePreemptiveGC();
            }
        }

#ifdef _X86_
        // Next, fixup the FS:0 chain.
        PEXCEPTION_REGISTRATION_RECORD pEHR = GetCurrentSEHRecord();

        while ((pEHR != EXCEPTION_CHAIN_END) && ((PVOID)pEHR < GetSP(pContext)))
        {

            pEHR = pEHR->Next;
        }

        if (pEHR != GetCurrentSEHRecord())
        {
            STRESS_LOG2(LF_EH, LL_INFO1000000, "CLRVectoredContinueHandler: update FS:0 from 0x%p to 0x%p\n",
                 GetCurrentSEHRecord(), pEHR);

            SetCurrentSEHRecord(pEHR);
        }
#endif
    }

    // Restore the last error setting.
    SetLastError(dwLastError);

    // No matter what, return with EXCEPTION_CONTINUE_SEARCH so that other vectored continue handlers get a chance.
    return EXCEPTION_CONTINUE_SEARCH;
}


void CLRAddVectoredHandlers(void)
{
}

//======================================================
// HRESULT ComCallExceptionCleanup()
//  Cleanup for calls from Com to COM+
//
HRESULT ComCallExceptionCleanup()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    WRAPPER_CONTRACT;
    Thread* pThread = GetThread();
    _ASSERTE(pThread != NULL);
    HRESULT hr;

    // set up ErrorInfo and the get the hresult to return
    GCX_COOP();
    hr = SetupErrorInfo(pThread->GetThrowable());
    _ASSERTE(hr != S_OK);

    return hr;
}

//
// This does the work of the Unwind and Continue Hanlder inside the catch clause of that handler. The stack has not
// been unwound when this is called. Keep that in mind when deciding where to put new code :)
//
void UnwindAndContinueRethrowHelperInsideCatch(Frame* pEntryFrame, Exception* pException)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    Thread* pThread = GetThread();

    GCX_COOP();

    LOG((LF_EH, LL_INFO1000, "UNWIND_AND_CONTINUE inside catch, unwinding frame chain\n"));

    //
    //
    pThread->SetFrame(pEntryFrame);

#ifdef _DEBUG
    // Call CLRException::GetThrowableFromException to force us to retrieve the THROWABLE
    // while we are still within the context of the catch block. This will help diagnose
    // cases where the last thrown object is NULL.
    OBJECTREF orThrowable = CLRException::GetThrowableFromException(pException);
    CONSISTENCY_CHECK(orThrowable != NULL);
#endif
}

//
// This does the work of the Unwind and Continue Hanlder after the catch clause of that handler. The stack has been
// unwound by the time this is called. Keep that in mind when deciding where to put new code :)
//
VOID DECLSPEC_NORETURN UnwindAndContinueRethrowHelperAfterCatch(Frame* pEntryFrame, Exception* pException)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;
    STATIC_CONTRACT_MODE_ANY;

    if (pException == Exception::GetSOException())
    {
        EEPolicy::HandleSoftStackOverflow();
    }

    // We really should probe before switching to cooperative mode, although there's no chance
    // we'll SO in doing that as we've just caught an exception.  We can't probe just
    // yet though, because we want to avoid reprobing on an SO exception and we need to switch
    // to cooperative to check the throwable for an SO as well as the pException object (as the
    // pException could be a LastThrownObjectException.)  Blech.
    CONTRACT_VIOLATION(SOToleranceViolation);

    GCX_COOP();

    LOG((LF_EH, LL_INFO1000, "UNWIND_AND_CONTINUE caught and will rethrow\n"));

    OBJECTREF orThrowable = CLRException::GetThrowableFromException(pException);
    LOG((LF_EH, LL_INFO1000, "UNWIND_AND_CONTINUE got throwable %p\n",
        OBJECTREFToObject(orThrowable)));

    Exception::Delete(pException);

    if (orThrowable != NULL && g_CLRPolicyRequested)
    {
        if (orThrowable->GetMethodTable() == g_pOutOfMemoryExceptionClass)
        {
            EEPolicy::HandleOutOfMemory();
        }
        else if (orThrowable->GetMethodTable() == g_pStackOverflowExceptionClass)
        {
            EEPolicy::HandleSoftStackOverflow();
        }
    }

    RaiseTheExceptionInternalOnly(orThrowable, FALSE);
}

#ifndef DACCESS_COMPILE
//******************************************************************************
//
// NotifyOfCHFFilterWrapper
//
// Helper function to deliver notifications of CatchHandlerFound inside a
//  EX_TRY/EX_CATCH.
//
// Parameters:
//   pExceptionInfo - the pExceptionInfo passed to a filter function.
//   pCatcherStackAddr - a Frame* from the PAL_TRY/PAL_EXCEPT_FILTER site.
//
// Return:
//   always returns EXCEPTION_CONTINUE_SEARCH.
//
//******************************************************************************
LONG NotifyOfCHFFilterWrapper(
    EXCEPTION_POINTERS *pExceptionInfo, // the pExceptionInfo passed to a filter function.
    PVOID       pCatcherStackAddr)      // a Frame* from the PAL_TRY/PAL_EXCEPT_FILTER site.
{
    ULONG ret = EXCEPTION_CONTINUE_SEARCH;

    // We are here to send an event notification to the debugger and to the appdomain.  To
    //  determine if it is safe to send these notifications, check the following:
    // 1) The thread object has been set up.
    // 2) The thread has an exception on it.
    // 3) The exception is the same as the one this filter is called on.
    Thread *pThread = GetThread();
    if ( (pThread == NULL)  ||
         (pThread->GetExceptionState()->GetContextRecord() == NULL)  ||
         (GetSP(pThread->GetExceptionState()->GetContextRecord()) != GetSP(pExceptionInfo->ContextRecord) ) )
    {
        LOG((LF_EH, LL_INFO1000, "NotifyOfCHFFilterWrapper: not sending notices. pThread: %0x8", pThread));
        if (pThread)
        {
            LOG((LF_EH, LL_INFO1000, ", Thread SP: %0x8, Exception SP: %08x",
                 pThread->GetExceptionState()->GetContextRecord() ? GetSP(pThread->GetExceptionState()->GetContextRecord()) : NULL,
                 pExceptionInfo->ContextRecord ? GetSP(pExceptionInfo->ContextRecord) : NULL ));
        }
        LOG((LF_EH, LL_INFO1000, "\n"));
        return ret;
    }

    // It looks safe, so make the debugger notification.
    ret = g_pDebugInterface->NotifyOfCHFFilter(pExceptionInfo, pCatcherStackAddr);

    return ret;
} // LONG NotifyOfCHFFilterWrapper()
#endif

#ifdef _DEBUG
bool DebugIsEECxxExceptionPointer(void* pv)
{
    CONTRACTL
    {
        DISABLED(NOTHROW);
        GC_NOTRIGGER;
        MODE_ANY;
        DEBUG_ONLY;
    }
    CONTRACTL_END;

    if (pv == NULL)
    {
        return false;
    }


    // check whether the memory is readable in no-throw way
    if (!isMemoryReadable((TADDR)pv, sizeof(UINT_PTR)))
    {
        return false;
    }

    bool retVal = false;

    EX_TRY
    {
        UINT_PTR  vtbl  = *(UINT_PTR*)pv;

        // ex.h

        HRException             boilerplate1;
        COMException            boilerplate2;
        SEHException            boilerplate3;

        // clrex.h

        CLRException            boilerplate4;
        CLRLastThrownObjectException boilerplate5;
        EEException             boilerplate6;
        EEMessageException      boilerplate7;
        EEResourceException     boilerplate8;
        EECOMException          boilerplate9;
        EEFieldException        boilerplate10;
        EEMethodException       boilerplate11;
        EEArgumentException     boilerplate12;
        EETypeLoadException     boilerplate13;
        EEFileLoadException     boilerplate14;
        ObjrefException         boilerplate15;

        UINT_PTR    ValidVtbls[] =
        {
            *((TADDR*)&boilerplate1),
            *((TADDR*)&boilerplate2),
            *((TADDR*)&boilerplate3),
            *((TADDR*)&boilerplate4),
            *((TADDR*)&boilerplate5),
            *((TADDR*)&boilerplate6),
            *((TADDR*)&boilerplate7),
            *((TADDR*)&boilerplate8),
            *((TADDR*)&boilerplate9),
            *((TADDR*)&boilerplate10),
            *((TADDR*)&boilerplate11),
            *((TADDR*)&boilerplate12),
            *((TADDR*)&boilerplate13),
            *((TADDR*)&boilerplate14),
            *((TADDR*)&boilerplate15)
        };

        const int nVtbls = sizeof(ValidVtbls) / sizeof(ValidVtbls[0]);

        for (int i = 0; i < nVtbls; i++)
        {
            if (vtbl == ValidVtbls[i])
            {
                retVal = true;
                break;
            }
        }
    }
    EX_CATCH
    {
        // Swallow any exception out of the exception constructors above and simply return false.
    }
    EX_END_CATCH(SwallowAllExceptions);

    return retVal;
}

void *DebugGetCxxException(EXCEPTION_RECORD* pExceptionRecord);

bool DebugIsEECxxException(EXCEPTION_RECORD* pExceptionRecord)
{
    return DebugIsEECxxExceptionPointer(DebugGetCxxException(pExceptionRecord));
}

//
// C++ EH cracking material gleaned from the debugger:
// (DO NOT USE THIS KNOWLEDGE IN NON-DEBUG CODE!!!)
//
// EHExceptionRecord::EHParameters
//     [0] magicNumber      : uint
//     [1] pExceptionObject : void*
//     [2] pThrowInfo       : ThrowInfo*

#define NUM_CXX_EXCEPTION_PARAMS 3

void *DebugGetCxxException(EXCEPTION_RECORD* pExceptionRecord)
{
    WRAPPER_CONTRACT;

    bool fExCodeIsCxx           = (EXCEPTION_MSVC == pExceptionRecord->ExceptionCode);
    bool fExHasCorrectNumParams = (NUM_CXX_EXCEPTION_PARAMS == pExceptionRecord->NumberParameters);

    if (fExCodeIsCxx && fExHasCorrectNumParams)
    {
        void** ppException = (void**)pExceptionRecord->ExceptionInformation[1];

        if (NULL == ppException)
        {
            return NULL;
        }


        return *ppException;

    }

    CONSISTENCY_CHECK_MSG(!fExCodeIsCxx || fExHasCorrectNumParams, "We expected an EXCEPTION_MSVC exception to have 3 parameters.  Did the CRT change its exception format?");

    return NULL;
}

#endif // _DEBUG

#endif // #ifndef DACCESS_COMPILE


//---------------------------------------------------------------------------------
// Funnel-worker for THROW_BAD_FORMAT and friends.
//
// Note: The "cond" argument is there to tide us over during the transition from
//  BAD_FORMAT_ASSERT to THROW_BAD_FORMAT. It will go away soon.
//---------------------------------------------------------------------------------
VOID ThrowBadFormatWorker(__in_z char *cond, UINT resID, const SString & imageName)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


#ifndef DACCESS_COMPILE


    SString msgStr;

    if (imageName.IsEmpty())
    {
    }
    else
    {
        msgStr += L"[";
        msgStr += imageName;
        msgStr += L"] ";
    }

    SString resStr;
    if (resID == 0 || !resStr.LoadResource(resID, /*bQuiet = */TRUE))
    {
        resStr.LoadResource(MSG_FOR_URT_HR(COR_E_BADIMAGEFORMAT));
    }
    msgStr += resStr;

#ifdef _DEBUG
    if (0 != strcmp(cond, "FALSE"))
    {
        msgStr += L" (Failed condition: "; // this is in DEBUG only - not going to localize it.
        msgStr += SString(SString::Ascii, cond);
        msgStr += L")";
    }
#endif

    ThrowHR(COR_E_BADIMAGEFORMAT, msgStr);

#endif // #ifndef DACCESS_COMPILE
}

#ifdef _MSC_VER
#pragma warning(default : 4640)
#endif



