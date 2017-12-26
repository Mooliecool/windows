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

#include "stdafx.h"
#include "profile.h"
#include "corprof.h"
#include "cor.h"

//*****************************************************************************
//*****************************************************************************
CorProfInfo::CorProfInfo() : CorProfBase(),
                             m_dwEventMask(COR_PRF_MONITOR_NONE)
{
    g_pInfo = NULL;
}

//*****************************************************************************
//*****************************************************************************
CorProfInfo::~CorProfInfo()
{
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::QueryInterface(REFIID id, void **pInterface)
{
    if (pInterface == NULL)
        return (E_POINTER);

    if (id == IID_ICorProfilerInfo)
    {
        *pInterface = static_cast<ICorProfilerInfo *>(this);
    }
    else if (id == IID_ICorProfilerInfo2)
    {
        *pInterface = static_cast<ICorProfilerInfo2 *>(this);
    }
    else if (id == IID_IUnknown)
    {
        *pInterface = static_cast<IUnknown *>(static_cast<ICorProfilerInfo *>(this));
    }
    else
    {
        *pInterface = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return (S_OK);
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetClassFromObject(
    /* [in] */ ObjectID objectId,
    /* [out] */ ClassID *pClassId)
{
    if (objectId == NULL)
        return (E_INVALIDARG);

    TypeID id;
    HRESULT hr = (g_pProfToEEInterface->GetTypeFromObject(objectId, &id));
    if (FAILED(hr))
        return hr;
    if (pClassId)
        *pClassId = TypeIDToClassID(id);
    return hr;
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetClassFromToken(
    /* [in] */ ModuleID moduleId,
    /* [in] */ mdTypeDef typeDef,
    /* [out] */ ClassID *pClassId)
{
    if (moduleId == NULL || typeDef == mdTypeDefNil || typeDef == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetClassFromToken(moduleId, typeDef, pClassId));

}


// For generic code,  FunctionIDToGenericCodeID returns a CodeIDs
// corresponding to the generic bucket, i.e. not corresponding to any particular
// instantiation.  Thus it is a "dangerous" function, because we may be losing information.
// It is only used with GetCodeInfo and GetILToNativeMapping, which
// both have special behaviour for generic code (they always gets
// the CodeInfo for the "object" instantiation).
static CodeID FunctionIDToGenericCodeID(FunctionID id)
{
    return (CodeID) id;
}


//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetCodeInfo(
    /* [in] */ FunctionID functionId,
    /* [out] */ LPCBYTE *pStart,
    /* [out] */ ULONG *pcSize)
{
    if (functionId == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetCodeInfo(FunctionIDToGenericCodeID(functionId), pStart, pcSize));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetEventMask(
    /* [out] */ DWORD *pdwEvents)
{
    LEAF_CONTRACT;
        
    if (pdwEvents)
        *pdwEvents = m_dwEventMask;

    return (S_OK);
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetFunctionFromIP(
    /* [in] */ LPCBYTE ip,
    /* [out] */ FunctionID *pFunctionId)
{
    CodeID id;
    HRESULT hr = (g_pProfToEEInterface->GetFunctionFromIP(ip, &id));
    if (FAILED(hr))
        return hr;
    if (pFunctionId)
        *pFunctionId = CodeIDToFunctionID(id);
    return hr;
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetFunctionFromToken(
    /* [in] */ ModuleID moduleId,
    /* [in] */ mdToken token,
    /* [out] */ FunctionID *pFunctionId)
{
    if (moduleId == NULL || token == mdTokenNil)
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetFunctionFromToken(moduleId, token, pFunctionId);

}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetHandleFromThread(
    /* [in] */ ThreadID threadId,
    /* [out] */ HANDLE *phThread)
{
    if (threadId == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetHandleFromThread(threadId, phThread));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetObjectSize(
    /* [in] */ ObjectID objectId,
    /* [out] */ ULONG *pcSize)
{
    if (objectId == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetObjectSize(objectId, pcSize));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::IsArrayClass(
    /* [in] */  ClassID classId,
    /* [out] */ CorElementType *pBaseElemType,
    /* [out] */ ClassID *pBaseClassId,
    /* [out] */ ULONG   *pcRank)
{
    if (classId == NULL)
        return (E_INVALIDARG);


    TypeID id;
    HRESULT hr = g_pProfToEEInterface->IsArrayClass(classId, pBaseElemType, &id, pcRank);
    if (FAILED(hr))
        return hr;
    if (pBaseClassId)
        *pBaseClassId = TypeIDToClassID(id);
    return hr;
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetThreadInfo(
    /* [in] */ ThreadID threadId,
    /* [out] */ DWORD *pdwWin32ThreadId)
{
    if (threadId == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetThreadInfo(threadId, pdwWin32ThreadId));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetCurrentThreadID(
    /* [out] */ ThreadID *pThreadId)
{
    return (g_pProfToEEInterface->GetCurrentThreadID(pThreadId));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetClassIDInfo(
    /* [in] */ ClassID classId,
    /* [out] */ ModuleID *pModuleId,
    /* [out] */ mdTypeDef *pTypeDefToken)
{
    if (classId == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetClassIDInfo(classId, pModuleId, pTypeDefToken));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetFunctionInfo(
    /* [in] */ FunctionID functionId,
    /* [out] */ ClassID  *pClassId,
    /* [out] */ ModuleID  *pModuleId,
    /* [out] */ mdToken  *pToken)
{
    if (functionId == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetFunctionInfo(functionId, pClassId, pModuleId, pToken));
}


//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::SetEventMask(
    /* [in] */ DWORD dwEvents)
{
    // First make sure that the EE can accomodate the changes
    if (g_pProfToEEInterface->SetEventMask(dwEvents))
    {
        m_dwEventMask = dwEvents;
        return (S_OK);
    }

    return (E_FAIL);
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::SetEnterLeaveFunctionHooks(FunctionEnter *pFuncEnter,
                                                   FunctionLeave *pFuncLeave,
                                                   FunctionTailcall *pFuncTailcall)
{
    if (pFuncEnter == NULL || pFuncLeave == NULL || pFuncTailcall == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->SetEnterLeaveFunctionHooks(pFuncEnter, pFuncLeave, pFuncTailcall));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::SetEnterLeaveFunctionHooks2(FunctionEnter2 *pFuncEnter,
                                                 FunctionLeave2 *pFuncLeave,
                                                 FunctionTailcall2 *pFuncTailcall)
{
    if ((pFuncEnter == NULL) ||
        (pFuncLeave == NULL) ||
        (pFuncTailcall == NULL))
    {
        return (E_INVALIDARG);
    }

    return (g_pProfToEEInterface->SetEnterLeaveFunctionHooks2(pFuncEnter, pFuncLeave, pFuncTailcall));
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::SetFunctionIDMapper(FunctionIDMapper *pFunc)
{
    return (g_pProfToEEInterface->SetFunctionIDMapper(pFunc));
}

//*****************************************************************************
// Need to return a metadata import scope for this method.  This amounts to
// finding the method desc behind this item, getting it's token, and then
// getting a metadata dispenser for it.
//*****************************************************************************
COM_METHOD CorProfInfo::GetTokenAndMetaDataFromFunction(
    FunctionID  functionId,
    REFIID      riid,
    IUnknown    **ppOut,
    mdToken     *pToken)
{
    if (functionId == NULL)
        return (E_INVALIDARG);

    return (g_pProfToEEInterface->GetTokenFromFunction(functionId, riid, ppOut, pToken));
}


//*****************************************************************************
// Retrieve information about a given module.
//*****************************************************************************
COM_METHOD CorProfInfo::GetModuleInfo(
    ModuleID    moduleId,
    LPCBYTE     *ppBaseLoadAddress,
    ULONG       cchName,
    ULONG       *pcchName,
    WCHAR       szName[],
    AssemblyID  *pAssemblyId)
{
    if (moduleId == NULL)
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetModuleInfo(moduleId, ppBaseLoadAddress,
            cchName, pcchName, szName, pAssemblyId);
}


//*****************************************************************************
// Get a metadata interface insance which maps to the given module.
// One may ask for the metadata to be opened in read+write mode, but
// this will result in slower metadata execution of the program, because
// changes made to the metadata cannot be optimized as they were from
// the compiler.
//*****************************************************************************
COM_METHOD CorProfInfo::GetModuleMetaData(
    ModuleID    moduleId,
    DWORD       dwOpenFlags,
    REFIID      riid,
    IUnknown    **ppOut)
{
    if (moduleId == NULL)
        return (E_INVALIDARG);

    if (!(dwOpenFlags == ofRead || dwOpenFlags == ofWrite || dwOpenFlags == (ofRead | ofWrite)))
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetModuleMetaData(moduleId, dwOpenFlags,
            riid, ppOut);
}


//*****************************************************************************
// Retrieve a pointer to the body of a method starting at it's header.
// A method is coped by the module it lives in.  Because this function
// is designed to give a tool access to IL before it has been loaded
// by the Runtime, it uses the metadata token of the method to find
// the instance desired.  Note that this function has no effect on
// already compiled code.
//*****************************************************************************
COM_METHOD CorProfInfo::GetILFunctionBody(
    ModuleID    moduleId,
    mdMethodDef methodId,
    LPCBYTE     *ppMethodHeader,
    ULONG       *pcbMethodSize)
{
    if (moduleId == NULL ||
        methodId == mdMethodDefNil ||
        methodId == 0 ||
        TypeFromToken(methodId) != mdtMethodDef)
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetILFunctionBody(moduleId, methodId,
                ppMethodHeader, pcbMethodSize);
}


//*****************************************************************************
// IL method bodies must be located as RVA's to the loaded module, which
// means they come after the module within 4 gb.  In order to make it
// easier for a tool to swap out the body of a method, this allocator
// will ensure memory allocated after that point.
//*****************************************************************************
COM_METHOD CorProfInfo::GetILFunctionBodyAllocator(
    ModuleID    moduleId,
    IMethodMalloc **ppMalloc)
{
    if (moduleId == NULL)
        return (E_INVALIDARG);

    if (ppMalloc)
        return g_pProfToEEInterface->GetILFunctionBodyAllocator(moduleId, ppMalloc);
    else
        return (S_OK);
}


//*****************************************************************************
// Replaces the method body for a function in a module.  This will replace
// the RVA of the method in the metadata to point to this new method body,
// and adjust any internal data structures as required.  This function can
// only be called on those methods which have never been compiled by a JITTER.
// Please use the GetILFunctionBodyAllocator to allocate space for the new method to
// ensure the buffer is compatible.
//*****************************************************************************
COM_METHOD CorProfInfo::SetILFunctionBody(
    ModuleID    moduleId,
    mdMethodDef methodId,
    LPCBYTE     pbNewILMethodHeader)
{
    if (moduleId == NULL ||
        methodId == mdMethodDefNil ||
        TypeFromToken(methodId) != mdtMethodDef ||
        pbNewILMethodHeader == NULL)
    {
        return (E_INVALIDARG);
    }

    return g_pProfToEEInterface->SetILFunctionBody(moduleId, methodId,
                pbNewILMethodHeader);
}


//*****************************************************************************
// Retrieve app domain information given its id.
//*****************************************************************************
COM_METHOD CorProfInfo::GetAppDomainInfo(
    AppDomainID appDomainId,
    ULONG       cchName,
    ULONG       *pcchName,
    WCHAR       szName[  ],
    ProcessID   *pProcessId)
{
    if (appDomainId == NULL)
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetAppDomainInfo(appDomainId, cchName, pcchName, szName, pProcessId);
}


//*****************************************************************************
// Retrieve information about an assembly given its ID.
//*****************************************************************************
COM_METHOD CorProfInfo::GetAssemblyInfo(
    AssemblyID  assemblyId,
    ULONG       cchName,
    ULONG       *pcchName,
    WCHAR       szName[  ],
    AppDomainID *pAppDomainId,
    ModuleID    *pModuleId)
{
    if (assemblyId == NULL)
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetAssemblyInfo(assemblyId, cchName, pcchName, szName,
                             pAppDomainId, pModuleId);
}

//
// DEPRECATED IN CLR VERSION 2.0
// 
//*****************************************************************************
// Marks a function as requiring a re-JIT.  The function will be re-JITted
// at its next invocation.  The normal profiller events will give the profiller
// an opportunity to replace the IL prior to the JIT.  By this means, a tool
// can effectively replace a function at runtime.  Note that active instances
// of the function are not affected by the replacement.
//*****************************************************************************
COM_METHOD CorProfInfo::SetFunctionReJIT(
    FunctionID functionId)
{
    return E_NOTIMPL;
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::SetILInstrumentedCodeMap(
        FunctionID functionId,
        BOOL fStartJit,
        ULONG cILMapEntries,
        COR_IL_MAP rgILMapEntries[])
{
    if (functionId == NULL)
        return (E_INVALIDARG);

    return g_pProfToEEInterface->SetILInstrumentedCodeMap(functionId,
                                                          fStartJit,
                                                          cILMapEntries,
                                                          rgILMapEntries);
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::ForceGC()
{
    return g_pProfToEEInterface->ForceGC();
}

/*
 * GetInprocInspectionInterface is used to get an interface to the
 * in-process portion of the debug interface, which is useful for things
 * like doing a stack trace.
 *
 * This function was deprecated in Whidbey.
 *
 * ppicd: *ppicd will be filled in with a pointer to the interface, or
 *          NULL if the interface is unavailable.
 */
COM_METHOD CorProfInfo::GetInprocInspectionInterface(
        IUnknown **ppicd)
{
    return E_NOTIMPL;
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetInprocInspectionIThisThread(
        IUnknown **ppicd)
{
    return E_NOTIMPL;
}

//*****************************************************************************
//*****************************************************************************
COM_METHOD CorProfInfo::GetThreadContext(
    ThreadID threadId,
    ContextID *pContextId)
{
    if (threadId == NULL)
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetThreadContext(threadId, pContextId);
}

//*****************************************************************************
// The profiler MUST call this function before using the in-process debugging
// APIs.  fThisThreadOnly indicates whether in-proc debugging will be used to
// trace the stack of the current managed thread only, or whether it might be
// used to trace the stack of any managed thread.
//*****************************************************************************
COM_METHOD CorProfInfo::BeginInprocDebugging(BOOL fThisThreadOnly, DWORD *pdwProfilerContext)
{
    return (E_NOTIMPL);
}

//*****************************************************************************
// The profiler MUST call this function when it is done using the in-process
// debugging APIs.  Failing to do so will result in undefined behaviour of
// the runtime.
//*****************************************************************************
COM_METHOD CorProfInfo::EndInprocDebugging(DWORD dwProfilerContext)
{
    return (E_NOTIMPL);
}
COM_METHOD CorProfInfo::GetILToNativeMapping(
            /* [in] */  FunctionID functionId,
            /* [in] */  ULONG32 cMap,
            /* [out] */ ULONG32 *pcMap,
            /* [out, size_is(cMap), length_is(*pcMap)] */
                COR_DEBUG_IL_TO_NATIVE_MAP map[])
{
    if (functionId == NULL)
        return (E_INVALIDARG);

    if (cMap > 0 && (!pcMap || !map))
        return (E_INVALIDARG);

    return g_pProfToEEInterface->GetILToNativeMapping(FunctionIDToGenericCodeID(functionId), cMap, pcMap, map);
}


//*****************************************************************************
// Get detailed information about the function
//*****************************************************************************
COM_METHOD CorProfInfo::GetFunctionInfo2(FunctionID funcId,
                                      COR_PRF_FRAME_INFO frameInfo,
                                      ClassID *pClassId,
                                      ModuleID *pModuleId,
                                      mdToken *pToken,
                                      ULONG32 cTypeArgs,
                                      ULONG32 *pcTypeArgs,
                                      ClassID typeArgs[])
{
    return g_pProfToEEInterface->GetFunctionInfo2(funcId,
                                                  frameInfo,
                                                  pClassId,
                                                  pModuleId,
                                                  pToken,
                                                  cTypeArgs,
                                                  pcTypeArgs,
                                                  typeArgs);
}

//*****************************************************************************
// Get detailed information about string objects are stored internally.
//*****************************************************************************

COM_METHOD CorProfInfo::GetStringLayout(ULONG *pBufferLengthOffset,
                                      ULONG *pStringLengthOffset,
                                      ULONG *pBufferOffset)
{
    return g_pProfToEEInterface->GetStringLayout(pBufferLengthOffset,
                                                 pStringLengthOffset,
                                                 pBufferOffset);
}

//*****************************************************************************
// GetClassLayout returns detailed information how a specific class is stored.
//*****************************************************************************

COM_METHOD CorProfInfo::GetClassLayout(ClassID classID,
                                      COR_FIELD_OFFSET rFieldOffset[],
                                      ULONG cFieldOffset,
                                      ULONG *pcFieldOffset,
                                      ULONG *pulClassSize)
{
    return g_pProfToEEInterface->GetClassLayout(classID,
                                                rFieldOffset,
                                                cFieldOffset,
                                                pcFieldOffset,
                                                pulClassSize);
}



//*****************************************************************************
// Pass on the stack walk request.
//*****************************************************************************
COM_METHOD CorProfInfo::DoStackSnapshot(ThreadID thread,
                                       StackSnapshotCallback *callback,
                                       ULONG32 infoFlags,
                                       void *clientData,
                                       BYTE context[],
                                       ULONG32 contextSize)

{
    return g_pProfToEEInterface->DoStackSnapshot(thread, callback, infoFlags, clientData, (CONTEXT *) context, contextSize);
}


COM_METHOD CorProfInfo::GetCodeInfo2(FunctionID functionID,
                                            ULONG32  cCodeInfos,
                                            ULONG32* pcCodeInfos,
                                            COR_PRF_CODE_INFO codeInfos[])
{
    return g_pProfToEEInterface->GetCodeInfo2(FunctionIDToGenericCodeID(functionID), cCodeInfos, pcCodeInfos, codeInfos);
}

COM_METHOD CorProfInfo::GetClassFromTokenAndTypeArgs(ModuleID moduleID,
                                                     mdTypeDef typeDef,
                                                     ULONG32 cTypeArgs,
                                                     ClassID typeArgs[],
                                                     ClassID* pClassID)
{
    return g_pProfToEEInterface->GetClassFromTokenAndTypeArgs(moduleID,
                                                              typeDef,
                                                              cTypeArgs,
                                                              typeArgs,
                                                              pClassID);
}

COM_METHOD CorProfInfo::GetFunctionFromTokenAndTypeArgs(ModuleID moduleID,
                                                        mdMethodDef funcDef,
                                                        ClassID classID,
                                                        ULONG32 cTypeArgs,
                                                        ClassID typeArgs[],
                                                        FunctionID* pFunctionID)
{
    return g_pProfToEEInterface->GetFunctionFromTokenAndTypeArgs(moduleID,
                                                                 funcDef,
                                                                 classID,
                                                                 cTypeArgs,
                                                                 typeArgs,
                                                                 pFunctionID);
}


HRESULT CorProfInfo::EnumModuleFrozenObjects(ModuleID moduleID,
                                             ICorProfilerObjectEnum** ppEnum)
{
    return g_pProfToEEInterface->EnumModuleFrozenObjects(moduleID,
                                                         ppEnum);
}


HRESULT CorProfInfo::GetArrayObjectInfo( ObjectID objectId,
                    ULONG32 cDimensionSizes,
                    ULONG32 pDimensionSizes[],
                    int     pDimensionLowerBounds[],
                    BYTE ** ppData)
{
    return g_pProfToEEInterface->GetArrayObjectInfo(
        objectId,
        cDimensionSizes, pDimensionSizes, pDimensionLowerBounds,
        ppData);
}

//*****************************************************************************
// GetBoxClassLayout returns information about how a particular value type is laid out.
//*****************************************************************************
HRESULT CorProfInfo::GetBoxClassLayout(ClassID classId,
                        ULONG32 *pBufferOffset)
{
    return g_pProfToEEInterface->GetBoxClassLayout(classId, pBufferOffset);
}

//*****************************************************************************
// GetThreadAppDomain returns the AppDomainID currently associated withthe given ThreadID
//*****************************************************************************
HRESULT CorProfInfo::GetThreadAppDomain(ThreadID threadId,
                          AppDomainID *pAppDomainId)
{
    return g_pProfToEEInterface->GetThreadAppDomain(threadId, pAppDomainId);
}

/*
 * GetRVAStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class.  The field must be an RVA Static token.
 *
 * As with almost all these profiler functions, this simply passes thru to the 
 * EE function that handles this request.
 *
 */
HRESULT CorProfInfo::GetRVAStaticAddress(ClassID classId,
                                         mdFieldDef fieldToken,
                                         void **ppAddress)
{
    return g_pProfToEEInterface->GetRVAStaticAddress(classId, fieldToken, ppAddress);
}


/*
 * GetAppDomainStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class in the given app domain.  The field must be an App Domain Static token.
 *
 * As with almost all these profiler functions, this simply passes thru to the 
 * EE function that handles this request.
 *
 */
HRESULT CorProfInfo::GetAppDomainStaticAddress(ClassID classId,
                                               mdFieldDef fieldToken,
                                               AppDomainID appDomainId,
                                               void **ppAddress)
{
    return g_pProfToEEInterface->GetAppDomainStaticAddress(classId, fieldToken, appDomainId, ppAddress);
}

/*
 * GetThreadStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class on the given thread.  The field must be an Thread Static token.
 *
 * As with almost all these profiler functions, this simply passes thru to the 
 * EE function that handles this request.
 *
 */
HRESULT CorProfInfo::GetThreadStaticAddress(ClassID classId,
                                            mdFieldDef fieldToken,
                                            ThreadID threadId,
                                            void **ppAddress)
{
    return g_pProfToEEInterface->GetThreadStaticAddress(classId, fieldToken, threadId, ppAddress);
}

/*
 * GetContextStaticAddress
 *
 * This function returns the absolute address of the given field in the given
 * class in the given context.  The field must be an Context Static token.
 *
 * As with almost all these profiler functions, this simply passes thru to the 
 * EE function that handles this request.
 *
 */
HRESULT CorProfInfo::GetContextStaticAddress(ClassID classId,
                                             mdFieldDef fieldToken,
                                             ContextID contextId,
                                             void **ppAddress)
{
    return g_pProfToEEInterface->GetContextStaticAddress(classId, fieldToken, contextId, ppAddress);
}

/*
 * GetStaticFieldInfo
 *
 * This function returns the static information for a specific field in a class.
 *
 * As with almost all these profiler functions, this simply passes thru to the 
 * EE function that handles this request.
 *
 */
HRESULT CorProfInfo::GetStaticFieldInfo(ClassID classId,
                                        mdFieldDef fieldToken,
                                        COR_PRF_STATIC_TYPE *pFieldInfo)
{
    return g_pProfToEEInterface->GetStaticFieldInfo(classId, fieldToken, pFieldInfo);
}


//*****************************************************************************
// GetClassIDInfo2 generalizes GetClassIDInfo for all types, generics and non-generics.
//*****************************************************************************
HRESULT CorProfInfo::GetClassIDInfo2(ClassID classId,
                                  ModuleID *pModuleId,
                                  mdTypeDef *pTypeDefToken,
                                  ClassID *pParentClassId,
                                  ULONG32 cNumTypeArgs,
                                  ULONG32 *pcNumTypeArgs,
                                  ClassID typeArgs[])
{
    return g_pProfToEEInterface->GetClassIDInfo2(classId,
                                                 pModuleId,
                                                 pTypeDefToken,
                                                 pParentClassId,
                                                 cNumTypeArgs,
                                                 pcNumTypeArgs,
                                                 typeArgs);
}

HRESULT CorProfInfo::GetGenerationBounds(ULONG cObjectRanges, ULONG *pcObjectRanges, COR_PRF_GC_GENERATION_RANGE ranges[])
{
    return g_pProfToEEInterface->GetGenerationBounds(cObjectRanges, pcObjectRanges, ranges);
}

HRESULT CorProfInfo::GetObjectGeneration(ObjectID objectId,
                               COR_PRF_GC_GENERATION_RANGE *range)
{
    return g_pProfToEEInterface->GetObjectGeneration(objectId, range);
}

HRESULT CorProfInfo::GetNotifiedExceptionClauseInfo(
                                COR_PRF_EX_CLAUSE_INFO *pinfo)
{
    return g_pProfToEEInterface->GetNotifiedExceptionClauseInfo(pinfo);
}

