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
// ===========================================================================
// File: JITinterface.H
//
// ===========================================================================

#ifndef JITINTERFACE_H
#define JITINTERFACE_H

#include "corjit.h"
#include "ecall.h"

// A number that represents the size of the largest value class
// that we want a fast array allocation for.   If you make it too big
// we get pesimistic about the number of elements we allow for fast
// allocation.
#define LARGE_ELEMENT_SIZE  16

class Stub;
class MethodDesc;
class FieldDesc;
enum RuntimeExceptionKind;
class AwareLock;
class PtrArray;
class DictionaryEntryLayout;

//@GENERICS: flags returned from IsPolyType indicating the presence or absence of class and
// method type parameters in a type whose instantiation cannot be determined at JIT-compile time
enum VarKind
{
  hasNoVars = 0x0000,
  hasClassVar = 0x0001,
  hasMethodVar = 0x0002,
  hasSharableClassVar = 0x0004,
  hasSharableMethodVar = 0x0008,
  hasAnyVarsMask = 0x0003,
  hasSharableVarsMask = 0x000c
};


inline FieldDesc* GetField(CORINFO_FIELD_HANDLE fieldHandle)
{
    LEAF_CONTRACT;
    return (FieldDesc*) fieldHandle;
}

inline
bool SigInfoFlagsAreValid (CORINFO_SIG_INFO *sig)
{
    LEAF_CONTRACT;
    return !(sig->flags & ~(  CORINFO_SIGFLAG_IS_LOCAL_SIG
                            | CORINFO_SIGFLAG_IL_STUB
                            ));
}


void InitJITHelpers1();
void InitJITHelpers2();

TADDR UnsafeJitFunction(MethodDesc* ftn, COR_ILMETHOD_DECODER* header,
                        DWORD flags);

void* getJitHelper(CorInfoHelpFunc);

void getMethodInfoHelper(MethodDesc * ftn,
                         CORINFO_METHOD_HANDLE ftnHnd,
                         COR_ILMETHOD_DECODER * header,
                         CORINFO_METHOD_INFO *  methInfo);

void getMethodInfoILMethodHeaderHelper(
    COR_ILMETHOD_DECODER* header,
    CORINFO_METHOD_INFO* methInfo
    );



//
// The WIN64 monitor helpers need a state argument
//

// If no state argument is needed, the FCALL helper is just an alias for JIT helper
#define JIT_MonEnter        JIT_MonEnterWorker
#define JIT_MonExit         JIT_MonExitWorker

#define FCDECL_MONHELPER(funcname, arg) FCDECL1(void, funcname, arg)
#define HCIMPL_MONHELPER(funcname, arg) HCIMPL1(void, funcname, arg)
#define MONHELPER_STATE(x)



//
// JIT HELPER ALIASING FOR PORTABILITY.
//
// The portable helper is used if the platform does not provide optimized implementation.
//

#ifndef JIT_MonEnterWorker
#define JIT_MonEnterWorker JIT_MonEnterWorker_Portable
#endif
EXTERN_C FCDECL1(void, JIT_MonEnter, Object *obj);
EXTERN_C FCDECL_MONHELPER(JIT_MonEnterWorker, Object *obj);
EXTERN_C FCDECL_MONHELPER(JIT_MonEnterWorker_Portable, Object *obj);

#ifndef JIT_MonTryEnter
#define JIT_MonTryEnter JIT_MonTryEnter_Portable
#endif
EXTERN_C FCDECL2(FC_BOOL_RET, JIT_MonTryEnter, Object *obj, INT32 timeout);
EXTERN_C FCDECL2(FC_BOOL_RET, JIT_MonTryEnter_Portable, Object *obj, INT32 timeout);

#ifndef JIT_MonExitWorker
#define JIT_MonExitWorker JIT_MonExitWorker_Portable
#endif
EXTERN_C FCDECL1(void, JIT_MonExit, Object *obj);
EXTERN_C FCDECL_MONHELPER(JIT_MonExitWorker, Object *obj);
EXTERN_C FCDECL_MONHELPER(JIT_MonExitWorker_Portable, Object *obj);

#ifndef JIT_MonEnterStatic
#define JIT_MonEnterStatic JIT_MonEnterStatic_Portable  
#endif
EXTERN_C FCDECL1(void, JIT_MonEnterStatic, AwareLock *lock);
EXTERN_C FCDECL1(void, JIT_MonEnterStatic_Portable, AwareLock *lock);

#ifndef JIT_MonExitStatic
#define JIT_MonExitStatic JIT_MonExitStatic_Portable
#endif
EXTERN_C FCDECL1(void, JIT_MonExitStatic, AwareLock *lock);
EXTERN_C FCDECL1(void, JIT_MonExitStatic_Portable, AwareLock *lock);


#ifndef JIT_GetSharedGCStaticBase
#define JIT_GetSharedGCStaticBase JIT_GetSharedGCStaticBase_Portable
#endif
EXTERN_C FCDECL2(void*, JIT_GetSharedGCStaticBase, SIZE_T moduleDomainID, DWORD dwModuleClassID);
EXTERN_C FCDECL2(void*, JIT_GetSharedGCStaticBase_Portable, SIZE_T moduleDomainID, DWORD dwModuleClassID);

#ifndef JIT_GetSharedNonGCStaticBase
#define JIT_GetSharedNonGCStaticBase JIT_GetSharedNonGCStaticBase_Portable
#endif
EXTERN_C FCDECL2(void*, JIT_GetSharedNonGCStaticBase, SIZE_T moduleDomainID, DWORD dwModuleClassID);
EXTERN_C FCDECL2(void*, JIT_GetSharedNonGCStaticBase_Portable, SIZE_T moduleDomainID, DWORD dwModuleClassID);

#ifndef JIT_GetSharedGCStaticBaseNoCtor
#define JIT_GetSharedGCStaticBaseNoCtor JIT_GetSharedGCStaticBaseNoCtor_Portable
#endif
EXTERN_C FCDECL2(void*, JIT_GetSharedGCStaticBaseNoCtor, SIZE_T moduleDomainID, DWORD dwModuleClassID);
EXTERN_C FCDECL2(void*, JIT_GetSharedGCStaticBaseNoCtor_Portable, SIZE_T moduleDomainID, DWORD dwModuleClassID);

#ifndef JIT_GetSharedNonGCStaticBaseNoCtor
#define JIT_GetSharedNonGCStaticBaseNoCtor JIT_GetSharedNonGCStaticBaseNoCtor_Portable
#endif
EXTERN_C FCDECL2(void*, JIT_GetSharedNonGCStaticBaseNoCtor, SIZE_T moduleDomainID, DWORD dwModuleClassID);
EXTERN_C FCDECL2(void*, JIT_GetSharedNonGCStaticBaseNoCtor_Portable, SIZE_T moduleDomainID, DWORD dwModuleClassID);

#ifndef JIT_ChkCastClass
#define JIT_ChkCastClass JIT_ChkCastClass_Portable
#endif
EXTERN_C FCDECL2(Object*, JIT_ChkCastClass, MethodTable* pMT, Object* pObject);
EXTERN_C FCDECL2(Object*, JIT_ChkCastClass_Portable, MethodTable* pMT, Object* pObject);

#ifndef JIT_ChkCastClassSpecial
#define JIT_ChkCastClassSpecial JIT_ChkCastClassSpecial_Portable
#endif
EXTERN_C FCDECL2(Object*, JIT_ChkCastClassSpecial, MethodTable* pMT, Object* pObject);
EXTERN_C FCDECL2(Object*, JIT_ChkCastClassSpecial_Portable, MethodTable* pMT, Object* pObject);

