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

#ifndef _DYNAMICMETHOD_H_
#define _DYNAMICMETHOD_H_

#include "jitinterface.h"
#include "methodtable.h"
#include <daccess.h>

//
// This links together a set of news and release in one object.
// The idea is to have a predefined size allocated up front and used by different calls to new.
// All the allocation will be released at the same time releaseing an instance of this class
// Here is how the object is laid out
// | ptr_to_next_chunk | size_left_in_chunk | data | ... | data 
// This is not a particularly efficient allocator but it works well for a small number of allocation
// needed while jitting a method
class ChunkAllocator {

private:
    #define CHUNK_SIZE 64

    BYTE *m_pData;

public:
    ChunkAllocator() : m_pData(NULL) {}

    ~ChunkAllocator();
    void* New(size_t size);
    void Delete();

};

class DynamicResolver 
{
public:
    // Keep in sync with dynamicIlGenerator.cs
    enum SecurityControlFlags
    {
        SkipVisibilityChecks = 0x1
    };


    // set up and clean up for jitting
    virtual void FreeCompileTimeState() = 0;
    virtual void GetJitContext(DWORD *securityControlFlags, TypeHandle *typeOwner) = 0;
    virtual ChunkAllocator* GetJitMetaHeap() = 0;

    //
    // code info data
    virtual BYTE* GetCodeInfo(unsigned *pCodeSize, unsigned short *pStackSize, CorInfoOptions *pOptions, unsigned short *pEHSize) = 0;
    virtual PCCOR_SIGNATURE GetLocalSig(DWORD *pSigSize) = 0;

    //
    // jit interface api
    virtual StringObject* GetStringLiteral(mdToken token) = 0;
    virtual void* ResolveToken(mdToken token) = 0;
    virtual unsigned ResolveParentToken(mdToken token) = 0;
    virtual PCCOR_SIGNATURE ResolveSignature(mdToken token) = 0;
    virtual PCCOR_SIGNATURE ResolveSignatureForVarArg(mdToken token) = 0;
    virtual void GetEHInfo(unsigned EHnumber, CORINFO_EH_CLAUSE* clause) = 0;
    virtual BOOL IsValidToken(mdToken token) = 0;
    virtual void* GetInstantiationInfo(mdToken token) = 0;

    virtual MethodDesc* GetDynamicMethod() = 0;
};

class LCGMethodResolver : public DynamicResolver 
{
    friend class DynamicMethodDesc;
    friend class EEJitManager;

public:
    DynamicMethodDesc* m_pDynamicMethod;
    OBJECTHANDLE m_managedResolver;
    BYTE *m_Code;
    DWORD m_CodeSize;
    PCCOR_SIGNATURE m_LocalSig;
    DWORD m_LocalSigSize;
    unsigned short m_StackSize;
    CorInfoOptions m_Options;
    unsigned short m_EHSize;
    DynamicMethodTable *m_DynamicMethodTable;
    DynamicMethodDesc *m_next;
    void *m_recordCodePointer;
    ChunkAllocator m_jitMetaHeap;
    ChunkAllocator m_jitTempData;

public:

    LCGMethodResolver() {}
    void Destroy(BOOL fRecycle);

    void FreeCompileTimeState();
    void GetJitContext(DWORD *securityControlFlags, TypeHandle *typeOwner);
    ChunkAllocator* GetJitMetaHeap();

    BYTE* GetCodeInfo(unsigned *pCodeSize, unsigned short *pStackSize, CorInfoOptions *pOptions, unsigned short *pEHSize);
    PCCOR_SIGNATURE GetLocalSig(DWORD *pSigSize);
    StringObject* GetStringLiteral(mdToken token);
    void* ResolveToken(mdToken token);
    unsigned ResolveParentToken(mdToken token);
    PCCOR_SIGNATURE ResolveSignature(mdToken token);
    PCCOR_SIGNATURE ResolveSignatureForVarArg(mdToken token);
    void GetEHInfo(unsigned EHnumber, CORINFO_EH_CLAUSE* clause);
    BOOL IsValidToken(mdToken token);
    void* GetInstantiationInfo(mdToken token);
    
    MethodDesc* GetDynamicMethod() { LEAF_CONTRACT; return m_pDynamicMethod; }
};

#ifndef DACCESS_COMPILE
        
//
// this class provides the static implementation
// of ICorDynamicInfo for CEEInfo
//
class CEEDynamicCodeInfo
{
public:
    ////////////////////
    //
    // ICorDynamicInfo
    //
    ////////////////////

    static InfoAccessType constructStringLiteral(
        CORINFO_MODULE_HANDLE module,
        mdToken metaTok,
        void **ppInfo);

    // returns the class typedesc given a methodTok (needed for arrays since
    // they share a common method table, so we can't use getMethodClass)
    static CORINFO_CLASS_HANDLE findMethodClass(
        CORINFO_MODULE_HANDLE module,
        mdToken methodTok,
        CORINFO_METHOD_HANDLE context);

