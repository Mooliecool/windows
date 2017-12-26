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
// File: CLASS.H
//
#ifndef CLASS_H
#define CLASS_H

/*
 *  Include Files
 */
#include "eecontract.h"
#include "argslot.h"
#include "vars.hpp"
#include "cor.h"
#include "clrex.h"
#include "hash.h"
#include "crst.h"
#include "objecthandle.h"
#include "cgensys.h"
#include "declsec.h"
#include "list.h"
#include "spinlock.h"
#include "typehandle.h"
#include "perfcounters.h"
#include "methodtable.h"
#include "eeconfig.h"
#include "typectxt.h"

VOID DECLSPEC_NORETURN RealCOMPlusThrowHR(HRESULT hr);

/*
 *  Macro definitions
 */
#define MAX_LOG2_PRIMITIVE_FIELD_SIZE   3

/*
 *  Forward declarations
 */
class   AppDomain;
class   ArrayClass;
class   ArrayMethodDesc;
class   Assembly;
class   ClassLoader;
class   DictionaryLayout;
class   DomainLocalBlock;
class   FCallMethodDesc;
class   EEClass;
class   EnCFieldDesc;
class   FieldDesc;
class   FieldMarshaler;
struct  LayoutRawFieldInfo;
class   MetaSig;
class   MethodDesc;
class   MethodDescChunk;
class   MethodNameHash;
class   MethodTable;
class   Module;
struct  ModuleCtorInfo;
class   Object;
class   Stub;
class   Substitution;
class   SystemDomain;
class   TypeHandle;
class   StackingAllocator;
class   AllocMemTracker;
class   ZapCodeMap;
class   InteropMethodTableSlotDataMap;
class   ZapMonitor;
class LoadingEntry_LockHolder;
class   DispatchMapBuilder;

typedef DPTR(DictionaryLayout) PTR_DictionaryLayout;


//---------------------------------------------------------------------------------
// Fields in an explicit-layout class present varying degrees of risk depending
// on how they overlap.
//
// Each level is a superset of the lower (in numerical value) level - i.e.
// all kVerifiable fields are also kLegal, but not vice-versa.
//---------------------------------------------------------------------------------
class ExplicitFieldTrust
{
    public:
        enum TrustLevel
        {
            // Note: order is important here - each guarantee also implicitly guarantees all promises
            // made by values lower in number.

            //                       What's guaranteed.                                                  What the loader does.
            //-----                  -----------------------                                             -------------------------------
            kNone         = 0,    // no guarantees at all                                              - Type refuses to load at all.
            kLegal        = 1,    // guarantees no objref <-> scalar overlap and no unaligned objref   - Type loads but field access won't verify
            kVerifiable   = 2,    // guarantees no objref <-> objref overlap and all guarantees above  - Type loads and field access will verify
            kNonOverLayed = 3,    // guarantees no overlap at all and all guarantees above             - Type loads, field access verifies and Equals() may be optimized if structure is tightly packed

            kMaxTrust     = kNonOverLayed,
        };

};





//=======================================================================
// Adjunct to the EEClass structure for classes w/ layout
//=======================================================================
class EEClassLayoutInfo
{
    static VOID CollectLayoutFieldMetadataThrowing(
       BaseDomain *pDomain,          // Domain in which to allocate anything we allocate
       mdTypeDef cl,                // cl of the NStruct being loaded
       BYTE packingSize,            // packing size (from @dll.struct)
       BYTE nlType,                 // nltype (from @dll.struct)
       BOOL fExplicitOffsets,       // explicit offsets?
       MethodTable *pParentMT,       // the loaded superclass
       ULONG cMembers,              // total number of members (methods + fields)
       HENUMInternal *phEnumField,  // enumerator for field
       Module* pModule,             // Module that defines the scope, loader and heap (for allocate FieldMarshalers)
       const SigTypeContext *pTypeContext,          // Type parameters for NStruct being loaded
       EEClassLayoutInfo *pEEClassLayoutInfoOut,  // caller-allocated structure to fill in.
       LayoutRawFieldInfo *pInfoArrayOut, // caller-allocated array to fill in.  Needs room for cMember+1 elements
       AllocMemTracker    *pamTracker

    );


    friend class ClassLoader;
    friend class EEClass;
    friend class MethodTableBuilder;

    private:
        // size (in bytes) of fixed portion of NStruct.
        UINT32      m_cbNativeSize;
        UINT32      m_cbManagedSize;

    public:
        // 1,2,4 or 8: this is equal to the largest of the alignment requirements
        // of each of the EEClass's members. If the NStruct extends another NStruct,
        // the base NStruct is treated as the first member for the purpose of
        // this calculation.
        BYTE        m_LargestAlignmentRequirementOfAllMembers;

        // Post V1.0 addition: This is the equivalent of m_LargestAlignmentRequirementOfAllMember
        // for the managed layout.
        BYTE        m_ManagedLargestAlignmentRequirementOfAllMembers;

    private:
        enum {
            // TRUE if the GC layout of the class is bit-for-bit identical
            // to its unmanaged counterpart (i.e. no internal reference fields,
            // no ansi-unicode char conversions required, etc.) Used to
            // optimize marshaling.
            e_BLITTABLE             = 0x01,
            // Post V1.0 addition: Is this type also sequential in managed memory?
            e_MANAGED_SEQUENTIAL    = 0x02,
            // When a sequential/explicit type has no fields, it is conceptually
            // zero-sized, but actually is 1 byte in length. This holds onto this
            // fact and allows us to revert the 1 byte of padding when another
            // explicit type inherits from this type.
            e_ZERO_SIZED            = 0x04,
        };

        BYTE        m_bFlags;

        // # of fields that are of the calltime-marshal variety.
        UINT        m_numCTMFields;

        // An array of FieldMarshaler data blocks, used to drive call-time
        // marshaling of NStruct reference parameters. The number of elements
        // equals m_numCTMFields.
        FieldMarshaler *m_pFieldMarshalers;


    public:
        BOOL GetNativeSize() const
        {
            LEAF_CONTRACT;
            return m_cbNativeSize;
        }

        UINT32 GetManagedSize() const
        {
            LEAF_CONTRACT;
            return m_cbManagedSize;
        }


        BYTE GetLargestAlignmentRequirementOfAllMembers() const
        {
            LEAF_CONTRACT;
            return m_LargestAlignmentRequirementOfAllMembers;
        }

        UINT GetNumCTMFields() const
        {
            LEAF_CONTRACT;
            return m_numCTMFields;
        }

        FieldMarshaler *GetFieldMarshalers() const
        {
            LEAF_CONTRACT;
            return m_pFieldMarshalers;
        }

        BOOL IsBlittable() const
        {
            LEAF_CONTRACT;
            return (m_bFlags & e_BLITTABLE) == e_BLITTABLE;
        }

        BOOL IsManagedSequential() const
        {
            LEAF_CONTRACT;
            return (m_bFlags & e_MANAGED_SEQUENTIAL) == e_MANAGED_SEQUENTIAL;
        }

        // If true, this says that the type was originally zero-sized
        // and the native size was bumped up to one for similar behaviour
        // to C++ structs. However, it is necessary to keep track of this
        // so that we can ignore the one byte padding if other types derive
        // from this type, that we can
        BOOL IsZeroSized() const
        {
            LEAF_CONTRACT;
            return (m_bFlags & e_ZERO_SIZED) == e_ZERO_SIZED;
        }

    private:
        void SetIsBlittable(BOOL isBlittable)
        {
            LEAF_CONTRACT;
            m_bFlags = isBlittable ? (m_bFlags | e_BLITTABLE)
                                   : (m_bFlags & ~e_BLITTABLE);
        }

        void SetIsManagedSequential(BOOL isManagedSequential)
        {
            LEAF_CONTRACT;
            m_bFlags = isManagedSequential ? (m_bFlags | e_MANAGED_SEQUENTIAL)
                                           : (m_bFlags & ~e_MANAGED_SEQUENTIAL);
        }

        void SetIsZeroSized(BOOL isZeroSized)
        {
            LEAF_CONTRACT;
            m_bFlags = isZeroSized ? (m_bFlags | e_ZERO_SIZED)
                                   : (m_bFlags & ~e_ZERO_SIZED);
        }
};



//
// This structure is used only when the classloader is building the interface map.  Before the class
// is resolved, the EEClass contains an array of these, which are all interfaces *directly* declared
// for this class/interface by the metadata - inherited interfaces will not be present if they are
// not specifically declared.
//
// This structure is destroyed after resolving has completed.
//
typedef struct
{
    // The interface method table; for instantiated interfaces, this is the generic interface
    MethodTable     *m_pMethodTable;
} BuildingInterfaceInfo_t;


//
// We should not need to touch anything in here once the classes are all loaded, unless we
// are doing reflection.  Try to avoid paging this data structure in.
//

// Size of hash bitmap for method names
#define METHOD_HASH_BYTES  8

// Hash table size - prime number
#define METHOD_HASH_BITS    61


// These are some macros for forming fully qualified class names for a class.
// These are abstracted so that we can decide later if a max length for a
// class name is acceptable.

// It doesn't make any sense not to have a small but usually quite capable
// stack buffer to build class names into. Most class names that I can think
// of would fit in 128 characters, and that's a pretty small amount of stack
// to use in exchange for not having to new and delete the memory.
#define DEFAULT_NONSTACK_CLASSNAME_SIZE (MAX_CLASSNAME_LENGTH/4)

#define DefineFullyQualifiedNameForClass() \
    ScratchBuffer<DEFAULT_NONSTACK_CLASSNAME_SIZE> _scratchbuffer_; \
    InlineSString<DEFAULT_NONSTACK_CLASSNAME_SIZE> _ssclsname_;

#define DefineFullyQualifiedNameForClassOnStack() \
    ScratchBuffer<MAX_CLASSNAME_LENGTH> _scratchbuffer_; \
    InlineSString<MAX_CLASSNAME_LENGTH> _ssclsname_;

#define DefineFullyQualifiedNameForClassW() \
    InlineSString<DEFAULT_NONSTACK_CLASSNAME_SIZE> _ssclsname_w_;

#define DefineFullyQualifiedNameForClassWOnStack() \
    InlineSString<MAX_CLASSNAME_LENGTH> _ssclsname_w_;

#define GetFullyQualifiedNameForClassNestedAware(pClass) \
    pClass->_GetFullyQualifiedNameForClassNestedAware(_ssclsname_).GetUTF8(_scratchbuffer_)

#define GetFullyQualifiedNameForClassNestedAwareW(pClass) \
    pClass->_GetFullyQualifiedNameForClassNestedAware(_ssclsname_w_).GetUnicode()

#define GetFullyQualifiedNameForClass(pClass) \
    pClass->_GetFullyQualifiedNameForClass(_ssclsname_).GetUTF8(_scratchbuffer_)

#define GetFullyQualifiedNameForClassW(pClass) \
    pClass->_GetFullyQualifiedNameForClass(_ssclsname_w_).GetUnicode()

//
// This enum represents the property methods that can be passed to FindPropertyMethod().
//

enum EnumPropertyMethods
{
    PropertyGet = 0,
    PropertySet = 1,
};


//
// This enum represents the event methods that can be passed to FindEventMethod().
//

enum EnumEventMethods
{
    EventAdd = 0,
    EventRemove = 1,
    EventRaise = 2,
};


//@GENERICS:
// For most types there is a one-to-one mapping between MethodTable* and EEClass*
// However this is not the case for instantiated types where code and representation
// are shared between compatible instantiations (e.g. List<string> and List<object>)
// Then a single EEClass structure is shared between multiple MethodTable structures
// Uninstantiated generic types (e.g. List) have their own EEClass and MethodTable,
// used (a) as a representative for the generic type itself, (b) for static fields and
// methods, which aren't present in the instantiations, and (c) to hold some information
// (e.g. formal instantiations of superclass and implemented interfaces) that is common
// to all instantiations and isn't stored in the EEClass structures for instantiated types
//
//
// **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE
//
// A word about EEClass vs. MethodTable
// ------------------------------------
//
// At compile-time, we are happy to touch both MethodTable and EEClass.  However,
// at runtime we want to restrict ourselves to the MethodTable.  This is critical
// for common code paths, where we want to keep the EEClass out of our working
// set.  For uncommon code paths, like throwing exceptions or strange Contexts
// issues, it's okay to access the EEClass.
//
// To this end, the TypeHandle (CLASS_HANDLE) abstraction is now based on the
// MethodTable pointer instead of the EEClass pointer.  If you are writing a
// runtime helper that calls GetClass() to access the associated EEClass, please
// stop to wonder if you are making a mistake.
//
// **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE  **  NOTE


class EEClass // DO NOT CREATE A NEW EEClass USING NEW!
{
    /************************************
     *  FRIEND FUNCTIONS
     ************************************/
    // DO NOT ADD FRIENDS UNLESS ABSOLUTELY NECESSARY
    // USE ACCESSORS TO READ/WRITE private field members

    // To access bmt stuff
    friend class Generics;
    friend class MethodTableBuilder;
    friend class FieldDesc;
    friend class CheckAsmOffsets;
    friend class ClrDataAccess;


    /************************************
     *  PUBLIC INSTANCE METHODS
     ************************************/
public:

    BaseDomain * GetDomain();
    Assembly * GetAssembly();
    Module* GetLoaderModule();
    Module* GetZapModule();

    DWORD  IsSealed()
    {
        LEAF_CONTRACT;
        return IsTdSealed(m_dwAttrClass);
    }

    inline DWORD IsObjectClass()
    {
        LEAF_CONTRACT;
        return (this == g_pObjectClass->GetClass());
    }

    // Is this System.ValueType?
    inline DWORD IsValueTypeClass()
    {
        LEAF_CONTRACT;
        return this == g_pValueTypeClass->GetClass();
    }

    inline DWORD IsInterface()
    {
        WRAPPER_CONTRACT;
        return IsTdInterface(m_dwAttrClass);
    }

    inline DWORD HasVarSizedInstances()
    {
        WRAPPER_CONTRACT;
        return this == g_pStringClass->GetClass() || IsArrayClass();
    }

    inline DWORD IsAbstract()
    {
        WRAPPER_CONTRACT;
        return IsTdAbstract(m_dwAttrClass);
    }

    inline DWORD IsAnyDelegateClass()
    {
        WRAPPER_CONTRACT;
        return IsMultiDelegateClass();
    }

    inline BOOL IsSharedByGenericInstantiations()
    {
        WRAPPER_CONTRACT;

        g_IBCLogger.LogEEClassAndMethodTableAccess(this);

        return (m_VMFlags & VMFLAG_GENERICS_MASK) == VMFLAG_GENERIC_SHAREDINST;
    }

