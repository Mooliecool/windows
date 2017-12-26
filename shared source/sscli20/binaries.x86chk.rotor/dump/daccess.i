#line 1 "c:\\sscli20\\clr\\src\\inc\\daccess.h"
#line 1 "c:\\sscli20\\clr\\src\\inc\\warningcontrol.h"

























#pragma warning(disable :4001)   
#pragma warning(error   :4007)   
#pragma warning(disable :4010)   
#pragma warning(error   :4013)   
#pragma warning(disable :4022)   
#pragma warning(disable :4028)	 
#pragma warning(disable :4047)   
#pragma warning(disable :4053)   
#pragma warning(disable :4056)   
#pragma warning(disable :4061)   
#pragma warning(error   :4071)   
#pragma warning(error   :4072)   
#pragma warning(3               :4092)   
#pragma warning(disable :4100)   
#pragma warning(disable :4101)   

#pragma warning(3               :4121)   
#pragma warning(disable :4127)   
#pragma warning(3               :4125)   
#pragma warning(3               :4130)   
#pragma warning(3               :4132)   
#pragma warning(error   :4171)   
#pragma warning(4               :4177)   
#pragma warning(disable :4201)   
#pragma warning(disable :4204)   
#pragma warning(4               :4206)   
#pragma warning(3               :4212)   
#pragma warning(error           :4259)   
#pragma warning(disable         :4291)   
#pragma warning(disable :4334)   
#pragma warning(disable :4345)   
#pragma warning(disable :4430)   
#pragma warning(3               :4509)   
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 

#pragma warning(3               :4530)   
#pragma warning(error   :4551)   

#pragma warning(error   :4700)   
#pragma warning(disable :4706)   
#pragma warning(error   :4806)   
#pragma warning(disable :4995)   
#line 82 "c:\\sscli20\\clr\\src\\inc\\warningcontrol.h"
#line 1 "c:\\sscli20\\clr\\src\\inc\\daccess.h"
#line 1 "c:\\sscli20\\clr\\src\\inc\\warningcontrol.h"

























#pragma warning(disable :4001)   
#pragma warning(error   :4007)   
#pragma warning(disable :4010)   
#pragma warning(error   :4013)   
#pragma warning(disable :4022)   
#pragma warning(disable :4028)	 
#pragma warning(disable :4047)   
#pragma warning(disable :4053)   
#pragma warning(disable :4056)   
#pragma warning(disable :4061)   
#pragma warning(error   :4071)   
#pragma warning(error   :4072)   
#pragma warning(3               :4092)   
#pragma warning(disable :4100)   
#pragma warning(disable :4101)   

#pragma warning(3               :4121)   
#pragma warning(disable :4127)   
#pragma warning(3               :4125)   
#pragma warning(3               :4130)   
#pragma warning(3               :4132)   
#pragma warning(error   :4171)   
#pragma warning(4               :4177)   
#pragma warning(disable :4201)   
#pragma warning(disable :4204)   
#pragma warning(4               :4206)   
#pragma warning(3               :4212)   
#pragma warning(error           :4259)   
#pragma warning(disable         :4291)   
#pragma warning(disable :4334)   
#pragma warning(disable :4345)   
#pragma warning(disable :4430)   
#pragma warning(3               :4509)   
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 
                                                                 

#pragma warning(3               :4530)   
#pragma warning(error   :4551)   

#pragma warning(error   :4700)   
#pragma warning(disable :4706)   
#pragma warning(error   :4806)   
#pragma warning(disable :4995)   
#line 82 "c:\\sscli20\\clr\\src\\inc\\warningcontrol.h"
#line 1 "c:\\sscli20\\clr\\src\\inc\\daccess.h"



























































































































































































































































































































































































#line 1 "c:\\sscli20\\clr\\src\\inc\\switches.h"













































#line 47 "c:\\sscli20\\clr\\src\\inc\\switches.h"



#line 51 "c:\\sscli20\\clr\\src\\inc\\switches.h"






#line 58 "c:\\sscli20\\clr\\src\\inc\\switches.h"










    
    
    








#line 80 "c:\\sscli20\\clr\\src\\inc\\switches.h"



#line 84 "c:\\sscli20\\clr\\src\\inc\\switches.h"

#line 381 "c:\\sscli20\\clr\\src\\inc\\daccess.h"


#line 1 "c:\\sscli20\\palrt\\inc\\specstrings.h"










































































































































































































































#pragma once
#line 237 "c:\\sscli20\\palrt\\inc\\specstrings.h"











#line 249 "c:\\sscli20\\palrt\\inc\\specstrings.h"
#line 250 "c:\\sscli20\\palrt\\inc\\specstrings.h"











    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    


























































































































































































































    
    
#line 514 "c:\\sscli20\\palrt\\inc\\specstrings.h"















#line 384 "c:\\sscli20\\clr\\src\\inc\\daccess.h"
#line 385 "c:\\sscli20\\clr\\src\\inc\\daccess.h"







typedef ULONG_PTR TADDR;









typedef struct _DacGlobals
{



#line 1 "c:\\sscli20\\clr\\src\\inc\\dacvars.h"







































ULONG ExecutionManager__m_CodeRangeList;
ULONG ExecutionManager__m_DataRangeList;
ULONG ExecutionManager__m_pJitList;
ULONG ExecutionManager__m_pDefaultCodeMan;
ULONG ExecutionManager__m_dwReaderCount;
ULONG ExecutionManager__m_dwWriterLock;

ULONG dac__hlpDynamicFuncTable;


ULONG dac__JIT_Writeable_Thunks_Buf;

#line 53 "c:\\sscli20\\clr\\src\\inc\\dacvars.h"

ULONG StubManager__g_pFirstManager;
ULONG IJWNOADThunk__s_AddrOfMakeCall;
ULONG IJWNOADThunkStubManager__g_pManager;  
ULONG MethodDescPrestubManager__g_pManager;
ULONG StubLinkStubManager__g_pManager;
ULONG ThunkHeapStubManager__g_pManager;
ULONG JumpStubStubManager__g_pManager;
ULONG EntryPointStubManager__g_pManager;
ULONG ILStubManager__g_pManager;
ULONG NDirectDispatchStubManager__g_pManager;
ULONG ComPlusDispatchStubManager__g_pManager;
ULONG ReverseInteropStubManager__g_pManager;
ULONG DelegateInvokeStubManager__g_pManager;
ULONG VirtualCallStubManagerManager__g_pManager;

ULONG ThreadStore__s_pThreadStore;

ULONG ThreadpoolMgr__cpuUtilization;
ULONG ThreadpoolMgr__NumWorkerThreads;
ULONG ThreadpoolMgr__MinLimitTotalWorkerThreads;
ULONG ThreadpoolMgr__MaxLimitTotalWorkerThreads;
ULONG ThreadpoolMgr__NumIdleWorkerThreads;
ULONG ThreadpoolMgr__NumQueuedWorkRequests;
ULONG ThreadpoolMgr__WorkRequestHead; 
ULONG ThreadpoolMgr__WorkRequestTail; 
ULONG ThreadpoolMgr__NumTimers;    
ULONG ThreadpoolMgr__NumCPThreads;
ULONG ThreadpoolMgr__NumFreeCPThreads;
ULONG ThreadpoolMgr__MaxFreeCPThreads;
ULONG ThreadpoolMgr__NumRetiredCPThreads;
ULONG ThreadpoolMgr__MaxLimitTotalCPThreads;
ULONG ThreadpoolMgr__CurrentLimitTotalCPThreads;
ULONG ThreadpoolMgr__MinLimitTotalCPThreads;        
ULONG ThreadpoolMgr__TimerQueue;        

ULONG GCHeap__gcHeapType;
ULONG GCHeap__FinalizerThread;
ULONG GCHeap__GcThread;

ULONG WKS__gc_heap__alloc_allocated;
ULONG WKS__gc_heap__ephemeral_heap_segment;
ULONG WKS__gc_heap__finalize_queue;
ULONG SVR__gc_heap__n_heaps;
ULONG SVR__gc_heap__g_heaps;

ULONG SystemDomain__m_pSystemDomain;
ULONG SystemDomain__m_appDomainIndexList;
ULONG SharedDomain__m_pSharedDomain;

ULONG CExecutionEngine__TlsIndex;

ULONG CNameSpace__m_GcStructuresInvalidCnt;
ULONG dac__g_ECustomDumpFlavor;

ULONG SString__s_Empty;

ULONG CTPMethodTable__s_pTPStub;

ULONG ArrayBase__s_arrayBoundsZero;

ULONG dac__g_pNotificationTable;
ULONG dac__g_dacNotificationFlags;

ULONG dac__gThreadTLSIndex;
ULONG dac__gAppDomainTLSIndex;
ULONG dac__g_pConfig;
ULONG dac__g_Mscorlib;

ULONG dac__generation_table;
ULONG dac__g_card_table;
ULONG dac__g_lowest_address;
ULONG dac__g_highest_address;

ULONG dac__g_pGCHeap;

ULONG dac__g_pThinLockThreadIdDispenser;    
ULONG dac__g_pObjectClass;
ULONG dac__g_pHiddenMethodTableClass;
ULONG dac__g_pStringClass;
ULONG dac__g_pArrayClass;
ULONG dac__g_pExceptionClass;
ULONG dac__g_pThreadAbortExceptionClass;
ULONG dac__g_pOutOfMemoryExceptionClass;
ULONG dac__g_pStackOverflowExceptionClass;
ULONG dac__g_pExecutionEngineExceptionClass;
ULONG dac__g_pDelegateClass;
ULONG dac__g_pMultiDelegateClass;
ULONG dac__g_pFreeObjectMethodTable;
ULONG dac__g_pOverlappedDataClass;
ULONG dac__g_pValueTypeClass;
ULONG dac__g_pEnumClass;
ULONG dac__g_pSharedStaticsClass;
ULONG dac__g_pThreadClass;
ULONG dac__g_pCriticalFinalizerObjectClass;
ULONG dac__g_pAsyncFileStream_AsyncResultClass;
ULONG dac__g_pPredefinedArrayTypes;
ULONG dac__g_pPrepareConstrainedRegionsMethod;
ULONG dac__g_pPrepareConstrainedRegionsNoOpMethod;
ULONG dac__g_pExecuteBackoutCodeHelperMethod;

ULONG dac__g_pObjectCtorMD;
ULONG dac__g_pObjectFinalizerMD;

ULONG dac__g_preStub;
ULONG dac__g_UMThunkPreStub;

ULONG dac__g_fProcessDetach;
ULONG dac__g_fHostConfig;

ULONG dac__g_CORDebuggerControlFlags;
ULONG dac__g_pDebugger;
ULONG dac__g_pDebugInterface;
ULONG dac__g_pEEDbgInterfaceImpl;
ULONG dac__g_pDebugInfoStore;
ULONG dac__g_pEEInterface;

ULONG dac__gLowestFCall;
ULONG dac__gHighestFCall;
ULONG dac__gFCallMethods;

ULONG dac__g_pSyncTable;
ULONG dac__g_pRCWCleanupList;

ULONG dac__g_mscorwksLoadedBaseAddress;
ULONG dac__g_mscorwksPreferredBaseAddress;
ULONG dac__g_mscorwksVirtualSize;
ULONG SyncBlockCache__s_pSyncBlockCache;

ULONG dac__g_HandleTableMap;
ULONG dac__g_pStressLog;

ULONG dac__g_pMdaStaticHeap;

ULONG dac__g_pIPCManagerInterface;
    
ULONG dac__g_FCDynamicallyAssignedImplementations;

ULONG Frame__s_pFrameTypeNames;



#line 408 "c:\\sscli20\\clr\\src\\inc\\daccess.h"

    union {
        ULONG dac__rgJITHelperRVAs[1];
        struct {
            
            


#line 1 "c:\\sscli20\\clr\\src\\inc\\jithelpers.h"



















#line 21 "c:\\sscli20\\clr\\src\\inc\\jithelpers.h"









    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_UNDEF__NULL;

    
    
    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LLSH__JIT_LLsh;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LRSH__JIT_LRsh;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LRSZ__JIT_LRsz;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LMUL__JIT_LMul;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LMUL_OVF__JIT_LMulOvf;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ULMUL_OVF__JIT_ULMulOvf;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LDIV__JIT_LDiv;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LMOD__JIT_LMod;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ULDIV__JIT_ULDiv;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ULMOD__JIT_ULMod;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ULNG2DBL__JIT_ULng2Dbl;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2INT__JIT_Dbl2Lng; 
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2INT_OVF__JIT_Dbl2IntOvf;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2LNG__JIT_Dbl2Lng;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2LNG_OVF__JIT_Dbl2LngOvf;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2UINT__JIT_Dbl2Lng; 
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2UINT_OVF__JIT_Dbl2UIntOvf;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2ULNG__JIT_Dbl2ULng;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBL2ULNG_OVF__JIT_Dbl2ULngOvf;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_FLTREM__JIT_FltRem;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBLREM__JIT_DblRem;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEW_DIRECT__JIT_New;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEW_CROSSCONTEXT__JIT_NewCrossContext;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWFAST__JIT_NewFast;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWSFAST__JIT_NewFast;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWSFAST_ALIGN8__JIT_NewFast;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWSFAST_CHKRESTORE__JIT_New;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEW_SPECIALDIRECT__JIT_NewSpecial;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEW_MDARR__JIT_NewMDArr;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEW_MDARR_NO_LBOUNDS__JIT_NewMDArrNoLBounds;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWARR_1_DIRECT__JIT_NewArr1;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWARR_1_OBJ__JIT_NewArr1;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWARR_1_VC__JIT_NewArr1;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_NEWARR_1_ALIGN8__JIT_NewArr1;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_STRCNS__JIT_StrCns;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_INITCLASS__JIT_InitClass;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_INITINSTCLASS__JIT_InitInstantiatedClass;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ISINSTANCEOFINTERFACE__JIT_IsInstanceOfInterface;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ISINSTANCEOFARRAY__JIT_IsInstanceOfArray;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ISINSTANCEOFCLASS__JIT_IsInstanceOfClass;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ISINSTANCEOFANY__JIT_IsInstanceOfAny;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHKCASTINTERFACE__JIT_ChkCastInterface;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHKCASTARRAY__JIT_ChkCastArray;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHKCASTCLASS__JIT_ChkCastClass;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHKCASTANY__JIT_ChkCastAny;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHKCASTCLASS_SPECIAL__JIT_ChkCastClassSpecial;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_BOX__JIT_Box;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_BOX_NULLABLE__JIT_Box;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_UNBOX__JIT_Unbox;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_UNBOX_NULLABLE__JIT_Unbox_Nullable;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETREFANY__JIT_GetRefAny;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ARRADDR_ST__JIT_Stelem_Ref;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_LDELEMA_REF__JIT_Ldelema_Ref;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_THROW__JIT_Throw;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_RETHROW__JIT_Rethrow;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_USER_BREAKPOINT__JIT_UserBreakpoint;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_RNGCHKFAIL__JIT_RngChkFail;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_OVERFLOW__JIT_Overflow;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_INTERNALTHROW__JIT_InternalThrow;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_INTERNALTHROW_FROM_HELPER__JIT_InternalThrowFromHelper;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_VERIFICATION__JIT_Verification;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SEC_UNMGDCODE_EXCPT__JIT_SecurityUnmanagedCodeException;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_FAIL_FAST__JIT_FailFast;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ENDCATCH__JIT_EndCatch;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_MON_ENTER__JIT_MonEnterWorker;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_MON_EXIT__JIT_MonExitWorker;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_MON_ENTER_STATIC__JIT_MonEnterStatic;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_MON_EXIT_STATIC__JIT_MonExitStatic;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETCLASSFROMMETHODPARAM__JIT_GetClassFromMethodParam;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSYNCFROMCLASSHANDLE__JIT_GetSyncFromClassHandle;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SECURITY_PROLOG__JIT_Security_Prolog;    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SECURITY_PROLOG_FRAMED__JIT_Security_Prolog_Framed;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SECURITY_EPILOG__JIT_Security_Epilog;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CALL_ALLOWED_BYSECURITY__JIT_CallAllowedBySecurity;

     
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_VERIFICATION_RUNTIME_CHECK__JIT_VerificationRuntimeCheck;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_STOP_FOR_GC__JIT_RareDisableHelper;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_POLL_GC__JIT_PollGC;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_STRESS_GC__JIT_StressGC;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECK_OBJ__JIT_CheckObj;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_REF__JIT_WriteBarrier;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECKED_ASSIGN_REF__JIT_CheckedWriteBarrier;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_BYREF__JIT_ByRefWriteBarrier;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_STRUCT__JIT_StructWriteBarrier;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SAFE_RETURNABLE_BYREF__JIT_SafeReturnableByref;


    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_REF_EAX__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_REF_EBX__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_REF_ECX__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_REF_ESI__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_REF_EDI__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_ASSIGN_REF_EBP__NULL;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECKED_ASSIGN_REF_EAX__JIT_CheckedWriteBarrierEAX;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECKED_ASSIGN_REF_EBX__JIT_CheckedWriteBarrierEBX;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECKED_ASSIGN_REF_ECX__JIT_CheckedWriteBarrierECX;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECKED_ASSIGN_REF_ESI__JIT_CheckedWriteBarrierESI;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECKED_ASSIGN_REF_EDI__JIT_CheckedWriteBarrierEDI;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECKED_ASSIGN_REF_EBP__JIT_CheckedWriteBarrierEBP;
#line 159 "c:\\sscli20\\clr\\src\\inc\\jithelpers.h"

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETFIELD32__JIT_GetField32;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SETFIELD32__JIT_SetField32;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETFIELD64__JIT_GetField64;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SETFIELD64__JIT_SetField64;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETFIELDOBJ__JIT_GetFieldObj;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SETFIELDOBJ__JIT_SetFieldObj;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETFIELDSTRUCT__JIT_GetFieldStruct;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SETFIELDSTRUCT__JIT_SetFieldStruct;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETFIELDFLOAT__JIT_GetFieldFloat;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SETFIELDFLOAT__JIT_SetFieldFloat;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETFIELDDOUBLE__JIT_GetFieldDouble;   
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_SETFIELDDOUBLE__JIT_SetFieldDouble;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETFIELDADDR__JIT_GetFieldAddr;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSTATICFIELDADDR__JIT_GetStaticFieldAddr;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETGENERICS_GCSTATIC_BASE__JIT_GetGenericsGCStaticBase;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE__JIT_GetGenericsNonGCStaticBase;


    
    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSHARED_GCSTATIC_BASE__JIT_GetSharedGCStaticBase;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE__JIT_GetSharedNonGCStaticBase;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR__JIT_GetSharedGCStaticBaseNoCtor;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR__JIT_GetSharedNonGCStaticBaseNoCtor;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSHARED_GCSTATIC_BASE_DYNAMICCLASS__JIT_GetSharedGCStaticBaseDynamicClass;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_DYNAMICCLASS__JIT_GetSharedNonGCStaticBaseDynamicClass;

    

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_DBG_IS_JUST_MY_CODE__JIT_DbgIsJustMyCode;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_PROF_FCN_ENTER__JIT_ProfilerEnterLeaveStub;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_PROF_FCN_LEAVE__JIT_ProfilerEnterLeaveStub;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_PROF_FCN_TAILCALL__JIT_ProfilerTailcallStub;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_BBT_FCN_ENTER__JIT_LogMethodEnter;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_PINVOKE_CALLI__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_TAILCALL__JIT_TailCall;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GET_THREAD_FIELD_ADDR_PRIMITIVE__JIT_GetThreadFieldAddr_Primitive;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GET_THREAD_FIELD_ADDR_OBJREF__JIT_GetThreadFieldAddr_Objref;


    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_GET_THREAD__NULL;


#line 213 "c:\\sscli20\\clr\\src\\inc\\jithelpers.h"

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_INIT_PINVOKE_FRAME__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_FAST_PINVOKE__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_CHECK_PINVOKE_DOMAIN__NULL;

    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_MEMSET__NULL;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_MEMCPY__NULL;

    
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_RUNTIMEHANDLE__JIT_GenericHandle;
    ULONG dac__JIT_HELPER_FUNCTION__CORINFO_HELP_VIRTUAL_FUNC_PTR__JIT_VirtualFunctionPointer;
















































#line 273 "c:\\sscli20\\clr\\src\\inc\\jithelpers.h"





#line 417 "c:\\sscli20\\clr\\src\\inc\\daccess.h"


        } _helpers_;
    };

    
    ULONG fn__QueueUserWorkItemCallback;
    ULONG fn__ThreadpoolMgr__AsyncCallbackCompletion;
    ULONG fn__ThreadpoolMgr__AsyncTimerCallbackCompletion;
    ULONG fn__DACNotifyCompilationFinished;

    ULONG fn__NativeDelayFixupAsmStub;
    ULONG fn__NativeDelayFixupAsmStubRet;
#line 431 "c:\\sscli20\\clr\\src\\inc\\daccess.h"
    ULONG fn__PInvokeCalliReturnFromCall;
    ULONG fn__NDirectGenericStubReturnFromCall;
    ULONG fn__DllImportForDelegateGenericStubReturnFromCall;

    
    


#line 1 "c:\\sscli20\\clr\\src\\inc\\vptr_list.h"


















ULONG Thread__vtAddr;

ULONG EEJitManager__vtAddr;
ULONG EEJitDebugInfoStore__vtAddr;




ULONG Fjit_EETwain__vtAddr;
#line 29 "c:\\sscli20\\clr\\src\\inc\\vptr_list.h"


ULONG RangeList__vtAddr;
ULONG LockedRangeList__vtAddr;

ULONG Module__vtAddr;
ULONG ReflectionModule__vtAddr;

ULONG AppDomain__vtAddr;
ULONG BaseDomain__vtAddr;
ULONG SharedDomain__vtAddr;
ULONG SystemDomain__vtAddr;

ULONG DomainAssembly__vtAddr;
ULONG DomainModule__vtAddr;

ULONG CNonVirtualThunkMgr__vtAddr;
ULONG CVirtualThunkMgr__vtAddr;
ULONG MethodDescPrestubManager__vtAddr;
ULONG StubLinkStubManager__vtAddr;
ULONG ThePreStubManager__vtAddr;
ULONG ThunkHeapStubManager__vtAddr;
ULONG VirtualCallStubManager__vtAddr;
ULONG VirtualCallStubManagerManager__vtAddr;
ULONG JumpStubStubManager__vtAddr;
ULONG EntryPointStubManager__vtAddr;
ULONG ILStubManager__vtAddr;
ULONG NDirectDispatchStubManager__vtAddr;
ULONG ComPlusDispatchStubManager__vtAddr;
ULONG ReverseInteropStubManager__vtAddr;
ULONG DelegateInvokeStubManager__vtAddr;
ULONG PEFile__vtAddr;
ULONG PEAssembly__vtAddr;
ULONG PEModule__vtAddr;
ULONG PEImageLayout__vtAddr;
ULONG RawImageLayout__vtAddr;
ULONG ConvertedImageLayout__vtAddr;
ULONG MappedImageLayout__vtAddr;
ULONG FlatImageLayout__vtAddr;
ULONG StreamImageLayout__vtAddr;
ULONG ContextTransitionFrame__vtAddr;
ULONG CustomGCFrame__vtAddr;
ULONG DebuggerClassInitMarkFrame__vtAddr;
ULONG DebuggerSecurityCodeMarkFrame__vtAddr;
ULONG DebuggerExitFrame__vtAddr;
ULONG FaultingExceptionFrame__vtAddr;
ULONG FramedMethodFrame__vtAddr;
ULONG FuncEvalFrame__vtAddr;
ULONG GCFrame__vtAddr;
ULONG HelperMethodFrame__vtAddr;
ULONG HelperMethodFrame_1OBJ__vtAddr;
ULONG HelperMethodFrame_2OBJ__vtAddr;
ULONG HelperMethodFrame_PROTECTOBJ__vtAddr;
ULONG InlinedCallFrame__vtAddr;

ULONG InterceptorFrame__vtAddr;
ULONG LeaveRuntimeFrame__vtAddr;
ULONG SecureDelegateFrame__vtAddr;
ULONG SecurityContextFrame__vtAddr;
ULONG MulticastFrame__vtAddr;
ULONG DelegateTransitionFrame__vtAddr;
ULONG NDirectMethodFrameGeneric__vtAddr;

ULONG NDirectMethodFrameSlim__vtAddr;
#line 94 "c:\\sscli20\\clr\\src\\inc\\vptr_list.h"
ULONG NDirectMethodFrameStandalone__vtAddr;
ULONG NDirectMethodFrameStandaloneCleanup__vtAddr;
ULONG PrestubMethodFrame__vtAddr;
ULONG ProtectByRefsFrame__vtAddr;
ULONG ProtectValueClassFrame__vtAddr;
ULONG ReverseEnterRuntimeFrame__vtAddr;
ULONG SecurityFrame__vtAddr;
ULONG TPMethodFrame__vtAddr;
ULONG UMThkCallFrame__vtAddr;

ULONG TailCallFrame__vtAddr;
#line 106 "c:\\sscli20\\clr\\src\\inc\\vptr_list.h"
ULONG ExceptionFilterFrame__vtAddr;


ULONG AssumeByrefFromJITStack__vtAddr;
#line 111 "c:\\sscli20\\clr\\src\\inc\\vptr_list.h"


ULONG Debugger__vtAddr;
ULONG EEDbgInterfaceImpl__vtAddr;
ULONG DebugInfoManager__vtAddr;
#line 117 "c:\\sscli20\\clr\\src\\inc\\vptr_list.h"

ULONG DebuggerMethodInfoTable__vtAddr;

ULONG LoaderCodeHeap__vtAddr;
ULONG HostCodeHeap__vtAddr;

#line 440 "c:\\sscli20\\clr\\src\\inc\\daccess.h"


} DacGlobals;

extern DacGlobals g_dacGlobals;








void __cdecl DacWarning(            char* format, ...);
void DacNotImpl(void);

void    DacError(HRESULT err);
TADDR   DacGlobalBase(void);
HRESULT DacReadAll(TADDR addr, PVOID buffer, ULONG32 size, bool throwEx);
HRESULT DacWriteAll(TADDR addr, PVOID buffer, ULONG32 size, bool throwEx);
HRESULT DacAllocVirtual(TADDR addr, ULONG32 size,
                        ULONG32 typeFlags, ULONG32 protectFlags,
                        bool throwEx, TADDR* mem);
HRESULT DacFreeVirtual(TADDR mem, ULONG32 size, ULONG32 typeFlags,
                       bool throwEx);
PVOID   DacInstantiateTypeByAddress(TADDR addr, ULONG32 size, bool throwEx);
PVOID   DacInstantiateTypeByAddressNoReport(TADDR addr, ULONG32 size, bool throwEx);
PVOID   DacInstantiateClassByVTable(TADDR addr, bool throwEx);
PSTR    DacInstantiateStringA(TADDR addr, ULONG32 maxChars, bool throwEx);
PWSTR   DacInstantiateStringW(TADDR addr, ULONG32 maxChars, bool throwEx);
TADDR   DacGetTargetAddrForHostAddr(LPCVOID ptr, bool throwEx);
TADDR   DacGetTargetVtForHostVt(LPCVOID vtHost, bool throwEx);
PWSTR   DacGetVtNameW(TADDR targetVtable);
void    DacEnumMemoryRegion(TADDR addr, ULONG32 size);
HRESULT DacWriteHostInstance(PVOID host, bool throwEx);
HRESULT DacPushInstanceState(bool throwEx);
void    DacPopInstanceState(void);





PVOID DacAllocHostOnlyInstance(ULONG32 size, bool throwEx);





bool DacHostPtrHasEnumMark(LPCVOID host);



bool DacHasMethodDescBeenEnumerated(LPCVOID pMD);




bool DacSetMethodDescEnumerated(LPCVOID pMD);


BOOL DacValidateMD(LPCVOID pMD);




    



























    
































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































#line 1527 "c:\\sscli20\\clr\\src\\inc\\daccess.h"























typedef DPTR(BYTE)    PTR_BYTE;
typedef DPTR(signed char) PTR_SBYTE;
typedef DPTR(const BYTE) PTR_CBYTE;
typedef DPTR(WORD)    PTR_WORD;
typedef DPTR(USHORT)  PTR_USHORT;
typedef DPTR(DWORD)   PTR_DWORD;
typedef DPTR(ULONG)   PTR_ULONG;
typedef DPTR(INT32)   PTR_INT32;
typedef DPTR(UINT32)  PTR_UINT32;
typedef DPTR(ULONG64) PTR_ULONG64;
typedef DPTR(INT64)   PTR_INT64;
typedef DPTR(UINT64)  PTR_UINT64;
typedef DPTR(SIZE_T)  PTR_SIZE_T;
typedef DPTR(size_t)  PTR_size_t;
typedef DPTR(TADDR)   PTR_TADDR;
typedef DPTR(int)     PTR_int;
typedef DPTR(unsigned) PTR_unsigned;

typedef S8PTR(char)           PTR_STR;
typedef S8PTR(const char)     PTR_CSTR;
typedef S8PTR(char)           PTR_UTF8;
typedef S8PTR(const char)     PTR_CUTF8;
typedef S16PTR(wchar_t)       PTR_WSTR;
typedef S16PTR(const wchar_t) PTR_CWSTR;

typedef DPTR(struct _CONTEXT)            PTR_CONTEXT;
typedef DPTR(struct _EXCEPTION_POINTERS) PTR_EXCEPTION_POINTERS;
typedef DPTR(struct _EXCEPTION_RECORD)   PTR_EXCEPTION_RECORD;

typedef DPTR(struct IMAGE_COR_VTABLEFIXUP) PTR_IMAGE_COR_VTABLEFIXUP;
typedef DPTR(IMAGE_DATA_DIRECTORY)  PTR_IMAGE_DATA_DIRECTORY;
typedef DPTR(IMAGE_DEBUG_DIRECTORY)  PTR_IMAGE_DEBUG_DIRECTORY;
typedef DPTR(IMAGE_DOS_HEADER)      PTR_IMAGE_DOS_HEADER;
typedef DPTR(IMAGE_NT_HEADERS)      PTR_IMAGE_NT_HEADERS;
typedef DPTR(IMAGE_NT_HEADERS32)    PTR_IMAGE_NT_HEADERS32;
typedef DPTR(IMAGE_NT_HEADERS64)    PTR_IMAGE_NT_HEADERS64;
typedef DPTR(IMAGE_COR20_HEADER)    PTR_IMAGE_COR20_HEADER;
typedef DPTR(IMAGE_SECTION_HEADER)  PTR_IMAGE_SECTION_HEADER;
typedef DPTR(IMAGE_TLS_DIRECTORY)   PTR_IMAGE_TLS_DIRECTORY;


#line 1 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"





























#pragma warning(disable:4200) 
#line 32 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"
typedef LPVOID  mdScope;                
typedef ULONG32 mdToken;                





typedef mdToken mdModule;               
typedef mdToken mdTypeRef;              
typedef mdToken mdTypeDef;              
typedef mdToken mdFieldDef;             
typedef mdToken mdMethodDef;            
typedef mdToken mdParamDef;             
typedef mdToken mdInterfaceImpl;        

typedef mdToken mdMemberRef;            
typedef mdToken mdCustomAttribute;      
typedef mdToken mdPermission;           

typedef mdToken mdSignature;            
typedef mdToken mdEvent;                
typedef mdToken mdProperty;             

typedef mdToken mdModuleRef;            


typedef mdToken mdAssembly;             
typedef mdToken mdAssemblyRef;          
typedef mdToken mdFile;                 
typedef mdToken mdExportedType;         
typedef mdToken mdManifestResource;     

typedef mdToken mdTypeSpec;             

typedef mdToken mdGenericParam;         
typedef mdToken mdMethodSpec;           
typedef mdToken mdGenericParamConstraint; 


typedef mdToken mdString;               

typedef mdToken mdCPToken;              


typedef ULONG RID;


#line 80 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"

typedef enum ReplacesGeneralNumericDefines
{


    IMAGE_DIRECTORY_ENTRY_COMHEADER     =14,
#line 87 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"

    _NEW_FLAGS_IMPLEMENTED              =1,
    __NEW_FLAGS_IMPLEMENTED             =1,
} ReplacesGeneralNumericDefines;





typedef enum ReplacesCorHdrNumericDefines
{

    COMIMAGE_FLAGS_ILONLY               =0x00000001,
    COMIMAGE_FLAGS_32BITREQUIRED        =0x00000002,
    COMIMAGE_FLAGS_IL_LIBRARY           =0x00000004,
    COMIMAGE_FLAGS_STRONGNAMESIGNED     =0x00000008,

    COMIMAGE_FLAGS_NATIVE_ENTRYPOINT    =0x00000010,

    COMIMAGE_FLAGS_TRACKDEBUGDATA       =0x00010000,


    COR_VERSION_MAJOR_V2                =2,
    COR_VERSION_MAJOR                   =COR_VERSION_MAJOR_V2,
    COR_VERSION_MINOR                   =5,
    COR_DELETED_NAME_LENGTH             =8,
    COR_VTABLEGAP_NAME_LENGTH           =8,


    NATIVE_TYPE_MAX_CB                  =1,   
    COR_ILMETHOD_SECT_SMALL_MAX_DATASIZE=0xFF,


    IMAGE_COR_MIH_METHODRVA             =0x01,
    IMAGE_COR_MIH_EHRVA                 =0x02,    
    IMAGE_COR_MIH_BASICBLOCK            =0x08,


    COR_VTABLE_32BIT                    =0x01,          
    COR_VTABLE_64BIT                    =0x02,          
    COR_VTABLE_FROM_UNMANAGED           =0x04,          
    COR_VTABLE_CALL_MOST_DERIVED        =0x10,          


    IMAGE_COR_EATJ_THUNK_SIZE           =32,

    MAX_CLASS_NAME                      =1024,
    MAX_PACKAGE_NAME                    =1024,
} ReplacesCorHdrNumericDefines;


typedef struct IMAGE_COR20_HEADER
{
    
    DWORD                   cb;              
    WORD                    MajorRuntimeVersion;
    WORD                    MinorRuntimeVersion;
    
    
    IMAGE_DATA_DIRECTORY    MetaData;        
    DWORD                   Flags;           



  
    
    
    union {
        DWORD               EntryPointToken;
        DWORD               EntryPointRVA;
    };

    
    
    IMAGE_DATA_DIRECTORY    Resources;
    IMAGE_DATA_DIRECTORY    StrongNameSignature;

    
    IMAGE_DATA_DIRECTORY    CodeManagerTable;
    IMAGE_DATA_DIRECTORY    VTableFixups;
    IMAGE_DATA_DIRECTORY    ExportAddressTableJumps;

    
    IMAGE_DATA_DIRECTORY    ManagedNativeHeader;
    
} IMAGE_COR20_HEADER, *PIMAGE_COR20_HEADER;





#line 179 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"






















typedef enum CorTypeAttr
{
    
    tdVisibilityMask        =   0x00000007,
    tdNotPublic             =   0x00000000,     
    tdPublic                =   0x00000001,     
    tdNestedPublic          =   0x00000002,     
    tdNestedPrivate         =   0x00000003,     
    tdNestedFamily          =   0x00000004,     
    tdNestedAssembly        =   0x00000005,     
    tdNestedFamANDAssem     =   0x00000006,     
    tdNestedFamORAssem      =   0x00000007,     

    
    tdLayoutMask            =   0x00000018,
    tdAutoLayout            =   0x00000000,     
    tdSequentialLayout      =   0x00000008,     
    tdExplicitLayout        =   0x00000010,     
    

    
    tdClassSemanticsMask    =   0x00000060,
    tdClass                 =   0x00000000,     
    tdInterface             =   0x00000020,     
    

    
    tdAbstract              =   0x00000080,     
    tdSealed                =   0x00000100,     
    tdSpecialName           =   0x00000400,     

    
    tdImport                =   0x00001000,     
    tdSerializable          =   0x00002000,     

    
    tdStringFormatMask      =   0x00030000,
    tdAnsiClass             =   0x00000000,     
    tdUnicodeClass          =   0x00010000,     
    tdAutoClass             =   0x00020000,     
    tdCustomFormatClass     =   0x00030000,     
    tdCustomFormatMask      =   0x00C00000,     

    

    tdBeforeFieldInit       =   0x00100000,     
    tdForwarder             =   0x00200000,     

    
    tdReservedMask          =   0x00040800,
    tdRTSpecialName         =   0x00000800,     
    tdHasSecurity           =   0x00040000,     
} CorTypeAttr;






































typedef enum CorMethodAttr
{
    
    mdMemberAccessMask          =   0x0007,
    mdPrivateScope              =   0x0000,     
    mdPrivate                   =   0x0001,     
    mdFamANDAssem               =   0x0002,     
    mdAssem                     =   0x0003,     
    mdFamily                    =   0x0004,     
    mdFamORAssem                =   0x0005,     
    mdPublic                    =   0x0006,     
    

    
    mdStatic                    =   0x0010,     
    mdFinal                     =   0x0020,     
    mdVirtual                   =   0x0040,     
    mdHideBySig                 =   0x0080,     

    
    mdVtableLayoutMask          =   0x0100,
    mdReuseSlot                 =   0x0000,     
    mdNewSlot                   =   0x0100,     
    

    
    mdCheckAccessOnOverride     =   0x0200,     
    mdAbstract                  =   0x0400,     
    mdSpecialName               =   0x0800,     

    
    mdPinvokeImpl               =   0x2000,     
    mdUnmanagedExport           =   0x0008,     

    
    mdReservedMask              =   0xd000,
    mdRTSpecialName             =   0x1000,     
    mdHasSecurity               =   0x4000,     
    mdRequireSecObject          =   0x8000,     

} CorMethodAttr;


































typedef enum CorFieldAttr
{
    
    fdFieldAccessMask           =   0x0007,
    fdPrivateScope              =   0x0000,     
    fdPrivate                   =   0x0001,     
    fdFamANDAssem               =   0x0002,     
    fdAssembly                  =   0x0003,     
    fdFamily                    =   0x0004,     
    fdFamORAssem                =   0x0005,     
    fdPublic                    =   0x0006,     
    

    
    fdStatic                    =   0x0010,     
    fdInitOnly                  =   0x0020,     
    fdLiteral                   =   0x0040,     
    fdNotSerialized             =   0x0080,     

    fdSpecialName               =   0x0200,     

    
    fdPinvokeImpl               =   0x2000,     

    
    fdReservedMask              =   0x9500,
    fdRTSpecialName             =   0x0400,     
    fdHasFieldMarshal           =   0x1000,     
    fdHasDefault                =   0x8000,     
    fdHasFieldRVA               =   0x0100,     
} CorFieldAttr;
























typedef enum CorParamAttr
{
    pdIn                        =   0x0001,     
    pdOut                       =   0x0002,     
    pdOptional                  =   0x0010,     

    
    pdReservedMask              =   0xf000,
    pdHasDefault                =   0x1000,     
    pdHasFieldMarshal           =   0x2000,     

    pdUnused                    =   0xcfe0,
} CorParamAttr;











typedef enum CorPropertyAttr
{
    prSpecialName           =   0x0200,     

    
    prReservedMask          =   0xf400,
    prRTSpecialName         =   0x0400,     
    prHasDefault            =   0x1000,     

    prUnused                =   0xe9ff,
} CorPropertyAttr;








typedef enum CorEventAttr
{
    evSpecialName           =   0x0200,     

    
    evReservedMask          =   0x0400,
    evRTSpecialName         =   0x0400,     
} CorEventAttr;








typedef enum CorMethodSemanticsAttr
{
    msSetter    =   0x0001,     
    msGetter    =   0x0002,     
    msOther     =   0x0004,     
    msAddOn     =   0x0008,     
    msRemoveOn  =   0x0010,     
    msFire      =   0x0020,     
} CorMethodSemanticsAttr;











typedef enum CorDeclSecurity
{
    dclActionMask               =   0x001f,     
    dclActionNil                =   0x0000,     
    dclRequest                  =   0x0001,     
    dclDemand                   =   0x0002,     
    dclAssert                   =   0x0003,     
    dclDeny                     =   0x0004,     
    dclPermitOnly               =   0x0005,     
    dclLinktimeCheck            =   0x0006,     
    dclInheritanceCheck         =   0x0007,     
    dclRequestMinimum           =   0x0008,     
    dclRequestOptional          =   0x0009,     
    dclRequestRefuse            =   0x000a,     
    dclPrejitGrant              =   0x000b,     
    dclPrejitDenied             =   0x000c,     
    dclNonCasDemand             =   0x000d,     
    dclNonCasLinkDemand         =   0x000e,     
    dclNonCasInheritance        =   0x000f,     
    dclMaximumValue             =   0x000f,     
} CorDeclSecurity;









typedef enum CorMethodImpl
{
    
    miCodeTypeMask      =   0x0003,   
    miIL                =   0x0000,   
    miNative            =   0x0001,   
    miOPTIL             =   0x0002,   
    miRuntime           =   0x0003,   
    

    
    miManagedMask       =   0x0004,   
    miUnmanaged         =   0x0004,   
    miManaged           =   0x0000,   
    

    
    miForwardRef        =   0x0010,   
    miPreserveSig       =   0x0080,   

    miInternalCall      =   0x1000,   

    miSynchronized      =   0x0020,   
    miNoInlining        =   0x0008,   
    miMaxMethodImplVal  =   0xffff,   
} CorMethodImpl;




















typedef enum  CorPinvokeMap
{
    pmNoMangle          = 0x0001,   

    
    pmCharSetMask       = 0x0006,
    pmCharSetNotSpec    = 0x0000,
    pmCharSetAnsi       = 0x0002,
    pmCharSetUnicode    = 0x0004,
    pmCharSetAuto       = 0x0006,


    pmBestFitUseAssem   = 0x0000,
    pmBestFitEnabled    = 0x0010,
    pmBestFitDisabled   = 0x0020,
    pmBestFitMask       = 0x0030,

    pmThrowOnUnmappableCharUseAssem   = 0x0000,
    pmThrowOnUnmappableCharEnabled    = 0x1000,
    pmThrowOnUnmappableCharDisabled   = 0x2000,
    pmThrowOnUnmappableCharMask       = 0x3000,

    pmSupportsLastError = 0x0040,   

    
    pmCallConvMask      = 0x0700,
    pmCallConvWinapi    = 0x0100,   
    pmCallConvCdecl     = 0x0200,
    pmCallConvStdcall   = 0x0300,
    pmCallConvThiscall  = 0x0400,   
    pmCallConvFastcall  = 0x0500,

    pmMaxValue          = 0xFFFF,
} CorPinvokeMap;


























typedef enum CorAssemblyFlags
{
    afPublicKey             =   0x0001,     
    
    afPA_None               =   0x0000,     
    afPA_MSIL               =   0x0010,     
    afPA_x86                =   0x0020,     
    afPA_IA64               =   0x0030,     
    afPA_AMD64              =   0x0040,     
    afPA_Specified          =   0x0080,     
    afPA_Mask               =   0x0070,     
    afPA_FullMask           =   0x00F0,     
    afPA_Shift              =   0x0004,     

    afEnableJITcompileTracking  =   0x8000, 
    afDisableJITcompileOptimizer=   0x4000, 

    afRetargetable          =   0x0100,     
                                            
} CorAssemblyFlags;























typedef enum CorManifestResourceFlags
{
    mrVisibilityMask        =   0x0007,
    mrPublic                =   0x0001,     
    mrPrivate               =   0x0002,     
} CorManifestResourceFlags;







typedef enum CorFileFlags
{
    ffContainsMetaData      =   0x0000,     
    ffContainsNoMetaData    =   0x0001,     
} CorFileFlags;






typedef enum CorPEKind
{
    peNot       = 0x00000000,   
    peILonly    = 0x00000001,   
    pe32BitRequired=0x00000002,  
    pe32Plus    = 0x00000004,   
    pe32Unmanaged=0x00000008    
} CorPEKind;



typedef enum CorGenericParamAttr
{
    
    
    gpVarianceMask          =   0x0003,
    gpNonVariant            =   0x0000, 
    gpCovariant             =   0x0001,
    gpContravariant         =   0x0002,

    
    gpSpecialConstraintMask =  0x001C,
    gpNoSpecialConstraint   =   0x0000,      
    gpReferenceTypeConstraint = 0x0004,      
    gpNotNullableValueTypeConstraint   =   0x0008,      
    gpDefaultConstructorConstraint = 0x0010, 
} CorGenericParamAttr;


typedef unsigned __int8 COR_SIGNATURE;

typedef COR_SIGNATURE* PCOR_SIGNATURE;      
                                            
typedef const COR_SIGNATURE* PCCOR_SIGNATURE;


typedef const char * MDUTF8CSTR;
typedef char * MDUTF8STR;







typedef enum CorElementType
{
    ELEMENT_TYPE_END            = 0x0,
    ELEMENT_TYPE_VOID           = 0x1,
    ELEMENT_TYPE_BOOLEAN        = 0x2,
    ELEMENT_TYPE_CHAR           = 0x3,
    ELEMENT_TYPE_I1             = 0x4,
    ELEMENT_TYPE_U1             = 0x5,
    ELEMENT_TYPE_I2             = 0x6,
    ELEMENT_TYPE_U2             = 0x7,
    ELEMENT_TYPE_I4             = 0x8,
    ELEMENT_TYPE_U4             = 0x9,
    ELEMENT_TYPE_I8             = 0xa,
    ELEMENT_TYPE_U8             = 0xb,
    ELEMENT_TYPE_R4             = 0xc,
    ELEMENT_TYPE_R8             = 0xd,
    ELEMENT_TYPE_STRING         = 0xe,

    
    ELEMENT_TYPE_PTR            = 0xf,      
    ELEMENT_TYPE_BYREF          = 0x10,     

    
    ELEMENT_TYPE_VALUETYPE      = 0x11,     
    ELEMENT_TYPE_CLASS          = 0x12,     
    ELEMENT_TYPE_VAR            = 0x13,     
    ELEMENT_TYPE_ARRAY          = 0x14,     
    ELEMENT_TYPE_GENERICINST    = 0x15,     
    ELEMENT_TYPE_TYPEDBYREF     = 0x16,     

    ELEMENT_TYPE_I              = 0x18,     
    ELEMENT_TYPE_U              = 0x19,     
    ELEMENT_TYPE_FNPTR          = 0x1B,     
    ELEMENT_TYPE_OBJECT         = 0x1C,     
    ELEMENT_TYPE_SZARRAY        = 0x1D,     
                                            
    ELEMENT_TYPE_MVAR           = 0x1e,     

    
    ELEMENT_TYPE_CMOD_REQD      = 0x1F,     
    ELEMENT_TYPE_CMOD_OPT       = 0x20,     

    
    ELEMENT_TYPE_INTERNAL       = 0x21,     

    
    ELEMENT_TYPE_MAX            = 0x22,     


    ELEMENT_TYPE_MODIFIER       = 0x40,
    ELEMENT_TYPE_SENTINEL       = 0x01 | ELEMENT_TYPE_MODIFIER, 
    ELEMENT_TYPE_PINNED         = 0x05 | ELEMENT_TYPE_MODIFIER,
    ELEMENT_TYPE_R4_HFA         = 0x06 | ELEMENT_TYPE_MODIFIER, 
    ELEMENT_TYPE_R8_HFA         = 0x07 | ELEMENT_TYPE_MODIFIER, 

} CorElementType;








typedef enum CorSerializationType
{
    SERIALIZATION_TYPE_UNDEFINED    = 0,
    SERIALIZATION_TYPE_BOOLEAN      = ELEMENT_TYPE_BOOLEAN,
    SERIALIZATION_TYPE_CHAR         = ELEMENT_TYPE_CHAR,
    SERIALIZATION_TYPE_I1           = ELEMENT_TYPE_I1,
    SERIALIZATION_TYPE_U1           = ELEMENT_TYPE_U1,
    SERIALIZATION_TYPE_I2           = ELEMENT_TYPE_I2,
    SERIALIZATION_TYPE_U2           = ELEMENT_TYPE_U2,
    SERIALIZATION_TYPE_I4           = ELEMENT_TYPE_I4,
    SERIALIZATION_TYPE_U4           = ELEMENT_TYPE_U4,
    SERIALIZATION_TYPE_I8           = ELEMENT_TYPE_I8,
    SERIALIZATION_TYPE_U8           = ELEMENT_TYPE_U8,
    SERIALIZATION_TYPE_R4           = ELEMENT_TYPE_R4,
    SERIALIZATION_TYPE_R8           = ELEMENT_TYPE_R8,
    SERIALIZATION_TYPE_STRING       = ELEMENT_TYPE_STRING,
    SERIALIZATION_TYPE_SZARRAY      = ELEMENT_TYPE_SZARRAY, 
    SERIALIZATION_TYPE_TYPE         = 0x50,
    SERIALIZATION_TYPE_TAGGED_OBJECT= 0x51,
    SERIALIZATION_TYPE_FIELD        = 0x53,
    SERIALIZATION_TYPE_PROPERTY     = 0x54,
    SERIALIZATION_TYPE_ENUM         = 0x55
} CorSerializationType;






typedef enum CorCallingConvention
{
    IMAGE_CEE_CS_CALLCONV_DEFAULT       = 0x0,

    IMAGE_CEE_CS_CALLCONV_VARARG        = 0x5,
    IMAGE_CEE_CS_CALLCONV_FIELD         = 0x6,
    IMAGE_CEE_CS_CALLCONV_LOCAL_SIG     = 0x7,
    IMAGE_CEE_CS_CALLCONV_PROPERTY      = 0x8,
    IMAGE_CEE_CS_CALLCONV_UNMGD         = 0x9,
    IMAGE_CEE_CS_CALLCONV_GENERICINST   = 0xa,  
    IMAGE_CEE_CS_CALLCONV_NATIVEVARARG  = 0xb,  
    IMAGE_CEE_CS_CALLCONV_MAX           = 0xc,  


        
    IMAGE_CEE_CS_CALLCONV_MASK      = 0x0f,  
    IMAGE_CEE_CS_CALLCONV_HASTHIS   = 0x20,  
    IMAGE_CEE_CS_CALLCONV_EXPLICITTHIS = 0x40,  
    IMAGE_CEE_CS_CALLCONV_GENERIC   = 0x10,  
} CorCallingConvention;



typedef enum CorUnmanagedCallingConvention
{
    IMAGE_CEE_UNMANAGED_CALLCONV_C         = 0x1,
    IMAGE_CEE_UNMANAGED_CALLCONV_STDCALL   = 0x2,
    IMAGE_CEE_UNMANAGED_CALLCONV_THISCALL  = 0x3,
    IMAGE_CEE_UNMANAGED_CALLCONV_FASTCALL  = 0x4,

    IMAGE_CEE_CS_CALLCONV_C         = IMAGE_CEE_UNMANAGED_CALLCONV_C,
    IMAGE_CEE_CS_CALLCONV_STDCALL   = IMAGE_CEE_UNMANAGED_CALLCONV_STDCALL,
    IMAGE_CEE_CS_CALLCONV_THISCALL  = IMAGE_CEE_UNMANAGED_CALLCONV_THISCALL,
    IMAGE_CEE_CS_CALLCONV_FASTCALL  = IMAGE_CEE_UNMANAGED_CALLCONV_FASTCALL,

} CorUnmanagedCallingConvention;


typedef enum CorArgType
{
    IMAGE_CEE_CS_END        = 0x0,
    IMAGE_CEE_CS_VOID       = 0x1,
    IMAGE_CEE_CS_I4         = 0x2,
    IMAGE_CEE_CS_I8         = 0x3,
    IMAGE_CEE_CS_R4         = 0x4,
    IMAGE_CEE_CS_R8         = 0x5,
    IMAGE_CEE_CS_PTR        = 0x6,
    IMAGE_CEE_CS_OBJECT     = 0x7,
    IMAGE_CEE_CS_STRUCT4    = 0x8,
    IMAGE_CEE_CS_STRUCT32   = 0x9,
    IMAGE_CEE_CS_BYVALUE    = 0xA,
} CorArgType;








typedef enum CorNativeType
{
    NATIVE_TYPE_END         = 0x0,    
    NATIVE_TYPE_VOID        = 0x1,    
    NATIVE_TYPE_BOOLEAN     = 0x2,    
    NATIVE_TYPE_I1          = 0x3,
    NATIVE_TYPE_U1          = 0x4,
    NATIVE_TYPE_I2          = 0x5,
    NATIVE_TYPE_U2          = 0x6,
    NATIVE_TYPE_I4          = 0x7,
    NATIVE_TYPE_U4          = 0x8,
    NATIVE_TYPE_I8          = 0x9,
    NATIVE_TYPE_U8          = 0xa,
    NATIVE_TYPE_R4          = 0xb,
    NATIVE_TYPE_R8          = 0xc,
    NATIVE_TYPE_SYSCHAR     = 0xd,    
    NATIVE_TYPE_VARIANT     = 0xe,    
    NATIVE_TYPE_CURRENCY    = 0xf,
    NATIVE_TYPE_PTR         = 0x10,   

    NATIVE_TYPE_DECIMAL     = 0x11,   
    NATIVE_TYPE_DATE        = 0x12,   
    NATIVE_TYPE_BSTR        = 0x13,   
    NATIVE_TYPE_LPSTR       = 0x14,
    NATIVE_TYPE_LPWSTR      = 0x15,
    NATIVE_TYPE_LPTSTR      = 0x16,
    NATIVE_TYPE_FIXEDSYSSTRING  = 0x17,
    NATIVE_TYPE_OBJECTREF   = 0x18,   
    NATIVE_TYPE_IUNKNOWN    = 0x19,   
    NATIVE_TYPE_IDISPATCH   = 0x1a,   
    NATIVE_TYPE_STRUCT      = 0x1b,
    NATIVE_TYPE_INTF        = 0x1c,   
    NATIVE_TYPE_SAFEARRAY   = 0x1d,   
    NATIVE_TYPE_FIXEDARRAY  = 0x1e,
    NATIVE_TYPE_INT         = 0x1f,
    NATIVE_TYPE_UINT        = 0x20,

    NATIVE_TYPE_NESTEDSTRUCT  = 0x21, 

    NATIVE_TYPE_BYVALSTR    = 0x22,   

    NATIVE_TYPE_ANSIBSTR    = 0x23,   

    NATIVE_TYPE_TBSTR       = 0x24, 
                                      

    NATIVE_TYPE_VARIANTBOOL = 0x25, 
                                      
    NATIVE_TYPE_FUNC        = 0x26,

    NATIVE_TYPE_ASANY       = 0x28,

    NATIVE_TYPE_ARRAY       = 0x2a,
    NATIVE_TYPE_LPSTRUCT    = 0x2b,

    NATIVE_TYPE_CUSTOMMARSHALER = 0x2c,  
                                         
                                         
                                         
                                         

    NATIVE_TYPE_ERROR       = 0x2d, 
                                    

    NATIVE_TYPE_MAX         = 0x50, 
} CorNativeType;


enum
{
    DESCR_GROUP_METHODDEF = 0,          
    DESCR_GROUP_METHODIMPL,             
};








typedef enum CorILMethodSect                             
{
    CorILMethod_Sect_Reserved    = 0,
    CorILMethod_Sect_EHTable     = 1,
    CorILMethod_Sect_OptILTable  = 2,

    CorILMethod_Sect_KindMask    = 0x3F,        
    CorILMethod_Sect_FatFormat   = 0x40,        
    CorILMethod_Sect_MoreSects   = 0x80,        
} CorILMethodSect;




typedef struct IMAGE_COR_ILMETHOD_SECT_SMALL
{
    BYTE Kind;
    BYTE DataSize;

} IMAGE_COR_ILMETHOD_SECT_SMALL;





typedef struct IMAGE_COR_ILMETHOD_SECT_FAT
{
    unsigned Kind : 8;
    unsigned DataSize : 24;

} IMAGE_COR_ILMETHOD_SECT_FAT;







typedef enum CorExceptionFlag                       
{
    COR_ILEXCEPTION_CLAUSE_NONE,                    
    COR_ILEXCEPTION_CLAUSE_OFFSETLEN = 0x0000,      
    COR_ILEXCEPTION_CLAUSE_DEPRECATED = 0x0000,     
    COR_ILEXCEPTION_CLAUSE_FILTER  = 0x0001,        
    COR_ILEXCEPTION_CLAUSE_FINALLY = 0x0002,        
    COR_ILEXCEPTION_CLAUSE_FAULT = 0x0004,          
    COR_ILEXCEPTION_CLAUSE_DUPLICATED = 0x0008,     
} CorExceptionFlag;







typedef struct IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT
{
    CorExceptionFlag    Flags;
    DWORD               TryOffset;
    DWORD               TryLength;      
    DWORD               HandlerOffset;
    DWORD               HandlerLength;  
    union {
        DWORD           ClassToken;     
        DWORD           FilterOffset;   
    };
} IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT;

typedef struct IMAGE_COR_ILMETHOD_SECT_EH_FAT
{
    IMAGE_COR_ILMETHOD_SECT_FAT   SectFat;
    IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_FAT Clauses[1];     
} IMAGE_COR_ILMETHOD_SECT_EH_FAT;


typedef struct IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL
{
    CorExceptionFlag    Flags         : 16;
    unsigned            TryOffset     : 16;
    unsigned            TryLength     : 8;  
    unsigned            HandlerOffset : 16;
    unsigned            HandlerLength : 8;  
    union {
        DWORD       ClassToken;
        DWORD       FilterOffset;
    };
} IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL;


typedef struct IMAGE_COR_ILMETHOD_SECT_EH_SMALL
{
    IMAGE_COR_ILMETHOD_SECT_SMALL SectSmall;
    WORD Reserved;
    IMAGE_COR_ILMETHOD_SECT_EH_CLAUSE_SMALL Clauses[1];   
} IMAGE_COR_ILMETHOD_SECT_EH_SMALL;



typedef union IMAGE_COR_ILMETHOD_SECT_EH
{
    IMAGE_COR_ILMETHOD_SECT_EH_SMALL Small;
    IMAGE_COR_ILMETHOD_SECT_EH_FAT Fat;
} IMAGE_COR_ILMETHOD_SECT_EH;



typedef enum CorILMethodFlags
{
    CorILMethod_InitLocals      = 0x0010,           
    CorILMethod_MoreSects       = 0x0008,           

    CorILMethod_CompressedIL    = 0x0040,           

        
    CorILMethod_FormatShift     = 3,
    CorILMethod_FormatMask      = ((1 << CorILMethod_FormatShift) - 1),
    CorILMethod_TinyFormat      = 0x0002,         
    CorILMethod_SmallFormat     = 0x0000,
    CorILMethod_FatFormat       = 0x0003,
    CorILMethod_TinyFormat1     = 0x0006,         
} CorILMethodFlags;



typedef struct IMAGE_COR_ILMETHOD_TINY
{
    BYTE Flags_CodeSize;
} IMAGE_COR_ILMETHOD_TINY;




typedef struct IMAGE_COR_ILMETHOD_FAT
{
    unsigned Flags    : 12;     
    unsigned Size     :  4;     
    unsigned MaxStack : 16;     
    DWORD   CodeSize;           
    mdSignature   LocalVarSigTok;     

} IMAGE_COR_ILMETHOD_FAT;

typedef union IMAGE_COR_ILMETHOD
{
    IMAGE_COR_ILMETHOD_TINY       Tiny;
    IMAGE_COR_ILMETHOD_FAT        Fat;
} IMAGE_COR_ILMETHOD;





typedef struct IMAGE_COR_NATIVE_DESCRIPTOR
{
    DWORD       GCInfo;
    DWORD       EHInfo;
} IMAGE_COR_NATIVE_DESCRIPTOR;


typedef struct IMAGE_COR_X86_RUNTIME_FUNCTION_ENTRY
{
    ULONG       BeginAddress;           
    ULONG       EndAddress;             
    ULONG       MIH;                    
} IMAGE_COR_X86_RUNTIME_FUNCTION_ENTRY;

typedef struct IMAGE_COR_MIH_ENTRY
{
    ULONG   EHRVA;
    ULONG   MethodRVA;
    mdToken Token;
    BYTE    Flags;
    BYTE    CodeManager;
    BYTE    MIHData[0];
} IMAGE_COR_MIH_ENTRY;










typedef struct IMAGE_COR_VTABLEFIXUP
{
    ULONG       RVA;                    
    USHORT      Count;                  
    USHORT      Type;                   
} IMAGE_COR_VTABLEFIXUP;




















typedef enum CorCheckDuplicatesFor
{
    MDDupAll                    = 0xffffffff,
    MDDupENC                    = MDDupAll,
    MDNoDupChecks               = 0x00000000,
    MDDupTypeDef                = 0x00000001,
    MDDupInterfaceImpl          = 0x00000002,
    MDDupMethodDef              = 0x00000004,
    MDDupTypeRef                = 0x00000008,
    MDDupMemberRef              = 0x00000010,
    MDDupCustomAttribute        = 0x00000020,
    MDDupParamDef               = 0x00000040,
    MDDupPermission             = 0x00000080,
    MDDupProperty               = 0x00000100,
    MDDupEvent                  = 0x00000200,
    MDDupFieldDef               = 0x00000400,
    MDDupSignature              = 0x00000800,
    MDDupModuleRef              = 0x00001000,
    MDDupTypeSpec               = 0x00002000,
    MDDupImplMap                = 0x00004000,
    MDDupAssemblyRef            = 0x00008000,
    MDDupFile                   = 0x00010000,
    MDDupExportedType           = 0x00020000,
    MDDupManifestResource       = 0x00040000,
    MDDupGenericParam           = 0x00080000,
    MDDupMethodSpec             = 0x00100000,
    MDDupGenericParamConstraint = 0x00200000,
    
    MDDupAssembly               = 0x10000000,

    
    MDDupDefault = MDNoDupChecks | MDDupTypeRef | MDDupMemberRef | MDDupSignature | MDDupTypeSpec | MDDupMethodSpec,
} CorCheckDuplicatesFor;


typedef enum CorRefToDefCheck
{
    
    MDRefToDefDefault           = 0x00000003,
    MDRefToDefAll               = 0xffffffff,
    MDRefToDefNone              = 0x00000000,
    MDTypeRefToDef              = 0x00000001,
    MDMemberRefToDef            = 0x00000002
} CorRefToDefCheck;



typedef enum CorNotificationForTokenMovement
{
    
    MDNotifyDefault             = 0x0000000f,
    MDNotifyAll                 = 0xffffffff,
    MDNotifyNone                = 0x00000000,
    MDNotifyMethodDef           = 0x00000001,
    MDNotifyMemberRef           = 0x00000002,
    MDNotifyFieldDef            = 0x00000004,
    MDNotifyTypeRef             = 0x00000008,

    MDNotifyTypeDef             = 0x00000010,
    MDNotifyParamDef            = 0x00000020,
    MDNotifyInterfaceImpl       = 0x00000040,
    MDNotifyProperty            = 0x00000080,
    MDNotifyEvent               = 0x00000100,
    MDNotifySignature           = 0x00000200,
    MDNotifyTypeSpec            = 0x00000400,
    MDNotifyCustomAttribute     = 0x00000800,
    MDNotifySecurityValue       = 0x00001000,
    MDNotifyPermission          = 0x00002000,
    MDNotifyModuleRef           = 0x00004000,

    MDNotifyNameSpace           = 0x00008000,

    MDNotifyAssemblyRef         = 0x01000000,
    MDNotifyFile                = 0x02000000,
    MDNotifyExportedType        = 0x04000000,
    MDNotifyResource            = 0x08000000,
} CorNotificationForTokenMovement;


typedef enum CorSetENC
{
    MDSetENCOn                  = 0x00000001,   
    MDSetENCOff                 = 0x00000002,   

    MDUpdateENC                 = 0x00000001,   
    MDUpdateFull                = 0x00000002,   
    MDUpdateExtension           = 0x00000003,   
    MDUpdateIncremental         = 0x00000004,   
    MDUpdateDelta               = 0x00000005,   
    MDUpdateMask                = 0x00000007,


} CorSetENC;




typedef enum CorErrorIfEmitOutOfOrder
{
    MDErrorOutOfOrderDefault    = 0x00000000,   
    MDErrorOutOfOrderNone       = 0x00000000,   
    MDErrorOutOfOrderAll        = 0xffffffff,   
    MDMethodOutOfOrder          = 0x00000001,   
    MDFieldOutOfOrder           = 0x00000002,   
    MDParamOutOfOrder           = 0x00000004,   
    MDPropertyOutOfOrder        = 0x00000008,   
    MDEventOutOfOrder           = 0x00000010,   
} CorErrorIfEmitOutOfOrder;



typedef enum CorImportOptions
{
    MDImportOptionDefault       = 0x00000000,   
    MDImportOptionAll           = 0xFFFFFFFF,   
    MDImportOptionAllTypeDefs   = 0x00000001,   
    MDImportOptionAllMethodDefs = 0x00000002,   
    MDImportOptionAllFieldDefs  = 0x00000004,   
    MDImportOptionAllProperties = 0x00000008,   
    MDImportOptionAllEvents     = 0x00000010,   
    MDImportOptionAllCustomAttributes = 0x00000020, 
    MDImportOptionAllExportedTypes  = 0x00000040,   

} CorImportOptions;



typedef enum CorThreadSafetyOptions
{
    
    
    MDThreadSafetyDefault       = 0x00000000,
    MDThreadSafetyOff           = 0x00000000,
    MDThreadSafetyOn            = 0x00000001,
} CorThreadSafetyOptions;



typedef enum CorLinkerOptions
{
    
    MDAssembly          = 0x00000000,
    MDNetModule         = 0x00000001,
} CorLinkerOptions;


typedef enum MergeFlags
{
    MergeFlagsNone      =   0,
    MergeManifest       =   0x00000001,     
    DropMemberRefCAs    =   0x00000002,
    NoDupCheck          =   0x00000004,
    MergeExportedTypes  =   0x00000008
} MergeFlags;










typedef struct COR_FIELD_OFFSET
{
    mdFieldDef  ridOfField;
    ULONG       ulOffset;
} COR_FIELD_OFFSET;

#line 1365 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"

typedef struct IMAGE_COR_FIXUPENTRY
{
    ULONG ulRVA;
    ULONG Count;
} IMAGE_COR_FIXUPENTRY;





typedef enum CorTokenType
{
    mdtModule               = 0x00000000,       
    mdtTypeRef              = 0x01000000,       
    mdtTypeDef              = 0x02000000,       
    mdtFieldDef             = 0x04000000,       
    mdtMethodDef            = 0x06000000,       
    mdtParamDef             = 0x08000000,       
    mdtInterfaceImpl        = 0x09000000,       
    mdtMemberRef            = 0x0a000000,       
    mdtCustomAttribute      = 0x0c000000,       
    mdtPermission           = 0x0e000000,       
    mdtSignature            = 0x11000000,       
    mdtEvent                = 0x14000000,       
    mdtProperty             = 0x17000000,       
    mdtModuleRef            = 0x1a000000,       
    mdtTypeSpec             = 0x1b000000,       
    mdtAssembly             = 0x20000000,       
    mdtAssemblyRef          = 0x23000000,       
    mdtFile                 = 0x26000000,       
    mdtExportedType         = 0x27000000,       
    mdtManifestResource     = 0x28000000,       
    mdtGenericParam         = 0x2a000000,       
    mdtMethodSpec           = 0x2b000000,       
    mdtGenericParamConstraint = 0x2c000000,

    mdtString               = 0x70000000,       
    mdtName                 = 0x71000000,       
    mdtBaseType             = 0x72000000,       
} CorTokenType;












































typedef enum CorOpenFlags
{
    ofRead              =   0x00000000,     
    ofWrite             =   0x00000001,     
    ofReadWriteMask     =   0x00000001,     

    ofCopyMemory        =   0x00000002,     

    ofManifestMetadata  =   0x00000008,     
    ofReadOnly          =   0x00000010,     
    ofTakeOwnership     =   0x00000020,     

    
    ofCacheImage        =   0x00000004,     
    ofNoTypeLib         =   0x00000080,     

    
    ofReserved1         =   0x00000100,     
    ofReserved2         =   0x00000200,     
    ofReserved          =   0xffffff40      

} CorOpenFlags;












typedef CorTypeAttr CorRegTypeAttr;




typedef void *HCORENUM;



typedef enum CorAttributeTargets
{
    catAssembly      = 0x0001,
    catModule        = 0x0002,
    catClass         = 0x0004,
    catStruct        = 0x0008,
    catEnum          = 0x0010,
    catConstructor   = 0x0020,
    catMethod        = 0x0040,
    catProperty      = 0x0080,
    catField         = 0x0100,
    catEvent         = 0x0200,
    catInterface     = 0x0400,
    catParameter     = 0x0800,
    catDelegate      = 0x1000,
    catGenericParameter = 0x4000,

    catAll           = catAssembly | catModule | catClass | catStruct | catEnum | catConstructor |
                    catMethod | catProperty | catField | catEvent | catInterface | catParameter | catDelegate | catGenericParameter,
    catClassMembers  = catClass | catStruct | catEnum | catConstructor | catMethod | catProperty | catField | catEvent | catDelegate | catInterface,

} CorAttributeTargets;






  
#line 1523 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"












































































































































































typedef enum CompilationRelaxationsEnum
{
    CompilationRelaxations_NoStringInterning       = 0x0008,
        
} CompilationRelaxationEnum;












typedef enum NGenHintEnum
{    
    NGenDefault             = 0x0000, 

    NGenEager               = 0x0001, 
    NGenLazy                = 0x0002, 
    NGenNever               = 0x0003  
};

typedef enum LoadHintEnum
{
    LoadDefault             = 0x0000, 

    LoadAlways              = 0x0001, 
    LoadSometimes           = 0x0002, 
    LoadNever               = 0x0003  
};


















#line 1748 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"






typedef enum CorSaveSize
{
    cssAccurate             = 0x0000,               
    cssQuick                = 0x0001,               
    cssDiscardTransientCAs  = 0x0002,               
} CorSaveSize;
#line 1761 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"









typedef enum NativeTypeArrayFlags
{
    ntaSizeParamIndexSpecified = 0x0001,
    ntaReserved                = 0xfffe      
};




typedef void  *  PSECURITY_PROPS ;
typedef void  *  PSECURITY_VALUE ;
typedef void ** PPSECURITY_PROPS ;
typedef void ** PPSECURITY_VALUE ;






typedef struct COR_SECATTR {
    mdMemberRef     tkCtor;         
    const void      *pCustomAttribute;  
    ULONG           cbCustomAttribute;  
} COR_SECATTR;

#line 1796 "c:\\sscli20\\clr\\src\\inc\\corhdr.h"


#line 1592 "c:\\sscli20\\clr\\src\\inc\\daccess.h"
#line 1 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



















 



#pragma warning( disable: 4049 )  
#line 26 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"





#line 32 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"

#line 1 "c:\\sscli20\\palrt\\inc\\rpc.h"























#line 1 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"












































#line 46 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"


























































































#line 1 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

































































typedef unsigned short wchar_t;
#line 68 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

#line 70 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

#line 1 "c:\\sscli20\\pal\\inc\\pal_error.h"















































































































































































































#line 209 "c:\\sscli20\\pal\\inc\\pal_error.h"
#line 72 "c:\\sscli20\\pal\\inc\\rotor_pal.h"
#line 1 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"



































#line 37 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"


























































#line 96 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"

#line 98 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"







#line 106 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"


















#line 125 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"








#line 134 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"



#line 138 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"



























#line 166 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"










































typedef void VOID;


typedef long LONG;
typedef unsigned long ULONG;



#line 217 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"

typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
typedef ULONGLONG DWORD64;
typedef DWORD64 *PDWORD64;
typedef LONGLONG *PLONG64;
typedef ULONGLONG *PULONG64;
typedef ULONG *PULONG;
typedef short SHORT;
typedef SHORT *PSHORT;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
typedef ULONGLONG DWORDLONG;


typedef unsigned long DWORD;


#line 239 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"

typedef unsigned int DWORD32, *PDWORD32;

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef float FLOAT;
typedef double DOUBLE;
typedef BOOL *PBOOL;
typedef BOOL *LPBOOL;
typedef BYTE *PBYTE;
typedef BYTE *LPBYTE;
typedef const BYTE *LPCBYTE;
typedef int *PINT;
typedef int *LPINT;
typedef WORD *PWORD;
typedef WORD *LPWORD;
typedef LONG *LPLONG;
typedef LPLONG PLONG;
typedef DWORD *PDWORD;
typedef DWORD *LPDWORD;
typedef void *PVOID;
typedef void *LPVOID;
typedef const void *LPCVOID;
typedef int INT;
typedef unsigned int UINT;
typedef unsigned int *PUINT;
typedef BYTE BOOLEAN;
typedef BOOLEAN *PBOOLEAN;

typedef unsigned __int8 UINT8;
typedef signed __int8 INT8;
typedef unsigned __int16 UINT16;
typedef signed __int16 INT16;
typedef unsigned __int32 UINT32, *PUINT32;
typedef signed __int32 INT32, *PINT32;
typedef unsigned __int64 UINT64, *PUINT64;
typedef signed __int64 INT64, *PINT64;

typedef unsigned __int32 ULONG32, *PULONG32;
typedef signed __int32 LONG32, *PLONG32;
typedef unsigned __int64 ULONG64;
typedef signed __int64 LONG64;





#line 288 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"


typedef __w64 __int32 INT_PTR;
typedef __w64 unsigned __int32 UINT_PTR;



typedef __w64 __int32 LONG_PTR;
typedef __w64 unsigned __int32 ULONG_PTR, *PULONG_PTR;
typedef __w64 unsigned __int32 DWORD_PTR, *PDWORD_PTR;







#line 306 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"


























typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;



#line 338 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"
typedef ULONG_PTR size_t;
#line 340 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"


typedef LONG_PTR ptrdiff_t;







typedef INT_PTR intptr_t;


typedef UINT_PTR uintptr_t;


typedef DWORD LCID;
typedef PDWORD PLCID;
typedef WORD LANGID;
typedef DWORD LCTYPE;

typedef wchar_t WCHAR;
typedef WCHAR *PWCHAR;
typedef WCHAR *LPWCH, *PWCH;
typedef const WCHAR *LPCWCH, *PCWCH;
typedef WCHAR *NWPSTR;
typedef WCHAR *LPWSTR, *PWSTR;

typedef const WCHAR *LPCWSTR, *PCWSTR;

typedef char CHAR;
typedef CHAR *PCHAR;
typedef CHAR *LPCH, *PCH;
typedef const CHAR *LPCCH, *PCCH;
typedef CHAR *NPSTR;
typedef CHAR *LPSTR, *PSTR;
typedef const CHAR *LPCSTR, *PCSTR;





typedef CHAR TCHAR;
typedef CHAR _TCHAR;
#line 385 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"
typedef TCHAR *PTCHAR;
typedef TCHAR *LPTSTR, *PTSTR;
typedef const TCHAR *LPCTSTR;








typedef VOID *HANDLE;
typedef struct __PAL_RemoteHandle__ { HANDLE h; } *RHANDLE;
typedef HANDLE *PHANDLE;
typedef HANDLE *LPHANDLE;



typedef HANDLE HMODULE;
typedef HANDLE HINSTANCE;
typedef HANDLE HGLOBAL;
typedef HANDLE HLOCAL;

typedef LONG HRESULT;
typedef LONG NTSTATUS;

typedef union _LARGE_INTEGER {
    struct {



#line 417 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"
        DWORD LowPart;
        LONG HighPart;
#line 420 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _GUID {
    ULONG   Data1;    
    USHORT  Data2;
    USHORT  Data3;
    UCHAR   Data4[ 8 ];
} GUID;


typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
















#line 453 "c:\\sscli20\\pal\\inc\\pal_mstypes.h"
#line 73 "c:\\sscli20\\pal\\inc\\rotor_pal.h"












#line 86 "c:\\sscli20\\pal\\inc\\rotor_pal.h"




































#line 123 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

#line 125 "c:\\sscli20\\pal\\inc\\rotor_pal.h"





#line 131 "c:\\sscli20\\pal\\inc\\rotor_pal.h"































#line 163 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

typedef char * va_list;









#line 175 "c:\\sscli20\\pal\\inc\\rotor_pal.h"















#line 191 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

#line 193 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

















 
















































#line 260 "c:\\sscli20\\pal\\inc\\rotor_pal.h"
#line 261 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

typedef ULONG64   fpos_t;


typedef __int64 time_t;


#line 269 "c:\\sscli20\\pal\\inc\\rotor_pal.h"


















































































typedef DWORD (__stdcall *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;



__declspec(dllimport)
int
__stdcall
PAL_Initialize(
            int argc,
            char *argv[]);

__declspec(dllimport)
DWORD_PTR
__stdcall
PAL_EntryPoint(
     LPTHREAD_START_ROUTINE lpStartAddress,
     LPVOID lpParameter);

__declspec(dllimport)
void
__stdcall
PAL_Terminate(
          void);

__declspec(dllimport)
void
__stdcall
PAL_InitializeDebug(
          void);



__declspec(dllimport)
BOOL
__stdcall
PAL_GetUserConfigurationDirectoryW(
                    LPWSTR lpDirectoryName,
                    UINT cbDirectoryName);







__declspec(dllimport)
HMODULE
__stdcall
PAL_RegisterLibraryW(
          LPCWSTR lpLibFileName);

__declspec(dllimport)
BOOL
__stdcall
PAL_UnregisterLibraryW(
          HMODULE hLibModule);






__declspec(dllimport)
BOOL
__stdcall
PAL_GetPALDirectoryW(
              LPWSTR lpDirectoryName,
              UINT cchDirectoryName);

__declspec(dllimport)
BOOL
__stdcall
PAL_GetPALDirectoryA(
              LPSTR lpDirectoryName,
              UINT cchDirectoryName);





#line 433 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
PAL_Random(
         BOOL bStrong,
          LPVOID lpBuffer,
         DWORD dwLength);

typedef LPVOID (__stdcall *PAL_POPTIMIZEDTLSGETTER)();

__declspec(dllimport)
PAL_POPTIMIZEDTLSGETTER
__stdcall
PAL_MakeOptimizedTlsGetter(
         DWORD dwTlsIndex);

__declspec(dllimport)
VOID
__stdcall
PAL_FreeOptimizedTlsGetter(
         PAL_POPTIMIZEDTLSGETTER pOptimizedTlsGetter);
























__declspec(dllimport)
LPSTR
__stdcall
CharNextA(
             LPCSTR lpsz);

__declspec(dllimport)
LPSTR
__stdcall
CharNextExA(
         WORD CodePage,
         LPCSTR lpCurrentChar,
         DWORD dwFlags);




#line 497 "c:\\sscli20\\pal\\inc\\rotor_pal.h"


__declspec(dllimport)
int
__cdecl
wsprintfA(
       LPSTR,
       LPCSTR,
      ...);

__declspec(dllimport)
int
__cdecl
wsprintfW(
       LPWSTR,
       LPCWSTR,
      ...);





#line 520 "c:\\sscli20\\pal\\inc\\rotor_pal.h"









































__declspec(dllimport)
int
__stdcall
MessageBoxW(
         LPVOID hWnd,  
         LPCWSTR lpText,
         LPCWSTR lpCaption,
         UINT uType);















typedef
BOOL
(__stdcall *PHANDLER_ROUTINE)(
    DWORD CtrlType
    );

__declspec(dllimport)
BOOL
__stdcall
SetConsoleCtrlHandler(
               PHANDLER_ROUTINE HandlerRoutine,
               BOOL Add);

__declspec(dllimport)
BOOL
__stdcall
GenerateConsoleCtrlEvent(
     DWORD dwCtrlEvent,
     DWORD dwProcessGroupId
    );





__declspec(dllimport)
BOOL
__stdcall
AreFileApisANSI(
        VOID);

typedef struct _SECURITY_ATTRIBUTES {
            DWORD nLength;
            LPVOID lpSecurityDescriptor;
            BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;



































__declspec(dllimport)
HANDLE
__stdcall
CreateFileA(
         LPCSTR lpFileName,
         DWORD dwDesiredAccess,
         DWORD dwShareMode,
         LPSECURITY_ATTRIBUTES lpSecurityAttributes,
         DWORD dwCreationDisposition,
         DWORD dwFlagsAndAttributes,
         HANDLE hTemplateFile);

__declspec(dllimport)
HANDLE
__stdcall
CreateFileW(
         LPCWSTR lpFileName,
         DWORD dwDesiredAccess,
         DWORD dwShareMode,
         LPSECURITY_ATTRIBUTES lpSecurityAttributes,
         DWORD dwCreationDisposition,
         DWORD dwFlagsAndAttributes,
         HANDLE hTemplateFile);





#line 684 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
LockFile(
     HANDLE hFile,
     DWORD dwFileOffsetLow,
     DWORD dwFileOffsetHigh,
     DWORD nNumberOfBytesToLockLow,
     DWORD nNumberOfBytesToLockHigh
    );

__declspec(dllimport)
BOOL
__stdcall
UnlockFile(
     HANDLE hFile,
     DWORD dwFileOffsetLow,
     DWORD dwFileOffsetHigh,
     DWORD nNumberOfBytesToUnlockLow,
     DWORD nNumberOfBytesToUnlockHigh
    );

__declspec(dllimport)
DWORD
__stdcall
SearchPathA(
     LPCSTR lpPath,
     LPCSTR lpFileName,
     LPCSTR lpExtension,
     DWORD nBufferLength,
     LPSTR lpBuffer,
     LPSTR *lpFilePart
    );

__declspec(dllimport)
DWORD
__stdcall
SearchPathW(
     LPCWSTR lpPath,
     LPCWSTR lpFileName,
     LPCWSTR lpExtension,
     DWORD nBufferLength,
     LPWSTR lpBuffer,
     LPWSTR *lpFilePart
    );




#line 735 "c:\\sscli20\\pal\\inc\\rotor_pal.h"


__declspec(dllimport)
BOOL
__stdcall
CopyFileA(
       LPCSTR lpExistingFileName,
       LPCSTR lpNewFileName,
       BOOL bFailIfExists);

__declspec(dllimport)
BOOL
__stdcall
CopyFileW(
       LPCWSTR lpExistingFileName,
       LPCWSTR lpNewFileName,
       BOOL bFailIfExists);





#line 758 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
DeleteFileA(
         LPCSTR lpFileName);

__declspec(dllimport)
BOOL
__stdcall
DeleteFileW(
         LPCWSTR lpFileName);





#line 776 "c:\\sscli20\\pal\\inc\\rotor_pal.h"


__declspec(dllimport)
BOOL
__stdcall
MoveFileA(
      LPCSTR lpExistingFileName,
      LPCSTR lpNewFileName);

__declspec(dllimport)
BOOL
__stdcall
MoveFileW(
      LPCWSTR lpExistingFileName,
      LPCWSTR lpNewFileName);





#line 797 "c:\\sscli20\\pal\\inc\\rotor_pal.h"




__declspec(dllimport)
BOOL
__stdcall
MoveFileExA(
         LPCSTR lpExistingFileName,
         LPCSTR lpNewFileName,
         DWORD dwFlags);

__declspec(dllimport)
BOOL
__stdcall
MoveFileExW(
         LPCWSTR lpExistingFileName,
         LPCWSTR lpNewFileName,
         DWORD dwFlags);





#line 822 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
CreateDirectoryA(
          LPCSTR lpPathName,
          LPSECURITY_ATTRIBUTES lpSecurityAttributes);

__declspec(dllimport)
BOOL
__stdcall
CreateDirectoryW(
          LPCWSTR lpPathName,
          LPSECURITY_ATTRIBUTES lpSecurityAttributes);





#line 842 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
RemoveDirectoryW(
          LPCWSTR lpPathName);

__declspec(dllimport)
BOOL
__stdcall
RemoveDirectoryA(
          LPCSTR lpPathName);





#line 860 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

typedef struct _BY_HANDLE_FILE_INFORMATION {  
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR cFileName[ 260 ];
    CHAR cAlternateFileName[ 14 ];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR cFileName[ 260 ];
    WCHAR cAlternateFileName[ 14 ];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;






typedef WIN32_FIND_DATAA WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAA PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAA LPWIN32_FIND_DATA;
#line 909 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
HANDLE
__stdcall
FindFirstFileA(
            LPCSTR lpFileName,
            LPWIN32_FIND_DATAA lpFindFileData);

__declspec(dllimport)
HANDLE
__stdcall
FindFirstFileW(
            LPCWSTR lpFileName,
            LPWIN32_FIND_DATAW lpFindFileData);





#line 929 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
FindNextFileA(
           HANDLE hFindFile,
           LPWIN32_FIND_DATAA lpFindFileData);

__declspec(dllimport)
BOOL
__stdcall
FindNextFileW(
           HANDLE hFindFile,
           LPWIN32_FIND_DATAW lpFindFileData);





#line 949 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
FindClose(
        HANDLE hFindFile);

__declspec(dllimport)
DWORD
__stdcall
GetFileAttributesA(
            LPCSTR lpFileName);

__declspec(dllimport)
DWORD
__stdcall
GetFileAttributesW(
            LPCWSTR lpFileName);





#line 973 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

typedef enum _GET_FILEEX_INFO_LEVELS {
  GetFileExInfoStandard
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD      dwFileAttributes;
    FILETIME   ftCreationTime;
    FILETIME   ftLastAccessTime;
    FILETIME   ftLastWriteTime;
    DWORD      nFileSizeHigh;
    DWORD      nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

__declspec(dllimport)
BOOL
__stdcall
GetFileAttributesExW(
              LPCWSTR lpFileName,
              GET_FILEEX_INFO_LEVELS fInfoLevelId,
              LPVOID lpFileInformation);





__declspec(dllimport)
BOOL
__stdcall
SetFileAttributesA(
            LPCSTR lpFileName,
            DWORD dwFileAttributes);

__declspec(dllimport)
BOOL
__stdcall
SetFileAttributesW(
            LPCWSTR lpFileName,
            DWORD dwFileAttributes);





#line 1018 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

typedef LPVOID LPOVERLAPPED;  

__declspec(dllimport)
BOOL
__stdcall
WriteFile(
       HANDLE hFile,
       LPCVOID lpBuffer,
       DWORD nNumberOfBytesToWrite,
       LPDWORD lpNumberOfBytesWritten,
       LPOVERLAPPED lpOverlapped);

__declspec(dllimport)
BOOL
__stdcall
ReadFile(
      HANDLE hFile,
      LPVOID lpBuffer,
      DWORD nNumberOfBytesToRead,
      LPDWORD lpNumberOfBytesRead,
      LPOVERLAPPED lpOverlapped);





__declspec(dllimport)
HANDLE
__stdcall
GetStdHandle(
          DWORD nStdHandle);

__declspec(dllimport)
BOOL
__stdcall
SetEndOfFile(
          HANDLE hFile);

__declspec(dllimport)
DWORD
__stdcall
SetFilePointer(
            HANDLE hFile,
            LONG lDistanceToMove,
            PLONG lpDistanceToMoveHigh,
            DWORD dwMoveMethod);

__declspec(dllimport)
DWORD
__stdcall
GetFileSize(
         HANDLE hFile,
         LPDWORD lpFileSizeHigh);

__declspec(dllimport)
BOOL
__stdcall
GetFileInformationByHandle(
         HANDLE hFile,
         BY_HANDLE_FILE_INFORMATION* lpFileInformation);

__declspec(dllimport)
LONG
__stdcall
CompareFileTime(
         const FILETIME *lpFileTime1,
         const FILETIME *lpFileTime2);

__declspec(dllimport)
BOOL
__stdcall
SetFileTime(
         HANDLE hFile,
         const FILETIME *lpCreationTime,
         const FILETIME *lpLastAccessTime,
         const FILETIME *lpLastWriteTime);

__declspec(dllimport)
BOOL
__stdcall
GetFileTime(
         HANDLE hFile,
         LPFILETIME lpCreationTime,
         LPFILETIME lpLastAccessTime,
         LPFILETIME lpLastWriteTime);

__declspec(dllimport)
BOOL
__stdcall
FileTimeToLocalFileTime(
             const FILETIME *lpFileTime,
             LPFILETIME lpLocalFileTime);

__declspec(dllimport)
BOOL
__stdcall
LocalFileTimeToFileTime(
             const FILETIME *lpLocalFileTime,
             LPFILETIME lpFileTime);

__declspec(dllimport)
VOID
__stdcall
GetSystemTimeAsFileTime(
             LPFILETIME lpSystemTimeAsFileTime);

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

__declspec(dllimport)
VOID
__stdcall
GetSystemTime(
           LPSYSTEMTIME lpSystemTime);

__declspec(dllimport)
BOOL
__stdcall
FileTimeToDosDateTime(
     const FILETIME *lpFileTime,
     LPWORD lpFatDate,
     LPWORD lpFatTime
    );

__declspec(dllimport)
BOOL
__stdcall
DosDateTimeToFileTime(
     WORD wFatDate,
     WORD wFatTime,
     LPFILETIME lpFileTime
    );



__declspec(dllimport)
BOOL
__stdcall
FlushFileBuffers(
          HANDLE hFile);







__declspec(dllimport)
DWORD
__stdcall
GetFileType(
         HANDLE hFile);

__declspec(dllimport)
UINT
__stdcall
GetConsoleCP(
         VOID);

__declspec(dllimport)
UINT
__stdcall
GetConsoleOutputCP(
           VOID);

__declspec(dllimport)
DWORD
__stdcall
GetFullPathNameA(
          LPCSTR lpFileName,
          DWORD nBufferLength,
          LPSTR lpBuffer,
          LPSTR *lpFilePart);

__declspec(dllimport)
DWORD
__stdcall
GetFullPathNameW(
          LPCWSTR lpFileName,
          DWORD nBufferLength,
          LPWSTR lpBuffer,
          LPWSTR *lpFilePart);





#line 1215 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
DWORD
__stdcall
GetLongPathNameW(
          LPCWSTR lpszShortPath,
                  LPWSTR lpszLongPath,
          DWORD cchBuffer);





__declspec(dllimport)
DWORD
__stdcall
GetShortPathNameW(
          LPCWSTR lpszLongPath,
                  LPWSTR lpszShortPath,
          DWORD cchBuffer);






__declspec(dllimport)
UINT
__stdcall
GetTempFileNameA(
          LPCSTR lpPathName,
          LPCSTR lpPrefixString,
          UINT uUnique,
          LPSTR lpTempFileName);

__declspec(dllimport)
UINT
__stdcall
GetTempFileNameW(
          LPCWSTR lpPathName,
          LPCWSTR lpPrefixString,
          UINT uUnique,
          LPWSTR lpTempFileName);





#line 1264 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
DWORD
__stdcall
GetTempPathA(
          DWORD nBufferLength,
          LPSTR lpBuffer);

__declspec(dllimport)
DWORD
__stdcall
GetTempPathW(
          DWORD nBufferLength,
          LPWSTR lpBuffer);





#line 1284 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
DWORD
__stdcall
GetCurrentDirectoryA(
              DWORD nBufferLength,
              LPSTR lpBuffer);

__declspec(dllimport)
DWORD
__stdcall
GetCurrentDirectoryW(
              DWORD nBufferLength,
              LPWSTR lpBuffer);





#line 1304 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
SetCurrentDirectoryA(
             LPCSTR lpPathName);

__declspec(dllimport)
BOOL
__stdcall
SetCurrentDirectoryW(
             LPCWSTR lpPathName);






#line 1323 "c:\\sscli20\\pal\\inc\\rotor_pal.h"







__declspec(dllimport)
BOOL
__stdcall
GetUserNameW(
     LPWSTR lpBuffer,      
      LPDWORD nSize );   

__declspec(dllimport)
BOOL
__stdcall
GetComputerNameW(
     LPWSTR lpBuffer,     
      LPDWORD nSize);   






__declspec(dllimport)
HANDLE
__stdcall
CreateSemaphoreA(
          LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
          LONG lInitialCount,
          LONG lMaximumCount,
          LPCSTR lpName);

__declspec(dllimport)
HANDLE
__stdcall
CreateSemaphoreW(
          LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
          LONG lInitialCount,
          LONG lMaximumCount,
          LPCWSTR lpName);





#line 1372 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
ReleaseSemaphore(
          HANDLE hSemaphore,
          LONG lReleaseCount,
          LPLONG lpPreviousCount);

__declspec(dllimport)
HANDLE
__stdcall
CreateEventA(
          LPSECURITY_ATTRIBUTES lpEventAttributes,
          BOOL bManualReset,
          BOOL bInitialState,
          LPCSTR lpName);

__declspec(dllimport)
HANDLE
__stdcall
CreateEventW(
          LPSECURITY_ATTRIBUTES lpEventAttributes,
          BOOL bManualReset,
          BOOL bInitialState,
          LPCWSTR lpName);





#line 1404 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
SetEvent(
      HANDLE hEvent);

__declspec(dllimport)
BOOL
__stdcall
ResetEvent(
        HANDLE hEvent);

__declspec(dllimport)
HANDLE
__stdcall
OpenEventW(
        DWORD dwDesiredAccess,
        BOOL bInheritHandle,
        LPCWSTR lpName);





__declspec(dllimport)
HANDLE
__stdcall
CreateMutexW(
     LPSECURITY_ATTRIBUTES lpMutexAttributes,
     BOOL bInitialOwner,
     LPCWSTR lpName);

__declspec(dllimport)
HANDLE
__stdcall
CreateMutexA(
     LPSECURITY_ATTRIBUTES lpMutexAttributes,
     BOOL bInitialOwner,
     LPCSTR lpName);





#line 1450 "c:\\sscli20\\pal\\inc\\rotor_pal.h"


__declspec(dllimport)
BOOL
__stdcall
ReleaseMutex(
     HANDLE hMutex);

__declspec(dllimport)
DWORD
__stdcall
GetCurrentProcessId(
            VOID);

__declspec(dllimport)
HANDLE
__stdcall
GetCurrentProcess(
          VOID);

__declspec(dllimport)
DWORD
__stdcall
GetCurrentThreadId(
           VOID);

__declspec(dllimport)
HANDLE
__stdcall
GetCurrentThread(
         VOID);




typedef struct _STARTUPINFOW {
    DWORD cb;
    LPWSTR lpReserved_PAL_Undefined;
    LPWSTR lpDesktop_PAL_Undefined;
    LPWSTR lpTitle_PAL_Undefined;
    DWORD dwX_PAL_Undefined;
    DWORD dwY_PAL_Undefined;
    DWORD dwXSize_PAL_Undefined;
    DWORD dwYSize_PAL_Undefined;
    DWORD dwXCountChars_PAL_Undefined;
    DWORD dwYCountChars_PAL_Undefined;
    DWORD dwFillAttribute_PAL_Undefined;
    DWORD dwFlags;
    WORD wShowWindow_PAL_Undefined;
    WORD cbReserved2_PAL_Undefined;
    LPBYTE lpReserved2_PAL_Undefined;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;

typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved_PAL_Undefined;
    LPSTR lpDesktop_PAL_Undefined;
    LPSTR lpTitle_PAL_Undefined;
    DWORD dwX_PAL_Undefined;
    DWORD dwY_PAL_Undefined;
    DWORD dwXSize_PAL_Undefined;
    DWORD dwYSize_PAL_Undefined;
    DWORD dwXCountChars_PAL_Undefined;
    DWORD dwYCountChars_PAL_Undefined;
    DWORD dwFillAttribute_PAL_Undefined;
    DWORD dwFlags;
    WORD wShowWindow_PAL_Undefined;
    WORD cbReserved2_PAL_Undefined;
    LPBYTE lpReserved2_PAL_Undefined;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;





typedef STARTUPINFOA STARTUPINFO;
typedef LPSTARTUPINFOW LPSTARTUPINFO;
#line 1534 "c:\\sscli20\\pal\\inc\\rotor_pal.h"





typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId_PAL_Undefined;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

__declspec(dllimport)
BOOL
__stdcall
CreateProcessA(
            LPCSTR lpApplicationName,
            LPSTR lpCommandLine,
            LPSECURITY_ATTRIBUTES lpProcessAttributes,
            LPSECURITY_ATTRIBUTES lpThreadAttributes,
            BOOL bInheritHandles,
            DWORD dwCreationFlags,
            LPVOID lpEnvironment,
            LPCSTR lpCurrentDirectory,
            LPSTARTUPINFOA lpStartupInfo,
            LPPROCESS_INFORMATION lpProcessInformation);

__declspec(dllimport)
BOOL
__stdcall
CreateProcessW(
            LPCWSTR lpApplicationName,
            LPWSTR lpCommandLine,
            LPSECURITY_ATTRIBUTES lpProcessAttributes,
            LPSECURITY_ATTRIBUTES lpThreadAttributes,
            BOOL bInheritHandles,
            DWORD dwCreationFlags,
            LPVOID lpEnvironment,
            LPCWSTR lpCurrentDirectory,
            LPSTARTUPINFOW lpStartupInfo,
            LPPROCESS_INFORMATION lpProcessInformation);





#line 1581 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
__declspec(noreturn)
VOID
__stdcall
ExitProcess(
         UINT uExitCode);

__declspec(dllimport)
BOOL
__stdcall
TerminateProcess(
          HANDLE hProcess,
          UINT uExitCode);

__declspec(dllimport)
BOOL
__stdcall
GetExitCodeProcess(
            HANDLE hProcess,
            LPDWORD lpExitCode);

__declspec(dllimport)
BOOL
__stdcall
GetProcessTimes(
         HANDLE hProcess,
         LPFILETIME lpCreationTime,
         LPFILETIME lpExitTime,
         LPFILETIME lpKernelTime,
         LPFILETIME lpUserTime);










__declspec(dllimport)
DWORD
__stdcall
WaitForSingleObject(
             HANDLE hHandle,
             DWORD dwMilliseconds);

__declspec(dllimport)
DWORD
__stdcall
WaitForSingleObjectEx(
             HANDLE hHandle,
             DWORD dwMilliseconds,
             BOOL bAlertable);

__declspec(dllimport)
DWORD
__stdcall
WaitForMultipleObjects(
                DWORD nCount,
                const HANDLE *lpHandles,
                BOOL bWaitAll,
                DWORD dwMilliseconds);

__declspec(dllimport)
DWORD
__stdcall
WaitForMultipleObjectsEx(
              DWORD nCount,
              const HANDLE *lpHandles,
              BOOL bWaitAll,
              DWORD dwMilliseconds,
              BOOL bAlertable);

__declspec(dllimport)
RHANDLE
__stdcall
PAL_LocalHandleToRemote(
             HANDLE hLocal);

__declspec(dllimport)
HANDLE
__stdcall
PAL_RemoteHandleToLocal(
             RHANDLE hRemote);





__declspec(dllimport)
BOOL
__stdcall
DuplicateHandle(
         HANDLE hSourceProcessHandle,
         HANDLE hSourceHandle,
         HANDLE hTargetProcessHandle,
         LPHANDLE lpTargetHandle,
         DWORD dwDesiredAccess,
         BOOL bInheritHandle,
         DWORD dwOptions);

__declspec(dllimport)
VOID
__stdcall
Sleep(
       DWORD dwMilliseconds);

__declspec(dllimport)
DWORD
__stdcall
SleepEx(
     DWORD dwMilliseconds,
     BOOL bAlertable);

__declspec(dllimport)
BOOL
__stdcall
SwitchToThread(
    VOID);



__declspec(dllimport)
HANDLE
__stdcall
CreateThread(
          LPSECURITY_ATTRIBUTES lpThreadAttributes,
          DWORD dwStackSize,
          LPTHREAD_START_ROUTINE lpStartAddress,
          LPVOID lpParameter,
          DWORD dwCreationFlags,
          LPDWORD lpThreadId);

__declspec(dllimport)
__declspec(noreturn)
VOID
__stdcall
ExitThread(
        DWORD dwExitCode);

__declspec(dllimport)
DWORD
__stdcall
SuspendThread(
           HANDLE hThread);

__declspec(dllimport)
DWORD
__stdcall
ResumeThread(
          HANDLE hThread);

typedef VOID (__stdcall *PAPCFUNC)(ULONG_PTR dwParam);

__declspec(dllimport)
DWORD
__stdcall
QueueUserAPC(
          PAPCFUNC pfnAPC,
          HANDLE hThread,
          ULONG_PTR dwData);

















typedef struct _FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[80];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT {
    ULONG ContextFlags;

    ULONG   Dr0_PAL_Undefined;
    ULONG   Dr1_PAL_Undefined;
    ULONG   Dr2_PAL_Undefined;
    ULONG   Dr3_PAL_Undefined;
    ULONG   Dr6_PAL_Undefined;
    ULONG   Dr7_PAL_Undefined;

    FLOATING_SAVE_AREA FloatSave;

    ULONG   SegGs_PAL_Undefined;
    ULONG   SegFs_PAL_Undefined;
    ULONG   SegEs_PAL_Undefined;
    ULONG   SegDs_PAL_Undefined;

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

    ULONG   Ebp;
    ULONG   Eip;
    ULONG   SegCs;
    ULONG   EFlags;
    ULONG   Esp;
    ULONG   SegSs;

    UCHAR   ExtendedRegisters[512];

} CONTEXT, *PCONTEXT, *LPCONTEXT;











































































































































#line 1949 "c:\\sscli20\\pal\\inc\\rotor_pal.h"


__declspec(dllimport)
BOOL
__stdcall
GetThreadContext(
          HANDLE hThread,
           LPCONTEXT lpContext);

__declspec(dllimport)
BOOL
__stdcall
SetThreadContext(
          HANDLE hThread,
          const CONTEXT *lpContext);
















__declspec(dllimport)
int
__stdcall
GetThreadPriority(
           HANDLE hThread);

__declspec(dllimport)
BOOL
__stdcall
SetThreadPriority(
           HANDLE hThread,
           int nPriority);



__declspec(dllimport)
DWORD
__stdcall
TlsAlloc(
     VOID);

__declspec(dllimport)
LPVOID
__stdcall
TlsGetValue(
         DWORD dwTlsIndex);

__declspec(dllimport)
BOOL
__stdcall
TlsSetValue(
         DWORD dwTlsIndex,
         LPVOID lpTlsValue);

__declspec(dllimport)
BOOL
__stdcall
TlsFree(
     DWORD dwTlsIndex);


typedef struct _CRITICAL_SECTION {
    PVOID DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;

} CRITICAL_SECTION, *PCRITICAL_SECTION, *LPCRITICAL_SECTION;

__declspec(dllimport) VOID __stdcall EnterCriticalSection(  LPCRITICAL_SECTION lpCriticalSection);
__declspec(dllimport) VOID __stdcall LeaveCriticalSection(  LPCRITICAL_SECTION lpCriticalSection);
__declspec(dllimport) VOID __stdcall InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection);
__declspec(dllimport) VOID __stdcall DeleteCriticalSection(  LPCRITICAL_SECTION lpCriticalSection);
__declspec(dllimport) BOOL __stdcall TryEnterCriticalSection(  LPCRITICAL_SECTION lpCriticalSection);




__declspec(dllimport)
UINT
__stdcall
SetErrorMode(
          UINT uMode);


















__declspec(dllimport)
HANDLE
__stdcall
CreateFileMappingA(
            HANDLE hFile,
            LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
            DWORD flProtect,
            DWORD dwMaximumSizeHigh,
            DWORD dwMaximumSizeLow,
            LPCSTR lpName);

__declspec(dllimport)
HANDLE
__stdcall
CreateFileMappingW(
            HANDLE hFile,
            LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
            DWORD flProtect,
            DWORD dwMaxmimumSizeHigh,
            DWORD dwMaximumSizeLow,
            LPCWSTR lpName);





#line 2090 "c:\\sscli20\\pal\\inc\\rotor_pal.h"










__declspec(dllimport)
HANDLE
__stdcall
OpenFileMappingA(
          DWORD dwDesiredAccess,
          BOOL bInheritHandle,
          LPCSTR lpName);

__declspec(dllimport)
HANDLE
__stdcall
OpenFileMappingW(
          DWORD dwDesiredAccess,
          BOOL bInheritHandle,
          LPCWSTR lpName);





#line 2121 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
LPVOID
__stdcall
MapViewOfFile(
           HANDLE hFileMappingObject,
           DWORD dwDesiredAccess,
           DWORD dwFileOffsetHigh,
           DWORD dwFileOffsetLow,
           SIZE_T dwNumberOfBytesToMap);

__declspec(dllimport)
BOOL
__stdcall
UnmapViewOfFile(
         LPCVOID lpBaseAddress);

__declspec(dllimport)
HMODULE
__stdcall
LoadLibraryA(
          LPCSTR lpLibFileName);

__declspec(dllimport)
HMODULE
__stdcall
LoadLibraryW(
          LPCWSTR lpLibFileName);





#line 2155 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

typedef INT_PTR (__stdcall *FARPROC)();

__declspec(dllimport)
FARPROC
__stdcall
GetProcAddress(
            HMODULE hModule,
            LPCSTR lpProcName);

__declspec(dllimport)
BOOL
__stdcall
FreeLibrary(
          HMODULE hLibModule);

__declspec(dllimport)
__declspec(noreturn)
VOID
__stdcall
FreeLibraryAndExitThread(
              HMODULE hLibModule,
              DWORD dwExitCode);

__declspec(dllimport)
BOOL
__stdcall
DisableThreadLibraryCalls(
     HMODULE hLibModule);

__declspec(dllimport)
DWORD
__stdcall
GetModuleFileNameA(
            HMODULE hModule,
            LPSTR lpFileName,
            DWORD nSize);

__declspec(dllimport)
DWORD
__stdcall
GetModuleFileNameW(
            HMODULE hModule,
            LPWSTR lpFileName,
            DWORD nSize);





#line 2206 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
LPVOID
__stdcall
VirtualAlloc(
          LPVOID lpAddress,
          SIZE_T dwSize,
          DWORD flAllocationType,
          DWORD flProtect);

__declspec(dllimport)
BOOL
__stdcall
VirtualFree(
         LPVOID lpAddress,
         SIZE_T dwSize,
         DWORD dwFreeType);

__declspec(dllimport)
BOOL
__stdcall
VirtualProtect(
            LPVOID lpAddress,
            SIZE_T dwSize,
            DWORD flNewProtect,
            PDWORD lpflOldProtect);

typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase_PAL_Undefined;
    DWORD AllocationProtect;
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

__declspec(dllimport)
SIZE_T
__stdcall
VirtualQuery(
          LPCVOID lpAddress,
          PMEMORY_BASIC_INFORMATION lpBuffer,
          SIZE_T dwLength);

__declspec(dllimport)
BOOL
__stdcall
ReadProcessMemory(
           HANDLE hProcess,
           LPCVOID lpBaseAddress,
           LPVOID lpBuffer,
           SIZE_T nSize,
           SIZE_T * lpNumberOfBytesRead);

__declspec(dllimport)
VOID
__stdcall
RtlMoveMemory(
           PVOID Destination,
           const VOID *Source,
           SIZE_T Length);






__declspec(dllimport)
HANDLE
__stdcall
GetProcessHeap(
           VOID);



__declspec(dllimport)
LPVOID
__stdcall
HeapAlloc(
       HANDLE hHeap,
       DWORD dwFlags,
       SIZE_T dwBytes);

__declspec(dllimport)
LPVOID
__stdcall
HeapReAlloc(
     HANDLE hHeap,
     DWORD dwFlags,
     LPVOID lpMem,
     SIZE_T dwBytes
    );

__declspec(dllimport)
BOOL
__stdcall
HeapFree(
      HANDLE hHeap,
      DWORD dwFlags,
      LPVOID lpMem);





__declspec(dllimport)
HLOCAL
__stdcall
LocalAlloc(
        UINT uFlags,
        SIZE_T uBytes);

__declspec(dllimport)
HLOCAL
__stdcall
LocalFree(
       HLOCAL hMem);

__declspec(dllimport)
BOOL
__stdcall
FlushInstructionCache(
               HANDLE hProcess,
               LPCVOID lpBaseAddress,
               SIZE_T dwSize);

__declspec(dllimport)
BOOL
__stdcall
GetStringTypeExW(
          LCID Locale,
          DWORD dwInfoType,
          LPCWSTR lpSrcStr,
          int cchSrc,
          LPWORD lpCharType);








__declspec(dllimport)
int
__stdcall
CompareStringA(
     LCID     Locale,
     DWORD    dwCmpFlags,
     LPCSTR   lpString1,
     int      cchCount1,
     LPCSTR   lpString2,
     int      cchCount2);

__declspec(dllimport)
int
__stdcall
CompareStringW(
     LCID     Locale,
     DWORD    dwCmpFlags,
     LPCWSTR  lpString1,
     int      cchCount1,
     LPCWSTR  lpString2,
     int      cchCount2);








__declspec(dllimport)
UINT
__stdcall
GetACP(void);

typedef struct _cpinfo {
    UINT MaxCharSize;
    BYTE DefaultChar[2];
    BYTE LeadByte[12];
} CPINFO, *LPCPINFO;

__declspec(dllimport)
BOOL
__stdcall
GetCPInfo(
       UINT CodePage,
       LPCPINFO lpCPInfo);

__declspec(dllimport)
BOOL
__stdcall
IsDBCSLeadByteEx(
          UINT CodePage,
          BYTE TestChar);

__declspec(dllimport)
BOOL
__stdcall
IsValidCodePage(
         UINT CodePage);
        




__declspec(dllimport)
int
__stdcall
MultiByteToWideChar(
             UINT CodePage,
             DWORD dwFlags,
             LPCSTR lpMultiByteStr,
             int cbMultiByte,
             LPWSTR lpWideCharStr,
             int cchWideChar);



__declspec(dllimport)
int
__stdcall
WideCharToMultiByte(
             UINT CodePage,
             DWORD dwFlags,
             LPCWSTR lpWideCharStr,
             int cchWideChar,
             LPSTR lpMultiByteStr,
             int cbMultyByte,
             LPCSTR lpDefaultChar,
             LPBOOL lpUsedDefaultChar);

__declspec(dllimport)
LANGID
__stdcall
GetSystemDefaultLangID(
               void);

__declspec(dllimport)
LANGID
__stdcall
GetUserDefaultLangID(
             void);

__declspec(dllimport)
BOOL
__stdcall
SetThreadLocale(
         LCID Locale);

__declspec(dllimport)
LCID
__stdcall
GetThreadLocale(
        void);






__declspec(dllimport)
int
__stdcall
GetLocaleInfoW(
     LCID     Locale,
     LCTYPE   LCType,
     LPWSTR  lpLCData,
     int      cchData);





__declspec(dllimport)
LCID
__stdcall
GetUserDefaultLCID(
           void);







typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

__declspec(dllimport)
DWORD
__stdcall
GetTimeZoneInformation(
                LPTIME_ZONE_INFORMATION lpTimeZoneInformation);




__declspec(dllimport)
BOOL
__stdcall
IsValidLocale(
           LCID Locale,
           DWORD dwFlags);


typedef DWORD CALID;
typedef DWORD CALTYPE;


















__declspec(dllimport)
int
__stdcall
GetCalendarInfoW(
          LCID Locale,
          CALID Calendar,
          CALTYPE CalType,
          LPWSTR lpCalData,
          int cchData,
          LPDWORD lpValue);







__declspec(dllimport)
int
__stdcall
GetDateFormatW(
            LCID Locale,
            DWORD dwFlags,
            const SYSTEMTIME *lpDate,
            LPCWSTR lpFormat,
            LPWSTR lpDateStr,
            int cchDate);










typedef struct _EXCEPTION_RECORD {
    DWORD ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    DWORD NumberParameters;
    ULONG_PTR ExceptionInformation[15];
} EXCEPTION_RECORD, *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS, *LPEXCEPTION_POINTERS;




typedef struct _RUNTIME_FUNCTION {
    DWORD BeginAddress;
    DWORD EndAddress;
    DWORD UnwindData;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;

__declspec(dllimport)
BOOL
__stdcall
WriteProcessMemory( HANDLE hProcess,
                    LPVOID lpBaseAddress,
                    LPVOID lpBuffer,
                    SIZE_T nSize,
                    SIZE_T * lpNumberOfBytesWritten);
































__declspec(dllimport)
HANDLE
__stdcall
OpenProcess(
     DWORD dwDesiredAccess, 
     BOOL bInheritHandle,
     DWORD dwProcessId
    );

__declspec(dllimport)
VOID
__stdcall
OutputDebugStringA(
            LPCSTR lpOutputString);

__declspec(dllimport)
VOID
__stdcall
OutputDebugStringW(
            LPCWSTR lpOutputStrig);





#line 2667 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
VOID
__stdcall
DebugBreak(
       VOID);

__declspec(dllimport)
LPWSTR
__stdcall
lstrcatW(
       LPWSTR lpString1,
      LPCWSTR lpString2);





__declspec(dllimport)
LPWSTR
__stdcall
lstrcpyW(
      LPWSTR lpString1,
      LPCWSTR lpString2);





__declspec(dllimport)
int
__stdcall
lstrlenA(
      LPCSTR lpString);

__declspec(dllimport)
int
__stdcall
lstrlenW(
      LPCWSTR lpString);





#line 2713 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
LPWSTR
__stdcall
lstrcpynW(
       LPWSTR lpString1,
       LPCWSTR lpString2,
       int iMaxLength);






__declspec(dllimport)
DWORD
__stdcall
GetEnvironmentVariableA(
             LPCSTR lpName,
             LPSTR lpBuffer,
             DWORD nSize);

__declspec(dllimport)
DWORD
__stdcall
GetEnvironmentVariableW(
             LPCWSTR lpName,
             LPWSTR lpBuffer,
             DWORD nSize);





#line 2748 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
SetEnvironmentVariableA(
             LPCSTR lpName,
             LPCSTR lpValue);

__declspec(dllimport)
BOOL
__stdcall
SetEnvironmentVariableW(
             LPCWSTR lpName,
             LPCWSTR lpValue);





#line 2768 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
LPSTR
__stdcall
GetEnvironmentStringsA(
               VOID);

__declspec(dllimport)
LPWSTR
__stdcall
GetEnvironmentStringsW(
               VOID);





#line 2786 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
FreeEnvironmentStringsA(
             LPSTR);

__declspec(dllimport)
BOOL
__stdcall
FreeEnvironmentStringsW(
             LPWSTR);





#line 2804 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
CloseHandle(
          HANDLE hObject);

__declspec(dllimport)
VOID
__stdcall
RaiseException(
            DWORD dwExceptionCode,
            DWORD dwExceptionFlags,
            DWORD nNumberOfArguments,
            const ULONG_PTR *lpArguments);

__declspec(dllimport)
DWORD
__stdcall
GetTickCount(
         VOID);

__declspec(dllimport)
BOOL
__stdcall
QueryPerformanceCounter(
     LARGE_INTEGER *lpPerformanceCount
    );

__declspec(dllimport)
BOOL
__stdcall
QueryPerformanceFrequency(
     LARGE_INTEGER *lpFrequency
    );


typedef LONG (__stdcall *PTOP_LEVEL_EXCEPTION_FILTER)(
                           struct _EXCEPTION_POINTERS *ExceptionInfo);
typedef PTOP_LEVEL_EXCEPTION_FILTER LPTOP_LEVEL_EXCEPTION_FILTER;

__declspec(dllimport)
LPTOP_LEVEL_EXCEPTION_FILTER
__stdcall
SetUnhandledExceptionFilter(
                 LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

__declspec(dllimport)
LONG
__stdcall
InterlockedIncrement(
               LONG volatile *lpAddend);

__declspec(dllimport)
LONG
__stdcall
InterlockedDecrement(
               LONG volatile *lpAddend);

__declspec(dllimport)
LONG
__stdcall
InterlockedExchange(
              LONG volatile *Target,
             LONG Value);

__declspec(dllimport)
LONG
__stdcall
InterlockedCompareExchange(
                 LONG volatile *Destination,
                LONG Exchange,
                LONG Comperand);










__declspec(dllimport)
VOID
__stdcall
MemoryBarrier(
    VOID);

__declspec(dllimport)
VOID
__stdcall
YieldProcessor(
    VOID);

__declspec(dllimport)
BOOL
__stdcall
IsBadReadPtr(
          const VOID *lp,
              UINT_PTR ucb);

__declspec(dllimport)
BOOL
__stdcall
IsBadWritePtr(
           LPVOID lp,
           UINT_PTR ucb);

__declspec(dllimport)
BOOL
__stdcall
IsBadCodePtr(
           FARPROC lp);







__declspec(dllimport)
DWORD
__stdcall
FormatMessageW(
            DWORD dwFlags,
            LPCVOID lpSource,
            DWORD dwMessageId,
            DWORD dwLanguageId,
            LPWSTR lpBffer,
            DWORD nSize,
            va_list *Arguments);






__declspec(dllimport)
DWORD
__stdcall
GetLastError(
         VOID);

__declspec(dllimport)
VOID
__stdcall
SetLastError(
          DWORD dwErrCode);

__declspec(dllimport)
LPWSTR
__stdcall
GetCommandLineW(
        VOID);










typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber_PAL_Undefined;
    DWORD dwPlatformId;
    CHAR szCSDVersion_PAL_Undefined[ 128 ];
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;

typedef struct _OSVERSIONINFOW {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber_PAL_Undefined;
    DWORD dwPlatformId;
    WCHAR szCSDVersion_PAL_Undefined[ 128 ];
} OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW;






typedef OSVERSIONINFOA OSVERSIONINFO;
typedef POSVERSIONINFOA POSVERSIONINFO;
typedef LPOSVERSIONINFOA LPOSVERSIONINFO;
#line 2996 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
BOOL
__stdcall
GetVersionExA(
            LPOSVERSIONINFOA lpVersionInformation);

__declspec(dllimport)
BOOL
__stdcall
GetVersionExW(
            LPOSVERSIONINFOW lpVersionInformation);





#line 3014 "c:\\sscli20\\pal\\inc\\rotor_pal.h"



typedef struct _SYSTEM_INFO {
    WORD wProcessorArchitecture_PAL_Undefined;
    WORD wReserved_PAL_Undefined; 
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask_PAL_Undefined;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType_PAL_Undefined;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel_PAL_Undefined;
    WORD wProcessorRevision_PAL_Undefined;
} SYSTEM_INFO, *LPSYSTEM_INFO;

__declspec(dllimport)
VOID
__stdcall
GetSystemInfo(
           LPSYSTEM_INFO lpSystemInfo);

__declspec(dllimport)
BOOL
__stdcall
GetDiskFreeSpaceW(
          LPCWSTR lpDirectoryName,
          LPDWORD lpSectorsPerCluster,
          LPDWORD lpBytesPerSector,
          LPDWORD lpNumberOfFreeClusters,
          LPDWORD lpTotalNumberOfClusters);





__declspec(dllimport)
BOOL
__stdcall
CreatePipe(
     PHANDLE hReadPipe,
     PHANDLE hWritePipe,
     LPSECURITY_ATTRIBUTES lpPipeAttributes,
     DWORD nSize
    );


































































































#line 3159 "c:\\sscli20\\pal\\inc\\rotor_pal.h"
#line 3160 "c:\\sscli20\\pal\\inc\\rotor_pal.h"






__declspec(dllimport) void * __cdecl memcpy(void *, const void *, size_t);
__declspec(dllimport) int    __cdecl memcmp(const void *, const void *, size_t);
__declspec(dllimport) void * __cdecl memset(void *, int, size_t);
__declspec(dllimport) void * __cdecl memmove(void *, const void *, size_t);
__declspec(dllimport) void * __cdecl memchr(const void *, int, size_t);

__declspec(dllimport) size_t __cdecl strlen(const char *);
__declspec(dllimport) int __cdecl strcmp(const char*, const char *);
__declspec(dllimport) int __cdecl strncmp(const char*, const char *, size_t);
__declspec(dllimport) int __cdecl _stricmp(const char *, const char *);
__declspec(dllimport) int __cdecl _strnicmp(const char *, const char *, size_t);
__declspec(dllimport) char * __cdecl strcat(char *, const char *);
__declspec(dllimport) char * __cdecl strncat(char *, const char *, size_t);
__declspec(dllimport) char * __cdecl strcpy(char *, const char *);
__declspec(dllimport) char * __cdecl strncpy(char *, const char *, size_t);
__declspec(dllimport) char * __cdecl strchr(const char *, int);
__declspec(dllimport) char * __cdecl strrchr(const char *, int);
__declspec(dllimport) char * __cdecl strpbrk(const char *, const char *);
__declspec(dllimport) char * __cdecl strstr(const char *, const char *);
__declspec(dllimport) char * __cdecl strtok(char *, const char *);
__declspec(dllimport) size_t __cdecl strspn(const char *, const char *);
__declspec(dllimport) size_t  __cdecl strcspn(const char *, const char *);
__declspec(dllimport) int __cdecl sprintf(char *, const char *, ...);
__declspec(dllimport) int __cdecl vsprintf(char *, const char *, va_list);
__declspec(dllimport) int __cdecl _snprintf(char *, size_t, const char *, ...);
__declspec(dllimport) int __cdecl _vsnprintf(char *, size_t, const char *, va_list);
__declspec(dllimport) int __cdecl sscanf(const char *, const char *, ...);
__declspec(dllimport) char * __cdecl _strlwr(char *);
__declspec(dllimport) int __cdecl atoi(const char *);
__declspec(dllimport) LONG __cdecl atol(const char *);
__declspec(dllimport) ULONG __cdecl strtoul(const char *, char **, int);
__declspec(dllimport) double __cdecl atof(const char *);
__declspec(dllimport) char * __cdecl _gcvt(double, int, char *);
__declspec(dllimport) char * __cdecl _ecvt(double, int, int *, int *);
__declspec(dllimport) double __cdecl strtod(const char *, char **);
__declspec(dllimport) int __cdecl isprint(int);
__declspec(dllimport) int __cdecl isspace(int);
__declspec(dllimport) int __cdecl isalpha(int);
__declspec(dllimport) int __cdecl isalnum(int);
__declspec(dllimport) int __cdecl isdigit(int);
__declspec(dllimport) int __cdecl isxdigit(int);
__declspec(dllimport) int __cdecl isupper(int);
__declspec(dllimport) int __cdecl islower(int);
__declspec(dllimport) int __cdecl __iscsym(int);
__declspec(dllimport) int __cdecl tolower(int);
__declspec(dllimport) int __cdecl toupper(int);

__declspec(dllimport) size_t __cdecl _mbslen(const unsigned char *);
__declspec(dllimport) unsigned char * __cdecl _mbsinc(const unsigned char *);
__declspec(dllimport) unsigned char * __cdecl _mbsninc(const unsigned char *, size_t);
__declspec(dllimport) unsigned char * __cdecl _mbsdec(const unsigned char *, const unsigned char *);

__declspec(dllimport) size_t __cdecl wcslen(const wchar_t *);
__declspec(dllimport) int __cdecl wcscmp(const wchar_t*, const wchar_t*);
__declspec(dllimport) int __cdecl wcsncmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) int __cdecl _wcsicmp(const wchar_t *, const wchar_t*);
__declspec(dllimport) int __cdecl _wcsnicmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcscat(wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsncat(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcscpy(wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsncpy(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcschr(const wchar_t *, wchar_t);
__declspec(dllimport) wchar_t * __cdecl wcsrchr(const wchar_t *, wchar_t);
__declspec(dllimport) wchar_t  * __cdecl wcspbrk(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t  * __cdecl wcsstr(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcstok(wchar_t *, const wchar_t *);
__declspec(dllimport) size_t __cdecl wcscspn(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl vswprintf(wchar_t *, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl _snwprintf(wchar_t *, size_t, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _vsnwprintf(wchar_t *, size_t, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl swscanf(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) wchar_t * __cdecl _wcslwr(wchar_t *);
__declspec(dllimport) LONG __cdecl wcstol(const wchar_t *, wchar_t **, int);
__declspec(dllimport) ULONG __cdecl wcstoul(const wchar_t *, wchar_t **, int);
__declspec(dllimport) wchar_t * __cdecl _itow(int, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _i64tow(__int64, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _ui64tow(unsigned __int64, wchar_t *, int);
__declspec(dllimport) int __cdecl _wtoi(const wchar_t *);
__declspec(dllimport) size_t __cdecl wcsspn (const wchar_t *, const wchar_t *);
__declspec(dllimport) double __cdecl wcstod(const wchar_t *, wchar_t **);
__declspec(dllimport) int __cdecl iswalpha(wchar_t);
__declspec(dllimport) int __cdecl iswprint(wchar_t);
__declspec(dllimport) int __cdecl iswupper(wchar_t);
__declspec(dllimport) int __cdecl iswspace(wchar_t);
__declspec(dllimport) int __cdecl iswdigit(wchar_t);
__declspec(dllimport) int __cdecl iswxdigit(wchar_t);
__declspec(dllimport) wchar_t __cdecl towlower(wchar_t);
__declspec(dllimport) wchar_t __cdecl towupper(wchar_t);















__declspec(dllimport) unsigned int __cdecl _rotl(unsigned int, int);
__declspec(dllimport) unsigned int __cdecl _rotr(unsigned int, int);
__declspec(dllimport) int __cdecl abs(int);
__declspec(dllimport) LONG __cdecl labs(LONG);

__declspec(dllimport) double __cdecl sqrt(double);
__declspec(dllimport) double __cdecl log(double);
__declspec(dllimport) double __cdecl log10(double);
__declspec(dllimport) double __cdecl exp(double);
__declspec(dllimport) double __cdecl pow(double, double);
__declspec(dllimport) double __cdecl acos(double);
__declspec(dllimport) double __cdecl asin(double);
__declspec(dllimport) double __cdecl atan(double);
__declspec(dllimport) double __cdecl atan2(double,double);
__declspec(dllimport) double __cdecl cos(double);
__declspec(dllimport) double __cdecl sin(double);
__declspec(dllimport) double __cdecl tan(double);
__declspec(dllimport) double __cdecl cosh(double);
__declspec(dllimport) double __cdecl sinh(double);
__declspec(dllimport) double __cdecl tanh(double);
__declspec(dllimport) double __cdecl fmod(double, double);
__declspec(dllimport) double __cdecl floor(double);
__declspec(dllimport) double __cdecl ceil(double);
__declspec(dllimport) double __cdecl fabs(double);
__declspec(dllimport) double __cdecl modf(double, double *);

__declspec(dllimport) int __cdecl _finite(double);
__declspec(dllimport) int __cdecl _isnan(double);
__declspec(dllimport) double __cdecl _copysign(double, double);









__declspec(dllimport) void * __cdecl malloc(size_t);
__declspec(dllimport) void   __cdecl free(void *);
__declspec(dllimport) void * __cdecl realloc(void *, size_t);









#line 3321 "c:\\sscli20\\pal\\inc\\rotor_pal.h"


__declspec(dllimport) void * __cdecl _alloca(size_t);
#line 3325 "c:\\sscli20\\pal\\inc\\rotor_pal.h"








#line 3334 "c:\\sscli20\\pal\\inc\\rotor_pal.h"




__declspec(dllimport) __declspec(noreturn) void __cdecl exit(int);
int __cdecl atexit(void (__cdecl *function)(void));

__declspec(dllimport) void __cdecl qsort(void *, size_t, size_t, int (__cdecl *)(const void *, const void *));
__declspec(dllimport) void * __cdecl bsearch(const void *, const void *, size_t, size_t,
int (__cdecl *)(const void *, const void *));

__declspec(dllimport) void __cdecl _splitpath(const char *, char *, char *, char *, char *);
__declspec(dllimport) void __cdecl _wsplitpath(const wchar_t *, wchar_t *, wchar_t *, wchar_t *, wchar_t *);
__declspec(dllimport) void __cdecl _makepath(char *, const char *, const char *, const char *, const char *);
__declspec(dllimport) void __cdecl _wmakepath(wchar_t *, const wchar_t *, const wchar_t *, const wchar_t *, const wchar_t *);
__declspec(dllimport) char * __cdecl _fullpath(char *, const char *, size_t);

__declspec(dllimport) void __cdecl _swab(char *, char *, int);

__declspec(dllimport) time_t __cdecl time(time_t *);

struct tm {
        int tm_sec;     
        int tm_min;     
        int tm_hour;    
        int tm_mday;    
        int tm_mon;     
        int tm_year;    
        int tm_wday;    
        int tm_yday;    
        int tm_isdst;   
        };

__declspec(dllimport) struct tm * __cdecl localtime(const time_t *);
__declspec(dllimport) time_t __cdecl mktime(struct tm *);
__declspec(dllimport) char * __cdecl ctime(const time_t *);

__declspec(dllimport) int __cdecl _open_osfhandle(INT_PTR, int);
__declspec(dllimport) int __cdecl _close(int);

struct _FILE;
typedef struct _FILE FILE;













__declspec(dllimport) int __cdecl fclose(FILE *);
__declspec(dllimport) void __cdecl setbuf(FILE *, char*);
__declspec(dllimport) int __cdecl fflush(FILE *);
__declspec(dllimport) size_t __cdecl fwrite(const void *, size_t, size_t, FILE *);
__declspec(dllimport) size_t __cdecl fread(void *, size_t, size_t, FILE *);
__declspec(dllimport) char * __cdecl fgets(char *, int, FILE *);
__declspec(dllimport) wchar_t * __cdecl fgetws(wchar_t *, int, FILE *);
__declspec(dllimport) int __cdecl fputs(const char *, FILE *);
__declspec(dllimport) int __cdecl fputc(int c, FILE *stream);
__declspec(dllimport) int __cdecl putchar(int c);
__declspec(dllimport) int __cdecl fprintf(FILE *, const char *, ...);
__declspec(dllimport) int __cdecl fwprintf(FILE *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl vfprintf(FILE *, const char *, va_list);
__declspec(dllimport) int __cdecl vfwprintf(FILE *, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl _getw(FILE *);
__declspec(dllimport) int __cdecl _putw(int, FILE *);
__declspec(dllimport) int __cdecl fseek(FILE *, LONG, int);
__declspec(dllimport) int __cdecl fgetpos(FILE *, fpos_t *);
__declspec(dllimport) int __cdecl fsetpos(FILE *, const fpos_t *);
__declspec(dllimport) LONG __cdecl ftell(FILE *);
__declspec(dllimport) int __cdecl feof(FILE *);
__declspec(dllimport) int __cdecl ferror(FILE *);
__declspec(dllimport) FILE * __cdecl fopen(const char *, const char *);
__declspec(dllimport) FILE * __cdecl _fdopen(int, const char *);
__declspec(dllimport) FILE * __cdecl _wfopen(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl getc(FILE *stream);
__declspec(dllimport) int __cdecl fgetc(FILE *stream);
__declspec(dllimport) int __cdecl ungetc(int c, FILE *stream);





__declspec(dllimport) int    __cdecl rand(void);
__declspec(dllimport) void   __cdecl srand(unsigned int);

__declspec(dllimport) int __cdecl printf(const char *, ...);
__declspec(dllimport) int __cdecl vprintf(const char *, va_list);
__declspec(dllimport) int __cdecl wprintf(const wchar_t*, ...);





#line 3434 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport) FILE * __cdecl PAL_get_stdout(int caller);
__declspec(dllimport) FILE * __cdecl PAL_get_stdin(int caller);
__declspec(dllimport) FILE * __cdecl PAL_get_stderr(int caller);
__declspec(dllimport) int * __cdecl PAL_errno(int caller);






__declspec(dllimport) char * __cdecl getenv(const char *);
__declspec(dllimport) int __cdecl _putenv(const char *);



































typedef UINT_PTR        SOCKET;
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;




typedef unsigned int    u_long;
#line 3492 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

struct  hostent {
        char     * h_name;           
        char     *  * h_aliases;  
        short   h_addrtype;             
        short   h_length;               
        char     *  * h_addr_list; 

};

struct sockaddr {
        u_short sa_family;              
        char    sa_data[14];            
};




struct in_addr {
        union {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
    };




struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};




struct  linger {
        u_short l_onoff;                
        u_short l_linger;               
};




struct ip_mreq {
        struct in_addr  imr_multiaddr;  
        struct in_addr  imr_interface;  
};

































































































































































#line 3705 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

                                       
                                       
                                       











__declspec(dllimport) struct hostent  * __stdcall gethostbyname( const char  * name);

__declspec(dllimport) struct hostent  * __stdcall gethostbyaddr(
                                               const char  * addr,
                                               int len,
                                               int type);

__declspec(dllimport) int __stdcall  gethostname (
                             char  * name,
                             int namelen);

__declspec(dllimport) LONG __stdcall inet_addr ( const char  * cp);

__declspec(dllimport) int __stdcall getpeername (
                             SOCKET s,
                             struct sockaddr  *name,
                              int  * namelen);

__declspec(dllimport) int __stdcall getsockopt (
                            SOCKET s,
                            int level,
                            int optname,
                            char  * optval,
                             int  *optlen);

__declspec(dllimport) int __stdcall setsockopt (
                            SOCKET s,
                            int level,
                            int optname,
                            const char  *optval,
                            int optlen);

__declspec(dllimport) int __stdcall connect (
                         SOCKET s,
                         const struct sockaddr  *name,
                         int namelen);









__declspec(dllimport) int __stdcall send (
                      SOCKET s,
                      const char  * buf,
                      int len,
                      int flags);

__declspec(dllimport) int __stdcall recv (
                      SOCKET s,
                      char  * buf,
                      int len,
                      int flags);

__declspec(dllimport) int __stdcall closesocket (  SOCKET s);

__declspec(dllimport) SOCKET __stdcall accept (
                           SOCKET s,
                           struct sockaddr  *addr,
                            int  *addrlen);

__declspec(dllimport) int __stdcall listen (
                        SOCKET s,
                        int backlog);

__declspec(dllimport) int __stdcall bind (
                      SOCKET s,
                      const struct sockaddr  *addr,
                      int namelen);

__declspec(dllimport) int __stdcall shutdown (
                          SOCKET s,
                          int how);

__declspec(dllimport) int __stdcall sendto (
                        SOCKET s,
                        const char  * buf,
                        int len,
                        int flags,
                        const struct sockaddr  *to,
                        int tolen);

__declspec(dllimport) int __stdcall recvfrom (
                          SOCKET s,
                          char  * buf,
                          int len,
                          int flags,
                          struct sockaddr  *from,
                           int  * fromlen);

__declspec(dllimport) int __stdcall getsockname (
                             SOCKET s,
                             struct sockaddr  *name,
                              int  * namelen);


typedef struct fd_set {
        u_int   fd_count;               
        SOCKET  fd_array[64];   
} fd_set;




















struct timeval {
        int    tv_sec;         
        int    tv_usec;        
};
__declspec(dllimport) int __stdcall select (
                         int nfds,
                          fd_set  *readfds,
                          fd_set  *writefds,
                          fd_set  *exceptfds,
                         const struct timeval  *timeout);

__declspec(dllimport) SOCKET __stdcall socket (
                           int af,
                           int type,
                           int protocol);
















typedef struct WSAData {
        WORD                    wVersion;
        WORD                    wHighVersion;
        char                    szDescription_PAL_Undefined[256+1];
        char                    szSystemStatus_PAL_Undefined[128+1];
        unsigned short          iMaxSockets_PAL_Undefined;
        unsigned short          iMaxUdpDg_PAL_Undefined;
        char  *              lpVendorInfo_PAL_Undefined;
} WSADATA,  * LPWSADATA;



typedef struct _WSAPROTOCOLCHAIN {
    int ChainLen;                                 
                                                  
                                                  
                                                  
    DWORD ChainEntries[7];       
} WSAPROTOCOLCHAIN,  * LPWSAPROTOCOLCHAIN;



typedef struct _WSAPROTOCOL_INFOA {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    CHAR   szProtocol[255+1];
} WSAPROTOCOL_INFOA,  * LPWSAPROTOCOL_INFOA;
typedef struct _WSAPROTOCOL_INFOW {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    WCHAR  szProtocol[255+1];
} WSAPROTOCOL_INFOW,  * LPWSAPROTOCOL_INFOW;




typedef WSAPROTOCOL_INFOA WSAPROTOCOL_INFO;
typedef LPWSAPROTOCOL_INFOA LPWSAPROTOCOL_INFO;
#line 3947 "c:\\sscli20\\pal\\inc\\rotor_pal.h"































__declspec(dllimport)
int
__stdcall
WSAStartup(
        WORD wVersionRequired,
        LPWSADATA lpWSAData);

__declspec(dllimport)
int
__stdcall
WSACleanup(
       void);

typedef struct _WSABUF {
    u_long      len;     
    char  *  buf;     
} WSABUF,  * LPWSABUF;

typedef struct _WSAOVERLAPPED
{
    DWORD  Internal;
    DWORD  InternalHigh;
    DWORD  Offset;
    DWORD  OffsetHigh;
    HANDLE hEvent;
} WSAOVERLAPPED, *LPWSAOVERLAPPED;

typedef DWORD_PTR LPWSAOVERLAPPED_COMPLETION_ROUTINE;  

typedef ULONG   SERVICETYPE;
 
typedef struct _flowspec
{
    ULONG       TokenRate;              
    ULONG       TokenBucketSize;        
    ULONG       PeakBandwidth;          
    ULONG       Latency;                
    ULONG       DelayVariation;         
    SERVICETYPE ServiceType;
    ULONG       MaxSduSize;             
    ULONG       MinimumPolicedSize;     

} FLOWSPEC, *PFLOWSPEC, * LPFLOWSPEC;

typedef struct _QualityOfService
{
    FLOWSPEC      SendingFlowspec;       
    FLOWSPEC      ReceivingFlowspec;     
    WSABUF        ProviderSpecific;      
} QOS,  * LPQOS;

__declspec(dllimport)
int 
__stdcall
WSAConnect(
            SOCKET s,
            const struct sockaddr* name,
            int namelen,
            LPWSABUF lpCallerData,
            LPWSABUF lpCalleeData,
            LPQOS lpSQOS,
            LPQOS lpGQOS);

__declspec(dllimport)
int
__stdcall
WSASend(
     SOCKET s,
     LPWSABUF lpBuffers,
     DWORD dwBufferCount,
     LPDWORD lpNumberOfBytesSent,
     DWORD dwFlags,
     LPWSAOVERLAPPED lpOverlapped,
     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

__declspec(dllimport)
int
__stdcall
WSASendTo(
       SOCKET s,
       LPWSABUF lpBuffers,
       DWORD dwBufferCount,
       LPDWORD lpNumberOfBytesSent,
       DWORD dwFlags,
       const struct sockaddr  *lpTo,
       int iTolen,
       LPWSAOVERLAPPED lpOverlapped,
       LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

__declspec(dllimport)
int
__stdcall
WSARecv(
     SOCKET s,
      LPWSABUF lpBuffers,
     DWORD dwBufferCount,
     LPDWORD lpNumberOfBytesRecvd,
      LPDWORD lpFlags,
     LPWSAOVERLAPPED lpOverlapped,
     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

__declspec(dllimport)
int
__stdcall
WSARecvFrom(
         SOCKET s,
          LPWSABUF lpBuffers,
         DWORD dwBufferCount,
         LPDWORD lpNumberOfBytesRecvd,
          LPDWORD lpFlags,
         struct sockaddr  *lpFrom,
          LPINT lpFromLen,
         LPWSAOVERLAPPED lpOverlapped,
         LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

typedef HANDLE WSAEVENT;

__declspec(dllimport)
int
__stdcall
WSAEventSelect(
            SOCKET s,
            WSAEVENT hEventObject,
            int lNetworkEvents);  

typedef unsigned int GROUP;



typedef struct _WSANETWORKEVENTS
{
    int lNetworkEvents;
    int iErrorCode[10];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

__declspec(dllimport)
int
__stdcall
WSAEnumNetworkEvents(
           SOCKET s,
           WSAEVENT hEventObject,
           LPWSANETWORKEVENTS lpNetworkEvents);  

__declspec(dllimport) 
SOCKET 
__stdcall 
WSASocketA(
     int af,
     int type,
     int protocol,
     LPWSAPROTOCOL_INFOA lpProtocolInfo,
     GROUP g,
     DWORD dwFlags);

__declspec(dllimport) 
SOCKET 
__stdcall 
WSASocketW(
     int af,
     int type,
     int protocol,
     LPWSAPROTOCOL_INFOW lpProtocolInfo,
     GROUP g,
     DWORD dwFlags);





#line 4148 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
int
__stdcall
WSAIoctl(
      SOCKET s,
      DWORD dwIoControlCode,
      LPVOID lpvInBuffer,
      DWORD cbInBuffer,
      LPVOID lpvOutBuffer,
      DWORD cbOutBuffer,
      LPDWORD lpcbBytesReturned,
      LPWSAOVERLAPPED lpOverlapped,
      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);






                                        
































struct tcp_keepalive
{
    u_long onoff;
    u_long keepalivetime;
    u_long keepaliveinterval;
};


__declspec(dllimport)
int
__stdcall
ioctlsocket(
         SOCKET s,
         int cmd,  
          u_long  *argp);

__declspec(dllimport)
BOOL
__stdcall
WSAGetOverlappedResult(
     SOCKET s,
     LPWSAOVERLAPPED lpOverlapped,
     LPDWORD lpcbTransfer,
     BOOL fWait,
     LPDWORD lpdwFlags
);

__declspec(dllimport)
HANDLE
__stdcall
PAL_CreateIoCompletionPort(
     HANDLE FileHandle,
     HANDLE ExistingCompletionPort,
     ULONG_PTR CompletionKey,
     DWORD NumberOfConcurrentThreads);

__declspec(dllimport)
BOOL
__stdcall
PostQueuedCompletionStatus(
     HANDLE CompletionPort,
     DWORD dwNumberOfBytesTransferred,
     ULONG_PTR dwCompletionKey,
     LPOVERLAPPED lpOverlapped);

__declspec(dllimport)
BOOL
__stdcall
GetQueuedCompletionStatus(
     HANDLE CompletionPort,
     LPDWORD lpNumberOfBytesTransferred,
     PULONG_PTR lpCompletionKey,
     LPOVERLAPPED *lpOverlapped,
     DWORD dwMilliseconds);



__declspec(dllimport)
BOOL
__stdcall
PAL_GetIOCPThreadIoPendingFlag(VOID);

typedef struct _PAL_IOCP_CPU_INFORMATION {
    union {
        FILETIME ftLastRecordedIdleTime;
        FILETIME ftLastRecordedCurrentTime;
    } LastRecordedTime;
    FILETIME ftLastRecordedKernelTime;
    FILETIME ftLastRecordedUserTime;
} PAL_IOCP_CPU_INFORMATION;

__declspec(dllimport)
INT
__stdcall
PAL_GetCPUBusyTime(
      PAL_IOCP_CPU_INFORMATION *lpPrevCPUInfo);












#line 4290 "c:\\sscli20\\pal\\inc\\rotor_pal.h"




#line 4295 "c:\\sscli20\\pal\\inc\\rotor_pal.h"











typedef LONG (*PFN_PAL_EXCEPTION_FILTER)(
     PEXCEPTION_POINTERS pExceptionPointers,
     LPVOID lpvParam);

typedef enum _TypeOfHandler {
    PALExceptFilter,
    PALExcept,
    PALFinally,
    CatchTyped,
    CatchAll
} TypeOfHandler;

typedef struct _PAL_EXCEPTION_REGISTRATION {
    
    
    struct _PAL_EXCEPTION_REGISTRATION *Next;

    
    
    
    PFN_PAL_EXCEPTION_FILTER Handler;

    
    PVOID pvFilterParameter;

    
    DWORD dwFlags;

    
    TypeOfHandler typeOfHandler;

    
    char ReservedForPAL[0x40];
} PAL_EXCEPTION_REGISTRATION, *PPAL_EXCEPTION_REGISTRATION;


int __cdecl _setjmp(void*);















#line 4359 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

__declspec(dllimport)
void
__stdcall
PAL_TryHelper(
          PPAL_EXCEPTION_REGISTRATION pRegistration);

__declspec(dllimport)
int
__stdcall
PAL_EndTryHelper(
          PPAL_EXCEPTION_REGISTRATION pRegistration,
         int ExceptionCode);

__declspec(dllimport)
PPAL_EXCEPTION_REGISTRATION
__stdcall
PAL_GetBottommostRegistration(
              VOID);

__declspec(dllimport)
VOID
__stdcall
PAL_SetBottommostRegistration(
              PPAL_EXCEPTION_REGISTRATION pRegistration);

__declspec(dllimport)
PPAL_EXCEPTION_REGISTRATION *
__stdcall
PAL_GetBottommostRegistrationPtr(
              VOID);

typedef enum _PAL_SEH_DISPATCHER_STATE {
    DoneWithSEH,
    SetUpForSEH,
    InTryBlock,
    InExceptFinallyBlock
} PAL_SEH_DISPATCHER_STATE;













#line 4411 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

























































__declspec(dllimport)
void 
__stdcall
PAL_DoLastMomentUnwind( PVOID target);






































































































#line 4575 "c:\\sscli20\\pal\\inc\\rotor_pal.h"














#pragma warning(disable:4611) 
#line 4591 "c:\\sscli20\\pal\\inc\\rotor_pal.h"






















#line 4614 "c:\\sscli20\\pal\\inc\\rotor_pal.h"





#line 4620 "c:\\sscli20\\pal\\inc\\rotor_pal.h"






































































#line 4691 "c:\\sscli20\\pal\\inc\\rotor_pal.h"

#line 137 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"











#line 1 "c:\\sscli20\\pal\\inc\\pal_assert.h"





























































#line 63 "c:\\sscli20\\pal\\inc\\pal_assert.h"



#line 67 "c:\\sscli20\\pal\\inc\\pal_assert.h"





#line 73 "c:\\sscli20\\pal\\inc\\pal_assert.h"
#line 149 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"























































#line 205 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"































#line 237 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"





































#line 275 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"








#line 284 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"

extern const GUID GUID_NULL;

typedef const GUID  *LPCGUID;















typedef GUID IID;




#line 309 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"





typedef GUID CLSID;





#line 321 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"



typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

typedef LONG SCODE;


typedef union _ULARGE_INTEGER {
    struct {



#line 337 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
        DWORD LowPart;
        DWORD HighPart;
#line 340 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;























































 LPVOID __stdcall CoTaskMemAlloc(SIZE_T cb);
 void __stdcall CoTaskMemFree(LPVOID pv);

typedef SHORT VARIANT_BOOL;



typedef WCHAR OLECHAR;
typedef OLECHAR* LPOLESTR;
typedef const OLECHAR* LPCOLESTR;

typedef WCHAR *BSTR;

 BSTR __stdcall SysAllocString(const OLECHAR*);
 BSTR __stdcall SysAllocStringLen(const OLECHAR*, UINT);
 BSTR __stdcall SysAllocStringByteLen(const char *, UINT);
 void __stdcall SysFreeString(BSTR);
 UINT __stdcall SysStringLen(BSTR);
 UINT __stdcall SysStringByteLen(BSTR);

typedef double DATE;

typedef union tagCY {
    struct {



#line 426 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
        ULONG   Lo;
        LONG    Hi;
#line 429 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
    } u;
    LONGLONG int64;
} CY, *LPCY;

typedef CY CURRENCY;

typedef struct tagDEC {
    
    
    
    









#line 450 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
    USHORT wReserved;
    union {
        struct {
            BYTE scale;
            BYTE sign;
        } u;
        USHORT signscale;
    } u;
#line 459 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
    ULONG Hi32;
    union {
        struct {
            ULONG Lo32;
            ULONG Mid32;
        } v;
        
    } v;
} DECIMAL, *LPDECIMAL;











typedef struct tagBLOB {
    ULONG cbSize;
    BYTE *pBlobData;
} BLOB, *LPBLOB;

struct IUnknown;
struct IStream;
struct IRecordInfo;

typedef unsigned short VARTYPE;

enum VARENUM {
    VT_EMPTY    = 0,
    VT_NULL = 1,
    VT_I2   = 2,
    VT_I4   = 3,
    VT_R4   = 4,
    VT_R8   = 5,
    VT_CY   = 6,
    VT_DATE = 7,
    VT_BSTR = 8,
    VT_DISPATCH = 9,
    VT_ERROR    = 10,
    VT_BOOL = 11,
    VT_VARIANT  = 12,
    VT_UNKNOWN  = 13,
    VT_DECIMAL  = 14,
    VT_I1   = 16,
    VT_UI1  = 17,
    VT_UI2  = 18,
    VT_UI4  = 19,
    VT_I8   = 20,
    VT_UI8  = 21,
    VT_INT  = 22,
    VT_UINT = 23,
    VT_VOID = 24,
    VT_HRESULT  = 25,
    VT_PTR  = 26,
    VT_SAFEARRAY    = 27,
    VT_CARRAY   = 28,
    VT_USERDEFINED  = 29,
    VT_LPSTR    = 30,
    VT_LPWSTR   = 31,
    VT_RECORD   = 36,

    VT_FILETIME        = 64,
    VT_BLOB            = 65,
    VT_STREAM          = 66,
    VT_STORAGE         = 67,
    VT_STREAMED_OBJECT = 68,
    VT_STORED_OBJECT   = 69,
    VT_BLOB_OBJECT     = 70,
    VT_CF              = 71,
    VT_CLSID           = 72,

    VT_VECTOR   = 0x1000,
    VT_ARRAY    = 0x2000,
    VT_BYREF    = 0x4000,
    VT_TYPEMASK = 0xfff,
};

typedef struct tagVARIANT VARIANT, *LPVARIANT;

struct tagVARIANT
    {
    union
        {
        struct
            {





#line 554 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
            VARTYPE vt;
            WORD wReserved1;
#line 557 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
            WORD wReserved2;
            WORD wReserved3;
            union
                {
                LONGLONG llVal;
                LONG lVal;
                BYTE bVal;
                SHORT iVal;
                FLOAT fltVal;
                DOUBLE dblVal;
                VARIANT_BOOL boolVal;
                SCODE scode;
                CY cyVal;
                DATE date;
                BSTR bstrVal;
                struct IUnknown *punkVal;
                BYTE *pbVal;
                SHORT *piVal;
                LONG *plVal;
                LONGLONG *pllVal;
                FLOAT *pfltVal;
                DOUBLE *pdblVal;
                VARIANT_BOOL *pboolVal;
                SCODE *pscode;
                CY *pcyVal;
                DATE *pdate;
                BSTR *pbstrVal;
                struct IUnknown **ppunkVal;
                VARIANT *pvarVal;
                PVOID byref;
                CHAR cVal;
                USHORT uiVal;
                ULONG ulVal;
                ULONGLONG ullVal;
                INT intVal;
                UINT uintVal;
                DECIMAL *pdecVal;
                CHAR *pcVal;
                USHORT *puiVal;
                ULONG *pulVal;
                ULONGLONG *pullVal;
                INT *pintVal;
                UINT *puintVal;
                struct __tagBRECORD
                    {
                    PVOID pvRecord;
                    struct IRecordInfo *pRecInfo;
                    } brecVal;
                } n3;
            } n2;
        DECIMAL decVal;
        } n1;
    };

typedef VARIANT VARIANTARG, *LPVARIANTARG;

 void __stdcall VariantInit(VARIANT * pvarg);
 HRESULT __stdcall VariantClear(VARIANT * pvarg);
























































 HRESULT __stdcall CreateStreamOnHGlobal(PVOID hGlobal, BOOL fDeleteOnRelease, struct IStream** ppstm);




















 HRESULT __stdcall IIDFromString(LPOLESTR lpsz, IID* lpiid);
 int __stdcall StringFromGUID2(const GUID * rguid, LPOLESTR lpsz, int cchMax); 

 HRESULT __stdcall CoCreateGuid( GUID * pguid);



typedef unsigned int ALG_ID;



































typedef ULONG_PTR HCRYPTPROV;
typedef ULONG_PTR HCRYPTKEY;
typedef ULONG_PTR HCRYPTHASH;


BOOL
__stdcall
CryptAcquireContextW(
    HCRYPTPROV *phProv,
    LPCWSTR szContainer,
    LPCWSTR szProvider,
    DWORD dwProvType,
    DWORD dwFlags);


BOOL
__stdcall
CryptAcquireContextA(
    HCRYPTPROV *phProv,
    LPCSTR szContainer,
    LPCSTR szProvider,
    DWORD dwProvType,
    DWORD dwFlags);





#line 764 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"


BOOL
__stdcall
CryptReleaseContext(
    HCRYPTPROV hProv,
    DWORD dwFlags);


BOOL
__stdcall
CryptCreateHash(
    HCRYPTPROV hProv,
    ALG_ID Algid,
    HCRYPTKEY hKey,
    DWORD dwFlags,
    HCRYPTHASH *phHash);


BOOL
__stdcall
CryptDestroyHash(
    HCRYPTHASH hHash);


BOOL
__stdcall
CryptHashData(
    HCRYPTHASH hHash,
    const BYTE *pbData,
    DWORD dwDataLen,
    DWORD dwFlags);


BOOL
__stdcall
CryptGetHashParam(
    HCRYPTHASH hHash,
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags);


BOOL
__stdcall
CryptGenKey(
    HCRYPTPROV hProv,
    ALG_ID Algid,
    DWORD dwFlags,
    HCRYPTKEY *phKey);


BOOL
__stdcall
CryptDestroyKey(
    HCRYPTKEY hKey);


BOOL
__stdcall
CryptImportKey(
    HCRYPTPROV hProv,
    const BYTE *pbData,
    DWORD dwDataLen,
    HCRYPTKEY hPubKey,
    DWORD dwFlags,
    HCRYPTKEY *phKey);


BOOL
__stdcall
CryptExportKey(
    HCRYPTKEY hKey,
    HCRYPTKEY hExpKey,
    DWORD dwBlobType,
    DWORD dwFlags,
    BYTE *pbData,
    DWORD *pdwDataLen);


BOOL
__stdcall
CryptGetKeyParam(
    HCRYPTKEY hKey,
    DWORD dwParam,
    BYTE *pbData,
    DWORD *pdwDataLen,
    DWORD dwFlags);


BOOL
__stdcall
CryptVerifySignatureW(
    HCRYPTHASH hHash,
    const BYTE *pbSignature,
    DWORD dwSigLen,
    HCRYPTKEY hPubKey,
    LPCWSTR szDescription,
    DWORD dwFlags);


BOOL
__stdcall
CryptVerifySignatureA(
    HCRYPTHASH hHash,
    const BYTE *pbSignature,
    DWORD dwSigLen,
    HCRYPTKEY hPubKey,
    LPCSTR szDescription,
    DWORD dwFlags);





#line 881 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"


BOOL
__stdcall
CryptSignHashW(
    HCRYPTHASH hHash,
    DWORD dwKeySpec,
    LPCWSTR szDescription,
    DWORD dwFlags,
    BYTE *pbSignature,
    DWORD *pdwSigLen);


BOOL
__stdcall
CryptSignHashA(
    HCRYPTHASH hHash,
    DWORD dwKeySpec,
    LPCSTR szDescription,
    DWORD dwFlags,
    BYTE *pbSignature,
    DWORD *pdwSigLen);





#line 909 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"



typedef 
enum tagMIMECONTF {
    MIMECONTF_MAILNEWS  = 0x1,
    MIMECONTF_BROWSER   = 0x2,
    MIMECONTF_MINIMAL   = 0x4,
    MIMECONTF_IMPORT    = 0x8,
    MIMECONTF_SAVABLE_MAILNEWS  = 0x100,
    MIMECONTF_SAVABLE_BROWSER   = 0x200,
    MIMECONTF_EXPORT    = 0x400,
    MIMECONTF_PRIVCONVERTER = 0x10000,
    MIMECONTF_VALID = 0x20000,
    MIMECONTF_VALID_NLS = 0x40000,
    MIMECONTF_MIME_IE4  = 0x10000000,
    MIMECONTF_MIME_LATEST   = 0x20000000,
    MIMECONTF_MIME_REGISTRY = 0x40000000
    }   MIMECONTF;












































 LPWSTR __stdcall StrNCatW(LPWSTR lpFront, LPCWSTR lpBack, int cchMax);
 int __stdcall StrToIntW(LPCWSTR lpSrc);
 LPWSTR __stdcall StrStrIW(LPCWSTR lpFirst, LPCWSTR lpSrch);
 LPWSTR __stdcall StrRChrW(LPCWSTR lpStart, LPCWSTR lpEnd, WCHAR wMatch);
 LPWSTR __stdcall StrCatBuffW(LPWSTR pszDest, LPCWSTR pszSrc, int cchDestBuffSize);



















































































































































































































































































































































































 BOOL __stdcall PathAppendW(LPWSTR pszPath, LPCWSTR pszMore);
 int __stdcall PathCommonPrefixW(LPCWSTR pszFile1, LPCWSTR pszFile2, LPWSTR  pszPath);
 LPWSTR __stdcall PathFindFileNameW(LPCWSTR pPath);
 LPWSTR __stdcall PathFindExtensionW(LPCWSTR pszPath);
 int __stdcall PathGetDriveNumberW(LPCWSTR lpsz);
 BOOL __stdcall PathIsRelativeW(LPCWSTR lpszPath);
 BOOL __stdcall PathIsUNCW(LPCWSTR pszPath);
 LPWSTR __stdcall PathAddBackslashW(LPWSTR lpszPath);
 LPWSTR __stdcall PathRemoveBackslashW(LPWSTR lpszPath);
 void __stdcall PathRemoveExtensionW(LPWSTR pszPath);
 LPWSTR __stdcall PathCombineW(LPWSTR lpszDest, LPCWSTR lpszDir, LPCWSTR lpszFile);
 BOOL __stdcall PathCanonicalizeW(LPWSTR lpszDst, LPCWSTR lpszSrc);
 BOOL __stdcall PathRelativePathToW(LPWSTR pszPath, LPCWSTR pszFrom, DWORD dwAttrFrom, LPCWSTR pszTo, DWORD dwAttrTo);
 BOOL __stdcall PathRenameExtensionW(LPWSTR pszPath, LPCWSTR pszExt);
 BOOL __stdcall PathRemoveFileSpecW(LPWSTR pFile);
 void __stdcall PathStripPathW (LPWSTR pszPath);

 HRESULT __stdcall PathCreateFromUrlW(LPCWSTR pszUrl, LPWSTR pszPath, LPDWORD pcchPath, DWORD dwFlags);
 BOOL __stdcall PathIsURLW(LPCWSTR pszPath);





typedef enum {
    URLIS_FILEURL = 3,
} URLIS;

typedef enum {
    URL_PART_SCHEME     = 1,
    URL_PART_HOSTNAME   = 2,
} URL_PART;

 HRESULT __stdcall UrlCanonicalizeW(LPCWSTR pszUrl, LPWSTR pszCanonicalized, LPDWORD pcchCanonicalized, DWORD dwFlags);
 HRESULT __stdcall UrlCombineW(LPCWSTR pszBase, LPCWSTR pszRelative, LPWSTR pszCombined, LPDWORD pcchCombined, DWORD dwFlags);
 HRESULT __stdcall UrlEscapeW(LPCWSTR pszUrl, LPWSTR pszEscaped, LPDWORD pcchEscaped, DWORD dwFlags);
 HRESULT __stdcall UrlUnescapeW(LPWSTR pszURL, LPWSTR pszUnescaped, LPDWORD pcchUnescaped, DWORD dwFlags);
 BOOL __stdcall UrlIsW(LPCWSTR pszUrl, URLIS dwUrlIs);
 HRESULT __stdcall UrlGetPartW(LPCWSTR pszIn, LPWSTR pszOut, LPDWORD pcchOut, DWORD dwPart, DWORD dwFlags);




































typedef VOID (__stdcall * WAITORTIMERCALLBACKFUNC) (PVOID, BOOLEAN );

typedef HANDLE HWND;






typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY;

typedef VOID (__stdcall *WAITORTIMERCALLBACK)(PVOID, BOOL);






typedef struct tagSAFEARRAYBOUND
    {
    ULONG cElements;
    LONG lLbound;
    } 	SAFEARRAYBOUND;

typedef struct tagSAFEARRAYBOUND *LPSAFEARRAYBOUND;

typedef struct tagSAFEARRAY
    {
    USHORT cDims;
    USHORT fFeatures;
    ULONG cbElements;
    ULONG cLocks;
    PVOID pvData;
    SAFEARRAYBOUND rgsabound[ 1 ];
    } 	SAFEARRAY;

typedef SAFEARRAY *LPSAFEARRAY;


 SAFEARRAY * __stdcall SafeArrayCreateVector(VARTYPE vt, LONG lLbound, ULONG cElements);
 UINT __stdcall SafeArrayGetDim(SAFEARRAY * psa);
 HRESULT __stdcall SafeArrayGetElement(SAFEARRAY * psa, LONG * rgIndices, void * pv);
 HRESULT __stdcall SafeArrayGetLBound(SAFEARRAY * psa, UINT nDim, LONG * plLbound);
 HRESULT __stdcall SafeArrayGetUBound(SAFEARRAY * psa, UINT nDim, LONG * plUbound);
 HRESULT __stdcall SafeArrayGetVartype(SAFEARRAY * psa, VARTYPE * pvt);
 HRESULT __stdcall SafeArrayPutElement(SAFEARRAY * psa, LONG * rgIndices, void * pv);
 HRESULT __stdcall SafeArrayDestroy(SAFEARRAY * psa);

 void * _stdcall _lfind(const void *, const void *, unsigned int *, unsigned int,
        int (__cdecl *)(const void *, const void *));





struct IDispatch;
struct ITypeInfo;
struct ITypeLib;
struct IMoniker;

typedef VOID (__stdcall *LPOVERLAPPED_COMPLETION_ROUTINE)( 
    DWORD dwErrorCode,
    DWORD dwNumberOfBytesTransfered,
    LPVOID lpOverlapped);
typedef int DEBUG_EVENT;
typedef DEBUG_EVENT *LPDEBUG_EVENT;



















typedef LPVOID HKEY;
typedef LPVOID PACL;
typedef LPVOID LPBC;
typedef LPVOID PSECURITY_DESCRIPTOR;

typedef struct _EXCEPTION_RECORD64 {
    DWORD ExceptionCode;
    ULONG ExceptionFlags;
    ULONG64 ExceptionRecord;
    ULONG64 ExceptionAddress;
    ULONG NumberParameters;
    ULONG __unusedAlignment;
    ULONG64 ExceptionInformation[15];
} EXCEPTION_RECORD64, *PEXCEPTION_RECORD64;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;



    PWSTR  Buffer;
#line 1534 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _STRING64 {
    USHORT   Length;
    USHORT   MaximumLength;
    ULONGLONG  Buffer;
} STRING64;
typedef STRING64 *PSTRING64;

typedef STRING64 UNICODE_STRING64;
typedef UNICODE_STRING64 *PUNICODE_STRING64;

typedef STRING64 ANSI_STRING64;
typedef ANSI_STRING64 *PANSI_STRING64;









typedef struct LIST_ENTRY32 {
    ULONG Flink;
    ULONG Blink;
} LIST_ENTRY32;
typedef LIST_ENTRY32 *PLIST_ENTRY32;

typedef struct LIST_ENTRY64 {
    ULONGLONG Flink;
    ULONGLONG Blink;
} LIST_ENTRY64;
typedef LIST_ENTRY64 *PLIST_ENTRY64;



typedef struct _HSATELLITE *HSATELLITE;

 HSATELLITE __stdcall PAL_LoadSatelliteResourceW(LPCWSTR SatelliteResourceFileName);
 HSATELLITE __stdcall PAL_LoadSatelliteResourceA(LPCSTR SatelliteResourceFileName);
 BOOL __stdcall PAL_FreeSatelliteResource(HSATELLITE SatelliteResource);
 UINT __stdcall PAL_LoadSatelliteStringW(HSATELLITE SatelliteResource,
             UINT uID,
             LPWSTR lpBuffer,
             UINT nBufferMax);
 UINT __stdcall PAL_LoadSatelliteStringA(HSATELLITE SatelliteResource,
             UINT uID,
             LPSTR lpBuffer,
             UINT nBufferMax);

 HRESULT __stdcall PAL_CoCreateInstance(const CLSID *   rclsid,
                             const IID *     riid,
                             void     **ppv);





 BOOL __stdcall PAL_GetMachineConfigurationDirectoryW(
                       LPWSTR lpDirectoryName,
                       UINT cbDirectoryName);





typedef struct _HINI *HINI;

 HINI __stdcall PAL_IniCreate();
 BOOL __stdcall PAL_IniClose(HINI hIni);
 BOOL __stdcall PAL_IniLoad(HINI hIni, 
             LPCWSTR pszFileName);
 BOOL __stdcall PAL_IniSave(HINI hIni, 
             LPCWSTR pszFileName, 
             BOOL fForce);
 DWORD __stdcall PAL_IniReadString(HINI hIni, 
             LPCWSTR pszSection, 
             LPCWSTR pszKey, 
             LPWSTR pszValue, 
             DWORD dwSize);
 BOOL __stdcall PAL_IniWriteString(HINI hIni, 
            LPCWSTR pszSection, 
            LPCWSTR pszKey, 
            LPCWSTR pszValue);
 BOOL __stdcall PAL_IniGetString(HINI hIni, 
             LPCWSTR pszSection, 
             LPCWSTR pszKey, 
             LPWSTR pszValue, 
             DWORD dwSize);


BOOL
__stdcall
PAL_FetchConfigurationStringW(
                     BOOL bPerMachine,
                     LPCWSTR lpParameterName,
                     LPWSTR lpParameterValue,
                     UINT cbParameterValueLength);
                    

BOOL
__stdcall
PAL_SetConfigurationStringW(
                     BOOL bPerMachine,
                     LPCWSTR lpParameterName,
                     LPCWSTR lpParameterValue);








#line 1 "c:\\sscli20\\pal\\inc\\pal_endian.h"







































































#line 73 "c:\\sscli20\\pal\\inc\\pal_endian.h"








#line 82 "c:\\sscli20\\pal\\inc\\pal_endian.h"










































#line 125 "c:\\sscli20\\pal\\inc\\pal_endian.h"











#line 137 "c:\\sscli20\\pal\\inc\\pal_endian.h"











#line 149 "c:\\sscli20\\pal\\inc\\pal_endian.h"
#line 1652 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"



#line 1 "c:\\sscli20\\palrt\\inc\\ntimage.h"



































#pragma once
#line 38 "c:\\sscli20\\palrt\\inc\\ntimage.h"















#line 1 "c:\\sscli20\\palrt\\inc\\pshpack4.h"




































#pragma warning(disable:4103)

#pragma pack(push,4)


#line 43 "c:\\sscli20\\palrt\\inc\\pshpack4.h"


#line 46 "c:\\sscli20\\palrt\\inc\\pshpack4.h"
#line 47 "c:\\sscli20\\palrt\\inc\\pshpack4.h"
#line 54 "c:\\sscli20\\palrt\\inc\\ntimage.h"







#line 1 "c:\\sscli20\\palrt\\inc\\pshpack2.h"



































#pragma warning(disable:4103)

#pragma pack(push,2)


#line 42 "c:\\sscli20\\palrt\\inc\\pshpack2.h"


#line 45 "c:\\sscli20\\palrt\\inc\\pshpack2.h"
#line 46 "c:\\sscli20\\palrt\\inc\\pshpack2.h"
#line 62 "c:\\sscli20\\palrt\\inc\\ntimage.h"


typedef struct _IMAGE_DOS_HEADER {      
    USHORT e_magic;                     
    USHORT e_cblp;                      
    USHORT e_cp;                        
    USHORT e_crlc;                      
    USHORT e_cparhdr;                   
    USHORT e_minalloc;                  
    USHORT e_maxalloc;                  
    USHORT e_ss;                        
    USHORT e_sp;                        
    USHORT e_csum;                      
    USHORT e_ip;                        
    USHORT e_cs;                        
    USHORT e_lfarlc;                    
    USHORT e_ovno;                      
    USHORT e_res[4];                    
    USHORT e_oemid;                     
    USHORT e_oeminfo;                   
    USHORT e_res2[10];                  
    LONG   e_lfanew;                    
  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_OS2_HEADER {      
    USHORT ne_magic;                    
    CHAR   ne_ver;                      
    CHAR   ne_rev;                      
    USHORT ne_enttab;                   
    USHORT ne_cbenttab;                 
    LONG   ne_crc;                      
    USHORT ne_flags;                    
    USHORT ne_autodata;                 
    USHORT ne_heap;                     
    USHORT ne_stack;                    
    LONG   ne_csip;                     
    LONG   ne_sssp;                     
    USHORT ne_cseg;                     
    USHORT ne_cmod;                     
    USHORT ne_cbnrestab;                
    USHORT ne_segtab;                   
    USHORT ne_rsrctab;                  
    USHORT ne_restab;                   
    USHORT ne_modtab;                   
    USHORT ne_imptab;                   
    LONG   ne_nrestab;                  
    USHORT ne_cmovent;                  
    USHORT ne_align;                    
    USHORT ne_cres;                     
    UCHAR  ne_exetyp;                   
    UCHAR  ne_flagsothers;              
    USHORT ne_pretthunks;               
    USHORT ne_psegrefbytes;             
    USHORT ne_swaparea;                 
    USHORT ne_expver;                   
  } IMAGE_OS2_HEADER, *PIMAGE_OS2_HEADER;

typedef struct _IMAGE_VXD_HEADER {      
    USHORT e32_magic;                   
    UCHAR  e32_border;                  
    UCHAR  e32_worder;                  
    ULONG  e32_level;                   
    USHORT e32_cpu;                     
    USHORT e32_os;                      
    ULONG  e32_ver;                     
    ULONG  e32_mflags;                  
    ULONG  e32_mpages;                  
    ULONG  e32_startobj;                
    ULONG  e32_eip;                     
    ULONG  e32_stackobj;                
    ULONG  e32_esp;                     
    ULONG  e32_pagesize;                
    ULONG  e32_lastpagesize;            
    ULONG  e32_fixupsize;               
    ULONG  e32_fixupsum;                
    ULONG  e32_ldrsize;                 
    ULONG  e32_ldrsum;                  
    ULONG  e32_objtab;                  
    ULONG  e32_objcnt;                  
    ULONG  e32_objmap;                  
    ULONG  e32_itermap;                 
    ULONG  e32_rsrctab;                 
    ULONG  e32_rsrccnt;                 
    ULONG  e32_restab;                  
    ULONG  e32_enttab;                  
    ULONG  e32_dirtab;                  
    ULONG  e32_dircnt;                  
    ULONG  e32_fpagetab;                
    ULONG  e32_frectab;                 
    ULONG  e32_impmod;                  
    ULONG  e32_impmodcnt;               
    ULONG  e32_impproc;                 
    ULONG  e32_pagesum;                 
    ULONG  e32_datapage;                
    ULONG  e32_preload;                 
    ULONG  e32_nrestab;                 
    ULONG  e32_cbnrestab;               
    ULONG  e32_nressum;                 
    ULONG  e32_autodata;                
    ULONG  e32_debuginfo;               
    ULONG  e32_debuglen;                
    ULONG  e32_instpreload;             
    ULONG  e32_instdemand;              
    ULONG  e32_heapsize;                
    UCHAR  e32_res3[12];                
    ULONG  e32_winresoff;
    ULONG  e32_winreslen;
    USHORT e32_devid;                   
    USHORT e32_ddkver;                  
  } IMAGE_VXD_HEADER, *PIMAGE_VXD_HEADER;

#line 1 "c:\\sscli20\\palrt\\inc\\poppack.h"





































#pragma warning(disable:4103)

#pragma pack(pop)


#line 44 "c:\\sscli20\\palrt\\inc\\poppack.h"


#line 47 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 48 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 174 "c:\\sscli20\\palrt\\inc\\ntimage.h"





typedef struct _IMAGE_FILE_HEADER {
    USHORT  Machine;
    USHORT  NumberOfSections;
    ULONG   TimeDateStamp;
    ULONG   PointerToSymbolTable;
    ULONG   NumberOfSymbols;
    USHORT  SizeOfOptionalHeader;
    USHORT  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;






















































typedef struct _IMAGE_DATA_DIRECTORY {
    ULONG   VirtualAddress;
    ULONG   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;







typedef struct _IMAGE_OPTIONAL_HEADER {
    
    
    

    USHORT  Magic;
    UCHAR   MajorLinkerVersion;
    UCHAR   MinorLinkerVersion;
    ULONG   SizeOfCode;
    ULONG   SizeOfInitializedData;
    ULONG   SizeOfUninitializedData;
    ULONG   AddressOfEntryPoint;
    ULONG   BaseOfCode;
    ULONG   BaseOfData;

    
    
    

    ULONG   ImageBase;
    ULONG   SectionAlignment;
    ULONG   FileAlignment;
    USHORT  MajorOperatingSystemVersion;
    USHORT  MinorOperatingSystemVersion;
    USHORT  MajorImageVersion;
    USHORT  MinorImageVersion;
    USHORT  MajorSubsystemVersion;
    USHORT  MinorSubsystemVersion;
    ULONG   Win32VersionValue;
    ULONG   SizeOfImage;
    ULONG   SizeOfHeaders;
    ULONG   CheckSum;
    USHORT  Subsystem;
    USHORT  DllCharacteristics;
    ULONG   SizeOfStackReserve;
    ULONG   SizeOfStackCommit;
    ULONG   SizeOfHeapReserve;
    ULONG   SizeOfHeapCommit;
    ULONG   LoaderFlags;
    ULONG   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

typedef struct _IMAGE_ROM_OPTIONAL_HEADER {
    USHORT Magic;
    UCHAR  MajorLinkerVersion;
    UCHAR  MinorLinkerVersion;
    ULONG  SizeOfCode;
    ULONG  SizeOfInitializedData;
    ULONG  SizeOfUninitializedData;
    ULONG  AddressOfEntryPoint;
    ULONG  BaseOfCode;
    ULONG  BaseOfData;
    ULONG  BaseOfBss;
    ULONG  GprMask;
    ULONG  CprMask[4];
    ULONG  GpValue;
} IMAGE_ROM_OPTIONAL_HEADER, *PIMAGE_ROM_OPTIONAL_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    USHORT      Magic;
    UCHAR       MajorLinkerVersion;
    UCHAR       MinorLinkerVersion;
    ULONG       SizeOfCode;
    ULONG       SizeOfInitializedData;
    ULONG       SizeOfUninitializedData;
    ULONG       AddressOfEntryPoint;
    ULONG       BaseOfCode;
    ULONGLONG   ImageBase;
    ULONG       SectionAlignment;
    ULONG       FileAlignment;
    USHORT      MajorOperatingSystemVersion;
    USHORT      MinorOperatingSystemVersion;
    USHORT      MajorImageVersion;
    USHORT      MinorImageVersion;
    USHORT      MajorSubsystemVersion;
    USHORT      MinorSubsystemVersion;
    ULONG       Win32VersionValue;
    ULONG       SizeOfImage;
    ULONG       SizeOfHeaders;
    ULONG       CheckSum;
    USHORT      Subsystem;
    USHORT      DllCharacteristics;
    ULONGLONG   SizeOfStackReserve;
    ULONGLONG   SizeOfStackCommit;
    ULONGLONG   SizeOfHeapReserve;
    ULONGLONG   SizeOfHeapCommit;
    ULONG       LoaderFlags;
    ULONG       NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;










typedef IMAGE_OPTIONAL_HEADER32             IMAGE_OPTIONAL_HEADER;
typedef PIMAGE_OPTIONAL_HEADER32            PIMAGE_OPTIONAL_HEADER;



typedef struct _IMAGE_NT_HEADERS64 {
    ULONG Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_NT_HEADERS {
    ULONG Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

typedef struct _IMAGE_ROM_HEADERS {
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_ROM_OPTIONAL_HEADER OptionalHeader;
} IMAGE_ROM_HEADERS, *PIMAGE_ROM_HEADERS;

typedef IMAGE_NT_HEADERS32                  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS32                 PIMAGE_NT_HEADERS;









































































typedef struct ANON_OBJECT_HEADER {
    USHORT  Sig1;            
    USHORT  Sig2;            
    USHORT  Version;         
    USHORT  Machine;
    ULONG   TimeDateStamp;
    CLSID   ClassID;         
    ULONG   SizeOfData;      
} ANON_OBJECT_HEADER;







typedef struct _IMAGE_SECTION_HEADER {
    UCHAR   Name[8];
    union {
            ULONG   PhysicalAddress;
            ULONG   VirtualSize;
    } Misc;
    ULONG   VirtualAddress;
    ULONG   SizeOfRawData;
    ULONG   PointerToRawData;
    ULONG   PointerToRelocations;
    ULONG   PointerToLinenumbers;
    USHORT  NumberOfRelocations;
    USHORT  NumberOfLinenumbers;
    ULONG   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;
































































#line 1 "c:\\sscli20\\palrt\\inc\\pshpack2.h"



































#pragma warning(disable:4103)

#pragma pack(push,2)


#line 42 "c:\\sscli20\\palrt\\inc\\pshpack2.h"


#line 45 "c:\\sscli20\\palrt\\inc\\pshpack2.h"
#line 46 "c:\\sscli20\\palrt\\inc\\pshpack2.h"
#line 547 "c:\\sscli20\\palrt\\inc\\ntimage.h"





typedef struct _IMAGE_SYMBOL {
    union {
        UCHAR   ShortName[8];
        struct {
            ULONG   Short;     
            ULONG   Long;      
        } Name;
        ULONG   LongName[2];    
    } N;
    ULONG   Value;
    SHORT   SectionNumber;
    USHORT  Type;
    UCHAR   StorageClass;
    UCHAR   NumberOfAuxSymbols;
} IMAGE_SYMBOL;
typedef IMAGE_SYMBOL  *PIMAGE_SYMBOL;


































































































#line 667 "c:\\sscli20\\palrt\\inc\\ntimage.h"




#line 672 "c:\\sscli20\\palrt\\inc\\ntimage.h"





#line 678 "c:\\sscli20\\palrt\\inc\\ntimage.h"




#line 683 "c:\\sscli20\\palrt\\inc\\ntimage.h"



#line 687 "c:\\sscli20\\palrt\\inc\\ntimage.h"


#line 690 "c:\\sscli20\\palrt\\inc\\ntimage.h"





typedef union _IMAGE_AUX_SYMBOL {
    struct {
        ULONG    TagIndex;                      
        union {
            struct {
                USHORT  Linenumber;             
                USHORT  Size;                   
            } LnSz;
           ULONG    TotalSize;
        } Misc;
        union {
            struct {                            
                ULONG    PointerToLinenumber;
                ULONG    PointerToNextFunction;
            } Function;
            struct {                            
                USHORT   Dimension[4];
            } Array;
        } FcnAry;
        USHORT  TvIndex;                        
    } Sym;
    struct {
        UCHAR   Name[18];
    } File;
    struct {
        ULONG   Length;                         
        USHORT  NumberOfRelocations;            
        USHORT  NumberOfLinenumbers;            
        ULONG   CheckSum;                       
        SHORT   Number;                         
        UCHAR   Selection;                      
    } Section;
} IMAGE_AUX_SYMBOL;
typedef IMAGE_AUX_SYMBOL  *PIMAGE_AUX_SYMBOL;



typedef enum IMAGE_AUX_SYMBOL_TYPE {
    IMAGE_AUX_SYMBOL_TYPE_TOKEN_DEF = 1,
} IMAGE_AUX_SYMBOL_TYPE;

#line 1 "c:\\sscli20\\palrt\\inc\\pshpack2.h"



































#pragma warning(disable:4103)

#pragma pack(push,2)


#line 42 "c:\\sscli20\\palrt\\inc\\pshpack2.h"


#line 45 "c:\\sscli20\\palrt\\inc\\pshpack2.h"
#line 46 "c:\\sscli20\\palrt\\inc\\pshpack2.h"
#line 737 "c:\\sscli20\\palrt\\inc\\ntimage.h"

typedef struct IMAGE_AUX_SYMBOL_TOKEN_DEF {
    UCHAR bAuxType;                  
    UCHAR bReserved;                 
    ULONG SymbolTableIndex;
    UCHAR rgbReserved[12];           
} IMAGE_AUX_SYMBOL_TOKEN_DEF;

typedef IMAGE_AUX_SYMBOL_TOKEN_DEF  *PIMAGE_AUX_SYMBOL_TOKEN_DEF;

#line 1 "c:\\sscli20\\palrt\\inc\\poppack.h"





































#pragma warning(disable:4103)

#pragma pack(pop)


#line 44 "c:\\sscli20\\palrt\\inc\\poppack.h"


#line 47 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 48 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 748 "c:\\sscli20\\palrt\\inc\\ntimage.h"





















typedef struct _IMAGE_RELOCATION {
    union {
        ULONG   VirtualAddress;
        ULONG   RelocCount;             
    } u;
    ULONG   SymbolTableIndex;
    USHORT  Type;
} IMAGE_RELOCATION;
typedef IMAGE_RELOCATION  *PIMAGE_RELOCATION;






































































































































































































































































































typedef struct _IMAGE_LINENUMBER {
    union {
        ULONG   SymbolTableIndex;               
        ULONG   VirtualAddress;                 
    } Type;
    USHORT  Linenumber;                         
} IMAGE_LINENUMBER;
typedef IMAGE_LINENUMBER  *PIMAGE_LINENUMBER;



#line 1 "c:\\sscli20\\palrt\\inc\\poppack.h"





































#pragma warning(disable:4103)

#pragma pack(pop)


#line 44 "c:\\sscli20\\palrt\\inc\\poppack.h"


#line 47 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 48 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 1084 "c:\\sscli20\\palrt\\inc\\ntimage.h"





typedef struct _IMAGE_BASE_RELOCATION {
    ULONG   VirtualAddress;
    ULONG   SizeOfBlock;

} IMAGE_BASE_RELOCATION;
typedef IMAGE_BASE_RELOCATION  * PIMAGE_BASE_RELOCATION;


































typedef struct _IMAGE_ARCHIVE_MEMBER_HEADER {
    UCHAR    Name[16];                          
    UCHAR    Date[12];                          
    UCHAR    UserID[6];                         
    UCHAR    GroupID[6];                        
    UCHAR    Mode[8];                           
    UCHAR    Size[10];                          
    UCHAR    EndHeader[2];                      
} IMAGE_ARCHIVE_MEMBER_HEADER, *PIMAGE_ARCHIVE_MEMBER_HEADER;











typedef struct _IMAGE_EXPORT_DIRECTORY {
    ULONG   Characteristics;
    ULONG   TimeDateStamp;
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    ULONG   Name;
    ULONG   Base;
    ULONG   NumberOfFunctions;
    ULONG   NumberOfNames;
    ULONG   AddressOfFunctions;     
    ULONG   AddressOfNames;         
    ULONG   AddressOfNameOrdinals;  
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;





typedef struct _IMAGE_IMPORT_BY_NAME {
    USHORT  Hint;
    UCHAR   Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;

#line 1 "c:\\sscli20\\palrt\\inc\\pshpack8.h"




































#pragma warning(disable:4103)

#pragma pack(push,8)


#line 43 "c:\\sscli20\\palrt\\inc\\pshpack8.h"


#line 46 "c:\\sscli20\\palrt\\inc\\pshpack8.h"
#line 47 "c:\\sscli20\\palrt\\inc\\pshpack8.h"
#line 1173 "c:\\sscli20\\palrt\\inc\\ntimage.h"

typedef struct _IMAGE_THUNK_DATA64 {
    union {
        ULONGLONG ForwarderString;  
        ULONGLONG Function;         
        ULONGLONG Ordinal;
        ULONGLONG AddressOfData;    
    } u1;
} IMAGE_THUNK_DATA64;
typedef IMAGE_THUNK_DATA64 * PIMAGE_THUNK_DATA64;

#line 1 "c:\\sscli20\\palrt\\inc\\poppack.h"





































#pragma warning(disable:4103)

#pragma pack(pop)


#line 44 "c:\\sscli20\\palrt\\inc\\poppack.h"


#line 47 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 48 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 1185 "c:\\sscli20\\palrt\\inc\\ntimage.h"

typedef struct _IMAGE_THUNK_DATA32 {
    union {
        ULONG ForwarderString;      
        ULONG Function;             
        ULONG Ordinal;
        ULONG AddressOfData;        
    } u1;
} IMAGE_THUNK_DATA32;
typedef IMAGE_THUNK_DATA32 * PIMAGE_THUNK_DATA32;












typedef VOID
(__stdcall *PIMAGE_TLS_CALLBACK) (
    PVOID DllHandle,
    ULONG Reason,
    PVOID Reserved
    );

typedef struct _IMAGE_TLS_DIRECTORY64 {
    ULONGLONG   StartAddressOfRawData;
    ULONGLONG   EndAddressOfRawData;
    ULONGLONG   AddressOfIndex;         
    ULONGLONG   AddressOfCallBacks;     
    ULONG   SizeOfZeroFill;
    ULONG   Characteristics;
} IMAGE_TLS_DIRECTORY64;
typedef IMAGE_TLS_DIRECTORY64 * PIMAGE_TLS_DIRECTORY64;

typedef struct _IMAGE_TLS_DIRECTORY32 {
    ULONG   StartAddressOfRawData;
    ULONG   EndAddressOfRawData;
    ULONG   AddressOfIndex;             
    ULONG   AddressOfCallBacks;         
    ULONG   SizeOfZeroFill;
    ULONG   Characteristics;
} IMAGE_TLS_DIRECTORY32;
typedef IMAGE_TLS_DIRECTORY32 * PIMAGE_TLS_DIRECTORY32;



typedef IMAGE_THUNK_DATA32              IMAGE_THUNK_DATA;
typedef PIMAGE_THUNK_DATA32             PIMAGE_THUNK_DATA;

typedef IMAGE_TLS_DIRECTORY32           IMAGE_TLS_DIRECTORY;
typedef PIMAGE_TLS_DIRECTORY32          PIMAGE_TLS_DIRECTORY;

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        ULONG   Characteristics;            
        ULONG   OriginalFirstThunk;         
    } u;
    ULONG   TimeDateStamp;                  
                                            
                                            
                                            

    ULONG   ForwarderChain;                 
    ULONG   Name;
    ULONG   FirstThunk;                     
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR  *PIMAGE_IMPORT_DESCRIPTOR;





typedef struct _IMAGE_BOUND_IMPORT_DESCRIPTOR {
    ULONG   TimeDateStamp;
    USHORT  OffsetModuleName;
    USHORT  NumberOfModuleForwarderRefs;

} IMAGE_BOUND_IMPORT_DESCRIPTOR,  *PIMAGE_BOUND_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_BOUND_FORWARDER_REF {
    ULONG   TimeDateStamp;
    USHORT  OffsetModuleName;
    USHORT  Reserved;
} IMAGE_BOUND_FORWARDER_REF, *PIMAGE_BOUND_FORWARDER_REF;



















typedef struct _IMAGE_RESOURCE_DIRECTORY {
    ULONG   Characteristics;
    ULONG   TimeDateStamp;
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    USHORT  NumberOfNamedEntries;
    USHORT  NumberOfIdEntries;

} IMAGE_RESOURCE_DIRECTORY, *PIMAGE_RESOURCE_DIRECTORY;


















typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY {
    union {
        struct {
            ULONG NameOffset:31;
            ULONG NameIsString:1;
        } u;
        ULONG   Name;
        USHORT  Id;
    } u;
    union {
        ULONG   OffsetToData;
        struct {
            ULONG   OffsetToDirectory:31;
            ULONG   DataIsDirectory:1;
        } v;
    } v;
} IMAGE_RESOURCE_DIRECTORY_ENTRY, *PIMAGE_RESOURCE_DIRECTORY_ENTRY;










typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING {
    USHORT  Length;
    CHAR    NameString[ 1 ];
} IMAGE_RESOURCE_DIRECTORY_STRING, *PIMAGE_RESOURCE_DIRECTORY_STRING;


typedef struct _IMAGE_RESOURCE_DIR_STRING_U {
    USHORT  Length;
    WCHAR   NameString[ 1 ];
} IMAGE_RESOURCE_DIR_STRING_U, *PIMAGE_RESOURCE_DIR_STRING_U;











typedef struct _IMAGE_RESOURCE_DATA_ENTRY {
    ULONG   OffsetToData;
    ULONG   Size;
    ULONG   CodePage;
    ULONG   Reserved;
} IMAGE_RESOURCE_DATA_ENTRY, *PIMAGE_RESOURCE_DATA_ENTRY;





typedef struct {
    ULONG   Characteristics;
    ULONG   TimeDateStamp;
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    ULONG   GlobalFlagsClear;
    ULONG   GlobalFlagsSet;
    ULONG   CriticalSectionDefaultTimeout;
    ULONG   DeCommitFreeBlockThreshold;
    ULONG   DeCommitTotalFreeThreshold;
    ULONG   LockPrefixTable;            
    ULONG   MaximumAllocationSize;
    ULONG   VirtualMemoryThreshold;
    ULONG   ProcessHeapFlags;
    ULONG   ProcessAffinityMask;
    USHORT  CSDVersion;
    USHORT  Reserved1;
    ULONG   EditList;                   
    ULONG   Reserved[ 1 ];
} IMAGE_LOAD_CONFIG_DIRECTORY32, *PIMAGE_LOAD_CONFIG_DIRECTORY32;

typedef struct {
    ULONG   Characteristics;
    ULONG   TimeDateStamp;
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    ULONG   GlobalFlagsClear;
    ULONG   GlobalFlagsSet;
    ULONG   CriticalSectionDefaultTimeout;
    ULONGLONG  DeCommitFreeBlockThreshold;
    ULONGLONG  DeCommitTotalFreeThreshold;
    ULONGLONG  LockPrefixTable;         
    ULONGLONG  MaximumAllocationSize;
    ULONGLONG  VirtualMemoryThreshold;
    ULONGLONG  ProcessAffinityMask;
    ULONG   ProcessHeapFlags;
    USHORT  CSDVersion;
    USHORT  Reserved1;
    ULONGLONG  EditList;                
    ULONG   Reserved[ 2 ];
} IMAGE_LOAD_CONFIG_DIRECTORY64, *PIMAGE_LOAD_CONFIG_DIRECTORY64;

typedef IMAGE_LOAD_CONFIG_DIRECTORY32   IMAGE_LOAD_CONFIG_DIRECTORY;
typedef PIMAGE_LOAD_CONFIG_DIRECTORY32  PIMAGE_LOAD_CONFIG_DIRECTORY;










typedef struct _IMAGE_CE_RUNTIME_FUNCTION_ENTRY {
    ULONG FuncStart;
    ULONG PrologLen : 8;
    ULONG FuncLen : 22;
    ULONG ThirtyTwoBit : 1;
    ULONG ExceptionFlag : 1;
} IMAGE_CE_RUNTIME_FUNCTION_ENTRY, * PIMAGE_CE_RUNTIME_FUNCTION_ENTRY;

typedef struct _IMAGE_ALPHA64_RUNTIME_FUNCTION_ENTRY {
    ULONGLONG BeginAddress;
    ULONGLONG EndAddress;
    ULONGLONG ExceptionHandler;
    ULONGLONG HandlerData;
    ULONGLONG PrologEndAddress;
} IMAGE_ALPHA64_RUNTIME_FUNCTION_ENTRY, *PIMAGE_ALPHA64_RUNTIME_FUNCTION_ENTRY;

typedef struct _IMAGE_ALPHA_RUNTIME_FUNCTION_ENTRY {
    ULONG BeginAddress;
    ULONG EndAddress;
    ULONG ExceptionHandler;
    ULONG HandlerData;
    ULONG PrologEndAddress;
} IMAGE_ALPHA_RUNTIME_FUNCTION_ENTRY, *PIMAGE_ALPHA_RUNTIME_FUNCTION_ENTRY;

typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY {
    ULONG BeginAddress;
    ULONG EndAddress;
    ULONG UnwindInfoAddress;
} _IMAGE_RUNTIME_FUNCTION_ENTRY, *_PIMAGE_RUNTIME_FUNCTION_ENTRY;

typedef  _IMAGE_RUNTIME_FUNCTION_ENTRY  IMAGE_IA64_RUNTIME_FUNCTION_ENTRY;
typedef _PIMAGE_RUNTIME_FUNCTION_ENTRY PIMAGE_IA64_RUNTIME_FUNCTION_ENTRY;


typedef  _IMAGE_RUNTIME_FUNCTION_ENTRY  IMAGE_RUNTIME_FUNCTION_ENTRY;
typedef _PIMAGE_RUNTIME_FUNCTION_ENTRY PIMAGE_RUNTIME_FUNCTION_ENTRY;






typedef struct _IMAGE_DEBUG_DIRECTORY {
    ULONG   Characteristics;
    ULONG   TimeDateStamp;
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    ULONG   Type;
    ULONG   SizeOfData;
    ULONG   AddressOfRawData;
    ULONG   PointerToRawData;
} IMAGE_DEBUG_DIRECTORY, *PIMAGE_DEBUG_DIRECTORY;



















typedef struct _IMAGE_COFF_SYMBOLS_HEADER {
    ULONG   NumberOfSymbols;
    ULONG   LvaToFirstSymbol;
    ULONG   NumberOfLinenumbers;
    ULONG   LvaToFirstLinenumber;
    ULONG   RvaToFirstByteOfCode;
    ULONG   RvaToLastByteOfCode;
    ULONG   RvaToFirstByteOfData;
    ULONG   RvaToLastByteOfData;
} IMAGE_COFF_SYMBOLS_HEADER, *PIMAGE_COFF_SYMBOLS_HEADER;






typedef struct _FPO_DATA {
    ULONG       ulOffStart;             
    ULONG       cbProcSize;             
    ULONG       cdwLocals;              
    USHORT      cdwParams;              
    USHORT      cbProlog : 8;           
    USHORT      cbRegs   : 3;           
    USHORT      fHasSEH  : 1;           
    USHORT      fUseBP   : 1;           
    USHORT      reserved : 1;           
    USHORT      cbFrame  : 2;           
} FPO_DATA, *PFPO_DATA;





typedef struct _IMAGE_DEBUG_MISC {
    ULONG       DataType;               
    ULONG       Length;                 
                                        
    BOOLEAN     Unicode;                
    UCHAR       Reserved[ 3 ];
    UCHAR       Data[ 1 ];              
} IMAGE_DEBUG_MISC, *PIMAGE_DEBUG_MISC;








typedef struct _IMAGE_FUNCTION_ENTRY {
    ULONG   StartingAddress;
    ULONG   EndingAddress;
    ULONG   EndOfPrologue;
} IMAGE_FUNCTION_ENTRY, *PIMAGE_FUNCTION_ENTRY;

typedef struct _IMAGE_FUNCTION_ENTRY64 {
    ULONGLONG   StartingAddress;
    ULONGLONG   EndingAddress;
    union {
        ULONGLONG   EndOfPrologue;
        ULONGLONG   UnwindInfoAddress;
    } u;
} IMAGE_FUNCTION_ENTRY64, *PIMAGE_FUNCTION_ENTRY64;





















typedef struct _IMAGE_SEPARATE_DEBUG_HEADER {
    USHORT      Signature;
    USHORT      Flags;
    USHORT      Machine;
    USHORT      Characteristics;
    ULONG       TimeDateStamp;
    ULONG       CheckSum;
    ULONG       ImageBase;
    ULONG       SizeOfImage;
    ULONG       NumberOfSections;
    ULONG       ExportedNamesSize;
    ULONG       DebugDirectorySize;
    ULONG       SectionAlignment;
    ULONG       Reserved[2];
} IMAGE_SEPARATE_DEBUG_HEADER, *PIMAGE_SEPARATE_DEBUG_HEADER;

typedef struct _NON_PAGED_DEBUG_INFO {
    USHORT      Signature;
    USHORT      Flags;
    ULONG       Size;
    USHORT      Machine;
    USHORT      Characteristics;
    ULONG       TimeDateStamp;
    ULONG       CheckSum;
    ULONG       SizeOfImage;
    ULONGLONG   ImageBase;
    
    
} NON_PAGED_DEBUG_INFO, *PNON_PAGED_DEBUG_INFO;






                                                









typedef struct _ImageArchitectureHeader {
    unsigned int AmaskValue: 1;                 
                                                
    int :7;                                     
    unsigned int AmaskShift: 8;                 
    int :16;                                    
    ULONG FirstEntryRVA;                        
} IMAGE_ARCHITECTURE_HEADER, *PIMAGE_ARCHITECTURE_HEADER;

typedef struct _ImageArchitectureEntry {
    ULONG FixupInstRVA;                         
    ULONG NewInst;                              
} IMAGE_ARCHITECTURE_ENTRY, *PIMAGE_ARCHITECTURE_ENTRY;

#line 1 "c:\\sscli20\\palrt\\inc\\poppack.h"





































#pragma warning(disable:4103)

#pragma pack(pop)


#line 44 "c:\\sscli20\\palrt\\inc\\poppack.h"


#line 47 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 48 "c:\\sscli20\\palrt\\inc\\poppack.h"
#line 1648 "c:\\sscli20\\palrt\\inc\\ntimage.h"








typedef struct IMPORT_OBJECT_HEADER {
    USHORT  Sig1;                       
    USHORT  Sig2;                       
    USHORT  Version;
    USHORT  Machine;
    ULONG   TimeDateStamp;              
    ULONG   SizeOfData;                 

    union {
        USHORT  Ordinal;                
        USHORT  Hint;
    } u;

    USHORT  Type : 2;                   
    USHORT  NameType : 3;               
    USHORT  Reserved : 11;              
} IMPORT_OBJECT_HEADER;

typedef enum IMPORT_OBJECT_TYPE
{
    IMPORT_OBJECT_CODE = 0,
    IMPORT_OBJECT_DATA = 1,
    IMPORT_OBJECT_CONST = 2,
} IMPORT_OBJECT_TYPE;

typedef enum IMPORT_OBJECT_NAME_TYPE
{
    IMPORT_OBJECT_ORDINAL = 0,          
    IMPORT_OBJECT_NAME = 1,             
    IMPORT_OBJECT_NAME_NO_PREFIX = 2,   
    IMPORT_OBJECT_NAME_UNDECORATE = 3,  
                                        
} IMPORT_OBJECT_NAME_TYPE;




























































































#line 1782 "c:\\sscli20\\palrt\\inc\\ntimage.h"

#line 1656 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"







#line 1 "c:\\sscli20\\palrt\\inc\\ccombstr.h"
































































































































































































































































#line 258 "c:\\sscli20\\palrt\\inc\\ccombstr.h"
#line 1664 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
#line 1 "c:\\sscli20\\palrt\\inc\\cstring.h"
















































































#line 82 "c:\\sscli20\\palrt\\inc\\cstring.h"
#line 1665 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"
#line 1 "c:\\sscli20\\palrt\\inc\\sscli_version.h"


























#line 28 "c:\\sscli20\\palrt\\inc\\sscli_version.h"




#line 33 "c:\\sscli20\\palrt\\inc\\sscli_version.h"








#line 42 "c:\\sscli20\\palrt\\inc\\sscli_version.h"
#line 1666 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"

#line 1668 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"

#line 1670 "c:\\sscli20\\palrt\\inc\\rotor_palrt.h"

#line 25 "c:\\sscli20\\palrt\\inc\\rpc.h"










struct IRpcStubBuffer;
struct IRpcChannelBuffer;

typedef void* PRPC_MESSAGE;
typedef void* RPC_IF_HANDLE;

#line 42 "c:\\sscli20\\palrt\\inc\\rpc.h"
#line 34 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"
#line 1 "c:\\sscli20\\palrt\\inc\\rpcndr.h"

























#line 27 "c:\\sscli20\\palrt\\inc\\rpcndr.h"

#line 29 "c:\\sscli20\\palrt\\inc\\rpcndr.h"
#line 35 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"














#pragma once
#line 51 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"

 



typedef struct ICLRDataTarget ICLRDataTarget;
#line 58 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"




typedef struct ICLRDataTarget2 ICLRDataTarget2;
#line 64 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"




typedef struct ICLRMetadataLocator ICLRMetadataLocator;
#line 70 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"




typedef struct ICLRDataEnumMemoryRegionsCallback ICLRDataEnumMemoryRegionsCallback;
#line 76 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"




typedef struct ICLRDataEnumMemoryRegions ICLRDataEnumMemoryRegions;
#line 82 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



#line 1 "c:\\sscli20\\palrt\\inc\\unknwn.h"




















#line 1 "c:\\sscli20\\palrt\\inc\\rpc.h"









































#line 22 "c:\\sscli20\\palrt\\inc\\unknwn.h"
#line 1 "c:\\sscli20\\palrt\\inc\\rpcndr.h"




























#line 23 "c:\\sscli20\\palrt\\inc\\unknwn.h"




typedef struct IUnknown IUnknown;

typedef  IUnknown *LPUNKNOWN;


 const IID IID_IUnknown;

struct IUnknown
{
    virtual HRESULT __stdcall QueryInterface( 
        const IID * riid,
        void **ppvObject) = 0;
        
    virtual ULONG __stdcall AddRef( void) = 0;
        
    virtual ULONG __stdcall Release( void) = 0;
};

#line 46 "c:\\sscli20\\palrt\\inc\\unknwn.h"





 const IID IID_IClassFactory;
    
struct IClassFactory : public IUnknown
{
    virtual HRESULT __stdcall CreateInstance( 
        IUnknown *pUnkOuter,
        const IID * riid,
        void **ppvObject) = 0;
    
    virtual HRESULT __stdcall LockServer( 
        BOOL fLock) = 0;
};

#line 65 "c:\\sscli20\\palrt\\inc\\unknwn.h"
#line 86 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"





void *  MIDL_user_allocate(size_t);
void  MIDL_user_free( void * ); 


 






typedef ULONG64 CLRDATA_ADDRESS;

 HRESULT __stdcall CLRDataCreateInstance(const IID * iid, ICLRDataTarget* target, void** iface);
typedef HRESULT (__stdcall* PFN_CLRDataCreateInstance)(const IID * iid, ICLRDataTarget* target, void** iface);


extern RPC_IF_HANDLE __MIDL_itf_clrdata_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_clrdata_0000_v0_0_s_ifspec;





 


 const IID IID_ICLRDataTarget;






























































#line 182 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"

    typedef struct ICLRDataTargetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            ICLRDataTarget * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            ICLRDataTarget * This);
        
        ULONG ( __stdcall *Release )( 
            ICLRDataTarget * This);
        
        HRESULT ( __stdcall *GetMachineType )( 
            ICLRDataTarget * This,
             ULONG32 *machineType);
        
        HRESULT ( __stdcall *GetPointerSize )( 
            ICLRDataTarget * This,
             ULONG32 *pointerSize);
        
        HRESULT ( __stdcall *GetImageBase )( 
            ICLRDataTarget * This,
             LPCWSTR imagePath,
             CLRDATA_ADDRESS *baseAddress);
        
        HRESULT ( __stdcall *ReadVirtual )( 
            ICLRDataTarget * This,
             CLRDATA_ADDRESS address,
             BYTE *buffer,
             ULONG32 bytesRequested,
             ULONG32 *bytesRead);
        
        HRESULT ( __stdcall *WriteVirtual )( 
            ICLRDataTarget * This,
             CLRDATA_ADDRESS address,
             BYTE *buffer,
             ULONG32 bytesRequested,
             ULONG32 *bytesWritten);
        
        HRESULT ( __stdcall *GetTLSValue )( 
            ICLRDataTarget * This,
             ULONG32 threadID,
             ULONG32 index,
             CLRDATA_ADDRESS *value);
        
        HRESULT ( __stdcall *SetTLSValue )( 
            ICLRDataTarget * This,
             ULONG32 threadID,
             ULONG32 index,
             CLRDATA_ADDRESS value);
        
        HRESULT ( __stdcall *GetCurrentThreadID )( 
            ICLRDataTarget * This,
             ULONG32 *threadID);
        
        HRESULT ( __stdcall *GetThreadContext )( 
            ICLRDataTarget * This,
             ULONG32 threadID,
             ULONG32 contextFlags,
             ULONG32 contextSize,
             BYTE *context);
        
        HRESULT ( __stdcall *SetThreadContext )( 
            ICLRDataTarget * This,
             ULONG32 threadID,
             ULONG32 contextSize,
             BYTE *context);
        
        HRESULT ( __stdcall *Request )( 
            ICLRDataTarget * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        END_INTERFACE
    } ICLRDataTargetVtbl;

    struct ICLRDataTarget
    {
        CONST_VTBL struct ICLRDataTargetVtbl *lpVtbl;
    };

    


















































#line 322 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



HRESULT __stdcall ICLRDataTarget_GetMachineType_Proxy( 
    ICLRDataTarget * This,
     ULONG32 *machineType);


void  ICLRDataTarget_GetMachineType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_GetPointerSize_Proxy( 
    ICLRDataTarget * This,
     ULONG32 *pointerSize);


void  ICLRDataTarget_GetPointerSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_GetImageBase_Proxy( 
    ICLRDataTarget * This,
     LPCWSTR imagePath,
     CLRDATA_ADDRESS *baseAddress);


void  ICLRDataTarget_GetImageBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_ReadVirtual_Proxy( 
    ICLRDataTarget * This,
     CLRDATA_ADDRESS address,
     BYTE *buffer,
     ULONG32 bytesRequested,
     ULONG32 *bytesRead);


void  ICLRDataTarget_ReadVirtual_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_WriteVirtual_Proxy( 
    ICLRDataTarget * This,
     CLRDATA_ADDRESS address,
     BYTE *buffer,
     ULONG32 bytesRequested,
     ULONG32 *bytesWritten);


void  ICLRDataTarget_WriteVirtual_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_GetTLSValue_Proxy( 
    ICLRDataTarget * This,
     ULONG32 threadID,
     ULONG32 index,
     CLRDATA_ADDRESS *value);


void  ICLRDataTarget_GetTLSValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_SetTLSValue_Proxy( 
    ICLRDataTarget * This,
     ULONG32 threadID,
     ULONG32 index,
     CLRDATA_ADDRESS value);


void  ICLRDataTarget_SetTLSValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_GetCurrentThreadID_Proxy( 
    ICLRDataTarget * This,
     ULONG32 *threadID);


void  ICLRDataTarget_GetCurrentThreadID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_GetThreadContext_Proxy( 
    ICLRDataTarget * This,
     ULONG32 threadID,
     ULONG32 contextFlags,
     ULONG32 contextSize,
     BYTE *context);


void  ICLRDataTarget_GetThreadContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_SetThreadContext_Proxy( 
    ICLRDataTarget * This,
     ULONG32 threadID,
     ULONG32 contextSize,
     BYTE *context);


void  ICLRDataTarget_SetThreadContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget_Request_Proxy( 
    ICLRDataTarget * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  ICLRDataTarget_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 479 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"






 


 const IID IID_ICLRDataTarget2;





















#line 511 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"

    typedef struct ICLRDataTarget2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            ICLRDataTarget2 * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            ICLRDataTarget2 * This);
        
        ULONG ( __stdcall *Release )( 
            ICLRDataTarget2 * This);
        
        HRESULT ( __stdcall *GetMachineType )( 
            ICLRDataTarget2 * This,
             ULONG32 *machineType);
        
        HRESULT ( __stdcall *GetPointerSize )( 
            ICLRDataTarget2 * This,
             ULONG32 *pointerSize);
        
        HRESULT ( __stdcall *GetImageBase )( 
            ICLRDataTarget2 * This,
             LPCWSTR imagePath,
             CLRDATA_ADDRESS *baseAddress);
        
        HRESULT ( __stdcall *ReadVirtual )( 
            ICLRDataTarget2 * This,
             CLRDATA_ADDRESS address,
             BYTE *buffer,
             ULONG32 bytesRequested,
             ULONG32 *bytesRead);
        
        HRESULT ( __stdcall *WriteVirtual )( 
            ICLRDataTarget2 * This,
             CLRDATA_ADDRESS address,
             BYTE *buffer,
             ULONG32 bytesRequested,
             ULONG32 *bytesWritten);
        
        HRESULT ( __stdcall *GetTLSValue )( 
            ICLRDataTarget2 * This,
             ULONG32 threadID,
             ULONG32 index,
             CLRDATA_ADDRESS *value);
        
        HRESULT ( __stdcall *SetTLSValue )( 
            ICLRDataTarget2 * This,
             ULONG32 threadID,
             ULONG32 index,
             CLRDATA_ADDRESS value);
        
        HRESULT ( __stdcall *GetCurrentThreadID )( 
            ICLRDataTarget2 * This,
             ULONG32 *threadID);
        
        HRESULT ( __stdcall *GetThreadContext )( 
            ICLRDataTarget2 * This,
             ULONG32 threadID,
             ULONG32 contextFlags,
             ULONG32 contextSize,
             BYTE *context);
        
        HRESULT ( __stdcall *SetThreadContext )( 
            ICLRDataTarget2 * This,
             ULONG32 threadID,
             ULONG32 contextSize,
             BYTE *context);
        
        HRESULT ( __stdcall *Request )( 
            ICLRDataTarget2 * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *AllocVirtual )( 
            ICLRDataTarget2 * This,
             CLRDATA_ADDRESS addr,
             ULONG32 size,
             ULONG32 typeFlags,
             ULONG32 protectFlags,
             CLRDATA_ADDRESS *virt);
        
        HRESULT ( __stdcall *FreeVirtual )( 
            ICLRDataTarget2 * This,
             CLRDATA_ADDRESS addr,
             ULONG32 size,
             ULONG32 typeFlags);
        
        END_INTERFACE
    } ICLRDataTarget2Vtbl;

    struct ICLRDataTarget2
    {
        CONST_VTBL struct ICLRDataTarget2Vtbl *lpVtbl;
    };

    

























































#line 672 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



HRESULT __stdcall ICLRDataTarget2_AllocVirtual_Proxy( 
    ICLRDataTarget2 * This,
     CLRDATA_ADDRESS addr,
     ULONG32 size,
     ULONG32 typeFlags,
     ULONG32 protectFlags,
     CLRDATA_ADDRESS *virt);


void  ICLRDataTarget2_AllocVirtual_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall ICLRDataTarget2_FreeVirtual_Proxy( 
    ICLRDataTarget2 * This,
     CLRDATA_ADDRESS addr,
     ULONG32 size,
     ULONG32 typeFlags);


void  ICLRDataTarget2_FreeVirtual_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 707 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"






 


 const IID IID_ICLRMetadataLocator;




















#line 738 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"

    typedef struct ICLRMetadataLocatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            ICLRMetadataLocator * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            ICLRMetadataLocator * This);
        
        ULONG ( __stdcall *Release )( 
            ICLRMetadataLocator * This);
        
        HRESULT ( __stdcall *GetMetadata )( 
            ICLRMetadataLocator * This,
             LPCWSTR imagePath,
             ULONG32 imageTimestamp,
             ULONG32 imageSize,
             GUID *mvid,
             ULONG32 mdRva,
             ULONG32 flags,
             ULONG32 bufferSize,
             BYTE *buffer,
             ULONG32 *dataSize);
        
        END_INTERFACE
    } ICLRMetadataLocatorVtbl;

    struct ICLRMetadataLocator
    {
        CONST_VTBL struct ICLRMetadataLocatorVtbl *lpVtbl;
    };

    




















#line 796 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



HRESULT __stdcall ICLRMetadataLocator_GetMetadata_Proxy( 
    ICLRMetadataLocator * This,
     LPCWSTR imagePath,
     ULONG32 imageTimestamp,
     ULONG32 imageSize,
     GUID *mvid,
     ULONG32 mdRva,
     ULONG32 flags,
     ULONG32 bufferSize,
     BYTE *buffer,
     ULONG32 *dataSize);


void  ICLRMetadataLocator_GetMetadata_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 821 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"






 


 const IID IID_ICLRDataEnumMemoryRegionsCallback;













#line 845 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"

    typedef struct ICLRDataEnumMemoryRegionsCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            ICLRDataEnumMemoryRegionsCallback * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            ICLRDataEnumMemoryRegionsCallback * This);
        
        ULONG ( __stdcall *Release )( 
            ICLRDataEnumMemoryRegionsCallback * This);
        
        HRESULT ( __stdcall *EnumMemoryRegion )( 
            ICLRDataEnumMemoryRegionsCallback * This,
             CLRDATA_ADDRESS address,
             ULONG32 size);
        
        END_INTERFACE
    } ICLRDataEnumMemoryRegionsCallbackVtbl;

    struct ICLRDataEnumMemoryRegionsCallback
    {
        CONST_VTBL struct ICLRDataEnumMemoryRegionsCallbackVtbl *lpVtbl;
    };

    




















#line 896 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



HRESULT __stdcall ICLRDataEnumMemoryRegionsCallback_EnumMemoryRegion_Proxy( 
    ICLRDataEnumMemoryRegionsCallback * This,
     CLRDATA_ADDRESS address,
     ULONG32 size);


void  ICLRDataEnumMemoryRegionsCallback_EnumMemoryRegion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 914 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



 

typedef 
enum CLRDataEnumMemoryFlags
    {	CLRDATA_ENUM_MEM_DEFAULT	= 0,
	CLRDATA_ENUM_MEM_MINI	= CLRDATA_ENUM_MEM_DEFAULT,
	CLRDATA_ENUM_MEM_HEAP	= 0x1
    } 	CLRDataEnumMemoryFlags;



extern RPC_IF_HANDLE __MIDL_itf_clrdata_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_clrdata_0012_v0_0_s_ifspec;





 


 const IID IID_ICLRDataEnumMemoryRegions;














#line 954 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"

    typedef struct ICLRDataEnumMemoryRegionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            ICLRDataEnumMemoryRegions * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            ICLRDataEnumMemoryRegions * This);
        
        ULONG ( __stdcall *Release )( 
            ICLRDataEnumMemoryRegions * This);
        
        HRESULT ( __stdcall *EnumMemoryRegions )( 
            ICLRDataEnumMemoryRegions * This,
             ICLRDataEnumMemoryRegionsCallback *callback,
             ULONG32 miniDumpFlags,
             CLRDataEnumMemoryFlags clrFlags);
        
        END_INTERFACE
    } ICLRDataEnumMemoryRegionsVtbl;

    struct ICLRDataEnumMemoryRegions
    {
        CONST_VTBL struct ICLRDataEnumMemoryRegionsVtbl *lpVtbl;
    };

    




















#line 1006 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



HRESULT __stdcall ICLRDataEnumMemoryRegions_EnumMemoryRegions_Proxy( 
    ICLRDataEnumMemoryRegions * This,
     ICLRDataEnumMemoryRegionsCallback *callback,
     ULONG32 miniDumpFlags,
     CLRDataEnumMemoryFlags clrFlags);


void  ICLRDataEnumMemoryRegions_EnumMemoryRegions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 1025 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"










#line 1036 "c:\\sscli20\\prebuilt\\idl\\clrdata.h"



#line 1593 "c:\\sscli20\\clr\\src\\inc\\daccess.h"
#line 1 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



















 



#pragma warning( disable: 4049 )  
#line 26 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"







#line 1 "c:\\sscli20\\palrt\\inc\\rpc.h"









































#line 34 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"
#line 1 "c:\\sscli20\\palrt\\inc\\rpcndr.h"




























#line 35 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"














#pragma once
#line 51 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

 



typedef struct IXCLRDataTarget3 IXCLRDataTarget3;
#line 58 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataProcess IXCLRDataProcess;
#line 64 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataAppDomain IXCLRDataAppDomain;
#line 70 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataAssembly IXCLRDataAssembly;
#line 76 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataModule IXCLRDataModule;
#line 82 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataTypeDefinition IXCLRDataTypeDefinition;
#line 88 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataTypeInstance IXCLRDataTypeInstance;
#line 94 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataMethodDefinition IXCLRDataMethodDefinition;
#line 100 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataMethodInstance IXCLRDataMethodInstance;
#line 106 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataTask IXCLRDataTask;
#line 112 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataStackWalk IXCLRDataStackWalk;
#line 118 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataFrame IXCLRDataFrame;
#line 124 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataExceptionState IXCLRDataExceptionState;
#line 130 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataValue IXCLRDataValue;
#line 136 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataExceptionNotification IXCLRDataExceptionNotification;
#line 142 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"




typedef struct IXCLRDataExceptionNotification2 IXCLRDataExceptionNotification2;
#line 148 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"









void *  MIDL_user_allocate(size_t);
void  MIDL_user_free( void * ); 


 
















typedef  struct __MIDL___MIDL_itf_xclrdata_0000_0001
    {
    CLRDATA_ADDRESS startAddress;
    CLRDATA_ADDRESS endAddress;
    } 	CLRDATA_ADDRESS_RANGE;

typedef ULONG64 CLRDATA_ENUM;


typedef  
enum __MIDL___MIDL_itf_xclrdata_0000_0002
    {	CLRDATA_REQUEST_REVISION	= 0xe0000000
    } 	CLRDataGeneralRequest;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0000_0003
    {	CLRDATA_TYPE_DEFAULT	= 0,
	CLRDATA_TYPE_IS_PRIMITIVE	= 0x1,
	CLRDATA_TYPE_IS_VALUE_TYPE	= 0x2,
	CLRDATA_TYPE_IS_STRING	= 0x4,
	CLRDATA_TYPE_IS_ARRAY	= 0x8,
	CLRDATA_TYPE_IS_REFERENCE	= 0x10,
	CLRDATA_TYPE_IS_POINTER	= 0x20,
	CLRDATA_TYPE_IS_ENUM	= 0x40,
	CLRDATA_TYPE_ALL_KINDS	= 0x7f
    } 	CLRDataTypeFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0000_0004
    {	CLRDATA_FIELD_DEFAULT	= 0,
	CLRDATA_FIELD_IS_PRIMITIVE	= CLRDATA_TYPE_IS_PRIMITIVE,
	CLRDATA_FIELD_IS_VALUE_TYPE	= CLRDATA_TYPE_IS_VALUE_TYPE,
	CLRDATA_FIELD_IS_STRING	= CLRDATA_TYPE_IS_STRING,
	CLRDATA_FIELD_IS_ARRAY	= CLRDATA_TYPE_IS_ARRAY,
	CLRDATA_FIELD_IS_REFERENCE	= CLRDATA_TYPE_IS_REFERENCE,
	CLRDATA_FIELD_IS_POINTER	= CLRDATA_TYPE_IS_POINTER,
	CLRDATA_FIELD_IS_ENUM	= CLRDATA_TYPE_IS_ENUM,
	CLRDATA_FIELD_ALL_KINDS	= CLRDATA_TYPE_ALL_KINDS,
	CLRDATA_FIELD_IS_INHERITED	= 0x80,
	CLRDATA_FIELD_IS_LITERAL	= 0x100,
	CLRDATA_FIELD_FROM_INSTANCE	= 0x200,
	CLRDATA_FIELD_FROM_TASK_LOCAL	= 0x400,
	CLRDATA_FIELD_FROM_STATIC	= 0x800,
	CLRDATA_FIELD_ALL_LOCATIONS	= 0xe00,
	CLRDATA_FIELD_ALL_FIELDS	= 0xeff
    } 	CLRDataFieldFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0000_0005
    {	CLRDATA_VALUE_DEFAULT	= 0,
	CLRDATA_VALUE_IS_PRIMITIVE	= CLRDATA_TYPE_IS_PRIMITIVE,
	CLRDATA_VALUE_IS_VALUE_TYPE	= CLRDATA_TYPE_IS_VALUE_TYPE,
	CLRDATA_VALUE_IS_STRING	= CLRDATA_TYPE_IS_STRING,
	CLRDATA_VALUE_IS_ARRAY	= CLRDATA_TYPE_IS_ARRAY,
	CLRDATA_VALUE_IS_REFERENCE	= CLRDATA_TYPE_IS_REFERENCE,
	CLRDATA_VALUE_IS_POINTER	= CLRDATA_TYPE_IS_POINTER,
	CLRDATA_VALUE_IS_ENUM	= CLRDATA_TYPE_IS_ENUM,
	CLRDATA_VALUE_ALL_KINDS	= CLRDATA_TYPE_ALL_KINDS,
	CLRDATA_VALUE_IS_INHERITED	= CLRDATA_FIELD_IS_INHERITED,
	CLRDATA_VALUE_IS_LITERAL	= CLRDATA_FIELD_IS_LITERAL,
	CLRDATA_VALUE_FROM_INSTANCE	= CLRDATA_FIELD_FROM_INSTANCE,
	CLRDATA_VALUE_FROM_TASK_LOCAL	= CLRDATA_FIELD_FROM_TASK_LOCAL,
	CLRDATA_VALUE_FROM_STATIC	= CLRDATA_FIELD_FROM_STATIC,
	CLRDATA_VALUE_ALL_LOCATIONS	= CLRDATA_FIELD_ALL_LOCATIONS,
	CLRDATA_VALUE_ALL_FIELDS	= CLRDATA_FIELD_ALL_FIELDS,
	CLRDATA_VALUE_IS_BOXED	= 0x1000
    } 	CLRDataValueFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0000_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataTarget3;




















#line 280 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataTarget3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataTarget3 * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataTarget3 * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataTarget3 * This);
        
        HRESULT ( __stdcall *GetMachineType )( 
            IXCLRDataTarget3 * This,
             ULONG32 *machineType);
        
        HRESULT ( __stdcall *GetPointerSize )( 
            IXCLRDataTarget3 * This,
             ULONG32 *pointerSize);
        
        HRESULT ( __stdcall *GetImageBase )( 
            IXCLRDataTarget3 * This,
             LPCWSTR imagePath,
             CLRDATA_ADDRESS *baseAddress);
        
        HRESULT ( __stdcall *ReadVirtual )( 
            IXCLRDataTarget3 * This,
             CLRDATA_ADDRESS address,
             BYTE *buffer,
             ULONG32 bytesRequested,
             ULONG32 *bytesRead);
        
        HRESULT ( __stdcall *WriteVirtual )( 
            IXCLRDataTarget3 * This,
             CLRDATA_ADDRESS address,
             BYTE *buffer,
             ULONG32 bytesRequested,
             ULONG32 *bytesWritten);
        
        HRESULT ( __stdcall *GetTLSValue )( 
            IXCLRDataTarget3 * This,
             ULONG32 threadID,
             ULONG32 index,
             CLRDATA_ADDRESS *value);
        
        HRESULT ( __stdcall *SetTLSValue )( 
            IXCLRDataTarget3 * This,
             ULONG32 threadID,
             ULONG32 index,
             CLRDATA_ADDRESS value);
        
        HRESULT ( __stdcall *GetCurrentThreadID )( 
            IXCLRDataTarget3 * This,
             ULONG32 *threadID);
        
        HRESULT ( __stdcall *GetThreadContext )( 
            IXCLRDataTarget3 * This,
             ULONG32 threadID,
             ULONG32 contextFlags,
             ULONG32 contextSize,
             BYTE *context);
        
        HRESULT ( __stdcall *SetThreadContext )( 
            IXCLRDataTarget3 * This,
             ULONG32 threadID,
             ULONG32 contextSize,
             BYTE *context);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataTarget3 * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *AllocVirtual )( 
            IXCLRDataTarget3 * This,
             CLRDATA_ADDRESS addr,
             ULONG32 size,
             ULONG32 typeFlags,
             ULONG32 protectFlags,
             CLRDATA_ADDRESS *virt);
        
        HRESULT ( __stdcall *FreeVirtual )( 
            IXCLRDataTarget3 * This,
             CLRDATA_ADDRESS addr,
             ULONG32 size,
             ULONG32 typeFlags);
        
        HRESULT ( __stdcall *GetMetaData )( 
            IXCLRDataTarget3 * This,
             LPCWSTR imagePath,
             ULONG32 imageTimestamp,
             ULONG32 imageSize,
             GUID *mvid,
             ULONG32 mdRva,
             ULONG32 flags,
             ULONG32 bufferSize,
             BYTE *buffer,
             ULONG32 *dataSize);
        
        END_INTERFACE
    } IXCLRDataTarget3Vtbl;

    struct IXCLRDataTarget3
    {
        CONST_VTBL struct IXCLRDataTarget3Vtbl *lpVtbl;
    };

    





























































#line 457 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataTarget3_GetMetaData_Proxy( 
    IXCLRDataTarget3 * This,
     LPCWSTR imagePath,
     ULONG32 imageTimestamp,
     ULONG32 imageSize,
     GUID *mvid,
     ULONG32 mdRva,
     ULONG32 flags,
     ULONG32 bufferSize,
     BYTE *buffer,
     ULONG32 *dataSize);


void  IXCLRDataTarget3_GetMetaData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 482 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0015_0001
    {	CLRDATA_BYNAME_CASE_SENSITIVE	= 0,
	CLRDATA_BYNAME_CASE_INSENSITIVE	= 0x1
    } 	CLRDataByNameFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0015_0002
    {	CLRDATA_GETNAME_DEFAULT	= 0,
	CLRDATA_GETNAME_NO_NAMESPACES	= 0x1,
	CLRDATA_GETNAME_NO_PARAMETERS	= 0x2
    } 	CLRDataGetNameFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0015_0003
    {	CLRDATA_PROCESS_DEFAULT	= 0,
	CLRDATA_PROCESS_IN_GC	= 0x1
    } 	CLRDataProcessFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0015_0004
    {	CLRDATA_ADDRESS_UNRECOGNIZED	= 0,
	CLRDATA_ADDRESS_MANAGED_METHOD	= CLRDATA_ADDRESS_UNRECOGNIZED + 1,
	CLRDATA_ADDRESS_RUNTIME_MANAGED_CODE	= CLRDATA_ADDRESS_MANAGED_METHOD + 1,
	CLRDATA_ADDRESS_RUNTIME_UNMANAGED_CODE	= CLRDATA_ADDRESS_RUNTIME_MANAGED_CODE + 1,
	CLRDATA_ADDRESS_GC_DATA	= CLRDATA_ADDRESS_RUNTIME_UNMANAGED_CODE + 1,
	CLRDATA_ADDRESS_RUNTIME_MANAGED_STUB	= CLRDATA_ADDRESS_GC_DATA + 1,
	CLRDATA_ADDRESS_RUNTIME_UNMANAGED_STUB	= CLRDATA_ADDRESS_RUNTIME_MANAGED_STUB + 1
    } 	CLRDataAddressType;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0015_0005
    {	CLRDATA_NOTIFY_ON_MODULE_LOAD	= 0x1,
	CLRDATA_NOTIFY_ON_MODULE_UNLOAD	= 0x2,
	CLRDATA_NOTIFY_ON_EXCEPTION	= 0x4
    } 	CLRDataOtherNotifyFlag;

typedef  struct __MIDL___MIDL_itf_xclrdata_0015_0006
    {
    ULONG64 Data[ 8 ];
    } 	CLRDATA_FOLLOW_STUB_BUFFER;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0015_0007
    {	CLRDATA_FOLLOW_STUB_DEFAULT	= 0
    } 	CLRDataFollowStubInFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0015_0008
    {	CLRDATA_FOLLOW_STUB_INTERMEDIATE	= 0,
	CLRDATA_FOLLOW_STUB_EXIT	= 0x1
    } 	CLRDataFollowStubOutFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0015_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0015_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataProcess;



















































































































































































































#line 765 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataProcessVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataProcess * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataProcess * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataProcess * This);
        
        HRESULT ( __stdcall *Flush )( 
            IXCLRDataProcess * This);
        
        HRESULT ( __stdcall *StartEnumTasks )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumTask )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTask **task);
        
        HRESULT ( __stdcall *EndEnumTasks )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetTaskByOSThreadID )( 
            IXCLRDataProcess * This,
             ULONG32 osThreadID,
             IXCLRDataTask **task);
        
        HRESULT ( __stdcall *GetTaskByUniqueID )( 
            IXCLRDataProcess * This,
             ULONG64 taskID,
             IXCLRDataTask **task);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataProcess * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataProcess * This,
             IXCLRDataProcess *process);
        
        HRESULT ( __stdcall *GetManagedObject )( 
            IXCLRDataProcess * This,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *GetDesiredExecutionState )( 
            IXCLRDataProcess * This,
             ULONG32 *state);
        
        HRESULT ( __stdcall *SetDesiredExecutionState )( 
            IXCLRDataProcess * This,
             ULONG32 state);
        
        HRESULT ( __stdcall *GetAddressType )( 
            IXCLRDataProcess * This,
             CLRDATA_ADDRESS address,
             CLRDataAddressType *type);
        
        HRESULT ( __stdcall *GetRuntimeNameByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ADDRESS address,
             ULONG32 flags,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             CLRDATA_ADDRESS *displacement);
        
        HRESULT ( __stdcall *StartEnumAppDomains )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumAppDomain )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle,
             IXCLRDataAppDomain **appDomain);
        
        HRESULT ( __stdcall *EndEnumAppDomains )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetAppDomainByUniqueID )( 
            IXCLRDataProcess * This,
             ULONG64 id,
             IXCLRDataAppDomain **appDomain);
        
        HRESULT ( __stdcall *StartEnumAssemblies )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumAssembly )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle,
             IXCLRDataAssembly **assembly);
        
        HRESULT ( __stdcall *EndEnumAssemblies )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumModules )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumModule )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *EndEnumModules )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetModuleByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ADDRESS address,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *StartEnumMethodInstancesByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ADDRESS address,
             IXCLRDataAppDomain *appDomain,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodInstanceByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodInstance **method);
        
        HRESULT ( __stdcall *EndEnumMethodInstancesByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetDataByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ADDRESS address,
             ULONG32 flags,
             IXCLRDataAppDomain *appDomain,
             IXCLRDataTask *tlsTask,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             IXCLRDataValue **value,
             CLRDATA_ADDRESS *displacement);
        
        HRESULT ( __stdcall *GetExceptionStateByExceptionRecord )( 
            IXCLRDataProcess * This,
             EXCEPTION_RECORD64 *record,
             IXCLRDataExceptionState **exState);
        
        HRESULT ( __stdcall *TranslateExceptionRecordToNotification )( 
            IXCLRDataProcess * This,
             EXCEPTION_RECORD64 *record,
             IXCLRDataExceptionNotification *notify);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataProcess * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *CreateMemoryValue )( 
            IXCLRDataProcess * This,
             IXCLRDataAppDomain *appDomain,
             IXCLRDataTask *tlsTask,
             IXCLRDataTypeInstance *type,
             CLRDATA_ADDRESS addr,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *SetAllTypeNotifications )( 
            IXCLRDataProcess * This,
            IXCLRDataModule *mod,
            ULONG32 flags);
        
        HRESULT ( __stdcall *SetAllCodeNotifications )( 
            IXCLRDataProcess * This,
            IXCLRDataModule *mod,
            ULONG32 flags);
        
        HRESULT ( __stdcall *GetTypeNotifications )( 
            IXCLRDataProcess * This,
             ULONG32 numTokens,
             IXCLRDataModule *mods[  ],
             IXCLRDataModule *singleMod,
             mdTypeDef tokens[  ],
             ULONG32 flags[  ]);
        
        HRESULT ( __stdcall *SetTypeNotifications )( 
            IXCLRDataProcess * This,
             ULONG32 numTokens,
             IXCLRDataModule *mods[  ],
             IXCLRDataModule *singleMod,
             mdTypeDef tokens[  ],
             ULONG32 flags[  ],
             ULONG32 singleFlags);
        
        HRESULT ( __stdcall *GetCodeNotifications )( 
            IXCLRDataProcess * This,
             ULONG32 numTokens,
             IXCLRDataModule *mods[  ],
             IXCLRDataModule *singleMod,
             mdMethodDef tokens[  ],
             ULONG32 flags[  ]);
        
        HRESULT ( __stdcall *SetCodeNotifications )( 
            IXCLRDataProcess * This,
             ULONG32 numTokens,
             IXCLRDataModule *mods[  ],
             IXCLRDataModule *singleMod,
             mdMethodDef tokens[  ],
             ULONG32 flags[  ],
             ULONG32 singleFlags);
        
        HRESULT ( __stdcall *GetOtherNotificationFlags )( 
            IXCLRDataProcess * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *SetOtherNotificationFlags )( 
            IXCLRDataProcess * This,
             ULONG32 flags);
        
        HRESULT ( __stdcall *StartEnumMethodDefinitionsByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ADDRESS address,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodDefinitionByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodDefinition **method);
        
        HRESULT ( __stdcall *EndEnumMethodDefinitionsByAddress )( 
            IXCLRDataProcess * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *FollowStub )( 
            IXCLRDataProcess * This,
             ULONG32 inFlags,
             CLRDATA_ADDRESS inAddr,
             CLRDATA_FOLLOW_STUB_BUFFER *inBuffer,
             CLRDATA_ADDRESS *outAddr,
             CLRDATA_FOLLOW_STUB_BUFFER *outBuffer,
             ULONG32 *outFlags);
        
        HRESULT ( __stdcall *FollowStub2 )( 
            IXCLRDataProcess * This,
             IXCLRDataTask *task,
             ULONG32 inFlags,
             CLRDATA_ADDRESS inAddr,
             CLRDATA_FOLLOW_STUB_BUFFER *inBuffer,
             CLRDATA_ADDRESS *outAddr,
             CLRDATA_FOLLOW_STUB_BUFFER *outBuffer,
             ULONG32 *outFlags);
        
        END_INTERFACE
    } IXCLRDataProcessVtbl;

    struct IXCLRDataProcess
    {
        CONST_VTBL struct IXCLRDataProcessVtbl *lpVtbl;
    };

    
























































































































































#line 1190 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataProcess_Flush_Proxy( 
    IXCLRDataProcess * This);


void  IXCLRDataProcess_Flush_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_StartEnumTasks_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataProcess_StartEnumTasks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EnumTask_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTask **task);


void  IXCLRDataProcess_EnumTask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EndEnumTasks_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM handle);


void  IXCLRDataProcess_EndEnumTasks_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetTaskByOSThreadID_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 osThreadID,
     IXCLRDataTask **task);


void  IXCLRDataProcess_GetTaskByOSThreadID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetTaskByUniqueID_Proxy( 
    IXCLRDataProcess * This,
     ULONG64 taskID,
     IXCLRDataTask **task);


void  IXCLRDataProcess_GetTaskByUniqueID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetFlags_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 *flags);


void  IXCLRDataProcess_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_IsSameObject_Proxy( 
    IXCLRDataProcess * This,
     IXCLRDataProcess *process);


void  IXCLRDataProcess_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetManagedObject_Proxy( 
    IXCLRDataProcess * This,
     IXCLRDataValue **value);


void  IXCLRDataProcess_GetManagedObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetDesiredExecutionState_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 *state);


void  IXCLRDataProcess_GetDesiredExecutionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_SetDesiredExecutionState_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 state);


void  IXCLRDataProcess_SetDesiredExecutionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetAddressType_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ADDRESS address,
     CLRDataAddressType *type);


void  IXCLRDataProcess_GetAddressType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetRuntimeNameByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ADDRESS address,
     ULONG32 flags,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     CLRDATA_ADDRESS *displacement);


void  IXCLRDataProcess_GetRuntimeNameByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_StartEnumAppDomains_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataProcess_StartEnumAppDomains_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EnumAppDomain_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle,
     IXCLRDataAppDomain **appDomain);


void  IXCLRDataProcess_EnumAppDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EndEnumAppDomains_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM handle);


void  IXCLRDataProcess_EndEnumAppDomains_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetAppDomainByUniqueID_Proxy( 
    IXCLRDataProcess * This,
     ULONG64 id,
     IXCLRDataAppDomain **appDomain);


void  IXCLRDataProcess_GetAppDomainByUniqueID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_StartEnumAssemblies_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataProcess_StartEnumAssemblies_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EnumAssembly_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle,
     IXCLRDataAssembly **assembly);


void  IXCLRDataProcess_EnumAssembly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EndEnumAssemblies_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM handle);


void  IXCLRDataProcess_EndEnumAssemblies_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_StartEnumModules_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataProcess_StartEnumModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EnumModule_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle,
     IXCLRDataModule **mod);


void  IXCLRDataProcess_EnumModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EndEnumModules_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM handle);


void  IXCLRDataProcess_EndEnumModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetModuleByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ADDRESS address,
     IXCLRDataModule **mod);


void  IXCLRDataProcess_GetModuleByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_StartEnumMethodInstancesByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ADDRESS address,
     IXCLRDataAppDomain *appDomain,
     CLRDATA_ENUM *handle);


void  IXCLRDataProcess_StartEnumMethodInstancesByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EnumMethodInstanceByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodInstance **method);


void  IXCLRDataProcess_EnumMethodInstanceByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EndEnumMethodInstancesByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM handle);


void  IXCLRDataProcess_EndEnumMethodInstancesByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetDataByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ADDRESS address,
     ULONG32 flags,
     IXCLRDataAppDomain *appDomain,
     IXCLRDataTask *tlsTask,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     IXCLRDataValue **value,
     CLRDATA_ADDRESS *displacement);


void  IXCLRDataProcess_GetDataByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetExceptionStateByExceptionRecord_Proxy( 
    IXCLRDataProcess * This,
     EXCEPTION_RECORD64 *record,
     IXCLRDataExceptionState **exState);


void  IXCLRDataProcess_GetExceptionStateByExceptionRecord_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_TranslateExceptionRecordToNotification_Proxy( 
    IXCLRDataProcess * This,
     EXCEPTION_RECORD64 *record,
     IXCLRDataExceptionNotification *notify);


void  IXCLRDataProcess_TranslateExceptionRecordToNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_Request_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataProcess_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_CreateMemoryValue_Proxy( 
    IXCLRDataProcess * This,
     IXCLRDataAppDomain *appDomain,
     IXCLRDataTask *tlsTask,
     IXCLRDataTypeInstance *type,
     CLRDATA_ADDRESS addr,
     IXCLRDataValue **value);


void  IXCLRDataProcess_CreateMemoryValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_SetAllTypeNotifications_Proxy( 
    IXCLRDataProcess * This,
    IXCLRDataModule *mod,
    ULONG32 flags);


void  IXCLRDataProcess_SetAllTypeNotifications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_SetAllCodeNotifications_Proxy( 
    IXCLRDataProcess * This,
    IXCLRDataModule *mod,
    ULONG32 flags);


void  IXCLRDataProcess_SetAllCodeNotifications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetTypeNotifications_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 numTokens,
     IXCLRDataModule *mods[  ],
     IXCLRDataModule *singleMod,
     mdTypeDef tokens[  ],
     ULONG32 flags[  ]);


void  IXCLRDataProcess_GetTypeNotifications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_SetTypeNotifications_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 numTokens,
     IXCLRDataModule *mods[  ],
     IXCLRDataModule *singleMod,
     mdTypeDef tokens[  ],
     ULONG32 flags[  ],
     ULONG32 singleFlags);


void  IXCLRDataProcess_SetTypeNotifications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetCodeNotifications_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 numTokens,
     IXCLRDataModule *mods[  ],
     IXCLRDataModule *singleMod,
     mdMethodDef tokens[  ],
     ULONG32 flags[  ]);


void  IXCLRDataProcess_GetCodeNotifications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_SetCodeNotifications_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 numTokens,
     IXCLRDataModule *mods[  ],
     IXCLRDataModule *singleMod,
     mdMethodDef tokens[  ],
     ULONG32 flags[  ],
     ULONG32 singleFlags);


void  IXCLRDataProcess_SetCodeNotifications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_GetOtherNotificationFlags_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 *flags);


void  IXCLRDataProcess_GetOtherNotificationFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_SetOtherNotificationFlags_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 flags);


void  IXCLRDataProcess_SetOtherNotificationFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_StartEnumMethodDefinitionsByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ADDRESS address,
     CLRDATA_ENUM *handle);


void  IXCLRDataProcess_StartEnumMethodDefinitionsByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EnumMethodDefinitionByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodDefinition **method);


void  IXCLRDataProcess_EnumMethodDefinitionByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_EndEnumMethodDefinitionsByAddress_Proxy( 
    IXCLRDataProcess * This,
     CLRDATA_ENUM handle);


void  IXCLRDataProcess_EndEnumMethodDefinitionsByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_FollowStub_Proxy( 
    IXCLRDataProcess * This,
     ULONG32 inFlags,
     CLRDATA_ADDRESS inAddr,
     CLRDATA_FOLLOW_STUB_BUFFER *inBuffer,
     CLRDATA_ADDRESS *outAddr,
     CLRDATA_FOLLOW_STUB_BUFFER *outBuffer,
     ULONG32 *outFlags);


void  IXCLRDataProcess_FollowStub_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataProcess_FollowStub2_Proxy( 
    IXCLRDataProcess * This,
     IXCLRDataTask *task,
     ULONG32 inFlags,
     CLRDATA_ADDRESS inAddr,
     CLRDATA_FOLLOW_STUB_BUFFER *inBuffer,
     CLRDATA_ADDRESS *outAddr,
     CLRDATA_FOLLOW_STUB_BUFFER *outBuffer,
     ULONG32 *outFlags);


void  IXCLRDataProcess_FollowStub2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 1802 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0016_0001
    {	CLRDATA_DOMAIN_DEFAULT	= 0
    } 	CLRDataAppDomainFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0016_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0016_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataAppDomain;




































#line 1862 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataAppDomainVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataAppDomain * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataAppDomain * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataAppDomain * This);
        
        HRESULT ( __stdcall *GetProcess )( 
            IXCLRDataAppDomain * This,
             IXCLRDataProcess **process);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataAppDomain * This,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetUniqueID )( 
            IXCLRDataAppDomain * This,
             ULONG64 *id);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataAppDomain * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataAppDomain * This,
             IXCLRDataAppDomain *appDomain);
        
        HRESULT ( __stdcall *GetManagedObject )( 
            IXCLRDataAppDomain * This,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataAppDomain * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        END_INTERFACE
    } IXCLRDataAppDomainVtbl;

    struct IXCLRDataAppDomain
    {
        CONST_VTBL struct IXCLRDataAppDomainVtbl *lpVtbl;
    };

    






































#line 1960 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataAppDomain_GetProcess_Proxy( 
    IXCLRDataAppDomain * This,
     IXCLRDataProcess **process);


void  IXCLRDataAppDomain_GetProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAppDomain_GetName_Proxy( 
    IXCLRDataAppDomain * This,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataAppDomain_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAppDomain_GetUniqueID_Proxy( 
    IXCLRDataAppDomain * This,
     ULONG64 *id);


void  IXCLRDataAppDomain_GetUniqueID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAppDomain_GetFlags_Proxy( 
    IXCLRDataAppDomain * This,
     ULONG32 *flags);


void  IXCLRDataAppDomain_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAppDomain_IsSameObject_Proxy( 
    IXCLRDataAppDomain * This,
     IXCLRDataAppDomain *appDomain);


void  IXCLRDataAppDomain_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAppDomain_GetManagedObject_Proxy( 
    IXCLRDataAppDomain * This,
     IXCLRDataValue **value);


void  IXCLRDataAppDomain_GetManagedObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAppDomain_Request_Proxy( 
    IXCLRDataAppDomain * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataAppDomain_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 2055 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0017_0001
    {	CLRDATA_ASSEMBLY_DEFAULT	= 0
    } 	CLRDataAssemblyFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0017_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0017_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataAssembly;

























































#line 2136 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataAssemblyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataAssembly * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataAssembly * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataAssembly * This);
        
        HRESULT ( __stdcall *StartEnumModules )( 
            IXCLRDataAssembly * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumModule )( 
            IXCLRDataAssembly * This,
             CLRDATA_ENUM *handle,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *EndEnumModules )( 
            IXCLRDataAssembly * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataAssembly * This,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetFileName )( 
            IXCLRDataAssembly * This,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataAssembly * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataAssembly * This,
             IXCLRDataAssembly *assembly);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataAssembly * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *StartEnumAppDomains )( 
            IXCLRDataAssembly * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumAppDomain )( 
            IXCLRDataAssembly * This,
             CLRDATA_ENUM *handle,
             IXCLRDataAppDomain **appDomain);
        
        HRESULT ( __stdcall *EndEnumAppDomains )( 
            IXCLRDataAssembly * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetDisplayName )( 
            IXCLRDataAssembly * This,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        END_INTERFACE
    } IXCLRDataAssemblyVtbl;

    struct IXCLRDataAssembly
    {
        CONST_VTBL struct IXCLRDataAssemblyVtbl *lpVtbl;
    };

    





















































#line 2275 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataAssembly_StartEnumModules_Proxy( 
    IXCLRDataAssembly * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataAssembly_StartEnumModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_EnumModule_Proxy( 
    IXCLRDataAssembly * This,
     CLRDATA_ENUM *handle,
     IXCLRDataModule **mod);


void  IXCLRDataAssembly_EnumModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_EndEnumModules_Proxy( 
    IXCLRDataAssembly * This,
     CLRDATA_ENUM handle);


void  IXCLRDataAssembly_EndEnumModules_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_GetName_Proxy( 
    IXCLRDataAssembly * This,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataAssembly_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_GetFileName_Proxy( 
    IXCLRDataAssembly * This,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataAssembly_GetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_GetFlags_Proxy( 
    IXCLRDataAssembly * This,
     ULONG32 *flags);


void  IXCLRDataAssembly_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_IsSameObject_Proxy( 
    IXCLRDataAssembly * This,
     IXCLRDataAssembly *assembly);


void  IXCLRDataAssembly_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_Request_Proxy( 
    IXCLRDataAssembly * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataAssembly_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_StartEnumAppDomains_Proxy( 
    IXCLRDataAssembly * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataAssembly_StartEnumAppDomains_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_EnumAppDomain_Proxy( 
    IXCLRDataAssembly * This,
     CLRDATA_ENUM *handle,
     IXCLRDataAppDomain **appDomain);


void  IXCLRDataAssembly_EnumAppDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_EndEnumAppDomains_Proxy( 
    IXCLRDataAssembly * This,
     CLRDATA_ENUM handle);


void  IXCLRDataAssembly_EndEnumAppDomains_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataAssembly_GetDisplayName_Proxy( 
    IXCLRDataAssembly * This,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataAssembly_GetDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 2436 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0018_0001
    {	CLRDATA_MODULE_DEFAULT	= 0,
	CLRDATA_MODULE_IS_DYNAMIC	= 0x1,
	CLRDATA_MODULE_IS_MEMORY_STREAM	= 0x2
    } 	CLRDataModuleFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0018_0002
    {	CLRDATA_MODULE_PE_FILE	= 0,
	CLRDATA_MODULE_PREJIT_FILE	= CLRDATA_MODULE_PE_FILE + 1,
	CLRDATA_MODULE_MEMORY_STREAM	= CLRDATA_MODULE_PREJIT_FILE + 1,
	CLRDATA_MODULE_OTHER	= CLRDATA_MODULE_MEMORY_STREAM + 1
    } 	CLRDataModuleExtentType;

typedef  struct __MIDL___MIDL_itf_xclrdata_0018_0003
    {
    CLRDATA_ADDRESS base;
    ULONG32 length;
    CLRDataModuleExtentType type;
    } 	CLRDATA_MODULE_EXTENT;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0018_0004
    {	CLRDATA_TYPENOTIFY_NONE	= 0,
	CLRDATA_TYPENOTIFY_LOADED	= 0x1,
	CLRDATA_TYPENOTIFY_UNLOADED	= 0x2
    } 	CLRDataTypeNotification;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0018_0005
    {	CLRDATA_METHNOTIFY_NONE	= 0,
	CLRDATA_METHNOTIFY_GENERATED	= 0x1,
	CLRDATA_METHNOTIFY_DISCARDED	= 0x2
    } 	CLRDataMethodCodeNotification;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0018_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0018_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataModule;






























































































































































#line 2649 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataModuleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataModule * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataModule * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataModule * This);
        
        HRESULT ( __stdcall *StartEnumAssemblies )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumAssembly )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataAssembly **assembly);
        
        HRESULT ( __stdcall *EndEnumAssemblies )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumTypeDefinitions )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumTypeDefinition )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTypeDefinition **typeDefinition);
        
        HRESULT ( __stdcall *EndEnumTypeDefinitions )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumTypeInstances )( 
            IXCLRDataModule * This,
             IXCLRDataAppDomain *appDomain,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumTypeInstance )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTypeInstance **typeInstance);
        
        HRESULT ( __stdcall *EndEnumTypeInstances )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumTypeDefinitionsByName )( 
            IXCLRDataModule * This,
             LPCWSTR name,
             ULONG32 flags,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumTypeDefinitionByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTypeDefinition **type);
        
        HRESULT ( __stdcall *EndEnumTypeDefinitionsByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumTypeInstancesByName )( 
            IXCLRDataModule * This,
             LPCWSTR name,
             ULONG32 flags,
             IXCLRDataAppDomain *appDomain,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumTypeInstanceByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTypeInstance **type);
        
        HRESULT ( __stdcall *EndEnumTypeInstancesByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetTypeDefinitionByToken )( 
            IXCLRDataModule * This,
             mdTypeDef token,
             IXCLRDataTypeDefinition **typeDefinition);
        
        HRESULT ( __stdcall *StartEnumMethodDefinitionsByName )( 
            IXCLRDataModule * This,
             LPCWSTR name,
             ULONG32 flags,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodDefinitionByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodDefinition **method);
        
        HRESULT ( __stdcall *EndEnumMethodDefinitionsByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumMethodInstancesByName )( 
            IXCLRDataModule * This,
             LPCWSTR name,
             ULONG32 flags,
             IXCLRDataAppDomain *appDomain,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodInstanceByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodInstance **method);
        
        HRESULT ( __stdcall *EndEnumMethodInstancesByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetMethodDefinitionByToken )( 
            IXCLRDataModule * This,
             mdMethodDef token,
             IXCLRDataMethodDefinition **methodDefinition);
        
        HRESULT ( __stdcall *StartEnumDataByName )( 
            IXCLRDataModule * This,
             LPCWSTR name,
             ULONG32 flags,
             IXCLRDataAppDomain *appDomain,
             IXCLRDataTask *tlsTask,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumDataByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *EndEnumDataByName )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataModule * This,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetFileName )( 
            IXCLRDataModule * This,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataModule * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataModule * This,
             IXCLRDataModule *mod);
        
        HRESULT ( __stdcall *StartEnumExtents )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumExtent )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             CLRDATA_MODULE_EXTENT *extent);
        
        HRESULT ( __stdcall *EndEnumExtents )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataModule * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *StartEnumAppDomains )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumAppDomain )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM *handle,
             IXCLRDataAppDomain **appDomain);
        
        HRESULT ( __stdcall *EndEnumAppDomains )( 
            IXCLRDataModule * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetVersionId )( 
            IXCLRDataModule * This,
             GUID *vid);
        
        END_INTERFACE
    } IXCLRDataModuleVtbl;

    struct IXCLRDataModule
    {
        CONST_VTBL struct IXCLRDataModuleVtbl *lpVtbl;
    };

    



































































































































#line 2993 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataModule_StartEnumAssemblies_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumAssemblies_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumAssembly_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataAssembly **assembly);


void  IXCLRDataModule_EnumAssembly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumAssemblies_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumAssemblies_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumTypeDefinitions_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumTypeDefinitions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumTypeDefinition_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTypeDefinition **typeDefinition);


void  IXCLRDataModule_EnumTypeDefinition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumTypeDefinitions_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumTypeDefinitions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumTypeInstances_Proxy( 
    IXCLRDataModule * This,
     IXCLRDataAppDomain *appDomain,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumTypeInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumTypeInstance_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTypeInstance **typeInstance);


void  IXCLRDataModule_EnumTypeInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumTypeInstances_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumTypeInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumTypeDefinitionsByName_Proxy( 
    IXCLRDataModule * This,
     LPCWSTR name,
     ULONG32 flags,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumTypeDefinitionsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumTypeDefinitionByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTypeDefinition **type);


void  IXCLRDataModule_EnumTypeDefinitionByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumTypeDefinitionsByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumTypeDefinitionsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumTypeInstancesByName_Proxy( 
    IXCLRDataModule * This,
     LPCWSTR name,
     ULONG32 flags,
     IXCLRDataAppDomain *appDomain,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumTypeInstancesByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumTypeInstanceByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTypeInstance **type);


void  IXCLRDataModule_EnumTypeInstanceByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumTypeInstancesByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumTypeInstancesByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_GetTypeDefinitionByToken_Proxy( 
    IXCLRDataModule * This,
     mdTypeDef token,
     IXCLRDataTypeDefinition **typeDefinition);


void  IXCLRDataModule_GetTypeDefinitionByToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumMethodDefinitionsByName_Proxy( 
    IXCLRDataModule * This,
     LPCWSTR name,
     ULONG32 flags,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumMethodDefinitionsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumMethodDefinitionByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodDefinition **method);


void  IXCLRDataModule_EnumMethodDefinitionByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumMethodDefinitionsByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumMethodDefinitionsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumMethodInstancesByName_Proxy( 
    IXCLRDataModule * This,
     LPCWSTR name,
     ULONG32 flags,
     IXCLRDataAppDomain *appDomain,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumMethodInstancesByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumMethodInstanceByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodInstance **method);


void  IXCLRDataModule_EnumMethodInstanceByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumMethodInstancesByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumMethodInstancesByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_GetMethodDefinitionByToken_Proxy( 
    IXCLRDataModule * This,
     mdMethodDef token,
     IXCLRDataMethodDefinition **methodDefinition);


void  IXCLRDataModule_GetMethodDefinitionByToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumDataByName_Proxy( 
    IXCLRDataModule * This,
     LPCWSTR name,
     ULONG32 flags,
     IXCLRDataAppDomain *appDomain,
     IXCLRDataTask *tlsTask,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumDataByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumDataByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **value);


void  IXCLRDataModule_EnumDataByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumDataByName_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumDataByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_GetName_Proxy( 
    IXCLRDataModule * This,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataModule_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_GetFileName_Proxy( 
    IXCLRDataModule * This,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataModule_GetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_GetFlags_Proxy( 
    IXCLRDataModule * This,
     ULONG32 *flags);


void  IXCLRDataModule_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_IsSameObject_Proxy( 
    IXCLRDataModule * This,
     IXCLRDataModule *mod);


void  IXCLRDataModule_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumExtents_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumExtent_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     CLRDATA_MODULE_EXTENT *extent);


void  IXCLRDataModule_EnumExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumExtents_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_Request_Proxy( 
    IXCLRDataModule * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataModule_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_StartEnumAppDomains_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataModule_StartEnumAppDomains_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EnumAppDomain_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM *handle,
     IXCLRDataAppDomain **appDomain);


void  IXCLRDataModule_EnumAppDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_EndEnumAppDomains_Proxy( 
    IXCLRDataModule * This,
     CLRDATA_ENUM handle);


void  IXCLRDataModule_EndEnumAppDomains_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataModule_GetVersionId_Proxy( 
    IXCLRDataModule * This,
     GUID *vid);


void  IXCLRDataModule_GetVersionId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 3489 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"






 


 const IID IID_IXCLRDataTypeDefinition;




























































































































































#line 3656 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataTypeDefinitionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataTypeDefinition * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataTypeDefinition * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataTypeDefinition * This);
        
        HRESULT ( __stdcall *GetModule )( 
            IXCLRDataTypeDefinition * This,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *StartEnumMethodDefinitions )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodDefinition )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodDefinition **methodDefinition);
        
        HRESULT ( __stdcall *EndEnumMethodDefinitions )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumMethodDefinitionsByName )( 
            IXCLRDataTypeDefinition * This,
             LPCWSTR name,
             ULONG32 flags,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodDefinitionByName )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodDefinition **method);
        
        HRESULT ( __stdcall *EndEnumMethodDefinitionsByName )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetMethodDefinitionByToken )( 
            IXCLRDataTypeDefinition * This,
             mdMethodDef token,
             IXCLRDataMethodDefinition **methodDefinition);
        
        HRESULT ( __stdcall *StartEnumInstances )( 
            IXCLRDataTypeDefinition * This,
             IXCLRDataAppDomain *appDomain,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumInstance )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTypeInstance **instance);
        
        HRESULT ( __stdcall *EndEnumInstances )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 flags,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        HRESULT ( __stdcall *GetTokenAndScope )( 
            IXCLRDataTypeDefinition * This,
             mdTypeDef *token,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *GetCorElementType )( 
            IXCLRDataTypeDefinition * This,
             CorElementType *type);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataTypeDefinition * This,
             IXCLRDataTypeDefinition *type);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *GetArrayRank )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 *rank);
        
        HRESULT ( __stdcall *GetBase )( 
            IXCLRDataTypeDefinition * This,
             IXCLRDataTypeDefinition **base);
        
        HRESULT ( __stdcall *GetNumFields )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 flags,
             ULONG32 *numFields);
        
        HRESULT ( __stdcall *StartEnumFields )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 flags,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumField )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle,
             ULONG32 nameBufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             IXCLRDataTypeDefinition **type,
             ULONG32 *flags,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *EndEnumFields )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumFieldsByName )( 
            IXCLRDataTypeDefinition * This,
             LPCWSTR name,
             ULONG32 nameFlags,
             ULONG32 fieldFlags,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumFieldByName )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTypeDefinition **type,
             ULONG32 *flags,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *EndEnumFieldsByName )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetFieldByToken )( 
            IXCLRDataTypeDefinition * This,
             mdFieldDef token,
             ULONG32 nameBufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             IXCLRDataTypeDefinition **type,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *GetTypeNotification )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *SetTypeNotification )( 
            IXCLRDataTypeDefinition * This,
             ULONG32 flags);
        
        HRESULT ( __stdcall *EnumField2 )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle,
             ULONG32 nameBufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             IXCLRDataTypeDefinition **type,
             ULONG32 *flags,
             IXCLRDataModule **tokenScope,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *EnumFieldByName2 )( 
            IXCLRDataTypeDefinition * This,
             CLRDATA_ENUM *handle,
             IXCLRDataTypeDefinition **type,
             ULONG32 *flags,
             IXCLRDataModule **tokenScope,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *GetFieldByToken2 )( 
            IXCLRDataTypeDefinition * This,
             IXCLRDataModule *tokenScope,
             mdFieldDef token,
             ULONG32 nameBufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             IXCLRDataTypeDefinition **type,
             ULONG32 *flags);
        
        END_INTERFACE
    } IXCLRDataTypeDefinitionVtbl;

    struct IXCLRDataTypeDefinition
    {
        CONST_VTBL struct IXCLRDataTypeDefinitionVtbl *lpVtbl;
    };

    

















































































































#line 3974 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataTypeDefinition_GetModule_Proxy( 
    IXCLRDataTypeDefinition * This,
     IXCLRDataModule **mod);


void  IXCLRDataTypeDefinition_GetModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_StartEnumMethodDefinitions_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeDefinition_StartEnumMethodDefinitions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EnumMethodDefinition_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodDefinition **methodDefinition);


void  IXCLRDataTypeDefinition_EnumMethodDefinition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EndEnumMethodDefinitions_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeDefinition_EndEnumMethodDefinitions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_StartEnumMethodDefinitionsByName_Proxy( 
    IXCLRDataTypeDefinition * This,
     LPCWSTR name,
     ULONG32 flags,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeDefinition_StartEnumMethodDefinitionsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EnumMethodDefinitionByName_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodDefinition **method);


void  IXCLRDataTypeDefinition_EnumMethodDefinitionByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EndEnumMethodDefinitionsByName_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeDefinition_EndEnumMethodDefinitionsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetMethodDefinitionByToken_Proxy( 
    IXCLRDataTypeDefinition * This,
     mdMethodDef token,
     IXCLRDataMethodDefinition **methodDefinition);


void  IXCLRDataTypeDefinition_GetMethodDefinitionByToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_StartEnumInstances_Proxy( 
    IXCLRDataTypeDefinition * This,
     IXCLRDataAppDomain *appDomain,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeDefinition_StartEnumInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EnumInstance_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTypeInstance **instance);


void  IXCLRDataTypeDefinition_EnumInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EndEnumInstances_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeDefinition_EndEnumInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetName_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 flags,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataTypeDefinition_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetTokenAndScope_Proxy( 
    IXCLRDataTypeDefinition * This,
     mdTypeDef *token,
     IXCLRDataModule **mod);


void  IXCLRDataTypeDefinition_GetTokenAndScope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetCorElementType_Proxy( 
    IXCLRDataTypeDefinition * This,
     CorElementType *type);


void  IXCLRDataTypeDefinition_GetCorElementType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetFlags_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 *flags);


void  IXCLRDataTypeDefinition_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_IsSameObject_Proxy( 
    IXCLRDataTypeDefinition * This,
     IXCLRDataTypeDefinition *type);


void  IXCLRDataTypeDefinition_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_Request_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataTypeDefinition_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetArrayRank_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 *rank);


void  IXCLRDataTypeDefinition_GetArrayRank_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetBase_Proxy( 
    IXCLRDataTypeDefinition * This,
     IXCLRDataTypeDefinition **base);


void  IXCLRDataTypeDefinition_GetBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetNumFields_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 flags,
     ULONG32 *numFields);


void  IXCLRDataTypeDefinition_GetNumFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_StartEnumFields_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 flags,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeDefinition_StartEnumFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EnumField_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle,
     ULONG32 nameBufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     IXCLRDataTypeDefinition **type,
     ULONG32 *flags,
     mdFieldDef *token);


void  IXCLRDataTypeDefinition_EnumField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EndEnumFields_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeDefinition_EndEnumFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_StartEnumFieldsByName_Proxy( 
    IXCLRDataTypeDefinition * This,
     LPCWSTR name,
     ULONG32 nameFlags,
     ULONG32 fieldFlags,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeDefinition_StartEnumFieldsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EnumFieldByName_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTypeDefinition **type,
     ULONG32 *flags,
     mdFieldDef *token);


void  IXCLRDataTypeDefinition_EnumFieldByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EndEnumFieldsByName_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeDefinition_EndEnumFieldsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetFieldByToken_Proxy( 
    IXCLRDataTypeDefinition * This,
     mdFieldDef token,
     ULONG32 nameBufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     IXCLRDataTypeDefinition **type,
     ULONG32 *flags);


void  IXCLRDataTypeDefinition_GetFieldByToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetTypeNotification_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 *flags);


void  IXCLRDataTypeDefinition_GetTypeNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_SetTypeNotification_Proxy( 
    IXCLRDataTypeDefinition * This,
     ULONG32 flags);


void  IXCLRDataTypeDefinition_SetTypeNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EnumField2_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle,
     ULONG32 nameBufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     IXCLRDataTypeDefinition **type,
     ULONG32 *flags,
     IXCLRDataModule **tokenScope,
     mdFieldDef *token);


void  IXCLRDataTypeDefinition_EnumField2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_EnumFieldByName2_Proxy( 
    IXCLRDataTypeDefinition * This,
     CLRDATA_ENUM *handle,
     IXCLRDataTypeDefinition **type,
     ULONG32 *flags,
     IXCLRDataModule **tokenScope,
     mdFieldDef *token);


void  IXCLRDataTypeDefinition_EnumFieldByName2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeDefinition_GetFieldByToken2_Proxy( 
    IXCLRDataTypeDefinition * This,
     IXCLRDataModule *tokenScope,
     mdFieldDef token,
     ULONG32 nameBufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     IXCLRDataTypeDefinition **type,
     ULONG32 *flags);


void  IXCLRDataTypeDefinition_GetFieldByToken2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 4414 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"






 


 const IID IID_IXCLRDataTypeInstance;

























































































































































#line 4578 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataTypeInstanceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataTypeInstance * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataTypeInstance * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataTypeInstance * This);
        
        HRESULT ( __stdcall *StartEnumMethodInstances )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodInstance )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodInstance **methodInstance);
        
        HRESULT ( __stdcall *EndEnumMethodInstances )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumMethodInstancesByName )( 
            IXCLRDataTypeInstance * This,
             LPCWSTR name,
             ULONG32 flags,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumMethodInstanceByName )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodInstance **method);
        
        HRESULT ( __stdcall *EndEnumMethodInstancesByName )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetNumStaticFields )( 
            IXCLRDataTypeInstance * This,
             ULONG32 *numFields);
        
        HRESULT ( __stdcall *GetStaticFieldByIndex )( 
            IXCLRDataTypeInstance * This,
             ULONG32 index,
             IXCLRDataTask *tlsTask,
             IXCLRDataValue **field,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             mdFieldDef *token);
        
        HRESULT ( __stdcall *StartEnumStaticFieldsByName )( 
            IXCLRDataTypeInstance * This,
             LPCWSTR name,
             ULONG32 flags,
             IXCLRDataTask *tlsTask,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumStaticFieldByName )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *EndEnumStaticFieldsByName )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetNumTypeArguments )( 
            IXCLRDataTypeInstance * This,
             ULONG32 *numTypeArgs);
        
        HRESULT ( __stdcall *GetTypeArgumentByIndex )( 
            IXCLRDataTypeInstance * This,
             ULONG32 index,
             IXCLRDataTypeInstance **typeArg);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataTypeInstance * This,
             ULONG32 flags,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        HRESULT ( __stdcall *GetModule )( 
            IXCLRDataTypeInstance * This,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *GetDefinition )( 
            IXCLRDataTypeInstance * This,
             IXCLRDataTypeDefinition **typeDefinition);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataTypeInstance * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataTypeInstance * This,
             IXCLRDataTypeInstance *type);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataTypeInstance * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *GetNumStaticFields2 )( 
            IXCLRDataTypeInstance * This,
             ULONG32 flags,
             ULONG32 *numFields);
        
        HRESULT ( __stdcall *StartEnumStaticFields )( 
            IXCLRDataTypeInstance * This,
             ULONG32 flags,
             IXCLRDataTask *tlsTask,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumStaticField )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *EndEnumStaticFields )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumStaticFieldsByName2 )( 
            IXCLRDataTypeInstance * This,
             LPCWSTR name,
             ULONG32 nameFlags,
             ULONG32 fieldFlags,
             IXCLRDataTask *tlsTask,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumStaticFieldByName2 )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *EndEnumStaticFieldsByName2 )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetStaticFieldByToken )( 
            IXCLRDataTypeInstance * This,
             mdFieldDef token,
             IXCLRDataTask *tlsTask,
             IXCLRDataValue **field,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        HRESULT ( __stdcall *GetBase )( 
            IXCLRDataTypeInstance * This,
             IXCLRDataTypeInstance **base);
        
        HRESULT ( __stdcall *EnumStaticField2 )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **value,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             IXCLRDataModule **tokenScope,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *EnumStaticFieldByName3 )( 
            IXCLRDataTypeInstance * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **value,
             IXCLRDataModule **tokenScope,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *GetStaticFieldByToken2 )( 
            IXCLRDataTypeInstance * This,
             IXCLRDataModule *tokenScope,
             mdFieldDef token,
             IXCLRDataTask *tlsTask,
             IXCLRDataValue **field,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        END_INTERFACE
    } IXCLRDataTypeInstanceVtbl;

    struct IXCLRDataTypeInstance
    {
        CONST_VTBL struct IXCLRDataTypeInstanceVtbl *lpVtbl;
    };

    














































































































#line 4889 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataTypeInstance_StartEnumMethodInstances_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeInstance_StartEnumMethodInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EnumMethodInstance_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodInstance **methodInstance);


void  IXCLRDataTypeInstance_EnumMethodInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EndEnumMethodInstances_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeInstance_EndEnumMethodInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_StartEnumMethodInstancesByName_Proxy( 
    IXCLRDataTypeInstance * This,
     LPCWSTR name,
     ULONG32 flags,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeInstance_StartEnumMethodInstancesByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EnumMethodInstanceByName_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodInstance **method);


void  IXCLRDataTypeInstance_EnumMethodInstanceByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EndEnumMethodInstancesByName_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeInstance_EndEnumMethodInstancesByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetNumStaticFields_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 *numFields);


void  IXCLRDataTypeInstance_GetNumStaticFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetStaticFieldByIndex_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 index,
     IXCLRDataTask *tlsTask,
     IXCLRDataValue **field,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     mdFieldDef *token);


void  IXCLRDataTypeInstance_GetStaticFieldByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_StartEnumStaticFieldsByName_Proxy( 
    IXCLRDataTypeInstance * This,
     LPCWSTR name,
     ULONG32 flags,
     IXCLRDataTask *tlsTask,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeInstance_StartEnumStaticFieldsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EnumStaticFieldByName_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **value);


void  IXCLRDataTypeInstance_EnumStaticFieldByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EndEnumStaticFieldsByName_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeInstance_EndEnumStaticFieldsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetNumTypeArguments_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 *numTypeArgs);


void  IXCLRDataTypeInstance_GetNumTypeArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetTypeArgumentByIndex_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 index,
     IXCLRDataTypeInstance **typeArg);


void  IXCLRDataTypeInstance_GetTypeArgumentByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetName_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 flags,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataTypeInstance_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetModule_Proxy( 
    IXCLRDataTypeInstance * This,
     IXCLRDataModule **mod);


void  IXCLRDataTypeInstance_GetModule_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetDefinition_Proxy( 
    IXCLRDataTypeInstance * This,
     IXCLRDataTypeDefinition **typeDefinition);


void  IXCLRDataTypeInstance_GetDefinition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetFlags_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 *flags);


void  IXCLRDataTypeInstance_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_IsSameObject_Proxy( 
    IXCLRDataTypeInstance * This,
     IXCLRDataTypeInstance *type);


void  IXCLRDataTypeInstance_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_Request_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataTypeInstance_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetNumStaticFields2_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 flags,
     ULONG32 *numFields);


void  IXCLRDataTypeInstance_GetNumStaticFields2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_StartEnumStaticFields_Proxy( 
    IXCLRDataTypeInstance * This,
     ULONG32 flags,
     IXCLRDataTask *tlsTask,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeInstance_StartEnumStaticFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EnumStaticField_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **value);


void  IXCLRDataTypeInstance_EnumStaticField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EndEnumStaticFields_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeInstance_EndEnumStaticFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_StartEnumStaticFieldsByName2_Proxy( 
    IXCLRDataTypeInstance * This,
     LPCWSTR name,
     ULONG32 nameFlags,
     ULONG32 fieldFlags,
     IXCLRDataTask *tlsTask,
     CLRDATA_ENUM *handle);


void  IXCLRDataTypeInstance_StartEnumStaticFieldsByName2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EnumStaticFieldByName2_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **value);


void  IXCLRDataTypeInstance_EnumStaticFieldByName2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EndEnumStaticFieldsByName2_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM handle);


void  IXCLRDataTypeInstance_EndEnumStaticFieldsByName2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetStaticFieldByToken_Proxy( 
    IXCLRDataTypeInstance * This,
     mdFieldDef token,
     IXCLRDataTask *tlsTask,
     IXCLRDataValue **field,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataTypeInstance_GetStaticFieldByToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetBase_Proxy( 
    IXCLRDataTypeInstance * This,
     IXCLRDataTypeInstance **base);


void  IXCLRDataTypeInstance_GetBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EnumStaticField2_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **value,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     IXCLRDataModule **tokenScope,
     mdFieldDef *token);


void  IXCLRDataTypeInstance_EnumStaticField2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_EnumStaticFieldByName3_Proxy( 
    IXCLRDataTypeInstance * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **value,
     IXCLRDataModule **tokenScope,
     mdFieldDef *token);


void  IXCLRDataTypeInstance_EnumStaticFieldByName3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTypeInstance_GetStaticFieldByToken2_Proxy( 
    IXCLRDataTypeInstance * This,
     IXCLRDataModule *tokenScope,
     mdFieldDef token,
     IXCLRDataTask *tlsTask,
     IXCLRDataValue **field,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataTypeInstance_GetStaticFieldByToken2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 5317 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0021_0001
    {	CLRDATA_SOURCE_TYPE_INVALID	= 0
    } 	CLRDataSourceType;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0021_0002
    {	CLRDATA_IL_OFFSET_NO_MAPPING	= -1,
	CLRDATA_IL_OFFSET_PROLOG	= -2,
	CLRDATA_IL_OFFSET_EPILOG	= -3
    } 	CLRDATA_IL_OFFSET_MARKER;

typedef  struct __MIDL___MIDL_itf_xclrdata_0021_0003
    {
    ULONG32 ilOffset;
    CLRDATA_ADDRESS startAddress;
    CLRDATA_ADDRESS endAddress;
    CLRDataSourceType type;
    } 	CLRDATA_IL_ADDRESS_MAP;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0021_0004
    {	CLRDATA_METHOD_DEFAULT	= 0,
	CLRDATA_METHOD_HAS_THIS	= 0x1
    } 	CLRDataMethodFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0021_0005
    {	CLRDATA_METHDEF_IL	= 0
    } 	CLRDataMethodDefinitionExtentType;

typedef  struct __MIDL___MIDL_itf_xclrdata_0021_0006
    {
    CLRDATA_ADDRESS startAddress;
    CLRDATA_ADDRESS endAddress;
    ULONG32 enCVersion;
    CLRDataMethodDefinitionExtentType type;
    } 	CLRDATA_METHDEF_EXTENT;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0021_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0021_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataMethodDefinition;







































































#line 5446 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataMethodDefinitionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataMethodDefinition * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataMethodDefinition * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataMethodDefinition * This);
        
        HRESULT ( __stdcall *GetTypeDefinition )( 
            IXCLRDataMethodDefinition * This,
             IXCLRDataTypeDefinition **typeDefinition);
        
        HRESULT ( __stdcall *StartEnumInstances )( 
            IXCLRDataMethodDefinition * This,
             IXCLRDataAppDomain *appDomain,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumInstance )( 
            IXCLRDataMethodDefinition * This,
             CLRDATA_ENUM *handle,
             IXCLRDataMethodInstance **instance);
        
        HRESULT ( __stdcall *EndEnumInstances )( 
            IXCLRDataMethodDefinition * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataMethodDefinition * This,
             ULONG32 flags,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetTokenAndScope )( 
            IXCLRDataMethodDefinition * This,
             mdMethodDef *token,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataMethodDefinition * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataMethodDefinition * This,
             IXCLRDataMethodDefinition *method);
        
        HRESULT ( __stdcall *GetLatestEnCVersion )( 
            IXCLRDataMethodDefinition * This,
             ULONG32 *version);
        
        HRESULT ( __stdcall *StartEnumExtents )( 
            IXCLRDataMethodDefinition * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumExtent )( 
            IXCLRDataMethodDefinition * This,
             CLRDATA_ENUM *handle,
             CLRDATA_METHDEF_EXTENT *extent);
        
        HRESULT ( __stdcall *EndEnumExtents )( 
            IXCLRDataMethodDefinition * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetCodeNotification )( 
            IXCLRDataMethodDefinition * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *SetCodeNotification )( 
            IXCLRDataMethodDefinition * This,
             ULONG32 flags);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataMethodDefinition * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *GetRepresentativeEntryAddress )( 
            IXCLRDataMethodDefinition * This,
             CLRDATA_ADDRESS *addr);
        
        HRESULT ( __stdcall *HasClassOrMethodInstantiation )( 
            IXCLRDataMethodDefinition * This,
             BOOL *bGeneric);
        
        END_INTERFACE
    } IXCLRDataMethodDefinitionVtbl;

    struct IXCLRDataMethodDefinition
    {
        CONST_VTBL struct IXCLRDataMethodDefinitionVtbl *lpVtbl;
    };

    




































































#line 5619 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataMethodDefinition_GetTypeDefinition_Proxy( 
    IXCLRDataMethodDefinition * This,
     IXCLRDataTypeDefinition **typeDefinition);


void  IXCLRDataMethodDefinition_GetTypeDefinition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_StartEnumInstances_Proxy( 
    IXCLRDataMethodDefinition * This,
     IXCLRDataAppDomain *appDomain,
     CLRDATA_ENUM *handle);


void  IXCLRDataMethodDefinition_StartEnumInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_EnumInstance_Proxy( 
    IXCLRDataMethodDefinition * This,
     CLRDATA_ENUM *handle,
     IXCLRDataMethodInstance **instance);


void  IXCLRDataMethodDefinition_EnumInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_EndEnumInstances_Proxy( 
    IXCLRDataMethodDefinition * This,
     CLRDATA_ENUM handle);


void  IXCLRDataMethodDefinition_EndEnumInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_GetName_Proxy( 
    IXCLRDataMethodDefinition * This,
     ULONG32 flags,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataMethodDefinition_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_GetTokenAndScope_Proxy( 
    IXCLRDataMethodDefinition * This,
     mdMethodDef *token,
     IXCLRDataModule **mod);


void  IXCLRDataMethodDefinition_GetTokenAndScope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_GetFlags_Proxy( 
    IXCLRDataMethodDefinition * This,
     ULONG32 *flags);


void  IXCLRDataMethodDefinition_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_IsSameObject_Proxy( 
    IXCLRDataMethodDefinition * This,
     IXCLRDataMethodDefinition *method);


void  IXCLRDataMethodDefinition_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_GetLatestEnCVersion_Proxy( 
    IXCLRDataMethodDefinition * This,
     ULONG32 *version);


void  IXCLRDataMethodDefinition_GetLatestEnCVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_StartEnumExtents_Proxy( 
    IXCLRDataMethodDefinition * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataMethodDefinition_StartEnumExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_EnumExtent_Proxy( 
    IXCLRDataMethodDefinition * This,
     CLRDATA_ENUM *handle,
     CLRDATA_METHDEF_EXTENT *extent);


void  IXCLRDataMethodDefinition_EnumExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_EndEnumExtents_Proxy( 
    IXCLRDataMethodDefinition * This,
     CLRDATA_ENUM handle);


void  IXCLRDataMethodDefinition_EndEnumExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_GetCodeNotification_Proxy( 
    IXCLRDataMethodDefinition * This,
     ULONG32 *flags);


void  IXCLRDataMethodDefinition_GetCodeNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_SetCodeNotification_Proxy( 
    IXCLRDataMethodDefinition * This,
     ULONG32 flags);


void  IXCLRDataMethodDefinition_SetCodeNotification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_Request_Proxy( 
    IXCLRDataMethodDefinition * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataMethodDefinition_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_GetRepresentativeEntryAddress_Proxy( 
    IXCLRDataMethodDefinition * This,
     CLRDATA_ADDRESS *addr);


void  IXCLRDataMethodDefinition_GetRepresentativeEntryAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodDefinition_HasClassOrMethodInstantiation_Proxy( 
    IXCLRDataMethodDefinition * This,
     BOOL *bGeneric);


void  IXCLRDataMethodDefinition_HasClassOrMethodInstantiation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 5839 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"






 


 const IID IID_IXCLRDataMethodInstance;














































































#line 5928 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataMethodInstanceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataMethodInstance * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataMethodInstance * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataMethodInstance * This);
        
        HRESULT ( __stdcall *GetTypeInstance )( 
            IXCLRDataMethodInstance * This,
             IXCLRDataTypeInstance **typeInstance);
        
        HRESULT ( __stdcall *GetDefinition )( 
            IXCLRDataMethodInstance * This,
             IXCLRDataMethodDefinition **methodDefinition);
        
        HRESULT ( __stdcall *GetTokenAndScope )( 
            IXCLRDataMethodInstance * This,
             mdMethodDef *token,
             IXCLRDataModule **mod);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataMethodInstance * This,
             ULONG32 flags,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataMethodInstance * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataMethodInstance * This,
             IXCLRDataMethodInstance *method);
        
        HRESULT ( __stdcall *GetEnCVersion )( 
            IXCLRDataMethodInstance * This,
             ULONG32 *version);
        
        HRESULT ( __stdcall *GetNumTypeArguments )( 
            IXCLRDataMethodInstance * This,
             ULONG32 *numTypeArgs);
        
        HRESULT ( __stdcall *GetTypeArgumentByIndex )( 
            IXCLRDataMethodInstance * This,
             ULONG32 index,
             IXCLRDataTypeInstance **typeArg);
        
        HRESULT ( __stdcall *GetILOffsetsByAddress )( 
            IXCLRDataMethodInstance * This,
             CLRDATA_ADDRESS address,
             ULONG32 offsetsLen,
             ULONG32 *offsetsNeeded,
             ULONG32 ilOffsets[  ]);
        
        HRESULT ( __stdcall *GetAddressRangesByILOffset )( 
            IXCLRDataMethodInstance * This,
             ULONG32 ilOffset,
             ULONG32 rangesLen,
             ULONG32 *rangesNeeded,
             CLRDATA_ADDRESS_RANGE addressRanges[  ]);
        
        HRESULT ( __stdcall *GetILAddressMap )( 
            IXCLRDataMethodInstance * This,
             ULONG32 mapLen,
             ULONG32 *mapNeeded,
             CLRDATA_IL_ADDRESS_MAP maps[  ]);
        
        HRESULT ( __stdcall *StartEnumExtents )( 
            IXCLRDataMethodInstance * This,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumExtent )( 
            IXCLRDataMethodInstance * This,
             CLRDATA_ENUM *handle,
             CLRDATA_ADDRESS_RANGE *extent);
        
        HRESULT ( __stdcall *EndEnumExtents )( 
            IXCLRDataMethodInstance * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataMethodInstance * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *GetRepresentativeEntryAddress )( 
            IXCLRDataMethodInstance * This,
             CLRDATA_ADDRESS *addr);
        
        END_INTERFACE
    } IXCLRDataMethodInstanceVtbl;

    struct IXCLRDataMethodInstance
    {
        CONST_VTBL struct IXCLRDataMethodInstanceVtbl *lpVtbl;
    };

    




































































#line 6108 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataMethodInstance_GetTypeInstance_Proxy( 
    IXCLRDataMethodInstance * This,
     IXCLRDataTypeInstance **typeInstance);


void  IXCLRDataMethodInstance_GetTypeInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetDefinition_Proxy( 
    IXCLRDataMethodInstance * This,
     IXCLRDataMethodDefinition **methodDefinition);


void  IXCLRDataMethodInstance_GetDefinition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetTokenAndScope_Proxy( 
    IXCLRDataMethodInstance * This,
     mdMethodDef *token,
     IXCLRDataModule **mod);


void  IXCLRDataMethodInstance_GetTokenAndScope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetName_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 flags,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataMethodInstance_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetFlags_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 *flags);


void  IXCLRDataMethodInstance_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_IsSameObject_Proxy( 
    IXCLRDataMethodInstance * This,
     IXCLRDataMethodInstance *method);


void  IXCLRDataMethodInstance_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetEnCVersion_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 *version);


void  IXCLRDataMethodInstance_GetEnCVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetNumTypeArguments_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 *numTypeArgs);


void  IXCLRDataMethodInstance_GetNumTypeArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetTypeArgumentByIndex_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 index,
     IXCLRDataTypeInstance **typeArg);


void  IXCLRDataMethodInstance_GetTypeArgumentByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetILOffsetsByAddress_Proxy( 
    IXCLRDataMethodInstance * This,
     CLRDATA_ADDRESS address,
     ULONG32 offsetsLen,
     ULONG32 *offsetsNeeded,
     ULONG32 ilOffsets[  ]);


void  IXCLRDataMethodInstance_GetILOffsetsByAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetAddressRangesByILOffset_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 ilOffset,
     ULONG32 rangesLen,
     ULONG32 *rangesNeeded,
     CLRDATA_ADDRESS_RANGE addressRanges[  ]);


void  IXCLRDataMethodInstance_GetAddressRangesByILOffset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetILAddressMap_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 mapLen,
     ULONG32 *mapNeeded,
     CLRDATA_IL_ADDRESS_MAP maps[  ]);


void  IXCLRDataMethodInstance_GetILAddressMap_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_StartEnumExtents_Proxy( 
    IXCLRDataMethodInstance * This,
     CLRDATA_ENUM *handle);


void  IXCLRDataMethodInstance_StartEnumExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_EnumExtent_Proxy( 
    IXCLRDataMethodInstance * This,
     CLRDATA_ENUM *handle,
     CLRDATA_ADDRESS_RANGE *extent);


void  IXCLRDataMethodInstance_EnumExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_EndEnumExtents_Proxy( 
    IXCLRDataMethodInstance * This,
     CLRDATA_ENUM handle);


void  IXCLRDataMethodInstance_EndEnumExtents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_Request_Proxy( 
    IXCLRDataMethodInstance * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataMethodInstance_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataMethodInstance_GetRepresentativeEntryAddress_Proxy( 
    IXCLRDataMethodInstance * This,
     CLRDATA_ADDRESS *addr);


void  IXCLRDataMethodInstance_GetRepresentativeEntryAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 6335 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0023_0001
    {	CLRDATA_TASK_DEFAULT	= 0,
	CLRDATA_TASK_WAITING_FOR_GC	= 0x1
    } 	CLRDataTaskFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0023_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0023_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataTask;




































































#line 6428 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataTaskVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataTask * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataTask * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataTask * This);
        
        HRESULT ( __stdcall *GetProcess )( 
            IXCLRDataTask * This,
             IXCLRDataProcess **process);
        
        HRESULT ( __stdcall *GetCurrentAppDomain )( 
            IXCLRDataTask * This,
             IXCLRDataAppDomain **appDomain);
        
        HRESULT ( __stdcall *GetUniqueID )( 
            IXCLRDataTask * This,
             ULONG64 *id);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataTask * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *IsSameObject )( 
            IXCLRDataTask * This,
             IXCLRDataTask *task);
        
        HRESULT ( __stdcall *GetManagedObject )( 
            IXCLRDataTask * This,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *GetDesiredExecutionState )( 
            IXCLRDataTask * This,
             ULONG32 *state);
        
        HRESULT ( __stdcall *SetDesiredExecutionState )( 
            IXCLRDataTask * This,
             ULONG32 state);
        
        HRESULT ( __stdcall *CreateStackWalk )( 
            IXCLRDataTask * This,
             ULONG32 flags,
             IXCLRDataStackWalk **stackWalk);
        
        HRESULT ( __stdcall *GetOSThreadID )( 
            IXCLRDataTask * This,
             ULONG32 *id);
        
        HRESULT ( __stdcall *GetContext )( 
            IXCLRDataTask * This,
             ULONG32 contextFlags,
             ULONG32 contextBufSize,
             ULONG32 *contextSize,
             BYTE contextBuf[  ]);
        
        HRESULT ( __stdcall *SetContext )( 
            IXCLRDataTask * This,
             ULONG32 contextSize,
             BYTE context[  ]);
        
        HRESULT ( __stdcall *GetCurrentExceptionState )( 
            IXCLRDataTask * This,
             IXCLRDataExceptionState **exception);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataTask * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *GetName )( 
            IXCLRDataTask * This,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetLastExceptionState )( 
            IXCLRDataTask * This,
             IXCLRDataExceptionState **exception);
        
        END_INTERFACE
    } IXCLRDataTaskVtbl;

    struct IXCLRDataTask
    {
        CONST_VTBL struct IXCLRDataTaskVtbl *lpVtbl;
    };

    

































































#line 6594 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataTask_GetProcess_Proxy( 
    IXCLRDataTask * This,
     IXCLRDataProcess **process);


void  IXCLRDataTask_GetProcess_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetCurrentAppDomain_Proxy( 
    IXCLRDataTask * This,
     IXCLRDataAppDomain **appDomain);


void  IXCLRDataTask_GetCurrentAppDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetUniqueID_Proxy( 
    IXCLRDataTask * This,
     ULONG64 *id);


void  IXCLRDataTask_GetUniqueID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetFlags_Proxy( 
    IXCLRDataTask * This,
     ULONG32 *flags);


void  IXCLRDataTask_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_IsSameObject_Proxy( 
    IXCLRDataTask * This,
     IXCLRDataTask *task);


void  IXCLRDataTask_IsSameObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetManagedObject_Proxy( 
    IXCLRDataTask * This,
     IXCLRDataValue **value);


void  IXCLRDataTask_GetManagedObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetDesiredExecutionState_Proxy( 
    IXCLRDataTask * This,
     ULONG32 *state);


void  IXCLRDataTask_GetDesiredExecutionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_SetDesiredExecutionState_Proxy( 
    IXCLRDataTask * This,
     ULONG32 state);


void  IXCLRDataTask_SetDesiredExecutionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_CreateStackWalk_Proxy( 
    IXCLRDataTask * This,
     ULONG32 flags,
     IXCLRDataStackWalk **stackWalk);


void  IXCLRDataTask_CreateStackWalk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetOSThreadID_Proxy( 
    IXCLRDataTask * This,
     ULONG32 *id);


void  IXCLRDataTask_GetOSThreadID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetContext_Proxy( 
    IXCLRDataTask * This,
     ULONG32 contextFlags,
     ULONG32 contextBufSize,
     ULONG32 *contextSize,
     BYTE contextBuf[  ]);


void  IXCLRDataTask_GetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_SetContext_Proxy( 
    IXCLRDataTask * This,
     ULONG32 contextSize,
     BYTE context[  ]);


void  IXCLRDataTask_SetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetCurrentExceptionState_Proxy( 
    IXCLRDataTask * This,
     IXCLRDataExceptionState **exception);


void  IXCLRDataTask_GetCurrentExceptionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_Request_Proxy( 
    IXCLRDataTask * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataTask_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetName_Proxy( 
    IXCLRDataTask * This,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataTask_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataTask_GetLastExceptionState_Proxy( 
    IXCLRDataTask * This,
     IXCLRDataExceptionState **exception);


void  IXCLRDataTask_GetLastExceptionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 6802 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0024_0001
    {	CLRDATA_SIMPFRAME_UNRECOGNIZED	= 0x1,
	CLRDATA_SIMPFRAME_MANAGED_METHOD	= 0x2,
	CLRDATA_SIMPFRAME_RUNTIME_MANAGED_CODE	= 0x4,
	CLRDATA_SIMPFRAME_RUNTIME_UNMANAGED_CODE	= 0x8
    } 	CLRDataSimpleFrameType;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0024_0002
    {	CLRDATA_DETFRAME_UNRECOGNIZED	= 0,
	CLRDATA_DETFRAME_UNKNOWN_STUB	= CLRDATA_DETFRAME_UNRECOGNIZED + 1,
	CLRDATA_DETFRAME_CLASS_INIT	= CLRDATA_DETFRAME_UNKNOWN_STUB + 1,
	CLRDATA_DETFRAME_EXCEPTION_FILTER	= CLRDATA_DETFRAME_CLASS_INIT + 1,
	CLRDATA_DETFRAME_SECURITY	= CLRDATA_DETFRAME_EXCEPTION_FILTER + 1,
	CLRDATA_DETFRAME_CONTEXT_POLICY	= CLRDATA_DETFRAME_SECURITY + 1,
	CLRDATA_DETFRAME_INTERCEPTION	= CLRDATA_DETFRAME_CONTEXT_POLICY + 1,
	CLRDATA_DETFRAME_PROCESS_START	= CLRDATA_DETFRAME_INTERCEPTION + 1,
	CLRDATA_DETFRAME_THREAD_START	= CLRDATA_DETFRAME_PROCESS_START + 1,
	CLRDATA_DETFRAME_TRANSITION_TO_MANAGED	= CLRDATA_DETFRAME_THREAD_START + 1,
	CLRDATA_DETFRAME_TRANSITION_TO_UNMANAGED	= CLRDATA_DETFRAME_TRANSITION_TO_MANAGED + 1,
	CLRDATA_DETFRAME_COM_INTEROP_STUB	= CLRDATA_DETFRAME_TRANSITION_TO_UNMANAGED + 1,
	CLRDATA_DETFRAME_DEBUGGER_EVAL	= CLRDATA_DETFRAME_COM_INTEROP_STUB + 1,
	CLRDATA_DETFRAME_CONTEXT_SWITCH	= CLRDATA_DETFRAME_DEBUGGER_EVAL + 1,
	CLRDATA_DETFRAME_FUNC_EVAL	= CLRDATA_DETFRAME_CONTEXT_SWITCH + 1,
	CLRDATA_DETFRAME_FINALLY	= CLRDATA_DETFRAME_FUNC_EVAL + 1
    } 	CLRDataDetailedFrameType;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0024_0003
    {	CLRDATA_STACK_WALK_REQUEST_SET_FIRST_FRAME	= 0xe1000000
    } 	CLRDataStackWalkRequest;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0024_0004
    {	CLRDATA_STACK_SET_UNWIND_CONTEXT	= 0,
	CLRDATA_STACK_SET_CURRENT_CONTEXT	= 0x1
    } 	CLRDataStackSetContextFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0024_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0024_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataStackWalk;











































#line 6903 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataStackWalkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataStackWalk * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataStackWalk * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataStackWalk * This);
        
        HRESULT ( __stdcall *GetContext )( 
            IXCLRDataStackWalk * This,
             ULONG32 contextFlags,
             ULONG32 contextBufSize,
             ULONG32 *contextSize,
             BYTE contextBuf[  ]);
        
        HRESULT ( __stdcall *SetContext )( 
            IXCLRDataStackWalk * This,
             ULONG32 contextSize,
             BYTE context[  ]);
        
        HRESULT ( __stdcall *Next )( 
            IXCLRDataStackWalk * This);
        
        HRESULT ( __stdcall *GetStackSizeSkipped )( 
            IXCLRDataStackWalk * This,
             ULONG64 *stackSizeSkipped);
        
        HRESULT ( __stdcall *GetFrameType )( 
            IXCLRDataStackWalk * This,
             CLRDataSimpleFrameType *simpleType,
             CLRDataDetailedFrameType *detailedType);
        
        HRESULT ( __stdcall *GetFrame )( 
            IXCLRDataStackWalk * This,
             IXCLRDataFrame **frame);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataStackWalk * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *SetContext2 )( 
            IXCLRDataStackWalk * This,
             ULONG32 flags,
             ULONG32 contextSize,
             BYTE context[  ]);
        
        END_INTERFACE
    } IXCLRDataStackWalkVtbl;

    struct IXCLRDataStackWalk
    {
        CONST_VTBL struct IXCLRDataStackWalkVtbl *lpVtbl;
    };

    









































#line 7012 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataStackWalk_GetContext_Proxy( 
    IXCLRDataStackWalk * This,
     ULONG32 contextFlags,
     ULONG32 contextBufSize,
     ULONG32 *contextSize,
     BYTE contextBuf[  ]);


void  IXCLRDataStackWalk_GetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataStackWalk_SetContext_Proxy( 
    IXCLRDataStackWalk * This,
     ULONG32 contextSize,
     BYTE context[  ]);


void  IXCLRDataStackWalk_SetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataStackWalk_Next_Proxy( 
    IXCLRDataStackWalk * This);


void  IXCLRDataStackWalk_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataStackWalk_GetStackSizeSkipped_Proxy( 
    IXCLRDataStackWalk * This,
     ULONG64 *stackSizeSkipped);


void  IXCLRDataStackWalk_GetStackSizeSkipped_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataStackWalk_GetFrameType_Proxy( 
    IXCLRDataStackWalk * This,
     CLRDataSimpleFrameType *simpleType,
     CLRDataDetailedFrameType *detailedType);


void  IXCLRDataStackWalk_GetFrameType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataStackWalk_GetFrame_Proxy( 
    IXCLRDataStackWalk * This,
     IXCLRDataFrame **frame);


void  IXCLRDataStackWalk_GetFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataStackWalk_Request_Proxy( 
    IXCLRDataStackWalk * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataStackWalk_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataStackWalk_SetContext2_Proxy( 
    IXCLRDataStackWalk * This,
     ULONG32 flags,
     ULONG32 contextSize,
     BYTE context[  ]);


void  IXCLRDataStackWalk_SetContext2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 7123 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"






 


 const IID IID_IXCLRDataFrame;

































































#line 7199 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataFrameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataFrame * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataFrame * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataFrame * This);
        
        HRESULT ( __stdcall *GetFrameType )( 
            IXCLRDataFrame * This,
             CLRDataSimpleFrameType *simpleType,
             CLRDataDetailedFrameType *detailedType);
        
        HRESULT ( __stdcall *GetContext )( 
            IXCLRDataFrame * This,
             ULONG32 contextFlags,
             ULONG32 contextBufSize,
             ULONG32 *contextSize,
             BYTE contextBuf[  ]);
        
        HRESULT ( __stdcall *GetAppDomain )( 
            IXCLRDataFrame * This,
             IXCLRDataAppDomain **appDomain);
        
        HRESULT ( __stdcall *GetNumArguments )( 
            IXCLRDataFrame * This,
             ULONG32 *numArgs);
        
        HRESULT ( __stdcall *GetArgumentByIndex )( 
            IXCLRDataFrame * This,
             ULONG32 index,
             IXCLRDataValue **arg,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetNumLocalVariables )( 
            IXCLRDataFrame * This,
             ULONG32 *numLocals);
        
        HRESULT ( __stdcall *GetLocalVariableByIndex )( 
            IXCLRDataFrame * This,
             ULONG32 index,
             IXCLRDataValue **localVariable,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR name[  ]);
        
        HRESULT ( __stdcall *GetCodeName )( 
            IXCLRDataFrame * This,
             ULONG32 flags,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        HRESULT ( __stdcall *GetMethodInstance )( 
            IXCLRDataFrame * This,
             IXCLRDataMethodInstance **method);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataFrame * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *GetNumTypeArguments )( 
            IXCLRDataFrame * This,
             ULONG32 *numTypeArgs);
        
        HRESULT ( __stdcall *GetTypeArgumentByIndex )( 
            IXCLRDataFrame * This,
             ULONG32 index,
             IXCLRDataTypeInstance **typeArg);
        
        END_INTERFACE
    } IXCLRDataFrameVtbl;

    struct IXCLRDataFrame
    {
        CONST_VTBL struct IXCLRDataFrameVtbl *lpVtbl;
    };

    





















































#line 7346 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataFrame_GetFrameType_Proxy( 
    IXCLRDataFrame * This,
     CLRDataSimpleFrameType *simpleType,
     CLRDataDetailedFrameType *detailedType);


void  IXCLRDataFrame_GetFrameType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetContext_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 contextFlags,
     ULONG32 contextBufSize,
     ULONG32 *contextSize,
     BYTE contextBuf[  ]);


void  IXCLRDataFrame_GetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetAppDomain_Proxy( 
    IXCLRDataFrame * This,
     IXCLRDataAppDomain **appDomain);


void  IXCLRDataFrame_GetAppDomain_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetNumArguments_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 *numArgs);


void  IXCLRDataFrame_GetNumArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetArgumentByIndex_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 index,
     IXCLRDataValue **arg,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataFrame_GetArgumentByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetNumLocalVariables_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 *numLocals);


void  IXCLRDataFrame_GetNumLocalVariables_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetLocalVariableByIndex_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 index,
     IXCLRDataValue **localVariable,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR name[  ]);


void  IXCLRDataFrame_GetLocalVariableByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetCodeName_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 flags,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataFrame_GetCodeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetMethodInstance_Proxy( 
    IXCLRDataFrame * This,
     IXCLRDataMethodInstance **method);


void  IXCLRDataFrame_GetMethodInstance_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_Request_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataFrame_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetNumTypeArguments_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 *numTypeArgs);


void  IXCLRDataFrame_GetNumTypeArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataFrame_GetTypeArgumentByIndex_Proxy( 
    IXCLRDataFrame * This,
     ULONG32 index,
     IXCLRDataTypeInstance **typeArg);


void  IXCLRDataFrame_GetTypeArgumentByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 7515 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0026_0001
    {	CLRDATA_EXCEPTION_DEFAULT	= 0,
	CLRDATA_EXCEPTION_NESTED	= 0x1,
	CLRDATA_EXCEPTION_PARTIAL	= 0x2
    } 	CLRDataExceptionStateFlag;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0026_0002
    {	CLRDATA_EXBASE_EXCEPTION	= 0,
	CLRDATA_EXBASE_OUT_OF_MEMORY	= CLRDATA_EXBASE_EXCEPTION + 1,
	CLRDATA_EXBASE_INVALID_ARGUMENT	= CLRDATA_EXBASE_OUT_OF_MEMORY + 1
    } 	CLRDataBaseExceptionType;

typedef  
enum __MIDL___MIDL_itf_xclrdata_0026_0003
    {	CLRDATA_EXSAME_SECOND_CHANCE	= 0,
	CLRDATA_EXSAME_FIRST_CHANCE	= 0x1
    } 	CLRDataExceptionSameFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0026_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0026_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataExceptionState;


















































#line 7604 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataExceptionStateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataExceptionState * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataExceptionState * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataExceptionState * This);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataExceptionState * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *GetPrevious )( 
            IXCLRDataExceptionState * This,
             IXCLRDataExceptionState **exState);
        
        HRESULT ( __stdcall *GetManagedObject )( 
            IXCLRDataExceptionState * This,
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *GetBaseType )( 
            IXCLRDataExceptionState * This,
             CLRDataBaseExceptionType *type);
        
        HRESULT ( __stdcall *GetCode )( 
            IXCLRDataExceptionState * This,
             ULONG32 *code);
        
        HRESULT ( __stdcall *GetString )( 
            IXCLRDataExceptionState * This,
             ULONG32 bufLen,
             ULONG32 *strLen,
             WCHAR str[  ]);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataExceptionState * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *IsSameState )( 
            IXCLRDataExceptionState * This,
             EXCEPTION_RECORD64 *exRecord,
             ULONG32 contextSize,
             BYTE cxRecord[  ]);
        
        HRESULT ( __stdcall *IsSameState2 )( 
            IXCLRDataExceptionState * This,
             ULONG32 flags,
             EXCEPTION_RECORD64 *exRecord,
             ULONG32 contextSize,
             BYTE cxRecord[  ]);
        
        HRESULT ( __stdcall *GetTask )( 
            IXCLRDataExceptionState * This,
             IXCLRDataTask **task);
        
        END_INTERFACE
    } IXCLRDataExceptionStateVtbl;

    struct IXCLRDataExceptionState
    {
        CONST_VTBL struct IXCLRDataExceptionStateVtbl *lpVtbl;
    };

    















































#line 7728 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataExceptionState_GetFlags_Proxy( 
    IXCLRDataExceptionState * This,
     ULONG32 *flags);


void  IXCLRDataExceptionState_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_GetPrevious_Proxy( 
    IXCLRDataExceptionState * This,
     IXCLRDataExceptionState **exState);


void  IXCLRDataExceptionState_GetPrevious_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_GetManagedObject_Proxy( 
    IXCLRDataExceptionState * This,
     IXCLRDataValue **value);


void  IXCLRDataExceptionState_GetManagedObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_GetBaseType_Proxy( 
    IXCLRDataExceptionState * This,
     CLRDataBaseExceptionType *type);


void  IXCLRDataExceptionState_GetBaseType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_GetCode_Proxy( 
    IXCLRDataExceptionState * This,
     ULONG32 *code);


void  IXCLRDataExceptionState_GetCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_GetString_Proxy( 
    IXCLRDataExceptionState * This,
     ULONG32 bufLen,
     ULONG32 *strLen,
     WCHAR str[  ]);


void  IXCLRDataExceptionState_GetString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_Request_Proxy( 
    IXCLRDataExceptionState * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataExceptionState_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_IsSameState_Proxy( 
    IXCLRDataExceptionState * This,
     EXCEPTION_RECORD64 *exRecord,
     ULONG32 contextSize,
     BYTE cxRecord[  ]);


void  IXCLRDataExceptionState_IsSameState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_IsSameState2_Proxy( 
    IXCLRDataExceptionState * This,
     ULONG32 flags,
     EXCEPTION_RECORD64 *exRecord,
     ULONG32 contextSize,
     BYTE cxRecord[  ]);


void  IXCLRDataExceptionState_IsSameState2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionState_GetTask_Proxy( 
    IXCLRDataExceptionState * This,
     IXCLRDataTask **task);


void  IXCLRDataExceptionState_GetTask_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 7864 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



 

typedef  
enum __MIDL___MIDL_itf_xclrdata_0027_0001
    {	CLRDATA_VLOC_MEMORY	= 0,
	CLRDATA_VLOC_REGISTER	= 0x1
    } 	ClrDataValueLocationFlag;



extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0027_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_xclrdata_0027_v0_0_s_ifspec;





 


 const IID IID_IXCLRDataValue;



















































































































































#line 8036 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataValueVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataValue * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataValue * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataValue * This);
        
        HRESULT ( __stdcall *GetFlags )( 
            IXCLRDataValue * This,
             ULONG32 *flags);
        
        HRESULT ( __stdcall *GetAddress )( 
            IXCLRDataValue * This,
             CLRDATA_ADDRESS *address);
        
        HRESULT ( __stdcall *GetSize )( 
            IXCLRDataValue * This,
             ULONG64 *size);
        
        HRESULT ( __stdcall *GetBytes )( 
            IXCLRDataValue * This,
             ULONG32 bufLen,
             ULONG32 *dataSize,
             BYTE buffer[  ]);
        
        HRESULT ( __stdcall *SetBytes )( 
            IXCLRDataValue * This,
             ULONG32 bufLen,
             ULONG32 *dataSize,
             BYTE buffer[  ]);
        
        HRESULT ( __stdcall *GetType )( 
            IXCLRDataValue * This,
             IXCLRDataTypeInstance **typeInstance);
        
        HRESULT ( __stdcall *GetNumFields )( 
            IXCLRDataValue * This,
             ULONG32 *numFields);
        
        HRESULT ( __stdcall *GetFieldByIndex )( 
            IXCLRDataValue * This,
             ULONG32 index,
             IXCLRDataValue **field,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             mdFieldDef *token);
        
        HRESULT ( __stdcall *Request )( 
            IXCLRDataValue * This,
             ULONG32 reqCode,
             ULONG32 inBufferSize,
             BYTE *inBuffer,
             ULONG32 outBufferSize,
             BYTE *outBuffer);
        
        HRESULT ( __stdcall *GetNumFields2 )( 
            IXCLRDataValue * This,
             ULONG32 flags,
             IXCLRDataTypeInstance *fromType,
             ULONG32 *numFields);
        
        HRESULT ( __stdcall *StartEnumFields )( 
            IXCLRDataValue * This,
             ULONG32 flags,
             IXCLRDataTypeInstance *fromType,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumField )( 
            IXCLRDataValue * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **field,
             ULONG32 nameBufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             mdFieldDef *token);
        
        HRESULT ( __stdcall *EndEnumFields )( 
            IXCLRDataValue * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *StartEnumFieldsByName )( 
            IXCLRDataValue * This,
             LPCWSTR name,
             ULONG32 nameFlags,
             ULONG32 fieldFlags,
             IXCLRDataTypeInstance *fromType,
             CLRDATA_ENUM *handle);
        
        HRESULT ( __stdcall *EnumFieldByName )( 
            IXCLRDataValue * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **field,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *EndEnumFieldsByName )( 
            IXCLRDataValue * This,
             CLRDATA_ENUM handle);
        
        HRESULT ( __stdcall *GetFieldByToken )( 
            IXCLRDataValue * This,
             mdFieldDef token,
             IXCLRDataValue **field,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        HRESULT ( __stdcall *GetAssociatedValue )( 
            IXCLRDataValue * This,
             IXCLRDataValue **assocValue);
        
        HRESULT ( __stdcall *GetAssociatedType )( 
            IXCLRDataValue * This,
             IXCLRDataTypeInstance **assocType);
        
        HRESULT ( __stdcall *GetString )( 
            IXCLRDataValue * This,
             ULONG32 bufLen,
             ULONG32 *strLen,
             WCHAR str[  ]);
        
        HRESULT ( __stdcall *GetArrayProperties )( 
            IXCLRDataValue * This,
             ULONG32 *rank,
             ULONG32 *totalElements,
             ULONG32 numDim,
             ULONG32 dims[  ],
             ULONG32 numBases,
             LONG32 bases[  ]);
        
        HRESULT ( __stdcall *GetArrayElement )( 
            IXCLRDataValue * This,
             ULONG32 numInd,
             LONG32 indices[  ],
             IXCLRDataValue **value);
        
        HRESULT ( __stdcall *EnumField2 )( 
            IXCLRDataValue * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **field,
             ULONG32 nameBufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ],
             IXCLRDataModule **tokenScope,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *EnumFieldByName2 )( 
            IXCLRDataValue * This,
             CLRDATA_ENUM *handle,
             IXCLRDataValue **field,
             IXCLRDataModule **tokenScope,
             mdFieldDef *token);
        
        HRESULT ( __stdcall *GetFieldByToken2 )( 
            IXCLRDataValue * This,
             IXCLRDataModule *tokenScope,
             mdFieldDef token,
             IXCLRDataValue **field,
             ULONG32 bufLen,
             ULONG32 *nameLen,
             WCHAR nameBuf[  ]);
        
        HRESULT ( __stdcall *GetNumLocations )( 
            IXCLRDataValue * This,
             ULONG32 *numLocs);
        
        HRESULT ( __stdcall *GetLocationByIndex )( 
            IXCLRDataValue * This,
             ULONG32 loc,
             ULONG32 *flags,
             CLRDATA_ADDRESS *arg);
        
        END_INTERFACE
    } IXCLRDataValueVtbl;

    struct IXCLRDataValue
    {
        CONST_VTBL struct IXCLRDataValueVtbl *lpVtbl;
    };

    


































































































#line 8325 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataValue_GetFlags_Proxy( 
    IXCLRDataValue * This,
     ULONG32 *flags);


void  IXCLRDataValue_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetAddress_Proxy( 
    IXCLRDataValue * This,
     CLRDATA_ADDRESS *address);


void  IXCLRDataValue_GetAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetSize_Proxy( 
    IXCLRDataValue * This,
     ULONG64 *size);


void  IXCLRDataValue_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetBytes_Proxy( 
    IXCLRDataValue * This,
     ULONG32 bufLen,
     ULONG32 *dataSize,
     BYTE buffer[  ]);


void  IXCLRDataValue_GetBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_SetBytes_Proxy( 
    IXCLRDataValue * This,
     ULONG32 bufLen,
     ULONG32 *dataSize,
     BYTE buffer[  ]);


void  IXCLRDataValue_SetBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetType_Proxy( 
    IXCLRDataValue * This,
     IXCLRDataTypeInstance **typeInstance);


void  IXCLRDataValue_GetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetNumFields_Proxy( 
    IXCLRDataValue * This,
     ULONG32 *numFields);


void  IXCLRDataValue_GetNumFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetFieldByIndex_Proxy( 
    IXCLRDataValue * This,
     ULONG32 index,
     IXCLRDataValue **field,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     mdFieldDef *token);


void  IXCLRDataValue_GetFieldByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_Request_Proxy( 
    IXCLRDataValue * This,
     ULONG32 reqCode,
     ULONG32 inBufferSize,
     BYTE *inBuffer,
     ULONG32 outBufferSize,
     BYTE *outBuffer);


void  IXCLRDataValue_Request_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetNumFields2_Proxy( 
    IXCLRDataValue * This,
     ULONG32 flags,
     IXCLRDataTypeInstance *fromType,
     ULONG32 *numFields);


void  IXCLRDataValue_GetNumFields2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_StartEnumFields_Proxy( 
    IXCLRDataValue * This,
     ULONG32 flags,
     IXCLRDataTypeInstance *fromType,
     CLRDATA_ENUM *handle);


void  IXCLRDataValue_StartEnumFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_EnumField_Proxy( 
    IXCLRDataValue * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **field,
     ULONG32 nameBufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     mdFieldDef *token);


void  IXCLRDataValue_EnumField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_EndEnumFields_Proxy( 
    IXCLRDataValue * This,
     CLRDATA_ENUM handle);


void  IXCLRDataValue_EndEnumFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_StartEnumFieldsByName_Proxy( 
    IXCLRDataValue * This,
     LPCWSTR name,
     ULONG32 nameFlags,
     ULONG32 fieldFlags,
     IXCLRDataTypeInstance *fromType,
     CLRDATA_ENUM *handle);


void  IXCLRDataValue_StartEnumFieldsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_EnumFieldByName_Proxy( 
    IXCLRDataValue * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **field,
     mdFieldDef *token);


void  IXCLRDataValue_EnumFieldByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_EndEnumFieldsByName_Proxy( 
    IXCLRDataValue * This,
     CLRDATA_ENUM handle);


void  IXCLRDataValue_EndEnumFieldsByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetFieldByToken_Proxy( 
    IXCLRDataValue * This,
     mdFieldDef token,
     IXCLRDataValue **field,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataValue_GetFieldByToken_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetAssociatedValue_Proxy( 
    IXCLRDataValue * This,
     IXCLRDataValue **assocValue);


void  IXCLRDataValue_GetAssociatedValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetAssociatedType_Proxy( 
    IXCLRDataValue * This,
     IXCLRDataTypeInstance **assocType);


void  IXCLRDataValue_GetAssociatedType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetString_Proxy( 
    IXCLRDataValue * This,
     ULONG32 bufLen,
     ULONG32 *strLen,
     WCHAR str[  ]);


void  IXCLRDataValue_GetString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetArrayProperties_Proxy( 
    IXCLRDataValue * This,
     ULONG32 *rank,
     ULONG32 *totalElements,
     ULONG32 numDim,
     ULONG32 dims[  ],
     ULONG32 numBases,
     LONG32 bases[  ]);


void  IXCLRDataValue_GetArrayProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetArrayElement_Proxy( 
    IXCLRDataValue * This,
     ULONG32 numInd,
     LONG32 indices[  ],
     IXCLRDataValue **value);


void  IXCLRDataValue_GetArrayElement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_EnumField2_Proxy( 
    IXCLRDataValue * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **field,
     ULONG32 nameBufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ],
     IXCLRDataModule **tokenScope,
     mdFieldDef *token);


void  IXCLRDataValue_EnumField2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_EnumFieldByName2_Proxy( 
    IXCLRDataValue * This,
     CLRDATA_ENUM *handle,
     IXCLRDataValue **field,
     IXCLRDataModule **tokenScope,
     mdFieldDef *token);


void  IXCLRDataValue_EnumFieldByName2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetFieldByToken2_Proxy( 
    IXCLRDataValue * This,
     IXCLRDataModule *tokenScope,
     mdFieldDef token,
     IXCLRDataValue **field,
     ULONG32 bufLen,
     ULONG32 *nameLen,
     WCHAR nameBuf[  ]);


void  IXCLRDataValue_GetFieldByToken2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetNumLocations_Proxy( 
    IXCLRDataValue * This,
     ULONG32 *numLocs);


void  IXCLRDataValue_GetNumLocations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataValue_GetLocationByIndex_Proxy( 
    IXCLRDataValue * This,
     ULONG32 loc,
     ULONG32 *flags,
     CLRDATA_ADDRESS *arg);


void  IXCLRDataValue_GetLocationByIndex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 8711 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"






 


 const IID IID_IXCLRDataExceptionNotification;


































#line 8756 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataExceptionNotificationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataExceptionNotification * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataExceptionNotification * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataExceptionNotification * This);
        
        HRESULT ( __stdcall *OnCodeGenerated )( 
            IXCLRDataExceptionNotification * This,
             IXCLRDataMethodInstance *method);
        
        HRESULT ( __stdcall *OnCodeDiscarded )( 
            IXCLRDataExceptionNotification * This,
             IXCLRDataMethodInstance *method);
        
        HRESULT ( __stdcall *OnProcessExecution )( 
            IXCLRDataExceptionNotification * This,
             ULONG32 state);
        
        HRESULT ( __stdcall *OnTaskExecution )( 
            IXCLRDataExceptionNotification * This,
             IXCLRDataTask *task,
             ULONG32 state);
        
        HRESULT ( __stdcall *OnModuleLoaded )( 
            IXCLRDataExceptionNotification * This,
             IXCLRDataModule *mod);
        
        HRESULT ( __stdcall *OnModuleUnloaded )( 
            IXCLRDataExceptionNotification * This,
             IXCLRDataModule *mod);
        
        HRESULT ( __stdcall *OnTypeLoaded )( 
            IXCLRDataExceptionNotification * This,
             IXCLRDataTypeInstance *typeInst);
        
        HRESULT ( __stdcall *OnTypeUnloaded )( 
            IXCLRDataExceptionNotification * This,
             IXCLRDataTypeInstance *typeInst);
        
        END_INTERFACE
    } IXCLRDataExceptionNotificationVtbl;

    struct IXCLRDataExceptionNotification
    {
        CONST_VTBL struct IXCLRDataExceptionNotificationVtbl *lpVtbl;
    };

    









































#line 8856 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataExceptionNotification_OnCodeGenerated_Proxy( 
    IXCLRDataExceptionNotification * This,
     IXCLRDataMethodInstance *method);


void  IXCLRDataExceptionNotification_OnCodeGenerated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification_OnCodeDiscarded_Proxy( 
    IXCLRDataExceptionNotification * This,
     IXCLRDataMethodInstance *method);


void  IXCLRDataExceptionNotification_OnCodeDiscarded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification_OnProcessExecution_Proxy( 
    IXCLRDataExceptionNotification * This,
     ULONG32 state);


void  IXCLRDataExceptionNotification_OnProcessExecution_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification_OnTaskExecution_Proxy( 
    IXCLRDataExceptionNotification * This,
     IXCLRDataTask *task,
     ULONG32 state);


void  IXCLRDataExceptionNotification_OnTaskExecution_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification_OnModuleLoaded_Proxy( 
    IXCLRDataExceptionNotification * This,
     IXCLRDataModule *mod);


void  IXCLRDataExceptionNotification_OnModuleLoaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification_OnModuleUnloaded_Proxy( 
    IXCLRDataExceptionNotification * This,
     IXCLRDataModule *mod);


void  IXCLRDataExceptionNotification_OnModuleUnloaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification_OnTypeLoaded_Proxy( 
    IXCLRDataExceptionNotification * This,
     IXCLRDataTypeInstance *typeInst);


void  IXCLRDataExceptionNotification_OnTypeLoaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification_OnTypeUnloaded_Proxy( 
    IXCLRDataExceptionNotification * This,
     IXCLRDataTypeInstance *typeInst);


void  IXCLRDataExceptionNotification_OnTypeUnloaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 8958 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"






 


 const IID IID_IXCLRDataExceptionNotification2;


















#line 8987 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"

    typedef struct IXCLRDataExceptionNotification2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( __stdcall *QueryInterface )( 
            IXCLRDataExceptionNotification2 * This,
             const IID * riid,
             void **ppvObject);
        
        ULONG ( __stdcall *AddRef )( 
            IXCLRDataExceptionNotification2 * This);
        
        ULONG ( __stdcall *Release )( 
            IXCLRDataExceptionNotification2 * This);
        
        HRESULT ( __stdcall *OnCodeGenerated )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataMethodInstance *method);
        
        HRESULT ( __stdcall *OnCodeDiscarded )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataMethodInstance *method);
        
        HRESULT ( __stdcall *OnProcessExecution )( 
            IXCLRDataExceptionNotification2 * This,
             ULONG32 state);
        
        HRESULT ( __stdcall *OnTaskExecution )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataTask *task,
             ULONG32 state);
        
        HRESULT ( __stdcall *OnModuleLoaded )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataModule *mod);
        
        HRESULT ( __stdcall *OnModuleUnloaded )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataModule *mod);
        
        HRESULT ( __stdcall *OnTypeLoaded )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataTypeInstance *typeInst);
        
        HRESULT ( __stdcall *OnTypeUnloaded )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataTypeInstance *typeInst);
        
        HRESULT ( __stdcall *OnAppDomainLoaded )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataAppDomain *domain);
        
        HRESULT ( __stdcall *OnAppDomainUnloaded )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataAppDomain *domain);
        
        HRESULT ( __stdcall *OnException )( 
            IXCLRDataExceptionNotification2 * This,
             IXCLRDataExceptionState *exception);
        
        END_INTERFACE
    } IXCLRDataExceptionNotification2Vtbl;

    struct IXCLRDataExceptionNotification2
    {
        CONST_VTBL struct IXCLRDataExceptionNotification2Vtbl *lpVtbl;
    };

    



















































#line 9109 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



HRESULT __stdcall IXCLRDataExceptionNotification2_OnAppDomainLoaded_Proxy( 
    IXCLRDataExceptionNotification2 * This,
     IXCLRDataAppDomain *domain);


void  IXCLRDataExceptionNotification2_OnAppDomainLoaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification2_OnAppDomainUnloaded_Proxy( 
    IXCLRDataExceptionNotification2 * This,
     IXCLRDataAppDomain *domain);


void  IXCLRDataExceptionNotification2_OnAppDomainUnloaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IXCLRDataExceptionNotification2_OnException_Proxy( 
    IXCLRDataExceptionNotification2 * This,
     IXCLRDataExceptionState *exception);


void  IXCLRDataExceptionNotification2_OnException_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#line 9150 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"










#line 9161 "c:\\sscli20\\prebuilt\\idl\\xclrdata.h"



#line 1594 "c:\\sscli20\\clr\\src\\inc\\daccess.h"
#line 1595 "c:\\sscli20\\clr\\src\\inc\\daccess.h"















#line 1611 "c:\\sscli20\\clr\\src\\inc\\daccess.h"




#line 1616 "c:\\sscli20\\clr\\src\\inc\\daccess.h"

