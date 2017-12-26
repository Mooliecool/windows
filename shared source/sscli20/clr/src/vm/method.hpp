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
// method.hpp
//
#ifndef _METHOD_H 
#define _METHOD_H

#include "cor.h"
#include "util.hpp"
#include "clsload.hpp"
#include "codeman.h"
#include "class.h"
#include "siginfo.hpp"
#include "declsec.h"
#include "methodimpl.h"
#include "typedesc.h"
#include <stddef.h>
#include "eeconfig.h"
#include "precode.h"

class Stub;
class FCallMethodDesc;
class LivePointerInfo;
class FieldDesc;
class NDirect;
class MethodDescChunk;
class MethodEntryChunk;
struct LayoutRawFieldInfo;
struct MLHeader;
class InstantiatedMethodDesc;
class DictionaryLayout;
class Dictionary;
class ZapCodeMap;
class ZapMonitor;
class GCCoverageInfo;
class DynamicMethodDesc;

typedef DPTR(DynamicMethodDesc)      PTR_DynamicMethodDesc;
typedef DPTR(InstantiatedMethodDesc) PTR_InstantiatedMethodDesc;
typedef DPTR(GCCoverageInfo)         PTR_GCCoverageInfo;
GPTR_DECL(Stub, g_preStub);
GPTR_DECL(Stub, g_UMThunkPreStub);

EXTERN_C VOID __stdcall NDirectImportThunk();

//=============================================================
// Splits methoddef token into a 1-byte and 2-byte piece for
// storage inside a methoddesc.
//=============================================================
FORCEINLINE BYTE GetTokenRange(mdToken tok)
{
    LEAF_CONTRACT;
    return (BYTE)(tok>>16);
}

FORCEINLINE VOID SplitToken(mdToken tok, BYTE *ptokrange, UINT16 *ptokremainder)
{
    LEAF_CONTRACT;
    *ptokrange = (BYTE)(tok>>16);
    *ptokremainder = (UINT16)(tok & 0x0000ffff);
}

FORCEINLINE mdToken MergeToken(BYTE tokrange, UINT16 tokremainder)
{
    LEAF_CONTRACT;
    return (tokrange << 16) | tokremainder;
}

// The MethodDesc is a union of several types. The following
// 3-bit field determines which type it is. Note that JIT'ed/non-JIT'ed
// is not represented here because this isn't known until the
// method is executed for the first time. Because any thread could
// change this bit, it has to be done in a place where access is
// synchronized.

// **** NOTE: if you add any new flags, make sure you add them to ClearFlagsOnUpdate
// so that when a method is replaced its relevant flags are updated

// Used in MethodDesc
enum MethodClassification
{
    mcIL        = 0, // IL
    mcFCall     = 1, // FCall (also includes tlbimped ctor, Delegate ctor)
    mcNDirect   = 2, // N/Direct
    mcEEImpl    = 3, // special method; implementation provided by EE (like Delegate Invoke)
    mcArray     = 4, // Array ECall
    mcInstantiated = 5, // Instantiated generic methods, including descriptors
                        // for both shared and unshared code (see InstantiatedMethodDesc)

    mcDynamic       = 7, // for method dewsc with no metadata behind
    mcCount,
};


// All flags in the MethodDesc now reside in a single 16-bit field.

enum MethodDescClassification
{
    // Method is IL, FCall etc., see MethodClassification above.
    mdcClassification                   = 0x0007,
    mdcClassificationCount              = mdcClassification+1,

    // Method is a body for a method impl (MI_MethodDesc, MI_NDirectMethodDesc, etc)
    // where the function explicitly implements IInterface.foo() instead of foo().
    mdcMethodImpl                       = 0x0008,

    // CAS Demands: Demands for Permissions that are CAS Permissions. CAS Perms are those 
    // that derive from CodeAccessPermission and need a stackwalk to evaluate demands
    // Non-CAS perms are those that don't need a stackwalk and don't derive from CodeAccessPermission. The implementor 
    // specifies the behavior on a demand. Examples: CAS: FileIOPermission. Non-CAS: PrincipalPermission.
    // This bit gets set if the demands are BCL CAS demands only. Even if there are non-BCL CAS demands, we don't set this
    // bit.
    mdcCASDemandsOnly                   = 0x0010,

    // This contains all bits that matters for calculation of methoddesc size
    mdcSizeClassificationMask           = mdcClassification
        | mdcMethodImpl
        ,

    // Method is static
    mdcStatic                           = 0x0020,

    // Temporary Security Interception.
    // Methods can now be intercepted by security. An intercepted method behaves
    // like it was an interpreted method. The Prestub at the top of the method desc
    // is replaced by an interception stub. Therefore, no back patching will occur.
    // We picked this approach to minimize the number variations given IL and native
    // code with edit and continue. E&C will need to find the real intercepted method
    // and if it is intercepted change the real stub. If E&C is enabled then there
    // is no back patching and needs to fix the pre-stub.
    mdcIntercepted                      = 0x0040,

    // Method requires linktime security checks.
    mdcRequiresLinktimeCheck            = 0x0080,

    // Method requires inheritance security checks.
    // If this bit is set, then this method demands inheritance permissions
    // or a method that this method overrides demands inheritance permissions
    // or both.
    mdcRequiresInheritanceCheck         = 0x0100,

    // The method that this method overrides requires an inheritance security check.
    // This bit is used as an optimization to avoid looking up overridden methods
    // during the inheritance check.
    mdcParentRequiresInheritanceCheck   = 0x0200,

    // Duplicate method. When a method needs to be placed in multiple slots in the
    // method table, because it could not be packed into one slot. For eg, a method
    // providing implementation for two interfaces, MethodImpl, etc
    mdcDuplicate                        = 0x0400,

    // Has this method been verified?
    mdcVerifiedState                    = 0x0800,

    // Is the method verifiable? It needs to be verified first to determine this
    mdcVerifiable                       = 0x1000,

    // Is this method elligible for inlining?
    mdcInlineEligibility                = 0x2000,

    // Is the method synchronized
    mdcSynchronized                     = 0x4000,

};


#define METHOD_MAX_RVA                          0x7FFFFFFF



// The size of this structure needs to be a multiple of 8-bytes
//
// The following members insure that the size of this structure obeys this rule
//
//  m_pDebugAlignPad
//  m_dwAlign2
//
// If the layout of this struct changes, these may need to be revisited
// to make sure the size is a multiple of 8-bytes.
//
// @GENERICS:
// Method descriptors for methods belonging to instantiated types may be shared between compatible instantiations
// Hence for reflection and elsewhere where exact types are important it's necessary to pair a method desc
// with the exact owning type handle.
//
// See genmeth.cpp for details of instantiated generic method descriptors.
class MethodDesc
{
    friend class EEClass;
    friend class MethodTableBuilder;
    friend class ArrayClass;
    friend class NDirect;
    friend class InstantiatedMethodDesc;
    friend class MDEnums;
    friend class MethodImpl;
    friend class CheckAsmOffsets;
    friend class ClrDataAccess;
    friend class ZapMonitor;

    friend class MethodDescCallSite;

public:

    enum
    {
        ALIGNMENT_SHIFT = 3,

        ALIGNMENT       = (1<<ALIGNMENT_SHIFT),
        ALIGNMENT_MASK  = (ALIGNMENT-1)
    };

#ifdef _DEBUG 

    LPCUTF8         m_pszDebugMethodName;
    LPCUTF8         m_pszDebugClassName;
    LPCUTF8         m_pszDebugMethodSignature;
    PTR_MethodTable m_pDebugMethodTable;

#ifdef STRESS_HEAP 
    PTR_GCCoverageInfo m_GcCover;
    DWORD_PTR       m_DebugAlignPad; // 32-bit: fixes 8-byte align, 64-bit: fixes 16-byte align
#endif // STRESS_HEAP

    // The following counters are invalid (overflowed) if negative
    volatile LONG   m_iPrestubCalls;            // How many calls entered the prestub
    volatile LONG   m_iPermittedPrestubCalls;   // How many times the prestub is allowed to be called for this method

    inline void InitPrestubCallChecking() {
        LEAF_CONTRACT;
        m_iPrestubCalls = 0;
        m_iPermittedPrestubCalls = 1;
    }

    inline void IncPrestubCalls() {
        LEAF_CONTRACT;

        // Since all managed apps have a finalizer thread, we would need allow
        // m_iPrestubCalls==2 (main thread and finalizer thread). However, the
        // finalizer thread does not run much code, and so we would like to
        // enforce that m_iPrestubCalls<=1 for simple single-threaded apps.
        // So ingore any code that runs on the finalizer thread
        if (GCHeap::GetGCHeap()->IsCurrentThreadFinalizer())
            return;

        if (m_iPrestubCalls >= 0) // prevent overflow
            FastInterlockIncrement(&m_iPrestubCalls);
    }

    inline void IncPermittedPrestubCalls() {
        LEAF_CONTRACT;
        if (m_iPermittedPrestubCalls >= 0) // prevent overflow
            FastInterlockIncrement(&m_iPermittedPrestubCalls);
    }

#endif // _DEBUG


    inline BOOL HasStableEntryPoint()
    {
        LEAF_CONTRACT;
        return (m_bFlags2 & enum_flag2_HasStableEntryPoint) != 0;
    }

