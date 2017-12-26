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
//*****************************************************************************

#ifndef _EEPROFINTERFACES_H_
#define _EEPROFINTERFACES_H_

#include <stddef.h>
#include "corprof.h"
#include "profilepriv.h"

#define PROF_USER_MASK 0xFFFFFFFF

class EEToProfInterface;
class ProfToEEInterface;
class Thread;
class Frame;
class MethodDesc;
class Object;
class Module;

// This file defines the _internal_ interface between the EE and the
// implementation of the COM profiling API.  The _external_ API is defined
// in inc/corprof.idl.
//
// Most IDs used by the _external_ API are just the pointer values
// of the corresponding CLR data structure.  However, the external API uses
// different IDs for classes and functions to the internal API, because
// the eternal API always uses the "typical" (i.e. canonicalized)  versions
// of instantiated classes and functions in order to remove any traces of
// However the internal API does use IDs which can represent instantitions,
// and leaves it up to the external API to canonicalize these.
// We do it this way because it is useful to locate the normalization calls within
// the implementation of the COM profiling API, and we want to leave room for future
// APIs to reveal information about instantiations.
//
// Summary: ClassID or FunctionID do not represent instantiations.
// Summary: TypeID and CodeID can represent instantiations of classes and functions respectively.
//
// We use dummy structs for TypeID and CodeID to enforce strong typing, as in the JIT interface.
typedef struct TypeID_STRUCT_*      TypeID;
typedef struct CodeID_STRUCT_*      CodeID;


#ifndef DEFINE_OBJECTREF
#define DEFINE_OBJECTREF
#ifdef _DEBUG_IMPL
class OBJECTREF;
#else
typedef DPTR(class Object) PTR_Object;
typedef PTR_Object OBJECTREF;
#endif
#endif

/*
 * The following methods dispatch allocations tracking to the profiler as
 * well as the method table reordering codes (as appropriate).
 */

bool __stdcall TrackAllocationsEnabled();

void __stdcall ProfilerObjectAllocatedCallback(OBJECTREF objref, TypeID typeId);

void __stdcall GarbageCollectionStartedCallback(int generation, BOOL induced);

void __stdcall GarbageCollectionFinishedCallback();

void __stdcall UpdateGenerationBounds();
/*
 * GetEEProfInterface is used to get the interface with the profiler code.
 */
typedef void __cdecl GETEETOPROFINTERFACE(EEToProfInterface **ppEEProf);

/*
 * SetProfEEInterface is used to provide the profiler code with an interface
 * to the profiler.
 */
typedef void __cdecl SETPROFTOEEINTERFACE(ProfToEEInterface *pProfEE);

/*
 * This virtual class provides the EE with access to the Profiling code.
 */
class EEToProfInterface
{
public:
    virtual ~EEToProfInterface()
    {
    }

    virtual HRESULT Init() = 0;

    virtual void Terminate(BOOL fProcessDetach) = 0;

    virtual HRESULT CreateProfiler(WCHAR *wszCLSID, WCHAR *wszProfileDLL) = 0;

    virtual HRESULT ThreadCreated(ThreadID threadID) = 0;

    virtual HRESULT ThreadDestroyed(ThreadID threadID) = 0;

    virtual HRESULT ThreadAssignedToOSThread(ThreadID managedThreadId, DWORD osThreadId) = 0;

    virtual HRESULT ThreadNameChanged(ThreadID managedThreadId, ULONG cchName, WCHAR name[]) = 0;

    virtual HRESULT Shutdown(ThreadID threadID) = 0;

    virtual HRESULT FunctionUnloadStarted(ThreadID threadID, CodeID codeId) = 0;

    virtual HRESULT JITCompilationFinished(ThreadID threadID, CodeID codeId,
                                           HRESULT hrStatus, BOOL fIsSafeToBlock) = 0;

    virtual HRESULT JITCompilationStarted(ThreadID threadId, CodeID codeId,
                                          BOOL fIsSafeToBlock) = 0;

	virtual HRESULT JITCachedFunctionSearchStarted(
		/* [in] */	ThreadID threadId,
        /* [in] */  CodeID codeId,
		/* [out] */ BOOL *pbUseCachedFunction) = 0;

