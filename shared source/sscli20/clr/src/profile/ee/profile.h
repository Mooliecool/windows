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

#ifndef __PROFILE_H__
#define __PROFILE_H__

#include <windows.h>

#include "corprof.h"
#include "eeprofinterfaces.h"

#define COM_METHOD HRESULT STDMETHODCALLTYPE

class CorProfInfo;

extern ProfToEEInterface     *g_pProfToEEInterface;
extern ICorProfilerCallback2 *g_pCallback;
extern CorProfInfo           *g_pInfo;
extern ICorProfilerCallback  *g_pEverettCallback_DELETE_ME;
extern BOOL                   g_fSupportWhidbeyOnly_DELETE_ME;

ClassID TypeIDToClassID(TypeID typeId);
FunctionID CodeIDToFunctionID(CodeID codeId);


class CorProfBase : public IUnknown
{
public:
    CorProfBase() : m_refCount(0)
    {
    }

    virtual ~CorProfBase() {}

    ULONG STDMETHODCALLTYPE BaseAddRef()
    {
        return (InterlockedIncrement((LONG *) &m_refCount));
    }

    ULONG STDMETHODCALLTYPE BaseRelease()
    {
        long refCount = InterlockedDecrement((LONG *) &m_refCount);

        if (refCount == 0)
            delete this;

        return (refCount);
    }

private:
    // For ref counting of COM objects
    ULONG m_refCount;

};

class CorProfInfo : public CorProfBase, public ICorProfilerInfo2
{
public:

    /*********************************************************************
     * Ctor/Dtor
     */
    CorProfInfo();

    virtual ~CorProfInfo();

    /*********************************************************************
     * IUnknown support
     */

    COM_METHOD QueryInterface(REFIID id, void **pInterface);

    ULONG STDMETHODCALLTYPE AddRef()
    {
        return (BaseAddRef());
    }

    ULONG STDMETHODCALLTYPE Release()
    {
        return (BaseRelease());
    }

    /*********************************************************************
     * ICorProfilerInfo support
     */
    COM_METHOD GetClassFromObject(
        /* [in] */ ObjectID objectId,
        /* [out] */ ClassID *pClassId);

    COM_METHOD GetClassFromToken(
        /* [in] */ ModuleID moduleId,
        /* [in] */ mdTypeDef typeDef,
        /* [out] */ ClassID *pClassId);

    COM_METHOD GetCodeInfo(
        /* [in] */ FunctionID functionId,
        /* [out] */ LPCBYTE *pStart,
        /* [out] */ ULONG *pcSize);

    COM_METHOD GetEventMask(
        /* [out] */ DWORD *pdwEvents);

    COM_METHOD GetFunctionFromIP(
        /* [in] */ LPCBYTE ip,
        /* [out] */ FunctionID *pFunctionId);

    COM_METHOD GetFunctionFromToken(
        /* [in] */ ModuleID ModuleId,
        /* [in] */ mdToken token,
        /* [out] */ FunctionID *pFunctionId);

    /* [local] */ COM_METHOD GetHandleFromThread(
        /* [in] */ ThreadID ThreadID,
        /* [out] */ HANDLE *phThread);

    COM_METHOD GetObjectSize(
        /* [in] */ ObjectID objectId,
        /* [out] */ ULONG *pcSize);

    COM_METHOD IsArrayClass(
        /* [in] */  ClassID classId,
        /* [out] */ CorElementType *pBaseElemType,
        /* [out] */ ClassID *pBaseClassId,
        /* [out] */ ULONG   *pcRank);

    COM_METHOD GetThreadInfo(
        /* [in] */ ThreadID threadId,
        /* [out] */ DWORD *pdwWin32ThreadId);

    COM_METHOD GetCurrentThreadID(
        /* [out] */ ThreadID *pThreadId);


    COM_METHOD GetClassIDInfo(
        /* [in] */ ClassID classId,
        /* [out] */ ModuleID  *pModuleId,
        /* [out] */ mdTypeDef  *pTypeDefToken);

    COM_METHOD GetFunctionInfo(
        /* [in] */ FunctionID functionId,
        /* [out] */ ClassID  *pClassId,
        /* [out] */ ModuleID  *pModuleId,
        /* [out] */ mdToken  *pToken);

