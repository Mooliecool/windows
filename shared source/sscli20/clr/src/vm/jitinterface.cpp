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
// File: JITinterface.CPP
//
// ===========================================================================

#include "common.h"
#include "jitinterface.h"
#include "codeman.h"
#include "method.hpp"
#include "class.h"
#include "object.h"
#include "field.h"
#include "stublink.h"
#include "virtualcallstub.h"
#include "corjit.h"
#include "eeconfig.h"
#include "comstring.h"
#include "excep.h"
#include "log.h"
#include "excep.h"
#include "float.h"      // for isnan
#include "dbginterface.h"
#include "gcscan.h"
#include "security.h"
#include "securitydescriptor.h"   // to get security method attribute
#include "securitymeta.h"
#include "dllimport.h"
#include "ml.h"
#include "gc.h"
#include "comdelegate.h"
#include "jitperf.h" // to track jit perf
#include "corprof.h"
#include "eeprofinterfaces.h"
#include "remoting.h" // create context bound and remote class instances
#include "perfcounters.h"
#ifdef PROFILING_SUPPORTED
#include "proftoeeinterfaceimpl.h"
#endif
#include "tls.h"
#include "ecall.h"
#include "comobject.h"
#include "comstringbuffer.h"
#include "generics.h"
#include "typestring.h"
#include "stackprobe.h"
#include "typedesc.h"
#include "genericdict.h"
#include "array.h"
#include "debuginfostore.h"
#include "constrainedexecutionregion.h"
#include "security.h"
#include "security.inl"
#include "tokeniter.hpp"
#include "safemath.h"


#include "mdaassistantsptr.h"



#define JIT_LINKTIME_SECURITY


#define JIT_TO_EE_TRANSITION()          INSTALL_UNWIND_AND_CONTINUE_HANDLER;    \
                                        GCX_COOP();                             \
                                        COOPERATIVE_TRANSITION_BEGIN();         \
                                        START_NON_JIT_PERF();

#define EE_TO_JIT_TRANSITION()          STOP_NON_JIT_PERF();                    \
                                        COOPERATIVE_TRANSITION_END();           \
                                        UNINSTALL_UNWIND_AND_CONTINUE_HANDLER;

#define JIT_TO_EE_TRANSITION_LEAF()
#define EE_TO_JIT_TRANSITION_LEAF()

#ifdef DACCESS_COMPILE

// The real definitions are in jithelpers.cpp and i386\jitinterfacex86.cpp.
// However, those files are not included in the DAC build.
// Hence, we add them here.
GARY_IMPL(VMHELPDEF, hlpDynamicFuncTable, DYNAMIC_CORINFO_HELP_COUNT);
#ifdef _X86_
GVAL_IMPL(JIT_Writeable_Thunks, JIT_Writeable_Thunks_Buf);
#endif // _X86_

#else // DACCESS_COMPILE

/*********************************************************************/


#if defined(ENABLE_PERF_COUNTERS)
LARGE_INTEGER g_lastTimeInJitCompilation;
#endif

/*********************************************************************/

inline CORINFO_MODULE_HANDLE GetScopeHandle(MethodDesc* method) 
{
    LEAF_CONTRACT;
    if (method->IsDynamicMethod())
    {
        return MakeDynamicScope(method->GetDynamicMethodDesc()->GetResolver());
    }
    else
    {
        return GetScopeHandle(method->GetModule());
    }
}

// Given a field or method token metaTOK return return its parent token
unsigned GetMemberParent(Module * module, unsigned metaTOK)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;


    if (TypeFromToken(metaTOK) == mdtMethodSpec)
    {
      PCCOR_SIGNATURE pSig;
      ULONG cSig;
      mdMemberRef GenericMemberRef;
      module->GetMDImport()->GetMethodSpecProps(metaTOK, &GenericMemberRef, &pSig, &cSig);
      _ASSERTE(TypeFromToken(GenericMemberRef) == mdtMethodDef || TypeFromToken(GenericMemberRef) == mdtMemberRef);
      metaTOK = GenericMemberRef;
    }


    // Extended to extract parent class of a member-ref, method-def or field-def

    if (TypeFromToken(metaTOK) == mdtMemberRef)
    {
      metaTOK = module->GetMDImport()->GetParentOfMemberRef(metaTOK);
      // For varargs, a memberref can point to a methodDef, so we
      if(TypeFromToken(metaTOK) == mdtMethodDef)
          module->GetMDImport()->GetParentToken(metaTOK, &metaTOK);

    }
    else if (TypeFromToken(metaTOK) == mdtMethodDef || TypeFromToken(metaTOK) == mdtFieldDef)
    {
      module->GetMDImport()->GetParentToken(metaTOK,&metaTOK);
    }

    _ASSERTE(TypeFromToken(metaTOK) == mdtTypeDef  ||
             TypeFromToken(metaTOK) == mdtTypeRef  ||
             TypeFromToken(metaTOK) == mdtTypeSpec ||
             TypeFromToken(metaTOK) == mdtModuleRef  );

    return metaTOK;
}


IEEMemoryManager* CEEJitInfo::getMemoryManager()
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    IEEMemoryManager* result = NULL;

    JIT_TO_EE_TRANSITION();

    result = GetEEMemoryManager();

    EE_TO_JIT_TRANSITION();

    return result;
}

/*****************************************************************************/

// Initialize from data we passed across to the JIT
inline static void GetTypeContext(const CORINFO_SIG_INST *info, SigTypeContext *pTypeContext)
{
    LEAF_CONTRACT;
    SigTypeContext::InitTypeContext(info->classInstCount,(TypeHandle *) info->classInst, info->methInstCount, (TypeHandle *) info->methInst, pTypeContext);
}

static MethodDesc* GetMethodFromContext(CORINFO_CONTEXT_HANDLE context)
{
    LEAF_CONTRACT;
    if (((size_t) context & CORINFO_CONTEXTFLAGS_MASK) == CORINFO_CONTEXTFLAGS_CLASS)
    {
        return NULL;
    }
    else
    {
        return GetMethod((CORINFO_METHOD_HANDLE)((size_t) context & ~CORINFO_CONTEXTFLAGS_MASK));
    }
}

static TypeHandle GetTypeFromContext(CORINFO_CONTEXT_HANDLE context)
{
    LEAF_CONTRACT;
    if (((size_t) context & CORINFO_CONTEXTFLAGS_MASK) == CORINFO_CONTEXTFLAGS_CLASS)
    {
        return TypeHandle((CORINFO_CLASS_HANDLE) ((size_t) context & ~CORINFO_CONTEXTFLAGS_MASK));
    }
    else
    {
        MethodTable * pMT = GetMethodFromContext(context)->GetMethodTable();
        return TypeHandle(pMT);
    }
}

// Initialize from a context parameter passed to the JIT and back.  This is a parameter
// that indicates which method is being jitted.

inline static void GetTypeContext(CORINFO_CONTEXT_HANDLE context, SigTypeContext *pTypeContext)
{
    WRAPPER_CONTRACT;
    if (context == NULL)
    {
        SigTypeContext::InitTypeContext(pTypeContext);
    }
    if (GetMethodFromContext(context))
    {
        SigTypeContext::InitTypeContext(GetMethodFromContext(context), pTypeContext);
    }
    else
    {
        SigTypeContext::InitTypeContext(GetTypeFromContext(context), pTypeContext);
    }
}

inline static Assembly * GetAssemblyFromContext(CORINFO_CONTEXT_HANDLE context)
{
    LEAF_CONTRACT;
    if (((size_t) context & CORINFO_CONTEXTFLAGS_MASK) == CORINFO_CONTEXTFLAGS_CLASS)
    {
        TypeHandle th = TypeHandle((CORINFO_CLASS_HANDLE) ((size_t) context & ~CORINFO_CONTEXTFLAGS_MASK));
        return th.GetAssembly();
    }
    else
    {
        MethodDesc * pMD = GetMethod((CORINFO_METHOD_HANDLE)((size_t) context & ~CORINFO_CONTEXTFLAGS_MASK));
        return pMD->GetAssembly();
    }
}

static BOOL ContextIsShared(CORINFO_CONTEXT_HANDLE context)
{
    LEAF_CONTRACT;
    if (GetMethodFromContext(context))
    {
        MethodDesc *pContextMD = GetMethodFromContext(context);
        if (pContextMD->IsSharedByGenericInstantiations())
            return TRUE;
#ifdef _DEBUG
        // We should never be generating code for a Canonical Subtype
        TypeHandle typeHnd = GetTypeFromContext(context);
        _ASSERTE(!typeHnd.IsCanonicalSubtype() ||
                 (typeHnd.AsMethodTable() == g_pHiddenMethodTableClass));
#endif // _DEBUG

        return FALSE;
    }
    else
    {
        // Type handle contexts are non-shared and are used for inlining of
        // non-generic methods in generic classes
        return FALSE;
    }
}

/*********************************************************************/
// This normalizes EE type information into the form expected by the JIT.
//
// If typeHnd contains exact type information, then *clsRet will contain
// the normalized CORINFO_CLASS_HANDLE information on return.

inline static CorInfoType toJitType(CorElementType eeType,
                                    TypeHandle typeHnd = TypeHandle() /* optional in */,
                                    CORINFO_CLASS_HANDLE *clsRet = NULL /* optional out */ ) {

    CONTRACT(CorInfoType) {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION((eeType == ELEMENT_TYPE_VAR || eeType == ELEMENT_TYPE_MVAR) == 
                     (!typeHnd.IsNull() && typeHnd.IsGenericVariable()));
        PRECONDITION(eeType != ELEMENT_TYPE_GENERICINST);
    } CONTRACT_END;

    TypeHandle typeHndUpdated = typeHnd;

    if (!typeHnd.IsNull())
    {
        CorElementType normType = typeHnd.GetInternalCorElementType();
        // If we have a type handle, then it has the better type
        // in some cases
        if (eeType == ELEMENT_TYPE_VALUETYPE && !CorTypeInfo::IsObjRef(normType))
            eeType = normType;

        // Zap the typeHnd when the type _really_ is a primitive
        // as far as verification is concerned. Returning a null class
        // handle means it is is a primitive.
        //
        // Enums are exactly like primitives, even from a verification standpoint,
        // so we zap the type handle in this case.
        //
        // However RuntimeTypeHandle etc. are reported as E_T_INT (or something like that)
        // but don't count as primitives as far as verification is concerned...
        //
        // To make things stranger, TypedReference returns true for "IsTruePrimitive".
        // However the JIT likes us to report the type handle in that case.
        if ((typeHnd.IsUnsharedMT() &&
             typeHnd.GetMethodTable()->IsTruePrimitive() &&
             typeHnd.GetSignatureCorElementType() != ELEMENT_TYPE_TYPEDBYREF)
            || typeHnd.IsEnum())
        {
            typeHndUpdated = TypeHandle();
        }

    }

    static const BYTE map[] = {
        CORINFO_TYPE_UNDEF,
        CORINFO_TYPE_VOID,
        CORINFO_TYPE_BOOL,
        CORINFO_TYPE_CHAR,
        CORINFO_TYPE_BYTE,
        CORINFO_TYPE_UBYTE,
        CORINFO_TYPE_SHORT,
        CORINFO_TYPE_USHORT,
        CORINFO_TYPE_INT,
        CORINFO_TYPE_UINT,
        CORINFO_TYPE_LONG,
        CORINFO_TYPE_ULONG,
        CORINFO_TYPE_FLOAT,
        CORINFO_TYPE_DOUBLE,
        CORINFO_TYPE_STRING,
        CORINFO_TYPE_PTR,            // PTR
        CORINFO_TYPE_BYREF,
        CORINFO_TYPE_VALUECLASS,
        CORINFO_TYPE_CLASS,
        CORINFO_TYPE_VAR,            // VAR (type variable)
        CORINFO_TYPE_CLASS,          // ARRAY
        CORINFO_TYPE_CLASS,          // WITH
        CORINFO_TYPE_REFANY,
        CORINFO_TYPE_VALUECLASS,     // VALUEARRAY
        CORINFO_TYPE_NATIVEINT,      // I
        CORINFO_TYPE_NATIVEUINT,     // U
        CORINFO_TYPE_DOUBLE,         // R

        // put the correct type when we know our implementation
        CORINFO_TYPE_PTR,            // FNPTR
        CORINFO_TYPE_CLASS,          // OBJECT
        CORINFO_TYPE_CLASS,          // SZARRAY
        CORINFO_TYPE_VAR,            // MVAR

        CORINFO_TYPE_UNDEF,          // CMOD_REQD
        CORINFO_TYPE_UNDEF,          // CMOD_OPT
        CORINFO_TYPE_UNDEF,          // INTERNAL
        };

    _ASSERTE(sizeof(map) == ELEMENT_TYPE_MAX);
    _ASSERTE(eeType < (CorElementType) sizeof(map));
        // spot check of the map
    _ASSERTE((CorInfoType) map[ELEMENT_TYPE_I4] == CORINFO_TYPE_INT);
    _ASSERTE((CorInfoType) map[ELEMENT_TYPE_VALUEARRAY] == CORINFO_TYPE_VALUECLASS);
    _ASSERTE((CorInfoType) map[ELEMENT_TYPE_PTR] == CORINFO_TYPE_PTR);
    _ASSERTE((CorInfoType) map[ELEMENT_TYPE_TYPEDBYREF] == CORINFO_TYPE_REFANY);
    _ASSERTE((CorInfoType) map[ELEMENT_TYPE_R] == CORINFO_TYPE_DOUBLE);

    CorInfoType res = ((CorInfoType) map[eeType]);

    if (clsRet)
        *clsRet = CORINFO_CLASS_HANDLE(typeHndUpdated.AsPtr());

    RETURN res;
}


inline static CorInfoType toJitType(TypeHandle typeHnd, CORINFO_CLASS_HANDLE *clsRet = NULL)
{
    WRAPPER_CONTRACT;
    return toJitType(typeHnd.GetInternalCorElementType(), typeHnd, clsRet);
}

// Active dependency helpers

void __stdcall CEEInfo::addActiveDependency(CORINFO_MODULE_HANDLE moduleFrom,CORINFO_MODULE_HANDLE moduleTo)
{
    CONTRACTL
    {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(moduleTo));
    }
    CONTRACTL_END;

    JIT_TO_EE_TRANSITION_LEAF();

    // Do nothing at this level - EE's implementation is in CEEJitInfo

    EE_TO_JIT_TRANSITION_LEAF();
}


void __stdcall CEEJitInfo::addActiveDependency(CORINFO_MODULE_HANDLE moduleFrom,CORINFO_MODULE_HANDLE moduleTo)
{
    CONTRACTL
    {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(moduleTo));
    }
    CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    Module *dependency = GetModule(moduleTo);
    if (!dependency->IsSystem())
    {
        if (m_FD->IsDynamicMethod())
        {
            // The context module of the m_FD is irrelevant.  Rather than tracking
            // the dependency, we just do immediate activation.

            dependency->EnsureActive();
        }
        else
        {
            Module *context = moduleFrom?GetModule(moduleFrom):m_FD->GetModule();

            // Record active dependency for loader.

            if (context != dependency)
                context->AddActiveDependency(dependency, FALSE);
        }
    }

    EE_TO_JIT_TRANSITION();
}



// Wrapper around CEEInfo::GetProfilingHandle.  The first time this is called for a
// method desc, it calls through to CEEInfo::GetProfilingHandle and caches the
// result in CEEJitInfo::GetProfilingHandleCache.  Thereafter, this wrapper regurgitates the cached values
// rather than calling into CEEInfo::GetProfilingHandle each time.  This avoids
// making duplicate calls into the profiler's FunctionIDMapper callback.
void __stdcall CEEJitInfo::GetProfilingHandle(CORINFO_METHOD_HANDLE method,
                                           BOOL                      *pbHookFunction,
                                           void                     **pEEHandle,
                                           void                     **pProfilerHandle,
                                           BOOL                      *pbIndirectedHandles)
{
    WRAPPER_CONTRACT;
    
    _ASSERTE(pbHookFunction != NULL);
    _ASSERTE(pProfilerHandle != NULL);
    _ASSERTE(pbIndirectedHandles != NULL);
    
    if (!m_gphCache.m_bGphIsCacheValid)
    {
        // Cache not filled in, so make our first and only call to CEEInfo::GetProfilingHandle here
        
        // NOTE: Since pbHookFunction & pbIndirectedHandles aren't NULL, it's safe to use them as our
        // [out] params (can't use &m_gphCache.m_bGphHookFunction
        // or &m_gphCache.m_bGphIndirectHandles directly, as they're bitfield bools).
        CEEInfo::GetProfilingHandle(
            method, 
            pbHookFunction,               
            &m_gphCache.m_pvGphEEHandle,
            &m_gphCache.m_pvGphProfilerHandle,
            pbIndirectedHandles
            );
        
        m_gphCache.m_bGphHookFunction = (*pbHookFunction != 0);
        m_gphCache.m_bGphIndirectedHandles = (*pbIndirectedHandles != 0);
        m_gphCache.m_bGphIsCacheValid = true;
    }
    else
    {
        // Cache was already filled in.
        
        // Our cache of these values are bitfield bools, but the interface requires
        // BOOL.  So to avoid setting aside a staging area on the stack for these
        // values, we filled them in directly in the if (not cached yet) case.
        *pbHookFunction = (m_gphCache.m_bGphHookFunction != false);
        *pbIndirectedHandles = (m_gphCache.m_bGphIndirectedHandles != false);
    }

    // At this point, the remaining values must be in the cache by now, so use them
    
    if (pEEHandle != NULL)
    {
        *pEEHandle = m_gphCache.m_pvGphEEHandle;
    }
    
    *pProfilerHandle = m_gphCache.m_pvGphProfilerHandle;
}


/*********************************************************************/
//@GENERICS:
// The method handle is used to instantiate method and class type parameters
// It's also used to determine whether an extra dictionary parameter is required
//
// sig      - Input metadata signature
// scopeHnd - The signature is to be interpreted in the context of this scope (module)
// token    - Metadata token used to refer to the signature (may be mdTokenNil for dynamic methods)
// sigRet   - Resulting output signature in a format that is understood by native compilers
// context  - The method accessing the signature (may be NULL for dynamic methods)
// localSig - Is it a local variables declaration, or a method signature (with return type, etc).
// owner    - type of the method accessing the signature (for qualifying generics access)
//
/*********************************************************************/
/* static */
void CEEInfo::ConvToJitSig(PCCOR_SIGNATURE sig,
                              CORINFO_MODULE_HANDLE scopeHnd,
                              mdToken token,
                              CORINFO_SIG_INFO* sigRet,
                              CORINFO_METHOD_HANDLE context,
                              bool localSig,
                              TypeHandle owner)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    SigTypeContext typeContext;

    if (context)
    {
        MethodDesc* pAccessingMD = GetMethod(context);
        // Restore method (method table + instantiation of generic method)
        pAccessingMD->CheckRestore();
        SigTypeContext::InitTypeContext(pAccessingMD, owner, &typeContext);
    }

    SigPointer ptr(sig);
    _ASSERTE(CORINFO_CALLCONV_DEFAULT == (CorInfoCallConv) IMAGE_CEE_CS_CALLCONV_DEFAULT);
    _ASSERTE(CORINFO_CALLCONV_VARARG == (CorInfoCallConv) IMAGE_CEE_CS_CALLCONV_VARARG);
    _ASSERTE(CORINFO_CALLCONV_MASK == (CorInfoCallConv) IMAGE_CEE_CS_CALLCONV_MASK);
    _ASSERTE(CORINFO_CALLCONV_HASTHIS == (CorInfoCallConv) IMAGE_CEE_CS_CALLCONV_HASTHIS);

    TypeHandle typeHnd = TypeHandle();

    sigRet->sig = (CORINFO_SIG_HANDLE) sig;
    sigRet->retTypeClass = 0;
    sigRet->retTypeSigClass = 0;
    sigRet->scope = scopeHnd;
    sigRet->token = token;
    sigRet->sigInst.classInst = (CORINFO_CLASS_HANDLE *) typeContext.m_classInst;
    sigRet->sigInst.classInstCount = (unsigned) typeContext.m_classInstCount;
    sigRet->sigInst.methInst = (CORINFO_CLASS_HANDLE *) typeContext.m_methInst;
    sigRet->sigInst.methInstCount = (unsigned) typeContext.m_methInstCount;

    if (!localSig)
    {
        // This is a method signature which includes calling convention, return type, 
        // arguments, etc

        _ASSERTE(sig != 0);
        Module* module = GetModule(scopeHnd);
        sigRet->flags = 0;

        ULONG data;
        IfFailThrow(ptr.GetCallingConvInfo(&data));
        sigRet->callConv = (CorInfoCallConv) data;
        // Skip number of type arguments
        if (sigRet->callConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
          IfFailThrow(ptr.GetData(NULL));

        ULONG numArgs;
        IfFailThrow(ptr.GetData(&numArgs));
        if (numArgs != (unsigned short) numArgs)
            COMPlusThrowHR(COR_E_INVALIDPROGRAM );

        sigRet->numArgs = (unsigned short) numArgs;

        CorElementType type = ptr.PeekElemTypeClosed(&typeContext);

        if (!CorTypeInfo::IsPrimitiveType(type))
        {
            typeHnd = ptr.GetTypeHandleThrowing(module, &typeContext);
            _ASSERTE(!typeHnd.IsNull());

            // I believe it doesn't make any diff. if this is
            // GetInternalCorElementType or GetSignatureCorElementType
            type = typeHnd.GetSignatureCorElementType();

        }
        sigRet->retType = toJitType(type, typeHnd, &sigRet->retTypeClass);
        sigRet->retTypeSigClass = CORINFO_CLASS_HANDLE(typeHnd.AsPtr());

        ptr.SkipExactlyOne();     // must to a skip so we skip any class tokens associated with the return type
        _ASSERTE(sigRet->retType < CORINFO_TYPE_COUNT);

        sigRet->args = (CORINFO_ARG_LIST_HANDLE)ptr.GetPtr();

    } 
    else
    {
        // This is local variables declaration

        sigRet->callConv = CORINFO_CALLCONV_DEFAULT;
        sigRet->retType = CORINFO_TYPE_VOID;
        sigRet->flags   = CORINFO_SIGFLAG_IS_LOCAL_SIG;
        sigRet->numArgs = 0;
        if (sig != 0) {
            if (*sig != IMAGE_CEE_CS_CALLCONV_LOCAL_SIG)
                COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_CALLCONV_NOT_LOCAL_SIG);
            IfFailThrow(ptr.GetData(NULL));

            ULONG numArgs;
            IfFailThrow(ptr.GetData(&numArgs));
            
            if (numArgs != (unsigned short) numArgs)
                COMPlusThrowHR(COR_E_INVALIDPROGRAM );

            sigRet->numArgs = (unsigned short) numArgs;
        }

        sigRet->args = (CORINFO_ARG_LIST_HANDLE)ptr.GetPtr();
    }

    _ASSERTE(SigInfoFlagsAreValid(sigRet));
}

/*********************************************************************/
CORINFO_CLASS_HANDLE __stdcall CEEInfo::getTokenTypeAsHandle (CORINFO_MODULE_HANDLE  scopeHnd, unsigned metaTOK)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE tokenType = NULL;

    JIT_TO_EE_TRANSITION();

    mdToken     tokType = TypeFromToken(metaTOK);
    InitStaticTypeHandles();

    switch (tokType)
    {
    case mdtTypeRef:
    case mdtTypeDef:
    case mdtTypeSpec:
        tokenType = CORINFO_CLASS_HANDLE(s_pRuntimeTypeHandle);
        break;

    case mdtMethodDef:
    case mdtMethodSpec:
        tokenType = CORINFO_CLASS_HANDLE(s_pRuntimeMethodHandle);
        break;

    case mdtMemberRef:
        {
            // OK, we have to look at the metadata to see if it's a field or method
            _ASSERTE(!IsDynamicScope(scopeHnd));

            Module *pModule = (Module*)scopeHnd;

            PCCOR_SIGNATURE pSig;
            ULONG cSig;
            pModule->GetMDImport()->GetNameAndSigOfMemberRef(metaTOK, &pSig, &cSig);

            if (isCallConv(MetaSig::GetCallingConventionInfo(pModule, pSig), IMAGE_CEE_CS_CALLCONV_FIELD))
            {
                tokenType = CORINFO_CLASS_HANDLE(s_pRuntimeFieldHandle);
            }
            else
            {
                tokenType = CORINFO_CLASS_HANDLE(s_pRuntimeMethodHandle);
            }
        }
        break;

    case mdtFieldDef:
        tokenType = CORINFO_CLASS_HANDLE(s_pRuntimeFieldHandle);
        break;

    default:
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_TOKEN);
    }

    EE_TO_JIT_TRANSITION();

    return tokenType;
}

/*********************************************************************/
size_t __stdcall CEEInfo::findNameOfToken (
            CORINFO_MODULE_HANDLE       scopeHnd,
            mdToken                     metaTOK,
            __out_ecount (FQNameCapacity)  char * szFQName,
            size_t FQNameCapacity)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    size_t NameLen = 0;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicScope(scopeHnd))
    {
        NameLen = CEEDynamicCodeInfo::findNameOfToken(scopeHnd, metaTOK, szFQName, FQNameCapacity);
    }
    else
    {
        Module* module = GetModule(scopeHnd);
        NameLen = findNameOfToken(module, metaTOK, szFQName, FQNameCapacity);
    }
        
    EE_TO_JIT_TRANSITION();

    return NameLen;
}

CorInfoCanSkipVerificationResult __stdcall CEEInfo::canSkipMethodVerification(CORINFO_METHOD_HANDLE ftnHnd, BOOL fQuickCheckOnly)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoCanSkipVerificationResult canSkipVerif = CORINFO_VERIFICATION_CANNOT_SKIP;

    JIT_TO_EE_TRANSITION();

    MethodDesc* pMD = GetMethod(ftnHnd);


#ifdef _DEBUG
    if (g_pConfig->IsVerifierOff())
    {
        canSkipVerif = CORINFO_VERIFICATION_CAN_SKIP;
    }
    else
#endif // _DEBUG
    {
        //
        // fQuickCheckOnly is set only by calls from Zapper::CompileAssembly
        // because that allows us make a determination for the most
        // common full trust scenarios (local machine) without actually
        // resolving policy and bringing in a whole list of assembly
        // dependencies.  Also, quick checks don't call
        // OnLinktimeCanSkipVerificationCheck which means we don't add
        // permission sets to the persisted ngen module.
        //
        // The scenario of interest here is determing whether or not an
        // assembly MVID comparison is enough when loading an NGEN'd
        // assembly or if a full binary hash comparison must be done.
        //
        
        canSkipVerif = Security::JITCanSkipVerification(pMD, fQuickCheckOnly);
    }

    EE_TO_JIT_TRANSITION();

    return canSkipVerif;

}

/*********************************************************************/
CorInfoCanSkipVerificationResult __stdcall CEEInfo::canSkipVerification(
        CORINFO_MODULE_HANDLE moduleHnd, 
        BOOL fQuickCheckOnlyWithoutCommit)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoCanSkipVerificationResult canSkipVerif = CORINFO_VERIFICATION_CANNOT_SKIP;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicScope(moduleHnd)) 
    {
        moduleHnd = GetScopeHandle(GetDynamicResolver(moduleHnd)->GetDynamicMethod()->GetModule());
    }

    Assembly * pAssem = GetModule(moduleHnd)->GetAssembly();

#ifdef _DEBUG
    if (g_pConfig->IsVerifierOff())
    {
        canSkipVerif = CORINFO_VERIFICATION_CAN_SKIP;
    }
    else
#endif // _DEBUG
    {
        //
        // fQuickCheckOnly is set only by calls from Zapper::CompileAssembly
        // because that allows us make a determination for the most
        // common full trust scenarios (local machine) without actually
        // resolving policy and bringing in a whole list of assembly
        // dependencies.  Also, quick checks don't call
        // OnLinktimeCanSkipVerificationCheck which means we don't add
        // permission sets to the persisted ngen module.
        //
        // The scenario of interest here is determing whether or not an
        // assembly MVID comparison is enough when loading an NGEN'd
        // assembly or if a full binary hash comparison must be done.
        //

        DomainAssembly * pAssembly = pAssem->GetDomainAssembly();
        canSkipVerif = Security::JITCanSkipVerification(pAssembly, fQuickCheckOnlyWithoutCommit);
    }

    EE_TO_JIT_TRANSITION();

    return canSkipVerif;
}

/*********************************************************************/
// Checks if the given metadata token is valid
BOOL __stdcall CEEInfo::isValidToken (
        CORINFO_MODULE_HANDLE       module,
        mdToken                     metaTOK)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicScope(module))
    {
        result = CEEDynamicCodeInfo::isValidToken(module, metaTOK);
    }
    else
    {
        result = GetModule(module)->GetMDImport()->IsValidToken(metaTOK);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Checks if the given metadata token is valid StringRef
BOOL __stdcall CEEInfo::isValidStringRef (
        CORINFO_MODULE_HANDLE       module,
        mdToken                     metaTOK)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicScope(module))
    {
        result = CEEDynamicCodeInfo::isValidStringRef(module, metaTOK);
    }
    else
    {
        result = GetModule(module)->CheckStringRef(metaTOK);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/* static */
size_t __stdcall CEEInfo::findNameOfToken (Module* module,
                                                 mdToken metaTOK,
                                                 __out_ecount (FQNameCapacity) char * szFQName,
                                                 size_t FQNameCapacity)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

#ifdef _DEBUG
    PCCOR_SIGNATURE sig = NULL;
    DWORD           cSig;
    LPCUTF8         pszNamespace = NULL;
    LPCUTF8         pszClassName = NULL;
    
    mdToken tokType = TypeFromToken(metaTOK);
    switch(tokType)
    {
        case mdtTypeRef:
            module->GetMDImport()->GetNameOfTypeRef(metaTOK, &pszNamespace, &pszClassName);
            ns::MakePath(szFQName, (int)FQNameCapacity, pszNamespace, pszClassName);
            break;
        case mdtTypeDef:
            module->GetMDImport()->GetNameOfTypeDef(metaTOK, &pszNamespace, &pszClassName);
            ns::MakePath(szFQName, (int)FQNameCapacity, pszNamespace, pszClassName);
            break;
        case mdtFieldDef:
            strncpy_s(szFQName,  FQNameCapacity,  (char*) module->GetMDImport()->GetNameOfFieldDef(metaTOK), 
                                FQNameCapacity - 1);
            break;
        case mdtMethodDef:
            strncpy_s(szFQName, FQNameCapacity, (char*) module->GetMDImport()->GetNameOfMethodDef(metaTOK), 
                                FQNameCapacity - 1);
            break;
        case mdtMemberRef:
            strncpy_s(szFQName, FQNameCapacity, (char *) module->GetMDImport()->GetNameAndSigOfMemberRef((mdMemberRef)metaTOK, &sig, &cSig), 
                                FQNameCapacity - 1);
            break;
        default:
            sprintf_s(szFQName, FQNameCapacity, "!TK_%x", metaTOK);
            break;
    }

#else // !_DEBUG
    strncpy_s (szFQName, FQNameCapacity, "<UNKNOWN>", FQNameCapacity - 1);
#endif // _DEBUG


    return strlen (szFQName);
}

bool __stdcall CEEInfo::canEmbedModuleHandleForHelper(CORINFO_MODULE_HANDLE handle)
{ 
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    bool result = false;

    JIT_TO_EE_TRANSITION();

    result = !IsDynamicScope(handle);

    EE_TO_JIT_TRANSITION();

    return result;
}


CHECK CheckContext(CORINFO_MODULE_HANDLE scopeHnd, CORINFO_CONTEXT_HANDLE context)
{
    CHECK_MSG(scopeHnd != NULL, "Illegal null scope");
    CHECK_MSG(((size_t) context & ~CORINFO_CONTEXTFLAGS_MASK) != NULL, "Illegal null context");
    if (((size_t) context & CORINFO_CONTEXTFLAGS_MASK) == CORINFO_CONTEXTFLAGS_CLASS)
    {
        TypeHandle handle((CORINFO_CLASS_HANDLE) ((size_t) context & ~CORINFO_CONTEXTFLAGS_MASK));
        CHECK_MSG(handle.GetModule() == GetModule(scopeHnd), "Inconsistent scope and context");
    }
    else
    {
        MethodDesc* handle = (MethodDesc*) ((size_t) context & ~CORINFO_CONTEXTFLAGS_MASK);
        CHECK_MSG(handle->GetModule() == GetModule(scopeHnd), "Inconsistent scope and context");
    }

    CHECK_OK;
}

/*********************************************************************/
CORINFO_CLASS_HANDLE __stdcall CEEInfo::findClass (
                            CORINFO_MODULE_HANDLE  scopeHnd,
                            unsigned               annotatedMetaTOK,
                            CORINFO_CONTEXT_HANDLE context,
                            CorInfoTokenKind       tokenKind /* = CORINFO_TOKENKIND_Default */)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE ret = 0;

    JIT_TO_EE_TRANSITION();

    CONSISTENCY_CHECK(CheckContext(scopeHnd, context));

    ret = findClassInternal(scopeHnd, annotatedMetaTOK, context, tokenKind);

    CheckTokenKind(ret, tokenKind);

    EE_TO_JIT_TRANSITION();

    return ret;
}

// Internal version of findClass - can return byrefs
CORINFO_CLASS_HANDLE CEEInfo::findClassInternal(
                         CORINFO_MODULE_HANDLE  scopeHnd,
                         unsigned               annotatedMetaTOK,
                         CORINFO_CONTEXT_HANDLE context,
                         CorInfoTokenKind       tokenKind)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckContext(scopeHnd, context));
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE ret = NULL;

    unsigned metaTOK = annotatedMetaTOK & ~CORINFO_ANNOT_PERMITUNINSTDEFORREF;
    
    if (TypeFromToken(metaTOK) != mdtTypeDef &&
        TypeFromToken(metaTOK) != mdtTypeRef &&
        TypeFromToken(metaTOK) != mdtTypeSpec &&
        TypeFromToken(metaTOK) != mdtModuleRef)
    {
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_CLASS_TOKEN);
    }

    OBJECTREF throwable = NULL;
    GCPROTECT_BEGIN(throwable);

    if (IsDynamicScope(scopeHnd))
    {
        ret = CEEDynamicCodeInfo::findClassInternal(scopeHnd,
                                                    metaTOK, 
                                                    context, 
                                                    annotatedMetaTOK & CORINFO_ANNOT_PERMITUNINSTDEFORREF, 
                                                    &throwable);
    }
    else
    {
        Module* module = GetModule(scopeHnd);
        SigTypeContext typeContext;
        GetTypeContext(context, &typeContext);
    
        TypeHandle clsHnd;
        if (TypeFromToken(metaTOK) == mdtModuleRef)
        {
            DomainFile *pModule = module->LoadModule(GetAppDomain(), metaTOK, FALSE);
            if (pModule != NULL)
                clsHnd = TypeHandle(pModule->GetModule()->GetGlobalMethodTable());
        }
        else
        {
            ClassLoader::LoadTypeDefOrRefOrSpecNoThrow(module,
                                                       metaTOK, 
                                                       &typeContext, 
                                                       &clsHnd, 
                                                       &throwable,
                                                       ClassLoader::ThrowIfNotFound,
                                                       (annotatedMetaTOK & CORINFO_ANNOT_PERMITUNINSTDEFORREF) != 0 ?
                                                         ClassLoader::PermitUninstDefOrRef : ClassLoader::FailIfUninstDefOrRef);
        };
    
        ret = CORINFO_CLASS_HANDLE(clsHnd.AsPtr());
    }