	virtual HRESULT JITCachedFunctionSearchFinished(
		/* [in] */	ThreadID threadId,
		/* [in] */  CodeID codeId,
		/* [in] */  COR_PRF_JIT_CACHE result) = 0;

    virtual HRESULT JITInlining(
        /* [in] */  ThreadID      threadId,
        /* [in] */  CodeID    callerId,
        /* [in] */  CodeID    calleeId,
        /* [out] */ BOOL         *pfShouldInline) = 0;

    virtual HRESULT JITFunctionPitched(ThreadID threadId,
                                             CodeID codeId) = 0;

	virtual HRESULT ModuleLoadStarted(ThreadID threadID, ModuleID moduleId) = 0;

	virtual HRESULT ModuleLoadFinished(
        ThreadID    threadID,
		ModuleID	moduleId,
		HRESULT		hrStatus) = 0;

	virtual HRESULT ModuleUnloadStarted(
        ThreadID    threadID,
        ModuleID    moduleId) = 0;

	virtual HRESULT ModuleUnloadFinished(
        ThreadID    threadID,
        ModuleID	moduleId,
		HRESULT		hrStatus) = 0;

    virtual HRESULT ModuleAttachedToAssembly(
        ThreadID    threadID,
        ModuleID    moduleId,
        AssemblyID  AssemblyId) = 0;

	virtual HRESULT ClassLoadStarted(
        ThreadID    threadID,
		TypeID		typeId) = 0;

	virtual HRESULT ClassLoadFinished(
        ThreadID    threadID,
		TypeID		typeId,
		HRESULT		hrStatus) = 0;

	virtual HRESULT ClassUnloadStarted(
        ThreadID    threadID,
		TypeID		typeId) = 0;

	virtual HRESULT ClassUnloadFinished(
        ThreadID    threadID,
		TypeID		typeId,
		HRESULT		hrStatus) = 0;

    virtual HRESULT AppDomainCreationStarted(
        ThreadID    threadId,
        AppDomainID appDomainId) = 0;

    virtual HRESULT AppDomainCreationFinished(
        ThreadID    threadId,
        AppDomainID appDomainId,
        HRESULT     hrStatus) = 0;

    virtual HRESULT AppDomainShutdownStarted(
        ThreadID    threadId,
        AppDomainID appDomainId) = 0;

    virtual HRESULT AppDomainShutdownFinished(
        ThreadID    threadId,
        AppDomainID appDomainId,
        HRESULT     hrStatus) = 0;

    virtual HRESULT AssemblyLoadStarted(
        ThreadID    threadId,
        AssemblyID  appDomainId) = 0;

    virtual HRESULT AssemblyLoadFinished(
        ThreadID    threadId,
        AssemblyID  appDomainId,
        HRESULT     hrStatus) = 0;

    virtual HRESULT AssemblyUnloadStarted(
        ThreadID    threadId,
        AssemblyID  appDomainId) = 0;

    virtual HRESULT AssemblyUnloadFinished(
        ThreadID    threadId,
        AssemblyID  appDomainId,
        HRESULT     hrStatus) = 0;

    virtual HRESULT UnmanagedToManagedTransition(
        CodeID codeId,
        COR_PRF_TRANSITION_REASON reason) = 0;

    virtual HRESULT ManagedToUnmanagedTransition(
        CodeID codeId,
        COR_PRF_TRANSITION_REASON reason) = 0;

    virtual HRESULT ExceptionThrown(
        ThreadID threadId,
        ObjectID thrownObjectId) = 0;

    virtual HRESULT ExceptionSearchFunctionEnter(
        ThreadID threadId,
        CodeID codeId) = 0;

    virtual HRESULT ExceptionSearchFunctionLeave(
        ThreadID threadId) = 0;

    virtual HRESULT ExceptionSearchFilterEnter(
        ThreadID threadId,
        CodeID codeId) = 0;

    virtual HRESULT ExceptionSearchFilterLeave(
        ThreadID threadId) = 0;

    virtual HRESULT ExceptionSearchCatcherFound (
        ThreadID threadId,
        CodeID codeId) = 0;

    virtual HRESULT ExceptionOSHandlerEnter(
        ThreadID threadId,
        CodeID codeId) = 0;

