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
// siginfo.hpp
//
#ifndef _H_SIGINFO
#define _H_SIGINFO


#include "util.hpp"
#include "vars.hpp"
#include "clsload.hpp"

#include "gcscan.h"
#include "threads.h"

#include "eecontract.h"
#include "typectxt.h"
#include "sigparser.h"

#define META_SIG_CACHE_SIZE 47

//---------------------------------------------------------------------------------------
// These macros define how arguments are mapped to the stack in the managed calling convention.
// We assume to be walking a method's signature left-to-right, in the virtual calling convention.
// See MethodDesc::Call for details on this virtual calling convention.
// These macros tell us whether the arguments we see as we proceed with the signature walk are mapped
//   to increasing or decreasing stack addresses. This is valid only for arguments that go on the stack.
//---------------------------------------------------------------------------------------
#if defined(_X86_)
#define STACK_GROWS_DOWN_ON_ARGS_WALK
#else
#define STACK_GROWS_UP_ON_ARGS_WALK
#endif

BOOL IsTypeRefOrDef(LPCSTR szClassName, Module *pModule, mdToken token);

struct ElementTypeInfo {
#ifdef _DEBUG
    int            m_elementType;     
#endif
    int            m_cbSize;
    CorInfoGCType  m_gc         : 3;
    int            m_fp         : 1;
    int            m_enregister : 1;
    int            m_isBaseType : 1;

};
extern const ElementTypeInfo gElementTypeInfo[];

unsigned GetSizeForCorElementType(CorElementType etyp);
const ElementTypeInfo* GetElementTypeInfo(CorElementType etyp);

//------------------------------------------------------------------------
// Encapsulates how compressed integers and typeref tokens are encoded into
// a bytestream.
//
// As you use this class please understand the implicit normalizations 
// on the CorElementType's returned by the various methods, especially
// for variable types (e.g. !0 in generic signatures), string types
// (i.e. E_T_STRING), object types (E_T_OBJECT), constructed types 
// (e.g. List<int>) and enums.
//------------------------------------------------------------------------
class SigPointer : public SigParser
{
    friend class MetaSig;

    public:
        //------------------------------------------------------------------------
        // Constructor.
        //------------------------------------------------------------------------
        SigPointer() { LEAF_CONTRACT; }

        //------------------------------------------------------------------------
        // Initialize 
        //------------------------------------------------------------------------
        FORCEINLINE SigPointer(PCCOR_SIGNATURE ptr)
        {
            LEAF_CONTRACT;

            m_ptr = ptr;
            // We don't know the size of the signature, so we'll say it's "big enough"
            m_dwLen = 0xffffffff;
        }

        FORCEINLINE SigPointer(PCCOR_SIGNATURE ptr, DWORD len)
        {
            LEAF_CONTRACT;

            m_ptr = ptr;
            m_dwLen = len;
        }


    //=========================================================================
    // The RAW interface for reading signatures.  You see exactly the signature,
    // apart from custom modifiers which for historical reasons tend to get eaten.
    //
    // DO NOT USE THESE METHODS UNLESS YOU'RE TOTALLY SURE YOU WANT
    // THE RAW signature.  You nearly always want GetElemTypeClosed() or 
    // PeekElemTypeClosed() or one of the MetaSig functions.  See the notes above.
    // These functions will return E_T_INTERNAL, E_T_VAR, E_T_MVAR and such
    // so the caller must be able to deal with those
    //=========================================================================


        size_t ConvertToInternalExactlyOne(Module* pSigModule, BYTE* pbBuffer);
        size_t ConvertToInternalSignature(Module* pSigModule, BYTE* pbBuffer);


    //=========================================================================
    // The CLOSED interface for reading signatures.  With the following
    // methods you see the signature "as if" all type variables are 
    // replaced by the given instantiations.  However, no type loads happen.
    //
    // In general this is what you want to use if the signature may include
    // generic type variables.  Even if you know it doesn't you can always
    // pass in NULL for the instantiations and put a comment to that effect.
    //
    // The CLOSED api also hides E_T_INTERNAL by return E_T_CLASS or E_T_VALUETYPE
    // appropriately (as directed by the TypeHandle following E_T_INTERNAL)
    //=========================================================================

        // The CorElementTypes returned correspond
        // to those returned by TypeHandle::GetSignatureCorElementType.
        CorElementType PeekElemTypeClosed(const SigTypeContext *pTypeContext) const;

        //------------------------------------------------------------------------
        // Fetch the token for a CLASS, VALUETYPE or GENRICINST, or a type
        // variable instantiatied to be one of these, taking into account
        // the given instantiations.
        //
        // SigPointer should be in a position that satisfies
        //  ptr.PeekElemTypeClosed(pTypeContext) = ELEMENT_TYPE_VALUETYPE
        //
        // A type ref or def is returned.  For an instantiated generic struct
        // this will return the token for the generic class, e.g. for a signature
        // for "struct Pair<int,int>" this will return a token for "Pair".
        //
        // The token will only make sense in the context of the module where 
        // the signature occurs.
        //
        // WARNING: This api will return a mdTokenNil for a E_T_VALUETYPE obtained
        //          from a E_T_INTERNAL, as the token is meaningless in that case
        //          Users of this api must be prepared to deal with a null token
        //------------------------------------------------------------------------
        mdTypeRef PeekValueTypeTokenClosed(const SigTypeContext *pTypeContext) const;


    //=========================================================================
    // The INTERNAL-NORMALIZED interface for reading signatures.  You see 
    // information concerning the signature, but taking into account normalizations
    // performed for layout of data, e.g. enums and one-field VCs.
    //=========================================================================

        // The CorElementTypes returned correspond
        // to those returned by TypeHandle::GetInternalCorElementType.
        CorElementType PeekElemTypeNormalized(Module* pModule, const SigTypeContext *pTypeContext) const;