#ifdef JIT_LINKTIME_SECURITY


    if (throwable != NULL)
        COMPlusThrow(throwable);
#endif // JIT_LINKTIME_SECURITY

    if (ret == NULL)
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

    if (tokenKind != CORINFO_TOKENKIND_Ldtoken)
    {
        MethodDesc * pAccessingMD = GetMethodFromContext(context);
        if (pAccessingMD != NULL && pAccessingMD->IsTypicalMethodDefinition())
        {
            if (!canAccessTypeInternal(context, ret, FALSE))
            {
                COMPlusThrowHR(COR_E_TYPELOAD);
            }
        }
    }

    GCPROTECT_END();

    return ret;
}


// Given a field or method token metaTOK return its parent token
unsigned __stdcall CEEInfo::getMemberParent(CORINFO_MODULE_HANDLE scopeHnd, unsigned metaTOK)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicScope(scopeHnd))
    {
        result = CEEDynamicCodeInfo::getMemberParent(scopeHnd, metaTOK);
    }
    else
    {
        Module* module = GetModule(scopeHnd);
        result = GetMemberParent(module, metaTOK);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}


/*********************************************************************/
CORINFO_FIELD_HANDLE CEEInfo::findFieldInternal (
                                           CORINFO_MODULE_HANDLE   scopeHnd,
                                           unsigned                metaTOK,
                                           CORINFO_CONTEXT_HANDLE  context,
                                           CorInfoTokenKind tokenKind)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_FIELD_HANDLE fieldHnd = 0;

    CONSISTENCY_CHECK(CheckContext(scopeHnd, context));

    if (TypeFromToken(metaTOK) != mdtFieldDef &&
        TypeFromToken(metaTOK) != mdtMemberRef)
    {
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_FIELD_TOKEN);
    }

    MethodDesc* method = GetMethodFromContext(context);
    MethodTable *pMethodMTOwner = (method) ? method->GetMethodTable() : NULL;
    FieldDesc* fieldDesc = NULL;
    OBJECTREF throwable = NULL;
    DWORD visibilityCheck = (tokenKind == CORINFO_TOKENKIND_Ldtoken) ? FALSE : TRUE; // default to TRUE
    
    GCPROTECT_BEGIN(throwable);
    
    SigTypeContext typeContext;
    GetTypeContext(context, &typeContext);

    // Token resolution will happen differently for DynamicMethodDesc and "normal" methods
    // DynamicMethodDesc may skip some visibility checks or pretend to live 
    // on a type they are not in.
    // Here is the info set by the if/else block
    // fieldDesc = the FieldDesc requested (defined by the token)
    // pMethodMTOwner = the type to use to make the visibility check
    // throwable = if an exception needs to be thrown
    // visibilityCheck = if visibility checks need to happen
    if (IsDynamicScope(scopeHnd))
    {
        DWORD dwSecurityControlFlags = 0;
        fieldDesc = CEEDynamicCodeInfo::findField(scopeHnd,
                                                  metaTOK, 
                                                  &typeContext, 
                                                  &pMethodMTOwner, 
                                                  &dwSecurityControlFlags, 
                                                  &throwable);

        if (dwSecurityControlFlags & DynamicResolver::SkipVisibilityChecks)
        {
            visibilityCheck = FALSE;
        }
    }
    else
    {
        HRESULT res = MemberLoader::GetFieldDescFromMemberRefNT(GetModule(scopeHnd), 
                                                                (mdMemberRef) metaTOK, 
                                                                &fieldDesc, 
                                                                &throwable, 
                                                                &typeContext);
        if (FAILED(res))
        {
            fieldDesc = NULL;
        }
        else  if (fieldDesc->IsStatic()) 
        {
            ScanToken(GetModule(scopeHnd), getMemberParent(scopeHnd, metaTOK), context, TRUE);
        }
    }
    
    //
    // perform common verification checks
    //
    if (fieldDesc)
    {
        if (method != NULL && method->IsTypicalMethodDefinition())
        {
            TypeHandle parentHnd;
            if (fieldDesc->IsSharedByGenericInstantiations())
            {
                // fieldDesc may be shared, so we must use the parent token
                parentHnd = TypeHandle(findClass(scopeHnd, getMemberParent(scopeHnd, metaTOK), context));
            }
            else
            {
                // fieldDesc is not shared, so we can use GetEnclosingMethodTable()
                parentHnd = TypeHandle(fieldDesc->GetEnclosingMethodTable());
            }

            // Check private/protected etc. access
            if (parentHnd.IsNull() || (visibilityCheck &&
                (ClassLoader::CanAccess(method, 
                                       pMethodMTOwner,
                                       pMethodMTOwner->GetAssembly(),
                                       parentHnd.GetMethodTable(),
                                       parentHnd.GetMethodTable()->GetAssembly(),
                                       fieldDesc->GetFieldProtection(),
                                       NULL,
                                       fieldDesc) == FALSE)))
            {
                EEFieldException e(fieldDesc);
                throwable = e.GetThrowable();

                fieldDesc = NULL;
            }

            if (fieldDesc != NULL && fieldDesc->IsNonVerifiableExplicitField() && 
                !(Security::CanAccessNonVerifiableExplicitField(method) && !isVerifyOnly()))
            {
                EEMessageException e(kVerificationException, VER_E_FIELD_OVERLAP);
                throwable = e.GetThrowable();

                fieldDesc = NULL;
            }
        }
    }

#ifdef JIT_LINKTIME_SECURITY


    if (fieldDesc == NULL)
    {
        if (throwable != NULL)
            COMPlusThrow(throwable);
        else
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_BAD_FIELD_TOKEN);
    }

#endif // JIT_LINKTIME_SECURITY

    fieldHnd = CORINFO_FIELD_HANDLE(fieldDesc);

    GCPROTECT_END();
    
    return fieldHnd;
}

CORINFO_FIELD_HANDLE __stdcall CEEInfo::findField (
                                           CORINFO_MODULE_HANDLE   scopeHnd,
                                           unsigned                metaTOK,
                                           CORINFO_CONTEXT_HANDLE  context)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_FIELD_HANDLE fieldHnd = 0;

    JIT_TO_EE_TRANSITION();

    fieldHnd = findFieldInternal(scopeHnd, metaTOK, context, CORINFO_TOKENKIND_Default);

    EE_TO_JIT_TRANSITION();
    
    return fieldHnd;
}

// Internal version of findMethod - allows query on open instantiation generic methods
//
CORINFO_METHOD_HANDLE CEEInfo::findMethodInternal(
                           CORINFO_MODULE_HANDLE  scopeHnd,
                           mdToken                metaTOK,
                           CORINFO_CONTEXT_HANDLE context,
                           bool                   allowInstParam,
                           CorInfoTokenKind       tokenKind)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckContext(scopeHnd, context));
        PRECONDITION((metaTOK & CORINFO_ANNOT_MASK) == 0);
    } CONTRACTL_END;

    if ((TypeFromToken(metaTOK) != mdtMethodDef)  &&
        (TypeFromToken(metaTOK) != mdtMethodSpec) &&
        (TypeFromToken(metaTOK) != mdtMemberRef)    )
    {
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_METHOD_TOKEN);
    }

    MethodDesc *  funcDesc             = NULL;
    MethodDesc *  exactFuncDesc        = NULL;
    MethodDesc *  method               = GetMethodFromContext(context);
    MethodTable * methodMTOwner        = (method) ? method->GetMethodTable() : NULL;
    bool          allowOpenGenericInstantiation = (tokenKind  == CORINFO_TOKENKIND_Ldtoken);
    BOOL          strictMetadataChecks = allowOpenGenericInstantiation ? FALSE : TRUE;
    DWORD         checkVisibility      = (tokenKind == CORINFO_TOKENKIND_Ldtoken) ? FALSE : TRUE;

    SigTypeContext typeContext;
    GetTypeContext(context, &typeContext);

    OBJECTREF throwable = NULL;

    GCPROTECT_BEGIN(throwable);

#ifdef _DEBUG
    LOG((LF_ENC, LL_INFO100000, "CEEInfo::findMethod: token %p\n", metaTOK));
    static int breakOnFindMethod = -1;

    if (breakOnFindMethod == -1)
        breakOnFindMethod = g_pConfig->GetConfigDWORD(L"BreakOnFindMethod", 0);

    if (breakOnFindMethod == (int)metaTOK )
    {
        _ASSERTE(!"CEEInfo::FindMethod");
    }
#endif // _DEBUG

    if (IsDynamicScope(scopeHnd))
    {
        DWORD dwSecurityControlFlags = 0;
        funcDesc = CEEDynamicCodeInfo::findMethod(
                            scopeHnd, 
                            metaTOK, 
                            &typeContext, 
                            &methodMTOwner, 
                            &dwSecurityControlFlags, 
                            &throwable);


        if (dwSecurityControlFlags & DynamicResolver::SkipVisibilityChecks)
        {
            checkVisibility = FALSE;
        }

        exactFuncDesc = funcDesc;
    }
    else
    {
        HRESULT res = S_OK;
        Module* module = GetModule(scopeHnd);

        res = MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecNT(module,
                                                                    (mdMemberRef) metaTOK,
                                                                    (MethodDesc **) &funcDesc,
                                                                    &throwable, &typeContext,
                                                                    strictMetadataChecks,
                                                                    FALSE);

        // Remember the exact funcDesc for access checks
        // "exact" means that it is not shared by different instantiations.
        // Note that "exactFuncDesc" might still be a method of an open type
        // "eg. List<T>.Add()".
        exactFuncDesc = funcDesc;

#ifdef _DEBUG
        static ConfigDWORD AlwaysCallInstantiatingStub;
#endif

        // Attempt to get any underlying shared code shared between instantiations
        // of a generic method etc. Only try to get this if we
        // are requested to try to do so.  Do not return the shared code if we aren't allowed to
        // call it directly (i.e. when the prestub may insert a security or remoting stub).
        // Set AlwaysCallInstantiatingStub=1 to force all calls to go through
        // instantiating stubs.
        if (SUCCEEDED(res) &&
            allowInstParam &&
            funcDesc->IsInstantiatingStub() &&
#ifdef _DEBUG
            (AlwaysCallInstantiatingStub.val(L"AlwaysCallInstantiatingStub", 0) == 0) &&
#endif
            !funcDesc->PrestubMayInsertStub())
        {
            res = MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecNT(module,
                                                                        (mdMemberRef) metaTOK,
                                                                        (MethodDesc **) &funcDesc,
                                                                        &throwable, &typeContext,
                                                                        strictMetadataChecks,
                                                                        TRUE);
        }

        // Attempt to get any underlying shared code shared between instantiations
        // of a generic method etc. Only try to get this if we
        // are requested to try to do so.  Do not return the shared code if we aren't allowed to
        // call it directly (i.e. when the prestub may insert a security or remoting stub).
        // Set AlwaysCallInstantiatingStub=1 to force all calls to go through
        // instantiating stubs.
        if (SUCCEEDED(res) &&
            allowInstParam &&
            funcDesc->IsInstantiatingStub() &&
#ifdef _DEBUG
            (AlwaysCallInstantiatingStub.val(L"AlwaysCallInstantiatingStub", 0) == 0) &&
#endif
            !funcDesc->PrestubMayInsertStub())
        {
            res = MemberLoader::GetMethodDescFromMemberDefOrRefOrSpecNT(module,
                                                                        (mdMemberRef) metaTOK,
                                                                        (MethodDesc **) &funcDesc,
                                                                        &throwable, &typeContext,
                                                                        strictMetadataChecks,
                                                                        TRUE);
        }

        if (FAILED(res))
            funcDesc = NULL;
    }

    if (funcDesc)
    {
        if (method != NULL && method->IsTypicalMethodDefinition())
        {
            TypeHandle parentHnd;
            if (exactFuncDesc->GetMethodTable()->IsSharedByGenericInstantiations())
            {
               // exactfuncdesc may be shared, so we must use the parent token
                parentHnd = TypeHandle(findClass(scopeHnd, getMemberParent(scopeHnd, metaTOK), context));
            }
            else
            {
                // exactfuncdesc is not shared, so we can use its method table
                parentHnd = TypeHandle(exactFuncDesc->GetMethodTable());
            }

            //
            //
            //
            _ASSERTE(method->IsTypicalMethodDefinition());

            if (parentHnd.IsNull() || (checkVisibility &&
                (ClassLoader::CanAccess(method, 
                                       methodMTOwner,
                                       methodMTOwner->GetAssembly(),
                                       parentHnd.GetMethodTable(),
                                       parentHnd.GetAssembly(),
                                       exactFuncDesc->GetAttrs(),
                                       exactFuncDesc,
                                       NULL) == FALSE)))
            {
#ifdef _DEBUG
                if (funcDesc)
                {
                    LOG((LF_CLASSLOADER,
                         LL_INFO10,
                         "\tERROR: Method %s:%s does not have access to %s:%s\n",
                         method->m_pszDebugClassName,
                         method->m_pszDebugMethodName,
                         exactFuncDesc->m_pszDebugClassName,
                         exactFuncDesc->m_pszDebugMethodName));
                }
#endif // _DEBUG
                EEMethodException e(exactFuncDesc);
                throwable = e.GetThrowable();

                funcDesc = NULL;
            }
        }

#ifdef JIT_LINKTIME_SECURITY
        if (!isVerifyOnly() &&
            funcDesc && funcDesc->RequiresLinktimeCheck() &&
            (Security::LinktimeCheckMethod(GetAssemblyFromContext(context), funcDesc, &throwable) == FALSE))
        {
            _ASSERTE(Security::IsSecurityOn());

            funcDesc = NULL;
        }
#endif // JIT_LINKTIME_SECURITY

        if (!isVerifyOnly() && method != NULL && method->IsIL() &&
            exactFuncDesc == g_pPrepareConstrainedRegionsMethod)
        {
            // If the method whose context we're in is attempting a call to PrepareConstrainedRegions() then we've found the root
            // method in a Constrained Execution Region (CER). Prepare the call graph of the critical parts of that method now so
            // they won't fail because of us at runtime.
            MethodCallGraphPreparer mcgp(method, &typeContext, false, false);
            bool fMethodHasCallsWithinExplicitCer = mcgp.Run();
            if (! g_pConfig->ProbeForStackOverflow() || ! fMethodHasCallsWithinExplicitCer)
            {
                // if the method does not contain any CERs that call out, we can optimize the probe away
                funcDesc = g_pPrepareConstrainedRegionsNoOpMethod;
            }
        }
    }
        
#ifdef JIT_LINKTIME_SECURITY


    if (funcDesc == NULL)
    {
        if (throwable != NULL)
        {
            COMPlusThrow(throwable);
        }
        else
        {
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT, BFA_INVALID_METHOD_TOKEN);
        }
    }

#endif // JIT_LINKTIME_SECURITY

    GCPROTECT_END();

    return CORINFO_METHOD_HANDLE(funcDesc);
}


CORINFO_METHOD_HANDLE __stdcall CEEInfo::findMethod(
                             CORINFO_MODULE_HANDLE  scopeHnd,
                             unsigned               annotatedMetaTOK,
                             CORINFO_CONTEXT_HANDLE context)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_METHOD_HANDLE methodHnd = NULL;

    JIT_TO_EE_TRANSITION();
    
    CONSISTENCY_CHECK(CheckContext(scopeHnd, context));

    bool     allowInstParam = ((annotatedMetaTOK & CORINFO_ANNOT_MASK) == CORINFO_ANNOT_ALLOWINSTPARAM);
    mdToken  metaTOK        = (mdToken) (annotatedMetaTOK & ~CORINFO_ANNOT_MASK);

    methodHnd = findMethodInternal(scopeHnd, 
                                   metaTOK, 
                                   context, 
                                   allowInstParam, 
                                   CORINFO_TOKENKIND_Default);

    EE_TO_JIT_TRANSITION();

    return methodHnd;
}

/*********************************************************************/
void __stdcall CEEInfo::findCallSiteSig (
        CORINFO_MODULE_HANDLE       scopeHnd,
        unsigned                    sigMethTok,
        CORINFO_METHOD_HANDLE       context,
        CORINFO_SIG_INFO *          sigRet)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    PCCOR_SIGNATURE sig = NULL;
    CORINFO_MODULE_HANDLE moduleHnd = scopeHnd;
    CORINFO_METHOD_HANDLE sigContext = context;
    unsigned sigToken = sigMethTok;

    if (IsDynamicScope(scopeHnd))
    {
        sig = CEEDynamicCodeInfo::findCallSiteSig(&moduleHnd, &sigToken, &sigContext);
    }
    else
    {
        Module* module = GetModule(scopeHnd);
        LPCUTF8         szName;
        DWORD           cSig;

        if (TypeFromToken(sigMethTok) == mdtMemberRef)
        {
            szName = module->GetMDImport()->GetNameAndSigOfMemberRef(sigMethTok, &sig, &cSig);

            // Defs have already been checked by the loader for validity
            // However refs need to be checked.
            if (!Security::CanSkipVerification(module->GetDomainAssembly()))
            {
                // Can pass 0 for the flags, since it is only used for defs.
                HRESULT hr = validateTokenSig(sigMethTok, sig, cSig, 0, module->GetMDImport());
                if (FAILED(hr))
                    COMPlusThrow(kInvalidProgramException);
            }
        }
        else if (TypeFromToken(sigMethTok) == mdtMethodDef)
        {
            sig = module->GetMDImport()->GetSigOfMethodDef(sigMethTok, &cSig);
    
        }
    }

    if (!ClassLoader::CanAccessSigForExtraChecks(GetMethod(sigContext), sig))
        COMPlusThrowHR(VER_E_METHOD_ACCESS);

    CEEInfo::ConvToJitSig(sig, moduleHnd, sigToken, sigRet, sigContext, false);
    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
void __stdcall CEEInfo::findSig (
        CORINFO_MODULE_HANDLE  scopeHnd,
        unsigned               sigTok,
        CORINFO_METHOD_HANDLE  context,
        CORINFO_SIG_INFO *     sigRet)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    PCCOR_SIGNATURE sig = NULL;
    CORINFO_MODULE_HANDLE moduleHnd = scopeHnd;
    unsigned sigToken = sigTok;

    if (IsDynamicScope(scopeHnd))
    {
        sig = CEEDynamicCodeInfo::findSig(&moduleHnd, &sigToken, context);
    }
    else
    {
        Module* module = GetModule(scopeHnd);
        DWORD cSig = 0;
    
        // We need to resolve this stand alone sig
        sig = module->GetMDImport()->GetSigFromToken((mdSignature)sigTok,
                                                    &cSig);
    }

    if (!ClassLoader::CanAccessSigForExtraChecks(GetMethod(context), sig))
        COMPlusThrowHR(VER_E_METHOD_ACCESS);

    CEEInfo::ConvToJitSig(sig, moduleHnd, sigToken, sigRet, context, false);
    
    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
unsigned __stdcall CEEInfo::getClassSize (CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

#ifdef VALUE_ARRAYS
    if (isValueArray(clsHnd)) {
        ValueArrayDescr* valArr = asValueArray(clsHnd);
        // This code is not used, but the NULLs ok for instantiations as the val. descr. sigs would not contain type variables anyway
        result = valArr->sig.SizeOf(valArr->module,NULL,NULL);
    }
    else
#endif // VALUE_ARRAYS
    {
        TypeHandle VMClsHnd(clsHnd);
        result = VMClsHnd.GetSize();
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

unsigned __stdcall CEEInfo::getClassAlignmentRequirement(CORINFO_CLASS_HANDLE type)
{
    CONTRACTL {
        SO_TOLERANT;
       THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

#ifdef VALUE_ARRAYS
    if (isValueArray(type))
    {
        result = 0;
    }
#endif // VALUE_ARRAYS
    TypeHandle clsHnd(type);

    MethodTable* pMT = clsHnd.GetMethodTable();

    if ((pMT != NULL) && pMT->HasLayout())
        {
            EEClassLayoutInfo* pInfo = pMT->GetLayoutInfo();

            if (pInfo->IsManagedSequential())
            {
                CONSISTENCY_CHECK(0 == pMT->GetNumGCPointerSeries());

                // if it's managed sequential, we use the managed alignment requirement
            result = pInfo->m_ManagedLargestAlignmentRequirementOfAllMembers;
            }
            else if (pInfo->IsBlittable())
            {
                CONSISTENCY_CHECK(0 == pMT->GetNumGCPointerSeries());

                // if it's blittable, we use the unmanaged alignment requirement
            result = pInfo->m_LargestAlignmentRequirementOfAllMembers;
        }
        else
        {
            // otherwise, we fallback to sizeof(void*)
            result = sizeof(void*);
        }
        }
    else
    {
        result = sizeof(void*);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
INT __stdcall CEEInfo::getClassCustomAttribute(CORINFO_CLASS_HANDLE clsHnd,
                                               LPCSTR attrib,
                                               const BYTE** ppVal)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    INT result = 0;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(ppVal);

    TypeHandle VMClsHnd(clsHnd);

    MethodTable* pMT= VMClsHnd.GetMethodTable();
    if (!pMT) {
        _ASSERTE(!"Can't get method table pointer in getClassCustomAttribute");
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }

    DWORD cbVal;
    HRESULT hr = pMT->GetMDImport()->GetCustomAttributeByName(pMT->GetCl(),
                                                              attrib,
                                                              (const void**)ppVal,
                                                              &cbVal);
    if (hr == S_OK) {
        _ASSERTE((!ppVal) || (*ppVal != NULL));
        result = cbVal;
    } else {
        result = -1;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

CORINFO_FIELD_HANDLE __stdcall
CEEInfo::getFieldInClass(CORINFO_CLASS_HANDLE clsHnd, INT num)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_FIELD_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    TypeHandle VMClsHnd(clsHnd);

    MethodTable* pMT= VMClsHnd.GetMethodTable();
    PREFIX_ASSUME(pMT != NULL);

    result = (CORINFO_FIELD_HANDLE) ((pMT->GetApproxFieldDescListRaw()) + num);

    EE_TO_JIT_TRANSITION();

    return result;
}

mdMethodDef __stdcall
CEEInfo::getMethodDefFromMethod(CORINFO_METHOD_HANDLE hMethod)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    mdMethodDef result = 0;

    JIT_TO_EE_TRANSITION();

    MethodDesc* pMD = GetMethod(hMethod);

    {
        result = pMD->GetMemberDef();
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

BOOL __stdcall CEEInfo::checkMethodModifier(CORINFO_METHOD_HANDLE hMethod,
                                            LPCSTR modifier,
                                            BOOL fOptional)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    MethodDesc* pMD = GetMethod(hMethod);
    Module* pModule = pMD->GetModule();
    MetaSig sig(pMD);
    CorElementType eeType = fOptional ? ELEMENT_TYPE_CMOD_OPT : ELEMENT_TYPE_CMOD_REQD;

    // modopts/modreqs for the method are by convention stored on the return type
    result = sig.GetReturnProps().HasCustomModifier(pModule, modifier, eeType);

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
unsigned __stdcall CEEInfo::getClassGClayout (CORINFO_CLASS_HANDLE clsHnd, BYTE* gcPtrs)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

#ifdef VALUE_ARRAYS
    if (isValueArray(clsHnd)) {
        ValueArrayDescr* valArr = asValueArray(clsHnd);

        SigPointer elemType;
        ULONG count;
        valArr->sig.PeekSDArrayElementProps(&elemType, &count);
        CorElementType type = elemType.PeekElemType();

        switch(GetElementTypeInfo(type)->m_gc) {
            case TYPE_GC_NONE:
                {
                    unsigned numSlots = (elemType.SizeOf(valArr->module) * count + sizeof(void*)-1) / sizeof(void*);
                    memset(gcPtrs, TYPE_GC_NONE, numSlots);
                    result = 0;
                }
                break;
            case TYPE_GC_REF:
                memset(gcPtrs, TYPE_GC_REF, count);
                result = count;
                break;
            case TYPE_GC_OTHER:
                // This has bit rotted, but we currently don't use it, so it may not be worth fixing
                {
                    unsigned elemSize = elemType.SizeOf(valArr->module);
                    _ASSERTE(elemSize % sizeof(void*) == 0);
                    unsigned numSlots = elemSize / sizeof(void*);
                    
                    CORINFO_CLASS_HANDLE elemCls = getArgClass(GetScopeHandle(valArr->module), *((CORINFO_ARG_LIST_HANDLE*) &elemType));
                    result = getClassGClayout(elemCls, gcPtrs) * count;
                    for(unsigned i = 0; i < count; i++)
                        memcpy(gcPtrs, &gcPtrs[i*numSlots], numSlots);
                }
                break;
            case TYPE_GC_BYREF:
            default:
                _ASSERTE(!"ILLEGAL");
                result = 0;
                break;
        }
    }
    else
#endif // VALUE_ARRAYS
    {
        TypeHandle VMClsHnd(clsHnd);

        MethodTable* pMT = VMClsHnd.GetMethodTable();

        if (g_Mscorlib.IsClass(pMT, CLASS__TYPED_REFERENCE))
        {
            gcPtrs[0] = TYPE_GC_BYREF;
            gcPtrs[1] = TYPE_GC_NONE;
            result = 1;
        }
        else
        {

            // Sanity test for class
            _ASSERTE(pMT);
            _ASSERTE(pMT->GetClass()->GetMethodTable()->GetClass() == pMT->GetClass());
            _ASSERTE(pMT->IsValueClass());
            _ASSERTE(sizeof(BYTE) == 1);

            // assume no GC pointers at first
            result = 0;
            memset(gcPtrs, TYPE_GC_NONE,
                   (VMClsHnd.GetSize() + sizeof(void*) -1)/ sizeof(void*));

            // walk the GC descriptors, turning on the correct bits
            if (pMT->GetNumGCPointerSeries() > 0)
            {
                CGCDescSeries * pByValueSeries = ((CGCDesc*) pMT)->GetLowestSeries();
                for (int i = 0; i < pMT->GetNumGCPointerSeries(); i++)
                {
                    // Get offset into the value class of the first pointer field (includes a +Object)
                    size_t cbSeriesSize = pByValueSeries->GetSeriesSize() + pMT->GetBaseSize();
                    size_t cbOffset = pByValueSeries->GetSeriesOffset() - sizeof(Object);

                    _ASSERTE (cbOffset % sizeof(void*) == 0);
                    _ASSERTE (cbSeriesSize % sizeof(void*) == 0);

                    result += (unsigned) (cbSeriesSize / sizeof(void*));
                    memset(&gcPtrs[cbOffset/sizeof(void*)], TYPE_GC_REF, cbSeriesSize / sizeof(void*));
                    pByValueSeries++;
                }
            }
        }
    }

    EE_TO_JIT_TRANSITION();

    return result;
}
/*********************************************************************/
unsigned __stdcall CEEInfo::getClassNumInstanceFields (CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

    TypeHandle VMClsHnd(clsHnd);
    MethodTable* pMT = VMClsHnd.GetMethodTable();
    PREFIX_ASSUME(pMT != NULL);
    result = pMT->GetNumInstanceFields();

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Check Visibility rules for field access.
//
// context      : Method being compiled which is accessing the field
// parentType   : The type containing the field
// targetField  : field being accessed
// instanceType : type of the object whose field is being accessed. This
//                is used for checking Protected (family access) members.

BOOL __stdcall CEEInfo::canAccessField(
                    CORINFO_METHOD_HANDLE   context,
                    CORINFO_CLASS_HANDLE    parentType,
                    CORINFO_FIELD_HANDLE    targetField,
                    CORINFO_CLASS_HANDLE    instanceType)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL canAccessField = FALSE;

    JIT_TO_EE_TRANSITION();

    FieldDesc *pFD =  (FieldDesc*) targetField;
    MethodTable *pParentMT = TypeHandle(parentType).GetMethodTable();
    MethodDesc* pCallerMD = GetMethod(context);
    TypeHandle contextOwner(pCallerMD->GetMethodTable());
    
    if (pCallerMD->IsDynamicMethod())
    {
        TypeHandle  contextTypeOwner;
        
        DynamicResolver * pResolver = pCallerMD->GetDynamicMethodDesc()->GetResolver();

        DWORD dwSecurityFlags = 0;
        pResolver->GetJitContext(&dwSecurityFlags, &contextTypeOwner);

        if (dwSecurityFlags & DynamicResolver::SkipVisibilityChecks)
        {
            canAccessField = TRUE;
        }
        
        if (!contextTypeOwner.IsNull())
            contextOwner = contextTypeOwner;
    }

    if (!canAccessField)
    {
        canAccessField = ClassLoader::CanAccess(
                        pCallerMD, 
                        contextOwner.GetMethodTable(),
                        contextOwner.GetAssembly(),
                        pParentMT,
                        pParentMT->GetAssembly(),
                        TypeHandle(instanceType).GetMethodTable(),
                        pFD->GetAttributes(),
                        NULL,
                        pFD);
    }

    EE_TO_JIT_TRANSITION();

    return canAccessField;
}

/*********************************************************************/
// get the class representing the single dimensional array for the
// element type represented by clsHnd

CORINFO_CLASS_HANDLE __stdcall CEEInfo::getSDArrayForClass(CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    TypeHandle elemType(clsHnd);
    TypeHandle ret = ClassLoader::LoadArrayTypeThrowing(elemType);
    _ASSERTE(ret.AsPtr());
    result = (CORINFO_CLASS_HANDLE) ret.AsPtr();

    EE_TO_JIT_TRANSITION();

    return result;
}

CorInfoType __stdcall CEEInfo::asCorInfoType (CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoType result = CORINFO_TYPE_UNDEF;

    JIT_TO_EE_TRANSITION();

    TypeHandle VMClsHnd(clsHnd);
    result = toJitType(VMClsHnd);

    EE_TO_JIT_TRANSITION();

    return result;
}


CORINFO_LOOKUP_KIND __stdcall CEEInfo::getLocationOfThisType(CORINFO_METHOD_HANDLE context)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_LOOKUP_KIND result;

    /* Initialize fields of result for debug build warning */
    result.needsRuntimeLookup = false;
    result.runtimeLookupKind  = CORINFO_LOOKUP_THISOBJ;

    JIT_TO_EE_TRANSITION();

    MethodDesc *pContextMD = GetMethod(context);

    // If the method table is not shared, then return CONST
    if (!pContextMD->GetClass()->IsSharedByGenericInstantiations())
    {
        result.needsRuntimeLookup = false;
    }
    else
    {
        result.needsRuntimeLookup = true;

    // If we've got a vtable extra argument, go through that
        if (pContextMD->RequiresInstMethodTableArg())
            result.runtimeLookupKind = CORINFO_LOOKUP_CLASSPARAM;

    // If we've got an object, go through its vtable
    else if (pContextMD->AcquiresInstMethodTableFromThis())
            result.runtimeLookupKind = CORINFO_LOOKUP_THISOBJ;
        else
        {
    // Otherwise go through the method-desc argument
    _ASSERTE(pContextMD->RequiresInstMethodDescArg());
            result.runtimeLookupKind = CORINFO_LOOKUP_METHODPARAM;
        }
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

CORINFO_METHOD_HANDLE __stdcall CEEInfo::GetDelegateCtor(
                                        CORINFO_METHOD_HANDLE methHnd,
                                        CORINFO_CLASS_HANDLE clsHnd,
                                        CORINFO_METHOD_HANDLE targetMethodHnd,
                                        DelegateCtorArgs *pCtorData)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_METHOD_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    MethodDesc *pCurrentCtor = (MethodDesc*)methHnd;
    if (!pCurrentCtor->IsFCall())
    {
        result =  methHnd;
    }
    else
    {
        MethodDesc *pTargetMethod = (MethodDesc*)targetMethodHnd;
        TypeHandle delegateType = (TypeHandle)clsHnd;

        MethodDesc *pDelegateCtor = COMDelegate::GetDelegateCtor(delegateType, pTargetMethod, pCtorData);
        if (!pDelegateCtor)
            pDelegateCtor = pCurrentCtor;
        result = (CORINFO_METHOD_HANDLE)pDelegateCtor;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

void __stdcall CEEInfo::MethodCompileComplete(CORINFO_METHOD_HANDLE methHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    MethodDesc* pMD = GetMethod(methHnd);

    if (pMD->IsDynamicMethod())
    {
        pMD->GetDynamicMethodDesc()->GetResolver()->FreeCompileTimeState();
    }

    EE_TO_JIT_TRANSITION();
}


// Given a module scope (scopeHnd), a method handle (context) and an annotated metadata token (annotatedMetaTOK),
// attempt to load the handle (type, field or method) associated with the token.
// If this is not possible at compile-time (because the method code is shared and the token contains type parameters)
// then indicate how the handle should be looked up at run-time.
//
// Type tokens can be combined with CORINFO_ANNOT_MASK flags to obtain array type handles and method entry points
// These are required by the newarr instruction which takes a token for the *element* type of the array
// and by call/ldftn instructions.
//
// See corinfo.h for more details
//
void  __stdcall CEEInfo::embedGenericHandle(
            CORINFO_MODULE_HANDLE  scopeHnd,
            unsigned      annotatedMetaTOK,
            CORINFO_CONTEXT_HANDLE context,
            CorInfoTokenKind       tokenKind,
            CORINFO_GENERICHANDLE_RESULT *pResult)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    memset(pResult, 0, sizeof(*pResult));

    JIT_TO_EE_TRANSITION();

    CONSISTENCY_CHECK(CheckContext(scopeHnd, context));
    CONSISTENCY_CHECK(CheckPointer(pResult));
    
    VarKind varKind = hasNoVars;

    Module* pModule = GetModule(scopeHnd);
    mdToken     metaTOK = annotatedMetaTOK & ~CORINFO_ANNOT_MASK;
    CORINFO_ANNOTATION annotation = (CORINFO_ANNOTATION)(annotatedMetaTOK & CORINFO_ANNOT_MASK);

    // Class or method type parameters present in token
    pResult->compileTimeHandle = loadTokenInternal(scopeHnd, annotation, metaTOK, 
                                                   context, tokenKind, 
                                                   NULL, NULL, NULL);
    if (!IsDynamicScope(scopeHnd))
        varKind = ScanToken(pModule, metaTOK, context, TRUE);
    
    _ASSERTE(pResult->compileTimeHandle);
        
    // If the target is not shared then we've already got our result and
    // can simply do a static look up
    if ((varKind & hasSharableVarsMask) == 0)
    {
        pResult->lookup.lookupKind.needsRuntimeLookup = false;
    }
    else
    {
        // need a bit more solid story in the future
        _ASSERTE(!IsDynamicScope(scopeHnd));
#ifdef _DEBUG
        // Is the context shared code (wrt either method or class type parameters)?
        BOOL contextShared = ContextIsShared(context);

        // If we've got this far we need to do the lookup at run-time
        _ASSERTE(contextShared);
        _ASSERTE((varKind & hasSharableClassVar) || (varKind & hasSharableMethodVar));
#endif // _DEBUG
        MethodDesc *pContextMD = GetMethodFromContext(context);

        DictionaryEntryLayout entryLayout(annotatedMetaTOK, 0);

        ComputeRuntimeLookupForSharedGenericToken(pModule,
                                                  &entryLayout,
                                                  pContextMD,
                                                  varKind,
                                                  &pResult->lookup);
    }

    pResult->lookup.constLookup.handle = pResult->compileTimeHandle;
    pResult->lookup.constLookup.accessType = IAT_VALUE;
    pResult->lookup.constLookup.accessModule = IAM_CURRENT_MODULE;
    
    EE_TO_JIT_TRANSITION();
}

void CEEInfo::CheckTokenKind(CORINFO_CLASS_HANDLE  clsHnd,
                             CorInfoTokenKind      tokenKind)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CorElementType et = TypeHandle(clsHnd).GetInternalCorElementType();

    switch (tokenKind)
    {
        case CORINFO_TOKENKIND_Default:
            // Disallow ELEMENT_TYPE_BYREF and ELEMENT_TYPE_VOID
            if (et == ELEMENT_TYPE_BYREF || et == ELEMENT_TYPE_VOID)
                COMPlusThrow(kInvalidProgramException);
            break;
            
        case CORINFO_TOKENKIND_Ldtoken:
            // Allow everything.
            break;
        
        case CORINFO_TOKENKIND_Casting:
            // We allow System.Void for backwards compatibility reason only,
            // while it should really be disallowed. 
            if (et == ELEMENT_TYPE_BYREF)
                COMPlusThrow(kInvalidProgramException);
            break;
            
        default:
            _ASSERTE(!"Unknown typeCheckFlag");
            break;
    }
}

// Lookup/load a token and work out if it involves type variables or not
CORINFO_GENERIC_HANDLE CEEInfo::loadTokenInternal(
                   CORINFO_MODULE_HANDLE  scopeHnd,
                   CORINFO_ANNOTATION     annotation,
                   mdToken                metaTOK,
                   CORINFO_CONTEXT_HANDLE context,
                   CorInfoTokenKind       tokenKind,
                   TypeHandle *           pTH,
                   MethodDesc **          ppMeth,
                   FieldDesc **           ppFld)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    CORINFO_GENERIC_HANDLE result = 0;

    switch (TypeFromToken(metaTOK))
    {
    case mdtTypeRef:
    case mdtTypeDef:
    case mdtTypeSpec:
    {
        if (annotation == CORINFO_ANNOT_PERMITUNINSTDEFORREF)
            metaTOK = (mdToken)(metaTOK | annotation);

        CORINFO_CLASS_HANDLE clsHnd = findClassInternal(scopeHnd, metaTOK, context, tokenKind);

        CheckTokenKind(clsHnd, tokenKind);

        if (annotation == CORINFO_ANNOT_ARRAY)
        {
            clsHnd = getSDArrayForClass(clsHnd);
            result = CORINFO_GENERIC_HANDLE(clsHnd);
        }
        else
        {
            result = CORINFO_GENERIC_HANDLE(clsHnd);
        }
        if (pTH)
            *pTH = TypeHandle(clsHnd);
        break;
    }

    case mdtMemberRef:
    {
        // dynamic scope never create member ref for field
        // this is a bit fragile as it's making a statement that cannot be guarded
        // asking the scope to analyze the sig is a better way to approach this
        if (!IsDynamicScope(scopeHnd))
        {
            // OK, we have to look at the metadata to see if it's a field or method
            Module *pModule = GetModule(scopeHnd);
            PCCOR_SIGNATURE pSig;
            ULONG cSig;
            pModule->GetMDImport()->GetNameAndSigOfMemberRef(metaTOK, &pSig, &cSig);

            if (isCallConv(MetaSig::GetCallingConventionInfo(pModule, pSig), IMAGE_CEE_CS_CALLCONV_FIELD))
            {
                CORINFO_FIELD_HANDLE fldHnd = findFieldInternal(scopeHnd, 
                                                                metaTOK, 
                                                                context, 
                                                                tokenKind);
                result = CORINFO_GENERIC_HANDLE(fldHnd);
                if (ppFld)
                    *ppFld = (FieldDesc *) fldHnd;
                break;
            }
        }
        //fall through
    }
    case mdtMethodDef:
    case mdtMethodSpec:
    {
        bool allowInstParam = (annotation == CORINFO_ANNOT_ALLOWINSTPARAM);

        CORINFO_METHOD_HANDLE methHnd = findMethodInternal(scopeHnd, metaTOK, context, 
                                                           allowInstParam, 
                                                           tokenKind);
        result = CORINFO_GENERIC_HANDLE(methHnd);

        if (ppMeth)
            *ppMeth = GetMethod(methHnd);
        break;
    }


    case mdtFieldDef:
    {
        CORINFO_FIELD_HANDLE fldHnd = findFieldInternal(scopeHnd, 
                                                        metaTOK, 
                                                        context, 
                                                        tokenKind);
        result = CORINFO_GENERIC_HANDLE(fldHnd);
        if (ppFld)
            *ppFld = (FieldDesc *) fldHnd;
        break;
    }

    default:
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }

    return result;
}

void CEEInfo::AddDependencyOnClassToken(Module* pModule, mdToken tk, BOOL bScanParents)
{
     _ASSERTE(pModule);
      Module* cModule=pModule;
      mdToken ctoken=tk;
      while(1)
      {
            if (TypeFromToken(ctoken)==mdtTypeSpec)
            {
                //only the class itself
                ULONG cSig;
                PCCOR_SIGNATURE pSig;         
                cModule->GetMDImport()->GetTypeSpecFromToken(ctoken, &pSig, &cSig);
                SigPointer sigptr = SigPointer(pSig);
                CorElementType type ;
                IfFailThrow(sigptr.GetElemType(&type));
        
                if (type != ELEMENT_TYPE_GENERICINST) 
                    break;
                IfFailThrow(sigptr.GetElemType(&type));
                IfFailThrow(sigptr.GetToken(&ctoken));
                if (TypeFromToken(ctoken)!=mdtTypeRef &&
                    TypeFromToken(ctoken)!=mdtTypeDef)
                    break;
            }
            Module* pTypeDefModule=cModule;
            mdToken tkTypeDef=ctoken;
            if (TypeFromToken(ctoken)==mdtTypeRef)
            {
                if(ClassLoader::ResolveTokenToTypeDefThrowing(cModule,ctoken,&pTypeDefModule,&tkTypeDef))
                {
                    STRESS_LOG4(LF_CLASSLOADER,LL_INFO1000,"TypeRef  %08x/%08x is %08x/%08x",ctoken,cModule,tkTypeDef,pTypeDefModule);                              
                    if(pTypeDefModule->IsSystem())
                        break;
                    if (pTypeDefModule!=pModule)
                        addActiveDependency((CORINFO_MODULE_HANDLE)pModule,(CORINFO_MODULE_HANDLE)pTypeDefModule);
                }
            }
            if (!bScanParents)
                break;
            if (IsNilToken(tkTypeDef))
                break;
            mdToken tkParent;
            IfFailThrow(pTypeDefModule->GetMDImport()->GetTypeDefProps(tkTypeDef,NULL,&tkParent));
            if (TypeFromToken(tkParent)==mdtTypeSpec)
                    ScanToken(pTypeDefModule,tkParent,NULL,TRUE);
            cModule=pTypeDefModule;
            ctoken=tkParent;
            
      }

}


VarKind CEEInfo::ScanToken(Module* pModule, mdToken token,  CORINFO_CONTEXT_HANDLE tokenContext, BOOL bScanModuleDeps)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;

    if (pModule->IsSystem())
        bScanModuleDeps=FALSE;
    
    VarKind varKind=hasNoVars;
    
   BOOL bScanVarKind=FALSE;
   SigTypeContext typeContext;
   if(tokenContext)
   {
        GetTypeContext(tokenContext, &typeContext);
        bScanVarKind=ContextIsShared(tokenContext);
   }

    if (bScanModuleDeps)
    {
        mdToken tk=token;

        // the iterator does not see tokens that are not part of signatures
        // so process the non-signature part here
        if (TypeFromToken(tk)==mdtMethodSpec)
            pModule->GetMDImport()->GetMethodSpecProps(tk,&tk,NULL,NULL);                

        if (TypeFromToken(tk)==mdtTypeRef ||
                TypeFromToken(tk)==mdtTypeDef )
        {
            AddDependencyOnClassToken(pModule,tk,TRUE);
        }
        else
        {
            if (TypeFromToken(tk)==mdtMemberRef)
            {
                mdToken tkParent=GetMemberParent(pModule,tk);
                if (TypeFromToken(tkParent)==mdtTypeRef)
                {
                    AddDependencyOnClassToken(pModule,tkParent,TRUE);
                }
                else
                if (TypeFromToken(tkParent)==mdtModuleRef)
                {
                    addActiveDependency((CORINFO_MODULE_HANDLE)pModule,(CORINFO_MODULE_HANDLE)pModule->LookupModule(tkParent,FALSE));
                }
            }
        }
    }
  
    SigPtrInstIterator sigIter(pModule,token);
    while(sigIter.Next())
    {
        if (bScanVarKind)
           varKind=(VarKind)(varKind | sigIter.Current().IsPolyType(&typeContext));
        if(bScanModuleDeps)
            ScanForModuleDependencies(pModule,sigIter.Current().GetPtr(), FALSE);
    }
    return varKind;
}


void CEEInfo::ScanForModuleDependencies(Module* pModule, PCCOR_SIGNATURE pSig, BOOL bIgnoreValueTypes)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pModule));
    }
    CONTRACTL_END;

    SigPointer psig(pSig);

    CorElementType eType;
    IfFailThrow(psig.GetElemType(&eType));
    
    switch (eType)
    {
        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        case ELEMENT_TYPE_R4:
        case ELEMENT_TYPE_R8:
        case ELEMENT_TYPE_VOID:
        case ELEMENT_TYPE_FNPTR:
        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_MVAR:
            break;

        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:
#ifdef VALUE_ARRAYS
        case ELEMENT_TYPE_VALUEARRAY:
#endif
        case ELEMENT_TYPE_PINNED:
        case ELEMENT_TYPE_BYREF:
        case ELEMENT_TYPE_PTR:
            ScanForModuleDependencies(pModule,psig.GetPtr(),bIgnoreValueTypes);
            break;

        case ELEMENT_TYPE_GENERICINST:
        {
            ScanForModuleDependencies(pModule,psig.GetPtr(),bIgnoreValueTypes);
            IfFailThrow(psig.SkipExactlyOne());

            ULONG ntypars;
            IfFailThrow(psig.GetData(&ntypars));
            for (ULONG i = 0; i < ntypars; i++)
            {
              ScanForModuleDependencies(pModule,psig.GetPtr(),FALSE);
              IfFailThrow(psig.SkipExactlyOne());
            }
            break;
        }


        case ELEMENT_TYPE_CLASS:
        {
              mdToken tk;
              IfFailThrow(psig.GetToken(&tk));
              AddDependencyOnClassToken(pModule,tk,FALSE);
              break;
        }
        case ELEMENT_TYPE_VALUETYPE:
              if(bIgnoreValueTypes)
                break;
              mdToken tk;
              IfFailThrow(psig.GetToken(&tk));
              if (TypeFromToken(tk)==mdtTypeRef)
              {
                    Module* pTypeDefModule=NULL;
                    if(ClassLoader::ResolveTokenToTypeDefThrowing(pModule,tk,&pTypeDefModule,NULL))
                    {
                        _ASSERTE(pTypeDefModule);
                        if (pTypeDefModule!=pModule)
                        addActiveDependency((CORINFO_MODULE_HANDLE)pModule,(CORINFO_MODULE_HANDLE)pTypeDefModule);
                    }
              }
            break;

        case ELEMENT_TYPE_TYPEDBYREF:
        case ELEMENT_TYPE_INTERNAL:
            break;
        default:
            BAD_FORMAT_NOTHROW_ASSERT(!"BadType");
    }
}