    inline TADDR GetStableEntryPoint()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(HasStableEntryPoint());
        return *GetAddrOfSlotUnchecked();
    }

    BOOL SetStableEntryPointInterlocked(TADDR addr);

    BOOL HasTemporaryEntryPoint();
    TADDR GetTemporaryEntryPoint();

    void SetTemporaryEntryPoint(BaseDomain *pDomain, AllocMemTracker *pamTracker);

    inline BOOL HasPrecode()
    {
        LEAF_CONTRACT;
        return (m_bFlags2 & enum_flag2_HasPrecode) != 0;
    }

    inline void SetHasPrecode()
    {
        LEAF_CONTRACT;
        m_bFlags2 |= (enum_flag2_HasPrecode | enum_flag2_HasStableEntryPoint);
    }

    inline void ResetHasPrecode()
    {
        LEAF_CONTRACT;
        m_bFlags2 &= ~enum_flag2_HasPrecode;
        m_bFlags2 |= enum_flag2_HasStableEntryPoint;
    }

    inline Precode* GetPrecode()
    {
        LEAF_CONTRACT;
        PRECONDITION(HasPrecode());
        Precode* pPrecode = Precode::GetPrecodeFromEntryPoint(GetStableEntryPoint());
        PREFIX_ASSUME(pPrecode != NULL);
        return pPrecode;
    }

    inline BOOL MayHavePrecode()
    {
        WRAPPER_CONTRACT;
        return !MayHaveNativeCode() || PrestubMayInsertStub() || RequiresPrestub();
    }

    void InterlockedUpdateFlags2(BYTE bMask, BOOL fSet);

    Precode* GetOrCreatePrecode();



    inline BYTE* GetCallablePreStubAddr()
    {
        WRAPPER_CONTRACT;


        return HasStableEntryPoint() ? (BYTE*)GetStableEntryPoint() : (BYTE*)GetTemporaryEntryPoint();

    }

    // return the address of the stub
    static inline MethodDesc* GetMethodDescFromStubAddr(TADDR addr, BOOL fSpeculative = FALSE);

    DWORD GetAttrs();

    DWORD GetImplAttrs();

    // This function can lie if a method impl was used to implement
    // more than one method on this class. Use GetName(int) to indicate
    // which slot you are interested in.
    // See the TypeString class for better control over name formatting.
    LPCUTF8 GetName();

    LPCUTF8 GetName(USHORT slot);

    FORCEINLINE LPCUTF8 GetNameOnNonArrayClass()
    {
        WRAPPER_CONTRACT;
        return (GetMDImport()->GetNameOfMethodDef(GetMemberDef()));
    }

    COUNT_T GetStableHash();

    // Non-zero for InstantiatedMethodDescs
    DWORD GetNumGenericMethodArgs();

    // Return the number of class type parameters that are in scope for this method
    DWORD GetNumGenericClassArgs()
    {
        WRAPPER_CONTRACT;
        return GetMethodTable()->GetNumGenericArgs();
    }

    BOOL IsGenericMethodDefinition();

    // True if the declaring type or instantiation of method (if any) contains formal generic type parameters
    BOOL ContainsGenericVariables();

    Module* GetDefiningModuleForOpenMethod();

    // True if this has a class or method instantiation that is anything other than <object,...,object>
    BOOL HasNonObjectClassOrMethodInstantiation();


    // True if and only if this is a method desriptor for :
    // 1. a non-generic method or a generic method at its typical method instantiation
    // 2. in a non-generic class or a typical instantiation of a generic class
    // This method can be called on a non-restored method desc
    BOOL IsTypicalMethodDefinition();

    // Force a load of the (typical) constraints on the type parameters of a typical method definition,
    // detecting cyclic bounds on class and method type parameters.
    void LoadConstraintsForTypicalMethodDefinition(BOOL *pfHasCircularClassConstraints,
                                                   BOOL *pfHasCircularMethodConstraints,
                                                   ClassLoadLevel level = CLASS_LOADED);

    DWORD IsStaticInitMethod()
    {
        WRAPPER_CONTRACT;
        return IsMdClassConstructor(GetAttrs(), GetName());
    }

    inline BOOL IsMethodImpl()
    {
        LEAF_CONTRACT;
        return mdcMethodImpl & m_wFlags;
    }

    inline void ResetMethodImpl()
    {
        LEAF_CONTRACT;
        m_wFlags &= ~mdcMethodImpl;
    }

    inline DWORD IsStatic()
    {
        LEAF_CONTRACT;
        // This bit caches the following check:

#ifdef _DEBUG

        CONTRACT_VIOLATION(FaultViolation);

        // GetMDImport() can return NULL during shutdown
        if (GetMDImport() != NULL)
        {
            // It's possible that GetAttrs can fail, and if so, it will return -1.
            DWORD flags = GetAttrs();

            if (flags != (DWORD)-1)
                _ASSERTE((((m_wFlags & mdcStatic) != 0) == (IsMdStatic(flags) != 0)));
        }
#endif


        return (m_wFlags & mdcStatic) != 0;
    }

    inline void SetStatic()
    {
        LEAF_CONTRACT;
        m_wFlags |= mdcStatic;
    }

    inline void ClearStatic()
    {
        LEAF_CONTRACT;
        m_wFlags &= ~mdcStatic;
    }

    inline BOOL IsIL()
    {
        WRAPPER_CONTRACT;
        return mcIL == GetClassification()  || mcInstantiated == GetClassification();
    }

    //================================================================
    // Generics-related predicates etc.

    // True if the method descriptor is an instantiation of a generic method.
    inline BOOL HasMethodInstantiation();

    // True if the method descriptor is either an instantiation of
    // a generic method or is an instance method in an instantiated class (or both).
    BOOL HasClassOrMethodInstantiation()
    {
        WRAPPER_CONTRACT;
        return (HasClassInstantiation() || HasMethodInstantiation());
    }

    BOOL HasClassOrMethodInstantiation_NoLogging()
    {
        WRAPPER_CONTRACT;
        return (HasClassInstantiation_NoLogging() || HasMethodInstantiation());
    }

    inline BOOL HasClassInstantiation()
    {
        WRAPPER_CONTRACT;
        return GetMethodTable()->HasInstantiation();
    }

    inline BOOL HasClassInstantiation_NoLogging()
    {
        WRAPPER_CONTRACT;
        return GetMethodTable_NoLogging()->HasInstantiation();
    }

    // Return the instantiation for an instantiated generic method
    // Return NULL if not an instantiated method
    // To get the (representative) instantiation of the declaring class use GetMethodTable()->GetInstantiation()
    // NOTE: This will assert if you try to get the instantiation of a generic method def in a non-typical class
    // e.g. C<int>.m<U> will fail but C<T>.m<U> will succeed
    TypeHandle *GetMethodInstantiation();

    // As above, but will succeed on C<int>.m<U>
    // To do this it might force a load of the typical parent
    TypeHandle *LoadMethodInstantiation();

    // Return a pointer to the method dictionary for an instantiated generic method
    // The initial slots in a method dictionary are the type arguments themselves
    // Return NULL if not an instantiated method
    Dictionary* GetMethodDictionary();
    DictionaryLayout* GetDictionaryLayout();

    InstantiatedMethodDesc* AsInstantiatedMethodDesc();

    BaseDomain *GetDomain();
    // GetDomainForAllocation returns the domain with the responsibility for allocation.
    // This is called from GetMulticallableAddrOfCode when allocating a small trampoline stub for the method.
    // Normally a method in a shared domain will allocate memory for stubs in the shared domain.
    // That has to be different for DynamicMethod as they need to be allocated always in the AppDomain
    // that created the method.
    BaseDomain *GetDomainForAllocation();

    inline BOOL IsDomainNeutral();

    Module* GetLoaderModule();

    Module* GetZapModule();

    // Does this immediate item live in an NGEN module?
    BOOL IsZapped();

    // Strip off method and class instantiation if present and replace by the typical instantiation
    // e.g. C<int>.m<string> -> C<T>.m<U>.  Does not modify the MethodDesc, but returns
    // the appropriate stripped MethodDesc.
    // This is the identity function on non-instantiated method descs in non-instantiated classes
    MethodDesc* LoadTypicalMethodDefinition();

    // Strip off the method instantiation (if present) and replace by the typical instantiation
    // e.g. // C<int>.m<string> -> C<int>.m<U>.   Does not modify the MethodDesc, but returns
    // the appropriate stripped MethodDesc.
    // This is the identity function on non-instantiated method descs
    MethodDesc* StripMethodInstantiation();

    // Return the instantiation of a method's enclosing class
    // Return NULL if the enclosing class is not instantiated
    // If the method code is shared then this might be a *representative* instantiation
    //
    // See GetExactClassInstantiation if you need to get the exact
    // instantiation of a shared method desc.
    TypeHandle *GetClassInstantiation();

    // Is the code shared between multiple instantiations of class or method?
    // If so, then when compiling the code we might need to look up tokens
    // in the class or method dictionary.  Also, when debugging the exact generic arguments
    // need to be ripped off the stack, either from the this pointer or from one of the
    // extra args below.
    BOOL IsSharedByGenericInstantiations(); // shared code of any kind

    BOOL IsSharedByGenericMethodInstantiations(); // shared due to method instantiation

    // How does a method shared between generic instantiations get at
    // the extra instantiation information at runtime?  Only one of the following three
    // will ever hold:
    //
    // AcquiresInstMethodTableFromThis()
    //    The method is in a generic class but is not itself a
    // generic method (the normal case). Furthermore a "this" pointer
    // is available and we can get the exact instantiation from it.
    //
    // RequiresInstMethodTableArg()
    //    The method is shared between generic classes but is not
    // itself generic.  Furthermore no "this" pointer is given
    // (e.g. a value type method), so we pass in the exact-instantation
    // method table as an extra argument.
    //   i.e. per-inst static methods in shared-code instantiated generic
    //        classes (e.g. static void MyClass<string>::m())
    //   i.e. shared-code instance methods in instantiated generic
    //        structs (e.g. void MyValueType<string>::m())
    //
    // RequiresInstMethodDescArg()
    //    The method is itself generic and is shared between generic
    // instantiations but is not itself generic.  Furthermore
    // no "this" pointer is given (e.g. a value type method), so we pass in the
    // exact-instantation method table as an extra argument.
    //   i.e. shared-code instantiated generic methods
    //
    // These are used for direct calls to instantiated generic methods
    //     e.g. call void C::m<string>()  implemented by calculating dict(m<string>) at compile-time and passing it as an extra parameter
    //          call void C::m<!0>()      implemented by calculating dict(m<!0>) at run-time (if the caller lives in shared-class code)

    BOOL AcquiresInstMethodTableFromThis();
    BOOL RequiresInstMethodTableArg();
    BOOL RequiresInstMethodDescArg();

    // Can this method handle be given out to reflection for use in a MethodInfo
    // object?
    BOOL IsRuntimeMethodHandle();

    // Given a type handle of an object and a method that comes from some
    // superclass of the class of that object, find the instantiation of
    // that superclass, i.e. the class instantiation which will be relevant
    // to interpreting the signature of the method.  The type handle of
    // the object does not need to be given in all circumstances, in
    // particular it is only needed for MethodDescs pMD that
    // return true for pMD->RequiresInstMethodTableArg() or
    // pMD->RequiresInstMethodDescArg(). In other cases it is
    // allowed to be null.
    //
    // Will return NULL if the method is not in a generic class.
    TypeHandle *GetExactClassInstantiation(TypeHandle possibleObjType);


    BOOL SatisfiesMethodConstraints(TypeHandle thParent, BOOL fThrowIfNotSatisfied = FALSE);

    // Does this method require any kind of extra argument for instantiation information?
    inline BOOL RequiresInstArg()
    {
        WRAPPER_CONTRACT;
        return RequiresInstMethodTableArg() || RequiresInstMethodDescArg();
    }


    //================================================================
    // Classifications of kinds of MethodDescs.

    inline BOOL IsRuntimeSupplied()
    {
        WRAPPER_CONTRACT;
        return mcFCall == GetClassification()
            || mcArray == GetClassification();
    }


    inline DWORD IsArray()
    {
        WRAPPER_CONTRACT;
        return mcArray == GetClassification();
    }

    inline DWORD IsEEImpl()
    {
        WRAPPER_CONTRACT;
        return mcEEImpl == GetClassification();
    }

    inline DWORD IsNoMetadata()
    {
        WRAPPER_CONTRACT;
        return (mcDynamic == GetClassification());
    }

    inline PTR_DynamicMethodDesc GetDynamicMethodDesc();
    inline bool IsDynamicMethod();
    inline bool IsILStub();
    inline bool IsLCGMethod();

    inline DWORD IsNDirect()
    {
        WRAPPER_CONTRACT;
        return mcNDirect == GetClassification();
    }

    inline DWORD IsInterface()
    {
        WRAPPER_CONTRACT;
        return GetMethodTable()->IsInterface();
    }

    void ComputeSuppressUnmanagedCodeAccessAttr(IMDInternalImport *pImport);
    BOOL HasSuppressUnmanagedCodeAccessAttr();

     // hardcoded to return FALSE to improve code readibility
    inline DWORD IsComPlusCall()
    {
        LEAF_CONTRACT;
        return FALSE;
    }

    // Update flags in a thread safe manner.
    WORD InterlockedUpdateFlags(WORD wMask, BOOL fSet);

    inline DWORD IsInterceptedForDeclSecurity()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;        
        return m_wFlags & mdcIntercepted;
    }

    inline DWORD SetInterceptedForDeclSecurity(BOOL set)
    {
        WRAPPER_CONTRACT;
        return InterlockedUpdateFlags(mdcIntercepted, set);
    }

    inline DWORD IsInterceptedForDeclSecurityCASDemandsOnly()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;        
        return m_wFlags & mdcCASDemandsOnly;
    }

    inline DWORD SetInterceptedForDeclSecurityCASDemandsOnly(BOOL set)
    {
        WRAPPER_CONTRACT;
        return InterlockedUpdateFlags(mdcCASDemandsOnly, set);
    }

    // If the method is in an Edit and Contine (EnC) module, then
    // we DON'T want to backpatch this, ever.  We MUST always call
    // through the MethodDesc's PrestubAddr (5 bytes before the MethodDesc)
    // so that we can restore those bytes to "call prestub" if we want
    // to update the method.
    inline DWORD IsEnCMethod()
    {
        WRAPPER_CONTRACT;
        Module *pModule = GetModule();
        PREFIX_ASSUME(pModule != NULL);
        return pModule->IsEditAndContinueEnabled();
    }

    inline BOOL IsNotInline()
    {
        LEAF_CONTRACT;
        return (m_wFlags & mdcInlineEligibility);
    }

    inline void SetNotInline(BOOL bEligibility)
    {
        WRAPPER_CONTRACT;
        InterlockedUpdateFlags(mdcInlineEligibility, bEligibility);
    }


    BOOL IsIntrospectionOnly();
    VOID EnsureActive();
    CHECK CheckActivated();


    //================================================================
    // REMOTING
    //
    // IsRemoting...: These predicates indicate how are remoting
    // intercepts are implemented.
    //
    // Remoting intercepts are required for all invocations of  methods on
    // MarshalByRef classes (including virtual calls on methods
    // which end up invoking a method on the MarshalByRef class).
    //
    // Remoting intercepts are implemented by one of the following techniques:
    //  (1) Non-virtual methods: inserting a stub in DoPrestub (for non-virtual calls)
    //   See: IsRemotingInterceptedViaPrestub
    //
    //  (2) Virtual methods: by transparent proxy vtables, where all the entries in the vtable
    //      are a special hook which traps into the remoting logic
    //   See: IsRemotingInterceptedViaVirtualDispatch (context indicates
    //        if it is a virtual call)
    //
    //  (3) Non-virtual-calls on virtual methods:
    //      by forcing calls to be indirect and wrapping the
    //      call with a stub returned by GetNonVirtualSlotForVirtualMethod.
    //      (this is used when invoking virtual methods non-virtually using 'call')
    //   See: IsRemotingInterceptedViaVirtualDispatch (context indicates
    //        if it is a virtual call)
    //
    // Ultimately essentially all calls go through CTPMethodTable::OnCall in
    // remoting.cpp.
    //
    // Check if this methoddesc needs to be intercepted
    // by the context code, using a stub.
    // Also see IsRemotingInterceptedViaVirtualDispatch()
    BOOL IsRemotingInterceptedViaPrestub();

    // Check if is intercepted by the context code, using the virtual table
    // of TransparentProxy.
    // If such a function is called non-virtually, it needs to be handled specially
    BOOL IsRemotingInterceptedViaVirtualDispatch();

    BOOL MayBeRemotingIntercepted();

    //================================================================
    // Does it represent a one way method call with no out/return parameters?
    inline BOOL IsOneWay()
    {
        WRAPPER_CONTRACT;
        return (S_OK == GetMDImport()->GetCustomAttributeByName(GetMemberDef(),
                                                                "System.Runtime.Remoting.Messaging.OneWayAttribute",
                                                                NULL,
                                                                NULL));

    }

    //================================================================
    // FCalls.
    BOOL IsFCall()
    {
        WRAPPER_CONTRACT;
        return mcFCall == GetClassification();
    }

    BOOL IsFCallOrIntrinsic();

    //================================================================
    // Has the method been verified?
    // This does not mean that the IL is verifiable, just that we have
    // determined if the IL is verfiable or unverifiable.
    // (Is this is dead code since the JIT now does verification?)

    inline BOOL IsVerified()
    {
        LEAF_CONTRACT;
        return m_wFlags & mdcVerifiedState;
    }

    inline void SetIsVerified(BOOL isVerifiable)
    {
        WRAPPER_CONTRACT;

        WORD flags = isVerifiable ? (WORD(mdcVerifiedState) | WORD(mdcVerifiable))
                                  : (WORD(mdcVerifiedState));
        InterlockedUpdateFlags(flags, TRUE);
    }

    inline void ResetIsVerified()
    {
        WRAPPER_CONTRACT;
        InterlockedUpdateFlags(mdcVerifiedState | mdcVerifiable, FALSE);
    }

    BOOL IsVerifiable();

    // fThrowException is used to prevent Verifier from
    // throwin an exception on error
    // fForceVerify is to be used by tools that need to
    // force verifier to verify code even if the code is fully trusted.
    HRESULT Verify(COR_ILMETHOD_DECODER* ILHeader,
                   BOOL fThrowException,
                   BOOL fForceVerify);


    //================================================================
    //

    inline void ClearFlagsOnUpdate()
    {
        WRAPPER_CONTRACT;
        ResetIsVerified();
        SetNotInline(FALSE);
#ifdef _DEBUG 
        InitPrestubCallChecking();
#endif
    }

    // Restore the MethodDesc to it's initial, pristine state, so that
    // it can be reused for new code.
    //
    // Things to think about before calling this:
    //
    // Does the caller need to free up the jitted code for the old IL
    // (including any other IJitManager datastructures) ?
    // Does the caller guarantee thread-safety ?
    //
    void Reset();

    //================================================================
    // About the signature.

    BOOL IsVarArg();
    BOOL IsVoid();
    BOOL HasRetBuffArg();

    // Returns the # of bytes of stack required to build a call-stack
    // using the internal linearized calling convention. Includes
    // "this" pointer.
    UINT SizeOfVirtualFixedArgStack();

    // Returns the # of bytes of stack required to build a call-stack
    // using the actual calling convention used by the method. Includes
    // "this" pointer.
    UINT SizeOfActualFixedArgStack();

    // Returns the # of bytes to pop after a call. Not necessary the
    // same as SizeOfActualFixedArgStack()!
    UINT CbStackPop();

    //================================================================
    // Unboxing stubs.
    //
    // Return TRUE if this is this a special stub used to implement delegates to an
    // instance method in a value class and/or virtual methods on a value class.
    //
    // For every BoxedEntryPointStub there is associated unboxed-this-MethodDesc
    // which accepts an unboxed "this" pointer.
    //
    // The action of a typical BoxedEntryPointStub is to
    // bump up the this pointer by one word so that it points to the interior of the object
    // and then call the underlying unboxed-this-MethodDesc.
    //
    // Additionally, if the non-BoxedEntryPointStub is RequiresInstMethodTableArg()
    // then pass on the MethodTable as an extra argument to the
    // underlying unboxed-this-MethodDesc.
    BOOL IsUnboxingStub();

    void SetIsUnboxingStub()
    {    
        LEAF_CONTRACT;
        m_bFlags2 |= enum_flag2_IsUnboxingStub;
    }


    //================================================================
    // Instantiating Stubs
    //
    // Return TRUE if this is this a special stub used to implement an
    // instantiated generic method or per-instantiation static method.
    // The action of an instantiating stub is
    // * pass on a MethodTable or InstantiatedMethodDesc extra argument to shared code
    BOOL IsInstantiatingStub();


    // A wrapper stub is either an unboxing stub or an instantiating stub
    BOOL IsWrapperStub();
    MethodDesc *GetWrappedMethodDesc();
    MethodDesc *GetExistingWrappedMethodDesc();


    //==================================================================
    // Access the underlying metadata

    BOOL HasILHeader()
    {
        return IsIL() && !IsUnboxingStub() && GetRVA();
    }

    COR_ILMETHOD* GetILHeader();

    LivePointerInfo* GetLivePointerInfo( const BYTE *IP );

    BOOL HasStoredSig()
    {
        WRAPPER_CONTRACT;
        return IsEEImpl() || IsArray() || IsNoMetadata();
    }

    PCCOR_SIGNATURE GetSig();

    void GetSig(PCCOR_SIGNATURE *ppSig, DWORD *pcSig);

    IMDInternalImport* GetMDImport()
    {
        WRAPPER_CONTRACT;
        Module *pModule = GetModule();
        PREFIX_ASSUME(pModule != NULL);
        return pModule->GetMDImport();
    }