#ifndef JIT_IsInstanceOfClass
#define JIT_IsInstanceOfClass JIT_IsInstanceOfClass_Portable
#endif
EXTERN_C FCDECL2(Object*, JIT_IsInstanceOfClass, MethodTable* pMT, Object* pObject);
EXTERN_C FCDECL2(Object*, JIT_IsInstanceOfClass_Portable, MethodTable* pMT, Object* pObject);

#ifndef JIT_ChkCastInterface
#define JIT_ChkCastInterface JIT_ChkCastInterface_Portable
#endif
EXTERN_C FCDECL2(Object*, JIT_ChkCastInterface, MethodTable* pMT, Object* pObject);
EXTERN_C FCDECL2(Object*, JIT_ChkCastInterface_Portable, MethodTable* pMT, Object* pObject);

#ifndef JIT_IsInstanceOfInterface
#define JIT_IsInstanceOfInterface JIT_IsInstanceOfInterface_Portable
#endif
EXTERN_C FCDECL2(Object*, JIT_IsInstanceOfInterface, MethodTable* pMT, Object* pObject);
EXTERN_C FCDECL2(Object*, JIT_IsInstanceOfInterface_Portable, MethodTable* pMT, Object* pObject);

#ifndef JIT_NewCrossContext
#define JIT_NewCrossContext JIT_NewCrossContext_Portable
#endif
EXTERN_C FCDECL1(Object*, JIT_NewCrossContext, CORINFO_CLASS_HANDLE typeHnd_);
EXTERN_C FCDECL1(Object*, JIT_NewCrossContext_Portable, CORINFO_CLASS_HANDLE typeHnd_);

#ifndef JIT_Stelem_Ref
#define JIT_Stelem_Ref JIT_Stelem_Ref_Portable
#endif
EXTERN_C FCDECL3(void, JIT_Stelem_Ref, PtrArray* array, unsigned idx, Object* val);
EXTERN_C FCDECL3(void, JIT_Stelem_Ref_Portable, PtrArray* array, unsigned idx, Object* val);


EXTERN_C FCDECL_MONHELPER(JITutil_MonSignal, AwareLock* lock);
EXTERN_C FCDECL_MONHELPER(JITutil_MonContention, AwareLock* awarelock);

// Slow versions to tail call if the fast version fails
EXTERN_C FCDECL2(void*, JIT_GetSharedNonGCStaticBase_Helper, DomainLocalModule *pLocalModule, DWORD dwClassDomainID);
EXTERN_C FCDECL2(void*, JIT_GetSharedGCStaticBase_Helper, DomainLocalModule *pLocalModule, DWORD dwClassDomainID);

EXTERN_C void DoJITFailFast ();
EXTERN_C FCDECL0(void, JIT_FailFast);

extern FCDECL2(__int32, JIT_GetField32, Object *obj, FieldDesc *pFD);
extern FCDECL2(__int64, JIT_GetField64, Object *obj, FieldDesc *pFD);
extern FCDECL2(Object* , JIT_GetFieldObj, Object *obj, FieldDesc *pFD);
extern FCDECL3(VOID, JIT_GetFieldStruct, LPVOID retBuff, Object *obj, FieldDesc *pFD);
extern FCDECL3(VOID, JIT_SetField32, Object *obj, FieldDesc *pFD, __int32 value);
extern FCDECL3(VOID, JIT_SetField64, Object *obj, FieldDesc *pFD, __int64 value);
extern FCDECL3(VOID, JIT_SetFieldObj, Object *obj, FieldDesc *pFD, Object *value);
extern FCDECL3(VOID, JIT_StructWriteBarrier, void *dst, void *src, CORINFO_CLASS_HANDLE typeHnd_);

FCDECL1(void*, JIT_SafeReturnableByref, void* byref);

extern "C" FCDECL2(VOID, JIT_CheckedWriteBarrier, Object **dst, Object *ref);
extern "C" FCDECL2(VOID, JIT_WriteBarrier, Object **dst, Object *ref);

extern "C" FCDECL2(Object*, JIT_ChkCastAny, CORINFO_CLASS_HANDLE type, Object *pObject);   // JITInterfaceX86.cpp, etc.
extern "C" FCDECL2(Object*, JIT_IsInstanceOfAny, CORINFO_CLASS_HANDLE type, Object *pObject);

extern "C" FCDECL2(Object*, JITutil_ChkCastAny, CORINFO_CLASS_HANDLE type, Object *obj);
extern "C" FCDECL2(Object*, JITutil_IsInstanceOfAny, CORINFO_CLASS_HANDLE type, Object *obj);

extern "C" FCDECL1(void, JIT_InternalThrow, unsigned exceptNum);
extern "C" FCDECL1(void*, JIT_InternalThrowFromHelper, unsigned exceptNum);

extern FCDECL3(VOID, JIT_SetFieldStruct, Object *obj, FieldDesc *pFD, LPVOID valuePtr);



EXTERN_C FCDECL2_VV(INT64, JIT_LMul, INT64 val1, INT64 val2);

EXTERN_C FCDECL1_V(INT64, JIT_Dbl2Lng, double val);
EXTERN_C FCDECL1_V(INT64, JIT_Dbl2IntSSE2, double val);
EXTERN_C FCDECL1_V(INT64, JIT_Dbl2LngP4x87, double val);
EXTERN_C FCDECL1_V(INT64, JIT_Dbl2LngOvf, double val);

//
// Making JIT_Dbl2IntOvf to be EXTERN_C is causing problems with the incremental BBT.
// It can be cleaned up once we switch over to BBT 3.41.
//
WIN64_ONLY(EXTERN_C) FCDECL1_V(INT32, JIT_Dbl2IntOvf, double val);

extern "C" float __stdcall JIT_FltRem(float divisor, float dividend);
extern "C" double __stdcall JIT_DblRem(double divisor, double dividend);

#ifdef _X86_

extern "C"
{
    void __stdcall JIT_CheckedWriteBarrierEAX(); // JIThelp.asm/JIThelp.s
    void __stdcall JIT_CheckedWriteBarrierEBX(); // JIThelp.asm/JIThelp.s
    void __stdcall JIT_CheckedWriteBarrierECX(); // JIThelp.asm/JIThelp.s
    void __stdcall JIT_CheckedWriteBarrierESI(); // JIThelp.asm/JIThelp.s
    void __stdcall JIT_CheckedWriteBarrierEDI(); // JIThelp.asm/JIThelp.s
    void __stdcall JIT_CheckedWriteBarrierEBP(); // JIThelp.asm/JIThelp.s

    void __stdcall JIT_WriteBarrierEAX();        // JIThelp.asm/JIThelp.s
    void __stdcall JIT_WriteBarrierEBX();        // JIThelp.asm/JIThelp.s
    void __stdcall JIT_WriteBarrierECX();        // JIThelp.asm/JIThelp.s
    void __stdcall JIT_WriteBarrierESI();        // JIThelp.asm/JIThelp.s
    void __stdcall JIT_WriteBarrierEDI();        // JIThelp.asm/JIThelp.s
    void __stdcall JIT_WriteBarrierEBP();        // JIThelp.asm/JIThelp.s

    void __stdcall JIT_WriteBarrierStart();
    void __stdcall JIT_WriteBarrierLast();
}

#endif //_X86_

extern "C"
{
    void __stdcall JIT_EndCatch();               // JIThelp.asm/JIThelp.s

    void __stdcall JIT_ByRefWriteBarrier();      // JIThelp.asm/JIThelp.s

    void __stdcall JIT_LLsh();                      // JIThelp.asm
    void __stdcall JIT_LRsh();                      // JIThelp.asm
    void __stdcall JIT_LRsz();                      // JIThelp.asm

    void __stdcall JIT_TailCall();                    // JIThelp.asm

    void __stdcall JIT_MemSet();
    void __stdcall JIT_MemCpy();

    void __stdcall DoNDirectCallWorker(void);

    void __stdcall JIT_ProfilerEnterLeaveStub(UINT_PTR EEHandle, UINT_PTR ProfilerHandle, UINT_PTR FrameInfo, UINT_PTR ArgInfo);
    void __stdcall JIT_ProfilerTailcallStub(UINT_PTR EEHandle, UINT_PTR ProfilingHandle, UINT_PTR FrameInfo);
};

