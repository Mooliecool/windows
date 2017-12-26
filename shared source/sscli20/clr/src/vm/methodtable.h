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
// File: MethodTable.H
//
#ifndef _METHODTABLE_H_
#define _METHODTABLE_H_

/*
 *  Include Files 
 */
#include "vars.hpp"
#include "cor.h"
#include "hash.h"
#include "crst.h"
#include "objecthandle.h"
#include "cgensys.h"
#include "declsec.h"
#include "list.h"
#include "spinlock.h"
#include "typehandle.h"
#include "eehash.h"
#include "contractimpl.h"

/*
 * Forward Declarations
 */
class    AppDomain;
class    ArrayClass;
class    ArrayMethodDesc;
struct   ClassCtorInfoEntry;
class ClassLoader;
class    DomainLocalBlock;
class FCallMethodDesc;
class    EEClass;
class    EnCFieldDesc;
class FieldDesc;
class    FieldMarshaler;
class JIT_TrialAlloc;
struct LayoutRawFieldInfo;
class MetaSig;
class    MethodDesc;
class    MethodDescChunk;
class    MethodTable;
class    Module;
class Module;
class    Object;
class    Stub;
class    Substitution;
class    TypeHandle;
class   CrossDomainOptimizationInfo;
class   Dictionary;
class   AllocMemTracker;
class   ZapCodeMap;
class   ZapMonitor;
class   SimpleRWLock;
class   MethodDataCache;
typedef DPTR(CrossDomainOptimizationInfo) PTR_CrossDomainOptimizationInfo;
class   EEClassLayoutInfo;

//============================================================================
// This is the in-memory structure of a class and it will evolve.
//============================================================================


//
// A MethodTable contains an array of these structures, which describes each interface implemented
// by this class (directly declared or indirectly declared).
//
// Generic type instantiations (in C# syntax: C<ty_1,...,ty_n>) are represented by
// MethodTables, i.e. a new MethodTable gets allocated for each such instantiation.
// The entries in these tables (i.e. the code) are, however, often shared.
//
// In particular, a MethodTable's vtable contents (and hence method descriptors) may be 
// shared between compatible instantiations (e.g. List<string> and List<object> have
// the same vtable *contents*).  Likewise the EEClass will be shared between 
// compatible instantiations whenever the vtable contents are. 
//
// !!! Thus that it is _not_ generally the case that GetClass.GetMethodTable() == t. !!!
//
// Instantiated interfaces have their own method tables unique to the instantiation e.g. I<string> is
// distinct from I<int> and I<object>
// 
// For generic types the interface map lists generic interfaces
// For instantiated types the interface map lists instantiated interfaces
//   e.g. for C<T> : I<T>, J<string>
// the interface map for C would list I and J
// the interface map for C<int> would list I<int> and J<string>
//
typedef struct
{
    enum {
        interface_declared_on_class = 0x1,
        interface_implemented_on_parent = 0x2,
    };

    MethodTable* m_pMethodTable;        // Method table of the interface
    WORD         m_wFlags;
    
private:
    WORD         m_wStartSlot;          // starting slot of interface in vtable
    
public:
    WORD         GetInteropStartSlot()
    {
        return m_wStartSlot;
    }
    void         SetInteropStartSlot(WORD wStartSlot)
    {
        m_wStartSlot = wStartSlot;
    }

    BOOL         IsDeclaredOnClass()
    {
        LEAF_CONTRACT;
        return (m_wFlags & interface_declared_on_class);
    }

    BOOL         IsImplementedByParent()
    {
        LEAF_CONTRACT;
        return (m_wFlags & interface_implemented_on_parent);
    }

} InterfaceInfo_t;

typedef DPTR(InterfaceInfo_t) PTR_InterfaceInfo;

// Data needed when simulating old VTable layout for COM Interop
// This is necessary as the data is saved in MethodDescs and we need
// to simulate different values without copying or changing the existing
// MethodDescs
//
// This will be created in a parallel array to ppMethodDescList and
// ppUnboxMethodDescList in the bmtMethAndFieldDescs structure below
struct InteropMethodTableSlotData {
    enum {
        e_DUPLICATE = 0x0001              // The entry is duplicate
    };

    MethodDesc *pMD;                // The MethodDesc for this slot
    WORD        wSlot;              // The simulated slot value for the MethodDesc
    WORD        wFlags;             // The simulated duplicate value
    MethodDesc *pDeclMD;            // To keep track of MethodImpl's

    void SetDuplicate()
    {
        wFlags |= e_DUPLICATE;
    }

    BOOL IsDuplicate() {
        return ((BOOL)(wFlags & e_DUPLICATE));
    }

    WORD GetSlot() {
        return wSlot;
    }

    void SetSlot(WORD wSlot) {
        this->wSlot = wSlot;
    }
};


//
// This struct contains cached information on the GUID associated with a type. 
//

typedef struct
{
    GUID         m_Guid;                // The actual guid of the type.
    BOOL         m_bGeneratedFromName;  // A boolean indicating if it was generated from the 
                                        // name of the type.
} GuidInfo;

typedef struct
{
    // Total number of instantiation dictionaries including inherited ones
    //   i.e. how many instantiated classes (including this one) are there in the hierarchy?
    // See comments about PerInstInfo
    WORD   m_wNumDicts;

    // Number of type parameters (NOT including those of superclasses).
    WORD   m_wNumTyPars;
} GenericsDictInfo;

typedef DPTR(GenericsDictInfo) PTR_GenericsDictInfo;

typedef struct
{
    // Pointer to field descs for statics
    FieldDesc*          m_pFieldDescs;

    // Method table ID for statics
    SIZE_T              m_DynamicTypeID;

    // Module this method table statics are attached to. 
    //
    // The statics has to be attached to module referenced from the generic instantiation 
    // in domain-neutral code. We need to guarantee that the module for the statics 
    // has a valid local represenation in an appdomain.
    // 
    PTR_Module          m_pModuleForStatics;

} GenericsStaticsInfo;

typedef DPTR(GenericsStaticsInfo) PTR_GenericsStaticsInfo;


// This structure records methods and fields which are interesting for VTS
// (Version Tolerant Serialization). A pointer to it is optionally appended to
// MethodTables with VTS event methods or NotSerialized or OptionallySerialized
// fields. The structure is variable length to incorporate a packed array of
// data describing the disposition of fields in the type.
struct RemotingVtsInfo
{
    enum VtsCallbackType
    {
        VTS_CALLBACK_ON_SERIALIZING = 0,
        VTS_CALLBACK_ON_SERIALIZED,
        VTS_CALLBACK_ON_DESERIALIZING,
        VTS_CALLBACK_ON_DESERIALIZED,
        VTS_NUM_CALLBACK_TYPES
    };

    MethodDesc         *m_pCallbacks[VTS_NUM_CALLBACK_TYPES];
#ifdef _DEBUG
    DWORD               m_dwNumFields;
#endif
    DWORD               m_rFieldTypes[1];

    static DWORD GetSize(DWORD dwNumFields)
    {
        LEAF_CONTRACT;
        // Encode each field in two bits. Round up allocation to the nearest DWORD.
        DWORD dwBitsRequired = dwNumFields * 2;
        DWORD dwBytesRequired = (dwBitsRequired + 7) / 8;
        return (DWORD)(offsetof(RemotingVtsInfo, m_rFieldTypes[0]) + ALIGN_UP(dwBytesRequired, sizeof(DWORD)));
    }

    void SetIsNotSerialized(DWORD dwFieldIndex)
    {
        LEAF_CONTRACT;
        _ASSERTE(dwFieldIndex < m_dwNumFields);
        DWORD dwRecordIndex = dwFieldIndex * 2;
        DWORD dwOffset = dwRecordIndex / (sizeof(DWORD) * 8);
        DWORD dwMask = 1 << (dwRecordIndex % (sizeof(DWORD) * 8));
        m_rFieldTypes[dwOffset] |= dwMask;
    }

    BOOL IsNotSerialized(DWORD dwFieldIndex)
    {
        LEAF_CONTRACT;
        _ASSERTE(dwFieldIndex < m_dwNumFields);
        DWORD dwRecordIndex = dwFieldIndex * 2;
        DWORD dwOffset = dwRecordIndex / (sizeof(DWORD) * 8);
        DWORD dwMask = 1 << (dwRecordIndex % (sizeof(DWORD) * 8));
        return m_rFieldTypes[dwOffset] & dwMask;
    }

    void SetIsOptionallySerialized(DWORD dwFieldIndex)
    {
        LEAF_CONTRACT;
        _ASSERTE(dwFieldIndex < m_dwNumFields);
        DWORD dwRecordIndex = dwFieldIndex * 2;
        DWORD dwOffset = dwRecordIndex / (sizeof(DWORD) * 8);
        DWORD dwMask = 2 << (dwRecordIndex % (sizeof(DWORD) * 8));
        m_rFieldTypes[dwOffset] |= dwMask;
    }

    BOOL IsOptionallySerialized(DWORD dwFieldIndex)
    {
        LEAF_CONTRACT;
        _ASSERTE(dwFieldIndex < m_dwNumFields);
        DWORD dwRecordIndex = dwFieldIndex * 2;
        DWORD dwOffset = dwRecordIndex / (sizeof(DWORD) * 8);
        DWORD dwMask = 2 << (dwRecordIndex % (sizeof(DWORD) * 8));
        return m_rFieldTypes[dwOffset] & dwMask;
    }
};

typedef DPTR(RemotingVtsInfo) PTR_RemotingVtsInfo;


struct ThreadAndContextStaticsBucket
{
    // Offset which points to the TLS storage. Allocated lazily - -1 means no offset allocated yet.
    DWORD m_dwThreadStaticsOffset;
    // Offset which points to the CLS storage. Allocated lazily - -1 means no offset allocated yet.
    DWORD m_dwContextStaticsOffset;
    // Size of TLS fields
    WORD m_wThreadStaticsSize;
    // Size of CLS fields
    WORD m_wContextStaticsSize;
};
typedef DPTR(ThreadAndContextStaticsBucket) PTR_ThreadAndContextStaticsBucket;

//
// This struct consolidates the writeable parts of the MethodTable
// so that we can layout a read-only MethodTable with a pointer
// to the writeable parts of the MethodTable in an ngen image
//
typedef struct // MethodTableWriteableData;
{
    enum
    {
    
        enum_flag_RemotingConfigChecked     = 0x00000001,
        enum_flag_RequiresManagedActivation = 0x00000002,
        enum_flag_Unrestored                = 0x00000004,
        enum_flag_CriticalTypePrepared      = 0x00000008,     // CriticalFinalizerObject derived type has had backout routines prepared
        enum_flag_HasApproxParent           = 0x00000010,
        enum_flag_UnrestoredTypeKey         = 0x00000020,     
        enum_flag_IsNotFullyLoaded          = 0x00000040,
        enum_flag_DependenciesLoaded        = 0x00000080,     // class and all depedencies loaded up to CLASS_LOADED_BUT_NOT_VERIFIED

    };
    DWORD      m_dwFlags;                  // Lot of empty bits here.
    /*
     * m_pExposedClassObject is a RuntimeType instance for this class.  But
     * do NOT use it for Arrays or remoted objects!  All arrays of objects 
     * share the same MethodTable/EEClass.  -- BrianGru, 9/11/2000
     * @GENERICS: this used to live in EEClass but now lives here because it is per-instantiation data
     */
    OBJECTREF *m_pExposedClassObject;

public:

    inline BOOL IsRemotingConfigChecked()
    {
        LEAF_CONTRACT;
        return m_dwFlags & enum_flag_RemotingConfigChecked;
    }
    inline void SetRemotingConfigChecked()
    {
        LEAF_CONTRACT;
        // remembers that we went through the rigorous
        // checks to decide whether this class should be
        // activated locally or remote
        FastInterlockOr((ULONG *) &m_dwFlags, enum_flag_RemotingConfigChecked);
    }
    inline BOOL RequiresManagedActivation()
    {
        LEAF_CONTRACT;
        return m_dwFlags & enum_flag_RequiresManagedActivation;
    }
    inline void SetRequiresManagedActivation()
    {
        LEAF_CONTRACT;
        FastInterlockOr((ULONG *) &m_dwFlags, enum_flag_RequiresManagedActivation|enum_flag_RemotingConfigChecked);
    }

    inline OBJECTREF *GetExposedClassObject()
    {
        LEAF_CONTRACT;
        return m_pExposedClassObject;
    }
    inline void SetExposedClassObject(OBJECTREF *pObjRef)
    {
        LEAF_CONTRACT;
        m_pExposedClassObject = pObjRef;
    }

    // Have the backout methods (Finalizer, Dispose, ReleaseHandle etc.) been prepared for this type? This currently only happens
    // for types derived from CriticalFinalizerObject.
    inline BOOL CriticalTypeHasBeenPrepared()
    {
        LEAF_CONTRACT;
        return m_dwFlags & enum_flag_CriticalTypePrepared;
    }
    inline void SetCriticalTypeHasBeenPrepared()
    {
        LEAF_CONTRACT;
        FastInterlockOr((ULONG*)&m_dwFlags, enum_flag_CriticalTypePrepared);
    }

} MethodTableWriteableData;

typedef DPTR(MethodTableWriteableData) PTR_MethodTableWriteableData;







