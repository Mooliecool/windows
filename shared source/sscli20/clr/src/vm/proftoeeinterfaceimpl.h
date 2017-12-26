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

#ifndef __PROFTOEEINTERFACEIMPL_H__
#define __PROFTOEEINTERFACEIMPL_H__

#include "eeprofinterfaces.h"
#include "vars.hpp"
#include "threads.h"
#include "codeman.h"
#include "cor.h"


//
// Helper routines.
//
extern MethodDesc *FunctionIdToMethodDesc(FunctionID functionID);
extern TypeID TypeHandleToTypeID(TypeHandle th);


//
// Function declarations for those functions that are platform specific.
//
extern UINT_PTR ProfileGetIPFromPlatformSpecificHandle(void *handle);

//
// The following class is implemented differently on each platform, using
// the PlatformSpecificHandle to initialize an ArgIterator.
//
class ProfileArgIterator
{
private:
    ArgIterator  m_argIterator;
    void        *m_handle;
    ULONG32      m_count;


public:
    ProfileArgIterator(MetaSig *pMetaSig, void* platformSpecificHandle);

    ~ProfileArgIterator();

    void Init(MetaSig *pMetaSig, void *platformSpecificHandle);

    //
    // After initialization, this method is called repeatedly until it
    // returns NULL to get the address of each arg. Additionally (currently
    // on IA64 only) it can return -1 to indicate that a buffer needs to be
    // allocated of size *pStructSize to hold an aggregate that crosses
    // the register-stack boundary. FillAggregate() needs to be called to
    // obtain a copy of that parameter.
    // 
    // Note: this address could be anywhere on the stack.
    //
    // pType is the IMAGE_CEE_CS_* type.
    // pStructSize is the struct size, if appropriate.
    //
    LPVOID GetNextArgAddr(BYTE *pType, UINT32 *pStructSize);



    //
    // Called after initialization, any number of times, to retrieve any
    // hidden argument, so that resolution for Generics can be done.
    //
    LPVOID GetHiddenArgValue(void);

    //
    // Called after initialization, any number of times, to retrieve the
    // value of 'this'.
    //
    LPVOID GetThis(void);

    //
    // Called after initialization, any number of times, to retrieve the
    // address of the return buffer, if there is one.  NULL indicates no
    // return buffer.
    //
    LPVOID GetReturnBufferAddr(void);
};

//*****************************************************************************
// One of these is allocated per EE instance.   A pointer is cached to this
// from the profiler implementation.  The profiler will call back on the v-table
// to get at EE internals as required.
//*****************************************************************************
class ProfToEEInterfaceImpl : public ProfToEEInterface
{
public:
    ProfToEEInterfaceImpl();

    ClassID GetClassIdForNonGenericType(TypeID typeID);

    FunctionID CodeIdToFunctionId(CodeID codeId);

    // Static version for JIT/EE interface
    static FunctionID _CodeIdToFunctionId(CodeID codeId);

    HRESULT Init();

    void Terminate();

    bool SetEventMask(DWORD dwEventMask);

    void DisablePreemptiveGC(ThreadID threadId);

    void EnablePreemptiveGC(ThreadID threadId);

    BOOL PreemptiveGCDisabled(ThreadID threadId);

    DWORD SetProfilerCallbackStateFlags(DWORD dwFlags, ThreadID* pThreadId);

    void SetProfilerCallbackFullState(DWORD dwFullState, ThreadID threadId);

    HRESULT GetHandleFromThread(ThreadID threadId, HANDLE *phThread);

    HRESULT GetObjectSize(ObjectID objectId, ULONG *pcSize);

    HRESULT IsArrayClass(
        /* [in] */  ClassID classId,
        /* [out] */ CorElementType *pBaseElemType,
        /* [out] */ TypeID *pBaseTypeId,
        /* [out] */ ULONG   *pcRank);

    HRESULT GetThreadInfo(ThreadID threadId, DWORD *pdwWin32ThreadId);

    HRESULT GetCurrentThreadID(ThreadID *pThreadId);

    HRESULT GetFunctionFromIP(LPCBYTE ip, CodeID *pCodeId);

    HRESULT GetTokenFromFunction(FunctionID functionId, REFIID riid, IUnknown **ppOut,
                                 mdToken *pToken);