    COM_METHOD SetEventMask(
        /* [in] */ DWORD dwEvents);

    COM_METHOD SetEnterLeaveFunctionHooks(
        /* [in] */ FunctionEnter *pFuncEnter,
        /* [in] */ FunctionLeave *pFuncLeave,
        /* [in] */ FunctionTailcall *pFuncTailcall);

    COM_METHOD SetFunctionIDMapper(
        /* [in] */ FunctionIDMapper *pFunc);

    COM_METHOD SetILMapFlag();

    COM_METHOD GetTokenAndMetaDataFromFunction(
        FunctionID  functionId,
        REFIID      riid,
        IUnknown    **ppImport,
        mdToken     *pToken);

    COM_METHOD GetModuleInfo(
        ModuleID    moduleId,
        LPCBYTE     *ppBaseLoadAddress,
        ULONG       cchName,
        ULONG       *pcchName,
        WCHAR       szName[],
        AssemblyID  *pAssemblyId);

    COM_METHOD GetModuleMetaData(
        ModuleID    moduleId,
        DWORD       dwOpenFlags,
        REFIID      riid,
        IUnknown    **ppOut);

    COM_METHOD GetILFunctionBody(
        ModuleID    moduleId,
        mdMethodDef methodid,
        LPCBYTE     *ppMethodHeader,
        ULONG       *pcbMethodSize);

    COM_METHOD GetILFunctionBodyAllocator(
        ModuleID    moduleId,
        IMethodMalloc **ppMalloc);

    COM_METHOD SetILFunctionBody(
        ModuleID    moduleId,
        mdMethodDef methodid,
        LPCBYTE     pbNewILMethodHeader);

    COM_METHOD GetAppDomainInfo(
        AppDomainID appDomainId,
        ULONG       cchName,
        ULONG       *pcchName,
        WCHAR       szName[  ],
        ProcessID   *pProcessId);

    COM_METHOD GetAssemblyInfo(
        AssemblyID  assemblyId,
        ULONG     cchName,
        ULONG     *pcchName,
        WCHAR       szName[  ],
        AppDomainID *pAppDomainId,
        ModuleID    *pModuleId);

    COM_METHOD SetFunctionReJIT(
        FunctionID  functionId);

    COM_METHOD SetILInstrumentedCodeMap(
        FunctionID functionID,
        BOOL fStartJit,
        ULONG cILMapEntries,
        COR_IL_MAP rgILMapEntries[]);

    COM_METHOD ForceGC();

    COM_METHOD GetInprocInspectionInterface(
        IUnknown **ppicd);

    COM_METHOD GetInprocInspectionIThisThread(
        IUnknown **ppicd);

    COM_METHOD GetThreadContext(
        ThreadID threadId,
        ContextID *pContextId);

    COM_METHOD BeginInprocDebugging(
        BOOL   fThisThreadOnly,
        DWORD *pdwProfilerContext);

    COM_METHOD EndInprocDebugging(
        DWORD  dwProfilerContext);

    COM_METHOD GetILToNativeMapping(
                /* [in] */  FunctionID functionId,
                /* [in] */  ULONG32 cMap,
                /* [out] */ ULONG32 *pcMap,
                /* [out, size_is(cMap), length_is(*pcMap)] */
                    COR_DEBUG_IL_TO_NATIVE_MAP map[]);

    /*********************************************************************
     * ICorProfilerInfo2 support
     */

    COM_METHOD DoStackSnapshot(
        /* [in] */ ThreadID thread,
        /* [in] */ StackSnapshotCallback *callback,
        /* [in] */ ULONG32 infoFlags,
        /* [in] */ void *clientData,
        /* [in] */ BYTE context[],
        /* [in] */ ULONG32 contextSize);

    COM_METHOD SetEnterLeaveFunctionHooks2(
        /* [in] */ FunctionEnter2 *pFuncEnter,
        /* [in] */ FunctionLeave2 *pFuncLeave,
        /* [in] */ FunctionTailcall2 *pFuncTailcall);

    COM_METHOD GetFunctionInfo2(
        /* in  */ FunctionID funcId,
        /* in  */ COR_PRF_FRAME_INFO frameInfo,
        /* out */ ClassID *pClassId,
        /* out */ ModuleID *pModuleId,
        /* out */ mdToken *pToken,
        /* in  */ ULONG32 cTypeArgs,
        /* out */ ULONG32 *pcTypeArgs,
        /* out */ ClassID typeArgs[]);

