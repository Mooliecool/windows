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

#include "reader.h"
#include "readerir.h"
#include "utilcode.h"

    #define _JIT64_PEV_

    #include "vererror.h"

    #define ASSERTM(p,m)   do { if (!(p)) {ReaderBase::GenIR_DebugError(__FILE__,__LINE__,m);}} while (0)
    #define ASSERT(p)      ASSERTM(p,#p)
    #define UNREACHED      0
#ifndef _MSC_VER
    #define ASSUME(p)      __assume(p)
#else
    #define ASSUME(p)
#endif

    #if !defined(_DEBUG)
    #define NODEBUG 1
    #define RELEASE 1
    #endif

    #ifndef NODEBUG
    #define ASSERTMNR(p,m) ASSERTM(p,m)
    #define ASSERTNR(p)    ASSERTMNR(p,#p)
    #define ASSERTDBG(p)   ASSERTM(p,#p)
    #define TODO()         ASSERTMNR(0,"TODO\n")
    #else
    #define ASSERTMNR(p,m) ASSUME(p)
    #define ASSERTNR(p)    ASSUME(p)
    #define ASSERTDBG(p)
    #define TODO()
    #endif



#include "jit.h"
#include "gverify.h"
#include "corerror.h"
#include "newvstate.h"
#include <limits.h>

#if !defined(_DEBUG)
#define NODEBUG 1
#define RELEASE 1
#endif

#define NO_WAY(x) Verify(0,x)
#define BADCODE(c) (ReaderBase::verGlobalError(c))
#define BADCODE3(x,y,z) (ReaderBase::verGlobalError(x))
#define IMPL_LIMITATION(c) (ReaderBase::GenIR_DebugError(__FILE__,__LINE__,c))

#ifdef VASSERT
#undef VASSERT
#endif
#define VASSERT(x)     ASSERTNR(x)

#define VP(s)

const CORINFO_CLASS_HANDLE  BAD_CLASS_HANDLE    = (CORINFO_CLASS_HANDLE) -1;

#define verbose 0

#define Verify(cond, msg)                   ReaderBase::VerifyOrReturn(cond, msg)
#define verCompatibleWith(child, parent)    tiCompatibleWith(m_jitInfo, child, parent)

extern const char* opcodeName[];

#ifdef _MSC_VER
#pragma warning(push,4)
#endif

HRESULT              verLastError;

#define MAX_TYPE_STRING_SIZE 512


void GetErrorMessage(ReaderBase *rbase, ICorJitInfo* pJitInfo)
{
    pJitInfo->GetErrorMessage(rbase->extended_error_message, ERROR_MSG_SIZE);
    //wprintf(extended_error_message);
}

#ifdef DEBUG

inline void ReaderBase::verDumpType(const vertype &v)
{
    WCHAR wszTypeFound[MAX_TYPE_STRING_SIZE];


    v.Dump();
    CORINFO_CLASS_HANDLE c = v.m_cls;//v.GetClassHandle();
    if (c)
        printf("\nname:%s\n", m_jitInfo->getClassName(c));

    v.ToString(wszTypeFound, MAX_TYPE_STRING_SIZE, m_jitInfo, getCurrentMethodHandle());
    wprintf(L"%s", wszTypeFound);
}

#endif

inline bool ReaderBase::verIsValueClass(CORINFO_CLASS_HANDLE clsHnd)
{
    return (getClassAttribs(clsHnd) & CORINFO_FLG_VALUECLASS) != 0;
}

inline bool ReaderBase::verIsValClassWithStackPtr(CORINFO_CLASS_HANDLE clsHnd)
{
    DWORD flags = getClassAttribs(clsHnd);
    return (flags & CORINFO_FLG_VALUECLASS)
        && (flags & CORINFO_FLG_CONTAINS_STACK_PTR);
}

inline bool ReaderBase::verIsGenericTypeVar(CORINFO_CLASS_HANDLE clsHnd)
{
    return (getClassAttribs(clsHnd) & CORINFO_FLG_GENERIC_TYPE_VARIABLE) != 0;
}

inline bool verTypeIsComposite(CorInfoType type)
{
    return type == CORINFO_TYPE_STRING
        || type == CORINFO_TYPE_PTR
        || type == CORINFO_TYPE_BYREF
        || type == CORINFO_TYPE_VALUECLASS
        || type == CORINFO_TYPE_CLASS
        || type == CORINFO_TYPE_REFANY;
}

void ReaderBase::VerifyIsDirectCallToken(mdToken tok)
{
#ifdef DEBUG
    if (verbose) printf(" %08X", tok);
#endif

    VerifyToken(tok);

    if (TypeFromToken(tok) == mdtMethodDef ||
        TypeFromToken(tok) == mdtMethodSpec ||
        TypeFromToken(tok) == mdtMemberRef)
        return;
    else
        Verify(false, MVER_E_TOKEN_TYPE_MEMBER);
}

bool ReaderBase::verIsCallToken(mdToken tok)
{
    // same as VerifyIsDirectCallToken but could also be a call site sig used in calli
    return (TypeFromToken(tok) == mdtMethodDef ||
            TypeFromToken(tok) == mdtMethodSpec ||
            TypeFromToken(tok) == mdtMemberRef ||
            TypeFromToken(tok) == mdtSignature);
}
void ReaderBase::VerifyIsCallToken(mdToken tok)
{
    // same as VerifyIsDirectCallToken but could also be a call site sig used in calli
#ifdef DEBUG
    if (verbose) printf(" %08X", tok);
#endif
    VerifyToken(tok);

    if (verIsCallToken(tok))
         return;
    else
        Verify(false, MVER_E_TOKEN_TYPE_MEMBER);

}

void ReaderBase::VerifyIsFieldToken(mdToken tok)
{
#ifdef DEBUG
    if (verbose) printf(" %08X", tok);
#endif
    if (TypeFromToken(tok) == mdtFieldDef ||
        TypeFromToken(tok) == mdtMemberRef)
        return;
    else
        Verify(false, MVER_E_TOKEN_TYPE_FIELD);
}

void ReaderBase::VerifyIsClassToken(mdToken tok)
{
#ifdef DEBUG
    if (verbose) printf(" %08X", tok);
#endif

    VerifyToken(tok);
    if (
        TypeFromToken(tok) == mdtTypeDef ||
        TypeFromToken(tok) == mdtTypeRef ||
        TypeFromToken(tok) == mdtTypeSpec)
        return;
    else
        Verify(false, MVER_E_TOKEN_TYPE_MEMBER);
}

CORINFO_CLASS_HANDLE ReaderBase::verGetClassHandle(mdToken tok, CorInfoTokenKind tokenKind)
{
// if in peverify try to trap it and continue
    // but if in JIT just let it go up
    CORINFO_CLASS_HANDLE clsHnd = NULL;
    PAL_TRY
    {
        clsHnd = ReaderBase::getClassHandle(tok,
                                            getCurrentMethodHandle(),
                                            getCurrentModuleHandle(),
                                            tokenKind);
    }
    PAL_EXCEPT_FILTER(ReaderBase::EEJITFilter, m_jitInfo)
    {
        GetErrorMessage(this, m_jitInfo);
        Verify(false, MVER_E_TOKEN_RESOLVE);
    }
    PAL_ENDTRY
    return clsHnd;
}


inline void ReaderBase::eeGetCallSiteSig     (unsigned                sigTok,
                                              CORINFO_MODULE_HANDLE   scope,
                                              CORINFO_METHOD_HANDLE   context,
                                              CORINFO_SIG_INFO*       sigRet,
                                              bool                    giveUp)
{
    m_jitInfo->findCallSiteSig(scope, sigTok, context, sigRet);

    if (giveUp &&
        sigRet->retTypeClass == NULL &&
        verTypeIsComposite(sigRet->retType)
        )
    {
        NO_WAY(MVER_E_TOKEN_RESOLVE);
    }
}

inline
void               ReaderBase::eeGetMethodSig      (CORINFO_METHOD_HANDLE  methHnd,
                                                    CORINFO_SIG_INFO*      sigRet,
                                                    bool                   giveUp,
                                                    CORINFO_CLASS_HANDLE   owner)
{
    m_jitInfo->getMethodSig(methHnd, sigRet, owner);

    if (giveUp &&
        verTypeIsComposite(sigRet->retType) &&
        sigRet->retTypeClass == NULL)
    {
        NO_WAY(MVER_E_TOKEN_RESOLVE);
    }
}


inline vertype ReaderBase::verGetArrayElemType(vertype ti)
{
    // you need to check for null explictly since that is a success case
    VASSERT(!ti.IsNullObjRef());

    VerifyIsSDArray(ti);

    CORINFO_CLASS_HANDLE childClassHandle = NULL;
    CorInfoType ciType = m_jitInfo->getChildType(ti.GetClassHandleForObjRef(), &childClassHandle);

    return verMakeTypeInfo(ciType, childClassHandle);
}

inline void ReaderBase::VerifyIsSDArray(const vertype &ti)
{
    if (ti.IsNullObjRef())      // nulls are SD arrays
        return;

    Verify(ti.IsType(TI_REF), MVER_E_ARRAY_SD);

    Verify(m_jitInfo->isSDArray(ti.GetClassHandleForObjRef()), MVER_E_ARRAY_SD);
}

inline bool ReaderBase::verIsByRefLike(const vertype& ti)
{
    if (ti.IsByRef())
        return TRUE;
    if (!ti.IsType(TI_STRUCT))
        return FALSE;
    if (getClassAttribs(ti.GetClassHandleForValueClass()) & CORINFO_FLG_CONTAINS_STACK_PTR)
        return TRUE;
    return FALSE;

}

bool ReaderBase::verIsSafeToReturnByRef(const vertype& ti)
{
    if (ti.IsPermanentHomeByRef())
    {
        return TRUE;
    }
    else
    {        
        return FALSE;
    }
}

BOOL ReaderBase::verIsBoxable(const vertype& ti)
{
    return (   ti.IsPrimitiveType()
               || ti.IsObjRef() // includes boxed generic type variables
               || ti.IsUnboxedGenericTypeVar()
               || (ti.IsType(TI_STRUCT) &&
                !verIsByRefLike(ti)));
}

// Is it a boxed value type?
bool ReaderBase::verIsBoxedValueType(const vertype &ti)
{
    if (ti.GetType() == TI_REF)
    {
        return verIsValueClass(ti.GetClassHandleForObjRef());
    }
    else
    {
        return false;
    }
}


void ReaderBase::VerifyIsBoxable(const vertype &v)
{
    VerifyAndReportFound(verIsBoxable(v), v, MVER_E_VALCLASS_OBJREF_VAR);
}

void ReaderBase::VerifyIsByref(const vertype &v)
{
    VerifyAndReportFound(v.IsByRef(), v, MVER_E_STACK_BYREF);
}

inline vertype ReaderBase::verMakeTypeInfo(CORINFO_CLASS_HANDLE clsHnd)
{
    if (clsHnd == NULL)
        return vertype();

    if (verIsValueClass(clsHnd))
    {
        CorInfoType t = m_jitInfo->getTypeForPrimitiveValueClass(clsHnd);

        // Meta-data validation should insure that CORINF_TYPE_BYREF should
        // not occur here, so we may want to change this to an assert instead.
        switch (t)
        {
        case CORINFO_TYPE_VOID:
        case CORINFO_TYPE_BYREF:
            return vertype();
            break;
        case CORINFO_TYPE_PTR:
            Verify(0, MVER_E_UNMANAGED_POINTER);
            return vertype();
            break;

        case CORINFO_TYPE_UNDEF:
        case CORINFO_TYPE_VALUECLASS:
            return vertype(TI_STRUCT, clsHnd);
            break;

        default:
            return vertype(JITtype2tiType(t));
            break;
        }
    }
    else if (verIsGenericTypeVar(clsHnd))
    {
        // See comment in vertypeInfo.h for why we do it this way.
        return(vertype(TI_REF, clsHnd, true));
    }
    else {
        return(vertype(TI_REF, clsHnd));
    }
}

vertype ReaderBase::verMakeTypeInfo(CorInfoType ciType, CORINFO_CLASS_HANDLE clsHnd)
{
    VASSERT(ciType < CORINFO_TYPE_COUNT);

    vertype tiResult;
    switch(ciType)
    {
    case CORINFO_TYPE_STRING:
    case CORINFO_TYPE_CLASS:

        tiResult = verMakeTypeInfo(clsHnd);
        Verify(tiResult.IsType(TI_REF), MVER_E_SIG_C_VC);
        break;

    case CORINFO_TYPE_VALUECLASS:
    case CORINFO_TYPE_REFANY:
        tiResult = verMakeTypeInfo(clsHnd);
        // type must be constant with element type;
        Verify(tiResult.IsValueClass(), MVER_E_SIG_VC_C);
        break;

    case CORINFO_TYPE_VAR:
        return verMakeTypeInfo(clsHnd);
        break;

    case CORINFO_TYPE_PTR: 
        return vertype(TI_PTR);
        break;


    case CORINFO_TYPE_VOID:
        return vertype();
        break;

    case CORINFO_TYPE_BYREF: 
    {
        CORINFO_CLASS_HANDLE childClassHandle;
        CorInfoType childType = m_jitInfo->getChildType(clsHnd, &childClassHandle);

        // cannot have a byref to a byref
        VerifyAndReportFound(childType != CORINFO_TYPE_BYREF, 
            tiResult, MVER_E_SIG_BYREF_BYREF);

        tiResult = verMakeTypeInfo(childType, childClassHandle);

        if (childClassHandle)
        {
            VerifyAndReportFound(!(CORINFO_FLG_CONTAINS_STACK_PTR & getClassAttribs(childClassHandle)), 
                                 tiResult, MVER_E_SIG_BYREF_TB_AH);
        }

        return ByRef(tiResult);
    }
    break;

    default:
        VASSERT(clsHnd != BAD_CLASS_HANDLE);
        if (clsHnd)       // If we have more precise information, use it
            return vertype(TI_STRUCT, clsHnd);
        else
            return vertype(JITtype2tiType(ciType));

    }
    return tiResult;
}

inline bool verTypeIsGC(CorInfoType type)
{
    return type == CORINFO_TYPE_CLASS || type == CORINFO_TYPE_BYREF;
}

inline vertype ReaderBase::verParseArgSigToTypeInfo(CORINFO_SIG_INFO* sig,
                                                    CORINFO_ARG_LIST_HANDLE args)

{
    CORINFO_CLASS_HANDLE classHandle = NULL;
    CorInfoType ciType = strip(m_jitInfo->getArgType(sig, args, &classHandle));

    if (verTypeIsGC(ciType))
    {
        // For efficiency, getArgType only returns something in classHandle for
        // value types.  For other types that have addition type info, you
        // have to call back explicitly
        classHandle = m_jitInfo->getArgClass(sig, args);
        //"Could not figure out Class specified in argument or local signature"
        if (!classHandle)
            BADCODE(MVER_E_TOKEN_RESOLVE);
        VASSERT(classHandle);
    }

    return verMakeTypeInfo(ciType, classHandle);
}


inline CORINFO_CLASS_HANDLE ReaderBase::verGetExactMethodClass(mdMemberRef token, CORINFO_METHOD_HANDLE method)
{
    mdToken parentTok = getMemberParent(token);
    // if method may be shared, find the exact parent from the token
    if (TypeFromToken(parentTok) == mdtTypeSpec)
        return verGetClassHandle(parentTok);
    else
        return getMethodClass(method);
}


inline CORINFO_CLASS_HANDLE ReaderBase::verGetExactFieldClass(mdMemberRef token, CORINFO_FIELD_HANDLE field)
{
    mdToken parentTok = getMemberParent(token);
    // if field may be shared, find the exact parent from the token
    if(TypeFromToken(parentTok) == mdtTypeSpec)
        return verGetClassHandle(parentTok);
    else
        return getFieldClass(field);
}

/*****************************************************************************
 *
 *              Functions to get various handles
 */

inline
CORINFO_CLASS_HANDLE ReaderBase::verifyClass(
    CORINFO_CLASS_HANDLE cls
){
    if (cls == 0) {
        NO_WAY(MVER_E_TOKEN_RESOLVE);
    }

    return(cls);
}

inline
CORINFO_METHOD_HANDLE ReaderBase::verifyMethodHandle(
    CORINFO_METHOD_HANDLE method
){
    if (method == 0) {
        BADCODE(MVER_E_TOKEN_RESOLVE);
    }
    return (method);
}


vertype
ReaderBase::verVerifyLDIND(
    const vertype& ptr,
    ti_types instrType
){
    vertype ptrVal;

    VASSERT(instrType != TI_STRUCT);

    if (ptr.IsByRef())
    {
        ptrVal = DereferenceByRef(ptr);
        if (instrType == TI_REF)
        {
            VerifyIsObjRef(ptrVal);
        }
        else
        {
            VerifyEqual(vertype(ptrVal).MakeByRef(), vertype(instrType).MakeByRef());
            VerifyAndReportFound(instrType == ptrVal.GetRawType(), ptr,
                                 MVER_E_STACK_UNEXPECTED);
        }
    }
    else {
        VerifyAndReportFound(false, ptr, MVER_E_STACK_BYREF);
    }


    return ptrVal;
}

vertype ReaderBase::verVerifySTIND(const vertype& ptr,
                                 const vertype& value, ti_types instrType)
{
    Verify(!ptr.IsReadonlyByRef(), MVER_E_READONLY_ILLEGAL_WRITE);//"write to readonly byref"
    vertype ptrVal = verVerifyLDIND(ptr, instrType);
    VerifyCompatibleWith(value, vertype(ptrVal).NormaliseForStack());
    return ptrVal;
}


BOOL ReaderBase::verIsCallToInitThisPtr(CORINFO_CLASS_HANDLE context,
                                        CORINFO_CLASS_HANDLE target)
{
    // Either target == context, in this case calling an alternate .ctor
    // Or target is the immediate parent of context

    return ((target == context) ||
            (target == m_jitInfo->getParentType(context)));
}


/*****************************************************************************
 *  Checks that a delegate creation is done using the following pattern:
 *     dup
 *     ldvirtftn targetMemberRef
 *  OR
 *     ldftn targetMemberRef
 *
 * 'delegateCreateStart' points at the last dup or ldftn in this basic block (null if
 *  not in this basic block) 
 *
 *  targetMemberRef is read from the code sequence.
 *  targetMemberRef is validated iff verificationNeeded.
 */

bool ReaderBase::verCheckDelegateCreation(ReaderBaseNS::OPCODE opcode,
                                          VerificationState *vstate,
                                          const BYTE *codeAddr,
                                          mdMemberRef &targetMemberRef,
                                          vertype ftnType,
                                          vertype objType)
{
    unsigned char *pToken;
    unsigned char *delegateCreateStart = (unsigned char *) vstate->delegateCreateStart;

    if (!delegateCreateStart)
        return false;

    unsigned int index = 0;

    if (codeAddr - delegateCreateStart == 6)        // LDFTN <TOK> takes 6 bytes
    {
        if (ParseMSILOpcode(delegateCreateStart, &pToken, &index, this) != ReaderBaseNS::CEE_LDFTN)
            return false;
        else
        {
            DWORD targetFlags = getMethodAttribs(ftnType.m_method);
            
            if ((targetFlags & CORINFO_FLG_VIRTUAL) && ((targetFlags & CORINFO_FLG_FINAL) == 0))
            {
                if (!verIsBoxedValueType(objType))
                {
                    Verify(objType.IsThisPtr(), MVER_E_LDFTN_NON_FINAL_VIRTUAL);
                    Verify(!m_thisPtrModified, MVER_E_LDFTN_NON_FINAL_VIRTUAL);
                }
            }

            targetMemberRef = ReadToken(pToken);
            return true;
        }
    }
    else if (codeAddr - delegateCreateStart == 7)       // DUP LDVIRTFTN <TOK> takes 7 bytes
    {
        if (ParseMSILOpcode(delegateCreateStart, NULL, &index, this) != ReaderBaseNS::CEE_DUP)
            return false;

        if (ParseMSILOpcode(delegateCreateStart + index, &pToken, &index, this) != ReaderBaseNS::CEE_LDVIRTFTN)
            return false;
        targetMemberRef = ReadToken(pToken);
        return true;
    }
    else
        return false;
}

