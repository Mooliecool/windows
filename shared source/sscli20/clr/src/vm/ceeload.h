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
// File: CEELOAD.H
//
// CEELOAD.H defines the class use to represent the PE file
// ===========================================================================
#ifndef CEELOAD_H_
#define CEELOAD_H_

#include "common.h"
#include <fusion.h>
#include "vars.hpp" // for LPCUTF8
#include "hash.h"
#include "clsload.hpp"
#include "cgensys.h"
#include "corsym.h"
#include "typehandle.h"
#include "arraylist.h"
#include "pefile.h"
#include "typehash.h"
#include "contractimpl.h"
#include "bitmask.h"
#include "instmethhash.h"
#include "eetwain.h"    // For EnumGCRefs (we should probably move that somewhere else, but can't
                        // find anything better (modulo common or vars.hpp)
#include "classloadlevel.h"
#include "precode.h"


class PELoader;
class Stub;
class MethodDesc;
class FieldDesc;
class Crst;
class AssemblySecurityDescriptor;
class ClassConverter;
class RefClassWriter;
class ReflectionModule;
class EEStringData;
class MethodDescChunk;
class SigTypeContext;
class Assembly;
class BaseDomain;
class AppDomain;
class CompilationDomain;
class DomainModule;
struct DomainLocalModule;
class SystemDomain;
class Module;
class SString;
class Pending;
class MethodTable;
class AppDomain;
class DynamicMethodTable;
struct CerPrepInfo;
class ModuleSecurityDescriptor;

// Used to help clean up interfaces
struct HelpForInterfaceCleanup
{
    void *pData;
    void (*pFunction)(void*);
};

//
// LookupMaps are used to implement RID maps
// It is a linked list of nodes, each handling a successive (and consecutive)
// range of RIDs.
//

typedef DPTR(struct LookupMap) PTR_LookupMap;

struct LookupMap
{
    // This is not actually a pointer to the beginning of the
    // allocated memory, but instead a pointer to &pTable[-MinIndex].
    // Thus, if we know that this LookupMap is the correct one, simply
    // index into it.
    PTR_TADDR           pTable;

    PTR_LookupMap       pNext;

    // Only RIDs less than this value can be present in this node (if they
    // are not handled by an earlier node).
    DWORD               dwMaxIndex;

    // Size of table to allocate for the next node that will be allocated at
    // the end of the list. This number is increased so that successive
    // nodes in the list are of increasingly bigger size
    // These all point to the same block size. Logically this belongs only to
    // the "head" of the list.
    DWORD              *pdwBlockSize;

    DWORD Find(TADDR pointer);

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                           DWORD minIndex,
                           bool enumThis);
    static void ListEnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                      LookupMap* headMap,
                                      bool enumHead);
#endif // DACCESS_COMPILE

    class Iterator
    {
    public:
        Iterator(LookupMap* map);

        BOOL Next();

        TADDR GetElement();
        DWORD GetIndex();
        LookupMap* GetMap();

    private:
        LookupMap* m_map;
        DWORD m_index;
    };

    enum {
        // flag used to mark back-pointers to the module (see below)
        // MUST NOT CLASH WITH IS_FIELD_MEMBER_REF = 0x00000001
        IS_MODULE_BACKPOINTER         = 0x00000002
    };

};

// TypeDefHandles and MethodDefHandles uniquely identify a TypeDef or
// MethodDef, essentially encoding a Module/Def pair in a
// pointer-sized value, *without* requiring the type or method to be
// loaded.
//
// They are represented by a pointer to the actual entry in the TypeDefToMethodTableMap or
// MethodDefToDescMap for a particular module.
//
// Functions are provided to convert back and forth between Module/Def pairs and DefHandles
//
// For types in ngen'ed modules, the Module is recovered using the
// range-tree For types in non-ngen'ed modules, the Module is
// recovered from the actual entry in the map, if loaded, or from a
// bit-twiddled back-pointer that is stored in the map when the handle
// is first requested but the type or method hasn't yet been loaded.
//
// (Dummy structs enforce strong typing here).
typedef struct TypeDefHandle_STRUCT_* TypeDefHandle;
typedef struct MethodDefHandle_STRUCT_* MethodDefHandle;

//
// VASigCookies are allocated to encapsulate a varargs call signature.
// A reference to the cookie is embedded in the code stream.  Cookies
// are shared amongst call sites with identical signatures in the same
// module
//

typedef DPTR(struct VASigCookie) PTR_VASigCookie;
struct VASigCookie
{
    // The JIT wants knows that the size of the arguments comes first
    // so please keep this field first
    unsigned        sizeOfArgs;             // size of argument list
    PTR_Stub        pNDirectMLStub;         // will be use if target is NDirect (tag == 0)
    PCCOR_SIGNATURE mdVASig;                // The debugger depends on this being here,
                                            // so please don't move it without changing
                                            // the GetVAInfo debugger routine.
    PTR_Module      pModule;
    VOID Destruct();
};

//
// VASigCookies are allocated in VASigCookieBlocks to amortize
// allocation cost and allow proper bookkeeping.
//

struct VASigCookieBlock
{
    enum {
#ifdef _DEBUG
        kVASigCookieBlockSize = 2
#else // !_DEBUG
        kVASigCookieBlockSize = 20
#endif // !_DEBUG
    };

    VASigCookieBlock    *m_Next;
    UINT                 m_numcookies;
    VASigCookie          m_cookies[kVASigCookieBlockSize];
};

// This lookup table persists the cctor specific information into the ngen'ed image
// which allows one to run the cctor without touching expensive EEClasses. Note
// that since the persisted info is stored at ngen time as opposed to class layout time,
// in jitted scenarios we would still touch EEClasses. This imples that the variables which store
// this info in the EEClasses are still present even when the prototype is ON.

typedef DPTR(struct ClassCtorInfoEntry) PTR_ClassCtorInfoEntry;
struct ClassCtorInfoEntry
{
    mdToken clTok;
    DWORD firstBoxedStaticOffset;
    DWORD firstBoxedStaticMTIndex;
    WORD numBoxedStatics;
    WORD CCtorSlot;
#ifdef _DEBUG
    PTR_MethodTable mt;
#endif
};



#define MODULE_CTOR_ELEMENTS 256
struct ModuleCtorInfo
{
    DWORD                   numElements;
    DWORD                   numLastAllocated;
    DWORD                   numElementsHot;
    DPTR(PTR_MethodTable)   ppMT;           // size is numElements
    PTR_ClassCtorInfoEntry  cctorInfoHot;   // size is numElementsHot
    PTR_ClassCtorInfoEntry  cctorInfoCold;  // size is numElements-numElementsHot

    PTR_DWORD               hotHashOffsets;  // Indices to the start of each "hash region" in the hot part of the ppMT array. 
    PTR_DWORD               coldHashOffsets; // Indices to the start of each "hash region" in the cold part of the ppMT array. 
    DWORD                   numHotHashes;
    DWORD                   numColdHashes;

    MethodTable **          ppHotGCStaticsMTs;            // hot table
    MethodTable **          ppColdGCStaticsMTs;           // cold table

