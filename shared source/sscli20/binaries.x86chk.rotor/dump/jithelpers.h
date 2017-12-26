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
// Allow multiple inclusion.

#ifndef DYNAMICJITHELPER
#define DYNAMICJITHELPER(code,fn) JITHELPER(code,fn)
#endif

#ifndef SPECIALJITHELPER
#define SPECIALJITHELPER(code,fn) JITHELPER(code,fn)
#endif


// pfnHelper is set to NULL if it is a stubbed helper.
// It will be set in InitJITHelpers2

    JITHELPER(CORINFO_HELP_UNDEF,               NULL                        )

    // Arithmetic
    // CORINFO_HELP_DBL2INT, CORINFO_HELP_DBL2UINT, and CORINFO_HELP_DBL2LONG get
    // patched for CPUs that support SSE2 (P4 and above).
    JITHELPER(CORINFO_HELP_LLSH,                JIT_LLsh                    )
    JITHELPER(CORINFO_HELP_LRSH,                JIT_LRsh                    )
    JITHELPER(CORINFO_HELP_LRSZ,                JIT_LRsz                    )
    JITHELPER(CORINFO_HELP_LMUL,                JIT_LMul                    )
    JITHELPER(CORINFO_HELP_LMUL_OVF,            JIT_LMulOvf                 )
    JITHELPER(CORINFO_HELP_ULMUL_OVF,           JIT_ULMulOvf                )
    JITHELPER(CORINFO_HELP_LDIV,                JIT_LDiv                    )
    JITHELPER(CORINFO_HELP_LMOD,                JIT_LMod                    )
    JITHELPER(CORINFO_HELP_ULDIV,               JIT_ULDiv                   )
    JITHELPER(CORINFO_HELP_ULMOD,               JIT_ULMod                   )
    JITHELPER(CORINFO_HELP_ULNG2DBL,            JIT_ULng2Dbl                )
    DYNAMICJITHELPER(CORINFO_HELP_DBL2INT,      JIT_Dbl2Lng                 ) // use long version
    JITHELPER(CORINFO_HELP_DBL2INT_OVF,         JIT_Dbl2IntOvf              )
    DYNAMICJITHELPER(CORINFO_HELP_DBL2LNG,      JIT_Dbl2Lng                 )
    JITHELPER(CORINFO_HELP_DBL2LNG_OVF,         JIT_Dbl2LngOvf              )
    DYNAMICJITHELPER(CORINFO_HELP_DBL2UINT,     JIT_Dbl2Lng                 ) // use long version
    JITHELPER(CORINFO_HELP_DBL2UINT_OVF,        JIT_Dbl2UIntOvf             )
    JITHELPER(CORINFO_HELP_DBL2ULNG,            JIT_Dbl2ULng                )
    JITHELPER(CORINFO_HELP_DBL2ULNG_OVF,        JIT_Dbl2ULngOvf             )
    JITHELPER(CORINFO_HELP_FLTREM,              JIT_FltRem                  )
    JITHELPER(CORINFO_HELP_DBLREM,              JIT_DblRem                  )

    // Allocating a new object
    JITHELPER(CORINFO_HELP_NEW_DIRECT,                  JIT_New             )
    JITHELPER(CORINFO_HELP_NEW_CROSSCONTEXT,            JIT_NewCrossContext )
    JITHELPER(CORINFO_HELP_NEWFAST,                     JIT_NewFast         )
    DYNAMICJITHELPER(CORINFO_HELP_NEWSFAST,             JIT_NewFast         )
    DYNAMICJITHELPER(CORINFO_HELP_NEWSFAST_ALIGN8,      JIT_NewFast         )
    DYNAMICJITHELPER(CORINFO_HELP_NEWSFAST_CHKRESTORE,  JIT_New             )
    JITHELPER(CORINFO_HELP_NEW_SPECIALDIRECT,           JIT_NewSpecial      )
    JITHELPER(CORINFO_HELP_NEW_MDARR,                   JIT_NewMDArr        )
    JITHELPER(CORINFO_HELP_NEW_MDARR_NO_LBOUNDS,        JIT_NewMDArrNoLBounds)
    JITHELPER(CORINFO_HELP_NEWARR_1_DIRECT,             JIT_NewArr1         )
    DYNAMICJITHELPER(CORINFO_HELP_NEWARR_1_OBJ,         JIT_NewArr1         )
    DYNAMICJITHELPER(CORINFO_HELP_NEWARR_1_VC,          JIT_NewArr1         )
    DYNAMICJITHELPER(CORINFO_HELP_NEWARR_1_ALIGN8,      JIT_NewArr1         )

    JITHELPER(CORINFO_HELP_STRCNS,              JIT_StrCns                  )

    // Object model
    JITHELPER(CORINFO_HELP_INITCLASS,           JIT_InitClass               )
    JITHELPER(CORINFO_HELP_INITINSTCLASS,       JIT_InitInstantiatedClass   )
    JITHELPER(CORINFO_HELP_ISINSTANCEOFINTERFACE,JIT_IsInstanceOfInterface  )
    JITHELPER(CORINFO_HELP_ISINSTANCEOFARRAY,   JIT_IsInstanceOfArray       )
    JITHELPER(CORINFO_HELP_ISINSTANCEOFCLASS,   JIT_IsInstanceOfClass       )
    JITHELPER(CORINFO_HELP_ISINSTANCEOFANY,     JIT_IsInstanceOfAny         )
    JITHELPER(CORINFO_HELP_CHKCASTINTERFACE,    JIT_ChkCastInterface        )
    JITHELPER(CORINFO_HELP_CHKCASTARRAY,        JIT_ChkCastArray            )
    JITHELPER(CORINFO_HELP_CHKCASTCLASS,        JIT_ChkCastClass            )
    JITHELPER(CORINFO_HELP_CHKCASTANY,          JIT_ChkCastAny              )
    JITHELPER(CORINFO_HELP_CHKCASTCLASS_SPECIAL,JIT_ChkCastClassSpecial     )
    DYNAMICJITHELPER(CORINFO_HELP_BOX,          JIT_Box                     )
    JITHELPER(CORINFO_HELP_BOX_NULLABLE,        JIT_Box                     )
    JITHELPER(CORINFO_HELP_UNBOX,               JIT_Unbox                   )
    JITHELPER(CORINFO_HELP_UNBOX_NULLABLE,      JIT_Unbox_Nullable         )
    JITHELPER(CORINFO_HELP_GETREFANY,           JIT_GetRefAny               )
    JITHELPER(CORINFO_HELP_ARRADDR_ST,          JIT_Stelem_Ref              )
    JITHELPER(CORINFO_HELP_LDELEMA_REF,         JIT_Ldelema_Ref             )

    // Exceptions
    JITHELPER(CORINFO_HELP_THROW,               JIT_Throw                   )
    JITHELPER(CORINFO_HELP_RETHROW,             JIT_Rethrow                 )
    JITHELPER(CORINFO_HELP_USER_BREAKPOINT,     JIT_UserBreakpoint          )
    JITHELPER(CORINFO_HELP_RNGCHKFAIL,          JIT_RngChkFail              )
    JITHELPER(CORINFO_HELP_OVERFLOW,            JIT_Overflow                )
    JITHELPER(CORINFO_HELP_INTERNALTHROW,       JIT_InternalThrow           )
    JITHELPER(CORINFO_HELP_INTERNALTHROW_FROM_HELPER, JIT_InternalThrowFromHelper )
    JITHELPER(CORINFO_HELP_VERIFICATION,        JIT_Verification            )
    JITHELPER(CORINFO_HELP_SEC_UNMGDCODE_EXCPT, JIT_SecurityUnmanagedCodeException )
    JITHELPER(CORINFO_HELP_FAIL_FAST,           JIT_FailFast                )

    JITHELPER(CORINFO_HELP_ENDCATCH,            JIT_EndCatch                )

    JITHELPER(CORINFO_HELP_MON_ENTER,               JIT_MonEnterWorker      )
    JITHELPER(CORINFO_HELP_MON_EXIT,                JIT_MonExitWorker       )

    JITHELPER(CORINFO_HELP_MON_ENTER_STATIC,        JIT_MonEnterStatic      )
    JITHELPER(CORINFO_HELP_MON_EXIT_STATIC,         JIT_MonExitStatic       )

    JITHELPER(CORINFO_HELP_GETCLASSFROMMETHODPARAM, JIT_GetClassFromMethodParam)
    JITHELPER(CORINFO_HELP_GETSYNCFROMCLASSHANDLE,  JIT_GetSyncFromClassHandle)

    // Security callout support 
    JITHELPER(CORINFO_HELP_SECURITY_PROLOG,         JIT_Security_Prolog     )    
    JITHELPER(CORINFO_HELP_SECURITY_PROLOG_FRAMED,  JIT_Security_Prolog_Framed)
    JITHELPER(CORINFO_HELP_SECURITY_EPILOG,         JIT_Security_Epilog     )
    JITHELPER(CORINFO_HELP_CALL_ALLOWED_BYSECURITY, JIT_CallAllowedBySecurity)

     // Verification runtime callout support 
    JITHELPER(CORINFO_HELP_VERIFICATION_RUNTIME_CHECK, JIT_VerificationRuntimeCheck )

    // GC support
    JITHELPER(CORINFO_HELP_STOP_FOR_GC,         JIT_RareDisableHelper       )
    JITHELPER(CORINFO_HELP_POLL_GC,             JIT_PollGC                  )
    JITHELPER(CORINFO_HELP_STRESS_GC,           JIT_StressGC                )
    JITHELPER(CORINFO_HELP_CHECK_OBJ,           JIT_CheckObj                )

    // GC Write barrier support
    JITHELPER(CORINFO_HELP_ASSIGN_REF,          JIT_WriteBarrier            )

    JITHELPER(CORINFO_HELP_CHECKED_ASSIGN_REF,  JIT_CheckedWriteBarrier     )

    JITHELPER(CORINFO_HELP_ASSIGN_BYREF,        JIT_ByRefWriteBarrier       )

    JITHELPER(CORINFO_HELP_ASSIGN_STRUCT,       JIT_StructWriteBarrier      )

    // Verifiable byref support
    JITHELPER(CORINFO_HELP_SAFE_RETURNABLE_BYREF, JIT_SafeReturnableByref   )