        //------------------------------------------------------------------------
        // Assumes that the SigPointer points to the start of an element type.
        // Returns size of that element in bytes. This is the minimum size that a
        // field of this type would occupy inside an object. 
        //------------------------------------------------------------------------
        UINT SizeOf(Module* pModule, const SigTypeContext *pTypeContext) const;
    
private:

        // SigPointer should be just after E_T_VAR or E_T_MVAR
        TypeHandle GetTypeVariable(CorElementType et,const SigTypeContext *pTypeContext);
        TypeHandle GetTypeVariableThrowing(Module *pModule, 
                                           CorElementType et,
                                           ClassLoader::LoadTypesFlag fLoadTypes,
                                           const SigTypeContext *pTypeContext);


public:
        //------------------------------------------------------------------------
        // Assuming that the SigPointer points the start if an element type.
        // Use SigTypeContext to fill in any  type parameters
        //
        // Also advance the pointer to after the element type.
        //------------------------------------------------------------------------

        // OBSOLETE - Use GetTypeHandleThrowing()
        TypeHandle GetTypeHandle(Module* pModule,
                                 const SigTypeContext *pTypeContext, // how to instantiate any generic type parameters we come across
                                 OBJECTREF *pThrowable,
                                 ClassLoader::LoadTypesFlag fLoadTypes = ClassLoader::LoadTypes,
                                 ClassLoadLevel level = CLASS_LOADED,    // level to which type should be loaded
                                 BOOL dropGenericArgumentLevel = FALSE,
                                 const Substitution *pSubst = NULL,
                                 Module *pCurrentModule = NULL) const;

        // pTypeContext indicates how to instantiate any generic type parameters we come 
        // However, first we implicitly apply the substitution pSubst to the metadata if pSubst is supplied.
        // That is, if the metadata contains a type variable "!0" then we first look up
        // !0 in pSubst to produce another item of metdata and continue processing.  
        // If pSubst is empty then we look up !0 in the pTypeContext to produce a final
        // type handle.  If any of these are out of range we throw an exception.
        //
        // The level is the level to which the result type will be loaded (see classloadlevel.h)
        // If dropGenericArgumentLevel is TRUE, and the metadata represents an instantiated generic type,
        // then generic arguments to the generic type will be loaded one level lower. (This is used by the
        // class loader to avoid looping on definitions such as class C : D<C>)
        //   
        // If dropGenericArgumentLevel is TRUE and
        // level=CLASS_LOAD_APPROXPARENTS, then the instantiated
        // generic type is "approximated" in the following way: 
        // - for generic interfaces, the generic type (uninstantiated) is returned 
        // - for other generic instantiations, System.Object is used in place of any reference types
        //   occurring in the type arguments 
        // This semantics is used by the class loader to load tricky recursive definitions in phases
        // (e.g. class C : D<C>, or struct S : I<S>)
        TypeHandle GetTypeHandleThrowing(Module* pModule,
                                         const SigTypeContext *pTypeContext, 
                                         ClassLoader::LoadTypesFlag fLoadTypes = ClassLoader::LoadTypes,
                                         ClassLoadLevel level = CLASS_LOADED,
                                         BOOL dropGenericArgumentLevel = FALSE,
                                         const Substitution *pSubst = NULL,
                                         Module *pCurrentModule = NULL) const;

public:
        //------------------------------------------------------------------------
        // Does this type contain class or method type parameters whose instantiation cannot
        // be determined at JIT-compile time from the instantiations in the method context? 
        // Return a combination of hasClassVar and hasMethodVar flags.
        //
        // Example: class C<A,B> containing instance method m<T,U>
        // Suppose that the method context is C<float,string>::m<double,object>
        // Then the type Dict<!0,!!0> is considered to have *no* "polymorphic" type parameters because 
        // !0 is known to be float and !!0 is known to be double
        // But Dict<!1,!!1> has polymorphic class *and* method type parameters because both
        // !1=string and !!1=object are reference types and so code using these can be shared with
        // other reference instantiations.
        //------------------------------------------------------------------------
        VarKind IsPolyType(const SigTypeContext *pTypeContext) const;

        //------------------------------------------------------------------------
        // Tests if the element type is a System.String. Accepts
        // either ELEMENT_TYPE_STRING or ELEMENT_TYPE_CLASS encoding.
        //------------------------------------------------------------------------
        BOOL IsStringType(Module* pModule, const SigTypeContext *pTypeContext) const;
        BOOL IsStringTypeThrowing(Module* pModule, const SigTypeContext *pTypeContext) const;

private:
        BOOL IsStringTypeHelper(Module* pModule, const SigTypeContext* pTypeContext, BOOL fThrow) const;

public:


        //------------------------------------------------------------------------
        // Tests if the element class name is szClassName. 
        //------------------------------------------------------------------------
        BOOL IsClass(Module* pModule, LPCUTF8 szClassName, const SigTypeContext *pTypeContext = NULL) const;
        BOOL IsClassThrowing(Module* pModule, LPCUTF8 szClassName, const SigTypeContext *pTypeContext = NULL) const;

private:
        BOOL IsClassHelper(Module* pModule, LPCUTF8 szClassName, const SigTypeContext* pTypeContext, BOOL fThrow) const;

public:
        //------------------------------------------------------------------------
        // Tests for the existence of a custom modifier
        //------------------------------------------------------------------------
        BOOL HasCustomModifier(Module *pModule, LPCSTR szModName, CorElementType cmodtype) const;

};


#ifdef _DEBUG
#define MAX_CACHED_SIG_SIZE     3       // To excercize non-cached code path
#else
#define MAX_CACHED_SIG_SIZE     15
#endif