void CEEInfo::ComputeRuntimeLookupForSharedGenericToken(Module *pModule,
                                                        DictionaryEntryLayout *pEntryLayout,
                                                        MethodDesc* pContextMD,
                                                        VarKind varKind,
                                                        CORINFO_LOOKUP *pResultLookup)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pResultLookup));
    } CONTRACTL_END;

    unsigned metaTOK = pEntryLayout->m_token1 & ~CORINFO_ANNOT_MASK;
    unsigned annotation = pEntryLayout->m_token1 & CORINFO_ANNOT_MASK;
    mdToken     tokType = TypeFromToken(metaTOK);

    // Where does the token live?
    IMDInternalImport *pInternalImport = pModule->GetMDImport();

    pResultLookup->lookupKind.needsRuntimeLookup = true;
    CORINFO_RUNTIME_LOOKUP *pResult = &pResultLookup->runtimeLookup;

    // This are the tokens the JIT needs to pass back to the EE at runtime.
    pResult->token1 = pEntryLayout->m_token1;
    pResult->token2 = pEntryLayout->m_token2;

    pResult->helper = CORINFO_HELP_RUNTIMEHANDLE;

    // If we've got a  method type parameter of any kind then we must look in the method desc arg
    if ((varKind & hasMethodVar) != 0)
    {
        _ASSERTE(pContextMD->RequiresInstMethodDescArg());
        pResultLookup->lookupKind.runtimeLookupKind = CORINFO_LOOKUP_METHODPARAM;
    }

    // Otherwise we must just have class type variables
    else
    {
        _ASSERTE((varKind & hasClassVar) != 0);
        _ASSERTE((varKind & hasSharableClassVar) != 0);
        _ASSERTE((varKind & hasMethodVar) == 0);
        _ASSERTE((varKind & hasSharableMethodVar) == 0);

        // If we've got a vtable extra argument, go through that
        if (pContextMD->RequiresInstMethodTableArg())
            pResultLookup->lookupKind.runtimeLookupKind = CORINFO_LOOKUP_CLASSPARAM;

        // If we've got an object, go through its vtable
        else if (pContextMD->AcquiresInstMethodTableFromThis())
            pResultLookup->lookupKind.runtimeLookupKind = CORINFO_LOOKUP_THISOBJ;

        // Otherwise go through the method-desc argument
        else
        {
            _ASSERTE(pContextMD->RequiresInstMethodDescArg());
            pResultLookup->lookupKind.runtimeLookupKind = CORINFO_LOOKUP_METHODPARAM;
        }
    }

    // Unless we decide otherwise, just do the lookup via a helper function
    pResult->indirections = CORINFO_USEHELPER;

    // It's a method dictionary lookup
    if (pResultLookup->lookupKind.runtimeLookupKind == CORINFO_LOOKUP_METHODPARAM)
    {
        // Special cases:
        // (1) Naked method type variable: look up directly in instantiation hanging off runtime md
        // (2) Reference to method-spec of current method (e.g. a recursive call) i.e. currentmeth<!0,...,!(n-1)>
        if (tokType == mdtTypeSpec && annotation == 0)
        {
            _ASSERTE(pEntryLayout->m_token2 == 0);
            PCCOR_SIGNATURE pSig;
            ULONG cSig;
            pInternalImport->GetTypeSpecFromToken(metaTOK, &pSig, &cSig);
            SigPointer sigptr = SigPointer(pSig, cSig);
            CorElementType type;
            IfFailThrow(sigptr.GetElemType(&type));
            if (type == ELEMENT_TYPE_MVAR)
            {
                pResult->indirections = 2;
                pResult->testForNull = 0;
                pResult->offsets[0] = offsetof(InstantiatedMethodDesc, m_pPerInstInfo);

                ULONG data;
                IfFailThrow(sigptr.GetData(&data));
                pResult->offsets[1] = sizeof(TypeHandle) * data;
            }
        }
        else if (tokType == mdtMethodSpec && annotation == 0)
        {
            _ASSERTE(pEntryLayout->m_token2 == 0);
            PCCOR_SIGNATURE pSig;
            ULONG cSig;
            mdMemberRef MemberRef;

            // Get the member def/ref and instantiation signature
            pInternalImport->GetMethodSpecProps(metaTOK, &MemberRef, &pSig, &cSig);

            // It's the context itself (i.e. a recursive call)
            if (TypeFromToken(MemberRef) == mdtMethodDef && MemberRef == pContextMD->GetMemberDef())
            {
                // Now just check that the instantiation is (!!0, ..., !!(n-1))
                _ASSERT(*pSig == IMAGE_CEE_CS_CALLCONV_GENERICINST);
                pSig++;

                SigPointer sigptr(pSig, cSig);
                ULONG ntypars;
                IfFailThrow(sigptr.GetData(&ntypars));

                for (ULONG i = 0; i < ntypars; i++)
                {
                    CorElementType type;
                    IfFailThrow(sigptr.GetElemType(&type));
                    if (type != ELEMENT_TYPE_MVAR)
                        goto NoSpecialCase1;

                    ULONG data;
                    IfFailThrow(sigptr.GetData(&data));
                    
                    if (data != i)
                        goto NoSpecialCase1;
                }

                // Just use the method descriptor that was passed in!
                pResult->indirections = 0;
                pResult->testForNull = 0;
            }
        }

NoSpecialCase1:
        // No special-casing has been applied
        if (pResult->indirections == CORINFO_USEHELPER)
        {
            _ASSERTE(pContextMD != NULL);
            _ASSERTE(pContextMD->HasMethodInstantiation());

            pResult->indirections = ((InstantiatedMethodDesc*) pContextMD)->FindDictionaryToken(pEntryLayout, pResult->offsets + 1);
            if (pResult->indirections != CORINFO_USEHELPER)
            {
                _ASSERTE(pResult->indirections+1 <= CORINFO_MAXINDIRECTIONS);

                pResult->testForNull = 1;

                // Indirect through dictionary table pointer in InstantiatedMethodDesc
                pResult->offsets[0] = offsetof(InstantiatedMethodDesc, m_pPerInstInfo);

                // ...and we've added an indirection
                pResult->indirections += 1;
            }
        }
    }

    // It's a class dictionary lookup (CORINFO_LOOKUP_CLASSPARAM or CORINFO_LOOKUP_THISOBJ)
    else
    {
        MethodTable *pContextMT= pContextMD->GetMethodTable();
        _ASSERTE(pContextMT->GetNumGenericArgs() > 0);

        // Special cases:
        // (1) Naked class type variable: look up directly in instantiation hanging off vtable
        // (2) C<!0,...,!(n-1)> where C is the context's class and C is sealed: just return vtable ptr
        if (tokType == mdtTypeSpec && annotation == 0)
        {
            _ASSERTE(pEntryLayout->m_token2 == 0);
            PCCOR_SIGNATURE pSig;
            ULONG cSig;
            pInternalImport->GetTypeSpecFromToken(metaTOK, &pSig, &cSig);
            SigPointer sigptr = SigPointer(pSig, cSig);
            CorElementType type;
            IfFailThrow(sigptr.GetElemType(&type));
            if (type == ELEMENT_TYPE_VAR)
            {
                pResult->indirections = 3;
                pResult->testForNull = 0;
                pResult->offsets[0] = MethodTable::GetOffsetOfPerInstInfo();
                pResult->offsets[1] = sizeof(TypeHandle*) * (pContextMT->GetNumDicts()-1);
                ULONG data;
                IfFailThrow(sigptr.GetData(&data));
                pResult->offsets[2] = sizeof(TypeHandle) * data;
            }
            else if (type == ELEMENT_TYPE_GENERICINST && pContextMT->IsSealed())
            {
                CorElementType tmpType;
                IfFailThrow(sigptr.GetElemType(&tmpType));
                BOOL isValueTypeSig = tmpType == ELEMENT_TYPE_VALUETYPE;
                if (isValueTypeSig == (pContextMT->IsValueClass() != 0))
                {
                    mdTypeRef typeref;
                    IfFailThrow(sigptr.GetToken(&typeref));
                    if (TypeFromToken(typeref) == mdtTypeDef && typeref == pContextMT->GetCl())
                    {
                        ULONG ntypars;
                        IfFailThrow(sigptr.GetData(&ntypars));
                        for (ULONG i = 0; i < ntypars; i++)
                        {
                            IfFailThrow(sigptr.GetElemType(&type));
                            if (type != ELEMENT_TYPE_VAR)
                                goto NoSpecialCase2;

                            ULONG data;
                            IfFailThrow(sigptr.GetData(&data));

                            if (data != i)
                                goto NoSpecialCase2;
                        }

                        // Just use the vtable pointer itself!
                        pResult->indirections = 0;
                        pResult->testForNull = 0;
                    }
                }
            }
        }
NoSpecialCase2:

        // No special-casing has been applied
        if (pResult->indirections == CORINFO_USEHELPER)
        {
            pResult->indirections = Generics::FindClassDictionaryToken(pContextMT->GetClass(), pEntryLayout, pResult->offsets + 2);
            if (pResult->indirections != CORINFO_USEHELPER)
            {
                _ASSERTE(pResult->indirections+2 <= CORINFO_MAXINDIRECTIONS);

                pResult->testForNull = 1;

                // Indirect through dictionary table pointer in vtable
                pResult->offsets[0] = MethodTable::GetOffsetOfPerInstInfo();

                // Next indirect through the dictionary appropriate to this instantiated type
                pResult->offsets[1] = sizeof(TypeHandle*) * (pContextMT->GetNumDicts()-1);

                // ...and we've added a couple of indirections
                pResult->indirections += 2;
            }
        }
    }
}



/*********************************************************************/
const char* __stdcall CEEInfo::getClassName (CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    const char* result = NULL;

    JIT_TO_EE_TRANSITION();

    TypeHandle VMClsHnd(clsHnd);
    MethodTable* pMT = VMClsHnd.GetMethodTable();
    if (pMT == NULL)
    {
        result = "";
    }
    else
    {
#ifdef _DEBUG
        result = pMT->GetDebugClassName();
#else // !_DEBUG
    // since this is for diagnostic purposes only,
    // give up on the namespace, as we don't have a buffer to concat it
    // also note this won't show array class names.
    LPCUTF8 nameSpace;
        result = pMT->GetClass()->GetFullyQualifiedNameInfo(&nameSpace);
#endif
}

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
const char* __stdcall CEEInfo::getHelperName (CorInfoHelpFunc ftnNum)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
        PRECONDITION(ftnNum >= 0 && ftnNum < CORINFO_HELP_COUNT);
    } CONTRACTL_END;

    const char* result = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

#ifdef _DEBUG
    result = hlpFuncTable[ftnNum].name;
#else
    result = "AnyJITHelper";
#endif

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}


/*********************************************************************/
int __stdcall CEEInfo::appendClassName(__deref_inout_ecount(*pnBufLen) WCHAR** ppBuf,
                                       int* pnBufLen,
                                       CORINFO_CLASS_HANDLE    clsHnd,
                                       BOOL fNamespace,
                                       BOOL fFullInst,
                                       BOOL fAssembly)
{
    CONTRACTL {
        SO_TOLERANT;
        MODE_ANY;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    int nLen = 0;

    JIT_TO_EE_TRANSITION();

    TypeHandle th(clsHnd);
    StackSString ss;
    TypeString::AppendType(ss,th,
                           (fNamespace ? TypeString::FormatNamespace : 0) |
                           (fFullInst ? TypeString::FormatFullInst : 0) |
                           (fAssembly ? TypeString::FormatAssembly : 0));
    const WCHAR* szString = ss.GetUnicode();
    nLen = (int)wcslen(szString);
    if (*pnBufLen > 0)
    {
    wcscpy_s(*ppBuf, *pnBufLen, szString );
    (*ppBuf)[(*pnBufLen) - 1] = L'\0';
    (*ppBuf) += nLen;
    (*pnBufLen) -= nLen;
    }

    EE_TO_JIT_TRANSITION();

    return nLen;
}

/*********************************************************************/
CORINFO_MODULE_HANDLE __stdcall CEEInfo::getClassModule(CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_MODULE_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    TypeHandle     VMClsHnd(clsHnd);

    result = CORINFO_MODULE_HANDLE(VMClsHnd.GetModule());

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
CORINFO_MODULE_HANDLE __stdcall CEEInfo::getClassModuleForStatics(CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_MODULE_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    TypeHandle     VMClsHnd(clsHnd);

    result = CORINFO_MODULE_HANDLE(VMClsHnd.AsMethodTable()->GetModuleForStatics());
    _ASSERTE(result);

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// If this method returns true, JIT will do optimization to inline the check for
//     GetClassFromHandle(handle) == obj.GetType()
//
// Note: 
//     We can't do this optimization for arrays or marshalbyrefs. 
//     This will enable to use directly the typehandle instead of going through getClassByHandle
BOOL __stdcall CEEInfo::canInlineTypeCheckWithObjectVTable (CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL ret = FALSE;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(clsHnd);

    TypeHandle VMClsHnd(clsHnd);
    
    if (!VMClsHnd.IsByRef() && !VMClsHnd.IsGenericVariable())
    {
        MethodTable *pMT = VMClsHnd.GetMethodTable();

        if (pMT)
        {
            ret = VMClsHnd.IsUnsharedMT() && !pMT->IsMarshaledByRef();
        }
    }
    
    EE_TO_JIT_TRANSITION();

    return(ret);
}

/*********************************************************************/
// The methodBeingCompiledHnd is only used to determine
// domain-neutrality of the code being generated,  if A calls B calls C and B and C are both inlined
// into A then A should be passed as the context parameter to getClassAttribs  when processing
// the IL of both B and C.  Note for most other methods "context" has a different meaning
// and refers to the static caller, i.e. in thist case B.

DWORD __stdcall CEEInfo::getClassAttribs (CORINFO_CLASS_HANDLE clsHnd, CORINFO_METHOD_HANDLE methodBeingCompiledHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    DWORD ret = 0;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(clsHnd);

    TypeHandle     VMClsHnd(clsHnd);

    // Byrefs should only occur in method and local signatures, which are accessed
    // using ICorClassInfo and ICorClassInfo.getChildType.
    // So findClass() and getClassAttribs() should not be called for byrefs

    if (VMClsHnd.IsByRef())
    {
        _ASSERTE(!"Did findClass() return a Byref?");
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }
    else if (VMClsHnd.IsGenericVariable())
    {
        //@GENERICSVER: for now, type variables simply report "variable".
        ret |= CORINFO_FLG_GENERIC_TYPE_VARIABLE;
    }
    else
    {
        MethodTable *pMT = VMClsHnd.GetMethodTable();

        if (!pMT)
        {
            _ASSERTE(!"Did findClass() return a Byref?");
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
        }

        // Get the calling method (note if A calls B calls C and B and C are both inlined
        // into A then A still counts as the calling method - this is only true for getClassAttribs)
        MethodDesc *methodBeingCompiled = GetMethod(methodBeingCompiledHnd);

        // The array flag is used to identify the faked-up methods on
        // array types, i.e. .ctor, Get, Set and Address
        if (pMT->IsArray())
            ret |= CORINFO_FLG_ARRAY;

        // Do not bother figuring out the initialization if we are only verifying the method.
        if (!isVerifyOnly())
        {
            // The Global Class "<Module>" has a couple of useful special properties
            // 1. It is always considered to be initialized
            // 2. We never set the NEEDS_INIT bit 
            //
            if (pMT->IsGlobalClass())
            {
                // For both jitted and ngen code the global class is always considered initialized
                ret |= CORINFO_FLG_INITIALIZED;
            }
            else
            {
                BOOL fSharedByGenerics = (pMT->IsSharedByGenericInstantiations() &&
                                         methodBeingCompiled->IsSharedByGenericInstantiations());
                
                // Set the INITIALIZED bit if the class is initialized
                if (!SystemDomain::GetCurrentDomain()->IsCompilationDomain() &&
                    !fSharedByGenerics)
                {
                    // For the Jitted case we can return true if we dynamically know
                    // that the class is initialized
                    if (pMT->IsClassPreInited())
                    {
                        ret |= CORINFO_FLG_INITIALIZED;
                    }
                    else if (!methodBeingCompiled->IsDomainNeutral())
                    {
                        // If we are accessing from unshared code, we can check the
                        // state of the current domain as well.
                        
                        EX_TRY
                        {
                            DomainLocalModule *pModule = pMT->GetDomainLocalModule();
                            pModule->PopulateClass(pMT);
                            if (pModule->IsClassInitialized(pMT))
                                ret |= CORINFO_FLG_INITIALIZED;
                        }
                        EX_CATCH
                        {
                            // just eat the exception and assume class not inited
                        }
                        EX_END_CATCH(SwallowAllExceptions);
                    }
                }

                // Set the NEEDS_INIT bit if we require the JIT to insert cctor logic before
                // access.  This is currently used only for static field accesses and method inlining.
                // Note that this is set independently of the INITIALIZED bit above.
                
                if (pMT != methodBeingCompiled->GetMethodTable())
                {
                    if (pMT->IsDomainNeutral())
                    {
                        // For domain neutral classes, the inited bit is only set when we never need
                        // class initialization (no cctor or static handles)
                        
                        if (!pMT->IsClassPreInited())
                        {
                            //
                            // For the shared->shared access, it turns out we never have to do any initialization
                            // in this case.  There are 2 cases:
                            //
                            // BeforeFieldInit: in this case, no init is required on method calls, and
                            //  the shared helper will perform the required init on static field accesses.
                            // Exact:  in this case we do need an init on method calls, but this will be
                            //  provided by a prolog of the method itself.
                            
                            if (!methodBeingCompiled->IsDomainNeutral())
                            {
                                // Unshared->shared access.  The difference in this case (from above) is
                                // that we don't use the shared helper.  Thus we need the JIT to provide
                                // a class construction call.
                                
                                ret |= CORINFO_FLG_NEEDS_INIT;
                            }
                        }
                    }
                    else
                    {
                        // For accesses to unshared classes (which are by necessity from other unshared classes),
                        // we need initialization iff we have a class constructor.
                        
                        if (pMT->HasClassConstructor() || pMT->GetNumBoxedStatics() > 0)
                            ret |= CORINFO_FLG_NEEDS_INIT;
                    }
                }
            }
        }

        if (pMT->GetClass()->IsBeforeFieldInit())
        {
            ret |= CORINFO_FLG_BEFOREFIELDINIT;
        }

        if (pMT->IsInterface())
            ret |= CORINFO_FLG_INTERFACE;

        if (pMT->GetClass()->HasVarSizedInstances())
            ret |= CORINFO_FLG_VAROBJSIZE;

        if (pMT->IsValueClass())
        {
            ret |= CORINFO_FLG_VALUECLASS;

            if (pMT->ContainsStackPtr())
                ret |= CORINFO_FLG_CONTAINS_STACK_PTR;

            if (pMT->GetClass()->IsUnsafeValueClass())
            {
                ret |= CORINFO_FLG_UNSAFE_VALUECLASS;
            }
        }

        if (pMT->IsContextful())
            ret |= CORINFO_FLG_CONTEXTFUL;

        if (pMT->IsMarshaledByRef())
            ret |= CORINFO_FLG_MARSHAL_BYREF;

        if (pMT->ContainsPointers())
            ret |= CORINFO_FLG_CONTAINS_GC_PTR;

        if (pMT->IsAnyDelegateClass())
            ret |= CORINFO_FLG_DELEGATE;

        if (pMT->IsAbstract())
            ret |= CORINFO_FLG_ABSTRACT;

        if (pMT->IsSealed())
        {
            ret |= CORINFO_FLG_FINAL;
        }

        if (pMT->GetClass()->IsAlign8Candidate())
        {
            ret |= CORINFO_FLG_ALIGN8_HINT;
        }
    }

    EE_TO_JIT_TRANSITION();

    return(ret);
}

/*********************************************************************/
BOOL __stdcall CEEInfo::initClass (CORINFO_CLASS_HANDLE typeToInitHnd,
                                   CORINFO_METHOD_HANDLE methodBeingCompiledHnd,
                                   BOOL speculative,
                                   BOOL *pfNeedsInitFixup)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    if (pfNeedsInitFixup)
        *pfNeedsInitFixup = FALSE;

    JIT_TO_EE_TRANSITION();
    {

    // Do not bother figuring out the initialization if we are only verifying the method.
    if (isVerifyOnly())
    {
        result = TRUE;
        goto exit;
    }

    TypeHandle typeToInitTH = TypeHandle(typeToInitHnd);
    if (typeToInitTH.IsGenericVariable())
    {
        result = TRUE;
        goto exit;
    }

    MethodTable *pTypeToInitMT = typeToInitTH.GetMethodTable();
    if (pTypeToInitMT->IsClassPreInited())
    {
        // If there is no initialization then there really is nothing to do.
        result = TRUE;
        goto exit;
    }

    // Eagerly run the .cctor for the classes that are not value classes and
    // that have the BeforeFieldInit bit set.  (But don't run it if the
    // caller is app-domain-shared as it cannot help in the general case.
    // Also don't run it if the caller is generics-shared as the typeToInitHnd
    // may only be representative).
    //
    //
    // In the future, this routine should decide situationally as well as
    // whether it's legal to run the constructor.  (But to do that, we need
    // to know what field or method we are accessing, not just the class.)
    MethodDesc *methodBeingCompiled = GetMethod(methodBeingCompiledHnd);
    BOOL fSharedByGenerics = (pTypeToInitMT->IsSharedByGenericInstantiations() &&
        methodBeingCompiled->IsSharedByGenericInstantiations());

    if (methodBeingCompiled->IsDomainNeutral() ||
        !pTypeToInitMT->GetClass()->IsBeforeFieldInit() ||
        fSharedByGenerics)
    {
        // Well, because of code sharing or because of other initialization
        // semantics we can't do anything at JIT/PreJIT-link-restore time.   We have
        // to do it at runtime.
        result = FALSE;
        goto exit;
    }

    if (speculative)
    {
        // If we're not Jitting (speculative = TRUE) the fixups has to
        // take care of the initialization.
        if (pfNeedsInitFixup)
            *pfNeedsInitFixup = TRUE;

        result = TRUE;
        goto exit;
    }

    //
    //
    if (!methodBeingCompiled->GetLoaderModule()->GetDomainFile()->IsActive())
    {
        result = FALSE;
        goto exit;
    }

    //
    // Run the .cctor
    //
    OBJECTREF throwable = NULL;
    GCPROTECT_BEGIN(throwable);
    result = !FAILED(pTypeToInitMT->CheckRunClassInitNT(&throwable));
    GCPROTECT_END();

    }
exit: ;
    EE_TO_JIT_TRANSITION();

    return result;
}



void __stdcall CEEInfo::classMustBeLoadedBeforeCodeIsRun (CORINFO_CLASS_HANDLE typeToLoadHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    TypeHandle th = TypeHandle(typeToLoadHnd);

    if (th.IsGenericVariable())
    {
        //@GENERICSVER: (generic verification time) type variables do not need to be loaded
    }
    else
    {
    //
    // It is always safe to restore a class at jit time, as it happens
    // once per process and has no interesting side effects.
    //
    th.CheckRestore();
}

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
void __stdcall CEEInfo::methodMustBeLoadedBeforeCodeIsRun (CORINFO_METHOD_HANDLE methHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    MethodDesc *pMD = (MethodDesc*) methHnd;

    //
    // It is always safe to restore a method at jit time, as it happens
    // once per process and has no interesting side effects.
    //
    pMD->CheckRestore();

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
CORINFO_METHOD_HANDLE __stdcall CEEInfo::mapMethodDeclToMethodImpl(CORINFO_METHOD_HANDLE methHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_METHOD_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    MethodDesc *pMD = GetMethod(methHnd);
    pMD = MethodTable::MapMethodDeclToMethodImpl(pMD);
    result = (CORINFO_METHOD_HANDLE) pMD;

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
CORINFO_CLASS_HANDLE __stdcall CEEInfo::getBuiltinClass(CorInfoClassId classId)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = (CORINFO_CLASS_HANDLE) 0;

    JIT_TO_EE_TRANSITION();

    switch (classId)
    {
    case CLASSID_SYSTEM_OBJECT:
        result = CORINFO_CLASS_HANDLE(g_pObjectClass);
        break;
    case CLASSID_TYPED_BYREF:
        InitStaticTypeHandles();
        result = CORINFO_CLASS_HANDLE(s_pTypedReference);
        break;
    case CLASSID_TYPE_HANDLE:
        InitStaticTypeHandles();
        result = CORINFO_CLASS_HANDLE(s_pRuntimeTypeHandle);
        break;
    case CLASSID_FIELD_HANDLE:
        InitStaticTypeHandles();
        result = CORINFO_CLASS_HANDLE(s_pRuntimeFieldHandle);
        break;
    case CLASSID_METHOD_HANDLE:
        InitStaticTypeHandles();
        result = CORINFO_CLASS_HANDLE(s_pRuntimeMethodHandle);
        break;
    case CLASSID_ARGUMENT_HANDLE:
        InitStaticTypeHandles();
        result = CORINFO_CLASS_HANDLE(s_pRuntimeArgumentHandle);
        break;
    case CLASSID_STRING:
        result = CORINFO_CLASS_HANDLE(g_pStringClass);
        break;
    default:
        _ASSERTE(!"NYI: unknown classId");
        break;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}



/*********************************************************************/
CorInfoType __stdcall CEEInfo::getTypeForPrimitiveValueClass(
        CORINFO_CLASS_HANDLE clsHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoType result = CORINFO_TYPE_UNDEF;

    JIT_TO_EE_TRANSITION();

    TypeHandle th(clsHnd);
    _ASSERTE (!th.IsGenericVariable());

    MethodTable    *pMT = th.GetMethodTable();
    PREFIX_ASSUME(pMT != NULL);

    // Is it a non primitive struct such as
    // RuntimeTypeHandle, RuntimeMethodHandle, RuntimeArgHandle?
    if (pMT->IsValueType() &&
        !pMT->IsTruePrimitive()  &&
        !pMT->IsEnum())
    {
        // default value CORINFO_TYPE_UNDEF is what we want
    }
    else
    {
        switch (th.GetInternalCorElementType())
        {
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        case ELEMENT_TYPE_BOOLEAN:
            result = toJitType(ELEMENT_TYPE_I1);
            break;

        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        case ELEMENT_TYPE_CHAR:
            result = toJitType(ELEMENT_TYPE_I2);
            break;

        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
            result = toJitType(ELEMENT_TYPE_I4);
            break;

        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
            result = toJitType(ELEMENT_TYPE_I8);
            break;

        case ELEMENT_TYPE_I:
        case ELEMENT_TYPE_U:
            result = toJitType(ELEMENT_TYPE_I);
            break;

        case ELEMENT_TYPE_R4:
            result = toJitType(ELEMENT_TYPE_R4);
            break;

        case ELEMENT_TYPE_R8:
            result = toJitType(ELEMENT_TYPE_R8);
            break;

        case ELEMENT_TYPE_VOID:
            result = toJitType(ELEMENT_TYPE_VOID);
            break;

        case ELEMENT_TYPE_PTR:
        case ELEMENT_TYPE_FNPTR:
            result = toJitType(ELEMENT_TYPE_PTR);
            break;

        default:
            break;
        }
    }

    EE_TO_JIT_TRANSITION();

    return result;
}


void __stdcall CEEInfo::getGSCookie(GSCookie * pCookieVal, GSCookie ** ppCookieVal)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    if (pCookieVal)
    {
        *pCookieVal = GetProcessGSCookie();
        *ppCookieVal = NULL;
    }
    else
    {
        *ppCookieVal = GetProcessGSCookiePtr();
    }

    EE_TO_JIT_TRANSITION();
}


/*********************************************************************/
// TRUE if child is a subtype of parent
// if parent is an interface, then does child implement / extend parent
BOOL __stdcall CEEInfo::canCast(
        CORINFO_CLASS_HANDLE        child,
        CORINFO_CLASS_HANDLE        parent)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    result = ((TypeHandle)child).CanCastTo((TypeHandle)parent);

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// returns is the intersection of cls1 and cls2.
CORINFO_CLASS_HANDLE __stdcall CEEInfo::mergeClasses(
        CORINFO_CLASS_HANDLE        cls1,
        CORINFO_CLASS_HANDLE        cls2)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    result = CORINFO_CLASS_HANDLE(TypeHandle::MergeTypeHandlesToCommonParent(
            TypeHandle(cls1), TypeHandle(cls2)).AsPtr());

    EE_TO_JIT_TRANSITION();
    return result;
}

/*********************************************************************/
// Given a class handle, returns the Parent type.
// For COMObjectType, it returns Class Handle of System.Object.
// Returns 0 if System.Object is passed in.
CORINFO_CLASS_HANDLE __stdcall CEEInfo::getParentType(
            CORINFO_CLASS_HANDLE    cls)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    TypeHandle th(cls);

    _ASSERTE(!th.IsNull());
    _ASSERTE(!th.IsGenericVariable());

    TypeHandle thParent = th.GetParent();

    {
        result = CORINFO_CLASS_HANDLE(thParent.AsPtr());
    }

    EE_TO_JIT_TRANSITION();

    return result;
}


/*********************************************************************/
// Returns the CorInfoType of the "child type". If the child type is
// not a primitive type, *clsRet will be set.
// Given an Array of Type Foo, returns Foo.
// Given BYREF Foo, returns Foo
CorInfoType __stdcall CEEInfo::getChildType (
        CORINFO_CLASS_HANDLE       clsHnd,
        CORINFO_CLASS_HANDLE       *clsRet
        )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoType ret = CORINFO_TYPE_UNDEF;
    *clsRet = 0;
    TypeHandle  retType = TypeHandle();

    JIT_TO_EE_TRANSITION();

    OBJECTREF throwable = NULL;
    GCPROTECT_BEGIN(throwable);

    BOOL fErr = FALSE;

#ifdef VALUE_ARRAYS
    if (isValueArray(clsHnd)) {

        ValueArrayDescr* valArr = asValueArray(clsHnd);

        CorElementType type;
        SigPointer ptr(valArr->sig);    // make a copy

        while (ptr.PeekElemType() == ELEMENT_TYPE_PINNED)
        {
            ptr.GetElemType();
        }
        if (type == ELEMENT_TYPE_VALUEARRAY) {

            ValueArrayDescr* elem = allocDescr(ptr, valArr->module);
            // set low order bits to mark as a value class array descriptor
            *clsRet = markAsValueArray(elem);
            ret = toJitType(ELEMENT_TYPE_VALUEARRAY);

        } else {

            retType = ptr.GetTypeHandle(valArr->module, &throwable);

            if (retType.IsNull())
                fErr = TRUE;
        }
    }
    else
#endif // VALUE_ARRAYS
    {

        TypeHandle th(clsHnd);

        _ASSERTE(!th.IsNull());

        // BYREF, ARRAY types
        if (th.IsTypeDesc())
        {
            retType = th.AsTypeDesc()->GetTypeParam();
        }
        else
        {
            MethodTable* pMT= th.GetMethodTable();
            if (pMT->IsArray())
                retType = pMT->GetApproxArrayElementTypeHandle();
        }

    }

    if (fErr) {
        // If don't have a throwable, find out who didn't create one,
        // and fix it.
        _ASSERTE(throwable!=NULL);
        COMPlusThrow(throwable);
    }
    GCPROTECT_END();

    if (!retType.IsNull()) {
        CorElementType type = retType.GetInternalCorElementType();
        ret = toJitType(type,retType, clsRet);

    }

    EE_TO_JIT_TRANSITION();

    return ret;
}

/*********************************************************************/
// Check Visibility rules for accessing a type
//
// fullAccessCheck==FALSE means that we are doing limited accessibility
// checking introduced in V2. Post-V2, we should always do the full
// checks all the time.

BOOL CEEInfo::canAccessTypeInternal(
        CORINFO_METHOD_HANDLE       context,
        CORINFO_CLASS_HANDLE        target,
        BOOL                        fullAccessCheck)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL doAccessCheck = TRUE;

    MethodDesc* pContextMD = GetMethodFromContext(context);
    MethodTable *pMDType = GetTypeFromContext(context).GetMethodTable();

    if (pContextMD && IsDynamicMethodHandle(context))
    {
        TypeHandle typeOwner;
        
        DWORD dwSecurityFlags = 0;
        pContextMD->GetDynamicMethodDesc()->GetResolver()->GetJitContext(&dwSecurityFlags, &typeOwner);
        if (dwSecurityFlags & DynamicResolver::SkipVisibilityChecks)
        {
            doAccessCheck = FALSE;
        }

        if (!typeOwner.IsNull())
            pMDType = typeOwner.GetMethodTable();
    }

    BOOL canAccessType = TRUE;

    if (doAccessCheck)
    {
        if (fullAccessCheck)
        {
            canAccessType = ClassLoader::CanAccessClass(pContextMD, 
                                                 pMDType,
                                                 pMDType->GetAssembly(),
                                                 TypeHandle(target).GetMethodTable(),
                                                 TypeHandle(target).GetAssembly());
        }
        else
        {
            TypeHandle thTarget = TypeHandle(target);
            CorElementType corTypeTarget = thTarget.GetSignatureCorElementType();
            // TypeVarTypeDesc do not have a MethodTable, and they are always accessible
            if (corTypeTarget != ELEMENT_TYPE_VAR && corTypeTarget != ELEMENT_TYPE_MVAR)
            {
                canAccessType = ClassLoader::CanAccessClassForExtraChecks(pContextMD, 
                                                                          pMDType,
                                                                          pMDType->GetAssembly(),
                                                                          thTarget.GetMethodTable(),
                                                                          thTarget.GetAssembly());
            }
        }
    }

    return canAccessType;
}