#ifdef _X86_
    SPECIALJITHELPER(CORINFO_HELP_ASSIGN_REF_EAX,      (LPVOID) &JIT_Writeable_Thunks_Buf.WriteBarrierReg[0] )
    SPECIALJITHELPER(CORINFO_HELP_ASSIGN_REF_EBX,      (LPVOID) &JIT_Writeable_Thunks_Buf.WriteBarrierReg[3] )
    SPECIALJITHELPER(CORINFO_HELP_ASSIGN_REF_ECX,      (LPVOID) &JIT_Writeable_Thunks_Buf.WriteBarrierReg[1] )
    SPECIALJITHELPER(CORINFO_HELP_ASSIGN_REF_ESI,      (LPVOID) &JIT_Writeable_Thunks_Buf.WriteBarrierReg[6] )
    SPECIALJITHELPER(CORINFO_HELP_ASSIGN_REF_EDI,      (LPVOID) &JIT_Writeable_Thunks_Buf.WriteBarrierReg[7] )
    SPECIALJITHELPER(CORINFO_HELP_ASSIGN_REF_EBP,      (LPVOID) &JIT_Writeable_Thunks_Buf.WriteBarrierReg[5] )

    JITHELPER(CORINFO_HELP_CHECKED_ASSIGN_REF_EAX, JIT_CheckedWriteBarrierEAX )
    JITHELPER(CORINFO_HELP_CHECKED_ASSIGN_REF_EBX, JIT_CheckedWriteBarrierEBX )
    JITHELPER(CORINFO_HELP_CHECKED_ASSIGN_REF_ECX, JIT_CheckedWriteBarrierECX )
    JITHELPER(CORINFO_HELP_CHECKED_ASSIGN_REF_ESI, JIT_CheckedWriteBarrierESI )
    JITHELPER(CORINFO_HELP_CHECKED_ASSIGN_REF_EDI, JIT_CheckedWriteBarrierEDI )
    JITHELPER(CORINFO_HELP_CHECKED_ASSIGN_REF_EBP, JIT_CheckedWriteBarrierEBP )