    ///////////////////
    //
    // ICorMethodInfo
    //
    ///////////////////

    static void getEHinfo(
        CORINFO_METHOD_HANDLE ftn, 
        unsigned EHnumber, 
        CORINFO_EH_CLAUSE* clause);

    ///////////////////
    //
    // ICorModuleInfo
    //
    ///////////////////

    static CORINFO_CLASS_HANDLE findClassInternal(
        CORINFO_MODULE_HANDLE module,
        unsigned metaTOK,
        CORINFO_METHOD_HANDLE context,
        BOOL permitUninstantiatedType,
        OBJECTREF *pThrowable);
    
    static FieldDesc* findField(
        CORINFO_MODULE_HANDLE module,
        unsigned metaTOK,
        const SigTypeContext *context,
        MethodTable **ppOwner, 
        DWORD *pdwSecurityControlFlags, 
        OBJECTREF *pThrowable);
    
    // Given a method token metaTOK, use context to instantiate any type variables in its *parent* type and return a method handle
    // This looks up a function by token (what the IL CALLVIRT, CALLSTATIC instructions use)
    // the context parameter is used to do access checks.  If 0, no access checks are done
    // If metaTOK is combined with the flag CORINFO_ANNOT_ALLOWINSTPARAM and it refers to a generic method or an instance method in a generic struct, 
    // then the method handle returned might be for shared code which expects an extra parameter providing extra instantiation information.
    // Otherwise (the default), a specialized stub method is returned that doesn't take this parameter 
    static MethodDesc* findMethod(
        CORINFO_MODULE_HANDLE module,
        unsigned metaTOK,
        const SigTypeContext *context,
        MethodTable **ppOwner, 
        DWORD *pdwSecurityControlFlags, 
        OBJECTREF *pThrowable);

    static unsigned getMemberParent(
        CORINFO_MODULE_HANDLE scopeHnd, 
        unsigned metaTOK);
    
    static PCCOR_SIGNATURE findSig(
        CORINFO_MODULE_HANDLE *module,
        unsigned *sigTOK,
        CORINFO_METHOD_HANDLE context); 
    
    // for Varargs, the signature at the call site may differ from
    // the signature at the definition.  Thus we need a way of
    // fetching the call site information
    static PCCOR_SIGNATURE findCallSiteSig(
        CORINFO_MODULE_HANDLE *module,
        unsigned *methTOK,
        CORINFO_METHOD_HANDLE *context);

    static size_t findNameOfToken(
        CORINFO_MODULE_HANDLE module, 
        mdToken metaTOK,
        __out_ecount (FQNameCapacity) char * szFQName,
        size_t FQNameCapacity);

    static BOOL isValidToken(
        CORINFO_MODULE_HANDLE module, 
        unsigned metaTOK);

    static BOOL isValidStringRef(
        CORINFO_MODULE_HANDLE module, 
        unsigned metaTOK);

};



#endif

// -----------------------------------------------------------------------------------------------
// a DynamicMethodTable is used by the light code generation to lazily allocate methods.
// The methods in this MethodTable are not known up front and their signature is defined
// at runtime
// -----------------------------------------------------------------------------------------------
class DynamicMethodTable
{

public:
#ifndef DACCESS_COMPILE
    static void CreateDynamicMethodTable(DynamicMethodTable **ppLocation, Module *pModule, BaseDomain *pDomain);
#endif

private:
    CrstExplicitInit m_Crst;
    DynamicMethodDesc *m_DynamicMethodList;
    MethodTable *m_pMethodTable;
    Module *m_Module;
    BaseDomain *m_pDomain;

    DynamicMethodTable() {WRAPPER_CONTRACT;}

    class LockHolder : public CrstPreempHolder
    {
      public:
        LockHolder(DynamicMethodTable *pDynMT, BOOL Take = TRUE)
            : CrstPreempHolder(&pDynMT->m_Crst, Take)
        {
            WRAPPER_CONTRACT;
        }
    };
    friend class LockHolder;

#ifndef DACCESS_COMPILE
    void MakeMethodTable(AllocMemTracker *pamTracker);
    void AddMethodsToList();

public:
    void Destroy();
    DynamicMethodDesc* GetDynamicMethod(BYTE *psig, DWORD sigSize, PTR_CUTF8 name);
    void LinkMethod(DynamicMethodDesc *pMethod);

#endif

#ifdef _DEBUG
public:
    DWORD m_Used;
#endif

};

//
// Implementation of the CodeHeap for DynamicMethods.
// This CodeHeap uses the host interface VirtualAlloc/Free and allows
// for reclamation of generated code
// (Check the base class - CodeHeap in codeman.h - for comments on the functions)
//
#define CODEHEAP_SIZE_ALIGN 64
typedef VPTR(class HostCodeHeap) PTR_HostCodeHeap;
class HostCodeHeap : CodeHeap
{
#ifdef DACCESS_COMPILE
    friend class ClrDataAccess;
#endif

