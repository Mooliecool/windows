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
// ProfToEEInterfaceImpl.cpp
//
// This module contains the code used by the Profiler to communicate with
// the EE.  This allows the Profiler DLL to get access to private EE data
// structures and other things that should never be exported outside of
// mscoree.dll.
//
//*****************************************************************************
#include "common.h"
#include <posterror.h>
#include "proftoeeinterfaceimpl.h"
#include "dllimport.h"
#include "threads.h"
#include "method.hpp"
#include "vars.hpp"
#include "dbginterface.h"
#include "corprof.h"
#include "class.h"
#include "object.h"
#include "ceegen.h"
#include "eeconfig.h"
#include "generics.h"
#include "methoditer.h"

//
// This type is an overlay onto the exported type COR_PRF_FRAME_INFO.
// The first four fields *must* line up with the same fields in the
// exported type.  After that, we can add to the end as we wish.
//
typedef struct _COR_PRF_FRAME_INFO_INTERNAL {
    USHORT size;
    USHORT version;
    FunctionID funcID;
    UINT_PTR IP;
    void *extraArg;
    LPVOID thisArg;
} COR_PRF_FRAME_INFO_INTERNAL, *PCOR_PRF_FRAME_INFO_INTERNAL;

//
// After we ship a product with a certain struct type for COR_PRF_FRAME_INFO_INTERNAL
// we have that as a version.  If we change that in a later product, we can increment
// the counter below and then we can properly do versioning.
//
#define COR_PRF_FRAME_INFO_INTERNAL_CURRENT_VERSION 1

// When we call into profiler code, we push one of these babies onto the stack to
// remember on the Thread how the profiler was called.  If the profiler calls back into us,
// we use the flags that this set to authorize.
//
// EEToProfInterfaceImpl.cpp has something similar to this that it uses for
// all the ICorProfilerCallback(2) methods (this one is for the profiler enter,
// leave, tailcall, FunctionIDMapper hooks).  Since we're in the VM DLL
// we can optimize a bit and call directly into the inlined Thread functions,
// which is why we have a separate implementation of this mini-holder.
class SetCallbackStateFlagsFromVMHolder
{
public:
    FORCEINLINE SetCallbackStateFlagsFromVMHolder(DWORD dwFlags)
    {
        // This is called before entering a profiler.  We set the specified dwFlags on
        // the Thread object, and remember the previous flags for later.
        m_pThread = GetThread();
        if (m_pThread != NULL)
        {
            m_dwOriginalFullState = m_pThread->SetProfilerCallbackStateFlags(dwFlags);
        }
        else
        {
            m_dwOriginalFullState = 0;
        }
    }
    
    FORCEINLINE ~SetCallbackStateFlagsFromVMHolder()
    {
        // This is called after the profiler returns to us.  We reinstate the
        // original flag set here.
        if (m_pThread != NULL)
        {
            m_pThread->SetProfilerCallbackFullState(m_dwOriginalFullState);
        }
    }
    
private:
    Thread*     m_pThread;
    DWORD      m_dwOriginalFullState;
};


// This helper function and macro are used to test the flags in order to authorize
// a profiler's call into us.  The macro is placed at the top of any call that's disallowed
// asynchronously.  If no flags are set, we reject the call.
inline BOOL AreCallbackStateFlagsSet(DWORD dwFlags)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    Thread* pThread = GetThread();
    if (pThread == NULL)
    {
        // Not a managed thread; profiler can do whatever it wants
        return TRUE;
    }

    return (pThread->GetProfilerCallbackFullState() & dwFlags);
}


#define ENSURE_CALLBACK_STATE_FLAGS_SET(dwFlags)    \
    if (!AreCallbackStateFlagsSet(dwFlags))                         \
    {                                                                                       \
        LOG((LF_CORPROF, LL_ERROR, "**PROF: ERROR: Returning CORPROF_E_UNSUPPORTED_CALL_SEQUENCE due to illegal asynchronous profiler call\n"));            \
        return CORPROF_E_UNSUPPORTED_CALL_SEQUENCE;     \
    }


// This macro is used to ensure that the current thread is not in a forbid
// suspend region.   Some methods are allowed to be called asynchronously,
// but some of them call JIT functions that take a reader lock.  So we need to ensure
// the current thread hasn't been hijacked by a profiler while it was holding the writer lock.
// Checking the ForbidSuspendThread region is a sufficient test for this
#define FAIL_IF_IN_FORBID_SUSPEND_REGION()                          \
    Thread* __pThread = GetThread();                                                  \
    if (__pThread != NULL && __pThread->IsInForbidSuspendRegion())         \
    {                                                                                                    \
        return CORPROF_E_ASYNCHRONOUS_UNSAFE;                           \
    }               
    

// See EEProfInterfaces.h for more details. In summary
// INTERNAL EE/Prof API:
//   A TypeID is just a type handle under the covers, possibly instantiated
//   A CodeID is just a method desc under the covers, possibly instantiated
//
// EXTERNAL COM/Prof API:
//   A ClassID is always a TypeHandle
//   A FunctionID is always a method desc pointer (inherited from being a CodeID).
//
// The above representations ensure that generic type handles don't need to be loaded for profiling purposes
//

TypeID TypeHandleToTypeID(TypeHandle th)
{
    WRAPPER_CONTRACT;
    return (TypeID) th.AsPtr();
}

static ClassID TypeHandleToClassID(TypeHandle th)
{
    WRAPPER_CONTRACT;
    return (ClassID) th.AsPtr();
}


static CodeID MethodDescToCodeID(MethodDesc  *pMD)
{
    LEAF_CONTRACT;
    return (CodeID) pMD;
}

static TypeHandle TypeIDToTypeHandle(TypeID id)
{
    WRAPPER_CONTRACT;
    return TypeHandle::FromPtr((void *) id);
}

static MethodDesc *CodeIDToMethodDesc(CodeID id)
{
    LEAF_CONTRACT;
    return (MethodDesc *) id;
}

MethodDesc *FunctionIdToMethodDesc(FunctionID functionID)
{
    LEAF_CONTRACT;
    
    MethodDesc *pMethodDesc;

    pMethodDesc = reinterpret_cast< MethodDesc* >(functionID);

    _ASSERTE(pMethodDesc != NULL);
    return pMethodDesc;
}

// A couple Info calls need to understand what constitutes an "array", and what
// kinds of arrays there are.  ArrayKindFromTypeHandle tries to put some of this
// knowledge in a single place
typedef enum
{
    ARRAY_KIND_TYPEDESC,        // Normal, garden-variety typedesc array
    ARRAY_KIND_METHODTABLE,  // Weirdo array with its own unshared methodtable (e.g., System.Object[])
    ARRAY_KIND_NOTARRAY,       // Not an array
} ARRAY_KIND;

inline ARRAY_KIND ArrayKindFromTypeHandle(TypeHandle th)
{
    LEAF_CONTRACT;

    if (th.IsArray())
    {
        return ARRAY_KIND_TYPEDESC;
    }

    if (!th.IsTypeDesc() && th.GetMethodTable()->IsArray())
    {
        return ARRAY_KIND_METHODTABLE;
    }

    return ARRAY_KIND_NOTARRAY;
}

ClassID ProfToEEInterfaceImpl::GetClassIdForNonGenericType(TypeID typeID)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetClassIdForNonGenericType. TypeID: 0x%p.\n", typeID));

    TypeHandle t = TypeIDToTypeHandle(typeID);
    if (!t.IsNull() && t.HasInstantiation())
    {
        return NULL;
    }
    else
    {
        return (ClassID)TypeHandleToClassID(t);
    }
}

// Exposed statically for the benefit of the JIT/EE interface
/*static*/
FunctionID  ProfToEEInterfaceImpl::_CodeIdToFunctionId(CodeID codeId)
{
    LEAF_CONTRACT;
    return reinterpret_cast< FunctionID >(codeId);
}


FunctionID ProfToEEInterfaceImpl::CodeIdToFunctionId(CodeID codeId)
{
    WRAPPER_CONTRACT;
    return ProfToEEInterfaceImpl::_CodeIdToFunctionId(codeId);
}

//********** Code. ************************************************************

bool __stdcall TrackAllocationsEnabled()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;      
    } CONTRACTL_END;

    return (
#ifdef PROFILING_SUPPORTED
        CORProfilerTrackAllocationsEnabled() ||
#endif // PROFILING_SUPPORTED
        g_IBCLogger.InstrEnabled());
}

// ProfilerObjectAllocatedCallback is called if a profiler is attached, requesting
// ObjectAllocated callbacks, OR if g_IBCLogger.InstrEnabled().  Although IBC 
// access logging has nothing to do with profilers, this function is a convenient
// place to do it for any newly allocated object.
void __stdcall ProfilerObjectAllocatedCallback(OBJECTREF objref, TypeID classId)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    
    TypeHandle th = OBJECTREFToObject(objref)->GetTypeHandle();
    g_IBCLogger.LogTypeMethodTableAccess(&th);

    // WARNING: objref can move as a result of the ObjectAllocated() call below if
    // the profiler causes a GC, so any operations on the objref should occur above
    // this comment (unless you're prepared to add a GCPROTECT around the objref).  

#ifdef PROFILING_SUPPORTED
    // Notify the profiler of the allocation
    if (CORProfilerTrackAllocations())
    {
        // Note that for generic code we always return uninstantiated ClassIDs and FunctionIDs.
        // Thus we strip any instantiations of the ClassID (which is really a type handle) here.
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ObjectAllocated(
                (ObjectID) OBJECTREFToObject(objref), classId);
    }
#endif // PROFILING_SUPPORTED

}

void __stdcall GarbageCollectionStartedCallback(int generation, BOOL induced)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; // can be called even on GC threads
    } CONTRACTL_END;

#ifdef PROFILING_SUPPORTED
    // Notify the profiler of start of the collection
    if (CORProfilerTrackGC())
    {
        BOOL generationCollected[COR_PRF_GC_LARGE_OBJECT_HEAP+1];
        if (generation == COR_PRF_GC_GEN_2)
            generation = COR_PRF_GC_LARGE_OBJECT_HEAP;
        for (int gen = 0; gen <= COR_PRF_GC_LARGE_OBJECT_HEAP; gen++)
            generationCollected[gen] = gen <= generation;

        //
        // Mark that we are starting a GC.  This will allow profilers to do limited object inspection
        // during callbacks that occur while a GC is happening.
        //
        g_profControlBlock.fGCInProgress = TRUE;

        PROFILER_CALL;
        g_profControlBlock.pProfInterface->GarbageCollectionStarted(COR_PRF_GC_LARGE_OBJECT_HEAP+1, generationCollected, induced ? COR_PRF_GC_INDUCED : COR_PRF_GC_OTHER);
    }
#endif // PROFILING_SUPPORTED
}

void __stdcall GarbageCollectionFinishedCallback()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; // can be called even on GC threads        
    } CONTRACTL_END;

#ifdef PROFILING_SUPPORTED
    // Notify the profiler of end of the collection
    if (CORProfilerTrackGC())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->GarbageCollectionFinished();
        
        g_profControlBlock.fGCInProgress = FALSE;
    }
#endif // PROFILING_SUPPORTED
}

struct GenerationDesc
{
    int generation;
    BYTE *rangeStart;
    BYTE *rangeEnd;
    BYTE *rangeEndReserved;
};

struct GenerationTable
{
    ULONG count;
    ULONG capacity;
    static const ULONG defaultCapacity = 4; // that's the minimum for 3 generation plus the large object heap
    GenerationTable *prev;
    GenerationDesc *genDescTable;
#ifdef  _DEBUG
    ULONG magic;
#define GENERATION_TABLE_MAGIC 0x34781256
#define GENERATION_TABLE_BAD_MAGIC 0x55aa55aa
#endif
};

static void GenWalkFunc(void *context, int generation, BYTE *rangeStart, BYTE * rangeEnd, BYTE *rangeEndReserved)
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; // can be called even on GC threads        
        PRECONDITION(CheckPointer(context));
        PRECONDITION(0 <= generation && generation <= 3);
        PRECONDITION(CheckPointer(rangeStart));
        PRECONDITION(CheckPointer(rangeEnd));
        PRECONDITION(CheckPointer(rangeEndReserved));
    } CONTRACT_END;

    GenerationTable *generationTable = (GenerationTable *)context;

    _ASSERTE(generationTable->magic == GENERATION_TABLE_MAGIC);

    ULONG count = generationTable->count;
    if (count >= generationTable->capacity)
    {
        ULONG newCapacity = generationTable->capacity == 0 ? GenerationTable::defaultCapacity : generationTable->capacity * 2;
        GenerationDesc *newGenDescTable = new (nothrow) GenerationDesc[newCapacity];
        if (newGenDescTable == NULL)
        {
            // if we can't allocate a bigger table, we'll have to ignore this call
            RETURN;
        }
        memcpy(newGenDescTable, generationTable->genDescTable, sizeof(generationTable->genDescTable[0]) * generationTable->count);
        delete[] generationTable->genDescTable;
        generationTable->genDescTable = newGenDescTable;
        generationTable->capacity = newCapacity;
    }
    _ASSERTE(count < generationTable->capacity);

    GenerationDesc *genDescTable = generationTable->genDescTable;

    genDescTable[count].generation = generation;
    genDescTable[count].rangeStart = rangeStart;
    genDescTable[count].rangeEnd = rangeEnd;
    genDescTable[count].rangeEndReserved = rangeEndReserved;

    generationTable->count = count + 1;
}

// This is the table of generation bounds updated by the gc 
// and read by the profiler. So this is a single writer,
// multiple readers scenario.
static GenerationTable *s_currentGenerationTable;

// The generation table is updated atomically by replacing the
// pointer to it. The only tricky part is knowing when
// the old table can be deleted.
static LONG s_generationTableLock;

// This is just so we can assert there's a single writer

// This is called from the gc to push a new set of generation bounds
void __stdcall UpdateGenerationBounds()
{
    CONTRACT_VOID
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; // can be called even on GC threads        
        PRECONDITION(FastInterlockIncrement(&s_generationTableWriterCount) == 1);
        POSTCONDITION(FastInterlockDecrement(&s_generationTableWriterCount) == 0);
    } CONTRACT_END;

#ifdef PROFILING_SUPPORTED
    // Notify the profiler of start of the collection
    if (CORProfilerTrackGC())
    {
        // generate a new generation table
        GenerationTable *newGenerationTable = new (nothrow) GenerationTable();
        if (newGenerationTable == NULL)
            RETURN;
        newGenerationTable->count = 0;
        newGenerationTable->capacity = GenerationTable::defaultCapacity;
        // if there is already a current table, use its count as a guess for the capacity
        if (s_currentGenerationTable != NULL)
            newGenerationTable->capacity = s_currentGenerationTable->count;
        newGenerationTable->prev = NULL;
        newGenerationTable->genDescTable = new (nothrow) GenerationDesc[newGenerationTable->capacity];
        if (newGenerationTable->genDescTable == NULL)
            newGenerationTable->capacity = 0;

#ifdef  _DEBUG
        newGenerationTable->magic = GENERATION_TABLE_MAGIC;
#endif
        // fill in the values by calling back into the gc, which will report
        // the ranges by calling GenWalkFunc for each one
        GCHeap *hp = GCHeap::GetGCHeap();
        hp->DescrGenerationsToProfiler(GenWalkFunc, newGenerationTable);

        // remember the old table and plug in the new one
        GenerationTable *oldGenerationTable = s_currentGenerationTable;
        s_currentGenerationTable = newGenerationTable;

        // WARNING: tricky code!
        //
        // We sample the generation table lock *after* plugging in the new table
        // We do so using an interlocked operation so the cpu can't reorder
        // the write to the s_currentGenerationTable with the increment.
        // If the interlocked increment returns 1, we know nobody can be using
        // the old table (readers increment the lock before using the table,
        // and decrement it afterwards). Any new readers coming in
        // will use the new table. So it's safe to delete the old
        // table.
        // On the other hand, if the interlocked increment returns
        // something other than one, we put the old table on a list
        // dangling off of the new one. Next time around, we'll try again
        // deleting any old tables.
        if (FastInterlockIncrement(&s_generationTableLock) == 1)
        {
            // We know nobody can be using any of the old tables
            while (oldGenerationTable != NULL)
            {
                _ASSERTE(oldGenerationTable->magic == GENERATION_TABLE_MAGIC);
#ifdef  _DEBUG
                oldGenerationTable->magic = GENERATION_TABLE_BAD_MAGIC;
#endif
                GenerationTable *temp = oldGenerationTable;
                oldGenerationTable = oldGenerationTable->prev;
                delete[] temp->genDescTable;
                delete temp;
            }
        }
        else
        {
            // put the old table on a list
            newGenerationTable->prev = oldGenerationTable;
        }
        FastInterlockDecrement(&s_generationTableLock);
    }
#endif // PROFILING_SUPPORTED
    RETURN;
}

FunctionIDMapper *g_pProfilersFuncIDMapper = NULL;

UINT_PTR __stdcall EEFunctionIDMapper(FunctionID funcId, BOOL *pbHookFunction)
{
    // At the time the JIT calls us, it's ok to throw or trigger gc, so we'll be
    // lenient and allow the profiler FunctionIDMapper callback to do so.
    CONTRACTL {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    
    if (g_pProfilersFuncIDMapper == NULL)
    {
        *pbHookFunction = TRUE;
        return ((UINT_PTR) NULL);
    }

    SetCallbackStateFlagsFromVMHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // The attached profiler may not want to hook this function, so ask it and return
    // for the mapped id whatever it returns.
    //
    PROFILER_CALL;
    return g_pProfilersFuncIDMapper(funcId, pbHookFunction);
}

FunctionIDMapper *g_pFuncIDMapper = &EEFunctionIDMapper;

#ifdef PROFILING_SUPPORTED

//
// Returns S_OK if we can determine that we are in a window to allow object inspection.
//
HRESULT AllowObjectInspection()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; // tests for preemptive mode dynamically as its main function so contract enforcement is not appropriate
    } CONTRACTL_END;

    //
    // Check first to see if we are in the process of doing a GC and presume that the profiler
    // is making this object inspection from the same thread that notified of a valid ObjectID.
    //
    if (g_profControlBlock.fGCInProgress)
    {
        return S_OK;
    }

    //
    // Thus we must have a managed thread, and it must be in coop mode.
    // (That will also guarantee we're in a callback).
    //
    Thread *pThread = GetThread();

    if (pThread == NULL)
    {
        return CORPROF_E_NOT_MANAGED_THREAD;
    }

    // note this is why we don't enforce the contract of being in cooperative mode the whole point
    // is that clients of this fellow want to return a robust error if not cooperative
    // so technically they are mode_any although the only true preemptive support they offer
    // is graceful failure in that case
    
    if (!pThread->PreemptiveGCDisabled())
    {
        return E_FAIL;
    }

    return S_OK;
}



ProfToEEInterfaceImpl::ProfToEEInterfaceImpl() :
    m_pHeapList(NULL)
{
    LEAF_CONTRACT;
}

HRESULT ProfToEEInterfaceImpl::Init()
{
    LEAF_CONTRACT;
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: Init.\n"));
    return (S_OK);
}

void ProfToEEInterfaceImpl::Terminate()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: Terminate.\n"));
    
    while (m_pHeapList)
    {
        HeapList *pDel = m_pHeapList;
        m_pHeapList = m_pHeapList->m_pNext;
        delete pDel;
    }

    // Terminate is called from another DLL, so we need to delete ourselves.
    delete this;
}

// Declarations for asm wrappers of profiler callbacks
#if defined(_X86_) || defined(_WIN64)
EXTERN_C void __stdcall ProfileEnterNaked(FunctionID functionId,
                                          void *clientData,
                                          COR_PRF_FRAME_INFO *pFrameInfo,
                                          COR_PRF_FUNCTION_ARGUMENT_INFO *pArgInfo);
EXTERN_C void __stdcall ProfileLeaveNaked(FunctionID functionId,
                                          void *clientData,
                                          COR_PRF_FRAME_INFO *pFrameInfo,
                                          COR_PRF_FUNCTION_ARGUMENT_RANGE *pArgInfo);
EXTERN_C void __stdcall ProfileTailcallNaked(FunctionID functionId,
                                             void *clientData,
                                             COR_PRF_FRAME_INFO *pFrameInfo);
#define PROFILECALLBACK(name) name##Naked

#else

// Use native calling convention for profiler callbacks
FCDECL4(EXTERN_C void, ProfileEnterWrapper, FunctionID functionId,
                                            void *clientData,
                                            COR_PRF_FRAME_INFO *pFrameInfo,
                                            COR_PRF_FUNCTION_ARGUMENT_INFO *pArgInfo);
FCDECL4(EXTERN_C void, ProfileLeaveWrapper, FunctionID functionId,
                                            void *clientData,
                                            COR_PRF_FRAME_INFO *pFrameInfo,
                                            COR_PRF_FUNCTION_ARGUMENT_RANGE *pArgInfo);
FCDECL3(EXTERN_C void, ProfileTailcallWrapper, FunctionID functionId,
                                               void *clientData,
                                               COR_PRF_FRAME_INFO *pFrameInfo);

FCDECL3(EXTERN_C void, ProfileEnter, FunctionID functionId,
                                     void *clientData,
                                     void *platformSpecificHandle);
FCDECL3(EXTERN_C void, ProfileLeave, FunctionID functionId,
                                     void *clientData,
                                     void *platformSpecificHandle);
FCDECL3(EXTERN_C void, ProfileTailcall, FunctionID functionId,
                                        void *clientData,
                                        void *platformSpecificHandle);

#define PROFILECALLBACK(name) name##Wrapper
#endif