/*********************************************************************/
// Check Visibility rules.
BOOL __stdcall CEEInfo::canAccessType(
        CORINFO_METHOD_HANDLE       context,
        CORINFO_CLASS_HANDLE        target)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL canAccessType = FALSE;
    
    JIT_TO_EE_TRANSITION();

    canAccessType = canAccessTypeInternal(context, target);

    EE_TO_JIT_TRANSITION();

    return canAccessType;
}
/*********************************************************************/
// Check any constraints on class type arguments
BOOL __stdcall CEEInfo::satisfiesClassConstraints(CORINFO_CLASS_HANDLE cls)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(cls != NULL);
    result = TypeHandle(cls).SatisfiesClassConstraints();

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Check if this is a single dimensional array type
BOOL __stdcall CEEInfo::isSDArray(CORINFO_CLASS_HANDLE  cls)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    TypeHandle th(cls);

    _ASSERTE(!th.IsNull());

    if (th.IsArrayType())
    {
        // Lots of code used to think that System.Array's methodtable returns TRUE for IsArray(). It doesn't.
        _ASSERTE(th != TypeHandle(g_pArrayClass));

        result = (th.GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Get the number of dimensions in an array
unsigned __stdcall CEEInfo::getArrayRank(CORINFO_CLASS_HANDLE  cls)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

    TypeHandle th(cls);

    _ASSERTE(!th.IsNull());

    if (th.IsArrayType())
    {
        // Lots of code used to think that System.Array's methodtable returns TRUE for IsArray(). It doesn't.
        _ASSERTE(th != TypeHandle(g_pArrayClass));

        result = th.GetPossiblySharedArrayMethodTable()->GetRank();
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Get static field data for an array
// Note that it's OK to return NULL from this method.  This will cause
// the JIT to make a runtime call to InitializeArray instead of doing
// the inline optimization (thus preserving the original behavior).
void * __stdcall CEEInfo::getArrayInitializationData(
            CORINFO_FIELD_HANDLE        field,
            DWORD                       size
            )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void * result = NULL;

    JIT_TO_EE_TRANSITION();

    FieldDesc* pField = (FieldDesc*) field;

    if (!pField                    ||
        !pField->IsRVA()           ||
        (pField->LoadSize() < size)
        )
    {
        result = NULL;
    }
    else
    {
        pField->GetApproxEnclosingMethodTable()->CheckRestore();  // <BUG> this probably should be Approx - see bug 184355 </BUG>
        result = pField->GetStaticAddressHandle(NULL);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Static helpers
/*********************************************************************/
MethodTable * CEEInfo::s_pRuntimeTypeHandle;
MethodTable * CEEInfo::s_pRuntimeMethodHandle;
MethodTable * CEEInfo::s_pRuntimeFieldHandle;
MethodTable * CEEInfo::s_pRuntimeArgumentHandle;
MethodTable * CEEInfo::s_pTypedReference;

/* static */
void CEEInfo::InitStaticTypeHandles()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    static bool fInited = false;

    if (fInited)
        return;

    if (s_pRuntimeTypeHandle == NULL)
        s_pRuntimeTypeHandle = g_Mscorlib.FetchClass(CLASS__TYPE_HANDLE);

    if (s_pRuntimeFieldHandle == NULL)
        s_pRuntimeFieldHandle = g_Mscorlib.FetchClass(CLASS__FIELD_HANDLE);

    if (s_pRuntimeMethodHandle == NULL)
        s_pRuntimeMethodHandle = g_Mscorlib.FetchClass(CLASS__METHOD_HANDLE);

    if (s_pRuntimeArgumentHandle == NULL)
        s_pRuntimeArgumentHandle = g_Mscorlib.FetchClass(CLASS__ARGUMENT_HANDLE);

    if (s_pTypedReference == NULL)
        s_pTypedReference = g_Mscorlib.FetchClass(CLASS__TYPED_REFERENCE);

    fInited = true;
}

/*********************************************************************
 * This method returns TRUE only for primitive types like I1, I2.. R8 etc
 * Returns FALSE if the types are OBJREF, VALUECLASS etc.
 */
/* static */
BOOL CEEInfo::CanCast(CorElementType el1, CorElementType el2)
{
    CONTRACTL {
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    if (el1 == el2)
        return CorIsPrimitiveType(el1);

    switch (el1)
    {
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        return (el2 == ELEMENT_TYPE_I1 ||
                el2 == ELEMENT_TYPE_U1 ||
                el2 == ELEMENT_TYPE_BOOLEAN);

    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        return (el2 == ELEMENT_TYPE_I2 ||
                el2 == ELEMENT_TYPE_U2 ||
                el2 == ELEMENT_TYPE_CHAR);

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
        return (el2 == ELEMENT_TYPE_I4 ||
                el2 == ELEMENT_TYPE_U4);

    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
        return (el2 == ELEMENT_TYPE_I8 ||
                el2 == ELEMENT_TYPE_U8);

    case ELEMENT_TYPE_I:
    case ELEMENT_TYPE_U:
        return  (el2 == ELEMENT_TYPE_I ||
                 el2 == ELEMENT_TYPE_U);

    case ELEMENT_TYPE_R4:
        return (el2 == ELEMENT_TYPE_R4);

    case ELEMENT_TYPE_R8:
        return (el2 == ELEMENT_TYPE_R8);

    default:
        break;
    }

    return FALSE;
}

/***********************************************************************/
unsigned __stdcall CEEInfo::getInterfaceTableOffset (CORINFO_CLASS_HANDLE       clsHnd,
                                                     void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

    TypeHandle  VMClsHnd(clsHnd);

        // Sanity test for class
    _ASSERTE(VMClsHnd.GetClass()->GetMethodTable()->GetClass() == VMClsHnd.GetClass());

     // Make sure we are restored so our interface ID is valid
    _ASSERTE(VMClsHnd.AsMethodTable()->IsRestored());

    _ASSERTE(VMClsHnd.GetClass()->IsInterface());

    _ASSERTE(!"STUB_DISPATCH and CEEInfo::getInterfaceTableOffset");
    result = (unsigned) -1;

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
// return the address of a pointer to a callable stub that will do the
// virtual or interface call
void __stdcall CEEInfo::getCallInfo(
            CORINFO_METHOD_HANDLE methodBeingCompiledHnd,
            CORINFO_MODULE_HANDLE  tokenScope,
            unsigned               methodToken,
            unsigned               constrainedCallToken,
            CORINFO_CONTEXT_HANDLE tokenContext,
            CORINFO_CALLINFO_FLAGS flags,
            CORINFO_CALL_INFO     *pResult)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    memset(pResult, 0, sizeof(*pResult));

    JIT_TO_EE_TRANSITION();

    CONSISTENCY_CHECK(CheckPointer(pResult));
    CONSISTENCY_CHECK(CheckContext(tokenScope, tokenContext));

    VarKind varKind = hasNoVars;

    CORINFO_ANNOTATION annotation = (flags & CORINFO_CALLINFO_ALLOWINSTPARAM) ? CORINFO_ANNOT_ALLOWINSTPARAM 
                                                                              : CORINFO_ANNOT_NONE;

    // Class or method type parameters present in token
    MethodDesc *pMD = NULL;
    loadTokenInternal(tokenScope, annotation, methodToken,
                      tokenContext, CORINFO_TOKENKIND_Default,
                      NULL, &pMD, NULL);

    if (!pMD)
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);

    if (!IsDynamicScope(tokenScope)) 
    {
        varKind = ScanToken(GetModule(tokenScope), methodToken, tokenContext, TRUE);
    }


    TypeHandle constrainedType;
    if ((flags & CORINFO_CALLINFO_CALLVIRT) && constrainedCallToken)
    {
        loadTokenInternal(tokenScope, CORINFO_ANNOT_NONE, constrainedCallToken,
                          tokenContext, CORINFO_TOKENKIND_Default,
                          &constrainedType, NULL, NULL);

        // For virtual calls through value types (interface or System.Object), the constrained type might be
        // obtained from a token that involves shared type parameters, and this must
        // be resolved at runtime. So roll in this information into varKind.
        // Example: constrained. valuetype VC<!U>    
        //          callvirt void I<!T>::Foo()
        // If VC implements I at multiple instantiations then we need to take VC<!U> into account at runtime
        
        BOOL virtualcall = pMD->GetMethodTable()->IsInterface() ||
            (pMD->IsVirtual() && !pMD->IsFinal() && !pMD->GetMethodTable()->IsSealed());

        if (virtualcall && constrainedType.IsValueType())
        {                
            if (!IsDynamicScope(tokenScope)) 
            {
                varKind = (VarKind)(varKind|ScanToken(GetModule(tokenScope), constrainedCallToken, tokenContext, FALSE));
            }
        }
    }

    TypeHandle ownerType = TypeHandle(findClass(tokenScope, getMemberParent(tokenScope, methodToken), tokenContext));

    GetCallInfoHelper(methodBeingCompiledHnd,
                      ownerType,
                      CORINFO_METHOD_HANDLE(pMD),
                      flags,
                      constrainedType,
                      varKind,
                      GetModule(tokenScope),
                      methodToken,
                      constrainedCallToken,
                      tokenContext,
                      pResult);

    EE_TO_JIT_TRANSITION();
}


/***********************************************************************/
// A helper used by getCallInfo and CEEDynamicCodeInfo
/* static */
void CEEInfo::GetCallInfoHelper(
            CORINFO_METHOD_HANDLE   methodBeingCompiled,
            TypeHandle             ownerType,
            CORINFO_METHOD_HANDLE   methHnd,
            CORINFO_CALLINFO_FLAGS flags,
            TypeHandle             constrainedType,
            VarKind varKind,
            Module * pModule, // only needed if varKind != hasNoVars, i.e. calling from shared generic code
            mdToken                methodToken,    // only needed if varKind != hasNoVars, i.e. calling from shared generic code
            mdToken                constrainedCallToken,// only needed if varKind != hasNoVars, i.e. calling from shared generic code
            CORINFO_CONTEXT_HANDLE tokenContext,   // only needed if varKind != hasNoVars, i.e. calling from shared generic code
            CORINFO_CALL_INFO     *pResult)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pResult));
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    bool callvirt = (flags & CORINFO_CALLINFO_CALLVIRT) != 0;

    memset(pResult, 0, sizeof(*pResult));

    MethodDesc* pMD = GetMethod(methHnd);

    MethodDesc *pMDAfterConstraintResolution;

    if (constrainedType.IsNull())
    {
        pMDAfterConstraintResolution = pMD;
        pResult->thisTransform = CORINFO_NO_THIS_TRANSFORM;
    }
    else if (constrainedType.ContainsGenericVariables() || ownerType.ContainsGenericVariables())
    {
        // <NICE> It shouldn't really matter what we do here - but the x86 JIT is annoyingly sensitive
        // about what we do, since it pretend generic variables are reference types and generates
        // an internal JIT tree even when just verifying generic code. </NICE>
        pMDAfterConstraintResolution = pMD;
        if (constrainedType.IsGenericVariable())
        {
            pResult->thisTransform = CORINFO_DEREF_THIS; // convert 'this' of type &T --> T
        }
        else if (constrainedType.IsValueType())
        {
            pResult->thisTransform = CORINFO_BOX_THIS; // convert 'this' of type &VC<T> --> boxed(VC<T>)
        }
        else
        {
            pResult->thisTransform = CORINFO_DEREF_THIS; // convert 'this' of type &C<T> --> C<T>
        }

    }
    else
    {
        // We have a "constrained." call.  Try a partial resolve of the constraint call.  Note that this
        // will not necessarily resolve the call exactly, since we might be compiling
        // shared generic code - it may just resolve it to a candidate suitable for
        // JIT compilation, and require a runtime lookup for the actual code pointer
        // to call.
        MethodDesc *directMethod = constrainedType.GetMethodTable()->TryResolveConstraintMethodApprox(ownerType, pMD);
        if (directMethod)
        {

            // Either
            //    1. no constraint resolution at compile time (!directMethod)
            // OR 2. no code sharing lookup in call
            // OR 3. we have have resolved to an instantiating stub
            // _ASSERTE(((varKind & hasSharableVarsMask) == 0) || directMethod->IsInstantiatingStub());

            pMDAfterConstraintResolution = directMethod;
            _ASSERTE(!pMDAfterConstraintResolution->IsInterface());
            callvirt = false;
            pResult->thisTransform = CORINFO_NO_THIS_TRANSFORM;
        }
        else  if (constrainedType.IsValueType())
        {
            pMDAfterConstraintResolution = pMD;
            pResult->thisTransform = CORINFO_BOX_THIS;
        }
        else
        {
            pMDAfterConstraintResolution = pMD;
            pResult->thisTransform = CORINFO_DEREF_THIS;
        }
    }

    pResult->targetMethodHandle = CORINFO_METHOD_HANDLE(pMDAfterConstraintResolution);

    // We don't allow a JIT to call the code directly if a runtime lookup is
    // needed. This is the case if
    //     1. the scan of the call token indicated that it involves code sharing
    // AND 2. the method is an instantiating stub
    //
    // In these cases the correct instantiating stub is only found via a runtime lookup.
    //
    // Note that most JITs don't call instantiating stubs directly if they can help it -
    // they call the underlying shared code and provide the type context parameter
    // explicitly. However
    //    (a) some JTs always call instantiating stubs (it makes the JIT simpler) and
    //    (b) if the method is a remote call or a security stub then the EE will force the
    //        call through an instantiating stub and
    //    (c) at the moment constraint calls are never resolved to underlying
    //        shared generic code (i.e. are never RequiresInstArg())

    BOOL useRuntimeLookupForInstantiatingStub =
       ((varKind & hasSharableVarsMask) != 0) && pMDAfterConstraintResolution->IsInstantiatingStub();

    // Is this a virtual call that can be resolved to resolve at compile time or dictionary-lookup-time, e.g. because class is sealed??
    BOOL resolvedcallvirt = callvirt && !pMDAfterConstraintResolution->GetMethodTable()->IsInterface() &&
        (!pMDAfterConstraintResolution->IsVirtual() || pMDAfterConstraintResolution->IsFinal() || pMDAfterConstraintResolution->GetMethodTable()->IsSealed());

    // We can't allow generic remotable methods to be considered resolved as above, it leads to a non-instantiating method desc being
    // passed to the remoting stub. The easiest way to deal with these is to force them through the virtual code path.
    // It is actually good to do this deoptimization for all remotable methods since remoting interception via virtual dispatch 
    // is faster then remoting interception via thunk
    if (resolvedcallvirt && pMDAfterConstraintResolution->IsRemotingInterceptedViaVirtualDispatch()
            /* && pMDAfterConstraintResolution->HasMethodInstantiation() */ )
        resolvedcallvirt = false;

    // Force all interface calls to be interpreted as if they are virtual.
    if (!pMDAfterConstraintResolution->IsStatic() && pMDAfterConstraintResolution->GetMethodTable()->IsInterface())
        callvirt = true;

    if (!callvirt || resolvedcallvirt)
    {
        if (useRuntimeLookupForInstantiatingStub)
        {
            _ASSERTE(!GetMethod(methodBeingCompiled)->IsDynamicMethod());
            pResult->kind = CORINFO_CALL_CODE_POINTER;

            _ASSERTE ((varKind & hasSharableVarsMask) != 0);
            MethodDesc *pContextMD = GetMethodFromContext(tokenContext);

            // For reference types, the constrained type does not affect method resolution
            if (!constrainedType.IsNull() && !constrainedType.IsValueType())
            {
                constrainedCallToken = 0;
            }

            DictionaryEntryLayout entryLayout(CORINFO_ANNOT_ENTRYPOINT | methodToken, constrainedCallToken);

            PREFIX_ASSUME(pContextMD != NULL);
            ComputeRuntimeLookupForSharedGenericToken(pModule,
                                                      &entryLayout,
                                                      pContextMD,
                                                      varKind,
                                                      &pResult->codePointerLookup);
        }
        else
        {
            pResult->kind = CORINFO_CALL;
        }
        pResult->nullInstanceCheck = resolvedcallvirt;
    }


    // All virtual calls which take method instantiations must
    // currently be implemented by an indirect call via a runtime-lookup
    // function pointer
    else if (pMDAfterConstraintResolution->HasMethodInstantiation())
    {
        _ASSERTE(callvirt);
        pResult->kind = CORINFO_VIRTUALCALL_LDVIRTFTN;  // stub dispatch can't handle generic method calls yet
        pResult->nullInstanceCheck = TRUE;
    }
    else
    {
        _ASSERTE(callvirt);
        pResult->nullInstanceCheck = TRUE;

        if (pMDAfterConstraintResolution->IsInterface())
        {
        // Sometimes we can't make stub calls, i.e. when we need exact information
        // for interface calls from shared code.
        if (
            // We only need exact information for interface calls.
            pMDAfterConstraintResolution->IsInterface()
            && g_pConfig->ExactInterfaceCalls()

            // If the token is not shared then we don't need a runtime lookup
            && (varKind & hasSharableVarsMask)  )
        {
            _ASSERTE(!GetMethod(methodBeingCompiled)->IsDynamicMethod());
            
            MethodDesc *pContextMD = GetMethodFromContext(tokenContext);
            pResult->kind = CORINFO_VIRTUALCALL_STUB;
            // "kindOnly" is passed by the 64-bit jit and NGEN, and for NGEN it includes
            // computing the runtime lookup path.  Thus the name is
            // a little misleading....
            //if (!(flags & CORINFO_CALLINFO_KINDONLY))
            {
                DictionaryEntryLayout entryLayout(CORINFO_ANNOT_DISPATCH_STUBADDR | methodToken, 0);

                ComputeRuntimeLookupForSharedGenericToken(pModule,
                    &entryLayout,
                    pContextMD,
                    varKind,
                    &pResult->stubLookup);
            }
        }
        else
        {
            pResult->kind = CORINFO_VIRTUALCALL_STUB;
            pResult->stubLookup.lookupKind.needsRuntimeLookup = false;
            if (!(flags & CORINFO_CALLINFO_KINDONLY))
            {
                MethodDesc* pMethodBeingCompiled = GetMethod(methodBeingCompiled);
                BaseDomain *pDomain = pMethodBeingCompiled->GetDomain();
                VirtualCallStubManager *pMgr = pDomain->GetVirtualCallStubManager();

                TADDR addr = (TADDR)pMgr->GetCallStub(ownerType, pMDAfterConstraintResolution);
                _ASSERTE(pMgr->isStub(addr));

                // We use an indirect call
                pResult->stubLookup.constLookup.accessType = IAT_PVALUE;

                pResult->stubLookup.constLookup.accessModule = IAM_CURRENT_MODULE;

                // Now we want to indirect through a cell so that updates can take place atomically.
                pResult->stubLookup.constLookup.addr = pMgr->GenerateStubIndirection((BYTE *) addr);
            }
        }
        }
        else
        {
            pResult->kind = CORINFO_VIRTUALCALL_VTABLE;
        }
    }

}


BOOL __stdcall CEEInfo::isRIDClassDomainID(CORINFO_CLASS_HANDLE cls)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    TypeHandle  VMClsHnd(cls);

        // Sanity test for class
    _ASSERTE(VMClsHnd.GetClass()->GetMethodTable()->GetClass() == VMClsHnd.GetClass());

    result = !VMClsHnd.AsMethodTable()->IsDynamicStatics();

    EE_TO_JIT_TRANSITION();

    return result;
    }


/***********************************************************************/
unsigned __stdcall CEEInfo::getClassDomainID (CORINFO_CLASS_HANDLE clsHnd,
                                              void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    TypeHandle  VMClsHnd(clsHnd);

        // Sanity test for class
    _ASSERTE(VMClsHnd.GetClass()->GetMethodTable()->GetClass() == VMClsHnd.GetClass());

    if (VMClsHnd.AsMethodTable()->IsDynamicStatics())
    {
        result = (unsigned)VMClsHnd.AsMethodTable()->GetModuleDynamicEntryID();
    }
    else
    {
        result = (unsigned)VMClsHnd.AsMethodTable()->GetClassIndex();
    }

    EE_TO_JIT_TRANSITION();

    return result;
}