/*********************************************************************/
/*********************************************************************/
/* This is a little helper class that CEEInfo uses */


#ifdef VALUE_ARRAYS
struct ValueArrayDescr
{
    ValueArrayDescr(SigPointer aSig, Module* aModule, ValueArrayDescr* aNext=0) :
        sig(aSig), module(aModule), next(aNext) {}
    ~ValueArrayDescr() { LEAF_CONTRACT; if (next != 0) delete next; }

    SigPointer          sig;    // points to the signature describing the value array
    Module             *module; // module the signature comes from
    ValueArrayDescr    *next;
};
#endif

class CEEInfo : public virtual ICorDynamicInfo 
{
    friend class CEEDynamicCodeInfo;
    
public:
    // ICorClassInfo stuff
    CorInfoType __stdcall asCorInfoType (CORINFO_CLASS_HANDLE cls);
    CORINFO_MODULE_HANDLE __stdcall getClassModule(CORINFO_CLASS_HANDLE clsHnd);
    CORINFO_MODULE_HANDLE __stdcall getClassModuleForStatics(CORINFO_CLASS_HANDLE clsHnd);
    const char* __stdcall getClassName (CORINFO_CLASS_HANDLE cls);
    const char* __stdcall getHelperName(CorInfoHelpFunc ftnNum);
    int __stdcall appendClassName(__deref_inout_ecount(*pnBufLen) WCHAR** ppBuf,
                                  int* pnBufLen,
                                  CORINFO_CLASS_HANDLE    cls,
                                  BOOL fNamespace,
                                  BOOL fFullInst,
                                  BOOL fAssembly);
    BOOL  __stdcall canInlineTypeCheckWithObjectVTable (CORINFO_CLASS_HANDLE cls);
    DWORD __stdcall getClassAttribs (CORINFO_CLASS_HANDLE cls, CORINFO_METHOD_HANDLE context);

    CORINFO_CLASS_HANDLE __stdcall getSDArrayForClass(CORINFO_CLASS_HANDLE clsHnd);
    unsigned __stdcall getClassSize (CORINFO_CLASS_HANDLE cls);
    unsigned __stdcall getClassAlignmentRequirement(CORINFO_CLASS_HANDLE cls);

    // Used for HFA's on IA64...and later for type based disambiguation
    CORINFO_FIELD_HANDLE __stdcall getFieldInClass(CORINFO_CLASS_HANDLE clsHnd, INT num);
    INT __stdcall getClassCustomAttribute(CORINFO_CLASS_HANDLE clsHnd, LPCSTR attrib, const BYTE** ppVal);

    mdMethodDef __stdcall getMethodDefFromMethod(CORINFO_METHOD_HANDLE hMethod);
    BOOL __stdcall checkMethodModifier(CORINFO_METHOD_HANDLE hMethod, LPCSTR modifier, BOOL fOptional);

    unsigned __stdcall getClassGClayout (CORINFO_CLASS_HANDLE cls, BYTE* gcPtrs); /* really GCType* gcPtrs */
    unsigned __stdcall getClassNumInstanceFields(CORINFO_CLASS_HANDLE cls);

    // Check Visibility rules.
    // For Protected (family access) members, type of the instance is also
    // considered when checking visibility rules.
    BOOL __stdcall canAccessField(
                        CORINFO_METHOD_HANDLE   context,
                        CORINFO_CLASS_HANDLE    parent,
                        CORINFO_FIELD_HANDLE    target,
                        CORINFO_CLASS_HANDLE    instance);

    CorInfoHelpFunc __stdcall getNewHelper(CORINFO_CLASS_HANDLE newCls, CORINFO_METHOD_HANDLE scopeHnd,
                    unsigned classToken, CORINFO_MODULE_HANDLE tokenContext);

    CorInfoHelpFunc __stdcall getNewArrHelper(CORINFO_CLASS_HANDLE arrayCls,
                                              CORINFO_METHOD_HANDLE context);
    CorInfoHelpFunc __stdcall getNewMDArrHelper(CORINFO_CLASS_HANDLE arrayCls,
                                                CORINFO_METHOD_HANDLE arrayCtorMethod,
                                                CORINFO_METHOD_HANDLE context);

    CorInfoHelpFunc __stdcall getCastingHelper(CORINFO_MODULE_HANDLE scopeHnd,
                                                    unsigned metaTOK,
                                                    CORINFO_CONTEXT_HANDLE context,
                                                    bool fThrowing);
    CorInfoHelpFunc __stdcall getIsInstanceOfHelper(CORINFO_MODULE_HANDLE scopeHnd,
                                                    unsigned metaTOK,
                                                    CORINFO_CONTEXT_HANDLE context);
    CorInfoHelpFunc __stdcall getChkCastHelper(CORINFO_MODULE_HANDLE scopeHnd,
                                               unsigned metaTOK,
                                               CORINFO_CONTEXT_HANDLE context);

    CorInfoHelpFunc __stdcall getStaticsCCtorHelper(CORINFO_FIELD_HANDLE fldHnd, CORINFO_CLASS_HANDLE clsHnd, BOOL fRuntimeLookup);
    CorInfoHelpFunc __stdcall getSharedStaticBaseHelper(CORINFO_FIELD_HANDLE fldHnd, BOOL runtimeLookup);
    CorInfoHelpFunc __stdcall getSharedCCtorHelper(CORINFO_CLASS_HANDLE clsHnd);
    CorInfoHelpFunc __stdcall getSecurityHelper(CORINFO_METHOD_HANDLE ftn, BOOL fEnter);
    CORINFO_CLASS_HANDLE  getTypeForBox(CORINFO_CLASS_HANDLE  cls); 
    CorInfoHelpFunc __stdcall getBoxHelper(CORINFO_CLASS_HANDLE cls);
    CorInfoHelpFunc __stdcall getUnBoxHelper(CORINFO_CLASS_HANDLE cls, BOOL* helperCopies);

    BOOL __stdcall initClass (CORINFO_CLASS_HANDLE cls, CORINFO_METHOD_HANDLE context, BOOL speculative, BOOL *pfNeedsInitFixup);
    void __stdcall classMustBeLoadedBeforeCodeIsRun (CORINFO_CLASS_HANDLE cls);
    void __stdcall methodMustBeLoadedBeforeCodeIsRun (CORINFO_METHOD_HANDLE meth);
    CORINFO_METHOD_HANDLE __stdcall mapMethodDeclToMethodImpl(CORINFO_METHOD_HANDLE methHnd);
    CORINFO_CLASS_HANDLE __stdcall getBuiltinClass(CorInfoClassId classId);
    void __stdcall getGSCookie(GSCookie * pCookieVal, GSCookie ** ppCookieVal);

    // "System.Int32" ==> CORINFO_TYPE_INT..
    CorInfoType __stdcall getTypeForPrimitiveValueClass(
            CORINFO_CLASS_HANDLE        cls
            );

    // TRUE if child is a subtype of parent
    // if parent is an interface, then does child implement / extend parent
    BOOL __stdcall canCast(
            CORINFO_CLASS_HANDLE        child,
            CORINFO_CLASS_HANDLE        parent
            );

    // returns is the intersection of cls1 and cls2.
    CORINFO_CLASS_HANDLE __stdcall mergeClasses(
            CORINFO_CLASS_HANDLE        cls1,
            CORINFO_CLASS_HANDLE        cls2
            );

    // Given a class handle, returns the Parent type.
    // For COMObjectType, it returns Class Handle of System.Object.
    // Returns 0 if System.Object is passed in.
    CORINFO_CLASS_HANDLE __stdcall getParentType (
            CORINFO_CLASS_HANDLE        cls
            );