    virtual HRESULT ExceptionOSHandlerLeave(
        ThreadID threadId,
        CodeID codeId) = 0;

    virtual HRESULT ExceptionUnwindFunctionEnter(
        ThreadID threadId,
        CodeID codeId) = 0;

    virtual HRESULT ExceptionUnwindFunctionLeave(
        ThreadID threadId) = 0;

    virtual HRESULT ExceptionUnwindFinallyEnter(
        ThreadID threadId,
        CodeID codeId) = 0;

    virtual HRESULT ExceptionUnwindFinallyLeave(
        ThreadID threadId) = 0;

    virtual HRESULT ExceptionCatcherEnter(
        ThreadID threadId,
        CodeID codeId,
        ObjectID objectId) = 0;

    virtual HRESULT ExceptionCatcherLeave(
        ThreadID threadId) = 0;

    virtual HRESULT COMClassicVTableCreated(
        /* [in] */ TypeID wrappedTypeId,
        /* [in] */ REFGUID implementedIID,
        /* [in] */ void *pVTable,
        /* [in] */ ULONG cSlots,
        /* [in] */ ThreadID threadId) = 0;

    virtual HRESULT COMClassicVTableDestroyed(
        /* [in] */ TypeID wrappedTypeId,
        /* [in] */ REFGUID implementedIID,
        /* [in] */ void *pVTable,
        /* [in] */ ThreadID threadId) = 0;

    virtual HRESULT RuntimeSuspendStarted(
        COR_PRF_SUSPEND_REASON suspendReason,
        ThreadID    threadId) = 0;

    virtual HRESULT RuntimeSuspendFinished(
        ThreadID    threadId) = 0;

    virtual HRESULT RuntimeSuspendAborted(
        ThreadID    threadId) = 0;

    virtual HRESULT RuntimeResumeStarted(
        ThreadID    threadId) = 0;

    virtual HRESULT RuntimeResumeFinished(
        ThreadID    threadId) = 0;

    virtual HRESULT RuntimeThreadSuspended(
        ThreadID    suspendedThreadId,
        ThreadID    threadId) = 0;

    virtual HRESULT RuntimeThreadResumed(
        ThreadID    resumedThreadId,
        ThreadID    threadId) = 0;

    virtual HRESULT ObjectAllocated(
        /* [in] */ ObjectID objectId,
        /* [in] */ TypeID typeId) = 0;

    virtual HRESULT MovedReference(BYTE *pbMemBlockStart,
                                   BYTE *pbMemBlockEnd,
                                   ptrdiff_t cbRelocDistance,
                                   void *pHeapId,
                                   BOOL compacting) = 0;

    virtual HRESULT EndMovedReferences(void *pHeapId) = 0;

    virtual HRESULT RootReference(ObjectID objId,
                                  void *pHeapId) = 0;

    virtual HRESULT EndRootReferences(void *pHeapId) = 0;

    virtual HRESULT ObjectReference(ObjectID objId,
                                    TypeID clsId,
                                    ULONG cNumRefs,
                                    ObjectID *arrObjRef) = 0;

    virtual HRESULT AllocByClass(ObjectID objId, TypeID clsId, void *pHeapId) = 0;

    virtual HRESULT EndAllocByClass(void *pHeapId) = 0;

    virtual HRESULT RemotingClientInvocationStarted(ThreadID threadId) = 0;

    virtual HRESULT RemotingClientSendingMessage(ThreadID threadId, GUID *pCookie,
                                                 BOOL fIsAsync) = 0;

    virtual HRESULT RemotingClientReceivingReply(ThreadID threadId, GUID *pCookie,
                                                 BOOL fIsAsync) = 0;

    virtual HRESULT RemotingClientInvocationFinished(ThreadID threadId) = 0;

    virtual HRESULT RemotingServerReceivingMessage(ThreadID threadId, GUID *pCookie,
                                                   BOOL fIsAsync) = 0;

    virtual HRESULT RemotingServerInvocationStarted(ThreadID threadId) = 0;

    virtual HRESULT RemotingServerInvocationReturned(ThreadID threadId) = 0;

    virtual HRESULT RemotingServerSendingReply(ThreadID threadId, GUID *pCookie,
                                               BOOL fIsAsync) = 0;