#ifndef DACCESS_COMPILE 
    IMetaDataEmit* GetEmitter()
    {
        WRAPPER_CONTRACT;
        Module *pModule = GetModule();
        PREFIX_ASSUME(pModule != NULL);
        return pModule->GetEmitter();
    }

    IMetaDataImport* GetRWImporter()
    {
        WRAPPER_CONTRACT;
        Module *pModule = GetModule();
        PREFIX_ASSUME(pModule != NULL);
        return pModule->GetRWImporter();
    }
#endif // !DACCESS_COMPILE


    inline DWORD IsCtor()
    {
        WRAPPER_CONTRACT;
        return IsMdInstanceInitializer(GetAttrs(), GetName());
    }

    inline DWORD IsStaticOrPrivate()
    {
        WRAPPER_CONTRACT;
        DWORD attr = GetAttrs();
        return IsMdStatic(attr) || IsMdPrivate(attr);
    }

    inline DWORD IsFinal()
    {
        WRAPPER_CONTRACT;
        return IsMdFinal(GetAttrs());
    }

    // does not necessarily return TRUE if private
    inline DWORD IsPrivate()
    {
        WRAPPER_CONTRACT;
        return IsMdPrivate(GetAttrs());
    }

    inline DWORD IsPublic()
    {
        WRAPPER_CONTRACT;
        return IsMdPublic(GetAttrs());
    }

    inline DWORD IsProtected()
    {
        WRAPPER_CONTRACT;
        return IsMdFamily(GetAttrs());
    }

    // does not necessarily return TRUE if virtual
    inline DWORD IsVirtual()
    {
        WRAPPER_CONTRACT;
        return IsMdVirtual(GetAttrs());
    }

    // does not necessarily return TRUE if abstract
    inline DWORD IsAbstract()
    {
        WRAPPER_CONTRACT;
        return IsMdAbstract(GetAttrs());
    }

    //==================================================================
    // Flags..

    inline void SetSynchronized()
    {
        LEAF_CONTRACT;
        m_wFlags |= mdcSynchronized;
    }

    inline DWORD IsSynchronized()
    {
        LEAF_CONTRACT;
        return (m_wFlags & mdcSynchronized) != 0;
    }

    // Be careful about races with profiler when using this method. The profiler can 
    // replace preimplemented code of the method with jitted code.
    // Avoid code patterns like if(IsPreImplemented()) { TADDR pCode = GetPreImplementedCode(); ... }.
    // Use TADDR pCode = GetPreImplementedCode(); if (pCode != NULL) { ... } instead.
    BOOL IsPreImplemented()
    {
        WRAPPER_CONTRACT;
        return GetPreImplementedCode() != NULL;
    }

    //==================================================================
    // The MethodDesc in relation to the VTable it is associated with.
    // WARNING: Not all MethodDescs have slots, nor do they all have
    // entries in MethodTables.  Beware.


    inline DWORD IsVtableMethod()
    {
        WRAPPER_CONTRACT;
        MethodTable *pMT = GetMethodTable();
        g_IBCLogger.LogMethodTableAccess(pMT);
        return
            !IsEnCAddedMethod()
            // The slot numbers are currently meaningless for
            // some unboxed-this-generic-method-instantiations
            && !(pMT->IsValueType() && !IsStatic() && !IsUnboxingStub())
            && GetSlot() < pMT->GetNumVtableSlots();
    }

    inline BOOL HasNonVtableSlot();

    // duplicate methods
    inline BOOL  IsDuplicate()
    {
        LEAF_CONTRACT;
        return (m_wFlags & mdcDuplicate) == mdcDuplicate;
    }

    void SetDuplicate()
    {
        LEAF_CONTRACT;
        // method table is not setup yet
        //_ASSERTE(!GetClass()->IsInterface());
        m_wFlags |= mdcDuplicate;
    }




    //==================================================================
    // EnC

    inline BOOL IsEnCAddedMethod();

    //==================================================================
    //

    inline EEClass* GetClass()
    {
        WRAPPER_CONTRACT;
        EEClass *pClass = GetMethodTable()->GetClass();
        PREFIX_ASSUME(pClass != NULL);
        return pClass;
    }

    inline MethodTable* GetMethodTable();
    inline MethodTable* GetMethodTable_NoLogging();

  protected:
    inline MethodTable* GetMethodTable_Unchecked();

  public:
    inline MethodDescChunk* GetMethodDescChunk();
    inline MethodDescChunk *GetMethodDescChunkUnchecked();
    inline int GetMethodDescIndex();

    // If this is an method desc. (whether non-generic shared-instantiated or exact-instantiated)
    // inside a shared class then get the method table for the representative
    // class.
    inline MethodTable* GetCanonicalMethodTable();

    Module *GetModule();
    Module *GetModule_NoLogging();

    Assembly *GetAssembly()
    {
        WRAPPER_CONTRACT;
        Module *pModule = GetModule();
        PREFIX_ASSUME(pModule != NULL);
        return pModule->GetAssembly();
    }

    //==================================================================
    // The slot number of this method in the corresponding method table.
    //
    // Use with extreme caution.  The slot number will not be
    // valid for EnC code or for MethodDescs representing instantations
    // of generic methods.  It may also not mean what you think it will mean
    // for strange method descs such as BoxedEntryPointStubs.
    //
    // In any case we should be moving to use slot numbers a lot less
    // since they make the EE code inflexible.

    inline WORD GetSlot()
    {
        WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE
        // The DAC build uses this method to test for "sanity" of a MethodDesc, and
        // doesn't need the assert.
        _ASSERTE(! IsEnCAddedMethod() || !"Cannot get slot for method added via EnC");
#endif // !DACCESS_COMPILE
        return m_wSlotNumber;
    }

    inline VOID SetSlot(WORD wSlotNum)
    {
        LEAF_CONTRACT;
        m_wSlotNumber = wSlotNum;
    }

    inline PTR_TADDR GetAddrOfSlot();
    inline PTR_TADDR GetAddrOfSlotUnchecked();

    PTR_MethodDesc GetDeclMethodDesc(UINT32 slotNumber);


    //==================================================================
    // Security...

    DWORD GetSecurityFlagsDuringPreStub();
    DWORD GetSecurityFlagsDuringClassLoad(IMDInternalImport *pInternalImport,
                           mdToken tkMethod, mdToken tkClass,
                           DWORD *dwClassDeclFlags, DWORD *dwClassNullDeclFlags,
                           DWORD *dwMethDeclFlags, DWORD *dwMethNullDeclFlags);

    inline DWORD RequiresLinktimeCheck()
    {
        LEAF_CONTRACT;
        return m_wFlags & mdcRequiresLinktimeCheck;
    }

    inline DWORD RequiresInheritanceCheck()
    {
        LEAF_CONTRACT;
        return m_wFlags & mdcRequiresInheritanceCheck;
    }

    inline DWORD ParentRequiresInheritanceCheck()
    {
        LEAF_CONTRACT;
        return m_wFlags & mdcParentRequiresInheritanceCheck;
    }

    void SetRequiresLinktimeCheck()
    {
        LEAF_CONTRACT;
        m_wFlags |= mdcRequiresLinktimeCheck;
    }

    void SetRequiresInheritanceCheck()
    {
        LEAF_CONTRACT;
        m_wFlags |= mdcRequiresInheritanceCheck;
    }

    void SetParentRequiresInheritanceCheck()
    {
        LEAF_CONTRACT;
        m_wFlags |= mdcParentRequiresInheritanceCheck;
    }

    mdMethodDef GetMemberDef();
    mdMethodDef GetMemberDef_NoLogging();

#ifdef _DEBUG 
    BOOL SanityCheck();
private:
    void SMDDebugCheck(mdMethodDef mb);
#endif // _DEBUG

public:

    void SetMemberDef(mdMethodDef mb);

    //================================================================
    // Set the offset of this method desc in a chunk table (which allows us
    // to work back to the method table/module pointer stored at the head of
    // the table.
    void SetChunkIndex(MethodDescChunk *pChunk, DWORD index);

    //================================================================
    // Code Addresses
    //
    // This covers:
    //   1. the RVA of method descriptors representing uncompiled code
    //   2. the native code address for JIT compiled IL code
    //   3. the address used to call the prestub should DoPrestub need to be run
    //   4. the actual address of the prestub (not the same as 3 on IA64)
    //   5. the address of the code for any intercepting stubs
    //      inserted by DoPrestub.
    // and probably a few other code addresses I've missed :-)

    BOOL InterlockedReplaceStub(Stub** ppStub, Stub *pNewStub);

    BOOL IsPointingToPrestub();

public:
    BOOL IsCodeReady();

    // Note: We are skipping the prestub based on addition information from the JIT.
    // (e.g. that the call is on same this ptr or that the this ptr is not null).
    // Thus we can end up with a running NGENed method for which IsPointingToNativeCode is false!
    BOOL IsPointingToNativeCode()
    {
        WRAPPER_CONTRACT;
        if (!HasStableEntryPoint())
            return FALSE;

        if (!HasPrecode())
            return TRUE;

        return GetPrecode()->IsPointingToNativeCode();
    }

    BOOL IsJitted()
    {
        WRAPPER_CONTRACT;
        // This function does what its name says only in the non-NGEN case since IsPointingToNativeCode
        // can return false for a running method in the NGEN case. See the comment above.
        _ASSERTE(!IsPreImplemented());
        return HasNativeCode();
    }

    // Be careful about races with profiler when using this method. The profiler can 
    // replace preimplemented code of the method with jitted code.
    // Avoid code patterns like if(HasNativeCode()) { TADDR pCode = GetNativeCode(); ... }.
    // Use TADDR pCode = GetNativeCode(); if (pCode != NULL) { ... } instead.
    BOOL HasNativeCode()
    {
        WRAPPER_CONTRACT;
        if (!HasStableEntryPoint())
            return FALSE;

        if (!HasPrecode())
            return TRUE;

        return GetPrecode()->GetNativeCode() != NULL;
    }

    // Native code can be on of jitted code, ngened code or fcall entrypoint
    BOOL MayHaveNativeCode()
    {
        LEAF_CONTRACT;
        return (m_bFlags2 & enum_flag2_MayHaveNativeCode) != 0;
    }

    void SetMayHaveNativeCode()
    {
        LEAF_CONTRACT;
        m_bFlags2 |= enum_flag2_MayHaveNativeCode;
    }

    BOOL ComputeMayHaveNativeCode(BOOL fEstimateForChunk = FALSE);

    ULONG GetRVA();

    void SetRVA(ULONG rva);