    COM_METHOD GetStringLayout(
        /* out */ ULONG *pBufferLengthOffset,
        /* out */ ULONG *pStringLengthOffset,
        /* out */ ULONG *pBufferOffset);

    COM_METHOD GetClassLayout(
        /* in  */   ClassID classID,
        /* in/out*/ COR_FIELD_OFFSET rFieldOffset[],
        /* in  */   ULONG cFieldOffset,
        /* out */   ULONG *pcFieldOffset,
        /* out */   ULONG *pulClassSize);

    COM_METHOD GetCodeInfo2(FunctionID functionID,
                            ULONG32  cCodeInfos,
                            ULONG32* pcCodeInfos,
                            COR_PRF_CODE_INFO codeInfos[]);


    COM_METHOD GetClassFromTokenAndTypeArgs(ModuleID moduleID,
                                            mdTypeDef typeDef,
                                            ULONG32 cTypeArgs,
                                            ClassID typeArgs[],
                                            ClassID* pClassID);

    COM_METHOD EnumModuleFrozenObjects(ModuleID moduleID,
                                       ICorProfilerObjectEnum** ppEnum);

    COM_METHOD GetFunctionFromTokenAndTypeArgs(ModuleID moduleID,
                                               mdMethodDef funcDef,
                                               ClassID classId,
                                               ULONG32 cTypeArgs,
                                               ClassID typeArgs[],
                                               FunctionID* pFunctionID);

    COM_METHOD GetArrayObjectInfo( ObjectID objectId,
                                ULONG32 cDimensionSizes,
                                ULONG32 pDimensionSizes[],
                                int pDimensionLowerBounds[],
                                BYTE    **ppData);

    COM_METHOD GetBoxClassLayout(ClassID classId,
                               ULONG32 *pBufferOffset);


    COM_METHOD GetThreadAppDomain(ThreadID threadId,
                                  AppDomainID *pAppDomainId);


    COM_METHOD GetRVAStaticAddress(ClassID classId,
                                   mdFieldDef fieldToken,
                                   void **ppAddress);

    COM_METHOD GetAppDomainStaticAddress(ClassID classId,
                                         mdFieldDef fieldToken,
                                         AppDomainID appDomainId,
                                         void **ppAddress);

    COM_METHOD GetThreadStaticAddress(ClassID classId,
                                      mdFieldDef fieldToken,
                                      ThreadID threadId,
                                      void **ppAddress);

    COM_METHOD GetContextStaticAddress(ClassID classId,
                                       mdFieldDef fieldToken,
                                       ContextID contextId,
                                       void **ppAddress);

    COM_METHOD GetStaticFieldInfo(ClassID classId,
                                  mdFieldDef fieldToken,
                                  COR_PRF_STATIC_TYPE *pFieldInfo);

    COM_METHOD GetClassIDInfo2(ClassID classId,
                            ModuleID *pModuleId,
                            mdTypeDef *pTypeDefToken,
                            ClassID *pParentClassId,
                            ULONG32 cNumTypeArgs,
                            ULONG32 *pcNumTypeArgs,
                            ClassID typeArgs[]);

    COM_METHOD GetGenerationBounds(ULONG cObjectRanges,
                                   ULONG *pcObjectRanges,
                                   COR_PRF_GC_GENERATION_RANGE ranges[]);
 
    COM_METHOD GetObjectGeneration(ObjectID objectId,
                                   COR_PRF_GC_GENERATION_RANGE *range);

    COM_METHOD GetNotifiedExceptionClauseInfo(
                                COR_PRF_EX_CLAUSE_INFO *pinfo);

private:
    HRESULT ForwardInprocInspectionRequestToEE(IUnknown **ppicd,
                                               bool fThisThread);

    DWORD m_dwEventMask;
};

/*
 * This will attempt to CoCreate a profiler, if one has been registered.
 */
HRESULT CoCreateProfiler( __inout_z LPWSTR wszCLSID, __in LPWSTR wszProfileDLL, ICorProfilerCallback **ppCallback,  BOOL fSupportEverett );

#endif //__PROFILE_H__