bool ProfToEEInterfaceImpl::SetEventMask(DWORD dwEventMask)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: SetEventMask 0x%08x.\n", dwEventMask));

    // This call is allowed asynchronously, so we don't need to check the callback state flags

    bool retVal = true;

    // If we're not in initialization or shutdown, make sure profiler is
    // not trying to set an immutable attribute
    if (g_profStatus != profInInit)
    {
        if ((dwEventMask & COR_PRF_MONITOR_IMMUTABLE) !=
            (g_profControlBlock.dwControlFlags & COR_PRF_MONITOR_IMMUTABLE))
        {
            return (false);
        }
    }

    // Now save the modified masks
    g_profControlBlock.dwControlFlags = dwEventMask;

    //
    // We'd love to be able to set the JIT function hooks here if the event mask
    // changed to disconnect/reconnect the hooks, but SetEventMask gets called from
    // various places that are NOTHROW, and setting the hooks throws (allocates stubs)
    //
    if ((g_profControlBlock.pEnter != NULL) || (g_profControlBlock.pEnter2 != NULL))
    {
        EX_TRY
        {
            if (CORProfilerFunctionArgsEnabled() ||
                CORProfilerFunctionReturnValueEnabled() ||
                CORProfilerFrameInfoEnabled() ||

                // DSS walk of thread inside profiler's enter/leave/tailcall hook would skip
                // call frames if we called directly into profiler's enter/leave/tailcall hook
                // from jitted code, since that doesn't push a Frame.  So we force the C++
                // ProfileEnter/ProfileLeave/ProfileTailcall FCALL to wrap the profiler's
                // hook so we have a chance to push the Frame.
                CORProfilerStackSnapshotEnabled() ||
                
                (g_profControlBlock.pEnter != NULL))
            {
                //
                // If the profiler wants args or generic info, then we have to go thru the EE's
                // functions to gather that data before going to the profiler, so set the callbacks
                // to the EE's functions.
                //
                SetEnterLeaveFunctionHooksForJit((FunctionEnter *)PROFILECALLBACK(ProfileEnter),
                                                 (FunctionLeave *)PROFILECALLBACK(ProfileLeave),
                                                 (FunctionTailcall *)PROFILECALLBACK(ProfileTailcall));
            }
            else
            {
                //
                // Otherwise we can have the JIT call directly to the profiler's routines.
                //
                SetEnterLeaveFunctionHooksForJit((FunctionEnter *)g_profControlBlock.pEnter2,
                                                 (FunctionLeave *)g_profControlBlock.pLeave2,
                                                 (FunctionTailcall *)g_profControlBlock.pTailcall2);
            }
        }
        EX_CATCH
        {
            retVal = false;
        }
        EX_END_CATCH(RethrowTerminalExceptions);
    }


    if (g_profStatus == profInInit)
    {
        // If the profiler has requested remoting cookies so that it can
        // track logical call stacks, then we must initialize the cookie
        // template.
        if (CORProfilerTrackRemotingCookie())
        {
            PROFILER_CALL;
            HRESULT hr = g_profControlBlock.pProfInterface->InitGUID();

            if (FAILED(hr))
                return (false);
        }
    }

    // Return success
    return retVal;
}

void ProfToEEInterfaceImpl::DisablePreemptiveGC(ThreadID threadId)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORPROF, LL_INFO10000, "**PROF: DisablePreemptiveGC 0x%p.\n", threadId));

    ((Thread *)threadId)->DisablePreemptiveGC();
}

void ProfToEEInterfaceImpl::EnablePreemptiveGC(ThreadID threadId)
{
    WRAPPER_CONTRACT;

    LOG((LF_CORPROF, LL_INFO10000, "**PROF: EnablePreemptiveGC 0x%p.\n", threadId));

    ((Thread *)threadId)->EnablePreemptiveGC();
}

BOOL ProfToEEInterfaceImpl::PreemptiveGCDisabled(ThreadID threadId)
{
    WRAPPER_CONTRACT;
    
    return ((Thread *)threadId)->PreemptiveGCDisabled();
}

// Gives EEToProfInterfaceImpl access to setting individual callback flags stored on the Thread object.
// Stores current thread's ID in *pThreadId.  Returns the previous state of all flags.
DWORD ProfToEEInterfaceImpl::SetProfilerCallbackStateFlags(DWORD dwFlags, ThreadID* pThreadId)
{
    WRAPPER_CONTRACT;

    _ASSERTE(pThreadId != NULL);
    
    *pThreadId = (ThreadID) GetThread();
    if (*pThreadId == 0)
    {
        return 0;
    }

    return ((Thread*) (*pThreadId))->SetProfilerCallbackStateFlags(dwFlags);
}

// Gives EEToProfInterfaceImpl access to setting at once all callback flag bits stored on the Thread object.
// Used to reinstate the previous state that had been modified by a previous call to
// SetProfilerCallbackStateFlags
void ProfToEEInterfaceImpl::SetProfilerCallbackFullState(DWORD dwFullState, ThreadID threadId)
{
    WRAPPER_CONTRACT;

    Thread* pThread = (Thread*) threadId;
    _ASSERTE(pThread != NULL);
    pThread->SetProfilerCallbackFullState(dwFullState);
}

HRESULT ProfToEEInterfaceImpl::GetHandleFromThread(ThreadID threadId, HANDLE *phThread)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetHandleFromThread 0x%p.\n", threadId));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    HRESULT hr = S_OK;

    HANDLE hThread = ((Thread *)threadId)->GetThreadHandle();

    if (hThread == INVALID_HANDLE_VALUE)
        hr = E_INVALIDARG;

    else if (phThread)
        *phThread = hThread;

    return (hr);
}

HRESULT ProfToEEInterfaceImpl::GetObjectSize(ObjectID objectId, ULONG *pcSize)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    // Get the object pointer
    Object *pObj = reinterpret_cast<Object *>(objectId);

    // Get the size
    if (pcSize)
        *pcSize = (ULONG) pObj->GetSize();

    // Indicate success
    return (S_OK);
}

HRESULT ProfToEEInterfaceImpl::IsArrayClass(
    /* [in] */  ClassID classId,
    /* [out] */ CorElementType *pBaseElemType,
    /* [out] */ TypeID *pBaseTypeId,
    /* [out] */ ULONG   *pcRank)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;

        // We really don't throw or trigger throughout this function, and runtime asserts and
        // even a chk runtime scan will agree with me.  But a dbg runtime scan will disagree,
        // because GetRank() below calls TypeDesc::GetMethodTable, which tests
        // GetInternalCorElementType() for various types that we know we're not.  We know
        // we're an array by the time we call GetRank(), and we are NOT the really annoying
        // ELEMENT_TYPE_FNPTR.   GetMethodTable(), when it encounters that type,
        // would have to call TheFnPtrClass(), which calls TheUIntPtrClass(), which has GC_TRIGGERS
        // and THROWS. This little sequence actually invalidates a ton of existing contracts throughout
        // the CLR, but the inlining trips up the chk runtime scanner, and you'll only see them when
        // scanning a dbg runtime, which pretty much no one does.
        DISABLED(NOTHROW);
        DISABLED(GC_NOTRIGGER);
        
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: IsArrayClass 0x%p.\n", classId));

    // This call is allowed asynchronously, so we don't need to check the callback state flags

    HRESULT hr;
    
    if (classId == NULL)
    {
        if (pBaseTypeId != NULL)
            *pBaseTypeId = NULL;
        return (S_FALSE);
    }

    TypeHandle th = TypeHandle::FromPtr((void *)classId);

    ARRAY_KIND arrayKind = ArrayKindFromTypeHandle(th);
        
    // If this is indeed an array class, get some info about it
    switch (arrayKind)
    {
        default:
        {
            _ASSERTE(!"Unexpected return from ArrayKindFromTypeHandle()");
            hr = E_UNEXPECTED;
            break;
        }
            
        case ARRAY_KIND_TYPEDESC:
        {
            // This is actually an array, so cast it up
            ArrayTypeDesc *pArr = th.AsArray();

            // Fill in the type if they want it
            if (pBaseElemType != NULL)
                *pBaseElemType = pArr->GetArrayElementTypeHandle().GetVerifierCorElementType();

            // If this is an array of classes and they wish to have the base type
            // If there is no associated class with this type, then there's no problem
            // because GetClass returns NULL which is the default we want to return in
            // this case.
            // Note that for generic code we always return uninstantiated ClassIDs and FunctionIDs
            if (pBaseTypeId != NULL)
                *pBaseTypeId = TypeHandleToTypeID(pArr->GetTypeParam());

            // If they want the number of dimensions of the array
            if (pcRank != NULL)
                *pcRank = (ULONG) pArr->GetRank();

            // S_OK indicates that this was indeed an array
            hr = S_OK;
            break;
        }
        case ARRAY_KIND_METHODTABLE:
        {
            MethodTable *pArrMT = th.GetMethodTable();

            // Fill in the type if they want it
            if (pBaseElemType != NULL)
                *pBaseElemType = pArrMT->GetArrayElementType();

            // If this is an array of classes and they wish to have the base type.
            if (pBaseTypeId != NULL)
                *pBaseTypeId = TypeHandleToTypeID(pArrMT->GetApproxArrayElementTypeHandle());

            // If they want the number of dimensions of the array
            if (pcRank != NULL)
                *pcRank = (ULONG) pArrMT->GetRank();

            // S_OK indicates that this was indeed an array
            hr = S_OK;
            break;
        }
        case ARRAY_KIND_NOTARRAY:
        {
            if (pBaseTypeId != NULL)
                *pBaseTypeId = NULL;
            
            // This is not an array, S_FALSE indicates so.
            hr = S_FALSE;
            break;
        }
    }

    return hr;
}
    
HRESULT ProfToEEInterfaceImpl::GetThreadInfo(ThreadID threadId, DWORD *pdwWin32ThreadId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetThreadInfo 0x%p.\n", threadId));  
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    if (pdwWin32ThreadId)
        *pdwWin32ThreadId = ((Thread *)threadId)->GetOSThreadId();

    return (S_OK);
}

HRESULT ProfToEEInterfaceImpl::GetCurrentThreadID(ThreadID *pThreadId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetCurrentThreadID.\n"));    

    // This call is allowed asynchronously, so we don't need to check the callback state flags

    HRESULT hr = S_OK;

    // No longer assert that GetThread doesn't return NULL, since callbacks
    // can now occur on non-managed threads (such as the GC helper threads)
    Thread *pThread = GetThread();

    // If pThread is null, then the thread has never run managed code and
    // so has no ThreadID
    if (pThread == NULL)
        hr = CORPROF_E_NOT_MANAGED_THREAD;

    // Only provide value if they want it
    else if (pThreadId)
        *pThreadId = (ThreadID) pThread;

    return (hr);
}

HRESULT ProfToEEInterfaceImpl::GetFunctionFromIP(LPCBYTE ip, CodeID *pFunctionId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetFunctionFromIP 0x%p.\n", ip));  

    // This call is allowed asynchronously, so we don't need to check the callback state flags
    //
    // However, the JIT functions take a reader lock.  So we need to ensure the current thread
    // hasn't been hijacked by a profiler while it was holding the writer lock.  Checking the
    // ForbidSuspendThread region is a sufficient test for this
    FAIL_IF_IN_FORBID_SUSPEND_REGION();
    
    HRESULT hr = S_OK;

    // Get the JIT manager for the current IP
    IJitManager *pJitMan = ExecutionManager::FindJitMan((SLOT)ip);

    // We got a JIT manager that claims to own the IP
    if (pJitMan != NULL)
    {
        // Get the FunctionDesc for the current IP from the JIT manager
        MethodDesc *pMethodDesc;
        pJitMan->JitCodeToMethodInfo((SLOT)ip, &pMethodDesc);

        // There are some headers (like CORCOMPILE_METHOD_HEADER) and some
        // padding area in the code heap. These may not correspond to a method
        if (pMethodDesc == NULL)
            return E_FAIL;

        // never return a method that the user of the profiler API cannot use
        if (pMethodDesc->IsNoMetadata())
            return E_FAIL;

        // Only fill out the value if they want one
        // Note that for generic code we always return uninstantiated ClassIDs and FunctionIDs
        if (pFunctionId)
        {
            *pFunctionId = MethodDescToCodeID(pMethodDesc);
        }
    }

    // IP does not belong to a JIT manager
    else
    {
        hr = E_FAIL;
    }

    return (hr);
}

//*****************************************************************************
// Given a function id, retrieve the metadata token and a reader api that
// can be used against the token.
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::GetTokenFromFunction(
    FunctionID  functionId,
    REFIID      riid,
    IUnknown    **ppOut,
    mdToken     *pToken)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetTokenFromFunction 0x%p.\n", functionId));  
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    HRESULT     hr = S_OK;

    MethodDesc *pMD = FunctionIdToMethodDesc(functionId);

    // it's not safe to examine a methoddesc that has not been restored so do not do so
    if (!pMD->IsRestored())
        return CORPROF_E_DATAINCOMPLETE;
    
    if (pToken)
    {
        *pToken = pMD->GetMemberDef();
    }

    // don't bother with any of this module fetching if the metadata access isn't requested
    if (ppOut)
    {
        // Ask for the importer interfaces from the metadata, and then QI for the requested guy.
        IMetaDataImport *pImport = NULL;
        EX_TRY
        {
            Module *pMod = pMD->GetModule();
            pImport = pMod->GetRWImporter();
        }
        EX_CATCH_HRESULT(hr);

        if (FAILED(hr))
            return hr;

        // Get the requested interface
        hr = pImport->QueryInterface(riid, (void **) ppOut);
    }

    return hr;
}

//*****************************************************************************
// Gets the location and size of a jitted function
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::GetCodeInfo(CodeID codeID, LPCBYTE *pStart, ULONG *pcSize)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetCodeInfo 0x%p.\n", codeID));     
    
    // This call is allowed asynchronously, so we don't need to check the callback state flags
    //
    // GetCodeInfo2Helper() will ensure we're not called at a dangerous time
    
    COR_PRF_CODE_INFO codeInfos[2];
    ULONG32 cCodeInfos;

    HRESULT hr = this->GetCodeInfo2(codeID, 2, &cCodeInfos, codeInfos);

    if ((FAILED(hr)) || (0 == cCodeInfos))
    {
        return hr;
    }

    if (NULL != pStart)
    {
        *pStart = reinterpret_cast< LPCBYTE >(codeInfos[0].startAddress);
    }

    if (NULL != pcSize)
    {
        *pcSize = codeInfos[0].size;
    }

    return hr;
}

HRESULT ProfToEEInterfaceImpl::GetCodeInfo2(CodeID codeID,
                                            ULONG32  cCodeInfos,
                                            ULONG32* pcCodeInfos,
                                            COR_PRF_CODE_INFO codeInfos[])


{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(pcCodeInfos, NULL_OK));
        PRECONDITION(CheckPointer(codeInfos, NULL_OK));
        SO_NOT_MAINLINE;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetCodeInfo2 0x%p.\n", codeID));         

    // This call is allowed asynchronously, so we don't need to check the callback state flags

    HRESULT hr = S_OK;

    EX_TRY
    {
        ENABLE_FORBID_GC_LOADER_USE_IN_THIS_SCOPE();

        hr = this->GetCodeInfo2Helper(codeID,
                                      cCodeInfos,
                                      pcCodeInfos,
                                      codeInfos);
    }
    EX_CATCH_HRESULT(hr);

    return hr;
}

HRESULT ProfToEEInterfaceImpl::GetCodeInfo2Helper(CodeID codeID,
                                                  ULONG32  cCodeInfos,
                                                  ULONG32* pcCodeInfos,
                                                  COR_PRF_CODE_INFO codeInfos[])

{
    SO_NOT_MAINLINE_FUNCTION;
    
    if (0 == codeID)
    {
        return E_INVALIDARG;
    }

    if ((0 != cCodeInfos) && (NULL == codeInfos))
    {
        return E_INVALIDARG;
    }

    // GetCodeInfo(2) may be called asynchronously, and the JIT functions take a reader
    // lock.  So we need to ensure the current thread hasn't been hijacked by a profiler while
    // it was holding the writer lock.  Checking the ForbidSuspendThread region is a sufficient test for this
    FAIL_IF_IN_FORBID_SUSPEND_REGION();
    
    MethodDesc* pMethodDesc = FunctionIdToMethodDesc(CodeIdToFunctionId(codeID));

    // it's not safe to examine a methoddesc that has not been restored so do not do so
    if (!pMethodDesc ->IsRestored())
        return CORPROF_E_DATAINCOMPLETE;

    if (pMethodDesc->HasClassOrMethodInstantiation() && pMethodDesc->IsTypicalMethodDefinition())
    {
        _ASSERTE(!"Profiler passed a typical method desc to GetCodeInfo2");
        return E_INVALIDARG;
    }

    ///////////////////////////////////
    // Get the code region info for this function. This is a multi step process.
    //
    // MethodDesc ==> Code Address ==> JitMananger ==>
    // MethodToken ==> MethodRegionInfo
    //
    // <WIN64-ONLY>
    //
    // On WIN64 we have a choice of where to go to find out the function code range size:
    // GC info (which is what we're doing below on all architectures) or the OS unwind
    // info, stored in the RUNTIME_FUNCTION structure.  The latter produces
    // a SMALLER size than the former, because we exclude some code from
    // the set we report to the OS for unwind info.  For example, jump tables can be
    // separated out from the regular code and not reported as OS unwind info,
    // because they're executed after the epilog.  OS unwind info rules state that
    // the code we report must be executed before the epilog so the OS knows where
    // the stack pointer is.  To avoid breaking the rules, we exclude code like this when
    // reporting unwind info, and such code will not appear in the range reported by
    // the RUNTIME_FUNCTION gotten via:
    //
    //      IJitManager* pJitMan = ExecutionManager::FindJitMan((PBYTE)codeInfos[0].startAddress);
    //      PRUNTIME_FUNCTION pfe = pJitMan->GetUnwindInfo((PBYTE)codeInfos[0].startAddress);
    //      *pcCodeInfos = (ULONG) (pfe->EndAddress - pfe->BeginAddress);
    //
    // (Note that GCInfo & OS unwind info report the same start address--it's the size that's
    // different.)
    //
    // The advantage of using the GC info is you get a more complete picture of the
    // addresses belonging to the function.  This is good for sampling profilers like F1 which
    // use GetCodeInfo2 to cache an address range for use later when determining whether
    // a sampled IP belongs to a managed function F1 previously queried about.
    //
    // A disadvantage of using GC info is we'll report those extra instructions (like jump
    // tables) that a profiler might turn back around and use in a call to
    // GetFunctionFromIP.  A profiler would expect we'd be able to map back any address
    // in the function's code range back to that function's FunctionID (methoddesc).  But querying
    // these extra instructions will cause GetFunctionFromIP to fail.  The advantage wins out,
    // so we're going with GC info everywhere.
    //
    // </WIN64-ONLY>

    IJitManager *pEEJM;

    LPCBYTE start = pMethodDesc->GetFunctionAddress(&pEEJM);

    if (start == NULL)
    {
        return CORPROF_E_FUNCTION_NOT_COMPILED;
    }

    METHODTOKEN methodtoken;
    pEEJM->JitCodeToMethodInfo((SLOT) start, NULL, &methodtoken);

    IJitManager::MethodRegionInfo methodRegionInfo;
    pEEJM->JitTokenToMethodRegionInfo(methodtoken, &methodRegionInfo);


    //
    // Fill out the codeInfo structures with valuse from the
    // methodRegion
    //
    // Note that we're assuming that a method will never be split into
    // more than two regions ... this is unlikely to change any time in
    // the near future.
    //
    if (NULL != codeInfos)
    {
        if (cCodeInfos > 0)
        {
            //
            // We have to return the two regions in the order that they would appear
            // if straight-line compiled
            //
            if (start == methodRegionInfo.hotStartAddress)
            {
                codeInfos[0].startAddress =
                    reinterpret_cast< UINT_PTR >(methodRegionInfo.hotStartAddress);
                codeInfos[0].size = methodRegionInfo.hotSize;
            }
            else
            {
                _ASSERTE(methodRegionInfo.coldStartAddress != NULL);
                codeInfos[0].startAddress =
                    reinterpret_cast< UINT_PTR >(methodRegionInfo.coldStartAddress);
                codeInfos[0].size = methodRegionInfo.coldSize;
            }

            if (NULL != methodRegionInfo.coldStartAddress)
            {
                if (cCodeInfos > 1)
                {
                    if (start == methodRegionInfo.hotStartAddress)
                    {
                        codeInfos[1].startAddress =
                            reinterpret_cast< UINT_PTR >(methodRegionInfo.coldStartAddress);
                        codeInfos[1].size = methodRegionInfo.coldSize;
                    }
                    else
                    {
                        codeInfos[1].startAddress =
                            reinterpret_cast< UINT_PTR >(methodRegionInfo.hotStartAddress);
                        codeInfos[1].size = methodRegionInfo.hotSize;
                    }
                }
            }
        }
    }

    if (NULL != pcCodeInfos)
    {
        *pcCodeInfos = (NULL != methodRegionInfo.coldStartAddress) ? 2 : 1;
    }


    return S_OK;
}


//
// ProfilerEnum
//
// This class is a one-size-fits-all implementation for COM style enumerators
//
// Template parameters:
//      EnumInterface -- the parent interface for this enumerator
//                       (e.g., ICorProfilerObjectEnum)
//      Element -- the type of the objects this enumerator returns.
//
//      pEnumInterfaceIID -- pointer to the class ID for this interface
//          (you probably don't need to use this)
//
//
template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
class ProfilerEnum : public EnumInterface
{
public:
    ProfilerEnum(CDynArray< Element >* elements);
    ProfilerEnum();
    ~ProfilerEnum();

    // IUnknown functions

    virtual HRESULT __stdcall QueryInterface(REFIID id, void** pInterface);
    virtual ULONG __stdcall AddRef();
    virtual ULONG __stdcall Release();


    // This template assumes that the enumerator confors to the interface
    //
    // (this matches the IEnumXXX interface documented in MSDN)

    virtual HRESULT __stdcall Skip(ULONG count);
    virtual HRESULT __stdcall Reset();
    virtual HRESULT __stdcall Clone(EnumInterface** ppEnum);
    virtual HRESULT __stdcall GetCount(ULONG *count);
    virtual HRESULT __stdcall Next(ULONG count,
                                   Element elements[],
                                   ULONG* elementsFetched);


private:
    ProfilerEnum(const ProfilerEnum& original);