    virtual HRESULT InitGUID() = 0;

    virtual void GetGUID(GUID *pGUID) = 0;

    virtual HRESULT FinalizeableObjectQueued(BOOL isCritical, ObjectID objectID) = 0;

    virtual HRESULT RootReference2(BYTE *o, COR_PRF_GC_ROOT_KIND rootKind, COR_PRF_GC_ROOT_FLAGS rootFlags, void *rootID, void *pHeapId) = 0;

    virtual HRESULT EndRootReferences2(void *pHeapId) = 0;

    virtual HRESULT HandleCreated(UINT_PTR handleId, ObjectID initialObjectId) = 0;

    virtual HRESULT HandleDestroyed(UINT_PTR handleId) = 0;

    virtual HRESULT GarbageCollectionStarted(int cGenerations, BOOL generationCollected[], COR_PRF_GC_REASON reason) = 0;

    virtual HRESULT GarbageCollectionFinished() = 0;

};

enum PTR_TYPE
{
    PT_MODULE,
    PT_ASSEMBLY,
};

/*
 * This virtual class provides the Profiling code with access to the EE.
 */
class ProfToEEInterface
{
public:

    virtual ~ProfToEEInterface()
    {
        LEAF_CONTRACT;

    }

    virtual HRESULT Init() = 0;

    virtual void Terminate() = 0;

    virtual bool SetEventMask(DWORD dwEventMask) = 0;

    virtual void DisablePreemptiveGC(ThreadID threadId) = 0;
    virtual void EnablePreemptiveGC(ThreadID threadId) = 0;
    virtual BOOL PreemptiveGCDisabled(ThreadID threadId) = 0;
    virtual DWORD SetProfilerCallbackStateFlags(DWORD dwFlags, ThreadID* pThreadId) = 0;
    virtual void SetProfilerCallbackFullState(DWORD dwFullState, ThreadID threadId) = 0;

    virtual HRESULT GetHandleFromThread(ThreadID threadId,
                                        HANDLE *phThread) = 0;

    virtual HRESULT GetObjectSize(ObjectID objectId,
                                  ULONG *pcSize) = 0;

    virtual HRESULT IsArrayClass(
        /* [in] */  ClassID classId,
        /* [out] */ CorElementType *pBaseElemType,
        /* [out] */ TypeID *pBaseTypeId,
        /* [out] */ ULONG   *pcRank) = 0;

    virtual HRESULT GetThreadInfo(ThreadID threadId,
                                  DWORD *pdwWin32ThreadId) = 0;

	virtual HRESULT GetCurrentThreadID(ThreadID *pThreadId) = 0;

    virtual HRESULT GetFunctionFromIP(LPCBYTE ip, CodeID *pCodeId) = 0;

    virtual HRESULT GetTokenFromFunction(FunctionID functionId,
                                         REFIID riid, IUnknown **ppOut,
                                         mdToken *pToken) = 0;

    virtual HRESULT GetCodeInfo(CodeID codeId, LPCBYTE *pStart,
                                ULONG *pcSize) = 0;

	virtual HRESULT GetModuleInfo(
		ModuleID	moduleId,
		LPCBYTE		*ppBaseLoadAddress,
		ULONG		cchName,
		ULONG		*pcchName,
		WCHAR		szName[],
        AssemblyID  *pAssemblyId) = 0;

	virtual HRESULT GetModuleMetaData(
		ModuleID	moduleId,
		DWORD		dwOpenFlags,
		REFIID		riid,
		IUnknown	**ppOut) = 0;

	virtual HRESULT GetILFunctionBody(
		ModuleID	moduleId,
		mdMethodDef	methodid,
		LPCBYTE		*ppMethodHeader,
		ULONG		*pcbMethodSize) = 0;

	virtual HRESULT GetILFunctionBodyAllocator(
		ModuleID	moduleId,
		IMethodMalloc **ppMalloc) = 0;

	virtual HRESULT SetILFunctionBody(
		ModuleID	moduleId,
		mdMethodDef	methodid,
		LPCBYTE		pbNewILMethodHeader) = 0;

	virtual HRESULT GetClassIDInfo(
		ClassID typeId,
		ModuleID *pModuleId,
		mdTypeDef *pTypeDefToken) = 0;