/***********************************************************************/
size_t __stdcall CEEInfo::getModuleDomainID (CORINFO_MODULE_HANDLE    module,
                                              void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    size_t result = 0;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    Module *pModule = (Module*)module;
    result = pModule->GetModuleID();

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getNewHelper (CORINFO_CLASS_HANDLE newClsHnd, CORINFO_METHOD_HANDLE context, mdToken classToken, CORINFO_MODULE_HANDLE tokenContext)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();

    TypeHandle  VMClsHnd(newClsHnd);

    if(VMClsHnd.IsTypeDesc())
    {
        COMPlusThrow(kInvalidOperationException,L"InvalidOperation_CantInstantiateFunctionPointer");
    }

    // Sanity test for class
    _ASSERTE(VMClsHnd.GetClass()->GetMethodTable()->GetClass() == VMClsHnd.GetClass());

    if(VMClsHnd.IsAbstract())
    {
        COMPlusThrow(kInvalidOperationException,L"InvalidOperation_CantInstantiateAbstractClass");
    }
    if (!IsDynamicScope(tokenContext))
        AddDependencyOnClassToken(GetModule(tokenContext),classToken,TRUE);
    MethodTable* pMT = VMClsHnd.AsMethodTable();

    if(CRemotingServices::IsRemoteActivationRequired(VMClsHnd))
    {
        result = CORINFO_HELP_NEW_CROSSCONTEXT;
    }
    else
    {
        CorInfoHelpFunc slowHelper;

        if (pMT->IsAnyDelegateClass())
        {
            result     = CORINFO_HELP_NEWSFAST_CHKRESTORE;
            slowHelper = CORINFO_HELP_NEW_DIRECT;
        }
        else
        {
            result     = CORINFO_HELP_NEWSFAST;
            slowHelper = CORINFO_HELP_NEWFAST;
        }

        // We shouldn't get here with a COM object (they're all potentially
        // remotable, so they're covered by the case above).
        _ASSERTE(!pMT->IsComObjectType());

        if (GCHeap::IsLargeObject(pMT) ||
            pMT->HasFinalizer())
        {
    #ifdef _DEBUG
            //printf("NEWFAST:   %s\n", pMT->GetDebugClassName());
    #endif
            result = slowHelper;
        }

    #ifdef STRESS_HEAP
            // don't call the super-optimized one since that does not check
            // for GCStress
        if (g_pConfig->GetGCStressLevel() & EEConfig::GCSTRESS_ALLOC)
        {
            result = slowHelper;
        }
    #endif // STRESS_HEAP

    #ifdef _LOGALLOC
    #ifdef LOGGING
        // Super fast version doesn't do logging
        if (LoggingOn(LF_GCALLOC, LL_INFO10))
        {
            result = slowHelper;
        }
    #endif // LOGGING
    #endif // _LOGALLOC

        // Don't use the SFAST allocator when tracking object allocations,
        // so we don't have to instrument it.
        if (TrackAllocationsEnabled() || CORProfilerTrackGC())
        {
            result = slowHelper;
        }

        // If we are use the the fast allocator we also may need the 
        // specialized varion for align8
        if (VMClsHnd.GetClass()->IsAlign8Candidate() &&
            (result == CORINFO_HELP_NEWSFAST))
        {
            result = CORINFO_HELP_NEWSFAST_ALIGN8;
        }
    }

    _ASSERTE(result != CORINFO_HELP_UNDEF);
        
    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getNewArrHelper (CORINFO_CLASS_HANDLE arrayClsHnd,
                                          CORINFO_METHOD_HANDLE context)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();

    TypeHandle arrayType(arrayClsHnd);
    ArrayTypeDesc* arrayTypeDesc = arrayType.AsArray();
    PREFIX_ASSUME(arrayTypeDesc->GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY);

#ifdef STRESS_HEAP
    if (g_pConfig->GetGCStressLevel() & EEConfig::GCSTRESS_ALLOC)
    {
        result = CORINFO_HELP_NEWARR_1_DIRECT;
    }
    else
#endif // STRESS_HEAP
    {
        CorElementType elemType = arrayTypeDesc->GetTypeParam().GetInternalCorElementType();

        // This is if we're asked for newarr !0 when verifying generic code
        // Of course ideally you wouldn't even be generating code when
        // simply doing verification (we run the JIT importer in import-only
        // mode), but importing does more than one would like so we try to be
        // tolerant when asked for non-sensical helpers.
        if (arrayTypeDesc->GetTypeParam().IsGenericVariable())
        {
            result = CORINFO_HELP_NEWARR_1_OBJ;
        }
        else if (arrayTypeDesc->GetMethodTable()->IsSharedByReferenceArrayTypes())
        {
            // It is an array of object refs
            _ASSERTE(CorTypeInfo::IsObjRef(elemType));
            result = CORINFO_HELP_NEWARR_1_OBJ;
        }
        else
        {
            _ASSERTE(!CorTypeInfo::IsObjRef(elemType));
            
            BOOL initClassRequired = FALSE;

            TypeHandle elementType = arrayTypeDesc->GetTypeParam();
            MethodTable* elementTypeMT = elementType.GetMethodTable();

            if (!elementTypeMT->GetClass()->IsBeforeFieldInit())
            {
                EX_TRY
                {
                    // this can throw an exception that will cause the JITing of this method to fail
                    initClassRequired = !m_pOverride->initClass(CORINFO_CLASS_HANDLE(elementType.AsPtr()),
                                                                context, FALSE);
                }
                EX_CATCH
                {
                    initClassRequired = TRUE;
                }
                EX_END_CATCH(SwallowAllExceptions);
            }
            
            // These cases always must use the slow helper
            if (initClassRequired ||
                (elementType.GetSize() > LARGE_ELEMENT_SIZE) ||
                (elemType == ELEMENT_TYPE_VOID) ||
                LoggingOn(LF_GCALLOC, LL_INFO10) ||
                TrackAllocationsEnabled())
            {
                // Use the slow helper
                result = CORINFO_HELP_NEWARR_1_DIRECT;
            }
            else if (elemType == ELEMENT_TYPE_R8)
            {
                // Use the Align8 fast helper
                result = CORINFO_HELP_NEWARR_1_ALIGN8;
            }
            else
            {
                // Yea, we can do it the fast way!
                result = CORINFO_HELP_NEWARR_1_VC;
            }
        }
    }

    _ASSERTE(result != CORINFO_HELP_UNDEF);

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************
// Return the best helper to use to allocate a multi-dimensional array
// Helper is of type pHlpNewMDArr.
*/

CorInfoHelpFunc CEEInfo::getNewMDArrHelper(CORINFO_CLASS_HANDLE arrayCls,
                                           CORINFO_METHOD_HANDLE arrayCtorMethod,
                                           CORINFO_METHOD_HANDLE context)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();

    MethodDesc *pMethod = GetMethod(arrayCtorMethod);
    Module *pModule = pMethod->GetModule();

    _ASSERTE(!pMethod->IsStatic());
    // Should be using one of the fast new helpers, if you aren't an array
    _ASSERTE(pMethod->GetMethodTable()->IsArray());

    TypeHandle arrayType(arrayCls);
    ArrayTypeDesc* arrayTypeDesc = arrayType.AsArray();

    _ASSERTE(arrayTypeDesc->GetMethodTable() == pMethod->GetMethodTable());

    // Does the constructor method take lower bounds (in addition to the dimensions) ?

    unsigned arrayRank = arrayTypeDesc->GetRank();

    PCCOR_SIGNATURE pSig;
    DWORD cbSigSize;

    pMethod->GetSig(&pSig, &cbSigSize);

    MetaSig msig(pSig, cbSigSize, pModule, NULL);

    unsigned dwNumArgs = msig.NumFixedArgs();

    _ASSERTE(dwNumArgs == arrayRank || dwNumArgs == 2*arrayRank ||                  // multidim array cases
             arrayTypeDesc->GetInternalCorElementType() == ELEMENT_TYPE_SZARRAY);   // jagged array case

    if (dwNumArgs == arrayRank)
    {
        result = CORINFO_HELP_NEW_MDARR_NO_LBOUNDS;
    }
    else
    {
        result = CORINFO_HELP_NEW_MDARR;
    }

    _ASSERTE(result != CORINFO_HELP_UNDEF);
        
    EE_TO_JIT_TRANSITION();
        
    return result;
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getCastingHelper(
        CORINFO_MODULE_HANDLE  scopeHnd,
        unsigned      metaTOK,
        CORINFO_CONTEXT_HANDLE context,
        bool fThrowing )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();


    // Check if it's a type variable
    if (result == CORINFO_HELP_UNDEF && TypeFromToken(metaTOK) == mdtTypeSpec)
    {
        ULONG cSig;
        PCCOR_SIGNATURE pSig;
        Module *pModule = GetModule(scopeHnd);
        pModule->GetMDImport()->GetTypeSpecFromToken(metaTOK, &pSig, &cSig);
        SigPointer sigptr = SigPointer(pSig);
        CorElementType type;
        IfFailThrow(sigptr.GetElemType(&type));
        // In shared code just use the catch-all helper for type variables, as the same
        // code may be used for interface/array/class instantiations
        if ((type == ELEMENT_TYPE_VAR || type == ELEMENT_TYPE_MVAR) && ContextIsShared(context))
        {
            result = fThrowing ? CORINFO_HELP_CHKCASTANY : CORINFO_HELP_ISINSTANCEOFANY;
        }
    }

    // Otherwise just look up the handle
    if (result == CORINFO_HELP_UNDEF)
    {
        CORINFO_CLASS_HANDLE IsInstClsHnd = findClass(scopeHnd, metaTOK, context, CORINFO_TOKENKIND_Casting);
        TypeHandle  clsHnd(IsInstClsHnd);
    
        if (clsHnd.IsInterface() && !clsHnd.AsMethodTable()->HasVariance())
        {
            // If it is a non-variant interface, use the fast interface helper
            result = fThrowing ? CORINFO_HELP_CHKCASTINTERFACE : CORINFO_HELP_ISINSTANCEOFINTERFACE;
        }
        else if (clsHnd.IsArray())
        {
            // If it is an array, use the fast array helper
            result = fThrowing ? CORINFO_HELP_CHKCASTARRAY : CORINFO_HELP_ISINSTANCEOFARRAY;
        }
        else if (!clsHnd.IsTypeDesc() && !clsHnd.AsMethodTable()->HasVariance()  && 
            !Nullable::IsNullableType(clsHnd))
        {
                
            // If it is a non-variant class, use the fast class helper
            result = fThrowing ? CORINFO_HELP_CHKCASTCLASS : CORINFO_HELP_ISINSTANCEOFCLASS;
        }
        else
        {
            // Otherwise, use the slow helper
            result = fThrowing ? CORINFO_HELP_CHKCASTANY : CORINFO_HELP_ISINSTANCEOFANY;
        }
    }

    _ASSERTE(result != CORINFO_HELP_UNDEF);

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getIsInstanceOfHelper(CORINFO_MODULE_HANDLE  scopeHnd,
                                               unsigned      metaTOK,
                                               CORINFO_CONTEXT_HANDLE context)
{
    WRAPPER_CONTRACT;
    return getCastingHelper(scopeHnd, metaTOK, context, false);
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getChkCastHelper(CORINFO_MODULE_HANDLE  scopeHnd,
                                          unsigned      metaTOK,
                                          CORINFO_CONTEXT_HANDLE context)
{
    WRAPPER_CONTRACT;
    return getCastingHelper(scopeHnd, metaTOK, context, true);
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getStaticsCCtorHelper(CORINFO_FIELD_HANDLE fldHnd, CORINFO_CLASS_HANDLE clsHnd, BOOL fRuntimeLookup)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();

    TypeHandle cls(clsHnd);
    BOOL fGCStatic = FALSE;

    if (fldHnd != NULL)
    {
        FieldDesc* field = (FieldDesc*) fldHnd;

        if (field->GetFieldType() == ELEMENT_TYPE_CLASS ||
            field->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
        {
            fGCStatic = TRUE;
        }

        _ASSERTE(cls == NULL);
        cls = TypeHandle(field->GetApproxEnclosingMethodTable());
    }

    MethodTable* pMT = cls.AsMethodTable();

    if (!pMT->IsDynamicStatics())
    {
        if (pMT->HasClassConstructor() ||
            (pMT->GetNumBoxedStatics() > 0))
        {
            result = fGCStatic ? CORINFO_HELP_GETSHARED_GCSTATIC_BASE :
                               CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE;
        }
        else
        {
            result = fGCStatic ? CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR :
                               CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR;
        }
    }
    else
    {
        if (fRuntimeLookup || (isVerifyOnly() && pMT->HasGenericsStaticsInfo()))
        {
            _ASSERTE(pMT->HasGenericsStaticsInfo());

            result = fGCStatic ? CORINFO_HELP_GETGENERICS_GCSTATIC_BASE :
                            CORINFO_HELP_GETGENERICS_NONGCSTATIC_BASE;
        }
        else
        {
            _ASSERTE(!cls.ContainsGenericVariables());
            _ASSERTE(pMT->GetModuleDynamicEntryID() != (unsigned) -1);

            result = fGCStatic ? CORINFO_HELP_GETSHARED_GCSTATIC_BASE_DYNAMICCLASS :
                           CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_DYNAMICCLASS;
        }
    }

    _ASSERTE(result != CORINFO_HELP_UNDEF);

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getSharedStaticBaseHelper(CORINFO_FIELD_HANDLE fldHnd, BOOL runtimeLookup)
{
    WRAPPER_CONTRACT;
    return getStaticsCCtorHelper(fldHnd, NULL, runtimeLookup);
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getSharedCCtorHelper(CORINFO_CLASS_HANDLE clsHnd)
{
    WRAPPER_CONTRACT;
    return getStaticsCCtorHelper(NULL, clsHnd, FALSE);
}

/***********************************************************************/
CorInfoHelpFunc __stdcall CEEInfo::getUnBoxHelper(CORINFO_CLASS_HANDLE clsHnd, BOOL* helperCopies)
{
    WRAPPER_CONTRACT;

    if (helperCopies != NULL)
        *helperCopies = FALSE;
    TypeHandle VMClsHnd(clsHnd);
    if (Nullable::IsNullableType(VMClsHnd))
        return CORINFO_HELP_UNBOX_NULLABLE;
    
    return CORINFO_HELP_UNBOX;
}


/***********************************************************************/
CORINFO_CLASS_HANDLE  CEEInfo::getTypeForBox(CORINFO_CLASS_HANDLE  cls)
{
    WRAPPER_CONTRACT;

    TypeHandle VMClsHnd(cls);
    if (Nullable::IsNullableType(VMClsHnd)) {
        VMClsHnd = VMClsHnd.AsMethodTable()->GetInstantiation()[0];
    }    
    return static_cast<CORINFO_CLASS_HANDLE>(VMClsHnd.AsPtr());
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getBoxHelper(CORINFO_CLASS_HANDLE clsHnd)
{
    WRAPPER_CONTRACT;

    TypeHandle VMClsHnd(clsHnd);
    if (Nullable::IsNullableType(VMClsHnd))
        return CORINFO_HELP_BOX_NULLABLE;
    
    return CORINFO_HELP_BOX;
}

/***********************************************************************/
CorInfoHelpFunc CEEInfo::getSecurityHelper(CORINFO_METHOD_HANDLE ftn, BOOL fEnter)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();


    if (result == CORINFO_HELP_UNDEF)
    {
        result = fEnter ? CORINFO_HELP_SECURITY_PROLOG : CORINFO_HELP_SECURITY_EPILOG;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
// This returns the typedesc from a method token.
CORINFO_CLASS_HANDLE __stdcall CEEInfo::findMethodClass(CORINFO_MODULE_HANDLE scopeHnd,
                                                        mdToken methodTOK,
                                                        CORINFO_METHOD_HANDLE context)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicScope(scopeHnd))
    {
        result = CEEDynamicCodeInfo::findMethodClass(scopeHnd, methodTOK, context);
    }
    else
    {
        Module* module = GetModule(scopeHnd);
        IMDInternalImport *pInternalImport = module->GetMDImport();

        _ASSERTE(TypeFromToken(methodTOK) == mdtMemberRef);

        SigTypeContext typeContext;
        GetTypeContext(context, &typeContext);

        mdTypeDef typeSpec;

        HRESULT hr;
        hr = pInternalImport->GetParentToken(methodTOK, &typeSpec);
        _ASSERTE(SUCCEEDED(hr));

        TypeHandle type = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(
                                           module, typeSpec, &typeContext,
                                                           ClassLoader::ThrowIfNotFound,
                                                           ClassLoader::FailIfUninstDefOrRef);

        result = (CORINFO_CLASS_HANDLE(type.AsPtr()));
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
// registers a vararg sig & returns a class-specific cookie for it.

CORINFO_VARARGS_HANDLE __stdcall CEEInfo::getVarArgsHandle(CORINFO_SIG_INFO *sig,
                                                           void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_VARARGS_HANDLE result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    Module* module = GetModule(sig->scope);

    result = CORINFO_VARARGS_HANDLE(module->GetVASigCookie((PCCOR_SIGNATURE)sig->sig));

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
unsigned __stdcall CEEInfo::getMethodHash (CORINFO_METHOD_HANDLE ftnHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = 0;

    JIT_TO_EE_TRANSITION();

    MethodDesc* ftn = GetMethod(ftnHnd);

    result = (unsigned) ftn->GetStableHash();

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
const char* __stdcall CEEInfo::getMethodName (CORINFO_METHOD_HANDLE ftnHnd, const char** scopeName)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    const char* result = NULL;

    JIT_TO_EE_TRANSITION();

    MethodDesc *ftn;

    ftn = GetMethod(ftnHnd);

    if (scopeName != 0)
    {
        if (ftn->IsLCGMethod())
        {
            *scopeName = "DynamicClass";
        }
        else
        {
            TypeHandle t = TypeHandle(ftn->GetMethodTable());
            *scopeName = "";
            if (!t.IsNull())
            {
// Turning this off for now because it causes a perf issue from our conversion
// to using SString. When SString debug performance issues are resolved, we can
// turn this back on.
                // since this is for diagnostic purposes only,
                // give up on the namespace, as we don't have a buffer to concat it
                // also note this won't show array class names.
                LPCUTF8 nameSpace;
                *scopeName= t.GetClass()->GetFullyQualifiedNameInfo(&nameSpace);
            }
        }
    }

    result = ftn->GetName();

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
//
// The callerHnd can be either the methodBeingCompiled or the immediate
// caller of an inlined function.
DWORD __stdcall CEEInfo::getMethodAttribs (CORINFO_METHOD_HANDLE calleeHnd, CORINFO_METHOD_HANDLE callerHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    DWORD result = 0;

    JIT_TO_EE_TRANSITION();
    {

/*
        returns method attribute flags (defined in corhdr.h)

        NOTE: This doesn't return certain method flags
        (mdAssem, mdFamANDAssem, mdFamORAssem, mdPrivateScope)
*/

    MethodDesc* callee = GetMethod(calleeHnd);
    MethodDesc* caller = GetMethod(callerHnd);

    if (callee->IsLCGMethod()) 
    {
        result = CORINFO_FLG_PUBLIC | CORINFO_FLG_STATIC | CORINFO_FLG_DONT_INLINE | CORINFO_FLG_NOSECURITYWRAP;
        goto exit;
    }


    DWORD attribs = callee->GetAttrs();

    if (IsMdPublic(attribs))
        result |= CORINFO_FLG_PUBLIC;
    if (IsMdPrivate(attribs))
        result |= CORINFO_FLG_PRIVATE;
    if (IsMdFamily(attribs))
        result |= CORINFO_FLG_PROTECTED;
    if (IsMdStatic(attribs))
        result |= CORINFO_FLG_STATIC;
    if (IsMdFinal(attribs))
        result |= CORINFO_FLG_FINAL;
    if (callee->IsSynchronized())
        result |= CORINFO_FLG_SYNCH;
    if (callee->IsFCallOrIntrinsic())
        result |= CORINFO_FLG_NOGCCHECK | CORINFO_FLG_INTRINSIC;
    if (IsMdVirtual(attribs))
        result |= CORINFO_FLG_VIRTUAL;
    if (IsMdAbstract(attribs))
        result |= CORINFO_FLG_ABSTRACT;
    if (IsMdInstanceInitializer(attribs, callee->GetName()) ||
        IsMdClassConstructor(   attribs, callee->GetName()))
        result |= CORINFO_FLG_CONSTRUCTOR;

    //
    // See if we need to embed a .cctor call at the head of the
    // method body.
    //

    MethodTable* pCalleeMT = callee->GetMethodTable();
    MethodTable* pCallerMT = caller->GetMethodTable();

    if (
        // Do not bother figuring out the initialization if we are only verifying the method.
        !isVerifyOnly()

        // For methods of the Global Class <Module> we don't need to call
        // the class initializer for <Module>
        && !pCalleeMT->IsGlobalClass()

        // We may need to run the initialization code if the class has some initialization
        // logic that still needs to be run.
        && !pCalleeMT->IsClassPreInited()

        // We only need to embed the .cctor call when the method we're
        // calling is domain netural code.  In other cases the JIT or NGEN fixups
        // can force the initialization.  However for shared generic code we have
        // to be more pessimistic - the JIT/NGEN can't force all the possible
        // initializations relevant to this codepath.                           
        && (   pCalleeMT->IsDomainNeutral()
            || pCalleeMT->IsSharedByGenericInstantiations()

            // If we're jitting a method with exact cctor semantics (not BeforeFieldInit)
            // and the class has not yet been initialized, this means that:
            // 1. if caller == callee, then the prestub interception to run the .cctor failed,
            //    likely because we're currently executing the .cctor higher up on the stack and so
            //    we detected the recursion and skipped calling the .cctor. This is an issue because
            //    in JITing this method we will backpatch the vtable and/or prestub and make the
            //    method available to other threads before the .cctor has completed.
            // 2. if caller != callee, we're inlining and need to insert the check if the type
            //    has not already been initialized because we won't be calling the prestub.
            || !pCalleeMT->IsClassInited()
            // || GetAppDomain()->IsCompilationDomain()
           )

        // Only if strict init is required - otherwise we can wait for
        // field accesses to run .cctor
        && !pCalleeMT->GetClass()->IsBeforeFieldInit()

        // Run .cctor on statics & constructors
        && (IsMdInstanceInitializer(attribs, callee->GetName()) || IsMdStatic(attribs))

        // Except don't class construct on .cctor - it would be circular
        && !IsMdClassConstructor(attribs, callee->GetName())

        && (
            // Note that jit has both methods the same if asking whether to emit cctor
            // for a given method's code (as opposed to inlining codegen).
            caller == callee

            // If we're inling a call to a method in our own type, then we should already
            // have triggered the .cctor when caller was itself called. The exception is
            // shared generic code.
            || pCallerMT != pCalleeMT
            || pCalleeMT->IsSharedByGenericInstantiations()
           )
        )
    {
        // If we start NGENing domain-bound assemblies again, this will add .cctor checks into the
        // prolog of all static methods, possibly causing an unexpected slowdown compared to the old
        // bit check on MethodTable. Note that if we do start NGENing domain-bound, then the commented
        // conditional above will likely need to be enabled, since IsDomainNeutral will not return true
        // even when we should insert the check and so detecting if we're in an NGEN domain should be
        // sufficient.
        CONSISTENCY_CHECK(!(GetAppDomain()->IsCompilationDomain() && !pCalleeMT->IsCompiledDomainNeutral()));

        result |= CORINFO_FLG_RUN_CCTOR;
    }

    // method may not have the final bit, but the class might
    if (IsMdFinal(attribs) == 0)
    {
        if (pCalleeMT->IsSealed())
            result |= CORINFO_FLG_FINAL;
    }

    if (callee->IsEnCAddedMethod())
    {
        result |= CORINFO_FLG_EnC;
    }

    if (callee->IsSharedByGenericInstantiations())
    {
        result |= CORINFO_FLG_SHAREDINST;
    }

    if (callee->IsNDirect())
    {
        result |= CORINFO_FLG_PINVOKE;
    }

    if (!Security::IsSecurityOn() || !callee->IsInterceptedForDeclSecurity())
    {
        result |= CORINFO_FLG_NOSECURITYWRAP;
    }

    if (callee->IsNotInline())
    {
        /* Function marked as not inlineable */
        result |= CORINFO_FLG_DONT_INLINE;
    }

    if (!callee->IsRuntimeSupplied())
    {
        if (Security::HasREQ_SOAttribute(callee) == S_OK)
        {
            result |= CORINFO_FLG_SECURITYCHECK;
        }
    }

    if (pCalleeMT->IsAnyDelegateClass() && ((DelegateEEClass*)(pCalleeMT->GetClass()))->m_pInvokeMethod == callee)
    {
        // This is now used to emit efficient invoke code for any delegate invoke,
        // including multicast.
        result |= CORINFO_FLG_DELEGATE_INVOKE;
    }

    }
exit: ;
    EE_TO_JIT_TRANSITION();

    return result;
}

static void *GetClassSync(MethodTable *pMT)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void * result = NULL;

    JIT_TO_EE_TRANSITION();

    OBJECTREF ref = NULL;

    ref = pMT->GetManagedClassObject();
    if (ref == NULL)
        result = NULL;
    else
        result = (void*)ref->GetSyncBlock()->GetMonitor();

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
void* __stdcall CEEInfo::getMethodSync(CORINFO_METHOD_HANDLE ftnHnd,
                                       void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void * result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    result = GetClassSync((GetMethod(ftnHnd))->GetMethodTable());

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
void __stdcall CEEInfo::setMethodAttribs (
        CORINFO_METHOD_HANDLE ftnHnd,
        CorInfoMethodRuntimeFlags attribs)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION_LEAF();

    MethodDesc* ftn = GetMethod(ftnHnd);

    if (attribs & CORINFO_FLG_BAD_INLINEE)
        ftn->SetNotInline(true);

    // Both CORINFO_FLG_UNVERIFIABLE and CORINFO_FLG_VERIFIABLE cannot be set
    _ASSERTE(!(attribs & CORINFO_FLG_UNVERIFIABLE) || 
             !(attribs & CORINFO_FLG_VERIFIABLE  ));

    if (attribs & CORINFO_FLG_VERIFIABLE)
        ftn->SetIsVerified(TRUE);
    else if (attribs & CORINFO_FLG_UNVERIFIABLE)
        ftn->SetIsVerified(FALSE);

    EE_TO_JIT_TRANSITION_LEAF();
}

/*********************************************************************/

void getMethodInfoILMethodHeaderHelper(
    COR_ILMETHOD_DECODER* header,
    CORINFO_METHOD_INFO* methInfo
    )
{
    LEAF_CONTRACT;
    
    methInfo->ILCode          = const_cast<BYTE*>(header->Code);
    methInfo->ILCodeSize      = header->GetCodeSize();
    methInfo->maxStack        = header->GetMaxStack();
    methInfo->EHcount         = header->EHCount();

    methInfo->options         =
        (CorInfoOptions)((header->GetFlags() & CorILMethod_InitLocals) ? CORINFO_OPT_INIT_LOCALS : 0) ;
}

/* static */
void getMethodInfoHelper(MethodDesc * ftn,
    CORINFO_METHOD_HANDLE ftnHnd,
    COR_ILMETHOD_DECODER * header,
    CORINFO_METHOD_INFO *  methInfo)
{
    _ASSERTE(ftn == GetMethod(ftnHnd));

    methInfo->ftn             = ftnHnd;
    methInfo->scope           = GetScopeHandle(ftn);

    /* Grab information from the IL header */

    PCCOR_SIGNATURE localSig;

    if (NULL != header)
    {
        getMethodInfoILMethodHeaderHelper(header, methInfo);
        localSig = header->LocalVarSig;
    }
    else
    {
        CONSISTENCY_CHECK(ftn->IsDynamicMethod());

        DynamicResolver* pResolver = ftn->GetDynamicMethodDesc()->GetResolver();        
        methInfo->ILCode = pResolver->GetCodeInfo(&methInfo->ILCodeSize,
                                                  &methInfo->maxStack,
                                                  &methInfo->options,
                                                  &methInfo->EHcount);

        localSig = pResolver->GetLocalSig(NULL);
    }

    if (!ClassLoader::CanAccessSigForExtraChecks(GetMethod(ftnHnd), localSig))
        COMPlusThrowHR(VER_E_METHOD_ACCESS);

    methInfo->options = (CorInfoOptions)(((UINT32)methInfo->options) | 
                            ((ftn->AcquiresInstMethodTableFromThis() ? CORINFO_GENERICS_CTXT_FROM_THIS : 0) |
                             (ftn->RequiresInstArg() ? CORINFO_GENERICS_CTXT_FROM_PARAMTYPEARG : 0)));

    // EEJitManager::ResolveEHClause and CrawlFrame::GetExactGenericInstantiations
    // need to be able to get to CORINFO_GENERICS_CTXT_MASK if there are any
    // catch clauses like "try {} catch(MyException<T> e) {}".
    // Such constructs are rare, and having to extend the lifetime of variable
    // for such cases is reasonable

    if (methInfo->options & CORINFO_GENERICS_CTXT_MASK)
    {
#if defined(PROFILING_SUPPORTED)
        if ( (g_profControlBlock.pEnter2    != NULL) ||
             (g_profControlBlock.pLeave2    != NULL) ||
             (g_profControlBlock.pTailcall2 != NULL) )
        {
            methInfo->options = CorInfoOptions(methInfo->options|CORINFO_GENERICS_CTXT_KEEP_ALIVE);
        }
        else
#endif // defined(PROFILING_SUPPORTED)
        {
            // Check all the exception clauses

            if (ftn->IsDynamicMethod())
            {
            }
            else
            {
                COR_ILMETHOD_SECT_EH_CLAUSE_FAT ehClause;

                for (unsigned i = 0; i < methInfo->EHcount; i++)
                {
                    const COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo =
                            (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)header->EH->EHClause(i, &ehClause);

                    // Is it a typed catch clause?
                    if (ehInfo->GetFlags() != COR_ILEXCEPTION_CLAUSE_NONE)
                        continue;

                    // Check if we catch "C<T>" ?

                    DWORD catchTypeToken = ehInfo->GetClassToken();
                    if (TypeFromToken(catchTypeToken) != mdtTypeSpec)
                        continue;

                    PCCOR_SIGNATURE pSig;
                    ULONG cSig;
                    ftn->GetMDImport()->GetTypeSpecFromToken(catchTypeToken, &pSig, &cSig);
                    SigPointer psig(pSig);

                    SigTypeContext sigTypeContext(ftn);
                    if (psig.IsPolyType(&sigTypeContext) & hasSharableVarsMask)
                    {
                        methInfo->options = CorInfoOptions(methInfo->options|CORINFO_GENERICS_CTXT_KEEP_ALIVE);
                        break;
                    }
                }
            }
        }
    }

    /* Fetch the method signature */
    // Type parameters in the signature should be instantiated according to the
    // class/method/array instantiation of ftnHnd
    CEEInfo::ConvToJitSig(  ftn->GetSig(),
                            GetScopeHandle(ftn),
                            mdTokenNil,
                            &methInfo->args,
                            ftnHnd,
                            false);

    // Shared generic or static per-inst methods and shared methods on generic structs
    // take an extra argument representing their instantiation
    if (ftn->RequiresInstArg())
        methInfo->args.callConv = (CorInfoCallConv) (methInfo->args.callConv | CORINFO_CALLCONV_PARAMTYPE);

    _ASSERTE( (IsMdStatic(ftn->GetAttrs()) == 0) == ((methInfo->args.callConv & CORINFO_CALLCONV_HASTHIS) != 0));

    /* And its local variables */
    // Type parameters in the signature should be instantiated according to the
    // class/method/array instantiation of ftnHnd
    CEEInfo::ConvToJitSig(  localSig,
                            GetScopeHandle(ftn),
                            mdTokenNil,
                            &methInfo->locals,
                            ftnHnd,
                            true);
}

bool __stdcall      CEEInfo::getMethodInfo (CORINFO_METHOD_HANDLE  ftnHnd,
                                            CORINFO_METHOD_INFO *  methInfo)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    bool result = false;

    JIT_TO_EE_TRANSITION();

    MethodDesc * ftn = GetMethod(ftnHnd);

    if (!ftn->IsDynamicMethod() && (!ftn->IsIL() || !ftn->GetRVA() || ftn->IsInstantiatingStub()))
    {
    /* Return false if not IL or has no code */
        result = false;
    }
    else
    {
        /* Get the IL header */
        /*                                                                      
                                                            */
        Module* pModule = ftn->GetModule();

        // CanSkipVerification will always cause a commit to SkipVerification for
        // for ngen images, even if they only contain verifiable code.
        // So we tentatively use CanSkipVerification with fCommit = FALSE
        bool    verify = !Security::CanSkipVerification(ftn, FALSE);

        COR_ILMETHOD_DECODER* pILHeader = NULL;

        if (ftn->IsDynamicMethod())
        {
            pILHeader = NULL;
        }
        else
        {
            COR_ILMETHOD_DECODER::DecoderStatus status = COR_ILMETHOD_DECODER::SUCCESS;
            COR_ILMETHOD_DECODER header(ftn->GetILHeader(), ftn->GetMDImport(), verify ? &status : NULL);
            pILHeader = &header;

            // If we get a verification error then we try to demand SkipVerification for the module
            if (status == COR_ILMETHOD_DECODER::VERIFICATION_ERROR &&
                Security::CanSkipVerification(pModule->GetDomainAssembly()))
            {
                status = COR_ILMETHOD_DECODER::SUCCESS;
            }

            if (status != COR_ILMETHOD_DECODER::SUCCESS ||
                !pModule->CheckIL(ftn->GetRVA(), header.GetHeaderSize()))
            {
                if (status == COR_ILMETHOD_DECODER::VERIFICATION_ERROR)
                {
                    // Throw a verification HR
                    COMPlusThrowHR(COR_E_VERIFICATION);
                }
                else
                {
                    THROW_BAD_FORMAT_MAYBE(!"Bad IL range", BFA_BAD_IL_RANGE, pModule);
                }
            }
        }

        getMethodInfoHelper(ftn, ftnHnd, pILHeader, methInfo);

        LOG((LF_JIT, LL_INFO100000, "Getting method info (possible inline) %s::%s%s\n",
            ftn->m_pszDebugClassName, ftn->m_pszDebugMethodName, ftn->m_pszDebugMethodSignature));

        result = true;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*************************************************************
 * Check if the caller and calle are in the same assembly
 * i.e. do not inline across assemblies
 *************************************************************/

DWORD GetDebuggerCompileFlags(MethodDesc * ftn, DWORD flags);
DWORD GetDebuggerCompileFlags(Module* pModule, DWORD flags);

CorInfoInline __stdcall      CEEInfo::canInline (CORINFO_METHOD_HANDLE hCaller,
                                        CORINFO_METHOD_HANDLE hCallee,
                                        DWORD*                pRestrictions)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoInline result = INLINE_PASS;  // By default we pass.  
                                         // Do not set pass in the rest of the method.
    DWORD         dwRestrictions = 0;    // By default, no restrictions

    JIT_TO_EE_TRANSITION();


    // Returns TRUE: if caller and callee are from the same assembly or the callee
    //               is part of the system assembly.
    //
    // If the caller and callee are from the same security and visibility scope
    // then the callee can be inlined into the caller. If they are not from the
    // same visibility scope then the callee cannot safely be inlined. For example,
    // if method b in class B in assembly BB calls method a in class A in assembly
    // AA it is not always safe to inline method a into method b. If method a makes
    // calles to methods in assembly AA that are private they would fail when made
    // from b. For security, if the callee does not have the same set of permissions
    // as the caller then subsequent security checks will have lost the set of permissions
    // granted method a. This can lead to security holes when permissions for a are
    // less then the granted permissions for b.


    MethodDesc* pCaller = GetMethod(hCaller);
    MethodDesc* pCallee = GetMethod(hCallee);

    if (pCallee->IsNoMetadata())
    {
        result = INLINE_FAIL;
        goto exit;
    }

    _ASSERTE(pCaller->GetModule());
    _ASSERTE(pCaller->GetModule()->GetClassLoader());

    _ASSERTE(pCallee->GetModule());
    _ASSERTE(pCallee->GetModule()->GetClassLoader());

    if (pCallee->GetModule()->GetClassLoader() != pCaller->GetModule()->GetClassLoader())
    {
        if (pCallee->GetModule()->GetClassLoader() != SystemDomain::Loader())
        {
            dwRestrictions |= INLINE_RESPECT_BOUNDARY;
        }
    }

    if ((pCaller->GetModule() != pCallee->GetModule()) &&
        pCallee->GetModule()->IsNoStringInterning())
    {
        dwRestrictions |= INLINE_NO_CALLEE_LDSTR;
    }

    // The remoting interception can be skipped only if the call is on same this pointer
    if (pCallee->MayBeRemotingIntercepted())
    {
        dwRestrictions |= INLINE_SAME_THIS;
    }

#ifdef DEBUGGING_SUPPORTED

    // If the callee want debuggable code, dont allow it to be inlined

    if (GetDebuggerCompileFlags(pCallee, 0) & CORJIT_FLG_DEBUG_CODE)
    {
        result = INLINE_NEVER;
        goto exit;
    }
#endif

#ifdef PROFILING_SUPPORTED
    if (IsProfilerPresent())
    {
        // If the profiler has set a mask preventing inlining, always return
        // false to the jit.
        if (CORProfilerDisableInlining())
            result = INLINE_FAIL;

        // If the profiler wishes to be notified of JIT events and the result from
        // the above tests will cause a function to be inlined, we need to tell the
        // profiler that this inlining is going to take place, and give them a
        // chance to prevent it.
        if (CORProfilerTrackJITInfo() && (result != INLINE_FAIL))
        {
            {
                BOOL fShouldInline;

                PROFILER_CALL;
                HRESULT hr = g_profControlBlock.pProfInterface->JITInlining(
                    (ThreadID)GetThread(),
                    (CodeID) pCaller,
                    (CodeID) pCallee,
                    &fShouldInline);

                if (SUCCEEDED(hr) && !fShouldInline)
                    result = INLINE_FAIL;
            }
        }
    }
#endif // PROFILING_SUPPORTED

    BOOL inlineeNeedsToBeVerifiable; inlineeNeedsToBeVerifiable = FALSE;

    // Prevent recursive compiling/inlining/verifying
    if (pCaller != pCallee)
    {
        //  The Inliner may not do code verification.
        //  So nvever inline anything that is unverifiable / bad code.

        if (!Security::CanSkipVerification(pCallee, FALSE)) // don't commit
            inlineeNeedsToBeVerifiable = TRUE;
    }

    if (!dontInline(result) && inlineeNeedsToBeVerifiable && !pCallee->IsVerifiable())
    {
        result = INLINE_NEVER;
        goto exit;
    }

    // We check this here as the call to MethodDesc::IsVerifiable()
    // may set CORINFO_FLG_DONT_INLINE.
    if (pCallee->IsNotInline()) 
    {
        result = INLINE_NEVER;
        goto exit;
    }

    if (result == INLINE_PASS)
    {
        MethodDesc  *pMDDecl = pCallee;
        MethodTable *pMT     = pMDDecl->GetMethodTable();
        MethodDesc  *pMDImpl = pMT->MapMethodDeclToMethodImpl(pMDDecl);

        if (pMDDecl != pMDImpl)
        {
            result = INLINE_NEVER;
        }
    }

exit: ;

    EE_TO_JIT_TRANSITION();

    if (result == INLINE_PASS && dwRestrictions)
    {
        if (pRestrictions)
        {
            *pRestrictions = dwRestrictions;
        }
        else
        {
            // If the jitter didn't want to know about restrictions, it shouldn't be inlining
            result = INLINE_FAIL;
        }
    }
    else
    {
        if (pRestrictions)
        {
            // Denied inlining, makes no sense to pass out restrictions,
            *pRestrictions = 0;
        }
    }

    return result;
}

/*************************************************************
 * Check if Transparency callout is needed.
 *************************************************************/

CorInfoIsCallAllowedResult __stdcall CEEInfo::isCallAllowed (
                                        CORINFO_METHOD_HANDLE hCaller,
                                        CORINFO_METHOD_HANDLE hCallee,
                                        CORINFO_CALL_ALLOWED_INFO * CallAllowedInfo)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoIsCallAllowedResult isCallAllowedResult = CORINFO_CALL_ALLOWED;

    // reset the info buffer
    CallAllowedInfo->mask = CORINFO_CALL_ALLOWED_NONE;
    CallAllowedInfo->context = NULL; 

    JIT_TO_EE_TRANSITION();

    MethodDesc* pCaller = GetMethod(hCaller);
    MethodDesc* pCallee = GetMethod(hCallee);

    // Check for security transparency interception
    switch (Security::RequiresTransparentAssemblyChecks(pCaller, pCallee))
    {
    case CORINFO_CALL_ALLOWED:
        break;
    case CORINFO_CALL_RUNTIME_CHECK:
        // if a run-time check is required, set the mask appropriately
        CallAllowedInfo->mask = (CORINFO_CALL_ALLOWED_FLAGS) ((int) CallAllowedInfo->mask | (int)CORINFO_CALL_ALLOWED_BYSECURITY);
        isCallAllowedResult = CORINFO_CALL_RUNTIME_CHECK; // mark the result as requiring run-time check        
        break;
    default:
        CallAllowedInfo->mask = CORINFO_CALL_ALLOWED_BYSECURITY;
        isCallAllowedResult = CORINFO_CALL_ILLEGAL;
        goto Done;
    }

    // add new conditions here

Done:
    // We must have set some bits in the mask if it is a runtime check
    _ASSERTE(isCallAllowedResult == CORINFO_CALL_ALLOWED || CallAllowedInfo->mask != CORINFO_CALL_ALLOWED_NONE);

    EE_TO_JIT_TRANSITION();

    return isCallAllowedResult;
}

/*************************************************************
This loads the (formal) declared constraints on the class and method type parameters, 
and detects (but does not itself reject) circularities among the class type parameters 
and (separately) method type parameters. 

It must be called whenever we verify a typical method, ie any method (generic or
nongeneric) in a typical class. It must be called for non-generic methods too, 
because their bodies may still mention class type parameters which will need to
have their formal constraints loaded in order to perform type compatibility tests.

We have to rule out cycles like "C<U,T> where T:U, U:T" only to avoid looping 
in the verifier (ie the T.CanCast(A) would loop calling U.CanCast(A) then 
T.CanCastTo(A) etc.). Since the JIT only tries to walk the hierarchy from a type
a parameter when verifying, it should be safe to JIT unverified, but trusted, 
instantiations even in the presence of cycle constraints.
@TODO: It should be possible (and easy) to detect cycles much earlier on by
directly inspecting the metadata. All you have to do is check that, for each
of the n type parameters to a class or method there is no path of length n 
obtained by following naked type parameter constraints of the same kind. 
This can be detected by looking directly at metadata, without actually loading
the typehandles for the naked type parameters.
 *************************************************************/

void __stdcall  CEEInfo::initConstraintsForVerification(CORINFO_METHOD_HANDLE hMethod,
                                                        BOOL *pfHasCircularClassConstraints,
                                                        BOOL *pfHasCircularMethodConstraints)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(pfHasCircularClassConstraints));
        PRECONDITION(CheckPointer(pfHasCircularMethodConstraints));
    } CONTRACTL_END;

    *pfHasCircularClassConstraints  = FALSE;
    *pfHasCircularMethodConstraints = FALSE;

    JIT_TO_EE_TRANSITION();

    MethodDesc* pMethod = GetMethod(hMethod);
    if (pMethod->IsTypicalMethodDefinition())
    {
        // Force a load of the constraints on the type parameters, detecting cyclic bounds
        pMethod->LoadConstraintsForTypicalMethodDefinition(pfHasCircularClassConstraints,pfHasCircularMethodConstraints);
    }

    EE_TO_JIT_TRANSITION();
}

/*************************************************************
 * Check if a method to be compiled is an instantiation
 * of generic code that has already been verified.
 * Three possible return values (see corinfo.h)
 *************************************************************/

CorInfoInstantiationVerification __stdcall  
    CEEInfo::isInstantiationOfVerifiedGeneric(CORINFO_METHOD_HANDLE hMethod)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoInstantiationVerification result = INSTVER_NOT_INSTANTIATION;

    JIT_TO_EE_TRANSITION();

    MethodDesc * pMethod = GetMethod(hMethod);

    if (!(pMethod->HasClassOrMethodInstantiation()))
    {
        result = INSTVER_NOT_INSTANTIATION;
        goto exit;
    }

    if (pMethod->IsTypicalMethodDefinition())
    {
        result = INSTVER_NOT_INSTANTIATION;
        goto exit;
    }

    result = pMethod->IsVerifiable() ? INSTVER_GENERIC_PASSED_VERIFICATION
                                     : INSTVER_GENERIC_FAILED_VERIFICATION;

 exit: ;

    EE_TO_JIT_TRANSITION();

    return result;
}


/*************************************************************
 * Similar to above, but perform check for tail call
 * eligibility. The callee can be passed as NULL if not known
 * (calli and callvirt).
 *************************************************************/

bool __stdcall      CEEInfo::canTailCall (CORINFO_METHOD_HANDLE hCaller,
                                          CORINFO_METHOD_HANDLE hCallee,
                                          bool fIsTailPrefix)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    bool result = false;

    JIT_TO_EE_TRANSITION();

    // See comments in canInline above.

    MethodDesc* pCaller = GetMethod(hCaller);
    MethodDesc* pCallee = GetMethod(hCallee);

    _ASSERTE(pCaller->GetModule());
    _ASSERTE(pCaller->GetModule()->GetClassLoader());

    _ASSERTE((pCallee == NULL) || pCallee->GetModule());
    _ASSERTE((pCallee == NULL) || pCallee->GetModule()->GetClassLoader());


    // If the caller is the static constructor (.cctor) of a class which has a ComImport base class
    // somewhere up the class hierarchy, then we cannot make the call into a tailcall.  See
    // RegisterObjectCreationCallback() in ExtensibleClassFactory.cpp for more information.
    if (pCaller->IsStaticInitMethod() &&
        pCaller->GetMethodTable()->IsComObjectType())
    {
        result = false;
        goto exit;
    }

    // TailCalls will throw off security stackwalking logic when there is a declarative Assert
    // Note that this check will also include declarative demands.  It's OK to do a tailcall in
    // those cases, but we currently don't have a way to check only for declarative Asserts.
    if (pCaller->IsInterceptedForDeclSecurity())
    {
        result = false;
        goto exit;
    }

    // If the callee is not known (callvirt, calli) or the caller and callee are
    // in different assemblies, we cannot allow the tailcall (since we'd
    // optimize away what might be the only stack frame for a given assembly,
    // skipping a security check).
    if (pCallee == NULL ||
             pCallee->GetModule()->GetClassLoader() != pCaller->GetModule()->GetClassLoader())
    {
        //                 |   non-virt non-xasm call   |   callvirt or xasm
        // specifies .tail |       Allowed              |   only if SkipVerify
        // no .tail        |       Allowed              |   Not allowed

        //If the code is marked as Skip Verification, then it is responsbile to
        //make sure that specifying .tail doesn't open up a security hole.

        if (fIsTailPrefix)
        {
            if (!Security::CanTailCall(pCaller))
            {
                result = false;
                goto exit;
            }
        }
        else
        {
            result = false;
            goto exit;
        }
    }

    // We don't want to tailcall the entrypoint for an application; JIT64 will sometimes
    // do this for simple entrypoints and it results in a rather confusing debugging 
    // experience.
    if (!fIsTailPrefix && pCaller->GetMemberDef() == pCaller->GetModule()->GetEntryPointToken())
    {
        result = false;
        goto exit;
    }

    // We cannot tail call from a root CER method, the thread abort algorithm to
    // detect CERs depends on seeing such methods on the stack.
    if (IsCerRootMethod(pCaller))
    {
        result = false;
        goto exit;
    }

    result = true;

exit: ;

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// canSkipMethodPreparation: Is there a need for all calls from
// NGEN'd code to a particular MethodDesc to go through DoPrestub,
// depending on the method sematics?  If so return FALSE.
//
// This is used to rule out both ngen-hardbinds and intra-ngen-module
// direct calls.
//
// The cases where direct calls are not allowed are typically where
// a stub must be inserted by DoPrestub (we do not save stubs) or where
// we haven't saved the code for some reason or another, or where fixups
// are required in the MethodDesc.
//
// callerHnd=NULL implies any/unspecified caller.
//
// Note that there may be other requirements for going through the prestub
// which vary based on the scenario. These need to be handled separately
// fCheckCode - Should code constraints be checked. Used for methods
//              which are known to have already been compiled

bool __stdcall CEEInfo::canSkipMethodPreparation (
        CORINFO_METHOD_HANDLE   callerHnd,
        CORINFO_METHOD_HANDLE   calleeHnd,
        bool                    fCheckCode,
        CorInfoIndirectCallReason *pReason,
        CORINFO_ACCESS_FLAGS    accessFlags/*=CORINFO_ACCESS_ANY*/)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    bool result = false;

    JIT_TO_EE_TRANSITION();

    MethodDesc *  calleeMD    = (MethodDesc *)calleeHnd;
    MethodDesc *  callerMD    = (MethodDesc *)callerHnd;

    result = calleeMD->CanSkipDoPrestub(callerMD, fCheckCode, pReason, accessFlags);

    EE_TO_JIT_TRANSITION();

    return result;
}


bool MethodDesc::CanSkipDoPrestub (
        MethodDesc *   callerMD,
        BOOL                    fCheckCode,
        CorInfoIndirectCallReason *pReason,
        CORINFO_ACCESS_FLAGS    accessFlags/*=CORINFO_ACCESS_ANY*/)
{
    CorInfoIndirectCallReason dummy;
    if (pReason == NULL)
        pReason = &dummy;
    *pReason = CORINFO_INDIRECT_CALL_UNKNOWN;

    return false;
}

/*********************************************************************/
// Can we make a direct call within an NGEN image using the method
// entry point.  This function is used only to compute direct calls
// inside NGEN images.
//
// A method entry point is created on the IA64 target
// (i.e. when METHOD_ENTRY_CHUNKS is defined).
// A call made using the method entry point will dispatch to the PreStub
// on the first call and then directly to the code afterwards
//
// Note that some method descs on IA64 require us to always set up r9
// and thus we can't use a direct call via the method entry point

bool __stdcall CEEInfo::canCallDirectViaEntryPointThunk (
        CORINFO_METHOD_HANDLE   calleeHnd,
        VOID **                 pEntryPoint)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    bool    result     = false;
    VOID *  entryPoint = NULL;

    JIT_TO_EE_TRANSITION();

    MethodDesc *  calleeMD    = (MethodDesc *)calleeHnd;


    entryPoint = calleeMD->HasTemporaryEntryPoint() ?
        (void*)calleeMD->GetTemporaryEntryPoint() : (void*)calleeMD->GetStableEntryPoint();

    result = true;


    EE_TO_JIT_TRANSITION();

    if (pEntryPoint)
        *pEntryPoint = entryPoint;

    return result;
}

void CEEInfo::getEHinfoHelper(
    CORINFO_METHOD_HANDLE   ftnHnd,
    unsigned                EHnumber,
    CORINFO_EH_CLAUSE*      clause,
    COR_ILMETHOD_DECODER*   pILHeader)
{
    CONTRACTL 
    {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    CONSISTENCY_CHECK(CheckPointer(pILHeader->EH));
    CONSISTENCY_CHECK(EHnumber < pILHeader->EH->EHCount());

    COR_ILMETHOD_SECT_EH_CLAUSE_FAT ehClause;
    const COR_ILMETHOD_SECT_EH_CLAUSE_FAT* ehInfo;
    ehInfo = (COR_ILMETHOD_SECT_EH_CLAUSE_FAT*)pILHeader->EH->EHClause(EHnumber, &ehClause);

    clause->Flags = (CORINFO_EH_CLAUSE_FLAGS)ehInfo->GetFlags();
    clause->TryOffset = ehInfo->GetTryOffset();
    clause->TryLength = ehInfo->GetTryLength();
    clause->HandlerOffset = ehInfo->GetHandlerOffset();
    clause->HandlerLength = ehInfo->GetHandlerLength();
    if ((clause->Flags & CORINFO_EH_CLAUSE_FILTER) == 0)
        clause->ClassToken = ehInfo->GetClassToken();
    else
        clause->FilterOffset = ehInfo->GetFilterOffset();

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
// get individual exception handler
void __stdcall CEEInfo::getEHinfo(
            CORINFO_METHOD_HANDLE ftnHnd,
            unsigned      EHnumber,
            CORINFO_EH_CLAUSE* clause)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    MethodDesc * ftn          = GetMethod(ftnHnd);

    if (IsDynamicMethodHandle(ftnHnd))
    {
        CEEDynamicCodeInfo::getEHinfo(ftnHnd, EHnumber, clause);
    }
    else
    {
    COR_ILMETHOD_DECODER header(ftn->GetILHeader(), ftn->GetMDImport(), NULL);
        getEHinfoHelper(ftnHnd, EHnumber, clause, &header);
    }

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
void __stdcall CEEInfo::getMethodSig (
            CORINFO_METHOD_HANDLE ftnHnd,
            CORINFO_SIG_INFO* sigRet,
            CORINFO_CLASS_HANDLE owner
            )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    MethodDesc* ftn = GetMethod(ftnHnd);
    PCCOR_SIGNATURE sig = ftn->GetSig();

    // Type parameters in the signature are instantiated
    // according to the class/method/array instantiation of ftnHnd and owner
    CEEInfo::ConvToJitSig(sig, GetScopeHandle(ftn), mdTokenNil, sigRet, ftnHnd, false, (TypeHandle) owner);

    //@GENERICS:
    // Shared generic methods and shared methods on generic structs take an extra argument representing their instantiation
    if (ftn->RequiresInstArg())
    {
      sigRet->callConv = (CorInfoCallConv) (sigRet->callConv | CORINFO_CALLCONV_PARAMTYPE);
    }

    // We want the calling convention bit to be consistant with the method attribute bit
    _ASSERTE( (IsMdStatic(ftn->GetAttrs()) == 0) == ((sigRet->callConv & CORINFO_CALLCONV_HASTHIS) != 0) );

    EE_TO_JIT_TRANSITION();
}

/***********************************************************************/
//@GENERICSVER: for a method desc in a typical instantiation of a generic class,
// this will return the typical instantiation of the generic class,
// but only provided type variables are never shared.
// The JIT verifier relies on this behaviour to extract the typical class from an instantiated method's typical method handle.
CORINFO_CLASS_HANDLE __stdcall CEEInfo::getMethodClass (CORINFO_METHOD_HANDLE methodHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    MethodDesc* method = GetMethod(methodHnd);

    if (method->IsDynamicMethod())
    {
        DynamicResolver* pResolver = method->GetDynamicMethodDesc()->GetResolver();

        DWORD securityControlFlags = 0;
        TypeHandle typeOwner;
        pResolver->GetJitContext(&securityControlFlags, &typeOwner);
        
        if (!typeOwner.IsNull() && (method == pResolver->GetDynamicMethod())) 
        {
            result = CORINFO_CLASS_HANDLE(typeOwner.AsPtr());
        }
    }

    if (result == NULL)
    {
        TypeHandle th = TypeHandle(method->GetMethodTable());

        result = CORINFO_CLASS_HANDLE(th.AsPtr());
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/***********************************************************************/
CORINFO_MODULE_HANDLE __stdcall CEEInfo::getMethodModule (CORINFO_METHOD_HANDLE methodHnd)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_MODULE_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    MethodDesc* method = GetMethod(methodHnd);

    if (method->IsDynamicMethod())
    {
        // this should never be called, thus the assert, I don't know if the (non existent) caller
        // expects the Module or the scope
        UNREACHABLE();
    }
    else
    {
        result = (CORINFO_MODULE_HANDLE) method->GetModule();
    }
    
    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
CorInfoIntrinsics __stdcall CEEInfo::getIntrinsicID(CORINFO_METHOD_HANDLE methodHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoIntrinsics result = CORINFO_INTRINSIC_Illegal;

    JIT_TO_EE_TRANSITION();

    MethodDesc* method = GetMethod(methodHnd);

    if (method->IsArray())
    {
        ArrayMethodDesc * arrMethod = (ArrayMethodDesc *)method;
        result = arrMethod->GetIntrinsicID();
    }
    else
    if (method->IsFCall())
    {
        result = ECall::GetIntrinsicID(method);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
unsigned __stdcall CEEInfo::getMethodVTableOffset (CORINFO_METHOD_HANDLE methodHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    unsigned result = (unsigned) -1;

    JIT_TO_EE_TRANSITION_LEAF();


    unsigned methodVtabOffset;

    MethodDesc* method = GetMethod(methodHnd);

    //@GENERICS: shouldn't be doing this for instantiated methods as they live elsewhere
    _ASSERTE(!method->HasMethodInstantiation());

    const int methTabOffset = MethodTable::GetVtableOffset();
    _ASSERTE(methTabOffset < 256);  // a rough sanity check

        // better be in the vtable
    _ASSERTE(method->GetSlot() < method->GetMethodTable()->GetNumVirtuals());

    if (method->IsInterface())
        methodVtabOffset = method->GetSlot()*sizeof(void*);
    else
        methodVtabOffset = method->GetSlot()*sizeof(void*) + methTabOffset;

    result = methodVtabOffset;


    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}

/*********************************************************************/
void __stdcall CEEInfo::getFunctionEntryPoint(CORINFO_METHOD_HANDLE  ftnHnd,
                                              InfoAccessType         requestedAccessType,
                                              CORINFO_CONST_LOOKUP * pResult,
                                              CORINFO_ACCESS_FLAGS   accessFlags)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
        // a JIT client can only request IAT_VALUE or IAT_PVALUE, not IAT_PPVALUE
        PRECONDITION(requestedAccessType == IAT_VALUE || requestedAccessType == IAT_PVALUE);
    } CONTRACTL_END;

    void* ret = NULL;
    InfoAccessType accessType = requestedAccessType;
    InfoAccessModule accessModule = IAM_UNKNOWN_MODULE;

    JIT_TO_EE_TRANSITION();

    MethodDesc * ftn = GetMethod(ftnHnd);

    // Resolve methodImpl.
    MethodDesc * ftnDecl = ftn;
    ftn = ftn->GetMethodTable()->MapMethodDeclToMethodImpl(ftn);

    // Record this call if required
    g_IBCLogger.LogMethodDescAccess(ftn);

    if (requestedAccessType == IAT_VALUE)
    {
        if (ftn->IsEnCAddedMethod())
        {
            // for EnC always just return the prestub. We never backpatch so can always call it and if
            // we update we force the prestub back.
            LOG((LF_ENC | LF_JIT, LL_INFO1000, "Returning prestub address for EnC, "));
            ret = (void *)ftn->GetCallablePreStubAddr();
            accessType = IAT_VALUE;
            accessModule = IAM_CURRENT_MODULE;
        }
        else 
        if (!(accessFlags & CORINFO_ACCESS_THIS) && ftn->IsRemotingInterceptedViaVirtualDispatch())
        {
            // This call will find or create the thunk and store it in a hash table
            ret = (void* )CRemotingServices::GetNonVirtualEntryPointForVirtualMethod(ftnDecl);
            accessType = IAT_VALUE;
            accessModule = IAM_CURRENT_MODULE;
        }
        else
        if (ftn->IsCodeReady())
        {

            ret = (void*) (ftn->GetSafeAddrofCode());
            accessType = IAT_VALUE;
            accessModule = IAM_CURRENT_MODULE;
        }
        else
        if (ftn->HasStableEntryPoint())
        {
            ret = (void*) ftn->GetStableEntryPoint();
            accessType = IAT_VALUE;
            accessModule = IAM_CURRENT_MODULE;
        }
        else 
        if (ftn->MayHavePrecode())
        {
            // Force the creation of the precode if we would eventually got one anyway
            ret = (void*) ftn->GetOrCreatePrecode()->GetEntryPoint();
            accessType = IAT_VALUE;
            accessModule = IAM_CURRENT_MODULE;
        }
#ifndef HAS_COMPACT_ENTRYPOINTS
        else
        {
            // Embed call to the temporary entrypoint into the code. It will be patched 
            // to point to the actual code later.
            ret = (void*)ftn->GetTemporaryEntryPoint();
            INDEBUG(ftn->IncPermittedPrestubCalls());
            accessType = IAT_VALUE;
            accessModule = IAM_CURRENT_MODULE;
        }
#endif
    }

    if (ret == NULL)
    {
        //
        // A direct call is either not possible, or not requested
        //

        // should never get here for EnC methods - always direct, non-virtual
        _ASSERTE(!ftn->IsEnCAddedMethod());

        // Check for calling a virtual method on marshalbyref class or object class
        // non virtually.  This is a rather special case.
        // Most other remoting intercepts on non-virtual calls the
        // intercept are done by a stub inserted by
        // DoPrestub.  However, here we wrap the function in a thunking stub .
        // Note that intercepting the call when it is
        // invoked virtually is handled by transparent proxy vtable intercepting.
        //
        // In other words, there are three ways we insert remoting intercepts:
        //  (1) in DoPrestub
        //  (2) by transparent proxy vtables, where all the entries in the vtable
        //      go to the same code.
        //  (3) by forcing non-virtual calls to virtual methods to be indirect calls
        //      and returning a code pointer via GetNonVirtualSlotForVirtualMethod.

        // The remoting interception is not necessary if we are calling on the same thisptr
        if(!(accessFlags & CORINFO_ACCESS_THIS) && ftn->IsRemotingInterceptedViaVirtualDispatch())
        {
            // Contextful classes imply marshal by ref but not vice versa
            _ASSERTE(!ftn->GetMethodTable()->IsContextful() || ftn->GetMethodTable()->IsMarshaledByRef());

            // This call will find or create the thunk and store it in
            // a hash table
            ret = (LPVOID) CRemotingServices::GetNonVirtualSlotForVirtualMethod(ftnDecl); // throws
        }
        else
        {
            ret = ftnDecl->GetAddrOfSlot();
        }
        accessType = IAT_PVALUE;
    }

    EE_TO_JIT_TRANSITION();

    _ASSERTE(ret != NULL);

    pResult->accessType = accessType;
    pResult->accessModule = accessModule;
    pResult->addr = ret;
}

/*********************************************************************/
void __stdcall CEEInfo::getFunctionFixedEntryPointInfo(CORINFO_MODULE_HANDLE  tokenScope,
                                                       unsigned               methodToken,
                                                       CORINFO_CONTEXT_HANDLE tokenContext,
                                                       CORINFO_LOOKUP *pResult)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    VarKind varKind = hasNoVars;
    MethodDesc *pMD = NULL;   // Class or method type parameters present in token
    loadTokenInternal(tokenScope, CORINFO_ANNOT_NONE, methodToken,
                      tokenContext, CORINFO_TOKENKIND_Default,
                      NULL, &pMD, NULL);
    
    if (!pMD)
    {
        COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
    }

    if (!IsDynamicScope(tokenScope))
    {
        varKind = ScanToken(GetModule(tokenScope), methodToken, tokenContext, TRUE);
    }

    if ((varKind & hasSharableVarsMask) == 0)
    {
        pResult->lookupKind.needsRuntimeLookup = false;
        pResult->constLookup.accessType = IAT_VALUE;
        pResult->constLookup.accessModule = IAM_CURRENT_MODULE;
        pResult->constLookup.addr = (void *) pMD->GetMultiCallableAddrOfCode();
    }
    else
    {
        _ASSERTE(!IsDynamicScope(tokenScope));
        MethodDesc *pContextMD = GetMethodFromContext(tokenContext);
        
        DictionaryEntryLayout entryLayout(CORINFO_ANNOT_ENTRYPOINT | methodToken, 0);
        
        ComputeRuntimeLookupForSharedGenericToken(GetModule(tokenScope),
                                                  &entryLayout,
                                                  pContextMD,
                                                  varKind,
                                                  pResult);
    }

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
const char* __stdcall CEEInfo::getFieldName (CORINFO_FIELD_HANDLE fieldHnd, const char** scopeName)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    const char* result = NULL;

    JIT_TO_EE_TRANSITION();

    FieldDesc* field = (FieldDesc*) fieldHnd;
    if (scopeName != 0)
    {
        TypeHandle t = TypeHandle(field->GetApproxEnclosingMethodTable());
        *scopeName = "";
        if (!t.IsNull())
        {
#ifdef _DEBUG
            t.GetName(ssClsNameBuff);
            *scopeName = ssClsNameBuff.GetUTF8(ssClsNameBuffScratch);
#else // !_DEBUG
            // since this is for diagnostic purposes only,
            // give up on the namespace, as we don't have a buffer to concat it
            // also note this won't show array class names.
            LPCUTF8 nameSpace;
            *scopeName= t.GetClass()->GetFullyQualifiedNameInfo(&nameSpace);
#endif // !_DEBUG
        }
    }

    result = field->GetName();

    EE_TO_JIT_TRANSITION();

    return result;
}


/*********************************************************************/
// The methodBeingCompiledHnd is only used to determine
// domain-neutrality of the code being generated,  if "A calls B calls C", and B and C are both inlined
// into A, then A should be passed as the methodBeingCompiledHnd parameter to getClassAttribs  when processing
// the IL of both B and C.  Note for most other methods "context" has a different meaning
// and refers to the immediate caller in a chain of inlined code, i.e. to B when inlining C.
DWORD __stdcall CEEInfo::getFieldAttribs (CORINFO_FIELD_HANDLE  fieldHnd,
                                          CORINFO_METHOD_HANDLE methodBeingCompiledHnd,
                                          CORINFO_ACCESS_FLAGS  flags)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    DWORD result = 0;

    JIT_TO_EE_TRANSITION();

/*
        returns field attribute flags (defined in corhdr.h)

        NOTE: This doesn't return certain field flags
        (fdAssembly, fdFamANDAssem, fdFamORAssem, fdPrivateScope)
*/

    FieldDesc* field = (FieldDesc*) fieldHnd;

    DWORD attribs = field->GetAttributes();

    if (IsFdPublic(attribs))
        result |= CORINFO_FLG_PUBLIC;

    if (IsFdPrivate(attribs))
        result |= CORINFO_FLG_PRIVATE;

    if (IsFdFamily(attribs))
        result |= CORINFO_FLG_PROTECTED;

    if (IsFdStatic(attribs))
        result |= CORINFO_FLG_STATIC;

    if (IsFdInitOnly(attribs))
        result |= CORINFO_FLG_FINAL;

    if (IsFdHasFieldRVA(attribs))
    {
        result |= CORINFO_FLG_UNMANAGED;

    }

    /*                  
           */

    /*                  
           */

    if (field->IsEnCNew())
        result |= CORINFO_FLG_EnC;

    if (field->IsStatic())
    {
        // static field reference
        if (isVerifyOnly())
        {
            // Ensure that the JIT does not call CEEInfo::getFieldAddress()
            // for statics of generic types
            result |= CORINFO_FLG_HELPER;
        }
        else if (field->IsThreadStatic() || field->IsContextStatic())
        {
            result |= CORINFO_FLG_HELPER;
        }
        else
        {
            if (!field->IsSpecialStatic())   // Special means RVA, context or thread local
            {
                if (field->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
                    result |= CORINFO_FLG_STATIC_IN_HEAP;

                // Use a helper to access static fields inside domain neutral assemblies or when
                // accessing static generic fields from shared generic code.  Currently we just have
                // one generics helper (actually two: one for the GC case and one for the non-GC case),
                // so always use that one for a generic static field if any kind of sharing is involved.
                // pFieldMT => the exact MT is always available for static fields
                MethodTable * pFieldMT            = field->GetEnclosingMethodTable();
                MethodDesc  * methodBeingCompiled = GetMethod(methodBeingCompiledHnd);

                BOOL fSharedByGenerics = pFieldMT->IsSharedByGenericCanonicalSubtype()
                                          && methodBeingCompiled->IsSharedByGenericInstantiations();
                BOOL fSharedByDomains  = methodBeingCompiled->IsDomainNeutral();

                if (fSharedByGenerics || 
                    (fSharedByDomains && field->GetNumGenericClassArgs() > 0))
                {
                    _ASSERTE(pFieldMT->HasGenericsStaticsInfo());
                    result |= CORINFO_FLG_GENERICS_STATIC;
                }
                else if (fSharedByDomains)
                {
                    _ASSERTE(!pFieldMT->HasGenericsStaticsInfo());
                    _ASSERTE(pFieldMT->IsDomainNeutral());
                    result |= CORINFO_FLG_SHARED_HELPER;
                }
            }
        }
    }
    else
    {
        // instance field reference

#if CHECK_APP_DOMAIN_LEAKS
        if (g_pConfig->EnableFullDebug()
            && field->IsDangerousAppDomainAgileField()
            && CorTypeInfo::IsObjRef(field->GetFieldType()))
        {
            //
            // In a checked field with all checks turned on, we use a helper to enforce the app domain
            // agile invariant.
            //
            result |= CORINFO_FLG_HELPER;
        }
        else
#endif // CHECK_APP_DOMAIN_LEAKS
        {
            MethodTable * pFieldApproxMT = field->GetApproxEnclosingMethodTable();

            // are we a contextful class? (approxMT is OK to use here)
            if (pFieldApproxMT->IsContextful())
            {
                // Allow the JIT to optimize special cases 

                // If the caller is states that we have a 'this reference'
                // and he is also willing to unwrap it himself
                // then we won't require a helper call.
                if (!(flags & CORINFO_ACCESS_THIS  )  ||
                    !(flags & CORINFO_ACCESS_UNWRAP))
                {
                    // Normally a helper call is required.
                    result |= CORINFO_FLG_HELPER;
                }
            }
            // are we a marshaled by ref class? (approxMT is OK to use here)
            else if (pFieldApproxMT->IsMarshaledByRef())
            {
                // Allow the JIT to optimize special cases 

                // If the caller is states that we have a 'this reference'
                // then we won't require a helper call.
                if (!(flags & CORINFO_ACCESS_THIS))
                {
                    // Normally a helper call is required.
                    result |= CORINFO_FLG_HELPER;
                }
            }

            if (field->IsNonVerifiableExplicitField())
            {
                result |= CORINFO_FLG_NONVERIFIABLYOVERLAPS;
            }

        }
    }


    if (field->IsStatic() &&
        field->IsThreadStatic() == FALSE &&
        field->IsContextStatic() == FALSE &&
        (result & CORINFO_FLG_TLS) == 0)
    {
        result |= CORINFO_FLG_SAFESTATIC_BYREF_RETURN;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Get the type that declares the field
CORINFO_CLASS_HANDLE __stdcall CEEInfo::getFieldClass (CORINFO_FIELD_HANDLE fieldHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    FieldDesc* field = (FieldDesc*) fieldHnd;
    result = CORINFO_CLASS_HANDLE(TypeHandle(field->GetApproxEnclosingMethodTable()).AsPtr());

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Returns the basic type of the field (not the the type that declares the field)
//
// pTypeHnd - On return, for reference and value types, *pTypeHnd will contain 
//            the normalized type of the field.
// owner - Optional. For resolving in a generic context

CorInfoType __stdcall CEEInfo::getFieldType (CORINFO_FIELD_HANDLE fieldHnd, 
                                               CORINFO_CLASS_HANDLE* pTypeHnd, 
                                               CORINFO_CLASS_HANDLE owner)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoType result = CORINFO_TYPE_UNDEF;
    *pTypeHnd = 0;

    JIT_TO_EE_TRANSITION();

    TypeHandle clsHnd = TypeHandle();
    FieldDesc* field = (FieldDesc*) fieldHnd;
    CorElementType type   = field->GetFieldType();

    _ASSERTE(type != ELEMENT_TYPE_BYREF);

    // For verifying code involving generics, use the class instantiation
    // of the optional owner (to provide exact, not representative,
    // type information)
    SigTypeContext typeContext(field, (TypeHandle) owner);

    if (!CorTypeInfo::IsPrimitiveType(type))
    {
        PCCOR_SIGNATURE sig;
        DWORD sigCount;
        CorCallingConvention conv;

        field->GetSig(&sig, &sigCount);

         conv = (CorCallingConvention) CorSigUncompressCallingConv(sig);
        _ASSERTE(isCallConv(conv, IMAGE_CEE_CS_CALLCONV_FIELD));

        SigPointer ptr(sig);

        clsHnd = ptr.GetTypeHandleThrowing(field->GetModule(), &typeContext);
        _ASSERTE(!clsHnd.IsNull());

        // I believe it doesn't make any diff. if this is GetInternalCorElementType 
        // or GetSignatureCorElementType.
        type = clsHnd.GetSignatureCorElementType();
    }

    result = toJitType(type, clsHnd, pTypeHnd);

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
unsigned __stdcall CEEInfo::getFieldOffset (CORINFO_FIELD_HANDLE fieldHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    unsigned result = (unsigned) -1;

    JIT_TO_EE_TRANSITION();

    FieldDesc* field = (FieldDesc*) fieldHnd;

    // GetOffset() does not include the size of Object
    result = field->GetOffset();

    // only the small types are not DWORD aligned.
    // FIX put this back _ASSERTE(field->GetFieldType() < ELEMENT_TYPE_I4 || (result & 3) == 0);


    // So if it is not a value class, add the Object into it
    if (field->IsStatic())
    {
        Module* pModule = field->GetModule();
        if (field->IsRVA() && pModule->IsRvaFieldTls(field->GetOffset()))
        {
            result = pModule->GetFieldTlsOffset(field->GetOffset());
        }
    }
    else if (!field->GetApproxEnclosingMethodTable()->IsValueClass())
    {
        result += sizeof(Object);
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
void* __stdcall CEEInfo::getFieldAddress(CORINFO_FIELD_HANDLE fieldHnd,
                                         void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void *result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    FieldDesc* field = (FieldDesc*) fieldHnd;

    MethodTable* pMT = field->GetEnclosingMethodTable();
    pMT->CheckRestore();

    // We always tell JIT to call the helper if we are only verifying the method
    _ASSERTE(!isVerifyOnly());

    _ASSERTE(!pMT->ContainsGenericVariables());

    if (1 /*&& !pMT->IsClassInited()*/)
    {

        // Allocate space for the local class if necessary, but don't trigger
        // class construction.
        DomainLocalModule *pLocalModule = pMT->GetDomainLocalModule();
        pLocalModule->PopulateClass(pMT);
    }

    void *base = (void *) field->GetBase();

    result = field->GetStaticAddressHandle(base);

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
void CEEJitInfo::BackoutJitData(IJitManager * jitMgr)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CodeHeader* pCodeHeader = GetCodeHeader();
    if (pCodeHeader)
        jitMgr->RemoveJitData((METHODTOKEN)pCodeHeader, m_GCinfo_len, m_EHinfo_len);
}

/*********************************************************************/
CorInfoHelpFunc __stdcall CEEInfo::getFieldHelper(CORINFO_FIELD_HANDLE fieldHnd, enum CorInfoFieldAccess kind)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();

    FieldDesc* field = (FieldDesc*) fieldHnd;
    CorElementType type = field->GetFieldType();

    if (kind == CORINFO_ADDRESS)
    {
        if (field->IsStatic())
        {
            if (field->GetOffset() < FIELD_OFFSET_LAST_REAL_OFFSET)
            {
                if (field->IsThreadStatic())
                {
                    if (CorTypeInfo::IsPrimitiveType(type))
                    {
                        result = CORINFO_HELP_GET_THREAD_FIELD_ADDR_PRIMITIVE;
                        goto exit;
                    }
                    if (CorTypeInfo::IsObjRef(type))
                    {
                        result = CORINFO_HELP_GET_THREAD_FIELD_ADDR_OBJREF;
                        goto exit;
                    }
                }
                // else if (field->IsContextStatic()) {
                //  We do not have optimized path for context statics because:
                //  - Remoting contexts are deprecated feature
                //  - All access to context statics store has to take s_ContextCrst. Taking crst would force 
                //    us to erect helper frame anyway.
                // }
            }

            result = CORINFO_HELP_GETSTATICFIELDADDR;
            goto exit;

        }

        result = CORINFO_HELP_GETFIELDADDR;
        goto exit;
    }

    if (CorTypeInfo::IsObjRef(type))
        result = CORINFO_HELP_GETFIELDOBJ;
    else if (CorTypeInfo::Size(type) <= 4)
        result = (type == ELEMENT_TYPE_R4) ? CORINFO_HELP_GETFIELDFLOAT : CORINFO_HELP_GETFIELD32;
    else if (CorTypeInfo::Size(type) == 8)
        result = (type == ELEMENT_TYPE_R8) ? CORINFO_HELP_GETFIELDDOUBLE : CORINFO_HELP_GETFIELD64;
    else
    {
        _ASSERTE(type == ELEMENT_TYPE_VALUETYPE);

        result = CORINFO_HELP_GETFIELDSTRUCT;
    }

    _ASSERTE(kind == CORINFO_GET || kind == CORINFO_SET);
    _ASSERTE(!field->IsStatic());       // Static fields always accessed through address
    _ASSERTE(CORINFO_GET == 0);
    _ASSERTE((int) CORINFO_HELP_SETFIELD32 == (int) CORINFO_HELP_GETFIELD32 + (int) CORINFO_SET);
    _ASSERTE((int) CORINFO_HELP_SETFIELD64 == (int) CORINFO_HELP_GETFIELD64 + (int) CORINFO_SET);
    _ASSERTE((int) CORINFO_HELP_SETFIELDOBJ == (int) CORINFO_HELP_GETFIELDOBJ + (int) CORINFO_SET);
    _ASSERTE((int) CORINFO_HELP_SETFIELDSTRUCT == (int) CORINFO_HELP_GETFIELDSTRUCT + (int) CORINFO_SET);
    _ASSERTE((int) CORINFO_HELP_SETFIELDFLOAT == (int) CORINFO_HELP_GETFIELDFLOAT + (int) CORINFO_SET);
    _ASSERTE((int) CORINFO_HELP_SETFIELDDOUBLE == (int) CORINFO_HELP_GETFIELDDOUBLE + (int) CORINFO_SET);

    result = (CorInfoHelpFunc) (result + kind);

 exit: ;

    EE_TO_JIT_TRANSITION();

    return result;
}


/*********************************************************************/
CorInfoFieldCategory __stdcall CEEInfo::getFieldCategory (CORINFO_FIELD_HANDLE fieldHnd)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CorInfoFieldCategory result = CORINFO_FIELDCATEGORY_UNKNOWN;

    JIT_TO_EE_TRANSITION_LEAF();
    
    result = CORINFO_FIELDCATEGORY_NORMAL;

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}

DWORD __stdcall CEEInfo::getFieldThreadLocalStoreID(CORINFO_FIELD_HANDLE fieldHnd, void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    DWORD result = 0;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(!"TLS not supported on platform");

    EE_TO_JIT_TRANSITION();

    return result;
}

void *CEEInfo::allocateArray(ULONG cBytes)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void * result = NULL;

    JIT_TO_EE_TRANSITION();

#ifdef DEBUGGING_SUPPORTED
    result = g_pDebugInterface->allocateArray(cBytes);
#else // !DEBUGGING_SUPPORTED
    result =  NULL;
#endif // !DEBUGGING_SUPPORTED

    EE_TO_JIT_TRANSITION();

    return result;
}

void CEEInfo::freeArray(void *array)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

#ifdef DEBUGGING_SUPPORTED
    g_pDebugInterface->freeArray(array);
#endif // DEBUGGING_SUPPORTED

    EE_TO_JIT_TRANSITION();
}

void CEEInfo::getBoundaries(CORINFO_METHOD_HANDLE ftn,
                               unsigned int *cILOffsets, DWORD **pILOffsets,
                               ICorDebugInfo::BoundaryTypes *implicitBoundaries)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

#ifdef DEBUGGING_SUPPORTED

#ifdef _DEBUG
    if (!CORDebuggerTrackJITInfo((GetMethod(ftn))->GetModule()->GetDebuggerInfoBits()) &&
        !SystemDomain::GetCurrentDomain()->IsCompilationDomain() &&
#ifdef PROFILING_SUPPORTED
        !CORProfilerJITMapEnabled() &&
#endif
        !g_pConfig->GenDebugInfo())
    {
        _ASSERTE(!"__FUNCTION__() called spuriously");
    }
#endif // _DEBUG

    g_pDebugInterface->getBoundaries(ftn, cILOffsets, pILOffsets,
                                     implicitBoundaries);
#endif // DEBUGGING_SUPPORTED

    EE_TO_JIT_TRANSITION();
}


// The setBoundary/setVars on ICorDebugInfo should never be called for the CEEInfo impl.
// It should only only be called on the derived CEEJitInfo class.
// But we still need impls here b/c CEEInfo needs to impl part of ICorDebugInfo.
void CEEInfo::setBoundaries(CORINFO_METHOD_HANDLE ftn, ULONG32 cMap,
                               OffsetMapping *pMap)
{
    UNREACHABLE();      // only called on derived class.
}

void CEEInfo::setVars(CORINFO_METHOD_HANDLE ftn, ULONG32 cVars, NativeVarInfo *vars)
    {
    UNREACHABLE();      // only called on derived class.
}

void CEEInfo::getVars(CORINFO_METHOD_HANDLE ftn, ULONG32 *cVars, ILVarInfo **vars,
                         bool *extendOthers)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

#ifdef DEBUGGING_SUPPORTED

#ifdef _DEBUG
    if (!CORDebuggerTrackJITInfo((GetMethod(ftn))->GetModule()->GetDebuggerInfoBits()) &&
        !SystemDomain::GetCurrentDomain()->IsCompilationDomain() &&
#ifdef PROFILING_SUPPORTED
        !CORProfilerJITMapEnabled() &&
#endif
        !g_pConfig->GenDebugInfo())
    {
        _ASSERTE(!"__FUNCTION__() called supuriously");
    }
#endif // _DEBUG

    g_pDebugInterface->getVars(ftn, cVars, vars, extendOthers);
#endif // DEBUGGING_SUPPORTED

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
// Route jit information to the Jit Debug store.
void CEEJitInfo::setBoundaries(CORINFO_METHOD_HANDLE ftn, ULONG32 cMap,
                               OffsetMapping *pMap)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    CodeHeader* pHeader = GetCodeHeader();
    this->m_jitManager->setBoundaries(pHeader, ftn, cMap, pMap);

    // We receive ownership of the array; but we don't pass that ownership onto the jitmanager
    // b/c they'll just compress the array and throw it away.
    // Hower, the jitmanagers don't know which heap the array was allocated on, so they
    // can't free it either. So we free it now.
    this->freeArray(pMap);

    EE_TO_JIT_TRANSITION();
}

void CEEJitInfo::setVars(CORINFO_METHOD_HANDLE ftn, ULONG32 cVars, NativeVarInfo *vars)
    {
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    CodeHeader* pHeader = GetCodeHeader();
    this->m_jitManager->setVars(pHeader, ftn, cVars, vars);

    // We receive ownership of the array; but we don't pass that ownership onto the jitmanager
    // b/c they'll just compress the array and throw it away.
    // Hower, the jitmanagers don't know which heap the array was allocated on, so they
    // can't free it either. So we free it now.
    this->freeArray(vars);

    EE_TO_JIT_TRANSITION();
}



/*********************************************************************/
CORINFO_ARG_LIST_HANDLE __stdcall CEEInfo::getArgNext(CORINFO_ARG_LIST_HANDLE args)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_ARG_LIST_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    SigPointer ptr((unsigned __int8*) args);
    IfFailThrow(ptr.SkipExactlyOne());

    result = (CORINFO_ARG_LIST_HANDLE) ptr.GetPtr();

    EE_TO_JIT_TRANSITION();
    
    return result;
}


/*********************************************************************/

CorInfoTypeWithMod __stdcall CEEInfo::getArgType (
        CORINFO_SIG_INFO*       sig,
        CORINFO_ARG_LIST_HANDLE args,
        CORINFO_CLASS_HANDLE*   vcTypeRet
        )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoTypeWithMod result = CorInfoTypeWithMod(CORINFO_TYPE_UNDEF);

    JIT_TO_EE_TRANSITION();

   _ASSERTE((BYTE*) sig->sig <= (BYTE*) sig->args);
   _ASSERTE((BYTE*) sig->args <= (BYTE*) args && (BYTE*) args < &((BYTE*) sig->args)[0x10000*5]);
    INDEBUG(*vcTypeRet = CORINFO_CLASS_HANDLE((size_t)INVALID_POINTER_CC));

    SigPointer ptr((unsigned __int8*) args);
    CorElementType eType;
    IfFailThrow(ptr.PeekElemType(&eType));
    while (eType == ELEMENT_TYPE_PINNED)
    {
        result = CORINFO_TYPE_MOD_PINNED;
        IfFailThrow(ptr.GetElemType(NULL));
        IfFailThrow(ptr.PeekElemType(&eType));
    }

    // Now read off the "real" element type after taking any instantiations into consideration
    SigTypeContext typeContext;
    GetTypeContext(&sig->sigInst,&typeContext);

    CorElementType type = ptr.PeekElemTypeClosed(&typeContext);

    Module* pModule = GetModule(sig->scope);

    TypeHandle typeHnd = TypeHandle();
    switch (type) {
      case ELEMENT_TYPE_VAR :
      case ELEMENT_TYPE_MVAR :
      case ELEMENT_TYPE_VALUETYPE :
      case ELEMENT_TYPE_TYPEDBYREF :
      case ELEMENT_TYPE_INTERNAL :
      {
            typeHnd = ptr.GetTypeHandleThrowing(pModule, &typeContext);
            _ASSERTE(!typeHnd.IsNull());

            CorElementType normType = typeHnd.GetInternalCorElementType();

            // if we are looking up a value class, don't morph it to a refernece type
            // (This can only happen in illegal IL
            if (!CorTypeInfo::IsObjRef(normType) || type != ELEMENT_TYPE_VALUETYPE)
            {
                type = normType;
            }
        }
        break;

    case ELEMENT_TYPE_PTR:
        // Load the type eagerly under debugger to make the eval work
        if (CORDisableJITOptimizations(pModule->GetDebuggerInfoBits()))
        {
            // NOTE: in some IJW cases, when the type pointed at is unmanaged,
            // the GetTypeHandle may fail, because there is no TypeDef for such type.
            // Usage of GetTypeHandleThrowing would lead to class load exception
            TypeHandle thPtr = ptr.GetTypeHandle(pModule, &typeContext, NULL);
            if(!thPtr.IsNull())
                m_pOverride->classMustBeLoadedBeforeCodeIsRun(CORINFO_CLASS_HANDLE(thPtr.AsPtr()));
        }
        break;

    case ELEMENT_TYPE_VOID:
        // void is not valid in local sigs
        if (sig->flags & CORINFO_SIGFLAG_IS_LOCAL_SIG)
            COMPlusThrowHR(COR_E_INVALIDPROGRAM);
        break;

    case ELEMENT_TYPE_END:
           COMPlusThrowHR(COR_E_BADIMAGEFORMAT);
        break;

    default:
        break;
    }

    result = CorInfoTypeWithMod(result | toJitType(type, typeHnd, vcTypeRet));
    ScanForModuleDependencies(pModule,(unsigned __int8*) args,TRUE);
    EE_TO_JIT_TRANSITION();
    
    return result;
}

/*********************************************************************/

CORINFO_CLASS_HANDLE __stdcall CEEInfo::getArgClass (
    CORINFO_SIG_INFO*       sig,
    CORINFO_ARG_LIST_HANDLE args
    )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result = NULL;

    JIT_TO_EE_TRANSITION();

    // make certain we dont have a completely wacked out sig pointer
    _ASSERTE((BYTE*) sig->sig <= (BYTE*) sig->args);
    _ASSERTE((BYTE*) sig->args <= (BYTE*) args && (BYTE*) args < &((BYTE*) sig->args)[0x10000*5]);

    Module* pModule = GetModule(sig->scope);

    SigPointer ptr((unsigned __int8*) args);

    CorElementType eType;
    IfFailThrow(ptr.PeekElemType(&eType));

    while (eType == ELEMENT_TYPE_PINNED)
    {
        IfFailThrow(ptr.GetElemType(NULL));
        IfFailThrow(ptr.PeekElemType(&eType));
    }
    // Now read off the "real" element type after taking any instantiations into consideration
    SigTypeContext typeContext;
    GetTypeContext(&sig->sigInst, &typeContext);
    CorElementType type = ptr.PeekElemTypeClosed(&typeContext);

    if (!CorTypeInfo::IsPrimitiveType(type)) {
        TypeHandle th = ptr.GetTypeHandleThrowing(pModule, &typeContext);
        result = CORINFO_CLASS_HANDLE(th.AsPtr());
    }

    EE_TO_JIT_TRANSITION();
    
    return result;
}