    // Returns the CorInfoType of the "child type". If the child type is
    // not a primitive type, *clsRet will be set.
    // Given an Array of Type Foo, returns Foo.
    // Given BYREF Foo, returns Foo
    CorInfoType __stdcall getChildType (
            CORINFO_CLASS_HANDLE       clsHnd,
            CORINFO_CLASS_HANDLE       *clsRet
            );

    // Check Visibility rules.
    BOOL __stdcall canAccessType(
            CORINFO_METHOD_HANDLE       context,
            CORINFO_CLASS_HANDLE        target
            );

    // Check constraints on type arguments of this class and parent classes
    BOOL __stdcall satisfiesClassConstraints(
            CORINFO_CLASS_HANDLE cls
            );

    // Check if this is a single dimensional array type
    BOOL __stdcall isSDArray(
            CORINFO_CLASS_HANDLE        cls
            );

    // Get the number of dimensions in an array 
    unsigned __stdcall getArrayRank(
            CORINFO_CLASS_HANDLE        cls
            );

    // Get static field data for an array
    void * __stdcall getArrayInitializationData(
            CORINFO_FIELD_HANDLE        field,
            DWORD                       size
            );

    // Static helpers
    static BOOL CanCast(
            CorElementType el1,
            CorElementType el2);

    // Returns that compilation flags that are shared between JIT and NGen
    static DWORD GetBaseCompileFlags();

    // Given a type token metaTOK, use class instantiation in context to instantiate any type variables and return a type handle
    CORINFO_CLASS_HANDLE __stdcall findClass(CORINFO_MODULE_HANDLE  scopeHnd,unsigned metaTOK, CORINFO_CONTEXT_HANDLE context, 
                                             CorInfoTokenKind tokenKind = CORINFO_TOKENKIND_Default);

    // Given a field token metaTOK, use class instantiation in context to instantiate any type variables in its *parent* type and return a field handle
    CORINFO_FIELD_HANDLE __stdcall findField(CORINFO_MODULE_HANDLE  scopeHnd, unsigned metaTOK, CORINFO_CONTEXT_HANDLE context);

    // Given a method token metaTOK, use class instantiation in context to instantiate any type variables in its *parent* type and return a method handle
    CORINFO_METHOD_HANDLE __stdcall findMethod(CORINFO_MODULE_HANDLE  scopeHnd, unsigned metaTOK, CORINFO_CONTEXT_HANDLE context);

    // Given a field or method token metaTOK return its parent token
    unsigned __stdcall getMemberParent(CORINFO_MODULE_HANDLE  scopeHnd, unsigned metaTOK);

    // Given a signature token sigTOK, use class/method instantiation in context to instantiate any type variables in the signature and return a new signature
    void __stdcall findSig(CORINFO_MODULE_HANDLE scopeHnd, unsigned sigTOK, CORINFO_METHOD_HANDLE context, CORINFO_SIG_INFO* sig);
    void __stdcall findCallSiteSig(CORINFO_MODULE_HANDLE scopeHnd, unsigned methTOK, CORINFO_METHOD_HANDLE context, CORINFO_SIG_INFO* sig);
    CORINFO_CLASS_HANDLE __stdcall getTokenTypeAsHandle (CORINFO_MODULE_HANDLE  scopeHnd, unsigned metaTOK);

    size_t __stdcall findNameOfToken (CORINFO_MODULE_HANDLE module, mdToken metaTOK,
                                                    __out_ecount (FQNameCapacity) char * szFQName, size_t FQNameCapacity);

    CorInfoCanSkipVerificationResult __stdcall canSkipVerification(CORINFO_MODULE_HANDLE moduleHnd, 
                                                                   BOOL fQuickCheckOnlyWithoutCommit);

    // Checks if the given metadata token is valid
    BOOL __stdcall isValidToken (
            CORINFO_MODULE_HANDLE       module,
            mdToken                    metaTOK);

    // Checks if the given metadata token is valid StringRef
    BOOL __stdcall isValidStringRef (
            CORINFO_MODULE_HANDLE       module,
            mdToken                    metaTOK);

    static size_t __stdcall findNameOfToken (Module* module, mdToken metaTOK, 
                            __out_ecount (FQNameCapacity) char * szFQName, size_t FQNameCapacity);

        // ICorMethodInfo stuff
    const char* __stdcall getMethodName (CORINFO_METHOD_HANDLE ftnHnd, const char** scopeName);
    unsigned __stdcall getMethodHash (CORINFO_METHOD_HANDLE ftnHnd);
    DWORD __stdcall getMethodAttribs (CORINFO_METHOD_HANDLE ftnHnd, CORINFO_METHOD_HANDLE callerHnd);
    void __stdcall setMethodAttribs (CORINFO_METHOD_HANDLE ftnHnd, CorInfoMethodRuntimeFlags attribs);

    bool __stdcall getMethodInfo (
            CORINFO_METHOD_HANDLE ftnHnd,
            CORINFO_METHOD_INFO*  methInfo);

    CorInfoInline __stdcall canInline (
            CORINFO_METHOD_HANDLE  callerHnd,
            CORINFO_METHOD_HANDLE  calleeHnd,
            DWORD*                 pRestrictions);

    // Used by ngen
    CORINFO_METHOD_HANDLE __stdcall instantiateMethodAtObject(CORINFO_METHOD_HANDLE method);

    // Loads the constraints on a typical method definition, detecting cycles;
    // used by verifiers.
    void __stdcall initConstraintsForVerification(
            CORINFO_METHOD_HANDLE   method,
            BOOL *pfHasCircularClassConstraints,
            BOOL *pfHasCircularMethodConstraints
            );

    CorInfoInstantiationVerification __stdcall isInstantiationOfVerifiedGeneric (
            CORINFO_METHOD_HANDLE  methodHnd);


    bool __stdcall canTailCall (
            CORINFO_METHOD_HANDLE  callerHnd,
            CORINFO_METHOD_HANDLE  calleeHnd,
            bool fIsTailPrefix);

    CorInfoCanSkipVerificationResult __stdcall canSkipMethodVerification(
        CORINFO_METHOD_HANDLE ftnHnd, 
        BOOL fQuickCheckOnly);
    
    //  Determines whether a callout is allowd.
    CorInfoIsCallAllowedResult __stdcall isCallAllowed (
            CORINFO_METHOD_HANDLE       callerHnd,
            CORINFO_METHOD_HANDLE       calleeHnd,
            CORINFO_CALL_ALLOWED_INFO * CallAllowedInfo);

    bool __stdcall canSkipMethodPreparation (
            CORINFO_METHOD_HANDLE   callerHnd,
            CORINFO_METHOD_HANDLE   calleeHnd,
            bool                    fCheckCode,
            CorInfoIndirectCallReason *pReason,
            CORINFO_ACCESS_FLAGS    accessFlags = CORINFO_ACCESS_ANY);

    bool __stdcall canCallDirectViaEntryPointThunk (
            CORINFO_METHOD_HANDLE   calleeHnd,
            void **                 pEntryPoint);

    // Given a method descriptor ftnHnd, extract signature information into sigInfo
    // Obtain (representative) instantiation information from ftnHnd's owner class
    //@GENERICSVER: added explicit owner parameter
    void __stdcall getMethodSig (
            CORINFO_METHOD_HANDLE ftnHnd,
            CORINFO_SIG_INFO* sigInfo,
            CORINFO_CLASS_HANDLE owner = NULL
            );

    void __stdcall getEHinfo(
            CORINFO_METHOD_HANDLE ftn,
            unsigned      EHnumber,
            CORINFO_EH_CLAUSE* clause);