void ReaderBase::verVerifyCall (
    ReaderBaseNS::OPCODE opcode,
    mdToken              memberRef,
    bool                 tailCall,
    const BYTE*          codeAddr,
    VerificationState*   vstate
){
    CORINFO_METHOD_HANDLE   methodHnd;
    DWORD                   mflags;
    CORINFO_SIG_INFO        sig;
    unsigned int            popCount = 0;
    bool                    isInlining = false;
    unsigned int            argCount = 0;
    CORINFO_ARG_LIST_HANDLE args = NULL;


    VerifyIsCallToken(memberRef);

    methodHnd = verifyMethodHandle(getMethodHandle(memberRef));

    mflags = getMethodAttribs(methodHnd);

    // When verifying we always need the exact, never the shared, class of the method
    // (getMethodClass will return a shared class if the method handle is in a shared class)
    CORINFO_CLASS_HANDLE methodClassHnd = verGetExactMethodClass(memberRef, methodHnd);
    VASSERT(methodClassHnd);

    eeGetMethodSig(methodHnd, &sig, false, methodClassHnd);

    if (sig.isVarArg())
        eeGetCallSiteSig(memberRef, getCurrentModuleHandle(), getCurrentMethodHandle(), &sig, !isInlining);

    DWORD clsFlags = getClassAttribs(methodClassHnd);

    // opcode specific check
    unsigned methodClassFlgs = getClassAttribs(methodClassHnd);

    VASSERT(!tailCall);      // Importer should not allow this
    Verify((mflags & CORINFO_FLG_CONSTRUCTOR) && !(mflags & CORINFO_FLG_STATIC), MVER_E_CTOR);

    if (methodClassFlgs & CORINFO_FLG_DELEGATE)
    {

        Verify(sig.numArgs == 2, MVER_E_DLGT_CTOR);
        vertype tiDeclaredObj = verParseArgSigToTypeInfo(&sig, sig.args).NormaliseForStack();
        vertype tiDeclaredFtn = verParseArgSigToTypeInfo(&sig, m_jitInfo->getArgNext(sig.args)).NormaliseForStack();

        //"ftn arg needs to be a type IntPtr"
        Verify(tiDeclaredFtn.IsType(TI_I), MVER_E_DLGT_SIG_I);

        VASSERT(popCount == 0);
        vertype tiActualObj = vstate->impStackTop(1);
        vertype tiActualFtn = vstate->impStackTop(0);

        // delegate needs method as first arg
        Verify(tiActualFtn.IsMethod(), MVER_E_STACK_METHOD);

        VerifyCompatibleWith(tiActualObj, tiDeclaredObj); // delegate object mismatch
        Verify(tiActualObj.IsNullObjRef() || tiActualObj.IsObjRef(), MVER_E_DLGT_SIG_O);

        CORINFO_CLASS_HANDLE objTypeHandle = tiActualObj.IsNullObjRef() ? NULL
                                                                        : tiActualObj.GetClassHandleForObjRef();
        mdToken delegateMethodRef = mdTokenNil;



        Verify(verCheckDelegateCreation(opcode, vstate, codeAddr, delegateMethodRef, 
                                        tiActualFtn, tiActualObj),
               MVER_E_DLGT_PATTERN); //"must create delegates with certain IL"

        // the method signature must be compatible with the delegate's invoke method

        // It is crucial to have verified verCheckDelegateCreation before using vstate->delegateMethodRef below;
        // This ensures delegateMethodRef was last set by a valid delegate creation sequence
        // For now, we just check it is at least a valid token.

        VerifyIsDirectCallToken(delegateMethodRef);
        VASSERT(delegateMethodRef == vstate->delegateMethodRef);

        CORINFO_CLASS_HANDLE parentTypeHandle = verGetExactMethodClass(delegateMethodRef, tiActualFtn.GetMethod());

        Verify(m_jitInfo->isCompatibleDelegate(objTypeHandle,
                                               parentTypeHandle,
                                               tiActualFtn.GetMethod(),
                                               methodClassHnd,
                                               getCurrentModuleHandle(),
                                               delegateMethodRef,
                                               memberRef),
               MVER_E_DLGT_CTOR);


        // in the case of protected methods, it is a requirement that the 'this'
        // pointer be a subclass of the current context.  Perform this check
        BOOL targetIsStatic = m_jitInfo->getMethodAttribs(
                                        tiActualFtn.GetMethod(),
                                        getCurrentMethodHandle()) & CORINFO_FLG_STATIC;
        CORINFO_CLASS_HANDLE instanceClassHnd = getCurrentMethodClass();
        if (!(tiActualObj.IsNullObjRef() || targetIsStatic))
            instanceClassHnd = tiActualObj.GetClassHandleForObjRef();

        Verify(m_jitInfo->satisfiesClassConstraints(parentTypeHandle),
                       MVER_E_UNSATISFIED_METHOD_PARENT_INST); //"delegate target has unsatisfied class constraints"
        Verify(m_jitInfo->satisfiesMethodConstraints(parentTypeHandle,tiActualFtn.GetMethod()),
                       MVER_E_UNSATISFIED_METHOD_INST); //"delegate target has unsatisfied method constraints");

        Verify(m_jitInfo->canAccessMethod(getCurrentMethodHandle(),
                                          parentTypeHandle,
                                          tiActualFtn.GetMethod(),
                                          instanceClassHnd),
               MVER_E_METHOD_ACCESS);


        goto DONE_ARGS;
    }
    Verify(!(mflags & CORINFO_FLG_ABSTRACT),  MVER_E_CALL_ABSTRACT);

    Verify(!((mflags & CORINFO_FLG_CONSTRUCTOR) && (methodClassFlgs & CORINFO_FLG_DELEGATE)),
           MVER_E_DLGT_PATTERN); //"can only newobj a delegate constructor"

    // check compatibility of the arguments
    argCount  = sig.numArgs;
    args = sig.args;

    while (argCount--)
    {
        vertype tiActual = vstate->impStackTop(popCount+argCount);
        vertype tiDeclared = verParseArgSigToTypeInfo(&sig, args).NormaliseForStack();
        VerifyCompatibleWith(tiActual, tiDeclared);

        // check that the argument is not a byref for tailcalls
        if (tailCall)
            VerifyAndReportFound(!verIsByRefLike(tiDeclared), tiDeclared, MVER_E_TAIL_BYREF);

        args = m_jitInfo->getArgNext(args);
    }


DONE_ARGS:

    // update popCount
    popCount += sig.numArgs;

    // check for 'this' which are on non-static methods, not called via NEWOBJ
    CORINFO_CLASS_HANDLE instanceClassHnd = getCurrentMethodClass();

    if (!(mflags & CORINFO_FLG_STATIC) && (opcode != ReaderBaseNS::CEE_NEWOBJ))
    {
        vertype tiThis = vstate->impStackTop(popCount);
        popCount++;

        if (getClassAttribs(methodClassHnd) & CORINFO_FLG_ARRAY)
            methodClassHnd = m_jitInfo->findMethodClass(getCurrentModuleHandle(), memberRef, getCurrentMethodHandle());

        // If it is null, we assume we can access it (since it will AV shortly)
        // If it is anything but a refernce class, there is no hierarchy, so
        // again, we don't need the precise instance class to compute 'protected' access
        if (tiThis.IsType(TI_REF))
            instanceClassHnd = tiThis.GetClassHandleForObjRef();

        // Check type compatability of the this argument
        vertype tiDeclaredThis = verMakeTypeInfo(methodClassHnd);

        // this enables the following construct:
        // local : native int pbase,
        // ldarga.s   pbase
        // call       instance int32 System.IntPtr::ToInt32()
        if (tiDeclaredThis.IsValueClass())
            tiDeclaredThis.MakeByRef();

        // If this is a call to the base class .ctor, set thisPtr Init for
        // this block.
        if (mflags & CORINFO_FLG_CONSTRUCTOR)
        {
            if (m_verTrackObjCtorInitState && tiThis.IsThisPtr()
                && verIsCallToInitThisPtr(getCurrentMethodClass(),methodClassHnd))
            {
                vstate->setThisInitialized();
                vstate->thisInitializedThisBlock = true;
                tiThis.SetInitialisedObjRef();
            }
            else
            {
                // We allow direct calls to value type constructors
                Verify(tiThis.IsByRef(), MVER_E_CALL_CTOR);
            }
        }

        VerifyCompatibleWith(tiThis, tiDeclaredThis);

        // also check the specil tailcall rule
        VerifyAndReportFound(!(tailCall && verIsByRefLike(tiDeclaredThis)), tiDeclaredThis, MVER_E_TAIL_BYREF);

    }

    // check any constraints on the callee's class and type parameters
    Verify(m_jitInfo->satisfiesClassConstraints(methodClassHnd),
                   MVER_E_UNSATISFIED_METHOD_PARENT_INST); //"method has unsatisfied class constraints
    Verify(m_jitInfo->satisfiesMethodConstraints(methodClassHnd,methodHnd),
                   MVER_E_UNSATISFIED_METHOD_INST); //"method has unsatisfied method constraints"

    // check access permission
    Verify(m_jitInfo->canAccessMethod(getCurrentMethodHandle(),
                                              methodClassHnd,
                                              methodHnd,
                                              instanceClassHnd), MVER_E_METHOD_ACCESS);

    // special checks for tailcalls
    if (tailCall)
    {

        // void return type gets morphed into the error type, so we have to treat them specially here
        if (sig.retType == CORINFO_TYPE_VOID)
        {
            Verify(m_methodInfo->args.retType == CORINFO_TYPE_VOID, MVER_E_TAIL_RET_VOID);
        }
        else if (m_methodInfo->args.retType == CORINFO_TYPE_VOID)
        {
            Verify(false, MVER_E_TAIL_RET_TYPE);
        }
        else 
        {
            vertype tiCalleeRetType = verMakeTypeInfo(sig.retType, sig.retTypeClass);
            vertype tiCallerRetType = verMakeTypeInfo(m_methodInfo->args.retType,
                                                      m_methodInfo->args.retTypeClass);

            VerifyCompatibleWith(NormaliseForStack(tiCalleeRetType), 
                                 NormaliseForStack(tiCallerRetType));
        }

        // for tailcall, stack must be empty
    }

    if  (sig.retType != CORINFO_TYPE_VOID)
    {
        if (clsFlags & CORINFO_FLG_ARRAY)
        {
            eeGetCallSiteSig(memberRef, getCurrentModuleHandle(), getCurrentMethodHandle(), &sig);
        }

        vertype tiRetVal = verMakeTypeInfo(sig.retType, sig.retTypeClass);
        tiRetVal.NormaliseForStack();
        vstate->push(tiRetVal);
    }
}

// ////////////////////////////////////////////////////////////////////////////////
// OPCODE VERIFICATION
// ////////////////////////////////////////////////////////////////////////////////


void
ReaderBase::initVerifyInfo(
    void
){
    m_needsRuntimeCallout = false;
    m_verificationNeeded = verifyNeedsVerification(&m_canSkipVerificationResult);
    m_isVerifiableCode = TRUE; // assume the code is verifiable unless proven otherwise
    m_verBBList = NULL;
    m_verTrackObjCtorInitState = true;
    m_thisPtrModified = false;

    // initialize and check constraints
    if (m_verificationNeeded) 
    {
        BOOL hasCircularClassConstraints,hasCircularMethodConstraints;
        m_jitInfo->initConstraintsForVerification(getCurrentMethodHandle(),
                                                  &hasCircularClassConstraints,
                                                  &hasCircularMethodConstraints);

    
        if (hasCircularClassConstraints)
            BADCODE(MVER_E_CIRCULAR_VAR_CONSTRAINTS);
        if (hasCircularMethodConstraints)
            BADCODE(MVER_E_CIRCULAR_MVAR_CONSTRAINTS);
    }      
    else if (!(m_flags & CORJIT_FLG_SKIP_VERIFICATION))
    { 
       CorInfoCanSkipVerificationResult verResult = m_jitInfo->canSkipMethodVerification(getCurrentMethodHandle(), false);
       if (verResult == CORINFO_VERIFICATION_RUNTIME_CHECK)
       {
          m_verificationNeeded = true;  
          m_needsRuntimeCallout = true;
       }
    }
}


void ReaderBase::verifyFinishBlock(VerificationState* vstate, FlowGraphNode * block)
{
    PGLOBALVERIFYDATA gvData;
    bool blockIsBad;

    if (!m_verificationNeeded) return;

    gvData = FgNodeGetGlobalVerifyData(block);
    ASSERTNR(gvData);

    blockIsBad = vstate->blockIsBad;

    if (!blockIsBad)
    {
        if (vstate->tailPrefix == true) {
            Verify(0, MVER_E_TAIL_CALL);
            blockIsBad = true;
        }
        if (vstate->unalignedPrefix == true) {
            blockIsBad = true;
            Verify(0, MVER_E_UNALIGNED);
        }
        if (vstate->volatilePrefix == true) {
            blockIsBad = true;
            Verify(0, MVER_E_VOLATILE);
        }
        if (vstate->readonlyPrefix == true) {
            blockIsBad = true;
            Verify(0, MVER_E_BAD_READONLY_PREFIX);
        }
        if (vstate->constrainedPrefix == true) {
            blockIsBad = true;
            Verify(0, MVER_E_BAD_CONSTRAINED_PREFIX);
        }

    }

    gvData->blockIsBad = blockIsBad;
    gvData->thisInitialized = vstate->isThisInitialized();

    // take this node off the worklist, if there is a cycle
    // then it may get added back on later
    if (gvData->isOnWorklist) {
        // remove from worklist
        gvData->isOnWorklist = FALSE;
        if (gvData->worklistPrev)
            gvData->worklistPrev->worklistNext = gvData->worklistNext;
        else
            m_gvWorklistHead = gvData->worklistNext;
        if (gvData->worklistNext)
           gvData->worklistNext->worklistPrev = gvData->worklistPrev;
        else
           m_gvWorklistTail = gvData->worklistPrev;
    }

    if (blockIsBad) return;

    // Global verification must propagate stack across block bounds.
    for (FlowGraphEdgeList * fgEdge = FgNodeGetSuccessorList(block);
         fgEdge;
         fgEdge = FgEdgeListGetNextSuccessor(fgEdge))
    {

        FlowGraphNode * succ = FgEdgeListGetSink(fgEdge);
        PGLOBALVERIFYDATA gvSucc = FgNodeGetGlobalVerifyData(succ);
        ASSERTNR(gvSucc);

        // if we initialized 'this' in the current block then don't propagate
        // the initialization state along nominal edges
        if (FgEdgeListIsNominal(fgEdge) && vstate->thisInitializedThisBlock)
            continue;

        // backwards branch with 'this' uninitialized is illegal

        // track initialization of 'this' pointer in .ctor
        if (gvSucc->thisInitialized == THISUNREACHED)
        {
            // analysis has not reached the block
            gvSucc->thisInitialized = gvData->thisInitialized;
        }
        else
        {
            ASSERTNR(gvData->thisInitialized != THISUNREACHED);


            if (gvSucc->thisInitialized == 1
                && gvData->thisInitialized == 0)
            {
                // we have a mismatch which means the successor must be 
                // re-verified with 'this' set as not yet initialized

                if (!gvSucc->isOnWorklist && !gvSucc->blockIsBad) 
                {
                    gvSucc->isOnWorklist = TRUE;
                    gvSucc->worklistNext = NULL;
                    gvSucc->worklistPrev = m_gvWorklistTail;
                    if (m_gvWorklistTail)
                        m_gvWorklistTail->worklistNext = gvSucc;
                    m_gvWorklistTail = gvSucc;
                    if (!m_gvWorklistHead)
                        m_gvWorklistHead = gvSucc;
                }
            }

            gvSucc->thisInitialized = gvSucc->thisInitialized && gvData->thisInitialized;
        }

        // Must skip nominal edges
        if (FgEdgeListIsNominal(fgEdge))
            continue;

        // the 'successor' block is unreachable but we added a fake edge to 
        // keep it attached to the flowgraph.  Be optimistic about thisInit
        // because the code isn't going to run anyway. 
        if (FgEdgeListIsFake(fgEdge))
        {
            gvSucc->thisInitialized = 1;
        }


        vertype * tiStack;
        int stkDepth = vstate->stackLevel();


        if (gvSucc->stkDepth == -1)
        {   // stack depth previously unknown
            gvSucc->stkDepth = stkDepth;
            if (stkDepth != 0) {
                // Only concerned with cases where there are temps.
                // This is the first time we are reaching this block.
                tiStack = (vertype *)getTempMemory(stkDepth*sizeof(vertype));
                gvSucc->tiStack = tiStack;
                for (int i=0; i<stkDepth; i++) {
                    vertype ti = vstate->impStackTop(i);
                    tiStack[i] = ti;
                }
            } else {
                // common case, no stack-carried temps
                ASSERTNR(gvSucc->tiStack == NULL);
            }
        } else {
            if(stkDepth != gvSucc->stkDepth)
            {
                // This info might be useful but reader class doesn't
                // have access to these macros.  We could add a hook
                // to the client for this if we find this info that useful.
                //dbprint("Confluence MSIL Offset=0x%08X expected %d\n",
                //        FgNodeGetStartMSILOffset(gvSucc->block),
                //        gvSucc->stkDepth);
                //dbprint("This incoming edge MSIL Offset=0x%08X had %d\n",
                //        FgNodeGetEndMSILOffset(gvData->block),
                //        stkDepth);
                verGlobalError(MVER_E_PATH_STACK_DEPTH); //Mismatched stack depth at confluence point
            }

            // handle non-empty stack condition
            if (stkDepth != 0) {
                // augment existing type data at the confluence point
                tiStack = gvSucc->tiStack;
                ASSERTNR(tiStack);
                for (int i=0; i<stkDepth; i++) {
                    vertype ti = vstate->impStackTop(i);

                    // If the new type is not a subtype of the current type,
                    // we must merge to the common parent type and stick
                    // this block on a worklist

                    if (!verCompatibleWith(ti,tiStack[i])) 
                    {
                        if (tiMergeToCommonParent(m_jitInfo,&ti,&(tiStack[i]))) 
                        {
                            tiStack[i] = ti;
                            // potential optimization: only re-verify if the
                            // block actually uses that stack index, else
                            // we can just propagate it w/o reverifying.
                            // But that is an optimization for later.
                            // potential optimization: move to head of list
                            // if all input paths have been taken, else add
                            // at tail position, reducing iteration

                            // if the successor block is bad, it has already been
                            // verified and has other problems.  There is no need
                            // to re-verify after the stack merge because:
                            //    - no code in the block will be run
                            //    - the block no longer has any successors that will be 
                            //      affected by the merge

                            if (!gvSucc->isOnWorklist && !gvSucc->blockIsBad) {
                                // add to worklist at tail position
                                gvSucc->isOnWorklist = TRUE;
                                gvSucc->worklistNext = NULL;
                                gvSucc->worklistPrev = m_gvWorklistTail;
                                if (m_gvWorklistTail)
                                    m_gvWorklistTail->worklistNext = gvSucc;
                                m_gvWorklistTail = gvSucc;
                                if (!m_gvWorklistHead)
                                    m_gvWorklistHead = gvSucc;
                            }
                        } else {
                            verGlobalError(MVER_E_PATH_STACK);//"Mismatched stack types at confluence point\n"
                        }
                    }
                }
            }
        }
    }
    return;
}

CORINFO_CLASS_HANDLE impGetStringClass(ICorJitInfo *jitInfo)
{
    static CORINFO_CLASS_HANDLE s_StringClass;

    if  (s_StringClass == (CORINFO_CLASS_HANDLE) 0)
    {
        s_StringClass = jitInfo->getBuiltinClass(CLASSID_STRING);
        VASSERT(s_StringClass != (CORINFO_CLASS_HANDLE) 0);
    }
    return s_StringClass;
}

// ==========================================================================
// Error Reporting
// ==========================================================================


typedef void (*VerifyErrorHandler) (void* pThis, HRESULT hrError, VerError* pError);

void __stdcall VerifyMethod2(ICorJitInfo* pJitInfo, CORINFO_METHOD_INFO* pMethodInfo, VerifyErrorHandler pErrorHandler, void* pThis)
{

    Jit jit;
    JitContext jc(pJitInfo);
    jc.jitter = &jit;
    jc.jitInfo = pJitInfo;
    jc.methodInfo = pMethodInfo;
    jc.flags = 0;
    pJitInfo->getEEInfo(&(jc.eeInfo));

    GenIR reader(&jc, NULL);

    // no extended error message found
    reader.extended_error_message[0] = 0;

    if(pErrorHandler)
    {
        reader.verSetPeverifyInfo((void *)pErrorHandler, pThis);
    }
    reader.MSILToIR();
}

/*++

Function: __JITfilter

Filter to detect/handle internal JIT exceptions. 
Returns EXCEPTION_EXECUTE_HANDLER for JIT64 exceptions, 
and EXCEPTION_CONTINUE_SEARCH for all others.

--*/

int __JITfilter(int exceptCode, void *exceptInfo)
{
    int facility = (exceptCode >> 16) & 0x0FFF;
//  int code     = exceptCode & 0xFFFF;

#ifdef DEBUG
    if (exceptCode == EXCEPTION_ACCESS_VIOLATION)
    {
        ASSERTNR(!"Access violation while verifying");
    }
#endif

    if (facility == FACILITY_JIT64)
        return  EXCEPTION_EXECUTE_HANDLER;
    else
        return  EXCEPTION_CONTINUE_SEARCH;
}

LONG OFilter(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam)
{
    return __JITfilter(pExceptionPointers->ExceptionRecord->ExceptionCode, pExceptionPointers);
}

// DLL Entrypoint for PEVERIFY

extern "C"
void __stdcall VerifyMethod(ICorJitInfo* pJitInfo, CORINFO_METHOD_INFO* pMethodInfo, VerifyErrorHandler pErrorHandler, void* pThis)
{
    ReaderException      *type;

    PAL_TRY
    {
        VerifyMethod2(pJitInfo, pMethodInfo, pErrorHandler, pThis);
    }
    PAL_EXCEPT_FILTER (OFilter, &type)
    {
        // catch any JIT64 exception code here to prevent this SEH
        // exception from being passed up to mscorwks

        // swallow the exception and we're done with this method
    }
    PAL_ENDTRY
}


void ReaderBase::PrintVerificationErrorMessage(UINT type, HRESULT hrMessage, 
                                               const vertype *expected, 
                                               const vertype *encountered, 
                                               mdToken token,
                                               bool andThrow)
{
    if(!m_peverifyErrorHandler)
        return;

    VerError ve;
    //VerificationException ve;
    VerError *pVE = &ve;
    //VerificationException* pVE = &ve;
    memset(pVE, '\0', sizeof(m_peverifyErrorHandler));


    // Offset
    if(verInstrStartOffset != (DWORD)-1)
    {
        pVE->dwFlags |= VER_ERR_OFFSET;
        pVE->dwOffset = verInstrStartOffset;
    }

    // Encountered type
    WCHAR wszTypeFound[MAX_TYPE_STRING_SIZE];
    if(encountered)
    {
        pVE->dwFlags |= VER_ERR_TYPE_F;
        encountered->ToString(wszTypeFound, MAX_TYPE_STRING_SIZE, m_jitInfo, getCurrentMethodHandle());
        pVE->wszTypeFound = wszTypeFound;
    }

    // Expected type
    WCHAR wszTypeExpected[MAX_TYPE_STRING_SIZE];
    if(expected)
    {
        pVE->dwFlags |= VER_ERR_TYPE_E;
        expected->ToString(wszTypeExpected, MAX_TYPE_STRING_SIZE, m_jitInfo,getCurrentMethodHandle());
        pVE->wszTypeExpected = wszTypeExpected;
    }

    if (type & TokenValid) {
        pVE->token = token;
        pVE->dwFlags |= VER_ERR_TOKEN;
    }

    if (extended_error_message[0])
    {
        VASSERT(!encountered);
        VASSERT(!expected);
        ve.wszAdditionalMessage = extended_error_message;
        pVE->dwFlags |= VER_ERR_ADDL_MSG;
    }

    ((VerifyErrorHandler)m_peverifyErrorHandler)(m_peverifyThis, hrMessage, pVE);
    // clear the error message
    extended_error_message[0] = 0;

    if (andThrow && !m_needsRuntimeCallout)
    {
       VerificationException * re = new VerificationException();
       re->type = type == GlobalError ? READER_GLOBAL_VERIFICATION_EXCEPTION : READER_LOCAL_VERIFICATION_EXCEPTION;
       RaiseException(JIT64_READEREXCEPTION_CODE, 0, 1, (const ULONG_PTR *)&re);
    }

}