    // It's some kind of instantiation (SHAREDINST, UNSHAREDINST, TYPICALINST)
    inline BOOL HasInstantiation()
    {
        LEAF_CONTRACT;
        return (m_VMFlags & VMFLAG_GENERICS_MASK) != 0;
    }

    // Return true if this is an instantiation of the same type def as the argument
    // or is the same type, if not instantiated
    inline BOOL HasSameTypeDefAs(EEClass *pClass)
    {
        LEAF_CONTRACT;
        return ((GetModule() == pClass->GetModule()) && 
                (GetCl_NoLogging()     == pClass->GetCl_NoLogging()));
    }

    BOOL IsIntrospectionOnly();
    VOID EnsureActive();

    // Returns true for any class which is either itself a generic
    // instantiation or is derived from a generic
    // instantiation anywhere in it's class hierarchy,
    //
    // e.g. class D : C<int>
    // or class E : D, class D : C<int>
    //
    // Does not return true just because the class supports
    // an instantiated interface type.
    inline BOOL HasGenericClassInstantiationInHierarchy()
    {

        WRAPPER_CONTRACT;
        return (GetNumDicts() != 0);
    }


    inline BOOL IsGenericTypeDefinition()
    {
        WRAPPER_CONTRACT;
        return (m_VMFlags & VMFLAG_GENERICS_MASK) == VMFLAG_GENERIC_TYPICALINST;
    }

    inline BOOL IsTypicalTypeDefinition()
    {
        WRAPPER_CONTRACT;
        return !HasInstantiation() || IsGenericTypeDefinition();
    }

    inline TypeHandle * GetCanonicalInstantiation()
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            FORBID_FAULT;
        }
        CONTRACTL_END

        if (HasInstantiation())
            return this->GetMethodTable()->GetInstantiation();
        else
            return NULL;
    }

    // Return a substitution suitbale for interpreting
    // the metadata in parent class, assuming we already have a subst.
    // suitable for interpreting the current class.
    //
    // If, for example, the definition for the current class is
    //   D<T> : C<List<T>, T[] > 
    // then this will be 
    //   0 --> List<T>
    //   1 --> T[]
    // added to the chain of substitutions.
    Substitution GetSubstitutionForParent(const Substitution *pSubst); 

    BOOL HasExplicitFieldOffsetLayout()
    {
        WRAPPER_CONTRACT;
        return IsTdExplicitLayout(GetAttrClass()) && HasLayout();
    }

    BOOL HasSequentialLayout()
    {
        WRAPPER_CONTRACT;
        return IsTdSequentialLayout(GetAttrClass());
    }
    BOOL IsSerializable()
    {
        WRAPPER_CONTRACT;
        return IsTdSerializable(GetAttrClass());
    }
    BOOL IsBeforeFieldInit()
    {
        WRAPPER_CONTRACT;
        return IsTdBeforeFieldInit(GetAttrClass());
    }

    DWORD GetProtection()
    {
        WRAPPER_CONTRACT;
        return (m_dwAttrClass & tdVisibilityMask);
    }

    // class is blittable
    BOOL IsBlittable();

    //
    // Security properties accessor methods
    //

    inline BOOL RequiresLinktimeCheck()
    {
        WRAPPER_CONTRACT;
        PSecurityProperties psp = GetSecurityProperties();
        return psp && psp->RequiresLinktimeCheck();
    }

    inline BOOL RequiresInheritanceCheck()
    {
        WRAPPER_CONTRACT;
        PSecurityProperties psp = GetSecurityProperties();
        return psp && psp->RequiresInheritanceCheck();
    }

    inline BOOL RequiresCasInheritanceCheck()
    {
        WRAPPER_CONTRACT;
        PSecurityProperties psp = GetSecurityProperties();
        return psp && psp->RequiresCasInheritanceCheck();
    }

    inline BOOL RequiresNonCasInheritanceCheck()
    {
        WRAPPER_CONTRACT;
        PSecurityProperties psp = GetSecurityProperties();
        return psp && psp->RequiresNonCasInheritanceCheck();
    }


#ifndef DACCESS_COMPILE
    void *operator new(size_t size, size_t extraSize, LoaderHeap* pHeap, Module *pModule, size_t *dwSizeRequestedForAlloc, AllocMemTracker *pamTracker);
    void Destruct();
#endif // !DACCESS_COMPILE

    BOOL     IsThunking()       { WRAPPER_CONTRACT; return m_pMethodTable->IsThunking(); }

    // Helper routines for the macros defined at the top of this class.
    // You probably should not use these functions directly.
    SString &_GetFullyQualifiedNameForClassNestedAware(SString &ssBuf);
    SString &_GetFullyQualifiedNameForClass(SString &ssBuf);

    LPCUTF8 GetFullyQualifiedNameInfo(LPCUTF8 *ppszNamespace);

    void GetPathForErrorMessages(SString & result);

    // Used by FindMethod and varieties
    enum FM_Flags
    {
        // Default behaviour is to scan all methods, virtual and non-virtual, of the current type
        // and and all non-virtual methods of all parent types.

        // Default set of flags - this must always be zero.
        FM_Default             = 0x0000,

        // Case sensitivity
        FM_IgnoreCase          = 0x0001,                        // Name matching is case insensitive

        // USE THE FOLLOWING WITH EXTREME CAUTION. We do not want to inadvertently
        // change binding semantics by using this without a really good reason.

        // Virtuals
        FM_ExcludeNonVirtual   = (FM_IgnoreCase          << 1), // has mdVirtual set
        FM_ExcludeVirtual      = (FM_ExcludeNonVirtual   << 1), // does not have mdVirtual set.

        // Accessibility. 
        // NOTE: These appear in the exact same order as mdPrivateScope ... mdPublic in corhdr.h. This enables some
        //       bit masking to quickly determine if a method qualifies in FM_ShouldSkipMethod.
        FM_ExcludePrivateScope = (FM_ExcludeVirtual      << 1), // Member not referenceable.
        FM_ExcludePrivate      = (FM_ExcludePrivateScope << 1), // Accessible only by the parent type.
        FM_ExcludeFamANDAssem  = (FM_ExcludePrivate      << 1), // Accessible by sub-types only in this Assembly.
        FM_ExcludeAssem        = (FM_ExcludeFamANDAssem  << 1), // Accessibly by anyone in the Assembly.
        FM_ExcludeFamily       = (FM_ExcludeAssem        << 1), // Accessible only by type and sub-types.
        FM_ExcludeFamORAssem   = (FM_ExcludeFamily       << 1), // Accessibly by sub-types anywhere, plus anyone in assembly.
        FM_ExcludePublic       = (FM_ExcludeFamORAssem   << 1),

        FM_ForInterface        = (FM_ExcludeNonVirtual |
                                  FM_ExcludePrivateScope |
                                  FM_ExcludePrivate |
                                  FM_ExcludeFamANDAssem |
                                  FM_ExcludeAssem |
                                  FM_ExcludeFamily |
                                  FM_ExcludeFamORAssem),
    };

private:
    // A mask to indicate that some filtering needs to be done.
    static const FM_Flags FM_SpecialAccessMask = (FM_Flags) (FM_ExcludePrivateScope |
                                                             FM_ExcludePrivate |
                                                             FM_ExcludeFamANDAssem |
                                                             FM_ExcludeAssem |
                                                             FM_ExcludeFamily |
                                                             FM_ExcludeFamORAssem |
                                                             FM_ExcludePublic);

    static const FM_Flags FM_SpecialVirtualMask = (FM_Flags) (FM_ExcludeNonVirtual |
                                                              FM_ExcludeVirtual);

    // Typedef for string comparition functions.
    typedef int (__cdecl *UTF8StringCompareFuncPtr)(const char *, const char *);

    inline UTF8StringCompareFuncPtr FM_GetStrCompFunc(DWORD dwFlags)
        { LEAF_CONTRACT; return (dwFlags & FM_IgnoreCase) ? stricmpUTF8 : strcmp; }

    BOOL FM_ShouldSkipMethod(DWORD dwAttrs, FM_Flags flags);

public:
    MethodDesc *FindMethod(
       LPCUTF8 pwzName,
       LPHARDCODEDMETASIG pwzSignature,
       FM_Flags flags = FM_Default);

    // typeHnd is the type handle associated with the class being looked up.
    // It has additional information in the case of a domain neutral class (Arrays)
    MethodDesc *FindMethod(
       LPCUTF8 pszName, 
       PCCOR_SIGNATURE pSignature, 
       DWORD cSignature, 
       Module* pModule, 
       const Substitution* pSigSubst = NULL,
       FM_Flags flags = FM_Default,
       const Substitution *pDefSubst = NULL);

    MethodDesc *FindMethod(mdMethodDef mb);

    inline MethodDesc *InterfaceFindMethod(
       LPCUTF8 pszName, 
       PCCOR_SIGNATURE pSignature, 
       DWORD cSignature, 
       Module* pModule, 
       FM_Flags flags = FM_Default,
       const Substitution *subst = NULL);

    MethodDesc *FindMethodByName(
       LPCUTF8 pszName,
       FM_Flags flags = FM_Default);

    MethodDesc *FindPropertyMethod(
       LPCUTF8 pszName,
       EnumPropertyMethods Method,
       FM_Flags flags = FM_Default);

    MethodDesc *FindEventMethod(
       LPCUTF8 pszName,
       EnumEventMethods Method,
       FM_Flags flags = FM_Default);

    MethodDesc *FindMethodForInterfaceSlot(
       MethodTable *pInterface,
       WORD slotNum);

    // pSignature can be NULL to find any field with the given name
    FieldDesc *FindField(
       LPCUTF8 pszName,
       PCCOR_SIGNATURE pSignature,
       DWORD cSignature,
       Module* pModule,
       BOOL bCaseSensitive = TRUE);

    MethodDesc *FindConstructor(LPHARDCODEDMETASIG pwzSignature);
    MethodDesc *FindConstructor(PCCOR_SIGNATURE pSignature,DWORD cSignature, Module* pModule);


    inline IMDInternalImport* GetMDImport()
    {
        WRAPPER_CONTRACT;
        return GetModule()->GetMDImport();
    }

    MethodDesc* GetBoxedEntryPointMD(MethodDesc *pMD);
    MethodDesc* GetUnboxedEntryPointMD(MethodDesc *pMD);




    EEClassLayoutInfo *GetLayoutInfo();


    // Used for debugging class layout. Dumps to the debug console
    // when debug is true.
    void DebugDumpVtable(LPCUTF8 pszClassName, BOOL debug)
    {
        WRAPPER_CONTRACT;
        GetMethodTable()->DebugDumpVtable(pszClassName, debug);
    }
    void DebugDumpFieldLayout(LPCUTF8 pszClassName, BOOL debug);
    void DebugRecursivelyDumpInstanceFields(LPCUTF8 pszClassName, BOOL debug);
    void DebugDumpGCDesc(LPCUTF8 pszClassName, BOOL debug);

private:
    inline ClassLoader *GetClassLoader()
    {
        WRAPPER_CONTRACT;
        return GetModule()->GetClassLoader();
    }
public:
    inline ClassLoader* GetLoader ()
    {
        WRAPPER_CONTRACT;
        return GetClassLoader();
    }

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    bool ComputeInternalCorElementTypeForValueType(CorElementType* pInternalTypeOut);



    /************************************
     *  INSTANCE MEMBER VARIABLES
     ************************************/
#ifdef _DEBUG
public:
    inline LPCUTF8 GetDebugClassName ()
    {
        LEAF_CONTRACT;
        return m_szDebugClassName;
    }
    inline void SetDebugClassName (LPCUTF8 szDebugClassName)
    {
        LEAF_CONTRACT;
        m_szDebugClassName = szDebugClassName;
    }

    /*
     * Controls debugging breaks and output if a method class
     * is mentioned in the registry ("BreakOnClassBuild")
     * Method layout within this class can cause a debug
     * break by setting "BreakOnMethodName". Not accessible
     * outside the class.
     */

#endif // _DEBUG


    /*
     * Each interface is assigned a unique Id based on the module scoped global interface table.
     */

    //private:
    // <NICE> Make this private to EEClass, and then remove it altogether. All the code
    // that uses it is too dependent on EEClass </NICE>
    inline EEClass *GetParentClass ()
    {
        LEAF_CONTRACT;
        //        _ASSERTE(GetMethodTable()->GetLoadLevel() >= CLASS_LOAD_APPROXPARENTS);
        if (m_pMethodTable)
        {
            MethodTable *pParentMT = m_pMethodTable->GetParentMethodTable();
            if (pParentMT)
                return pParentMT->GetClass();
        }
        return NULL;
    }

public:
    /*
     * Maintain backpointer to the module that this class was declared in.
     * @GENERICS: this will be the same for all instantiations of a generic type
     */
    inline Module* GetModule()
    {
        WRAPPER_CONTRACT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return GetModule_NoLogging();
    }
    // We have this version just so if anyone thinks of adding logging to GetModule then
    // we record the places where we should use the NoLogging variant.
    inline Module* GetModule_NoLogging()
    {
        LEAF_CONTRACT;
        PREFIX_ASSUME(m_pModule != NULL);
        return m_pModule;
    }
#ifndef DACCESS_COMPILE
    void SetModule (Module* pModule)
    {
        LEAF_CONTRACT;
        m_pModule = pModule;
    }