#endif

    // Accessing fields
    JITHELPER(CORINFO_HELP_GETFIELD32,                  JIT_GetField32                  )
    JITHELPER(CORINFO_HELP_SETFIELD32,                  JIT_SetField32                  )
    JITHELPER(CORINFO_HELP_GETFIELD64,                  JIT_GetField64                  )
    JITHELPER(CORINFO_HELP_SETFIELD64,                  JIT_SetField64                  )
    JITHELPER(CORINFO_HELP_GETFIELDOBJ,                 JIT_GetFieldObj                 )
    JITHELPER(CORINFO_HELP_SETFIELDOBJ,                 JIT_SetFieldObj                 )
    JITHELPER(CORINFO_HELP_GETFIELDSTRUCT,              JIT_GetFieldStruct              )
    JITHELPER(CORINFO_HELP_SETFIELDSTRUCT,              JIT_SetFieldStruct              )
    JITHELPER(CORINFO_HELP_GETFIELDFLOAT,               JIT_GetFieldFloat               )
    JITHELPER(CORINFO_HELP_SETFIELDFLOAT,               JIT_SetFieldFloat               )
    JITHELPER(CORINFO_HELP_GETFIELDDOUBLE,              JIT_GetFieldDouble              )   
    JITHELPER(CORINFO_HELP_SETFIELDDOUBLE,              JIT_SetFieldDouble              )

    JITHELPER(CORINFO_HELP_GETFIELDADDR,                JIT_GetFieldAddr                )

    JITHELPER(CORINFO_HELP_GETSTATICFIELDADDR,          JIT_GetStaticFieldAddr          )
    JITHELPER(CORINFO_HELP_GETGENERICS_GCSTATIC_BASE,   JIT_GetGenericsGCStaticBase     )
    JITHELPER(CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE, JIT_GetGenericsNonGCStaticBase )


    // Default Helpers for more efficient static access in Domain Neutral code. Platform specific code should
    // override them
    DYNAMICJITHELPER(CORINFO_HELP_GETSHARED_GCSTATIC_BASE,          JIT_GetSharedGCStaticBase)
    DYNAMICJITHELPER(CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE,       JIT_GetSharedNonGCStaticBase)
    DYNAMICJITHELPER(CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR,   JIT_GetSharedGCStaticBaseNoCtor)
    DYNAMICJITHELPER(CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR,JIT_GetSharedNonGCStaticBaseNoCtor)
    JITHELPER(CORINFO_HELP_GETSHARED_GCSTATIC_BASE_DYNAMICCLASS,    JIT_GetSharedGCStaticBaseDynamicClass)
    JITHELPER(CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_DYNAMICCLASS, JIT_GetSharedNonGCStaticBaseDynamicClass)

    // Debugger

    JITHELPER(CORINFO_HELP_DBG_IS_JUST_MY_CODE, JIT_DbgIsJustMyCode         )

    /* Profiling enter/leave probe addresses */
    DYNAMICJITHELPER(CORINFO_HELP_PROF_FCN_ENTER,      JIT_ProfilerEnterLeaveStub)
    DYNAMICJITHELPER(CORINFO_HELP_PROF_FCN_LEAVE,      JIT_ProfilerEnterLeaveStub)
    DYNAMICJITHELPER(CORINFO_HELP_PROF_FCN_TAILCALL,   JIT_ProfilerTailcallStub  )

    // Miscellaneous
    JITHELPER(CORINFO_HELP_BBT_FCN_ENTER,       JIT_LogMethodEnter          )

    DYNAMICJITHELPER(CORINFO_HELP_PINVOKE_CALLI,    NULL                    )
    JITHELPER(CORINFO_HELP_TAILCALL,            JIT_TailCall                )

    JITHELPER(CORINFO_HELP_GET_THREAD_FIELD_ADDR_PRIMITIVE,     JIT_GetThreadFieldAddr_Primitive )
    JITHELPER(CORINFO_HELP_GET_THREAD_FIELD_ADDR_OBJREF,        JIT_GetThreadFieldAddr_Objref    )