/*********************************************************************/

    // return the unmanaged calling convention for a PInvoke
CorInfoUnmanagedCallConv __stdcall CEEInfo::getUnmanagedCallConv(CORINFO_METHOD_HANDLE method)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    } CONTRACTL_END;

    CorInfoUnmanagedCallConv result = CORINFO_UNMANAGED_CALLCONV_UNKNOWN;

    JIT_TO_EE_TRANSITION();

    MethodDesc* pMD = NULL;
    pMD = GetMethod(method);
    _ASSERTE(pMD->IsNDirect());

#ifdef _X86_
    EX_TRY
    {
        PInvokeStaticSigInfo sigInfo(pMD, PInvokeStaticSigInfo::NO_THROW_ON_ERROR);

        switch (sigInfo.GetCallConv()) {
            case pmCallConvCdecl:
                result = CORINFO_UNMANAGED_CALLCONV_C;
                break;
            case pmCallConvStdcall:
                result = CORINFO_UNMANAGED_CALLCONV_STDCALL;
                break;
            case pmCallConvThiscall:
                result = CORINFO_UNMANAGED_CALLCONV_THISCALL;
                break;
            default:
                result = CORINFO_UNMANAGED_CALLCONV_UNKNOWN;
        }
    }
    EX_CATCH
    {
        result = CORINFO_UNMANAGED_CALLCONV_UNKNOWN;
    }
    EX_END_CATCH(SwallowAllExceptions)