#endif // !DACCESS_COMPILE
    /*                                                       */
    TADDR* GetModulePtr ()
    {
        LEAF_CONTRACT;
        return (TADDR*)&m_pModule;
    }

    BOOL HasModuleDependencies()
    {
        WRAPPER_CONTRACT;
        return m_classDependencies.TestAnyBit();
    }

    class Module::DependencySetIterator IterateModuleDependencies()
    {
        WRAPPER_CONTRACT;
        return GetModule()->IterateClassDependencies(&m_classDependencies);
    }

    /*
     * Maintain class ctor slot to check if class ctor has been run
     */
    BOOL HasClassConstructor()
    {
        LEAF_CONTRACT;
        return m_wCCtorSlot != MethodTable::NO_SLOT;
    }
    WORD GetClassConstructorSlot()
    {
        LEAF_CONTRACT;
        _ASSERTE(HasClassConstructor());
        return m_wCCtorSlot;
    }
    void SetClassConstructorSlot (WORD wCCtorSlot)
    {
        LEAF_CONTRACT;
        m_wCCtorSlot = wCCtorSlot;
    }

    /*
     * Maintain default ctor slot
     */
    BOOL HasDefaultConstructor()
    {
        LEAF_CONTRACT;
        return m_wDefaultCtorSlot != MethodTable::NO_SLOT;
    }
    WORD GetDefaultConstructorSlot()
    {
        LEAF_CONTRACT;
        _ASSERTE(HasDefaultConstructor());
        return m_wDefaultCtorSlot;
    }
    void SetDefaultConstructorSlot (WORD wDefaultCtorSlot)
    {
        LEAF_CONTRACT;
        m_wDefaultCtorSlot = wDefaultCtorSlot;
    }

    /*
     * Maintain back pointer to statcally hot portion of EEClass.
     * For an EEClass representing multiple instantiations of a generic type, this is the method table
     * for the first instantiation requested and is the only one containing entries for non-virtual instance methods
     * (i.e. non-vtable entries).
     */

    // Note that EEClass structures may be shared between generic instantiations
    // (see IsSharedByGenericInstantiations).  In these cases  EEClass::GetMethodTable
    // will return the method table pointer corresponding to the "canonical"
    // instantiation, as defined in typehandle.h.
    //
    inline MethodTable* GetMethodTable()
    {
        LEAF_CONTRACT;

        g_IBCLogger.LogEEClassAndMethodTableAccess(this);

        return m_pMethodTable;
    }

    // DO NOT ADD ANY ASSERTS TO THIS METHOD.
    // DO NOT USE THIS METHOD.
    // Yes folks, for better or worse the debugger pokes supposed object addresses
    // to try to see if objects are valid, possibly firing an AccessViolation or worse,
    // and then catches the AV and reports a failure to the debug client.  This makes
    // the debugger slightly more robust should any corrupted object references appear
    // in a session. Thus it is "correct" behaviour for this to AV when used with
    // an invalid object pointer, and incorrect behaviour for it to
    // assert.
    inline MethodTable* GetMethodTableWithPossibleAV()
    {
        CANNOT_HAVE_CONTRACT;
        return m_pMethodTable;
    }
#ifndef DACCESS_COMPILE
    inline void SetMethodTableForTransparentProxy(MethodTable*  pMT)
    {
        LEAF_CONTRACT;
        // Transparent proxy class' true method table
        // is replaced by a global thunk table

        _ASSERTE(pMT->IsTransparentProxyType() &&
                m_pMethodTable->IsTransparentProxyType());

        g_IBCLogger.LogEEClassCOWTableAccess(this);

        m_pMethodTable = pMT;
    }
    inline void SetMethodTable(MethodTable*  pMT)
    {
        LEAF_CONTRACT;
        m_pMethodTable = pMT;
    }
#endif // !DACCESS_COMPILE

    /*
     * Number of fields in the class, including inherited fields.
     * Does not include fields added from EnC.
     */
    inline WORD GetNumInstanceFields()
    {
        LEAF_CONTRACT;
        return m_wNumInstanceFields;
    }
    inline WORD GetNumIntroducedInstanceFields()
    {
        LEAF_CONTRACT;
        _ASSERTE(GetMethodTable()->IsRestored() || IsValueClass());
        // Special check for IsRestored - local variable value types may be
        // reachable but not restored.
        if (GetMethodTable()->IsRestored() && GetParentClass() != NULL)
        {
            WORD m_wNumParentInstanceFields = GetParentClass()->GetNumInstanceFields();

            // If this assert fires, then our bookkeaping is bad. Perhaps we incremented the count
            // of fields on the base class w/o incrementing the count in the derived class. (EnC scenarios).
            _ASSERTE(m_wNumInstanceFields >= m_wNumParentInstanceFields);
        
            return m_wNumInstanceFields - m_wNumParentInstanceFields;
        }            
        return m_wNumInstanceFields;
    }
    inline void SetNumInstanceFields (WORD wNumInstanceFields)
    {
        LEAF_CONTRACT;
        m_wNumInstanceFields = wNumInstanceFields;
    }

    /*
     * Number of static fields declared in this class.
     * Implementation Note: Static values are laid out at the end of the MethodTable vtable.
     */
    inline WORD GetNumStaticFields()
    {
        LEAF_CONTRACT;
        return m_wNumStaticFields;
    }
    inline void SetNumStaticFields (WORD wNumStaticFields)
    {
        LEAF_CONTRACT;
        m_wNumStaticFields = wNumStaticFields;
    }

    // Statics are stored in a big chunk inside the module
    #define    MODULE_NON_DYNAMIC_STATICS      ((DWORD)-1)

    inline  DWORD GetModuleDynamicID()
    {
        LEAF_CONTRACT;
        return m_cbModuleDynamicID;
    }

    inline void SetModuleDynamicID(DWORD cbModuleDynamicID)
    {
        LEAF_CONTRACT;
        m_cbModuleDynamicID = cbModuleDynamicID;
    }

    /*
     * Difference between the InterfaceMap ptr and Vtable in the
     * MethodTable used to indicate the number of static bytes
     * Now interfaceMap ptr can be optional hence we store it here
     * @TODO:akhune : Investigate if we can eliminate this field by using the m_wNumStaticFields.
     */
    inline DWORD GetNonGCStaticFieldBytes()
    {
        LEAF_CONTRACT;
        return m_cbNonGCStaticFieldBytes;
    }
    inline void SetNonGCStaticFieldBytes (DWORD cbNonGCStaticFieldBytes)
    {
        LEAF_CONTRACT;
        m_cbNonGCStaticFieldBytes = cbNonGCStaticFieldBytes;
    }


    /*
     * Number of static handles allocated
     */
    inline WORD GetNumHandleStatics ()
    {
        LEAF_CONTRACT;
        return m_wNumHandleStatics;
    }
    inline void SetNumHandleStatics (WORD wNumHandleStatics)
    {
        LEAF_CONTRACT;
        m_wNumHandleStatics = wNumHandleStatics;
    }

    /*
     * Number of boxed statics allocated
     */
    inline WORD GetNumBoxedStatics ()
    {
        LEAF_CONTRACT;
        return m_wNumBoxedStatics;
    }
    inline void SetNumBoxedStatics (WORD wNumBoxedStatics)
    {
        LEAF_CONTRACT;
        m_wNumBoxedStatics = wNumBoxedStatics;
    }


    /*
     * Number of bytes of instance fields stored in the object on the GC heap.
     * Implementation Note: Warning, this can be any number, it is NOT rounded up to DWORD alignment etc.
     * This doesn't make sense for generic types (but does for their instantiations)
     */
    inline DWORD GetNumInstanceFieldBytes()
    {
        WRAPPER_CONTRACT;

        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        
        return(m_dwNumInstanceFieldBytes);
    }
    inline DWORD GetAlignedNumInstanceFieldBytes()
    {
        WRAPPER_CONTRACT;
        
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);

        return ((m_dwNumInstanceFieldBytes + 3) & (~3));
    }
    inline void SetNumInstanceFieldBytes (DWORD dwNumInstanceFieldBytes)
    {
        LEAF_CONTRACT;
        m_dwNumInstanceFieldBytes = dwNumInstanceFieldBytes;
    }


    /*
     * Pointer to a list of FieldDescs declared in this class
     * There are (m_wNumInstanceFields - GetParentClass()->m_wNumInstanceFields + m_wNumStaticFields) entries
     * in this array
     */

#ifndef DACCESS_COMPILE
    inline FieldDesc *GetApproxFieldDescListRaw()
    {
        WRAPPER_CONTRACT;
        // Careful about using this method. If it's possible that fields may have been added via EnC, then
        // must use the FieldDescIterator as any fields added via EnC won't be in the raw list

        g_IBCLogger.LogEEClassAndMethodTableAccess(this); // touched from Binder::FetchField, This THROWS!

        return m_pFieldDescList;
    }
#endif // !DACCESS_COMPILE
    inline PTR_FieldDesc GetFieldDescListPtr()
    {
        WRAPPER_CONTRACT;
        // Careful about using this method. If it's possible that fields may have been added via EnC, then
        // must use the FieldDescIterator as any fields added via EnC won't be in the raw list

#ifndef DACCESS_COMPILE
        g_IBCLogger.LogEEClassAndMethodTableAccess(this); // touched from Binder::FetchField, This THROWS!
        return PTR_FieldDesc((TADDR)m_pFieldDescList);
#else // DACCESS_COMPILE
        return PTR_FieldDesc((TADDR)m_pFieldDescList_UseAccessor);
#endif // DACCESS_COMPILE
    }
#ifndef DACCESS_COMPILE
    inline void SetFieldDescList (FieldDesc* pFieldDescList)
    {
        LEAF_CONTRACT;
        m_pFieldDescList = pFieldDescList;
    }
#endif // !DACCESS_COMPILE

    /*
     * Number of pointer series @TODO:akhune (Provide better explanation for this member...)
     * This doesn't make sense for generic types (but does for their instantiations)
     */
    inline WORD GetNumGCPointerSeries()
    {
        LEAF_CONTRACT;
        return m_wNumGCPointerSeries;
    }
    inline void SetNumGCPointerSeries (WORD wNumGCPointerSeries)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        m_wNumGCPointerSeries = wNumGCPointerSeries;
    }

    /*
     * Cached metadata for this class (GetTypeDefProps)
     */
    inline DWORD GetAttrClass()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return m_dwAttrClass;
    }
    inline void SetAttrClass (DWORD dwAttrClass)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        m_dwAttrClass = dwAttrClass;
    }


    inline DWORD IsDestroyed()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_DESTROYED);
    }
private:
    inline DWORD IsFixedUp()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        return (m_VMFlags & VMFLAG_FIXED_UP);
    }
    inline void SetFixedUp()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        m_VMFlags |= (DWORD) VMFLAG_FIXED_UP;
    }
public:
    inline DWORD IsValueClass()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_VALUETYPE);
    }

    inline DWORD IsUnsafeValueClass()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_UNSAFEVALUETYPE);
    }

    
private:
    inline void SetValueClass()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_VALUETYPE);
    }

    inline void SetUnsafeValueClass()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_UNSAFEVALUETYPE);
    }
    
public:
    inline BOOL HasNoGuid()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_NO_GUID);
    }
    inline void SetHasNoGuid()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags, VMFLAG_NO_GUID);
    }

public:

    // Is this a contextful class?
    inline BOOL IsContextful()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_VMFlags & VMFLAG_CONTEXTFUL;
    }
    // Is this class marshaled by reference
    inline BOOL IsMarshaledByRef()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        _ASSERTE(GetMethodTable());
        return GetMethodTable()->IsMarshaledByRef();
    }
    inline void SetDoesNotHaveSuppressUnmanagedCodeAccessAttr()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_NOSUPPRESSUNMGDCODEACCESS);
    }

    inline BOOL HasSuppressUnmanagedCodeAccessAttr()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return !(m_VMFlags & VMFLAG_NOSUPPRESSUNMGDCODEACCESS);
    }

    inline BOOL HasRemotingProxyAttribute()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_VMFlags & VMFLAG_REMOTING_PROXY_ATTRIBUTE;
    }
    inline void SetHasRemotingProxyAttribute()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        m_VMFlags |= (DWORD)VMFLAG_REMOTING_PROXY_ATTRIBUTE;
    }
    int IsSparseForCOMInterop()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_VMFlags & VMFLAG_SPARSE_FOR_COMINTEROP;
    }
    void SetSparseForCOMInterop()
    {
        LEAF_CONTRACT;
        m_VMFlags |= (DWORD) VMFLAG_SPARSE_FOR_COMINTEROP;
    }
    inline BOOL IsEnum()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_ENUMTYPE);
    }
    inline BOOL IsTruePrimitive()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_TRUEPRIMITIVE);
    }
    inline void SetIsTruePrimitive()
    {
        LEAF_CONTRACT;
        m_VMFlags |= (DWORD)VMFLAG_TRUEPRIMITIVE;
    }
    inline void SetEnum()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_ENUMTYPE);
    }
    inline BOOL IsAlign8Candidate()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_PREFER_ALIGN8);
    }
    inline void SetAlign8Candidate()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_PREFER_ALIGN8);
    }
    inline void SetContextfull()
    {
        LEAF_CONTRACT;
        COUNTER_ONLY(GetPrivatePerfCounters().m_Context.cClasses++);

        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_CONTEXTFUL);
    }
    inline void SetDestroyed()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr((ULONG *) &m_VMFlags, VMFLAG_DESTROYED);
    }
    inline void SetHasLayout()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        m_VMFlags |= (DWORD) VMFLAG_HASLAYOUT;  //modified before the class is published
    }
    inline void SetHasOverLayedFields()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_HASOVERLAYEDFIELDS);
    }
    inline void SetHasNonVerifiablyOverLayedFields()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_HASNONVERIFIABLYOVERLAYEDFIELDS);
    }
    inline void SetIsNested()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_ISNESTED);
    }
    inline DWORD IsArrayClass()
    {
        WRAPPER_CONTRACT;
#ifndef DACCESS_COMPILE
        // DAC can consume bad data in some cases
        _ASSERTE((m_pMethodTable == 0) || ((m_wAuxFlags & AUXFLAG_ARRAY_CLASS) != 0) == (GetMethodTable()->IsArray() != 0));
#endif // !DACCESS_COMPILE

        // We know that the Array Methodtable is constructed on the fly along with the EEClass
        // so if the methodtable pointer is null then this means that the class is not an array class.
        // We asserte this first
#ifdef _DEBUG
        if (m_wAuxFlags & AUXFLAG_ARRAY_CLASS)
        {
            // This is an array class, then make sure no one calls this method while building the
            // class and methodtables
            _ASSERTE(m_pMethodTable);
        }
#endif // _DEBUG
        return (GetMethodTable() ? GetMethodTable()->IsArray() : 0);
    }
    inline DWORD IsMultiDelegateClass()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_ISMULTIDELEGATE);
    }
    inline void SetIsMultiDelegate()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassCOWTableAccess(this);
        FastInterlockOr(&m_VMFlags,VMFLAG_ISMULTIDELEGATE);
    }
    inline void SetVMFlags (DWORD fVMFlags)
    {
        LEAF_CONTRACT;
        m_VMFlags = fVMFlags;
    }

    // This is only applicable to interfaces. This method does not
    // provide correct information for non-interface types.
    DWORD  SomeMethodsRequireInheritanceCheck();
    void SetSomeMethodsRequireInheritanceCheck();

    BOOL ContainsStackPtr()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_VMFlags & VMFLAG_CONTAINS_STACK_PTR;
    }
    void SetContainsStackPtr()
    {
        LEAF_CONTRACT;
        m_VMFlags |= (DWORD)VMFLAG_CONTAINS_STACK_PTR;
    }
    BOOL HasLayout()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return m_VMFlags & VMFLAG_HASLAYOUT;
    }
    BOOL HasOverLayedField()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_VMFlags & VMFLAG_HASOVERLAYEDFIELDS;
    }
    BOOL HasNonVerifiablyOverLayedField()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_VMFlags & VMFLAG_HASNONVERIFIABLYOVERLAYEDFIELDS;
    }
    BOOL IsNested()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_VMFlags & VMFLAG_ISNESTED;
    }
    BOOL HasFieldsWhichMustBeInited()
    {
        LEAF_CONTRACT;
        return (m_VMFlags & VMFLAG_HAS_FIELDS_WHICH_MUST_BE_INITED);
    }
    void SetHasFieldsWhichMustBeInited()
    {
        LEAF_CONTRACT;
        m_VMFlags |= (DWORD)VMFLAG_HAS_FIELDS_WHICH_MUST_BE_INITED;
    }
    DWORD CannotBeBlittedByObjectCloner()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_CANNOT_BE_BLITTED_BY_OBJECT_CLONER);
    }
    void SetCannotBeBlittedByObjectCloner()
    {
        LEAF_CONTRACT;
        m_VMFlags |= (DWORD)VMFLAG_CANNOT_BE_BLITTED_BY_OBJECT_CLONER;
    }
    DWORD HasNonPublicFields()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_VMFlags & VMFLAG_HASNONPUBLICFIELDS);
    }
    void SetHasNonPublicFields()
    {
        LEAF_CONTRACT;
        m_VMFlags |= (DWORD)VMFLAG_HASNONPUBLICFIELDS;
    }

    BOOL IsManagedSequential();

    /*
     * Security attributes for the class are stored here.  Do not update this field after the
     * class is constructed without also updating the enum_flag_NoSecurityProperties on the
     * methodtable.
     */
    inline SecurityProperties* GetSecurityProperties()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        SecurityProperties* psp = PSPS_FROM_PSECURITY_PROPS(&m_SecProps);
        _ASSERTE((IsArrayClass() || psp != NULL) &&
                 "Security properties object expected for non-array class");
        return psp;
    }

    /*
     * Metadata typedef token for this class
     * @GENERICS: this will be the same for all instantiations of a generic type
     * The token is valid only in the context of the module (and its scope)
     */
    inline mdTypeDef GetCl()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return GetCl_NoLogging();
    }
    inline mdTypeDef GetCl_NoLogging()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_cl;
    }
    inline void Setcl (mdTypeDef cl)
    {
        LEAF_CONTRACT;
        m_cl = cl;
    }

    inline BOOL IsGlobalClass()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (GetCl() == COR_GLOBAL_PARENT_TOKEN);
    }

    /*
     * The CorElementType for this class (most classes = ELEMENT_TYPE_CLASS)
     */