    DWORD                   numHotGCStaticsMTs;
    DWORD                   numColdGCStaticsMTs;

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    typedef enum {HOT, COLD} REGION;
    FORCEINLINE DWORD ModuleCtorInfo::GenerateHash(PTR_MethodTable pMT, REGION region)
    {
        DWORD tmp1  = pMT->GetNumMethods();
        DWORD tmp2  = pMT->GetNumVirtuals();
        DWORD tmp3  = pMT->GetNumInterfaces();

        tmp1        = (tmp1 << 7) + (tmp1 << 0); // 10000001
        tmp2        = (tmp2 << 6) + (tmp2 << 1); // 01000010
        tmp3        = (tmp3 << 4) + (tmp3 << 3); // 00011000

        tmp1       ^= (tmp1 >> 4);               // 10001001 0001
        tmp2       ^= (tmp2 >> 4);               // 01000110 0010   
        tmp3       ^= (tmp3 >> 4);               // 00011001 1000

        DWORD hashVal = tmp1 + tmp2 + tmp3;

        if (region == HOT)
            hashVal     &= (numHotHashes - 1);   // numHotHashes is required to be a power of two
        else
            hashVal     &= (numColdHashes - 1);  // numColdHashes is required to be a power of two

        return hashVal;
    };

    MethodTable **      GetGCStaticMTs(DWORD index);

};





#ifdef _MSC_VER
// Disable warning: C4324: 'XXX' : structure was padded due to __declspec(align())
// This will let the compiler automatically pad for us, rather than us maintaining
// it manually.
#pragma warning (push)
#pragma warning (disable:4324)
#endif  // _MSC_VER

class ManagedToUnmanagedVTableThunk;
class ManagedToUnmanagedVTableThunkArray
{
public:
    ManagedToUnmanagedVTableThunkArray() : 
        m_rgbArrayBytes(NULL)
    {
    }
    
    void Create(size_t nElements);
    void Destroy();
    ManagedToUnmanagedVTableThunk* GetElement(size_t index);
    void Fixup(DataImage* image);

private:
    BYTE* m_rgbArrayBytes;
};

//
// A Module is the primary unit of code packaging in the runtime.  It
// corresponds mostly to an OS executable image, although other kinds
// of modules exist.
//
class UMEntryThunk;
 
struct ModuleEx
{
friend class Module;

private:
    // We disable warning C4324 which will allow the compiler to pad
    // this for us (rather than requiring the padding to be updated
    // manually).

    // This is a workaround to a problem with REL_32 fixups
    DECLSPEC_ALIGN(CODE_SIZE_ALIGN)
    BYTE _pad[CODE_SIZE_ALIGN];

    // This buffer is used to jump to the prestub in preloaded modules
    DECLSPEC_ALIGN(CODE_SIZE_ALIGN)
    BYTE                    m_PrestubJumpStub[BACK_TO_BACK_JUMP_ALLOCATE_SIZE];

#ifdef HAS_FIXUP_PRECODE
    // This buffer is used to jump to the precode fixup thunk in preloaded modules
    DECLSPEC_ALIGN(CODE_SIZE_ALIGN)
    BYTE                    m_PrecodeFixupJumpStub[BACK_TO_BACK_JUMP_ALLOCATE_SIZE];
#endif


    // This buffer is used to jump to vtable fixup stub
    DECLSPEC_ALIGN(CODE_SIZE_ALIGN)
    BYTE                    m_RestoreVTableJumpStub[BACK_TO_BACK_JUMP_ALLOCATE_SIZE];

    // This buffer is used to jump to the ndirect import stub in preloaded modules
    DECLSPEC_ALIGN(CODE_SIZE_ALIGN)
    BYTE                    m_NDirectImportJumpStub[BACK_TO_BACK_JUMP_ALLOCATE_SIZE];

};


class Module
{
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif

    friend class ZapMonitor;
    friend struct LookupMap;

    VPTR_BASE_CONCRETE_VTABLE_CLASS(Module)

 public:

#ifdef _DEBUG
    // Force verification even if it's turned off
    BOOL                    m_fForceVerify;

#endif // _DEBUG

private:

#ifdef _DEBUG
    DWORD               m_dwDebugArrayClassSize;
#endif

    PTR_PEFile               m_file;

    MethodDesc              *m_pDllMain;

    enum {
        // These are the values set in m_dwTransientFlags.
        // Note that none of these flags survive a prejit save/restore.

        MODULE_IS_TENURED           = 0x00000004,   // Set once we know for sure the Module will not be freed until the appdomain itself exits
        M_CER_ROOT_TABLE_ON_HEAP    = 0x00000010,   // Set when m_pCerNgenRootTable is allocated from heap (at ngen time)
        // UNUSED                   = 0x00000020,
        CLASSES_FREED               = 0x00000040,
        HAS_PHONY_IL_RVAS           = 0x00000080,
        IS_EDIT_AND_CONTINUE        = 0x00000200,   // is EnC Enabled for this module


        //
        // Note: the order of these must match the order defined in
        // cordbpriv.h for DebuggerAssemblyControlFlags. The three
        // values below should match the values defined in
        // DebuggerAssemblyControlFlags when shifted right
        // DEBUGGER_INFO_SHIFT bits.
        //
        DEBUGGER_USER_OVERRIDE_PRIV = 0x00000400,
        DEBUGGER_ALLOW_JIT_OPTS_PRIV= 0x00000800,
        DEBUGGER_TRACK_JIT_INFO_PRIV= 0x00001000,
        DEBUGGER_ENC_ENABLED_PRIV   = 0x00002000,   // this is what was attempted to be set.  IS_EDIT_AND_CONTINUE is actual result.
        DEBUGGER_PDBS_COPIED        = 0x00004000,
        DEBUGGER_IGNORE_PDBS        = 0x00008000,
        DEBUGGER_INFO_MASK_PRIV     = 0x0000Fc00,
        DEBUGGER_INFO_SHIFT_PRIV    = 10,

        // Used to indicate that this module has had it's IJW fixups properly installed.
        IS_IJW_FIXED_UP             = 0x000080000,
     IS_BEING_UNLOADED        = 0x000100000
    };

    enum {
        // These are the values set in m_dwPersistedFlags.  These will survive
        // a prejit save/restore
        COMPUTED_GLOBAL_CLASS       = 0x00000001,
        COMPILED_DOMAIN_NEUTRAL     = 0x00000002
    };

    enum {
        // flag used to mark member ref pointers to field descriptors in the member ref cache
        IS_FIELD_MEMBER_REF         = 0x00000001,
    };

    DWORD                   m_dwTransientFlags;
    DWORD                   m_dwPersistedFlags;

    // Linked list of VASig cookie blocks: protected by m_pStubListCrst
    VASigCookieBlock        *m_pVASigCookieBlock;

    PTR_Assembly            m_pAssembly;
    mdFile                  m_moduleRef;

    CrstExplicitInit        m_Crst;
    CrstExplicitInit        m_FixupCrst;

    // Debugging symbols reader interface. This will only be
    // initialized if needed, either by the debugging subsystem or for
    // an exception.
    ISymUnmanagedReader     *m_pISymUnmanagedReader;
    Crst                    *m_pISymUnmanagedReaderLock;

    // Active dependencies
    ArrayList               m_activeDependencies;
    BitMask                 m_unconditionalDependencies;
    ULONG                  m_dwNumberOfActivations;
    // Class dependencies
    ArrayList               m_classDependencies;

    // Table of thunks for unmanaged vtables
    ManagedToUnmanagedVTableThunkArray  m_ThunkTable;

    // For protecting additions to the heap
    CrstExplicitInit        m_LookupTableCrst;

    // Linear mapping from TypeDef token to MethodTable *
    // For generic types, IsGenericTypeDefinition() is true i.e. instantiation at formals
    LookupMap               m_TypeDefToMethodTableMap;
    DWORD                   m_dwTypeDefMapBlockSize;

    // Linear mapping from TypeRef token to TypeHandle *
    LookupMap               m_TypeRefToMethodTableMap;
    DWORD                   m_dwTypeRefMapBlockSize;