void ReaderBase::PrintVerificationErrorMessage(VerErrType type, __in __in_z char *message, 
                                               const vertype *expected, 
                                               const vertype *encountered, 
                                               mdToken token,
                                               bool andThrow)
{
    char *subtype;

    // (Eventually) this will never hit in peverify... 

    {
        switch(type)
        {
        case LocalError:
            subtype = "Local_Type";
            break;
        case GlobalError:
            subtype = "Global_type";
            break;
        default:
            subtype = "unknown_type";
            break;
        }

        fprintf(stderr, "===[VERERR]  AREA='%s'    ", subtype);
        fprintf(stderr, "[offset 0x%08x]", verInstrStartOffset);
        fprintf(stderr, "  [opcode %s]", opcodeName[verInstrOpcode]);
        if (expected)
        {
            fprintf(stderr, "  [expected type:%s]", expected->ToStaticString());
        }
        if (encountered)
        {
            fprintf(stderr, "  [encountered type:%s]", encountered->ToStaticString());
        }
        fprintf(stderr, "  [MESSAGE:'%s']", message);
        fprintf(stderr, "\n");
    }


    m_isVerifiableCode = FALSE;
    if (!m_needsRuntimeCallout)
    { 
       if (type == GlobalError)
       {
          GenIR_Fatal(CORJIT_BADCODE);
       }
       else
       {
          VerificationException * re = new VerificationException();
          re->type = READER_LOCAL_VERIFICATION_EXCEPTION;
          RaiseException(JIT64_READEREXCEPTION_CODE, 0, 1, (const ULONG_PTR *)&re);
       }
    }
}



void ReaderBase::verGlobalError(HRESULT msg)
{
    PrintVerificationErrorMessage(GlobalError, msg, NULL, NULL, mdTokenNil, true);
}
void ReaderBase::verGlobalError(__in __in_z char *message)
{
    PrintVerificationErrorMessage(GlobalError, message, NULL, NULL, mdTokenNil, true);

}

#define THROW_IFNDEF_PEVERIFY false

void ReaderBase::VerifyCompatibleWith(const vertype &encountered, const vertype &expected)
{

    if (!verCompatibleWith(encountered, expected))
    {
        if (expected.IsType(TI_REF) && m_jitInfo->isSDArray(expected.GetClassHandleForObjRef()))
        {
            PrintVerificationErrorMessage(LocalError, MVER_E_STACK_UNEXP_ARRAY, &expected, &encountered, mdTokenNil, THROW_IFNDEF_PEVERIFY);
        }
        else
        {
            PrintVerificationErrorMessage(LocalError, MVER_E_STACK_UNEXPECTED, &expected, &encountered, mdTokenNil, THROW_IFNDEF_PEVERIFY);
        }
    }
}

void ReaderBase::VerifyEqual(const vertype &encountered, const vertype &expected)
{
    if (!(encountered == expected))
    {
        if (expected.IsType(TI_REF) && m_jitInfo->isSDArray(expected.GetClassHandleForObjRef()))
        {
            PrintVerificationErrorMessage(LocalError, MVER_E_STACK_UNEXP_ARRAY, &expected, &encountered, mdTokenNil, THROW_IFNDEF_PEVERIFY);
        }
        else
        {
            PrintVerificationErrorMessage(LocalError, MVER_E_STACK_UNEXPECTED, &expected, &encountered, mdTokenNil, THROW_IFNDEF_PEVERIFY);
        }
    }
}


void ReaderBase::VerifyIsNumberType(const vertype &v)
{
    VerifyAndReportFound(v.IsNumberType(), v, MVER_E_STACK_NUMERIC);
}

void ReaderBase::VerifyIsIntegerType(const vertype &v)
{
    VerifyAndReportFound(v.IsIntegerType(), v, MVER_E_STACK_I_I4_I8);
}

void ReaderBase::VerifyIsObjRef(const vertype &v)
{
    VerifyAndReportFound(v.IsObjRef(), v, MVER_E_STACK_OBJREF);
    Verify(!v.IsUninitialisedObjRef(), MVER_E_STACK_UNINIT);
}

void ReaderBase::VerifyAndReportFound(int cond, const vertype &v, __in __in_z char *message)
{
    if (!cond)
    {
        PrintVerificationErrorMessage(LocalError, message, NULL, &v, mdTokenNil, true);
    }
}

void ReaderBase::VerifyOrReturn(int cond, __in __in_z char *message)
{
    if (!cond)
    {
        PrintVerificationErrorMessage(LocalError, message, NULL, NULL, mdTokenNil, true);
    }
}

void ReaderBase::GVerifyOrReturn(int cond, __in __in_z char *message)
{
    if (!cond)
    {
        ReaderBase::verGlobalError(message);
    }
}

void ReaderBase::VerifyAndReportFound(int cond, const vertype &v, HRESULT message)
{
    if (!cond)
    {
        PrintVerificationErrorMessage(LocalError, message, NULL, &v, mdTokenNil, true);
    }
}

void ReaderBase::VerifyOrReturn(int cond, HRESULT hr)
{
    if (!cond)
    {
        PrintVerificationErrorMessage(LocalError, hr, NULL, NULL, mdTokenNil, true);
    }
}

void ReaderBase::GVerifyOrReturn(int cond, HRESULT message)
{
    if (!cond)
    {
        PrintVerificationErrorMessage(GlobalError, message, NULL, NULL, mdTokenNil, true);
    }
}

void ReaderBase::VerifyFailure(VerificationState *vstate)
{
    if(!m_verificationNeeded) return;
    Verify(false, MVER_E_UNVERIFIABLE);
}

// ==========================================================================
// Setup
// ==========================================================================


// return true if we need to verify this function
bool ReaderBase::verifyNeedsVerification( CorInfoCanSkipVerificationResult * verificationResult)
{
    CorInfoInstantiationVerification instVerInfo;
    m_verIsInstantiationOfFailedGeneric = false;
    
    // we might be currently jiting a method (say list<int>) and have previously
    // verified list<!0> so the runtime will tell us it's ok to skip.
    // another possibility is the following call will trigger a reentrant invocation of the
    // JIT on the generic (non-instantiated aka list<!0>) method, and we will be able to skip.
    instVerInfo = m_jitInfo->isInstantiationOfVerifiedGeneric(getCurrentMethodHandle());

    if (verForceVerification())
        return true;

    if (m_flags & CORJIT_FLG_SKIP_VERIFICATION)
        return false;
    if (m_flags & CORJIT_FLG_IL_STUB)
        return false;

    // If we are only doing verification and not code generation, we do need to
    // verify the method, no matter what
    if (m_flags & CORJIT_FLG_IMPORT_ONLY)
        return true;

    CorInfoCanSkipVerificationResult verResult = m_jitInfo->canSkipMethodVerification(getCurrentMethodHandle(), false); 

    if (verificationResult != NULL) 
    { 
       *verificationResult = verResult;
    }

    if (verResult != CORINFO_VERIFICATION_CANNOT_SKIP)
        return false;



    if (instVerInfo == INSTVER_GENERIC_PASSED_VERIFICATION)
    {
        return false;
    }
    else if (instVerInfo == INSTVER_GENERIC_FAILED_VERIFICATION)
    {
        m_verIsInstantiationOfFailedGeneric = true;
    }

    return true;
}

VerificationState *
ReaderBase::verCreateNewVState(unsigned maxstack, unsigned numLocals, bool fInitLocals, bool fThisInitialized)
{
    VerificationState *vstate;

    vstate = (VerificationState *)   getTempMemory(sizeof(VerificationState));

    vstate->setStack( (vertype *) getTempMemory(sizeof(vertype) * maxstack) );

    vstate->argsInitialized = (bool *) getTempMemory(sizeof(bool) * numLocals);

    vstate->Init(maxstack, numLocals, fInitLocals, fThisInitialized);

    return vstate;

}

VerificationState *
ReaderBase::verifyInitializeBlock(
    FlowGraphNode * block, unsigned int ilOffset
)
{
    if (!m_verificationNeeded)
        return NULL;


    verInstrStartOffset = ilOffset;

    // find the stack-carried verification state
    VerificationState * vstate;
    EHRegion *region = FgNodeGetRegion(block);

    PGLOBALVERIFYDATA gvData = FgNodeGetGlobalVerifyData(block);

    ASSERTNR(gvData);

    vstate = verCreateNewVState(m_methodInfo->maxStack,
                                m_numVerifyAutos,
                                isZeroInitLocals(), gvData->thisInitialized == THISINITED);
    vstate->base = this;
    vstate->delegateMethodRef = mdTokenNil;

    // must verify stack is empty on GenIR_IsRegionStartBlock(block) being true.
    // Note that the exception object is already on the stack, though.
    if (GenIR_IsRegionStartBlock(block))
    {
        // cannot enter a try with an uninitialized 'this'
        if (RgnGetRegionType(region) != ReaderBaseNS::RGN_TRY
            && RgnGetRegionType(region) != ReaderBaseNS::RGN_FAULT)
        GVerifyOrReturn(gvData->thisInitialized == TRUE, MVER_E_THIS_UNINIT_EXCEP);
        // can entry a try/fault 
        if (RgnGetRegionType(region) == ReaderBaseNS::RGN_TRY)
        {
            EHRegionList* rl = RgnGetChildList(region);
            while (rl)
            {
                EHRegion *r = RgnListGetRgn(rl);
                if (RgnGetRegionType(r) != ReaderBaseNS::RGN_FAULT
                    && RgnGetRegionType(r) != ReaderBaseNS::RGN_TRY)
                    GVerifyOrReturn(gvData->thisInitialized == TRUE, MVER_E_THIS_UNINIT_EXCEP);
                rl = RgnListGetNext(rl);
            }
        }

        switch(RgnGetRegionType(region))
        {
        case ReaderBaseNS::RGN_MCATCH:  // typed exception on stack
        case ReaderBaseNS::RGN_FILTER:  // random system.object
        case ReaderBaseNS::RGN_MEXCEPT: // random system.object
            // stack must uninit or 1 (exception object)
            GVerifyOrReturn(gvData->stkDepth == -1
                         || gvData->stkDepth == 1, MVER_E_PATH_STACK_DEPTH);

            // 1st time we've seen, gvdata needs a new stack
            if (gvData->stkDepth == -1)
            {
                VASSERT(gvData->tiStack == NULL);
                gvData->stkDepth = 1;
                gvData->tiStack = (vertype *)getTempMemory(sizeof(vertype));
            }

            if (RgnGetRegionType(region) == ReaderBaseNS::RGN_MCATCH)
            {
                // If it's a catch push the token of the except object
                VerifyPushExceptionObject(vstate, RgnGetCatchClassToken(region));
            }
            else
            {
                // For filter/except we know nothing of the exception object
                //  so just push a generic System.Object
                vstate->push(typeInfo(TI_REF, m_jitInfo->getBuiltinClass(CLASSID_SYSTEM_OBJECT)));
            }
            gvData->tiStack[0] = vstate->impStackTop();

            break;

        default:
            // stack must be uninit or empty
            GVerifyOrReturn(gvData->stkDepth ==-1
                         || gvData->stkDepth == 0, MVER_E_TRY_N_EMPTY_STACK);

            gvData->stkDepth = 0;
            break;

        }
    }
    else
    {
        if (gvData->stkDepth < 0) {
            // ... as might happen with the head block, if initialize at -1
            // then its first visit won't involve predecessors
            gvData->stkDepth = 0;
        }

        if (gvData->stkDepth > 0)
        {
            // ECMA III 1.7.5 Backward Branch Constraints
            // if stack is not empty at beginning of this block,
            // there must exist a predecessor block with lower MSIL offset.
            //
            // JIT64 does not care, but we enforce anyway.
            bool found_OK_predecessor = false;
            for (FlowGraphEdgeList * fgEdge = FgNodeGetPredecessorList(block);
                 fgEdge;
                 fgEdge = FgEdgeListGetNextPredecessor(fgEdge))
            {
                if (FgEdgeListIsNominal(fgEdge))
                    continue;
                FlowGraphNode * pred = FgEdgeListGetSink(fgEdge);
                // note '<=' here due to flowgraph strangeness.
                if (FgNodeGetStartMSILOffset(pred) <= FgNodeGetStartMSILOffset(block))
                    found_OK_predecessor = true;
            }
            // a suitable arc did not exist
            if (!found_OK_predecessor)
            {
                // just report the problem without throwing.
                // This condition poses no problem to us 
                // jit64 does not care and peverify can continue verifying with no worries
                PrintVerificationErrorMessage(LocalError, MVER_E_BACKWARD_BRANCH, NULL, NULL, mdTokenNil, false);
            }
        }
        

        for(int i = (gvData->stkDepth - 1); i >= 0; i--)
            vstate->push(gvData->tiStack[i]);
    }
    
    return vstate;
}

void ReaderBase::VerifyPushExceptionObject(VerificationState *vstate, mdToken tok)
{
    if(!m_verificationNeeded) return;

    // can't use this because we want to throw invalid program exception
    // not a local verification exception
    // VerifyIsClassToken(tok);
    GVerify(TypeFromToken(tok) == mdtTypeDef ||
            TypeFromToken(tok) == mdtTypeRef ||
            TypeFromToken(tok) == mdtTypeSpec,
            MVER_E_TOKEN_TYPE_MEMBER);

    GVerify(m_jitInfo->isValidToken(getCurrentModuleHandle(), tok), MVER_E_TOKEN_TYPE_MEMBER);

    //vertype xcepType = verMakeTypeInfo(getClassHandle(tok));
    // if you are catching something, it's been boxed
    vertype xcepType = vertype(TI_REF, getClassHandle(tok));

    GVerify(!xcepType.IsByRef(), MVER_E_CATCH_BYREF);

    vstate->push(xcepType);
}


void ReaderBase::VerifyToken(mdToken token)
{
    //  To keep us honest: I want to get away from calls to VerifyToken in verification code.
    //  Goal is we always call the verifyIsXXXToken which verifies the token and that it is of
    //  the correct type
    if (!m_jitInfo->isValidToken(getCurrentModuleHandle(), token)) {
        PrintVerificationErrorMessage(LocalError|TokenValid, MVER_E_TOKEN_RESOLVE, 
                                      NULL, NULL, token, true);
    }
}

void ReaderBase::VerifyBinary(VerificationState *vstate, ReaderBaseNS::OPCODE opcode)
{
    if(!m_verificationNeeded) return;

    vertype vt1, vt2;

    VP("Verify Binary\n");

    switch (opcode)
    {
    //logical operations only work on integer
    case ReaderBaseNS::CEE_AND:
    case ReaderBaseNS::CEE_OR:
    case ReaderBaseNS::CEE_XOR:
    // as do unsigned and overflow ops
    case ReaderBaseNS::CEE_DIV_UN:
    case ReaderBaseNS::CEE_MUL_OVF:
    case ReaderBaseNS::CEE_MUL_OVF_UN:
    case ReaderBaseNS::CEE_REM_UN:
    case ReaderBaseNS::CEE_ADD_OVF_UN:
    case ReaderBaseNS::CEE_ADD_OVF:
    case ReaderBaseNS::CEE_SUB_OVF:
    case ReaderBaseNS::CEE_SUB_OVF_UN:
    {
        vt1 = vstate->pop();
        VerifyIsIntegerType(vt1);

        // =
        vt2 = vstate->pop();
        VerifyIsIntegerType(vt2);
        VerifyCompatibleWith(vt2, vt1);

        vstate->push(vt2);

        // -
        // pop, then push same type = NOP
    }
    break;

    case ReaderBaseNS::CEE_ADD:
    case ReaderBaseNS::CEE_DIV:
    case ReaderBaseNS::CEE_MUL:
    case ReaderBaseNS::CEE_REM:
    case ReaderBaseNS::CEE_SUB:
    {
    // N=:-
        vt1 = vstate->pop();

        VerifyIsNumberType(vt1);

        // =
        vt2 = vstate->pop();
        VerifyIsNumberType(vt2);
        VerifyCompatibleWith(vt2, vt1);
        vstate->push(vt2);

        // -
        // pop, then push same type = NOP
    }
    break;

    default:
        VASSERT(UNREACHED);
    }

}

void ReaderBase::VerifyArgList(VerificationState *vstate)
{
    if(!m_verificationNeeded) return;

    CORINFO_CLASS_HANDLE cls;
    vertype tiRetVal;

    VP("Verify arglist\n");

    cls = m_jitInfo->getBuiltinClass(CLASSID_ARGUMENT_HANDLE);
    // arglist in non-vararg method
    // this is fatal for compatibility with x86
    GVerify(m_methodInfo->args.isVarArg(), MVER_E_ARGLIST);

    tiRetVal = vertype(TI_STRUCT, cls);
    vstate->push(tiRetVal);
}