//
//
//
// 
//
class MethodTable
{
    /************************************
     *  FRIEND FUNCTIONS
     ************************************/
    // DO NOT ADD FRIENDS UNLESS ABSOLUTELY NECESSARY
    // USE ACCESSORS TO READ/WRITE private field members

    friend class ZapMonitor;

    // Special access for setting up String object method table correctly
    friend class ClassLoader;
    friend class JIT_TrialAlloc;

    friend class EEClass;
    friend class MethodTableBuilder;
    friend class CheckAsmOffsets;

public:
    // Do some sanity checking to make sure it's a method table 
    // and not pointing to some random memory.  In particular
    // check that (apart from the special case of instantiated generic types) we have
    // GetCanonicalMethodTable() == this;
    BOOL SanityCheck();

    static void         CallFinalizer(Object *obj);

public:
    // This is the way to create a new method table. Don't try calling new directly.

    // Mehtod tables are also created in array.cpp where an EEClass
    // and MethodTable are created in one fell swoop.  I see no rationale basis for 
    // such an approach.
    //
    // Even worse almost exactly the same stuff is duplicated in DynamicMethod.cpp.
    // Worse still almost exactly the same stuff is duplicated in generics.cpp.
    //
    static MethodTable * AllocateNewMT(EEClass *pClass,
                                       DWORD dwVtableSlots, 
                                       DWORD dwGCSize, 
                                       DWORD dwNumInterfaces, 
                                       DWORD numGenericArgs,
                                       DWORD dwNumDicts, 
                                       DWORD dwNumTypeSlots, 
                                       ClassLoader *pClassLoader,
                                       BaseDomain *pDomain, 
                                       BOOL isIFace, 
                                       BOOL fHasGenericsStaticsInfo,
                                       BOOL fNeedsRemotableMethodInfo,
                                       BOOL fNeedsRemotingVtsInfo,
                                       BOOL fHasThreadOrContextStatics
                                       , AllocMemTracker *pamTracker
        );
#ifndef DACCESS_COMPILE
#endif // !DACCESS_COMPILE

    Module* GetModule();
    void SetModule (Module *pModule);
    Assembly *GetAssembly();

    // GetDomain on an instantiated type, e.g. C<ty1,ty2> returns the SharedDomain if all the
    // constituent parts of the type are SharedDomain (i.e. domain-neutral), 
    // and returns an AppDomain if any of the parts are from an AppDomain, 
    // i.e. are domain-bound.  Note that if any of the parts are domain-bound
    // then they will all belong to the same domain.
    BaseDomain *GetDomain();

    // Does this immediate item live in an NGEN module?
    BOOL IsZapped();

    // For types that are part of an ngen-ed assembly this gets the
    // Module* that contains this methodtable.
    PTR_Module GetZapModule();


    // For regular, non-constructed types, GetLoaderModule() == GetModule()
    // For constructed types (e.g. int[], Dict<int[], C>) the hash table through which a type
    // is accessed lives in a "loader module". The rule for determining the loader module must ensure
    // that a type never outlives its loader module with respect to app-domain unloading
    //
    // GetModuleForStatics() is the third kind of module. GetModuleForStatics() is module that 
    // statics are attached to. It is the preffered zap module for the ngened generics code.
    Module* GetLoaderModule();

    void SetLoaderModule(Module* pModule);

    // Get the domain local module - useful for static init checks
    PTR_DomainLocalModule GetDomainLocalModule(AppDomain* pAppDomain = NULL);

    // Return whether the type lives in the shared domain.
    BOOL IsDomainNeutral();

    // This low level flag tells whether the class was loaded as domain neutral.  You should
    // almost never need to know this - instead use IsDomainNeutral
    BOOL IsCompiledDomainNeutral()
    {
        LEAF_CONTRACT;
        return m_wFlags & enum_flag_CompiledDomainNeutral;
    }
    void SetCompiledDomainNeutral(BOOL fDomainNeutral);

    // Is this methodtable System.Array or System.Array<T>?
    BOOL IsAncestorOfArrayType()
    {
        LEAF_CONTRACT;
        // This is a bogus implementation but it's only needed for a short time
        // to abstract away g_pArrayClass awareness from the verifier.
        // Soon, it'll be replaced by a test for one of the Array<T> classes.
        return this == g_pArrayClass;
    }

    MethodTable *LoadEnclosingMethodTable();


    void GetPathForErrorMessages(SString & result);


    //-------------------------------------------------------------------
    // COM INTEROP
    //

    int IsComObjectType()
    {
        return 0;
    }


    //-------------------------------------------------------------------
    // DYNAMIC ADDITION OF INTERFACES FOR COM INTEROP
    //
    // Support for dynamically added interfaces on extensible RCW's.


    BOOL IsIntrospectionOnly();
    VOID EnsureActive();
    CHECK CheckActivated();

    VOID EnsureInstanceActive();
    CHECK CheckInstanceActivated();

    //-------------------------------------------------------------------
    // THE DEFAULT CONSTRUCTOR
    //

public:
    BOOL HasDefaultConstructor();
    void SetDefaultConstructorSlot (WORD wDefaultCtorSlot);
    MethodDesc *GetDefaultConstructor();

    BOOL HasPublicDefaultConstructor();

    // Note: we really shouldn't need both HasPublicDefaultConstructor() (which doesn't detect implicit default ctors for valuetypes)
    // and this one (which does) but I don't want to risk changing the semantics of the former at this late date.
    BOOL HasExplicitOrImplicitPublicDefaultConstructor();

private:
    WORD GetDefaultConstructorSlot();

    //-------------------------------------------------------------------
    // THE CLASS INITIALIZATION CONDITION 
    //  (and related DomainLocalBlock/DomainLocalModule storage)
    //
    // - populate the DomainLocalModule if needed
    // - run the cctor 
    //

public:

    // checks whether the class initialiser should be run on this class, and runs it if necessary
    void CheckRunClassInitThrowing();
    HRESULT CheckRunClassInitNT(OBJECTREF *pThrowable);

    
    // compute whether the type can be considered to have had its
    // <clinit> run without doing anything at all, i.e. whether we know
    // immediately that the type requires nothing to do for initialization
    //
    // If a type used as a representiative during JITting is PreInit then 
    // any types that it may represent within a code-sharing 
    // group are also PreInit.   For example, if List<object> is PreInit then List<string> 
    // and List<MyType> are also PreInit.  This is because the dynamicStatics, staticRefHandles
    // and hasCCtor are all identical given a head type, and weakening the domainNeutrality 
    // to DomainSpecific only makes more types PreInit.
    inline static BOOL ComputeIsPreInit(BOOL fContainsGenericVariables,
                                        BOOL fHasClassConstructor, 
                                        BOOL fHasStaticBoxes, 
                                        BOOL fIsDynamicStatics)
    {
        if (fContainsGenericVariables)
            return TRUE;
        
        if (fHasClassConstructor)
            return FALSE;
        
        if (fHasStaticBoxes)
            return FALSE;
        
        if (fIsDynamicStatics)
            return FALSE;
        
        return TRUE;
    }

    BOOL ComputeIsPreInit()
    {
        return 
            MethodTable::ComputeIsPreInit(ContainsGenericVariables(),
                                          HasClassConstructor(),
                                          (GetNumBoxedStatics() > 0),
                                          IsDynamicStatics());
    }

    void InitializeFlags(DWORD initFlags)
    {
        LEAF_CONTRACT;
        m_wFlags = initFlags;
    }
    
    void InitializeFlags2(DWORD initFlags2)
    {
        LEAF_CONTRACT;
        m_wFlags2 = initFlags2;
    }
    
    // mark the class as having no class init logic required (this includes
    // running .cctor and/or setting up any statics).
    void SetClassPreInited();

    BOOL IsClassPreInited()
    {
        LEAF_CONTRACT;
        _ASSERTE_IMPL(!!ComputeIsPreInit() == !!(m_wFlags2 & enum_flag2_ClassPreInited));
        return (m_wFlags2 & enum_flag2_ClassPreInited);
    }
        
    // mark the class as having its cctor run.  
    void SetClassInited();
    BOOL  IsClassInited(AppDomain* pAppDomain = NULL);   
    BOOL  IsClassInitedHaveIndex(DWORD iIndex);   

    BOOL IsInitError();
    void SetClassInitError();

    inline BOOL IsGlobalClass()
    {
        WRAPPER_CONTRACT;
        return (GetCl() == COR_GLOBAL_PARENT_TOKEN);
    }

    // uniquely identifes this type in the Domain table
    DWORD GetClassIndex();

private:

    DWORD   GetClassIndexFromToken(mdTypeDef typeToken)
    {
        LEAF_CONTRACT;
        return RidFromToken(typeToken) - 1;
    }
    
    // called from CheckRunClassInitThrowing().  The type wasn't marked as
    // inited while we were there, so let's attempt to do the work.
    void  DoRunClassInitThrowing();

    BOOL RunClassInitEx(OBJECTREF *pThrowable); 
    BOOL RunClassInitWorker(MethodDesc* pInitMethod, OBJECTREF *pThrowable); // only runs .cctor does not throw


public:
    //-------------------------------------------------------------------
    // THE CLASS CONSTRUCTOR
    //

    MethodDesc *GetClassConstructor()
    {
        WRAPPER_CONTRACT;
        _ASSERTE_IMPL(HasClassConstructor());
        return GetMethodDescForSlot(GetClassConstructorSlot());
    }

    BOOL HasClassConstructor();
    WORD GetClassConstructorSlot();
    void SetClassConstructorSlot (WORD wCCtorSlot);

    ClassCtorInfoEntry* GetClassCtorInfoIfExists();


    void GetExtent(TADDR *ppStart, TADDR *ppEnd, BOOL classAvailable = TRUE);

    //-------------------------------------------------------------------
    // Save/Fixup/Restore/NeedsRestore
    //
    // Restore this method table if it's not already restored
    // This is done by forcing a class load which in turn calls the Restore method
    // The pending list is required for restoring types that reference themselves through
    // instantiations of the superclass or interfaces e.g. System.Int32 : IComparable<System.Int32>



    void AllocateStaticBoxes();
    OBJECTREF AllocateStaticBox(MethodTable* pFieldMT, OBJECTHANDLE* pHandle = 0);

    void RestoreStaticMethodTablePointers();

    void CheckRestore();

    // Perform restore actions on type key components of method table (EEClass pointer + Module, generic args)
    void DoRestoreTypeKey();

    inline BOOL HasUnrestoredTypeKey() const
    {
        LEAF_CONTRACT;

        return MayNeedRestore() && 
            (GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_UnrestoredTypeKey) != 0;
    }

    // Actually do the restore actions on the method table
    void Restore();
    
    void SetIsRestored();
 
    inline BOOL IsRestored_NoLogging()
    {
        LEAF_CONTRACT;
        // If we are prerestored or we are not in an ngen-ed disk image
        // then we are considered a restored methodtable.
        // Note that MayNeedRestore is always false for jitted code.
        if (!MayNeedRestore())
            return TRUE;

        return !(GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_Unrestored);
    }
    inline BOOL IsRestored()
    {
        WRAPPER_CONTRACT;
        g_IBCLogger.LogMethodTableAccess(this);
        // If we are prerestored or we are not in an ngen-ed disk image
        // then we are considered a restored methodtable.
        // Note that MayNeedRestore is always false for jitted code.
        if (!MayNeedRestore())
            return TRUE;

        g_IBCLogger.LogMethodTableWriteableDataAccess(this);
        return !(GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_Unrestored);
    }

    //-------------------------------------------------------------------
    // LOAD LEVEL 
    //
    // The load level of a method table is derived from various flag bits
    // See classloadlevel.h for details of each level
    //
    // Level CLASS_LOADED (fully loaded) is special: a type only
    // reaches this level once all of its dependent types are also at
    // this level (generic arguments, parent, interfaces, etc).
    // Fully loading a type to this level is done outside locks, hence the need for
    // a single atomic action that sets the level.
    // 
    inline void SetIsFullyLoaded()
    {
        LEAF_CONTRACT;
        PRECONDITION(!HasApproxParent());
        PRECONDITION(IsRestored());

        FastInterlockAnd(&GetWriteableData()->m_dwFlags, ~MethodTableWriteableData::enum_flag_IsNotFullyLoaded);
    }

    inline void SetIsNotFullyLoaded()
    {
        LEAF_CONTRACT;

        // Used only during method table initialization - no need for Interlocked Exchange. 
        GetWriteableData()->m_dwFlags |= MethodTableWriteableData::enum_flag_IsNotFullyLoaded;
    }


    // Equivalent to GetLoadLevel() == CLASS_LOADED
    inline BOOL IsFullyLoaded() 
    {
        WRAPPER_CONTRACT;
        
        return (IsZapped() && !MayNeedRestore())
            || (GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_IsNotFullyLoaded) == 0;
    }

    inline void SetIsDependenciesLoaded()
    {
        LEAF_CONTRACT;
        PRECONDITION(!HasApproxParent());
        PRECONDITION(IsRestored());

        FastInterlockOr(&GetWriteableData()->m_dwFlags, MethodTableWriteableData::enum_flag_DependenciesLoaded);
    }