	virtual HRESULT GetFunctionInfo(
		FunctionID functionId,
		ClassID *pClassId,
		ModuleID *pModuleId,
		mdToken *pToken) = 0;

	virtual HRESULT GetTypeFromObject(
        ObjectID objectId,
        TypeID *pTypeId) = 0;

	virtual HRESULT GetClassFromToken(
		ModuleID moduleId,
		mdTypeDef typeDef,
		ClassID *pClassId) = 0;

	virtual HRESULT GetFunctionFromToken(
		ModuleID moduleId,
		mdToken typeDef,
		FunctionID *pFunctionId) = 0;

    virtual HRESULT GetAppDomainInfo(
        AppDomainID appDomainId,
		ULONG  		cchName,
		ULONG  		*pcchName,
        WCHAR		szName[],
        ProcessID   *pProcessId) = 0;

    virtual HRESULT GetAssemblyInfo(
        AssemblyID  assemblyId,
		ULONG		cchName,
		ULONG		*pcchName,
		WCHAR		szName[],
        AppDomainID *pAppDomainId,
        ModuleID    *pModuleId) = 0;

    virtual HRESULT SetILInstrumentedCodeMap(
        FunctionID functionId,
        BOOL fStartJit,
        ULONG cILMapEntries,
        COR_IL_MAP rgILMapEntries[]) = 0;

    virtual HRESULT ForceGC() = 0;

    virtual HRESULT SetEnterLeaveFunctionHooks(
        FunctionEnter *pFuncEnter,
        FunctionLeave *pFuncLeave,
        FunctionTailcall *pFuncTailcall) = 0;

    virtual HRESULT SetEnterLeaveFunctionHooks2(
        FunctionEnter2 *pFuncEnter,
        FunctionLeave2 *pFuncLeave,
        FunctionTailcall2 *pFuncTailcall) = 0;

    virtual HRESULT SetEnterLeaveFunctionHooksForJit(
        FunctionEnter *pFuncEnter,
        FunctionLeave *pFuncLeave,
        FunctionTailcall *pFuncTailcall) = 0;

    virtual HRESULT SetFunctionIDMapper(
        FunctionIDMapper *pFunc) = 0;

    virtual HRESULT GetThreadContext(
        ThreadID threadId,
        ContextID *pContextId) = 0;

    virtual HRESULT GetILToNativeMapping(
                /* [in] */  CodeID codeId,
                /* [in] */  ULONG32 cMap,
                /* [out] */ ULONG32 *pcMap,
                /* [out, size_is(cMap), length_is(*pcMap)] */
                    COR_DEBUG_IL_TO_NATIVE_MAP map[]) = 0;

     virtual HRESULT GetFunctionInfo2(
                        FunctionID funcId,
                        COR_PRF_FRAME_INFO frameInfo,
                        ClassID *pClassId,
                        ModuleID *pModuleId,
                        mdToken *pToken,
                        ULONG32 cTypeArgs,
                        ULONG32 *pcTypeArgs,
                        ClassID typeArgs[]) = 0;

    virtual HRESULT GetStringLayout(
                        ULONG *pBufferLengthOffset,
                        ULONG *pStringLengthOffset,
                        ULONG *pBufferOffset) = 0;

    virtual HRESULT GetClassLayout(
                        ClassID classID,
                        COR_FIELD_OFFSET rFieldOffset[],
                        ULONG cFieldOffset,
                        ULONG *pcFieldOffset,
                        ULONG *pulClassSize) = 0;

    virtual HRESULT DoStackSnapshot(
                        ThreadID thread,
                        StackSnapshotCallback *callback,
                        ULONG32 infoFlags,
                        void *clientData,
                        LPCONTEXT pctx,
                        ULONG32 contextSize) = 0;

    virtual HRESULT GetCodeInfo2(CodeID codeID,
                                 ULONG32  cCodeInfos,
                                 ULONG32* pcCodeInfos,
                                 COR_PRF_CODE_INFO codeInfos[]) = 0;

    virtual HRESULT EnumModuleFrozenObjects(ModuleID moduleID,
                                            ICorProfilerObjectEnum** ppEnum) = 0;