//------------------------------------------------------------------------
// A substitution represents the composition of several formal type instantiations
// It is used when matching formal signatures across the inheritance hierarchy.
//
// It has the form of a linked list:
//   [mod_1, <inst_1>] ->
//   [mod_2, <inst_2>] ->
//   ...
//   [mod_n, <inst_n>]
//
// Here the types in <inst_1> must be resolved in the scope of module mod_1 but
// may contain type variables instantiated by <inst_2>
// ...
// and the types in <inst_(n-1)> must be resolved in the scope of mould mod_(n-1) but
// may contain type variables instantiated by <inst_n>
//
// Any type variables in <inst_n> are treated as "free".
//------------------------------------------------------------------------
class Substitution
{
private:
    Module* m_pModule;        // Module in which instantiation lives (needed to resolve typerefs)
    PCCOR_SIGNATURE m_inst;  // pointer into metadata for instantiation (immediately following number of params)
    const Substitution* m_pNext;

public:
    Substitution()
    { 
        LEAF_CONTRACT;
        m_pModule = NULL; 
        m_inst = NULL;
        m_pNext = NULL;
    }
    Substitution(Module* pModuleArg, PCCOR_SIGNATURE instArg, const Substitution *nextArg)
    { 
        LEAF_CONTRACT;
        m_pModule = pModuleArg; 
        m_inst = instArg;
        m_pNext = nextArg;
    }
    Substitution(mdToken parentTypeDefOrRefOrSpec, Module* pModuleArg, const Substitution *nextArg);
    void DeleteChain();

    Module* GetModule() const { LEAF_CONTRACT; return m_pModule; }
    const Substitution* GetNext() const { LEAF_CONTRACT; return m_pNext; }
    PCCOR_SIGNATURE GetInstSig() const { LEAF_CONTRACT; return m_inst; }
    DWORD GetLength() const;
    
    void CopyToArray(Substitution *pTarget /* must have type Substitution[GetLength()] */ ) const;

};

struct SignatureNative;

class MetaSig
{
    friend class ArgIterator;
    public:
        enum MetaSigKind { 
            sigMember, 
            sigLocalVars,
            sigField,
            };

        //------------------------------------------------------------------
        // Constructor. Warning: Does NOT make a copy of szMetaSig.
        //
        // The instantiations are used to fill in type variables on calls
        // to PeekArg, GetReturnType, GetNextArg, GetTypeHandle, GetRetTypeHandle and
        // so on.  
        //
        // Please make sure you know what you're doing by leaving classInst and methodInst to default NULL
        // Are you sure the signature cannot contain type parameters (E_T_VAR, E_T_MVAR)?
        //------------------------------------------------------------------
        MetaSig(PCCOR_SIGNATURE szMetaSig, 
                DWORD cbMetaSig,
                Module* pModule, 
                const SigTypeContext *pTypeContext, 
                BOOL fConvertSigAsVarArg = FALSE, 
                MetaSigKind kind = sigMember, 
                BOOL fParamTypeArg = FALSE);

        // The following create MetaSigs for parsing the signature of the given method.  
        // They are identical except that they give slightly different 
        // type contexts.  (Note the type context will only be relevant if we 
        // are parsing a method on an array type or on a generic type.)
        // See TypeCtxt.h for more details.
        // If declaringType is omitted then a *representative* instantiation may be obtained from pMD or pFD
        MetaSig(MethodDesc *pMD, TypeHandle declaringType = TypeHandle());
        MetaSig(MethodDesc *pMD, TypeHandle *classInst, TypeHandle *methodInst);

        MetaSig(FieldDesc *pFD, TypeHandle declaringType = TypeHandle());

        // Used to avoid touching metadata for mscorlib methods.  Nb. only use for non-generic methods.
        MetaSig(MethodDesc* pMD, BinderMethodID id);

        //------------------------------------------------------------------
        // Constructor. Copy state from existing MetaSig (does not deep copy
        // zsMetaSig). Iterator fields are reset.
        //------------------------------------------------------------------
        MetaSig(MetaSig *pSig) { LEAF_CONTRACT; memcpy(this, pSig, sizeof(MetaSig)); Reset(); }

        //------------------------------------------------------------------
        // Constructor. Copy state from existing SignatureNative (used by
        // reflection)
        //------------------------------------------------------------------
        MetaSig(MethodDesc* pMD, SignatureNative* pSigNative, BOOL bCanCacheTargetAndCrackedSig);

        //------------------------------------------------------------------
        // Returns type of current argument, then advances the argument
        // index. Returns ELEMENT_TYPE_END if already past end of arguments.
        //------------------------------------------------------------------
        CorElementType NextArg();

        //------------------------------------------------------------------
        // Retreats argument index, then returns type of the argument
        // under the new index. Returns ELEMENT_TYPE_END if already at first
        // argument.
        //------------------------------------------------------------------
        CorElementType PrevArg();

        //------------------------------------------------------------------
        // Returns type of current argument index. Returns ELEMENT_TYPE_END if already past end of arguments.
        //------------------------------------------------------------------
        CorElementType PeekArg() const;

    //CorElementType PeekArgNormalized() const;

        //------------------------------------------------------------------
        // Returns a read-only SigPointer for the last type to be returned
        // via NextArg() or PrevArg(). This allows extracting more information
        // for complex types.
        //------------------------------------------------------------------
        const SigPointer & GetArgProps() const
        {
            LEAF_CONTRACT;
            return m_pLastType;
        }

        //------------------------------------------------------------------
        // Returns a read-only SigPointer for the return type.
        // This allows extracting more information for complex types.
        //------------------------------------------------------------------
        const SigPointer & GetReturnProps() const
        {
            LEAF_CONTRACT;
            return m_pRetType;
        }


        //------------------------------------------------------------------------
        // Returns # of arguments. Does not count the return value.
        // Does not count the "this" argument (which is not reflected om the
        // sig.) 64-bit arguments are counted as one argument.
        //------------------------------------------------------------------------
        UINT NumFixedArgs()
        {
            LEAF_CONTRACT;
            return m_nArgs;
        }
        
        //----------------------------------------------------------
        // Returns the calling convention (see IMAGE_CEE_CS_CALLCONV_*
        // defines in cor.h)
        //----------------------------------------------------------
        static BYTE GetCallingConvention(Module* pModule, PCCOR_SIGNATURE pSig)
        {
            WRAPPER_CONTRACT;

            return (BYTE)(IMAGE_CEE_CS_CALLCONV_MASK & (CorSigUncompressCallingConv(/*modifies*/pSig)));
        }