    // Equivalent to GetLoadLevel() == CLASS_DEPENDENCIES_LOADED
    inline BOOL IsDependenciesLoaded() 
    {
        WRAPPER_CONTRACT;
        
        return (IsZapped() && !MayNeedRestore())
            || (GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_DependenciesLoaded) != 0;
    }

    inline ClassLoadLevel GetLoadLevel()
    {
        LEAF_CONTRACT;

        g_IBCLogger.LogMethodTableAccess(this);
        
        // Fast path for zapped images (don't look at writeable data)
        if (IsZapped())
        {
            // If it was marked as not needing a restore, then it was fully loaded at ngen time
            if (!MayNeedRestore())
                return CLASS_LOADED;

            // Otherwise look at the restore bits
            g_IBCLogger.LogMethodTableWriteableDataAccess(this);

            if (GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_UnrestoredTypeKey)
                return CLASS_LOAD_UNRESTOREDTYPEKEY;

            if (GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_Unrestored)
                return CLASS_LOAD_UNRESTORED;
        }

        g_IBCLogger.LogMethodTableWriteableDataAccess(this);

        if (HasApproxParent())
            return CLASS_LOAD_APPROXPARENTS;
        
        if (!(GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_DependenciesLoaded))
            return CLASS_LOAD_EXACTPARENTS;

        if (GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_IsNotFullyLoaded)
            return CLASS_DEPENDENCIES_LOADED;

        return CLASS_LOADED;
    }

#ifdef _DEBUG
    CHECK CheckLoadLevel(ClassLoadLevel level)
    {
        return TypeHandle(this).CheckLoadLevel(level);
    }
#endif
 

    void DoFullyLoad(TypeHandleList *pVisited, ClassLoadLevel level, DFLPendingList *pPending, BOOL *pfBailed, BOOL fDoConstraintChecks = TRUE);

    //-------------------------------------------------------------------
    // METHOD TABLES AS TYPE DESCRIPTORS
    //
    // A MethodTable can represeent a type such as "String" or an 
    // instantiated type such as "List<String>".
    //
    
    inline BOOL IsInterface()
    {
        LEAF_CONTRACT;
        return m_wFlags2 & enum_flag2_IsInterface;
    }

    void SetIsInterface()
    {
        LEAF_CONTRACT;
        m_wFlags2 |= enum_flag2_IsInterface;
    }

    inline BOOL IsSealed();

    inline BOOL IsAbstract();

    BOOL IsExternallyVisible();

    // Get the instantiation for this instantiated type e.g. for Dict<string,int> 
    // this would be an array {string,int}
    // If not instantiated, return NULL
    TypeHandle* GetInstantiation();

    // Get the instantiation for an instantiated type or a pointer to the
    // element type for an array
    TypeHandle* GetClassOrArrayInstantiation();

    // Is this a method table for a generic type instantiation, e.g. List<string>?
    inline BOOL HasInstantiation();

    // Is this an instantiation of a generic class at its formal
    // type parameters ie. List<T> ?
    inline BOOL IsGenericTypeDefinition();

    BOOL ContainsGenericMethodVariables();

    static BOOL ComputeContainsGenericVariables(DWORD nGenericClassArgs, TypeHandle *pClassArgs);

    inline void SetContainsGenericVariables()
    {
        m_wFlags2 |= enum_flag2_ContainsGenericVariables;
    }

    inline void SetHasVariance()
    {
        m_wFlags2 |= enum_flag2_HasVariance;
    }

    inline BOOL HasVariance()
    {
        return (m_wFlags2 & enum_flag2_HasVariance) != 0;
    }

    // Is this something like List<T> or List<Stack<T>>?
    // List<Blah<T>> only exists for reflection and verification.
    inline DWORD ContainsGenericVariables(BOOL methodVarsOnly = FALSE)
    {
        WRAPPER_CONTRACT;
        if (methodVarsOnly)
            return ContainsGenericMethodVariables();
        else
            return (m_wFlags2 & enum_flag2_ContainsGenericVariables);
    }


    inline BOOL ContainsStackPtr();

    // class is a com object class
    Module* GetDefiningModuleForOpenType();
    
    inline BOOL IsTypicalTypeDefinition()       
    {
        WRAPPER_CONTRACT;
        return !HasInstantiation() || IsGenericTypeDefinition();
    }

    // Is this a ground/closed/concrete instantiation of a generic type?
    // ie. List<String>, as opposed to List<T>, List<Blah<T>>, or String.
    inline BOOL IsClosedInstantiation()
    {
        WRAPPER_CONTRACT;
        return HasInstantiation() && !ContainsGenericVariables();
    }
    
    BOOL HasSameTypeDefAs(MethodTable *pMT);

    //-------------------------------------------------------------------
    // GENERICS & CODE SHARING 
    //

    BOOL IsSharedByGenericInstantiations();

    //==========================================================================================
    // Similar to MethodTable::IsSharedByGenericInstantiations()
    //  but will also return true for subtypes that contain a Canonical type:
    //
    // A<__Canon>    is the canonical MethodTable (aka "representative" generic MT)
    // A<B<__Canon>> is a subtype that contains a Canonical type
    //
    BOOL IsSharedByGenericCanonicalSubtype();

    // If this is a "representative" generic MT or a non-generic (regular) MT return true
    inline BOOL IsCanonicalMethodTable();

    // Return the canonical representative MT amongst the set of MT's that share
    // code with the given MT because of generics.
    MethodTable *GetCanonicalMethodTable();

    //-------------------------------------------------------------------
    // Accessing methods by slot number
    //
    // Some of these functions are also currently used to get non-virtual
    // methods, relying on the assumption that they are contiguous.  This
    // is not true for non-virtual methods in generic instantiations, which
    // only live on the canonical method table.

    enum
    {
        NO_SLOT = 0xffff // a unique slot number used to indicate "empty" for fields that record slot numbers
    };

    SLOT GetSlot(UINT32 slotNumber)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        return *GetSlotPtr(slotNumber);
    }

    PTR_SLOT GetSlotPtr(UINT32 slotNum);

    void SetSlot(UINT32 slotNum, SLOT slotVal);
    void SetSlot_Log(UINT32 slotNum, SLOT slotVal);

    //-------------------------------------------------------------------
    // The VTABLE
    //

    inline unsigned GetNumVirtuals()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogMethodTableAccess(this);
        return m_wNumVirtuals;
    }
    unsigned GetNumVtableSlots() { WRAPPER_CONTRACT; return GetNumVirtuals(); }

    inline void SetNumVirtuals (WORD wNumVtableSlots)
    {
        LEAF_CONTRACT;
        m_wNumVirtuals = wNumVtableSlots;
    }
    void SetNumVtableSlots(WORD wNumVtableSlots) { WRAPPER_CONTRACT; SetNumVirtuals(wNumVtableSlots); }

    unsigned GetNumParentVirtuals()
    {
        WRAPPER_CONTRACT;
        if (IsInterface() || IsTransparentProxyType()) {
            return 0;
    }
        MethodTable *pMTParent = GetParentMethodTable();
        g_IBCLogger.LogMethodTableAccess(this);
        return pMTParent == NULL ? 0 : pMTParent->GetNumVirtuals();
    }

    inline PTR_SLOT GetVtable()
    {
        LEAF_CONTRACT;
        return PTR_SLOT((PTR_HOST_TO_TADDR(this) + TADDR(GetVtableOffset())));
    }

    static inline DWORD GetVtableOffset()
    {
        LEAF_CONTRACT;
        return (sizeof(MethodTable));
    }

    //-------------------------------------------------------------------
    // The NON-VIRTUAL METHODS IN THE METHOD TABLE
    //

    // Return total vtable slots : virtual, static, and instance method slots.
    unsigned GetNumMethods()
    {
        LEAF_CONTRACT;
        return m_wNumMethods;
    }

    void SetNumMethods(WORD wNumSlots)
    {
        LEAF_CONTRACT;
        m_wNumMethods = wNumSlots;
    }

    unsigned GetTotalSlots() { WRAPPER_CONTRACT; return GetNumMethods(); }
    void SetTotalSlots(WORD wNumSlots) { WRAPPER_CONTRACT; SetNumMethods(wNumSlots); }

    //-------------------------------------------------------------------
    // Slots <-> the MethodDesc associated with the slot.
    //

    // <NICE> What does "Unknown" mean in this context!? In particular how
    // does GetMethodDescForSlot differ from GetUnknownMethodDescForSlot?  They
    // actually do identical work underneath but perhaps the distinction means
    // something to the caller, i.e. we may be able to add an assert to one 
    // to indicate that it can only be used for particular kinds of slots.</NICE>
    MethodDesc* GetMethodDescForSlot(DWORD slot);
    MethodDesc* GetUnknownMethodDescForSlot(DWORD slot);

    // This returns the declaration method desc, not the implementation methoddesc
    // in the light of methodImpls.
    MethodDesc* GetDeclMethodDescForSlot(DWORD slot);

    static MethodDesc*  GetUnknownMethodDescForSlotAddress(SLOT addr, BOOL fSpeculative = FALSE);

    SLOT GetRestoredSlotForMethod(MethodDesc* method)
    {
        WRAPPER_CONTRACT;
        return *GetAddrOfRestoredSlotForMethod(method);
    }
    SLOT *GetAddrOfRestoredSlotForMethod(MethodDesc* method);    // Works for both static and virtual method

    SLOT GetRestoredSlot(DWORD slot);
    PTR_SLOT GetRestoredSlotPtr(DWORD slot);


    //-------------------------------------------------------------------
    // BoxedEntryPoint MethodDescs.  
    //
    // Virtual methods on structs have BoxedEntryPoint method descs in their vtable.
    // See also notes for MethodDesc::FindOrCreateAssociatedMethodDesc.  You should
    // probably be using that function if you need to map between unboxing
    // stubs and non-unboxing stubs.

    MethodDesc* GetBoxedEntryPointMD(MethodDesc *pMD);

    MethodDesc* GetUnboxedEntryPointMD(MethodDesc *pMD);
    MethodDesc* GetExistingUnboxedEntryPointMD(MethodDesc *pMD);

    //-------------------------------------------------------------------
    // GCInfo
    //
    inline WORD GetNumGCPointerSeries();

    //-------------------------------------------------------------------
    // FIELD LAYOUT, OBJECT SIZE ETC. 
    //

    inline BOOL HasLayout();

    inline EEClassLayoutInfo *GetLayoutInfo();

    inline BOOL IsBlittable();

    inline BOOL IsManagedSequential();

    UINT32 GetNativeSize();
    void SetNativeSize(UINT32 nativeSize);

    DWORD           GetBaseSize()       
    { 
        LEAF_CONTRACT;
#ifndef DACCESS_COMPILE        
        // DAC needs to look at MethodTables that may be invalid
        _ASSERTE(m_BaseSize % sizeof(void*) == 0); 
#endif // !DACCESS_COMPILE
        return(m_BaseSize); 
    }

    void            SetBaseSize(DWORD baseSize)       
    { 
        LEAF_CONTRACT;
        _ASSERTE(baseSize % sizeof(void*) == 0); 
        m_BaseSize = baseSize; 
    }
    WORD            GetComponentSize()  
    {
        LEAF_CONTRACT;
#if BIGENDIAN
        return *((WORD*)&m_wFlags + 1);
#else // !BIGENDIAN
        return *(WORD*)&m_wFlags;
#endif // !BIGENDIAN
    }

    void SetComponentSize(WORD wComponentSize)
    {
        LEAF_CONTRACT;
        m_wFlags = (m_wFlags & ~0xFFFF) | wComponentSize;
    }

    inline WORD GetNumInstanceFields();

    inline WORD GetNumStaticFields();

    // Note that for value types GetBaseSize returns the size of instance fields for
    // a boxed value, and GetNumInstanceFieldsBytes for an unboxed value.
    // We place methods like these on MethodTable primarily so we can choose to cache
    // the information within MethodTable, and so less code manipulates EEClass
    // objects directly, because doing so can lead to bugs related to generics.
    //
    //
    inline DWORD GetNumInstanceFieldBytes();

    inline WORD GetNumIntroducedInstanceFields();

    inline DWORD GetAlignedNumInstanceFieldBytes();


    // Note: This flag MUST be available even from an unrestored MethodTable - see GcScanRoots in siginfo.cpp.
    DWORD           ContainsPointers()  
    {
        LEAF_CONTRACT;
        return(m_wFlags & enum_flag_ContainsPointers); 
    }
    BOOL            IsNotTightlyPacked()
    {
        LEAF_CONTRACT;
        return (m_wFlags & enum_flag_NotTightlyPacked); 
    }
    
    void SetContainsPointers()
    {
        LEAF_CONTRACT;
        m_wFlags |= enum_flag_ContainsPointers;
    }
    void SetNotTightlyPacked()
    {
        LEAF_CONTRACT;
        m_wFlags |= enum_flag_NotTightlyPacked;
    }


    //-------------------------------------------------------------------
    // FIELD DESCRIPTORS
    //
    // Most of this API still lives on EEClass.  
    //
    // ************************************ WARNING *************
    // **   !!!!INSTANCE FIELDDESCS ARE REPRESENTATIVES!!!!!   **
    // ** THEY ARE SHARED BY COMPATIBLE GENERIC INSTANTIATIONS **
    // ************************************ WARNING *************