public:
    // This is what would be used in the calling convention for this type. 
    CorElementType  GetInternalCorElementType()
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        return CorElementType(m_NormType);
    }
    static DWORD GetOffsetOfInternalType ()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return offsetof(EEClass, m_NormType);
    }
    void SetInternalCorElementType (CorElementType _NormType)
    {
        LEAF_CONTRACT;
        m_NormType = _NormType;
    }

    /*
     * Chain of MethodDesc chunks for the MethodTable
     */
public:
    PTR_MethodDescChunk GetChunks();
    PTR_MethodDescChunk *GetChunksPtr();

#ifndef DACCESS_COMPILE
    inline void SetChunks (MethodDescChunk* pChunks)
    {
        LEAF_CONTRACT;
        m_pChunks = pChunks;
    }
#endif // !DACCESS_COMPILE
    void AddChunk (MethodDescChunk* pNewChunk);


    inline GuidInfo *GetGuidInfo()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        _ASSERTE(IsInterface());
        return m_pGuidInfo;
    }
    inline void SetGuidInfo(GuidInfo* pGuidInfo)
    {
        LEAF_CONTRACT;
        m_pGuidInfo = pGuidInfo;
    }

    // Cached class level reliability contract info, see ConstrainedExecutionRegion.cpp for details.
    inline DWORD GetReliabilityContract()
    {
        LEAF_CONTRACT;
        return m_dwReliabilityContract;
    }

    inline void SetReliabilityContract(DWORD dwValue)
    {
        LEAF_CONTRACT;
        m_dwReliabilityContract = dwValue;
    }



    inline UINT32 GetNativeSize()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return m_cbNativeSize;
    }
    static UINT32 GetOffsetOfNativeSize()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (UINT32)(offsetof(EEClass, m_cbNativeSize));
    }
    void SetNativeSize(UINT32 nativeSize)
    {
        LEAF_CONTRACT;
        m_cbNativeSize = nativeSize;
    }


public:

    // Number of type parameters for a generic or instantiated type:
    DWORD GetNumGenericArgs()
    {
        WRAPPER_CONTRACT;
        return GetMethodTable()->GetNumGenericArgs();
    }

    // The number of dictionary entries in PerInstInfo
    DWORD GetNumDicts()
    {
        WRAPPER_CONTRACT;
        return GetMethodTable()->GetNumDicts();
    }

    DictionaryLayout* GetDictionaryLayout()
    {
        WRAPPER_CONTRACT;
        if (IsSharedByGenericInstantiations())
            return m_pDictLayout;
        else
            return NULL;
    }

    CorGenericParamAttr GetVarianceOfTypeParameter(DWORD i)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        PRECONDITION(i >= 0 && i < GetNumGenericArgs());
        if (m_pVarianceInfo == NULL)
            return gpNonVariant;
        else
            return (CorGenericParamAttr) (m_pVarianceInfo[i]);
    }

    BYTE* GetVarianceInfo() {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_pVarianceInfo;
    }

    // Check that a signature blob uses type parameters correctly
    // in accordance with the variance annotations specified by this class
    // The position parameter indicates the variance of the context we're in
    // (result type is gpCovariant, argument types are gpContravariant, deeper in a signature
    // we might be gpNonvariant e.g. in a pointer type or non-variant generic type)
    BOOL CheckVarianceInSig(DWORD numGenericArgs, BYTE *pVarianceInfo, SigPointer sp, CorGenericParamAttr position);

    // This is the size of the instantiation/dictionary stored at the end of the
    // method table for each method table for this EEClass.
    DWORD GetInstAndDictSize();



#if defined(CHECK_APP_DOMAIN_LEAKS) || defined(_DEBUG)
public:
    enum{
        AUXFLAG_APP_DOMAIN_AGILE                = 0x00000001,
        AUXFLAG_CHECK_APP_DOMAIN_AGILE          = 0x00000002,
        AUXFLAG_APP_DOMAIN_AGILITY_DONE         = 0x00000004,
        AUXFLAG_ARRAY_CLASS                     = 0x00000020,
        AUXFLAG_CLASS_IS_MARSHALED_BY_REF       = 0x00000040
    };

    inline DWORD GetAuxFlagsRaw()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return m_wAuxFlags;
    }
    inline DWORD*  GetAuxFlagsPtr()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (DWORD*)(&m_wAuxFlags);
    }
    inline void SetAuxFlags(DWORD flag)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        m_wAuxFlags |= (WORD)flag;
    }

    // This flag is set (in a checked build only?) for classes whose
    // instances are always app domain agile.  This can
    // be either because of type system guarantees or because
    // the class is explicitly marked.
    inline BOOL IsAppDomainAgile()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_wAuxFlags & AUXFLAG_APP_DOMAIN_AGILE);
    }
    inline void SetAppDomainAgile()
    {
        LEAF_CONTRACT;
        m_wAuxFlags |= AUXFLAG_APP_DOMAIN_AGILE;
    }
    // This flag is set in a checked build for classes whose
    // instances may be marked app domain agile, but agility
    // isn't guaranteed by type safety.  The JIT will compile
    // in extra checks to field assignment on some fields
    // in such a class.
    inline BOOL IsCheckAppDomainAgile()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_wAuxFlags & AUXFLAG_CHECK_APP_DOMAIN_AGILE);
    }

    inline void SetCheckAppDomainAgile()
    {
        LEAF_CONTRACT;
        m_wAuxFlags |= AUXFLAG_CHECK_APP_DOMAIN_AGILE;
    }

    // This flag is set in a checked build to indicate that the
    // appdomain agility for a class had been set. This is used
    // for debugging purposes to make sure that we don't allocate
    // an object before the agility is set.
    inline BOOL IsAppDomainAgilityDone()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return (m_wAuxFlags & AUXFLAG_APP_DOMAIN_AGILITY_DONE);
    }
    inline void SetAppDomainAgilityDone()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        m_wAuxFlags |= AUXFLAG_APP_DOMAIN_AGILITY_DONE;
    }
    //
    // This predicate checks whether or not the class is "naturally"
    // app domain agile - that is:
    //      (1) it is in the system domain
    //      (2) all the fields are app domain agile
    //      (3) it has no finalizer
    //
    // Or, this also returns true for a proxy type which is allowed
    // to have cross app domain refs.
    //
    inline BOOL IsTypesafeAppDomainAgile()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return IsAppDomainAgile() && !IsCheckAppDomainAgile();
    }
    //
    // This predictate tests whether any instances are allowed
    // to be app domain agile.
    //
    inline BOOL IsNeverAppDomainAgile()
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return !IsAppDomainAgile() && !IsCheckAppDomainAgile();
    }
    void SetAppDomainAgileAttribute();

#endif // defined(CHECK_APP_DOMAIN_LEAKS) || defined(_DEBUG)

#if CHECK_APP_DOMAIN_LEAKS
    void GetPredefinedAgility(Module *pModule, mdTypeDef td, BOOL *pfIsAgile, BOOL *pfIsCheckAgile);
#endif


    //
public:
     enum
    {
        VMFLAG_FIXED_UP                        = 0x00000002,

        VMFLAG_SPARSE_FOR_COMINTEROP           = 0x00000004,
        // unused                = 0x00000008, 
        VMFLAG_GENERICS_MASK                   = 0x00000030,
        VMFLAG_NONGENERIC                      = 0x00000000, // It's not generic
        VMFLAG_GENERIC_SHAREDINST              = 0x00000010, // An instantiated generic shared by mutliple MethodTables
        VMFLAG_GENERIC_UNSHAREDINST            = 0x00000020, // An instantiated generic not shared by mutliple MethodTables type
        VMFLAG_GENERIC_TYPICALINST             = 0x00000030, // The typical instantiation (at formal type parameters)

        VMFLAG_HASLAYOUT                       = 0x00000040,
        VMFLAG_ISNESTED                        = 0x00000080,
        VMFLAG_CANNOT_BE_BLITTED_BY_OBJECT_CLONER = 0x00000100,  // This class has GC type fields, or implements ISerializable or has non-Serializable fields
        VMFLAG_CONTEXTFUL                      = 0x00000200,
        VMFLAG_DESTROYED                       = 0x00000400, // The Destruct() method has already been called on this class
        // unused                              = 0x00000800,
        // unused                              = 0x00001000,
        VMFLAG_ENUMTYPE                        = 0x00002000,
        VMFLAG_TRUEPRIMITIVE                   = 0x00004000,

        // NONVERIFIABLYOVERLAYEDFIELDS covers a smaller case than OVERLAYED fields.
        //   OVERLAYED is used to detect whether Equals can safely optimize to a bit-compare across the structure.
        //   NONVERIFIABLYOVERLAYEDFIELDS is used to detect whether a field access to this class is verifiable.
        //
        VMFLAG_HASOVERLAYEDFIELDS              = 0x00008000,
        VMFLAG_HASNONVERIFIABLYOVERLAYEDFIELDS = 0x00010000,
        


    // Set this if this class or its parent have instance fields which
    // must be explicitly inited in a constructor (e.g. pointers of any
    // kind, gc or native).
    //
    // Currently this is used by the verifier when verifying value classes
    // - it's ok to use uninitialised value classes if there are no
    // pointer fields in them.

        VMFLAG_HAS_FIELDS_WHICH_MUST_BE_INITED = 0x00080000,

        VMFLAG_NOSUPPRESSUNMGDCODEACCESS       = 0x00100000,
        // Unused                              = 0x00200000,
        VMFLAG_UNSAFEVALUETYPE                 = 0x00400000,
        VMFLAG_VALUETYPE                       = 0x00800000,
        VMFLAG_NO_GUID                         = 0x01000000,
        VMFLAG_HASNONPUBLICFIELDS              = 0x02000000,
        VMFLAG_REMOTING_PROXY_ATTRIBUTE        = 0x04000000,
        VMFLAG_CONTAINS_STACK_PTR              = 0x08000000,
        //Unused                               = 0x10000000,
        VMFLAG_ISMULTIDELEGATE                 = 0x20000000,
        VMFLAG_PREFER_ALIGN8                   = 0x40000000, // Would like to have 8-byte alignment
        VMFLAG_METHODS_REQUIRE_INHERITANCE_CHECKS = 0x80000000,
     };

public: 
    // C_ASSERTs in Jitinterface.cpp need this to be public to check the offset.
    // Put it first so the offset rarely changes, which just reduces the number of times we have to fiddle
    // with the offset.
    GuidInfo*   m_pGuidInfo;            // The cached guid inforation for interfaces.

#ifdef _DEBUG
public:
    LPCUTF8 m_szDebugClassName;
    BOOL m_fDebuggingClass;
#endif 

private: 
    PTR_Module m_pModule;
    mdTypeDef m_cl;
    PTR_MethodTable m_pMethodTable;

    // NOTE: Place items that are WORD sized or smaller together, otherwise padding will be used implicitly by the C++ compiler
    WORD m_wCCtorSlot;
    WORD m_wDefaultCtorSlot;
    BYTE m_NormType;
    WORD m_wNumInstanceFields;
    WORD m_wNumStaticFields;
    WORD m_wNumHandleStatics; 
    WORD m_wNumBoxedStatics; 
    WORD m_wNumGCPointerSeries;

    DWORD m_cbModuleDynamicID;
    DWORD m_cbNonGCStaticFieldBytes;
    DWORD m_dwNumInstanceFieldBytes;
#ifndef DACCESS_COMPILE
    FieldDesc *m_pFieldDescList;
#else // DACCESS_COMPILE
    FieldDesc* m_pFieldDescList_UseAccessor;
#endif // DACCESS_COMPILE
    DWORD m_dwAttrClass;
    volatile DWORD m_VMFlags;
    SecurityProperties m_SecProps;
        
    PTR_MethodDescChunk m_pChunks;

    BitMask m_classDependencies;

    DWORD m_dwReliabilityContract;

