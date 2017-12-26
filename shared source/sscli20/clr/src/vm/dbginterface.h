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
//
// COM+99 Debug Interface Header
//

#ifndef _dbgInterface_h_
#define _dbgInterface_h_

#include "common.h"
#include "eedbginterface.h"
#include "corjit.h"
#include "enc.h"
#include "../debug/inc/dbgipcevents.h"
#include "primitives.h"

typedef DPTR(struct ICorDebugInfo::NativeVarInfo) PTR_NativeVarInfo;

typedef void (*FAVORCALLBACK)(void *);

//
// The purpose of this object is to serve as an entry point to the
// debugger, which used to reside in a seperate DLL.
//

class DebugInterface : public ICorDebugInfo
{
    VPTR_BASE_VTABLE_CLASS(DebugInterface);

public:

    //
    // Functions exported from the debugger to the EE.
    //

#ifndef DACCESS_COMPILE

    virtual HRESULT Startup(void) = 0;

    virtual void SetEEInterface(EEDebugInterface* i) = 0;

    virtual void StopDebugger(void) = 0;

    virtual BOOL IsStopped(void) = 0;

    virtual void ThreadCreated(Thread* pRuntimeThread) = 0;

    virtual void ThreadStarted(Thread* pRuntimeThread,
                               BOOL fAttaching) = 0;

    virtual void DetachThread(Thread *pRuntimeThread) = 0;

    // Called when a module is being loaded into an AppDomain.  
    // This includes when a domain neutral module is loaded into a new AppDomain.
    // This is called only when a debugger is attached, and will occur after the 
    // related LoadAssembly and AddAppDomainToIPCBlock calls and before any 
    // LoadClass calls for this module.
    virtual void LoadModule(Module* pRuntimeModule,     // the module being loaded
                            LPCWSTR psModuleName,       // module file name
                            DWORD dwModuleName,         // number of characters in file name not including null
                            Assembly *pAssembly,        // the assembly the module belongs to
                            AppDomain *pAppDomain,      // the AppDomain the module is being loaded into
                            BOOL fAttaching) = 0;       // true if this notification is due to a debugger being attached to the process 

    // Called for all modules in an AppDomain when the AppDomain is unloaded.
    // This includes domain neutral modules that are also loaded into other domains.
    // This is called only when a debugger is attached, and will occur after all UnloadClass
    // calls and before any UnloadAssembly or RemoveAppDomainFromIPCBlock calls realted
    // to this module.  On CLR shutdown, we are not guarenteed to get UnloadModule calls for
    // all outstanding loaded modules.
    virtual void UnloadModule(Module* pRuntimeModule, AppDomain *pAppDomain) = 0;

    // Called when a Module* is being destroyed.
    // Specifically, the Module has completed unloading (which may have been done asyncronously), all resources 
    // associated are being freed, and the Module* is about to become invalid.  The debugger should remove all
    // references to this Module*.
    // NOTE: This is called REGARDLESS of whether a debugger is attached or not, and will occur after any other
    // notifications about this module (including any RemoveAppDomainFromIPCBlock call indicating the module's
    // domain has been unloaded).
    virtual void DestructModule(Module *pModule) = 0;

    virtual BOOL LoadClass(TypeHandle th,
                           mdTypeDef classMetadataToken,
                           Module *classModule,
                           AppDomain *pAppDomain,
                           BOOL fSendEventToAllAppDomains,
                           BOOL fAttaching) = 0;

    virtual void UnloadClass(mdTypeDef classMetadataToken,
                             Module *classModule,
                             AppDomain *pAppDomain,
                             BOOL fSendEventToAllAppDomains) = 0;

    // Filter we call in 1st-pass to dispatch a CHF callback.
    // pCatchStackAddress really should be a Frame* onto the stack. That way the CHF stack address
    // and the debugger's stacktrace Frames will match up.
    // This is only called by stubs.
    virtual LONG NotifyOfCHFFilter(EXCEPTION_POINTERS* pExceptionPointers, PVOID pCatchStackAddr) = 0;


    virtual bool FirstChanceNativeException(EXCEPTION_RECORD *exception,
                                       CONTEXT *context,
                                       DWORD code,
                                       Thread *thread) = 0;