#ifndef DACCESS_COMPILE
    // This goes straight to the EEClass 
    // Careful about using this method. If it's possible that fields may have been added via EnC, then
    // must use the FieldDescIterator as any fields added via EnC won't be in the raw list
    inline FieldDesc *GetApproxFieldDescListRaw();
#endif // !DACCESS_COMPILE

    // This returns a type-exact FieldDesc for a static field, but may still return a representative
    // for a non-static field.
    FieldDesc *GetFieldDescByIndex(DWORD fieldIndex);

    DWORD GetIndexForFieldDesc(FieldDesc *pField);

    //-------------------------------------------------------------------
    // REMOTING and THUNKING.  
    //
    // We find a lot of information from the VTable.  But sometimes the VTable is a
    // thunking layer rather than the true type's VTable.  For instance, context
    // proxies use a single VTable for proxies to all the types we've loaded.
    // The following service adjusts a MethodTable based on the supplied instance.  As
    // we add new thunking layers, we just need to teach this service how to navigate
    // through them.

    inline BOOL IsThunking()
    { 
        LEAF_CONTRACT; 

        // return IsTransparentProxyType(); 
        return m_wFlags & (enum_flag_TransparentProxy);
    }

    // mark as transparent proxy type
    void SetTransparentProxyType();

    int IsTransparentProxyType()
    {
        LEAF_CONTRACT;
        return m_wFlags & enum_flag_TransparentProxy;
    }
    // This is only used during shutdown, to suppress assertions
    void MarkAsNotThunking()
    {
        LEAF_CONTRACT;
        m_wFlags &= ~(enum_flag_TransparentProxy);
    }

    BOOL IsMarshaledByRef();
    void SetMarshaledByRef();

    BOOL IsContextful();
    
    // True if interface casts for an object having this type require more
    // than a simple scan of the interface map
    // See JIT_IsInstanceOfInterface
    inline BOOL InstanceRequiresNonTrivialInterfaceCast()
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;

        return m_wFlags & MethodTable::enum_flag_NonTrivialInterfaceCast;
    }


    //-------------------------------------------------------------------
    // PARENT INTERFACES
    //
    unsigned GetNumInterfaces()
    {
        LEAF_CONTRACT;
        return m_wNumInterfaces;
    }

    //-------------------------------------------------------------------
    // CASTING
    // 
    // There are two variants of each of these methods:
    //
    // CanCastToX
    // - restore encoded pointers on demand
    // - might throw, might trigger GC
    // - return type is boolean (FALSE = cannot cast, TRUE = can cast)
    //
    // CanCastToXNoGC
    // - do not restore encoded pointers on demand
    // - does not throw, does not trigger GC
    // - return type is three-valued (CanCast, CannotCast, MaybeCast)
    // - MaybeCast indicates that the test tripped on an encoded pointer
    //   so the caller should now call CanCastToXRestoring if it cares
    // 
    BOOL CanCastToInterface(MethodTable *pTargetMT);
    BOOL CanCastToClass(MethodTable *pTargetMT);
    BOOL CanCastToClassOrInterface(MethodTable *pTargetMT);
    BOOL CanCastByVarianceToInterfaceOrDelegate(MethodTable *pTargetMT);

    BOOL CanCastToNonVariantInterface(MethodTable *pTargetMT);

    TypeHandle::CastResult CanCastToInterfaceNoGC(MethodTable *pTargetMT);
    TypeHandle::CastResult CanCastToClassNoGC(MethodTable *pTargetMT);
    TypeHandle::CastResult CanCastToClassOrInterfaceNoGC(MethodTable *pTargetMT);

    //-------------------------------------------------------------------
    // THE METHOD TABLE PARENT (SUPERCLASS/BASE CLASS)
    //

    BOOL HasApproxParent()
    {
        LEAF_CONTRACT;
        return (GetWriteableData()->m_dwFlags & MethodTableWriteableData::enum_flag_HasApproxParent) != 0;
    }
    inline void SetHasExactParent()
    {
        WRAPPER_CONTRACT;
        FastInterlockAnd(&(GetWriteableData()->m_dwFlags), ~MethodTableWriteableData::enum_flag_HasApproxParent);
    }

    inline void SetHasApproxParent()
    {
        WRAPPER_CONTRACT;
        FastInterlockOr(&(GetWriteableData()->m_dwFlags), MethodTableWriteableData::enum_flag_HasApproxParent);
    }


    // Caller must know that the parent method table is not an encoded fixup
    inline PTR_MethodTable GetParentMethodTable()
    {
        WRAPPER_CONTRACT;
        PRECONDITION(IsParentMethodTableValid());
        return m_pParentMethodTable;
    }

    // Caller is responsible for dealing with encoded fixup
    inline MethodTable* GetPossiblyEncodedParentMethodTable()
    {
        LEAF_CONTRACT;
        return m_pParentMethodTable;
    }

    // Is the parent method table pointer equal to the given argument?
    // This method has a GC_NOTRIGGER/NOTHROW contract, but can deal with the parent being an encoded fixup
    BOOL ParentEquals(MethodTable *pMT);

    bool IsParentMethodTableValid()
    {
        return !m_pParentMethodTable
            || !CORCOMPILE_IS_TOKEN_TAGGED(
                reinterpret_cast< SIZE_T >( static_cast< MethodTable const * >( m_pParentMethodTable ) ) );
    }

#ifndef DACCESS_COMPILE
    void SetParentMethodTable (MethodTable *pParentMethodTable)
    {
        LEAF_CONTRACT;
        m_pParentMethodTable = pParentMethodTable;
    }
#endif // !DACCESS_COMPILE
    TypeHandle* GetInstantiationOfParentClass(MethodTable *pWhichParent);

    //-------------------------------------------------------------------
    // THE  EEClass (Possibly shared between instantiations!).
    //
    // Note that it is not generally the case that GetClass.GetMethodTable() == t.

    EEClass* GetClass();

    //-------------------------------------------------------------------
    // This one can return null in the special case that we are looking at a
    // phantom methodtable created for the purpose of default domain migration.
    // Only our objref validation routines peek at the EEClasses of these methodtables
    // and only they should use this entrypoint.
    EEClass* GetClassIfPresent();

    // We have this version just so if anyone thinks of adding logging to GetModule then
    // we record the places where we should use the NoLogging variant.
    inline EEClass* GetClass_NoLogging();

    EEClass* GetClassWithPossibleAV();

    BOOL ValidateWithPossibleAV();

#ifndef DACCESS_COMPILE
    inline EEClass** GetClassPtr()
    {
        LEAF_CONTRACT;
        return &m_pEEClass;
    }
#endif // !DACCESS_COMPILE
    static UINT32 GetOffsetOfEEClass()
    {
        LEAF_CONTRACT;
        return offsetof(MethodTable, m_pEEClass);
    }

    //-------------------------------------------------------------------
    // CONSTRUCTION
    //
    // Do not call the following at any time except when creating a method table.
    // One day we will have proper constructors for method tables and all these
    // will disappear.
    
    inline void SetClass(EEClass *pClass)
    {
        LEAF_CONTRACT;
#ifndef DACCESS_COMPILE
        m_pEEClass = pClass;
#endif
    }
    inline void SetHasInstantiation(BOOL typical);

    //-------------------------------------------------------------------
    // INTERFACE IMPLEMENTATION
    //
    // If a method table implements an interface then the following functions
    // gather information about which slots implement the interface.
    //
    // Much of this will go away once we have virtual stub dispatch
    // implemented on all platforms.
    
 private:
    InterfaceInfo_t* FindInterface(MethodTable *pInterface);
 public:
    BOOL ImplementsInterface(MethodTable *pInterface)
    {
        WRAPPER_CONTRACT;
        return FindInterface(pInterface) != NULL;
    }

    MethodDesc *GetMethodDescForInterfaceMethod(TypeHandle ownerType, MethodDesc *pInterfaceMD);
    MethodDesc *GetMethodDescForInterfaceMethod(MethodDesc *pInterfaceMD); // You can only use this one for non-generic interfaces
    
    



    //-------------------------------------------------------------------
    // INTERFACE MAP.  
    //

    inline PTR_InterfaceInfo GetInterfaceMap();

#ifdef _DEBUG
    inline InterfaceInfo_t** GetIMapDEBUGPtr();
#endif
#ifndef DACCESS_COMPILE
    void SetInterfaceMap(WORD wNumInterfaces, InterfaceInfo_t* iMap);
#endif

    inline int HasInterfaceMap()
    {
        LEAF_CONTRACT;
        return (m_wNumInterfaces != 0);
    }

    // Fast scan for an already-restored interface, used by fast cast paths 
    inline BOOL FindRestoredInterfaceInMap(MethodTable *pMT)
    {
        InterfaceInfo_t *pInfo = GetInterfaceMap();
        for (DWORD i = 0; i < GetNumInterfaces(); i++)
        {
            if (pInfo->m_pMethodTable == pMT)
                return TRUE;
            pInfo++;
        }
        return FALSE;
    }

    // Where possible, use this iterator over the interface map instead of accessing the map directly
    // That way we can easily change the implementation of the map
    class InterfaceMapIterator
    {
        friend class MethodTable;

    private:
        PTR_InterfaceInfo m_pMap;
        DWORD m_i;
        DWORD m_count;

        InterfaceMapIterator(MethodTable *pMT)
          : m_pMap(pMT->GetInterfaceMap()),
            m_i((DWORD) -1),
            m_count(pMT->GetNumInterfaces())
        {
            WRAPPER_CONTRACT;
        }

        InterfaceMapIterator(MethodTable *pMT, DWORD index)
          : m_pMap(pMT->GetInterfaceMap() + index),
            m_i(index),
            m_count(pMT->GetNumInterfaces())
        {
            WRAPPER_CONTRACT;
            CONSISTENCY_CHECK(index >= 0 && index < m_count);
        }

        void Restore();
        InterfaceInfo_t* GetInterfaceInfo()
        {
            LEAF_CONTRACT;
            return m_pMap;
        }

    public:
        // Move to the next item in the map, returning TRUE if an item
        // exists or FALSE if we've run off the end
        inline BOOL Next()
        {
            LEAF_CONTRACT;
            PRECONDITION(!Finished());
            if (m_i != (DWORD) -1)
                m_pMap++;
            return (++m_i < m_count);
        }

        // Have we iterated over all of the items?
        BOOL Finished()
        {
            return (m_i == m_count);
        }

        // Get the interface at the current position, restoring it if it is encoded
        // Note the contract!
        inline MethodTable *GetInterface()
        {
            CONTRACT(MethodTable*)
            {
                GC_TRIGGERS;
                if (g_pConfig->LazyInterfaceRestore()) THROWS; else NOTHROW;
                PRECONDITION(m_i != (DWORD) -1 && m_i < m_count);
                POSTCONDITION(CheckPointer(RETVAL));
            }
            CONTRACT_END;

            if (g_pConfig->LazyInterfaceRestore())
            {
                Restore();
            }
            
            RETURN (m_pMap->m_pMethodTable);
        }

        // Is the interface at the current position restored?
        BOOL IsRestored();

        void SetInterface(MethodTable *pMT)
        {
            WRAPPER_CONTRACT;
            m_pMap->m_pMethodTable = pMT;
        }

        DWORD GetIndex()
        {
            LEAF_CONTRACT;
            return m_i;            
        }

        BOOL InterfaceEquals(MethodTable *pMT);

        BOOL IsImplementedByParent()
        {
            WRAPPER_CONTRACT;
            return GetInterfaceInfo()->IsImplementedByParent();
        }

        BOOL IsDeclaredOnClass()
        {
            WRAPPER_CONTRACT;
            return GetInterfaceInfo()->IsDeclaredOnClass();
        }

    };

    // Create a new iterator over the interface map
    // The iterator starts just before the first item in the map
    InterfaceMapIterator IterateInterfaceMap()
    {
        WRAPPER_CONTRACT;
        return InterfaceMapIterator(this);
    }

    // Create a new iterator over the interface map, starting at the index specified
    InterfaceMapIterator IterateInterfaceMapFrom(DWORD index)
    {
        WRAPPER_CONTRACT;
        return InterfaceMapIterator(this, index);
    }

    //-------------------------------------------------------------------
    // VIRTUAL/INTERFACE CALL RESOLUTION
    //
    // These should probably go in method.hpp since they don't have 
    // much to do with method tables per se.
    //

    // get the address of code given the method desc and server
    static const BYTE *GetTargetFromMethodDescAndServer(TypeHandle ownerType, MethodDesc *pMD, OBJECTREF *ppServer, BOOL fContext, MethodDesc **ppTargetMD=NULL);

    // get the method desc given the interface method desc
    static MethodDesc *GetMethodDescForInterfaceMethodAndServer(TypeHandle ownerType, MethodDesc *pItfMD, OBJECTREF *pServer);



    // Try a partial resolve of the constraint call, up to generic code sharing.  
    //
    // Note that this will not necessarily resolve the call exactly, since we might be compiling
    // shared generic code - it may just resolve it to a candidate suitable for
    // JIT compilation, and require a runtime lookup for the actual code pointer 
    // to call.
    //
    // Return NULL if the call could not be resolved, e.g. because it is invoked
    // on a type that inherits the implementation of the method from System.Object
    // or System.ValueType.
    //
    // Always returns an unboxed entry point with a uniform calling convention.
    MethodDesc * TryResolveConstraintMethodApprox(TypeHandle ownerType,
                                                  MethodDesc *pMD);


    //-------------------------------------------------------------------
    // CONTRACT IMPLEMENTATIONS
    //

    inline BOOL HasDispatchMap()
    {
        LEAF_CONTRACT;
        return m_wFlags2 & enum_flag2_HasDispatchMap;
    }

    inline void SetHasDispatchMap()
    {
        LEAF_CONTRACT;
        // Only set it if it's not already set. This can happen when fixing up
        // non-canonical MethodTables.
        if (!(m_wFlags2 & enum_flag2_HasDispatchMap))
            m_wFlags2 |= enum_flag2_HasDispatchMap;
    }

    inline DispatchMap *GetDispatchMap()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(HasDispatchMap());
        } CONTRACTL_END;
        CONSISTENCY_CHECK(CheckPointer(m_pDispatchMap, NULL_OK));
        return PTR_DispatchMap((TADDR)m_pDispatchMap);
    }