public:

    // Returns preimplemented code of the method if method has one.
    // Returns NULL if method has no preimplemented code.
    // Be careful about races with profiler when using this method. The profiler can 
    // replace preimplemented code of the method with jitted code.
    TADDR GetPreImplementedCode();

    //
    // GetAddrofCode is used in far too many places for different purposes.
    // It really corresponds to "GetSingleCallableAddrOfCode", i.e.
    // it returns an address which should not be invoked multiple times
    // (if you do invoke it multiple times you may end up running the
    // prestub too often).
    //
    // However, it is also called in other situations where we know the
    // method has been jitted and we want the address of the jitted code,
    // or where we know the method is a stub.  It is also used to get
    // an "identifier" for the MethodDesc, or to get an address to help
    // determine which module the MethodDesc lives in.  One day we'll clean
    // this up.
    //
    // Only call GetAddrofCode() if you know what you are doing.  If
    // you can guarantee that no virtualization is necessary, or if you can
    // guarantee that it has already happened.  For instance, the frame of a
    // stackwalk has obviously been virtualized as much as it will be.
    //
    // This address cannot be invoked multiple times. Use GetMultiCallableAddrOfCode() for that.
    // This returns either the actual address of the code if the method has already
    // been jitted (best perf) or the address of a stub
    const BYTE* GetAddrofCode();

    // This is an attempt to start to clean up the uses of GetAddrofCode.
    // Callsites which are fetching the address for the purposes of making a call should
    // call this one.
    const BYTE* GetSingleCallableAddrOfCode() { LEAF_CONTRACT; return GetAddrofCode(); }

    // This one is used to implement "ldftn".
    const BYTE* GetMultiCallableAddrOfCode(BOOL fAllowInstParam = FALSE);

    // These return an address after resolving "virtual methods" correctly, including any
    // handling of context proxies, other thunking layers and also including
    // instantiation of generic virtual methods if required.
    // The first one returns an address which cannot be invoked
    // multiple times. Use GetMultiCallableAddrOfVirtualizedCode() for that.
    //
    // The code that implements these was taken verbatim from elsewhere in the
    // codebase, and there may be subtle differences between the two, e.g. with
    // regard to thunking.
    const BYTE* GetSingleCallableAddrOfVirtualizedCode(OBJECTREF *orThis);
    const BYTE* GetMultiCallableAddrOfVirtualizedCode(OBJECTREF *orThis, TypeHandle staticTH);

    // Functions to get the "entrypoint". THIS IS ONLY CALLABLE ON IA64 BY
    // A SPECIAL CALLING CONVENTION.
    //
    // This method is to be used when you are trying to call a managed method.
    // It is equivalent to GetAddrofCode on x86, but on other platforms,
    // the entry point is defined as a pointer to the prestub area of the MethodDesc.
    // The reason for this is that we cannot put code in the prestub area and,
    // as a result, cannot recover the callee's MethodDesc for JITing without
    // passing it in a hidden way (r9 on IA64).
    const BYTE* GetMethodEntryPoint();

    BOOL IsSafeToHandoutJittedCode();
    // Functions to get the raw address of the code - uses of these should
    // be cleaned up.
    //
    // This function returns a callable address that takes into account
    // that an interception stub may always need to be called before the
    // jitted code.
    const BYTE* GetSafeAddrofCode();


    const BYTE* GetNativeAddrofCode();

    //*******************************************************************************
    // Returns the address of the native code. The native code can be one of:
    // - jitted code if !IsFCall() && !IsPreImplemented()
    // - ngened code if !IsFCall() && IsPreImplemented()
    // - fcall entrypoint if IsFCall()
    TADDR GetNativeCode();

    //*******************************************************************************
    // Address of native code slot.
    PTR_TADDR GetAddrOfNativeCodeSlot()
    {
        WRAPPER_CONTRACT;

#ifndef DACCESS_COMPILE
        _ASSERTE(HasStableEntryPoint());
#endif

        if (HasPrecode())
        {
            return GetPrecode()->GetAddrOfNativeCodeSlot();
        }
        else
        {
            return GetAddrOfSlotUnchecked();
        }
    }

    // Yet another method for getting the address of the jitted function.
    // This returns either NULL or the actual start of the jitted code.
    // Optionally return matching IJitManaged* in *ppEEJM.
    const BYTE* GetFunctionAddress(IJitManager** ppEEJM /*=NULL*/);


    //
    //
    //
    //
    //
    //
    //
    //
    static MethodDesc* FindOrCreateAssociatedMethodDesc(MethodDesc* pPrimaryMD,
                                                        MethodTable *pExactMT,
                                                        BOOL forceBoxedEntryPoint,
                                                        DWORD nGenericMethodArgs,
                                                        TypeHandle *pGenericMethodArgs,
                                                        BOOL allowInstParam,
                                                        BOOL forceRemotableMethod = FALSE,
                                                        BOOL allowCreate = TRUE,
                                                        ClassLoadLevel level = CLASS_LOADED);


    // For method descriptors which are non-generic this is the identity function
    // (except it returns the primary descriptor, not an BoxedEntryPointStub).
    //
    // For a generic method definition C<T>.m<U> this will return
    // C<object>.m<object>
    //
    // allowCreate may be set to FALSE to enforce that the method searched
    // should already be in existence - thus preventing creation and GCs during 
    // inappropriate times.
    //
    MethodDesc * FindOrCreateInstantiationAtObject(BOOL allowCreate = TRUE);

    // Given an object and an method descriptor for an instantiation of
    // a virtualized generic method, get the
    // corresponding instantiation of the target of a call.
    MethodDesc *ResolveGenericVirtualMethod(OBJECTREF *orThis);


public:

    // does this function return an object reference?
    MetaSig::RETURNTYPE ReturnsObject(
#ifdef _DEBUG 
    bool supportStringConstructors = false
#endif
        );


    void Destruct();

private:
    ARG_SLOT CallOnInterfaceWorker      (const BYTE* pUNUSED, MetaSig* pMetaSig, const ARG_SLOT* pArguments DEBUG_ARG(BOOL fPermitValueTypes));
    ARG_SLOT CallTransparentProxyWorker (const BYTE* pUNUSED, MetaSig* pMetaSig, const ARG_SLOT* pArguments DEBUG_ARG(BOOL fPermitValueTypes));
    ARG_SLOT CallTargetWorker           (const BYTE* pTarget, MetaSig* pMetaSig, const ARG_SLOT* pArguments, BOOL fCriticalCall DEBUG_ARG(BOOL fPermitValueTypes));

    ARG_SLOT CallDescr(const BYTE *pTarget, MetaSig* pMetaSig, const ARG_SLOT *pArguments, BOOL fIsStatic, BOOL fCriticalCall DEBUG_ARG(BOOL fPermitValueTypes));

public:
    //

    const BYTE* GetCallTarget(OBJECTREF* pThisObj);

    MethodImpl *GetMethodImpl();

    //================================================================
    // Precompilation (NGEN)


    BOOL IsRestored();
    void CheckRestore(ClassLoadLevel level = CLASS_LOADED);

    //================================================================
    // Running the Prestub preparation step.

    // Returns true if code should always be calling the prestub.
    BOOL ShouldAlwaysCallPrestub();

    // Returns true if the method has to be called via prestub due due 
    // to other implementation reasons (shared fcalls, interface, etc.)
    BOOL RequiresPrestub();

    // Returns true if the method has to have stable entrypoint always.
    BOOL RequiresStableEntryPoint(BOOL fEstimateForChunk = FALSE);

    BOOL PrestubMayInsertSecurityStub();

    // True if running the prestub may result in a security or remoting
    // stub being wrapped around the 'real' native
    // code for the prestub.  The 'real' code may itself still be a stub.
    BOOL PrestubMayInsertStub(CORINFO_ACCESS_FLAGS accessFlags = CORINFO_ACCESS_ANY);

    // True if running the prestub may result in generating code which
    // is a stub, either because the 'real' underlying code is a stub or
    // because PrestubMayInsertStub() is true.
    BOOL PrestubMayProduceStub(CORINFO_ACCESS_FLAGS accessFlags = CORINFO_ACCESS_ANY);

    bool CanSkipDoPrestub(MethodDesc * callerMD, 
                          BOOL                  fCheckCode,
                          CorInfoIndirectCallReason *pReason,
                          CORINFO_ACCESS_FLAGS  accessFlags = CORINFO_ACCESS_ANY);

    void DoBackpatch(MethodTable *pMT, BOOL bBackpatched = FALSE);

    const BYTE * DoPrestub(MethodTable *pDispatchingMT);


    TADDR MakeJitWorker(COR_ILMETHOD_DECODER* ILHeader, DWORD  flags);


    Stub *GetStub();

    //================================================================
    // The actual data stored in a MethodDesc follows.

protected:
    UINT16      m_wTokenRemainder;
    BYTE        m_chunkIndex;

    enum {
        // enum_flag2_HasPrecode implies that enum_flag2_HasStableEntryPoint is set.
        enum_flag2_HasStableEntryPoint      = 0x01,   // The method entrypoint is stable (either precode or actual code)
        enum_flag2_HasPrecode               = 0x02,   // Precode has been allocated for this method

        enum_flag2_IsUnboxingStub           = 0x04,
        enum_flag2_MayHaveNativeCode        = 0x08,   // May have jitted code, ngened code or fcall entrypoint.
    };
    BYTE        m_bFlags2;

    // The slot number of this MethodDesc in the vtable array.
    WORD           m_wSlotNumber;

    // Flags.
    WORD           m_wFlags;


public:
#ifdef DACCESS_COMPILE 
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

public:
    inline DWORD GetClassification()
    {
        LEAF_CONTRACT;
        return (m_wFlags & mdcClassification);
    }

    void SetSizeClassification(DWORD dwClassification)
    {
        LEAF_CONTRACT;
        _ASSERTE((dwClassification & ~mdcSizeClassificationMask) == 0);
        _ASSERTE((m_wFlags & mdcSizeClassificationMask) == 0);
        m_wFlags |= (WORD)dwClassification;
    }


// Stub Dispatch code
public:
    MethodDesc *GetInterfaceMD();
};

// Forward declarations used in MethodDescCallSite.
struct SignatureNative;
bool IsCerRootMethod(MethodDesc *pMD);

class MethodDescCallSite
{
private:
    MethodDesc* m_pMD;
    const BYTE* m_pbCallTarget;
    MetaSig     m_methodSig;
    BOOL        m_fCriticalCall;

    void FindCallTarget(OBJECTREF* porThis, TypeHandle ownerType)
    {
        CONTRACTL
        {
            MODE_COOPERATIVE;
            GC_TRIGGERS;
            THROWS;
        }
        CONTRACTL_END;

        if (!m_pbCallTarget)
            m_pbCallTarget = MethodTable::GetTargetFromMethodDescAndServer(ownerType, m_pMD, porThis, FALSE, &m_pMD);
    }

    void ForceSigWalk()
    {
        CONTRACTL
        {
            MODE_ANY;
            GC_TRIGGERS;
            THROWS;
        }
        CONTRACTL_END;

        if (m_methodSig.NeedsSigWalk())
        {
            m_methodSig.ForceSigWalk(m_pMD->IsStatic());
        }
#ifdef _DEBUG 
        m_methodSig.SetSigWalkIsPrecondition();
#endif
    }

#ifdef _DEBUG 
    __declspec(noinline) void LogWeakAssert()
    {
        LOG((LF_ASSERT, LL_WARNING, "%s::%s\n", m_pMD->m_pszDebugClassName, m_pMD->m_pszDebugMethodName));
    }
#endif // _DEBUG

    void DefaultInit(OBJECTREF* porProtectedThis, TypeHandle ownerType)
    {
        CONTRACTL
        {
            MODE_ANY;
            GC_TRIGGERS;
            THROWS;
        }
        CONTRACTL_END;

#ifdef _DEBUG 

        //
        // Check to make sure the MethodDesc is a CER root method if m_fCriticalCall is specified.
        // Note that we only check this for mscorlib.dll since it is possible for buggy user
        // code to override virtual methods without making them CER roots (for example user code that
        // overrides SafeHandle methods).
        //
        if (m_fCriticalCall)
        {
            // Retrieve the actual implementation MD (which may be different then m_pMD if m_pMD is virtual).
            MethodDesc *pImplementationMD = m_pMD;
            if (m_pMD->IsVirtual())
            {
                pImplementationMD = (*porProtectedThis)->GetMethodTable()->GetMethodDescForSlot(m_pMD->GetSlot());
            }
            
            if (pImplementationMD->GetAssembly() == SystemDomain::SystemAssembly())
            {
                CONSISTENCY_CHECK_MSG(IsCerRootMethod(pImplementationMD), "The MethodDesc being called isn't a CER root method. Only CER root methods should be called via critical calls");
            }
        }

        //
        // Make sure we are passing in a 'this' if and only if it is required
        //
        if (m_pMD->IsVtableMethod())
        {
            CONSISTENCY_CHECK_MSG(NULL != porProtectedThis, "You did not pass in the 'this' object for a vtable method");
        }
        else
        {
            if (NULL != porProtectedThis)
            {
                if (g_pConfig->GetConfigDWORD(L"AssertOnUnneededThis", 0))
                {
                    CONSISTENCY_CHECK_MSG(NULL == porProtectedThis, "You passed in a 'this' object to a non-vtable method.");
                }
                else
                {
                    LogWeakAssert();
                }

            }
        }
#endif // _DEBUG

        FindCallTarget(porProtectedThis, ownerType);
        ForceSigWalk();

        if (porProtectedThis == NULL)
        {
            // We don't have a "this" pointer - ensure that we have activated the containing module
            m_pMD->EnsureActive();
        }
    }

public:
    // instance methods must pass in the 'this' object
    // static methods must pass null
    MethodDescCallSite(MethodDesc* pMD, OBJECTREF* porProtectedThis = NULL, TypeHandle ownerType = TypeHandle(), BOOL fCriticalCall = FALSE) :
        m_pMD(pMD),
        m_pbCallTarget(NULL),
        m_methodSig(pMD),
        m_fCriticalCall(fCriticalCall)
    {
        WRAPPER_CONTRACT;
        DefaultInit(porProtectedThis, ownerType);
    }

    // instance methods must pass in the 'this' object
    // static methods must pass null
    MethodDescCallSite(MethodDesc* pMD, OBJECTHANDLE hThis, TypeHandle ownerType = TypeHandle(), BOOL fCriticalCall = FALSE) :
        m_pMD(pMD),
        m_pbCallTarget(NULL),
        m_methodSig(pMD),
        m_fCriticalCall(fCriticalCall)
    {
        WRAPPER_CONTRACT;
        DefaultInit((OBJECTREF*)hThis, ownerType);
    }

    // Used to avoid touching metadata for mscorlib methods.
    // instance methods must pass in the 'this' object
    // static methods must pass null
    MethodDescCallSite(MethodDesc* pMD, BinderMethodID id, OBJECTREF* porProtectedThis = NULL, TypeHandle ownerType = TypeHandle(), BOOL fCriticalCall = FALSE) :
        m_pMD(pMD),
        m_pbCallTarget(NULL),
        m_methodSig(pMD, id),
        m_fCriticalCall(fCriticalCall)
    {
        WRAPPER_CONTRACT;
        DefaultInit(porProtectedThis, ownerType);
    }

    // instance methods must pass in the 'this' object
    // static methods must pass null
    MethodDescCallSite(MethodDesc* pMD, MetaSig* pMetaSig, OBJECTREF* porProtectedThis = NULL, TypeHandle ownerType = TypeHandle(), BOOL fCriticalCall = FALSE) :
        m_pMD(pMD),
        m_pbCallTarget(NULL),
        m_methodSig(pMetaSig),
        m_fCriticalCall(fCriticalCall)
    {
        WRAPPER_CONTRACT;
        DefaultInit(porProtectedThis, ownerType);
    }

