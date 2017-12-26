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
//*****************************************************************************
// File: dacimpl.h
//
// Central header file for external data access implementation.
//
//*****************************************************************************


#ifndef __DACIMPL_H__
#define __DACIMPL_H__

extern CRITICAL_SECTION g_dacCritSec;

// Convert between CLRDATA_ADDRESS and TADDR.
#define TO_TADDR(cdaddr) ((TADDR)(cdaddr))
#define TO_CDADDR(taddr) ((CLRDATA_ADDRESS)(LONG_PTR)(taddr))
// XXX drewb - Should SLOT get defined to be TADDR when
// building for daccess?
#define TO_SLOT(taddr) ((SLOT)(taddr))

#define TO_CDENUM(ptr) ((CLRDATA_ENUM)(ULONG_PTR)(ptr))
#define FROM_CDENUM(type, cdenum) ((type*)(ULONG_PTR)(cdenum))

#ifdef _X86_
// Older platforms didn't have extended registers in
// the context definition so only enforce that size
// if the extended register flag is set.
#define ContextSizeForFlags(_flags) \
    (((_flags) & CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS ? \
     sizeof(CONTEXT) : offsetof(CONTEXT, ExtendedRegisters))
#else
#define ContextSizeForFlags(_flags) sizeof(CONTEXT)
#endif
#define CheckContextSizeForFlags(_size, _flags) \
    ((_size) >= ContextSizeForFlags(_flags))
#define CheckContextSizeForInBuffer(_size, _buffer) \
    ((_size) >= offsetof(CONTEXT, ContextFlags) + sizeof(ULONG) && \
     (_size) >= ContextSizeForFlags(((PCONTEXT)(_buffer))->ContextFlags))

#define SIMPFRAME_ALL \
    (CLRDATA_SIMPFRAME_UNRECOGNIZED | \
     CLRDATA_SIMPFRAME_MANAGED_METHOD | \
     CLRDATA_SIMPFRAME_RUNTIME_MANAGED_CODE | \
     CLRDATA_SIMPFRAME_RUNTIME_UNMANAGED_CODE)

enum DAC_USAGE_TYPE
{
    DAC_DPTR,
    DAC_VPTR,
    DAC_STRA,
    DAC_STRW,
};

// mscordacwks's module handle 
extern HINSTANCE g_thisModule;

struct DAC_MD_IMPORT
{
    DAC_MD_IMPORT* next;
    TADDR peFile;
    bool isAlternate;
    IMDInternalImport* impl;
};

struct METH_EXTENTS
{
    ULONG32 numExtents;
    ULONG32 curExtent;
    // Currently only one is needed.
    CLRDATA_ADDRESS_RANGE extents[1];
};

HRESULT ConvertUtf8(__in LPCUTF8 utf8,
                    ULONG32 bufLen,
                    ULONG32* nameLen,
                    __inout_ecount_part_opt(bufLen, nameLen) PWSTR buffer);
HRESULT AllocUtf8(__in_opt LPCWSTR wstr,
                  ULONG32 srcChars,
                  __deref_out LPUTF8* utf8);

HRESULT GetFullClassNameFromMetadata(IMDInternalImport* mdImport,
                                     mdTypeDef classToken,
                                     ULONG32 bufferChars,
                                     __inout_ecount(bufferChars) LPUTF8 buffer);
HRESULT GetFullMethodNameFromMetadata(IMDInternalImport* mdImport,
                                      mdMethodDef methodToken,
                                      ULONG32 bufferChars,
                                      __inout_ecount(bufferChars) LPUTF8 buffer);

enum SplitSyntax
{
    SPLIT_METHOD,
    SPLIT_TYPE,
    SPLIT_FIELD,
    SPLIT_NO_NAME,
};

HRESULT SplitFullName(__in_z __in PCWSTR fullName,
                      SplitSyntax syntax,
                      ULONG32 memberDots,
                      __deref_out_opt LPUTF8* namespaceName,
                      __deref_out_opt LPUTF8* typeName,
                      __deref_out_opt LPUTF8* memberName,
                      __deref_out_opt LPUTF8* params);

int CompareUtf8(__in LPCUTF8 str1, __in LPCUTF8 str2, __in ULONG32 nameFlags);

#define INH_STATIC \
    (CLRDATA_VALUE_ALL_KINDS | \
     CLRDATA_VALUE_IS_INHERITED | CLRDATA_VALUE_FROM_STATIC)

HRESULT InitFieldIter(DeepFieldDescIterator* fieldIter,
                      TypeHandle typeHandle,
                      bool canHaveFields,
                      ULONG32 flags,
                      IXCLRDataTypeInstance* fromType);

ULONG32 GetTypeFieldValueFlags(TypeHandle typeHandle,
                               FieldDesc* fieldDesc,
                               ULONG32 otherFlags,
                               bool isDeref);

//----------------------------------------------------------------------------
//
// MetaEnum.
//
//----------------------------------------------------------------------------

class MetaEnum
{
public:
    MetaEnum(void)
        : m_domainIter(FALSE)
    {
        Clear();
        m_appDomain = NULL;
    }
    ~MetaEnum(void)
    {
        End();
    }

    void Clear(void)
    {
        m_mdImport = NULL;
        m_kind = 0;
        m_lastToken = mdTokenNil;
    }

    HRESULT Start(IMDInternalImport* mdImport, ULONG32 kind,
                  mdToken container);
    void End(void);

    HRESULT NextToken(mdToken* token,
                      __deref_opt_out_opt LPCUTF8* namespaceName,
                      __deref_opt_out_opt LPCUTF8* name);
    HRESULT NextDomainToken(AppDomain** appDomain,
                            mdToken* token);
    HRESULT NextTokenByName(__in_opt LPCUTF8 namespaceName,
                            __in_opt LPCUTF8 name,
                            ULONG32 nameFlags,
                            mdToken* token);
    HRESULT NextDomainTokenByName(__in_opt LPCUTF8 namespaceName,
                                  __in_opt LPCUTF8 name,
                                  ULONG32 nameFlags,
                                  AppDomain** appDomain, mdToken* token);

    static HRESULT CdNextToken(CLRDATA_ENUM* handle,
                               mdToken* token)
    {
        MetaEnum* iter = FROM_CDENUM(MetaEnum, *handle);
        if (!iter)
        {
            return S_FALSE;
        }

        return iter->NextToken(token, NULL, NULL);
    }
    static HRESULT CdNextDomainToken(CLRDATA_ENUM* handle,
                                     AppDomain** appDomain,
                                     mdToken* token)
    {
        MetaEnum* iter = FROM_CDENUM(MetaEnum, *handle);
        if (!iter)
        {
            return S_FALSE;
        }

        return iter->NextDomainToken(appDomain, token);
    }
    static HRESULT CdEnd(CLRDATA_ENUM handle)
    {
        MetaEnum* iter = FROM_CDENUM(MetaEnum, handle);
        if (iter)
        {
            delete iter;
            return S_OK;
        }
        else
        {
            return E_INVALIDARG;
        }
    }

    IMDInternalImport* m_mdImport;
    ULONG32 m_kind;
    HENUMInternal m_enum;
    AppDomain* m_appDomain;
    AppDomainIterator m_domainIter;
    mdToken m_lastToken;

    static HRESULT New(Module* mod,
                       ULONG32 kind,
                       mdToken container,
                       IXCLRDataAppDomain* pubAppDomain,
                       MetaEnum** metaEnum,
                       CLRDATA_ENUM* handle);
};

//----------------------------------------------------------------------------
//
// SplitName.
//
//----------------------------------------------------------------------------

class SplitName
{
public:
    // Type of name and splitting being done in this instance.
    SplitSyntax m_syntax;
    ULONG32 m_nameFlags;
    ULONG32 m_memberDots;

    // Split fields.
    LPUTF8 m_namespaceName;
    LPUTF8 m_typeName;
    mdTypeDef m_typeToken;
    LPUTF8 m_memberName;
    mdMethodDef m_memberToken;
    LPUTF8 m_params;
    // XXX drewb - Translated signature.

    // Arbitrary extra data.
    Thread* m_tlsThread;
    Module* m_module;
    MetaEnum m_metaEnum;
    DeepFieldDescIterator m_fieldEnum;
    ULONG64 m_objBase;
    FieldDesc* m_lastField;

    SplitName(SplitSyntax syntax, ULONG32 nameFlags,
              ULONG32 memberDots);
    ~SplitName(void)
    {
        Delete();
    }

    void Delete(void);
    void Clear(void);

    HRESULT SplitString(__in_opt PCWSTR fullName);

    bool FindType(IMDInternalImport* mdInternal);
    bool FindMethod(IMDInternalImport* mdInternal);
    bool FindField(IMDInternalImport* mdInternal);

    int Compare(LPCUTF8 str1, LPCUTF8 str2)
    {
        return CompareUtf8(str1, str2, m_nameFlags);
    }

    static HRESULT AllocAndSplitString(__in_opt PCWSTR fullName,
                                       SplitSyntax syntax,
                                       ULONG32 nameFlags,
                                       ULONG32 memberDots,
                                       SplitName** split);

    static HRESULT CdStartMethod(__in_opt PCWSTR fullName,
                                 ULONG32 nameFlags,
                                 Module* mod,
                                 mdTypeDef typeToken,
                                 AppDomain* appDomain,
                                 IXCLRDataAppDomain* pubAppDomain,
                                 SplitName** split,
                                 CLRDATA_ENUM* handle);
    static HRESULT CdNextMethod(CLRDATA_ENUM* handle,
                                mdMethodDef* token);
    static HRESULT CdNextDomainMethod(CLRDATA_ENUM* handle,
                                      AppDomain** appDomain,
                                      mdMethodDef* token);

    static HRESULT CdStartField(__in_opt PCWSTR fullName,
                                ULONG32 nameFlags,
                                ULONG32 fieldFlags,
                                IXCLRDataTypeInstance* fromTypeInst,
                                TypeHandle typeHandle,
                                Module* mod,
                                mdTypeDef typeToken,
                                ULONG64 objBase,
                                Thread* tlsThread,
                                IXCLRDataTask* pubTlsThread,
                                AppDomain* appDomain,
                                IXCLRDataAppDomain* pubAppDomain,
                                SplitName** split,
                                CLRDATA_ENUM* handle);
    static HRESULT CdNextField(ClrDataAccess* dac,
                               CLRDATA_ENUM* handle,
                               IXCLRDataTypeDefinition** fieldType,
                               ULONG32* fieldFlags,
                               IXCLRDataValue** value,
                               ULONG32 nameBufRetLen,
                               ULONG32* nameLenRet,
                               __inout_ecount_opt(nameBufRetLen) WCHAR nameBufRet[  ],
                               IXCLRDataModule** tokenScopeRet,
                               mdFieldDef* tokenRet);
    static HRESULT CdNextDomainField(ClrDataAccess* dac,
                                     CLRDATA_ENUM* handle,
                                     IXCLRDataValue** value);

    static HRESULT CdStartType(__in_opt PCWSTR fullName,
                               ULONG32 nameFlags,
                               Module* mod,
                               AppDomain* appDomain,
                               IXCLRDataAppDomain* pubAppDomain,
                               SplitName** split,
                               CLRDATA_ENUM* handle);
    static HRESULT CdNextType(CLRDATA_ENUM* handle,
                              mdTypeDef* token);
    static HRESULT CdNextDomainType(CLRDATA_ENUM* handle,
                                    AppDomain** appDomain,
                                    mdTypeDef* token);

    static HRESULT CdEnd(CLRDATA_ENUM handle)
    {
        SplitName* split = FROM_CDENUM(SplitName, handle);
        if (split)
        {
            delete split;
            return S_OK;
        }
        else
        {
            return E_INVALIDARG;
        }
    }
};

//----------------------------------------------------------------------------
//
// ProcessModIter.
//
//----------------------------------------------------------------------------

struct ProcessModIter
{
    AppDomainIterator m_domainIter;
    bool m_nextDomain;
    AppDomain::AssemblyIterator m_assemIter;
    bool m_iterShared;
    SharedDomain::SharedAssemblyIterator m_sharedIter;
    Assembly* m_curAssem;
    Assembly::ModuleIterator m_modIter;

    ProcessModIter(void)
        : m_domainIter(FALSE)
    {
        m_nextDomain = true;
        m_iterShared = false;
        m_curAssem = NULL;
    }

    // The Loader recently changed to exclude assemblies in state FILE_LOADED
    // from the assembly iterator flag kIncludeLoaded. Now, FILE_LOADED is just
    // an internal loader flag, and an assembly needs to reach FILE_ACTIVE to
    // be included in the iterator. For our module/breakpoint purposes, we
    // just need to reach FILE_LOADED state, so include all the assemblies in
    // our iterator, and skip past those that didn't yet reach FILE_LOADED.
    BOOL NextToLoaded(void)
    {
        while(true)
        {
            if(!m_assemIter.Next())
            {
                m_nextDomain = true;
                return FALSE;
            }
            
            DomainAssembly *pDA = m_assemIter.GetDomainAssembly();
            if (pDA->IsLoaded())
            {
                return TRUE;
            }
        }
        return TRUE;
    }
            
        
    Assembly* NextAssem(void)
    {
        while (!m_iterShared)
        {
            if (m_nextDomain)
            {
                if (!m_domainIter.Next())
                {
                    m_iterShared = true;
                    break;
                }

                m_nextDomain = false;
                m_assemIter =
                    m_domainIter.GetDomain()->IterateAssembliesEx((AssemblyIterationFlags)(kIncludeLoading | kIncludeLoaded | kIncludeExecution));
            }

            if (!NextToLoaded())
            {
                continue;
            }

            if (!m_assemIter.GetAssembly()->IsDomainNeutral())
            {
                // We've found a domain-specific assembly, so
                // this is a unique element in the Assembly
                // iteration.
                return m_assemIter.GetAssembly();
            }

            // Found a shared assembly, which may be duplicated
            // across app domains.  Ignore it now and let
            // it get picked up in the shared iteration where
            // it'll only occur once.
        }

        if (!m_sharedIter.Next())
        {
            return NULL;
        }

        return m_sharedIter.GetAssembly();
    }

    Module* NextModule(void)
    {
        for (;;)
        {
            if (!m_curAssem)
            {
                m_curAssem = NextAssem();
                if (!m_curAssem)
                {
                    return NULL;
                }

                m_modIter = m_curAssem->IterateModules();
            }

            if (!m_modIter.Next())
            {
                m_curAssem = NULL;
                continue;
            }

            return m_modIter.GetModule();
        }
    }
};

//----------------------------------------------------------------------------
//
// DacInstanceManager.
//
//----------------------------------------------------------------------------

// The data for an access may have special alignment needs and
// the cache must provide similar semantics.
#define DAC_INSTANCE_ALIGN 16

#define DAC_INSTANCE_SIG 0xdac1

// The instance manager allocates large blocks and then
// suballocates those for particular instances.
struct DAC_INSTANCE_BLOCK
{
    DAC_INSTANCE_BLOCK* next;
    ULONG32 bytesUsed;
    ULONG32 bytesFree;
};

#define DAC_INSTANCE_BLOCK_ALLOCATION 0x40000

// Sufficient memory is allocated to guarantee storage of the
// instance header plus room for alignment padding.
// Once the aligned pointer is found, this structure is prepended to
// the aligned pointer and therefore doesn't affect the alignment
// of the actual instance data.
struct DAC_INSTANCE
{
    DAC_INSTANCE* next;
    TADDR addr;
    ULONG32 size;
    // Identifying marker to give a simple
    // check for host->taddr validity.
    ULONG32 sig:16;
    // DPTR or VPTR.
    ULONG32 usage:2;
	
    // Marker that can be used to prevent reporting this memory to the callback 
    // object (via ICLRDataEnumMemoryRegionsCallback:EnumMemoryRegion)
    // more than once. This bit is checked only by the DacEnumHost?PtrMem
    // macros, so consistent use of those macros ensures that the memory is 
    // reported at most once
    ULONG32 enumMem:1;

    // Marker to prevent metadata gets reported to mini-dump
    ULONG32 noReport:1;
    
    // Marker to determine if EnumMemoryRegions has been called on 
    // a method descriptor
    ULONG32 MDEnumed:1;

};

struct DAC_INSTANCE_PUSH
{
    DAC_INSTANCE_PUSH* next;
    DAC_INSTANCE_BLOCK* blocks;
    ULONG64 blockMemUsage;
    ULONG32 numInst;
    ULONG64 instMemUsage;
};
    
// The runtime will want the best access locality possible,
// so it's likely that many instances will be clustered.
// The hash function needs to spread near addresses across
// hash entries, so hash on the low bits of the target address.
// Not all the way down to the LSB, though, as there generally
// won't be individual accesses at the byte level.  Assume that
// most accesses will be natural-word aligned.
#define DAC_INSTANCE_HASH_BITS 10
#define DAC_INSTANCE_HASH_SHIFT 2
#define DAC_INSTANCE_HASH(addr) \
    (((ULONG32)(ULONG_PTR)(addr) >> DAC_INSTANCE_HASH_SHIFT) & \
     ((1 << DAC_INSTANCE_HASH_BITS) - 1))
#define DAC_INSTANCE_HASH_SIZE (1 << DAC_INSTANCE_HASH_BITS)


struct DumpMemoryReportStatics
{
    UINT    m_cbStack;              // number of bytes that we report directly for stack walk
    UINT    m_cbNgen;               // number of bytes that we report directly for ngen images
    UINT    m_cbModuleList;         // number of bytes that we report for module list directly
    UINT    m_cbClrStatics;         // number of bytes that we report for CLR statics
    UINT    m_cbClrHeapStatics;     // number of bytes that we report for CLR heap statics
    UINT    m_cbImplicity;          // number of bytes that we report implicitly.        
};


class DacInstanceManager
{
public:
    DacInstanceManager(void);
    ~DacInstanceManager(void);

    DAC_INSTANCE* Add(DAC_INSTANCE* inst);

    DAC_INSTANCE* Alloc(TADDR addr, ULONG32 size, DAC_USAGE_TYPE usage);
    void ReturnAlloc(DAC_INSTANCE* inst);
    DAC_INSTANCE* Find(TADDR addr);
    HRESULT Write(DAC_INSTANCE* inst, bool throwEx);
    void Supersede(DAC_INSTANCE* inst);
    void Flush(void);
    void ClearEnumMemMarker(void);
    bool PushState(void);
    void PopState(void);

    void AddSuperseded(DAC_INSTANCE* inst)
    {
        inst->next = m_superseded;
        m_superseded = inst;
    }

    UINT DumpAllInstances(ICLRDataEnumMemoryRegionsCallback *pCallBack);

private:

    DAC_INSTANCE_BLOCK* FindInstanceBlock(DAC_INSTANCE* inst);
    void FreeAllBlocks(void);
    
    void InitEmpty(void)
    {
        m_blocks = NULL;
        m_blockMemUsage = 0;
        m_numInst = 0;
        m_instMemUsage = 0;
        ZeroMemory(m_hash, sizeof(m_hash));
        m_superseded = NULL;
        m_instPushed = NULL;
    }


    typedef struct _HashInstanceKey {
        TADDR addr;
        DAC_INSTANCE* instance;
    } HashInstanceKey;

    typedef struct _HashInstanceKeyBlock {
        // Blocks are chained in reverse order of allocation so that the most recently allocated
        // block is searched first.
        _HashInstanceKeyBlock* next;

        // Entries to a block are added from the max index on down so that recently added
        // entries are at the start of the block.
        DWORD firstElement;
        HashInstanceKey instanceKeys[] ;
    } HashInstanceKeyBlock;

// The hashing function does a good job of distributing the entries across buckets. To handle a
// SO on x86, we have under 250 entries in a bucket. A 4K block size allows 511 entries on x86 and
// about half that on x64. On x64, the number of entries added to the hash table is significantly 
// smaller than on x86 (and the max recursion depth for default stack sizes is also far less), so
// 4K is generally adequate.

#define HASH_INSTANCE_BLOCK_ALLOC_SIZE (4 * 1024)
#define HASH_INSTANCE_BLOCK_NUM_ELEMENTS ((HASH_INSTANCE_BLOCK_ALLOC_SIZE - offsetof(_HashInstanceKeyBlock, instanceKeys))/sizeof(HashInstanceKey))


    DAC_INSTANCE_BLOCK* m_blocks;
    ULONG64 m_blockMemUsage;
    ULONG32 m_numInst;
    ULONG64 m_instMemUsage;

    HashInstanceKeyBlock* m_hash[DAC_INSTANCE_HASH_SIZE];

    DAC_INSTANCE* m_superseded;
    DAC_INSTANCE_PUSH* m_instPushed;
};

//----------------------------------------------------------------------------
//
// ClrDataAccess.
//
//----------------------------------------------------------------------------

class ClrDataAccess
    : public IXCLRDataProcess,
      public ICLRDataEnumMemoryRegions
{
public:
    ClrDataAccess(ICLRDataTarget* target);
    ~ClrDataAccess(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataProcess.
    //

    virtual HRESULT STDMETHODCALLTYPE Flush( void);

    virtual HRESULT STDMETHODCALLTYPE StartEnumTasks(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumTask(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataTask **task);

    virtual HRESULT STDMETHODCALLTYPE EndEnumTasks(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetTaskByOSThreadID(
        /* [in] */ ULONG32 OSThreadID,
        /* [out] */ IXCLRDataTask **task);

    virtual HRESULT STDMETHODCALLTYPE GetTaskByUniqueID(
        /* [in] */ ULONG64 uniqueID,
        /* [out] */ IXCLRDataTask **task);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataProcess *process);

    virtual HRESULT STDMETHODCALLTYPE GetManagedObject(
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE GetDesiredExecutionState(
        /* [out] */ ULONG32 *state);

    virtual HRESULT STDMETHODCALLTYPE SetDesiredExecutionState(
        /* [in] */ ULONG32 state);

    virtual HRESULT STDMETHODCALLTYPE GetAddressType(
        /* [in] */ CLRDATA_ADDRESS address,
        /* [out] */ CLRDataAddressType* type);

    virtual HRESULT STDMETHODCALLTYPE GetRuntimeNameByAddress(
        /* [in] */ CLRDATA_ADDRESS address,
        /* [in] */ ULONG32 flags,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ CLRDATA_ADDRESS* displacement);

    virtual HRESULT STDMETHODCALLTYPE StartEnumAppDomains(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumAppDomain(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataAppDomain **appDomain);

    virtual HRESULT STDMETHODCALLTYPE EndEnumAppDomains(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetAppDomainByUniqueID(
        /* [in] */ ULONG64 uniqueID,
        /* [out] */ IXCLRDataAppDomain **appDomain);

    virtual HRESULT STDMETHODCALLTYPE StartEnumAssemblies(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumAssembly(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataAssembly **assembly);

    virtual HRESULT STDMETHODCALLTYPE EndEnumAssemblies(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumModules(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumModule(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataModule **mod);

    virtual HRESULT STDMETHODCALLTYPE EndEnumModules(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetModuleByAddress(
        /* [in] */ CLRDATA_ADDRESS address,
        /* [out] */ IXCLRDataModule** mod);

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodDefinitionsByAddress(
        /* [in] */ CLRDATA_ADDRESS address,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodDefinitionByAddress(
        /* [in] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodDefinition **method);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodDefinitionsByAddress(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodInstancesByAddress(
        /* [in] */ CLRDATA_ADDRESS address,
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodInstanceByAddress(
        /* [in] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodInstance **method);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodInstancesByAddress(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetDataByAddress(
        /* [in] */ CLRDATA_ADDRESS address,
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [in] */ IXCLRDataTask* tlsTask,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ IXCLRDataValue **value,
        /* [out] */ CLRDATA_ADDRESS *displacement);

    virtual HRESULT STDMETHODCALLTYPE GetExceptionStateByExceptionRecord(
        /* [in] */ EXCEPTION_RECORD64 *record,
        /* [out] */ IXCLRDataExceptionState **exception);

    virtual HRESULT STDMETHODCALLTYPE TranslateExceptionRecordToNotification(
        /* [in] */ EXCEPTION_RECORD64 *record,
        /* [in] */ IXCLRDataExceptionNotification *notify);

    virtual HRESULT STDMETHODCALLTYPE CreateMemoryValue(
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [in] */ IXCLRDataTask* tlsTask,
        /* [in] */ IXCLRDataTypeInstance* type,
        /* [in] */ CLRDATA_ADDRESS addr,
        /* [out] */ IXCLRDataValue** value);

    virtual HRESULT STDMETHODCALLTYPE SetAllTypeNotifications(
        /* [in] */ IXCLRDataModule* mod,
        /* [in] */ ULONG32 flags);

    virtual HRESULT STDMETHODCALLTYPE SetAllCodeNotifications(
        /* [in] */ IXCLRDataModule* mod,
        /* [in] */ ULONG32 flags);

    virtual HRESULT STDMETHODCALLTYPE GetTypeNotifications(
        /* [in] */ ULONG32 numTokens,
        /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
        /* [in] */ IXCLRDataModule* singleMod,
        /* [in, size_is(numTokens)] */ mdTypeDef tokens[],
        /* [out, size_is(numTokens)] */ ULONG32 flags[]);

    virtual HRESULT STDMETHODCALLTYPE SetTypeNotifications(
        /* [in] */ ULONG32 numTokens,
        /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
        /* [in] */ IXCLRDataModule* singleMod,
        /* [in, size_is(numTokens)] */ mdTypeDef tokens[],
        /* [in, size_is(numTokens)] */ ULONG32 flags[],
        /* [in] */ ULONG32 singleFlags);

    virtual HRESULT STDMETHODCALLTYPE GetCodeNotifications(
        /* [in] */ ULONG32 numTokens,
        /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
        /* [in] */ IXCLRDataModule* singleMod,
        /* [in, size_is(numTokens)] */ mdMethodDef tokens[],
        /* [out, size_is(numTokens)] */ ULONG32 flags[]);

    virtual HRESULT STDMETHODCALLTYPE SetCodeNotifications(
        /* [in] */ ULONG32 numTokens,
        /* [in, size_is(numTokens)] */ IXCLRDataModule* mods[],
        /* [in] */ IXCLRDataModule* singleMod,
        /* [in, size_is(numTokens)] */ mdMethodDef tokens[],
        /* [in, size_is(numTokens)] */ ULONG32 flags[],
        /* [in] */ ULONG32 singleFlags);

    virtual HRESULT STDMETHODCALLTYPE GetOtherNotificationFlags(
        /* [out] */ ULONG32* flags);

    virtual HRESULT STDMETHODCALLTYPE SetOtherNotificationFlags(
        /* [in] */ ULONG32 flags);

    virtual HRESULT STDMETHODCALLTYPE FollowStub(
        /* [in] */ ULONG32 inFlags,
        /* [in] */ CLRDATA_ADDRESS inAddr,
        /* [in] */ CLRDATA_FOLLOW_STUB_BUFFER* inBuffer,
        /* [out] */ CLRDATA_ADDRESS* outAddr,
        /* [out] */ CLRDATA_FOLLOW_STUB_BUFFER* outBuffer,
        /* [out] */ ULONG32* outFlags);

    virtual HRESULT STDMETHODCALLTYPE FollowStub2(
        /* [in] */ IXCLRDataTask* task,
        /* [in] */ ULONG32 inFlags,
        /* [in] */ CLRDATA_ADDRESS inAddr,
        /* [in] */ CLRDATA_FOLLOW_STUB_BUFFER* inBuffer,
        /* [out] */ CLRDATA_ADDRESS* outAddr,
        /* [out] */ CLRDATA_FOLLOW_STUB_BUFFER* outBuffer,
        /* [out] */ ULONG32* outFlags);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);


    //
    // ICLRDataEnumMemoryRegions.
    //
    virtual HRESULT STDMETHODCALLTYPE EnumMemoryRegions(
        /* [in] */ ICLRDataEnumMemoryRegionsCallback *callback,
        /* [in] */ ULONG32 miniDumpFlags,
        /* [in] */ CLRDataEnumMemoryFlags clrFlags);


    //
    // ClrDataAccess.
    //

    HRESULT Initialize(void);

    Thread* FindClrThreadByTaskId(ULONG64 taskId);
    HRESULT IsPossibleCodeAddress(IN CLRDATA_ADDRESS address);

    HRESULT GetFullMethodName(IN MethodDesc* methodDesc,
                              IN ULONG32 symbolChars,
                              IN ULONG32* symbolLen,
                              __inout_ecount_part_opt(symbolChars, symbolLen) LPWSTR symbol);
    HRESULT RawGetMethodName(/* [in] */ CLRDATA_ADDRESS address,
                             /* [in] */ ULONG32 flags,
                             /* [in] */ ULONG32 bufLen,
                             /* [out] */ ULONG32 *nameLen,
                             /* [size_is][out] */ WCHAR nameBuf[  ],
                             /* [out] */ CLRDATA_ADDRESS* displacement);

    HRESULT FollowStubStep(
        /* [in] */ Thread* thread,
        /* [in] */ ULONG32 inFlags,
        /* [in] */ TADDR inAddr,
        /* [in] */ union STUB_BUF* inBuffer,
        /* [out] */ TADDR* outAddr,
        /* [out] */ union STUB_BUF* outBuffer,
        /* [out] */ ULONG32* outFlags);

    DebuggerJitInfo* GetDebuggerJitInfo(MethodDesc* methodDesc,
                                        TADDR addr)
    {
        if (g_pDebugger)
        {
            return g_pDebugger->GetJitInfo(methodDesc, (PBYTE)addr, NULL);
        }

        return NULL;
    }

    HRESULT GetMethodExtents(MethodDesc* methodDesc,
                             METH_EXTENTS** extents);
    HRESULT GetMethodVarInfo(MethodDesc* methodDesc,
                             TADDR address,
                             ULONG32* numVarInfo,
                             ICorDebugInfo::NativeVarInfo** varInfo,
                             ULONG32* codeOffset);

    // If the method has multiple copies of code (because of EnC or code-pitching),
    // this returns the info corresponding to address.
    // If 'address' and 'codeOffset' are both non-NULL, *codeOffset gets set to
    // the offset of 'address' from the start of the method.
    HRESULT GetMethodNativeMap(MethodDesc* methodDesc,
                               TADDR address,
                               ULONG32* numMap,
                               DebuggerILToNativeMap** map,
                               bool* mapAllocated,
                               CLRDATA_ADDRESS* codeStart,
                               ULONG32* codeOffset);

#define DECLARE_DAC_REQUEST(name) \
    HRESULT (name)(IN ULONG32 inBufferSize,     \
                   IN BYTE* inBuffer,           \
                   IN ULONG32 outBufferSize,    \
                   OUT BYTE* outBuffer)

    DECLARE_DAC_REQUEST(RequestThreadStoreData);
    DECLARE_DAC_REQUEST(RequestThreadpoolData);
    DECLARE_DAC_REQUEST(RequestMethodTableData);
    DECLARE_DAC_REQUEST(RequestThreadData);
    DECLARE_DAC_REQUEST(RequestThreadFromThinlock);
    DECLARE_DAC_REQUEST(RequestContextData);
    DECLARE_DAC_REQUEST(RequestMethodDescData);
    DECLARE_DAC_REQUEST(RequestMethodDescIPData);
    DECLARE_DAC_REQUEST(RequestMethodDescFrameData);
    DECLARE_DAC_REQUEST(RequestMethodName);
    DECLARE_DAC_REQUEST(RequestCodeHeaderData);
    DECLARE_DAC_REQUEST(RequestWorkRequestData);
    DECLARE_DAC_REQUEST(RequestObjectData);
    DECLARE_DAC_REQUEST(RequestObjectStringData);
    DECLARE_DAC_REQUEST(RequestObjectClassName);
    DECLARE_DAC_REQUEST(RequestMethodTableName);
    DECLARE_DAC_REQUEST(RequestModuleTokenData);
    DECLARE_DAC_REQUEST(RequestModuleData);
    DECLARE_DAC_REQUEST(RequestModuleILData);
    DECLARE_DAC_REQUEST(RequestModuleMapTraverse);
    DECLARE_DAC_REQUEST(RequestEEClassData);
    DECLARE_DAC_REQUEST(RequestFieldDescData);
    DECLARE_DAC_REQUEST(RequestManagedStaticAddr);
    DECLARE_DAC_REQUEST(RequestPEFileName);
    DECLARE_DAC_REQUEST(RequestFrameNameData);
    DECLARE_DAC_REQUEST(RequestPEFileData);
    DECLARE_DAC_REQUEST(RequestAppDomainStoreData);
    DECLARE_DAC_REQUEST(RequestAppDomainList);
    DECLARE_DAC_REQUEST(RequestAppDomainData);
    DECLARE_DAC_REQUEST(RequestAppDomainName);
    DECLARE_DAC_REQUEST(RequestAppDomainAppBase);
    DECLARE_DAC_REQUEST(RequestAppDomainPrivateBinPaths);
    DECLARE_DAC_REQUEST(RequestAppDomainConfigFile);
    DECLARE_DAC_REQUEST(RequestAssemblyList);
    DECLARE_DAC_REQUEST(RequestFailedAssemblyList);
    DECLARE_DAC_REQUEST(RequestAssemblyData);
    DECLARE_DAC_REQUEST(RequestAssemblyName);
    DECLARE_DAC_REQUEST(RequestAssemblyDisplayName);
    DECLARE_DAC_REQUEST(RequestAssemblyLocation);
    DECLARE_DAC_REQUEST(RequestFailedAssemblyData);
    DECLARE_DAC_REQUEST(RequestFailedAssemblyDisplayName);
    DECLARE_DAC_REQUEST(RequestFailedAssemblyLocation);
    DECLARE_DAC_REQUEST(RequestAssemblyModuleList);
    DECLARE_DAC_REQUEST(RequestGCHeapList);
    DECLARE_DAC_REQUEST(RequestGCHeapData);
    DECLARE_DAC_REQUEST(RequestGCHeapDetails);
    DECLARE_DAC_REQUEST(RequestGCHeapDetailsStatic);
    DECLARE_DAC_REQUEST(RequestGCHeapSegment);
    DECLARE_DAC_REQUEST(RequestDACUnitTestData);
    DECLARE_DAC_REQUEST(RequestHandleTableTraverse);
    DECLARE_DAC_REQUEST(RequestLoaderHeapTraverse);
    DECLARE_DAC_REQUEST(RequestVirtCallStubHeapTraverse);
    DECLARE_DAC_REQUEST(RequestIsStub);
    DECLARE_DAC_REQUEST(RequestDomainLocalModuleData);
    DECLARE_DAC_REQUEST(RequestDomainLocalModuleFromAppDomainData);
    DECLARE_DAC_REQUEST(RequestDomainLocalModuleFromModule);
    DECLARE_DAC_REQUEST(RequestRcwCleanupTraverse);
    DECLARE_DAC_REQUEST(RequestEHInfoTraverse);
    DECLARE_DAC_REQUEST(RequestNestedException);
    DECLARE_DAC_REQUEST(RequestSyncBlockData);
    DECLARE_DAC_REQUEST(RequestSyncBlockCleanupData);
    
#ifdef STRESS_LOG
    DECLARE_DAC_REQUEST(RequestStressLogData);
#endif

#ifdef _DEBUG
    DECLARE_DAC_REQUEST(RequestMda);
#endif

    DECLARE_DAC_REQUEST(RequestJitList);
    DECLARE_DAC_REQUEST(RequestJitHelperFunctionName);
    DECLARE_DAC_REQUEST(RequestJumpThunkTarget);
    DECLARE_DAC_REQUEST(RequestJitManagerList);
    DECLARE_DAC_REQUEST(RequestJitHeapList);
    DECLARE_DAC_REQUEST(RequestCodeHeapList);
    DECLARE_DAC_REQUEST(RequestMethodTableSlot);
    DECLARE_DAC_REQUEST(RequestUsefulGlobals);
    DECLARE_DAC_REQUEST(RequestCLRTLSDataIndex);
    

#ifndef GC_SMP
    HRESULT ServerGCHeapDetails(CLRDATA_ADDRESS heapAddr,
                                DacpGcHeapDetails *detailsData);
#endif

    HRESULT VisitOneHandleTable(VISITHANDLE pFunc,LPVOID token,HHANDLETABLE Table);

    //
    // Memory enumeration.
    //

    HRESULT EnumMemoryRegionsWrapper(ICLRDataEnumMemoryRegionsCallback *callback, CLRDataEnumMemoryFlags flags);

    // skinny minidump functions
    HRESULT EnumMemoryRegionsWorkerSkinny (ICLRDataEnumMemoryRegionsCallback *callback, CLRDataEnumMemoryFlags flags);    
    HRESULT EnumMemoryRegionsWorkerHeap(IN ICLRDataEnumMemoryRegionsCallback *callback, IN CLRDataEnumMemoryFlags flags);
    
    HRESULT EnumMemWalkStackHelper(CLRDataEnumMemoryFlags flags, IXCLRDataStackWalk  *pStackWalk);
    HRESULT DumpManagedObject(CLRDataEnumMemoryFlags flags, OBJECTREF objRef);
    HRESULT DumpManagedExcepObject(CLRDataEnumMemoryFlags flags, OBJECTREF objRef);

    HRESULT EnumMemWriteDataSegment();
    
    // Custom Dump    
    HRESULT EnumMemoryRegionsWorkerCustom (ICLRDataEnumMemoryRegionsCallback *callback);    

    // helper function for dump code
    void EnumWksGlobalMemoryRegions(CLRDataEnumMemoryFlags flags);
#ifndef GC_SMP
    void EnumSvrGlobalMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
    HRESULT EnumMemCollectNgenImages();
    HRESULT EnumMemCLRStatic(CLRDataEnumMemoryFlags flags);
    HRESULT EnumMemCLRHeapCrticalStatic(CLRDataEnumMemoryFlags flags);
    HRESULT EnumMemDumpModuleList(CLRDataEnumMemoryFlags flags);
    HRESULT EnumMemDumpAllThreadsStack(CLRDataEnumMemoryFlags flags);

    void ReportMem(TADDR addr, ULONG32 size)
    {
        // This block of code is to help debugging blocks that we report
        // to minidump/heapdump. You can set break point here to view the static
        // variable to figure out the size of blocks that we are reporting.
        // Most useful is set conditional break point to catch large chuck of 
        // memory. We will leave it here for all builds. 
        //         
        static TADDR debugAddr;
        static ULONG32 debugSize;
        debugAddr = addr;
        debugSize = size;

        HRESULT status;        
        if (!addr || addr == (TADDR)-1 || !size)
        {
            return;
        }

        // track the total memory reported. 
        m_cbMemoryReported += size;
        
        if ((status = m_enumMemCb->
             EnumMemoryRegion(TO_CDADDR(addr), size)) != S_OK)
        {
            m_memStatus = status;
        }
    }

    void ClearDumpStats();
    JITNotification* GetHostJitNotificationTable();

    void* GetMetaDataFromHost(PEFile* peFile,
                              bool* isAlternate);
    interface IMDInternalImport* GetMDImport(const PEFile* peFile,
                                             bool throwEx);

    ICLRDataTarget* m_target;
    ICLRDataTarget2* m_target2;
    IXCLRDataTarget3* m_target3;
    ICLRMetadataLocator* m_metadataLocator;
    TADDR m_globalBase;
    DacInstanceManager m_instances;
    ULONG32 m_instanceAge;
    bool m_debugMode;

private:
    LONG m_refs;
    HRESULT m_memStatus;
    DAC_MD_IMPORT* m_mdImports;
    ICLRDataEnumMemoryRegionsCallback* m_enumMemCb;
    JITNotification* m_jitNotificationTable;

    static LONG s_procInit;

    HRESULT GetDacGlobals(void);
    UINT m_cbMemoryReported;
    
    DumpMemoryReportStatics m_dumpStats;
};

extern ClrDataAccess* g_dacImpl;

//----------------------------------------------------------------------------
//
// ClrDataAppDomain.
//
//----------------------------------------------------------------------------

class ClrDataAppDomain : public IXCLRDataAppDomain
{
public:
    ClrDataAppDomain(ClrDataAccess* dac,
                     AppDomain* appDomain);
    ~ClrDataAppDomain(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataAppDomain.
    //

    virtual HRESULT STDMETHODCALLTYPE GetProcess(
        /* [out] */ IXCLRDataProcess **process);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetUniqueID(
        /* [out] */ ULONG64 *id);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataAppDomain *appDomain);

    virtual HRESULT STDMETHODCALLTYPE GetManagedObject(
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    AppDomain* GetAppDomain(void)
    {
        return m_appDomain;
    }

private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    AppDomain* m_appDomain;
};

//----------------------------------------------------------------------------
//
// ClrDataAssembly.
//
//----------------------------------------------------------------------------

class ClrDataAssembly : public IXCLRDataAssembly
{
public:
    ClrDataAssembly(ClrDataAccess* dac,
                    Assembly* assembly);
    ~ClrDataAssembly(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataAssembly.
    //

    virtual HRESULT STDMETHODCALLTYPE StartEnumModules(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumModule(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataModule **mod);

    virtual HRESULT STDMETHODCALLTYPE EndEnumModules(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumAppDomains(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumAppDomain(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataAppDomain **appDomain);

    virtual HRESULT STDMETHODCALLTYPE EndEnumAppDomains(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetFileName(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataAssembly *assembly);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    Assembly* m_assembly;
};

//----------------------------------------------------------------------------
//
// ClrDataModule.
//
//----------------------------------------------------------------------------

class ClrDataModule : public IXCLRDataModule
{
public:
    ClrDataModule(ClrDataAccess* dac,
                  Module* module);
    ~ClrDataModule(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataModule.
    //

    virtual HRESULT STDMETHODCALLTYPE StartEnumAssemblies(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumAssembly(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataAssembly **assembly);

    virtual HRESULT STDMETHODCALLTYPE EndEnumAssemblies(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumAppDomains(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumAppDomain(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataAppDomain **appDomain);

    virtual HRESULT STDMETHODCALLTYPE EndEnumAppDomains(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumTypeDefinitions(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumTypeDefinition(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataTypeDefinition **typeDefinition);

    virtual HRESULT STDMETHODCALLTYPE EndEnumTypeDefinitions(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumTypeInstances(
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumTypeInstance(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataTypeInstance **typeInstance);

    virtual HRESULT STDMETHODCALLTYPE EndEnumTypeInstances(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumTypeDefinitionsByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumTypeDefinitionByName(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataTypeDefinition **type);

    virtual HRESULT STDMETHODCALLTYPE EndEnumTypeDefinitionsByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumTypeInstancesByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataAppDomain *appDomain,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumTypeInstanceByName(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataTypeInstance **type);

    virtual HRESULT STDMETHODCALLTYPE EndEnumTypeInstancesByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetTypeDefinitionByToken(
        /* [in] */ mdTypeDef token,
        /* [out] */ IXCLRDataTypeDefinition **typeDefinition);

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodDefinitionsByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodDefinitionByName(
        /* [in] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodDefinition **method);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodDefinitionsByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodInstancesByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodInstanceByName(
        /* [in] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodInstance **method);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodInstancesByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetMethodDefinitionByToken(
        /* [in] */ mdMethodDef token,
        /* [out] */ IXCLRDataMethodDefinition **methodDefinition);

    virtual HRESULT STDMETHODCALLTYPE StartEnumDataByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [in] */ IXCLRDataTask* tlsTask,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumDataByName(
        /* [in] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE EndEnumDataByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetFileName(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetVersionId(
        /* [out] */ GUID* vid);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataModule *mod);

    virtual HRESULT STDMETHODCALLTYPE StartEnumExtents(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumExtent(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ CLRDATA_MODULE_EXTENT *extent);

    virtual HRESULT STDMETHODCALLTYPE EndEnumExtents(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    HRESULT RequestGetModulePtr(IN ULONG32 inBufferSize,
                              IN BYTE* inBuffer,
                              IN ULONG32 outBufferSize,
                              OUT BYTE* outBuffer);

    Module* GetModule(void)
    {
        return m_module;
    }

private:
    HRESULT GetMdInterface(REFIID interfaceid,
                           IUnknown** cacheIface,
                           PVOID* retIface);

    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    Module* m_module;
    IMetaDataImport* m_mdImport;
    bool m_setExtents;
    CLRDATA_MODULE_EXTENT m_extents[2];
    CLRDATA_MODULE_EXTENT* m_extentsEnd;
};

//----------------------------------------------------------------------------
//
// ClrDataTypeDefinition.
//
//----------------------------------------------------------------------------

class ClrDataTypeDefinition : public IXCLRDataTypeDefinition
{
public:
    ClrDataTypeDefinition(ClrDataAccess* dac,
                          Module* module,
                          mdTypeDef token,
                          TypeHandle typeHandle);
    ~ClrDataTypeDefinition(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataTypeDefinition.
    //

    virtual HRESULT STDMETHODCALLTYPE GetModule(
        /* [out] */ IXCLRDataModule **mod);

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodDefinitions(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodDefinition(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodDefinition **methodDefinition);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodDefinitions(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodDefinitionsByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodDefinitionByName(
        /* [in] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodDefinition **method);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodDefinitionsByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetMethodDefinitionByToken(
        /* [in] */ mdMethodDef token,
        /* [out] */ IXCLRDataMethodDefinition **methodDefinition);

    virtual HRESULT STDMETHODCALLTYPE StartEnumInstances(
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumInstance(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataTypeInstance **instance);

    virtual HRESULT STDMETHODCALLTYPE EndEnumInstances(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetNumFields(
        /* [in] */ ULONG32 flags,
        /* [out] */ ULONG32 *numFields);

    virtual HRESULT STDMETHODCALLTYPE StartEnumFields(
        /* [in] */ ULONG32 flags,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumField(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [in] */ ULONG32 nameBufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ IXCLRDataTypeDefinition **type,
        /* [out] */ ULONG32 *flags,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EnumField2(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [in] */ ULONG32 nameBufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ IXCLRDataTypeDefinition **type,
        /* [out] */ ULONG32 *flags,
        /* [out] */ IXCLRDataModule** tokenScope,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EndEnumFields(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumFieldsByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 nameFlags,
        /* [in] */ ULONG32 fieldFlags,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumFieldByName(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataTypeDefinition **type,
        /* [out] */ ULONG32 *flags,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EnumFieldByName2(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataTypeDefinition **type,
        /* [out] */ ULONG32 *flags,
        /* [out] */ IXCLRDataModule** tokenScope,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EndEnumFieldsByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetFieldByToken(
        /* [in] */ mdFieldDef token,
        /* [in] */ ULONG32 nameBufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ IXCLRDataTypeDefinition **type,
        /* [out] */ ULONG32* flags);

    virtual HRESULT STDMETHODCALLTYPE GetFieldByToken2(
        /* [in] */ IXCLRDataModule* tokenScope,
        /* [in] */ mdFieldDef token,
        /* [in] */ ULONG32 nameBufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ IXCLRDataTypeDefinition **type,
        /* [out] */ ULONG32* flags);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 flags,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetTokenAndScope(
        /* [out] */ mdTypeDef *token,
        /* [out] */ IXCLRDataModule **mod);

    virtual HRESULT STDMETHODCALLTYPE GetCorElementType(
        /* [out] */ CorElementType *type);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE GetBase(
        /* [out] */ IXCLRDataTypeDefinition **base);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataTypeDefinition *type);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    virtual HRESULT STDMETHODCALLTYPE GetArrayRank(
        /* [out] */ ULONG32* rank);

    virtual HRESULT STDMETHODCALLTYPE GetTypeNotification(
        /* [out] */ ULONG32* flags);

    virtual HRESULT STDMETHODCALLTYPE SetTypeNotification(
        /* [in] */ ULONG32 flags);

    static HRESULT NewFromModule(ClrDataAccess* dac,
                                 Module* module,
                                 mdTypeDef token,
                                 ClrDataTypeDefinition** typeDef,
                                 IXCLRDataTypeDefinition** pubTypeDef);

    TypeHandle GetTypeHandle(void)
    {
        return m_typeHandle;
    }

private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    Module* m_module;
    mdTypeDef m_token;
    TypeHandle m_typeHandle;
};

//----------------------------------------------------------------------------
//
// ClrDataTypeInstance.
//
//----------------------------------------------------------------------------

class ClrDataTypeInstance : public IXCLRDataTypeInstance
{
public:
    ClrDataTypeInstance(ClrDataAccess* dac,
                        AppDomain* appDomain,
                        TypeHandle typeHandle);
    ~ClrDataTypeInstance(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataTypeInstance.
    //

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodInstances(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodInstance(
        /* [in, out] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodInstance **methodInstance);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodInstances(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumMethodInstancesByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumMethodInstanceByName(
        /* [in] */ CLRDATA_ENUM* handle,
        /* [out] */ IXCLRDataMethodInstance **method);

    virtual HRESULT STDMETHODCALLTYPE EndEnumMethodInstancesByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetNumStaticFields(
        /* [out] */ ULONG32 *numFields);

    virtual HRESULT STDMETHODCALLTYPE GetStaticFieldByIndex(
        /* [in] */ ULONG32 index,
        /* [in] */ IXCLRDataTask *tlsTask,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE StartEnumStaticFieldsByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataTask *tlsTask,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumStaticFieldByName(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE EndEnumStaticFieldsByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetNumStaticFields2(
        /* [in] */ ULONG32 flags,
        /* [out] */ ULONG32 *numFields);

    virtual HRESULT STDMETHODCALLTYPE StartEnumStaticFields(
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataTask *tlsTask,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumStaticField(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE EnumStaticField2(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **value,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ IXCLRDataModule** tokenScope,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EndEnumStaticFields(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumStaticFieldsByName2(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 nameFlags,
        /* [in] */ ULONG32 fieldFlags,
        /* [in] */ IXCLRDataTask *tlsTask,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumStaticFieldByName3(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **value,
        /* [out] */ IXCLRDataModule** tokenScope,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EnumStaticFieldByName2(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE EndEnumStaticFieldsByName2(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetStaticFieldByToken(
        /* [in] */ mdFieldDef token,
        /* [in] */ IXCLRDataTask *tlsTask,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetStaticFieldByToken2(
        /* [in] */ IXCLRDataModule* tokenScope,
        /* [in] */ mdFieldDef token,
        /* [in] */ IXCLRDataTask *tlsTask,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 flags,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetModule(
        /* [out] */ IXCLRDataModule **mod);

    virtual HRESULT STDMETHODCALLTYPE GetDefinition(
        /* [out] */ IXCLRDataTypeDefinition **typeDefinition);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE GetBase(
        /* [out] */ IXCLRDataTypeInstance **base);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataTypeInstance *type);

    virtual HRESULT STDMETHODCALLTYPE GetNumTypeArguments(
        /* [out] */ ULONG32 *numTypeArgs);

    virtual HRESULT STDMETHODCALLTYPE GetTypeArgumentByIndex(
        /* [in] */ ULONG32 index,
        /* [out] */ IXCLRDataTypeInstance **typeArg);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    static HRESULT NewFromModule(ClrDataAccess* dac,
                                 AppDomain* appDomain,
                                 Module* module,
                                 mdTypeDef token,
                                 ClrDataTypeInstance** typeInst,
                                 IXCLRDataTypeInstance** pubTypeInst);

    TypeHandle GetTypeHandle(void)
    {
        return m_typeHandle;
    }

private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    AppDomain* m_appDomain;
    TypeHandle m_typeHandle;
};

//----------------------------------------------------------------------------
//
// ClrDataMethodDefinition.
//
//----------------------------------------------------------------------------

class ClrDataMethodDefinition : public IXCLRDataMethodDefinition
{
public:
    ClrDataMethodDefinition(ClrDataAccess* dac,
                            Module* module,
                            mdMethodDef token,
                            MethodDesc* methodDesc);
    ~ClrDataMethodDefinition(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataMethodDefinition.
    //

    virtual HRESULT STDMETHODCALLTYPE GetTypeDefinition(
        /* [out] */ IXCLRDataTypeDefinition **typeDefinition);

    virtual HRESULT STDMETHODCALLTYPE StartEnumInstances(
        /* [in] */ IXCLRDataAppDomain* appDomain,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumInstance(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataMethodInstance **instance);

    virtual HRESULT STDMETHODCALLTYPE EndEnumInstances(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 flags,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetTokenAndScope(
        /* [out] */ mdMethodDef *token,
        /* [out] */ IXCLRDataModule **mod);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataMethodDefinition *method);

    virtual HRESULT STDMETHODCALLTYPE GetLatestEnCVersion(
        /* [out] */ ULONG32* version);

    virtual HRESULT STDMETHODCALLTYPE StartEnumExtents(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumExtent(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ CLRDATA_METHDEF_EXTENT *extent);

    virtual HRESULT STDMETHODCALLTYPE EndEnumExtents(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetCodeNotification(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE SetCodeNotification(
        /* [in] */ ULONG32 flags);

    virtual HRESULT STDMETHODCALLTYPE GetRepresentativeEntryAddress(
        /* [out] */ CLRDATA_ADDRESS* addr);

    virtual HRESULT STDMETHODCALLTYPE HasClassOrMethodInstantiation(
        /*[out]*/ BOOL* bGeneric);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    COR_ILMETHOD* GetIlMethod(void);
    
    static HRESULT NewFromModule(ClrDataAccess* dac,
                                 Module* module,
                                 mdMethodDef token,
                                 ClrDataMethodDefinition** methDef,
                                 IXCLRDataMethodDefinition** pubMethDef);

    static HRESULT GetSharedMethodFlags(MethodDesc* methodDesc,
                                        ULONG32* flags);

    // We don't need this if we are able to form name using token
    MethodDesc *GetMethodDesc() { return m_methodDesc;}
private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    Module* m_module;
    mdMethodDef m_token;
    MethodDesc* m_methodDesc;
};

//----------------------------------------------------------------------------
//
// ClrDataMethodInstance.
//
//----------------------------------------------------------------------------

class ClrDataMethodInstance : public IXCLRDataMethodInstance
{
public:
    ClrDataMethodInstance(ClrDataAccess* dac,
                          AppDomain* appDomain,
                          MethodDesc* methodDesc);
    ~ClrDataMethodInstance(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataMethodInstance.
    //

    virtual HRESULT STDMETHODCALLTYPE GetTypeInstance(
        /* [out] */ IXCLRDataTypeInstance **typeInstance);

    virtual HRESULT STDMETHODCALLTYPE GetDefinition(
        /* [out] */ IXCLRDataMethodDefinition **methodDefinition);

    virtual HRESULT STDMETHODCALLTYPE GetTokenAndScope(
        /* [out] */ mdMethodDef *token,
        /* [out] */ IXCLRDataModule **mod);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 flags,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataMethodInstance *method);

    virtual HRESULT STDMETHODCALLTYPE GetEnCVersion(
        /* [out] */ ULONG32* version);

    virtual HRESULT STDMETHODCALLTYPE GetNumTypeArguments(
        /* [out] */ ULONG32 *numTypeArgs);

    virtual HRESULT STDMETHODCALLTYPE GetTypeArgumentByIndex(
        /* [in] */ ULONG32 index,
        /* [out] */ IXCLRDataTypeInstance **typeArg);

    virtual HRESULT STDMETHODCALLTYPE GetILOffsetsByAddress(
        /* [in] */ CLRDATA_ADDRESS address,
        /* [in] */ ULONG32 offsetsLen,
        /* [out] */ ULONG32 *offsetsNeeded,
        /* [size_is][out] */ ULONG32 ilOffsets[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetAddressRangesByILOffset(
        /* [in] */ ULONG32 ilOffset,
        /* [in] */ ULONG32 rangesLen,
        /* [out] */ ULONG32 *rangesNeeded,
        /* [size_is][out] */ CLRDATA_ADDRESS_RANGE addressRanges[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetILAddressMap(
        /* [in] */ ULONG32 mapLen,
        /* [out] */ ULONG32 *mapNeeded,
        /* [size_is][out] */ CLRDATA_IL_ADDRESS_MAP maps[  ]);

    virtual HRESULT STDMETHODCALLTYPE StartEnumExtents(
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumExtent(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ CLRDATA_ADDRESS_RANGE *extent);

    virtual HRESULT STDMETHODCALLTYPE EndEnumExtents(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetRepresentativeEntryAddress(
        /* [out] */ CLRDATA_ADDRESS* addr);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    static HRESULT NewFromModule(ClrDataAccess* dac,
                                 AppDomain* appDomain,
                                 Module* module,
                                 mdMethodDef token,
                                 ClrDataMethodInstance** methInst,
                                 IXCLRDataMethodInstance** pubMethInst);

private:
    friend class ClrDataAccess;
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    AppDomain* m_appDomain;
    MethodDesc* m_methodDesc;
};

//----------------------------------------------------------------------------
//
// ClrDataTask.
//
//----------------------------------------------------------------------------

class ClrDataTask : public IXCLRDataTask
{
public:
    ClrDataTask(ClrDataAccess* dac,
                Thread* Thread);
    ~ClrDataTask(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataTask.
    //

    virtual HRESULT STDMETHODCALLTYPE GetProcess(
        /* [out] */ IXCLRDataProcess **process);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentAppDomain(
        /* [out] */ IXCLRDataAppDomain **appDomain);

    virtual HRESULT STDMETHODCALLTYPE GetName(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetUniqueID(
        /* [out] */ ULONG64 *id);

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE IsSameObject(
        /* [in] */ IXCLRDataTask *task);

    virtual HRESULT STDMETHODCALLTYPE GetManagedObject(
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE GetDesiredExecutionState(
        /* [out] */ ULONG32 *state);

    virtual HRESULT STDMETHODCALLTYPE SetDesiredExecutionState(
        /* [in] */ ULONG32 state);

    virtual HRESULT STDMETHODCALLTYPE CreateStackWalk(
        /* [in] */ ULONG32 flags,
        /* [out] */ IXCLRDataStackWalk **stackWalk);

    virtual HRESULT STDMETHODCALLTYPE GetOSThreadID(
        /* [out] */ ULONG32 *id);

    virtual HRESULT STDMETHODCALLTYPE GetContext(
        /* [in] */ ULONG32 contextFlags,
        /* [in] */ ULONG32 contextBufSize,
        /* [out] */ ULONG32 *contextSize,
        /* [size_is][out] */ BYTE contextBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE SetContext(
        /* [in] */ ULONG32 contextSize,
        /* [size_is][in] */ BYTE context[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetCurrentExceptionState(
        /* [out] */ IXCLRDataExceptionState **exception);

    virtual HRESULT STDMETHODCALLTYPE GetLastExceptionState(
        /* [out] */ IXCLRDataExceptionState **exception);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    HRESULT RequestIsCantStop(IN ULONG32 inBufferSize,
                              IN BYTE* inBuffer,
                              IN ULONG32 outBufferSize,
                              OUT BYTE* outBuffer);

    Thread* GetThread(void)
    {
        return m_thread;
    }

private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    Thread* m_thread;
};

//----------------------------------------------------------------------------
//
// ClrDataStackWalk.
//
//----------------------------------------------------------------------------

class ClrDataStackWalk : public IXCLRDataStackWalk
{
public:
    ClrDataStackWalk(ClrDataAccess* dac,
                     Thread* Thread,
                     ULONG32 flags);
    ~ClrDataStackWalk(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataStackWalk.
    //

    virtual HRESULT STDMETHODCALLTYPE GetContext(
        /* [in] */ ULONG32 contextFlags,
        /* [in] */ ULONG32 contextBufSize,
        /* [out] */ ULONG32 *contextSize,
        /* [size_is][out] */ BYTE contextBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE SetContext(
        /* [in] */ ULONG32 contextSize,
        /* [size_is][in] */ BYTE context[  ]);

    virtual HRESULT STDMETHODCALLTYPE Next( void);

    virtual HRESULT STDMETHODCALLTYPE GetStackSizeSkipped(
        /* [out] */ ULONG64 *stackSizeSkipped);

    virtual HRESULT STDMETHODCALLTYPE GetFrameType(
        /* [out] */ CLRDataSimpleFrameType *simpleType,
        /* [out] */ CLRDataDetailedFrameType *detailedType);

    virtual HRESULT STDMETHODCALLTYPE GetFrame(
        /* [out] */ IXCLRDataFrame **frame);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    virtual HRESULT STDMETHODCALLTYPE SetContext2(
        /* [in] */ ULONG32 flags,
        /* [in] */ ULONG32 contextSize,
        /* [size_is][in] */ BYTE context[  ]);

    HRESULT Init(void);

private:
    void FilterFrames(void);
    void UpdateContextFromRegDisp(PREGDISPLAY regDisp,
                                  PCONTEXT context);
    void RawGetFrameType(
        /* [out] */ CLRDataSimpleFrameType* simpleType,
        /* [out] */ CLRDataDetailedFrameType* detailedType);

    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    Thread* m_thread;
    ULONG32 m_walkFlags;
    StackFrameIterator m_frameIter;
    REGDISPLAY m_regDisp;
    CONTEXT m_context;
    TADDR m_stackPrev;
};

//----------------------------------------------------------------------------
//
// ClrDataFrame.
//
//----------------------------------------------------------------------------

class ClrDataFrame : public IXCLRDataFrame
{
    friend class ClrDataStackWalk;

public:
    ClrDataFrame(ClrDataAccess* dac,
                 CLRDataSimpleFrameType simpleType,
                 CLRDataDetailedFrameType detailedType,
                 AppDomain* appDomain,
                 MethodDesc* methodDesc);
    ~ClrDataFrame(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataFrame.
    //

    virtual HRESULT STDMETHODCALLTYPE GetContext(
        /* [in] */ ULONG32 contextFlags,
        /* [in] */ ULONG32 contextBufSize,
        /* [out] */ ULONG32 *contextSize,
        /* [size_is][out] */ BYTE contextBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetFrameType(
        /* [out] */ CLRDataSimpleFrameType *simpleType,
        /* [out] */ CLRDataDetailedFrameType *detailedType);

    virtual HRESULT STDMETHODCALLTYPE GetAppDomain(
        /* [out] */ IXCLRDataAppDomain **appDomain);

    virtual HRESULT STDMETHODCALLTYPE GetNumArguments(
        /* [out] */ ULONG32 *numParams);

    virtual HRESULT STDMETHODCALLTYPE GetArgumentByIndex(
        /* [in] */ ULONG32 index,
        /* [out] */ IXCLRDataValue **arg,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetNumLocalVariables(
        /* [out] */ ULONG32 *numLocals);

    virtual HRESULT STDMETHODCALLTYPE GetLocalVariableByIndex(
        /* [in] */ ULONG32 index,
        /* [out] */ IXCLRDataValue **localVariable,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR name[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetNumTypeArguments(
        /* [out] */ ULONG32 *numTypeArgs);

    virtual HRESULT STDMETHODCALLTYPE GetTypeArgumentByIndex(
        /* [in] */ ULONG32 index,
        /* [out] */ IXCLRDataTypeInstance **typeArg);

    virtual HRESULT STDMETHODCALLTYPE GetCodeName(
        /* [in] */ ULONG32 flags,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetMethodInstance(
        /* [out] */ IXCLRDataMethodInstance **method);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

private:
    HRESULT GetMethodSig(MetaSig** sig,
                         ULONG32* count);
    HRESULT GetLocalSig(MetaSig** sig,
                        ULONG32* count);
    HRESULT ValueFromDebugInfo(MetaSig* sig,
                               bool isArg,
                               ULONG32 sigIndex,
                               ULONG32 varInfoSlot,
                               IXCLRDataValue** value);

    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    CLRDataSimpleFrameType m_simpleType;
    CLRDataDetailedFrameType m_detailedType;
    AppDomain* m_appDomain;
    MethodDesc* m_methodDesc;
    REGDISPLAY m_regDisp;
    CONTEXT m_context;
    MetaSig* m_methodSig;
    MetaSig* m_localSig;
};

//----------------------------------------------------------------------------
//
// ClrDataExceptionState.
//
//----------------------------------------------------------------------------

typedef ExInfo ClrDataExStateType;


class ClrDataExceptionState : public IXCLRDataExceptionState
{
public:
    ClrDataExceptionState(ClrDataAccess* dac,
                          AppDomain* appDomain,
                          Thread* thread,
                          ULONG32 flags,
                          ClrDataExStateType* exInfo,
                          OBJECTHANDLE throwable,
                          ClrDataExStateType* prevExInfo);
    ~ClrDataExceptionState(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataExceptionState.
    //

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE GetPrevious(
        /* [out] */ IXCLRDataExceptionState **exState);

    virtual HRESULT STDMETHODCALLTYPE GetManagedObject(
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE GetBaseType(
        /* [out] */ CLRDataBaseExceptionType *type);

    virtual HRESULT STDMETHODCALLTYPE GetCode(
        /* [out] */ ULONG32 *code);

    virtual HRESULT STDMETHODCALLTYPE GetString(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *strLen,
        /* [size_is][out] */ WCHAR str[  ]);

    virtual HRESULT STDMETHODCALLTYPE IsSameState(
        /* [in] */ EXCEPTION_RECORD64 *exRecord,
        /* [in] */ ULONG32 contextSize,
        /* [size_is][in] */ BYTE cxRecord[  ]);

    virtual HRESULT STDMETHODCALLTYPE IsSameState2(
        /* [in] */ ULONG32 flags,
        /* [in] */ EXCEPTION_RECORD64 *exRecord,
        /* [in] */ ULONG32 contextSize,
        /* [size_is][in] */ BYTE cxRecord[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetTask(
        /* [out] */ IXCLRDataTask** task);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    static HRESULT NewFromThread(ClrDataAccess* dac,
                                 Thread* thread,
                                 ClrDataExceptionState** exception,
                                 IXCLRDataExceptionState** pubException);
    
    PTR_CONTEXT          GetCurrentContextRecord();
    PTR_EXCEPTION_RECORD GetCurrentExceptionRecord();

friend class ClrDataAccess;    
private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    AppDomain* m_appDomain;
    Thread* m_thread;
    ULONG32 m_flags;
    ClrDataExStateType* m_exInfo;
    OBJECTHANDLE m_throwable;
    ClrDataExStateType* m_prevExInfo;
};

//----------------------------------------------------------------------------
//
// ClrDataValue.
//
//----------------------------------------------------------------------------

class ClrDataValue : public IXCLRDataValue
{
public:
    ClrDataValue(ClrDataAccess* dac,
                 AppDomain* appDomain,
                 Thread* thread,
                 ULONG32 flags,
                 TypeHandle typeHandle,
                 ULONG64 baseAddr,
                 ULONG32 numLocs,
                 NativeVarLocation* locs);
    ~ClrDataValue(void);

    // IUnknown.
    STDMETHOD(QueryInterface)(THIS_
                              IN REFIID interfaceId,
                              OUT PVOID* iface);
    STDMETHOD_(ULONG, AddRef)(THIS);
    STDMETHOD_(ULONG, Release)(THIS);

    //
    // IXCLRDataValue.
    //

    virtual HRESULT STDMETHODCALLTYPE GetFlags(
        /* [out] */ ULONG32 *flags);

    virtual HRESULT STDMETHODCALLTYPE GetAddress(
        /* [out] */ CLRDATA_ADDRESS *address);

    virtual HRESULT STDMETHODCALLTYPE GetSize(
        /* [out] */ ULONG64 *size);

    virtual HRESULT STDMETHODCALLTYPE GetBytes(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *dataSize,
        /* [size_is][out] */ BYTE buffer[  ]);

    virtual HRESULT STDMETHODCALLTYPE SetBytes(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *dataSize,
        /* [size_is][in] */ BYTE buffer[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetType(
        /* [out] */ IXCLRDataTypeInstance **typeInstance);

    virtual HRESULT STDMETHODCALLTYPE GetNumFields(
        /* [out] */ ULONG32 *numFields);

    virtual HRESULT STDMETHODCALLTYPE GetFieldByIndex(
        /* [in] */ ULONG32 index,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE GetNumFields2(
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataTypeInstance *fromType,
        /* [out] */ ULONG32 *numFields);

    virtual HRESULT STDMETHODCALLTYPE StartEnumFields(
        /* [in] */ ULONG32 flags,
        /* [in] */ IXCLRDataTypeInstance *fromType,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumField(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 nameBufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EnumField2(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 nameBufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ],
        /* [out] */ IXCLRDataModule** tokenScope,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EndEnumFields(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE StartEnumFieldsByName(
        /* [in] */ LPCWSTR name,
        /* [in] */ ULONG32 nameFlags,
        /* [in] */ ULONG32 fieldFlags,
        /* [in] */ IXCLRDataTypeInstance *fromType,
        /* [out] */ CLRDATA_ENUM *handle);

    virtual HRESULT STDMETHODCALLTYPE EnumFieldByName(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **field,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EnumFieldByName2(
        /* [out][in] */ CLRDATA_ENUM *handle,
        /* [out] */ IXCLRDataValue **field,
        /* [out] */ IXCLRDataModule** tokenScope,
        /* [out] */ mdFieldDef *token);

    virtual HRESULT STDMETHODCALLTYPE EndEnumFieldsByName(
        /* [in] */ CLRDATA_ENUM handle);

    virtual HRESULT STDMETHODCALLTYPE GetFieldByToken(
        /* [in] */ mdFieldDef token,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetFieldByToken2(
        /* [in] */ IXCLRDataModule* tokenScope,
        /* [in] */ mdFieldDef token,
        /* [out] */ IXCLRDataValue **field,
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *nameLen,
        /* [size_is][out] */ WCHAR nameBuf[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetAssociatedValue(
        /* [out] */ IXCLRDataValue **assocValue);

    virtual HRESULT STDMETHODCALLTYPE GetAssociatedType(
        /* [out] */ IXCLRDataTypeInstance **assocType);

    virtual HRESULT STDMETHODCALLTYPE GetString(
        /* [in] */ ULONG32 bufLen,
        /* [out] */ ULONG32 *strLen,
        /* [size_is][out] */ WCHAR str[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetArrayProperties(
        /* [out] */ ULONG32 *rank,
        /* [out] */ ULONG32 *totalElements,
        /* [in] */ ULONG32 numDim,
        /* [size_is][out] */ ULONG32 dims[  ],
        /* [in] */ ULONG32 numBases,
        /* [size_is][out] */ LONG32 bases[  ]);

    virtual HRESULT STDMETHODCALLTYPE GetArrayElement(
        /* [in] */ ULONG32 numInd,
        /* [size_is][in] */ LONG32 indices[  ],
        /* [out] */ IXCLRDataValue **value);

    virtual HRESULT STDMETHODCALLTYPE GetNumLocations(
        /* [out] */ ULONG32* numLocs);
    
    virtual HRESULT STDMETHODCALLTYPE GetLocationByIndex(
        /* [in] */ ULONG32 loc,
        /* [out] */ ULONG32* flags,
        /* [out] */ CLRDATA_ADDRESS* arg);

    virtual HRESULT STDMETHODCALLTYPE Request(
        /* [in] */ ULONG32 reqCode,
        /* [in] */ ULONG32 inBufferSize,
        /* [size_is][in] */ BYTE *inBuffer,
        /* [in] */ ULONG32 outBufferSize,
        /* [size_is][out] */ BYTE *outBuffer);

    HRESULT GetRefAssociatedValue(IXCLRDataValue** assocValue);

    static HRESULT NewFromFieldDesc(ClrDataAccess* dac,
                                    AppDomain* appDomain,
                                    ULONG32 flags,
                                    FieldDesc* fieldDesc,
                                    TADDR objBase,
                                    Thread* tlsThread,
                                    ClrDataValue** value,
                                    IXCLRDataValue** pubValue,
                                    ULONG32 nameBufRetLen,
                                    ULONG32* nameLenRet,
                                    WCHAR nameBufRet[  ],
                                    IXCLRDataModule** tokenScopeRet,
                                    mdFieldDef* tokenRet);

    HRESULT NewFromSubField(FieldDesc* fieldDesc,
                            ULONG32 flags,
                            ClrDataValue** value,
                            IXCLRDataValue** pubValue,
                            ULONG32 nameBufRetLen,
                            ULONG32* nameLenRet,
                            WCHAR nameBufRet[  ],
                            IXCLRDataModule** tokenScopeRet,
                            mdFieldDef* tokenRet)
    {
        return ClrDataValue::NewFromFieldDesc(m_dac,
                                              m_appDomain,
                                              flags,
                                              fieldDesc,
                                              m_baseAddr,
                                              m_thread,
                                              value,
                                              pubValue,
                                              nameBufRetLen,
                                              nameLenRet,
                                              nameBufRet,
                                              tokenScopeRet,
                                              tokenRet);
    }

    bool CanHaveFields(void)
    {
        return (m_flags & CLRDATA_VALUE_IS_REFERENCE) == 0;
    }

    HRESULT IntGetBytes(
        /* [in] */ ULONG32 bufLen,
        /* [size_is][out] */ BYTE buffer[  ]);

private:
    LONG m_refs;
    ClrDataAccess* m_dac;
    ULONG32 m_instanceAge;
    AppDomain* m_appDomain;
    Thread* m_thread;
    ULONG32 m_flags;
    TypeHandle m_typeHandle;
    ULONG64 m_totalSize;
    ULONG64 m_baseAddr;
    ULONG32 m_numLocs;
    NativeVarLocation m_locs[MAX_NATIVE_VAR_LOCS];
};

//----------------------------------------------------------------------------
//
// EnumMethodDefinitions.
//
//----------------------------------------------------------------------------

class EnumMethodDefinitions
{
public:
    HRESULT Start(Module* mod,
                  bool useAddrFilter,
                  CLRDATA_ADDRESS addrFilter);
    HRESULT Next(ClrDataAccess* dac,
                 IXCLRDataMethodDefinition **method);

    static HRESULT CdStart(Module* mod,
                           bool useAddrFilter,
                           CLRDATA_ADDRESS addrFilter,
                           CLRDATA_ENUM* handle);
    static HRESULT CdNext(ClrDataAccess* dac,
                          CLRDATA_ENUM* handle,
                          IXCLRDataMethodDefinition** method);
    static HRESULT CdEnd(CLRDATA_ENUM handle);

    Module* m_module;
    bool m_useAddrFilter;
    CLRDATA_ADDRESS m_addrFilter;
    MetaEnum m_typeEnum;
    mdToken m_typeToken;
    bool m_needMethodStart;
    MetaEnum m_methodEnum;
};

//----------------------------------------------------------------------------
//
// EnumMethodInstances.
//
//----------------------------------------------------------------------------

class EnumMethodInstances
{
public:
    EnumMethodInstances(MethodDesc* methodDesc,
                        IXCLRDataAppDomain* givenAppDomain);

    HRESULT Next(ClrDataAccess* dac,
                 IXCLRDataMethodInstance **instance);

    static HRESULT CdStart(MethodDesc* methodDesc,
                           IXCLRDataAppDomain* appDomain,
                           CLRDATA_ENUM* handle);
    static HRESULT CdNext(ClrDataAccess* dac,
                          CLRDATA_ENUM* handle,
                          IXCLRDataMethodInstance** method);
    static HRESULT CdEnd(CLRDATA_ENUM handle);

    MethodDesc* m_methodDesc;
    AppDomain* m_givenAppDomain;
    bool m_givenAppDomainUsed;
    AppDomainIterator m_domainIter;
    AppDomain* m_appDomain;
    LoadedMethodDescIterator m_methodIter;
};

//----------------------------------------------------------------------------
//
// Internal functions.
//
//----------------------------------------------------------------------------

#define DAC_ENTER() \
    EnterCriticalSection(&g_dacCritSec); \
    ClrDataAccess* __prevDacImpl = g_dacImpl; \
    g_dacImpl = this;

// When entering a child object we validate that
// the process's host instance cache hasn't been flushed
// since the child was created.
#define DAC_ENTER_SUB(dac) \
    EnterCriticalSection(&g_dacCritSec); \
    if (dac->m_instanceAge != m_instanceAge) \
    { \
        LeaveCriticalSection(&g_dacCritSec); \
        return E_INVALIDARG; \
    } \
    ClrDataAccess* __prevDacImpl = g_dacImpl; \
    g_dacImpl = (dac)

#define DAC_LEAVE() \
    g_dacImpl = __prevDacImpl; \
    LeaveCriticalSection(&g_dacCritSec)

HRESULT DacGetHostVtPtrs(void);
bool DacExceptionFilter(Exception* ex, ClrDataAccess* process,
                        HRESULT* status);
Thread* __stdcall DacGetThread(ULONG32 osThread);
inline Thread* __stdcall DacGetThread(void) { return DacGetThread(0); }
AppDomain* __stdcall DacGetAppDomain(void);
BOOL DacGetThreadContext(Thread* thread, CONTEXT* context);

BOOL DacValidateMD(MethodDesc *pMD, HINSTANCE hMscorwks);


// Imports from request_svr.cpp, to provide data we need from the SVR namespace
#ifndef GC_SMP
int GCHeapCount();
HRESULT GetServerHeapData(CLRDATA_ADDRESS addr, DacpHeapSegmentData *pSegment);
HRESULT GetServerHeaps(CLRDATA_ADDRESS pGCHeaps[], ICLRDataTarget* pTarget);
#endif


#endif // #ifndef __DACIMPL_H__