    // pThread is thread that exception is on.
    // currentSP is stack frame of the throw site.
    // currentIP is ip of the throw site.
    // pStubFrame = NULL if the currentSp is for a non-stub frame (ie, a regular JITed catched).
    // For stub-based throws, pStubFrame is the EE Frame of the stub.
    virtual bool FirstChanceManagedException(Thread *pThread, SIZE_T currentIP, SIZE_T currentSP
                                             IA64_ARG(SIZE_T currentBSP)) = 0;

    virtual void FirstChanceManagedExceptionCatcherFound(Thread *pThread,
                                                         MethodDesc *pMD, TADDR pMethodAddr,
                                                         BYTE *currentSP
                                                         IA64_ARG(BYTE *currentBSP),
                                                         EE_ILEXCEPTION_CLAUSE *pEHClause) = 0;

    virtual LONG LastChanceManagedException(EXCEPTION_RECORD *pExceptionRecord,
            CONTEXT *pContext,
            Thread *thread,
            BOOL jitAttachRequested,
            BOOL useManagedDebugger) = 0;

    virtual void ManagedExceptionUnwindBegin(Thread *pThread) = 0;

    virtual void DeleteInterceptContext(void *pContext) = 0;

    virtual void ExceptionFilter(MethodDesc *fd, TADDR pMethodAddr,
                                 SIZE_T offset,
                                 BYTE *pStack
                                 IA64_ARG(BYTE *pBStore)) = 0;

    virtual void ExceptionHandle(MethodDesc *fd, TADDR pMethodAddr,
                                 SIZE_T offset,
                                 BYTE *pStack
                                 IA64_ARG(BYTE *pBStore)) = 0;

    virtual void SendUserBreakpoint(Thread *thread, WCHAR* szMessage = NULL) = 0;

    virtual void UpdateModuleSyms(Module *pRuntimeModule,
                                  AppDomain *pAppDomain,
                                  BOOL fAttaching) = 0;

    // JITBeginning() is called before a method is jit-compiled.
    // Note: perhaps have JITBeginning return a BOOL so the debugger
    // can decide wether or not to let the JIT take place?
    //
    virtual void JITBeginning(MethodDesc* fd,
                              bool trackJITInfo) = 0;

    //
    // RequestFavor gets the debugger helper thread to call a function. It's
    // typically called when the current thread can't call the function directly,
    // e.g, there isn't enough stack space.
    //
    // RequestFavor ensures that the helper thread has been initialized to
    // execute favors and then calls Debugger:DoFavor. It blocks until the
    // favor callback completes. 
    //
    // Parameters:
    //   fp    - Favour callback function
    //   pData - the parameter passed to the favor callback function.
    //
    // Return values:
    //   S_OK if the function succeeds, else a failure HRESULT
    //   
    virtual HRESULT RequestFavor(FAVORCALLBACK fp, void * pData) = 0;

#endif // #ifndef DACCESS_COMPILE

    // JITComplete() is called after a method is jit-compiled, successfully or not

    enum JITCodeInfo
    {
        JCI_NONE                = 0x00,
        JCI_TRACK_DEBUG_INFO    = 0x01, // JIT-Compiler generated debug tracking info (CORJIT_FLG_DEBUG_INFO)
        JCI_DEBUG_CODE          = 0x02, // JIT-Compiler generated debuggable non-optimized code (CORJIT_FLG_DEBUG_CODE)
        JCI_JMC_PROBE           = 0x04  // JIT-Compiler inserted a JMC probe (ICorDynamicInfo.getJustMyCodeHandle)
    };

#ifndef DACCESS_COMPILE

    virtual void JITComplete(MethodDesc* fd,
                            BYTE* newAddress,
                            SIZE_T sizeOfCode,
                            JITCodeInfo jitCodeInfo) = 0;

    //
    // EnC functions
    //

    virtual     void GetVarInfo(MethodDesc *       fd,         // [IN] method of interest
                            void *DebuggerVersionToken,    // [IN] which edit version
                            SIZE_T *           cVars,      // [OUT] size of 'vars'
                            const NativeVarInfo **vars     // [OUT] map telling where local vars are stored
                            ) = 0;

//    virtual bool InterpretedBreak(Thread *thread, const BYTE *ip) = 0;