void ReaderBase::VerifyCall(
    VerificationState *  vstate,
    ReaderBaseNS::OPCODE opcode,
    bool                 tailCall,
    bool                 readonlyCall,
    bool                 constraintCall,
    bool                 thisPossiblyModified,
    mdToken              constraintTypeRef,
    mdToken              memberRef
){
    CORINFO_METHOD_HANDLE   methodHnd;
    DWORD                   mflags;
    CORINFO_SIG_INFO        sig;
    unsigned int            popCount = 0;
    CORINFO_CLASS_HANDLE    methodClassHnd;
    unsigned int            argCount  = 0;
    CORINFO_ARG_LIST_HANDLE args = NULL;

    VASSERT(opcode != ReaderBaseNS::CEE_NEWOBJ);

    if(!m_verificationNeeded) return;

    VP("verify call\n");

    Verify(opcode != ReaderBaseNS::CEE_CALLI, MVER_E_UNVERIFIABLE);

    if(opcode == ReaderBaseNS::CEE_CALLVIRT)
    {
        vstate->constrainedPrefix = false;
    }

    VerifyIsCallToken(memberRef);

    //
    methodHnd = getMethodHandle(memberRef);
    verifyMethodHandle(methodHnd);

    mflags = getMethodAttribs(methodHnd);

    // When verifying we always need the exact, never the shared, class of the method
    // (getMethodClass will return a shared class if the method handle is in a shared class)
    methodClassHnd = verGetExactMethodClass(memberRef,methodHnd);
    VASSERT(methodClassHnd);

    eeGetMethodSig(methodHnd, &sig, false, methodClassHnd);

    if (sig.isVarArg())
        eeGetCallSiteSig(memberRef, getCurrentModuleHandle(), getCurrentMethodHandle(), &sig, false);


    unsigned  methodClassFlgs = methodClassHnd ? getClassAttribs(methodClassHnd) : 0;

    // opcode specific checks
    switch(opcode)
    {
    case ReaderBaseNS::CEE_CALLVIRT:
        // cannot do callvirt on valuetypes
        Verify(!verIsValueClass(methodClassHnd), MVER_E_CALLVIRT_VALCLASS);
        Verify(sig.hasThis(), MVER_E_CALLVIRT_STATIC); //"CallVirt on static method"
        break;

    case ReaderBaseNS::CEE_CALL:

    default:
        Verify(!(mflags & CORINFO_FLG_ABSTRACT),  MVER_E_CALL_ABSTRACT); //"method abstract"
    }

    // check compatibility of the arguments
    argCount  = sig.numArgs;
    args = sig.args;
    while (argCount--)
    {
        vertype tiActual = vstate->impStackTop(popCount+argCount);
        vertype tiDeclared = verParseArgSigToTypeInfo(&sig, args).NormaliseForStack();

        VerifyCompatibleWith(tiActual, tiDeclared);

        // check that the argument is not a byref for tailcalls
        if (tailCall)
            VerifyAndReportFound(!verIsByRefLike(tiDeclared), tiDeclared, MVER_E_TAIL_BYREF);

        args = m_jitInfo->getArgNext(args);
    }

    // update popCount
    popCount += sig.numArgs;

    // check for 'this' which are on non-static methods, not called via NEWOBJ
    CORINFO_CLASS_HANDLE instanceClassHnd = getCurrentMethodClass();

    if (!(mflags & CORINFO_FLG_STATIC))
    {
        vertype tiThis = vstate->impStackTop(popCount);
        popCount++;

        if (getClassAttribs(methodClassHnd) & CORINFO_FLG_ARRAY)
            methodClassHnd = m_jitInfo->findMethodClass(getCurrentModuleHandle(), memberRef, getCurrentMethodHandle());

        // If it is null, we assume we can access it (since it will AV shortly)
        // If it is anything but a refernce class, there is no hierarchy, so
        // again, we don't need the precise instance class to compute 'protected' access
        if (tiThis.IsType(TI_REF))
            instanceClassHnd = tiThis.GetClassHandleForObjRef();

        // Check type compatability of the this argument
        vertype tiDeclaredThis = verMakeTypeInfo(methodClassHnd);

        // this enables the following construct:
        // local : native int pbase,
        // ldarga.s   pbase
        // call       instance int32 System.IntPtr::ToInt32()
        if (verIsValueClass(methodClassHnd))
        {
            tiDeclaredThis.MakeByRef();
        }

        // If this is a call to the base class .ctor, set thisPtr Init for
        // this block.
        if (mflags & CORINFO_FLG_CONSTRUCTOR)
        {
            if (m_verTrackObjCtorInitState && tiThis.IsThisPtr()
                && verIsCallToInitThisPtr(getCurrentMethodClass(), methodClassHnd))
            {
                vstate->setThisInitialized();
                vstate->thisInitializedThisBlock = true;
                tiThis.SetInitialisedObjRef();
            }
            else
            {
                // We allow direct calls to value type constructors
                // NB: we have to check that the contents of tiThis is a value type, otherwise we could use a constrained
                // callvirt to illegally re-enter a .ctor on a value of reference type.
                VerifyAndReportFound(tiThis.IsByRef() && DereferenceByRef(tiThis).IsValueClass(), tiThis, MVER_E_CALL_CTOR);
            }
        }

        if (constraintCall) {
            Verify(tiThis.IsByRef(),MVER_E_CONSTRAINED_CALL_WITH_NON_BYREF_THIS); //"non-byref this type in constrained call"

            CORINFO_CLASS_HANDLE constraintClsHnd = verGetClassHandle(constraintTypeRef);
            
            vertype tiConstraint = verMakeTypeInfo(constraintClsHnd);

            //@nice GENERICS: relax this restriction in the spec, to support closure under substitution for .constrained calls
            //"non-variable type operand in constrained instruction"
            //Verify(tiConstraint.IsUnboxedGenericTypeVar(),MVER_E_CONSTRAINED_OF_NON_VARIABLE_TYPE);

            // We just dereference this and test for equality
            tiThis.DereferenceByRef();
            //todo: improve error
            Verify(tiThis == tiConstraint, MVER_E_STACK_UNEXPECTED); //"this type mismatch with constrained type operand"

            // Now pretend the this type is the boxed constrained type, for the sake of subsequent checks
            tiThis = vertype(TI_REF, constraintClsHnd); 
        }

        // To support direct calls on readonly byrefs, just pretend tiDeclaredThis is readonly too
        if(tiDeclaredThis.IsByRef() && tiThis.IsReadonlyByRef())
        {
            tiDeclaredThis.SetIsReadonlyByRef();
        }

        VerifyCompatibleWith(tiThis, tiDeclaredThis);

        if (tiThis.IsByRef())
        {
            // Find the actual type where the method exists (as opposed to what is declared
            // in the metadata). This is to prevent passing a byref as the "this" argument
            // while calling methods like System.ValueType.GetHashCode() which expect boxed objects.
            
            CORINFO_CLASS_HANDLE actualClassHnd = m_jitInfo->getMethodClass(methodHnd);
            Verify(verIsValueClass(actualClassHnd), MVER_E_CALL_TO_VTYPE_BASE);
        }

        // Rules for non-virtual call to a non-final virtual method:
        
        // Define: 
        // The "this" pointer is considered to be "possibly written" if
        //   1. Its address have been taken (LDARGA 0) anywhere in the method.
        //   (or)
        //   2. It has been stored to (STARG.0) anywhere in the method.

        // A non-virtual call to a non-final virtual method is only allowed if
        //   1. The this pointer passed to the callee is an instance of a boxed value type. 
        //   (or)
        //   2. The this pointer passed to the callee is the current method's this pointer.
        //      (and) The current method's this pointer is not "possibly written".

        // Thus the rule is that if you assign to this ANYWHERE you can't make "base" calls to 
        // virtual methods.  (Luckily this does affect .ctors, since they are not virtual).    
        // This is stronger that is strictly needed, but implementing a laxer rule is significantly 
        // hard and more error prone.

        if (opcode == ReaderBaseNS::CEE_CALL 
            && (mflags & CORINFO_FLG_VIRTUAL) 
            && ((mflags & CORINFO_FLG_FINAL) == 0)
            && (!verIsBoxedValueType(tiThis)))
        {
            Verify(tiThis.IsThisPtr() && !thisPossiblyModified, MVER_E_THIS_MISMATCH);
        }

        // also check the specil tailcall rule
        VerifyAndReportFound(!(tailCall && verIsByRefLike(tiDeclaredThis)), tiDeclaredThis, MVER_E_TAIL_BYREF);

        // Tail calls on constrained calls should be illegal too:
        // when instantiated at a value type, a constrained call may pass the address of a stack allocated value 
        Verify(!(tailCall && constraintCall), MVER_E_TAIL_BYREF);//"byref in constrained tailcall"


    }

    // check any constraints on the callee's class and type parameters
    Verify(m_jitInfo->satisfiesClassConstraints(methodClassHnd),
                   MVER_E_UNSATISFIED_METHOD_PARENT_INST); //"method has unsatisfied class constraints"
    Verify(m_jitInfo->satisfiesMethodConstraints(methodClassHnd,methodHnd),
                   MVER_E_UNSATISFIED_METHOD_INST); //"method has unsatisfied method constraints"
    

    // check access permission
    Verify(m_jitInfo->canAccessMethod(getCurrentMethodHandle(),
                                              methodClassHnd,
                                              methodHnd,
                                              instanceClassHnd), MVER_E_METHOD_ACCESS);

    // pop off the args
    while (popCount --) {
        vstate->popPossiblyUninit();
    }

    // special checks for tailcalls
    if (tailCall)
    {
        vertype tiCalleeRetType = verMakeTypeInfo(sig.retType, sig.retTypeClass);
        vertype tiCallerRetType = verMakeTypeInfo(m_methodInfo->args.retType,
                                                m_methodInfo->args.retTypeClass);

        // void return type gets morphed into the error type, so we have to treat them specially here
        if (sig.retType == CORINFO_TYPE_VOID)
            Verify(m_methodInfo->args.retType == CORINFO_TYPE_VOID, MVER_E_TAIL_RET_VOID);
        else
            VerifyCompatibleWith(NormaliseForStack(tiCalleeRetType), NormaliseForStack(tiCallerRetType));

        // for tailcall, stack must be empty
        Verify(vstate->stackLevel() == 0, MVER_E_TAIL_STACK_EMPTY);
    }

    // now push on the result
    if  (sig.retType != CORINFO_TYPE_VOID)
    {
        if (methodClassFlgs & CORINFO_FLG_ARRAY)
        {
            eeGetCallSiteSig(memberRef, getCurrentModuleHandle(), getCurrentMethodHandle(), &sig);
        }

        methodHnd = getMethodHandle(memberRef);
        eeGetMethodSig(methodHnd, &sig, false, methodClassHnd);

        vertype tiRetVal = verMakeTypeInfo(sig.retType, sig.retTypeClass);

        // "readonly." prefixed calls only allowed for the Address operation on arrays.
        // The methods supported by array types are under the control of the EE
        // so we can trust that only the Address operation returns a byref.
        if (readonlyCall)
        {
            VerifyOrReturn ((methodClassFlgs & CORINFO_FLG_ARRAY) && tiRetVal.IsByRef(), 
                            MVER_E_READONLY_UNEXPECTED_CALLEE);//"unexpected use of readonly prefix"
            vstate->readonlyPrefix = false;
            tiRetVal.SetIsReadonlyByRef();
        }

        if (tiRetVal.IsByRef())
        {                
            tiRetVal.SetIsPermanentHomeByRef();

            // If we return byrefs, we will have to verify that we are returning 
            // a good one
            if (m_methodInfo->args.retType == CORINFO_TYPE_BYREF)
            {
                fNeedsByrefReturnCheck = true;
            }                
        }

        tiRetVal.NormaliseForStack();

        vstate->push(tiRetVal);
    }
}

void ReaderBase::VerifyCastClass(VerificationState *vstate, mdToken token)
{
    CORINFO_CLASS_HANDLE clsHnd;
    vertype tiRetVal;

    if(!m_verificationNeeded) return;

    VP("verify castclass\n");

    VerifyIsClassToken(token);

    clsHnd = verGetClassHandle(token, CORINFO_TOKENKIND_Casting);

    VerifyIsObjRef(vstate->pop());
    // Even if this is a value class, we know it is boxed.
    tiRetVal = vertype(TI_REF, clsHnd);

    vstate->push(tiRetVal);
}

void ReaderBase::VerifyCompare(VerificationState *vstate, ReaderBaseNS::OPCODE opcode)
{
    bool compareForEquality = false, withBranch = false;

    if(!m_verificationNeeded) return;

    vertype v1 = vstate->pop();
    vertype v2 = vstate->pop();

    VP("verify compare\n");

    switch (opcode)
    {
    case ReaderBaseNS::CEE_CGT: //                      CG:4)
    case ReaderBaseNS::CEE_CLT: //                      CG:4)
    case ReaderBaseNS::CEE_CLT_UN: //                   CG:4)
        compareForEquality = false;
        withBranch = false;
        break;

    case ReaderBaseNS::CEE_CEQ: //                      CE:4)
    case ReaderBaseNS::CEE_CGT_UN: //                   CE:4)
        //            vstate, equality?, branch?
        compareForEquality = true;
        withBranch = false;
        break;

    case ReaderBaseNS::CEE_BEQ_S: //                    CE:b1)
    case ReaderBaseNS::CEE_BEQ: //                      CE:b4)
    case ReaderBaseNS::CEE_BNE_UN_S: //                 CE:b1)
    case ReaderBaseNS::CEE_BNE_UN: //                   CE:b4)
        //            vstate, equality?, branch?
        compareForEquality = true;
        withBranch = true;
        break;

    case ReaderBaseNS::CEE_BGE_S: //                    CG:b1)
    case ReaderBaseNS::CEE_BGT_S: //                    CG:b1)
    case ReaderBaseNS::CEE_BLE_S: //                    CG:b1)
    case ReaderBaseNS::CEE_BLT_S: //                    CG:b1)
    case ReaderBaseNS::CEE_BGE_UN_S: //                 CG:b1)
    case ReaderBaseNS::CEE_BGT_UN_S: //                 CG:b1)
    case ReaderBaseNS::CEE_BLE_UN_S: //                 CG:b1)
    case ReaderBaseNS::CEE_BLT_UN_S: //                 CG:b1)
    case ReaderBaseNS::CEE_BGE: //                      CG:b4)
    case ReaderBaseNS::CEE_BGT: //                      CG:b4)
    case ReaderBaseNS::CEE_BLE: //                      CG:b4)
    case ReaderBaseNS::CEE_BLT: //                      CG:b4)
    case ReaderBaseNS::CEE_BGE_UN: //                   CG:b4)
    case ReaderBaseNS::CEE_BGT_UN: //                   CG:b4)
    case ReaderBaseNS::CEE_BLE_UN: //                   CG:b4)
    case ReaderBaseNS::CEE_BLT_UN: //                   CG:b4)
        //            vstate, equality?, branch?
        compareForEquality = false;
        withBranch = true;
        break;

    default:
        VASSERT(UNREACHED);
    }

    // Convert &System/Int32 to &I4, &System/Char to &I2 etc.
    v1.NormaliseForStack();
    v2.NormaliseForStack();

    if (v1.IsNumberType())
    {
        // want a better error message
        // Verify(v1 == v2, MVER_E_STACK_NOT_EQ); 

        if (!(v1 == v2))
        {
            PrintVerificationErrorMessage(LocalError, 
                                          MVER_E_STACK_NOT_EQ, &v1, &v2, mdTokenNil, 
                                          THROW_IFNDEF_PEVERIFY);
        }
    }
    else if (v1.IsObjRef())
    {
        //must be compare for equality
        Verify(compareForEquality, MVER_E_STACK_UNEXPECTED); //"Cond not allowed on object types"

        VerifyAndReportFound(v2.IsObjRef(), v2, MVER_E_STACK_NOT_EQ);
    }
    else if (v1.IsByRef())
    {
        VerifyAndReportFound(v2.IsByRef(), v2, MVER_E_STACK_NOT_EQ);
    }
    else {
        VerifyAndReportFound(v1.IsMethod(), v1, MVER_E_STACK_NOT_EQ);
        VerifyAndReportFound(v2.IsMethod(), v2, MVER_E_STACK_NOT_EQ);
    }

    if (!withBranch)
        vstate->push(vertype(TI_INT));
}

void ReaderBase::VerifyConvert(VerificationState *vstate, ReaderBaseNS::ConvOpcode opcode)
{
    CorInfoType lclTyp = CORINFO_TYPE_UNDEF;

    if(!m_verificationNeeded) return;

    VP("verify convert\n");

    switch(opcode)
    {
    case ReaderBaseNS::CONV_OVF_I1:      lclTyp = CORINFO_TYPE_BYTE ;     break;
    case ReaderBaseNS::CONV_OVF_I2:      lclTyp = CORINFO_TYPE_SHORT;     break;
    case ReaderBaseNS::CONV_OVF_I :      lclTyp = CORINFO_TYPE_NATIVEINT; break;
    case ReaderBaseNS::CONV_OVF_I4:      lclTyp = CORINFO_TYPE_INT  ;     break;
    case ReaderBaseNS::CONV_OVF_I8:      lclTyp = CORINFO_TYPE_LONG ;     break;

    case ReaderBaseNS::CONV_OVF_U1:      lclTyp = CORINFO_TYPE_UBYTE;     break;
    case ReaderBaseNS::CONV_OVF_U2:      lclTyp = CORINFO_TYPE_CHAR ;     break;
    case ReaderBaseNS::CONV_OVF_U :      lclTyp = CORINFO_TYPE_NATIVEUINT;break;
    case ReaderBaseNS::CONV_OVF_U4:      lclTyp = CORINFO_TYPE_UINT ;     break;
    case ReaderBaseNS::CONV_OVF_U8:      lclTyp = CORINFO_TYPE_ULONG;     break;

    case ReaderBaseNS::CONV_OVF_I1_UN:   lclTyp = CORINFO_TYPE_BYTE ;     break;
    case ReaderBaseNS::CONV_OVF_I2_UN:   lclTyp = CORINFO_TYPE_SHORT;     break;
    case ReaderBaseNS::CONV_OVF_I_UN :   lclTyp = CORINFO_TYPE_NATIVEINT; break;
    case ReaderBaseNS::CONV_OVF_I4_UN:   lclTyp = CORINFO_TYPE_INT  ;     break;
    case ReaderBaseNS::CONV_OVF_I8_UN:   lclTyp = CORINFO_TYPE_LONG ;     break;

    case ReaderBaseNS::CONV_OVF_U1_UN:   lclTyp = CORINFO_TYPE_UBYTE;     break;
    case ReaderBaseNS::CONV_OVF_U2_UN:   lclTyp = CORINFO_TYPE_CHAR ;     break;
    case ReaderBaseNS::CONV_OVF_U_UN :   lclTyp = CORINFO_TYPE_NATIVEUINT;break;
    case ReaderBaseNS::CONV_OVF_U4_UN:   lclTyp = CORINFO_TYPE_UINT ;     break;
    case ReaderBaseNS::CONV_OVF_U8_UN:   lclTyp = CORINFO_TYPE_ULONG;     break;

    case ReaderBaseNS::CONV_I1:       lclTyp = CORINFO_TYPE_BYTE  ;    break;
    case ReaderBaseNS::CONV_I2:       lclTyp = CORINFO_TYPE_SHORT ;    break;
    case ReaderBaseNS::CONV_I:        lclTyp = CORINFO_TYPE_NATIVEINT; break;
    case ReaderBaseNS::CONV_I4:       lclTyp = CORINFO_TYPE_INT   ;    break;
    case ReaderBaseNS::CONV_I8:       lclTyp = CORINFO_TYPE_LONG  ;    break;

    case ReaderBaseNS::CONV_U1:       lclTyp = CORINFO_TYPE_UBYTE ;    break;
    case ReaderBaseNS::CONV_U2:       lclTyp = CORINFO_TYPE_CHAR  ;    break;
    case ReaderBaseNS::CONV_U:        lclTyp = CORINFO_TYPE_NATIVEUINT;break;
    case ReaderBaseNS::CONV_U4:       lclTyp = CORINFO_TYPE_UINT  ;    break;
    case ReaderBaseNS::CONV_U8:       lclTyp = CORINFO_TYPE_ULONG ;    break;

    case ReaderBaseNS::CONV_R4:       lclTyp = CORINFO_TYPE_FLOAT;     break;
    case ReaderBaseNS::CONV_R8:       lclTyp = CORINFO_TYPE_DOUBLE;    break;

    case ReaderBaseNS::CONV_R_UN :    lclTyp = CORINFO_TYPE_DOUBLE;    break;
    default:
        VASSERT(UNREACHED);
    }

    const vertype& tiVal = vstate->pop();
    vertype tiRetVal;
    VerifyIsNumberType(tiVal);
    tiRetVal = vertype(lclTyp).NormaliseForStack();
    vstate->push(tiRetVal);
}

void ReaderBase::VerifyIsInst(VerificationState *vstate, mdToken typeRef)
{
    vertype tiRetVal;
    vertype tiActualType;

    if (!m_verificationNeeded)
        return;

    VP("verify isinst\n");

    VerifyIsClassToken(typeRef);

    // Get the type token
    CORINFO_CLASS_HANDLE clsHnd = verifyClass(verGetClassHandle(typeRef,
                                                                CORINFO_TOKENKIND_Casting));

    vertype tiVal = vstate->pop();
    VerifyIsObjRef(tiVal);
    // input of ininst must be boxed.
    // Output must be as well, so force it to be a ref, not the type of
    // whatever clsHnd really is (value class, primitive, etc)
    tiRetVal = vertype(TI_REF, clsHnd);

    // It turns out the J# uses isisnt System.Void.
    // I have no idea what they think this accomplishes


    Verify(!tiRetVal.IsDead(), MVER_E_TOKEN_TYPE );

    vstate->push(tiRetVal);
}

void ReaderBase::VerifyDup(VerificationState *vstate, const BYTE *codeAddr)
{
    if(!m_verificationNeeded){
        return;
    }

    VP("verify dup\n");

    // this could be the beginning of a delegate create
    vstate->delegateCreateStart = codeAddr;

    // valid to dup an uninit thisptr
    vstate->push(vstate->impStackTop(0));
}

void ReaderBase::VerifyEndFilter(VerificationState *vstate, DWORD msilOffset)
{
    if(!m_verificationNeeded){
        return;
    }

    VP("verify endfilter\n");

    if (!m_currentRegion || ReaderBaseNS::RGN_FILTER != RgnGetRegionType(m_currentRegion)) {
        BADCODE(MVER_E_ENDFILTER); //"endfilter instuction can only appear inside of a filter"
    }

    if (RgnGetEndMSILOffset(m_currentRegion) != msilOffset) {
        // endfilter instuction can only appear at the end of a filter region
        BADCODE(MVER_E_ENDFILTER);
    }

    // Ensure stack contains one item and that item is an int.
    Verify(vstate->stackLevel() == 1, MVER_E_ENDFILTER_STACK);
    // fiter return value must be an integer
    VerifyAndReportFound(vstate->impStackTop(0).IsType(TI_INT), vstate->impStackTop(0), MVER_E_STACK_I_I4);
}

void ReaderBase::VerifyEndFinally(VerificationState *vstate)
{
    if(!m_verificationNeeded){
        return;
    }

    VP("verify endfinally\n");

    // Nothing to verify
}

void ReaderBase::VerifyInitObj(VerificationState *vstate, mdToken typeRef)
{
    CORINFO_CLASS_HANDLE clsHnd;

    if(!m_verificationNeeded){
        return;
    }

    VP("verify initobj\n");

    VerifyIsClassToken(typeRef);

    clsHnd = verGetClassHandle(typeRef);

    vertype tiTo = vstate->pop();

    VerifyIsByref(tiTo); //this test should be added to Importer.cpp

    Verify(!tiTo.IsReadonlyByRef(), MVER_E_READONLY_ILLEGAL_WRITE);//"write to readonly byref"

    tiTo.DereferenceByRef();

    vertype tiInstr = verMakeTypeInfo(clsHnd);

    VerifyEqual(tiInstr, tiTo);

}

void ReaderBase::VerifyCpObj(VerificationState *vstate, mdToken typeRef)
{
    CORINFO_CLASS_HANDLE clsHnd;

    if(!m_verificationNeeded){
        return;
    }

    VP("verify cpobj\n");

    VerifyIsClassToken(typeRef);

    clsHnd = verGetClassHandle(typeRef);

    vertype tiFrom  = vstate->pop();
    vertype tiTo    = vstate->pop();
    vertype tiInstr = verMakeTypeInfo(clsHnd);

    //todo: add these checks to Importer.cpp
    VerifyIsByref(tiFrom); 
    VerifyIsByref(tiTo);

    Verify(!tiTo.IsReadonlyByRef(), MVER_E_READONLY_ILLEGAL_WRITE);//"write to readonly byref"

    tiFrom.DereferenceByRef();
    tiTo.DereferenceByRef();

    VerifyCompatibleWith(tiFrom, tiInstr);
    VerifyCompatibleWith(tiInstr, tiTo);

}

void ReaderBase::VerifyLdarg(VerificationState *vstate, unsigned num, ReaderBaseNS::OPCODE opcode)
{
    vertype x;

    if(!m_verificationNeeded){
        return;
    }

    VP("verify loadarg\n");

    bool normalizeType;

    // this function is called for both ldarg and ldarga
    // for ldarga you don't want to normalize the value pushed on the stack because
    // verifyloadaddr will create a type which is a byref to TOS and &int8 != &int32

    if (opcode == ReaderBaseNS::CEE_LDARGA || opcode == ReaderBaseNS::CEE_LDARGA_S)
    {
        // fatal on ldarga to match JIT32
        normalizeType = false;
        GVerify(num < m_numVerifyParams, MVER_E_ARG_NUM); //"arg out of range"
    }
    else
    {
        normalizeType = true;
        Verify(num < m_numVerifyParams, MVER_E_ARG_NUM); //"arg out of range"
    }

    x = m_paramVerifyMap[num];

    VerifyIsNotUnmanaged(x);
    VerifyTypeIsValid(x);

    if (normalizeType)
        x.NormaliseForStack();

    if (m_verTrackObjCtorInitState && !vstate->isThisInitialized() && x.IsThisPtr())
        x.SetUninitialisedObjRef();

    vstate->push(x);
}

void ReaderBase::VerifyLoadAddr(VerificationState *vstate)
{
    vertype tiRetVal;

    if(!m_verificationNeeded){
        return;
    }

    VP("verify loadaddr\n");

    tiRetVal = vstate->pop();

    if (m_verTrackObjCtorInitState && !vstate->isThisInitialized() )
        Verify(!tiRetVal.IsThisPtr(), MVER_E_THIS_UNINIT_STORE);

    Verify(!tiRetVal.IsByRef(), MVER_E_SIG_BYREF_BYREF); //"byref to byref"

    tiRetVal.MakeByRef();
    
    vstate->push(tiRetVal);
}