    ULONG m_currentElement;

    CDynArray< Element > m_elements;

    LONG m_refCount;
};

typedef ProfilerEnum< ICorProfilerObjectEnum, ObjectID, &IID_ICorProfilerObjectEnum > ProfilerObjectEnum;




void ProfToEEInterfaceImpl::MethodTableCallback(void* context, void* objectUNSAFE)
{
    // each callback identifies the address of a method table within the frozen object segment
    // that pointer is an object ID by definition -- object references point to the method table
    CDynArray< ObjectID >* objects = reinterpret_cast< CDynArray< ObjectID >* >(context);

    *objects->Append() = reinterpret_cast< ObjectID >(objectUNSAFE);
}

void ProfToEEInterfaceImpl::ObjectRefCallback(void* context, void* objectUNSAFE)
{
    // we don't care about embedded object references, ignore them
}


//
// ProfToEEInterfaceImpl::EnumModuleFrozenObjects
//
// Description
//
// Parameters
//
// S_OK
//
//
HRESULT ProfToEEInterfaceImpl::EnumModuleFrozenObjects(ModuleID moduleID,
                                                       ICorProfilerObjectEnum** ppEnum)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: EnumModuleFrozenObjects 0x%p.\n", moduleID));             

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    if (NULL == ppEnum)
    {
        return E_INVALIDARG;
    }

    // If we don't support frozen objects at all, then just return "not
    // implemented". There's really no other sane value to return here

    if (NULL != ppEnum)
    {
        *ppEnum = NULL;
    }
    return E_NOTIMPL;
}



/*
 * GetArrayObjectInfo
 *
 * This function returns informatin about array objects.  In particular, the dimensions
 * and where the data buffer is stored.
 *
 */
HRESULT ProfToEEInterfaceImpl::GetArrayObjectInfo( ObjectID objectId,
                    ULONG32 cDimensionSizes,
                    ULONG32 pDimensionSizes[],
                    int pDimensionLowerBounds[],
                    BYTE    **ppData)
{
    // This is basically a public API so we need to do runtime checks.
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY; // fail at runtime if in preemp.
        SO_NOT_MAINLINE;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetArrayObjectInfo 0x%p.\n", objectId));             

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    HRESULT hr = AllowObjectInspection();
    if (FAILED(hr))
    {
        return hr;
    }

    Object *pObj = reinterpret_cast<Object *>(objectId);
    if (pObj == NULL)
    {
        return E_INVALIDARG;
    }
    if ((pDimensionSizes == NULL) ||
        (pDimensionLowerBounds == NULL) ||
        (ppData == NULL))
    {
        return E_INVALIDARG;
    }


    // Must have an array.
    MethodTable * pMT = pObj->GetTrueMethodTable();
    if (!pMT->IsArray())
    {
        return E_INVALIDARG;
    }

    ArrayBase * pArray = static_cast<ArrayBase*> (pObj);

    unsigned rank = pArray->GetRank();

    if (cDimensionSizes < rank)
    {
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
    }

    // Copy range for each dimension (rank)
    int * pBounds      = pArray->GetBoundsPtr();
    int * pLowerBounds = pArray->GetLowerBoundsPtr();

    unsigned i;
    for(i = 0; i < rank; i++)
    {
        pDimensionSizes[i]       = pBounds[i];
        pDimensionLowerBounds[i] = pLowerBounds[i];
    }

    // Pointer to data.
    *ppData = pArray->GetDataPtr();

    return S_OK;
}

/*
 * GetBoxClassLayout
 *
 * Returns information about how a particular value type is laid out.
 *
 */
HRESULT ProfToEEInterfaceImpl::GetBoxClassLayout(ClassID classId,
                                                ULONG32 *pBufferOffset)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetBoxClassLayout 0x%p.\n", classId));             

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    if (pBufferOffset == NULL)
    {
        return E_INVALIDARG;
    }

    if (classId == NULL)
    {
        return E_INVALIDARG;
    }

    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classId);

    //
    // This is the incorrect API for arrays.  Use GetArrayInfo and GetArrayLayout.
    //
    if (!typeHandle.IsValueType())
    {
        return E_INVALIDARG;
    }

    *pBufferOffset = Object::GetOffsetOfFirstField();

    return S_OK;
}

/*
 * GetThreadAppDomain
 *
 * Returns the app domain currently associated with the given thread.
 *
 */
HRESULT ProfToEEInterfaceImpl::GetThreadAppDomain(ThreadID threadId,
                                                  AppDomainID *pAppDomainId)

{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetThreadAppDomain 0x%p.\n", threadId));             

    // This call is allowed asynchronously, so we don't need to check the callback state flags

    if (pAppDomainId == NULL)
    {
        return E_INVALIDARG;
    }

    Thread *pThread;

    if (threadId == NULL)
    {
        pThread = GetThread();
    }
    else
    {
        pThread = (Thread *)threadId;
    }

    //
    // If pThread is null, then the thread has never run managed code and
    // so has no ThreadID.
    //
    if (pThread == NULL)
    {
        return CORPROF_E_NOT_MANAGED_THREAD;
    }

    *pAppDomainId = (AppDomainID)pThread->GetDomain();
    return S_OK;
}


/*
 * GetRVAStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class.  The field must be an RVA Static token.
 *
 * Parameters:
 *    classId - the containing class.
 *    fieldToken - the field we are querying.
 *    pAddress - location for storing the resulting address location.
 *
 * Returns:
 *    S_OK on success,
 *    E_INVALIDARG if not an RVA static,
 *    CORPROF_E_DATAINCOMPLETE if not yet initialized.
 *
 */
HRESULT ProfToEEInterfaceImpl::GetRVAStaticAddress(ClassID classId,
                                                   mdFieldDef fieldToken,
                                                   void **ppAddress)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetRVAStaticAddress 0x%p, 0x%08x.\n", classId, fieldToken));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // Check for NULL parameters
    //
    if ((classId == NULL) || (ppAddress == NULL))
    {
        return E_INVALIDARG;
    }

    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classId);

    //
    // Get the field descriptor object
    //
    FieldDesc *pFieldDesc = typeHandle.GetModule()->LookupFieldDef(fieldToken);

    if (pFieldDesc == NULL)
    {
        return E_INVALIDARG;
    }

    //
    // Verify this field is of the right type
    //
    if(!pFieldDesc->IsStatic() ||
       !pFieldDesc->IsRVA() ||
       pFieldDesc->IsThreadStatic() ||
       pFieldDesc->IsContextStatic())
    {
        return E_INVALIDARG;
    }

    MethodTable *pMethodTable = pFieldDesc->GetEnclosingMethodTable();

    //
    // Check that the data is available
    //
    if (!pMethodTable->IsRestored() || !pMethodTable->IsClassInited())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    //
    // Store the result and return
    //
    *ppAddress = pFieldDesc->GetStaticAddress(NULL);

    return S_OK;
}


/*
 * GetAppDomainStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class in the given app domain.  The field must be an App Domain Static token.
 *
 * Parameters:
 *    classId - the containing class.
 *    fieldToken - the field we are querying.
 *    appDomainId - the app domain container.
 *    pAddress - location for storing the resulting address location.
 *
 * Returns:
 *    S_OK on success,
 *    E_INVALIDARG if not an app domain static,
 *    CORPROF_E_DATAINCOMPLETE if not yet initialized.
 *
 */
HRESULT ProfToEEInterfaceImpl::GetAppDomainStaticAddress(ClassID classId,
                                                         mdFieldDef fieldToken,
                                                         AppDomainID appDomainId,
                                                         void **ppAddress)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetAppDomainStaticAddress 0x%p, 0x%08x, 0x%p.\n",
        classId, fieldToken, appDomainId));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // Check for NULL parameters
    //
    if ((classId == NULL) || (appDomainId == NULL) || (ppAddress == NULL))
    {
        return E_INVALIDARG;
    }

    // Some domains, like the system domain, aren't APP domains, and thus don't contain any
    // statics.  See if the profiler is trying to be naughty.
    if (!((BaseDomain*) appDomainId)->IsAppDomain())
    {
        return E_INVALIDARG;
    }

    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classId);

    //
    // Get the field descriptor object
    //
    FieldDesc *pFieldDesc = typeHandle.GetModule()->LookupFieldDef(fieldToken);

    if (pFieldDesc == NULL)
    {
        //
        // Give specific error code for literals.
        //
        DWORD dwFieldAttrs = typeHandle.GetModule()->GetMDImport()->GetFieldDefProps(fieldToken);

        if (IsFdLiteral(dwFieldAttrs))
        {
            return CORPROF_E_LITERALS_HAVE_NO_ADDRESS;
        }

        return E_INVALIDARG;
    }

    //
    // Verify this field is of the right type
    //
    if(!pFieldDesc->IsStatic() ||
       pFieldDesc->IsRVA() ||
       pFieldDesc->IsThreadStatic() ||
       pFieldDesc->IsContextStatic())
    {
        return E_INVALIDARG;
    }

    MethodTable *pMethodTable = pFieldDesc->GetEnclosingMethodTable();

    //
    // Check that the data is available
    //
    if (!pMethodTable->IsRestored() || !pMethodTable->IsClassInited((AppDomain *)appDomainId))
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    //
    // Get the address
    //
    void *base = (void*)pFieldDesc->GetBaseInDomain((AppDomain *)appDomainId);

    if (base == NULL)
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    //
    // Store the result and return
    //
    *ppAddress = pFieldDesc->GetStaticAddress(base);

    return S_OK;
}

/*
 * GetThreadStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class on the given thread.  The field must be an Thread Static token.
 *
 * Parameters:
 *    classId - the containing class.
 *    fieldToken - the field we are querying.
 *    threadId - the thread container.
 *    pAddress - location for storing the resulting address location.
 *
 * Returns:
 *    S_OK on success,
 *    E_INVALIDARG if not a thread static,
 *    CORPROF_E_DATAINCOMPLETE if not yet initialized.
 *
 */
HRESULT ProfToEEInterfaceImpl::GetThreadStaticAddress(ClassID classId,
                                                      mdFieldDef fieldToken,
                                                      ThreadID threadId,
                                                      void **ppAddress)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetThreadStaticAddress 0x%p, 0x%08x, 0x%p.\n",
        classId, fieldToken, threadId));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // Check for NULL parameters
    //
    if ((classId == NULL) || (ppAddress == NULL))
    {
        return E_INVALIDARG;
    }

    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classId);

    //
    // Get the field descriptor object
    //
    FieldDesc *pFieldDesc = typeHandle.GetModule()->LookupFieldDef(fieldToken);

    if (pFieldDesc == NULL)
    {
        return E_INVALIDARG;
    }

    //
    // Verify this field is of the right type
    //
    if(!pFieldDesc->IsStatic() ||
       !pFieldDesc->IsThreadStatic() ||
       pFieldDesc->IsRVA() ||
       pFieldDesc->IsContextStatic())
    {
        return E_INVALIDARG;
    }

    MethodTable *pMethodTable = pFieldDesc->GetEnclosingMethodTable();

    //
    // Check that the data is available
    //
    if (!pMethodTable->IsRestored() || !pMethodTable->IsClassInited())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    //
    // Get the thread
    //
    Thread *pThread;

    if (threadId == NULL)
    {
        pThread = GetThread();
    }
    else
    {
        pThread = (Thread *)threadId;
    }

    if (pThread == NULL)
    {
        return E_INVALIDARG;
    }

    //
    // Store the result and return
    //
    *ppAddress = (void*)(pThread->GetStaticFieldAddrNoCreate(pFieldDesc));

    return S_OK;
}

/*
 * GetContextStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class in the given context.  The field must be an Context Static token.
 *
 * Parameters:
 *    classId - the containing class.
 *    fieldToken - the field we are querying.
 *    contextId - the context container.
 *    pAddress - location for storing the resulting address location.
 *
 * Returns:
 *    S_OK on success,
 *    E_INVALIDARG if not a context static,
 *    CORPROF_E_DATAINCOMPLETE if not yet initialized.
 *
 */
HRESULT ProfToEEInterfaceImpl::GetContextStaticAddress(ClassID classId,
                                                       mdFieldDef fieldToken,
                                                       ContextID contextId,
                                                       void **ppAddress)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetContextStaticAddress 0x%p, 0x%08x, 0x%p.\n", 
        classId, fieldToken, contextId));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // Check for NULL parameters
    //
    if ((classId == NULL) || (contextId == NULL) || (ppAddress == NULL))
    {
        return E_INVALIDARG;
    }

    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classId);

    //
    // Get the field descriptor object
    //
    FieldDesc *pFieldDesc = typeHandle.GetModule()->LookupFieldDef(fieldToken);

    if (pFieldDesc == NULL)
    {
        return E_INVALIDARG;
    }

    //
    // Verify this field is of the right type
    //
    if(!pFieldDesc->IsStatic() ||
       !pFieldDesc->IsContextStatic() ||
       pFieldDesc->IsRVA() ||
       pFieldDesc->IsThreadStatic())
    {
        return E_INVALIDARG;
    }

    MethodTable *pMethodTable = pFieldDesc->GetEnclosingMethodTable();

    //
    // Check that the data is available
    //
    if (!pMethodTable->IsRestored() || !pMethodTable->IsClassInited())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    //
    // Get the context
    //
    Context *pContext = reinterpret_cast<Context *>(contextId);

    //
    // Store the result and return
    //
    *ppAddress = pContext->GetStaticFieldAddrNoCreate(pFieldDesc);

    return S_OK;
}


/*
 * GetStaticFieldInfo
 *
 * This function returns a bit mask of the type of statics the
 * given field is.
 *
 * Parameters:
 *    classId - the containing class.
 *    fieldToken - the field we are querying.
 *    pFieldInfo - location for storing the resulting bit mask.
 *
 * Returns:
 *    S_OK on success,
 *    E_INVALIDARG if pFieldInfo is NULL
 *
 */
HRESULT ProfToEEInterfaceImpl::GetStaticFieldInfo(ClassID classId,
                                                  mdFieldDef fieldToken,
                                                  COR_PRF_STATIC_TYPE *pFieldInfo)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetStaticFieldInfo 0x%p, 0x%08x.\n", 
        classId, fieldToken));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // Check for NULL parameters
    //
    if ((classId == NULL) || (pFieldInfo == NULL))
    {
        return E_INVALIDARG;
    }

    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classId);

    //
    // Get the field descriptor object
    //
    FieldDesc *pFieldDesc = typeHandle.GetModule()->LookupFieldDef(fieldToken);

    if (pFieldDesc == NULL)
    {
        return E_INVALIDARG;
    }

    *pFieldInfo = COR_PRF_FIELD_NOT_A_STATIC;

    if (pFieldDesc->IsContextStatic())
    {
        *pFieldInfo = (COR_PRF_STATIC_TYPE)(*pFieldInfo | COR_PRF_FIELD_CONTEXT_STATIC);
    }

    if (pFieldDesc->IsRVA())
    {
        *pFieldInfo = (COR_PRF_STATIC_TYPE)(*pFieldInfo | COR_PRF_FIELD_RVA_STATIC);
    }

    if (pFieldDesc->IsThreadStatic())
    {
        *pFieldInfo = (COR_PRF_STATIC_TYPE)(*pFieldInfo | COR_PRF_FIELD_THREAD_STATIC);
    }

    if ((*pFieldInfo == COR_PRF_FIELD_NOT_A_STATIC) && pFieldDesc->IsStatic())
    {
        *pFieldInfo = (COR_PRF_STATIC_TYPE)(*pFieldInfo | COR_PRF_FIELD_APP_DOMAIN_STATIC);
    }

    return S_OK;
}



/*
 * GetClassIDInfo2
 *
 * This function generalizes GetClassIDInfo for all types, both generic and non-generic.  It returns
 * the module, type token, and an array of instantiation classIDs that were used to instantiate the
 * given classId.
 *
 * Parameters:
 *   classId - The classId (TypeHandle) to query information about.
 *   pParentClassId - The ClassID (TypeHandle) of the parent class.
 *   pModuleId - An optional parameter for returning the module of the class.
 *   pTypeDefToken - An optional parameter for returning the metadata token of the class.
 *   cNumTypeArgs - The count of the size of the array typeArgs
 *   pcNumTypeArgs - Returns the number of elements of typeArgs filled in, or if typeArgs is NULL
 *         the number that would be needed.
 *   typeArgs - An array to store generic type parameters for the class.
 *
 * Returns:
 *   S_OK if successful.
 */
HRESULT ProfToEEInterfaceImpl::GetClassIDInfo2(ClassID classId,
                                            ModuleID *pModuleId,
                                            mdTypeDef *pTypeDefToken,
                                            ClassID *pParentClassId,
                                            ULONG32 cNumTypeArgs,
                                            ULONG32 *pcNumTypeArgs,
                                            ClassID typeArgs[])
{

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(pParentClassId, NULL_OK));
        PRECONDITION(CheckPointer(pModuleId, NULL_OK));
        PRECONDITION(CheckPointer(pTypeDefToken,  NULL_OK));
        PRECONDITION(CheckPointer(pcNumTypeArgs, NULL_OK));
        PRECONDITION(CheckPointer(typeArgs, NULL_OK));
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetClassIDInfo2 0x%p.\n", classId));
    
    // This call is allowed asynchronously, so we don't need to check the callback state flags
    
    //
    // Verify parameters.
    //
    if (classId == NULL)
    {
        return E_INVALIDARG;
    }

    if ((cNumTypeArgs != 0) && (typeArgs == NULL))
    {
        return E_INVALIDARG;
    }

    //
    // Handle globals which don't have the instances.
    //
    if (classId == PROFILER_GLOBAL_CLASS)
    {
        if (pParentClassId != NULL)
        {
            *pParentClassId = NULL;
        }

        if (pModuleId != NULL)
        {
            *pModuleId = PROFILER_GLOBAL_MODULE;
        }

        if (pTypeDefToken != NULL)
        {
            *pTypeDefToken = mdTokenNil;
        }

        return S_OK;
    }

    //
    // Get specific data.
    //
    TypeHandle th = TypeHandle::FromPtr((void *)classId);

    //
    // Do not do arrays via this API
    //
    ARRAY_KIND arrayKind = ArrayKindFromTypeHandle(th);    
    if (arrayKind == ARRAY_KIND_TYPEDESC || arrayKind == ARRAY_KIND_METHODTABLE)
    {
        return CORPROF_E_CLASSID_IS_ARRAY;
    }

    _ASSERTE (arrayKind == ARRAY_KIND_NOTARRAY);
    
    if (th.IsTypeDesc())
    {
        // Not an array, but still a typedesc?  We don't know how to
        // deal with those.
        return CORPROF_E_CLASSID_IS_COMPOSITE;
    }

    //
    // Fill in the basic information
    //
    if (pParentClassId != NULL)
    {
        TypeHandle parentTypeHandle = th.GetParent();
        if (!parentTypeHandle.IsNull())
        {
            *pParentClassId = TypeHandleToClassID(parentTypeHandle);
        }
        else
        {
            *pParentClassId = NULL;
        }
    }

    if (pModuleId != NULL)
    {
        *pModuleId = (ModuleID) th.GetModule();
        _ASSERTE(*pModuleId != NULL);
    }

    if (pTypeDefToken != NULL)
    {
        *pTypeDefToken = th.GetCl();
        _ASSERTE(*pTypeDefToken != NULL);
    }

    //
    // See if they are just looking to get the buffer size.
    //
    if (cNumTypeArgs == 0)
    {
        if (pcNumTypeArgs != NULL)
        {
            *pcNumTypeArgs = th.GetMethodTable()->GetNumGenericArgs();
        }
        return S_OK;
    }

    //
    // Adjust the count for the size of the given array.
    //
    if (cNumTypeArgs > th.GetMethodTable()->GetNumGenericArgs())
    {
        cNumTypeArgs = th.GetMethodTable()->GetNumGenericArgs();
    }

    if (pcNumTypeArgs != NULL)
    {
        *pcNumTypeArgs = cNumTypeArgs;
    }

    //
    // Copy over the instantiating types.
    //
    ULONG32 count;
    TypeHandle *instantiations = th.GetMethodTable()->GetInstantiation();
    PREFIX_ASSUME(cNumTypeArgs == 0 || instantiations != NULL);

    for (count = 0; count < cNumTypeArgs; count ++)
    {
        typeArgs[count] = TypeHandleToClassID(instantiations[count]);
    }

    return S_OK;
}

HRESULT ProfToEEInterfaceImpl::GetModuleInfo(
    ModuleID    moduleId,
    LPCBYTE     *ppBaseLoadAddress,
    ULONG       cchName,
    ULONG      *pcchName,
    WCHAR       szName[],
    AssemblyID  *pAssemblyId)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER; 
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer((Module *)moduleId));
        PRECONDITION(CheckPointer(ppBaseLoadAddress,  NULL_OK));
        PRECONDITION(CheckPointer(pcchName, NULL_OK));
        PRECONDITION(CheckPointer(szName, NULL_OK));
        PRECONDITION(CheckPointer(pAssemblyId, NULL_OK));
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetModuleInfo 0x%p.\n", moduleId));    

    // This call is allowed asynchronously, so we don't need to check the callback state flags
    
    Module * pModule = (Module *) moduleId;
    if (pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }
    
    HRESULT     hr = S_OK;

    EX_TRY
    {
        
        PEFile *pFile = pModule->GetFile();

        // Pick some safe defaults to begin with.
        if (ppBaseLoadAddress)
            *ppBaseLoadAddress = 0;
        if (szName)
            *szName = 0;
        if (pcchName)
            *pcchName = 0;
        if (pAssemblyId)
            *pAssemblyId = PROFILER_PARENT_UNKNOWN;

        // Get the module file name
        LPCWSTR pFileName = pFile->GetPath();
        _ASSERTE(pFileName != NULL);
        PREFIX_ASSUME(pFileName != NULL);

        ULONG trueLen = (ULONG)(wcslen(pFileName) + 1);

        // Return name of module as required.
        if (szName && cchName > 0)
        {
            if (cchName < trueLen)
            {
                hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
            }
            else
            {
                wcsncpy_s(szName, cchName, pFileName, trueLen);
            }
        }

        // If they request the actual length of the name
        if (pcchName)
            *pcchName = trueLen;

        if (ppBaseLoadAddress != NULL && !pFile->IsDynamic())
        {
            if (pModule->IsProfilerNotified())
            {
                // Set the base load address -- this could be null in certain error conditions
                *ppBaseLoadAddress = pModule->GetProfilerBase();
            }
            else
            {
                *ppBaseLoadAddress = NULL;
            }

            if (*ppBaseLoadAddress == NULL)
            {
                hr = CORPROF_E_DATAINCOMPLETE;
            }
        }

        // Return the parent assembly for this module if desired.
        if (pAssemblyId != NULL)
        {
            // Lie and say the assembly isn't avaialable until we are loaded (even though it is.)
            // This is for backward compatibilty - we may want to change it
            if (pModule->IsProfilerNotified())
            {
                Assembly *pAssembly = pModule->GetAssembly();
                _ASSERTE(pAssembly);

                *pAssemblyId = (AssemblyID) pAssembly;
            }
            else
            {
                hr = CORPROF_E_DATAINCOMPLETE;
            }
        }
    }
    EX_CATCH_HRESULT(hr);

    return (hr);
}