    // Linear mapping from MethodDef token to MethodDesc *
    // For generic methods, IsGenericTypeDefinition() is true i.e. instantiation at formals
    LookupMap               m_MethodDefToDescMap;
    DWORD                   m_dwMethodDefMapBlockSize;

    // Linear mapping from FieldDef token to FieldDesc*
    LookupMap               m_FieldDefToDescMap;
    DWORD                   m_dwFieldDefMapBlockSize;

    // Linear mapping from MemberRef token to MethodDesc*, FieldDesc*
    LookupMap               m_MemberRefToDescMap;
    DWORD                   m_dwMemberRefMapBlockSize;

    // Mapping from File token to Module *
    LookupMap               m_FileReferencesMap;
    DWORD                   m_dwFileReferencesMapBlockSize;

    // Mapping of AssemblyRef token to Module *
    LookupMap               m_ManifestModuleReferencesMap;
    DWORD                   m_dwManifestModuleReferencesMapBlockSize;

public:
    // Hash of available types by name
    PTR_EEClassHashTable    m_pAvailableClasses;

    // Hashtable of generic type instances
    PTR_EETypeHashTable     m_pAvailableParamTypes;

    // For protecting additions to m_pInstMethodHashTable
    CrstExplicitInit        m_InstMethodHashTableCrst;

    PTR_InstMethodHashTable m_pInstMethodHashTable;

    // This is used by the Debugger. We need to store a dword
    // for a count of JMC functions. This is a count, not a pointer.
    // We'll pass the address of this field
    // off to the jit, which will include it in probes injected for
    // debuggable code.
    // This means we need the dword at the time a function is jitted.
    // The Debugger has its own module structure, but those aren't created
    // if a debugger isn't attached.
    // We put it here instead of in the debugger's module because:
    // 1) we need a module structure that's around even when the debugger
    // isn't attached... so we use the EE's module.
    // 2) Needs to be here for ngen
    DWORD                   m_dwDebuggerJMCProbeCount;

private:
    // Cannoically-cased hashtable of the available class names for
    // case insensitive lookup.  Contains pointers into
    // m_pAvailableClasses.
    PTR_EEClassHashTable     m_pAvailableClassesCaseIns;

    // Pointer to binder, if we have one
    friend class Binder;
    PTR_Binder               m_pBinder;





    // Module wide static fields information
    ModuleCtorInfo          m_ModuleCtorInfo;

#ifdef PROFILING_SUPPORTED_DATA
    LONG                    m_profilerNotified;
#endif



protected:

    void CreateDomainThunks();

protected:
    void DoInit(AllocMemTracker *pamTracker);

protected:
#ifndef DACCESS_COMPILE
    virtual void Initialize(AllocMemTracker *pamTracker);
#endif

    void AllocateMaps();

    // RID maps
    LookupMap *IncMapSize(LookupMap *pMap, DWORD rid);
    void AddToRidMap(LookupMap *pMap, DWORD rid, TADDR pDatum);
#ifndef DACCESS_COMPILE
    BOOL TryAddToRidMap(LookupMap *pMap, DWORD rid, TADDR pDatum);
    void SetInRidMap(LookupMap *pMap, DWORD rid, TADDR pDatum);
#endif // !DACCESS_COMPILE
    TADDR GetFromRidMap(LookupMap *pMap, DWORD rid);
    DWORD GetRidFromMapAddress(LookupMap *pMap, TADDR* addr);
    TADDR* RidToDefHandle(LookupMap *pMap, DWORD rid);

#ifdef _DEBUG
    void DebugGetRidMapOccupancy(LookupMap *pMap,
                                 DWORD *pdwOccupied, DWORD *pdwSize);
    void DebugLogRidMapOccupancy();
#endif // _DEBUG

    static HRESULT VerifyFile(PEFile *file, BOOL fZap);

 public:
    static Module *Create(Assembly *pAssembly, mdFile kFile, PEFile *pFile, AllocMemTracker *pamTracker);

 protected:
    Module(Assembly *pAssembly, mdFile moduleRef, PEFile *file);


 public:
#ifndef DACCESS_COMPILE
    virtual void Destruct();
#endif

    PTR_PEFile GetFile() { LEAF_CONTRACT; return m_file; }
    static size_t GetFileOffset() { LEAF_CONTRACT; return offsetof(Module, m_file); }

    BOOL IsManifest();


    void FreeClassTables();

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                                   bool enumThis);
#endif // DACCESS_COMPILE

    ReflectionModule *GetReflectionModule()
    {
        LEAF_CONTRACT;

        _ASSERTE(IsReflection());
        return PTR_ReflectionModule(PTR_HOST_TO_TADDR(this));
    }

    Assembly* GetAssembly() const;

    int GetClassLoaderIndex()
    {
        LEAF_CONTRACT;

        return RidFromToken(m_moduleRef);
    }

    MethodTable *GetGlobalMethodTable();
    bool         NeedsGlobalMethodTable();

    // Only for non-manifest modules
    DomainModule *GetDomainModule(AppDomain *pDomain = NULL);
    DomainModule *FindDomainModule(AppDomain *pDomain);

    // This works for manifest modules too
    DomainFile *GetDomainFile(AppDomain *pDomain = NULL);
    DomainFile *FindDomainFile(AppDomain *pDomain);

    // Operates on assembly of module
    DomainAssembly *GetDomainAssembly(AppDomain *pDomain = NULL);
    DomainAssembly *FindDomainAssembly(AppDomain *pDomain);

    void SetDomainFile(DomainFile *pDomainFile);

    BOOL IsCompiledDomainNeutral() 
    { 
        LEAF_CONTRACT; 
        return (m_dwPersistedFlags & COMPILED_DOMAIN_NEUTRAL) != 0;
    }

    void SetCompiledDomainNeutral() 
    { 
        LEAF_CONTRACT; 
        m_dwPersistedFlags |= COMPILED_DOMAIN_NEUTRAL;
    }

    OBJECTREF GetExposedObject();

    ClassLoader *GetClassLoader();
    BaseDomain* GetDomain();
    AssemblySecurityDescriptor* GetSecurityDescriptor();

    mdFile GetModuleRef()
    {
        LEAF_CONTRACT;

        return m_moduleRef;
    }


    BOOL IsResource() { WRAPPER_CONTRACT; return GetFile()->IsResource(); }
    BOOL IsPEFile() { WRAPPER_CONTRACT; return !GetFile()->IsDynamic(); }
    BOOL IsReflection() { WRAPPER_CONTRACT; return GetFile()->IsDynamic(); }

    BOOL IsEditAndContinueEnabled() 
    { 
        LEAF_CONTRACT; 
        return (IsEditAndContinueCapable()) && ((m_dwTransientFlags & IS_EDIT_AND_CONTINUE) != 0); 
    }

    BOOL IsEditAndContinueCapable(); 
    
    BOOL IsIStream() { LEAF_CONTRACT; return GetFile()->IsIStream(); }

    BOOL IsSystem() { WRAPPER_CONTRACT; return m_file->IsSystem(); }

    static BOOL IsEditAndContinueCapable(PEFile *file) 
    { 
        WRAPPER_CONTRACT; 
        // Some modules are never EnC-capable
        return ! (file->IsSystem() || file->IsResource() || file->HasNativeImage() || file->IsDynamic());
    }

    void EnableEditAndContinue()
    {
        LEAF_CONTRACT;
        LOG((LF_ENC, LL_INFO100, "EnableEditAndContinue: this:0x%x, %s\n", this, GetDebugName()));
        m_dwTransientFlags |= IS_EDIT_AND_CONTINUE;
    }

    void DisableEditAndContinue()
    {
        LEAF_CONTRACT;
        LOG((LF_ENC, LL_INFO100, "DisableEditAndContinue: this:0x%x, %s\n", this, GetDebugName()));
        m_dwTransientFlags &= ~IS_EDIT_AND_CONTINUE;
    }

    // Does pDependentModule depend on "this"?
    BOOL IsDependencyOf(Module * pDependentModule)
    {
        WRAPPER_CONTRACT;
        // This is a watered-down implementation
        // Everything depends on mscorlib.dll and on self
        return (IsSystem() || this == pDependentModule);
    }


    BOOL IsTenured()
    {
        LEAF_CONTRACT;
        return m_dwTransientFlags & MODULE_IS_TENURED;
    }