#ifndef DACCESS_COMPILE
    void SetDispatchMap(DispatchMap *pDispatchMap)
    {
        LEAF_CONTRACT;
        CONSISTENCY_CHECK(CheckPointer(pDispatchMap));
        m_pDispatchMap = pDispatchMap; 
        g_IBCLogger.LogMethodTableWriteAccess(this);
        SetHasDispatchMap();
    }

#endif // !DACCESS_COMPILE

protected:

    DispatchSlot ResolveNonVirtualEntry(UINT32 iInheritanceDelta, UINT32 iSlotIndex);
    DispatchSlot ResolveNonVirtualEntry(DispatchMapEntry *pDE);

    BOOL FindEncodedMapDispatchEntry(UINT32 typeID,
                                     UINT32 slotNumber,
                                     DispatchMapEntry *pEntry,
                                     BOOL fIgnoreVirtualMappings);

    BOOL FindIntroducedImplementationTableDispatchEntry(UINT32 slotNumber,
                                                        DispatchMapEntry *pEntry,
                                                        BOOL fVirtualMethodsOnly);

    BOOL FindDispatchEntryForCurrentType(UINT32 typeID,
                                         UINT32 slotNumber,
                                         DispatchMapEntry *pEntry,
                                         BOOL fIgnoreVirtualMappings,
                                         BOOL fVirtualMethodsOnly);

    BOOL FindDispatchEntry(UINT32 typeID,
                           UINT32 slotNumber,
                           DispatchMapEntry *pEntry,
                           BOOL fIgnoreVirtualMappings);

public:
    BOOL FindDispatchImpl(UINT32 typeID,
                          UINT32 slotNumber,
                          DispatchSlot *pDeclSlot,
                          DispatchSlot *pImplSlot);

    DispatchSlot FindDispatchSlot(UINT32 typeID, UINT32 slotNumber);

    DispatchSlot FindDispatchSlot(UINT32 slotNumber);

    DispatchSlot FindDispatchSlot(DispatchToken tok);

    // You must use the second of these two if there is any chance the pMD is a method
    // on a generic interface such as IComparable<T> (which it normally can be).  The 
    // ownerType is used to provide an exact qualification in the case the pMD is
    // a shared method descriptor.
    DispatchSlot FindDispatchSlotForInterfaceMD(MethodDesc *pMD);
    DispatchSlot FindDispatchSlotForInterfaceMD(TypeHandle ownerType, MethodDesc *pMD);

    DispatchSlot FindRepresentativeDispatchSlot(UINT32 slotNumber);

    MethodDesc *ReverseInterfaceMDLookup(UINT32 slotNumber);

    // Lookup, does not assign if not already done.
    UINT32 LookupTypeID();
    // Lookup, will assign ID if not already done.
    UINT32 GetTypeID();


    MethodTable *LookupDispatchMapType(DispatchMapTypeID typeID);

private:
   // This one can be used after load time, using MT comparisons
    static DispatchMapTypeID ComputeDispatchMapTypeID(MethodTable *pExactIntfMT, 
                                                      MethodTable *pExactImplMT);

public:
    static MethodDesc *MapMethodDeclToMethodImpl(MethodDesc *pMDDecl);

    //-------------------------------------------------------------------
    // FINALIZATION SEMANTICS
    //

    DWORD  CannotUseSuperFastHelper()
    {
        WRAPPER_CONTRACT;
        return HasFinalizer();
    }

    void MaybeSetHasFinalizer();

    void SetHasCriticalFinalizer()
    {
        LEAF_CONTRACT;
        g_IBCLogger.LogMethodTableWriteAccess(this);
        m_wFlags |= (enum_flag_HasCriticalFinalizer | enum_flag_HasFinalizer);
    }
    // Does this class have non-trivial finalization requirements?
    DWORD               HasFinalizer()
    {
        LEAF_CONTRACT;
        return (m_wFlags & enum_flag_HasFinalizer);
    }
    // Must this class be finalized during a rude appdomain unload, and
    // must it's finalizer run in a different order from normal finalizers?
    DWORD HasCriticalFinalizer() const
    {
        LEAF_CONTRACT;
        return (m_wFlags & enum_flag_HasCriticalFinalizer);
    }

    // Have the backout methods (Finalizer, Dispose, ReleaseHandle etc.) been prepared for this type? This currently only happens
    // for types derived from CriticalFinalizerObject.
    BOOL CriticalTypeHasBeenPrepared()
    {
        LEAF_CONTRACT;
        _ASSERTE(HasCriticalFinalizer());
        g_IBCLogger.LogMethodTableWriteableDataAccess(this);
        return GetWriteableData()->CriticalTypeHasBeenPrepared();
    }

    void SetCriticalTypeHasBeenPrepared()
    {
        LEAF_CONTRACT;
        _ASSERTE(HasCriticalFinalizer());
        g_IBCLogger.LogMethodTableWriteableDataWriteAccess(this);
        GetWriteableData()->SetCriticalTypeHasBeenPrepared();
    }

    //-------------------------------------------------------------------
    // STATIC FIELDS
    //

    DWORD  GetOffsetOfFirstStaticHandle();
    DWORD  GetOffsetOfFirstStaticMT();

    inline TADDR GetNonGCStaticsBasePointer();

    inline TADDR GetGCStaticsBasePointer();

    inline void SetDynamicStatics()
    {
        LEAF_CONTRACT;
        m_wFlags2 |= enum_flag2_IsDynamicStatics;
    }

    inline DWORD IsDynamicStatics()
    {
        LEAF_CONTRACT;
        return (m_wFlags2 & enum_flag2_IsDynamicStatics);
    }

    inline void SetFixedAddressStaticVTs()
    {
        LEAF_CONTRACT;
        m_wFlags2 |= enum_flag2_FixedAddressVTStatics;
    }

    inline DWORD IsFixedAddressVTStatics()
    {
        LEAF_CONTRACT;
        return (m_wFlags2 & enum_flag2_FixedAddressVTStatics);
    }


    //-------------------------------------------------------------------
    // PER-INSTANTIATION STATICS INFO
    //

    inline void SetHasGenericsStaticsInfo()
    {
        LEAF_CONTRACT;
        m_wFlags2 |= enum_flag2_HasGenericsStaticsInfo;
    }

    void SetupGenericsStaticsInfo(FieldDesc* pStaticFieldDescs);

    BOOL HasGenericsStaticsInfo()
    {
        LEAF_CONTRACT;
        return (m_wFlags2 & enum_flag2_HasGenericsStaticsInfo) != 0;
    }

    FieldDesc* GetGenericsStaticFieldDescs()
    {
        WRAPPER_CONTRACT;
        if (HasGenericsStaticsInfo())
            return GetGenericsStaticsInfo()->m_pFieldDescs;
        else
            return NULL;
    }

    WORD GetNumHandleStatics();

    WORD GetNumBoxedStatics ();

    //-------------------------------------------------------------------
    // DYNAMIC ID
    //

    // Used for generics and reflection emit in memory
    DWORD GetModuleDynamicEntryID();
    Module* GetModuleForStatics();

    //-------------------------------------------------------------------
    // GENERICS DICT INFO
    //

    inline BOOL HasGenericsDictInfo()
    {
        WRAPPER_CONTRACT;
        return HasPerInstInfo();
    }

    // Number of generic arguments, whether this is a method table for 
    // a generic type instantiation, e.g. List<string> or the "generic" MethodTable
    // e.g. for List.
    inline DWORD GetNumGenericArgs()
    {
        WRAPPER_CONTRACT;
        if (HasInstantiation())
            return (DWORD) (GetGenericsDictInfo()->m_wNumTyPars);
        else
            return 0;
    }

    inline DWORD GetNumDicts()
    {
        if (GetPerInstInfo() != NULL)
        {
            GenericsDictInfo* pDictInfo = GetGenericsDictInfo();
            return (DWORD) (pDictInfo->m_wNumDicts);
        }
        else
            return 0;
    }

    //-------------------------------------------------------------------
    // OBJECTS 
    //

    OBJECTREF Allocate();
    
    // This flavor of Allocate is more efficient, but can only be used
    // if IsRestored(), CheckInstanceActivated(), IsClassInited() are known to be true.
    // A sufficient condition is that another instance of the exact same type already
    // exists in the same appdomain. It's currently called only from Delegate.Combine
    // via COMDelegate::InternalAllocLike.
    OBJECTREF AllocateNoChecks();

    OBJECTREF Box(void* data, BOOL mayHaveRefs = TRUE);
    OBJECTREF FastBox(void** data);
    BOOL UnBoxInto(void *dest, OBJECTREF src);
    void UnBoxIntoUnchecked(void *dest, OBJECTREF src);

    // Used for debugging class layout. Dumps to the debug console
    // when debug is true.
    void DebugDumpVtable(LPCUTF8 pszClassName, BOOL debug);

    inline BOOL IsAgileAndFinalizable()
    {
        LEAF_CONTRACT;
        // Right now, System.Thread is the only case of this. 
        // Things should stay this way - please don't change without talking to EE team.
        return this == g_pThreadClass;
    }


    //-------------------------------------------------------------------
    // ENUMS, DELEGATES, VALUE TYPES, ARRAYS
    //

    // GetInternalCorElementType() retrieves the internal representation of the type. It's
    // not always appropiate to use this. For example, we treat enums as their underlying type
    // , or some structs are optimized to be ints. To get the signature type 
    // or the verifier type (same as signature except for enums), use the APIs in Typehandle.h
    //      
    //      TypeHandle th(MethodTable)
    //      th.GetSignatureCorElementType()
    //   or th.GetVertifierCorElementType()
    //   or th.GetInternalCorElementType()
    inline CorElementType  GetInternalCorElementType();
    void SetInternalCorElementType(CorElementType _NormType, BOOL isTruePrimitive = FALSE);

    // A true primitive is one who's GetVerifierCorElementType() == 
    //      ELEMENT_TYPE_I, 
    //      ELEMENT_TYPE_I4, 
    //      ELEMENT_TYPE_TYPEDREF etc.
    // Note that GetIntenalCorElementType might return these same values for some additional
    // types such as Enums and some structs.
    inline BOOL IsTruePrimitive();

    inline BOOL IsAnyDelegateClass ()
    {
        LEAF_CONTRACT;
        return ParentEquals(g_pDelegateClass) || 
               ParentEquals(g_pMultiDelegateClass);
    }
    inline BOOL IsAnyDelegateExact()
    {
        LEAF_CONTRACT;
        return IsSingleDelegateExact() || IsMultiDelegateExact();
    }
    
    inline BOOL IsSingleDelegateExact()
    {
        LEAF_CONTRACT;
        return this == g_pDelegateClass;
    }
    
    inline BOOL IsMultiDelegateExact()
    {
        LEAF_CONTRACT;
        return this == g_pMultiDelegateClass;
    }

    // Is this System.Object?
    inline BOOL IsObjectClass()
    {
        LEAF_CONTRACT;
        _ASSERTE(g_pObjectClass);
        return (this == g_pObjectClass);
    }
    // Is this System.ValueType?
    inline DWORD IsValueTypeClass()
    {
        LEAF_CONTRACT;
        _ASSERTE(g_pValueTypeClass);
        return (this == g_pValueTypeClass);
    }

    // For internal use only, currently only public because they
    // are used by class.h
    static inline BOOL IsChildValueType(MethodTable *pParentMT);
    static inline BOOL IsChildEnum(MethodTable *pParentMT);
    
    // "IsValueType/IsValueClass" both mean "Is ValueType, including Enums".
    // IsValueClass goes straight to the EEClass and is thus not optimized
    // for working set. Use IsValueType instead.  
    inline BOOL IsValueClass();

    inline BOOL IsValueType();
          
    inline BOOL IsEnum();

    inline BOOL IsArray()           
    { 
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        return(m_wFlags & enum_flag_Array); 
    }
    inline BOOL IsMultiDimArray()
    {
        STATIC_CONTRACT_SO_TOLERANT;
        LEAF_CONTRACT;
        PRECONDITION(IsArray());
        return !(m_wFlags & enum_flag_IfArrayThenSzArray);
    }
    
    inline BOOL IsSharedByReferenceArrayTypes() 
    {
        LEAF_CONTRACT;
        PRECONDITION(IsArray());
        return (m_wFlags & enum_flag_IfArrayThenSharedByReferenceTypes);
    }

    inline void SetSharedByReferenceArrayTypes()
    {
        LEAF_CONTRACT;
        PRECONDITION(IsArray());
        m_wFlags |= enum_flag_IfArrayThenSharedByReferenceTypes;
    }

    inline BOOL IsStructMarshalable() 
    {
        LEAF_CONTRACT;
        return m_wFlags & enum_flag_IsMarshalable; 
    }

    inline void SetStructMarshalable()
    {
        LEAF_CONTRACT;
        m_wFlags |= enum_flag_IsMarshalable;
    }
    
    // The following methods are only valid for the 
    // method tables for array types.  These MTs may 
    // be shared between array types and thus GetArrayElementTypeHandle
    // may only be approximate.  If you need the exact element type handle then
    // you should probably be calling GetArrayElementTypeHandle on a TypeHandle,
    // or an ArrayTypeDesc, or on an object reference that is known to be an array,
    // e.g. a BASEARRAYREF.
    //
    // At the moment only the object[] MethodTable is shared between array types.
    // In the future the amount of sharing of method tables is likely to be increased.
    inline CorElementType GetArrayElementType(); 
    inline TypeHandle GetApproxArrayElementTypeHandle(); 
    inline TypeHandle GetApproxArrayElementTypeHandle_NoLogging(); 
    inline DWORD GetRank(); 

    //-------------------------------------------------------------------
    // UNDERLYING METADATA
    //


    // Get the token for the metadata for the corresponding type declaration
    mdTypeDef GetCl();

    // Get the MD Import for the metadata for the corresponding type declaration
    IMDInternalImport* GetMDImport();
    

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    //-------------------------------------------------------------------
    // REMOTEABLE METHOD INFO
    //

    BOOL    HasRemotableMethodInfo();

    PTR_CrossDomainOptimizationInfo GetRemotableMethodInfo()
    {
        WRAPPER_CONTRACT;
        _ASSERTE(HasRemotableMethodInfo());
        return *GetRemotableMethodInfoPtr();
    }
    void SetupRemotableMethodInfo(BaseDomain *pDomain, AllocMemTracker *pamTracker);
        
    static MethodTable *GetISerializableMT();
    static MethodTable *GetIDeserializationCBMT();
    static MethodTable *GetIObjectReferenceMT();

    //-------------------------------------------------------------------
    // REMOTING VTS INFO
    //
    // This optional addition to MethodTables allows us to locate VTS (Version
    // Tolerant Serialization) event callback methods and optionally
    // serializable fields quickly. We also store the NotSerialized field
    // information in here so remoting can avoid one more touch of the metadata
    // during cross appdomain cloning.
    //

    void SetHasRemotingVtsInfo();
    BOOL HasRemotingVtsInfo();
    PTR_RemotingVtsInfo GetRemotingVtsInfo();
    PTR_RemotingVtsInfo AllocateRemotingVtsInfo(BaseDomain *pDomain, AllocMemTracker *pamTracker, DWORD dwNumFields);

    // The following two methods produce correct results only if this type is
    // marked Serializable (verified by assert in checked builds) and the field
    // in question was introduced in this type (the index is the FieldDesc
    // index).
    BOOL IsFieldNotSerialized(DWORD dwFieldIndex);
    BOOL IsFieldOptionallySerialized(DWORD dwFieldIndex);

    //
    //
    //
    //
    //
    //

    DPTR(PTR_Dictionary) GetPerInstInfo()
    {
        LEAF_CONTRACT;
        return m_pPerInstInfo;
    }
    BOOL HasPerInstInfo()
    {
        WRAPPER_CONTRACT;
        return GetPerInstInfo() != NULL;
    }