/*
 * Get a metadata interface insance which maps to the given module.
 * One may ask for the metadata to be opened in read+write mode, but
 * this will result in slower metadata execution of the program, because
 * changes made to the metadata cannot be optimized as they were from
 * the compiler.
 */
HRESULT ProfToEEInterfaceImpl::GetModuleMetaData(
    ModuleID    moduleId,
    DWORD       dwOpenFlags,
    REFIID      riid,
    IUnknown    **ppOut)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetModuleMetaData 0x%p, 0x%08x.\n", moduleId, dwOpenFlags));
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    Module      *pModule;
    HRESULT     hr = S_OK;

    pModule = (Module *) moduleId;
    _ASSERTE(pModule != NULL);  // Already checked in CorProfInfo::GetModuleMetaData
    if (pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    // Make sure we can get the importer first
    if (pModule->IsResource())
    {
        if (ppOut)
            *ppOut = NULL;
        hr = S_FALSE;
    }
    else
    {
        IUnknown *pObj = NULL;
        EX_TRY
        {
            // Decide which type of open mode we are in to see which you require.
            if (dwOpenFlags & ofWrite)
                pObj = pModule->GetEmitter();
            else
                pObj = pModule->GetRWImporter();
        }
        EX_CATCH_HRESULT(hr);

        // Ask for the interface the caller wanted, only if they provide a out param
        if (SUCCEEDED(hr) && ppOut)
            hr = pObj->QueryInterface(riid, (void **) ppOut);
    }

    return (hr);
}


/*
 * Retrieve a pointer to the body of a method starting at it's header.
 * A method is scoped by the module it lives in.  Because this function
 * is designed to give a tool access to IL before it has been loaded
 * by the Runtime, it uses the metadata token of the method to find
 * the instance desired.  Note that this function has no effect on
 * already compiled code.
 */
HRESULT ProfToEEInterfaceImpl::GetILFunctionBody(
    ModuleID    moduleId,
    mdMethodDef methodId,
    LPCBYTE     *ppMethodHeader,
    ULONG       *pcbMethodSize)
{
    Module      *pModule;               // Working pointer for real class.
    ULONG       RVA;                    // Return RVA of the method body.
    DWORD       dwImplFlags;            // Flags for the item.
    ULONG       cbExtra;                // Extra bytes past code (eg, exception table)

    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetILFunctionBody 0x%p, 0x%08x.\n", moduleId, methodId));    
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    pModule = (Module *) moduleId;
    _ASSERTE(pModule != NULL && methodId != mdMethodDefNil);
    if (pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    // Find the method body based on metadata.
    IMDInternalImport *pImport = pModule->GetMDImport();
    _ASSERTE(pImport);

    PEFile *pFile = pModule->GetFile();

    if (!pFile->CheckLoaded())
        return (CORPROF_E_DATAINCOMPLETE);

    pImport->GetMethodImplProps(methodId, &RVA, &dwImplFlags);

    // Check to see if the method has associated IL
    if ((RVA == 0 && !pFile->IsDynamic()) || !(IsMiIL(dwImplFlags) || IsMiOPTIL(dwImplFlags) || IsMiInternalCall(dwImplFlags)))
        return (CORPROF_E_FUNCTION_NOT_IL);

    // Get the location of the IL
    LPCBYTE pbMethod = (LPCBYTE) (pModule->GetIL(RVA));

    // Fill out param if provided
    if (ppMethodHeader)
        *ppMethodHeader = pbMethod;

    // Calculate the size of the method itself.
    if (pcbMethodSize)
    {
        if (((COR_ILMETHOD_FAT *)pbMethod)->IsFat())
        {
            COR_ILMETHOD_FAT *pMethod = (COR_ILMETHOD_FAT *)pbMethod;
            cbExtra = 0;

            // Also look for variable sized data that comes after the method itself.
            const COR_ILMETHOD_SECT *pFirst = pMethod->GetSect();
            const COR_ILMETHOD_SECT *pLast = pFirst;
            if (pFirst)
            {
                // Skip to the last extra sect.
                while (pLast->More())
                    pLast = pLast->NextLoc();

                // Skip to where next sect would be
                pLast = pLast->NextLoc();

                // Extra is delta from first extra sect to first sect past this method.
                cbExtra = (ULONG)((BYTE *) pLast - (BYTE *) pFirst);
            }

            *pcbMethodSize = pMethod->GetSize() * 4;
            *pcbMethodSize += pMethod->GetCodeSize();
            *pcbMethodSize += cbExtra;
        }
        else
        {
            // Make sure no one has added any other header type
            _ASSERTE(((COR_ILMETHOD_TINY *)pbMethod)->IsTiny() && "PROFILER: Unrecognized header type.");

            COR_ILMETHOD_TINY *pMethod = (COR_ILMETHOD_TINY *)pbMethod;

            *pcbMethodSize = sizeof(COR_ILMETHOD_TINY);
            *pcbMethodSize += ((COR_ILMETHOD_TINY *) pMethod)->GetCodeSize();
        }
    }

    return (S_OK);
}


/*
 * IL method bodies must be located as RVA's to the loaded module, which
 * means they come after the module within 4 gb.  In order to make it
 * easier for a tool to swap out the body of a method, this allocator
 * will ensure memory allocated after that point.
 */
HRESULT ProfToEEInterfaceImpl::GetILFunctionBodyAllocator(
    ModuleID    moduleId,
    IMethodMalloc **ppMalloc)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;         // ModuleILHeap::CreateNew throws
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetILFunctionBodyAllocator 0x%p.\n", moduleId));
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    Module      *pModule;               // Working pointer for real class.
    HRESULT     hr;

    pModule = (Module *) moduleId;

    if (pModule->IsBeingUnloaded() ||
        !pModule->GetFile()->CheckLoaded())
    {
        return (CORPROF_E_DATAINCOMPLETE);
    }

    BYTE *base;

    if (pModule->IsReflection() || pModule->IsResource())
        base = NULL;
    else
        base = pModule->GetFile()->GetPhonyILBase();

    hr = ModuleILHeap::CreateNew(IID_IMethodMalloc, (void **) ppMalloc,
                                 (LPCBYTE) base, this, pModule);
    return (hr);
}


/*
 * Replaces the method body for a function in a module.  This will replace
 * the RVA of the method in the metadata to point to this new method body,
 * and adjust any internal data structures as required.  This function can
 * only be called on those methods which have never been compiled by a JITTER.
 * Please use the GetILFunctionBodyAllocator to allocate space for the new method to
 * ensure the buffer is compatible.
 */
HRESULT ProfToEEInterfaceImpl::SetILFunctionBody(
    ModuleID    moduleId,
    mdMethodDef methodId,
    LPCBYTE     pbNewILMethodHeader)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;             // PEFile::GetEmitter, MethodDesc::SetRVA all throw
        GC_TRIGGERS;    // AppDomainIterator::Next triggers
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: SetILFunctionBody 0x%p, 0x%08x.\n", moduleId, methodId));        
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    Module      *pModule;               // Working pointer for real class.
    ULONG       rva;                    // Location of code.
    HRESULT     hr = S_OK;

    // Cannot set the body for anything other than a method def
    if (TypeFromToken(methodId) != mdtMethodDef)
        return (E_INVALIDARG);

    // Cast module to appropriate type
    pModule = (Module *) moduleId;
    _ASSERTE (pModule != NULL); // Enforced in CorProfInfo::SetILFunctionBody
    if (pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    if (pModule->IsReflection())
    {
        ReflectionModule *pIMM = (ReflectionModule *)pModule;
        ICeeGen *pICG = pIMM->GetCeeGen();
        _ASSERTE(pICG != NULL);

        if (pICG != NULL)
        {
            HCEESECTION hCeeSection;
            pICG->GetIlSection(&hCeeSection);

            CeeSection *pCeeSection = (CeeSection *)hCeeSection;
            if ((rva = pCeeSection->computeOffset((char *)pbNewILMethodHeader)) != 0)
            {
                // Lookup the method desc
                MethodDesc *pDesc = LookupMethodDescFromMethodDef(methodId, pModule);
                _ASSERTE(pDesc != NULL);
                PREFIX_ASSUME(pDesc != NULL);

                // Set the RVA in the desc
                pDesc->SetRVA(rva);

                // Set the RVA in the metadata
                IMetaDataEmit *pEmit = pDesc->GetEmitter();
                pEmit->SetRVA(methodId, rva);
            }
            else
                hr = E_FAIL;
        }
        else
            hr = E_FAIL;
    }
    else
    {
        // If the module is not initialized, then there probably isn't a valid IL base yet.
        if (!pModule->GetFile()->CheckLoaded())
            return (CORPROF_E_DATAINCOMPLETE);

        // Sanity check the new method body.
        if (pbNewILMethodHeader <= (LPCBYTE) pModule->GetPhonyILBase())
        {
            _ASSERTE(!"Bogus RVA for new method, need to use our allocator");
            return E_INVALIDARG;
        }

        // Find the RVA for the new method and replace this in metadata.
        rva = (ULONG) pModule->GetPhonyILRva((BYTE*)pbNewILMethodHeader);
        _ASSERTE(rva < (ULONG)~0);

        // Get the metadata emitter
        IMetaDataEmit *pEmit = pModule->GetEmitter();

        // Set the new RVA
        hr = pEmit->SetRVA(methodId, rva);

        // If the method has already been instantiated, then we must go whack the
        // RVA address in the MethodDesc
        if (hr == S_OK)
        {
            AppDomainIterator ADIter(FALSE /*OnlyOpen*/);
            while (ADIter.Next())
            {
                LoadedMethodDescIterator MDIter(ADIter.GetDomain(), pModule, methodId);
                while(MDIter.Next())
                {
                    MethodDesc * pMD = MDIter.Current();
                    if (pMD)
                    {
                        _ASSERTE(pMD->IsIL());
                        pMD->SetRVA(rva);
                    }
                }
            }
        }
    }

    return (hr);
}

/*
 * Sets the codemap for the replaced IL function body
 */
HRESULT ProfToEEInterfaceImpl::SetILInstrumentedCodeMap(
        FunctionID functionId,
        BOOL fStartJit,
        ULONG cILMapEntries,
        COR_IL_MAP rgILMapEntries[])
{

    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: SetILInstrumentedCodeMap 0x%p, %d.\n", functionId, fStartJit));        
       
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
#ifdef DEBUGGING_SUPPORTED

    MethodDesc *pMethodDesc = FunctionIdToMethodDesc(functionId);

    // it's not safe to examine a methoddesc that has not been restored so do not do so
    if (!pMethodDesc ->IsRestored())
        return CORPROF_E_DATAINCOMPLETE;

    // If we're tracking JIT info for this module, then update the bits
    if (CORDebuggerTrackJITInfo(pMethodDesc->GetModule()->GetDebuggerInfoBits()))
    {
        return g_pDebugInterface->SetILInstrumentedCodeMap(pMethodDesc,
                                                           fStartJit,
                                                           cILMapEntries,
                                                           rgILMapEntries);
    }
    else
    {
        // Throw it on the floor & life is good
        CoTaskMemFree(rgILMapEntries);
        return S_OK;
    }

#else //DEBUGGING_SUPPORTED

        return E_NOTIMPL;

#endif //DEBUGGING_SUPPORTED
}

HRESULT ProfToEEInterfaceImpl::ForceGC()
{
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: ForceGC.\n"));        

    // This call is allowed asynchronously, so we don't need to check the callback state flags
    // At first glance, this might appear suicidal, but if you check the next line below, we ensure
    // that the current thread has not run any managed code, so it's actually quite
    // safe for the current thread to become the GC thread and wait for the rest
    // of the threads to cooperate with the GC

    if (GetThread() != NULL)
        return (E_FAIL);

    if (GCHeap::GetGCHeap() == NULL)
        return (CORPROF_E_NOT_YET_AVAILABLE);

    // -1 Indicates that all generations should be collected
    return (GCHeap::GetGCHeap()->GarbageCollect(-1));
}


/*
 * Returns the ContextID for the current thread.
 */
HRESULT ProfToEEInterfaceImpl::GetThreadContext(ThreadID threadId,
                                                ContextID *pContextId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetThreadContext 0x%p.\n", threadId));
    
    // This call is allowed asynchronously, so we don't need to check the callback state flags

    // Cast to right type
    Thread *pThread = reinterpret_cast<Thread *>(threadId);

    // Get the context for the Thread* provided
    Context *pContext = pThread->GetContext();
    _ASSERTE(pContext);

    // If there's no current context, return incomplete info
    if (!pContext)
        return (CORPROF_E_DATAINCOMPLETE);

    // Set the result and return
    if (pContextId)
        *pContextId = reinterpret_cast<ContextID>(pContext);

    return (S_OK);
}

HRESULT ProfToEEInterfaceImpl::GetClassIDInfo(
    ClassID classId,
    ModuleID *pModuleId,
    mdTypeDef *pTypeDefToken)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetClassIDInfo 0x%p.\n", classId));

    // This call is allowed asynchronously, so we don't need to check the callback state flags
    
    if (pModuleId != NULL)
        *pModuleId = NULL;

    if (pTypeDefToken != NULL)
        *pTypeDefToken = NULL;

    // Handle globals which don't have the instances.
    if (classId == PROFILER_GLOBAL_CLASS)
    {
        if (pModuleId != NULL)
            *pModuleId = PROFILER_GLOBAL_MODULE;

        if (pTypeDefToken != NULL)
            *pTypeDefToken = mdTokenNil;
    }
    else if (classId == NULL)
    {
        return E_INVALIDARG;
    }
    // Get specific data.
    else
    {
        TypeHandle th = TypeHandle::FromPtr((void *)classId);

        if (!th.IsTypeDesc())
        {
            if (!th.IsArray())
            {
                if (pModuleId != NULL)
                {
                    *pModuleId = (ModuleID) th.GetModule();
                    _ASSERTE(*pModuleId != NULL);
                }

                if (pTypeDefToken != NULL)
                {
                    *pTypeDefToken = th.GetCl();
                    _ASSERTE(*pTypeDefToken != NULL);
                }
            }
        }
    }

    return (S_OK);
}


HRESULT ProfToEEInterfaceImpl::GetFunctionInfo(
    FunctionID functionId,
    ClassID *pClassId,
    ModuleID *pModuleId,
    mdToken *pToken)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetFunctionInfo 0x%p.\n", functionId));
    
    // This call is allowed asynchronously, so we don't need to check the callback state flags

    MethodDesc *pMDesc = (MethodDesc *) functionId;
    MethodTable *pMT = pMDesc->GetMethodTable();

    ClassID classId = PROFILER_GLOBAL_CLASS;

    if (pMT != NULL)
    {
        classId = GetClassIdForNonGenericType(TypeHandleToTypeID(TypeHandle(pMT)));
    }

    if (pClassId != NULL)
    {
        *pClassId = classId;
    }

    if (pModuleId != NULL)
    {
        *pModuleId = (ModuleID) pMDesc->GetModule();
    }

    if (pToken != NULL)
    {
        *pToken = pMDesc->GetMemberDef();
    }

    return (S_OK);
}

/*
 * GetILToNativeMapping returns a map from IL offsets to native
 * offsets for this code. An array of COR_DEBUG_IL_TO_NATIVE_MAP
 * structs will be returned, and some of the ilOffsets in this array
 * may be the values specified in CorDebugIlToNativeMappingTypes.
 */
HRESULT ProfToEEInterfaceImpl::GetILToNativeMapping(
            /* [in] */  CodeID codeId,
            /* [in] */  ULONG32 cMap,
            /* [out] */ ULONG32 *pcMap,
            /* [out, size_is(cMap), length_is(*pcMap)] */
                COR_DEBUG_IL_TO_NATIVE_MAP map[])
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;                     // MethodDesc::FindOrCreateInstantiationAtObject throws
        GC_TRIGGERS;                // MethodDesc::FindOrCreateInstantiationAtObject triggers
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetILToNativeMapping 0x%p.\n", codeId));
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    // If JIT maps are not enabled, then we can't provide it
    if (!CORProfilerJITMapEnabled())
        return (CORPROF_E_JITMAPS_NOT_ENABLED);

#ifdef DEBUGGING_SUPPORTED
    // Cast to proper type
    MethodDesc *pMD = CodeIDToMethodDesc(codeId);

    // If the code is generic, then GetCodeInfo is not really supported and may not give
    // accurate results.  This is better than always failing,
    // though we may prejudice profiling by which
    // copy of the code we return, so we just return none.
    if (pMD->HasClassOrMethodInstantiation() && pMD->IsTypicalMethodDefinition())
        pMD = pMD->FindOrCreateInstantiationAtObject();  // Can we pass allowCreate=FALSE to make this no trigger?

    return (g_pDebugInterface->GetILToNativeMapping(pMD, cMap, pcMap, map));
#else
    return E_NOTIMPL;
#endif
}

/*
 * This tries to reserve memory of size dwMemSize as high up in virtual memory
 * as possible, and return a heap that manages it.
 */
HRESULT ProfToEEInterfaceImpl::NewHeap(ExplicitControlLoaderHeap **ppHeap, LPCBYTE pBase, DWORD dwMemSize)
{
    SO_NOT_MAINLINE_FUNCTION;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: NewHeap 0x%p, 0x%08x.\n", pBase, dwMemSize));
    
    HRESULT hr = S_OK;
    *ppHeap = NULL;

    // Create a new loader heap we can use for suballocation.
    ExplicitControlLoaderHeap *pHeap = new ExplicitControlLoaderHeap(4096, 0);

    if (!pHeap)
    {
        hr = E_OUTOFMEMORY;
        goto ErrExit;
    }

    // Note: its important to use pBase + METHOD_MAX_RVA as the upper limit on the allocation!
    if (!pHeap->ReservePages(0, NULL, dwMemSize, pBase, (PBYTE)((UINT_PTR)pBase + (UINT_PTR)METHOD_MAX_RVA), FALSE))
    {
        hr = E_OUTOFMEMORY;
        goto ErrExit;
    }

    // Succeeded, so return the created heap
    *ppHeap = pHeap;

ErrExit:
    if (FAILED(hr))
    {
        if (pHeap)
            delete pHeap;
    }

    return (hr);
}

/*
 * This will add a heap to the list of heaps available for allocations.
 */
HRESULT ProfToEEInterfaceImpl::AddHeap(ExplicitControlLoaderHeap *pHeap)
{
    SO_NOT_MAINLINE_FUNCTION;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: AddHeap 0x%p.\n", pHeap));    
    
    HRESULT hr = S_OK;

    HeapList *pElem = new HeapList(pHeap);
    if (!pElem)
    {
        hr = E_OUTOFMEMORY;
        goto ErrExit;
    }

    // For now, add it to the front of the list
    pElem->m_pNext = m_pHeapList;
    m_pHeapList = pElem;

ErrExit:
    if (FAILED(hr))
    {
        if (pElem)
            delete pElem;
    }

    return (hr);
}

/*
 * This allocates memory for use as an IL method body.
 */
void *ProfToEEInterfaceImpl::Alloc(LPCBYTE pBase, ULONG cb, Module *pModule)
{
    SO_NOT_MAINLINE_FUNCTION;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: Alloc 0x%p, 0x%08x, 0x%p.\n", pBase, cb, pModule));
        
    _ASSERTE(pBase != 0 || pModule->IsReflection() || pModule->IsResource());

    if (cb == 0)
    {
        return NULL;
    }

    LPBYTE pb = NULL;
    ExplicitControlLoaderHeap *pHeap = NULL;

    if (pModule->IsReflection())
    {
        ReflectionModule *pIMM = (ReflectionModule *)pModule;
        ICeeGen *pICG = pIMM->GetCeeGen();
        _ASSERTE(pICG != NULL);

        if (pICG != NULL)
        {
            ULONG RVA;  // Dummy - will compute later
            pICG->AllocateMethodBuffer(cb, (UCHAR **) &pb, &RVA);
        }
    }
    else
    {
        // Now try to allocate the memory
        HRESULT hr = S_OK;
        HeapList **ppCurHeap = &m_pHeapList;
        while (*ppCurHeap && !pb)
        {
            // Note: its important to use pBase + METHOD_MAX_RVA as the upper limit on the allocation!
            if ((*ppCurHeap)->m_pHeap->CanAllocMemWithinRange((size_t) cb, (BYTE *)pBase,
                                                              (BYTE *)((UINT_PTR)pBase + (UINT_PTR)METHOD_MAX_RVA), FALSE))
            {
                pb = (LPBYTE) (*ppCurHeap)->m_pHeap->AllocMem_NoThrow(cb);

                if (pb)
                {
                    break;
                }
            }

            ppCurHeap = &((*ppCurHeap)->m_pNext);
        }

        // If we failed to allocate memory, grow the heap
        if (!pb)
        {
            // Free reserved at top of virtual address space in case module is near top 
            // in order to try and have allocation succeed (since RVA must be greater than module base)
            if (g_profControlBlock.pReservedMem)
            {
                ClrVirtualFree(g_profControlBlock.pReservedMem, 0, MEM_RELEASE);
                g_profControlBlock.pReservedMem = NULL;
            }
            
            // Create new heap, reserving at least a meg at a time
            // Add sizeof(LoaderHeapBlock) to requested size since
            // beginning of the heap is used by the heap manager, and will
            // fail if an exact size over 1 meg is requested if we
            // don't compensate
            EX_TRY          // AllocMemNoGrow can throw if it runs out of memory
            {
                hr = NewHeap(&pHeap, pBase, max(cb + sizeof(LoaderHeapBlock), 0x1000*8));
                if (FAILED(hr))
                {
                    goto Error;
                }
                
                hr = AddHeap(pHeap);
                if (FAILED(hr))
                {
                    goto Error;
                }

                // Try to make allocation once more
                pb = (LPBYTE) pHeap->AllocMemNoGrow(cb);
                if (pb == NULL)
                {
                    goto Error;
                }
            }
            EX_CATCH
            {
                goto Error;
            }
            EX_END_CATCH(RethrowTerminalExceptions);            
        }

        // Pointer must come *after* the base or we are in bad shape.
        _ASSERTE(pb > pBase);
    }

    // Limit ourselves to RVA's that fit within the MethodDesc.
    if ((ULONG_PTR)(pb - pBase) >= (ULONG_PTR)METHOD_MAX_RVA)
    {
        goto Error;
    }

    return ((void *) pb);

Error:
    if (pHeap)
        delete pHeap;

    return NULL;
}