#ifndef DACCESS_COMPILE
    VOID SetIsTenured()
    {
        LEAF_CONTRACT;
        FastInterlockOr(&m_dwTransientFlags, MODULE_IS_TENURED);
    }

    // CAUTION: This should only be used as backout code if an assembly is unsuccessfully
    //          added to the shared domain assembly map.
    VOID UnsetIsTenured()
    {
        LEAF_CONTRACT;
        FastInterlockAnd(&m_dwTransientFlags, ~MODULE_IS_TENURED);
    }
#endif // !DACCESS_COMPILE

    BOOL IsIntrospectionOnly();
    VOID EnsureActive();
    VOID EnsureAllocated();    
    VOID EnsureLibraryLoaded();
    CHECK CheckActivated();
    ULONG GetNumberOfActivations();
    ULONG IncrementNumberOfActivations();

    IMDInternalImport *GetMDImport() const
    {
        WRAPPER_CONTRACT;

        return m_file->GetPersistentMDImport();
    }

#ifndef DACCESS_COMPILE
    IMetaDataEmit *GetEmitter()
    {
        WRAPPER_CONTRACT;

        return m_file->GetEmitter();
    }

    IMetaDataImport *GetRWImporter()
    {
        WRAPPER_CONTRACT;

        return m_file->GetRWImporter();
    }

    IMetaDataAssemblyImport *GetAssemblyImporter()
    {
        WRAPPER_CONTRACT;

        return m_file->GetAssemblyImporter();
    }

    IMetaDataImport2 *GetRWImporter2()
    {
        WRAPPER_CONTRACT;

        return m_file->GetRWImporter2();
    }

#endif // !DACCESS_COMPILE


    void GetPathForErrorMessages(SString & result);


    ISymUnmanagedReader *GetISymUnmanagedReader(void);
    ISymUnmanagedReader *GetISymUnmanagedReaderNoThrow(void);
    HRESULT SetSymbolBytes(BYTE *pSyms, DWORD cbSyms);

    // Does the current configuration permit reading of symbols for this module?
    // Note that this may require calling into managed code (to resolve security policy).
    BOOL IsSymbolReadingEnabled(void);

    BOOL IsPersistedObject(void *address);


    // This is used by the debugger, in case the symbols aren't
    // available in an on-disk .pdb file (reflection emit,
    // Assembly.Load(byte[],byte[]), etc.
    CGrowableStream *m_pIStreamSym;
    CGrowableStream *GetInMemorySymbolStream()
    {
        LEAF_CONTRACT;

        return m_pIStreamSym;
    }

    void SetInMemorySymbolStream(CGrowableStream *pStream)
    {
        LEAF_CONTRACT;

        m_pIStreamSym = pStream;
    }

#ifndef DACCESS_COMPILE
    static HRESULT TrackIUnknownForDelete(IUnknown *pUnk,
                                          IUnknown ***pppUnk,
                                          HelpForInterfaceCleanup *pCleanHelp=NULL);
#endif // !DACCESS_COMPILE
    static void ReleaseAllIUnknowns(void);
    static void ReleaseIUnknown(IUnknown *pUnk);
    static void ReleaseIUnknown(IUnknown **pUnk);
    void ReleaseISymUnmanagedReader(void);
#ifndef DACCESS_COMPILE
    virtual void ReleaseILData();
#endif

    static void ReleaseMemoryForTracking();

    void FusionCopyPDBs(LPCWSTR moduleName);

    // This function will return PDB stream if exist.
    HRESULT GetHostPdbStream(IStream **ppStream);
    HRESULT ClearHostPdbStream(void);

    // Classes
    void AddClass(mdTypeDef classdef);
    void BuildClassForModule();
    EEClassHashTable *GetAvailableClassHash()
    {
        LEAF_CONTRACT;
        _ASSERTE(!IsResource());
        return m_pAvailableClasses;
    }
#ifndef DACCESS_COMPILE
    void SetAvailableClassHash(EEClassHashTable *pAvailableClasses)
    {
        LEAF_CONTRACT;
        _ASSERTE(!IsResource());
        m_pAvailableClasses = pAvailableClasses;
    }
#endif // !DACCESS_COMPILE
    EEClassHashTable *GetAvailableClassCaseInsHash()
    {
        LEAF_CONTRACT;
        _ASSERTE(!IsResource());
        return m_pAvailableClassesCaseIns;
    }
#ifndef DACCESS_COMPILE
    void SetAvailableClassCaseInsHash(EEClassHashTable *pAvailableClassesCaseIns)
    {
        LEAF_CONTRACT;
        _ASSERTE(!IsResource());
        m_pAvailableClassesCaseIns = pAvailableClassesCaseIns;
    }
#endif // !DACCESS_COMPILE

    // Constructed types tables
    EETypeHashTable *GetAvailableParamTypes()
    {
        LEAF_CONTRACT;
        _ASSERTE(!IsResource());
        return m_pAvailableParamTypes;
    }

    InstMethodHashTable *GetInstMethodHashTable()
    {
        LEAF_CONTRACT;
        _ASSERTE(!IsResource());
        return m_pInstMethodHashTable;
    }

    // Creates a new Method table for an array.  Used to make type handles
    // Note that if kind == SZARRAY or ARRAY, we get passed the GENERIC_ARRAY
    // needed to create the array.  That way we dont need to load classes during
    // the class load, which avoids the need for a 'being loaded' list
    MethodTable* CreateArrayMethodTable(TypeHandle elemType, CorElementType kind, unsigned rank, class AllocMemTracker *pamTracker);

    // This is called from CreateArrayMethodTable
    MethodTable* CreateGenericArrayMethodTable(TypeHandle elemType);

    // Generate a short sig for an array accessor
    VOID GenerateArrayAccessorCallSig(DWORD   dwRank,
                                      DWORD   dwFuncType, // Load, store, or <init>
                                      PCCOR_SIGNATURE *ppSig, // Generated signature
                                      DWORD * pcSig,      // Generated signature size
                                      class AllocMemTracker *pamTracker
    );

    // string helper
    void InitializeStringData(DWORD token, EEStringData *pstrData, CQuickBytes *pqb);

    // Resolving
    OBJECTHANDLE ResolveStringRef(DWORD Token, BaseDomain *pDomain, bool bNeedToSyncWithFixups);
    
    CHECK CheckStringRef(RVA rva);

    // Module/Assembly traversal
    Assembly *GetAssemblyIfLoaded(mdAssemblyRef kAssemblyRef);
    DomainAssembly *LoadAssembly(AppDomain *pDomain, mdAssemblyRef kAssemblyRef);
    Module *GetModuleIfLoaded(mdFile kFile, BOOL loadResources = TRUE);
    DomainFile *LoadModule(AppDomain *pDomain, mdFile kFile, BOOL loadResources = TRUE, BOOL bindOnly = FALSE);
#ifndef DACCESS_COMPILE
    Module *LookupModule(mdToken kFile, BOOL loadResources = TRUE); //wrapper over GetModuleIfLoaded, takes modulerefs as well