    CORINFO_CLASS_HANDLE __stdcall getMethodClass (CORINFO_METHOD_HANDLE methodHnd);
    CORINFO_MODULE_HANDLE __stdcall getMethodModule (CORINFO_METHOD_HANDLE methodHnd);
    unsigned __stdcall getMethodVTableOffset (CORINFO_METHOD_HANDLE methodHnd);
    CorInfoIntrinsics __stdcall getIntrinsicID(CORINFO_METHOD_HANDLE method);

    CorInfoUnmanagedCallConv __stdcall getUnmanagedCallConv(CORINFO_METHOD_HANDLE method);
    BOOL __stdcall pInvokeMarshalingRequired(CORINFO_METHOD_HANDLE method, CORINFO_SIG_INFO* callSiteSig);

    // Generate a cookie based on the signature that would needs to be passed
    //  to the above generic stub
    LPVOID GetCookieForPInvokeCalliSig(CORINFO_SIG_INFO* szMetaSig, void ** ppIndirection);

    // Check Visibility rules.
    // For Protected (family access) members, type of the instance is also
    // considered when checking visibility rules.
    BOOL __stdcall canAccessMethod(
            CORINFO_METHOD_HANDLE       context,
            CORINFO_CLASS_HANDLE        parent,
            CORINFO_METHOD_HANDLE       target,
            CORINFO_CLASS_HANDLE        instance);

    // Check constraints on method type arguments (only).
    // The parent class should be checked separately using satisfiesClassConstraints(parent).
    BOOL __stdcall satisfiesMethodConstraints(
            CORINFO_CLASS_HANDLE        parent, // the exact parent of the method
            CORINFO_METHOD_HANDLE       method
            );

    // Given a Delegate type and a method, check if the method signature
    // is Compatible with the Invoke method of the delegate.
    //@GENERICSVER: new (suitable for generics)
    BOOL __stdcall isCompatibleDelegate(
            CORINFO_CLASS_HANDLE        objCls,
            CORINFO_CLASS_HANDLE        methodParentCls,
            CORINFO_METHOD_HANDLE       method,
            CORINFO_CLASS_HANDLE        delegateCls,
            CORINFO_MODULE_HANDLE       moduleHnd,
            unsigned        methodMemberRef,
            unsigned        delegateConstructorMemberRef);

        // ICorFieldInfo stuff
    const char* __stdcall getFieldName (CORINFO_FIELD_HANDLE field,
                                        const char** scopeName);

    DWORD __stdcall getFieldAttribs (CORINFO_FIELD_HANDLE  field,
                                     CORINFO_METHOD_HANDLE context,
                                     CORINFO_ACCESS_FLAGS  flags);

    CORINFO_CLASS_HANDLE __stdcall getFieldClass (CORINFO_FIELD_HANDLE field);

    //@GENERICSVER: added owner parameter
    CorInfoType __stdcall getFieldType (CORINFO_FIELD_HANDLE field, CORINFO_CLASS_HANDLE* structType,CORINFO_CLASS_HANDLE owner = NULL);

    unsigned __stdcall getFieldOffset (CORINFO_FIELD_HANDLE field);
    void* __stdcall getFieldAddress(CORINFO_FIELD_HANDLE field, void **ppIndirection);
    CorInfoHelpFunc __stdcall getFieldHelper(CORINFO_FIELD_HANDLE field, enum CorInfoFieldAccess kind);


    virtual CorInfoFieldCategory __stdcall getFieldCategory (CORINFO_FIELD_HANDLE field);

    // ICorDebugInfo stuff
    void * __stdcall allocateArray(ULONG cBytes);
    void __stdcall freeArray(void *array);
    void __stdcall getBoundaries(CORINFO_METHOD_HANDLE ftn,
                                 unsigned int *cILOffsets, DWORD **pILOffsets,
                                 ICorDebugInfo::BoundaryTypes *implictBoundaries);
    void __stdcall setBoundaries(CORINFO_METHOD_HANDLE ftn,
                                 ULONG32 cMap, OffsetMapping *pMap);
    void __stdcall getVars(CORINFO_METHOD_HANDLE ftn, ULONG32 *cVars,
                           ILVarInfo **vars, bool *extendOthers);
    void __stdcall setVars(CORINFO_METHOD_HANDLE ftn, ULONG32 cVars,
                           NativeVarInfo *vars);

        // ICorArgInfo stuff

    virtual CorInfoTypeWithMod __stdcall getArgType (
            CORINFO_SIG_INFO*       sig,
            CORINFO_ARG_LIST_HANDLE    args,
            CORINFO_CLASS_HANDLE       *vcTypeRet
            );

    CORINFO_CLASS_HANDLE __stdcall getArgClass (
            CORINFO_SIG_INFO*       sig,
            CORINFO_ARG_LIST_HANDLE    args
            );

    CORINFO_ARG_LIST_HANDLE __stdcall getArgNext (
            CORINFO_ARG_LIST_HANDLE args
            );

    // ICorLinkInfo stuff

    void __stdcall recordRelocation(
            void                    *location,
            WORD                     fRelocType)
            {LEAF_CONTRACT;    /* Regular JIT doesn't record relocations */ }

    // ICorErrorInfo stuff

    HRESULT __stdcall GetErrorHRESULT(struct _EXCEPTION_POINTERS *pExceptionPointers);
    CORINFO_CLASS_HANDLE __stdcall GetErrorClass();
    ULONG __stdcall GetErrorMessage(__out_ecount(bufferLength) LPWSTR buffer,
                                    ULONG bufferLength);
    int __stdcall FilterException(struct _EXCEPTION_POINTERS *pExceptionPointers);

        // ICorStaticInfo stuff
    void __stdcall getEEInfo(CORINFO_EE_INFO *pEEInfoOut);

        //ICorDynamicInfo stuff
    virtual DWORD __stdcall getFieldThreadLocalStoreID (CORINFO_FIELD_HANDLE field, void **ppIndirection);
    unsigned __stdcall getInterfaceTableOffset (CORINFO_CLASS_HANDLE    cls, void **ppIndirection);

    // Stub dispatch stuff
    void __stdcall getCallInfo(
                        CORINFO_METHOD_HANDLE   methodBeingCompiledHnd,
                        CORINFO_MODULE_HANDLE   tokenScope,
                        unsigned                methodToken,
                        unsigned                constraintToken,
                        CORINFO_CONTEXT_HANDLE  tokenContext,
                        CORINFO_CALLINFO_FLAGS  flags,
                        CORINFO_CALL_INFO *pResult);
protected:

    // This is a helper for CEEDynamicCodeInfo
    static void GetCallInfoHelper(
                           CORINFO_METHOD_HANDLE  methodBeingCompiledHnd,
                           TypeHandle             ownerTH,
                           CORINFO_METHOD_HANDLE  methHnd,
                           CORINFO_CALLINFO_FLAGS flags,
                           TypeHandle             constraintType,
                           VarKind                varKind,
                           Module                *pModule,        // only needed if varKind != hasNoVars, i.e. calling from shared generic code
                           mdToken                methodToken,    // only needed if varKind != hasNoVars, i.e. calling from shared generic code
                           mdToken                constraintToken,// only needed if varKind != hasNoVars, i.e. calling from shared generic code
                           CORINFO_CONTEXT_HANDLE tokenContext,   // only needed if varKind != hasNoVars, i.e. calling from shared generic code
                           CORINFO_CALL_INFO     *pResult);

    static void getEHinfoHelper(
        CORINFO_METHOD_HANDLE   ftnHnd,
        unsigned                EHnumber,
        CORINFO_EH_CLAUSE*      clause,
        COR_ILMETHOD_DECODER*   pILHeader);

    virtual bool isVerifyOnly()
    {
        return false;
    }

public:

    size_t __stdcall getModuleDomainID (CORINFO_MODULE_HANDLE   module, void **ppIndirection);
    BOOL __stdcall     isRIDClassDomainID(CORINFO_CLASS_HANDLE cls);
    unsigned __stdcall getClassDomainID (CORINFO_CLASS_HANDLE   cls, void **ppIndirection);
    CORINFO_VARARGS_HANDLE __stdcall getVarArgsHandle(CORINFO_SIG_INFO *sig, void **ppIndirection);
    virtual void* __stdcall getMethodSync(CORINFO_METHOD_HANDLE ftnHnd, void **ppIndirection);
    void* __stdcall getPInvokeUnmanagedTarget(CORINFO_METHOD_HANDLE method, void **ppIndirection);
    void* __stdcall getAddressOfPInvokeFixup(CORINFO_METHOD_HANDLE method, void **ppIndirection);
    CORINFO_JUST_MY_CODE_HANDLE __stdcall getJustMyCodeHandle(CORINFO_METHOD_HANDLE method, CORINFO_JUST_MY_CODE_HANDLE **ppIndirection);

    void __stdcall GetProfilingHandle(
                    CORINFO_METHOD_HANDLE      method,
                    BOOL                      *pbHookFunction,
                    void                     **pEEHandle,
                    void                     **pProfilerHandle,
                    BOOL                      *pbIndirectedHandles
                    );

    InfoAccessType __stdcall constructStringLiteral(CORINFO_MODULE_HANDLE scopeHnd, mdToken metaTok, void **ppInfo);

    CORINFO_CLASS_HANDLE __stdcall findMethodClass(CORINFO_MODULE_HANDLE module, mdToken methodTok,
                                                   CORINFO_METHOD_HANDLE context);

    DWORD __stdcall getThreadTLSIndex(void **ppIndirection);
    const void * __stdcall getInlinedCallFrameVptr(void **ppIndirection);

    // Returns the address of the domain neutral module id. This only makes sense for domain neutral (shared)
    // modules
    SIZE_T* __stdcall getAddrModuleDomainID(CORINFO_MODULE_HANDLE   module);

    LONG * __stdcall getAddrOfCaptureThreadGlobal(void **ppIndirection);
    void* __stdcall getHelperFtn(CorInfoHelpFunc    ftnNum,                 /* IN  */
                                 void **            ppIndirection,          /* OUT */
                                 InfoAccessModule * pAccessModule = NULL);  /* OUT */

    void __stdcall getFunctionEntryPoint(CORINFO_METHOD_HANDLE   ftn,                 /* IN  */
                                         InfoAccessType          requestedAccessType, /* IN  */
                                         CORINFO_CONST_LOOKUP *  pResult,             /* OUT */
                                         CORINFO_ACCESS_FLAGS    accessFlags = CORINFO_ACCESS_ANY);

    void __stdcall getFunctionFixedEntryPointInfo(CORINFO_MODULE_HANDLE  scopeHnd,
                                                  unsigned               metaTOK,
                                                  CORINFO_CONTEXT_HANDLE context,
                                                  CORINFO_LOOKUP *       pResult);

    // a module handle may not always be available. A call to embedModuleHandle should always
    // be preceeded by a call to canEmbedModuleHandleForHelper. A dynamicMethod does not have a module
    bool __stdcall canEmbedModuleHandleForHelper(CORINFO_MODULE_HANDLE handle);

    CORINFO_MODULE_HANDLE __stdcall embedModuleHandle(CORINFO_MODULE_HANDLE handle,
                                                      void **ppIndirection);
    CORINFO_CLASS_HANDLE __stdcall embedClassHandle(CORINFO_CLASS_HANDLE handle,
                                                    void **ppIndirection);
    CORINFO_FIELD_HANDLE __stdcall embedFieldHandle(CORINFO_FIELD_HANDLE handle,
                                                    void **ppIndirection);
    CORINFO_METHOD_HANDLE __stdcall embedMethodHandle(CORINFO_METHOD_HANDLE handle,
                                                      void **ppIndirection);
    void __stdcall embedGenericHandle(
                        CORINFO_MODULE_HANDLE   module,
                        unsigned                metaTOK,
                        CORINFO_CONTEXT_HANDLE  context,
                        CorInfoTokenKind        tokenKind,
                        CORINFO_GENERICHANDLE_RESULT *pResult);

    CORINFO_LOOKUP_KIND __stdcall getLocationOfThisType(CORINFO_METHOD_HANDLE context);


    void __stdcall setOverride(ICorDynamicInfo *pOverride) {LEAF_CONTRACT;  m_pOverride = pOverride; }

    void __stdcall addActiveDependency(CORINFO_MODULE_HANDLE moduleFrom, CORINFO_MODULE_HANDLE moduleTo);

    CORINFO_METHOD_HANDLE __stdcall GetDelegateCtor(
                        CORINFO_METHOD_HANDLE       methHnd,
                        CORINFO_CLASS_HANDLE        clsHnd,
                        CORINFO_METHOD_HANDLE       targetMethodHnd,
                        DelegateCtorArgs *          pCtorData);

    void __stdcall MethodCompileComplete(
                CORINFO_METHOD_HANDLE methHnd);

    CEEInfo() :
#ifdef VALUE_ARRAYS
        m_ValueArrays(NULL),
#endif
        m_pOverride(NULL)
      {
          LEAF_CONTRACT;
          m_pOverride = this;
      }

    ~CEEInfo()
    {
        LEAF_CONTRACT;
#ifdef VALUE_ARRAYS
        delete m_ValueArrays;
#endif
    }

private:
#ifdef VALUE_ARRAYS
        // allocate a descriptor that will be removed when this ICorJitInfo goes away
    ValueArrayDescr* allocDescr(SigPointer sig, Module* mod) {
        CONTRACTL {
            THROWS;
            GC_NOTRIGGER;
        } CONTRACTL_END;
        m_ValueArrays = new ValueArrayDescr(sig, mod, m_ValueArrays);
        return(m_ValueArrays);
    }

        // Value arrays descriptors have the low order bits set to indicate
        // that it is a value array descriptor
    static CORINFO_CLASS_HANDLE markAsValueArray(ValueArrayDescr* descr) {
        LEAF_CONTRACT;
        _ASSERTE(((size_t)descr & 3) == 0);
        return(CORINFO_CLASS_HANDLE((size_t)descr | 3));
    }
    static bool isValueArray(CORINFO_CLASS_HANDLE clsHnd) {
        LEAF_CONTRACT;
        return(((size_t)clsHnd & 3) == 3);
    }
    static ValueArrayDescr* asValueArray(CORINFO_CLASS_HANDLE clsHnd) {
        LEAF_CONTRACT;
        _ASSERTE(isValueArray(clsHnd));
        return((ValueArrayDescr*)(((size_t)clsHnd & ~3)));
    }

    ValueArrayDescr* m_ValueArrays;     // descriptions of value class arrays
#endif

#ifdef _DEBUG
    InlineSString<MAX_CLASSNAME_LENGTH> ssClsNameBuff;
    ScratchBuffer<MAX_CLASSNAME_LENGTH> ssClsNameBuffScratch;
#endif

public:

    //@GENERICS:
    // The method handle is used to instantiate method and class type parameters
    // It's also used to determine whether an extra dictionary parameter is required
    static void ConvToJitSig(PCCOR_SIGNATURE sig,
                                CORINFO_MODULE_HANDLE scopeHnd,
                                mdToken token,
                                CORINFO_SIG_INFO* sigRet,
                                CORINFO_METHOD_HANDLE context,
                                bool localSig,
                                TypeHandle owner = TypeHandle());

protected:

    // Cached type handles
    static MethodTable * s_pRuntimeTypeHandle;
    static MethodTable * s_pRuntimeMethodHandle;
    static MethodTable * s_pRuntimeFieldHandle;
    static MethodTable * s_pRuntimeArgumentHandle;
    static MethodTable * s_pTypedReference;

    static void InitStaticTypeHandles();

    ICorDynamicInfo *       m_pOverride;