    // instance methods must pass in the 'this' object
    // static methods must pass null
    //
    MethodDescCallSite(MethodDesc* pMD, SignatureNative* pSigNative, BOOL bCanCacheTargetAndCrackedSig, OBJECTREF* porProtectedThis = NULL, TypeHandle ownerType = TypeHandle(), BOOL fCriticalCall = FALSE);
    
    // Used to avoid touching metadata for mscorlib methods.
    // instance methods must pass in the 'this' object
    // static methods must pass null
    MethodDescCallSite(BinderMethodID id, OBJECTREF* porProtectedThis = NULL, TypeHandle ownerType = TypeHandle(), BOOL fCriticalCall = FALSE) :
        m_pMD(
#ifdef DACCESS_COMPILE 
            NULL
#else // !DACCESS_COMPILE
            g_Mscorlib.GetMethod(id)
#endif // !DACCESS_COMPILE
            ),
        m_pbCallTarget(NULL),
        m_methodSig(m_pMD, id),
        m_fCriticalCall(fCriticalCall)
    {
        WRAPPER_CONTRACT;
        DefaultInit(porProtectedThis, ownerType);
    }

    // Used to avoid touching metadata for mscorlib methods.
    // instance methods must pass in the 'this' object
    // static methods must pass null
    MethodDescCallSite(BinderMethodID id, OBJECTHANDLE hThis, TypeHandle ownerType = TypeHandle(), BOOL fCriticalCall = FALSE) :
        m_pMD(
#ifdef DACCESS_COMPILE 
            NULL
#else // !DACCESS_COMPILE
            g_Mscorlib.GetMethod(id)
#endif // !DACCESS_COMPILE
            ),
        m_pbCallTarget(NULL),
        m_methodSig(m_pMD, id),
        m_fCriticalCall(fCriticalCall)
    {
        WRAPPER_CONTRACT;

        DefaultInit((OBJECTREF*)hThis, ownerType);
    }

    //
    // Only use this constructor if you're certain you know where
    // you're going and it cannot be affected by generics/virtual
    // dispatch/etc..
    //
    MethodDescCallSite(MethodDesc* pMD, const BYTE* pbCallTarget, BOOL fCriticalCall = FALSE) :
        m_pMD(pMD),
        m_pbCallTarget(pbCallTarget),
        m_methodSig(pMD),
        m_fCriticalCall(fCriticalCall)
    {
        WRAPPER_CONTRACT;
        ForceSigWalk();
        pMD->EnsureActive();
    }

    ARG_SLOT FORCEINLINE CallTargetWorker(const ARG_SLOT* pArguments
                                          DEBUG_ARG(BOOL fPermitValueTypes))
    {
        WRAPPER_CONTRACT;
        return m_pMD->CallTargetWorker(m_pbCallTarget, &m_methodSig, pArguments, m_fCriticalCall DEBUG_ARG(fPermitValueTypes));
    }

    MetaSig* GetMetaSig()
    {
        return &m_methodSig;
    }

    //
    // Call_RetXXX definition macros:
    //
    // These macros provide type protection for the return value from calls to managed
    // code. This should help to prevent errors like what we're seeing on 64bit where
    // the JIT64 is returning the BOOL as 1byte with the rest of the ARG_SLOT still
    // polluted by the remnants of its last value. Previously we would cast to a (BOOL)
    // and end up having if((BOOL)pMD->Call(...)) statements always being true.
    //

    // Use OTHER_ELEMENT_TYPE when defining CallXXX_RetXXX variations where the return type
    // is not in CorElementType (like LPVOID) or the return type can be one of a number of
    // CorElementTypes, like XXX_RetObjPtr which is used for all kinds of Object* return
    // types, or XXX_RetArgSlot which is unspecified.
#define OTHER_ELEMENT_TYPE -1



    // Each of these function definition macros comes in two flavors:
    // MDCALLDEF_ARG_SIGXXX() takes (const ARG_SLOT *pArguments, const MetaSig* pSig)

#define MDCALLDEF(wrappedmethod, worker, permitvaluetypes, ext, rettype, eltype)    \
        rettype wrappedmethod##ext (const ARG_SLOT* pArguments)                     \
        {                                                                           \
            WRAPPER_CONTRACT;                                                       \
            {                                                                       \
                GCX_FORBID();  /* arg array is not protected */                     \
                CONSISTENCY_CHECK(!m_methodSig.NeedsSigWalk());                     \
                CONSISTENCY_CHECK(eltype == OTHER_ELEMENT_TYPE ||                   \
                                  eltype == m_methodSig.GetReturnType());           \
            }                                                                       \
            ARG_SLOT retval;                                                        \
            retval = worker (pArguments DEBUG_ARG(permitvaluetypes));               \
            return *(rettype *)ArgSlotEndianessFixup(&retval, sizeof(rettype));     \
        }

#define MDCALLDEF_REFTYPE(wrappedmethod, worker, permitvaluetypes, ext, ptrtype, reftype)       \
        reftype FORCEINLINE wrappedmethod##ext (const ARG_SLOT* pArguments)                     \
        {                                                                                       \
            WRAPPER_CONTRACT;                                                                   \
            {                                                                                   \
                GCX_FORBID();  /* arg array is not protected */                                 \
                CONSISTENCY_CHECK(!m_methodSig.NeedsSigWalk());                                 \
                CONSISTENCY_CHECK(MetaSig::RETNONOBJ != m_methodSig.GetReturnObjectKind() ||    \
                    (m_pMD->IsCtor() && m_pMD->GetClass()->HasVarSizedInstances()));            \
            }                                                                                   \
            ARG_SLOT retval;                                                                    \
            retval = worker (pArguments DEBUG_ARG(permitvaluetypes));        \
            return ObjectTo##reftype(*(ptrtype *)                                               \
                        ArgSlotEndianessFixup(&retval, sizeof(ptrtype)));                       \
        }


    // The MDCALLDEF_XXX_VOID macros take a customized assertion and calls the worker without
    // returning a value, this is the macro that _should_ be used to define the CallXXX variations
    // (without _RetXXX extension) so that misuse will be caught at compile time.

#define MDCALLDEF_VOID(wrappedmethod, worker, permitvaluetypes)     \
        void wrappedmethod (const ARG_SLOT* pArguments)             \
        {                                                           \
            WRAPPER_CONTRACT;                                       \
            worker (pArguments DEBUG_ARG(permitvaluetypes));        \
        }

#define MDCALLDEFF_STD_RETTYPES(wrappedmethod,worker,permitvaluetypes)                                         \
        MDCALLDEF_VOID(wrappedmethod,worker,permitvaluetypes)                                                  \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetBool,   CLR_BOOL,      ELEMENT_TYPE_BOOLEAN)  \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetChar,   CLR_CHAR,      ELEMENT_TYPE_CHAR)     \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetI1,     CLR_I1,        ELEMENT_TYPE_I1)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetU1,     CLR_U1,        ELEMENT_TYPE_U1)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetI2,     CLR_I2,        ELEMENT_TYPE_I2)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetU2,     CLR_U2,        ELEMENT_TYPE_U2)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetI4,     CLR_I4,        ELEMENT_TYPE_I4)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetU4,     CLR_U4,        ELEMENT_TYPE_U4)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetI8,     CLR_I8,        ELEMENT_TYPE_I8)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetU8,     CLR_U8,        ELEMENT_TYPE_U8)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetR4,     CLR_R4,        ELEMENT_TYPE_R4)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetR8,     CLR_R8,        ELEMENT_TYPE_R8)       \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetI,      CLR_I,         ELEMENT_TYPE_I)        \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetU,      CLR_U,         ELEMENT_TYPE_U)        \
        MDCALLDEF(wrappedmethod,worker,permitvaluetypes,     _RetArgSlot,ARG_SLOT,      OTHER_ELEMENT_TYPE)


    public:
        //
        //
        //

        //
        //
        //
        //
        //
        //
        //
        //



        MDCALLDEFF_STD_RETTYPES(Call, CallTargetWorker, FALSE)
        MDCALLDEF(              Call, CallTargetWorker, FALSE, _RetHR,        HRESULT,       OTHER_ELEMENT_TYPE)
        MDCALLDEF(              Call, CallTargetWorker, FALSE, _RetObjPtr,    Object*,       OTHER_ELEMENT_TYPE)
        MDCALLDEF_REFTYPE(      Call, CallTargetWorker, FALSE, _RetOBJECTREF, Object*,       OBJECTREF)
        MDCALLDEF_REFTYPE(      Call, CallTargetWorker, FALSE, _RetSTRINGREF, StringObject*, STRINGREF)
        MDCALLDEF(              Call, CallTargetWorker, FALSE, _RetLPVOID,    LPVOID,        OTHER_ELEMENT_TYPE)

        // XXX CallWithValueTypes_RetXXX(const ARG_SLOT* pArguments);
        // XXX CallWithValueTypes_RetXXX(const ARG_SLOT* pArguments, MetaSig* sig);
        MDCALLDEF_VOID(     CallWithValueTypes, CallTargetWorker, TRUE)
        MDCALLDEF(          CallWithValueTypes, CallTargetWorker, TRUE,   _RetArgSlot,    ARG_SLOT,   OTHER_ELEMENT_TYPE)
        MDCALLDEF_REFTYPE(  CallWithValueTypes, CallTargetWorker, TRUE,   _RetOBJECTREF,  Object*,    OBJECTREF)
#undef OTHER_ELEMENT_TYPE
#undef MDCALL_ARG_SIG_STD_RETTYPES
#undef MDCALLDEF
#undef MDCALLDEF_REFTYPE
#undef MDCALLDEF_VOID
}; // MethodDescCallSite

/***********************************************************************/
/* Macros used to indicate a call to managed code is starting/ending   */
/***********************************************************************/

enum EEToManagedCallFlags
{
    EEToManagedDefault                  = 0x0000,
    EEToManagedCriticalCall             = 0x0001,
};

#define BEGIN_CALL_TO_MANAGED()                                                 \
    BEGIN_CALL_TO_MANAGEDEX(EEToManagedDefault) 