void ReaderBase::VerifyLoadConstant(VerificationState *vstate, ReaderBaseNS::OPCODE opcode)
{
    if(!m_verificationNeeded){
        return;
    }

    VP("verify loadconstant\n");

    switch(opcode)
    {
    case ReaderBaseNS::CEE_LDC_I4_M1:
    case ReaderBaseNS::CEE_LDC_I4_0:
    case ReaderBaseNS::CEE_LDC_I4_1:
    case ReaderBaseNS::CEE_LDC_I4_2:
    case ReaderBaseNS::CEE_LDC_I4_3:
    case ReaderBaseNS::CEE_LDC_I4_4:
    case ReaderBaseNS::CEE_LDC_I4_5:
    case ReaderBaseNS::CEE_LDC_I4_6:
    case ReaderBaseNS::CEE_LDC_I4_7:
    case ReaderBaseNS::CEE_LDC_I4_8:
    case ReaderBaseNS::CEE_LDC_I4_S:
    case ReaderBaseNS::CEE_LDC_I4:
        vstate->push(vertype(TI_INT));
        break;
    case ReaderBaseNS::CEE_LDC_I8:
        vstate->push(vertype(TI_LONG));
        break;
    case ReaderBaseNS::CEE_LDC_R4:
        vstate->push(vertype(TI_DOUBLE));
        break;
    case ReaderBaseNS::CEE_LDC_R8:
        vstate->push(vertype(TI_DOUBLE));
        break;
    default:
        VASSERT(0);
    }
}


void ReaderBase::VerifyLoadElem(VerificationState *vstate, ReaderBaseNS::OPCODE opcode, mdToken token)
{
    ti_types lclTyp = TI_ERROR;
    vertype tiRetVal;
    vertype tiArray, tiIndex;

    if (!m_verificationNeeded) {
        return;
    }

    VP("verify loadelem\n");

    tiIndex = vstate->pop();
    tiArray = vstate->pop();

    VerifyIsSDArray(tiArray); //todo: add this check to LDELEM case in Importer.cpp

    VerifyAndReportFound(tiIndex.IsType(TI_INT) || tiIndex.IsType(TI_I), tiIndex, MVER_E_STACK_UNEXPECTED);

    switch (opcode) {
    case ReaderBaseNS::CEE_LDELEM_I1 : lclTyp = TI_BYTE   ; break;
    case ReaderBaseNS::CEE_LDELEM_I2 : lclTyp = TI_SHORT  ; break;
    case ReaderBaseNS::CEE_LDELEM_I  : lclTyp = TI_I      ; break;
    case ReaderBaseNS::CEE_LDELEM_U4 : lclTyp = TI_INT    ; break;
    case ReaderBaseNS::CEE_LDELEM_I4 : lclTyp = TI_INT    ; break;
    case ReaderBaseNS::CEE_LDELEM_I8 : lclTyp = TI_LONG   ; break;
    case ReaderBaseNS::CEE_LDELEM_REF: lclTyp = TI_REF    ; break;
    case ReaderBaseNS::CEE_LDELEM_R4 : lclTyp = TI_FLOAT  ; break;
    case ReaderBaseNS::CEE_LDELEM_R8 : lclTyp = TI_DOUBLE ; break;
    case ReaderBaseNS::CEE_LDELEM_U1 : lclTyp = TI_BYTE   ; break;
    case ReaderBaseNS::CEE_LDELEM_U2 : lclTyp = TI_SHORT  ; break;
    // the only case where the type is from a token is CEE_LDELEM
    case ReaderBaseNS::CEE_LDELEM    : // Generics
    {
        VerifyIsClassToken(token);

        tiRetVal = verMakeTypeInfo(verGetClassHandle(token));

        if (!tiArray.IsNullObjRef())
        {
            vertype tiArrayElemType = verGetArrayElemType(tiArray);
            VerifyCompatibleWith(tiArrayElemType,tiRetVal);
        }

        tiRetVal.NormaliseForStack();
        vstate->push(tiRetVal);
        return;
    }
    break;

    default:
        Verify(0, "todo\n");
    }

    if (tiArray.IsNullObjRef())
    {
        if (lclTyp == TI_REF)  // we will say a deref of a null array yields a null ref
            tiRetVal = vertype(TI_NULL);
        else
            tiRetVal = vertype(lclTyp);
    }
    else
    {
        tiRetVal = verGetArrayElemType(tiArray);
        VerifyAndReportFound(tiRetVal.IsType(lclTyp), tiRetVal, MVER_E_STACK_UNEXPECTED);
    }

    tiRetVal.NormaliseForStack();
    vstate->push(tiRetVal);
}

void ReaderBase::VerifyLoadElemA(VerificationState *vstate, bool readOnlyPrefix, mdToken typeRef)
{
    vertype tiRetVal;

    if (!m_verificationNeeded) {
        return;
    }

    VP("verify loadelema\n");

    vstate->readonlyPrefix = false;

    vertype tiIndex = vstate->pop();
    vertype tiArray = vstate->pop();

    VerifyIsSDArray(tiArray);

    VerifyAndReportFound(tiIndex.IsType(TI_INT) || tiIndex.IsType(TI_I), tiIndex, MVER_E_STACK_UNEXPECTED);

    VerifyIsClassToken(typeRef);

    tiRetVal = verMakeTypeInfo(verGetClassHandle(typeRef));

    if (!tiArray.IsNullObjRef())
    {
        vertype tiArrayType = verGetArrayElemType(tiArray);

        VerifyEqual(tiArrayType, tiRetVal);

        // must be exact (i4, i)
        VerifyAndReportFound(
            tiArrayType.GetType() == tiRetVal.GetType(), tiArrayType, MVER_E_STACK_UNEXP_ARRAY);
    }

    // runtime should not allow
    Verify(!tiRetVal.IsByRef(), MVER_E_SIG_BYREF_BYREF); 

    tiRetVal.MakeByRef();

    if (readOnlyPrefix)
    {
        tiRetVal.SetIsReadonlyByRef();
    }
    
    // an array interior pointer is always in the heap
    tiRetVal.SetIsPermanentHomeByRef();

    vstate->push(tiRetVal);
}

void ReaderBase::VerifyLoadLen(VerificationState *vstate)
{
    if(!m_verificationNeeded){
        return;
    }

    VP("verify loadlen\n");

    vertype tiArray = vstate->pop();
    VerifyIsSDArray(tiArray);

    /* Push the result back on the stack */
    vstate->push(vertype(TI_I));
}

void ReaderBase::VerifyLdloc(VerificationState *vstate, unsigned num, ReaderBaseNS::OPCODE opcode)
{
    vertype x;

    if(!m_verificationNeeded){
        return;
    }

    VP("verify loadlocal\n");

    bool normalize;

    // this function is called for both ldloc and ldloca
    // for ldloca you don't want to normalize the value pushed on the stack because
    // verifyloadaddr will create a type which is a byref to TOS and &int8 != &int32
    if (opcode == ReaderBaseNS::CEE_LDLOCA || opcode == ReaderBaseNS::CEE_LDLOCA_S)
    {
        // fatal on ldloca to match JIT32
        normalize = false;
        GVerify(num < m_numVerifyAutos, MVER_E_LOC_NUM); //"local out of range"
    }
    else
    {
        normalize = true;
        Verify(num < m_numVerifyAutos, MVER_E_LOC_NUM); //"local out of range"
    }
    

    Verify(isZeroInitLocals(), MVER_E_INITLOCALS); // locals must be initialized

    x = m_autoVerifyMap[num];

    VerifyIsNotUnmanaged(x);
    VerifyTypeIsValid(x);

    if(normalize)
        x.NormaliseForStack();

    vstate->push(x);
}

void ReaderBase::VerifyLoadIndirect(VerificationState *vstate, ReaderBaseNS::LdIndirOpcode opcode)
{
    ti_types instrType = TI_ERROR;
    vertype result;

    if (!m_verificationNeeded) {
        return;
    }

    vstate->unalignedPrefix = false;
    vstate->volatilePrefix  = false;

    VP("verify loadindirect\n");

    switch (opcode) {
    case ReaderBaseNS::LDIND_I1:      instrType  = TI_BYTE;     break;
    case ReaderBaseNS::LDIND_I2:      instrType  = TI_SHORT;    break;
    case ReaderBaseNS::LDIND_U4:
    case ReaderBaseNS::LDIND_I4:      instrType  = TI_INT;      break;
    case ReaderBaseNS::LDIND_I8:      instrType  = TI_LONG;     break;
    case ReaderBaseNS::LDIND_REF:     instrType  = TI_REF;      break;
    case ReaderBaseNS::LDIND_I:       instrType  = TI_I;        break;
    case ReaderBaseNS::LDIND_R4:      instrType  = TI_FLOAT;    break;
    case ReaderBaseNS::LDIND_R8:      instrType  = TI_DOUBLE;   break;
    case ReaderBaseNS::LDIND_U1:      instrType  = TI_BYTE;     break;
    case ReaderBaseNS::LDIND_U2:      instrType  = TI_SHORT;    break;
    default:
        VASSERT(UNREACHED);
    }
    result = verVerifyLDIND(vstate->pop(), instrType);
    result.NormaliseForStack();
    vstate->push(result);
}


void ReaderBase::VerifyLoadNull(VerificationState* vstate){
    if (!m_verificationNeeded) {
        return;
    }

    VP("verify loadnull\n");

    vstate->push(TI_NULL);
}

void ReaderBase::VerifyLoadObj(VerificationState *vstate, mdToken typeRef)
{
    CORINFO_CLASS_HANDLE clsHnd;
    vertype tiRetVal;

    if(!m_verificationNeeded) return;

    vstate->unalignedPrefix = false;
    vstate->volatilePrefix  = false;

    VP("verify loadobj\n");

    VerifyIsClassToken(typeRef);

    clsHnd = verGetClassHandle(typeRef);

    tiRetVal = verMakeTypeInfo(clsHnd);

    vertype tiPtr = vstate->pop();

    VerifyAndReportFound(tiPtr.IsByRef(), tiPtr, MVER_E_STACK_BYREF);    
    typeInfo tiPtrVal = DereferenceByRef(tiPtr);
    
    VerifyCompatibleWith(tiPtrVal, tiRetVal);
    tiRetVal.NormaliseForStack();
    vstate->push(tiRetVal);
}

void ReaderBase::VerifyLoadStr(VerificationState *vstate, mdToken token)
{
    if (!m_verificationNeeded) return;

    VP("verify loadstr\n");

    VerifyToken(token);

    Verify(m_jitInfo->isValidStringRef(getCurrentModuleHandle(), token), MVER_E_LDSTR_OPERAND);
    vstate->push(vertype(TI_REF, impGetStringClass(m_jitInfo)));
}



HRESULT
verTranslateErrorHR(HRESULT error)
{
    switch (error)
    {
    case COR_E_MEMBERACCESS:
    case COR_E_METHODACCESS:  
    case COR_E_MISSINGMEMBER:
    case COR_E_MISSINGMETHOD:
        return MVER_E_METHOD_ACCESS;
    case COR_E_MISSINGFIELD:
    case COR_E_FIELDACCESS:
        return MVER_E_FIELD_ACCESS;
    case COR_E_BADIMAGEFORMAT:
    default:
        return error;

    }
}


/*
The ldtoken instruction pushes a RuntimeHandle for the specified metadata token. The token must be one of:
A methoddef or methodref : pushes a RuntimeMethodHandle
A typedef or typeref : pushes a RuntimeTypeHandle
A fielddef or fieldref : pushes a RuntimeFieldHandle
The value pushed on the stack can be used in calls to Reflection methods in the system class library

Verifiability:
Correct CIL requires that token describes a valid metadata token.
*/

void ReaderBase::VerifyLoadToken(VerificationState *vstate, mdToken token)
{
    CORINFO_CLASS_HANDLE tokenType;
    vertype tiRetVal;

    if (!m_verificationNeeded) return;

    VP("verify loadtoken\n");

    VerifyToken(token);

    tokenType = m_jitInfo->getTokenTypeAsHandle(getCurrentModuleHandle(), token);


    // if we're in peverify then we should touch this token to see if it is accessible
    // If in the Jit, we will do this later
    PAL_TRY
    {
        CORINFO_GENERICHANDLE_RESULT result;
        m_jitInfo->embedGenericHandle(getCurrentModuleHandle(),  
                                      token | CORINFO_ANNOT_PERMITUNINSTDEFORREF,
                                      getCurrentMethodHandle(), 
                                      CORINFO_TOKENKIND_Ldtoken,
                                      &result);
    }
    PAL_EXCEPT_FILTER(ReaderBase::FieldFilter, m_jitInfo)
    {
        GetErrorMessage(this, m_jitInfo);
        PrintVerificationErrorMessage(LocalError|TokenValid, verTranslateErrorHR(verLastError), 
                                      NULL, NULL, token, true);
    }
    PAL_ENDTRY




    tiRetVal = verMakeTypeInfo(tokenType);

    vstate->push(tiRetVal);
}


void ReaderBase::VerifyUnary(VerificationState *vstate, ReaderBaseNS::UnaryOpcode opcode)
{
    vertype tiRetVal;

    if (!m_verificationNeeded) return;

    VP("verify unary\n");

    tiRetVal = vstate->pop();
    switch(opcode){
    case ReaderBaseNS::NEG:
        VerifyIsNumberType(tiRetVal);
        break;
    case ReaderBaseNS::NOT:
        VerifyIsIntegerType(tiRetVal);
        break;
    default:
        VASSERT(UNREACHED);
        break;
    }
    vstate->push(tiRetVal);
}

void ReaderBase::VerifyLoadFtn(VerificationState *vstate, ReaderBaseNS::OPCODE opcode, mdToken memberRef, const BYTE *codeAddr)
{
    CORINFO_METHOD_HANDLE methHnd;
    CORINFO_CLASS_HANDLE  methodClassHnd;
    CORINFO_SIG_INFO      sig;
    DWORD                 mflags;
    CORINFO_CLASS_HANDLE  instanceClassHnd = getCurrentMethodClass();

    if (!m_verificationNeeded) return;

    VP("verify loadftn\n");

    VerifyIsDirectCallToken(memberRef);

    vstate->delegateMethodRef = memberRef;

    methHnd = verifyMethodHandle(getMethodHandle(memberRef));

    methodClassHnd = verGetExactMethodClass(memberRef, methHnd);

    m_jitInfo->getMethodSig(methHnd, &sig);

    if (sig.hasTypeArg())
        NO_WAY("Currently do not support LDFTN of Parameterized functions");

    mflags = getMethodAttribs(methHnd);

    Verify(!(mflags & CORINFO_FLG_CONSTRUCTOR), MVER_E_LDFTN_CTOR);

    if (opcode == ReaderBaseNS::CEE_LDFTN)
    {
        vstate->delegateCreateStart = codeAddr;

        if (sig.hasTypeArg())
            NO_WAY("Currently do not support LDFTN of Parameterized functions");


        Verify(m_jitInfo->satisfiesClassConstraints(methodClassHnd),
                       MVER_E_UNSATISFIED_METHOD_PARENT_INST); //"method has unsatisfied class constraints"
        Verify(m_jitInfo->satisfiesMethodConstraints(methodClassHnd,methHnd),
                       MVER_E_UNSATISFIED_METHOD_INST); //"method has unsatisfied method constraints"

        Verify(m_jitInfo->canAccessMethod(getCurrentMethodHandle(), //from
                                          methodClassHnd, // in
                                          methHnd, // what
                                          instanceClassHnd),
               MVER_E_METHOD_ACCESS);
    }
    else //ldvirtftn
    {
        vertype declType;

        VASSERT((mflags & CORINFO_FLG_SECURITYCHECK) == 0);

        Verify(sig.hasThis(), MVER_E_LDVIRTFTN_STATIC);

        // Get the object-ref

        // decltype might not be an objref -- someone could have called
        // ldvirtftn with a value class token.  Value classes don't have virtual functions
        // so this feature is dubious at best.
        if (verIsValueClass(methodClassHnd))
        {
            // cons up a boxed valueclass
            declType = vertype(TI_REF, methodClassHnd);
        }
        else
        {
            // just go through the normal path
            declType = verMakeTypeInfo(methodClassHnd);
        }
        vertype arg = vstate->pop();
        
        // we can box ldvirtftn a method on a value class

        VerifyIsObjRef(arg);
        VerifyCompatibleWith(arg, declType);

        if (!(arg.IsType(TI_NULL) || (mflags & CORINFO_FLG_STATIC)))
            instanceClassHnd = arg.GetClassHandleForObjRef();

        // check any constraints on the callee's class and type parameters
        Verify(m_jitInfo->satisfiesClassConstraints(methodClassHnd),
                       MVER_E_UNSATISFIED_METHOD_PARENT_INST); //"method has unsatisfied class constraints"
        Verify(m_jitInfo->satisfiesMethodConstraints(methodClassHnd,methHnd),
                       MVER_E_UNSATISFIED_METHOD_INST); //"method has unsatisfied method constraints"


        Verify(m_jitInfo->canAccessMethod(getCurrentMethodHandle(), //from
                                          methodClassHnd,
                                          methHnd,
                                          instanceClassHnd),
               MVER_E_METHOD_ACCESS);
    }
    vstate->push(vertype(methHnd));
}

void ReaderBase::VerifyNewObj(
    VerificationState*   vstate,
    ReaderBaseNS::OPCODE opcode,
    bool                 tailCall,
    mdToken              memberRef,
    const BYTE           *codeAddr
)
{
    CORINFO_METHOD_HANDLE methHnd;
    CORINFO_CLASS_HANDLE  clsHnd;
    CORINFO_SIG_INFO sig;

    DWORD mflags, clsFlags;
    mdToken typeRef;
    unsigned int count;

    VASSERT(opcode == ReaderBaseNS::CEE_NEWOBJ);

    if (!m_verificationNeeded) return;

    VP("verify newobj\n");

    // tail.newobj not allowed
    Verify(!tailCall, MVER_E_TAIL_CALL);

    VerifyIsDirectCallToken(memberRef);

    methHnd = getMethodHandle(memberRef);
    m_jitInfo->getMethodSig(methHnd, &sig);

    if (sig.isVarArg())
        eeGetCallSiteSig(memberRef, getCurrentModuleHandle(), getCurrentMethodHandle(), &sig, true);

    mflags = m_jitInfo->getMethodAttribs(methHnd, getCurrentMethodHandle());

    if ((mflags & (CORINFO_FLG_STATIC|CORINFO_FLG_ABSTRACT)) != 0)
        Verify(0, MVER_E_CTOR); //"newobj on static or abstract method"

    typeRef = m_jitInfo->getMemberParent(getCurrentModuleHandle(), memberRef);

    VerifyIsClassToken(typeRef);
    clsHnd = verGetClassHandle(typeRef);

    // There are three different cases for new
    // Object size is variable (depends on arguments)
    //      1) Object is an array (arrays treated specially by the EE)
    //      2) Object is some other variable sized object (e.g. String)
    //      3) Class Size can be determined beforehand (normal case)
    // In the first case, we need to call a NEWOBJ helper (multinewarray)
    // in the second case we call the constructor with a '0' this pointer
    // In the third case we alloc the memory, then call the constuctor

    clsFlags = getClassAttribs(clsHnd);

    // if we are in the JIT, the runtime will throw an illegal operation exception
    // when the ctor is actually called (dynamically)
    // in peverify we have to check
    if (clsFlags & CORINFO_FLG_ABSTRACT)
        Verify(0, MVER_E_NEWOBJ_OF_ABSTRACT_CLASS); 

    if (clsFlags & CORINFO_FLG_ARRAY)
    {
        CORINFO_CLASS_HANDLE elemTypeHnd;

        CorInfoType corType;
        corType = m_jitInfo->getChildType(clsHnd, &elemTypeHnd);
        VASSERT(!(elemTypeHnd == 0 && corType == CORINFO_TYPE_VALUECLASS));

        Verify(elemTypeHnd == 0 ||
               !(getClassAttribs(elemTypeHnd) & CORINFO_FLG_CONTAINS_STACK_PTR),
               MVER_E_SIG_ARRAY_BYREF);

        verVerifyCall(opcode, memberRef, false, codeAddr, vstate );

        // Arrays need to call the NEWOBJ helper.
        VASSERT(clsFlags & CORINFO_FLG_VAROBJSIZE);

        /* The varargs helper needs the type and method handles as last
           and  last-1 param (this is a cdecl call, so args will be
           pushed in reverse order on the CPU stack) */


        eeGetMethodSig(methHnd, &sig, false);
        VASSERT(sig.numArgs);

        for (count = 0; count < sig.numArgs; count++)
        {
            vstate->pop();
        }

        clsHnd = m_jitInfo->findMethodClass(getCurrentModuleHandle(), memberRef, getCurrentMethodHandle());
        vstate->push(vertype(TI_REF, clsHnd));
        return;
    }
    else
    {
        // goto CALL;

        verVerifyCall(opcode, memberRef, tailCall,
                      codeAddr, vstate);

        for (count = 0; count < sig.numArgs; count++) { // this pointer?
            vstate->pop();
        }

        if (clsFlags & CORINFO_FLG_VALUECLASS)
        {
            VASSERT(!(clsFlags & CORINFO_FLG_VAROBJSIZE));
            vstate->push(verMakeTypeInfo(clsHnd).NormaliseForStack());
        }
        else
        {
            vstate->push(vertype(TI_REF, clsHnd));
        }
    }

}

void ReaderBase::VerifyPop(VerificationState *vstate)
{
    if (!m_verificationNeeded) {
        return;
    }

    VP("verify pop\n");

    vstate->popPossiblyUninit();
}

void ReaderBase::VerifyRefAnyType(VerificationState *vstate)
{
    if (!m_verificationNeeded) {
        return;
    }

    VP("verify refanytype\n");

    VerifyEqual(vstate->pop(),
                verMakeTypeInfo(m_jitInfo->getBuiltinClass(CLASSID_TYPED_BYREF)));
    vstate->push(vertype(TI_STRUCT, m_jitInfo->getBuiltinClass(CLASSID_TYPE_HANDLE)));
}

void ReaderBase::VerifyRefAnyVal(VerificationState *vstate, mdToken typeRef)
{
    vertype tiRetVal;
    CORINFO_CLASS_HANDLE clsHnd;

    if (!m_verificationNeeded) {
        return;
    }

    VP("verify refanyval\n");

    VerifyIsClassToken(typeRef);

    clsHnd = verGetClassHandle(typeRef);

    VerifyEqual(vstate->pop(),
                verMakeTypeInfo(m_jitInfo->getBuiltinClass(CLASSID_TYPED_BYREF)));

    tiRetVal = verMakeTypeInfo(clsHnd).MakeByRef();

    vstate->push(tiRetVal);
}