    virtual BOOL CheckGetPatchedOpcode(CORDB_ADDRESS_TYPE *address, /*OUT*/ PRD_TYPE *pOpcode) = 0;

    virtual PRD_TYPE GetPatchedOpcode(CORDB_ADDRESS_TYPE *ip) = 0;

    virtual void TraceCall(const BYTE *target) = 0;

    virtual void PossibleTraceCall(UMEntryThunk *pUMEntryThunk,
                                   Frame *pFrame) = 0;

    virtual bool ThreadsAtUnsafePlaces(void) = 0;

    virtual void PitchCode(MethodDesc *fd,
                           const BYTE *pbAddr ) = 0;

    virtual void MovedCode(MethodDesc *fd,
                       CORDB_ADDRESS_TYPE *pbOldAddress,
                        CORDB_ADDRESS_TYPE *pbNewAddress) = 0;

    virtual HRESULT LaunchDebuggerForUser (void) = 0;

    virtual void SendLogMessage (int iLevel,
                             WCHAR *pCategory,
                             int iCategoryLen,
                             WCHAR *pMessage,
                             int iMessageLen) = 0;

    // Send an MDA notification. This ultimately translates to an ICorDebugMDA object on the Right-Side.
    virtual void SendMDANotification(
        Thread * pThread, // may be NULL. Lets us send on behalf of other threads.
        SString * szName,
        SString * szDescription,
        SString * szXML,
        CorDebugMDAFlags flags,
        BOOL bAttach
    ) = 0;

    virtual bool IsJMCMethod(Module* pModule, mdMethodDef tkMethod) = 0;

    // Given a method, get's its EnC version number. 1 if the method is not EnCed.
    // Note that MethodDescs are reused between versions so this will give us
    // the most recent EnC number.
    virtual int GetMethodEncNumber(MethodDesc * pMethod) = 0;

    virtual void SendLogSwitchSetting (int iLevel,
                                   int iReason,
                                   WCHAR *pLogSwitchName,
                                   WCHAR *pParentSwitchName) = 0;

    virtual bool IsLoggingEnabled (void) = 0;

    virtual bool GetILOffsetFromNative (MethodDesc *PFD,
                                                                            const BYTE *pbAddr,
                                                                            DWORD nativeOffset,
                                                                            DWORD *ilOffset) = 0;

    virtual HRESULT GetILToNativeMapping(MethodDesc *pMD,
                                         ULONG32 cMap,
                                         ULONG32 *pcMap,
                                         COR_DEBUG_IL_TO_NATIVE_MAP map[]) = 0;

    virtual DWORD GetHelperThreadID(void ) = 0;

    // Called whenever a new AppDomain is created, regardless of whether a debugger is attached.
    // This will be called before any LoadAssembly calls for assemblies in this domain.
    virtual HRESULT AddAppDomainToIPC (AppDomain *pAppDomain) = 0;

    // Called whenever an AppDomain is unloaded, regardless of whether a Debugger is attached
    // This will occur after any UnloadAssembly and UnloadModule callbacks for this domain (if any).
    virtual HRESULT RemoveAppDomainFromIPC (AppDomain *pAppDomain) = 0;

    virtual HRESULT UpdateAppDomainEntryInIPC (AppDomain *pAppDomain) = 0;

    // Called when an assembly is being loaded into an AppDomain.  
    // This includes when a domain neutral assembly is loaded into a new AppDomain.
    // This is called only when a debugger is attached, and will occur after the 
    // related AddAppDomainToIPCBlock call and before any LoadModule or
    // LoadClass calls for this assembly.
    virtual void LoadAssembly(AppDomain* pRuntimeAppDomain,
                              Assembly *pAssembly,      // the assembly being loaded
                              BOOL fIsSystemAssembly,   // true for mscorlib
                              BOOL fAttaching) = 0;     // is the notification due to a debugger being attached?

    // Called for all assemblies in an AppDomain when the AppDomain is unloaded.
    // This includes domain neutral assemblies that are also loaded into other domains.
    // This is called only when a debugger is attached, and will occur after all UnloadClass
    // and UnloadModule calls and before any RemoveAppDomainFromIPCBlock calls realted
    // to this assembly.  On CLR shutdown, we are not guarenteed to get UnloadAssembly calls for
    // all outstanding loaded assemblies.
    virtual void UnloadAssembly(AppDomain *pAppDomain, Assembly* pAssembly) = 0;