//*****************************************************************************
// Given an ObjectID, go get the EE ClassID for it.
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::GetTypeFromObject(
    ObjectID objectId,
    TypeID *pTypeId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetTypeFromObject 0x%p.\n", objectId));    
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    HRESULT hr = AllowObjectInspection();
    if (FAILED(hr))
    {
        return hr;
    }

    _ASSERTE(objectId);

    // Cast the ObjectID as a Object
    Object *pObj = reinterpret_cast<Object *>(objectId);

    // Set the out param and indicate success
    // Note that for generic code we always return uninstantiated ClassIDs and FunctionIDs
    if (pTypeId)
        *pTypeId = TypeHandleToTypeID(pObj->GetTypeHandle());

    return (S_OK);
}

//*****************************************************************************
// Given a module and a token for a class, go get the EE data structure for it.
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::GetClassFromToken(
    ModuleID    moduleId,
    mdTypeDef   typeDef,
    ClassID     *pClassId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_TRIGGERS;        // ClassLoader::LoadTypeDefOrRefNoThrow triggers
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetClassFromToken 0x%p, 0x%08x.\n", moduleId, typeDef));    
        
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    // Get the module
    Module *pModule = (Module *) moduleId;

    // No module, or it's disassociated from metadata
    if (pModule == NULL || pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    // First, check the RID map. This is important since it
    // works during teardown (and the below doesn't)
    TypeHandle th;
    th = pModule->LookupTypeDef(typeDef);
    if (th.IsNull())
    {
        HRESULT hr = ClassLoader::LoadTypeDefOrRefNoThrow(pModule, typeDef, &th, NULL,
                                                          ClassLoader::ThrowIfNotFound,
                                                          ClassLoader::PermitUninstDefOrRef);

        if (FAILED(hr))
        {
            return hr;
        }
    }

    if (!th.GetMethodTable())
        return (CORPROF_E_DATAINCOMPLETE);

    //
    // Check if it is generic
    //
    ClassID classId = GetClassIdForNonGenericType(TypeHandleToTypeID(th));

    if (classId == NULL)
    {
        return CORPROF_E_TYPE_IS_PARAMETERIZED;
    }

    // Return value if necessary
    if (pClassId)
    {
        *pClassId = classId;
    }

    return (S_OK);
}


HRESULT ProfToEEInterfaceImpl::GetClassFromTokenAndTypeArgs(ModuleID moduleID,
                                                            mdTypeDef typeDef,
                                                            ULONG32 cTypeArgs,
                                                            ClassID typeArgs[],
                                                            ClassID* pClassID)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetClassFromTokenAndTypeArgs 0x%p, 0x%08x.\n", moduleID, typeDef));    
        
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    Module* pModule = reinterpret_cast< Module* >(moduleID);

    if (pModule == NULL || pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    // This array needs to be accessible at least until the call to
    // ClassLoader::LoadGenericInstantiationNoThrow
    TypeHandle* genericParameters = new (nothrow) TypeHandle[cTypeArgs];
    NewArrayHolder< TypeHandle > holder(genericParameters);

    if (NULL == genericParameters)
    {
        return E_OUTOFMEMORY;
    }

    for (ULONG32 i = 0; i < cTypeArgs; ++i)
    {
        genericParameters[i] = TypeHandle(reinterpret_cast< MethodTable* >(typeArgs[i]));
    }

    //
    TypeHandle th;
    HRESULT hr = S_OK;

    EX_TRY
    {
        th = ClassLoader::LoadGenericInstantiationThrowing(pModule,
                                                           typeDef,
                                                           cTypeArgs,
                                                           genericParameters,
                                                           ClassLoader::LoadTypes);
    }
    EX_CATCH_HRESULT(hr);

    if (FAILED(hr))
    {
        return hr;
    }

    if (th.IsNull())
    {
        // Hmm, the type isn't loaded yet.
        return CORPROF_E_DATAINCOMPLETE;
    }

    *pClassID = TypeHandleToClassID(th);

    return S_OK;
}

//*****************************************************************************
// Given the token for a method, return the fucntion id.
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::GetFunctionFromToken(
    ModuleID moduleId,
    mdToken typeDef,
    FunctionID *pFunctionId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetFunctionFromToken 0x%p, 0x%08x.\n", moduleId, typeDef));    
        
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    // Default HRESULT
    HRESULT hr = S_OK;

    // Get the module
    Module *pModule = (Module *) moduleId;

    // No module, or disassociated from metadata
    if (pModule == NULL || pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    // Default return value of NULL
    MethodDesc *pDesc = NULL;

    // Different lookup depending on whether it's a Def or Ref
    if (TypeFromToken(typeDef) == mdtMethodDef)
    {
        pDesc = pModule->LookupMethodDef(typeDef);
    }
    else if (TypeFromToken(typeDef) == mdtMemberRef)
    {
        pDesc = pModule->LookupMemberRefAsMethod(typeDef);
    }
    else
    {
        return E_INVALIDARG;
    }

    if (NULL == pDesc)
    {
        return E_INVALIDARG;
    }

    //
    // Check that this is a non-generic method
    //
    if (pDesc->HasClassOrMethodInstantiation())
    {
        return CORPROF_E_FUNCTION_IS_PARAMETERIZED;
    }

    if (pFunctionId && SUCCEEDED(hr))
    {
        *pFunctionId = CodeIdToFunctionId(MethodDescToCodeID(pDesc));
    }

    return (hr);
}

HRESULT ProfToEEInterfaceImpl::GetFunctionFromTokenAndTypeArgs(ModuleID moduleID,
                                                               mdMethodDef funcDef,
                                                               ClassID classId,
                                                               ULONG32 cTypeArgs,
                                                               ClassID typeArgs[],
                                                               FunctionID* pFunctionID)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetFunctionFromTokenAndTypeArgs 0x%p, 0x%08x, 0x%p.\n", moduleID, funcDef, classId));        

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classId);
    Module* pModule = reinterpret_cast< Module* >(moduleID);

    if ((pModule == NULL) || typeHandle.IsNull())
    {
        return E_INVALIDARG;
    }
         
    if (pModule->IsBeingUnloaded())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    MethodDesc* pMethodDesc = NULL;

    if (mdtMethodDef == TypeFromToken(funcDef))
    {
        pMethodDesc = pModule->LookupMethodDef(funcDef);
    }
    else if (mdtMemberRef == TypeFromToken(funcDef))
    {
        pMethodDesc = pModule->LookupMemberRefAsMethod(funcDef);
    }
    else
    {
        return E_INVALIDARG;
    }

    MethodTable* pMethodTable = typeHandle.GetMethodTable();

    if (pMethodTable == NULL || !pMethodTable->IsRestored() || 
        pMethodDesc == NULL || !pMethodDesc->IsRestored())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }

    // This array needs to be accessible at least until the call to
    // ClassLoader::LoadGenericInstantiationNoThrow
    TypeHandle* genericParameters = new (nothrow) TypeHandle[cTypeArgs];
    NewArrayHolder< TypeHandle > holder(genericParameters);

    if (NULL == genericParameters)
    {
        return E_OUTOFMEMORY;
    }

    for (ULONG32 i = 0; i < cTypeArgs; ++i)
    {
        genericParameters[i] = TypeHandle(reinterpret_cast< MethodTable* >(typeArgs[i]));
    }

    MethodDesc* result = NULL;
    HRESULT hr = S_OK;

    EX_TRY
    {
        result = MethodDesc::FindOrCreateAssociatedMethodDesc(pMethodDesc,
                                                              pMethodTable,
                                                              FALSE,
                                                              cTypeArgs,
                                                              genericParameters,
                                                              TRUE);
    }
    EX_CATCH_HRESULT(hr);

    if (NULL != result)
    {
        *pFunctionID = CodeIdToFunctionId(MethodDescToCodeID(result));
    }

    return hr;
}

//*****************************************************************************
// Retrive information about a given application domain, which is like a
// sub-process.
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::GetAppDomainInfo(
    AppDomainID appDomainId,
    ULONG       cchName,
    ULONG       *pcchName,
    WCHAR       szName[],
    ProcessID   *pProcessId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetAppDomainInfo 0x%p.\n", appDomainId));
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    BaseDomain   *pDomain;            // Internal data structure.
    HRESULT     hr = S_OK;

    //

    pDomain = (BaseDomain *) appDomainId;

    // Make sure they've passed in a valid appDomainId
    if (pDomain == NULL)
        return (E_INVALIDARG);

    // Pick sensible defaults.
    if (pcchName)
        *pcchName = 0;
    if (szName)
        *szName = 0;
    if (pProcessId)
        *pProcessId = 0;

    LPCWSTR szFriendlyName;
    if (pDomain == SystemDomain::System())
        szFriendlyName = g_pwBaseLibrary;
    else if (pDomain == SharedDomain::GetDomain())
        szFriendlyName = L"EE Shared Assembly Repository";
    else
        szFriendlyName = ((AppDomain*)pDomain)->GetFriendlyNameForDebugger();

    if (szFriendlyName != NULL)
    {
        // Get the module file name
        ULONG trueLen = (ULONG)(wcslen(szFriendlyName) + 1);

        // Return name of module as required.
        if (szName && cchName > 0)
        {
            ULONG copyLen = trueLen;

            if (copyLen >= cchName)
            {
                copyLen = cchName - 1;
            }

            wcsncpy_s(szName, cchName, szFriendlyName, copyLen);
        }

        // If they request the actual length of the name
        if (pcchName)
            *pcchName = trueLen;
    }

    // If we don't have a friendly name but the call was requesting it, then return incomplete data HR
    else
    {
        if ((szName != NULL && cchName > 0) || pcchName)
            hr = CORPROF_E_DATAINCOMPLETE;
    }

    if (pProcessId)
        *pProcessId = (ProcessID) GetCurrentProcessId();

    return (hr);
}


//*****************************************************************************
// Retrieve information about an assembly, which is a collection of dll's.
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::GetAssemblyInfo(
    AssemblyID  assemblyId,
    ULONG       cchName,
    ULONG       *pcchName,
    WCHAR       szName[],
    AppDomainID *pAppDomainId,
    ModuleID    *pModuleId)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        THROWS;         // SString::SString throws
        GC_NOTRIGGER;
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetAssemblyInfo 0x%p.\n", assemblyId));    
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    HRESULT hr = S_OK;

    Assembly    *pAssembly;             // Internal data structure for assembly.

    pAssembly = (Assembly *) assemblyId;
    _ASSERTE(pAssembly != NULL);

    if (pcchName || szName)
    {
        // Get the friendly name of the assembly
        SString name(SString::Utf8, pAssembly->GetSimpleName());

        const COUNT_T nameLength = name.GetCount() + 1;

        if ((NULL != szName) && (cchName > 0))
        {
            wcsncpy_s(szName, cchName, name.GetUnicode(), min(nameLength, cchName - 1));
        }

        if (NULL != pcchName)
        {
            *pcchName = nameLength;
        }
    }

    // Get the parent application domain.
    if (pAppDomainId)
    {
        *pAppDomainId = (AppDomainID) pAssembly->GetDomain();
        _ASSERTE(*pAppDomainId != NULL);
    }

    // Find the module the manifest lives in.
    if (pModuleId)
    {
        *pModuleId = (ModuleID) pAssembly->GetManifestModule();

        // This is the case where the profiler has called GetAssemblyInfo
        // on an assembly that has been completely created yet.
        if (!*pModuleId)
            hr = CORPROF_E_DATAINCOMPLETE;
    }

    return (hr);
}

//*****************************************************************************
//
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::SetEnterLeaveFunctionHooks(FunctionEnter *pFuncEnter,
                                                          FunctionLeave *pFuncLeave,
                                                          FunctionTailcall *pFuncTailcall)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(pFuncEnter));
        PRECONDITION(CheckPointer(pFuncLeave));
        PRECONDITION(CheckPointer(pFuncTailcall));
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: SetEnterLeaveFunctionHooks 0x%p, 0x%p, 0x%p.\n", pFuncEnter, pFuncLeave, pFuncTailcall));    

    // We enforce below that we must be in Init for this to continue, so no need to check the callback state flags
    
    HRESULT hr = S_OK;

    // The profiler must call SetEnterLeaveFunctionHooks during initialization, since
    // the enter/leave events are immutable and must also be set during initialization.
    if (g_profStatus != profInInit)
        return (CORPROF_E_CALL_ONLY_FROM_INIT);

    // Always save onto the function pointers, since we won't know if the profiler
    // is going to tracking enter/leave until after it returns from Initialize
    g_profControlBlock.pEnter = pFuncEnter;
    g_profControlBlock.pLeave = pFuncLeave;
    g_profControlBlock.pTailcall = pFuncTailcall;

    if (g_profControlBlock.pEnter2 == NULL)
    {
        EX_TRY
        {
            //
            // Set the function pointer so that the JIT calls to the EE functions, so that
            // we can convert the V2 parameters from the JIT to the single parameter expected
            // in V1.
            //
            SetEnterLeaveFunctionHooksForJit((FunctionEnter *)PROFILECALLBACK(ProfileEnter),
                                             (FunctionLeave *)PROFILECALLBACK(ProfileLeave),
                                             (FunctionTailcall *)PROFILECALLBACK(ProfileTailcall));
        }
        EX_CATCH_HRESULT(hr);
    }

    return hr;
}

//*****************************************************************************
//
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::SetEnterLeaveFunctionHooks2(FunctionEnter2 *pFuncEnter,
                                                           FunctionLeave2 *pFuncLeave,
                                                           FunctionTailcall2 *pFuncTailcall)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(pFuncEnter));
        PRECONDITION(CheckPointer(pFuncLeave));
        PRECONDITION(CheckPointer(pFuncTailcall));
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: SetEnterLeaveFunctionHooks2 0x%p, 0x%p, 0x%p.\n", 
        pFuncEnter, pFuncLeave, pFuncTailcall));    

    // We enforce below that we must be in Init for this to continue, so no need to check the callback state flags
    
    HRESULT hr = S_OK;

    //
    // The profiler must call SetEnterLeaveFunctionHooks2 during initialization, since
    // the enter/leave events are immutable and must also be set during initialization.
    //
    if (g_profStatus != profInInit)
        return (CORPROF_E_CALL_ONLY_FROM_INIT);

    //
    // Always save onto the function pointers, since we won't know if the profiler
    // is going to track enter/leave until after it returns from Initialize
    //
    g_profControlBlock.pEnter2 = pFuncEnter;
    g_profControlBlock.pLeave2 = pFuncLeave;
    g_profControlBlock.pTailcall2 = pFuncTailcall;

    EX_TRY
    {
        if (CORProfilerFunctionArgsEnabled() ||
            CORProfilerFunctionReturnValueEnabled() ||
            CORProfilerFrameInfoEnabled() ||

            // DSS walk of thread inside profiler's enter/leave/tailcall hook would skip
            // call frames if we called directly into profiler's enter/leave/tailcall hook
            // from jitted code, since that doesn't push a Frame.  So we force the C++
            // ProfileEnter/ProfileLeave/ProfileTailcall FCALL to wrap the profiler's
            // hook so we have a chance to push the Frame.
            CORProfilerStackSnapshotEnabled())
        {
            //
            // If the profiler wants args or generic info, then we have to go thru the EE's
            // functions to gather that data before going to the profiler, so set the callbacks
            // to the EE's functions.
            //
            SetEnterLeaveFunctionHooksForJit((FunctionEnter *)PROFILECALLBACK(ProfileEnter),
                                             (FunctionLeave *)PROFILECALLBACK(ProfileLeave),
                                             (FunctionTailcall *)PROFILECALLBACK(ProfileTailcall));
        }
        else
        {
            //
            // Otherwise we can have the JIT call directly to the profiler's routines.
            //
            SetEnterLeaveFunctionHooksForJit((FunctionEnter *)pFuncEnter,
                                             (FunctionLeave *)pFuncLeave,
                                             (FunctionTailcall *)pFuncTailcall);
        }
    }
    EX_CATCH_HRESULT(hr);

    return hr;
}

//*****************************************************************************
//
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::SetFunctionIDMapper(FunctionIDMapper *pFunc)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    LOG((LF_CORPROF, LL_INFO1000, "**PROF: SetFunctionIDMapper 0x%p.\n", pFunc));
    
    // This call is allowed asynchronously, so we don't need to check the callback state flags

    g_pProfilersFuncIDMapper = pFunc;

    return (S_OK);
}


/*
 * GetFunctionInfo2
 *
 * This function takes the frameInfo returned from a profiler callback and splays it
 * out into as much information as possible.
 *
 * Parameters:
 *   funcId - The function that is being requested.
 *   frameInfo - Frame specific information from a callback (for resolving generics).
 *   pClassId - An optional parameter for returning the class id of the function.
 *   pModuleId - An optional parameter for returning the module of the function.
 *   pToken - An optional parameter for returning the metadata token of the function.
 *   cTypeArgs - The count of the size of the array typeArgs
 *   pcTypeArgs - Returns the number of elements of typeArgs filled in, or if typeArgs is NULL
 *         the number that would be needed.
 *   typeArgs - An array to store generic type parameters for the function.
 *
 * Returns:
 *   S_OK if successful.
 */
HRESULT ProfToEEInterfaceImpl::GetFunctionInfo2(FunctionID funcId,
                                             COR_PRF_FRAME_INFO frameInfo,
                                             ClassID *pClassId,
                                             ModuleID *pModuleId,
                                             mdToken *pToken,
                                             ULONG32 cTypeArgs,
                                             ULONG32 *pcTypeArgs,
                                             ClassID typeArgs[])
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(pClassId, NULL_OK));
        PRECONDITION(CheckPointer(pModuleId, NULL_OK));
        PRECONDITION(CheckPointer(pToken,  NULL_OK));
        PRECONDITION(CheckPointer(pcTypeArgs, NULL_OK));
        PRECONDITION(CheckPointer(typeArgs, NULL_OK));
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetFunctionInfo2 0x%p.\n", funcId));

    // This call is allowed asynchronously, so we don't need to check the callback state flags

    //
    // Verify parameters.
    //
    COR_PRF_FRAME_INFO_INTERNAL *pFrameInfo = (COR_PRF_FRAME_INFO_INTERNAL *)frameInfo;

    if ((funcId == NULL) ||
        ((pFrameInfo != NULL) && (pFrameInfo->funcID != funcId)))
    {
        return E_INVALIDARG;
    }

    MethodDesc *pMethDesc = FunctionIdToMethodDesc(funcId);

    if (pMethDesc == NULL)
    {
        return E_INVALIDARG;
    }

    if ((cTypeArgs != 0) && (typeArgs == NULL))
    {
        return E_INVALIDARG;
    }

    // it's not safe to examine a methoddesc that has not been restored so do not do so
    if (!pMethDesc ->IsRestored())
        return CORPROF_E_DATAINCOMPLETE;      

    //
    // Find the exact instantiation of this function.
    //
    TypeHandle specificClass;
    MethodDesc* pActualMethod;

    ClassID classId = NULL;

    if (pMethDesc->IsSharedByGenericInstantiations())
    {
        BOOL exactMatch;
        OBJECTREF pThis = NULL;

        if (pFrameInfo != NULL)
        {
            // If FunctionID represents a generic methoddesc on a struct, then pFrameInfo->thisArg
            // isn't an Object*.  It's a pointer directly into the struct's members (i.e., it's not pointing at the
            // method table).  That means pFrameInfo->thisArg cannot be casted to an OBJECTREF for
            // use by Generics::GetExactInstantiationsFromCallInformation.  However,
            // Generics::GetExactInstantiationsFromCallInformation won't even need a this pointer
            // for the methoddesc it's processing if the methoddesc is on a value type.  So we
            // can safely pass NULL for the methoddesc's this in such a case.
            if (pMethDesc->GetMethodTable()->IsValueClass())
            {
                _ASSERTE(!pMethDesc->AcquiresInstMethodTableFromThis());
                _ASSERTE(pThis == NULL);
            }
            else
            {
                pThis = ObjectToOBJECTREF((PTR_Object)(pFrameInfo->thisArg));
            }
        }

        exactMatch = Generics::GetExactInstantiationsFromCallInformation(pMethDesc,
                                                                 pThis,
                                                                 ((pFrameInfo != NULL) ? pFrameInfo->extraArg : NULL),
                                                                 &specificClass,
                                                                 &pActualMethod);

        if (exactMatch)
        {
            classId = TypeHandleToClassID(specificClass);
        }
        else if (!specificClass.HasInstantiation())
        {
            //
            // In this case we could not get the type args for the method, but it
            // the case that the class is not a generic class, and thus this
            // value is correct.
            //
            classId = TypeHandleToClassID(specificClass);
        }
        else
        {
            //
            // We could not get any class information.
            //
            classId = NULL;
        }
    }
    else
    {
        TypeHandle typeHandle(pMethDesc->GetMethodTable());
        classId = TypeHandleToClassID(typeHandle);
        pActualMethod = pMethDesc;
    }


    //
    // Fill in the ClassId, if desired
    //
    if (pClassId != NULL)
    {
        *pClassId = classId;
    }

    //
    // Fill in the ModuleId, if desired.
    //
    if (pModuleId != NULL)
    {
        *pModuleId = (ModuleID)pMethDesc->GetModule();
    }

    //
    // Fill in the token, if desired.
    //
    if (pToken != NULL)
    {
        *pToken = (mdToken)pMethDesc->GetMemberDef();
    }

    if ((cTypeArgs == 0) && (pcTypeArgs != NULL))
    {
        //
        // They are searching for the size of the array needed, we can return that now and
        // short-circuit all the work below.
        //
        if (pcTypeArgs != NULL)
        {
            *pcTypeArgs = pActualMethod->GetNumGenericMethodArgs();
        }
        return S_OK;
    }

    //
    // If no place to store resulting count, quit now.
    //
    if (pcTypeArgs == NULL)
    {
        return S_OK;
    }

    //
    // Fill in the type args
    //
    DWORD cArgsToFill = pActualMethod->GetNumGenericMethodArgs();

    if (cArgsToFill > cTypeArgs)
    {
        cArgsToFill = cTypeArgs;
    }

    *pcTypeArgs = cArgsToFill;

    if (cArgsToFill == 0)
    {
        return S_OK;
    }

    TypeHandle *pMethodInst = pActualMethod->GetMethodInstantiation();

    for (DWORD i = 0; i < cArgsToFill; i++)
    {
        typeArgs[i] = (pMethodInst != NULL) ? TypeHandleToClassID(pMethodInst[i]) : NULL;
    }

    return S_OK;
}