    // Lookup/load a token and work out if it involves shared type variables or not
    void ScanForModuleDependencies(Module* pModule, PCCOR_SIGNATURE sigPtr, BOOL bIgnoreValueTypes);
    VarKind ScanToken(Module* pModule, mdToken token, CORINFO_CONTEXT_HANDLE pContext, BOOL bScanModuleDeps);
    void AddDependencyOnClassToken(Module* pModule, mdToken tk, BOOL bScanParents);
    void CheckTokenKind(CORINFO_CLASS_HANDLE  clsHnd,
                        CorInfoTokenKind      tokenKind);

    CORINFO_GENERIC_HANDLE loadTokenInternal(
                CORINFO_MODULE_HANDLE  scopeHnd,
                CORINFO_ANNOTATION     annotation,
                mdToken                metaTOK,
                CORINFO_CONTEXT_HANDLE context,
                CorInfoTokenKind       tokenKind,
                TypeHandle *           pTH,
                MethodDesc **          ppMeth,
                FieldDesc **           ppFld);

    // Internal version of findClass - can return byrefs
    CORINFO_CLASS_HANDLE findClassInternal(
                CORINFO_MODULE_HANDLE  scopeHnd,
                unsigned               annotatedMetaTOK,
                CORINFO_CONTEXT_HANDLE context,
                CorInfoTokenKind       tokenKind);

    CORINFO_FIELD_HANDLE CEEInfo::findFieldInternal (
                                       CORINFO_MODULE_HANDLE   scopeHnd,
                                       unsigned                metaTOK,
                                       CORINFO_CONTEXT_HANDLE  context,
                                       CorInfoTokenKind        tokenKind);

    // Internal version of findMethod - allows query on open instantiation generic methods
    CORINFO_METHOD_HANDLE findMethodInternal(
                CORINFO_MODULE_HANDLE  scopeHnd,
                mdToken                metaTOK,
                CORINFO_CONTEXT_HANDLE context,
                bool                   allowInstParam,
                CorInfoTokenKind       tokenKind);

    BOOL canAccessTypeInternal(
            CORINFO_METHOD_HANDLE       context,
            CORINFO_CLASS_HANDLE        target,
            BOOL                        fullAccessCheck = TRUE);

    // If a token contains shared type variables then prepare the information
    // about how to do a runtime lookup of the handle.
    static void ComputeRuntimeLookupForSharedGenericToken(Module *pModule,
                                                   DictionaryEntryLayout *pLayout,
                                                   MethodDesc* pContextMD,
                                                   VarKind varKind,
                                                   CORINFO_LOOKUP *pResultKind);
};


/*********************************************************************/

class  IJitManager;
struct _hpCodeHdr;
typedef struct _hpCodeHdr CodeHeader;

#ifdef _MSC_VER
#pragma warning (disable:4250)
#endif

class CEEJitInfo : public CEEInfo, public ICorJitInfo
{
public:

    // ICorJitInfo stuff

    IEEMemoryManager* __stdcall getMemoryManager();

    void __stdcall allocMem (
            ULONG               hotCodeSize,    /* IN */
            ULONG               coldCodeSize,   /* IN */
            ULONG               roDataSize,     /* IN */
            ULONG               rwDataSize,     /* IN */
            ULONG               xcptnsCount,    /* IN */
            CorJitAllocMemFlag  flag,           /* IN */
            void **             hotCodeBlock,   /* OUT */
            void **             coldCodeBlock,  /* OUT */
            void **             roDataBlock,    /* OUT */
            void **             rwDataBlock     /* OUT */
            );


    void * __stdcall allocGCInfo (ULONG   size);
    void * __stdcall getEHInfo();

    void   __stdcall setEHcount (unsigned cEH);

    void   __stdcall setEHinfo (
            unsigned      EHnumber,
            const CORINFO_EH_CLAUSE* clause);

    void __stdcall getEHinfo(
            CORINFO_METHOD_HANDLE ftn,              /* IN  */
            unsigned      EHnumber,                 /* IN */
            CORINFO_EH_CLAUSE* clause               /* OUT */
            );

    void __stdcall yieldExecution();

    BOOL _cdecl logMsg(unsigned level, const char* fmt, va_list args);
    virtual int __stdcall doAssert(const char* szFile, int iLine, const char* szExpr);

    CorInfoHelpFunc __stdcall getNewHelper(CORINFO_CLASS_HANDLE newCls,  CORINFO_METHOD_HANDLE  scopeHnd,
                    unsigned classToken, CORINFO_MODULE_HANDLE tokenContext);

    HRESULT __stdcall allocBBProfileBuffer (
        ULONG                         size,
        ICorJitInfo::ProfileBuffer ** profileBuffer
    );

    HRESULT __stdcall getBBProfileData (
        CORINFO_METHOD_HANDLE         ftnHnd,
        ULONG *                       size,
        ICorJitInfo::ProfileBuffer ** profileBuffer,
        ULONG *                       numRuns
    );


    bool isVerifyOnly()
    {
        return !!(m_compileFlags & CORJIT_FLG_IMPORT_ONLY);
    }

    CodeHeader* GetCodeHeader()
    {
        LEAF_CONTRACT;
        return m_CodeHeader;
    }

    CEEJitInfo(MethodDesc* fd,  COR_ILMETHOD_DECODER* header, 
               IJitManager* jm, CorJitFlag compileFlags)
        : m_jitManager(jm),
          m_FD(fd),
          m_compileFlags(compileFlags),
          m_CodeHeader(NULL),
          m_ILHeader(header)
#ifdef USE_INDIRECT_CODEHEADER
          , m_totalCodeHeaderSize(0),
          m_usedCodeHeaderSize(0),
          m_realCodeHeaderBlock(0)
#endif
          , m_GCinfo_len(0),
          m_EHinfo_len(0),
          m_gphCache()
    {
        LEAF_CONTRACT;
    }


    // ICorDebugInfo stuff.
    void __stdcall setBoundaries(CORINFO_METHOD_HANDLE ftn,
                                 ULONG32 cMap, OffsetMapping *pMap);
    void __stdcall setVars(CORINFO_METHOD_HANDLE ftn, ULONG32 cVars,
                           NativeVarInfo *vars);

    // Override active dependency to talk to loader
    void __stdcall addActiveDependency(CORINFO_MODULE_HANDLE moduleFrom, CORINFO_MODULE_HANDLE moduleTo);

    // Override of CEEInfo::GetProfilingHandle.  The first time this is called for a
    // method desc, it calls through to CEEInfo::GetProfilingHandle and caches the
    // result in CEEJitInfo::GetProfilingHandleCache.  Thereafter, this wrapper regurgitates the cached values
    // rather than calling into CEEInfo::GetProfilingHandle each time.  This avoids
    // making duplicate calls into the profiler's FunctionIDMapper callback.
    void __stdcall GetProfilingHandle(
                    CORINFO_METHOD_HANDLE      method,
                    BOOL                      *pbHookFunction,
                    void                     **pEEHandle,
                    void                     **pProfilerHandle,
                    BOOL                      *pbIndirectedHandles
                    );

    void BackoutJitData(IJitManager * jitMgr);

protected :
    IJitManager*            m_jitManager;   // responsible for allocating memory
    MethodDesc*             m_FD;           // method being compiled
    CorJitFlag              m_compileFlags; // compiler options
    CodeHeader*             m_CodeHeader;   // descriptor for JITTED code
    COR_ILMETHOD_DECODER *  m_ILHeader;     // the code header as exist in the file
    OBJECTREF               pThrowable;     // pointer to exception object
#ifdef USE_INDIRECT_CODEHEADER
    ULONG                   m_totalCodeHeaderSize;
    ULONG                   m_usedCodeHeaderSize;
    BYTE *                  m_realCodeHeaderBlock;
#endif