#endif

    // RID maps
    TypeHandle LookupTypeDef(mdTypeDef token, ClassLoadLevel level = CLASS_LOAD_UNRESTOREDTYPEKEY)
    {
        WRAPPER_CONTRACT;

        BAD_FORMAT_NOTHROW_ASSERT(TypeFromToken(token) == mdtTypeDef);

        g_IBCLogger.LogRidMapAccess( MakePair( this, token ) );
        PTR_MethodTable pMT = PTR_MethodTable(GetFromRidMap(&m_TypeDefToMethodTableMap, RidFromToken(token)));
        if (pMT == NULL || pMT->GetLoadLevel() < level)
            return TypeHandle();
        else
            return (TypeHandle)pMT;
    }


#ifndef DACCESS_COMPILE
    VOID EnsureTypeDefCanBeStored(mdTypeDef token)
    {
        WRAPPER_CONTRACT; // THROWS/GC_NOTRIGGER/INJECT_FAULT()/MODE_ANY
        if (!IncMapSize(&m_TypeDefToMethodTableMap, RidFromToken(token)))
        {
            COMPlusThrowOM();
        }
    }

    void EnsuredStoreTypeDef(mdTypeDef token, TypeHandle value)
    {
        WRAPPER_CONTRACT; // NOTHROW/GC_NOTRIGGER/FORBID_FAULT/MODE_ANY

        _ASSERTE(TypeFromToken(token) == mdtTypeDef);
        SetInRidMap(&m_TypeDefToMethodTableMap,
                    RidFromToken(token),
                    value.AsTAddr());
    }

#endif // !DACCESS_COMPILE

    DWORD GetTypeDefMax() { LEAF_CONTRACT; return m_TypeDefToMethodTableMap.dwMaxIndex; }

    TypeHandle LookupTypeRef(mdTypeRef token, ClassLoadLevel level = CLASS_LOAD_UNRESTOREDTYPEKEY);

#ifndef DACCESS_COMPILE
    void StoreTypeRef(mdTypeRef token, TypeHandle value)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(TypeFromToken(token) == mdtTypeRef);

        g_IBCLogger.LogRidMapAccess( MakePair( this, token ) );

        // The TypeRef cache is strictly a lookaside cache. If we get an OOM trying to grow the table,
        // we cannot abort the load. (This will cause fatal errors during gc promotion.)
        TryAddToRidMap(&m_TypeRefToMethodTableMap,
                    RidFromToken(token),
                    value.AsTAddr());
    }
#endif // !DACCESS_COMPILE

    // Turn a type or method def into a handle
    TypeDefHandle TypeDefToTypeDefHandle(mdTypeDef typeDef);
    MethodDefHandle MethodDefToMethodDefHandle(mdMethodDef methodDef);

    // Determine the module that this type/method def handle belongs to
    static Module* GetModuleForTypeDefHandle(TypeDefHandle handle);
    static Module* GetModuleForMethodDefHandle(MethodDefHandle handle);

    // Convert type or method def handles to defs
    mdTypeDef GetTypeDefForTypeDefHandle(TypeDefHandle handle);
    mdMethodDef GetMethodDefForMethodDefHandle(MethodDefHandle handle);

    DWORD GetTypeRefMax() { return m_TypeRefToMethodTableMap.dwMaxIndex; }

    MethodDesc *LookupMethodDef(mdMethodDef token);

#ifndef DACCESS_COMPILE
    void EnsureMethodDefCanBeStored(mdMethodDef token)
    {
        WRAPPER_CONTRACT; // THROWS/GC_NOTRIGGER/INJECT_FAULT()/MODE_ANY
        if (!IncMapSize(&m_MethodDefToDescMap, RidFromToken(token)))
        {
            COMPlusThrowOM();
        }
    }

    void EnsuredStoreMethodDef(mdMethodDef token, MethodDesc *value)
    {
        WRAPPER_CONTRACT; // NOTHROW/GC_NOTRIGGER/FORBID_FAULT/MODE_ANY

        _ASSERTE(TypeFromToken(token) == mdtMethodDef);
        SetInRidMap(&m_MethodDefToDescMap,
                    RidFromToken(token),
                    PTR_HOST_TO_TADDR(value));
    }

    mdMethodDef FindMethodDef(MethodDesc *value)
    {
        WRAPPER_CONTRACT;

        return m_MethodDefToDescMap.Find(PTR_HOST_TO_TADDR(value)) |
            mdtMethodDef;
    }
#endif // !DACCESS_COMPILE

    DWORD GetMethodDefMax() { return m_MethodDefToDescMap.dwMaxIndex; }

#ifndef DACCESS_COMPILE
    FieldDesc *LookupFieldDef(mdFieldDef token)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(TypeFromToken(token) == mdtFieldDef);
        return PTR_FieldDesc(GetFromRidMap(&m_FieldDefToDescMap,
                                           RidFromToken(token)));
    }
#else
    FieldDesc *LookupFieldDef(mdFieldDef token);
#endif // DACCESS_COMPILE

#ifndef DACCESS_COMPILE
    void EnsureFieldDefCanBeStored(mdFieldDef token)
    {
        WRAPPER_CONTRACT; // THROWS/GC_NOTRIGGER/INJECT_FAULT()/MODE_ANY
        if (!IncMapSize(&m_FieldDefToDescMap, RidFromToken(token)))
        {
            COMPlusThrowOM();
        }
    }

    void EnsuredStoreFieldDef(mdFieldDef token, FieldDesc *value)
    {
        WRAPPER_CONTRACT; // NOTHROW/GC_NOTRIGGER/FORBID_FAULT/MODE_ANY

        _ASSERTE(TypeFromToken(token) == mdtFieldDef);
        SetInRidMap(&m_FieldDefToDescMap,
                    RidFromToken(token),
                    PTR_HOST_TO_TADDR(value));
    }

    mdFieldDef FindFieldDef(FieldDesc *value)
    {
        WRAPPER_CONTRACT;

        return m_FieldDefToDescMap.Find(PTR_HOST_TO_TADDR(value)) |
            mdtFieldDef;
    }
#endif // !DACCESS_COMPILE

    DWORD GetFieldDefMax() { LEAF_CONTRACT; return m_FieldDefToDescMap.dwMaxIndex; }

    void *LookupMemberRef(mdMemberRef token, BOOL *pfIsMethod)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(TypeFromToken(token) == mdtMemberRef);
        TADDR pResult = GetFromRidMap(&m_MemberRefToDescMap,
                                      RidFromToken(token));
        g_IBCLogger.LogRidMapAccess( MakePair( this, token ) );
        *pfIsMethod = (pResult & IS_FIELD_MEMBER_REF) == 0;
        return (void*)(pResult & ~(TADDR)IS_FIELD_MEMBER_REF);
    }
    MethodDesc *LookupMemberRefAsMethod(mdMemberRef token);
#ifndef DACCESS_COMPILE
    void StoreMemberRef(mdMemberRef token, FieldDesc *value)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(TypeFromToken(token) == mdtMemberRef);
        TryAddToRidMap(&m_MemberRefToDescMap,
                           RidFromToken(token),
                           (PTR_HOST_TO_TADDR(value) | IS_FIELD_MEMBER_REF));
    }
    void StoreMemberRef(mdMemberRef token, MethodDesc *value)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(TypeFromToken(token) == mdtMemberRef);
        TryAddToRidMap(&m_MemberRefToDescMap,
                           RidFromToken(token),
                           PTR_HOST_TO_TADDR(value));
    }
    mdMemberRef FindMemberRef(MethodDesc *value)
    {
        WRAPPER_CONTRACT;

        return m_MemberRefToDescMap.Find(PTR_HOST_TO_TADDR(value)) | mdtMemberRef;
    }
    mdMemberRef FindMemberRef(FieldDesc *value)
    {
        WRAPPER_CONTRACT;

        return m_MemberRefToDescMap.Find(PTR_HOST_TO_TADDR(value)) | mdtMemberRef;
    }