/*
 * GetStringLayout
 *
 * This function describes to a profiler the internal layout of a string.
 *
 * Parameters:
 *   pBufferLengthOffset - Offset within an OBJECTREF of a string of the ArrayLength field.
 *   pStringLengthOffset - Offset within an OBJECTREF of a string of the StringLength field.
 *   pBufferOffset - Offset within an OBJECTREF of a string of the Buffer field.
 *
 * Returns:
 *   S_OK if successful.
 */
HRESULT ProfToEEInterfaceImpl::GetStringLayout(ULONG *pBufferLengthOffset,
                                             ULONG *pStringLengthOffset,
                                             ULONG *pBufferOffset)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(pBufferLengthOffset, NULL_OK));
        PRECONDITION(CheckPointer(pStringLengthOffset, NULL_OK));
        PRECONDITION(CheckPointer(pBufferOffset,  NULL_OK));
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetStringLayout.\n"));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    if (pBufferLengthOffset != NULL)
    {
        *pBufferLengthOffset = StringObject::GetArrayLengthOffset();
    }

    if (pStringLengthOffset != NULL)
    {
        *pStringLengthOffset = StringObject::GetStringLengthOffset_MaskOffHighBit();
    }

    if (pBufferOffset != NULL)
    {
        *pBufferOffset = StringObject::GetBufferOffset();
    }

    return S_OK;
}

/*
 * GetClassLayout
 *
 * This function describes to a profiler the internal layout of a class.
 *
 * Parameters:
 *   classID - The class that is being queried.  It is really a TypeHandle.
 *   rFieldOffset - An array to store information about each field in the class.
 *   cFieldOffset - Count of the number of elements in rFieldOffset.
 *   pcFieldOffset - Upon return contains the number of elements filled in, or if
 *         cFieldOffset is zero, the number of elements needed.
 *   pulClassSize - Optional parameter for containing the size in bytes of the underlying
 *         internal class structure.
 *
 * Returns:
 *   S_OK if successful.
 */
HRESULT ProfToEEInterfaceImpl::GetClassLayout(ClassID classID,
                                             COR_FIELD_OFFSET rFieldOffset[],
                                             ULONG cFieldOffset,
                                             ULONG *pcFieldOffset,
                                             ULONG *pulClassSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(rFieldOffset, NULL_OK));
        PRECONDITION(CheckPointer(pcFieldOffset));
        PRECONDITION(CheckPointer(pulClassSize,  NULL_OK));
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetClassLayout 0x%p.\n", classID));
    
    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // Verify parameters
    //
    if ((pcFieldOffset == NULL) || (classID == NULL))
    {
         return E_INVALIDARG;
    }

    if ((cFieldOffset != 0) && (rFieldOffset == NULL))
    {
        return E_INVALIDARG;
    }

    TypeHandle typeHandle = TypeHandle::FromPtr((void *)classID);

    //
    // This is the incorrect API for arrays.  Use GetArrayInfo and GetArrayLayout.
    //
    if (typeHandle.IsArray() ||
        (!typeHandle.IsTypeDesc() && typeHandle.GetMethodTable()->IsArray()))
    {
        return E_INVALIDARG;
    }

    //
    // Fill in class size now
    //
    if (pulClassSize != NULL)
    {
        *pulClassSize = typeHandle.GetMethodTable()->GetBaseSize();
    }

    //
    // If this class is not fully restored, that is all the information we can get at this time.
    //
    if (!typeHandle.GetMethodTable()->IsRestored())
    {
        return CORPROF_E_DATAINCOMPLETE;
    }


    ApproxFieldDescIterator fieldDescIterator(typeHandle.GetMethodTable(), ApproxFieldDescIterator::INSTANCE_FIELDS);

    ULONG cFields = fieldDescIterator.Count();

    //
    // If they are looking to just get the count, return that.
    //
    if ((cFieldOffset == 0)  || (rFieldOffset == NULL))
    {
        *pcFieldOffset = cFields;
        return S_OK;
    }

    //
    // Dont put too many in the array.
    //
    if (cFields > cFieldOffset)
    {
        cFields = cFieldOffset;
    }

    *pcFieldOffset = cFields;

    //
    // Now fill in the array
    //
    BOOL fHasMethodTablePtr = !typeHandle.IsValueType();

    ULONG i;
    FieldDesc *pField;

    for (i = 0; i < cFields; i++)
    {
        pField = fieldDescIterator.Next();
        rFieldOffset[i].ridOfField = (ULONG)pField->GetMemberDef();
        rFieldOffset[i].ulOffset = (ULONG)pField->GetOffset() + (fHasMethodTablePtr ? sizeof(void *) : 0);
    }

    return S_OK;
}


typedef struct _PROFILER_STACK_WALK_DATA
{
    StackSnapshotCallback *callback;
    ULONG32 infoFlags;
    ULONG32 contextFlags;
    void *clientData;
    
} PROFILER_STACK_WALK_DATA;


/*
 * ProfilerStackWalkCallback
 *
 * This routine is used as the callback from the general stack walker for
 * doing snapshot stack walks
 *
 */
StackWalkAction ProfilerStackWalkCallback(CrawlFrame *pCf, PROFILER_STACK_WALK_DATA *pData)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;  // throw is RIGHT out... the throw at minimum allocates the thrown object which we *must* not do
        GC_NOTRIGGER; // the stack is not necessarily crawlable at this state !!!) we must not induce a GC 
    } CONTRACTL_END;

    MethodDesc *pFunc = pCf->GetFunction();

    COR_PRF_FRAME_INFO_INTERNAL frameInfo;
    ULONG32 contextSize = 0;
    BYTE *context = NULL;

    UINT_PTR currentIP = 0;
    REGDISPLAY *pRegDisplay = pCf->GetRegisterSet();
#if defined(_X86_) || defined(_PPC_)
    CONTEXT builtContext;
#endif

    //
    // For Unmanaged-to-managed transitions we get a NativeMarker back, which we want
    // to return to the profiler as the context seed if it wants to walk the unmanaged
    // stack frame, so we report the functionId as NULL to indicate this.
    //
    if (pCf->IsNativeMarker())
    {
        pFunc = NULL;
    }

    //
    // Skip all Lightweight reflection/emit functions
    //
    if ((pFunc != NULL) && pFunc->IsNoMetadata())
    {
        return SWA_CONTINUE;
    }

    //
    // If this is not a transition of any sort and not a managed
    // method, ignore it.
    //
    if (!pCf->IsNativeMarker() && !pCf->IsFrameless())
    {
        return SWA_CONTINUE;
    }

#ifdef _X86_
    currentIP = (UINT_PTR)*(pRegDisplay->pPC);
#endif

    frameInfo.size = sizeof(COR_PRF_FRAME_INFO_INTERNAL);
    frameInfo.version = COR_PRF_FRAME_INFO_INTERNAL_CURRENT_VERSION;

    if (pFunc != NULL)
    {
        frameInfo.funcID = ProfToEEInterfaceImpl::_CodeIdToFunctionId(MethodDescToCodeID(pFunc));
        frameInfo.extraArg = NULL;
    }
    else
    {
        frameInfo.funcID = NULL;
        frameInfo.extraArg = NULL;
    }

    frameInfo.IP = currentIP;
    frameInfo.thisArg = NULL;

    if (pData->infoFlags & COR_PRF_SNAPSHOT_REGISTER_CONTEXT)
    {
#if defined(_X86_) || defined(_PPC_)
        //
        // X86 stack walking does not keep the context up-to-date during the
        // walk.  Instead it keeps the REGDISPLAY up-to-date.  Thus, we need to
        // build a CONTEXT from the REGDISPLAY.
        //

        memset(&builtContext, 0, sizeof(builtContext));
        builtContext.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
        CopyRegDisplay(pRegDisplay, NULL, &builtContext);
        context = (BYTE *)(&builtContext);
#else
        context = (BYTE *)pRegDisplay->pCurrentContext;
#endif
        contextSize = sizeof(CONTEXT);
    }

    
    if (pData->callback(frameInfo.funcID,
                        frameInfo.IP,
                        (COR_PRF_FRAME_INFO)&frameInfo,
                        contextSize,
                        context,
                        pData->clientData) == S_OK)
    {
        return SWA_CONTINUE;
    }

    return SWA_ABORT;
}


//*****************************************************************************
// Exception swallowing wrapper around the profiler stackwalk
//*****************************************************************************
HRESULT ProfilerStackWalkCallbackWrapper(Thread* pThreadToSnapshot, PROFILER_STACK_WALK_DATA *pData, unsigned flags)
{
    STATIC_CONTRACT_NOTHROW;

    HRESULT hr = E_UNEXPECTED;

    //
    PAL_TRY
    {
        StackWalkAction swaRet = pThreadToSnapshot->StackWalkFrames(
            (PSTACKWALKFRAMESCALLBACK)ProfilerStackWalkCallback,
             pData,
             flags,
             NULL);
        switch (swaRet)
        {
        default:
            _ASSERTE(!"Unexpected StackWalkAction returned from Thread::StackWalkFrames");
            hr = E_FAIL;
            break;
            
        case SWA_FAILED:
            hr = E_FAIL;
            break;
            
        case SWA_ABORT:
            hr = CORPROF_E_STACKSNAPSHOT_ABORTED;
            break;

        case SWA_DONE:
            hr = S_OK;
            break;
        }
    }
    PAL_EXCEPT(EXCEPTION_EXECUTE_HANDLER)
    {
        hr = E_UNEXPECTED;
    }
    PAL_ENDTRY;

    return hr;
}

//*****************************************************************************
// Perform a stack walk, calling back to callback at each managed frame.
//*****************************************************************************
HRESULT ProfToEEInterfaceImpl::DoStackSnapshot(ThreadID thread,
                                              StackSnapshotCallback *callback,
                                              ULONG32 infoFlags,
                                              void *clientData,
                                              LPCONTEXT pctxSeed,
                                              ULONG32 contextSize)
{
    CONTRACTL
    {
        SO_NOT_MAINLINE;
        NOTHROW;  // throw is RIGHT out... the throw at minimum allocates the thrown object which we *must* not do
        GC_NOTRIGGER; // the stack is not necessarily crawlable at this state !!!) we must not induce a GC 
    } CONTRACTL_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: DoStackSnapshot 0x%p, 0x%p, 0x%08x, 0x%p, 0x%p, 0x%08x.\n", 
        thread, callback, infoFlags, clientData, pctxSeed, contextSize));
   
    // This call is allowed asynchronously, so we don't need to check the callback state flags

    HRESULT hr = E_UNEXPECTED;
    // (hr assignment is to appease the rotor compiler; we won't actually return without explicitly setting hr again)
    
    Thread *pThreadToSnapshot = NULL;
    Thread *pCurrentThread = GetThread();
    BOOL fResumeThread = FALSE;
    BOOL fResetProfilerFilterContext = FALSE;
    INDEBUG(ULONG ulForbidTypeLoad = 0;)
    BOOL fResetSnapshotThreadExternalCount = FALSE;
    int cRefsSnapshotThread = 0;

    CONTEXT ctxCurrent;
    memset(&ctxCurrent, 0, sizeof(ctxCurrent));

    REGDISPLAY rd;

    PROFILER_STACK_WALK_DATA data;

    if (!g_fEEStarted )
    {
        // no managed code has run and things are likely in a very bad have loaded state
        // this is a bad time to try to walk the stack
        
        // Returning directly as there is nothing to cleanup yet
        return CORPROF_E_STACKSNAPSHOT_UNSAFE;
    }

    if (!CORProfilerStackSnapshotEnabled())
    {
        // Returning directly as there is nothing to cleanup yet, and can't skip gcholder ctor
        return E_INVALIDARG;
    }

    if (thread == NULL)
    {
        pThreadToSnapshot = pCurrentThread;
    }
    else
    {
        pThreadToSnapshot = (Thread *)thread;
    }

    if (infoFlags & ~COR_PRF_SNAPSHOT_REGISTER_CONTEXT)
    {
        // Returning directly as there is nothing to cleanup yet, and can't skip gcholder ctor
        return E_INVALIDARG;
    }

    if ((pThreadToSnapshot == NULL) || !GCHeap::IsGCHeapInitialized())
    {
        //
        // No managed frames, return now.
        //
        // Returning directly as there is nothing to cleanup yet, and can't skip gcholder ctor
        return S_OK;
    }

    Thread::HijackLockHolder hijackLockHolder(pThreadToSnapshot);
    if (!hijackLockHolder.Acquired())
    {
        // Returning directly as there is nothing to cleanup yet, and can't skip gcholder ctor
        return CORPROF_E_STACKSNAPSHOT_UNSAFE;
    }

    if (pThreadToSnapshot != pCurrentThread         // Walking separate thread
        && pCurrentThread != NULL                         // Walker (current) thread is a managed / VM thread
        && Thread::SysIsSuspendInProgress())          // EE is trying suspend itself
    {
        //
        return CORPROF_E_STACKSNAPSHOT_UNSAFE;
    }

    // We only allow stackwalking if:
    // 1) Target thread to walk == current thread OR Target thread is suspended, AND
    // 2) Target thread to walk is currently executing JITted / NGENd code, AND
    // 3) Target thread to walk is seeded OR currently NOT unwinding the stack, AND
    // 4) Target thread to walk != current thread OR current thread is NOT in a can't stop or forbid suspend region

    // If the thread is in a forbid suspend region, it's dangerous to do anything:
    // - The code manager datastructures accessed during the stackwalk may be in inconsistent state.
    // - Thread::Suspend won't be able to suspend the thread.
    if (pThreadToSnapshot->IsInForbidSuspendRegion())
    {
        hr = CORPROF_E_STACKSNAPSHOT_UNSAFE;
        goto Cleanup;
    }

    // First, check "1) Target thread to walk == current thread OR Target thread is suspended"
    if (pThreadToSnapshot != pCurrentThread)
    {
        // Walking separate thread, so it must be suspended.  First, ensure that
        // target thread exists.
        //
        // NOTE: We're using the "dangerous" variant of this refcount function, because we
        // rely on the profiler to ensure it never tries to walk a thread being destroyed.
        // (Profiler must block in its ThreadDestroyed() callback until all uses of that thread,
        // such as walking its stack, are complete.)
        cRefsSnapshotThread = pThreadToSnapshot->IncExternalCountDANGEROUSProfilerOnly();
        fResetSnapshotThreadExternalCount = TRUE;

        if (cRefsSnapshotThread == 1 || !pThreadToSnapshot->HasValidThreadHandle())
        {
            // At this point, we've modified the VM state based on bad input
            // (pThreadToSnapshot) from the profiler.  This could cause
            // memory corruption and leave us vulnerable to security problems.
            // So destroy the process.
            _ASSERTE(!"Profiler trying to walk destroyed thread");
            EEPOLICY_HANDLE_FATAL_ERROR(CORPROF_E_STACKSNAPSHOT_INVALID_TGT_THREAD);
        }
        
        // Thread::SuspendThread() ensures that no one else should try to suspend us
        // while we're suspending pThreadToSnapshot.
        //
        // TRUE: OneTryOnly.  Don't loop waiting for others to get out of our way in
        // order to suspend the thread.  If it's not safe, just return an error immediately.
        Thread::SuspendThreadResult str = pThreadToSnapshot->SuspendThread(TRUE);
        if (str == Thread::STR_Success)
        {
            fResumeThread = TRUE;
        }
        else
        {
            hr = CORPROF_E_STACKSNAPSHOT_UNSAFE;
            goto Cleanup;            
        }
    }

    // Second, check "2) Target thread to walk is currently executing JITted / NGENd code"
    // To do this, we need to find the proper context to investigate.  Start with
    // the seeded context, if available.  If not, use the target thread's current context.
    if (pctxSeed == NULL)
    {
        // Profiler didn't seed a context for us.  Are we walking ourselves?
        if (pThreadToSnapshot == pCurrentThread)
        {
            // Walking ourselves, so we'll skip check 1).  This case maps to
            // a profiler walking the stack of the current thread in response
            // to a callback (e.g., GC allocation), without any hijacking.  In
            // such a case, we know we're executing JITd/NGENd code,
            // so we're safe.

            _ASSERTE(pctxSeed == NULL);     // NOOP: leave NULL
        }
        else
        {
            //
            if (!pThreadToSnapshot->InitRegDisplay(&rd, &ctxCurrent, false /* validContext */))
            {
                LOG((LF_CORPROF, LL_INFO100, "**PROF: InitRegDisplay failure leads to E_FAIL.\n"));
                hr = E_FAIL;
                goto Cleanup;
            }
            pctxSeed = &ctxCurrent;
        }
    }

    // If target thread is in pre-emptive mode, the profiler's seed context is unnecessary
    // because our frame chain is good enough: it will give us at least as accurate a
    // starting point as the profiler could.  Also, since profiler contexts cannot be
    // trusted, we don't want to set the thread's profiler filter context to this, as a GC
    // that interrupts the profiler's stackwalk will end up using the profiler's (potentially
    // bogus) filter context.
    if (!pThreadToSnapshot->PreemptiveGCDisabledOther())
    {
        // Thread to be walked is in preemptive mode.  Throw out seed.
        pctxSeed = NULL;
    }

    // If we've filled out pctxSeed, then we need to check that context to see if
    // it's in JITd/NGENd code.  If it's not filled out, the check is unnecessary
    if (pctxSeed != NULL)
    {
        // if there's no Jit Manager for the IP, it's not managed code.
        if (ExecutionManager::FindJitMan((SLOT)GetIP(pctxSeed)) == NULL)
        {
            hr = CORPROF_E_STACKSNAPSHOT_UNMANAGED_CTX;
            goto Cleanup;
        }
    }

#ifdef _DEBUG
    //
    // Sanity check: If we are doing a cross-thread walk and there is no seed context, then
    // we better not be in managed code, otw we do not have a Frame on the stack from which to start
    // walking and we may miss the leaf-most chain of managed calls due to the way StackWalkFrames 
    // is implemented.
    //
    if (pThreadToSnapshot != pCurrentThread)
    {
        if (pctxSeed == NULL)
        {
            _ASSERTE(pThreadToSnapshot->InitRegDisplay(&rd, &ctxCurrent, false /* validContext */));
            _ASSERTE(ExecutionManager::FindJitMan((SLOT)GetIP(&ctxCurrent)) == NULL);
        }
    }
#endif

    // Third, verify the target thread is seeded or not in the midst of an unwind.
    if (pctxSeed == NULL)
    {
        ThreadExceptionState* pExState = pThreadToSnapshot->GetExceptionState();

        // this tests to see if there is an exception in flight
        if (pExState->IsExceptionInProgress() && pExState->GetFlags()->UnwindHasStarted())
        {
            EHClauseInfo *pCurrentEHClauseInfo = pThreadToSnapshot->GetExceptionState()->GetCurrentEHClauseInfo();

            // if the exception code is telling us that we have entered a managed context then all is well
            if (!pCurrentEHClauseInfo->IsManagedCodeEntered())
            {
                hr = CORPROF_E_STACKSNAPSHOT_UNMANAGED_CTX;
                goto Cleanup;
            }
        }
    }

    // Check if the exception state is consistent.  See the comment for ThreadExceptionFlag for more information. 
    if (pThreadToSnapshot->GetExceptionState()->HasThreadExceptionFlag(ThreadExceptionState::TEF_InconsistentExceptionState))
    {
        hr = CORPROF_E_STACKSNAPSHOT_UNSAFE;
        goto Cleanup;
    }

    // We're now fairly confident the stackwalk should be ok, so set
    // the context seed, if one was provided or cooked up.
    if (pctxSeed != NULL)
    {
        pThreadToSnapshot->SetProfilerFilterContext(pctxSeed);
        fResetProfilerFilterContext = TRUE;
    }

    data.callback = callback;
    data.infoFlags = infoFlags;
    data.contextFlags = 0;
    data.clientData = clientData;

    INDEBUG(if (pCurrentThread) ulForbidTypeLoad = pCurrentThread->m_ulForbidTypeLoad;)

    {
        // An AV during a profiler stackwalk is an isolated event and shouldn't bring
        // down the runtime.  Need to place the holder here, outside of ProfilerStackWalkCallbackWrapper
        // since ProfilerStackWalkCallbackWrapper uses __try, which doesn't like objects
        // with destructors.
        AVInRuntimeImplOkayHolder AVOkay(TRUE /* take */);

        hr = ProfilerStackWalkCallbackWrapper(pThreadToSnapshot, &data, 
                 HANDLESKIPPEDFRAMES 
                    | FUNCTIONSONLY
                    | NOTIFY_ON_U2M_TRANSITIONS
                    | ((pThreadToSnapshot == pCurrentThread) ?
                        0 : 
                        ALLOW_ASYNC_STACK_WALK | THREAD_IS_SUSPENDED) 
                    | THREAD_EXECUTING_MANAGED_CODE
                    | PROFILER_DO_STACK_SNAPSHOT
                    | ALLOW_INVALID_OBJECTS);   // stack walk logic should not look at objects - we could be in the middle of a gc.
    }

    INDEBUG(if (pCurrentThread) pCurrentThread->m_ulForbidTypeLoad = ulForbidTypeLoad;)