    virtual HRESULT SetILInstrumentedCodeMap(MethodDesc *fd,
                                             BOOL fStartJit,
                                             ULONG32 cILMapEntries,
                                             COR_IL_MAP rgILMapEntries[]) = 0;

    virtual void EarlyHelperThreadDeath(void) = 0;

    virtual void ShutdownBegun(void) = 0;

    virtual void LockDebuggerForShutdown(void) = 0;

    virtual void DisableDebugger(void) = 0;

    virtual HRESULT NameChangeEvent(AppDomain *pAppDomain,
                                    Thread *pThread) = 0;

    virtual BOOL SendCtrlCToDebugger(DWORD dwCtrlType) = 0;

    // Allows the debugger to keep an up to date list of special threads
    virtual HRESULT UpdateSpecialThreadList(DWORD cThreadArrayLength,
                                            DWORD *rgdwThreadIDArray) = 0;

    // Updates the pointer for the debugger services
    virtual void SetIDbgThreadControl(IDebuggerThreadControl *pIDbgThreadControl) = 0;

    virtual DWORD GetRCThreadId(void) = 0;

    virtual HRESULT GetVariablesFromOffset(MethodDesc                 *pMD,
                                           UINT                        varNativeInfoCount,
                                           ICorJitInfo::NativeVarInfo *varNativeInfo,
                                           SIZE_T                      offsetFrom,
                                           CONTEXT                    *pCtx,
                                           SIZE_T                     *rgVal1,
                                           SIZE_T                     *rgVal2,
                                           UINT                       uRgValSize,
                                           BYTE                     ***rgpVCs) = 0;

    virtual HRESULT SetVariablesAtOffset(MethodDesc                 *pMD,
                                         UINT                        varNativeInfoCount,
                                         ICorJitInfo::NativeVarInfo *varNativeInfo,
                                         SIZE_T                      offsetTo,
                                         CONTEXT                    *pCtx,
                                         SIZE_T                     *rgVal1,
                                         SIZE_T                     *rgVal2,
                                         BYTE                      **rgpVCs) = 0;

    virtual BOOL IsThreadContextInvalid(Thread *pThread) = 0;

    virtual AppDomainEnumerationIPCBlock *GetAppDomainEnumIPCBlock() = 0;

    // For Just-My-Code (aka Just-User-Code).
    // The jit inserts probes that look like.
    // if (*pAddr != 0) call g_pDebugInterface->OnMethodEnter()

    // Invoked when we enter a user method.
    // pIP is an ip within the method, right after the prolog.
    virtual void OnMethodEnter(void * pIP) = 0;

    // Given a method, the debugger provides the address of the flag.
    // This allows the debugger to store the flag whereever it wants
    // and with whatever granularity (per-module, per-class, per-function, etc).
    virtual DWORD* GetJMCFlagAddr(Module * pModule) = 0;

    // notification for SQL fiber debugging support
    virtual void CreateConnection(CONNID dwConnectionId, WCHAR *wzName, BOOL fAttaching) = 0;
    virtual void DestroyConnection(CONNID dwConnectionId) = 0;
    virtual void ChangeConnection(CONNID dwConnectionId, BOOL fAttaching) = 0;

    //
    // This function is used to identify the helper thread.
    //
    virtual bool ThisIsHelperThread(void) = 0;

    virtual HRESULT ReDaclEvents(PSECURITY_DESCRIPTOR securityDescriptor) = 0;

    virtual BOOL ShouldAutoAttach() = 0;
    virtual BOOL FallbackJITAttachPrompt() = 0;
    virtual HRESULT SetFiberMode(bool isFiberMode) = 0;

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags) = 0;
#endif
};

#ifndef DACCESS_COMPILE
// Helper to make GCC compile. GCC can't handle putting a virtual call in a filter.
LONG NotifyOfCHFFilterWrapper(EXCEPTION_POINTERS *pExceptionInfo, PVOID pCatcherStackAddr);
#endif


#endif // _dbgInterface_h_