#ifdef _MSC_VER
#pragma optimize("",off)
#endif

void ReaderBase::VerifyReturn(VerificationState *vstate, EHRegion *region)
{
    // First need the signature.
    CORINFO_SIG_INFO sig;
    CorInfoType corType;
    unsigned expectedStack;

    if (!m_verificationNeeded) return;

    VP("verify return\n");

    // 'this' must be init before return
    if (m_verTrackObjCtorInitState)
        Verify(vstate->isThisInitialized() == true, MVER_E_THIS_UNINIT_RET);

    if (region)
    {
        switch (RgnGetRegionType(region))
        {
            case ReaderBaseNS::RGN_FILTER:
                BADCODE(MVER_E_RET_FROM_FIL);
                break;
            case ReaderBaseNS::RGN_TRY:
                BADCODE(MVER_E_RET_FROM_TRY);
                break;
            case ReaderBaseNS::RGN_FAULT:
            case ReaderBaseNS::RGN_FINALLY:
            case ReaderBaseNS::RGN_MEXCEPT:
            case ReaderBaseNS::RGN_MCATCH:
                BADCODE(MVER_E_RET_FROM_HND);
                break;
            case ReaderBaseNS::RGN_ROOT:
                break;
            default:
                VASSERT(UNREACHED);
                break;
        }
    }

    m_jitInfo->getMethodSig(getCurrentMethodHandle(), &sig);

    // Now get the return type and convert it to our format.
    corType = sig.retType;

    expectedStack = 0;
    if (corType != CORINFO_TYPE_VOID)
    {
        Verify(vstate->stackLevel() > 0, MVER_E_RET_MISSING);
        Verify(vstate->stackLevel() == 1, MVER_E_RET_EMPTY);

        vertype tiVal = vstate->impStackTop(0);
        vertype tiDeclared = verMakeTypeInfo(corType, sig.retTypeClass);

        VerifyCompatibleWith(tiVal, tiDeclared.NormaliseForStack());
        Verify((!verIsByRefLike(tiDeclared)) || verIsSafeToReturnByRef(tiVal), MVER_E_RET_PTR_TO_STACK);
        expectedStack=1;
    }
    else
    {
        Verify(vstate->stackLevel() == 0, MVER_E_RET_VOID);
    }

    if (expectedStack == 1)
        vstate->pop();
    else
        VASSERT(!expectedStack);
}
#ifdef _MSC_VER
#pragma optimize("",on)
#endif

void ReaderBase::VerifyShift(VerificationState *vstate)
{
    if (!m_verificationNeeded) return;

    VP("verify shift\n");

    vertype amount = vstate->pop();

    VerifyCompatibleWith(amount, vertype(TI_I));
    VerifyIsIntegerType(vstate->impStackTop(0));
    // pop, then push same type = NOP
}

void ReaderBase::VerifySizeOf(VerificationState *vstate, mdToken typeRef)
{
    if (!m_verificationNeeded) return;

    VP("verify sizeof\n");

    VerifyIsClassToken(typeRef);

    CORINFO_CLASS_HANDLE clsHnd;

    clsHnd = verGetClassHandle(typeRef);


    vstate->push(vertype(TI_INT));
}

void ReaderBase::VerifyStarg(VerificationState *vstate, unsigned num)
{
    if (!m_verificationNeeded) return;

    VP("verify storearg\n");

    Verify(num < m_numVerifyParams, MVER_E_ARG_NUM); //"arg out of range"

    vertype x = m_paramVerifyMap[num];

    x.NormaliseForStack();

    VerifyCompatibleWith(vstate->pop(), x);

    if (m_verTrackObjCtorInitState && !vstate->isThisInitialized() )
        Verify(!m_paramVerifyMap[num].IsThisPtr(), MVER_E_THIS_UNINIT_STORE); //"storing to uninit this ptr"

}

void ReaderBase::VerifyStoreElem(VerificationState *vstate, ReaderBaseNS::StElemOpcode opcode, mdToken tok)
{
    CorInfoType type = CORINFO_TYPE_UNDEF;

    if (!m_verificationNeeded) return;

    VP("verify storeelem\n");

    vertype tiValue = vstate->pop();
    vertype tiIndex = vstate->pop();
    vertype tiArray = vstate->pop();

    VerifyAndReportFound(tiIndex.IsType(TI_INT) || tiIndex.IsType(TI_I), tiIndex, MVER_E_STACK_UNEXPECTED);

    // note null objref counts
    VerifyIsSDArray(tiArray);

    switch(opcode){
    case ReaderBaseNS::STELEM_I1:
        type = CORINFO_TYPE_BYTE;
        break;
    case ReaderBaseNS::STELEM_I2:
        type = CORINFO_TYPE_SHORT;
        break;
    case ReaderBaseNS::STELEM_I:
        type = CORINFO_TYPE_NATIVEINT;
        break;
    case ReaderBaseNS::STELEM_I4:
        type = CORINFO_TYPE_INT;
        break;
    case ReaderBaseNS::STELEM_I8:
        type = CORINFO_TYPE_LONG;
        break;
    case ReaderBaseNS::STELEM_R4:
        type = CORINFO_TYPE_FLOAT;
        break;
    case ReaderBaseNS::STELEM_R8:
        type = CORINFO_TYPE_DOUBLE;
        break;
    case ReaderBaseNS::STELEM_REF:
        break;
    case ReaderBaseNS::STELEM: // Generic
    {
        VerifyIsClassToken(tok); //TODO: add this check to Importer.cpp

        CORINFO_CLASS_HANDLE stelemClsHnd = verGetClassHandle(tok);
        vertype tiDeclared;

        tiDeclared = verMakeTypeInfo(stelemClsHnd);

        if (!tiArray.IsNullObjRef())
        {
            VerifyCompatibleWith(tiDeclared,verGetArrayElemType(tiArray));
        }

        VerifyCompatibleWith(tiValue, tiDeclared.NormaliseForStack());
        return;

    }
    break;

    default:
        VASSERT(UNREACHED);
    }

    if (opcode != ReaderBaseNS::STELEM_REF)
    {
        vertype tiDeclared(type);

        if (!tiArray.IsNullObjRef())
        {
            vertype tiArrayType = verGetArrayElemType(tiArray);

            VerifyEqual(tiArrayType, tiDeclared);
            VerifyAndReportFound(
                tiArrayType.GetType() == tiDeclared.GetType(), tiArrayType,MVER_E_STACK_UNEXP_ARRAY);

        }
        VerifyEqual(tiValue, tiDeclared.NormaliseForStack());
    }
    else
    {
        VerifyIsObjRef(tiValue);
        // we only check that it is an object referece, The helper does additional checks

        Verify(tiArray.IsNullObjRef() ||
               verGetArrayElemType(tiArray).IsType(TI_REF), MVER_E_ARRAY_SD_PTR);
    }


}

void ReaderBase::VerifyStloc(VerificationState *vstate, unsigned num)
{
    vertype x;

    if (!m_verificationNeeded) return;

    VP("verify storelocal\n");

    // out of range
    Verify(num < m_numVerifyAutos, MVER_E_LOC_NUM);

    x = m_autoVerifyMap[num];
    x.NormaliseForStack();

    VerifyCompatibleWith(vstate->pop(), x);
}

void ReaderBase::VerifyStoreIndir(VerificationState *vstate,ReaderBaseNS::StIndirOpcode opcode)
{
    ti_types instrType = TI_ERROR;
    vertype ptrVal;

    if (!m_verificationNeeded) return;

    VP("verify storeindir\n");

    vstate->unalignedPrefix = false;
    vstate->volatilePrefix  = false;

    switch(opcode)
    {
    case ReaderBaseNS::STIND_I1:      instrType  = TI_BYTE;     break;
    case ReaderBaseNS::STIND_I2:      instrType  = TI_SHORT;    break;
    case ReaderBaseNS::STIND_I4:      instrType  = TI_INT;      break;
    case ReaderBaseNS::STIND_I8:      instrType  = TI_LONG;     break;
    case ReaderBaseNS::STIND_I:       instrType  = TI_I;        break;
    case ReaderBaseNS::STIND_REF:     instrType  = TI_REF;      break;
    case ReaderBaseNS::STIND_R4:      instrType  = TI_FLOAT;    break;
    case ReaderBaseNS::STIND_R8:      instrType  = TI_DOUBLE;   break;
    default:
        VASSERT(UNREACHED);
    }
    Verify(!vstate->impStackTop(1).IsReadonlyByRef(), MVER_E_READONLY_ILLEGAL_WRITE); 
    ptrVal = verVerifyLDIND(vstate->impStackTop(1), instrType);
    VerifyCompatibleWith(vstate->impStackTop(0), ptrVal.NormaliseForStack());

    vstate->pop();
    vstate->pop();
}

void ReaderBase::VerifyStoreObj(VerificationState *vstate, mdToken tok)
{
    CORINFO_CLASS_HANDLE clsHnd;
    vertype ptrVal;
    vertype srcItem, dstItem;
    vertype clsItem;

    if (!m_verificationNeeded) return;

    VP("verify storeobj\n");

    vstate->unalignedPrefix = false;
    vstate->volatilePrefix  = false;

    VerifyIsClassToken(tok);

    srcItem = vstate->pop();
    dstItem = vstate->pop();

    VerifyIsByref(dstItem);
    Verify(!dstItem.IsReadonlyByRef(), MVER_E_READONLY_ILLEGAL_WRITE);//"write to readonly byref"
    
    clsHnd = verifyClass(verGetClassHandle(tok));

    clsItem = verMakeTypeInfo(clsHnd);

    VerifyCompatibleWith(srcItem, NormaliseForStack(clsItem));

    ptrVal = DereferenceByRef(dstItem);
    VerifyCompatibleWith(clsItem, ptrVal);

}

void ReaderBase::VerifyCkFinite(VerificationState *vstate)
{
    if (!m_verificationNeeded) return;

    VP("verify ckfinite\n");

    vertype tiRetVal = vstate->pop();
    Verify(tiRetVal.IsType(TI_DOUBLE), MVER_E_STACK_R_R4_R8);
    vstate->push(tiRetVal);
}

void ReaderBase::VerifyRethrow(VerificationState *vstate, EHRegion *region)
{
    if (!m_verificationNeeded) return;

    VP("verify rethrow\n");

    // ECMA says rethrow can only be used in catch clause
    // walk up the tree until we find the first handler, and it better be a catch
    while (region)
    {
        if (RgnGetRegionType(region) == ReaderBaseNS::RGN_TRY)
        {
            region = RgnGetParent(region);
        }
        else
        {
            Verify(RgnGetRegionType(region) == ReaderBaseNS::RGN_MCATCH
                   || RgnGetRegionType(region) == ReaderBaseNS::RGN_MEXCEPT,
                   MVER_E_RETHROW);
            return;
        }
    }

    Verify(0, MVER_E_RETHROW);

    // rethrow needs no type checking for verification
    return;
}




void ReaderBase::VerifyBoolBranch(VerificationState *vstate, unsigned int nextOffset, unsigned int targetOffset)
{
    if (!m_verificationNeeded) return;

    VP("verify bool branch\n");

    vertype temp = vstate->pop();
    VerifyAndReportFound(temp.GetType() != TI_DOUBLE, temp, MVER_E_STACK_NO_R_I8);

    VerifyAndReportFound(temp.IsObjRef()
                         || temp.IsByRef()
                         || temp.IsIntegerType()
                         || temp.IsMethod(), temp, MVER_E_STACK_UNEXPECTED);

    // backwards branch with uninitialized 'this' is actually allowed

}

void ReaderBase::VerifyBox(VerificationState *vstate, mdToken typeRef)
{
    CORINFO_CLASS_HANDLE clsHnd;

    vertype tiRetVal;
    vertype tiActual;
    vertype tiBox;

    if (!m_verificationNeeded) return;

    VP("verify box\n");

    VerifyIsClassToken(typeRef);

    // Get the Class index
    clsHnd = verGetClassHandle(typeRef);

    tiActual = vstate->pop();
    tiBox = verMakeTypeInfo(clsHnd);

    VerifyIsBoxable(tiBox);

    VerifyAndReportFound(m_jitInfo->satisfiesClassConstraints(clsHnd),
                         tiBox,
                         MVER_E_UNSATISFIED_BOX_OPERAND);
    
    
    VerifyCompatibleWith(tiActual, tiBox.NormaliseForStack());

    // Push the result back on the stack,
    // even if clsHnd is a value class we want the TI_REF
    // we call back to the EE to get find out what hte type we should push (for nullable<T> we push T) 
    tiRetVal = vertype(TI_REF, m_jitInfo->getTypeForBox(clsHnd));

    vstate->push(tiRetVal);
}

void ReaderBase::VerifyUnboxAny(VerificationState *vstate, mdToken typeRef)
{
    CORINFO_CLASS_HANDLE clsHnd;
    vertype tiRetVal;

    if (!m_verificationNeeded) return;

    VP("verify unbox\n");

    //any:
    // objref, initialized
    //regular:
    // initialized, not valueclass w/ pointer, and compatible

    VerifyIsClassToken(typeRef);

    clsHnd = verGetClassHandle(typeRef);

    VerifyIsObjRef(vstate->pop());

    tiRetVal = verMakeTypeInfo(clsHnd);

    // doc : unbox.any on ref type or type parameter is equiv to castclass
    if (!verIsValueClass(clsHnd))
    {
        vstate->push(tiRetVal);
        return;
    }
    else
    {
        VerifyAndReportFound(tiRetVal.IsValueClass() || tiRetVal.IsPrimitiveType(), tiRetVal, MVER_E_VALCLASS);
        tiRetVal.MakeByRef();

        // We always come from an objref, so this is safe byref
        tiRetVal.SetIsPermanentHomeByRef();
        
        vstate->push(tiRetVal);
        
        // doc : when applied to boxed value type, unbox.any is equiv to unbox + ldobj
        VerifyLoadObj(vstate, typeRef);
    }
}


void ReaderBase::VerifyUnbox(VerificationState *vstate, mdToken typeRef)
{

    CORINFO_CLASS_HANDLE clsHnd;
    vertype tiRetVal;

    if (!m_verificationNeeded) return;

    VP("verify unbox\n");

    //any:
    // objref, initialized
    //regular:
    // initialized, not valueclass w/ pointer, and compatible


    VerifyIsClassToken(typeRef);

    clsHnd = verGetClassHandle(typeRef);

    tiRetVal = verMakeTypeInfo(clsHnd);

    VerifyIsObjRef(vstate->pop());
    VerifyAndReportFound(tiRetVal.IsValueClass() || tiRetVal.IsPrimitiveType(), tiRetVal, MVER_E_VALCLASS);
    tiRetVal.MakeByRef();

    // We always come from an objref, so this is safe byref
    tiRetVal.SetIsPermanentHomeByRef();
    tiRetVal.SetIsReadonlyByRef();

    vstate->push(tiRetVal);
}

void ReaderBase::VerifyMkRefAny(VerificationState *vstate, mdToken typeRef)
{
    CORINFO_CLASS_HANDLE clsHnd;

    if (!m_verificationNeeded) return;

    VP("verify mkrefany\n");

    VerifyIsClassToken(typeRef);

    clsHnd = verGetClassHandle(typeRef);

    vertype tiPtr = vstate->pop();
    vertype tiInstr = verMakeTypeInfo(clsHnd);

    Verify(!verIsByRefLike(tiInstr), MVER_E_RA_PTR_TO_STACK);

    Verify(!tiPtr.IsReadonlyByRef(), MVER_E_READONLY_IN_MKREFANY); //"readonly byref used with mkrefany"
    VerifyEqual(tiPtr.DereferenceByRef(), tiInstr);

    vstate->push(verMakeTypeInfo(
                     m_jitInfo->getBuiltinClass(CLASSID_TYPED_BYREF)
                     ));
}

void ReaderBase::VerifyNewArr(VerificationState *vstate, mdToken token)
{
    CORINFO_CLASS_HANDLE clsHnd = NULL;
    CORINFO_CLASS_HANDLE arrayClsHnd = NULL;

    if (!m_verificationNeeded) return;

    vertype tiRetVal;
    vertype tiVal = vstate->pop();


    VP("verify newarr\n");

    VerifyIsClassToken(token);

    clsHnd = verGetClassHandle(token);

    PAL_CPP_TRY
    {
        arrayClsHnd = m_jitInfo->getSDArrayForClass(clsHnd);
    }
    PAL_CPP_CATCH_ALL
    {
        GetErrorMessage(this, m_jitInfo);
        Verify(false, MVER_E_TOKEN_RESOLVE);
    }
    PAL_CPP_ENDTRY

    //"Can't get array class"
    if (arrayClsHnd == 0)
        BADCODE(MVER_E_SIG_ARRAY);

    VerifyEqual(tiVal, vertype(TI_I));

    //MVER_E_SIG_ARRAY_TB_AH
    Verify(
        !(getClassAttribs(clsHnd) & CORINFO_FLG_CONTAINS_STACK_PTR),
        MVER_E_SIG_ARRAY_BYREF);
    tiRetVal = verMakeTypeInfo(arrayClsHnd);

    /* Push the result of the call on the stack */
    vstate->push(tiRetVal);
}

void ReaderBase::verVerifyField(CORINFO_FIELD_HANDLE fldHnd, CORINFO_CLASS_HANDLE enclosingClass, const vertype* tiThis,
                                 unsigned fieldFlags, BOOL mutator)
{
    CORINFO_CLASS_HANDLE instanceClass = getCurrentMethodClass();

    // for statics, we imagine the instance is the same as the current
    bool isStaticField = ((fieldFlags & CORINFO_FLG_STATIC) != 0);

    if (mutator)
    {
        //"mutating an RVA bases static"
        Verify(!(fieldFlags & CORINFO_FLG_UNMANAGED), MVER_E_WRITE_RVA_STATIC);

        // corinfo_flg_final means initonly...don't ask me
        if ((fieldFlags & CORINFO_FLG_FINAL))
        {
            bool isStaticMethod = (getCurrentMethodAttribs() & CORINFO_FLG_STATIC) != 0;
            //"bad use of initonly field (set or address taken)"
            Verify((getCurrentMethodAttribs() & CORINFO_FLG_CONSTRUCTOR) &&
                   enclosingClass == instanceClass
                   && isStaticMethod == isStaticField,
                   MVER_E_INITONLY);
        }
    }

    if (tiThis == 0)
    {
        Verify(isStaticField, MVER_E_FIELD_STATIC); //"used static opcode with non-static field"
    }
    else
    {
        vertype tThis = *tiThis;
        // If it is null, we assume we can access it (since it will AV shortly)
        // If it is anything but a refernce class, there is no hierarchy, so
        // again, we don't need the precise instance class to compute 'protected' access
        if (tiThis->IsType(TI_REF))
            instanceClass = tiThis->GetClassHandleForObjRef();

        // Note that even if the field is static, we require that the this pointer
        // satisfy the same constraints as a non-static field  This happens to
        // be simpler and seems reasonable
        vertype tiDeclaredThis = verMakeTypeInfo(enclosingClass);
        if (tiDeclaredThis.IsValueClass()) {
            tiDeclaredThis.MakeByRef();

                 // we allow read-only tThis, on any field access (even stores!), because if the
                 // class implementor wants to prohibit stores he should make the field private.
                 // we do this by setting the read-only bit on the type we compare tThis to. 
            tiDeclaredThis.SetIsReadonlyByRef();
        }
        else if (verNeedsCtorTrack() && tThis.IsThisPtr())
            tThis.SetInitialisedObjRef();

        VerifyCompatibleWith(tThis, tiDeclaredThis);
    }

    // Presently the JIT doe not check that we dont store or take the address of init-only fields
    // since we can not guarentee their immutability and it is not a security issue

    // check any constraints on the fields's class --- accessing the field might cause a class constructor to run.
    Verify(m_jitInfo->satisfiesClassConstraints(enclosingClass),
           MVER_E_UNSATISFIED_FIELD_PARENT_INST); //"field has unsatisfied class constraints"


    Verify(instanceClass &&
           m_jitInfo->canAccessField(getCurrentMethodHandle(), enclosingClass, fldHnd, instanceClass),
           MVER_E_FIELD_ACCESS);
}


LONG ReaderBase::FieldFilter(PEXCEPTION_POINTERS pExceptionPointers, LPVOID lpvParam)
{
   ICorJitInfo * jitInfo = (ICorJitInfo *)lpvParam;

   int answer = EXCEPTION_EXECUTE_HANDLER;
   verLastError = jitInfo->GetErrorHRESULT(pExceptionPointers);


   return answer;
}