#ifndef DACCESS_COMPILE
    Dictionary*** GetPerInstInfoPtr()
    {
        LEAF_CONTRACT;
        return &m_pPerInstInfo; // for ngen fixup
    }
    static inline DWORD GetOffsetOfPerInstInfo()
    {
        LEAF_CONTRACT;
        return offsetof(MethodTable, m_pPerInstInfo);
    }
    void SetPerInstInfo(Dictionary** pPerInstInfo)
    {
        LEAF_CONTRACT;
        m_pPerInstInfo = pPerInstInfo;
    }
    void SetDictInfo(DWORD numDicts, DWORD numTyPars)
    {
        WRAPPER_CONTRACT;
        if (numDicts)
        {
            GenericsDictInfo* pInfo = GetGenericsDictInfo();
            pInfo->m_wNumDicts  = (WORD) numDicts;
            pInfo->m_wNumTyPars = (WORD) numTyPars;
        }
    }
#endif // !DACCESS_COMPILE

    // Get a pointer to the dictionary for this instantiated type
    // (The instantiation is stored in the initial slots of the dictionary)
    // If not instantiated, return NULL
    Dictionary* GetDictionary();





    inline BOOL IsSerializable();
    inline BOOL CannotBeBlittedByObjectCloner();
    inline BOOL HasFieldsWhichMustBeInited();

    //-------------------------------------------------------------------
    // SECURITY SEMANTICS 
    //


    BOOL IsNoSecurityProperties()
    {
        LEAF_CONTRACT;
        return (m_wFlags2 & enum_flag2_NoSecurityProperties);
    }

    void SetNoSecurityProperties()
    {
        LEAF_CONTRACT;
        m_wFlags2 |= enum_flag2_NoSecurityProperties;
    }

    BOOL IsAsyncPinType()
    {
        LEAF_CONTRACT;
        return m_wFlags2 == enum_flag2_IsAsyncPin;
    }

    inline BOOL MayNeedRestore() const
    {
        LEAF_CONTRACT;
        return (m_wFlags2 & enum_flag2_MayNeedRestore);
    }

    //-------------------------------------------------------------------
    // THE EXPOSED CLASS OBJECT
    //
    /*
     * m_ExposedClassObject is a RuntimeType instance for this class.  But
     * do NOT use it for Arrays or remoted objects!  All arrays of objects 
     * share the same MethodTable/EEClass.  -- BrianGru, 9/11/2000
     * @GENERICS: this is per-instantiation data
     */
    // There are two version of GetManagedClassObject.  The GetManagedClassObject()
    //  method will get the class object.  If it doesn't exist it will be created.
    //  GetManagedClassObjectIfExists() will return null if the Type object doesn't exist.
    OBJECTREF      GetManagedClassObject();
    FORCEINLINE OBJECTREF      GetManagedClassObjectIfExists() 
    {
        LEAF_CONTRACT;
        _ASSERTE(GetWriteableData());
        OBJECTREF* pExposedClassObject = GetWriteableData()->m_pExposedClassObject;
        if (pExposedClassObject == NULL)
            return NULL;
        else
            return *pExposedClassObject;
    }    
   
    // ------------------------------------------------------------------
    // Private part of MethodTable
    // ------------------------------------------------------------------

    inline void SetWriteableData(PTR_MethodTableWriteableData pMTWriteableData)
    {
        _ASSERTE(pMTWriteableData);
        m_pWriteableData = pMTWriteableData;
    }
    
    inline PTR_MethodTableWriteableData GetWriteableData() const
    {
        return m_pWriteableData;
    }

    //-------------------------------------------------------------------
    // Remoting related
    // 
    inline BOOL IsRemotingConfigChecked()
    {
        WRAPPER_CONTRACT;
        return GetWriteableData()->IsRemotingConfigChecked();
    }
    inline void SetRemotingConfigChecked()
    {
        WRAPPER_CONTRACT;
        g_IBCLogger.LogMethodTableWriteableDataWriteAccess(this);
        GetWriteableData()->SetRemotingConfigChecked();
    }
    inline BOOL RequiresManagedActivation()
    {
        WRAPPER_CONTRACT;
        return GetWriteableData()->RequiresManagedActivation();
    }
    inline void SetRequiresManagedActivation()
    {
        WRAPPER_CONTRACT;
        g_IBCLogger.LogMethodTableWriteableDataWriteAccess(this);        
        GetWriteableData()->SetRequiresManagedActivation();
    }

    //-------------------------------------------------------------------
    // Misc.
    //

    void ClearFlags()
    {
        LEAF_CONTRACT;
        m_wFlags &= 0xFFFF;
        m_wFlags2 = 0;
    }
    static DWORD GetOffsetOfFlags()
    {
        LEAF_CONTRACT;
        return offsetof(MethodTable, m_wFlags);
    }


    //-------------------------------------------------------------------
    // The GUID Info 
    //
    //<NICE> Document what this is and what it is for </NICE>

    GuidInfo *GetGuidInfo();
    void SetGuidInfo(GuidInfo* pGuidInfo);

    void    GetGuid(GUID *pGuid, BOOL bGenerateIfNotFound);

    //-------------------------------------------------------------------
    // Debug Info 
    //


#ifdef _DEBUG
    inline LPCUTF8 GetDebugClassName() 
    { 
        LEAF_CONTRACT;
        return m_szDebugClassName; 
    }
    inline void SetDebugClassName(LPCUTF8 name) 
    { 
        LEAF_CONTRACT;
        m_szDebugClassName = name; 
    }
#endif // _DEBUG


#ifndef DACCESS_COMPILE
public:
    //--------------------------------------------------------------------------------------
    class MethodData
    {
      public:
        inline ULONG AddRef()
            { LEAF_CONTRACT; return (ULONG) InterlockedIncrement((volatile LONG *)&m_cRef); }

        ULONG Release();

        // Since all methods that return a MethodData already AddRef'd, we do NOT
        // want to AddRef when putting a holder around it. We only want to release it.
        static void HolderAcquire(MethodData *pEntry)
            { LEAF_CONTRACT; return; }
        static void HolderRelease(MethodData *pEntry)
            { WRAPPER_CONTRACT; if (pEntry != NULL) pEntry->Release(); }

      protected:
        ULONG m_cRef;

      public:
        MethodData() : m_cRef(1) { LEAF_CONTRACT; }
        virtual ~MethodData() { LEAF_CONTRACT; }

        virtual MethodData  *GetDeclMethodData() = 0;
        virtual MethodTable *GetDeclMethodTable() = 0;
        virtual MethodDesc  *GetDeclMethodDesc(UINT32 slotNumber) = 0;
        
        virtual MethodData  *GetImplMethodData() = 0;
        virtual MethodTable *GetImplMethodTable() = 0;
        virtual DispatchSlot GetImplSlot(UINT32 slotNumber) = 0;
        virtual MethodDesc  *GetImplMethodDesc(UINT32 slotNumber) = 0;

        virtual UINT32 GetNumVirtuals() = 0;
        virtual UINT32 GetNumMethods() = 0;

      protected:
        static const UINT32 INVALID_SLOT_NUMBER = UINT32_MAX;

        // This is used when building the data
        struct MethodDataEntry
        {
          private:
            static const UINT32 INVALID_CHAIN_AND_INDEX = (UINT32)(-1);
            static const UINT16 INVALID_IMPL_SLOT_NUM = (UINT16)(-1);

            // This contains both the chain delta and the table index. The
            // reason that they are combined is that we need atomic update
            // of both, and it is convenient that both are on UINT16 in size.
            UINT32           m_chainDeltaAndTableIndex;
            UINT16           m_implSlotNum;     // For virtually remapped slots
            DispatchSlot     m_slot;            // The entry in the DispatchImplTable
            MethodDesc      *m_pMD;             // The MethodDesc for this slot

          public:
            inline MethodDataEntry() : m_slot(NULL)
                { WRAPPER_CONTRACT; Init(); }

            inline void Init()
            {
                LEAF_CONTRACT;
                m_chainDeltaAndTableIndex = INVALID_CHAIN_AND_INDEX;
                m_implSlotNum = INVALID_IMPL_SLOT_NUM;
                m_slot = NULL;
                m_pMD = NULL;
            }

            inline BOOL IsDeclInit()
                { LEAF_CONTRACT; return m_chainDeltaAndTableIndex != INVALID_CHAIN_AND_INDEX; }
            inline BOOL IsImplInit()
                { LEAF_CONTRACT; return m_implSlotNum != INVALID_IMPL_SLOT_NUM; }

            inline void SetDeclData(UINT32 chainDelta, UINT32 tableIndex)
                { LEAF_CONTRACT; m_chainDeltaAndTableIndex = ((((UINT16) chainDelta) << 16) | ((UINT16) tableIndex)); }
            inline UINT32 GetChainDelta()
                { LEAF_CONTRACT; CONSISTENCY_CHECK(IsDeclInit()); return m_chainDeltaAndTableIndex >> 16; }
            inline UINT32 GetTableIndex()
                { LEAF_CONTRACT; CONSISTENCY_CHECK(IsDeclInit()); return (m_chainDeltaAndTableIndex & (UINT32)UINT16_MAX); }

            inline void SetImplData(UINT32 implSlotNum)
                { LEAF_CONTRACT; m_implSlotNum = (UINT16) implSlotNum; }
            inline UINT32 GetImplSlotNum()
                { LEAF_CONTRACT; CONSISTENCY_CHECK(IsImplInit()); return m_implSlotNum; }

            inline void SetSlot(DispatchSlot slot)
                { LEAF_CONTRACT; m_slot = slot; }
            inline DispatchSlot GetSlot()
                { LEAF_CONTRACT; return m_slot; }

            inline void SetMethodDesc(MethodDesc *pMD)
                { LEAF_CONTRACT; m_pMD = pMD; }
            inline MethodDesc *GetMethodDesc()
                { LEAF_CONTRACT; return m_pMD; }
                
        };

        static void ProcessMap(DispatchMapTypeID typeID,
                               MethodTable *pMT,
                               UINT32 cCurrentChainDepth,
                               MethodDataEntry *rgWorkingData);
    };

    typedef ::Holder < MethodData *, MethodData::HolderAcquire, MethodData::HolderRelease > MethodDataHolder;
    typedef ::Wrapper < MethodData *, MethodData::HolderAcquire, MethodData::HolderRelease > MethodDataWrapper;