    virtual HRESULT GetClassFromTokenAndTypeArgs(ModuleID moduleID,
                                                 mdTypeDef typeDef,
                                                 ULONG32 cTypeArgs,
                                                 ClassID typeArgs[],
                                                 ClassID* pClassID) = 0;

    virtual HRESULT GetFunctionFromTokenAndTypeArgs(ModuleID moduleID,
                                                    mdMethodDef funcDef,
                                                    ClassID classId,
                                                    ULONG32 cTypeArgs,
                                                    ClassID typeArgs[],
                                                    FunctionID* pFunctionID) = 0;

    virtual HRESULT GetArrayObjectInfo( ObjectID objectId,
                                     ULONG32 cDimensionSizes,
                                     ULONG32 pDimensionSizes[],
                                     int  pDimensionLowerBounds[],
                                     BYTE ** ppData) = 0;

    virtual HRESULT GetBoxClassLayout(ClassID classId,
                                    ULONG32 *pBufferOffset) = 0;

    virtual HRESULT GetThreadAppDomain(ThreadID threadId,
                                      AppDomainID *pAppDomainId) = 0;

    virtual HRESULT GetRVAStaticAddress(ClassID classId,
                                        mdFieldDef fieldToken,
                                        void **ppAddress) = 0;

    virtual HRESULT GetAppDomainStaticAddress(ClassID classId,
                                              mdFieldDef fieldToken,
                                              AppDomainID appDomainId,
                                              void **ppAddress) = 0;

    virtual HRESULT GetThreadStaticAddress(ClassID classId,
                                           mdFieldDef fieldToken,
                                           ThreadID threadId,
                                           void **ppAddress) = 0;

    virtual HRESULT GetContextStaticAddress(ClassID classId,
                                            mdFieldDef fieldToken,
                                            ContextID contextId,
                                            void **ppAddress) = 0;

    virtual HRESULT GetStaticFieldInfo(ClassID classId,
                                       mdFieldDef fieldToken,
                                       COR_PRF_STATIC_TYPE *pFieldInfo) = 0;

    virtual HRESULT GetClassIDInfo2(ClassID classId,
                                 ModuleID *pModuleId,
                                 mdTypeDef *pTypeDefToken,
                                 ClassID *pParentClassId,
                                 ULONG32 cNumTypeArgs,
                                 ULONG32 *pcNumTypeArgs,
                                 ClassID typeArgs[]) = 0;

    virtual HRESULT GetGenerationBounds(ULONG cObjectRanges,
                                        ULONG *pcObjectRanges,
                                        COR_PRF_GC_GENERATION_RANGE ranges[]) = 0;
 
    virtual HRESULT GetObjectGeneration(ObjectID objectId,
                                        COR_PRF_GC_GENERATION_RANGE *range) = 0;
    
    virtual HRESULT GetNotifiedExceptionClauseInfo(
                                        COR_PRF_EX_CLAUSE_INFO *pinfo) = 0;                 

    virtual BOOL ProfilerCallbacksAllowed(ThreadID threadId) = 0;
    
    virtual void SetProfilerCallbacksAllowed(ThreadID threadId, BOOL fValue) = 0;
};

void __stdcall ProfilerManagedToUnmanagedTransition(Frame *pFrame,
                                                          COR_PRF_TRANSITION_REASON reason);

void __stdcall ProfilerUnmanagedToManagedTransition(Frame *pFrame,
                                                          COR_PRF_TRANSITION_REASON reason);

void __stdcall ProfilerManagedToUnmanagedTransitionMD(MethodDesc * pMD,
                                                          COR_PRF_TRANSITION_REASON reason);

void __stdcall ProfilerUnmanagedToManagedTransitionMD(MethodDesc * pMD,
                                                          COR_PRF_TRANSITION_REASON reason);

#ifndef PREVENT_RUNTIMETHREADSUSPENDED_REDEF
// (See RuntimeTheadSuspendedWrapper in threads.h for more information.)
#define RuntimeThreadSuspended(x, y)  Use_RuntimeThreadSuspendedWrapper_instead
#endif //PREVENT_RUNTIMETHREADSUSPENDED_REDEF


#endif //_EEPROFINTERFACES_H_