    HRESULT GetCodeInfo(CodeID codeId, LPCBYTE *pStart, ULONG *pcSize);

    HRESULT GetModuleInfo(
        ModuleID    moduleId,
        LPCBYTE     *ppBaseLoadAddress,
        ULONG       cchName,
        ULONG       *pcchName,
        WCHAR       szName[],
        AssemblyID  *pAssemblyId);

    HRESULT GetModuleMetaData(
        ModuleID    moduleId,
        DWORD       dwOpenFlags,
        REFIID      riid,
        IUnknown    **ppOut);

    HRESULT GetILFunctionBody(
        ModuleID    moduleId,
        mdMethodDef methodid,
        LPCBYTE     *ppMethodHeader,
        ULONG       *pcbMethodSize);

    HRESULT GetILFunctionBodyAllocator(
        ModuleID    moduleId,
        IMethodMalloc **ppMalloc);

    HRESULT SetILFunctionBody(
        ModuleID    moduleId,
        mdMethodDef methodid,
        LPCBYTE     pbNewILMethodHeader);

    HRESULT SetILInstrumentedCodeMap(
        FunctionID functionId,
        BOOL fStartJit,
        ULONG cILMapEntries,
        COR_IL_MAP rgILMapEntries[]);

    HRESULT ForceGC();

    HRESULT GetClassIDInfo(
        ClassID classId,
        ModuleID *pModuleId,
        mdTypeDef *pTypeDefToken);

    HRESULT GetFunctionInfo(
        FunctionID functionId,
        ClassID *pClassId,
        ModuleID *pModuleId,
        mdToken *pToken);

    HRESULT GetTypeFromObject(
        ObjectID objectId,
        TypeID *pTypeId);

    HRESULT GetClassFromToken(
        ModuleID moduleId,
        mdTypeDef typeDef,
        ClassID *pClassId);

    HRESULT GetFunctionFromToken(
        ModuleID moduleId,
        mdToken typeDef,
        FunctionID *pFunctionId);

    HRESULT GetAppDomainInfo(
        AppDomainID appDomainId,
        ULONG       cchName,
        ULONG       *pcchName,
        WCHAR       szName[],
        ProcessID   *pProcessId);

    HRESULT GetAssemblyInfo(
        AssemblyID  assemblyId,
        ULONG       cchName,
        ULONG       *pcchName,
        WCHAR       szName[],
        AppDomainID *pAppDomainId,
        ModuleID    *pModuleId);

    HRESULT SetEnterLeaveFunctionHooks(
        FunctionEnter *pFuncEnter,
        FunctionLeave *pFuncLeave,
        FunctionTailcall *pFuncTailcall);

    HRESULT SetEnterLeaveFunctionHooks2(
        FunctionEnter2 *pFuncEnter,
        FunctionLeave2 *pFuncLeave,
        FunctionTailcall2 *pFuncTailcall);

    HRESULT SetEnterLeaveFunctionHooksForJit(
        FunctionEnter *pFuncEnter,
        FunctionLeave *pFuncLeave,
        FunctionTailcall *pFuncTailcall);

    HRESULT SetFunctionIDMapper(
        FunctionIDMapper *pFunc);

    HRESULT GetThreadContext(
        ThreadID threadId,
        ContextID *pContextId);

    HRESULT GetILToNativeMapping(
                /* [in] */  CodeID codeId,
                /* [in] */  ULONG32 cMap,
                /* [out] */ ULONG32 *pcMap,
                /* [out, size_is(cMap), length_is(*pcMap)] */
                    COR_DEBUG_IL_TO_NATIVE_MAP map[]);

    HRESULT GetFunctionInfo2(
        /* in  */ FunctionID funcId,
        /* in  */ COR_PRF_FRAME_INFO frameInfo,
        /* out */ ClassID *pClassId,
        /* out */ ModuleID *pModuleId,
        /* out */ mdToken *pToken,
        /* in  */ ULONG32 cTypeArgs,
        /* out */ ULONG32 *pcTypeArgs,
        /* out */ ClassID typeArgs[]);

    HRESULT GetStringLayout(
         /* out */ ULONG *pBufferLengthOffset,
         /* out */ ULONG *pStringLengthOffset,
         /* out */ ULONG *pBufferOffset);