private:
    union
    {
        // valid only if EEClass::IsBlittable() or EEClass::HasLayout() is true
        UINT32          m_cbNativeSize; // size of fixed portion in bytes

    };

    //
    // GENERICS RELATED FIELDS.  The (VMFLAGS) in parens indicate the conditions under
    // which the fields are valid.
    //

    // If IsSharedByGenericInstantiations():
    //     Layout of handle dictionary for generic type (the last dictionary pointed to from PerInstInfo)
    // Otherwise: unused/invalid
    PTR_DictionaryLayout m_pDictLayout;  // (VMFLAG_GENERIC_SHAREDINST)

    // Variance info for each type parameter (gpNonVariant, gpCovariant, or gpContravariant)
    // If NULL, this type has no type parameters that are co/contravariant
    BYTE* m_pVarianceInfo;

    /*
     * We maintain some auxillary flags in DEBUG or CHECK_APP_DOMAIN_LEAKS builds,
     * this frees up some bits in m_wVMFlags
     */
#if defined(CHECK_APP_DOMAIN_LEAKS) || defined(_DEBUG)
    WORD m_wAuxFlags;
#endif


    //-------------------------------------------------------------
    // END CONCRETE DATA LAYOUT
    //-------------------------------------------------------------



    /************************************
     *  PROTECTED METHODS
     ************************************/
protected:
#ifndef DACCESS_COMPILE
    /*
     * Constructor: prevent any other class from doing a new()
     */
    EEClass(Module *pModule, DWORD genericsFlags);

    /*
     * Destructor: prevent any other class from deleting
     */
    ~EEClass()
    {
        LEAF_CONTRACT;
    }
#endif // !DACCESS_COMPILE

};


// MethodTableBuilder simply acts as a holder for the 
// large algorithm that "compiles" a type into
// a MethodTable/EEClass/DispatchMap/VTable etc. etc. 
// 
// The user of this class (the ClassLoader) currently builds the EEClass 
// first, and does a couple of other things too, though all
// that work should probably be folded into BuildMethodTableThrowing.
//
class MethodTableBuilder
{

public:

    friend class EEClass;

    // Information gathered by the class loader relating to generics
    // Fields in this structure are initialized very early in class loading
    // See ClassLoader::CreateTypeHandleForTypeDefThrowing
    struct bmtGenericsInfo {
        SigTypeContext typeContext;     // Type context used for metadata parsing
        WORD numDicts;                  // Number of dictionaries including this class
        BYTE *pVarianceInfo;            // Variance annotations on type parameters, NULL if none specified
        BOOL fContainsGenericVariables; // TRUE if this is an open type
        DWORD genericsKind;             // VMFLAG_GENERICS_MASK flags
        
        inline bmtGenericsInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
        inline DWORD GetNumGenericArgs() const { LEAF_CONTRACT; return typeContext.m_classInstCount; }
        inline BOOL HasInstantiation() const { LEAF_CONTRACT; return typeContext.m_classInstCount != 0; }
        inline TypeHandle* GetInstantiation() const { LEAF_CONTRACT; return typeContext.m_classInst; }
    };


    // information for Thread and Context Static. Filled by InitializedFieldDesc and used when
    // setting up a MethodTable
    struct bmtThreadContextStaticInfo
    {
        // size of thread statics
        DWORD dwThreadStaticsSize;
        // size of context statics
        DWORD dwContextStaticsSize;
    
        inline bmtThreadContextStaticInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    MethodTableBuilder(EEClass *pHalfBakedClass) 
    {
        LEAF_CONTRACT;
        m_pHalfBakedClass = pHalfBakedClass;
        NullBMTData();
    }
public:
    //==========================================================================
    // This function is very specific about how it constructs a EEClass.
    //==========================================================================
    static void CreateClass(BaseDomain *pDomain,
                            Module *pModule,
                            mdTypeDef cl,
                            BOOL fHasLayout,
                            BOOL fDelegate,
                            BOOL fIsEnum,
                            const bmtGenericsInfo *bmtGenericsInfo,
                            EEClass **ppEEClass,
                            size_t *pdwAllocRequestSize,
                            AllocMemTracker *pamTracker);
    
    static void CreateMinimalClass(LoaderHeap *pHeap,
                                    Module* pModule,
                                    AllocMemTracker *pamTracker,
                                    SIZE_T cbExtra,
                                    EEClass** ppEEClass);
    

    static void GatherGenericsInfo(Module *pModule, mdTypeDef cl, TypeHandle *genericArgs, bmtGenericsInfo *bmtGenericsInfo);

    VOID BuildMethodTableThrowing(BaseDomain *bmtDomain,
                                  Module *pLoaderModule,
                                  Module *pModule,
                                  mdToken cl,
                                  BuildingInterfaceInfo_t *pBuildingInterfaceList,
                                  const LayoutRawFieldInfo *pLayoutRawFieldInfos,
                                  MethodTable *pParentMethodTable,
                                  const bmtGenericsInfo *bmtGenericsInfo,
                                  PCCOR_SIGNATURE parentInst,
                                  WORD wNumInterfaces,
                                  AllocMemTracker *pamTracker);


    void GetPathForErrorMessages(SString & result);

private:
    enum
    {
        METHOD_IMPL_NOT,
        METHOD_IMPL,
        METHOD_IMPL_COUNT
    };

    enum
    {
        METHOD_TYPE_NORMAL,
        METHOD_TYPE_FCALL,
        METHOD_TYPE_EEIMPL,
        METHOD_TYPE_NDIRECT,
        METHOD_TYPE_INSTANTIATED,
        METHOD_TYPE_COUNT
    };

private:
    // <NICE> Get rid of this.</NICE>
    EEClass *m_pHalfBakedClass;

    // GetHalfBakedClass: The EEClass you get back from this function may not have all its fields filled in yet.
    // Thus you have to make sure that the relevant item which you are accessing has
    // been correctly initialized in the EEClass/MethodTable construction sequence
    // at the point at which you access it.  
    //
    // Gradually we will move the code to a model where the process of constructing an EEClass/MethodTable
    // is more obviously correct, e.g. by relying much less on reading information using GetHalfBakedClass
    // and GetHalfBakedMethodTable.
    //
    // <NICE> Get rid of this.</NICE>
    EEClass *GetHalfBakedClass() { LEAF_CONTRACT; return m_pHalfBakedClass; }

    // <NOTE> The following functions are used during MethodTable construction to access/set information about the type being constructed.
    // Beware that some of the fields of the underlying EEClass/MethodTable being constructed may not
    // be initialized.  Becauase of this, ideally the code will gradually be cleaned up so that
    // none of these functions are used and instead we use the data in the bmt structures below
    // or we explicitly pass around the data as arguments. </NOTE>
    //
    // <NICE> Get rid of all of these.</NICE>
    mdTypeDef GetCl()    { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetCl(); }
    BOOL IsGlobalClass() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsGlobalClass(); }
    WORD GetNumIntroducedInstanceFields() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetNumIntroducedInstanceFields(); }
    DWORD GetNumInstanceFieldBytes() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetNumInstanceFieldBytes(); }
    DWORD GetAttrClass() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetAttrClass(); }
    WORD GetNumHandleStatics() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetNumHandleStatics(); }
    WORD GetNumStaticFields() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetNumStaticFields(); }
    WORD GetNumInstanceFields() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetNumInstanceFields(); }
    BOOL IsInterface() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsInterface(); } 
    BOOL HasOverLayedField() { WRAPPER_CONTRACT; return GetHalfBakedClass()->HasOverLayedField(); } 
    BOOL IsEnum() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsEnum(); } 
    BOOL IsValueClass() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsValueClass(); } 
    BOOL IsUnsafeValueClass() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsUnsafeValueClass(); }
    BOOL IsAbstract() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsAbstract(); } 
    BOOL HasLayout() { WRAPPER_CONTRACT; return GetHalfBakedClass()->HasLayout(); } 
    BOOL IsSharedByGenericInstantiations() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsSharedByGenericInstantiations(); } 
    BOOL IsTypicalTypeDefinition() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsTypicalTypeDefinition(); } 
    BOOL IsAnyDelegateClass() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsAnyDelegateClass(); } 
    BOOL IsContextful() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsContextful(); } 
    BOOL IsNested() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsNested(); } 
    BOOL HasRemotingProxyAttribute() { WRAPPER_CONTRACT; return GetHalfBakedClass()->HasRemotingProxyAttribute(); } 
    BOOL IsBlittable() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsBlittable(); } 
    PTR_MethodDescChunk GetChunks() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetChunks(); } 
    BOOL HasExplicitFieldOffsetLayout() { WRAPPER_CONTRACT; return GetHalfBakedClass()->HasExplicitFieldOffsetLayout(); } 
    CorGenericParamAttr GetVarianceOfTypeParameter(DWORD i) { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetVarianceOfTypeParameter(i); } 
    BOOL IsManagedSequential() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsManagedSequential(); } 
    BOOL RequiresLinktimeCheck() { WRAPPER_CONTRACT; return GetHalfBakedClass()->RequiresLinktimeCheck(); } 
    WORD GetClassConstructorSlot() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetClassConstructorSlot(); } 
    SecurityProperties* GetSecurityProperties() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetSecurityProperties(); } 
#ifdef _DEBUG
    BOOL IsAppDomainAgilityDone() { WRAPPER_CONTRACT; return GetHalfBakedClass()->IsAppDomainAgilityDone(); } 
    LPCUTF8 GetDebugClassName() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetDebugClassName(); } 
#endif // _DEBUG
    Assembly *GetAssembly() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetAssembly(); } 
    Module *GetModule() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetModule(); } 
    MethodTable *GetHalfBakedMethodTable() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetMethodTable(); } 
    BaseDomain *GetDomain() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetDomain(); } 
    ClassLoader *GetClassLoader() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetClassLoader(); } 
    IMDInternalImport* GetMDImport()  { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetMDImport(); } 
    EEClass* GetParentClass()  { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetParentClass(); } 
#ifndef DACCESS_COMPILE
    FieldDesc *GetApproxFieldDescListRaw() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetApproxFieldDescListRaw(); } 
#endif
    EEClassLayoutInfo *GetLayoutInfo() { WRAPPER_CONTRACT; return GetHalfBakedClass()->GetLayoutInfo(); } 

    // <NOTE> The following functions are used during MethodTable construction to setup information 
    // about the type being constructedm in particular information stored in the EEClass.
    // USE WITH CAUTION!!  TRY NOT TO ADD MORE OF THESE!! </NOTE>
    //
    // <NICE> Get rid of all of these - we should be able to evaluate these conditions BEFORE
    // we create the EEClass object, and thus set the flags immediately at the point
    // we create that object.</NICE>
    void SetValueClass() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetValueClass(); } 
    void SetUnsafeValueClass() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetUnsafeValueClass(); } 
    void SetCannotBeBlittedByObjectCloner() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetCannotBeBlittedByObjectCloner(); } 
    void SetHasFieldsWhichMustBeInited() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetHasFieldsWhichMustBeInited(); } 
    void SetHasNonPublicFields() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetHasNonPublicFields(); } 
    void SetEnum() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetEnum(); } 
    void SetModuleDynamicID(DWORD x) { WRAPPER_CONTRACT; GetHalfBakedClass()->SetModuleDynamicID(x); } 
    void SetNumGCPointerSeries(WORD x) { WRAPPER_CONTRACT; GetHalfBakedClass()->SetNumGCPointerSeries(x); } 
    void SetNumHandleStatics(WORD x) { WRAPPER_CONTRACT; GetHalfBakedClass()->SetNumHandleStatics(x); } 
    void SetNumBoxedStatics(WORD x) { WRAPPER_CONTRACT; GetHalfBakedClass()->SetNumBoxedStatics(x); } 
    void SetNumInstanceFieldBytes(DWORD x) { WRAPPER_CONTRACT; GetHalfBakedClass()->SetNumInstanceFieldBytes(x); } 
#if defined(CHECK_APP_DOMAIN_LEAKS) || defined(_DEBUG)
    void SetAppDomainAgileAttribute(){ WRAPPER_CONTRACT; GetHalfBakedClass()->SetAppDomainAgileAttribute(); } 
#endif
    void SetAlign8Candidate() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetAlign8Candidate(); } 
    void SetHasRemotingProxyAttribute() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetHasRemotingProxyAttribute(); } 
    void SetContextfull() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetContextfull(); } 
    void SetHasOverLayedFields() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetHasOverLayedFields(); } 
    void SetHasNonVerifiablyOverLayedFields() { WRAPPER_CONTRACT; GetHalfBakedClass()->SetHasNonVerifiablyOverLayedFields(); } 
    void SetNonGCStaticFieldBytes(DWORD x) { WRAPPER_CONTRACT; GetHalfBakedClass()->SetNonGCStaticFieldBytes(x); } 
#ifdef _DEBUG
    void SetDebugClassName(LPUTF8 x) { WRAPPER_CONTRACT; GetHalfBakedClass()->SetDebugClassName(x); } 
#endif

    /************************************
     *  PRIVATE INTERNAL STRUCTS
     ************************************/