        //----------------------------------------------------------
        // Returns the calling convention (see IMAGE_CEE_CS_CALLCONV_*
        // defines in cor.h)
        //----------------------------------------------------------
        static BYTE GetCallingConventionInfo(Module* pModule, PCCOR_SIGNATURE pSig)
        {
            WRAPPER_CONTRACT;
            return (BYTE)CorSigUncompressCallingConv(/*modifies*/pSig);
        }

        //----------------------------------------------------------
        // Returns the calling convention (see IMAGE_CEE_CS_CALLCONV_*
        // defines in cor.h)
        //----------------------------------------------------------
        BYTE GetCallingConvention()
        {
            LEAF_CONTRACT;
            return m_CallConv & IMAGE_CEE_CS_CALLCONV_MASK; 
        }

        //----------------------------------------------------------
        // Returns the calling convention & flags (see IMAGE_CEE_CS_CALLCONV_*
        // defines in cor.h)
        //----------------------------------------------------------
        BYTE GetCallingConventionInfo()
        {
            LEAF_CONTRACT;

            return m_CallConv;
        }
        
        // Used by DelegateTransitionFrame to force static calling convention in conjunction with a ShuffleThunk.
        VOID DangerousRemoveHasThis()
        {
            LEAF_CONTRACT;
            m_CallConv &= ~IMAGE_CEE_CS_CALLCONV_HASTHIS;
        }

        //----------------------------------------------------------
        // Has a 'this' pointer?
        //----------------------------------------------------------
        BOOL HasThis()
        {
            LEAF_CONTRACT;

            return m_CallConv & IMAGE_CEE_CS_CALLCONV_HASTHIS;
        }  

        //----------------------------------------------------------
        // Is a generic method with explicit arity?
        //----------------------------------------------------------
        BOOL IsGenericMethod()
        {
            LEAF_CONTRACT;
            return m_CallConv & IMAGE_CEE_CS_CALLCONV_GENERIC;
        }  

        //----------------------------------------------------------
        // Is vararg?
        //----------------------------------------------------------
        BOOL IsVarArg()
        {
            WRAPPER_CONTRACT;
            return GetCallingConvention() == IMAGE_CEE_CS_CALLCONV_VARARG;
        }

        //----------------------------------------------------------
        // Is vararg?
        //----------------------------------------------------------
        static BOOL IsVarArg(Module* pModule, PCCOR_SIGNATURE pSig)
        {
            WRAPPER_CONTRACT;
            return GetCallingConvention(pModule, pSig) == IMAGE_CEE_CS_CALLCONV_VARARG;
        }



        Module* GetModule() const {
            LEAF_CONTRACT;

            return m_pModule;
        }
            
        //----------------------------------------------------------
        // Returns the unmanaged calling convention.
        //----------------------------------------------------------
        static BOOL GetUnmanagedCallingConvention(Module *pModule, PCCOR_SIGNATURE pSig, ULONG cSig, CorPinvokeMap *pPinvokeMapOut);

        //------------------------------------------------------------------
        // Like NextArg, but return only normalized type (enums flattned to 
        // underlying type ...
        //------------------------------------------------------------------
        CorElementType NextArgNormalized() {
            CONTRACTL
            {
                if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
                if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
                if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
                MODE_ANY;
            }
            CONTRACTL_END

            m_pLastType = m_pWalk;
            if (m_iCurArg == m_nArgs)
                return ELEMENT_TYPE_END;
            else
            {
                m_iCurArg++;
                CorElementType mt = m_pWalk.PeekElemTypeNormalized(m_pModule, &m_typeContext);
                // We should not hit ELEMENT_TYPE_END in the middle of the signature
                if (mt == ELEMENT_TYPE_END)
                    THROW_BAD_FORMAT(BFA_BAD_SIGNATURE, (Module*)NULL);
                m_pWalk.SkipExactlyOne();
                return mt;
            }
        }

        CorElementType NextArgNormalized(UINT32 *size) {
            CONTRACTL
            {
                if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
                if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
                if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
                MODE_ANY;
            }
            CONTRACTL_END

            m_pLastType = m_pWalk;
            if (m_iCurArg == m_nArgs)
                return ELEMENT_TYPE_END;
            else
            {
                m_iCurArg++;
                CorElementType mt = m_pWalk.PeekElemTypeNormalized(m_pModule, &m_typeContext);
                // We should not hit ELEMENT_TYPE_END in the middle of the signature
                if (mt == ELEMENT_TYPE_END)
                    THROW_BAD_FORMAT(BFA_BAD_SIGNATURE, (Module*)NULL);
                *size = m_pWalk.SizeOf(m_pModule, &m_typeContext);
                m_pWalk.SkipExactlyOne();
                return mt;
            }
        }

        // Is there a hidden parameter for the return parameter.  

        BOOL HasRetBuffArg()
        {
            WRAPPER_CONTRACT;

#ifdef ENREGISTERED_RETURNTYPE_MAXSIZE
            UINT size = GetReturnTypeSize();


            return (size > ENREGISTERED_RETURNTYPE_INTEGER_MAXSIZE

                   );
#else
            CorElementType type = GetReturnTypeNormalized();
            return (type == ELEMENT_TYPE_VALUETYPE || type == ELEMENT_TYPE_TYPEDBYREF);
#endif
        }