#else // !_X86_
    //
    // we have only one calling convention
    //
    result = CORINFO_UNMANAGED_CALLCONV_STDCALL;
#endif // !_X86_

    EE_TO_JIT_TRANSITION();
    
    return result;
}

/*********************************************************************/
BOOL NDirectMethodDesc::ComputeMarshalingRequired()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    PInvokeStaticSigInfo sigInfo(this);

    StubHolder<Stub> pTempMLStub;

    pTempMLStub = CreateNDirectStubNoThrow(
        sigInfo.GetSig(), 
        sigInfo.GetSigCount(),
        sigInfo.GetModule(), 
        GetMemberDef(),
        sigInfo.GetCharSet(), 
        sigInfo.GetLinkFlags(), 
        sigInfo.GetCallConv(),
        sigInfo.GetStubFlags()
        , this, NULL);

    if (!pTempMLStub)
    {
        return TRUE;
    }

    SetStackArgumentSize(((MLHeader*)(pTempMLStub->GetEntryPoint()))->m_cbDstBuffer,
        sigInfo.GetCallConv());

    MLHeader *header = (MLHeader*)(pTempMLStub->GetEntryPoint());

    if ((header->m_Flags & ~MLHF_CAN_BE_INLINED_MASK) != 0)
    {
        // Some unsafe flag is set
        return TRUE;
    }

    const MLCode *pMLCode = header->GetMLCode();
    MLCode mlcode;
    while (ML_END != (mlcode = *(pMLCode++)))
    {
        if (!(mlcode == ML_COPY4  || mlcode == ML_COPYI4 ||
                mlcode == ML_COPYU4 || mlcode == ML_COPY8  ||
                mlcode == ML_INTERRUPT))
        {
            if (mlcode == ML_BUMPSRC)
            {
                pMLCode += 2;
            }
            else
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*********************************************************************/
BOOL __stdcall CEEInfo::pInvokeMarshalingRequired(CORINFO_METHOD_HANDLE method, CORINFO_SIG_INFO* callSiteSig)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();


    result = TRUE;
    goto exit;

exit: ;
    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Generate a cookie based on the signature that would needs to be passed
// to CORINFO_HELP_PINVOKE_CALLI
LPVOID CEEInfo::GetCookieForPInvokeCalliSig(CORINFO_SIG_INFO* szMetaSig,
                                            void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    LPVOID result = NULL;

    JIT_TO_EE_TRANSITION();

    result = getVarArgsHandle(szMetaSig, ppIndirection);

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Check Visibility rules for accessing a method
//
// context      : The method which is doing the access
// parentType   : The type which defines the targetMethod
// targetMethod : The method being called/accessed
// instanceType : The type of the instance object being used for the call.
//      This is used or checking protected (family access) members. It should
//      be the same/equivalent as "context" for accessig static methods, 
//      or if the object instance if null

BOOL __stdcall CEEInfo::canAccessMethod(
        CORINFO_METHOD_HANDLE       context,
        CORINFO_CLASS_HANDLE        parentType,
        CORINFO_METHOD_HANDLE       targetMethod,
        CORINFO_CLASS_HANDLE        instanceType)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL canAccessMethod = TRUE;

    JIT_TO_EE_TRANSITION();

    MethodDesc* pContextMD = GetMethod(context);    
    MethodDesc* pTargetMD = GetMethod(targetMethod);
    TypeHandle contextOwner(pContextMD->GetMethodTable());

    BOOL doAccessCheck = TRUE;

    if (pContextMD->IsDynamicMethod())
    {
        TypeHandle  contextTypeOwner;
        
        DynamicResolver * pResolver = pContextMD->GetDynamicMethodDesc()->GetResolver();

        DWORD dwSecurityFlags = 0;
        pResolver->GetJitContext(&dwSecurityFlags, &contextTypeOwner);

        if (dwSecurityFlags & DynamicResolver::SkipVisibilityChecks)
        {
            doAccessCheck = FALSE;
        }
        
        if (!contextTypeOwner.IsNull())
            contextOwner = contextTypeOwner;
    }
    
    if (doAccessCheck)
    {
        canAccessMethod = ClassLoader::CanAccess(
                pContextMD,
                contextOwner.GetMethodTable(),
                contextOwner.GetAssembly(),
                TypeHandle(parentType).GetMethodTable(),
                TypeHandle(parentType).GetAssembly(),
                TypeHandle(instanceType).GetMethodTable(),
                pTargetMD->GetAttrs(),
                pTargetMD);
    }

    EE_TO_JIT_TRANSITION();
    
    return canAccessMethod;
}

// Check any constraints on method type arguments
BOOL __stdcall CEEInfo::satisfiesMethodConstraints(
    CORINFO_CLASS_HANDLE        parent,
    CORINFO_METHOD_HANDLE       method)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(parent != NULL);
    _ASSERTE(method != NULL);
    result = GetMethod(method)->SatisfiesMethodConstraints(TypeHandle(parent));

    EE_TO_JIT_TRANSITION();

    return result;
}



/*********************************************************************/
// Given a delegate target class, a target method parent class,  a  target method,
// a delegate class, a scope, the target method ref, and the delegate constructor member ref
// check if the method signature is compatible with the Invoke method of the delegate
// (under the typical instantiation of any free type variables in the memberref signatures).
// NB: arguments 2-4 could be inferred from 5-7, but are assumed to be available, and thus passed in for efficiency.
//
// objCls should be NULL if the target object is NULL
//@GENERICSVER: new (suitable for generics)
BOOL __stdcall CEEInfo::isCompatibleDelegate(
            CORINFO_CLASS_HANDLE        objCls,
            CORINFO_CLASS_HANDLE        methodParentCls,
            CORINFO_METHOD_HANDLE       method,
            CORINFO_CLASS_HANDLE        delegateCls,
            CORINFO_MODULE_HANDLE       moduleHnd,
            unsigned        methodMemberRef,
            unsigned        delegateConstructorMemberRef)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(method != NULL);
    _ASSERTE(delegateCls != NULL);

    TypeHandle delegateClsHnd = (TypeHandle) delegateCls;

    _ASSERTE(delegateClsHnd.GetMethodTable()->IsAnyDelegateClass());

    TypeHandle methodParentHnd = (TypeHandle) (methodParentCls);
    MethodDesc* pMDFtn = GetMethod(method);
    TypeHandle objClsHnd(objCls);

    EX_TRY
    {
      result = COMDelegate::ValidateCtor(objClsHnd, methodParentHnd, pMDFtn, delegateClsHnd);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions)

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
    // return the unmanaged target *if method has already been prelinked.*
void* __stdcall CEEInfo::getPInvokeUnmanagedTarget(CORINFO_METHOD_HANDLE method,
                                                    void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void* result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    MethodDesc* ftn = GetMethod(method);
    _ASSERTE(ftn->IsNDirect());
    NDirectMethodDesc *pMD = (NDirectMethodDesc*)ftn;

    if (pMD->NDirectTargetIsImportThunk())
    {
    }
    else
    {
        result = pMD->GetNDirectTarget();
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
    // return address of fixup area for late-bound N/Direct calls.
void* __stdcall CEEInfo::getAddressOfPInvokeFixup(CORINFO_METHOD_HANDLE method,
                                                   void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void * result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    MethodDesc* ftn = GetMethod(method);
    _ASSERTE(ftn->IsNDirect());
    NDirectMethodDesc *pMD = (NDirectMethodDesc*)ftn;

    result = (LPVOID)&(pMD->GetWriteableData()->m_pNDirectTarget);

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}


/*********************************************************************/
CORINFO_JUST_MY_CODE_HANDLE __stdcall CEEInfo::getJustMyCodeHandle(
                CORINFO_METHOD_HANDLE       method,
                CORINFO_JUST_MY_CODE_HANDLE**ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CORINFO_JUST_MY_CODE_HANDLE result = NULL;

    if (ppIndirection)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    // Get the flag from the debugger.
    MethodDesc* ftn = GetMethod(method);
    DWORD * pFlagAddr = g_pDebugInterface->GetJMCFlagAddr(ftn->GetModule());

    result = (CORINFO_JUST_MY_CODE_HANDLE) pFlagAddr;

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}

/*********************************************************************/
    // Gets a method handle that can be used to correlate profiling data.
    // This is the IP of a native method, or the address of the descriptor struct
    // for IL.  Always guaranteed to be unique per process, and not to move. */
void __stdcall CEEInfo::GetProfilingHandle(CORINFO_METHOD_HANDLE      method,
                                           BOOL                      *pbHookFunction,
                                           void                     **pEEHandle,
                                           void                     **pProfilerHandle,
                                           BOOL                      *pbIndirectedHandles)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    MethodDesc* ftn = GetMethod(method);

    // methods with no metadata behind cannot be exposed to tools expecting metadata (profiler, debugger...)
    // they shouldnever come here as they are called out in GetCompileFlag
    _ASSERTE(!ftn->IsNoMetadata());

    extern FunctionIDMapper *g_pFuncIDMapper;
#ifdef PROFILING_SUPPORTED
    FunctionID funId = ProfToEEInterfaceImpl::_CodeIdToFunctionId((CodeID) ftn);
#else // !PROFILING_SUPPORTED
    FunctionID funId = (CodeID) ftn;
#endif // !PROFILING_SUPPORTED

    if (pEEHandle != NULL)
    {
        *pEEHandle = (void *)funId;
    }

    *pProfilerHandle = (CORINFO_PROFILING_HANDLE) g_pFuncIDMapper(funId, pbHookFunction);

    //
    // This is the JIT case, which is never indirected.
    //
    *pbIndirectedHandles = FALSE;

    EE_TO_JIT_TRANSITION();
}



/*********************************************************************/
    // Return details about EE internal data structures
void __stdcall CEEInfo::getEEInfo(CORINFO_EE_INFO *pEEInfoOut)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    ZeroMemory(pEEInfoOut, sizeof(CORINFO_EE_INFO));

    JIT_TO_EE_TRANSITION();

    InlinedCallFrame::GetEEInfo(pEEInfoOut);

    // Details about NDirectMethodFrameStandalone
    pEEInfoOut->sizeOfNDirectFrame = sizeof(NDirectMethodFrameStandalone);
    pEEInfoOut->sizeOfNDirectNegSpace = NDirectMethodFrameStandalone::GetNegSpaceSize();
    pEEInfoOut->offsetOfTransitionFrameDatum = NDirectMethodFrameStandalone::GetOffsetOfDatum();

    // Offsets into the Thread structure
    pEEInfoOut->offsetOfThreadFrame = Thread::GetOffsetOfCurrentFrame();
    pEEInfoOut->offsetOfGCState     = Thread::GetOffsetOfGCFlag();

    // Offsets into the method table.
    pEEInfoOut->offsetOfEEClass = MethodTable::GetOffsetOfEEClass();

    // Offsets into the EEClass.
    pEEInfoOut->offsetOfInterfaceTable = 0x80000000;

    // Delegate offsets
    pEEInfoOut->offsetOfDelegateInstance    = DelegateObject::GetOffsetOfTarget();
    pEEInfoOut->offsetOfDelegateFirstTarget = DelegateObject::GetOffsetOfMethodPtr();

    // Remoting offsets
    pEEInfoOut->offsetOfTransparentProxyRP = TransparentProxyObject::GetOffsetOfRP();
    pEEInfoOut->offsetOfRealProxyServer    = RealProxyObject::GetOffsetOfServerObject();


    pEEInfoOut->osType  = CORINFO_PAL;


    EE_TO_JIT_TRANSITION();
}

    // Return details about EE internal data structures
DWORD __stdcall CEEInfo::getThreadTLSIndex(void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    DWORD result = (DWORD)-1;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    result = GetThreadTLSIndex();

    // The JIT can use the optimized TLS access only if the runtime is using it as well.
    //  (This is necessaryto make managed code work well under appverifier.)
    if (GetTLSAccessMode(result) == TLSACCESS_GENERIC)
        result = (DWORD)-1;

    EE_TO_JIT_TRANSITION();

    return result;
}

const void * __stdcall CEEInfo::getInlinedCallFrameVptr(void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void * result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION();

    result = (void*)InlinedCallFrame::GetInlinedCallFrameFrameVPtr();

    EE_TO_JIT_TRANSITION();

    return result;
}


SIZE_T * __stdcall CEEInfo::getAddrModuleDomainID(CORINFO_MODULE_HANDLE   module)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    SIZE_T * result = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    Module* pModule = GetModule(module);

    result = pModule->GetAddrModuleID();

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}

LONG * __stdcall CEEInfo::getAddrOfCaptureThreadGlobal(void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    LONG * result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    result = (LONG *)&g_TrapReturningThreads;

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}



HRESULT __stdcall CEEInfo::GetErrorHRESULT(struct _EXCEPTION_POINTERS *pExceptionPointers)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    HRESULT hr = S_OK;

    JIT_TO_EE_TRANSITION();

    OBJECTREF throwable = GetThread()->LastThrownObject();

    if (throwable == NULL)
    {
        hr = S_OK;
    }
    else
    {
        hr = Security::MapToHR(throwable);
    }

    EE_TO_JIT_TRANSITION();

    return hr;
}

CORINFO_CLASS_HANDLE __stdcall CEEInfo::GetErrorClass()
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CORINFO_CLASS_HANDLE result;

    JIT_TO_EE_TRANSITION_LEAF();

    OBJECTREF throwable = GetThread()->LastThrownObject();

    if (throwable == NULL)
        result = NULL;
    else
        result = (CORINFO_CLASS_HANDLE) TypeHandle(throwable->GetMethodTable()).AsPtr();

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}



ULONG __stdcall CEEInfo::GetErrorMessage(__inout_ecount(bufferLength) LPWSTR buffer, ULONG bufferLength)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    ULONG result = 0;

    JIT_TO_EE_TRANSITION();

    OBJECTREF throwable = GetThread()->LastThrownObject();

    if (throwable != NULL)
    {
        EX_TRY
          {
              result = GetExceptionMessage(throwable, buffer, bufferLength);
          }
        EX_CATCH
          {
          }
        EX_END_CATCH(SwallowAllExceptions)
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

// This method is called from CEEInfo::FilterException which
// is run as part of the SEH filter clause for the JIT.
// It is fatal to throw an exception while running a SEH filter clause
// so our contract is NOTHROW, NOTRIGGER.
//
int __stdcall CEEInfo::FilterException(struct _EXCEPTION_POINTERS *pExceptionPointers)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    int result = 0;

    JIT_TO_EE_TRANSITION_LEAF();

    VALIDATE_BACKOUT_STACK_CONSUMPTION;

    unsigned code = pExceptionPointers->ExceptionRecord->ExceptionCode;


#ifdef _DEBUG
    if (code == EXCEPTION_ACCESS_VIOLATION)
    {
        static int hit = 0;
        if (hit++ == 0)
        {
            _ASSERTE(!"Access violation while Jitting!");
        }
        else
        {
            result = EXCEPTION_CONTINUE_SEARCH;
    }
    }
    else
#endif // _DEBUG
    // No one should be catching breakpoint
    if (code == EXCEPTION_BREAKPOINT || code == EXCEPTION_SINGLE_STEP)
    {
        result = EXCEPTION_CONTINUE_SEARCH;
    }
    else if (code != EXCEPTION_COMPLUS)
    {
        result = EXCEPTION_EXECUTE_HANDLER;
    }
    else
    {
        GCX_COOP();

        OBJECTREF throwable = CLRException::GetThrowableFromExceptionRecord(pExceptionPointers->ExceptionRecord);

        if (throwable != NULL)
        {
            GCPROTECT_BEGIN(throwable);

            // Don't catch ThreadAbort and other uncatchable exceptions
            if (IsUncatchable(&throwable))
                result = EXCEPTION_CONTINUE_SEARCH;
            else
                result = EXCEPTION_EXECUTE_HANDLER;
            
            GCPROTECT_END();
        }
    }

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}

CORINFO_MODULE_HANDLE __stdcall CEEInfo::embedModuleHandle(CORINFO_MODULE_HANDLE handle,
                                                             void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
        CONSISTENCY_CHECK(!IsDynamicScope(handle));
    }
    CONTRACTL_END;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    EE_TO_JIT_TRANSITION_LEAF();

    return handle;
}

CORINFO_CLASS_HANDLE __stdcall CEEInfo::embedClassHandle(CORINFO_CLASS_HANDLE handle,
                                                           void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    EE_TO_JIT_TRANSITION_LEAF();

    return handle;
}

CORINFO_FIELD_HANDLE __stdcall CEEInfo::embedFieldHandle(CORINFO_FIELD_HANDLE handle,
                                                           void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    EE_TO_JIT_TRANSITION_LEAF();

    return handle;
}

CORINFO_METHOD_HANDLE __stdcall CEEInfo::embedMethodHandle(CORINFO_METHOD_HANDLE handle,
                                                             void **ppIndirection)
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    EE_TO_JIT_TRANSITION_LEAF();

    return handle;
}



/*********************************************************************/

int __stdcall CEEJitInfo::doAssert(const char* szFile, int iLine, const char* szExpr)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    int result = 0;

    JIT_TO_EE_TRANSITION_LEAF();

#ifdef _DEBUG
    BEGIN_DEBUG_ONLY_CODE;
    result = _DbgBreakCheck(szFile, iLine, szExpr);
    END_DEBUG_ONLY_CODE;
#else // !_DEBUG
    result = 1;   // break into debugger
#endif // !_DEBUG

    EE_TO_JIT_TRANSITION_LEAF();

    return result;
}

void __stdcall CEEJitInfo::yieldExecution()
{
    // SwitchToTask forces the current thread to give up quantum, while a host can decide what
    // to do with Sleep if the current thread has not run out of quantum yet.
    ClrSleepEx(0, FALSE);
}

BOOL __cdecl CEEJitInfo::logMsg(unsigned level, const char* fmt, va_list args)
{
    STATIC_CONTRACT_SO_TOLERANT;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_TRIGGERS;

    BOOL result = FALSE;

    JIT_TO_EE_TRANSITION();

#ifdef LOGGING
    _ASSERTE(GetThread()->PreemptiveGCDisabled());  // can be used with FJIT codeLog to quickly localize the problem

    if (LoggingOn(LF_JIT, level))
    {
        LogSpewValist(LF_JIT, level, (char*) fmt, args);
        result = TRUE;
    }
    else
#endif // LOGGING
    {
        result = FALSE;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

InfoAccessType __stdcall CEEInfo::constructStringLiteral(CORINFO_MODULE_HANDLE scopeHnd,
                                                         mdToken metaTok,
                                                         void **ppInfo)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    InfoAccessType result = IAT_PVALUE;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicScope(scopeHnd))
    {
        result = CEEDynamicCodeInfo::constructStringLiteral(scopeHnd, metaTok, ppInfo);
    }
    else
    {
        LPVOID hndStr = NULL;
        _ASSERTE(ppInfo != NULL);
        
        *ppInfo = NULL;
        
        hndStr = (LPVOID)ConstructStringLiteral(scopeHnd, metaTok); // throws

        if (hndStr == NULL)
            COMPlusThrowHR(COR_E_BADIMAGEFORMAT); // "Could not get string handle"
        
        *ppInfo = hndStr;
    }

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
HRESULT __stdcall CEEJitInfo::allocBBProfileBuffer (
    ULONG                         size,
    ICorJitInfo::ProfileBuffer ** profileBuffer
    )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    HRESULT hr = E_FAIL;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(!"allocBBProfileBuffer not implemented on CEEJitInfo!");
    hr = E_NOTIMPL;

    EE_TO_JIT_TRANSITION();
    
    return hr;
}

HRESULT __stdcall CEEJitInfo::getBBProfileData (
    CORINFO_METHOD_HANDLE         ftnHnd,
    ULONG *                       size,
    ICorJitInfo::ProfileBuffer ** profileBuffer,
    ULONG *                       numRuns
    )
{
    LEAF_CONTRACT;
    _ASSERTE(!"getBBProfileData not implemented on CEEJitInfo!");
    return E_NOTIMPL;
}

void __stdcall CEEJitInfo::allocMem (
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

            )
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    assert(coldCodeSize == 0);
    if (coldCodeBlock)
    {
        *coldCodeBlock = NULL;
    }

    ULONG codeSize      = hotCodeSize;
    void **codeBlock    = hotCodeBlock;

    size_t  roDataExtra = 0;
    size_t  rwDataExtra = 0;

    if (roDataSize > 0)
    {
        // Make sure that the codeSize a multiple of 4
        // This will insure that the first byte of
        // the roDataSize will be 4 byte aligned.
        codeSize = (ULONG) ALIGN_UP(codeSize, CODE_SIZE_ALIGN);

        if (roDataSize >= 8)
        {
            // allocates an extra 4 bytes so that we can
            // double align the roData section.
            roDataExtra = 4;
        }
    }
    if (rwDataSize > 0)
    {
        // Make sure that the codeSize a multiple of 4
        codeSize   = (ULONG) ALIGN_UP(codeSize, CODE_SIZE_ALIGN);
        // Make sure that the roDataSize a multiple of 4
        roDataSize = (ULONG) ALIGN_UP(roDataSize, 4);
        // Make sure that the rwDataSize a full cache line
        rwDataSize = (ULONG) ALIGN_UP(rwDataSize, CACHE_LINE_SIZE);

        // We also need to make sure that the rwData section
        // starts on a new cache line
        rwDataExtra = (CACHE_LINE_SIZE - 4);
    }

    S_SIZE_T totalSize = S_SIZE_T( codeSize );
    totalSize += roDataSize;
    totalSize += roDataExtra;
    totalSize += rwDataSize;
    totalSize += rwDataExtra;