#endif // !DACCESS_COMPILE
    DWORD GetMemberRefMax() { LEAF_CONTRACT; return m_MemberRefToDescMap.dwMaxIndex; }

    Module *LookupFile(mdFile token)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(TypeFromToken(token) == mdtFile);
        return PTR_Module(GetFromRidMap(&m_FileReferencesMap,
                                        RidFromToken(token)));
    }


#ifndef DACCESS_COMPILE
    void EnsureFileCanBeStored(mdFile token)
    {
        WRAPPER_CONTRACT; // THROWS/GC_NOTRIGGER/INJECT_FAULT()/MODE_ANY

        _ASSERTE(TypeFromToken(token) == mdtFile);
        if (!IncMapSize(&m_FileReferencesMap, RidFromToken(token)))
        {
            COMPlusThrowOM();
        }
    }

    void EnsuredStoreFile(mdFile token, Module *value)
    {
        WRAPPER_CONTRACT; // NOTHROW/GC_NOTRIGGER/FORBID_FAULT


        _ASSERTE(TypeFromToken(token) == mdtFile);
        SetInRidMap(&m_FileReferencesMap,
                    RidFromToken(token),
                    PTR_HOST_TO_TADDR(value));
    }


    void StoreFileThrowing(mdFile token, Module *value)
    {
        WRAPPER_CONTRACT;


        _ASSERTE(TypeFromToken(token) == mdtFile);
        AddToRidMap(&m_FileReferencesMap,
                    RidFromToken(token),
                    PTR_HOST_TO_TADDR(value));
    }

    BOOL StoreFileNoThrow(mdFile token, Module *value)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(TypeFromToken(token) == mdtFile);
        return TryAddToRidMap(&m_FileReferencesMap,
                              RidFromToken(token),
                              PTR_HOST_TO_TADDR(value));
    }

    mdFile FindFile(Module *value)
    {
        WRAPPER_CONTRACT;

        return m_FileReferencesMap.Find(PTR_HOST_TO_TADDR(value)) | mdtFile;
    }
#endif // !DACCESS_COMPILE

    DWORD GetFileMax() {LEAF_CONTRACT;  return m_FileReferencesMap.dwMaxIndex; }

    Assembly *LookupAssemblyRef(mdAssemblyRef token);

#ifndef DACCESS_COMPILE
    void ForceStoreAssemblyRef(mdAssemblyRef token, Assembly *value);
    void StoreAssemblyRef(mdAssemblyRef token, Assembly *value);
    mdAssemblyRef FindAssemblyRef(Assembly *value);
#endif // !DACCESS_COMPILE

    DWORD GetAssemblyRefMax() {LEAF_CONTRACT;  return m_ManifestModuleReferencesMap.dwMaxIndex; }

    MethodDesc *FindMethodThrowing(mdToken pMethod);
    MethodDesc *FindMethod(mdToken pMethod);

#ifdef DEBUGGING_SUPPORTED
    // Debugger stuff
    BOOL NotifyDebuggerLoad(AppDomain *pDomain, int level, BOOL attaching);
    void NotifyDebuggerUnload(AppDomain *pDomain);

    DebuggerAssemblyControlFlags GetDebuggerInfoBits(void)
    {
        LEAF_CONTRACT;

        return (DebuggerAssemblyControlFlags)((m_dwTransientFlags &
                                               DEBUGGER_INFO_MASK_PRIV) >>
                                              DEBUGGER_INFO_SHIFT_PRIV);
    }

    void SetDebuggerInfoBits(DebuggerAssemblyControlFlags newBits);
#endif // DEBUGGING_SUPPORTED

#ifdef PROFILING_SUPPORTED
    BOOL IsProfilerNotified() {LEAF_CONTRACT;  return m_profilerNotified; }
    void NotifyProfilerLoadFinished(HRESULT hr);
#endif // PROFILING_SUPPORTED

    // Get any cached ITypeLib* for the module.
    ITypeLib *GetTypeLib();
    // Cache the ITypeLib*, if one is not already cached.
    void SetTypeLib(ITypeLib *pITLB);
    ITypeLib *GetTypeLibTCE();
    void SetTypeLibTCE(ITypeLib *pITLB);

    // Enregisters a VASig. Returns NULL for failure (out of memory.)
    VASigCookie *GetVASigCookie(PCCOR_SIGNATURE pVASig);

    // DLL entry point
    MethodDesc *GetDllEntryPoint()
    {
        LEAF_CONTRACT;
        return m_pDllMain;
    }
    void SetDllEntryPoint(MethodDesc *pMD)
    {
        LEAF_CONTRACT;
        m_pDllMain = pMD;
    }

    BOOL CanExecuteCode();


    // This data is only valid for NGEN'd modules, and for modules we're creating at NGEN time.
    ModuleCtorInfo* GetZapModuleCtorInfo()
    {
        LEAF_CONTRACT;
        
        return &m_ModuleCtorInfo;
    }

 private:


 public:
    ULONG HashIdentity() { WRAPPER_CONTRACT; return m_file->HashIdentity(); }
#ifndef DACCESS_COMPILE
    BOOL Equals(Module *pModule) { WRAPPER_CONTRACT; return m_file->Equals(pModule->m_file); }
    BOOL Equals(PEFile *pFile) { WRAPPER_CONTRACT; return m_file->Equals(pFile); }
#endif // !DACCESS_COMPILE

    LPCUTF8 GetSimpleName()  { WRAPPER_CONTRACT; return m_file->GetSimpleName(); }
    const SString &GetPath() { WRAPPER_CONTRACT; return m_file->GetPath(); }

#ifdef LOGGING
    LPCWSTR GetDebugName() { WRAPPER_CONTRACT; return m_file->GetDebugName(); }