    HRESULT GetClassLayout(
         /* in    */ ClassID classID,
         /* in.out*/ COR_FIELD_OFFSET rFieldOffset[],
         /* in    */ ULONG cFieldOffset,
         /* out   */ ULONG *pcFieldOffset,
         /* out   */ ULONG *pulClassSize);

    HRESULT DoStackSnapshot(
        ThreadID thread,
        StackSnapshotCallback *callback,
        ULONG32 infoFlags,
        void *clientData,
        LPCONTEXT pctx,
        ULONG32 contextSize);

    HRESULT GetCodeInfo2(CodeID codeID,
                         ULONG32  cCodeInfos,
                         ULONG32* pcCodeInfos,
                         COR_PRF_CODE_INFO codeInfos[]);

    HRESULT GetCodeInfo2Helper(CodeID codeID,
                               ULONG32  cCodeInfos,
                               ULONG32* pcCodeInfos,
                               COR_PRF_CODE_INFO codeInfos[]);

    HRESULT GetArrayObjectInfo( ObjectID objectId,
                    ULONG32 cDimensionSizes,
                    ULONG32 pDimensionSizes[],
                    int pDimensionLowerBounds[],
                    BYTE **ppData);

    HRESULT GetBoxClassLayout(ClassID classId,
                            ULONG32 *pBufferOffset);

    HRESULT GetClassIDInfo2(ClassID classId,
                         ModuleID *pModuleId,
                         mdTypeDef *pTypeDefToken,
                         ClassID *pParentClassId,
                         ULONG32 cNumTypeArgs,
                         ULONG32 *pcNumTypeArgs,
                         ClassID typeArgs[]);

    HRESULT GetThreadAppDomain(ThreadID threadId,
                               AppDomainID *pAppDomainId);

    HRESULT GetRVAStaticAddress(ClassID classId,
                                mdFieldDef fieldToken,
                                void **ppAddress);

    HRESULT GetAppDomainStaticAddress(ClassID classId,
                                      mdFieldDef fieldToken,
                                      AppDomainID appDomainId,
                                      void **ppAddress);

    HRESULT GetThreadStaticAddress(ClassID classId,
                                   mdFieldDef fieldToken,
                                   ThreadID threadId,
                                   void **ppAddress);

    HRESULT GetContextStaticAddress(ClassID classId,
                                    mdFieldDef fieldToken,
                                    ContextID contextId,
                                    void **ppAddress);

    HRESULT GetStaticFieldInfo(ClassID classId,
                               mdFieldDef fieldToken,
                               COR_PRF_STATIC_TYPE *pFieldInfo);
    
    HRESULT GetClassFromTokenAndTypeArgs(ModuleID moduleID,
                                         mdTypeDef typeDef,
                                         ULONG32 cTypeArgs,
                                         ClassID typeArgs[],
                                         ClassID* pClassID);

    HRESULT EnumModuleFrozenObjects(ModuleID moduleID,
                                    ICorProfilerObjectEnum** ppEnum);



    HRESULT GetFunctionFromTokenAndTypeArgs(ModuleID moduleID,
                                            mdMethodDef funcDef,
                                            ClassID classId,
                                            ULONG32 cTypeArgs,
                                            ClassID typeArgs[],
                                            FunctionID* pFunctionID);

    HRESULT GetGenerationBounds(ULONG cObjectRanges,
                                ULONG *pcObjectRanges,
                                COR_PRF_GC_GENERATION_RANGE ranges[]);
 
    HRESULT GetObjectGeneration(ObjectID objectId,
                                COR_PRF_GC_GENERATION_RANGE *range);

    HRESULT GetNotifiedExceptionClauseInfo(
                                COR_PRF_EX_CLAUSE_INFO *pinfo);

    BOOL ProfilerCallbacksAllowed(ThreadID threadId);
    void SetProfilerCallbacksAllowed(ThreadID threadId, BOOL fValue);

    static void MethodTableCallback(void* context, void* methodTable);
    static void ObjectRefCallback(void* context, void* objectRefUNSAFE);



private:
    struct HeapList
    {
        ExplicitControlLoaderHeap *m_pHeap;
        struct HeapList *m_pNext;

        HeapList(ExplicitControlLoaderHeap *pHeap) : m_pHeap(pHeap), m_pNext(NULL)
        {
        }

        ~HeapList()
        {
            delete m_pHeap;
        }
    };