#ifdef USE_INDIRECT_CODEHEADER
    // m_totalCodeHeaderSize is the amount of space which will be needed for all 
    // of the RealCodeHeaders for this function and its funclets (it is bumped up
    // funclet by funclet in reserveUnwindInfo). 
    //
    // allocCode() will allocate a single RealCodeHeader for the main method, so 
    // we need to account for that now by subtracting sizeof(RealCodeHeader)
    m_totalCodeHeaderSize -= sizeof(RealCodeHeader);

    // if we are in the LCG method case we want to put the RealCodeHeader in with
    // the rest of the code (immediately preceding the CodeHeader), this is because
    // LCG uses its own heap allocator which can throw things out but for it
    // to work easily everything needs to be a single allocation (see DynamicMethod.cpp)
    if (m_FD->IsLCGMethod())
    {
        _ASSERTE(IS_ALIGNED(CODE_SIZE_ALIGN, sizeof(void*)));
        
        totalSize += ALIGN_UP(m_totalCodeHeaderSize, CODE_SIZE_ALIGN);
    }
#endif  // USE_INDIRECT_CODEHEADER

    _ASSERTE(m_CodeHeader == 0 &&
            // The jit-compiler sometimes tries to compile a method a second time
            // if it failed the first time. In such a situation, m_CodeHeader may
            // have already been assigned. Its OK to ignore this assert in such a
            // situation - we will leak some memory, but that is acceptable
            // since this should happen very rarely.
            "Note that this may fire if the JITCompiler tries to recompile a method");

    if( totalSize.IsOverflow() )
    {
        COMPlusThrowHR(CORJIT_OUTOFMEM);
    }
    else
    {
        m_CodeHeader = m_jitManager->allocCode(m_FD, totalSize.Value(), flag);
    }

    BYTE* start = m_CodeHeader->GetCodeStartAddress();
    BYTE* current = start + codeSize;
    
#ifdef USE_INDIRECT_CODEHEADER
    // allocate RealCodeHeaders
    if (m_totalCodeHeaderSize > 0)
    {
        // we have funclets which will need code headers
        if (m_FD->IsLCGMethod())
        {
            // LCG Methods: RealCodeHeaders are allocated in allocCode in the CodeHeap so
            // that the blocks of code can easily be removed.
            m_realCodeHeaderBlock = (BYTE*) current;
            m_usedCodeHeaderSize = 0;
            current += m_totalCodeHeaderSize;
        }
        else
        {
            // allocate non LCG-method funclet RealCodeHeaders in LowFrequencyHeap
            size_t cbExtra;
            m_realCodeHeaderBlock = (BYTE*)(void*)m_FD->GetDomain()->GetLowFrequencyHeap()->AllocAlignedMem_NoThrow(m_totalCodeHeaderSize, sizeof(void*), &cbExtra);
            m_usedCodeHeaderSize = 0;
        }
    }
    else
    {
        // this method doesn't have any funclets
        m_realCodeHeaderBlock = NULL;
        m_usedCodeHeaderSize = 0;
    }
#endif  // USE_INDIRECT_CODEHEADER

    if (!m_CodeHeader 
#ifdef USE_INDIRECT_CODEHEADER
        || (m_totalCodeHeaderSize > 0 && !m_realCodeHeaderBlock) 
#endif  // USE_INDIRECT_CODEHEADER
        )
    {
        COMPlusThrowHR(CORJIT_OUTOFMEM);
    }

    // don't assign the codeBlock pointer until after we've been assured we could allocate everything
    *codeBlock = start;


    /* Do we need to 8-byte align the roData section? */
    if (roDataSize >= 8)
    {
        _ASSERTE( ALIGN_UP((size_t) current, 8) - (size_t) current <= roDataExtra );
        current = (BYTE*) ALIGN_UP((size_t) current, 8);
    }

    *roDataBlock = current;
    current += roDataSize;

    /* Do we need to cache line align the rwData section? */
    if (rwDataSize > 0)
    {
        _ASSERTE( ALIGN_UP((size_t) current, CACHE_LINE_SIZE) - (size_t) current <= rwDataExtra );
        current = (BYTE*) ALIGN_UP((size_t) current, CACHE_LINE_SIZE);
    }

    *rwDataBlock = current;
    current += rwDataSize;

    _ASSERTE(((size_t) (current - start)) <= totalSize.Value() );

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
void * __stdcall CEEJitInfo::allocGCInfo (ULONG size)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void * block = NULL;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(m_CodeHeader != 0);
    _ASSERTE(m_CodeHeader->GetGCInfo() == 0);
    block = m_jitManager->allocGCInfo(m_CodeHeader,(DWORD)size, &m_GCinfo_len);
    if (!block)
    {
        COMPlusThrowHR(CORJIT_OUTOFMEM);
    }

    _ASSERTE(m_CodeHeader->GetGCInfo() != 0 && block == m_CodeHeader->GetGCInfo());

    EE_TO_JIT_TRANSITION();

    return block;
}

void * __stdcall CEEJitInfo::getEHInfo()
{
    LEAF_CONTRACT;

    _ASSERTE(m_CodeHeader != 0);
    void * pEHInfo  = (void*) m_CodeHeader->GetEHInfo();

    return pEHInfo;
}

/*********************************************************************/
void __stdcall CEEJitInfo::setEHcount (
        unsigned      cEH)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(cEH != 0);
    _ASSERTE(m_CodeHeader != 0);
    _ASSERTE(m_CodeHeader->GetEHInfo() == 0);

    EE_ILEXCEPTION* ret;
    ret = m_jitManager->allocEHInfo(m_CodeHeader,cEH, &m_EHinfo_len);
    _ASSERTE(ret);      // allocEHInfo throws if there's not enough memory

    _ASSERTE(m_CodeHeader->GetEHInfo() != 0 && m_CodeHeader->GetEHInfo()->EHCount() == cEH);

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
void __stdcall CEEJitInfo::setEHinfo (
        unsigned      EHnumber,
        const CORINFO_EH_CLAUSE* clause)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    _ASSERTE(m_CodeHeader->GetEHInfo() != 0 && EHnumber < m_CodeHeader->GetEHInfo()->EHCount());

    EE_ILEXCEPTION_CLAUSE* pEHClause = m_CodeHeader->GetEHInfo()->EHClause(EHnumber);

    pEHClause->TryEndPC       = clause->TryLength;
    pEHClause->HandlerEndPC   = clause->HandlerLength;
    pEHClause->TryStartPC     = clause->TryOffset;
    pEHClause->HandlerStartPC = clause->HandlerOffset;
    pEHClause->ClassToken     = clause->ClassToken;
    pEHClause->Flags          = (CorExceptionFlag)clause->Flags;

    LOG((LF_EH, LL_INFO1000000, "Setting EH clause #%d for %s::%s\n", EHnumber, m_FD->m_pszDebugClassName, m_FD->m_pszDebugMethodName));
    LOG((LF_EH, LL_INFO1000000, "    Flags         : 0x%08lx  ->  0x%08lx\n",            clause->Flags,         pEHClause->Flags));
    LOG((LF_EH, LL_INFO1000000, "    TryOffset     : 0x%08lx  ->  0x%08lx (startpc)\n",  clause->TryOffset,     pEHClause->TryStartPC));
    LOG((LF_EH, LL_INFO1000000, "    TryLength     : 0x%08lx  ->  0x%08lx (endpc)\n",    clause->TryLength,     pEHClause->TryEndPC));
    LOG((LF_EH, LL_INFO1000000, "    HandlerOffset : 0x%08lx  ->  0x%08lx\n",            clause->HandlerOffset, pEHClause->HandlerStartPC));
    LOG((LF_EH, LL_INFO1000000, "    HandlerLength : 0x%08lx  ->  0x%08lx\n",            clause->HandlerLength, pEHClause->HandlerEndPC));
    LOG((LF_EH, LL_INFO1000000, "    ClassToken    : 0x%08lx  ->  0x%08lx\n",            clause->ClassToken,    pEHClause->ClassToken));
    LOG((LF_EH, LL_INFO1000000, "    FilterOffset  : 0x%08lx  ->  0x%08lx\n",            clause->FilterOffset,  pEHClause->FilterOffset));

    if (m_FD->IsDynamicMethod() &&
        ((pEHClause->Flags & COR_ILEXCEPTION_CLAUSE_FILTER) == 0) &&
        (clause->ClassToken != NULL))
    {
        pEHClause->TypeHandle = m_FD->GetDynamicMethodDesc()->GetResolver()->ResolveToken(clause->ClassToken);
        SetHasCachedTypeHandle(pEHClause);
        LOG((LF_EH, LL_INFO1000000, "  CachedTypeHandle: 0x%08lx  ->  0x%08lx\n",        clause->ClassToken,    pEHClause->TypeHandle));
    }

    EE_TO_JIT_TRANSITION();
}

/*********************************************************************/
// get individual exception handler
void __stdcall CEEJitInfo::getEHinfo(
                              CORINFO_METHOD_HANDLE  ftn,      /* IN  */
                              unsigned               EHnumber, /* IN  */
                              CORINFO_EH_CLAUSE*     clause)   /* OUT */
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    JIT_TO_EE_TRANSITION();

    if (IsDynamicMethodHandle(ftn))
    {
        CEEDynamicCodeInfo::getEHinfo(ftn, EHnumber, clause);
    }
    else
    {
        _ASSERTE(ftn == CORINFO_METHOD_HANDLE(m_FD));  // For now only support if the method being jitted
        getEHinfoHelper(ftn, EHnumber, clause, m_ILHeader);
    }

    EE_TO_JIT_TRANSITION();
}

CorInfoHelpFunc __stdcall CEEJitInfo::getNewHelper (CORINFO_CLASS_HANDLE newClsHnd,  CORINFO_METHOD_HANDLE context, mdToken classToken, CORINFO_MODULE_HANDLE tokenContext)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    CorInfoHelpFunc result = CORINFO_HELP_UNDEF;

    JIT_TO_EE_TRANSITION();

    if (context == NULL)
        context = CORINFO_METHOD_HANDLE(m_FD);

    result =  CEEInfo::getNewHelper (newClsHnd, context,classToken, tokenContext);

    EE_TO_JIT_TRANSITION();

    return result;
}

/*********************************************************************/
// Converts jit internal flags (CorJitFlag) into flags for debugger (DebugInterface::JITCodeInfo)
// This conversion just isolates the debugger from jit-internal flags.
// This should be 100% faithful and not introduce any additional switching layers.
DebugInterface::JITCodeInfo GetDebuggerJitCodeInfoFlags(DWORD flags)
{
    LEAF_CONTRACT;

    DebugInterface::JITCodeInfo jitCodeInfo = DebugInterface::JCI_NONE;

    // Note that we always track jit info now.
    if (flags & CORJIT_FLG_DEBUG_INFO)
    {
        jitCodeInfo = DebugInterface::JITCodeInfo(jitCodeInfo | DebugInterface::JCI_TRACK_DEBUG_INFO);
    }

    if (flags & CORJIT_FLG_DEBUG_CODE)
    {
        jitCodeInfo = DebugInterface::JITCodeInfo(jitCodeInfo | DebugInterface::JCI_DEBUG_CODE);
    }

    if (flags & CORJIT_FLG_DEBUG_CODE)
    {
        jitCodeInfo = DebugInterface::JITCodeInfo(jitCodeInfo | DebugInterface::JCI_JMC_PROBE);
    }

    return jitCodeInfo;
}

//
// Helper function because can't have dtors in BEGIN_SO_TOLERANT_CODE
//
CorJitResult invokeCompileMethodHelper(IJitManager *jitMgr,
                                 ICorJitInfo *comp,
                                 struct CORINFO_METHOD_INFO *info,
                                 unsigned flags,
                                 BYTE **nativeEntry,
                                 ULONG *nativeSizeOfCode)
{
    WRAPPER_CONTRACT;

    CorJitResult ret = CORJIT_OK;

    BEGIN_SO_TOLERANT_CODE(GetThread());

    ret = jitMgr->m_jit->compileMethod( comp,
                                         info,
                                         flags,
                                         nativeEntry,
                                         nativeSizeOfCode);

    if (SUCCEEDED(ret))
    {
        comp->MethodCompileComplete(info->ftn);
    }

    END_SO_TOLERANT_CODE;

    return ret;
}

/*********************************************************************/
CorJitResult invokeCompileMethod(IJitManager *jitMgr,
                                 ICorJitInfo *comp,
                                 struct CORINFO_METHOD_INFO *info,
                                 unsigned flags,
                                 BYTE **nativeEntry,
                                 ULONG *nativeSizeOfCode)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    //
    // The JIT runs in preemptive mode
    //

    GCX_PREEMP();

    CorJitResult ret = invokeCompileMethodHelper(jitMgr, comp, info, flags, nativeEntry, nativeSizeOfCode);

    //
    // Verify that we are still in preemptive mode when we return
    // from the JIT
    //

    _ASSERTE(GetThread() && GetThread()->PreemptiveGCDisabled() == FALSE);

    return ret;
}

CorJitFlag GetCompileFlagsIfGenericInstantiation(
        CORINFO_METHOD_HANDLE method,
        CorJitFlag compileFlags,
        ICorJitInfo * pCorJitInfo,
        BOOL * raiseVerificationException,
        BOOL * unverifiableGenericCode);

CorJitResult CallCompileMethodWithSEHWrapper(IJitManager *jitMgr,
                                ICorJitInfo *comp,
                                struct CORINFO_METHOD_INFO *info,
                                unsigned flags,
                                BYTE **nativeEntry,
                                ULONG *nativeSizeOfCode,
                                MethodDesc *ftn)
{
    // no dynamic contract here because SEH is used, with a finally clause
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_TRIGGERS;

    CorJitResult res = CORJIT_INTERNALERROR;

    bool debuggerTrackJITInfo = (flags & CORJIT_FLG_DEBUG_INFO) != 0;
    DebugInterface::JITCodeInfo jitCodeInfo = GetDebuggerJitCodeInfoFlags(flags);

    LOG((LF_CORDB, LL_EVERYTHING, "CallCompileMethodWithSEHWrapper called...\n"));
    // debuggerTrackJITInfo is only to be used to determine whether or not
    // to communicate with the debugger, NOT with how to generate code - use
    // flags for code gen, instead.

    PAL_TRY
    {
#ifdef FJITONLY
#else
        BOOL raiseVerificationException, unverifiableGenericCode;

        flags = GetCompileFlagsIfGenericInstantiation(
                    info->ftn,
                    (CorJitFlag)flags,
                    comp,
                    &raiseVerificationException,
                    &unverifiableGenericCode);

        if (raiseVerificationException)
            COMPlusThrow(kVerificationException);
#endif // FJITONLY

#ifdef DEBUGGING_SUPPORTED
        if (!(flags & CORJIT_FLG_IMPORT_ONLY))
            g_pDebugInterface->JITBeginning(ftn, debuggerTrackJITInfo);
#endif // DEBUGGING_SUPPORTED

        //
        // Call out to the JIT-compiler
        //

        res = invokeCompileMethod( jitMgr,
                                   comp,
                                   info,
                                   flags,
                                   nativeEntry,
                                   nativeSizeOfCode);
    }
    PAL_FINALLY
    {
#ifdef DEBUGGING_SUPPORTED
        if (!(flags & CORJIT_FLG_IMPORT_ONLY))
        {
            //
            // Notify the debugger that we have successfully jitted the function
            //
            if (ftn->IsJitted())
            {
                //
                // Nothing to do here (don't need to notify the debugger
                // because the function has already been successfully jitted)
                //
                // This is the case where we aborted the jit because of a deadlock cycle
                // in initClass.  
                //
            }
            else
            {
                if (res == CORJIT_OK)
                {
                    g_pDebugInterface->JITComplete(ftn,
                                                jitMgr->GetNativeEntry(*nativeEntry),
                                                *nativeSizeOfCode,
                                                jitCodeInfo);
                }
                else
                {
                    LOG((LF_CORDB,LL_INFO10000, "FUDW: compileMethod threw an exception, and"
                        " FilterUndoDebuggerWork is backing out the DebuggerJitInfo! ("
                        "JITInterface.cpp"));
                    g_pDebugInterface->JITComplete(ftn, 0, 0, jitCodeInfo);
                }
            }
        }
#endif // DEBUGGING_SUPPORTED
    }
    PAL_ENDTRY

    return res;
}

/*********************************************************************/
// Figures out the compile flags that are used by both JIT and NGen

/* static */ DWORD CEEInfo::GetBaseCompileFlags()
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    //
    // Figure out the code quality flags
    //

    DWORD flags = 0;
    if (g_pConfig->GenLooseExceptOrder())
        flags |= CORJIT_FLG_LOOSE_EXCEPT_ORDER;
    if (g_pConfig->JitFramed())
        flags |= CORJIT_FLG_FRAMED;
    if (g_pConfig->JitAlignLoops())
        flags |= CORJIT_FLG_ALIGN_LOOPS;

    return flags;
}

/*********************************************************************/
// Figures out (some of) the flags to use to compile the method
// Returns the new set to use

DWORD GetDebuggerCompileFlags(MethodDesc* ftn, DWORD flags)
{
    WRAPPER_CONTRACT;
    return GetDebuggerCompileFlags(ftn->GetModule(), flags);
}

DWORD GetDebuggerCompileFlags(Module* pModule, DWORD flags)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    bool debuggerTrackJITInfo = false;

#ifdef DEBUGGING_SUPPORTED

    DWORD dwDebugBits = pModule->GetDebuggerInfoBits();
    debuggerTrackJITInfo = CORDebuggerTrackJITInfo(dwDebugBits)
#ifdef PROFILING_SUPPORTED
                           || CORProfilerJITMapEnabled()
#endif
                          ;
#ifdef _DEBUG
    if (g_pConfig->GenDebugInfo())
        debuggerTrackJITInfo = true;

    if (g_pConfig->GenDebuggableCode())
        flags |= CORJIT_FLG_DEBUG_CODE;
#endif



    if (debuggerTrackJITInfo)
    {
        flags |= CORJIT_FLG_DEBUG_INFO;
    }
#endif // DEBUGGING_SUPPORTED

    if (CORDisableJITOptimizations(dwDebugBits))
    {
        flags |= CORJIT_FLG_DEBUG_CODE;
    }

    if (flags & CORJIT_FLG_IMPORT_ONLY)
    {
        // If we are only verifying the method, dont need any debug info and this
        // prevents getVars()/getBoundaries() from being called unnecessarily.
        flags &= ~(CORJIT_FLG_DEBUG_INFO|CORJIT_FLG_DEBUG_CODE);
    }

    return flags;
}

CorJitFlag GetCompileFlags(MethodDesc * ftn, DWORD flags, CORINFO_METHOD_INFO * methodInfo)
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    //
    // Get the compile flags that are shared between JIT and NGen
    //

    flags |= CEEInfo::GetBaseCompileFlags();

    //
    // Find the debugger and profiler related flags
    //

#ifdef DEBUGGING_SUPPORTED
    flags |= GetDebuggerCompileFlags(ftn, flags);
#endif

    if (g_pConfig->DoBBInstr())
        flags |= CORJIT_FLG_BBINSTR;

#ifdef PROFILING_SUPPORTED
    if (CORProfilerTrackEnterLeave()
        && !ftn->IsNoMetadata()
       )
        flags |= CORJIT_FLG_PROF_ENTERLEAVE;

    if (CORProfilerTrackTransitions())
        flags |= CORJIT_FLG_PROF_NO_PINVOKE_INLINE;
#endif // PROFILING_SUPPORTED

    /*      
           */

    // Set optimization flags

    unsigned optType = g_pConfig->GenOptimizeType();
    assert(optType <= OPT_RANDOM);

    if (optType == OPT_RANDOM)
        optType = methodInfo->ILCodeSize % OPT_RANDOM;

    const static unsigned optTypeFlags[] =
    {
        0,                      // OPT_BLENDED
        CORJIT_FLG_SIZE_OPT,    // OPT_CODE_SIZE
        CORJIT_FLG_SPEED_OPT    // OPT_CODE_SPEED
    };

    assert(optType < OPT_RANDOM);
    assert((sizeof(optTypeFlags)/sizeof(optTypeFlags[0])) == OPT_RANDOM);
    flags |= optTypeFlags[optType];
    flags |= g_pConfig->GetCpuFlag();
    flags |= g_pConfig->GetCpuModel();
    flags |= g_pConfig->GetCpuStepping();
    flags |= g_pConfig->GetCpuCapabilities();



    //
    // Verification flags
    //

#ifdef _DEBUG
    if (g_pConfig->IsJitVerificationDisabled())
        flags |= CORJIT_FLG_SKIP_VERIFICATION;
#endif // _DEBUG

    if ((flags & CORJIT_FLG_IMPORT_ONLY) == 0 && 
        Security::CanSkipVerification(ftn, FALSE)) // don't commit
        flags |= CORJIT_FLG_SKIP_VERIFICATION;


    return (CorJitFlag)flags;
}

/*********************************************************************/
// We verify generic code once and for all using the typical open type,
// and then no instantiations need to be verified.  If verification
// failed, then we need to throw an exception whenever we try
// to compile a real instantiation

CorJitFlag GetCompileFlagsIfGenericInstantiation(
        CORINFO_METHOD_HANDLE method,
        CorJitFlag compileFlags,
        ICorJitInfo * pCorJitInfo,
        BOOL * raiseVerificationException,
        BOOL * unverifiableGenericCode)
{
    *raiseVerificationException = FALSE;
    *unverifiableGenericCode = FALSE;

    // If we have already decided to skip verification, keep on going.
    if (compileFlags & CORJIT_FLG_SKIP_VERIFICATION)
        return compileFlags;

    CorInfoInstantiationVerification ver = pCorJitInfo->isInstantiationOfVerifiedGeneric(method);

    switch(ver)
    {
    case INSTVER_NOT_INSTANTIATION:
        // Non-generic, or open instantiation of a generic type/method
        return compileFlags;

    case INSTVER_GENERIC_PASSED_VERIFICATION:
        // If the typical instantiation is verifiable, there is no need
        // to verify the concrete instantiations
        return (CorJitFlag)(compileFlags | CORJIT_FLG_SKIP_VERIFICATION);

    case INSTVER_GENERIC_FAILED_VERIFICATION:

        *unverifiableGenericCode = TRUE;

        // The generic method is not verifiable.
        // Check if it has SkipVerification permission
        MethodDesc * pGenMethod = GetMethod(method)->LoadTypicalMethodDefinition();

        CORINFO_METHOD_HANDLE genMethodHandle = CORINFO_METHOD_HANDLE(pGenMethod);

        CorInfoCanSkipVerificationResult canSkipVer;
        canSkipVer = pCorJitInfo->canSkipMethodVerification(genMethodHandle, FALSE);
        
        switch(canSkipVer)
        {
            case CORINFO_VERIFICATION_CANNOT_SKIP:
            {
                // For unverifiable generic code without SkipVerification permission,
                // we cannot ask the compiler to emit CORINFO_HELP_VERIFICATION in
                // unverifiable branches as the compiler cannot determine the unverifiable
                // branches while compiling the concrete instantiation. Instead,
                // just throw a VerificationException right away.
                *raiseVerificationException = TRUE;
                return (CorJitFlag)-1; // This value will not be used
            }

            case CORINFO_VERIFICATION_CAN_SKIP:
            {
                return (CorJitFlag)(compileFlags | CORJIT_FLG_SKIP_VERIFICATION);
            }

            case CORINFO_VERIFICATION_RUNTIME_CHECK:
            {
                // Compile the method without CORJIT_FLG_SKIP_VERIFICATION.
                // The compiler will know to add a call to
                // CORINFO_HELP_VERIFICATION_RUNTIME_CHECK, and then to skip verification.
                return compileFlags;
            }
        }
    }

    _ASSERTE(!"We should never get here");
    return compileFlags;
}

// ********************************************************************

#ifdef _DEBUG
LONG g_JitCount = 0;
#endif


// ********************************************************************
//                  README!!
// ********************************************************************

// The reason that this is named UnsafeJitFunction is that this helper
// method is not thread safe!  When multiple threads get in here for
// the same pMD, ALL of them MUST return the SAME value.
// To insure that this happens you must call MakeJitWorker.
// It creates a DeadlockAware list of methods being jitted and prevents us
// from trying to jit the same method more that once.
//
// Calls to this method that occur to check if inlining can occur on x86,
// are OK since they discard the return value of this method.

TADDR UnsafeJitFunction(MethodDesc* ftn, COR_ILMETHOD_DECODER* ILHeader,
                        DWORD flags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    TADDR ret = NULL;

    COOPERATIVE_TRANSITION_BEGIN();


    IJitManager *jitMgr = NULL;

#if !defined(FJITONLY)
    jitMgr = ExecutionManager::GetJitForType(miManaged|miIL);
#else // !!defined(FJITONLY)
    jitMgr = ExecutionManager::GetJitForType(miManaged_IL_EJIT);
#endif // !!defined(FJITONLY)
    if (!jitMgr || !jitMgr->m_jit)
    {
        // Don't want to throw InvalidProgram from here.
        COMPlusThrowNonLocalized(kExecutionEngineException, L"Failed to load JIT compiler");
    }

#ifdef _DEBUG
    // This is here so we can see the name and class easily in the debugger

    LPCUTF8 cls  = ftn->GetClass() ? ftn->GetClass()->GetDebugClassName()
                                   : "GlobalFunction";
    LPCUTF8 name = ftn->GetName();


    if (ftn->IsNoMetadata())
    {
        if (ftn->IsILStub())
        {
            LOG((LF_JIT, LL_INFO10000, "{ Jitting IL Stub }\n"));
        }
        else
        {
            LOG((LF_JIT, LL_INFO10000, "{ Jitting dynamic method }\n"));
        }
    }
    else
    {
        SString methodString;
        if (LoggingOn(LF_JIT, LL_INFO10000))
            TypeString::AppendMethodDebug(methodString, ftn);

        LOG((LF_JIT, LL_INFO10000, "{ Jitting method (%p) %S %s\n", ftn, methodString.GetUnicode(), ftn->m_pszDebugMethodSignature));
    }

#endif // _DEBUG

    CORINFO_METHOD_INFO methodInfo;
    CorJitResult res;
    SLOT nativeEntry;
    ULONG sizeOfCode;

    CORINFO_METHOD_HANDLE ftnHnd;

    ftnHnd = (CORINFO_METHOD_HANDLE)ftn;

    getMethodInfoHelper(ftn, ftnHnd, ILHeader, &methodInfo);

    // If it's generic then we can only enter through an instantiated md (unless we're just verifying it)
    _ASSERTE((flags & CORJIT_FLG_IMPORT_ONLY) != 0 || !ftn->IsGenericMethodDefinition());

    // If it's an instance method then it must not be entered from a generic class
    _ASSERTE((flags & CORJIT_FLG_IMPORT_ONLY) != 0 || ftn->IsStatic() ||
             ftn->GetNumGenericClassArgs() == 0 || ftn->HasClassInstantiation());

    // method attributes and signature are consistant
    _ASSERTE(!!ftn->IsStatic() == ((methodInfo.args.callConv & CORINFO_CALLCONV_HASTHIS) == 0));

    flags = GetCompileFlags(ftn, flags, &methodInfo);

    CEEJitInfo jitInfo(ftn, ILHeader, jitMgr, CorJitFlag(flags));

    // Can the method access itself? This is a way to check if all the arugments
    // are accessible to the method.
    if (ftn->IsTypicalMethodDefinition() &&
        !jitInfo.canAccessMethod((CORINFO_METHOD_HANDLE)ftn,
                                 (CORINFO_CLASS_HANDLE) ftn->GetMethodTable(), 
                                 (CORINFO_METHOD_HANDLE)ftn,
                                 (CORINFO_CLASS_HANDLE) ftn->GetMethodTable()))
    {
        EEMethodException e(ftn);
        OBJECTREF throwable = e.GetThrowable();
        COMPlusThrow(throwable);
    }

    {
        /* There is a double indirection to call compileMethod  - can we
           improve this with the new structure? */

#if defined(ENABLE_PERF_COUNTERS)
        START_JIT_PERF();
#endif

#if defined(ENABLE_PERF_COUNTERS)
        LARGE_INTEGER CycleStart;
        QueryPerformanceCounter (&CycleStart);
#endif // defined(ENABLE_PERF_COUNTERS)

        // Note on debuggerTrackInfo arg: if we're only importing (ie, verifying/
        // checking to make sure we could JIT, but not actually generating code (
        // eg, for inlining), then DON'T TELL THE DEBUGGER about this.
        res = CallCompileMethodWithSEHWrapper(jitMgr,
                                              &jitInfo,
                                              &methodInfo,
                                              flags,
                                              &nativeEntry,
                                              &sizeOfCode,
                                              (MethodDesc*)ftn);
        LOG((LF_CORDB, LL_EVERYTHING, "Got through CallCompile MethodWithSEHWrapper\n"));

#if defined(ENABLE_PERF_COUNTERS)
        LARGE_INTEGER CycleStop;
        QueryPerformanceCounter(&CycleStop);

        GetPrivatePerfCounters().m_Jit.timeInJit = (CycleStop.QuadPart - CycleStart.QuadPart);
        GetPrivatePerfCounters().m_Jit.timeInJitBase = (CycleStop.QuadPart - g_lastTimeInJitCompilation.QuadPart);
        g_lastTimeInJitCompilation = CycleStop;

        GetPrivatePerfCounters().m_Jit.cMethodsJitted++;
        GetPrivatePerfCounters().m_Jit.cbILJitted+=methodInfo.ILCodeSize;
#endif // defined(ENABLE_PERF_COUNTERS)

#if defined(ENABLE_PERF_COUNTERS)
        STOP_JIT_PERF();
#endif

    }

    LOG((LF_JIT, LL_INFO10000, "Done Jitting method %s::%s  %s }\n",cls,name, ftn->m_pszDebugMethodSignature));

    if (SUCCEEDED(res) && (flags & CORJIT_FLG_IMPORT_ONLY))
    {
        // The method must been processed by the verifier. Note that it may
        // either have been marked as verifiable or unverifiable.
        // ie. IsVerified() does not imply IsVerifiable()
        _ASSERTE(ftn->IsVerified());

        goto done;
    }

    if (!SUCCEEDED(res))
    {
        COUNTER_ONLY(GetPrivatePerfCounters().m_Jit.cJitFailures++);

            jitInfo.BackoutJitData(jitMgr);

        if (res == CORJIT_OUTOFMEM)
            COMPlusThrowOM();

        goto exit;
    }

    LOG((LF_JIT, LL_INFO10000,
        "Jitted Entry at" FMT_ADDR "method %s::%s %s\n", DBG_ADDR(nativeEntry),
         ftn->m_pszDebugClassName, ftn->m_pszDebugMethodName, ftn->m_pszDebugMethodSignature));

    ret = (TADDR)nativeEntry;
    ClrFlushInstructionCache(nativeEntry, sizeOfCode ); 

exit:
    if (!ret)
        COMPlusThrow(kInvalidProgramException);

done: ;

#ifdef _DEBUG
    FastInterlockIncrement(&g_JitCount);
    static BOOL fHeartbeat = -1;

    if (fHeartbeat == -1)
        fHeartbeat = g_pConfig->GetConfigDWORD(L"JitHeartbeat", 0);

    if (fHeartbeat)
        printf(".");
#endif // _DEBUG

    COOPERATIVE_TRANSITION_END();
    return ret;
}


/*********************************************************************/
void* __stdcall CEEInfo::getHelperFtn(CorInfoHelpFunc    ftnNum,         /* IN  */
                                      void **            ppIndirection,  /* OUT */
                                      InfoAccessModule * pAccessModule)  /* OUT */
{
    CONTRACTL {
        SO_TOLERANT;
        NOTHROW;
        GC_TRIGGERS;
    } CONTRACTL_END;

    void* result = NULL;

    if (ppIndirection != NULL)
        *ppIndirection = NULL;

    JIT_TO_EE_TRANSITION_LEAF();

    _ASSERTE(ftnNum < CORINFO_HELP_COUNT);

    void* pfnHelper = hlpFuncTable[ftnNum].pfnHelper;

    size_t dynamicFtnNum = ((size_t)pfnHelper - 1);
    if (dynamicFtnNum < DYNAMIC_CORINFO_HELP_COUNT)
    {


        pfnHelper = hlpDynamicFuncTable[dynamicFtnNum].pfnHelper;

    }

    _ASSERTE(pfnHelper);

    result = GetEEFuncEntryPoint(pfnHelper);

    if (pAccessModule != NULL)
    {
        if (g_pConfig->UsePcRel32Calls())
            *pAccessModule = IAM_CURRENT_MODULE;
        else
            *pAccessModule = IAM_EXTERNAL_MODULE;
    }

    EE_TO_JIT_TRANSITION_LEAF();
    
    return result;
}

#endif // !DACCESS_COMPILE

EECodeInfo::EECodeInfo(METHODTOKEN token, IJitManager * pJM)
: m_methodToken(token), m_pMD(pJM->JitTokenToMethodDesc(token)), m_pJM(pJM)  WIN64_ARG(m_moduleBase(NULL))
{
    WRAPPER_CONTRACT;
    WIN64_ONLY(CommonCtor());
}


EECodeInfo::EECodeInfo(METHODTOKEN token, IJitManager * pJM, MethodDesc *pMD)
: m_methodToken(token), m_pMD(pMD), m_pJM(pJM)  WIN64_ARG(m_moduleBase(NULL))
{
    WRAPPER_CONTRACT;
    WIN64_ONLY(CommonCtor());
}


BYTE EECodeInfo::s_ceeInfoMemory[sizeof(CEEInfo)];

#ifndef DACCESS_COMPILE

void EECodeInfo::Init()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    // construct static CEEInfo
    new (s_ceeInfoMemory) CEEInfo();
}

#endif // !DACCESS_COMPILE

const char* __stdcall EECodeInfo::getMethodName(const char **moduleName /* OUT */ )
{
    CONTRACTL {
        // All ICodeInfo methods must be NOTHROW/GC_NOTRIGGER since they can
        // be used during GC.
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (moduleName)
        *moduleName = "<class name not available>";
    return "<method name not available>";
}

void         __stdcall EECodeInfo::getMethodSig(CORINFO_SIG_HANDLE    *phsig,     /* OUT */
                                                DWORD                 *pcbSigSize,/* OUT */
                                                CORINFO_MODULE_HANDLE *phscope)   /* OUT */
{
    CONTRACTL {
        // All ICodeInfo methods must be NOTHROW/GC_NOTRIGGER since they can
        // be used during GC.
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifndef DACCESS_COMPILE
    PCCOR_SIGNATURE pSig;
    DWORD cbSigSize;
    
    m_pMD->GetSig(&pSig, &cbSigSize);

    if (phsig)
        *phsig = (CORINFO_SIG_HANDLE)m_pMD->GetSig();

    if (pcbSigSize)
    {
        *pcbSigSize = cbSigSize;
    }

    if (phscope)
        *phscope = GetScopeHandle(m_pMD);
#else // DACCESS_COMPILE
    DacNotImpl();
#endif // DACCESS_COMPILE
}

LPVOID      __stdcall EECodeInfo::getStartAddress()
{
    CONTRACTL {
        // All ICodeInfo methods must be NOTHROW/GC_NOTRIGGER since they can
        // be used during GC.
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    return m_pJM->JitTokenToStartAddress(m_methodToken);
}


bool __stdcall EECodeInfo::IsSynchronized()
{
    WRAPPER_CONTRACT;
    return m_pMD->IsSynchronized() != 0;
}
bool __stdcall EECodeInfo::AcquiresInstMethodTableFromThis()
{
    WRAPPER_CONTRACT;
    return m_pMD->AcquiresInstMethodTableFromThis() != FALSE;
}
bool __stdcall EECodeInfo::RequiresInstArg()
{
    WRAPPER_CONTRACT;
    return m_pMD->RequiresInstArg() != FALSE;
}