Cleanup:
    // Undo the context seeding & thread suspend we did (if any)
    // to ensure that the thread we walked stayed suspended
    if (fResetProfilerFilterContext)
    {
        pThreadToSnapshot->SetProfilerFilterContext(NULL);
    }
    if (fResumeThread)
    {
        pThreadToSnapshot->ResumeThread();
    }
    if (fResetSnapshotThreadExternalCount)
    {
        pThreadToSnapshot->DecExternalCountDANGEROUSProfilerOnly();
    }

    return hr;
}


HRESULT ProfToEEInterfaceImpl::GetGenerationBounds(ULONG cObjectRanges,
                                                   ULONG *pcObjectRanges,
                                                   COR_PRF_GC_GENERATION_RANGE ranges[])
{
    CONTRACT(HRESULT)
    {
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(pcObjectRanges));
        PRECONDITION(cObjectRanges <= 0 || ranges != NULL);
        PRECONDITION(s_generationTableLock >= 0);
        POSTCONDITION(s_generationTableLock >= 0);
    } CONTRACT_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetGenerationBounds.\n"));

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    // Announce we are using the generation table now
    FastInterlockIncrement(&s_generationTableLock);

    GenerationTable *generationTable = s_currentGenerationTable;

    if (generationTable == NULL)
    {
        // Announce we are done using the generation table now
        FastInterlockDecrement(&s_generationTableLock);

        RETURN E_FAIL;
    }

    _ASSERTE(generationTable->magic == GENERATION_TABLE_MAGIC);

    GenerationDesc *genDescTable = generationTable->genDescTable;
    ULONG count = min(generationTable->count, cObjectRanges);
    for (ULONG i = 0; i < count; i++)
    {
        ranges[i].generation          = (COR_PRF_GC_GENERATION)genDescTable[i].generation;
        ranges[i].rangeStart          = (ObjectID)genDescTable[i].rangeStart;
        ranges[i].rangeLength         = genDescTable[i].rangeEnd         - genDescTable[i].rangeStart;
        ranges[i].rangeLengthReserved = genDescTable[i].rangeEndReserved - genDescTable[i].rangeStart;
    }

    *pcObjectRanges = generationTable->count;

    // Announce we are done using the generation table now
    FastInterlockDecrement(&s_generationTableLock);

    RETURN S_OK;
}


HRESULT ProfToEEInterfaceImpl::GetNotifiedExceptionClauseInfo(
                                COR_PRF_EX_CLAUSE_INFO *pinfo)
{
    CONTRACT(HRESULT)
    {
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(CheckPointer(pinfo));
    } CONTRACT_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetNotifiedExceptionClauseInfo.\n"));    

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    HRESULT hr = S_OK;

    ThreadExceptionState* pExState             = NULL;
    EHClauseInfo*         pCurrentEHClauseInfo = NULL;

    // notification requires that we are on a managed thread with an exception in flight
    Thread *pThread = GetThread();

    // If pThread is null, then the thread has never run managed code 
    if (pThread == NULL)
    {
        hr = CORPROF_E_NOT_MANAGED_THREAD;
        goto NullReturn;
    }

    pExState = pThread->GetExceptionState();
    if (!pExState->IsExceptionInProgress())
    {
        // no exception is in flight -- successful failure
        hr = S_FALSE;
        goto NullReturn;
    }

    pCurrentEHClauseInfo = pExState->GetCurrentEHClauseInfo();
    if (pCurrentEHClauseInfo->GetClauseType() == COR_PRF_CLAUSE_NONE)
    {
        // no exception is in flight -- successful failure
        hr = S_FALSE;
        goto NullReturn;
    }

    pinfo->clauseType     = pCurrentEHClauseInfo->GetClauseType();
    pinfo->programCounter = pCurrentEHClauseInfo->GetIPForEHClause();
    pinfo->framePointer   = pCurrentEHClauseInfo->GetFramePointerForEHClause();
    IA64_ONLY(pinfo->shadowStackPointer = pCurrentEHClauseInfo->GetBSPForEHClause();)
    NOT_IA64(pinfo->shadowStackPointer = 0;)

    RETURN S_OK;

NullReturn:
    memset(pinfo, 0, sizeof(*pinfo));
    RETURN hr;
}

/*
 * ProfilerCallbacksAllowed
 *
 * A silly wrapper around Thread::ProfilerCallbacksAllowed only because
 * we build the profiler portion of the EE outside the VM directory.
 *
 * Parameters:
 *   threadId - Really a pointer to a Thread passed to the profiler portion by the EE
 *
 * Returns:
 *   TRUE if the profiler portion has marked this thread as allowable, else FALSE.
 */
BOOL ProfToEEInterfaceImpl::ProfilerCallbacksAllowed(ThreadID threadId)
{
    LEAF_CONTRACT;

    return ((((Thread *)threadId) == NULL) || 
            (((Thread *)threadId)->ProfilerCallbacksAllowed()));
}

/*
 * SetProfilerCallbacksAllowed
 *
 * A silly wrapper around Thread::SetProfilerCallbacksAllowed only because
 * we build the profiler portion of the EE outside the VM directory.
 *
 * Parameters:
 *   threadId - Really a pointer to a Thread passed to the profiler portion by the EE
 *   fValue - The value to store.
 *
 * Returns:
 *   None.
 */
void ProfToEEInterfaceImpl::SetProfilerCallbacksAllowed(ThreadID threadId, BOOL fValue)
{
    LEAF_CONTRACT;

    _ASSERTE(threadId != NULL);

    ((Thread *)threadId)->SetProfilerCallbacksAllowed(fValue);
}

HRESULT ProfToEEInterfaceImpl::GetObjectGeneration(ObjectID objectId,
                                                   COR_PRF_GC_GENERATION_RANGE *range)
{
    CONTRACT(HRESULT)
    {
        NOTHROW;
        MODE_ANY;
        GC_NOTRIGGER;
        SO_NOT_MAINLINE;
        PRECONDITION(objectId != NULL);
        PRECONDITION(CheckPointer(range));
        PRECONDITION(s_generationTableLock >= 0);
        POSTCONDITION(s_generationTableLock >= 0);
    } CONTRACT_END;

    LOG((LF_CORPROF, LL_INFO1000, "**PROF: GetObjectGeneration 0x%p.\n", objectId));        

    ENSURE_CALLBACK_STATE_FLAGS_SET(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    // Announce we are using the generation table now
    FastInterlockIncrement(&s_generationTableLock);

    GenerationTable *generationTable = s_currentGenerationTable;

    if (generationTable == NULL)
    {
        // Announce we are done using the generation table now
        FastInterlockDecrement(&s_generationTableLock);
     
        RETURN E_FAIL;
    }

    _ASSERTE(generationTable->magic == GENERATION_TABLE_MAGIC);

    GenerationDesc *genDescTable = generationTable->genDescTable;
    ULONG count = generationTable->count;
    for (ULONG i = 0; i < count; i++)
    {
        if (genDescTable[i].rangeStart <= (BYTE *)objectId && (BYTE *)objectId < genDescTable[i].rangeEndReserved)
        {
            range->generation          = (COR_PRF_GC_GENERATION)genDescTable[i].generation;
            range->rangeStart          = (ObjectID)genDescTable[i].rangeStart;
            range->rangeLength         = genDescTable[i].rangeEnd         - genDescTable[i].rangeStart;
            range->rangeLengthReserved = genDescTable[i].rangeEndReserved - genDescTable[i].rangeStart;

            FastInterlockDecrement(&s_generationTableLock);

            RETURN S_OK;
        }
    }

    // Announce we are done using the generation table now
    FastInterlockDecrement(&s_generationTableLock);

    RETURN E_FAIL;
}

//
//
//  ProfilerEnum implementation
//
//




//
// ProfilerEnum::ProfilerEnum
//
// Description
//      The enumerator constructor
//
// Parameters
//      elements -- the array of elements in the enumeration.
//
// Notes
//      The enumerator does NOT take ownership of data in the array of elements;
//      it maintains its own private copy.
//
//
//
//
template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::ProfilerEnum(CDynArray< Element >* elements) :
    m_refCount(1),
    m_currentElement(0)
{
    const ULONG count = elements->Count();

    for (ULONG i = 0; i < count; ++i)
    {
        *m_elements.Append() = (*elements)[i];
    }
}

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::ProfilerEnum() :
    m_refCount(1),
    m_currentElement(0)
{
}

//
// ProfilerEnum::ProfileEnum
//
// Description
//      Copy constructor for enumerators
//
// Parameters
//      constOriginal -- a reference to the enumerator to copy
//
// Returns
//      None
//
// Notes
//      This function is private to ProfileEnum, and is for use only to
//      implement the Clone()
//
template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::ProfilerEnum(const ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >& constOriginal) :
    m_refCount(1),
    m_currentElement(constOriginal.m_currentElement)
{
    //  CDynArray isn't const-correct, so I have to cast away constness.
    //
    //  [this is safe, provided the CDynArray was not intially declared
    //   as const.]
    ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >&
        original(const_cast< ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >& >(constOriginal));

    const ULONG count = original.m_elements.Count();

    for (ULONG i = 0; i < count; ++i)
    {
        *m_elements.Append() = original.m_elements[i];
    }
}


//
// ProfilerEnum::ProfileEnum
//
// Description
//      Destructor for enumerators
//
// Parameters
//      None
//
// Returns
//      None
//
template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::~ProfilerEnum()
{
}

//
// ProfilerEnum::QueryInterface
//
// Description
//      dynamically cast this object to a specific interface.
//
// Parameters
//      id          -- the interface ID requested
//      ppInterface -- [out] pointer to the appropriate interface
//
// Returns
//      S_OK            -- if the QueryInterface succeeded
//      E_NOINTERFACE   -- if the enumerator does not implement the requested interface
//

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
HRESULT
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::QueryInterface(REFIID id, void** pInterface)
{
    if (*pEnumInterfaceIID == id)
    {
        *pInterface = static_cast< EnumInterface* >(this);
    }
    else if (IID_IUnknown == id)
    {
        *pInterface = static_cast< IUnknown* >(this);
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    this->AddRef();
    return S_OK;
}

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
ULONG
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
ULONG
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::Release()
{
    ULONG refCount = InterlockedIncrement(&m_refCount);

    if (0 == refCount)
    {
        delete this;
    }

    return refCount;
}

//
// ProfilerEnum::Next
//
// Description
//    Retrieves elements from the enumeration and advances the enumerator
//
// Parameters
//    elementsRequested -- the number of elements to read
//    elements -- [out] an array to store the retrieved elements
//    elementsFetched -- [out] the number of elements actually retrieved
//
//
// Returns
//    S_OK
//    S_FALSE
//    E_INVALIDARG
//
// Notes
//    if elementsRequested is 1 and elementsFetched is NULL, the enumerator will
//    try to advance 1 item and return S_OK if it is successful
//

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
HRESULT
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::Next(ULONG elementsRequested,
                                                                Element elements[],
                                                                ULONG* elementsFetched)
{
    // sanity check the location of the iterator
    _ASSERTE(0 <= m_currentElement);
    _ASSERTE((m_currentElement < static_cast< ULONG >(m_elements.Count())) || (0 == m_elements.Count()));

    // It's illegal to try and advance more than one element without giving a
    // legitimate pointer for elementsRequested
    if ((NULL == elementsFetched) && (1 != elementsRequested))
    {
        return E_INVALIDARG;
    }

    //  If, for some reason, you ask for zero elements, well, we'll just tell
    //  you that's fine.
    if (0 == elementsRequested)
    {
        if (NULL != elementsFetched)
        {
            *elementsFetched = 0;
        }

        return S_OK;
    }

    // okay, enough with the corner cases.

    // We don't want to walk past the end of our array, so figure out how far we
    // need to walk.
    const ULONG elementsToCopy = min(elementsRequested, m_elements.Count() - m_currentElement);

    for (ULONG i = 0; i < elementsToCopy; ++i)
    {
        elements[i] = m_elements[m_currentElement + i];
    }

    // advance the enumerator
    m_currentElement += elementsToCopy;

    // sanity check that we haven't gone any further than we were supposed to
    _ASSERTE(0 <= m_currentElement);
    _ASSERTE((m_currentElement <= static_cast< ULONG >(m_elements.Count())) || (0 == m_elements.Count()));


    if (NULL != elementsFetched)
    {
        *elementsFetched = elementsToCopy;
    }

    if (elementsToCopy < elementsRequested)
    {
        return S_FALSE;
    }

    return S_OK;
}


//
// ProfilerEnum:GetCount
//
// Description
//   Computes the number of elements remaining in the enumeration
//
// Parameters
//   count -- [out] the number of element remaining in the enumeration
//
// Returns
//   S_OK   always (function never fails)
//
//

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
HRESULT
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::GetCount(ULONG* count)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    if (NULL == count)
    {
        return E_INVALIDARG;
    }

    *count = m_elements.Count() - m_currentElement;

    return S_OK;
}

//
//
//
//
//
//
//
template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
HRESULT
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::Skip(ULONG count)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    const ULONG elementsToSkip = min(count, m_elements.Count() - m_currentElement);
    m_currentElement += elementsToSkip;

    if (elementsToSkip < count)
    {
        return S_FALSE;
    }

    return S_OK;
}



//
// ProfilerEnum::Reset
//
// Description
//  Returns the enumerator to the beginning of the enumeration
//
// Parameters
//  None
//
// Returns
//  S_OK -- always (function never fails)
//
//

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
HRESULT
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::Reset()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    m_currentElement = 0;
    return S_OK;
}

//
// ProfilerEnum::Clone
//
// Description
//  Creates a copy of this enumerator.
//
// Parameters
//  None
//
// Returns
//  S_OK -- always (function never fails)
//
//

template< typename EnumInterface, typename Element, const IID* pEnumInterfaceIID >
HRESULT
ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >::Clone(EnumInterface** pInterface)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        SO_NOT_MAINLINE;
    }
    CONTRACTL_END;

    if (NULL == pInterface)
    {
        return E_INVALIDARG;
    }

    *pInterface = new (nothrow) ProfilerEnum< EnumInterface, Element, pEnumInterfaceIID >(*this);

    return S_OK;
}

//
//
// Module helpers.
//
//

//*****************************************************************************
// Static creator
//*****************************************************************************
HRESULT ModuleILHeap::CreateNew(
    REFIID riid, void **pp, LPCBYTE pBase, ProfToEEInterfaceImpl *pParent, Module *pModule)
{
    HRESULT hr;

    ModuleILHeap *pHeap = new ModuleILHeap(pBase, pParent, pModule);
    if (!pHeap)
        hr = OutOfMemory();
    else
    {
        hr = pHeap->QueryInterface(riid, pp);
        pHeap->Release();
    }
    return (hr);
}


//*****************************************************************************
// ModuleILHeap ctor
//*****************************************************************************
ModuleILHeap::ModuleILHeap(LPCBYTE pBase, ProfToEEInterfaceImpl *pParent, Module *pModule) :
    m_cRef(1),
    m_pBase(pBase),
    m_pParent(pParent),
    m_pModule(pModule)
{
}


//*****************************************************************************
// AddRef
//*****************************************************************************
ULONG ModuleILHeap::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}


//*****************************************************************************
// Release
//*****************************************************************************
ULONG ModuleILHeap::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
        delete this;
    return (cRef);
}


//*****************************************************************************
// QI
//*****************************************************************************
HRESULT ModuleILHeap::QueryInterface(REFIID riid, void **pp)
{
    HRESULT     hr = S_OK;

    if (pp == NULL)
        return (E_POINTER);

    *pp = 0;
    if (riid == IID_IUnknown)
        *pp = (IUnknown *) this;
    else if (riid == IID_IMethodMalloc)
        *pp = (IMethodMalloc *) this;
    else
        hr = E_NOINTERFACE;

    if (hr == S_OK)
        AddRef();
    return (hr);
}


//*****************************************************************************
// Alloc
//*****************************************************************************
void *STDMETHODCALLTYPE ModuleILHeap::Alloc(
        /* [in] */ ULONG cb)
{
    return m_pParent->Alloc(m_pBase, cb, m_pModule);
}

void __stdcall ProfilerManagedToUnmanagedTransition(Frame *pFrame,
                                                    COR_PRF_TRANSITION_REASON reason)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        STUBS_AS_IL_ONLY(MODE_PREEMPTIVE);  // x86 may call this function in cooperative mode, which seems like a bug
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_ENTRYPOINT_VOIDRET;

    MethodDesc *pMD = pFrame->GetFunction();
    if (pMD != NULL)
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ManagedToUnmanagedTransition(MethodDescToCodeID(pMD),
                                                                        reason);
    }
    END_ENTRYPOINT_VOIDRET;

}

void __stdcall ProfilerUnmanagedToManagedTransition(Frame *pFrame,
                                                    COR_PRF_TRANSITION_REASON reason)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        STUBS_AS_IL_ONLY(MODE_PREEMPTIVE);  // x86 may call this function in cooperative mode, which seems like a bug
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BEGIN_ENTRYPOINT_VOIDRET;

    MethodDesc *pMD = pFrame->GetFunction();
    if (pMD != NULL)
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->UnmanagedToManagedTransition(MethodDescToCodeID(pMD),
                                                                        reason);
    }
    END_ENTRYPOINT_VOIDRET;

}

void __stdcall ProfilerManagedToUnmanagedTransitionMD(MethodDesc *pMD,
                                                      COR_PRF_TRANSITION_REASON reason)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        STUBS_AS_IL_ONLY(MODE_PREEMPTIVE);  // x86 may call this function in cooperative mode, which seems like a bug
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // On WIN64, this function is called within the runtime, not directly from managed code.
    // Also, the only case where WIN64 has a NULL MD is the calli pinvoke case, and we still
    // want to notify the profiler in that case.
    NOT_STUBS_AS_IL(BEGIN_ENTRYPOINT_VOIDRET);

    if (pMD != NULL)
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->ManagedToUnmanagedTransition(MethodDescToCodeID(pMD),
                                                                        reason);
    }

    NOT_STUBS_AS_IL(END_ENTRYPOINT_VOIDRET);
}

void __stdcall ProfilerUnmanagedToManagedTransitionMD(MethodDesc *pMD,
                                                      COR_PRF_TRANSITION_REASON reason)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        STUBS_AS_IL_ONLY(MODE_PREEMPTIVE);  // x86 may call this function in cooperative mode, which seems like a bug
        SO_TOLERANT;
    }
    CONTRACTL_END;

    // On WIN64, this function is called within the runtime, not directly from managed code.
    // Also, the only case where WIN64 has a NULL MD is the calli pinvoke case, and we still
    // want to notify the profiler in that case.
    NOT_STUBS_AS_IL(BEGIN_ENTRYPOINT_VOIDRET);

    if (pMD != NULL)
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->UnmanagedToManagedTransition(MethodDescToCodeID(pMD),
                                                                        reason);
    }

    NOT_STUBS_AS_IL(END_ENTRYPOINT_VOIDRET);
}

/**********************************************************************************************
 * These are helper functions for the GC events
 **********************************************************************************************/

class CObjectHeader;

BOOL CountContainedObjectRef(Object* pBO, void *context)
{
    // Increase the count
    (*((size_t *)context))++;

    return (TRUE);
}

BOOL SaveContainedObjectRef(Object* pBO, void *context)
{
    // Assign the value
    **((BYTE ***)context) = (BYTE *)pBO;

    // Now increment the array pointer
    (*((Object ***)context))++;

    return (TRUE);
}

BOOL HeapWalkHelper(Object* pBO, void* pv)
{
    OBJECTREF   *arrObjRef      = NULL;
    ULONG        cNumRefs       = 0;
    bool         bOnStack       = false;
    MethodTable *pMT            = pBO->GetMethodTable();

    if (pMT->ContainsPointers())
    {
        // First round through calculates the number of object refs for this class
        GCHeap::GetGCHeap()->WalkObject(pBO, &CountContainedObjectRef, (void *)&cNumRefs);

        if (cNumRefs > 0)
        {
            // Create an array to contain all of the refs for this object
            bOnStack = cNumRefs <= 32 ? true : false;

            // If it's small enough, just allocate on the stack
            if (bOnStack)
                arrObjRef = (OBJECTREF *)_alloca(cNumRefs * sizeof(OBJECTREF));

            // Otherwise, allocate from the heap
            else
            {
                arrObjRef = new OBJECTREF[cNumRefs];

                if (!arrObjRef)
                    return (FALSE);
            }

            // Second round saves off all of the ref values
            OBJECTREF *pCurObjRef = arrObjRef;
            GCHeap::GetGCHeap()->WalkObject(pBO, &SaveContainedObjectRef, (void *)&pCurObjRef);
        }
    }

    HRESULT hr = S_OK;
    {
        PROFILER_CALL;
        
        hr = g_profControlBlock.pProfInterface->
            ObjectReference((ObjectID) pBO, TypeHandleToTypeID(pBO->GetTypeHandle()),
                            cNumRefs, (ObjectID *)arrObjRef);
    }
    // If the data was not allocated on the stack, need to clean it up.
    if (arrObjRef != NULL && !bOnStack)
        delete [] arrObjRef;

    // Must return the hr from the callback, as a failed hr will cause
    // the heap walk to cease
    return (SUCCEEDED(hr));
}