        // This API returns true if we are returning a structure in registers when we and it is bigger than 64bits
        // This currently only happens on IA64.
        BOOL HasNonStandardByvalReturn()
        {
            WRAPPER_CONTRACT;

#ifdef ENREGISTERED_RETURNTYPE_MAXSIZE
            CorElementType type = GetReturnTypeNormalized();
            return ((type == ELEMENT_TYPE_VALUETYPE || type == ELEMENT_TYPE_TYPEDBYREF)) && !HasRetBuffArg();
#else
            return FALSE;
#endif
        }

#if defined(ENREGISTERED_PARAMTYPE_MAXSIZE)
        // 
        static BOOL IsArgPassedByRef(UINT size)
        {
            // If the size is bigger than ENREGISTERED_PARAM_TYPE_MAXSIZE, or if the size is NOT a power of 2, then
            // the argument is passed by reference.
            return ( 
                            (size > ENREGISTERED_PARAMTYPE_MAXSIZE)
                       
        }
#endif // ENREGISTERED_PARAMTYPE_MAXSIZE

        // Tests if the return type is an object ref.  Loads types
        // if needed (though it shouldn't really need to)
        BOOL IsObjectRefReturnType();

        UINT GetReturnTypeSize() 
        {    
            WRAPPER_CONTRACT;

            if (m_cbRetType == (UINT32)-1)
            {
                CONSISTENCY_CHECK(!SigWalkIsPrecondition());
                m_cbRetType = (UINT32)m_pRetType.SizeOf(m_pModule, &m_typeContext);
            }
            
            return m_cbRetType;
        }

        int GetLastTypeSize() 
        {
            WRAPPER_CONTRACT;
            return m_pLastType.SizeOf(m_pModule, &m_typeContext);
        }

        static UINT GetElemSize(TypeHandle th)
        {    
            WRAPPER_CONTRACT;  //NOTHROW,GC_NOTRIGGER,FAULT_FORBID - Don't want the overhead of a contract here

            CorElementType type = th.GetInternalCorElementType();
            _ASSERTE(type >= 0 && type < ELEMENT_TYPE_MAX);
            UINT ret = gElementTypeInfo[type].m_cbSize;
            if (ret != (UINT) -1)
                return ret;

            return th.GetMethodTable()->GetNumInstanceFieldBytes();
        }

        //------------------------------------------------------------------
        // Perform type-specific GC promotion on the value (based upon the
        // last type retrieved by NextArg()).
        //------------------------------------------------------------------
        VOID GcScanRoots(LPVOID pValue, promote_func *fn,
                         ScanContext* sc, promote_carefully_func *fnc = NULL);

        //------------------------------------------------------------------
        // Is the return type 64 bit?
        //------------------------------------------------------------------
        BOOL Is64BitReturn() const
        {
            WRAPPER_CONTRACT;
            CorElementType rt = GetReturnTypeNormalized();
            return (rt == ELEMENT_TYPE_I8 || rt == ELEMENT_TYPE_U8 || rt == ELEMENT_TYPE_R8);
        }

        //------------------------------------------------------------------
        // reset: goto start pos
        //------------------------------------------------------------------
        VOID Reset();

        //------------------------------------------------------------------
        // Moves index to end of argument list.
        //------------------------------------------------------------------
        VOID GotoEnd();

        //------------------------------------------------------------------
        // Returns CorElementType of return value, taking into account
        // any instantiations due to generics.  Does not load types.
        // Does not return normalized type.
        //------------------------------------------------------------------
        CorElementType GetReturnType() const;

        BOOL IsReturnTypeVoid() const;


        enum RETURNTYPE {RETOBJ, RETBYREF, RETNONOBJ};
        RETURNTYPE GetReturnObjectKind() const;

        CorElementType GetReturnTypeNormalized() const;

        //------------------------------------------------------------------
        // used to treat some sigs as special case vararg
        // used by calli to unmanaged target
        //------------------------------------------------------------------
        BOOL IsTreatAsVarArg()
        {
            LEAF_CONTRACT;
            
            return (m_flags & TREAT_AS_VARARG);
        }

        //------------------------------------------------------------------
        // Determines if the current argument is System/String.
        // Caller must determine first that the argument type is 
        // ELEMENT_TYPE_CLASS or ELEMENT_TYPE_STRING.  This may be used during
        // GC.
        //------------------------------------------------------------------
        BOOL IsStringType() const;

        //------------------------------------------------------------------
        // Determines if the current argument is a particular class.
        // Caller must determine first that the argument type 
        // is ELEMENT_TYPE_CLASS.
        //------------------------------------------------------------------
        BOOL IsClass(LPCUTF8 szClassName) const;


        //------------------------------------------------------------------
        // This method will return a TypeHandle for the last argument
        // examined.
        // If NextArg() returns ELEMENT_TYPE_BYREF, you can also call GetByRefType()
        // to get to the underlying type of the byref
        //------------------------------------------------------------------
        TypeHandle GetLastTypeHandle(OBJECTREF *pThrowable,
                                     ClassLoader::LoadTypesFlag fLoadTypes = ClassLoader::LoadTypes,
                                     ClassLoadLevel level = CLASS_LOADED) const
        {
             WRAPPER_CONTRACT;
             return m_pLastType.GetTypeHandle(m_pModule, &m_typeContext, pThrowable, fLoadTypes, level);
        }

        //------------------------------------------------------------------
        // This method will return a TypeHandle for the last argument
        // examined.
        // If NextArg() returns ELEMENT_TYPE_BYREF, you can also call GetByRefType()
        // to get to the underlying type of the byref
        //------------------------------------------------------------------
        TypeHandle GetLastTypeHandleThrowing(ClassLoader::LoadTypesFlag fLoadTypes = ClassLoader::LoadTypes,
                                             ClassLoadLevel level = CLASS_LOADED,
                                             BOOL dropGenericArgumentLevel = FALSE) const
        {
             WRAPPER_CONTRACT;
             return m_pLastType.GetTypeHandleThrowing(m_pModule, &m_typeContext, fLoadTypes, 
                                                      level, dropGenericArgumentLevel);
        }

        //------------------------------------------------------------------
        // Returns the TypeHandle for the return type of the signature
        //------------------------------------------------------------------
        TypeHandle GetRetTypeHandle(OBJECTREF *pThrowable,
                                    ClassLoader::LoadTypesFlag fLoadTypes = ClassLoader::LoadTypes) const
        {
             WRAPPER_CONTRACT;
             return m_pRetType.GetTypeHandle(m_pModule, &m_typeContext, pThrowable, fLoadTypes);
        }

        TypeHandle GetRetTypeHandleThrowing(ClassLoader::LoadTypesFlag fLoadTypes = ClassLoader::LoadTypes) const
        {
             WRAPPER_CONTRACT;
             return m_pRetType.GetTypeHandleThrowing(m_pModule, &m_typeContext, fLoadTypes);
        }

        //------------------------------------------------------------------
        // Returns the base type of the byref type of the last argument examined
        // which needs to have been ELEMENT_TYPE_BYREF.
        // For object references, the class being accessed byref is also returned in *pTy.
        // eg. for "int32 &",            return value = ELEMENT_TYPE_I4,    *pTy= ???
        //     for "System.Exception &", return value = ELEMENT_TYPE_CLASS, *pTy=System.Exception
        // Note that byref to byref is not allowed, and so the return value
        // can never be ELEMENT_TYPE_BYREF.
        //------------------------------------------------------------------
        CorElementType GetByRefType(TypeHandle* pTy) const;

        // Compare types in two signatures, first applying
        // - optional substitutions pSubst1 and pSubst2
        //   to class type parameters (E_T_VAR) in the respective signatures
        // - optional instantiations pMethodInst1 and pMethodInst2 
        //   to method type parameters (E_T_MVAR) in the respective signatures
        static BOOL CompareElementType(PCCOR_SIGNATURE &pSig1,         PCCOR_SIGNATURE &pSig2, 
                                       PCCOR_SIGNATURE pEndSig1,       PCCOR_SIGNATURE pEndSig2, 
                                       Module*         pModule1,       Module*         pModule2, 
                                       const Substitution   *pSubst1,  const Substitution   *pSubst2,
                                       SigPointer *pMethodInst1 = NULL, SigPointer *pMethodInst2 = NULL);


        // If pTypeDef1 is C<...> and pTypeDef2 is C<...> (for possibly different instantiations)
        // then check C<!0, ... !n> @ pSubst1 == C<!0, ..., !n> @ pSubst2, i.e.
        // that the head type (C) is the same and that when the head type is treated
        // as an uninstantiated type definition and we apply each of the substitutions 
        // then the same type results.  This effectively checks that the two substitutions
        // are equivalent.
        static BOOL CompareTypeDefsUnderSubstitutions(MethodTable *pTypeDef1,          MethodTable *pTypeDef2,
                                                      const Substitution*   pSubst1,   const Substitution*   pSubst2);


        // Compare two complete method signatures, first applying optional substitutions pSubst1 and pSubst2
        // to class type parameters (E_T_VAR) in the respective signatures
        static BOOL CompareMethodSigs(
            PCCOR_SIGNATURE pSig1, 
            DWORD       cSig1, 
            Module*     pModule1, 
            const Substitution* pSubst1,
            PCCOR_SIGNATURE pSig2, 
            DWORD       cSig2, 
            Module*     pModule2,
            const Substitution* pSubst2
        );

        // Nonthrowing version of CompareMethodSigs
        //
        //   Return S_OK if they match
        //          S_FALSE if they don't match
        //          FAILED  if OOM or some other blocking error
        //
        static HRESULT CompareMethodSigsNT(
            PCCOR_SIGNATURE pSig1, 
            DWORD       cSig1, 
            Module*     pModule1, 
            const Substitution* pSubst1,
            PCCOR_SIGNATURE pSig2, 
            DWORD       cSig2, 
            Module*     pModule2,
            const Substitution* pSubst2
        );

        static BOOL CompareFieldSigs(
            PCCOR_SIGNATURE pSig1, 
            DWORD       cSig1, 
            Module*     pModule1, 
            PCCOR_SIGNATURE pSig2, 
            DWORD       cSig2, 
            Module*     pModule2
        );

        static BOOL CompareMethodSigs(MetaSig &msig1,
                                      MetaSig &msig2,
                                      BOOL ignoreCallconv);

        // Is each set of constraints on the implementing method's type parameters a subset
        // of the corresponding set of constraints on the declared method's type parameters,
        // given a subsitution for the latter's (class) type parameters.
        // This is used by the class loader to verify type safety of method overriding and interface implementation.
        static BOOL CompareMethodConstraints(Module *pModule1, 
                                             mdMethodDef tok1, //implementing method
                                             const Substitution *pSubst2,
                                             Module *pModule2,
                                             mdMethodDef tok2); //declared method

private:
        static BOOL CompareVariableConstraints(Module *pModule1, mdGenericParam tok1, //overriding
                                               const Substitution *pSubst2,
                                               Module *pModule2, mdGenericParam tok2); //overridden

        static BOOL CompareTypeDefOrRefOrSpec(Module *pModule1, mdToken tok1,
                                              const Substitution *pSubst1,
                                              Module *pModule2, mdToken tok2,
                                              const Substitution *pSubst2);
        static BOOL CompareTypeSpecToToken(mdTypeSpec tk1,
                                           mdToken tk2,
                                           Module *pModule1,
                                           Module *pModule2,
                                           const Substitution *pSubst1,
                                           SigPointer *pMethodInst1);

        static BOOL CompareElementTypeToToken(PCCOR_SIGNATURE &pSig1,
                                             PCCOR_SIGNATURE pEndSig1, // end of sig1
                                             mdToken         tk2,
                                             Module*         pModule1,
                                             Module*         pModule2,
                                             const Substitution*   pSubst1,
                                             SigPointer *pMethodInst1); //not used by any caller so far...

        //------------------------------------------------------------------------
        // Returns # of stack bytes required to create a call-stack using
        // the internal calling convention.
        // Includes indication of "this" pointer since that's not reflected
        // in the sig.
        //------------------------------------------------------------------------
private:
        static UINT SizeOfVirtualFixedArgStack(Module* pModule, 
                                               PCCOR_SIGNATURE szMetaSig, 
                                               DWORD cbMetaSigSize,
                                               BOOL fIsStatic, 
                                               const SigTypeContext *pTypeContext);
public:
        static UINT SizeOfActualFixedArgStack(Module* pModule, 
                                              PCCOR_SIGNATURE szMetaSig, 
                                              DWORD cbMetaSigSize,
                                              BOOL fIsStatic,
                                              const SigTypeContext *pTypeContext, 
                                              BOOL fParamTypeArg = FALSE, 
                                              int *paramTypeReg = NULL);

        static UINT SizeOfFrameArgumentArray(Module* pModule, 
                                             PCCOR_SIGNATURE szMetaSig, 
                                             DWORD cbMetaSigSize,
                                             BOOL fIsStatic,
                                             const SigTypeContext *pTypeContext, 
                                             BOOL fParamTypeArg = FALSE, 
                                             int *paramTypeReg = NULL)
        {
            WRAPPER_CONTRACT;

            return SizeOfActualFixedArgStack(pModule, szMetaSig, cbMetaSigSize, fIsStatic, pTypeContext, fParamTypeArg, paramTypeReg);
        }

        //------------------------------------------------------------------
        // Ensures that all the value types in the sig are loaded. This
        // should be called on sig's that have value types before they
        // are passed to Call(). This ensures that value classes will not
        // be loaded during the operation to determine the size of the
        // stack. Thus preventing the resulting GC hole.
        //------------------------------------------------------------------
        static void EnsureSigValueTypesLoaded(MethodDesc *pMD)
        {
            CONTRACTL
            {
                THROWS;
                GC_TRIGGERS;
                INJECT_FAULT(COMPlusThrowOM());
                MODE_ANY;
            }
            CONTRACTL_END
            MetaSig(pMD).ForceSigWalk(FALSE);
        }

        // this walks the sig and checks to see if all  types in the sig can be loaded
        static void CheckSigTypesCanBeLoaded(PCCOR_SIGNATURE pSig, Module *pModule, const SigTypeContext *pTypeContext);

        // See the comments about thread-safety in ForceSigWalk to understand why
        // this predicate cannot be arbitrarily changed to some other member.
        BOOL NeedsSigWalk()
        {
            
            // The normal use of this predicate is in a construct like this:
            //
            //  if (NeedsSigWalk()) ForceSigWalk()
            //
            // Thus, we will duplicate ForceSigWalk()'s contract here to make
            // sure people do the right code coverage thing by default.

            //
            // in some cases, we correctly force a sig walk early and then assert
            // that we may not trigger a GC.  In those cases, the user must call
            // SigWalkIsPrecondition, which will cause this contract to fall out
            // into the less-restrictive version.
            //

            CONTRACT(BOOL)
            {
#ifdef _DEBUG                
                if (FORBIDGC_LOADER_USE_ENABLED() || SigWalkIsPrecondition()) NOTHROW; else THROWS;
                if (FORBIDGC_LOADER_USE_ENABLED() || SigWalkIsPrecondition()) GC_NOTRIGGER; else GC_TRIGGERS;
                if (FORBIDGC_LOADER_USE_ENABLED() || SigWalkIsPrecondition()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
#endif // _DEBUG
                MODE_ANY;
                POSTCONDITION(!SigWalkIsPrecondition() || !RETVAL); // if sigwalk is a precondition, we better return false
            }
            CONTRACT_END;

            RETURN (m_nVirtualStack == (UINT32) -1);
        }

        //------------------------------------------------------------------------
        // The following three routines are the same as the above routines except
        //  they are called on the MetaSig which will cache these values
        //------------------------------------------------------------------------

        UINT SizeOfVirtualFixedArgStack(BOOL fIsStatic)
        {
            WRAPPER_CONTRACT;

            if (NeedsSigWalk())
                ForceSigWalk(fIsStatic);
            _ASSERTE(!!fIsStatic == !!m_WalkStatic);    // booleanize

            return m_nVirtualStack;
        }


        UINT SizeOfActualFixedArgStack(BOOL fIsStatic)
        {
            WRAPPER_CONTRACT;

            if (NeedsSigWalk())
                ForceSigWalk(fIsStatic);
            _ASSERTE(!!fIsStatic == !!m_WalkStatic);    // booleanize

            return m_nActualStack;
        }

        UINT NumVirtualFixedArgs(BOOL fIsStatic)
        {
            WRAPPER_CONTRACT;

            if (NeedsSigWalk())
                ForceSigWalk(fIsStatic);
            _ASSERTE(!!fIsStatic == !!m_WalkStatic);    // booleanize

            return m_nNumVirtualFixedArgs;
        }

        // For use with ArgIterator.  FramedMethodFrame::GetRawNegSpaceSize()
        // returns the fixed amount of memory to allocate underneath the
        // FramedMethodFrame.  This function computes the amount of additional
        // memory required above the FramedMethodFrame.  The parameter offsets
        // returned by ArgIterator::GetNextOffset are relative to a
        // FramedMethodFrame, and may be in either of these regions.
        UINT SizeOfFrameArgumentArray(BOOL fIsStatic)
        {
            WRAPPER_CONTRACT;

            // No space left by caller; argument registers are stored
            // underneath the frame.
            return SizeOfActualFixedArgStack(fIsStatic);
        }

        //------------------------------------------------------------------------

        UINT CbStackPop(BOOL fIsStatic)
        {
            WRAPPER_CONTRACT;

#if defined(_X86_) || defined(_AMD64_)
            if (IsVarArg())
                return 0;
            else
                return SizeOfActualFixedArgStack(fIsStatic);

#else
            // no meaning on other platforms
            return 0;
#endif
        }
        
        UINT GetFPReturnSize()
        {
            WRAPPER_CONTRACT;

            if (HasFPReturn())
            {
                return GetReturnTypeSize();
            }
            else
            {
                return 0;
            }
        }
        
       
#ifdef _DEBUG    
        void SetSigWalkIsPrecondition()
        {
            LEAF_CONTRACT;
        
            m_flags |= SIGWALK_IS_PRECONDITION;
        }

        bool SigWalkIsPrecondition()
        {
            LEAF_CONTRACT;

            return !!(m_flags & SIGWALK_IS_PRECONDITION);
        }
#endif // _DEBUG
    

        void SetHasFPReturn()
        {
            LEAF_CONTRACT;

            m_flags |= HAS_FP_RETVAL;
        }

        bool HasFPReturn()
        {
            WRAPPER_CONTRACT;

            if (!(m_flags & HAS_FP_RETVAL_INITTED))
            {
                CONSISTENCY_CHECK(!SigWalkIsPrecondition());
                CalculateHasFPReturn();
            }
            
            return !!(m_flags & HAS_FP_RETVAL);
        }
        
        void ForceSigWalk(BOOL fIsStatic);

        void CalculateHasFPReturn();

        const SigTypeContext *GetSigTypeContext() const { LEAF_CONTRACT; return &m_typeContext; }


    // These are protected because Reflection subclasses Metasig
    protected:

        enum MetaSigFlags
        {
#ifdef _DEBUG
            SIGWALK_IS_PRECONDITION = 0x0001,
#endif // _DEBUG
            SIG_OFFSETS_INITTED     = 0x0002,
            SIG_RET_TYPE_INITTED    = 0x0004,
            TREAT_AS_VARARG         = 0x0008,    // used to treat some sigs as special case vararg
                                                 // used by calli to unmanaged target
            HAS_FP_RETVAL           = 0x0010,
            HAS_FP_RETVAL_INITTED   = 0x0020,    // have we initialized the HAS_FP_RETVAL bit?
        };

        static const UINT32 s_cSigHeaderOffset;

        Module*      m_pModule;
        SigPointer   m_pStart;
        SigPointer   m_pWalk;
        SigPointer   m_pLastType;
        SigPointer   m_pRetType;
        PCCOR_SIGNATURE m_pszMetaSig;
        UINT32       m_nArgs;
        UINT32       m_iCurArg;
        UINT32       m_cbSigSize;

        // The following are cached so we don't the signature
        //  multiple times
        UINT32       m_nVirtualStack;   // Size of the virtual stack
        UINT32       m_nActualStack;    // Size of the actual stack
        UINT32       m_nNumVirtualFixedArgs;   // number of virtual fixed arguments
        UINT32       m_cbRetType;   // return type size

        SigTypeContext m_typeContext;   // Instantiation for type parameters
        BYTE            m_types[MAX_CACHED_SIG_SIZE + 1];
        short           m_sizes[MAX_CACHED_SIG_SIZE + 1];
        short           m_offsets[MAX_CACHED_SIG_SIZE + 1];
        CorElementType  m_corNormalizedRetType;
        BYTE         m_flags;
        BYTE         m_CallConv;
        BYTE         m_WalkStatic;      // The type of function we walked
};



class MetaSigCache 
{
public:
    static void LookUp(MetaSig* pMetaSig, MethodDesc* pMD, PCCOR_SIGNATURE pCorSig, DWORD cpCorSig, Module* pModule);

private:
    static void Initialize(AppDomain* pAD, SIZE_T index, MetaSig* pMetaSig, MethodDesc* pMD, PCCOR_SIGNATURE pCorSig, DWORD cpCorSig, Module* pModule);
    
public:
    BOOL AquireWriterLock();
    void ReleaseWriterLock();
    
    BOOL AquireReaderLock();
    void ReleaseReaderLock();
    
    MetaSig* GetCachedMetaSig(PCCOR_SIGNATURE pCorSig, Module* pModule);    
    void CacheMetaSig(MetaSig* pMetaSig, PCCOR_SIGNATURE pCorSig, Module* pModule);
    
    MetaSigCache() 
    {
        m_pModule = NULL;
        m_pCorSig = NULL;
        m_cacheState = 0;
    }
   
private:
    enum {
        MetaSigCacheUninitialized = 0,
        MetaSigCacheInitializing = 1,
        MetaSigCacheInitialized = 2,
    };
    
    BYTE m_metaSig[sizeof(MetaSig)];
    Module* m_pModule;
    PCCOR_SIGNATURE m_pCorSig;
    LONG m_cacheState;
};



//=========================================================================
// Indicates whether an argument is to be put in a register using the
// default IL calling convention. This should be called on each parameter
// in the order it appears in the call signature. For a non-static method,
// this function should also be called once for the "this" argument, prior
// to calling it for the "real" arguments. Pass in a typ of IMAGE_CEE_CS_OBJECT.
//
//  *pNumRegistersUsed:  [in,out]: keeps track of the number of argument
//                       registers assigned previously. The caller should
//                       initialize this variable to 0 - then each call
//                       will update it.
//
//  typ:                 the signature type
//  structSize:          for structs, the size in bytes
//  fThis:               is this about the "this" pointer?
//  callconv:            see IMAGE_CEE_CS_CALLCONV_*
//  *pOffsetIntoArgumentRegisters:
//                       If this function returns TRUE, then this out variable
//                       receives the identity of the register, expressed as a
//                       byte offset into the ArgumentRegisters structure.
//
// 
//=========================================================================
BOOL IsArgumentInRegister(int   *pNumRegistersUsed,
                          BYTE   typ,
                          UINT32 structSize,
                          BOOL   fThis,
                          BYTE   callconv,
                          int    *pOffsetIntoArgumentRegisters);

void HandleInstParam(MetaSig& msig, int& numregsused, UINT& cb, BYTE callconv, int* paramTypeReg, UINT32* nNumVirtualFixedArgs);

BOOL IsTypeRefOrDef(LPCSTR szClassName, Module *pModule, mdToken token);

BOOL CompareTypeTokens(mdToken tk1, mdToken tk2, Module *pModule1, Module *pModule2);

// Nonthrowing version of CompareTypeTokens.
//
//   Return S_OK if they match
//          S_FALSE if they don't match
//          FAILED  if OOM or some other blocking error
//
HRESULT  CompareTypeTokensNT(mdToken tk1, mdToken tk2, Module *pModule1, Module *pModule2);

#endif /* _H_SIGINFO */