#define BEGIN_CALL_TO_MANAGEDEX(flags)                                          \
{                                                                               \
    _ASSERTE(!g_fForbidEnterEE);                                                \
    Thread* ___pCurThread = GetThread();                                        \
    DECLARE_CPFH_EH_RECORD(___pCurThread);                                      \
    INTERNAL_BEGIN_CALL_TO_MANAGEDEX_HELPER(flags)                       

#define INTERNAL_BEGIN_CALL_TO_MANAGEDEX_HELPER(flags)                          \
    _ASSERTE(___pCurThread);                                                    \
    _ASSERTE((___pCurThread->m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);      \
    _ASSERTE(!g_fSuspendOnShutdown || IsFinalizerThread());                     \
    /* If this isn't a critical transition, we need to check to see if a */     \
    /* thread abort has been requested */                                       \
    if (!(flags & EEToManagedCriticalCall))                                     \
    {                                                                           \
        TESTHOOKCALL(AppDomainCanBeUnloaded(___pCurThread->GetDomain()->GetId().m_dwId,FALSE)); \
        if (___pCurThread->IsAbortRequested()) {                                \
            ___pCurThread->HandleThreadAbort();                                 \
        }                                                                       \
    }                                                                           \
    BEGIN_SO_TOLERANT_CODE(___pCurThread);                                      \
    INSTALL_COMPLUS_EXCEPTION_HANDLEREX_NO_DECLARE();

#define END_CALL_TO_MANAGED()                                                   \
    UNINSTALL_COMPLUS_EXCEPTION_HANDLER();                                      \
    END_SO_TOLERANT_CODE;                                                       \
}


/***********************************************************************/
/* Macros that provide abstraction to the usage of DispatchCallNoEH    */
/***********************************************************************/

#define ARGHOLDER_TYPE ARG_SLOT
#define OBJECTREF_TO_ARGHOLDER(x) ObjToArgSlot(x)
#define STRINGREF_TO_ARGHOLDER(x) StringToArgSlot(x)
#define PTR_TO_ARGHOLDER(x) PtrToArgSlot(x)
#define DWORD_TO_ARGHOLDER(x)   (ARG_SLOT)x

#define PREPARE_NONVIRTUAL_CALLSITE(id) MethodDescCallSite __callSite(id);
#define PREPARE_VIRTUAL_CALLSITE(id, objref) MethodDescCallSite __callSite(id, &objref);
#define PREPARE_NONVIRTUAL_CALLSITE_USING_METHODDESC(pMD)    MethodDescCallSite __callSite(pMD);

#define CALL_MANAGED_METHOD(ret, ext, rettype, args)    \
            ret = __callSite.Call_Ret##ext (args);               

#define CALL_MANAGED_METHOD_NORET(args)    \
            __callSite.Call (args);               

#define CALL_MANAGED_METHOD_RETREF(ret, reftype, ptrtype, args)           \
            ret = __callSite.Call_Ret##reftype (args);               

#define DECLARE_ARGHOLDER_ARRAY(arg, count)                 \
        ARGHOLDER_TYPE arg[count];


#define ARGNUM_0    0
#define ARGNUM_1    1
#define ARGNUM_N(n)    n

#define ARGNUM_2 ARGNUM_N(2)
#define ARGNUM_3 ARGNUM_N(3)
#define ARGNUM_4 ARGNUM_N(4)
#define ARGNUM_5 ARGNUM_N(5)
#define ARGNUM_6 ARGNUM_N(6)
#define ARGNUM_7 ARGNUM_N(7)
#define ARGNUM_8 ARGNUM_N(8)
/******************************************************************/

class MethodDescChunk
{
    friend class CheckAsmOffsets;
#ifdef DACCESS_COMPILE 
    friend void EEClass::EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif // DACCESS_COMPILE

public:
    static UINT32 GetMaxMethodDescs(DWORD classification)
    {
        WRAPPER_CONTRACT;
        SIZE_T mdSize = GetMethodDescSize(classification);
        _ASSERTE((mdSize & MethodDesc::ALIGNMENT_MASK) == 0);
        return (UINT32)(255 / (mdSize >> MethodDesc::ALIGNMENT_SHIFT));
    }

    static UINT32 GetChunkCount(UINT32 methodDescCount, DWORD classification)
    {
        WRAPPER_CONTRACT;
        UINT32 maxMethodDescs = GetMaxMethodDescs(classification);
        return (methodDescCount + maxMethodDescs-1) / maxMethodDescs;
    }

    static MethodDescChunk *CreateChunk(LoaderHeap *pHeap, DWORD methodDescCount,
                                        DWORD classification, BYTE tokRange,
                                        MethodTable *initialMT,
                                        class AllocMemTracker *pamTracker);

    static BOOL IsValidMethodDescChunk(MethodDesc *methodDesc, MethodDescChunk *methodDescChunk);


    BOOL HasTemporaryEntryPoints()
    {
        LEAF_CONTRACT;
        return m_temporaryEntryPoints != NULL;
    }

    TADDR GetTemporaryEntryPoint(int index);

    void EnsureTemporaryEntryPointsCreated(BaseDomain *pDomain, AllocMemTracker *pamTracker)
    {
        WRAPPER_CONTRACT;
        if (m_temporaryEntryPoints == NULL)
            CreateTemporaryEntryPoints(pDomain, pamTracker);
    }

    void CreateTemporaryEntryPoints(BaseDomain *pDomain, AllocMemTracker *pamTracker);

#ifdef HAS_COMPACT_ENTRYPOINTS
    //
    // There two implementation options for temporary entrypoints:
    //
    // (1) Compact entrypoints. They provide as dense entrypoints as possible, but can't be patched
    // to point to the final code. The call to unjitted method is indirect call via slot.
    //
    // (2) Precodes. The precode will be patched to point to the final code eventually, thus
    // the temporary entrypoint can be embedded in the code. The call to unjitted method is
    // direct call to direct jump.
    //
    // We use (1) for x86 and (2) for 64-bit to get the best performance on each platform.
    //

    TADDR AllocateCompactEntryPoints(BaseDomain* pDomain, AllocMemTracker *pamTracker);

    static MethodDesc* GetMethodDescFromCompactEntryPoint(TADDR addr, BOOL fSpeculative = FALSE);
    static SIZE_T SizeOfCompactEntryPoints(int count);

    static BOOL IsCompactEntryPointAtAddress(TADDR addr)
    {
#if defined(_X86_) || defined(_AMD64_)
        // Compact entrypoints start at odd addresses
        LEAF_CONTRACT;
        return (addr & 1) != 0;
#else
        #error Unsupported platform
#endif
    }
#endif // HAS_COMPACT_ENTRYPOINTS


    void SetMethodTable(MethodTable *pMT);

    inline MethodTable *GetMethodTable()
    {
        LEAF_CONTRACT;
#ifndef DACCESS_COMPILE 
#endif // !DACCESS_COMPILE
        return m_methodTable;
    }


#ifndef DACCESS_COMPILE 
    void SetNextChunk(MethodDescChunk *chunk)
    {
        LEAF_CONTRACT;
        m_next = chunk;
    }
#endif // !DACCESS_COMPILE

    MethodDescChunk *GetNextChunk()
    {
        LEAF_CONTRACT;
        return m_next;
    }

    UINT32 GetCount()
    {
        LEAF_CONTRACT;
        return m_count;
    }

#define CHUNK_KIND_MASK    0x0F
#define CHUNK_SECTION_MASK 0xc0
#define CHUNK_SECTION_WRITEABLE 0x80
#define CHUNK_SECTION_COLD 0x40
#define CHUNK_COMPACT_ENTRYPOINTS   0x20 // Compact temporary entry points
#define CHUNK_NATIVE_CODE_SLOTS     0x10 // Precodes are interleaved with the native code slots
    C_ASSERT((mdcSizeClassificationMask & ~CHUNK_KIND_MASK) == 0);

    inline DWORD GetKind()
    {
        LEAF_CONTRACT;
        
        return m_kind & CHUNK_KIND_MASK;
    }

    void SetHasCompactEntryPoints()
    {
        LEAF_CONTRACT;
        m_kind |= CHUNK_COMPACT_ENTRYPOINTS;
    }

    inline BOOL HasCompactEntryPoints()
    {
        LEAF_CONTRACT;

#ifdef HAS_COMPACT_ENTRYPOINTS
        return (m_kind & CHUNK_COMPACT_ENTRYPOINTS) != 0;
#else
        return FALSE;
#endif
    }

    void SetHasNativeCodeSlots()
    {
        LEAF_CONTRACT;
        m_kind |= CHUNK_NATIVE_CODE_SLOTS;
    }

    inline BOOL HasNativeCodeSlots()
    {
        LEAF_CONTRACT;
        return (m_kind & CHUNK_NATIVE_CODE_SLOTS) != 0;
    }

    inline BOOL IsColdChunk()
    {
        LEAF_CONTRACT;
        return (m_kind & CHUNK_SECTION_COLD) != 0;
    }
    inline void SetIsColdChunk()
    {
        LEAF_CONTRACT;
        m_kind = m_kind | CHUNK_SECTION_COLD;
    }

    BOOL IsWriteableChunk()
    {
        LEAF_CONTRACT;
        return (m_kind & CHUNK_SECTION_WRITEABLE) != 0;
    }
    void SetIsWriteableChunk()
    {
        LEAF_CONTRACT;
        m_kind = m_kind | CHUNK_SECTION_WRITEABLE;
    }

    static SIZE_T GetMethodDescSize(DWORD classification);

    SIZE_T GetMethodDescSize();

    SIZE_T GetUnit();

    inline BYTE GetTokRange()
    {
        LEAF_CONTRACT;
        return m_tokrange;
    }

    inline SIZE_T Sizeof()
    {
        WRAPPER_CONTRACT;
        return sizeof(MethodDescChunk) + GetMethodDescSize() * GetCount();
    }

    inline int GetOffsetOfFirstMethodDesc()
    {
        LEAF_CONTRACT;
        return sizeof(*this) + METHOD_PREPAD;
    }

    inline MethodDesc *GetFirstMethodDesc()
    {
        WRAPPER_CONTRACT;
        return PTR_MethodDesc(PTR_HOST_TO_TADDR(this) + GetOffsetOfFirstMethodDesc());
    }

    inline MethodDesc *GetMethodDescAt(int n)
    {
        WRAPPER_CONTRACT;
        return PTR_MethodDesc(PTR_HOST_TO_TADDR(this) + GetOffsetOfFirstMethodDesc()
                              + GetMethodDescSize() * n);
    }


#ifdef DACCESS_COMPILE 
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

private:

    void SetCount(UINT32 count)
    {
        LEAF_CONTRACT;
        m_count = count;
        _ASSERTE(GetCount() == count);
    }

        // This must be at the beginning for the asm routines to work.
    PTR_MethodTable      m_methodTable;
    PTR_MethodDescChunk  m_next;
    TADDR                m_temporaryEntryPoints;

    BYTE                 m_count;
    BYTE                 m_unit;   // the size of one method desc (in multiples of 8)

    // m_kind comprises of two parts, the top bit denotes whether chunk's methoddescs are
    // hot or cold and the lower nibble (mdcSizeClassificationMask) denotes the methoddesc kind.
    BYTE                 m_kind;
    BYTE                 m_tokrange;

    BYTE                 m_alignpad[METHOD_DESC_CHUNK_ALIGNPAD_BYTES];

    // If the MethodDescs do not have prestubs, there is a gap of METHOD_PREPAD here
    // to make address arithmetic easier.

    // Followed by array of method descs...
};


class MDEnums
{
public:
    static const int MD_IndexOffset =
        offsetof(MethodDesc, m_chunkIndex);

    static const int MD_SkewOffset =
        -METHOD_PREPAD - (int)sizeof(MethodDescChunk);
};


inline SIZE_T MethodDescChunk::GetUnit()
{
    LEAF_CONTRACT;
    return m_unit;
}

inline SIZE_T MethodDescChunk::GetMethodDescSize()
{
    WRAPPER_CONTRACT;
    SIZE_T mdSize = (GetUnit() << MethodDesc::ALIGNMENT_SHIFT);
    _ASSERTE(mdSize == GetMethodDescSize(GetKind()));
    return mdSize;
}

inline int MethodDesc::GetMethodDescIndex()
{
    LEAF_CONTRACT;

    return *PTR_BYTE(PTR_HOST_TO_TADDR(this) + MDEnums::MD_IndexOffset);
}

inline MethodDescChunk *MethodDesc::GetMethodDescChunkUnchecked()
{
    WRAPPER_CONTRACT;

    return
        PTR_MethodDescChunk(PTR_HOST_TO_TADDR(this) -
                            (GetMethodDescIndex() * MethodDesc::ALIGNMENT) +
                            MDEnums::MD_SkewOffset);
}

inline /*static*/ BOOL MethodDescChunk::IsValidMethodDescChunk(MethodDesc * methodDesc, MethodDescChunk *methodDescChunk)
{
    WRAPPER_CONTRACT;
    if (!methodDesc || !methodDescChunk)
        return FALSE;
    int mdcIndex = methodDesc->GetMethodDescIndex();
    int mdUnit = methodDescChunk->GetUnit();
    return (mdUnit != 0) && ((mdcIndex % mdUnit) == 0);
}

inline /*static*/ MethodDescChunk *MethodDesc::GetMethodDescChunk()
{
    WRAPPER_CONTRACT;
    MethodDescChunk * mdChunk = GetMethodDescChunkUnchecked();
#ifndef DACCESS_COMPILE 
    // DAC is often testing a pointer for validity, and doesn't need a pop-up.
    PREFIX_ASSUME (MethodDescChunk::IsValidMethodDescChunk (this, mdChunk));
#endif // !DACCESS_COMPILE
    return mdChunk;
}


// convert arbitrary IP location in jitted code to a MethodDesc
MethodDesc* IP2MethodDesc(const BYTE* IP);

// convert an entry point into a MethodDesc
MethodDesc* Entry2MethodDesc(const BYTE* entryPoint, MethodTable *pMT);


inline const BYTE* MethodDesc::GetAddrofCode()
{
    WRAPPER_CONTRACT;

    _ASSERTE(!IsGenericMethodDefinition());
    _ASSERTE(!IsVtableMethod() || !GetMethodTable()->IsThunking());

    return IsPointingToNativeCode() ? (BYTE*)GetNativeCode() : GetCallablePreStubAddr();
}

// Returns true if code should always be calling the prestub.
inline BOOL MethodDesc::ShouldAlwaysCallPrestub()
{
    WRAPPER_CONTRACT;
    return IsInterceptedForDeclSecurity()       ||
           IsComPlusCall()       ||
           IsNDirect()           ||
           IsEnCMethod()         ||
           IsRemotingInterceptedViaPrestub();
}

inline BOOL MethodDesc::IsSafeToHandoutJittedCode()
{
    WRAPPER_CONTRACT;
    return !ShouldAlwaysCallPrestub() && IsPointingToNativeCode();
}

inline const BYTE* MethodDesc::GetSafeAddrofCode()
{
    WRAPPER_CONTRACT;

    _ASSERTE(!IsGenericMethodDefinition());

    // See comments above.  Only call this if you are sure it is safe
    _ASSERTE(!GetMethodTable()->IsThunking());

    if (IsSafeToHandoutJittedCode())
    {
        return (BYTE*)GetNativeCode();
    }
    else
    {
        return GetCallablePreStubAddr();
    }
}

inline const BYTE* MethodDesc::GetNativeAddrofCode()
{
    WRAPPER_CONTRACT;

    const BYTE *addrOfCode = (const BYTE*)GetNativeCode();

#if !defined(DACCESS_COMPILE) 
#endif // !defined(DACCESS_COMPILE)

    return addrOfCode;
}

// This function returns the "native code" if the method has one.  
// Otherwise, it returns NULL.
// See MethodDesc::GetNativeCode() for a definition of "native code".
inline const BYTE* MethodDesc::GetFunctionAddress(IJitManager** ppEEJM = NULL)
{
    WRAPPER_CONTRACT;

    const BYTE* adr = GetNativeAddrofCode();

    if (adr == NULL)
    {
        if (ppEEJM)
            *ppEEJM = NULL;

        return NULL;
    }

    IJitManager* pEEJM = ExecutionManager::FindJitMan((SLOT)adr);

    if (ppEEJM)
        *ppEEJM = pEEJM;

    if (pEEJM == NULL)
        return NULL;

    return adr;
}

typedef DPTR(class StoredSigMethodDesc) PTR_StoredSigMethodDesc;
class StoredSigMethodDesc : public MethodDesc
{
  public:
    // Put the sig RVA in here - this allows us to avoid
    // touching the method desc table when mscorlib is prejitted.

    TADDR           m_pSig;
    DWORD           m_cSig;

    bool HasStoredMethodSig(void)
    {
        return m_pSig != 0;
    }
    PCCOR_SIGNATURE GetStoredMethodSig(DWORD* sigLen = NULL)
    {
        if (sigLen)
        {
            *sigLen = m_cSig;
        }
#ifdef DACCESS_COMPILE 
        return (PCCOR_SIGNATURE)
            DacInstantiateTypeByAddress(m_pSig, m_cSig, true);
#else // !DACCESS_COMPILE
        return (PCCOR_SIGNATURE)m_pSig;
#endif // !DACCESS_COMPILE
    }
    void SetStoredMethodSig(PCCOR_SIGNATURE sig, DWORD sigBytes)
    {
#ifndef DACCESS_COMPILE 
        m_pSig = (TADDR)sig;
        m_cSig = sigBytes;
#endif // !DACCESS_COMPILE
    }

#ifdef DACCESS_COMPILE 
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
};

//-----------------------------------------------------------------------
// Operations specific to FCall methods. We use a derived class to get
// the compiler involved in enforcing proper method type usage.
// DO NOT ADD FIELDS TO THIS CLASS.
//-----------------------------------------------------------------------

class FCallMethodDesc : public MethodDesc
{
public:
};

class HostCodeHeap;
class LCGMethodResolver;
typedef DPTR(LCGMethodResolver)       PTR_LCGMethodResolver;
class ILStubResolver;
typedef DPTR(ILStubResolver)          PTR_ILStubResolver;
class DynamicResolver;
typedef DPTR(DynamicResolver)         PTR_DynamicResolver;
//TODO: make sense of this MethodDesc with the ArrayMethodDesc
class DynamicMethodDesc : public StoredSigMethodDesc
{
    friend class ILStubCache;
    friend class DynamicMethodTable;
    friend class MethodDesc;

protected:
    PTR_CUTF8           m_pszMethodName;
    PTR_DynamicResolver m_pResolver;

    TADDR               m_slot;

    // We m_dwExtendedFlags from StoredSigMethodDesc on WIN64
    DWORD               m_dwExtendedFlags;   // see DynamicMethodDesc::ExtendedFlags enum

    typedef enum ExtendedFlags
    {
        nomdAttrs           = 0x0000FFFF, // method attributes
        nomdILStub          = 0x00010000,
        nomdLCGMethod       = 0x00020000,
        nomdUnused          = 0xFFFC0000,
    } ExtendedFlags;

public:
    bool IsILStub() { LEAF_CONTRACT; return !!(m_dwExtendedFlags & nomdILStub); }
    bool IsLCGMethod() { LEAF_CONTRACT; return !!(m_dwExtendedFlags & nomdLCGMethod); }

    inline PTR_DynamicResolver    GetResolver();
    inline PTR_LCGMethodResolver  GetLCGMethodResolver();
    inline PTR_ILStubResolver     GetILStubResolver();

    PTR_CUTF8 GetMethodName() { LEAF_CONTRACT; return m_pszMethodName; }
    WORD GetAttrs() { LEAF_CONTRACT; return (m_dwExtendedFlags & nomdAttrs); }

    //
    // following implementations defined in DynamicMethod.cpp
    //
    void Destroy(BOOL fRecycle);
};


class ArrayMethodDesc : public StoredSigMethodDesc
{
public:
    // The VTABLE for an array look like

    //  System.Object Vtable
    //  System.Array Vtable
    //  type[] Vtable
    //      Get(<rank specific)
    //      Set(<rank specific)
    //      Address(<rank specific)
    //      .ctor(int)      // Possibly more

    enum {
        ARRAY_FUNC_GET      = 0,
        ARRAY_FUNC_SET      = 1,
        ARRAY_FUNC_ADDRESS  = 2,
        ARRAY_FUNC_CTOR     = 3, // Anything >= ARRAY_FUNC_CTOR is .ctor
    };

    // Get the index of runtime provided array method
    DWORD GetArrayFuncIndex()
    {
        WRAPPER_CONTRACT;

        // The ru
        DWORD dwSlot = GetSlot();
        DWORD dwVirtuals = GetMethodTable()->GetNumVirtuals();
        _ASSERTE(dwSlot >= dwVirtuals);
        return dwSlot - dwVirtuals;
    }

    LPCUTF8 GetMethodName();
    DWORD GetAttrs();
    CorInfoIntrinsics GetIntrinsicID();
};

#ifdef HAS_NDIRECT_IMPORT_PRECODE
typedef NDirectImportPrecode NDirectImportThunkGlue;
#else // HAS_NDIRECT_IMPORT_PRECODE


class NDirectImportThunkGlue
{
public:
    LPVOID GetEntrypoint()
    {
        LEAF_CONTRACT;
        return NULL;
    }
    void Init(MethodDesc *pMethod, BaseDomain *pDomain)
    {        
        LEAF_CONTRACT;
    }
};


#endif // HAS_NDIRECT_IMPORT_PRECODE

typedef DPTR(NDirectImportThunkGlue)      PTR_NDirectImportThunkGlue;


//
// This struct consolidates the writeable parts of the NDirectMethodDesc
// so that we can eventually layout a read-only NDirectMethodDesc with a pointer
// to the writeable parts in an ngen image
//
class NDirectWriteableData
{
public:
    // The JIT generates an indirect call through this location in some cases.
    // Initialized to NDirectImportThunkGlue. Patched to the true target or 
    // host interceptor stub after linking.
    LPVOID      m_pNDirectTarget;

    MLHeader    *m_pMLHeader;           // If not ASM'ized, points to
                                        //  marshaling code and info.
};

typedef DPTR(NDirectWriteableData)      PTR_NDirectWriteableData;

//-----------------------------------------------------------------------
// Operations specific to NDirect methods. We use a derived class to get
// the compiler involved in enforcing proper method type usage.
// DO NOT ADD FIELDS TO THIS CLASS.
//-----------------------------------------------------------------------
class NDirectMethodDesc : public MethodDesc
{
    friend class ZapMonitor;

public:
    struct temp1
    {
        // If we are hosted, we will intercept m_pNDirectTarget. The true target is saved here.
        // Valid only if we are running under host.
        LPVOID      m_pNativeNDirectTarget;

        // Various attributes needed at runtime. 
        WORD        m_wFlags;

        // Information for optimized lookup of the entrypoint
        WORD        m_wEntryPointOrdinal;
        DWORD       m_dwLibIdentity;

        // Information about the entrypoint
        LPCUTF8     m_pszEntrypointName;
        LPCUTF8     m_pszLibName;

        // The writeable part of the methoddesc.
        PTR_NDirectWriteableData    m_pWriteableData;

#ifdef HAS_NDIRECT_IMPORT_PRECODE
        PTR_NDirectImportThunkGlue  m_pImportThunkGlue;        
#else // METHOD_PRECODE
        NDirectImportThunkGlue      m_ImportThunkGlue;
#endif // METHOD_PRECODE

        // Size of outgoing arguments (on stack)
        WORD        m_cbStackArgumentSize;
        WORD        m_padding;

    } ndirect;

    enum Flags
    {
        // There are two groups of flag bits here each which gets initialized
        // at different times. 

        //
        // Group 1: The init group.
        //
        //   This group is set during MethodDesc construction. No race issues
        //   here since they are initialized before the MD is ever published
        //   and never change after that.

        kEarlyBound                     = 0x0001,   // IJW managed->unmanaged thunk. Standard [sysimport] stuff otherwise.

        kHasSuppressUnmanagedCodeAccess = 0x0002,

        // kUnusedMask                  = 0x0004,

        //
        // Group 2: The runtime group.
        //
        //   This group is set during runtime potentially by multiple threads
        //   at the same time. All flags in this category has to be set via interlocked operation.
        //
        kNeedHostHook                   = 0x0008,   // Mark if we need to apply host hook

        kIsMarshalingRequiredCached     = 0x0010,   // Set if we have cached the results of marshaling required computation
        kCachedMarshalingRequired       = 0x0020,   // The result of the marshaling required computation

        kNativeAnsi                     = 0x0040,

        kLastError                      = 0x0080,   // setLastError keyword specified
        kNativeNoMangle                 = 0x0100,   // nomangle keyword specified

        kVarArgs                        = 0x0200,
        kStdCall                        = 0x0400,
        kThisCall                       = 0x0800,


        // kUnusedMask                  = 0xF800,
    };

    // Retrieves the cached result of marshaling required computation, or performs the computation
    // if the result is not cached yet.
    BOOL MarshalingRequired()
    {
        WRAPPER_CONTRACT;
        if ((ndirect.m_wFlags & kIsMarshalingRequiredCached) == 0)
        {
            // Compute the flag and cache the result
            InterlockedSetNDirectFlags(kIsMarshalingRequiredCached |
                (ComputeMarshalingRequired() ? kCachedMarshalingRequired : 0));
        }
        _ASSERTE((ndirect.m_wFlags & kIsMarshalingRequiredCached) != 0);
        return (ndirect.m_wFlags & kCachedMarshalingRequired) != 0;
    }

    BOOL ComputeMarshalingRequired();

    // Atomically set specified flags. Only setting of the bits is supported.
    void InterlockedSetNDirectFlags(WORD wFlags);

    void SetIsEarlyBound()
    {
        LEAF_CONTRACT;
        ndirect.m_wFlags |= kEarlyBound;
    }

    BOOL IsEarlyBound()
    {
        LEAF_CONTRACT;
        return FALSE;
    }

    BOOL IsNativeAnsi() const
    {
        LEAF_CONTRACT;

        return (ndirect.m_wFlags & kNativeAnsi) != 0;
    }

    BOOL IsNativeNoMangled() const
    {
        LEAF_CONTRACT;

        return (ndirect.m_wFlags & kNativeNoMangle) != 0;
    }

    BOOL HasSuppressUnmanagedCodeAccessAttr() const
    {
        LEAF_CONTRACT;

        return (ndirect.m_wFlags & kHasSuppressUnmanagedCodeAccess) != 0;
    }

    void SetSuppressUnmanagedCodeAccessAttr(BOOL value)
    {
        LEAF_CONTRACT;

        if (value)
            ndirect.m_wFlags |= kHasSuppressUnmanagedCodeAccess;
    }

    LPCUTF8 GetLibName() const
    {
        LEAF_CONTRACT;

        return ndirect.m_pszLibName;
    }

    LPCUTF8 GetEntrypointName() const
    {
        LEAF_CONTRACT;

        return ndirect.m_pszEntrypointName;
    }

    BOOL IsVarArgs()
    {
        LEAF_CONTRACT;

        return (ndirect.m_wFlags & kVarArgs) != 0;
    }

    BOOL IsStdCall()
    {
        LEAF_CONTRACT;

        return (ndirect.m_wFlags & kStdCall) != 0;
    }

    BOOL IsThisCall()
    {
        LEAF_CONTRACT;

        return (ndirect.m_wFlags & kThisCall) != 0;
    }

    NDirectWriteableData* GetWriteableData()
    {
        LEAF_CONTRACT;

        return ndirect.m_pWriteableData;
    }

    NDirectImportThunkGlue* GetNDirectImportThunkGlue()
    {
        LEAF_CONTRACT;

#ifdef HAS_NDIRECT_IMPORT_PRECODE
        return ndirect.m_pImportThunkGlue;
#else
        return &ndirect.m_ImportThunkGlue;
#endif
    }

    LPVOID GetNDirectTarget()
    {
        LEAF_CONTRACT;

        _ASSERTE(IsNDirect());
        return GetWriteableData()->m_pNDirectTarget;
    }

    LPVOID GetNativeNDirectTarget()
    {
        LEAF_CONTRACT;

        _ASSERTE(IsNDirect());
        _ASSERTE_IMPL(!NDirectTargetIsImportThunk());

        LPVOID pNativeNDirectTarget = ndirect.m_pNativeNDirectTarget;
        if (pNativeNDirectTarget != NULL)
            return pNativeNDirectTarget;

        return GetNDirectTarget();
    }

    VOID SetNDirectTarget(LPVOID pTarget);

    BOOL NeedHostHook()
    {
        LEAF_CONTRACT;

        return (ndirect.m_wFlags & kNeedHostHook) != 0;
    }

    BOOL NDirectTargetIsImportThunk()
    {
        WRAPPER_CONTRACT;

        _ASSERTE(IsNDirect());

        return (GetNDirectTarget() == GetNDirectImportThunkGlue()->GetEntrypoint());
    }

    //  Find the entry point name and function address
    //  based on the module and data from NDirectMethodDesc
    //
    //  returns true is retFuncName contains the exact entry point name which can be found
    //    in the export table, false - otherwise and if retFuncName==NULL
    //
    bool FindEntryPoint( HINSTANCE hMod, UINT16 numParamBytes, void ** retFunc, SString * retFuncName ) const;

private:
    Stub* CheckForHostHook(LPVOID pNativeTarget);
    Stub* GenerateStubForHost(LPVOID pNativeTarget);

public:
    void SetStackArgumentSize(WORD cbDstBuffer, CorPinvokeMap unmgdCallConv)
    {
        LEAF_CONTRACT;

        // the outgoing marshalling buffer. That worked for stdcalls, but for
        // thiscalls it's not a match (ecx is the this pointer).            
        ndirect.m_cbStackArgumentSize = (unmgdCallConv == pmCallConvThiscall)
            ? (cbDstBuffer - sizeof(void*)) : cbDstBuffer;
    }

    WORD GetStackArgumentSize()
    {
        LEAF_CONTRACT;

        // If we have a methoddesc, stackArgSize is the number of bytes of
        // the outgoing marshalling buffer.
        return ndirect.m_cbStackArgumentSize;
    }

    MLHeader *GetMLHeader()
    {
        LEAF_CONTRACT;

        _ASSERTE(IsNDirect());
        g_IBCLogger.LogStoredMethodDataAccess(this);
        return GetWriteableData()->m_pMLHeader;
    }

    BOOL InterlockedReplaceMLHeader(MLHeader *pMLHeader, MLHeader *pMLOldHeader);


    static UINT32 GetOffsetofNativeNDirectTarget()
    {
        LEAF_CONTRACT;

        size_t ofs = offsetof(NDirectMethodDesc, ndirect.m_pNativeNDirectTarget);
        _ASSERTE(ofs == (UINT32)ofs);
        return (UINT32)ofs;
    }
};


//-----------------------------------------------------------------------
// Operations specific to EEImplCall methods. We use a derived class to get
// the compiler involved in enforcing proper method type usage.
//
// For now, the only EE impl is the delegate Invoke method. If we
// add other EE impl types in the future, may need a discriminator
// field here.
//-----------------------------------------------------------------------
class EEImplMethodDesc : public StoredSigMethodDesc
{
public:

};


//-----------------------------------------------------------------------
// InstantiatedMethodDesc's are used for generics and
// come in four flavours, discriminated by the
// low order bits of the first field:
//
//  00 --> GenericMethodDefinition
//  01 --> UnsharedMethodInstantiation
//  10 --> SharedMethodInstantiation
//  11 --> WrapperStubWithInstantiations - and unboxing or instantiating stub
//
// A SharedMethodInstantiation descriptor extends MethodDesc
// with a pointer to dictionary layout and a representative instantiation.
//
// A GenericMethodDefinition is the instantiation of a
// generic method at its formals, used for verifying the method and
// also for reflection.
//
// A WrapperStubWithInstantiations extends MethodDesc with:
//    (1) a method instantiation
//    (2) an "underlying" method descriptor.
// A WrapperStubWithInstantiations may be placed in a MethodChunk for
// a method table which specifies an exact instantiation for the class/struct.
// A WrapperStubWithInstantiations may be either
// an BoxedEntryPointStub or an exact-instantiation stub.
//
// Exact-instantiation stubs are used as extra type-context parameters. When
// used as an entry, instantiating stubs pass an instantiation
// dictionary on to the underlying method.  These entries are required to
// implement ldftn instructions on instantiations of shared generic
// methods, as the InstantiatingStub's pointer does not expect a
// dictionary argument; instead, it passes itself on to the shared
// code as the dictionary.
//
// An UnsharedMethodInstantiation contains just an instantiation.
// These are fully-specialized wrt method and class type parameters.
// These satisfy (!IMD_IsGenericMethodDefinition() &&
//                !IMD_IsSharedByGenericMethodInstantiations() &&
//                !IMD_IsWrapperStubWithInstantiations())
//
// Note that plain MethodDescs may represent shared code w.r.t. class type
// parameters (see MethodDesc::IsSharedByGenericInstantiations()).
//-----------------------------------------------------------------------

class InstantiatedMethodDesc : public MethodDesc
{
    friend class ZapMonitor;

public:

    WORD FindDictionaryToken(DictionaryEntryLayout *pEntryLayout, WORD *offsets);

    // All varities of InstantiatedMethodDesc's support this method.
    BOOL IMD_HasMethodInstantiation()
    {
        WRAPPER_CONTRACT;
        if (IMD_IsGenericMethodDefinition())
            return TRUE;
        else
#ifndef DACCESS_COMPILE 
        if (IMD_IsSharedByGenericMethodInstantiations())
            return m_pPerInstInfo != NULL;
        else
            return m_pMethInst != NULL;
#else // DACCESS_COMPILE
        return IMD_GetMethodInstantiation() != NULL;
#endif // DACCESS_COMPILE
    }

    // All varieties of InstantiatedMethodDesc's support this method.
    TypeHandle* IMD_GetMethodInstantiation()
    {
        WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE 
        if (IMD_IsSharedByGenericMethodInstantiations())
            return m_pPerInstInfo;
        else
            return m_pMethInst;
#else // DACCESS_COMPILE
        // We could change everything to take PTR_TypeHandle
        // so that array indexing works but that touches
        // a bunch of stuff.  Instead, just pull the whole array over
        // so that the host pointer can be indexed.
        return (TypeHandle*)
            DacInstantiateTypeByAddress(m_arrayAddr, sizeof(TypeHandle) *
                                        m_wNumGenericArgs, true);
#endif // DACCESS_COMPILE
    }


    BOOL IMD_IsGenericMethodDefinition()
    {
        LEAF_CONTRACT;
        return((m_wFlags2 & KindMask) == GenericMethodDefinition);
    }

    BOOL IMD_IsSharedByGenericMethodInstantiations()
    {
        LEAF_CONTRACT;

        return((m_wFlags2 & KindMask) == SharedMethodInstantiation);
    }
    BOOL IMD_IsWrapperStubWithInstantiations()
    {
        LEAF_CONTRACT;

        return((m_wFlags2 & KindMask) == WrapperStubWithInstantiations);
    }

    BOOL IMD_IsEnCAddedMethod()
    {
        LEAF_CONTRACT;

        return FALSE;
    }

    BOOL IMD_HasNonVtableSlot()
    {
        WRAPPER_CONTRACT;
        return (m_wFlags2 & HasNonVtableSlot) != 0;
    }

    // Get the dictionary layout, if this is an instantiating stub
    DictionaryLayout* IMD_GetDictionaryLayout()
    {
        WRAPPER_CONTRACT;
        if (IMD_IsWrapperStubWithInstantiations() && IMD_HasMethodInstantiation())
            return IMD_GetWrappedMethodDesc()->AsInstantiatedMethodDesc()->m_pDictLayout;
        else
            return NULL;
    }

    MethodDesc* IMD_GetWrappedMethodDesc()
    {
        LEAF_CONTRACT;

        _ASSERTE(IMD_IsWrapperStubWithInstantiations());
        return m_pWrappedMethodDesc;
    }



    // Setup the IMD as shared code
    void SetupSharedMethodInstantiation(DWORD numGenericArgs, TypeHandle *pPerInstInfo);

    // Setup the IMD as unshared code
    void SetupUnsharedMethodInstantiation(DWORD numGenericArgs, TypeHandle *pInst);

    // Setup the IMD as the special MethodDesc for a "generic" method
    void SetupGenericMethodDefinition(IMDInternalImport *pIMDII, BaseDomain *bmtDomain, Module *pModule, mdMethodDef tok, BOOL newTypeParams);

    // Setup the IMD as a wrapper around another method desc
    void SetupWrapperStubWithInstantiations(MethodDesc* wrappedMD,DWORD numGenericArgs, TypeHandle *pGenericMethodInst);


private:
    enum
    {
        KindMask                            = 0x07,
        GenericMethodDefinition             = 0x00,
        UnsharedMethodInstantiation  = 0x01,
        SharedMethodInstantiation           = 0x02,
        WrapperStubWithInstantiations       = 0x03,


        Unrestored                          = 0x08,
        HasNonVtableSlot                    = 0x10,
    };

    friend class MethodDesc; // this fields are currently accessed by MethodDesc::Save/Restore etc.
    union {
        DictionaryLayout* m_pDictLayout; //SharedMethodInstantiation

        MethodDesc* m_pWrappedMethodDesc; // For WrapperStubWithInstantiations
    };

public:
    // Note we can't steal bits off m_pPerInstInfo as the JIT generates code to access through it!!
    union
    {
#ifndef DACCESS_COMPILE 
        // Type parameters to method (representative)
        TypeHandle* m_pPerInstInfo;  //SHARED

        // Type parameters to method (exact)
        // For non-unboxing instantiating stubs this is actually
        // a dictionary and further slots may hang off the end of the
        // instantiation.
        //
        // For generic method definitions that are not the typical method definition (e.g. C<int>.m<U>)
        // this field is null; to obtain the instantiation use LoadMethodInstantiation
        TypeHandle* m_pMethInst;  // UNSHARED, STUB, GENERIC
#else // DACCESS_COMPILE
        TADDR m_arrayAddr;
#endif // DACCESS_COMPILE
    };
private:
    WORD          m_wFlags2;
    WORD          m_wNumGenericArgs;

    TADDR         m_slot;

public:
    static InstantiatedMethodDesc* FindLoadedInstantiatedMethodDesc(MethodTable *pMT,
                                                                    mdMethodDef methodDef,
                                                                    DWORD ntypars,
                                                                    TypeHandle *typars,
                                                                    BOOL getSharedNotStub);

private:

    static InstantiatedMethodDesc *NewInstantiatedMethodDesc(MethodTable *pMT,
                                                             MethodDesc* pGenericMDescInRepMT,
                                                             MethodDesc* pSharedMDescForStub,
                                                             DWORD ntypars,
                                                             TypeHandle *typars,
                                                             BOOL getSharedNotStub);

};

// This version is used when the caller knows that MethodTable could
// be NULL. This should only happen while building the MethodTable.
inline MethodTable* MethodDesc::GetMethodTable_Unchecked()
{
    WRAPPER_CONTRACT;

    MethodDescChunk *pChunk = GetMethodDescChunk();
    PREFIX_ASSUME(pChunk != NULL);
    MethodTable *pMT = pChunk->GetMethodTable();
    return pMT;
}

inline MethodTable* MethodDesc::GetMethodTable_NoLogging()
{
    WRAPPER_CONTRACT;
    MethodTable *pMT = GetMethodTable_Unchecked();
    PREFIX_ASSUME(pMT != NULL);
    return pMT;
}

inline MethodTable* MethodDesc::GetMethodTable()
{
    WRAPPER_CONTRACT;
    g_IBCLogger.LogMethodDescAccess(this);
    return GetMethodTable_NoLogging();
}

inline MethodTable* MethodDesc::GetCanonicalMethodTable()
{
    WRAPPER_CONTRACT;

    _ASSERTE(GetMethodTable()->GetClass()->GetMethodTable() == GetClass()->GetMethodTable());
    return GetMethodTable()->GetCanonicalMethodTable();
}

inline mdMethodDef MethodDesc::GetMemberDef_NoLogging()
{
    WRAPPER_CONTRACT;

    MethodDescChunk *pChunk = GetMethodDescChunk();
    PREFIX_ASSUME(pChunk != NULL);
    BYTE   tokrange = pChunk->GetTokRange();

    UINT16 tokremainder = m_wTokenRemainder;

    return MergeToken(tokrange, tokremainder) | mdtMethodDef;
}

inline mdMethodDef MethodDesc::GetMemberDef()
{
    WRAPPER_CONTRACT;
    g_IBCLogger.LogMethodDescAccess(this);
    return GetMemberDef_NoLogging();
}

// Set the offset of this method desc in a chunk table (which allows us
// to work back to the method table/module pointer stored at the head of
// the table.
inline void MethodDesc::SetChunkIndex(MethodDescChunk * pChunk, DWORD index)
{
    WRAPPER_CONTRACT;

    _ASSERTE(pChunk->GetMethodDescAt(index) == this);
    _ASSERTE((pChunk->GetKind() & mdcClassification) == GetClassification());

    // Calculate the offset (mod 8) from the chunk table header.
    SIZE_T offset = (BYTE*)this - (BYTE*)pChunk->GetFirstMethodDesc();
    _ASSERTE((offset & ALIGNMENT_MASK) == 0);
    offset >>= ALIGNMENT_SHIFT;

    // Make sure that we did not overflow the BYTE
    _ASSERTE(offset == (BYTE)offset);

    *((BYTE*)this + MDEnums::MD_IndexOffset) = (BYTE)offset;

    // Make sure that the MethodDescChunk is setup correctly
    _ASSERTE(GetMethodDescChunk() == pChunk);
}

inline void MethodDesc::SetMemberDef(mdMethodDef mb)
{
    WRAPPER_CONTRACT;

    // Note: In order for this assert to work, SetChunkIndex must be called
    // before SetMemberDef.
#ifdef _DEBUG 
    SMDDebugCheck(mb);
#endif

    BYTE tokrange;
    UINT16 tokremainder;
    SplitToken(mb, &tokrange, &tokremainder);

    m_wTokenRemainder = tokremainder;

    if (mb != 0)
    {
        _ASSERTE(GetMethodDescChunk()->GetTokRange() == tokrange);
    }
}

#ifdef _DEBUG 

inline BOOL MethodDesc::SanityCheck()
{
    WRAPPER_CONTRACT;

    // Do a simple sanity test
    if (IsRestored())
    {
        // If it looks good, do a more intensive sanity test. We don't care about the result,
        // we just want it to not AV.
        return GetMethodTable() == m_pDebugMethodTable && this->GetModule() != NULL;
    }
    
    return TRUE;
}

inline void MethodDesc::SMDDebugCheck(mdMethodDef mb)
{
    WRAPPER_CONTRACT;

    if (TypeFromToken(mb) != 0)
    {
        _ASSERTE( GetTokenRange(mb) == GetMethodDescChunk()->GetTokRange() );
    }
}
#endif // _DEBUG

inline BOOL MethodDesc::IsEnCAddedMethod()
{
    LEAF_CONTRACT;

    return (GetClassification() == mcInstantiated) && AsInstantiatedMethodDesc()->IMD_IsEnCAddedMethod();
}

inline BOOL MethodDesc::HasNonVtableSlot()
{
    WRAPPER_CONTRACT;
    return (GetClassification() == mcInstantiated && AsInstantiatedMethodDesc()->IMD_HasNonVtableSlot())
        || (GetClassification() == mcDynamic);
}

inline TypeHandle *MethodDesc::GetMethodInstantiation()
{
    WRAPPER_CONTRACT;

    // This PRECONDITION fires when logging is enabled
    // The callstack originates from from TypeString::AppendMethodDebug
    // whenever you ngen an assembly with generics.
    //
    //    PRECONDITION(!(IsGenericMethodDefinition() && !IsTypicalMethodDefinition()));

    return
        (GetClassification() == mcInstantiated)
        ? AsInstantiatedMethodDesc()->IMD_GetMethodInstantiation()
        : NULL;
}

inline TypeHandle* MethodDesc::GetClassInstantiation()
{
    WRAPPER_CONTRACT;

    return GetMethodTable()->GetInstantiation();
}

inline BOOL MethodDesc::IsGenericMethodDefinition()
{
    WRAPPER_CONTRACT;

    return GetClassification() == mcInstantiated && AsInstantiatedMethodDesc()->IMD_IsGenericMethodDefinition();
}

inline PTR_TADDR MethodDesc::GetAddrOfSlot()
{
    LEAF_CONTRACT;

    _ASSERTE(!IsGenericMethodDefinition());
    _ASSERTE(!IsEnCAddedMethod()); // Assert from MethodDesc::GetSlot()

    return GetAddrOfSlotUnchecked();
}

inline PTR_TADDR MethodDesc::GetAddrOfSlotUnchecked()
{
    LEAF_CONTRACT;

    if (GetClassification() == mcInstantiated)
    {
        InstantiatedMethodDesc *pIMD = AsInstantiatedMethodDesc();
        if (pIMD->IMD_HasNonVtableSlot())
            return PTR_TADDR(PTR_HOST_MEMBER_TADDR(InstantiatedMethodDesc, pIMD, m_slot));
    }
    else
    if (GetClassification() == mcDynamic)
    {
        DynamicMethodDesc* pDMD = GetDynamicMethodDesc();
        return PTR_TADDR(PTR_HOST_MEMBER_TADDR(DynamicMethodDesc, pDMD, m_slot));
    }

    return PTR_TADDR(PTR_HOST_TO_TADDR(GetCanonicalMethodTable()->GetAddrOfRestoredSlotForMethod(this)));
}

// True if the method descriptor is an instantiation of a generic method.
inline BOOL MethodDesc::HasMethodInstantiation()
{
    WRAPPER_CONTRACT;
    return mcInstantiated == GetClassification() && AsInstantiatedMethodDesc()->IMD_HasMethodInstantiation();
}

// static
inline MethodDesc* MethodDesc::GetMethodDescFromStubAddr(TADDR addr, BOOL fSpeculative /*=FALSE*/)
{
    WRAPPER_CONTRACT;


#ifdef HAS_COMPACT_ENTRYPOINTS
    if (MethodDescChunk::IsCompactEntryPointAtAddress(addr))
    {
        return MethodDescChunk::GetMethodDescFromCompactEntryPoint(addr, fSpeculative);
    }
    else
#endif // HAS_COMPACT_ENTRYPOINTS
    {
        Precode* pPrecode = Precode::GetPrecodeFromEntryPoint(addr, fSpeculative);
        PREFIX_ASSUME(fSpeculative || (pPrecode != NULL));
        return (pPrecode != NULL) ? pPrecode->GetMethodDesc() : NULL;
    }

}




#ifdef _DEBUG 
BOOL MethodDescMatchesBinderSig(MethodDesc* pMD, BinderMethodID id);
#endif

#ifdef CALLDESCR_REGTYPEMAP 

void FillInRegTypeMap(ArgIterator* argit, BYTE* pMap, int* pRegArgNum, BYTE* typ, UINT32 structSize);

#endif // CALLDESCR_REGTYPEMAP

inline bool UpdateFPReturnSizeForHFAReturn(MetaSig* pSig, UINT* pfpReturnSize)
{
    return false;
}

#include "method.inl"

#endif // !_METHOD_H