BOOL AllocByClassHelper(Object* pBO, void* pv)
{
    _ASSERTE(pv != NULL);

    HRESULT hr;

    // Pass along the call
    PROFILER_CALL;
    hr = g_profControlBlock.pProfInterface->AllocByClass(
        (ObjectID) pBO, TypeHandleToTypeID(pBO->GetTypeHandle()), pv);

    _ASSERTE(SUCCEEDED(hr));

    return (TRUE);
}

void ScanRootsHelper(Object*& o, ScanContext *pSC, DWORD dwFlags)
{
    // RootReference2 can return E_OUTOFMEMORY, and we're swallowing that.
    // Furthermore, we can't really handle it because we're callable during GC promotion.
    // On the other hand, this only means profiling information will be incomplete,
    // so it's ok to swallow E_OUTOFMEMORY.
    //
    FAULT_NOT_FATAL();

    ProfilingScanContext *pPSC = (ProfilingScanContext *)pSC;

    DWORD rootFlags = 0;
    if (dwFlags & GC_CALL_INTERIOR)
        rootFlags |= COR_PRF_GC_ROOT_INTERIOR;
    if (dwFlags & GC_CALL_PINNED)
        rootFlags |= COR_PRF_GC_ROOT_PINNING;
    void *rootID = NULL;
    switch (pPSC->rootKind)
    {
    case    COR_PRF_GC_ROOT_STACK:
        rootID = pPSC->pMD;
        break;

    case    COR_PRF_GC_ROOT_HANDLE:
        _ASSERT(!"Shouldn't see handle here");

    case    COR_PRF_GC_ROOT_FINALIZER:
    default:
        break;
    }
    // Let the profiling code know about this root reference
    PROFILER_CALL;
    g_profControlBlock.pProfInterface->
        RootReference2((BYTE *)o, pPSC->rootKind, (COR_PRF_GC_ROOT_FLAGS)rootFlags, (BYTE *)rootID, &((pPSC)->pHeapId));
}

#endif // PROFILING_SUPPORTED


FCIMPL0(FC_BOOL_RET, ProfilingFCallHelper::FC_TrackRemoting)
{
#ifdef PROFILING_SUPPORTED
    FC_RETURN_BOOL(CORProfilerTrackRemoting());
#else // !PROFILING_SUPPORTED
    FC_RETURN_BOOL(FALSE);
#endif // !PROFILING_SUPPORTED
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, ProfilingFCallHelper::FC_TrackRemotingCookie)
{
#ifdef PROFILING_SUPPORTED
    FC_RETURN_BOOL(CORProfilerTrackRemotingCookie());
#else // !PROFILING_SUPPORTED
    FC_RETURN_BOOL(FALSE);
#endif // !PROFILING_SUPPORTED
}
FCIMPLEND

FCIMPL0(FC_BOOL_RET, ProfilingFCallHelper::FC_TrackRemotingAsync)
{
#ifdef PROFILING_SUPPORTED
    FC_RETURN_BOOL(CORProfilerTrackRemotingAsync());
#else // !PROFILING_SUPPORTED
    FC_RETURN_BOOL(FALSE);
#endif // !PROFILING_SUPPORTED
}
FCIMPLEND

FCIMPL2(void, ProfilingFCallHelper::FC_RemotingClientSendingMessage, GUID *pId, CLR_BOOL fIsAsync)
{
#ifdef PROFILING_SUPPORTED
    // Need to erect a GC frame so that GCs can occur without a problem
    // within the profiler code.

    // Note that we don't need to worry about pId moving around since
    // it is a value class declared on the stack and so GC doesn't
    // know about it.

    _ASSERTE (!GCHeap::GetGCHeap()->IsHeapPointer(pId));     // should be on the stack, not in the heap
    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    if (CORProfilerTrackRemotingCookie())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->GetGUID(pId);
        _ASSERTE(pId->Data1);

        g_profControlBlock.pProfInterface->RemotingClientSendingMessage(
            reinterpret_cast<ThreadID>(GetThread()), pId, fIsAsync);
    }
    else
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingClientSendingMessage(
                reinterpret_cast<ThreadID>(GetThread()), NULL, fIsAsync);
    }
    HELPER_METHOD_FRAME_END_POLL();
#endif // PROFILING_SUPPORTED
}
FCIMPLEND


FCIMPL2_VI(void, ProfilingFCallHelper::FC_RemotingClientReceivingReply, GUID id, CLR_BOOL fIsAsync)
{
#ifdef PROFILING_SUPPORTED
    // Need to erect a GC frame so that GCs can occur without a problem
    // within the profiler code.

    // Note that we don't need to worry about pId moving around since
    // it is a value class declared on the stack and so GC doesn't
    // know about it.

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    if (CORProfilerTrackRemotingCookie())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingClientReceivingReply(
            reinterpret_cast<ThreadID>(GetThread()), &id, fIsAsync);
    }
    else
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingClientReceivingReply(
            reinterpret_cast<ThreadID>(GetThread()), NULL, fIsAsync);
    }

    HELPER_METHOD_FRAME_END_POLL();
#endif // PROFILING_SUPPORTED
}
FCIMPLEND


FCIMPL2_VI(void, ProfilingFCallHelper::FC_RemotingServerReceivingMessage, GUID id, CLR_BOOL fIsAsync)
{
#ifdef PROFILING_SUPPORTED
    // Need to erect a GC frame so that GCs can occur without a problem
    // within the profiler code.

    // Note that we don't need to worry about pId moving around since
    // it is a value class declared on the stack and so GC doesn't
    // know about it.

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();


    if (CORProfilerTrackRemotingCookie())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingServerReceivingMessage(
            reinterpret_cast<ThreadID>(GetThread()), &id, fIsAsync);
    }
    else
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingServerReceivingMessage(
            reinterpret_cast<ThreadID>(GetThread()), NULL, fIsAsync);
    }

    HELPER_METHOD_FRAME_END_POLL();
#endif // PROFILING_SUPPORTED
}
FCIMPLEND

FCIMPL2(void, ProfilingFCallHelper::FC_RemotingServerSendingReply, GUID *pId, CLR_BOOL fIsAsync)
{
#ifdef PROFILING_SUPPORTED
    // Need to erect a GC frame so that GCs can occur without a problem
    // within the profiler code.

    // Note that we don't need to worry about pId moving around since
    // it is a value class declared on the stack and so GC doesn't
    // know about it.

    HELPER_METHOD_FRAME_BEGIN_NOPOLL();

    if (CORProfilerTrackRemotingCookie())
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->GetGUID(pId);
        _ASSERTE(pId->Data1);

        g_profControlBlock.pProfInterface->RemotingServerSendingReply(
            reinterpret_cast<ThreadID>(GetThread()), pId, fIsAsync);
    }
    else
    {
        PROFILER_CALL;
        g_profControlBlock.pProfInterface->RemotingServerSendingReply(
            reinterpret_cast<ThreadID>(GetThread()), NULL, fIsAsync);
    }

    HELPER_METHOD_FRAME_END_POLL();
#endif // PROFILING_SUPPORTED
}
FCIMPLEND

//
// Define wrapper functions for rotor.
//
// NOTE: These do not currently implement correctly passing the platform
// specific handle.  The Rotor people need to implement these correctly if
// they care to support getting arguments, return value, and generic information.
//
#if !defined(_X86_) && !defined(_WIN64)

FCIMPL4(EXTERN_C void, ProfileEnterWrapper, FunctionID functionId,
                                            void *clientData,
                                            COR_PRF_FRAME_INFO *pFrameInfo,
                                            COR_PRF_FUNCTION_ARGUMENT_INFO *pArgInfo)
{
    //
    // Create some empty buffer space for the platformSpecificHandle
    //
    BYTE buffer[60] = {0};

    PORTABILITY_ASSERT("ProfileEnterWrapper not implemented");
    PROFILER_CALL;
    ProfileEnter(functionId, clientData, &buffer);
}
FCIMPLEND

FCIMPL4(EXTERN_C void, ProfileLeaveWrapper, FunctionID functionId,
                                            void *clientData,
                                            COR_PRF_FRAME_INFO *pFrameInfo,
                                            COR_PRF_FUNCTION_ARGUMENT_RANGE *pArgInfo)
{
    //
    // Create some empty buffer space for the platformSpecificHandle
    //
    BYTE buffer[60] = {0};

    PORTABILITY_ASSERT("ProfileLeaveWrapper not implemented");

    PROFILER_CALL;
    ProfileLeave(functionId, clientData, &buffer);
}
FCIMPLEND

FCIMPL3(EXTERN_C void, ProfileTailcallWrapper, FunctionID functionId,
                                               void *clientData,
                                               COR_PRF_FRAME_INFO *pFrameInfo)
{
    //
    // Create some empty buffer space for the platformSpecificHandle
    //
    BYTE buffer[60] = {0};

    PORTABILITY_ASSERT("ProfileTailcallWrapper not implemented");

    PROFILER_CALL;
    ProfileTailcall(functionId, clientData, &buffer);
}
FCIMPLEND

#endif


//*******************************************************************************************
// These do a lot of work for us, setting up Frames, gathering arg info and resolving generics.
//*******************************************************************************************


HCIMPL3(EXTERN_C void, ProfileEnter, FunctionID functionId, void *clientData, void *platformSpecificHandle)
{
#ifdef PROFILING_SUPPORTED
    //
    // Set up a frame
    //
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_CAPTURE_DEPTH_2);    // Set up a frame

    _ASSERTE(GetThread()->PreemptiveGCDisabled());

    SetCallbackStateFlagsFromVMHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);

    LPVOID pThis = NULL;
    
    //
    // If we have an Enter2 callback, that gets precedence.
    //
    if (g_profControlBlock.pEnter2 != NULL)
    {
        COR_PRF_FUNCTION_ARGUMENT_INFO *argumentInfo = NULL;
        COR_PRF_FRAME_INFO_INTERNAL *pFrameInfo = NULL;
        COR_PRF_FRAME_INFO_INTERNAL frameInfo;

        if (CORProfilerFrameInfoEnabled() || CORProfilerFunctionArgsEnabled())
        {
            //
            // Find the method this is referring to, so we can get the signature
            //
            MethodDesc *pMethodDesc = FunctionIdToMethodDesc(functionId);
            MetaSig metaSig(pMethodDesc);

            ProfileArgIterator profileArgIterator(&metaSig, platformSpecificHandle);

            if (CORProfilerFrameInfoEnabled())
            {
                //
                // Setup the COR_PRF_FRAME_INFO structure first.
                //
                frameInfo.size = sizeof(COR_PRF_FRAME_INFO_INTERNAL);
                frameInfo.version = COR_PRF_FRAME_INFO_INTERNAL_CURRENT_VERSION;
                frameInfo.funcID = functionId;
                frameInfo.IP = ProfileGetIPFromPlatformSpecificHandle(platformSpecificHandle);
                frameInfo.extraArg = profileArgIterator.GetHiddenArgValue();
                frameInfo.thisArg = profileArgIterator.GetThis();
                pFrameInfo = &frameInfo;
            }

            //
            // Do argument processing if desired.
            //
            if (CORProfilerFunctionArgsEnabled())
            {
                ULONG32 count = 0;
                ULONG32 length;
                BYTE dummyType;

                if (metaSig.HasThis())
                {
                    count++;
                }

                // -1 is a valid return here from a counting perspective. It indicates
                // an argument where a buffer is needed to be created by the caller.
                while (profileArgIterator.GetNextArgAddr(&dummyType, &length) != NULL)
                {
                    count++;
                }

                argumentInfo = (COR_PRF_FUNCTION_ARGUMENT_INFO *)_alloca(sizeof(COR_PRF_FUNCTION_ARGUMENT_INFO) +
                                                                         (count * sizeof(COR_PRF_FUNCTION_ARGUMENT_RANGE)));

                argumentInfo->numRanges = count;
                argumentInfo->totalArgumentSize = 0;

                profileArgIterator.Init(&metaSig, platformSpecificHandle);

                count = 0;

                if (metaSig.HasThis())
                {
                    // startAddress needs to point TO the argument value, not BE the argument value.  So,
                    // when the argument is this, we need to point TO this.  Since the profiler is not allowed to
                    // use these values outside of its enter hook, it's safe for us to pass back a pointer
                    // to one of our stack variables (pThis).
                    pThis = profileArgIterator.GetThis();
                    argumentInfo->ranges[count].startAddress = (UINT_PTR) (&pThis);
                    argumentInfo->ranges[count].length = sizeof(pThis);
                    argumentInfo->totalArgumentSize += argumentInfo->ranges[count].length;
                    count++;
                }

#ifdef _DEBUG
                IA64_ONLY(int fillAggregateCount = 0);
#endif

                while (count < argumentInfo->numRanges)
                {
                    argumentInfo->ranges[count].startAddress = (UINT_PTR)(profileArgIterator.GetNextArgAddr(&dummyType, &length));
                    argumentInfo->ranges[count].length = length;


                    argumentInfo->totalArgumentSize += length;
                    count++;
                }
            }
        }

        PROFILER_CALL;
        g_profControlBlock.pEnter2(functionId, (UINT_PTR)clientData, (COR_PRF_FRAME_INFO)pFrameInfo, argumentInfo);
        goto LExit;
    }

    // We will not be here unless the jit'd or ngen'd function we're about to enter
    // was backpatched with this wrapper around the profiler's hook, and that
    // wouldn't have happened unless the profiler supplied us with a hook
    // in the first place.  (Note that SetEnterLeaveFunctionHooks* will return
    // an error unless it's called in the profiler's Initialize(), so a profiler can't change
    // its mind about where the hooks are.)
    _ASSERTE(g_profControlBlock.pEnter != NULL);

    // Note that we cannot assert CORProfilerTrackEnterLeave() (i.e., profiler flag
    // COR_PRF_MONITOR_ENTERLEAVE), because the profiler may decide whether
    // to enable the jitter to add enter/leave callouts independently of whether
    // the profiler actually has enter/leave hooks.  (If the profiler has no such hooks,
    // the callouts quickly return and do nothing.)

    if (clientData != NULL)
    {
        PROFILER_CALL;
        g_profControlBlock.pEnter((FunctionID)clientData);
    }
    else
    {
        PROFILER_CALL;
        g_profControlBlock.pEnter(functionId);
    }

LExit:

    ;

    HELPER_METHOD_FRAME_END();      // Un-link the frame

#endif // PROFILING_SUPPORTED
}
HCIMPLEND

HCIMPL3(EXTERN_C void, ProfileLeave, FunctionID functionId, void *clientData, void *platformSpecificHandle)
{
    FC_GC_POLL_NOT_NEEDED();            // we pulse GC mode, so we are doing a poll

#ifdef PROFILING_SUPPORTED

    //
    // Set up a frame
    //
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_CAPTURE_DEPTH_2);    // Set up a frame

    _ASSERTE(GetThread()->PreemptiveGCDisabled());

    SetCallbackStateFlagsFromVMHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // If we have an Leave2 callback, that gets precedence.
    //
    if (g_profControlBlock.pLeave2 != NULL)
    {
        COR_PRF_FUNCTION_ARGUMENT_RANGE *pArgumentRange = NULL;
        COR_PRF_FUNCTION_ARGUMENT_RANGE argumentRange;

        COR_PRF_FRAME_INFO_INTERNAL *pFrameInfo = NULL;
        COR_PRF_FRAME_INFO_INTERNAL frameInfo;

        if (CORProfilerFrameInfoEnabled() || CORProfilerFunctionReturnValueEnabled())
        {
            //
            // Find the method this is referring to, so we can get the signature
            //
            if (CORProfilerFrameInfoEnabled())
            {
                //
                // Setup the COR_PRF_FRAME_INFO structure first.
                //
                frameInfo.size = sizeof(COR_PRF_FRAME_INFO_INTERNAL);
                frameInfo.version = COR_PRF_FRAME_INFO_INTERNAL_CURRENT_VERSION;
                frameInfo.funcID = functionId;
                frameInfo.IP = ProfileGetIPFromPlatformSpecificHandle(platformSpecificHandle);
                frameInfo.extraArg = NULL;
                frameInfo.thisArg = NULL;
                pFrameInfo = &frameInfo;
            }

            //
            // Do argument processing if desired.
            //
            if (CORProfilerFunctionReturnValueEnabled())
            {
                MethodDesc *pMethodDesc = FunctionIdToMethodDesc(functionId);
                MetaSig metaSig(pMethodDesc);

                ProfileArgIterator profileArgIterator(&metaSig, platformSpecificHandle);

                if (!metaSig.IsReturnTypeVoid())
                {
                    argumentRange.length = metaSig.GetReturnTypeSize();
                    argumentRange.startAddress = (UINT_PTR)profileArgIterator.GetReturnBufferAddr();
                }
                else
                {
                    argumentRange.length = 0;
                    argumentRange.startAddress = 0;
                }

                pArgumentRange = &argumentRange;
            }
        }

        PROFILER_CALL;
        g_profControlBlock.pLeave2(functionId, (UINT_PTR)clientData, (COR_PRF_FRAME_INFO)pFrameInfo, pArgumentRange);
        goto LExit;
    }

    // We will not be here unless the jit'd or ngen'd function we're about to leave
    // was backpatched with this wrapper around the profiler's hook, and that
    // wouldn't have happened unless the profiler supplied us with a hook
    // in the first place.  (Note that SetEnterLeaveFunctionHooks* will return
    // an error unless it's called in the profiler's Initialize(), so a profiler can't change
    // its mind about where the hooks are.)
    _ASSERTE(g_profControlBlock.pLeave != NULL);

    // Note that we cannot assert CORProfilerTrackEnterLeave() (i.e., profiler flag
    // COR_PRF_MONITOR_ENTERLEAVE), because the profiler may decide whether
    // to enable the jitter to add enter/leave callouts independently of whether
    // the profiler actually has enter/leave hooks.  (If the profiler has no such hooks,
    // the callouts quickly return and do nothing.)

    if (clientData != NULL)
    {
        PROFILER_CALL;
        g_profControlBlock.pLeave((FunctionID)clientData);
    }
    else
    {
        PROFILER_CALL;
        g_profControlBlock.pLeave(functionId);
    }

LExit:

    ;

    HELPER_METHOD_FRAME_END();      // Un-link the frame

#endif // PROFILING_SUPPORTED
}
HCIMPLEND

HCIMPL3(EXTERN_C void, ProfileTailcall, FunctionID functionId, void *clientData, void *platformSpecificHandle)
{
    FC_GC_POLL_NOT_NEEDED();            // we pulse GC mode, so we are doing a poll

#ifdef PROFILING_SUPPORTED

    //
    // Set up a frame
    //
    HELPER_METHOD_FRAME_BEGIN_ATTRIB_NOPOLL(Frame::FRAME_ATTR_CAPTURE_DEPTH_2);    // Set up a frame

    _ASSERTE(GetThread()->PreemptiveGCDisabled());

    SetCallbackStateFlagsFromVMHolder csf(COR_PRF_CALLBACKSTATE_INCALLBACK);
    
    //
    // If we have an Tailcall2 callback, that gets precedence.
    //
    if (g_profControlBlock.pTailcall2 != NULL)
    {
        COR_PRF_FRAME_INFO_INTERNAL *pFrameInfo = NULL;
        COR_PRF_FRAME_INFO_INTERNAL frameInfo;

        if (CORProfilerFrameInfoEnabled())
        {
            //
            // Find the method this is referring to, so we can get the signature
            //
            MethodDesc *pMethodDesc = FunctionIdToMethodDesc(functionId);
            MetaSig metaSig(pMethodDesc);

            ProfileArgIterator profileArgIterator(&metaSig, platformSpecificHandle);

            //
            // Setup the COR_PRF_FRAME_INFO structure first.
            //
            frameInfo.size = sizeof(COR_PRF_FRAME_INFO_INTERNAL);
            frameInfo.version = COR_PRF_FRAME_INFO_INTERNAL_CURRENT_VERSION;
            frameInfo.funcID = functionId;
            frameInfo.IP = ProfileGetIPFromPlatformSpecificHandle(platformSpecificHandle);
            frameInfo.extraArg = profileArgIterator.GetHiddenArgValue();
            frameInfo.thisArg = profileArgIterator.GetThis();
            pFrameInfo = &frameInfo;
        }

        PROFILER_CALL;
        g_profControlBlock.pTailcall2(functionId, (UINT_PTR)clientData, (COR_PRF_FRAME_INFO)pFrameInfo);
        goto LExit;
    }

    // We will not be here unless the jit'd or ngen'd function we're about to tailcall
    // was backpatched with this wrapper around the profiler's hook, and that
    // wouldn't have happened unless the profiler supplied us with a hook
    // in the first place.  (Note that SetEnterLeaveFunctionHooks* will return
    // an error unless it's called in the profiler's Initialize(), so a profiler can't change
    // its mind about where the hooks are.)
    _ASSERTE(g_profControlBlock.pTailcall != NULL);

    // Note that we cannot assert CORProfilerTrackEnterLeave() (i.e., profiler flag
    // COR_PRF_MONITOR_ENTERLEAVE), because the profiler may decide whether
    // to enable the jitter to add enter/leave callouts independently of whether
    // the profiler actually has enter/leave hooks.  (If the profiler has no such hooks,
    // the callouts quickly return and do nothing.)
    
    if (clientData != NULL)
    {
        PROFILER_CALL;
        g_profControlBlock.pTailcall((FunctionID)clientData);
    }
    else
    {
        PROFILER_CALL;
        g_profControlBlock.pTailcall(functionId);
    }

LExit:

    ;

    HELPER_METHOD_FRAME_END();      // Un-link the frame

#endif // PROFILING_SUPPORTED
}
HCIMPLEND