void ReaderBase::VerifyFieldAccess(
    VerificationState *vstate,
    ReaderBaseNS::OPCODE opcode,
    mdToken token
){
    if (!m_verificationNeeded)
        return;

    vstate->volatilePrefix  = false;

    VP("verify fieldaccess\n");

    CORINFO_FIELD_HANDLE fldHnd = NULL;
    CORINFO_CLASS_HANDLE fieldClsHnd; // class of the field (if it's a ref type)
    CORINFO_CLASS_HANDLE parentClsHnd; // exact parent of the field
    CORINFO_CLASS_HANDLE clsHnd;
    vertype type;
    vertype fieldType;
    bool isStaticOpcode = false, isLoadAddress = false, isLoad = false;

    CORINFO_METHOD_HANDLE methodHandle;
    CORINFO_MODULE_HANDLE moduleHandle;

    if (!m_verificationNeeded)
        return;

    VP("verify fieldaccess\n");

    VerifyToken(token);

    methodHandle = getCurrentMethodHandle();
    moduleHandle = getCurrentModuleHandle();

    switch(opcode){
    case ReaderBaseNS::CEE_LDFLD:
        isStaticOpcode = false;
        isLoadAddress =  false;
        isLoad =         true;
        vstate->unalignedPrefix = false;
        break;
    case ReaderBaseNS::CEE_LDSFLD:
        isStaticOpcode = true;
        isLoadAddress =  false;
        isLoad =         true;
        break;
    case ReaderBaseNS::CEE_LDFLDA:
        isStaticOpcode = false;
        isLoadAddress =  true;
        isLoad =         true;
        break;
    case ReaderBaseNS::CEE_LDSFLDA:
        isStaticOpcode = true;
        isLoadAddress =  true;
        isLoad =         true;
        break;
    case ReaderBaseNS::CEE_STSFLD:
        isStaticOpcode = true;
        isLoadAddress =  false;
        isLoad =         false;
        break;
    case ReaderBaseNS::CEE_STFLD:
        isStaticOpcode = false;
        isLoadAddress =  false;
        isLoad =         false;
        vstate->unalignedPrefix = false;
        break;
    default:
        VASSERT(UNREACHED);
        break;
    }

    VerifyIsFieldToken(token);

    PAL_TRY
    {
        fldHnd = m_jitInfo->findField(moduleHandle, token, methodHandle);
    }
    PAL_EXCEPT_FILTER(ReaderBase::FieldFilter, m_jitInfo)
    {
        GetErrorMessage(this, m_jitInfo);
        Verify(false, verTranslateErrorHR(verLastError));
    }
    PAL_ENDTRY

    parentClsHnd = verGetExactFieldClass(token, fldHnd);

    /* Figure out the type of the member */
    CorInfoType ciType = getFieldType(fldHnd, &fieldClsHnd, parentClsHnd);

    type = JITtype2tiType(ciType);

    vertype tiObj, tiVal;
    vertype *ptiObj = NULL;

    /* Pull the value from the stack */
    if (!isLoad) {
        tiVal = vstate->pop();
        clsHnd = tiVal.GetClassHandle();
    }

    if (!isStaticOpcode)
    {
        tiObj = vstate->popPossiblyUninit();
        ptiObj = &tiObj;
    }

    /* Get hold of the flags for the member */
    unsigned mflags = getFieldAttribs(fldHnd, CORINFO_ACCESS_ANY);

    if (mflags & CORINFO_FLG_NONVERIFIABLYOVERLAPS)
    {
        Verify(false, MVER_E_FIELD_OVERLAP);
    }

    if (opcode == ReaderBaseNS::CEE_LDFLD 
        && tiObj.IsValueClass() && !tiObj.IsPrimitiveType())
        tiObj.MakeByRef();

    verVerifyField(fldHnd, parentClsHnd, ptiObj, mflags, (!isLoad) || isLoadAddress);
    fieldType = verMakeTypeInfo(ciType, fieldClsHnd);
    if (isLoad)
    {
        if (isLoadAddress) 
        {
            fieldType.MakeByRef();

            if (isStaticOpcode 
                && (mflags & CORINFO_FLG_SAFESTATIC_BYREF_RETURN))
            {
                fieldType.SetIsPermanentHomeByRef();
            }
            else if (tiObj.IsObjRef() || tiObj.IsPermanentHomeByRef())
            {
                // ldflda of byref is safe if done on a gc object or on  a 
                // safe byref
                fieldType.SetIsPermanentHomeByRef();
            }                                                        
        }
        else
            fieldType.NormaliseForStack();
    }
    else //isStore
    {
        VerifyCompatibleWith(tiVal, fieldType.NormaliseForStack());
    }


    if (isLoad)
        vstate->push(fieldType);
}

void ReaderBase::VerifySwitch(VerificationState *vstate)
{
    if (!m_verificationNeeded) return;

    VP("verify switch\n");

    VerifyCompatibleWith(vstate->pop(), TI_INT);
}

void ReaderBase::VerifyThrow(VerificationState *vstate)
{
    if (!m_verificationNeeded) return;
    vertype tiRetVal;

    VP("verify throw\n");

    tiRetVal = vstate->pop();
    VerifyIsObjRef(tiRetVal);

    if (m_verTrackObjCtorInitState &&
        !vstate->isThisInitialized() )
        Verify(!tiRetVal.IsThisPtr(), MVER_E_STACK_UNINIT);

    while (vstate->stackLevel() > 0)
    {
        // vstate->pop();
        // throw is not a return so we don't need to be initialized
        vstate->popPossiblyUninit();
    }

}

void ReaderBase::VerifyRecordLocalType(int num, CorInfoType type, CORINFO_CLASS_HANDLE hclass)
{
    vertype v = verMakeTypeInfo(type, hclass);
    m_autoVerifyMap[num] = v;

    // to match x86 JIT we must defer checking for unmanaged pointers 
    // until they are used
    if (v.GetRawType() == TI_PTR)
    {
        return;
    }
    VerifyTypeIsValid(v);
}

void ReaderBase::VerifyRecordLocalType(int num, CORINFO_SIG_INFO* sig, CORINFO_ARG_LIST_HANDLE args)
{
    vertype v = verParseArgSigToTypeInfo(sig, args);
    m_autoVerifyMap[num] = v;

    // to match x86 JIT we must defer checking for unmanaged pointers 
    // until they are used
    if (v.GetRawType() == TI_PTR)
    {
        return;
    }
    VerifyTypeIsValid(v);
}


bool ReaderBase::verNeedsCtorTrack()
{
    DWORD mflags = getMethodAttribs(getCurrentMethodHandle());

    bool needTrack;

    // Track this ptr initialization
    if (!(mflags & CORINFO_FLG_STATIC) &&
        (mflags & CORINFO_FLG_CONSTRUCTOR) &&
        m_paramVerifyMap[0].IsObjRef())
    {
        needTrack = true;
    }
    else
        needTrack = false;

    return needTrack;
}

void ReaderBase::VerifyIsNotUnmanaged(const vertype &v)
{
    Verify(v.GetRawType() != TI_PTR, MVER_E_UNMANAGED_POINTER);
}

void ReaderBase::VerifyTypeIsValid(const vertype &v)
{
    // want to give more descriptive error message in this case
    // even though it is caught in the below comparison
    if (v.GetRawType() == TI_PTR)
    {
        BADCODE(MVER_E_UNMANAGED_POINTER);
    }
    
    // overloaded operator, invalid typeinfo will not match itself
    if (!(v == v))
    {
        BADCODE(MVER_E_TOKEN_RESOLVE);
    }
}


void ReaderBase::VerifyRecordParamType(int num, CORINFO_SIG_INFO* sig, CORINFO_ARG_LIST_HANDLE args)
{
    vertype v = verParseArgSigToTypeInfo(sig, args);
    m_paramVerifyMap[num] = v;

    // to match x86 JIT we must defer checking for unmanaged pointers 
    // until they are used
    if (v.GetRawType() == TI_PTR)
    {
        return;
    }
    VerifyTypeIsValid(v);
}

void ReaderBase::VerifyRecordParamType(int num, CorInfoType type, CORINFO_CLASS_HANDLE hclass, bool makeByRef, bool isThis)
{
    vertype v = verMakeTypeInfo(type, hclass);


    if (makeByRef)
        v.MakeByRef();

    if (isThis) {
        v.SetIsThisPtr();
    }

    m_paramVerifyMap[num] = v;

    if (isThis)
    {
        m_verTrackObjCtorInitState = verNeedsCtorTrack();
    }
    // to match x86 JIT we must defer checking for unmanaged pointers 
    // until they are used
    if (v.GetRawType() == TI_PTR)
    {
        return;
    }

    VerifyTypeIsValid(v);
}


void ReaderBase::VerifyVolatile(VerificationState *vstate)
{
    if (!m_verificationNeeded) {
        return;
    }

    // illegal to stack prefixes except volatile and unaligned can be combined

    // note : duplicate prefixes are not fatal
    if (vstate->volatilePrefix)
        Verify(0, MVER_E_VOLATILE);
    if (vstate->tailPrefix)
        GVerify(0, MVER_E_TAIL_CALL);
    if (vstate->readonlyPrefix)
        GVerify(0, MVER_E_BAD_READONLY_PREFIX);
    if (vstate->constrainedPrefix)
        GVerify(0, MVER_E_BAD_CONSTRAINED_PREFIX);

    vstate->volatilePrefix = 1;
}

void ReaderBase::VerifyUnaligned(VerificationState *vstate, ReaderAlignType alignment)
{
    if (!m_verificationNeeded) {
        return;
    }

    // "alignment must be 1, 2, or 4"
    Verify(alignment == 1 || alignment == 2 || alignment == 4, MVER_E_UNALIGNED_ALIGNMENT);
    // illegal to stack prefixes except volatile and unaligned can be combined

    // note : duplicate prefixes are not fatal
    if (vstate->unalignedPrefix)
        Verify(0, MVER_E_UNALIGNED);
    if (vstate->tailPrefix)
        GVerify(0, MVER_E_TAIL_CALL);
    if (vstate->readonlyPrefix)
        GVerify(0, MVER_E_BAD_READONLY_PREFIX);
    if (vstate->constrainedPrefix)
        GVerify(0, MVER_E_BAD_CONSTRAINED_PREFIX);


    vstate->unalignedPrefix = 1;
}

void ReaderBase::VerifyTail(VerificationState *vstate, EHRegion *reg)
{
    if (!m_verificationNeeded) {
        return;
    }
    bool inHandler = !(m_currentRegion == NULL || ReaderBaseNS::RGN_ROOT == RgnGetRegionType( m_currentRegion));

    // The tail.call (or calli or callvirt) instruction cannot be used to transfer 
    // control out of a try, filter, catch, or finally block
    if (inHandler)
        BADCODE(MVER_E_TAILCALL_INSIDE_EH);

    // illegal to stack prefixes
    if (vstate->unalignedPrefix)
        GVerify(0, MVER_E_UNALIGNED);
    // note : duplicate prefixes are not fatal
    if (vstate->tailPrefix)
        Verify(0, MVER_E_TAIL_CALL);
    if (vstate->volatilePrefix)
        GVerify(0, MVER_E_VOLATILE);
    if (vstate->readonlyPrefix)
        GVerify(0, MVER_E_BAD_READONLY_PREFIX);
    if (vstate->constrainedPrefix)
        GVerify(0, MVER_E_BAD_CONSTRAINED_PREFIX);

    vstate->tailPrefix = true;
}

void ReaderBase::VerifyConstrained(VerificationState *vstate, mdToken typeDefOrRefOrSpec)
{
    if (!m_verificationNeeded) {
        return;
    }

    VP("verify constrained\n");

    VerifyIsClassToken(typeDefOrRefOrSpec);

    //illegal to stack prefixes
    if (vstate->unalignedPrefix)
        GVerify(0, MVER_E_UNALIGNED);
    if (vstate->tailPrefix)
        GVerify(0, MVER_E_TAIL_CALL);
    if (vstate->volatilePrefix)
        GVerify(0, MVER_E_VOLATILE);
    if (vstate->readonlyPrefix)
        GVerify(0, MVER_E_BAD_READONLY_PREFIX);
    // note : duplicate prefixes are not fatal
    if (vstate->constrainedPrefix)
        Verify(0, MVER_E_BAD_CONSTRAINED_PREFIX);

    vstate->constrainedPrefix = true;

}

void ReaderBase::VerifyReadOnly(VerificationState *vstate)
{
    if (!m_verificationNeeded) {
        return;
    }

    VP("verify readonly\n");

    //illegal to stack prefixes
    if (vstate->unalignedPrefix)
        GVerify(0, MVER_E_UNALIGNED);
    if (vstate->tailPrefix)
        GVerify(0, MVER_E_TAIL_CALL);
    if (vstate->volatilePrefix)
        GVerify(0, MVER_E_VOLATILE);
    if (vstate->readonlyPrefix)
        GVerify(0, MVER_E_BAD_READONLY_PREFIX);
    if (vstate->constrainedPrefix)
        GVerify(0, MVER_E_BAD_CONSTRAINED_PREFIX);

    vstate->readonlyPrefix = true;
}


void ReaderBase::VerifyPrefixConsumed(VerificationState *vstate, ReaderBaseNS::OPCODE opcode)
{
    if (!m_verificationNeeded) {
        return;
    }

    // prefixes are immediately followed by instructions that can use.
    // unless last instr was a prefix none of the prefix flags better be set

    if (opcode == ReaderBaseNS::CEE_CALL
        || opcode == ReaderBaseNS::CEE_CALLI
        || opcode == ReaderBaseNS::CEE_CALLVIRT)
    {
        if (vstate->tailPrefix == true)
        {
            // all successive instrs must be
            vstate->tailInBlock = true;
        }

        // call consumes the prefix
        vstate->tailPrefix = false;
    }
    else
    {
        // "tail.call may only be followed by return"
        GVerify(vstate->tailInBlock == false || opcode == ReaderBaseNS::CEE_RET, MVER_E_TAIL_RET);
    }


    if (opcode != ReaderBaseNS::CEE_UNALIGNED
        && opcode != ReaderBaseNS::CEE_TAILCALL
        && opcode != ReaderBaseNS::CEE_VOLATILE
        && opcode != ReaderBaseNS::CEE_READONLY
        && opcode != ReaderBaseNS::CEE_CONSTRAINED
        )
    {
        //"The tail. instruction must immediately precede a call, calli, or callvirt instruction"
        GVerify(vstate->tailPrefix == false, MVER_E_TAIL_CALL);
        // "unaligned prefix must immediately precede ldind, stind, ldfld, stfld, ldobj, stobj, initblk, or cpblk "
        GVerify(vstate->unalignedPrefix == false, MVER_E_UNALIGNED);
        // "volatile prefix must immediately precede ldind, stind, ldfld, stfld, ldobj, stobj, initblk, or cpblk instruction"
        GVerify(vstate->volatilePrefix == false,  MVER_E_VOLATILE);
        GVerify(vstate->tailPrefix == false || vstate->unalignedPrefix == false, MVER_E_UNALIGNED);
        //"cannot stack volatile and tail prefix"
        GVerify(vstate->tailPrefix == false || vstate->volatilePrefix == false, MVER_E_VOLATILE);
        //"Missing ldelema/call following readonly. prefix."
        GVerify(vstate->readonlyPrefix == false, MVER_E_BAD_READONLY_PREFIX);
        // "Missing callvirt following constrained. prefix."
        GVerify(vstate->constrainedPrefix == false, MVER_E_BAD_CONSTRAINED_PREFIX);
    }
}
void ReaderBase::VerifyReturnFlow(unsigned int srcOffset)
{
    if (!m_verificationNeeded) {
        return;
    }

    EHRegion *sourceEHRegion = fgGetRegionFromMSILOffset(srcOffset);

    VP("verify return flow");
    // If there is no EH in the method then no further verification is needed
    if (NULL == m_ehRegionTree) {
        return;
    }

    if (sourceEHRegion != NULL
        && ReaderBaseNS::RGN_ROOT != RgnGetRegionType(sourceEHRegion))
    {
        switch (RgnGetRegionType(sourceEHRegion))
        {
            case ReaderBaseNS::RGN_FILTER:
                BADCODE(MVER_E_RET_FROM_FIL);
                break;
            case ReaderBaseNS::RGN_TRY:
                BADCODE(MVER_E_RET_FROM_TRY);
                break;
            case ReaderBaseNS::RGN_FAULT:
            case ReaderBaseNS::RGN_FINALLY:
            case ReaderBaseNS::RGN_MEXCEPT:
            case ReaderBaseNS::RGN_MCATCH:
                BADCODE(MVER_E_RET_FROM_HND);
                break;
            default:
                VASSERT(UNREACHED);
                break;

        }
    }
}


//
// Verify that a given branch has a legal target. We already would have bailed at this point if the
//  target of the branch is outside the scope of the program.
//
void ReaderBase::VerifyBranchTarget(VerificationState *vstate, FlowGraphNode *fg,
                                    EHRegion *sourceEHRegion,
                                    unsigned int targetOffset, bool isLeave)
{
    if (!m_verificationNeeded) {
        return;
    }

    VP("verify branch target");

    // At this point we can assume that the target offset is between the start and end of the program, if
    //  it wasn't we should have bailed already
    VASSERT(targetOffset >= 0 && targetOffset < m_methodInfo->ILCodeSize);

    // Check to see if the branch target is an actual instruction
    if (!IsOffsetInstrStart(targetOffset)) {
        // FATAL ERROR
        BADCODE(MVER_E_BAD_JMP_TARGET);
    }

    // If there is no EH in the method then no further verification is needed
    if (NULL == m_ehRegionTree) {
        return;
    }


    EHRegion *targetEHRegion, *rgn, *rgnLCA;
#if !defined(NODEBUG)
    targetEHRegion = (EHRegion *)NULL;
#endif


    targetEHRegion = fgGetRegionFromMSILOffset(targetOffset);

    // Assert that both nodes have valid EH pointers
    VASSERT(sourceEHRegion && targetEHRegion);

    // No further verification is needed if the branch is within a single EH region
    if (sourceEHRegion == targetEHRegion) {
        return;
    }

    // Walk a path from the source to the LCA of the source and the target
    // Vefify that it is legal to exit each region
    rgn = sourceEHRegion;
    while (   (RgnGetRegionType(rgn) != ReaderBaseNS::RGN_ROOT)
           && !(   (RgnGetStartMSILOffset(rgn) <= targetOffset)
                && (RgnGetEndMSILOffset(rgn)   >  targetOffset) ))
    {
        // Source region must be try, catch, root and
        //  must be a leave
        switch(RgnGetRegionType(rgn)) {
            case ReaderBaseNS::RGN_TRY:
                if (!isLeave)
                    BADCODE(MVER_E_BR_OUTOF_TRY);
                break;

            case ReaderBaseNS::RGN_MCATCH:
            case ReaderBaseNS::RGN_MEXCEPT:
                if (!isLeave)
                    BADCODE(MVER_E_BR_OUTOF_HND);
                break;

            case ReaderBaseNS::RGN_FAULT:
                if (isLeave)
                    BADCODE(MVER_E_LEAVE);
                else
                    BADCODE(MVER_E_BR_OUTOF_HND);

            case ReaderBaseNS::RGN_FINALLY:
                if (isLeave)
                    BADCODE(MVER_E_LEAVE);
                else
                    BADCODE(MVER_E_BR_OUTOF_FIN);
                break;

            case ReaderBaseNS::RGN_FILTER:
                if (isLeave)
                    BADCODE(MVER_E_LEAVE);
                else
                    BADCODE(MVER_E_BR_OUTOF_FIL);
                break;


            default:
                // THROW BAD PROGRAM EXCEPTION
                // should not get here
                BADCODE("Verification Error: Source of nonlocal flow must be try/catch/except");
                break;
        }

        rgn = RgnGetParent(rgn);
        VASSERT(rgn);

    }
    rgnLCA = rgn; // Mark the Lowest Common Ancestor

    // Walk a path from the target to the LCA of the source and the target
    // Vefify that it is legal to enter each region
    rgn = targetEHRegion;
    while (rgn != rgnLCA)
    {
        // If the target is in a try region, then the target must be the
        //  the start of the try region or the source must be within
        //  the try/handler pair. (rule 2)
        if (RgnGetRegionType(rgn) == ReaderBaseNS::RGN_TRY) {
            if (RgnGetStartMSILOffset(rgn) != targetOffset)
            {
                // THROW BAD PROGRAM EXCEPTION
                //"Verification Error: Flow from outside try and its hander must enter try at its start"
                BADCODE(MVER_E_BR_INTO_TRY);
            }
        } else {
            // Flow can never enter INTO a handler (rule 1)
            //"Verification Error: Can not branch into a handler"
            if (RgnGetRegionType(rgn) == ReaderBaseNS::RGN_FILTER) {
                BADCODE(MVER_E_BR_INTO_FIL);
            }
            else {
                BADCODE(MVER_E_BR_INTO_HND);
            }
        }

        rgn = RgnGetParent(rgn);
        VASSERT(rgn);
    }
}

void
ReaderBase::VerifyFallThrough(VerificationState *vs, FlowGraphNode *fg)
{
    VASSERT(m_verificationNeeded);

    VP("verify fallthrough");

    // If there is no EH in the method then no further verification is needed
    if (NULL == m_ehRegionTree) {
        return;
    }

    // If this is an empty block, the reader won't accurately know whether
    // there is a fallthrough.  In those cases in which there was an actual
    // MSIL fallthrough, the block containing the MSIL would have called
    // VerifyFallThrough to make sure that all is well.
    if (FgNodeGetStartMSILOffset(fg) == FgNodeGetEndMSILOffset(fg)) {
        return;
    }

    EHRegion *sourceEHRegion = FgNodeGetRegion(fg);
    EHRegion *targetEHRegion = (EHRegion *)NULL;
    EHRegion  *rgn;
    DWORD   targetOffset = FgNodeGetEndMSILOffset(fg);

    // Fallthrough follows two simple rules
    //   1. You can never fall out of an EH Region
    //   2. You can never fall into an EH Handler Region

    // Find the block containing the target region
    FlowGraphNode *succ = NULL;
    for(FlowGraphEdgeList *fgEdge = FgNodeGetSuccessorList(fg);
        fgEdge != NULL;
        fgEdge = FgEdgeListGetNextSuccessor(fgEdge))
    {
        // Must skip nominal edges
        if (FgEdgeListIsNominal(fgEdge))
            continue;

        succ = FgEdgeListGetSink(fgEdge);
        if (FgNodeGetStartMSILOffset(succ) == targetOffset)
        {
            break;
        }
    }

    // RULE 0:
    // It is never legal to fall-through when there is nothing to
    // fall-through to.
    if (succ == NULL) {
        //"Verification Error: No successor to fall-through instruction"
        if (GenIR_FgBlockIsRegionEnd(fg))
            BADCODE(MVER_E_FALLTHRU_EXCEP);
        else
            BADCODE(MVER_E_FALLTHRU);
    }

    targetEHRegion = FgNodeGetRegion(succ);
    VASSERT(targetEHRegion);

    // RULE 1:
    // It is never legal to exit an EH region via fall-through
    //
    // In the event that we fall-through into the block
    // marking the exit of EH, there may be no flow-graph
    // edge from that terminating block even if there is
    // fall-through.  This is good for the stability of EH
    // if we were to attempt to treat such errors as local faults,
    // because it means that we don't insert edges that could
    // confuse the complex EH code beyond the local impact.
    // However, it means that we have to verify leaving a
    // region without counting on the fallthrough edge.  This
    // is accomplished by checking explicitly to see if we
    // are falling off a block that is the end of a region.
    // Note that the annotation edges may have created an empty
    // block after this one that just contains the region end.
    if (GenIR_FgBlockIsRegionEnd(fg) ||
        ((FgNodeGetEndMSILOffset(succ) == targetOffset) &&
         GenIR_FgBlockIsRegionEnd(succ)))
    {
        //"Verification Error: Fall through detected out of an EH region"
        BADCODE(MVER_E_FALLTHRU_EXCEP);
    }


    // No further verification is needed if the flow is within a single EH region
    if (sourceEHRegion == targetEHRegion) {
        return;
    }

    // Assert that both nodes have valid EH pointers
    VASSERT(sourceEHRegion && targetEHRegion);

    // RULE 2:
    // You can never fall into an EH Handler Region
    rgn = targetEHRegion;
    while (rgn != sourceEHRegion) {
        switch (RgnGetRegionType(rgn)) {
            case ReaderBaseNS::RGN_TRY:
                // We can fall into a try
                break;

            default:
               //"Verification Error: Can not have fall-through into a handler"
               BADCODE(MVER_E_FALLTHRU_INTO_HND);
               break;
        }

        rgn = RgnGetParent(rgn);
        VASSERT(rgn);
    }
}