protected:
    //--------------------------------------------------------------------------------------
    class MethodDataObject : public MethodData
    {
      public:
        // Static method that returns the amount of memory to allocate for a particular type.
        static UINT32 GetObjectSize(MethodTable *pMT);

        // Constructor. Make sure you have allocated enough memory using GetObjectSize.
        inline MethodDataObject(MethodTable *pMT)
            { WRAPPER_CONTRACT; Init(pMT, NULL); }

        inline MethodDataObject(MethodTable *pMT, MethodData *pParentData)
            { WRAPPER_CONTRACT; Init(pMT, pParentData); }

        virtual ~MethodDataObject() { LEAF_CONTRACT; }

        virtual MethodData  *GetDeclMethodData()
            { LEAF_CONTRACT; return this; }
        virtual MethodTable *GetDeclMethodTable()
            { LEAF_CONTRACT; return m_pMT; }
        virtual MethodDesc *GetDeclMethodDesc(UINT32 slotNumber);

        virtual MethodData  *GetImplMethodData()
            { LEAF_CONTRACT; return this; }
        virtual MethodTable *GetImplMethodTable()
            { LEAF_CONTRACT; return m_pMT; }
        virtual DispatchSlot GetImplSlot(UINT32 slotNumber);
        virtual MethodDesc  *GetImplMethodDesc(UINT32 slotNumber);

        virtual UINT32 GetNumVirtuals()
            { LEAF_CONTRACT; return m_pMT->GetNumVirtuals(); }
        virtual UINT32 GetNumMethods()
            { LEAF_CONTRACT; return m_pMT->GetCanonicalMethodTable()->GetNumMethods(); }

      protected:
        void Init(MethodTable *pMT, MethodData *pParentData);

        // This is the method table for the actual type we're gathering the data for
        MethodTable *m_pMT;

        // This is used when building the data
        struct MethodDataObjectEntry
        {
          private:
            MethodDesc *m_pMDDecl;
            MethodDesc *m_pMDImpl;

          public:
            inline MethodDataObjectEntry() : m_pMDDecl(NULL), m_pMDImpl(NULL) {}

            inline void SetDeclMethodDesc(MethodDesc *pMD)
                { LEAF_CONTRACT; m_pMDDecl = pMD; }
            inline MethodDesc *GetDeclMethodDesc()
                { LEAF_CONTRACT; return m_pMDDecl; }
            inline void SetImplMethodDesc(MethodDesc *pMD)
                { LEAF_CONTRACT; m_pMDImpl = pMD; }
            inline MethodDesc *GetImplMethodDesc()
                { LEAF_CONTRACT; return m_pMDImpl; }
        };

        //
        // At the end of this object is an array, so you cannot derive from this class.
        //

        inline MethodDataObjectEntry *GetEntryData()
            { LEAF_CONTRACT; return (MethodDataObjectEntry *)(this + 1); }

        inline MethodDataObjectEntry *GetEntry(UINT32 i)
            { LEAF_CONTRACT; CONSISTENCY_CHECK(i < GetNumMethods()); return GetEntryData() + i; }

        // MethodDataObjectEntry m_rgEntries[...];
    };

    //--------------------------------------------------------------------------------------
    class MethodDataInterface : public MethodData
    {
      public:
        // Static method that returns the amount of memory to allocate for a particular type.
        static UINT32 GetObjectSize(MethodTable *pMT)
            { LEAF_CONTRACT; return sizeof(MethodDataInterface); }

        // Constructor. Make sure you have allocated enough memory using GetObjectSize.
        MethodDataInterface(MethodTable *pMT)
        {
            LEAF_CONTRACT;
            CONSISTENCY_CHECK(CheckPointer(pMT));
            CONSISTENCY_CHECK(pMT->IsInterface());
            m_pMT = pMT;
        }
        virtual ~MethodDataInterface()
            { LEAF_CONTRACT; }

        //
        // Decl data
        //
        virtual MethodData  *GetDeclMethodData()
            { LEAF_CONTRACT; return this; }
        virtual MethodTable *GetDeclMethodTable()
            { LEAF_CONTRACT; return m_pMT; }
        virtual MethodDesc *GetDeclMethodDesc(UINT32 slotNumber);

        //
        // Impl data
        //
        virtual MethodData  *GetImplMethodData()
            { LEAF_CONTRACT; return this; }
        virtual MethodTable *GetImplMethodTable()
            { LEAF_CONTRACT; return m_pMT; }
        virtual DispatchSlot GetImplSlot(UINT32 slotNumber)
            { WRAPPER_CONTRACT; return DispatchSlot(PTR_TADDR(m_pMT->GetSlotPtr(slotNumber))); }
        virtual MethodDesc  *GetImplMethodDesc(UINT32 slotNumber);

        //
        // Slot count data
        //
        virtual UINT32 GetNumVirtuals()
            { LEAF_CONTRACT; return m_pMT->GetNumVirtuals(); }
        virtual UINT32 GetNumMethods()
            { LEAF_CONTRACT; return m_pMT->GetNumMethods(); }

      protected:
        // This is the method table for the actual type we're gathering the data for
        MethodTable *m_pMT;
    };

    //--------------------------------------------------------------------------------------
    class MethodDataInterfaceImpl : public MethodData
    {
      public:
        // Object construction-related methods
        static UINT32 GetObjectSize(MethodTable *pMTDecl);

        MethodDataInterfaceImpl(DispatchMapTypeID declTypeID,
                                MethodData *pDecl,
                                MethodData *pImpl);
        virtual ~MethodDataInterfaceImpl();

        // Decl-related methods
        virtual MethodData  *GetDeclMethodData()
            { LEAF_CONTRACT; return m_pDecl; }
        virtual MethodTable *GetDeclMethodTable()
            { WRAPPER_CONTRACT; return m_pDecl->GetDeclMethodTable(); }
        virtual MethodDesc  *GetDeclMethodDesc(UINT32 slotNumber)
            { WRAPPER_CONTRACT; return m_pDecl->GetDeclMethodDesc(slotNumber); }

        // Impl-related methods
        virtual MethodData  *GetImplMethodData()
            { LEAF_CONTRACT; return m_pImpl; }
        virtual MethodTable *GetImplMethodTable()
            { WRAPPER_CONTRACT; return m_pImpl->GetImplMethodTable(); }
        virtual DispatchSlot GetImplSlot(UINT32 slotNumber);
        virtual MethodDesc  *GetImplMethodDesc(UINT32 slotNumber);

        virtual UINT32 GetNumVirtuals()
            { WRAPPER_CONTRACT; return m_pDecl->GetNumVirtuals(); }
        virtual UINT32 GetNumMethods()
            { WRAPPER_CONTRACT; return m_pDecl->GetNumVirtuals(); }

      protected:
        UINT32 MapToImplSlotNumber(UINT32 slotNumber);

        BOOL PopulateNextLevel();
        void Init(DispatchMapTypeID declTypeID, MethodData *pDecl, MethodData *pImpl);

        MethodData *m_pDecl;
        MethodData *m_pImpl;

        // This is used in staged map decoding - it indicates which type we will next decode.
        DispatchMapTypeID       m_declTypeID;
        UINT32                  m_iNextChainDepth;
        static const UINT32     MAX_CHAIN_DEPTH = UINT32_MAX;

        inline UINT32 GetNextChainDepth()
            { LEAF_CONTRACT; return *((UINT32 volatile *)&m_iNextChainDepth); }

        inline void SetNextChainDepth(UINT32 iDepth)
        {
            LEAF_CONTRACT;
            if (GetNextChainDepth() < iDepth) {
                *((UINT32 volatile *)&m_iNextChainDepth) = iDepth;
            }
        }

        //
        // At the end of this object is an array, so you cannot derive from this class.
        //

        inline MethodDataEntry *GetEntryData()
            { LEAF_CONTRACT; return (MethodDataEntry *)(this + 1); }

        inline MethodDataEntry *GetEntry(UINT32 i)
            { LEAF_CONTRACT; CONSISTENCY_CHECK(i < GetNumMethods()); return GetEntryData() + i; }

        // MethodDataEntry m_rgEntries[...];
    };

    //--------------------------------------------------------------------------------------
    static MethodDataCache *s_pMethodDataCache;
    static BOOL             s_fUseParentMethodData;
    static BOOL             s_fUseMethodDataCache;
    static BOOL             s_fUseMethodDataCacheInFindDispatchImpl;

public:
    static void AllowMethodDataCaching()
        { WRAPPER_CONTRACT; CheckInitMethodDataCache(); s_fUseMethodDataCache = TRUE; }
    static void ClearMethodDataCache();
    static void AllowParentMethodDataCopy()
        { LEAF_CONTRACT; s_fUseParentMethodData = TRUE; }
    static void AllowMethodDataCacheUseInFindDispatchImpl()
        { LEAF_CONTRACT; s_fUseMethodDataCacheInFindDispatchImpl = TRUE; }
    // NOTE: The fCanCache argument determines if the resulting MethodData object can
    //       be added to the global MethodDataCache. This is used when requesting a
    //       MethodData object for a type currently being built.
    static MethodData *GetMethodData(MethodTable *pMT, BOOL fCanCache = TRUE);
    static MethodData *GetMethodData(MethodTable *pMTDecl, MethodTable *pMTImpl, BOOL fCanCache = TRUE);
    // This method is used by BuildMethodTable because the exact interface has not yet been loaded.
    // NOTE: This method does not cache the resulting MethodData object in the global MethodDataCache.
    static MethodData *GetMethodData(DispatchMapTypeID declTypeID, MethodTable *pMTDecl, MethodTable *pMTImpl);

protected:
    static void CheckInitMethodDataCache();
    static MethodData *FindParentMethodDataHelper(MethodTable *pMT);
    static MethodData *FindMethodDataHelper(MethodTable *pMTDecl, MethodTable *pMTImpl);
    static MethodData *GetMethodDataHelper(MethodTable *pMTDecl, MethodTable *pMTImpl, BOOL fCanCache);
    // NOTE: This method does not cache the resulting MethodData object in the global MethodDataCache.
    static MethodData *GetMethodDataHelper(DispatchMapTypeID declTypeID, MethodTable *pMTDecl, MethodTable *pMTImpl);

public:
    //--------------------------------------------------------------------------------------
    class MethodIterator
    {
    public:
        MethodIterator(MethodTable *pMT);
        MethodIterator(MethodTable *pMTDecl, MethodTable *pMTImpl);
        MethodIterator(MethodData *pMethodData);
        MethodIterator(const MethodIterator &it);
        inline ~MethodIterator() { WRAPPER_CONTRACT; m_pMethodData->Release(); }
        INT32 GetNumMethods() const;
        inline BOOL IsValid() const;
        inline BOOL MoveTo(UINT32 idx);
        inline BOOL Prev();
        inline BOOL Next();
        inline void MoveToBegin();
        inline void MoveToEnd();
        inline UINT32 GetSlotNumber() const;
        inline BOOL IsVirtual() const;
        inline UINT32 GetNumVirtuals() const;
        inline DispatchSlot GetTarget() const;

        // Can be called only if IsValid()=TRUE
        inline MethodDesc *GetMethodDesc() const;
        inline MethodDesc *GetDeclMethodDesc() const;

    protected:
        void Init(MethodTable *pMTDecl, MethodTable *pMTImpl);

        MethodData         *m_pMethodData;
        INT32               m_iCur;           // Current logical slot index
        INT32               m_iMethods;
    };

    //--------------------------------------------------------------------------------------
    // This iterator lets you walk over all the method bodies introduced by this type.
    // This includes new static methods, new non-virtual methods, and any overrides
    // of the parent's virtual methods. It does not include virtual method implementations
    // provided by the parent
    
    class IntroducedMethodIterator
    {
    public:
        IntroducedMethodIterator(MethodTable *pMT, BOOL restrictToCanonicalTypes = TRUE);
        inline BOOL IsValid() const;
        BOOL Next();

        // Can be called only if IsValid()=TRUE
        inline MethodDesc *GetMethodDesc() const;

    protected:
        MethodDescChunk    *m_pMethodDescChunk;     // Current chunk
        INT32               m_iCurMethodInChunk;    // Index within m_pMethodDescChunk
    };

#endif // !DACCESS_COMPILE

    //-------------------------------------------------------------------
    // INSTANCE MEMBER VARIABLES 
    //