    HeapList *m_pHeapList;      // Heaps for allocator.

public:
    // Helpers.
    HRESULT NewHeap(ExplicitControlLoaderHeap **ppHeap, LPCBYTE pBase, DWORD dwMemSize = 1024*1024);
    HRESULT GrowHeap(ExplicitControlLoaderHeap *pHeap, DWORD dwMemSize = 1024*1024);
    HRESULT AddHeap(ExplicitControlLoaderHeap *pHeap);
    void *Alloc(LPCBYTE pBase, ULONG cb, Module *pModule);

    static MethodDesc *LookupMethodDescFromMethodDef(mdMethodDef methodId, Module *pModule)
    {
        _ASSERTE(TypeFromToken(methodId) == mdtMethodDef);

        return (pModule->LookupMethodDef(methodId));
    }

    static MethodDesc *LookupMethodDescFromMemberRef(mdMemberRef memberId, Module *pModule)
    {
        _ASSERTE(TypeFromToken(memberId) == mdtMemberRef);

        return (pModule->LookupMemberRefAsMethod(memberId));
    }

    MethodDesc *LookupMethodDesc(mdMemberRef memberId, Module *pModule)
    {
        MethodDesc *pDesc = NULL;

        // Different lookup depending on whether it's a Def or Ref
        if (TypeFromToken(memberId) == mdtMethodDef)
            pDesc = pModule->LookupMethodDef(memberId);

        else if (TypeFromToken(memberId) == mdtMemberRef)
            pDesc = pModule->LookupMemberRefAsMethod(memberId);

        return (pDesc);
    }
};





//*****************************************************************************
// This helper class wraps a loader heap which can be used to allocate
// memory for IL after the current module.
//*****************************************************************************
class ModuleILHeap : public IMethodMalloc
{
public:
    ModuleILHeap(LPCBYTE pBase, ProfToEEInterfaceImpl *pParent, Module *pModule);

    static HRESULT CreateNew(
        REFIID riid, void **pp, LPCBYTE pBase, ProfToEEInterfaceImpl *pParent, Module *pModule);

// IUnknown
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **pp);

// IMethodMalloc
    virtual void *STDMETHODCALLTYPE Alloc(
        /* [in] */ ULONG cb);

private:
    LONG        m_cRef;                 // Ref count for object.
    LPCBYTE     m_pBase;                // Base address for this module.
    ProfToEEInterfaceImpl *m_pParent;   // Parent class.
    Module     *m_pModule;              // Module associated with allocator
};
//**********************************************************************************
// This provides the implementations for FCALLs in managed code related to profiling
//**********************************************************************************
class ProfilingFCallHelper
{
public:
    // This is a high-efficiency way for managed profiler code to determine if
    // profiling of remoting is active.
    static FCDECL0(FC_BOOL_RET, FC_TrackRemoting);

    // This is a high-efficiency way for managed profiler code to determine if
    // profiling of remoting with RPC cookie IDs is active.
    static FCDECL0(FC_BOOL_RET, FC_TrackRemotingCookie);

    // This is a high-efficiency way for managed profiler code to determine if
    // profiling of asynchronous remote calls is profiled
    static FCDECL0(FC_BOOL_RET, FC_TrackRemotingAsync);

    // This will let the profiler know that the client side is sending a message to
    // the server-side.
    static FCDECL2(void, FC_RemotingClientSendingMessage, GUID *pId, CLR_BOOL fIsAsync);

    // For __cdecl calling convention both arguments end up on
    // the stack but the order in which the jit puts them there needs to be reversed
    // For __fastcall calling convention the reversal has no effect because the GUID doesn't
    // fit in a register. On IA64 the macro is different.

    // This will let the profiler know that the client side is receiving a reply
    // to a message that it sent
    static FCDECL2_VI(void, FC_RemotingClientReceivingReply, GUID id, CLR_BOOL fIsAsync);

    // This will let the profiler know that the server side is receiving a message
    // from a client
    static FCDECL2_VI(void, FC_RemotingServerReceivingMessage, GUID id, CLR_BOOL fIsAsync);

    // This will let the profiler know that the server side is sending a reply to
    // a received message.
    static FCDECL2(void, FC_RemotingServerSendingReply, GUID *pId, CLR_BOOL fIsAsync);
};

#endif // __PROFTOEEINTERFACEIMPL_H__