#endif

    BOOL IsILOnly() { WRAPPER_CONTRACT; return m_file->IsILOnly(); }
    BOOL HasNativeImage() { WRAPPER_CONTRACT; return m_file->HasNativeImage(); }
    
    PEImageLayout *GetNativeImage()
    {
        CONTRACT(PEImageLayout *)
        {
            PRECONDITION(m_file->HasNativeImage());
            POSTCONDITION(CheckPointer(RETVAL));
            NOTHROW;
            GC_NOTRIGGER;
        }
        CONTRACT_END;

        RETURN m_file->GetLoadedNative();
    }

    // These are overridden by reflection modules
    virtual const void *GetIL(RVA il);
    virtual void *GetRvaField(RVA field);

    CHECK CheckIL(RVA il, COUNT_T size);
    CHECK CheckIL(RVA il);
    CHECK CheckRvaField(RVA field);
    CHECK CheckRvaField(RVA field, COUNT_T size);

    BYTE *GetPhonyILBase();
    RVA GetPhonyILRva(BYTE *il);

    const void *GetInternalPInvokeTarget(RVA target)
    { WRAPPER_CONTRACT; return m_file->GetInternalPInvokeTarget(target); }

    BOOL HasTls();
    BOOL IsRvaFieldTls(DWORD field);
    UINT32 GetFieldTlsOffset(DWORD field);
    UINT32 GetTlsIndex();

    PCCOR_SIGNATURE GetSignature(RVA signature);
    RVA GetSignatureRva(PCCOR_SIGNATURE signature);
    CHECK CheckSignatureRva(RVA signature);
    CHECK CheckSignature(PCCOR_SIGNATURE signature);

    mdToken GetEntryPointToken();

    BYTE *GetProfilerBase();


    // Active transition path management
    // 
    // This list keeps track of module which we have active transition
    // paths to.  An active transition path is where we move from
    // active execution in one module to another module without
    // involving triggering the file loader to ensure that the
    // destination module is active.  We must explicitly list these
    // relationships so the the loader can ensure that the activation
    // constraints are a priori satisfied.
    //
    // Conditional vs. Unconditional describes how we deal with
    // activation failure of a dependency.  In the unconditional case,
    // we propagate the activation failure to the depending module.
    // In the conditional case, we activate a "trigger" in the active
    // transition path which will cause the path to fail in particular
    // app domains where the destination module failed to activate.
    // (This trigger in the path typically has a perf cost even in the
    // nonfailing case.)
    //
    // In either case we must try to perform the activation eagerly -
    // even in the conditional case we have to know whether to turn on
    // the trigger or not before we let the active transition path
    // execute.

    BOOL AddActiveDependency(Module *pModule, BOOL unconditional);

    // Active dependency iterator
    class DependencyIterator
    {
      protected:
        ArrayList::Iterator m_i;
        COUNT_T             m_index;
        BitMask             *m_unconditionalFlags;

        friend class Module;

        DependencyIterator(ArrayList *list, BitMask *unconditionalFlags)
          : m_index((COUNT_T)-1),
            m_unconditionalFlags(unconditionalFlags)
        {
            m_i = list->Iterate();
        }

      public:
        Module *GetDependency()
        {
            return (Module *) m_i.GetElement();
        }

        BOOL Next()
        {
            while (m_i.Next())
            {
                ++m_index;

                // When iterating all dependencies, we do not restore any tokens
                // as we want to be lazy.
                if (!CORCOMPILE_IS_TOKEN_TAGGED(m_i.GetElement()))
                    return TRUE;
            }
            return FALSE;
        }
        BOOL IsUnconditional()
        {
            if (m_unconditionalFlags == NULL)
                return TRUE;
            else
                return m_unconditionalFlags->TestBit(m_index);
        }
    };

    DependencyIterator IterateActiveDependencies()
    {
        return DependencyIterator(&m_activeDependencies, &m_unconditionalDependencies);
    }

    BOOL HasActiveDependency(Module *pModule);
    BOOL HasUnconditionalActiveDependency(Module *pModule);

    // Class dependencies are rolled up inheritence dependencies of individual
    // classes in the module.  The list is shared across all classes to save space.
    // Each class has a BitMask indicating which entries in the shared dependencies list 
    // apply to it.

    BOOL AddClassDependency(Module *pModule, BitMask *classMask);

    class DependencySetIterator : public DependencyIterator
    {
      protected:
        BitMask           *m_mask;

        friend class Module;
        DependencySetIterator(ArrayList *list, BitMask *mask)
          : DependencyIterator(list, NULL),
            m_mask(mask)
        {
        }

      public:
        BOOL Next()
        {
            while (m_i.Next())
            {
                COUNT_T index = ++m_index;

                if (m_mask->TestBit(index))
                {
                    // When asking for specific dependencies, we will go ahead
                    // and restore them.

                    
                    return TRUE;
                }
            }
            return FALSE;
        }
    };

    DependencySetIterator IterateClassDependencies(BitMask *classMask)
    {
        return DependencySetIterator(&m_classDependencies, classMask);
    }

    // Turn triggers from this module into runtime checks
    void EnableModuleFailureTriggers(Module *pModule, AppDomain *pDomain);


    BOOL IsBeingUnloaded() { return m_dwTransientFlags & IS_BEING_UNLOADED; }
    void   SetBeingUnloaded();
    void   StartUnload();
    

public:
    void LogInstantiatedType(TypeHandle typeHnd, ULONG flagNum);
    void LogInstantiatedMethod( MethodDesc * md );

private:
    // First entry contains the size of the table.
    PTR_SIZE_T   m_rgDispatchTypeTable;

    SIZE_T GetDispatchTypeTableEntryCount()
    {
        if (m_rgDispatchTypeTable != NULL)
            return m_rgDispatchTypeTable[0];
        else
            return 0;
    }

public:
    UINT32 MapZapTypeID(UINT32 typeID);

public:
    // This helper returns to offsets for the slots/bytes/handles. They return the offset in bytes from the beggining
    // of the 1st GC pointer in the statics block for the module.
    void        GetOffsetsForStaticData(
                    mdTypeDef cl,
                    BOOL bDynamic,
                    DWORD dwGCStaticHandles,
                    DWORD dwNonGCStaticBytes,
                    DWORD * pOutStaticHandleOffset,
                    DWORD * pOutNonGCStaticOffset);
public:

    CrstBase*           GetFixupCrst()
    {
        return &m_FixupCrst;
    }

    void                AllocateStaticHandles(AppDomain* pDomainMT);

    DWORD               GetStaticsAndClassInitBlockSize()
    {
        return m_dwStaticsBlockSize;
    }

    DWORD               AllocateDynamicEntry(MethodTable *pMT);

    // We need this for the jitted shared case,
    inline MethodTable* GetDynamicClassMT(DWORD dynamicClassID)
    {
        LEAF_CONTRACT;
        _ASSERTE(m_cDynamicEntries > dynamicClassID);
        return m_pDynamicStaticsInfo[dynamicClassID].pEnclosingMT;
    }

    SIZE_T               GetModuleID()
    {
        LEAF_CONTRACT;
        return PTR_TO_TADDR(m_pDomainLocalModule);
    }

    SIZE_T *             GetAddrModuleID()
    {
        LEAF_CONTRACT;
        return (SIZE_T*) &m_pDomainLocalModule;
    }

    PTR_DomainLocalModule   GetDomainLocalModule(AppDomain *pDomain = NULL);


    BOOL IsFCallMapActive()
    {
        return FALSE;
    }

protected:
    void            BuildStaticsOffsets     ();
    void            AllocateStatics         (AllocMemTracker *pamTracker);

public:
    void            EnumStaticGCRefs        (AppDomain* pAppDomain, GCEnumCallback  pCallback, LPVOID hCallBack);

protected:    

    // Will return underlying type if it's an enum
    //             ELEMENT_TYPE_VALUETYPE if it is a non enum
    //             ELEMENT_TYPE_END if it doesn't know (we may not want to load other assemblies)
    CorElementType  ParseMetadataForStaticsIsTypeDefEnum(mdToken tk, const SigTypeContext* pContext);
    CorElementType  ParseMetadataForStaticsIsValueTypeEnum(mdToken tk, const SigTypeContext* pContext);
    void            ParseMetadataForStatics(DWORD* pdwNumTypes, DWORD* pdwNonGCStaticBytes, DWORD* pdwGCStaticHandles);

    PTR_DomainLocalModule   m_pDomainLocalModule;       // MultiDomain case: tagged (low bit 1) DLS index
                                                        // SingleDomain case: domain local module

    // reusing the statics area of a method table to store
    // these for the non domain neutral case, but they're now unified
    // it so that we don't have different code paths for this.
    PTR_DWORD               m_pStaticOffsets;               // Offset of statics in each class

    // @NICE: see if we can remove these fields
    DWORD                   m_dwMaxGCStaticHandles;         // Max number of handles we can have.

    // Size of the precomputed statics block. This includes class init bytes, gc handles and non gc statics
    DWORD                   m_dwStaticsBlockSize;


    // For 'dynamic' statics (Reflection and generics)