private:
    //The following structs are used in buildmethodtable
    // The 'bmt' in front of each struct reminds us these are for MethodTableBuilder

    // for each 64K token range, stores the number of methods found within that token range,
    // the current methoddescchunk being filled in and the next available index within
    // that chunk. Note that we'll very rarely generate a TokenRangeNode for any range
    // other than 0..64K range.
    struct bmtTokenRangeNode
    {
        BYTE    tokenHiByte;
        DWORD   cMethods;
        DWORD   dwCurrentChunk;
        DWORD   dwCurrentIndex;

        bmtTokenRangeNode *pNext;
    };

    struct bmtErrorInfo
    {
        UINT resIDWhy;
        LPCUTF8 szMethodNameForError;
        mdToken dMethodDefInError;
        Module* pModule;
        mdTypeDef cl;
        OBJECTREF *pThrowable;

        // Set the reason and the offending method def. If the method information
        // is not from this class set the method name and it will override the method def.
        inline bmtErrorInfo() : resIDWhy(0), szMethodNameForError(NULL), dMethodDefInError(mdMethodDefNil), pThrowable(NULL) {LEAF_CONTRACT; }
    };

    struct bmtProperties
    {
        BOOL fNoSanityChecks;
        BOOL fSparse;                           // Set to true if a sparse interface is being used.
        BOOL fMarshaledByRef;


        BOOL fDynamicStatics;                   // Set to true if the statics will be allocated in the dynamic
        BOOL fGenericsStatics;                   // Set to true if the there are per-instantiation statics


        inline bmtProperties() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    struct bmtVtable
    {
        DWORD dwCurrentVtableSlot;
        DWORD dwCurrentNonVtableSlot;
        DWORD dwNonGCStaticFieldBytes;

        // Temporary vtable - use GetMethodDescForSlot/SetMethodDescForSlot for access.
        // pVtableMD is initialized lazily from pVtable
        // pVtable is invalidated if the slot is overwritten.
        SLOT* pVtable;
        MethodDesc** pVtableMD;
        MethodTable *pParentMethodTable;

        MethodDesc** pNonVtableMD;
        InteropMethodTableSlotData **ppSDVtable;
        InteropMethodTableSlotData **ppSDNonVtable;
        DWORD dwMaxVtableSize;                  // Upper bound on size of vtable
        WORD  wDefaultCtorSlot;
        WORD  wCCtorSlot;
        InteropMethodTableSlotDataMap *pInteropData;

        DispatchMapBuilder *pDispatchMapBuilder;

        MethodDesc* GetMethodDescForSlot(DWORD slot)
        {
            WRAPPER_CONTRACT;
            if (pVtable[slot] != NULL && pVtableMD[slot] == NULL)
                pVtableMD[slot] = pParentMethodTable->GetMethodDescForSlot(slot);
            _ASSERTE((pVtable[slot] == NULL) ||
                (MethodTable::GetUnknownMethodDescForSlotAddress(pVtable[slot]) == pVtableMD[slot]));
            return pVtableMD[slot];
        }

        void SetMethodDescForSlot(DWORD slot, MethodDesc* pMD)
        {
            WRAPPER_CONTRACT;
            pVtable[slot] = NULL;
            pVtableMD[slot] = pMD;
        }

        inline bmtVtable() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    struct bmtParentInfo
    {
        DWORD dwNumParentInterfaces;
        MethodDesc **ppParentMethodDescBuf;     // Cache for declared methods
        MethodDesc **ppParentMethodDescBufPtr;  // Pointer for iterating over the cache

        WORD NumParentPointerSeries;
        MethodNameHash *pParentMethodHash;
        Substitution parentSubst;
        MethodTable *pParentMethodTable;
        mdToken token;

        inline bmtParentInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    struct bmtInterfaceInfo
    {
        DWORD dwTotalNewInterfaceMethods;
        InterfaceInfo_t *pInterfaceMap;         // Temporary interface map

        // ppInterfaceSubstitutionChains[i][0] holds the primary substitution for each interface
        // ppInterfaceSubstitutionChains[i][0..depth[i] ] is the chain of substitutions for each interface
        Substitution **ppInterfaceSubstitutionChains;        
           
        DWORD *pdwOriginalStart;                // If an interface is moved this is the original starting location.
        DWORD dwInterfaceMapSize;               // # members in interface map
        DWORD dwLargestInterfaceSize;           // # members in largest interface we implement
        DWORD dwMaxExpandedInterfaces;          // Upper bound on size of interface map
        MethodDesc **ppInterfaceMethodDescList; // List of MethodDescs for current interface
        MethodDesc **ppInterfaceDeclMethodDescList; // List of MethodDescs for the interface itself

        MethodDesc ***pppInterfaceImplementingMD; // List of MethodDescs that implement interface methods
        MethodDesc ***pppInterfaceDeclaringMD;    // List of MethodDescs from the interface itself

        inline bmtInterfaceInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    struct bmtEnumMethAndFields
    {
        DWORD dwNumStaticFields;
        DWORD dwNumInstanceFields;
        DWORD dwNumStaticObjRefFields;
        DWORD dwNumStaticBoxedFields;
        DWORD dwNumDeclaredFields;           // For calculating amount of FieldDesc's to allocate
        DWORD dwNumDeclaredNonAbstractMethods;          // For calculating approx generic dictionary size
        DWORD dwNumILInstanceMethods;        // Used as a heuristic for size of type slots table
        DWORD dwNumUnboxingMethods;

        HENUMInternal hEnumField;
        HENUMInternal hEnumMethod;
        BOOL fNeedToCloseEnumField;
        BOOL fNeedToCloseEnumMethod;

        DWORD dwNumberMethodImpls;              // Number of method impls defined for this type
        HENUMInternal hEnumDecl;                // Method Impl's contain a declaration
        HENUMInternal hEnumBody;                //  and a body.
        BOOL fNeedToCloseEnumMethodImpl;        //

        IMDInternalImport *m_pInternalImport;

        inline bmtEnumMethAndFields(IMDInternalImport *pInternalImport) { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); m_pInternalImport = pInternalImport; }
        inline ~bmtEnumMethAndFields()
        {
            LEAF_CONTRACT;
            if (fNeedToCloseEnumField)
                (m_pInternalImport)->EnumClose(&hEnumField);

            if (fNeedToCloseEnumMethod)
                (m_pInternalImport)->EnumClose(&hEnumMethod);

            if (fNeedToCloseEnumMethodImpl)
                (m_pInternalImport)->EnumMethodImplClose(&hEnumBody,
                                                         &hEnumDecl);
        }
    };

    struct bmtMetaDataInfo
    {
        DWORD    cMethods;                  // # meta-data methods of this class
        DWORD    cMethAndGaps;              // # meta-data methods of this class ( including the gaps )
        DWORD    cFields;                   // # meta-data fields of this class
        mdToken *pFields;                   // Enumeration of metadata fields
        mdToken *pMethods;                  // Enumeration of metadata methods
        DWORD   *pFieldAttrs;               // Enumeration of the attributes of the fields
        DWORD   *pMethodAttrs;              // Enumeration of the attributes of the methods
        DWORD   *pMethodImplFlags;          // Enumeration of the method implementation flags
        ULONG   *pMethodRVA;                // Enumeration of the method RVA's
        DWORD   *pMethodClassifications;    // Enumeration of the method classifications
        LPCSTR  *pstrMethodName;            // Enumeration of the method names
        BYTE    *pMethodImpl;               // Enumeration of impl value
        BYTE    *pMethodType;               // Enumeration of type value

        bmtTokenRangeNode *ranges[METHOD_TYPE_COUNT][METHOD_IMPL_COUNT]; //linked list of token ranges that contain at least one method

        struct MethodImplTokenPair
        {
            mdToken methodBody;             // MethodDef's for the bodies of MethodImpls. Must be defined in this type.
            mdToken methodDecl;             // Method token that body implements. Is a MethodDef or MemberRef
            static int __cdecl Compare(const void *elem1, const void *elem2);
            static BOOL Equal(const MethodImplTokenPair *elem1, const MethodImplTokenPair *elem2);
        };

        MethodImplTokenPair *rgMethodImplTokens;
        Substitution *pMethodDeclSubsts;    // Used to interpret generic variables in the interface of the declaring type

        inline bmtMetaDataInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }

        inline void SetMethodData(int idx,
            mdToken tok,
            DWORD dwAttrs,
            DWORD dwRVA,
            DWORD dwImplFlags,
            DWORD classification,
            LPCSTR szMethodName,
            BYTE  impl,
            BYTE  type)
        {
            pMethods[idx] = tok;
            pMethodAttrs[idx] = dwAttrs;
            pMethodRVA[idx] = dwRVA;
            pMethodImplFlags[idx] = dwImplFlags;
            pMethodClassifications[idx] = classification;
            pstrMethodName[idx] = szMethodName;
            pMethodImpl[idx] = impl;
            pMethodType[idx] = type;
        }
    };

    struct bmtMethodDescSet
    {
        DWORD dwNumMethodDescs;         // # MD's
        DWORD dwNumBoxedEntryPointMDs; // # Unboxing MD's
        DWORD dwChunks;                 // # chunks to allocate
        MethodDescChunk **pChunkList;    // Array of pointers to chunks
    };

    struct bmtMethAndFieldDescs
    {
        MethodDesc **ppUnboxMethodDescList; // Keep track unboxed entry points (for value classes)
        MethodDesc **ppMethodDescList;      // MethodDesc pointer for each member
        FieldDesc **ppFieldDescList;        // FieldDesc pointer (or NULL if field not preserved) for each field

        bmtMethodDescSet sets[METHOD_TYPE_COUNT][METHOD_IMPL_COUNT];

        MethodDesc *pBodyMethodDesc;        // The method desc for the body.

        // Tracking info for VTS (Version Tolerant Serialization)
        MethodDesc *pOnSerializingMethod;
        MethodDesc *pOnSerializedMethod;
        MethodDesc *pOnDeserializingMethod;
        MethodDesc *pOnDeserializedMethod;
        bool *prfNotSerializedFields;
        bool *prfOptionallySerializedFields;
        bool fNeedsRemotingVtsInfo;

        inline void SetFieldNotSerialized(DWORD dwIndex, DWORD dwNumInstanceFields)
        {
            WRAPPER_CONTRACT;
            if (prfNotSerializedFields == NULL)
            {
                DWORD cbSize = sizeof(bool) * dwNumInstanceFields;
                prfNotSerializedFields = (bool*)GetThread()->m_MarshalAlloc.Alloc(sizeof(bool) * dwNumInstanceFields);
                ZeroMemory(prfNotSerializedFields, cbSize);
            }
            prfNotSerializedFields[dwIndex] = true;
            fNeedsRemotingVtsInfo = true;
        }

        inline void SetFieldOptionallySerialized(DWORD dwIndex, DWORD dwNumInstanceFields)
        {
            WRAPPER_CONTRACT;
            if (prfOptionallySerializedFields == NULL)
            {
                DWORD cbSize = sizeof(bool) * dwNumInstanceFields;
                prfOptionallySerializedFields = (bool*)GetThread()->m_MarshalAlloc.Alloc(sizeof(bool) * dwNumInstanceFields);
                ZeroMemory(prfOptionallySerializedFields, cbSize);
            }
            prfOptionallySerializedFields[dwIndex] = true;
            fNeedsRemotingVtsInfo = true;
        }

        inline bmtMethAndFieldDescs() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    struct bmtFieldPlacement
    {
        // For compacting field placement
        DWORD StaticFieldStart[MAX_LOG2_PRIMITIVE_FIELD_SIZE+1];            // Byte offset where to start placing fields of this size
        DWORD InstanceFieldStart[MAX_LOG2_PRIMITIVE_FIELD_SIZE+1];
        DWORD NumStaticFieldsOfSize[MAX_LOG2_PRIMITIVE_FIELD_SIZE+1];       // # Fields of this size

        DWORD NumInstanceFieldsOfSize[MAX_LOG2_PRIMITIVE_FIELD_SIZE+1];
        DWORD FirstInstanceFieldOfSize[MAX_LOG2_PRIMITIVE_FIELD_SIZE+1];
        DWORD GCPointerFieldStart;
        DWORD NumInstanceGCPointerFields;   // does not include inherited pointer fields
        DWORD NumStaticGCPointerFields;   // does not include inherited pointer fields
        DWORD NumStaticGCBoxedFields;   // does not include inherited pointer fields
        bool  fHasFixedAddressValueTypes;

        inline bmtFieldPlacement() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    struct bmtInternalInfo
    {
        IMDInternalImport *pInternalImport;
        Module *pModule;
        mdToken cl;

        inline bmtInternalInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };


    enum bmtFieldLayoutTag {empty, nonoref, oref};

    // used for calculating pointer series for tdexplicit
    struct bmtGCSeriesInfo
    {
        UINT numSeries;
        struct Series {
            UINT offset;
            UINT len;
        } *pSeries;
        bmtGCSeriesInfo() : numSeries(0), pSeries(NULL) {LEAF_CONTRACT;}
    };

    struct bmtMethodImplInfo
    {
        DWORD        pIndex;     // Next open spot in array, we load the BodyDesc's up in order of appearance in the
                                 // type's list of methods (a body can appear more then once in the list of MethodImpls)
        struct Entry
        {
            mdToken      declToken;  // Either the token or the method desc is set for the declaration
            Substitution declSubst;  // Signature instantiations of parent types for Declaration (NULL if not instantiated)
            MethodDesc*  pDeclDesc;  // Method descs for Declaration. If null then Declaration is in this type and use the token
            MethodDesc*  pBodyDesc;  // Method descs created for Method impl bodies
            DWORD        dwFlags;
        };

        Entry *rgEntries;

        void AddMethod(MethodDesc* pImplDesc, MethodDesc* pDeclDesc, mdToken mdDecl, Substitution *pDeclSubst);

        MethodDesc* GetDeclarationMethodDesc(DWORD i)
        {
            LEAF_CONTRACT;
            _ASSERTE(i < pIndex);
            return rgEntries[i].pDeclDesc;
        }

        mdToken GetDeclarationToken(DWORD i)
        {
            LEAF_CONTRACT;
            _ASSERTE(i < pIndex);
            return rgEntries[i].declToken;
        }

        const Substitution *GetDeclarationSubst(DWORD i)
        {
            LEAF_CONTRACT;

            _ASSERTE(i < pIndex);
            return &rgEntries[i].declSubst;
        }

        MethodDesc* GetBodyMethodDesc(DWORD i)
        {
            LEAF_CONTRACT;
            _ASSERTE(i < pIndex);
            return rgEntries[i].pBodyDesc;
        }
        inline bmtMethodImplInfo() { LEAF_CONTRACT; memset((void*) this, NULL, sizeof(*this)); }

        // Returns TRUE if tok acts as a body for any methodImpl entry. FALSE, otherwise.
        BOOL IsBody(mdToken tok);
    };

    // The following structs, defined as private members of MethodTableBuilder, contain the necessary local
    // parameters needed for BuildMethodTable

    // Look at the struct definitions for a detailed list of all parameters available
    // to BuildMethodTable.

    BaseDomain *bmtDomain;
    bmtErrorInfo *bmtError;
    bmtProperties *bmtProp;
    bmtVtable *bmtVT;
    bmtParentInfo *bmtParent;
    bmtInterfaceInfo *bmtInterface;
    bmtMetaDataInfo *bmtMetaData;
    bmtMethAndFieldDescs *bmtMFDescs;
    bmtFieldPlacement *bmtFP;
    bmtInternalInfo *bmtInternal;
    bmtGCSeriesInfo *bmtGCSeries;
    bmtMethodImplInfo *bmtMethodImpl;
    const bmtGenericsInfo *bmtGenerics;
    bmtEnumMethAndFields *bmtEnumMF;
    bmtThreadContextStaticInfo *bmtTCSInfo;

    void SetBMTData(
        BaseDomain *bmtDomain,
        bmtErrorInfo *bmtError,
        bmtProperties *bmtProp,
        bmtVtable *bmtVT,
        bmtParentInfo *bmtParent,
        bmtInterfaceInfo *bmtInterface,
        bmtMetaDataInfo *bmtMetaData,
        bmtMethAndFieldDescs *bmtMFDescs,
        bmtFieldPlacement *bmtFP,
        bmtInternalInfo *bmtInternal,
        bmtGCSeriesInfo *bmtGCSeries,
        bmtMethodImplInfo *bmtMethodImpl,
        const bmtGenericsInfo *bmtGenerics,
        bmtEnumMethAndFields *bmtEnumMF,
        bmtThreadContextStaticInfo *bmtTCSInfo);

    void NullBMTData();

    class DeclaredMethodIterator
    {
      private:
        MethodTableBuilder &m_mtb;
        int                 m_idx;

      public:
        inline                  DeclaredMethodIterator(MethodTableBuilder &mtb);
        inline int              CurrentIndex();
        inline BOOL             Next();
        inline mdToken          Token();
        inline DWORD            Attrs();
        inline DWORD            RVA();
        inline DWORD            ImplFlags();
        inline DWORD            Classification();
        inline LPCSTR           Name();
        inline PCCOR_SIGNATURE  GetSig(DWORD *pcbSig);
        inline BYTE             MethodImpl();
        inline BOOL             IsMethodImpl();
        inline BYTE             MethodType();
        inline MethodDesc      *GetMethodDesc();
        inline void             SetMethodDesc(MethodDesc *pMD);
        inline MethodDesc      *GetParentMethodDesc();
        inline void             SetParentMethodDesc(MethodDesc *pMD);
        inline MethodDesc      *GetUnboxedMethodDesc();
    };
    friend class DeclaredMethodIterator;

    inline DWORD NumDeclaredMethods() { LEAF_CONTRACT; return bmtMetaData->cMethods; }
    inline void  IncNumDeclaredMethods() { LEAF_CONTRACT; bmtMetaData->cMethods++; }
    inline DWORD NumDeclaredFields() { LEAF_CONTRACT; return bmtEnumMF->dwNumDeclaredFields; }

public:
    static VOID MarkInheritedVirtualMethods(MethodTable *childMT, MethodTable * parentMT);

private:
    static bmtTokenRangeNode *GetTokenRange(mdToken tok, bmtTokenRangeNode **ppHead);

    static VOID DECLSPEC_NORETURN BuildMethodTableThrowException(HRESULT hr,
                                              const bmtErrorInfo & bmtError);


    inline VOID DECLSPEC_NORETURN BuildMethodTableThrowException(
                                              HRESULT hr,
                                              UINT idResWhy,
                                              mdMethodDef tokMethodDef)
    {
        WRAPPER_CONTRACT;
        bmtError->resIDWhy = idResWhy;
        bmtError->dMethodDefInError = tokMethodDef;
        bmtError->szMethodNameForError = NULL;
        bmtError->cl = GetCl();
        BuildMethodTableThrowException(hr, *bmtError);
    }

    inline VOID DECLSPEC_NORETURN BuildMethodTableThrowException(
        HRESULT hr,
        UINT idResWhy,
        LPCUTF8 szMethodName)
    {
        WRAPPER_CONTRACT;
        bmtError->resIDWhy = idResWhy;
        bmtError->dMethodDefInError = mdMethodDefNil;
        bmtError->szMethodNameForError = szMethodName;
        bmtError->cl = GetCl();
        BuildMethodTableThrowException(hr, *bmtError);
    }

    inline VOID DECLSPEC_NORETURN BuildMethodTableThrowException(
                                              UINT idResWhy,
                                              mdMethodDef tokMethodDef = mdMethodDefNil)
    {
        WRAPPER_CONTRACT;
        BuildMethodTableThrowException(COR_E_TYPELOAD, idResWhy, tokMethodDef);
    }

    inline VOID DECLSPEC_NORETURN BuildMethodTableThrowException(
        UINT idResWhy,
        LPCUTF8 szMethodName)
    {
        WRAPPER_CONTRACT;
        BuildMethodTableThrowException(COR_E_TYPELOAD, idResWhy, szMethodName);
    }

private:
    MethodNameHash *CreateMethodChainHash(
        MethodTable *pMT);

    // Only used in the resolve phase of the classloader
    static void ExpandApproxInterface(
        bmtInterfaceInfo *      bmtInterface,  // out parameter, various parts cumulatively written to.
        const Substitution *    pNewInterfaceSubstChain, 
        MethodTable *           pNewInterface,
        WORD                    flags);

    static void ExpandApproxDeclaredInterfaces(
        bmtInterfaceInfo *      bmtInterface, // out parameter, various parts cumulatively written to.
        Module *                pModule, 
        mdToken                 typeDef,  
        const Substitution *    pSubstChain,  
        WORD                    flags);

    static void ExpandApproxInherited(
        bmtInterfaceInfo *      bmtInterface, // out parameter, various parts cumulatively written to.
        MethodTable *           pApproxParentMT,  
        const Substitution *    pSubstChain);

    void LoadApproxInterfaceMap(
        BuildingInterfaceInfo_t *   pBuildingInterfaceList,
        MethodTable *               pApproxParentMT);

public:
    //------------------------------------------------------------------------
    // Loading exact interface instantiations.(slow technique)
    //
    // These place the exact interface instantiations into the interface map at the 
    // appropriate locations.

    struct bmtExactInterfaceInfo {
        DWORD nAssigned;
        MethodTable **pExactMTs;

        // ppInterfaceSubstitutionChains[i][0] holds the primary substitution for each interface
        // ppInterfaceSubstitutionChains[i][0..depth[i] ] is the chain of substitutions for each interface
        Substitution **ppInterfaceSubstitutionChains;        
        SigTypeContext typeContext;
           
        inline bmtExactInterfaceInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };
private:

    static void ExpandExactInterface(
        bmtExactInterfaceInfo *,
        const Substitution *    pSubstChain, 
        MethodTable *           pIntfMT);

public:
    static void ExpandExactDeclaredInterfaces(
        bmtExactInterfaceInfo *,
        Module *                pModule, 
        mdToken                 typeDef,  
        const Substitution *    pSubstChain);

    static void ExpandExactInheritedInterfaces(
        bmtExactInterfaceInfo *,
        MethodTable *           pParentMT, 
        const Substitution *    pSubstChain);

    //------------------------------------------------------------------------
    // Interface ambiguity checks when loading exact interface instantiations
    //
    // These implement the check that the exact instantiation does not introduce any
    // ambiguity in the interface dispatch logic, i.e. amongst the freshly declared interfaces.

public: 
    struct bmtInterfaceAmbiguityCheckInfo {
        MethodTable *pMT;
        DWORD nAssigned;
        MethodTable **ppExactDeclaredInterfaces;
        Substitution **ppInterfaceSubstitutionChains;        
        SigTypeContext typeContext;

        inline bmtInterfaceAmbiguityCheckInfo() { LEAF_CONTRACT; memset((void *)this, NULL, sizeof(*this)); }
    };

    static void InterfacesAmbiguityCheck(
        bmtInterfaceAmbiguityCheckInfo *,
        Module *pModule, 
        mdToken typeDef,  
        const Substitution *pSubstChain);

private:

    static void InterfaceAmbiguityCheck(
        bmtInterfaceAmbiguityCheckInfo *,
        const Substitution *pSubstChain, 
        MethodTable *pIntfMT);

public:
    static void LoadExactInterfaceMap(
        MethodTable *pMT);


    // This one is used at load time, using metadata-based comparisons
    DispatchMapTypeID ComputeDispatchMapTypeID(
        MethodTable *pDeclInftMT,
        const Substitution *pDeclIntfSubst);


private:
    HRESULT LoaderFindMethodInClass(
        LPCUTF8             pszMemberName,
        Module*             pModule,
        mdMethodDef         mdToken,
        MethodDesc **       ppMethodDesc,
        PCCOR_SIGNATURE *   ppMemberSignature,
        DWORD *             pcMemberSignature,
        DWORD               dwHashName,
        BOOL *              pMethodConstraintsMatch);

    //These functions are used by MethodTableBuilder
    VOID ResolveInterfaces(BuildingInterfaceInfo_t*);

    VOID ComputeModuleDependencies();

    // Finds a method declaration from a MemberRef or Def. It handles the case where
    // the Ref or Def point back to this class even though it has not been fully
    // laid out.
    HRESULT FindMethodDeclarationForMethodImpl(
        mdToken  pToken,       // Token that is being located (MemberRef or MemberDef)
        mdToken* pDeclaration, // Method definition for Member
        BOOL fSameClass,       // Does the declaration need to be in this class
        Module** pModule);     // Module that the Method Definitions is part of

    // Enumerates the method impl token pairs and resolves the impl tokens to mdtMethodDef
    // tokens, since we currently have the limitation that all impls are in the current class.
    VOID EnumerateMethodImpls();

    VOID EnumerateClassMembers();

    // Allocate temporary memory for tracking all information used in building the MethodTable
    VOID AllocateWorkingSlotTables();

    VOID AllocateMethodFieldDescs(AllocMemTracker *pamTracker);

    VOID InitializeFieldDescs(
        BaseDomain *bmtDomain,
        FieldDesc *,
        const LayoutRawFieldInfo*,
        bmtInternalInfo*,
        const bmtGenericsInfo*,
        bmtMetaDataInfo*,
        bmtEnumMethAndFields*,
        bmtErrorInfo*,
        EEClass***,
        bmtMethAndFieldDescs*,
        bmtFieldPlacement*,
        bmtThreadContextStaticInfo*,
        unsigned * totalDeclaredSize,
        bmtParentInfo* bmtParent = NULL);

    BOOL IsSelfReferencingStaticValueTypeField(
        mdToken                 dwByValueClassToken,
        bmtInternalInfo*        bmtInternal,
        const bmtGenericsInfo * bmtGenericsInfo,
        PCCOR_SIGNATURE         pMemberSignature,
        DWORD                   cMemberSignature);

    VOID SetSecurityFlagsOnMethod(
        MethodDesc*         pParentMethodDesc,
        MethodDesc*         pNewMD,
        mdToken             tokMethod,
        DWORD               dwMemberAttrs,
        bmtInternalInfo*    bmtInternal,
        bmtMetaDataInfo*    bmtMetaData);

    VOID PlaceMembers(
        DWORD                    numDeclaredInterfaces,
        BuildingInterfaceInfo_t *pBuildingInterfaceMap,
        AllocMemTracker *        pamTracker);

    VOID InitMethodDesc(
        BaseDomain *        bmtDomain,
        MethodDesc *        pNewMD,
        DWORD               Classification,
        mdToken             tok,
        DWORD               dwImplFlags,
        DWORD               dwMemberAttrs,
        BOOL                fEnC,
        DWORD               RVA,          // Only needed for NDirect case
        IMDInternalImport * pIMDII,  // Needed for NDirect, EEImpl(Delegate) cases
        LPCSTR              pMethodName, // Only needed for mcEEImpl (Delegate) case
#ifdef _DEBUG
        LPCUTF8             pszDebugMethodName,
        LPCUTF8             pszDebugClassName,
        LPUTF8              pszDebugMethodSignature,
#endif //_DEBUG
        AllocMemTracker *   pamTracker);

    // Throws if an entry already exists that has been MethodImpl'd
    VOID AddMethodImplDispatchMapping(
        DispatchMapTypeID   typeID,
        UINT32              slotNumber,
        MethodDesc*         pMDBody,
        BOOL                fIsVirtual);

    VOID MethodImplCompareSignatures(
        mdMethodDef         mdDecl,
        IMDInternalImport*  pImportDecl,
        Module*             pModuleDecl,
        const Substitution* pSubstDecl,
        mdMethodDef         mdImpl,
        IMDInternalImport*  pImportImpl,
        Module*             pModuleImpl,
        const Substitution* pSubstImpl,
        PCCOR_SIGNATURE*    ppImplSignature,
        DWORD*              pcImplSignature,
        DWORD               dwConstraintErrorCode);

    // This will validate that all interface methods that were matched during
    // layout also validate against type constraints.
    VOID    ValidateInterfaceMethodConstraints();

    VOID    PlaceMethodImpls(AllocMemTracker *pamTracker);

    HRESULT PlaceLocalDeclaration(
        mdMethodDef      mdef,
        MethodDesc*      body,
        DWORD*           slots,
        MethodDesc**     replaced,
        DWORD*           pSlotIndex,
        PCCOR_SIGNATURE* ppBodySignature,
        DWORD*           pcBodySignature);

    HRESULT PlaceInterfaceDeclaration(
        MethodDesc*       pDecl,
        MethodDesc*       body,
        const Substitution *pDeclSubst,
        DWORD*            slots,
        MethodDesc**      replaced,
        DWORD*            pSlotIndex,
        PCCOR_SIGNATURE*  ppBodySignature,
        DWORD*            pcBodySignature);

    HRESULT PlaceParentDeclaration(
        MethodDesc*       pDecl,
        MethodDesc*       body,
        const Substitution *pDeclSubst,
        DWORD*            slots,
        MethodDesc**      replaced,
        DWORD*            pSlotIndex,
        PCCOR_SIGNATURE*  ppBodySignature,
        DWORD*            pcBodySignature);

    VOID ChangeValueClassVirtualsToBoxedEntryPointsAndCreateUnboxedEntryPoints(
        AllocMemTracker *pamTracker);

    VOID PlaceVtableMethods(
        DWORD numDeclaredInterfaces,
        BuildingInterfaceInfo_t *);

    VOID PlaceStaticFields();

    VOID PlaceInstanceFields(EEClass**);

    BOOL CheckForVtsEventMethod(
        IMDInternalImport  *pImport,
        MethodDesc         *pMD,
        DWORD               dwAttrs,
        LPCUTF8             szAttrName,
        MethodDesc        **ppMethodDesc);

    VOID ScanTypeForVtsInfo();

    VOID SetupMethodTable2(
        AllocMemTracker *pamTracker,
        Module* pLoaderModule);

    VOID HandleGCForValueClasses(
        EEClass**);

    // These methods deal with inheritance security. They're executed
    // after the type has been constructed, but before it is published.
    VOID VerifyMethodInheritanceSecurityHelper(
        MethodDesc *pParentMD,
        MethodDesc *pChildMD);

    VOID VerifyClassInheritanceSecurityHelper(
        EEClass *pParentCls,
        EEClass *pChildCls);

    VOID VerifyInheritanceSecurity();

    VOID EnsureRIDMapsCanBeFilled();

    VOID CheckForRemotingProxyAttrib();


    VOID SetContextfulOrByRef();

    VOID HandleExplicitLayout(
        EEClass **pByValueClassCache);

    static ExplicitFieldTrust::TrustLevel CheckValueClassLayout(
        EEClass * pClass,
        BYTE *    pFieldLayout,
        DWORD *  pFirstObjectOverlapOffset);

    void FindPointerSeriesExplicit(
        UINT   instanceSliceSize,
        BYTE * pFieldLayout);

    VOID    HandleGCForExplicitLayout();

    MethodDescChunk ** AllocateMDChunks(
        bmtTokenRangeNode * pTokenRanges,
        DWORD               type,
        DWORD               impl,
        DWORD *             pNumChunks,
        AllocMemTracker *   pamTracker);

    // this accesses the field size which is temporarily stored in m_pMTOfEnclosingClass
    // during class loading. Don't use any other time
    DWORD GetFieldSize(FieldDesc *pFD);

    inline size_t InstanceSliceOffsetForExplicit(
        BOOL containsPointers,
        MethodTable* pParentMethodTable);

    BOOL TestOverrideForAccessibility(
        Assembly *pParentAssembly,
        Assembly *pChildAssembly,
        DWORD     dwParentAttrs);

    VOID TestOverRide(
        DWORD dwParentAttrs,
        DWORD dwMemberAttrs,
        Module *pModule,
        Module *pParentModule,
        mdToken method);

    VOID TestMethodImpl(
        Module *pDeclModule,
        Module *pImplModule,
        mdToken tokDecl,
        mdToken tokImpl);

    // Heuristic to detemine if we would like instances of this class 8 byte aligned
    BOOL ShouldAlign8(
        DWORD dwR8Fields,
        DWORD dwTotalFields);

}; // end of class EEClass

typedef EEClass *LPEEClass;


class LayoutEEClass : public EEClass
{
public:
    EEClassLayoutInfo m_LayoutInfo;

#ifndef DACCESS_COMPILE
    LayoutEEClass(Module *pModule, DWORD genericsFlags) : EEClass(pModule,genericsFlags)
    {
        LEAF_CONTRACT;
#ifdef _DEBUG
        FillMemory(&m_LayoutInfo, sizeof(m_LayoutInfo), 0xcc);
#endif
    }
#endif // !DACCESS_COMPILE
};

class UMThunkMarshInfo;
struct MLHeader;

class DelegateEEClass : public EEClass
{
public:
    PTR_Stub            m_pSecurityStub;
    PTR_Stub            m_pStaticCallStub;
    PTR_Stub            m_pUMCallStub;
    PTR_Stub            m_pInstRetBuffCallStub;
    PTR_MethodDesc      m_pInvokeMethod;
    PTR_Stub            m_pMultiCastInvokeStub;
    UMThunkMarshInfo*   m_pUMThunkMarshInfo;
    PTR_MethodDesc      m_pBeginInvokeMethod;
    PTR_MethodDesc      m_pEndInvokeMethod;
    PTR_Stub            m_pMLStub;

#ifndef DACCESS_COMPILE
    DelegateEEClass(Module *pModule, DWORD genericsFlags) : EEClass(pModule,genericsFlags)
    {
        LEAF_CONTRACT;
        m_pSecurityStub = NULL;
        m_pStaticCallStub = NULL;
        m_pUMCallStub = NULL;
        m_pInstRetBuffCallStub = NULL;
        m_pInvokeMethod = NULL;
        m_pMultiCastInvokeStub = NULL;
        m_pUMThunkMarshInfo = NULL;
        m_pBeginInvokeMethod = NULL;
        m_pEndInvokeMethod = NULL;
        m_pMLStub = NULL;
    }

    // We need a LoaderHeap that lives at least as long as the DelegateEEClass, but ideally no longer
    LoaderHeap *GetStubHeap();
#endif // !DACCESS_COMPILE

};


#include <pshpack8.h> //m_names and m_values are updated via InterlockedCompareExchange - have to be pointer-aligned.
class EnumEEClass : public EEClass
{
    friend class EEClass;

 private:

    DWORD           m_countPlusOne; // biased by 1 so zero can be used as uninit flag
    union
    {
        void        *m_values;
        BYTE        *m_byteValues;
        USHORT      *m_shortValues;
        UINT        *m_intValues;
        UINT64      *m_longValues;
    };
    LPCUTF8         *m_names;

 public:
#ifndef DACCESS_COMPILE
    EnumEEClass(Module *pModule, DWORD genericsFlags) : EEClass(pModule,genericsFlags)
    {
        LEAF_CONTRACT;
        // Rely on zero init from LoaderHeap
    }
#endif // !DACCESS_COMPILE

    BOOL EnumTablesBuilt() { LEAF_CONTRACT; return m_countPlusOne > 0; }

    DWORD GetEnumCount() { LEAF_CONTRACT; return m_countPlusOne-1; } // note -1 because of bias

    int GetEnumLogSize();

    // These all return arrays of size GetEnumCount() :
    BYTE *GetEnumByteValues() { LEAF_CONTRACT; g_IBCLogger.LogStoredEnumDataAccess(this); return m_byteValues; }
    USHORT *GetEnumShortValues() { LEAF_CONTRACT;g_IBCLogger.LogStoredEnumDataAccess(this); return m_shortValues; }
    UINT *GetEnumIntValues() { LEAF_CONTRACT; g_IBCLogger.LogStoredEnumDataAccess(this); return m_intValues; }
    UINT64 *GetEnumLongValues() { LEAF_CONTRACT; g_IBCLogger.LogStoredEnumDataAccess(this); return m_longValues; }
    LPCUTF8 *GetEnumNames() { LEAF_CONTRACT; g_IBCLogger.LogStoredEnumDataAccess(this); return m_names; }


    HRESULT BuildEnumTables();
};
#include <poppack.h>


typedef DPTR(ArrayClass) PTR_ArrayClass;

// Dynamically generated array class structure
class ArrayClass : public EEClass
{

    friend MethodTable* Module::CreateArrayMethodTable(TypeHandle elemTypeHnd, CorElementType arrayKind, unsigned Rank, AllocMemTracker *pamTracker);
#ifndef DACCESS_COMPILE
    ArrayClass(Module *pModule) : EEClass(pModule,VMFLAG_NONGENERIC) { }
#endif

private:

    // struct {
        unsigned char   m_dwRank      : 8;
        CorElementType  m_ElementType : 8; // Cache of element type in m_ElementTypeHnd
    // };

    TypeHandle      m_ElementTypeHnd;
    MethodDesc*     m_elementCtor; // if is a value class array and has a default constructor, this is it

public:
    DWORD GetRank() {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return m_dwRank;
    }
    void SetRank (unsigned Rank) {
        LEAF_CONTRACT;
        m_dwRank = Rank;
    }

    MethodDesc* GetArrayElementCtor() {
        LEAF_CONTRACT;
        return(m_elementCtor);
    }
    void SetElementCtor (MethodDesc *elementCtor) {
        LEAF_CONTRACT;
        m_elementCtor = elementCtor;
    }

    TypeHandle GetApproxArrayElementTypeHandle() {
        WRAPPER_CONTRACT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return GetApproxArrayElementTypeHandle_NoLogging();
    }

    TypeHandle GetApproxArrayElementTypeHandle_NoLogging() {
        LEAF_CONTRACT;

        return m_ElementTypeHnd;
    }
    TypeHandle* GetInstantiation() {
        LEAF_CONTRACT;
        return &m_ElementTypeHnd;
    }
    void SetArrayElementTypeHandle (TypeHandle ElementTypeHnd) {
        LEAF_CONTRACT;
        m_ElementTypeHnd = ElementTypeHnd;
    }


    CorElementType GetArrayElementType() {
        LEAF_CONTRACT;
        g_IBCLogger.LogEEClassAndMethodTableAccess(this);
        return m_ElementType;
    }
    void SetArrayElementType(CorElementType ElementType) {
        LEAF_CONTRACT;
        m_ElementType = ElementType;
    }


    // Allocate a new MethodDesc for the methods we add to this class
    void InitArrayMethodDesc(
        ArrayMethodDesc* pNewMD,
        PCCOR_SIGNATURE pShortSig,
        DWORD   cShortSig,
        DWORD   dwVtableSlot,
        BaseDomain *pDomain,
        AllocMemTracker *pamTracker);

};


inline BOOL EEClass::IsBlittable()
{
    WRAPPER_CONTRACT;

    // Either we have an opaque bunch of bytes, or we have some fields that are
    // all isomorphic and explicitly layed out.
    return (HasLayout() && ((LayoutEEClass*)this)->GetLayoutInfo()->IsBlittable());
}

inline BOOL EEClass::IsManagedSequential()
{
    WRAPPER_CONTRACT;
    return HasLayout() && ((LayoutEEClass*)this)->GetLayoutInfo()->IsManagedSequential();
}

//==========================================================================
// These routines manage the prestub (a bootstrapping stub that all
// FunctionDesc's are initialized with.)
//==========================================================================
VOID InitPreStubManager();
Stub *ThePreStub();
Stub *TheUMThunkPreStub();


//-----------------------------------------------------------
// Invokes a specified non-static method on an object.
//-----------------------------------------------------------

void CallDefaultConstructor(OBJECTREF ref);

extern "C" const BYTE * __stdcall PreStubWorker(PrestubMethodFrame *pPFrame);

extern "C" ARG_SLOT __stdcall CallDescrWorker(
#ifdef CALLDESCR_BOTTOMUP
                LPVOID                   pSrcStart,
#else // !CALLDESCR_BOTTOMUP
                LPVOID                      pSrcEnd,
#endif // !CALLDESCR_BOTTOMUP
                UINT32                      numStackSlots,
#ifdef CALLDESCR_ARGREGS
                const ArgumentRegisters *   pArgumentRegisters,
#endif
#ifdef CALLDESCR_REGTYPEMAP
                UINT64                      dwRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF
                LPVOID                      pRetBuff,
                UINT64                      cbRetBuff,
#endif // CALLDESCR_RETBUF
                UINT32                      fpRetSize,
                LPVOID                      pTarget);

extern "C" ARG_SLOT __stdcall CallDescrWorkerWithHandler(
#ifdef CALLDESCR_BOTTOMUP
                LPVOID                   pSrcStart,
#else // !CALLDESCR_BOTTOMUP
                LPVOID                      pSrcEnd,
#endif // !CALLDESCR_BOTTOMUP
                UINT32                      numStackSlots,
#ifdef CALLDESCR_ARGREGS
                const ArgumentRegisters *   pArgumentRegisters,
#endif
#ifdef CALLDESCR_REGTYPEMAP
                UINT64                      dwRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF
                LPVOID                      pRetBuff,
                UINT64                      cbRetBuff,
#endif // CALLDESCR_RETBUF
                UINT32                      fpRetSize,
                LPVOID                      pTarget,
                BOOL                        fCriticalCall);

#define IsReallyMdPinvokeImpl(x) ( ((x) & mdPinvokeImpl) && !((x) & mdUnmanagedExport) )

//
// The MethodNameHash is a temporary loader structure which may be allocated if there are a large number of
// methods in a class, to quickly get from a method name to a MethodDesc (potentially a chain of MethodDescs).
//

#define METH_NAME_CACHE_SIZE        5
#define MAX_MISSES                  3

// Entry in the method hash table
class MethodHashEntry
{
public:
    MethodHashEntry *   m_pNext;        // Next item with same hash value
    DWORD               m_dwHashValue;  // Hash value
    MethodDesc *        m_pDesc;
    LPCUTF8             m_pKey;         // Method name
};

class MethodNameHash
{
public:

    MethodHashEntry **m_pBuckets;       // Pointer to first entry for each bucket
    DWORD             m_dwNumBuckets;
    BYTE *            m_pMemory;        // Current pointer into preallocated memory for entries
    BYTE *            m_pMemoryStart;   // Start pointer of pre-allocated memory fo entries
    MethodNameHash   *m_pNext;          // Chain them for stub dispatch lookup
#ifdef _DEBUG
    BYTE *            m_pDebugEndMemory;
#endif

    MethodNameHash()
    {
        LEAF_CONTRACT;
        m_pMemoryStart = NULL;
        m_pNext = NULL;
    }

    ~MethodNameHash()
    {
        LEAF_CONTRACT;
        if (m_pMemoryStart != NULL)
            delete(m_pMemoryStart);
    }

    // Throws on error
    void Init(DWORD dwMaxEntries, StackingAllocator *pAllocator = NULL);

    // Insert new entry at head of list
    void Insert(
        LPCUTF8 pszName,
        MethodDesc *pDesc);

    // Return the first MethodHashEntry with this name, or NULL if there is no such entry
    MethodHashEntry *Lookup(
        LPCUTF8 pszName,
        DWORD dwHash);

    void SetNext(MethodNameHash *pNext) { m_pNext = pNext; }
    MethodNameHash *GetNext() { return m_pNext; }
};



// For generic instantiations the FieldDescs stored for instance 
// fields are approximate, not exact, i.e. they are representatives owned by 
// canonical instantiation and they do not carry exact type information.
// This will not include EnC related fields. (See EncApproxFieldDescIterator for that)
class ApproxFieldDescIterator
{
private:
    int m_iteratorType;
    EEClass *m_pClass;
    int m_currField;
    int m_totalFields;

  public:
    enum IteratorType {
       INSTANCE_FIELDS = 0x1,
       STATIC_FIELDS   = 0x2,
       ALL_FIELDS      = (INSTANCE_FIELDS | STATIC_FIELDS)
    };
    ApproxFieldDescIterator();
    ApproxFieldDescIterator(MethodTable *pMT, int iteratorType, BOOL fixupEnC=FALSE)
    {
        Init(pMT, iteratorType, fixupEnC);
    }
    void Init(MethodTable *pMT, int iteratorType, BOOL fixupEnC=FALSE);
    FieldDesc* Next();

    int GetIteratorType() {
        LEAF_CONTRACT;
        return m_iteratorType;
    }
    
    int Count() {
        LEAF_CONTRACT;
        return m_totalFields;
    }
    int CountRemaining() {
        LEAF_CONTRACT;
        return m_totalFields - m_currField - 1;
    }
};

//
// DeepFieldDescIterator iterates over the entire
// set of fields available to a class, inherited or
// introduced.
//

class DeepFieldDescIterator
{
private:
    ApproxFieldDescIterator m_fieldIter;
    int m_numClasses;
    int m_curClass;
    EEClass* m_classes[16];
    int m_deepTotalFields;
    bool m_lastNextFromParentClass;

    bool NextClass();
    
public:
    DeepFieldDescIterator()
    {
        LEAF_CONTRACT;
        
        m_numClasses = 0;
        m_curClass = 0;
        m_deepTotalFields = 0;
        m_lastNextFromParentClass = false;
    }
    DeepFieldDescIterator(MethodTable* pMT, int iteratorType,
                          bool includeParents = true)
    {
        WRAPPER_CONTRACT;
        
        Init(pMT, iteratorType, includeParents);
    }
    void Init(MethodTable* pMT, int iteratorType,
              bool includeParents = true);
    
    FieldDesc* Next();

    bool Skip(int numSkip);
    
    int Count()
    {
        LEAF_CONTRACT;
        return m_deepTotalFields;
    }
    bool IsFieldFromParentClass()
    {
        LEAF_CONTRACT;
        return m_lastNextFromParentClass;
    }
};

#endif // !CLASS_H