    VPTR_VTABLE_CLASS(HostCodeHeap, CodeHeap)

private:
    // pointer back to jit manager info
    PTR_HeapList m_pHeapList;
    PTR_EEJitManager m_pJitManager;
    // basic allocation data
    PTR_BYTE m_pBaseAddr;
    PTR_BYTE m_pLastAvailableCommittedAddr;
    size_t m_TotalBytesAvailable;
    size_t m_ReservedData;
    // Heap ref count
    DWORD m_AllocationCount;

    // data to track free list and pointers into this h;eap
    // - on an used block this struct has got a pointer back to the CodeHeap, size and start of aligned allocation
    // - on an unused block (free block) this tracks the size of the block and the pointer to the next non contiguos free block
    struct TrackAllocation {
        union {
            HostCodeHeap *pHeap;
            TrackAllocation *pNext;
        };
        size_t size;
        void *pad; // just to ensure there is room for where the block starts
    };
    TrackAllocation *m_pFreeList;

    // used for cleanup. Keep track of the next potential heap to release. Normally NULL
    HostCodeHeap *m_pNextHeapToRelease;
    BaseDomain *m_pDomain;

public:
    static HeapList* CreateCodeHeap(CodeHeapRequestInfo *pInfo, EEJitManager *pJitManager);

private:
    HostCodeHeap(size_t ReserveBlockSize, EEJitManager *pJitManager, CodeHeapRequestInfo *pInfo);
    BYTE* InitCodeHeapPrivateData(size_t ReserveBlockSize, size_t otherData, size_t nibbleMapSize);
    void* AllocFromFreeList(size_t size, DWORD alignment);
    void AddToFreeList(TrackAllocation *pBlockToInsert);
    static size_t GetPadding(TrackAllocation *pCurrent, size_t size, DWORD alignement);

public:
    virtual ~HostCodeHeap();
    virtual void* AllocMemory(size_t size, DWORD alignment);
    virtual void* AllocMemory_NoThrow(size_t size, DWORD alignment);
    virtual void* GetHeapStartAddress();

#ifdef _DEBUG
    virtual size_t GetReservedPrivateData();
#endif

#ifdef DACCESS_COMPILE
    virtual void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    static TrackAllocation * GetTrackAllocation(CodeHeader* pCodeHdr);
    static HostCodeHeap* GetCodeHeap(CodeHeader* pCodeHdr);

    // cleanup logic for code heaps
    static HostCodeHeap *s_cleanupList;
    static CrstStatic s_CleanupCrst;
    static void Init();
    static void CleanupCodeHeaps();
    void DestroyCodeHeap();

protected:
    friend class DynamicMethodDesc;
    friend class LCGMethodResolver;
    virtual void ReleaseReferenceToHeap(MethodDesc* pMD);
    virtual void DestroyHeap();

};



inline MethodDesc* GetMethod(CORINFO_METHOD_HANDLE methodHandle)
{
    LEAF_CONTRACT;
    return (MethodDesc*) methodHandle;
}

#ifndef DACCESS_COMPILE

#define CORINFO_MODULE_HANDLE_TYPE_MASK 1

enum CORINFO_MODULE_HANDLE_TYPES
{
    CORINFO_NORMAL_MODULE  = 0,
    CORINFO_DYNAMIC_MODULE,
};

inline bool IsDynamicScope(CORINFO_MODULE_HANDLE module)
{
    LEAF_CONTRACT;
    return (CORINFO_DYNAMIC_MODULE == (((size_t)module) & CORINFO_MODULE_HANDLE_TYPE_MASK));
}

inline CORINFO_MODULE_HANDLE MakeDynamicScope(DynamicResolver* pResolver)
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(0 == (((size_t)pResolver) & CORINFO_MODULE_HANDLE_TYPE_MASK));
    return (CORINFO_MODULE_HANDLE)(((size_t)pResolver) | CORINFO_DYNAMIC_MODULE);
}

inline DynamicResolver* GetDynamicResolver(CORINFO_MODULE_HANDLE module)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(IsDynamicScope(module));
    return (DynamicResolver*)(((size_t)module) & ~((size_t)CORINFO_MODULE_HANDLE_TYPE_MASK));
}

inline Module* GetModule(CORINFO_MODULE_HANDLE scope) 
{
    WRAPPER_CONTRACT;

    if (IsDynamicScope(scope))
    {
        return GetDynamicResolver(scope)->GetDynamicMethod()->GetModule();
    }
    else
    {
        return((Module*)scope);
    }
}

inline CORINFO_MODULE_HANDLE GetScopeHandle(Module* module) 
{
    LEAF_CONTRACT;
    return(CORINFO_MODULE_HANDLE(module));
}

inline bool IsDynamicMethodHandle(CORINFO_METHOD_HANDLE method)
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(NULL != GetMethod(method));
    return GetMethod(method)->IsDynamicMethod();
}

#endif // DACCESS_COMPILE

#endif // _DYNAMICMETHOD_H_