#if USE_INDIRECT_GET_THREAD_APPDOMAIN
    DYNAMICJITHELPER(CORINFO_HELP_GET_THREAD,   NULL                        )
#else
    JITHELPER(CORINFO_HELP_GET_THREAD,          GetThread                   )
#endif

    JITHELPER(CORINFO_HELP_INIT_PINVOKE_FRAME,  NULL                        )
    JITHELPER(CORINFO_HELP_FAST_PINVOKE,        NULL                        )
    JITHELPER(CORINFO_HELP_CHECK_PINVOKE_DOMAIN,NULL                        )

    JITHELPER(CORINFO_HELP_MEMSET,              NULL                        )
    JITHELPER(CORINFO_HELP_MEMCPY,              NULL                        )

    // Generics
    JITHELPER(CORINFO_HELP_RUNTIMEHANDLE,       JIT_GenericHandle           )
    JITHELPER(CORINFO_HELP_VIRTUAL_FUNC_PTR,    JIT_VirtualFunctionPointer  )

#if !CPU_HAS_FP_SUPPORT
    //
    // Some architectures need helpers for FP support
    //
    // Keep these at the end of the enum
    //
    JITHELPER(CORINFO_HELP_R4_NEG,              NULL                        )
    JITHELPER(CORINFO_HELP_R8_NEG,              NULL                        )

    JITHELPER(CORINFO_HELP_R4_ADD,              NULL                        )
    JITHELPER(CORINFO_HELP_R8_ADD,              NULL                        )
    JITHELPER(CORINFO_HELP_R4_SUB,              NULL                        )
    JITHELPER(CORINFO_HELP_R8_SUB,              NULL                        )
    JITHELPER(CORINFO_HELP_R4_MUL,              JIT_FloatMul                )
    JITHELPER(CORINFO_HELP_R8_MUL,              NULL                        )
    JITHELPER(CORINFO_HELP_R4_DIV,              JIT_FloatDiv                )
    JITHELPER(CORINFO_HELP_R8_DIV,              JIT_DoubleDiv               )

    JITHELPER(CORINFO_HELP_R4_EQ,               NULL                        )
    JITHELPER(CORINFO_HELP_R8_EQ,               NULL                        )
    JITHELPER(CORINFO_HELP_R4_NE,               NULL                        )
    JITHELPER(CORINFO_HELP_R8_NE,               NULL                        )
    JITHELPER(CORINFO_HELP_R4_LT,               NULL                        )
    JITHELPER(CORINFO_HELP_R8_LT,               NULL                        )
    JITHELPER(CORINFO_HELP_R4_LE,               NULL                        )
    JITHELPER(CORINFO_HELP_R8_LE,               NULL                        )
    JITHELPER(CORINFO_HELP_R4_GE,               NULL                        )
    JITHELPER(CORINFO_HELP_R8_GE,               NULL                        )
    JITHELPER(CORINFO_HELP_R4_GT,               NULL                        )
    JITHELPER(CORINFO_HELP_R8_GT,               NULL                        )

    JITHELPER(CORINFO_HELP_R8_TO_I4,            NULL                        )
    JITHELPER(CORINFO_HELP_R8_TO_I8,            NULL                        )
    JITHELPER(CORINFO_HELP_R8_TO_R4,            NULL                        )

    JITHELPER(CORINFO_HELP_R4_TO_I4,            NULL                        )
    JITHELPER(CORINFO_HELP_R4_TO_I8,            NULL                        )
    JITHELPER(CORINFO_HELP_R4_TO_R8,            NULL                        )

    JITHELPER(CORINFO_HELP_I4_TO_R4,            NULL                        )
    JITHELPER(CORINFO_HELP_I4_TO_R8,            NULL                        )
    JITHELPER(CORINFO_HELP_I8_TO_R4,            NULL                        )
    JITHELPER(CORINFO_HELP_I8_TO_R8,            NULL                        )
    JITHELPER(CORINFO_HELP_U4_TO_R4,            NULL                        )
    JITHELPER(CORINFO_HELP_U4_TO_R8,            NULL                        )
    JITHELPER(CORINFO_HELP_U8_TO_R4,            NULL                        )
    JITHELPER(CORINFO_HELP_U8_TO_R8,            NULL                        )
#endif // CPU_HAS_FP_SUPPORT

#undef JITHELPER
#undef DYNAMICJITHELPER
#undef SPECIALJITHELPER