    size_t                  m_GCinfo_len;   // Cached copy of GCinfo_len so we can backout in BackoutJitData()
    size_t                  m_EHinfo_len;   // Cached copy of EHinfo_len so we can backout in BackoutJitData()

    // The first time a call is made to CEEJitInfo::GetProfilingHandle() from this thread
    // for this method, these values are filled in.   Thereafter, these values are used
    // in lieu of calling into the base CEEInfo::GetProfilingHandle() again.  This protects the
    // profiler from duplicate calls to its FunctionIDMapper() callback.
    struct GetProfilingHandleCache
    {
        GetProfilingHandleCache() :
            m_bGphIsCacheValid(false),
            m_bGphHookFunction(false),
            m_bGphIndirectedHandles(false),
            m_pvGphEEHandle(NULL),
            m_pvGphProfilerHandle(NULL)
        {
            LEAF_CONTRACT;
        }
          
        bool                    m_bGphIsCacheValid : 1;        // Tells us whether below values are valid
        bool                    m_bGphHookFunction : 1;
        bool                    m_bGphIndirectedHandles : 1;
        void*                  m_pvGphEEHandle;
        void*                  m_pvGphProfilerHandle;
    } m_gphCache;


};


/*********************************************************************/
/*********************************************************************/

typedef struct {
    void * pfnHelper;
#ifdef _DEBUG
    const char* name;
#endif
} VMHELPDEF;

extern "C" const VMHELPDEF hlpFuncTable[CORINFO_HELP_COUNT+1];

#if defined(_DEBUG) && (defined(_AMD64_) || defined(_X86_))
typedef struct {
    void*       pfnRealHelper;
    const char* helperName;
    LONG        count;
    LONG        helperSize;
} VMHELPCOUNTDEF;

extern "C" VMHELPCOUNTDEF hlpFuncCountTable[CORINFO_HELP_COUNT+1];

void InitJitHelperLogging();
void WriteJitHelperCountToSTRESSLOG();
#else
inline void InitJitHelperLogging() { }
inline void WriteJitHelperCountToSTRESSLOG() { }
#endif

// enum for dynamically assigned helper calls
enum DynamicCorInfoHelpFunc {
#define JITHELPER(code, pfnHelper)
#define DYNAMICJITHELPER(code, pfnHelper) DYNAMIC_##code,
#include "jithelpers.h"
    DYNAMIC_CORINFO_HELP_COUNT
};

#ifdef _MSC_VER
// GCC complains about duplicate "extern". And it is not needed for the GCC build
extern "C"
#endif
GARY_DECL(VMHELPDEF, hlpDynamicFuncTable, DYNAMIC_CORINFO_HELP_COUNT);

#define SetJitHelperFunction(ftnNum, pFunc) _SetJitHelperFunction(DYNAMIC_##ftnNum, pFunc)
void    _SetJitHelperFunction(DynamicCorInfoHelpFunc ftnNum, void * pFunc);

#ifdef _X86_

#include <pshpack1.h>
typedef struct
{
    BYTE Bytes[41];
} JIT_WriteBarrierReg_Thunk;
#include <poppack.h>

#ifdef HARDBOUND_DYNAMIC_CALLS
typedef struct
{
    DECLSPEC_ALIGN(8) BYTE Bytes[8];
} JIT_DynamicCall_Thunk;
#endif

typedef struct
{
    JIT_WriteBarrierReg_Thunk   WriteBarrierReg[8]; // 8 copies for 8 regs
#ifdef HARDBOUND_DYNAMIC_CALLS
    JIT_DynamicCall_Thunk       DynamicHCall[DYNAMIC_CORINFO_HELP_COUNT];
    JIT_DynamicCall_Thunk       DynamicFCall[ECall::NUM_DYNAMICALLY_ASSIGNED_FCALL_IMPLEMENTATIONS];
#endif
} JIT_Writeable_Thunks;

#ifdef DACCESS_COMPILE
GVAL_DECL(JIT_Writeable_Thunks, JIT_Writeable_Thunks_Buf);
#elif defined(_MSC_VER)
#pragma code_seg(push, ".text")
__declspec(allocate(".text")) extern JIT_Writeable_Thunks JIT_Writeable_Thunks_Buf; 
#pragma code_seg(pop)
#else
extern const JIT_Writeable_Thunks JIT_Writeable_Thunks_Buf;
#endif //_MSC_VER

extern JIT_WriteBarrierReg_Thunk *JIT_WriteBarrierReg_Buf;

extern BYTE *JIT_WriteBarrier_Buf_Start;
extern BYTE *JIT_WriteBarrier_Buf_Last;

class JIT_TrialAlloc
{
public:
    enum Flags
    {
        NORMAL       = 0x0,
        MP_ALLOCATOR = 0x1,
        SIZE_IN_EAX  = 0x2,
        OBJ_ARRAY    = 0x4,
        ALIGN8       = 0x8,     // insert a dummy object to insure 8 byte alignment (until the next GC)
        ALIGN8OBJ    = 0x10,
        NO_FRAME     = 0x20,    // call is from unmanaged code - don't try to put up a frame
        CHKRESTORE   = 0x40,
    };

    static void *GenAllocSFast(Flags flags);
    static void *GenBox(Flags flags);
    static void *GenAllocArray(Flags flags);
    static void *GenAllocString(Flags flags);

private:
    static void EmitAlignmentRoundup(CPUSTUBLINKER *psl,X86Reg regTestAlign, X86Reg regToAdj, Flags flags);
    static void EmitDummyObject(CPUSTUBLINKER *psl, X86Reg regTestAlign, Flags flags);
    static void EmitCore(CPUSTUBLINKER *psl, CodeLabel *noLock, CodeLabel *noAlloc, Flags flags);
    static void EmitNoAllocCode(CPUSTUBLINKER *psl, Flags flags);

#if CHECK_APP_DOMAIN_LEAKS
    static void EmitSetAppDomain(CPUSTUBLINKER *psl);
    static void EmitCheckRestore(CPUSTUBLINKER *psl);
#endif
};
#endif // _X86_

#ifdef HARDBOUND_DYNAMIC_CALLS
BYTE* GetDynamicFCallThunk(int index);
BYTE* GetDynamicHCallThunk(int index);
#endif

void *GenFastGetSharedStaticBase(bool bCheckCCtor);


EXTERN_C FCDECL2(FC_BOOL_RET, ArrayStoreCheck, Object** pElement, PtrArray** pArray);
FCDECL1(StringObject*, FramedAllocateString, DWORD stringLength);
FCDECL1(StringObject*, UnframedAllocateString, DWORD stringLength);

OBJECTHANDLE ConstructStringLiteral(CORINFO_MODULE_HANDLE scopeHnd, mdToken metaTok);

FCDECL1(Object*, JIT_NewFast, CORINFO_CLASS_HANDLE typeHnd_);
FCDECL1(Object*, JIT_New, CORINFO_CLASS_HANDLE typeHnd_);
FCDECL2(Object*, JIT_NewArr1, CORINFO_CLASS_HANDLE typeHnd_, int size);
FCDECL2(Object*, JIT_Box, CORINFO_CLASS_HANDLE type, void* data);

BOOL ObjIsInstanceOf(Object *pObject, TypeHandle toTypeHnd);
EXTERN_C TypeHandle::CastResult ObjIsInstanceOfNoGC(Object *pObject, TypeHandle toTypeHnd);



#ifdef _DEBUG
extern LONG g_JitCount;
#endif


CORINFO_GENERIC_HANDLE JIT_GenericHandleWorker(MethodDesc              *pMD,
                                               TypeHandle               declaringCls,
                                               unsigned                 annotatedMetaTOK1,
                                               unsigned                 token2,
                                               CORINFO_GENERIC_HANDLE  *slotPtr);

void ClearJitGenericHandleCache(AppDomain *pDomain);


#endif // JITINTERFACE_H