void ReaderBase::VerifyLeave(VerificationState *vs)
{

    if (!m_verificationNeeded) {
        return;
    }

    VP("verify leave");

    while(vs->stackLevel() > 0)
        vs->pop();
}


// =============================================================================
// =============================================================================
// =======    EIT Verification   ===============================================
// =============================================================================
// =============================================================================

#define WELL_FORMED_IL_CHECK 1
#define MAX_XCPTN_INDEX (USHRT_MAX-1)

struct  ReaderBase::EHblkDsc
{
    CORINFO_EH_CLAUSE_FLAGS ebdFlags;
    IL_OFFSET                 tryBegOff;
    IL_OFFSET                 tryEndOff;
    IL_OFFSET                 hndBegOff;
    IL_OFFSET                 hndEndOff;
    IL_OFFSET                 filterOff;

    union
    {
        EITVerBasicBlock *    ebdFilter;  // First block of filter, if (ebdFlags & CORINFO_EH_CLAUSE_FILTER)
        unsigned        ebdTyp;     // Exception type,        otherwise
    };
    unsigned short      ebdEnclosing; // The index of the enclosing outer region
};


struct  ReaderBase::EHNodeDsc
{
    enum EHBlockType {
        TryNode,
        FilterNode,
        HandlerNode,
        FinallyNode,
        FaultNode
    };

    EHBlockType             ehnBlockType;      // kind of EH block
    unsigned                ehnStartOffset;    // IL offset of start of the EH block
    unsigned                ehnEndOffset;      // IL offset past end of the EH block
    pEHNodeDsc              ehnNext;           // next (non-nested) block in sequential order
    pEHNodeDsc              ehnChild;          // leftmost nested block
    union {
        pEHNodeDsc          ehnTryNode;        // for filters and handlers, the corresponding try node
        pEHNodeDsc          ehnHandlerNode;    // for a try node, the corresponding handler node
    };
    pEHNodeDsc              ehnFilterNode;     // if this is a try node and has a filter, otherwise 0
    pEHNodeDsc              ehnEquivalent;     // if blockType=tryNode, start offset and end offset is same,


    inline void ehnSetTryNodeType()        {ehnBlockType = TryNode;}
    inline void ehnSetFilterNodeType()     {ehnBlockType = FilterNode;}
    inline void ehnSetHandlerNodeType()    {ehnBlockType = HandlerNode;}
    inline void ehnSetFinallyNodeType()    {ehnBlockType = FinallyNode;}
    inline void ehnSetFaultNodeType()      {ehnBlockType = FaultNode;}

    inline BOOL ehnIsTryBlock()            {return ehnBlockType == TryNode;}
    inline BOOL ehnIsFilterBlock()         {return ehnBlockType == FilterNode;}
    inline BOOL ehnIsHandlerBlock()        {return ehnBlockType == HandlerNode;}
    inline BOOL ehnIsFinallyBlock()        {return ehnBlockType == FinallyNode;}
    inline BOOL ehnIsFaultBlock()          {return ehnBlockType == FaultNode;}
};




/*****************************************************************************
 * The following code checks the following rules for the EH table:
 * 1. Overlapping of try blocks not allowed.
 * 2. Handler blocks cannot be shared between different try blocks.
 * 3. Try blocks with Finally or Fault blocks cannot have other handlers.
 * 4. If block A contains block B, A should also contain B's try/filter/handler.
 * 5. A block cannot contain it's related try/filter/handler.
 * 6. Nested block must appear before containing block
 *
 */

void                ReaderBase::verInitEHTree(unsigned numEHClauses)
{
    m_ehnNext = (EHNodeDsc*) getTempMemory(numEHClauses * sizeof(EHNodeDsc)*3);

    m_ehnTree = NULL;
}
/* Inserts the try, handler and filter (optional) clause information in a tree structure
 * in order to catch incorrect eh formatting (e.g. illegal overlaps, incorrect order)
 */


void                ReaderBase::verInsertEhNode(CORINFO_EH_CLAUSE* clause, EHblkDsc* handlerTab)
{
    EHNodeDsc* tryNode = m_ehnNext++;
    EHNodeDsc* handlerNode = m_ehnNext++;
    EHNodeDsc* filterNode = NULL; // optional

    tryNode->ehnSetTryNodeType();
    tryNode->ehnStartOffset = clause->TryOffset;
    tryNode->ehnEndOffset   = clause->TryOffset+clause->TryLength - 1;
    tryNode->ehnHandlerNode = handlerNode;

    if (clause->Flags & CORINFO_EH_CLAUSE_FINALLY)
        handlerNode->ehnSetFinallyNodeType();
    else if (clause->Flags & CORINFO_EH_CLAUSE_FAULT)
        handlerNode->ehnSetFaultNodeType();
    else
        handlerNode->ehnSetHandlerNodeType();

    handlerNode->ehnStartOffset = clause->HandlerOffset;
    handlerNode->ehnEndOffset = clause->HandlerOffset + clause->HandlerLength - 1;
    handlerNode->ehnTryNode = tryNode;

    if (clause->Flags & CORINFO_EH_CLAUSE_FILTER)
    {
        filterNode = m_ehnNext++;
        filterNode->ehnStartOffset = clause->FilterOffset;
        // compute end offset of filter by walking the BB chain

        // this is the definitive answer.  endfilter lies
        filterNode->ehnEndOffset = clause->HandlerOffset - 1;
        VASSERT(filterNode->ehnEndOffset != 0);
        filterNode->ehnSetFilterNodeType();
        filterNode->ehnTryNode = tryNode;
        tryNode->ehnFilterNode = filterNode;
    }

    verInsertEhNodeInTree(&m_ehnTree, tryNode);
    verInsertEhNodeInTree(&m_ehnTree, handlerNode);
    if (filterNode)
        verInsertEhNodeInTree(&m_ehnTree, filterNode);
}


/*
    The root node could be changed by this method.

    node is inserted to

        (a) right       of root (root.right       <-- node)
        (b) left        of root (node.right       <-- root; node becomes root)
        (c) child       of root (root.child       <-- node)
        (d) parent      of root (node.child       <-- root; node becomes root)
        (e) equivalent  of root (root.equivalent  <-- node)

    such that siblings are ordered from left to right
    child parent relationship and equivalence relationship are not violated


    Here is a list of all possible cases

    Case 1 2 3 4 5 6 7 8 9 10 11 12 13

         | | | | |
         | | | | |
    .......|.|.|.|..................... [ root start ] .....
    |        | | | |             |  |
    |        | | | |             |  |
   r|        | | | |          |  |  |
   o|          | | |          |     |
   o|          | | |          |     |
   t|          | | |          |     |
    |          | | | |     |  |     |
    |          | | | |     |        |
    |..........|.|.|.|.....|........|.. [ root end ] ........
                 | | | |
                 | | | | |
                 | | | | |

        |<-- - - - n o d e - - - -->|


   Case Operation
   --------------
    1    (b)
    2    Error
    3    Error
    4    (d)
    5    (d)
    6    (d)
    7    Error
    8    Error
    9    (a)
    10   (c)
    11   (c)
    12   (c)
    13   (e)


*/

void ReaderBase::verInsertEhNodeInTree(EHNodeDsc** ppRoot,
                                        EHNodeDsc* node)
{
    unsigned nStart = node->ehnStartOffset;
    unsigned nEnd   = node->ehnEndOffset;

    if (nStart > nEnd)
    {
        BADCODE(MVER_E_TRY_GTEQ_END); //"start offset greater or equal to end offset"
    }
    node->ehnNext = NULL;
    node->ehnChild = NULL;
    node->ehnEquivalent = NULL;

    while (TRUE)
    {
        if (*ppRoot == NULL)
        {
            *ppRoot = node;
            break;
        }
        unsigned rStart = (*ppRoot)->ehnStartOffset;
        unsigned rEnd   = (*ppRoot)->ehnEndOffset;

        if (nStart < rStart)
        {
            // Case 1
            if (nEnd < rStart)
            {
                // Left sibling
                node->ehnNext     = *ppRoot;
                *ppRoot         = node;
                return;
            }
            // Case 2, 3
            if (nEnd < rEnd)
            {
//[Error]
                BADCODE(MVER_E_TRY_OVERLAP);//"Overlapping try regions"
            }

            // Case 4, 5
//[Parent]
            verInsertEhNodeParent(ppRoot, node);
            return;
        }


        // Cases 6 - 13 (nStart >= rStart)

        if (nEnd > rEnd)
        {   // Case 6, 7, 8, 9

            // Case 9
            if (nStart > rEnd)
            {
//[RightSibling]

                // Recurse with Root.Sibling as the new root
                ppRoot = &((*ppRoot)->ehnNext);
                continue;
            }

            // Case 6
            if (nStart == rStart)
//[Parent]
            {
                verInsertEhNodeParent(ppRoot, node);
                return;
            }

            // Case 7, 8
            BADCODE(MVER_E_TRY_OVERLAP); //"Overlapping try regions"
        }

        // Case 10-13 (nStart >= rStart && nEnd <= rEnd)
        if ((nStart != rStart) || (nEnd != rEnd))
        {   // Cases 10,11,12
//[Child]

            if ((*ppRoot)->ehnIsTryBlock())
            {
                //"Inner try appears after outer try in exception handling table"
                BADCODE(MVER_E_INNERMOST_FIRST);
            }
            else
            {
                // check this!
                ppRoot = &((*ppRoot)->ehnChild);
                continue;
            }
        }

        // Case 13
//[Equivalent]
        if (!node->ehnIsTryBlock() &&
            !(*ppRoot)->ehnIsTryBlock())
        {
            BADCODE(MVER_E_HND_EQ);//"Handlers cannot be shared"
        }

        node->ehnEquivalent = node->ehnNext = *ppRoot;

        // check that the corresponding handler is either a catch handler
        // or a filter
        if (node->ehnHandlerNode->ehnIsFaultBlock()           ||
            node->ehnHandlerNode->ehnIsFinallyBlock()         ||
            (*ppRoot)->ehnHandlerNode->ehnIsFaultBlock()      ||
            (*ppRoot)->ehnHandlerNode->ehnIsFinallyBlock() )
        {
            //"Try block with multiple non-filter/non-handler blocks"
            BADCODE(MVER_E_TRY_SHARE_FIN_FAL);
        }


        break;
    }
}

/**********************************************************************
 * Make node the parent of *ppRoot. All siblings of *ppRoot that are
 * fully or partially nested in node remain siblings of *ppRoot
 */


void            ReaderBase::verInsertEhNodeParent(EHNodeDsc** ppRoot,
                                                   EHNodeDsc*  node)
{
    VASSERT(node->ehnNext == NULL);
    VASSERT(node->ehnChild == NULL);

    // Root is nested in Node
    VASSERT(node->ehnStartOffset <= (*ppRoot)->ehnStartOffset);
    VASSERT(node->ehnEndOffset   >= (*ppRoot)->ehnEndOffset);

    // Root is not the same as Node
    VASSERT(node->ehnStartOffset != (*ppRoot)->ehnStartOffset ||
           node->ehnEndOffset != (*ppRoot)->ehnEndOffset);

    if (node->ehnIsFilterBlock())
    {
        switch((*ppRoot)->ehnBlockType)
        {
        case ReaderBase::EHNodeDsc::TryNode:
            BADCODE(MVER_E_FIL_CONT_TRY);
            break;
        case ReaderBase::EHNodeDsc::FilterNode:
            BADCODE(MVER_E_FIL_CONT_FIL);
            break;
        case ReaderBase::EHNodeDsc::HandlerNode:
        case ReaderBase::EHNodeDsc::FinallyNode:
        case ReaderBase::EHNodeDsc::FaultNode:
            BADCODE(MVER_E_FIL_CONT_HND);
            break;
        default:
            VASSERT(UNREACHED);

        }

    }

    EHNodeDsc *lastChild = NULL;
    EHNodeDsc *sibling   = (*ppRoot)->ehnNext;

    while (sibling)
    {
        // siblings are ordered left to right, largest right.
        // nodes have a width of atleast one.
        // Hence sibling start will always be after Node start.

        VASSERT(sibling->ehnStartOffset > node->ehnStartOffset);   // (1)

        // disjoint
        if (sibling->ehnStartOffset > node->ehnEndOffset)
            break;

        // partial containment.
        if (sibling->ehnEndOffset > node->ehnEndOffset)   // (2)
        {

            BADCODE(MVER_E_TRY_OVERLAP); //"Overlapping try regions"
        }
        //else full containment (follows from (1) and (2))

        lastChild = sibling;
        sibling = sibling->ehnNext;
    }

    // All siblings of Root upto and including lastChild will continue to be
    // siblings of Root (and children of Node). The node to the right of
    // lastChild will become the first sibling of Node.
    //

    if (lastChild)
    {
        // Node has more than one child including Root

        node->ehnNext      = lastChild->ehnNext;
        lastChild->ehnNext = NULL;
    }
    else
    {
        // Root is the only child of Node
        node->ehnNext      = (*ppRoot)->ehnNext;
        (*ppRoot)->ehnNext  = NULL;
    }

    node->ehnChild = *ppRoot;
    *ppRoot     = node;

}

#define NO_ENCLOSING_INDEX 0xffff

/*****************************************************************************
 * Checks the following two conditions:
 * 1) If block A contains block B, A should also contain B's try/filter/handler.
 * 2) A block cannot contain it's related try/filter/handler.
 * Both these conditions are checked by making sure that all the blocks for an
 * exception clause is at the same level.
 * The algorithm is: for each exception clause, determine the first block and
 * search through the next links for its corresponding try/handler/filter as the
 * case may be. If not found, then fail.
 */


void ReaderBase::verCheckNestingLevel(EHNodeDsc* root)
{
    CORINFO_METHOD_HANDLE  methodHnd  = getCurrentMethodHandle();

    EHNodeDsc* ehnNode = root;

    #define exchange(a,b) { temp = a; a = b; b = temp;}

    for (unsigned XTnum = 0; XTnum < m_methodInfo->EHcount; XTnum++)
    {
        EHNodeDsc *p1, *p2, *p3, *temp, *search;

        p1 = ehnNode++;
        p2 = ehnNode++;

        // we are relying on the fact that ehn nodes are allocated sequentially.
        VASSERT(p1->ehnHandlerNode == p2);
        VASSERT(p2->ehnTryNode == p1);

        // arrange p1 and p2 in sequential order
        if (p1->ehnStartOffset == p2->ehnStartOffset)
            BADCODE(MVER_E_HND_EQ); //"shared exception handler"

        if (p1->ehnStartOffset > p2->ehnStartOffset)
            exchange(p1,p2);

        temp = p1->ehnNext;
        unsigned numSiblings = 0;

        search = p2;
        if (search->ehnEquivalent)
            search = search->ehnEquivalent;

        do {
            if (temp == search)
            {
                numSiblings++;
                break;
            }
            if (temp)
                temp = temp->ehnNext;
        } while (temp);

        CORINFO_EH_CLAUSE clause;
        m_jitInfo->getEHinfo(methodHnd, XTnum, &clause);

        if (clause.Flags & CORINFO_EH_CLAUSE_FILTER)
        {
            p3 = ehnNode++;

            VASSERT(p3->ehnTryNode == p1 || p3->ehnTryNode == p2);
            VASSERT(p1->ehnFilterNode == p3 || p2->ehnFilterNode == p3);

            if (p3->ehnStartOffset < p1->ehnStartOffset)
            {
                temp = p3; search = p1;
            }
            else if (p3->ehnStartOffset < p2->ehnStartOffset)
            {
                temp = p1; search = p3;
            }
            else
            {
                temp = p2; search = p3;
            }
            if (search->ehnEquivalent)
                search = search->ehnEquivalent;
            do {
                if (temp == search)
                {
                    numSiblings++;
                    break;
                }
                temp = temp->ehnNext;
            } while (temp);
        }
        else
        {
            numSiblings++;
        }

        if (numSiblings != 2)
            // Outer block does not contain all code in inner handler
            BADCODE(MVER_E_LEXICAL_NESTING);
    }

}

inline bool jitIsBetween(unsigned offs, unsigned start, unsigned end)
{
     return (offs >= start && offs < end);
}

void ReaderBase::VerifyEIT()
{
    CORINFO_METHOD_HANDLE  methodHnd  = getCurrentMethodHandle();

    unsigned  XTnum;

    if (m_methodInfo->EHcount == 0)
        return;

    m_verBBCount = 0;

    // Check and mark all the exception handlers

    for (XTnum = 0; XTnum < m_methodInfo->EHcount; XTnum++)
    {
        DWORD tmpOffset;
        CORINFO_EH_CLAUSE clause;
        m_jitInfo->getEHinfo(methodHnd, XTnum, &clause);
        VASSERT(clause.HandlerLength != (unsigned)-1);


        if (clause.TryLength <= 0)
            BADCODE(MVER_E_TRY_GTEQ_END); //"try block length <=0"
        // Mark the 'try' block extent and the handler itself

        if (clause.TryOffset > m_methodInfo->ILCodeSize)
            BADCODE(MVER_E_TRYEND_GT_CS);

        tmpOffset = clause.TryOffset + clause.TryLength;
        if (tmpOffset > m_methodInfo->ILCodeSize)
            BADCODE(MVER_E_TRYEND_GT_CS);

        if (clause.HandlerOffset > m_methodInfo->ILCodeSize)
            BADCODE(MVER_E_HNDEND_GT_CS);

        // Special case handler's beginning at offset 0.
        if (clause.HandlerOffset == 0)
            BADCODE(MVER_E_FALLTHRU_INTO_HND);

        tmpOffset = clause.HandlerOffset + clause.HandlerLength;
        if (tmpOffset <= clause.HandlerOffset)
            BADCODE(MVER_E_HND_GTEQ_END);
        if (tmpOffset > m_methodInfo->ILCodeSize)
            BADCODE(MVER_E_HNDEND_GT_CS);

        if (clause.Flags & CORINFO_EH_CLAUSE_FILTER)
        {
            if (clause.FilterOffset > m_methodInfo->ILCodeSize)
                BADCODE(MVER_E_FIL_GTEQ_CS);
        }
    }

    if (m_methodInfo->EHcount > MAX_XCPTN_INDEX)
    {
        IMPL_LIMITATION("JitCompiler does not handle too many exception clauses");
        BADCODE(MVER_E_INTERNAL);
    }

    // Allocate the exception handler table

    m_compHndBBTab = (EHblkDsc *) getTempMemory(m_methodInfo->EHcount * sizeof(*m_compHndBBTab));

    verInitEHTree(m_methodInfo->EHcount);
    EHNodeDsc* initRoot = m_ehnNext; // remember the original root since
                                     // it may get modified during insertion

    // Annotate BBs with exception handling information required for generating correct eh code
    // as well as checking for correct IL
    EHblkDsc * handlerTab = m_compHndBBTab;

    for (XTnum = 0; XTnum < m_methodInfo->EHcount; XTnum++, handlerTab++)
    {
        CORINFO_EH_CLAUSE clause;
        m_jitInfo->getEHinfo(methodHnd, XTnum, &clause);
        VASSERT(clause.HandlerLength != (unsigned)-1); // @DEPRECATED

        unsigned      tryBegOff = clause.TryOffset;
        unsigned      tryEndOff = tryBegOff + clause.TryLength;
        unsigned      hndBegOff = clause.HandlerOffset;
        unsigned      hndEndOff = hndBegOff + clause.HandlerLength;

        if  (tryEndOff > m_methodInfo->ILCodeSize)
            BADCODE3(MVER_E_TRY_GTEQ_END,
                     " at offset %04X",tryBegOff);
        if  (hndEndOff > m_methodInfo->ILCodeSize)
            BADCODE3(MVER_E_HNDEND_GT_CS,
                     " at offset %04X",tryBegOff);

        /* Convert the various addresses to basic blocks */

        handlerTab->tryBegOff = tryBegOff;
        handlerTab->tryEndOff = tryEndOff;
        handlerTab->hndBegOff = hndBegOff;
        handlerTab->tryBegOff = hndEndOff;

        if (clause.Flags & CORINFO_EH_CLAUSE_FILTER)
        {

        }
        else
        {
            handlerTab->ebdTyp = clause.ClassToken;

        }

        // Append the info to the table of try block handlers

        handlerTab->ebdFlags  = clause.Flags;

        handlerTab->ebdEnclosing = 0xFFFF;//NO_ENCLOSING_INDEX;


        for (EHblkDsc * xtab = m_compHndBBTab; xtab < handlerTab; xtab++)
        {
            // If we haven't recorded an enclosing index for xtab then see
            // if this EH regions should be recorded.  We check if the
            //  first or last offsets in the xtab lies within our region
            //
            if (xtab->ebdEnclosing == NO_ENCLOSING_INDEX)
            {
                bool begBetween = jitIsBetween(xtab->tryBegOff,
                                               tryBegOff, tryEndOff);
                bool endBetween = jitIsBetween(xtab->tryEndOff - 1,
                                               tryBegOff, tryEndOff);
                if (begBetween || endBetween)
                {
                    // Record the enclosing scope link
                    xtab->ebdEnclosing = (unsigned short) XTnum;

                    VASSERT(XTnum <  m_methodInfo->EHcount);

                    VASSERT(XTnum == (unsigned)(handlerTab - m_compHndBBTab));
                }
            }
        }

        verInsertEhNode(&clause, handlerTab);
    }

#ifndef DEBUG
    if (m_verificationNeeded)
#endif
    {
        // always run these checks for a debug build
        verCheckNestingLevel(initRoot);
    }
}