protected:
    SIZE_T                  m_cDynamicEntries;              // Number of used entries in DynamicStaticsInfo table
    SIZE_T                  m_maxDynamicEntries;            // Size of table itself, including unused entries

    // Info we need for dynamic statics that we can store per-module (ie, no need for it to be duplicated
    // per appdomain)
    struct DynamicStaticsInfo
    {
        MethodTable*        pEnclosingMT;                   // Enclosing type; necessarily in this loader module
    };
    DynamicStaticsInfo*     m_pDynamicStaticsInfo;          // Table with entry for each dynamic ID

public:
    BOOL                    IsNoStringInterning();



protected:
    void                    GenerateAllCompilationRelaxationFlags();
    DWORD                   m_dwCompilationRelaxationFlags; // We cache these so that we don't have
                                                            // to pull in a custom attribute;



#ifndef DACCESS_COMPILE
public:

    // Support for getting and creating information about Constrained Execution Regions rooted in this module.

    // Access to CerPrepInfo, the structure used to track CERs prepared at runtime (as opposed to ngen time). GetCerPrepInfo will
    // return the structure associated with the given method desc if it exists or NULL otherwise. CreateCerPrepInfo will get the
    // structure if it exists or allocate and return a new struct otherwise. Creation of CerPrepInfo structures is automatically
    // synchronized by the CerCrst (lazily allocated as needed).
    CerPrepInfo *GetCerPrepInfo(MethodDesc *pMD);
    CerPrepInfo *CreateCerPrepInfo(MethodDesc *pMD);


    Crst *GetCerCrst()
    {
        LEAF_CONTRACT;
        return m_pCerCrst;
    }

    BOOL GetRVAOverrideForMethod(MethodDesc* pMD, DWORD* pdwOverride);
    void SetRVAOverrideForMethod(MethodDesc* pMD, DWORD dwOverride);
#endif // !DACCESS_COMPILE

private:
    EEPtrHashTable       *m_pCerPrepInfo;       // Root methods prepared for Constrained Execution Regions
    Crst                 *m_pCerCrst;           // Mutex protecting update access to both of the above hashes

    EEPtrHashTable       *m_pRVAOverrides;      // Overriden 
    Crst                 *m_pRVAOverridesCrst;  // Mutex protecting update access to both of the above hashes

public:
    // Support for per-module remoting thunks used to dispatch interface calls on transparent proxies in some edge cases.

    EEPtrHashTable       *m_pRemotingInterfaceThunks;       // Hash map of method descs to stub entry points
    Crst                 *m_pRemotingInterfaceThunksCrst;   // Mutex protecting access to the above         

    ModuleSecurityDescriptor* m_pModuleSecurityDescriptor;
    
};



#ifdef _MSC_VER
#pragma warning (pop)
#endif


//
// A ReflectionModule is a module created by reflection
//

// {F5398690-98FE-11d2-9C56-00A0C9B7CC45}
extern "C" const GUID IID_ICorReflectionModule;

class ReflectionModule : public Module
{
    VPTR_VTABLE_CLASS(ReflectionModule, Module)

 public:
    HCEESECTION m_sdataSection;
 protected:
    ICeeGen *m_pCeeFileGen;
private:
    Assembly             *m_pCreatingAssembly;
    ISymUnmanagedWriter **m_ppISymUnmanagedWriter;
    RefClassWriter       *m_pInMemoryWriter;

    ReflectionModule(Assembly *pAssembly, mdFile token, PEFile *pFile);

public:

    static ReflectionModule *Create(Assembly *pAssembly, PEFile *pFile, AllocMemTracker *pamTracker);

    void Initialize(AllocMemTracker *pamTracker);

    void Destruct();
#ifndef DACCESS_COMPILE    
    void ReleaseILData();
#endif

    // Overides functions to access sections
    virtual const void *GetIL(RVA target);
    virtual void *GetRvaField(RVA rva);

    Assembly* GetCreatingAssembly( void )
    {
        LEAF_CONTRACT;

        return m_pCreatingAssembly;
    }

    void SetCreatingAssembly( Assembly* assembly )
    {
        LEAF_CONTRACT;

        m_pCreatingAssembly = assembly;
    }

    ICeeGen *GetCeeGen() {LEAF_CONTRACT;  return m_pCeeFileGen; }

    RefClassWriter *GetClassWriter()
    {
        LEAF_CONTRACT;

        return m_pInMemoryWriter;
    }

    ISymUnmanagedWriter *GetISymUnmanagedWriter()
    {
        LEAF_CONTRACT;

        // If we haven't set up room for a writer, then we certinally
        // haven't set one, so just return NULL.
        if (m_ppISymUnmanagedWriter == NULL)
            return NULL;
        else
            return *m_ppISymUnmanagedWriter;
    }

    ISymUnmanagedWriter **GetISymUnmanagedWriterAddr()
    {
        LEAF_CONTRACT;

        // We must have setup room for the writer before trying to get
        // the address for it. Any calls to this before a
        // SetISymUnmanagedWriter are very incorrect.
        _ASSERTE(m_ppISymUnmanagedWriter != NULL);

        return m_ppISymUnmanagedWriter;
    }

#ifndef DACCESS_COMPILE
    HRESULT SetISymUnmanagedWriter(ISymUnmanagedWriter *pWriter, HelpForInterfaceCleanup* hlp=NULL)
    {
        CONTRACTL
        {
            NOTHROW;
            GC_NOTRIGGER;
            INJECT_FAULT(return E_OUTOFMEMORY;);
        }
        CONTRACTL_END


        // Setting to NULL when we've never set a writer before should
        // do nothing.
        if ((pWriter == NULL) && (m_ppISymUnmanagedWriter == NULL))
            return S_OK;

        // Make room for the writer if necessary.
        if (m_ppISymUnmanagedWriter == NULL)
        {

            return Module::TrackIUnknownForDelete(
                                   (IUnknown*)pWriter,
                                   (IUnknown***)&m_ppISymUnmanagedWriter,
                                   hlp);
        }
        else
        {
            if (*m_ppISymUnmanagedWriter)
                ((IUnknown*)(*m_ppISymUnmanagedWriter))->Release();
            *m_ppISymUnmanagedWriter = pWriter;
            return S_OK;
        }
    }
#endif // !DACCESS_COMPILE
};


// Module holders
FORCEINLINE void VoidModuleDestruct(Module *pModule)
{
#ifndef DACCESS_COMPILE
    if (g_fEEStarted)
        pModule->Destruct();
#endif
}

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(Module *);
template BOOL CompareDefault(Module*,Module*);
#endif // defined (_MSC_VER) && _MSC_VER <= 1300
typedef Wrapper<Module*, DoNothing, VoidModuleDestruct, 0> ModuleHolder;



FORCEINLINE void VoidReflectionModuleDestruct(ReflectionModule *pModule)
{
#ifndef DACCESS_COMPILE
    pModule->Destruct();
#endif
}

#if defined (_MSC_VER) && _MSC_VER <= 1300
template void DoNothing(ReflectionModule *);
template BOOL CompareDefault(ReflectionModule*,ReflectionModule*);
#endif // defined (_MSC_VER) && _MSC_VER <= 1300
typedef Wrapper<ReflectionModule*, DoNothing, VoidReflectionModuleDestruct, 0> ReflectionModuleHolder;



//----------------------------------------------------------------------
// VASigCookieEx (used to create a fake VASigCookie for unmanaged->managed
// calls to vararg functions. These fakes are distinguished from the
// real thing by having a null mdVASig.
//----------------------------------------------------------------------
struct VASigCookieEx : public VASigCookie
{
    const BYTE *m_pArgs;        // pointer to first unfixed unmanaged arg
};

#endif // !CEELOAD_H_