public:
    enum
    {
        // AS YOU ADD NEW FLAGS PLEASE CONSIDER WHETHER Generics::NewInstantiation NEEDS
        // TO BE UPDATED IN ORDER TO ENSURE THAT METHODTABLES DUPLICATED FOR GENERIC INSTANTIATIONS
        // CARRY THE CORECT FLAGS.
        //
        // DO NOT use flags that have bits set in the low 2 bytes.
        // These flags are DWORD sized so that our atomic masking
        // operations can operate on the entire 4-byte aligned DWORD
        // instead of the logical non-aligned WORD of flags.  The
        // low WORD of flags is reserved for the component size.
        //
        

        // The following bits store information related to GetInternalCorElementType.
        // Two places in the code need a single bit to indicate if the type
        // is an array type or not. <NICE> Review why </NICE> 
        // We also use two extra bits to encode enough
        // information to compute GetInternalCorElementType in
        // all cases.
        enum_flag_Array                       = 0x00010000,

        enum_flag_InternalCorElementTypeExtraInfoMask                         = 0x00060000,
        enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenTruePrimitive = 0x00020000,
        enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenClass         = 0x00040000,
        enum_flag_InternalCorElementTypeExtraInfo_IfNotArrayThenValueType     = 0x00060000,
        enum_flag_IfArrayThenSzArray                                          = 0x00020000,
        enum_flag_IfArrayThenSharedByReferenceTypes                           = 0x00040000,

        enum_flag_ContainsPointers            = 0x00080000,
        enum_flag_HasFinalizer                = 0x00100000, // instances require finalization

        enum_flag_IsMarshalable               = 0x00200000, // Is this type marshalable by the pinvoke marshalling layer

        enum_flag_HasRemotingVtsInfo          = 0x00400000, // Optional data present indicating VTS methods and optional fields
        enum_flag_IsFreezingRequired          = 0x00800000, // Static data should be frozen after .cctor

        enum_flag_TransparentProxy            = 0x01000000, // tranparent proxy
        enum_flag_CompiledDomainNeutral       = 0x02000000, // Class was compiled in a domain neutral assembly

        // This one indicates that the fields of the valuetype are 
        // not tightly packed and is used to check whether we can
        // do bit-equality on value types to implement ValueType::Equals.
        // It is not valid for classes, and only matters if ContainsPointer
        // is false.
        //
        enum_flag_NotTightlyPacked            = 0x04000000, 

        enum_flag_HasCriticalFinalizer        = 0x08000000, // finalizer must be run on Appdomain Unload
        enum_flag_UNUSED                      = 0x10000000,
        enum_flag_ThreadContextStatic         = 0x20000000, 

        enum_flag_IsFreezingCompleted         = 0x80000000, // Static data has been frozen

        enum_flag_NonTrivialInterfaceCast     = enum_flag_Array |
                                                enum_flag_TransparentProxy,
    };

    enum 
    {


        enum_flag2_MarshaledByRef             = 0x0001, // Class is marshaled by ref (needs a remoting stub)
        enum_flag2_NoSecurityProperties       = 0x0002, // Class does not have security properties (that is,
                                                        // GetClass()->GetSecurityProperties will return 0).
        enum_flag2_HasGenericsStaticsInfo     = 0x0004,  

        enum_flag2_MayNeedRestore             = 0x0008, // Class may need restore
                                                        // This flag is set only for NGENed classes

        enum_flag2_UNUSED                     = 0x0010,
        enum_flag2_IsZapped                   = 0x0020, // This could be fetched from m_pLoaderModule if we run out of flags
                                                               
        enum_flag2_IsDynamicStatics           = 0x0040, // Static data will be stored in the dynamic table of the module
        enum_flag2_FixedAddressVTStatics      = 0x0080, // Value type Statics in this class will be pinned

        // These two flags only use three out of four possible combinations.  We should
        // use the other combination to store some useful information about generics.
        enum_flag2_GenericsMask               = 0x0300,
        enum_flag2_GenericsMask_NonGeneric    = 0x0000, // no instantiation
        enum_flag2_GenericsMask_CanonInst     = 0x0100, // an unshared or canonical-shared instantiation,
                                                        // apart from the typical inst. e.g. List<int> or List<__Canon>
        enum_flag2_GenericsMask_NonCanonInst  = 0x0200, // a non-canonical-shared instantiation, e.g. List<string>
        enum_flag2_GenericsMask_TypicalInst   = 0x0300, // the type instantiated at its formal parameters, e.g. List<T>

        enum_flag2_ClassPreInited             = 0x0400, // If this flag is set, we dont need to do any class initialization logic at all
        enum_flag2_IsAsyncPin                 = 0x0800,   
        enum_flag2_ContainsGenericVariables   = 0x1000, // we cache this flag to help detect these efficiently and
                                                        // to detect this condition when restoring
        enum_flag2_IsInterface                = 0x2000, // This MT is an interface
        enum_flag2_HasDispatchMap             = 0x4000, // TRUE:  m_pDispatchMap is valid
                                                        // FALSE: m_pImplTable is valid
        enum_flag2_HasVariance                = 0x8000, // This is an instantiated type some of whose type parameters are co or contra-variant
    };

private: 
    /*
     * This stuff must be first in the struct and should fit on a cache line - don't move it. Used by the GC.
     */   
    // struct
    // {

    // Low WORD is component size for array and string types, zero otherwise
    DWORD           m_wFlags;

    // Base size of instance of this class when allocated on the heap
    DWORD           m_BaseSize;
    // }

    WORD            m_wFlags2;
        
    // <NICE> In the normal cases we shouldn't need a full word for each of these </NICE>
    WORD            m_wNumMethods; 
    WORD            m_wNumVirtuals;
    WORD            m_wNumInterfaces;

    PTR_MethodTable m_pParentMethodTable;

    // This could be union-ed with m_pPerInstInfo
    // This allows us a super fast lookup of the ZapModule which is now an important perf code path
    // because we cache some EEClass information in the Module's m_ModuleCtorInfo.
    // The cache allows us to avoid touching the EEClass structure in a zapped image (so these stay in cold pages)
    // If we union this with m_pPerInstInfo then for generics we will use the slower MethodTable::FindZapModule()
    // to get the ZapModule when we have a generic methodtable

    PTR_Module      m_pLoaderModule;    // LoaderModule. It is equal to the ZapModule in ngened images

    PTR_MethodTableWriteableData m_pWriteableData;

    PTR_EEClass     m_pEEClass;

    // <NICE> This could one day be an optional member.  Currently JITted code needs to know its offset.  It
    // could, for example, live as the first optional member if the optional members themselves lived at
    // a known offset, which is not currently possible because the vtable is the only variable-sized
    // member to live at a known offset.  However once we use virtual stub dispatch everywhere
    // we will not need to access the vtable at a known offset, and we may be able to reorganise the
    // layout. </NICE>   
      
    DPTR(PTR_Dictionary) m_pPerInstInfo; 

#ifdef _DEBUG
    LPCUTF8  m_szDebugClassName;
    // <NICE> It would be nice to have pointers in debug mode for each of the optional members </NICE>
    InterfaceInfo_t* m_pIMapDEBUG;
    
    // to avoid verify same method table too many times when it's not changing, we cache the GC count
    // on which the method table is verified. When fast GC STRESS is turned on, we only verify the MT if 
    // current GC count is bigger than the number. Note most thing which will invalidate a MT will require a 
    // GC (like AD unload)
    volatile DWORD m_dwLastVerifedGCCnt;

#endif // DEBUG

    //@TODO:NEWVTWORK: This should become an optional member. </STRIP>
    DispatchMap                 *m_pDispatchMap;

    // VTable and Non-Virtual slots go here

    // Optional Members go here
    //    See above for the list of optional members

    // Generic dictionary pointers go here

    // Interface map goes here

    // Generic instantiation+dictionary goes here

    /************************************
     *  STATIC MEMBER VARIABLES
     ************************************/

private:
    static MetaSig      *s_cctorSig;

private:

    // disallow direct creation
    void *operator new(size_t dummy);
    void operator delete(void *pData);
    MethodTable();
    
    // Optional members.  These are used for fields in the data structure where
    // the fields are (a) known when MT is created and (b) there is a default
    // value for the field in the common case.  That is, they are normally used
    // for data that is only relevant to a small number of method tables.

    // The following macro will automatically create GetXXX accessors for the optional members.

#define METHODTABLE_OPTIONAL_MEMBERS() \
    /*                          NAME                    TYPE                            GETTER                     */ \
    /* This must come first - see JIT_IsInstanceOfInterface in JITinterfaceX86.cpp */                                 \
    METHODTABLE_OPTIONAL_MEMBER(InterfaceMap,           PTR_InterfaceInfo,              GetInterfaceMapPtr          ) \
    /* Accessing this member efficiently is still currently performance critical for static field accesses */         \
    /* in generic classes, so place it early in the list. */                                                          \
    METHODTABLE_OPTIONAL_MEMBER(GenericsStaticsInfo,    GenericsStaticsInfo,            GetGenericsStaticsInfo      ) \
    METHODTABLE_OPTIONAL_MEMBER(GenericsDictInfo,       GenericsDictInfo,               GetGenericsDictInfo         ) \
    METHODTABLE_OPTIONAL_MEMBER(ThreadOrContextStatics, PTR_ThreadAndContextStaticsBucket, GetThreadOrContextStaticsBucketPtr) \
    /* Accessed during x-domain transition only, so place it late in the list. */                                     \
    METHODTABLE_OPTIONAL_MEMBER(RemotingVtsInfo,        PTR_RemotingVtsInfo,            GetRemotingVtsInfoPtr       ) \
    METHODTABLE_OPTIONAL_MEMBER(RemotableMethodInfo,    PTR_CrossDomainOptimizationInfo,GetRemotableMethodInfoPtr   ) \

    enum OptionalMemberId
    {
#undef METHODTABLE_OPTIONAL_MEMBER
#define METHODTABLE_OPTIONAL_MEMBER(NAME, TYPE, GETTER) OptionalMember_##NAME,
        METHODTABLE_OPTIONAL_MEMBERS()
        OptionalMember_Count,

        OptionalMember_First = OptionalMember_InterfaceMap,
    };

    FORCEINLINE DWORD GetOffsetOfOptionalMember(OptionalMemberId id);

public:

    //
    // Public accessor helpers for the optional members of MethodTable
    //

#undef METHODTABLE_OPTIONAL_MEMBER
#define METHODTABLE_OPTIONAL_MEMBER(NAME, TYPE, GETTER) \
    inline DPTR(TYPE) GETTER() \
    { \
        WRAPPER_CONTRACT; \
        STATIC_CONTRACT_SO_TOLERANT; \
        _ASSERTE(Has##NAME()); \
        return (DPTR(TYPE))(PTR_HOST_TO_TADDR(this) + GetOffsetOfOptionalMember(OptionalMember_##NAME)); \
    }

    METHODTABLE_OPTIONAL_MEMBERS()
 
private:
    inline DWORD GetStartOffsetOfOptionalMembers()
    {
        WRAPPER_CONTRACT;
        return GetOffsetOfOptionalMember(OptionalMember_First);
    }

    inline DWORD GetEndOffsetOfOptionalMembers()
    {
        WRAPPER_CONTRACT;
        return GetOffsetOfOptionalMember(OptionalMember_Count);
    }

    inline static DWORD GetOptionalMembersAllocationSize(BOOL needsInterfaceMap,
                                                  BOOL needsRemotableMethodInfo,
                                                  BOOL needsGenericsStaticsInfo,
                                                  BOOL needsRemotingVtsInfo,
                                                  BOOL needsGenericDictionaryInfo,
                                                  BOOL needsThreadContextStatic);
    inline DWORD GetOptionalMembersSize();

    // The PerInstInfo is a (possibly empty) array of pointers to 
    // Instantiations/Dictionaries. This array comes after the optional members.
    inline DWORD GetPerInstInfoSize();

    // This is the size of the interface map chunk in the method table.
    // If the MethodTable has a dynamic interface map then the size includes the pointer
    // that stores the extra info for that map.
    // The interface map itself comes after the PerInstInfo (if any)
    inline DWORD GetInterfaceMapSize();

    // The instantiation/dictionary comes at the end of the MethodTable after
    //  the interface map.  
    inline DWORD GetInstAndDictSize();

    static MethodTable          *s_ISerializableMT;
    static MethodTable          *s_IObjectReferenceMT;
    static MethodTable          *s_IDeserializationCBMT;

    /************************************
    //
    // THREAD & CONTEXT STATIC
    //
    ************************************/

public:
    inline BOOL HasThreadOrContextStatics();
    inline void SetHasThreadOrContextStatics();

    inline PTR_ThreadAndContextStaticsBucket GetThreadOrContextStaticsBucket()
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;
        _ASSERTE(HasThreadOrContextStatics());
        PTR_ThreadAndContextStaticsBucket pBucket = *GetThreadOrContextStaticsBucketPtr();
        _ASSERTE(pBucket != NULL);
        return pBucket;
    }

    inline DWORD GetThreadStaticsOffset();
    inline DWORD GetContextStaticsOffset();

    inline WORD GetThreadStaticsSize();
    inline WORD GetContextStaticsSize();

    inline DWORD AllocateThreadStaticsOffset();
    inline DWORD AllocateContextStaticsOffset();

    void SetupThreadOrContextStatics(AllocMemTracker *pamTracker, WORD dwThreadStaticsSize, WORD dwContextStaticsSize);
    DWORD AllocateThreadOrContextStaticsOffset(BOOL fContextStatics);

    BOOL Validate ();
}; // end of class MethodTable

#endif // !_METHODTABLE_H_
