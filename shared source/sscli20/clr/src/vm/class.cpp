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
// File: CLASS.CPP
//

#include "common.h"

#include "clsload.hpp"
#include "method.hpp"
#include "class.h"
#include "class.inl"
#include "object.h"
#include "field.h"
#include "util.hpp"
#include "excep.h"
#include "siginfo.hpp"
#include "threads.h"
#include "stublink.h"
#include "ecall.h"
#include "dllimport.h"
#include "gcdesc.h"
#include "verifier.hpp"
#include "jitinterface.h"
#include "eeconfig.h"
#include "log.h"
#include "fieldmarshaler.h"
#include "cgensys.h"
#include "gc.h"
#include "security.h"
#include "comstringbuffer.h"
#include "dbginterface.h"
#include "comdelegate.h"
#include "sigformat.h"
#include "remoting.h"
#include "eeprofinterfaces.h"
#include "dllimportcallback.h"
#include "listlock.h"
#include "methodimpl.h"
#include "guidfromname.h"
#include "stackprobe.h"
#include "encee.h"
#include "encee.h"
#include "comsynchronizable.h"
#include "customattribute.h"
#include "virtualcallstub.h"
#include "eeconfig.h"
#include "contractimpl.h"
#include "prettyprintsig.h"
#include "objectclone.h"
#include "mdaassistantsptr.h"



#include "listlock.inl"
#include "generics.h"
#include "genericdict.h"
#include "instmethhash.h"
#include "typeparse.h"
#include "typestring.h"
#include "typedesc.h"
#include "ecmakey.h"
#include "constrainedexecutionregion.h"

#include "security.inl"

#ifndef DACCESS_COMPILE 

//*******************************************************************************
// Helper functions to sort GCdescs by offset (decending order)
int __cdecl compareCGCDescSeries(const void *arg1, const void *arg2)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    CGCDescSeries* gcInfo1 = (CGCDescSeries*) arg1;
    CGCDescSeries* gcInfo2 = (CGCDescSeries*) arg2;

    return (int)(gcInfo2->GetSeriesOffset() - gcInfo1->GetSeriesOffset());
}


#define RVA_FIELD_VALIDATION_ENABLED

#define UNPLACED_NONVTABLE_SLOT_NUMBER ((WORD) -2)

#include "assembly.hpp"

char* FormatSig(MethodDesc* pMD, BaseDomain *pDomain, AllocMemTracker *pamTracker);


//
// The MethodNameHash is a temporary loader structure which may be allocated if there are a large number of
// methods in a class, to quickly get from a method name to a MethodDesc (potentially a chain of MethodDescs).
//

//*******************************************************************************
// Returns TRUE for success, FALSE for failure
void MethodNameHash::Init(DWORD dwMaxEntries, StackingAllocator *pAllocator)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
    }
    CONTRACTL_END;

    // Given dwMaxEntries, determine a good value for the number of hash buckets
    m_dwNumBuckets = (dwMaxEntries / 10);

    if (m_dwNumBuckets < 5)
        m_dwNumBuckets = 5;

    unsigned cbMemory = 0;
    unsigned cbEntries = 0;
    if (!ClrSafeInt<unsigned>::multiply(m_dwNumBuckets, sizeof(MethodHashEntry*), cbMemory) ||
        !ClrSafeInt<unsigned>::multiply(dwMaxEntries, sizeof(MethodHashEntry), cbEntries) ||
        !ClrSafeInt<unsigned>::addition(cbMemory, cbEntries, cbMemory))
        ThrowHR(E_INVALIDARG);

    if (pAllocator)
    {
        m_pMemoryStart = (BYTE*)pAllocator->Alloc(cbMemory);
    }
    else
    {
    // We're given the number of hash table entries we're going to insert, so we can allocate the appropriate size
        m_pMemoryStart = new BYTE[cbMemory];
    }

    INDEBUG(m_pDebugEndMemory = m_pMemoryStart + cbMemory;)

    // Current alloc ptr
    m_pMemory       = m_pMemoryStart;

    // Allocate the buckets out of the alloc ptr
    m_pBuckets      = (MethodHashEntry**) m_pMemory;
    m_pMemory += sizeof(MethodHashEntry*)*m_dwNumBuckets;

    // Buckets all point to empty lists to begin with
    memset(m_pBuckets, 0, sizeof(MethodHashEntry*)*m_dwNumBuckets);
}

//*******************************************************************************
// Insert new entry at head of list
void MethodNameHash::Insert(LPCUTF8 pszName, MethodDesc *pDesc)
{
    LEAF_CONTRACT;
    DWORD           dwHash = HashStringA(pszName);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;
    MethodHashEntry*pNewEntry;

    pNewEntry = (MethodHashEntry *) m_pMemory;
    m_pMemory += sizeof(MethodHashEntry);

    _ASSERTE(m_pMemory <= m_pDebugEndMemory);

    // Insert at head of bucket chain
    pNewEntry->m_pNext        = m_pBuckets[dwBucket];
    pNewEntry->m_pDesc        = pDesc;
    pNewEntry->m_dwHashValue  = dwHash;
    pNewEntry->m_pKey         = pszName;

    m_pBuckets[dwBucket] = pNewEntry;
}

//*******************************************************************************
// Return the first MethodHashEntry with this name, or NULL if there is no such entry
MethodHashEntry *MethodNameHash::Lookup(LPCUTF8 pszName, DWORD dwHash)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;


    if (!dwHash)
        dwHash = HashStringA(pszName);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;
    MethodHashEntry*pSearch;

    for (pSearch = m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->m_pNext)
    {
        if (pSearch->m_dwHashValue == dwHash && !strcmp(pSearch->m_pKey, pszName))
            return pSearch;
    }

    return NULL;
}

#define MAX(a,b)    (((a)>(b))?(a):(b))

#ifdef _DEBUG
unsigned g_dupMethods = 0;
unsigned g_numMethods = 0;
#endif // _DEBUG

// Define this to cause all vtable and field information to be dumped to the screen
//#define FULL_DEBUG

//*******************************************************************************
EEClass::EEClass(Module *pModule, DWORD genericsFlags)
{
    LEAF_CONTRACT;

    m_VMFlags        = 0;
    m_pModule        = pModule;
    _ASSERTE(pModule != NULL);
    m_pMethodTable   = NULL;

    // Set the generics flags early so we can always determine whether this is an EEClass for an
    // instantiated type, e.g. List<int> or List<object>.  This helps
    // us sanity check things during class creation.
    //
    m_VMFlags |= genericsFlags;

    // Set the interface ID to -1 to indicate it hasn't been set yet.
    m_cbModuleDynamicID     = (DWORD) -1;



#ifdef _DEBUG
    m_szDebugClassName = NULL;
    m_fDebuggingClass = FALSE;
#endif // _DEBUG

#if CHECK_APP_DOMAIN_LEAKS
    m_wAuxFlags = 0;
#endif

}

//*******************************************************************************
void *EEClass::operator new(size_t size, size_t extraSize, LoaderHeap *pHeap, Module *pModule, size_t *dwSizeRequestedForAlloc, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    if(extraSize!=0)
    {
       if (size+extraSize<size)
            ThrowHR(COR_E_OVERFLOW);
        size+=extraSize;
    }

#ifdef _DEBUG
        pModule->GetClassLoader()->m_dwEEClassData += size;
#endif
    // Make sure that the EEClass is ptr size aligned.
    // This ensures that the buckets that follow are also aligned.
    _ASSERTE(size == ALIGN_UP(size, sizeof(UINT_PTR)));
    size = ALIGN_UP(size, sizeof(UINT_PTR));

    void *pTmp;
    *dwSizeRequestedForAlloc = size; // Must give caller our alloc size so he can call BackoutMem
    pTmp = pamTracker->Track(pHeap->AllocMem_NoThrow(size));
    if (!pTmp)
    {
        COMPlusThrowOM();
    }

    // No need to memset since this memory came from VirtualAlloc'ed memory
    // memset (pTmp, 0, size);

    return pTmp;
}

//*******************************************************************************
void EEClass::Destruct()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        FORBID_FAULT;
    }
    CONTRACTL_END


    // If we haven't been restored, we can ignore the class
    if (GetMethodTable() && !GetMethodTable()->IsRestored())
        return;

    SetDestroyed();


#ifdef PROFILING_SUPPORTED
    // If profiling, then notify the class is getting unloaded.
    TypeID clsId = NULL;
    if (CORProfilerTrackClasses() && !IsArrayClass() && GetMethodTable())
    {
        //
        //

        FAULT_NOT_FATAL();

        EX_TRY
        {
            PROFILER_CALL;
       
            g_profControlBlock.pProfInterface->ClassUnloadStarted(
                (ThreadID) GetThread(),
                clsId = (TypeID) TypeHandle(this->GetMethodTable()).AsPtr());


        }
        EX_CATCH
        {
            // The exception here came from the profiler itself. We'll just
            // swallow the exception, since we don't want the profiler to bring
            // down the runtime.
        }
        EX_END_CATCH(RethrowTerminalExceptions);
    }
#endif // PROFILING_SUPPORTED



    if (IsAnyDelegateClass())
    {
        DelegateEEClass* pDelegateEEClass = (DelegateEEClass*)this;

        if (pDelegateEEClass->m_pSecurityStub)
        {
            Stub* pSecurityStub = pDelegateEEClass->m_pSecurityStub;
            pSecurityStub->DecRef();

            Stub* pInterceptedStub = *(((InterceptStub*)pSecurityStub)->GetInterceptedStub());
            if (pInterceptedStub == pDelegateEEClass->m_pUMCallStub)
            {
                pDelegateEEClass->m_pUMCallStub = NULL;
            }
            else if (pInterceptedStub == pDelegateEEClass->m_pMLStub)
            {
                pDelegateEEClass->m_pMLStub = NULL;
            }
        }
        if (pDelegateEEClass->m_pStaticCallStub)
        {
            BOOL fStubDeleted = pDelegateEEClass->m_pStaticCallStub->DecRef();
            if (fStubDeleted)
            {
                DelegateInvokeStubManager::g_pManager->RemoveStub(pDelegateEEClass->m_pStaticCallStub);
            }
        }
        if (pDelegateEEClass->m_pUMCallStub)
        {
            pDelegateEEClass->m_pUMCallStub->DecRef();
        }
        if (pDelegateEEClass->m_pInstRetBuffCallStub)
        {
            pDelegateEEClass->m_pInstRetBuffCallStub->DecRef();
        }
        if (pDelegateEEClass->m_pMLStub)
        {
            pDelegateEEClass->m_pMLStub->DecRef();
        }
        // While m_pMultiCastInvokeStub is also a member,
        // it is owned by the m_pMulticastStubCache, not by the class
        // - it is shared across classes. So we don't decrement
        // its ref count here
        delete pDelegateEEClass->m_pUMThunkMarshInfo;
    }

    if (GetMethodTable() && GetMethodTable()->IsThunking())
    {
        GetMethodTable()->MarkAsNotThunking();
    }

    // Destruct the method descs by walking the chunks.
    DWORD i, n;
    MethodDescChunk *pChunk = GetChunks();


    // If we haven't created a methodtable yet, we better not have created any chunks!
    _ASSERTE(GetMethodTable() || !pChunk);

    while (pChunk != NULL)
    {
        n = pChunk->GetCount();
        for (i = 0; i < n; i++)
        {
            MethodDesc *pMD = pChunk->GetMethodDescAt(i);
            pMD->Destruct();
        }
        pChunk = pChunk->GetNextChunk();
    }


#ifdef PROFILING_SUPPORTED
    // If profiling, then notify the class is getting unloaded.
    if (CORProfilerTrackClasses() &&
        !IsArrayClass() &&
        // If there was an exception while the type was being loaded,
        // clsId may not be set. No need to do the callback in that case.
        clsId)
    {
        // See comments in the call to ClassUnloadStarted for details on this
        // FAULT_NOT_FATAL marker and exception swallowing.
        FAULT_NOT_FATAL();
        EX_TRY
        {
            PROFILER_CALL;

            g_profControlBlock.pProfInterface->ClassUnloadFinished((ThreadID) GetThread(), clsId, S_OK);

        }
        EX_CATCH
        {
        }
        EX_END_CATCH(RethrowTerminalExceptions);
    }
#endif // PROFILING_SUPPORTED
}

//*******************************************************************************
EEClassLayoutInfo *EEClass::GetLayoutInfo()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;
    STATIC_CONTRACT_SO_TOLERANT;

    _ASSERTE(HasLayout());
    g_IBCLogger.LogEEClassAndMethodTableAccess(this);
    return &((LayoutEEClass *) this)->m_LayoutInfo;
}


void MethodTableBuilder::CreateMinimalClass(LoaderHeap *pHeap,
									Module* pModule,
									AllocMemTracker *pamTracker,
									SIZE_T cbExtra,
									EEClass** ppEEClass)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(ppEEClass!=NULL);
    }
    CONTRACTL_END;
    size_t cbSize;
    *ppEEClass = new (cbExtra,pHeap, pModule, &cbSize, pamTracker) EEClass(pModule, 0);
}
//==========================================================================
// This function is very specific about how it constructs a EEClass.  It first
// determines the necessary size of the vtable and the number of statics that
// this class requires.  The necessary memory is then allocated for a EEClass
// and its vtable and statics.  The class members are then initialized and
// the memory is then returned to the caller
//
// LPEEClass CreateClass()
//
// Parameters :
//      [in] scope - scope of the current class not the one requested to be opened
//      [in] cl - class token of the class to be created.
//      [out] ppEEClass - pointer to pointer to hold the address of the EEClass
//                        allocated in this function.
// Return : returns an HRESULT indicating the success of this function.
//
// This parameter has been removed but might need to be reinstated if the
// global for the metadata loader is removed.
//      [in] pIMLoad - MetaDataLoader class/object for the current scope.


//==========================================================================
/*static*/
void MethodTableBuilder::CreateClass(BaseDomain * pDomain,
                                     Module *pModule,
                                     mdTypeDef cl,
                                     BOOL fHasLayout,
                                     BOOL fDelegate,
                                     BOOL fIsEnum,
                                     const MethodTableBuilder::bmtGenericsInfo *bmtGenericsInfo,
                                     EEClass **ppEEClass,
                                     size_t *pdwAllocRequestSize,
                                     AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(!(fHasLayout && fDelegate));
        PRECONDITION(!(fHasLayout && fIsEnum));
        PRECONDITION(ppEEClass!=NULL);
        PRECONDITION(pdwAllocRequestSize!=NULL);
        PRECONDITION(CheckPointer(bmtGenericsInfo));
    }
    CONTRACTL_END;

    EEClass *pEEClass = NULL;
    IMDInternalImport *pInternalImport;
    HRESULT hrToThrow;


    pEEClass = NULL;






    if (fHasLayout)
    {
        pEEClass = new (0,pDomain->GetLowFrequencyHeap(), pModule, pdwAllocRequestSize, pamTracker) LayoutEEClass(pModule, bmtGenericsInfo->genericsKind);
    }
    else if (fDelegate)
    {
        pEEClass = new (0,pDomain->GetLowFrequencyHeap(), pModule, pdwAllocRequestSize, pamTracker) DelegateEEClass(pModule, bmtGenericsInfo->genericsKind);
    }
    else if (fIsEnum)
    {
        pEEClass = new (0,pDomain->GetLowFrequencyHeap(), pModule, pdwAllocRequestSize, pamTracker) EnumEEClass(pModule, bmtGenericsInfo->genericsKind);
    }
    else
    {
        pEEClass = new (0,pDomain->GetLowFrequencyHeap(), pModule, pdwAllocRequestSize, pamTracker) EEClass(pModule, bmtGenericsInfo->genericsKind);
    }

    if (!pEEClass)
    {
        COMPlusThrowOM();
    }

    // Caller will clean up if we throw below here.
    *ppEEClass = pEEClass;

    DWORD dwAttrClass = 0;
    mdToken tkExtends = mdTokenNil;

    pEEClass->m_cl = cl;

    // Set up variance info
    if (bmtGenericsInfo->pVarianceInfo)
    {
        pEEClass->m_pVarianceInfo = (BYTE*) pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocMem(bmtGenericsInfo->GetNumGenericArgs()));
        memcpy(pEEClass->m_pVarianceInfo, bmtGenericsInfo->pVarianceInfo, bmtGenericsInfo->GetNumGenericArgs());
    }
    else
        pEEClass->m_pVarianceInfo = NULL;

    pInternalImport = pModule->GetMDImport();

    if (pInternalImport == NULL)
        COMPlusThrowHR(COR_E_TYPELOAD);

    pInternalImport->GetTypeDefProps(
        cl,
        &dwAttrClass,
        &tkExtends
    );

    pEEClass->m_dwAttrClass = dwAttrClass;

    // MDVal check: can't be both tdSequentialLayout and tdExplicitLayout
    if((dwAttrClass & tdLayoutMask) == tdLayoutMask)
        COMPlusThrowHR(COR_E_TYPELOAD);

    if (IsTdInterface(dwAttrClass))
    {
        // MDVal check: must have nil tkExtends and must be tdAbstract
        if((tkExtends & 0x00FFFFFF)||(!IsTdAbstract(dwAttrClass)))
            COMPlusThrowHR(COR_E_TYPELOAD);
    }

    //
    // Initialize SecurityProperties structure
    //

    if (Security::IsSecurityOn() && IsTdHasSecurity(dwAttrClass))
    {
        DWORD dwSecFlags;
        DWORD dwNullDeclFlags;

        hrToThrow = Security::GetDeclarationFlags(pInternalImport, cl, &dwSecFlags, &dwNullDeclFlags);
        if (FAILED(hrToThrow))
            COMPlusThrowHR(hrToThrow);

        pEEClass->m_SecProps.SetFlags(dwSecFlags, dwNullDeclFlags);
    }

    // Cache class level reliability contract info.
    pEEClass->SetReliabilityContract(::GetReliabilityContract(pInternalImport, cl));

    if (fHasLayout)
        pEEClass->SetHasLayout();

#ifdef _DEBUG
    pModule->GetClassLoader()->m_dwDebugClasses++;
#endif
}

//*******************************************************************************
//
// Create a hash of all methods in this class.  The hash is from method name to MethodDesc.
//
MethodNameHash *MethodTableBuilder::CreateMethodChainHash(MethodTable *pMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    Thread *pThread = GetThread();
    PVOID pvMem = pThread->m_MarshalAlloc.Alloc(sizeof(MethodNameHash));
    MethodNameHash *pHash = new (pvMem) MethodNameHash();

    pHash->Init(pMT->GetNumVirtuals(), &(pThread->m_MarshalAlloc));

    MethodTable::MethodIterator it(pMT);
    for (;it.IsValid(); it.Next())
    {
        if (it.IsVirtual())
            {
            MethodDesc *pImplDesc = it.GetMethodDesc();
            CONSISTENCY_CHECK(CheckPointer(pImplDesc));
            MethodDesc *pDeclDesc = it.GetDeclMethodDesc();
            CONSISTENCY_CHECK(CheckPointer(pDeclDesc));

            CONSISTENCY_CHECK(pMT->IsInterface() || !pDeclDesc->IsInterface());
            pHash->Insert(pDeclDesc->GetNameOnNonArrayClass(), pDeclDesc);
        }
    }

    // Success
    return pHash;
}

//*******************************************************************************

//-----------------------------------------------------------------------------------
// Note: this only loads the type to CLASS_DEPENDENCIES_LOADED as this can be called
// indirectly from DoFullyLoad() as part of accessibility checking.
//-----------------------------------------------------------------------------------
MethodTable *MethodTable::LoadEnclosingMethodTable()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END

    if (! GetClass()->IsNested())
        return NULL;

    mdTypeDef tdEnclosing = mdTypeDefNil;

    HRESULT hr;
    hr = GetMDImport()->GetNestedClassProps(GetCl(), &tdEnclosing);
    if (FAILED(hr))
        ThrowHR(hr, BFA_UNABLE_TO_GET_NESTED_PROPS);

    return ClassLoader::LoadTypeDefThrowing(GetModule(),
                                            tdEnclosing,
                                            ClassLoader::ThrowIfNotFound,
                                            ClassLoader::PermitUninstDefOrRef,
                                            tdNoTypes,
                                            CLASS_DEPENDENCIES_LOADED
                                            ).GetMethodTable();

}


//*******************************************************************************
//
// Find a method in this class hierarchy - used ONLY by the loader during layout.  Do not use at runtime.
//
// *ppMemberSignature must be NULL on entry - it and *pcMemberSignature may or may not be filled out
//
// ppMethodDesc will be filled out with NULL if no matching method in the hierarchy is found.
//
// Returns FALSE if there was an error of some kind.
//
// pMethodConstraintsMatch receives the result of comparing the method constraints.
HRESULT MethodTableBuilder::LoaderFindMethodInClass(
    LPCUTF8             pszMemberName,
    Module*             pModule,
    mdMethodDef         mdToken,
    MethodDesc **       ppMethodDesc,
    PCCOR_SIGNATURE *   ppMemberSignature,
    DWORD *             pcMemberSignature,
    DWORD               dwHashName,
    BOOL *              pMethodConstraintsMatch
)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtParent));
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(ppMethodDesc));
        PRECONDITION(CheckPointer(ppMemberSignature));
        PRECONDITION(CheckPointer(pcMemberSignature));
    }
    CONTRACTL_END;

    MethodHashEntry *pEntry;
    DWORD            dwNameHashValue;

    _ASSERTE(pModule);
    _ASSERTE(*ppMemberSignature == NULL);

    // No method found yet
    *ppMethodDesc = NULL;

    // Have we created a hash of all the methods in the class chain?
    if (bmtParent->pParentMethodHash == NULL)
    {
        // There may be such a method, so we will now create a hash table to reduce the pain for
        // further lookups

        bmtParent->pParentMethodHash = CreateMethodChainHash(bmtParent->pParentMethodTable);
    }

    // We have a hash table, so use it
    pEntry = bmtParent->pParentMethodHash->Lookup(pszMemberName, dwHashName);
    if (pEntry == NULL)
        return S_OK; // No method by this name exists in the hierarchy

    // Get signature of the method we're searching for - we will need this to verify an exact name-signature match
    *ppMemberSignature = pModule->GetMDImport()->GetSigOfMethodDef(
        mdToken,
        pcMemberSignature
    );

    // Hash value we are looking for in the chain
    dwNameHashValue = pEntry->m_dwHashValue;

    // We've found a method with the same name, but the signature may be different
    // Traverse the chain of all methods with this name
    while (1)
    {
        PCCOR_SIGNATURE pHashMethodSig;
        DWORD       cHashMethodSig;
        Substitution* pSubst = NULL;
        MethodTable *entryMT = pEntry->m_pDesc->GetMethodTable();
        EEClass *entryEEClass = entryMT->GetClass();

        if (entryEEClass->GetNumGenericArgs() > 0)
        {
          EEClass *here = GetHalfBakedClass();
          _ASSERTE(here->GetModule());
          MethodTable *pParent = bmtParent->pParentMethodTable;

          do
          {
              Substitution *newSubst = new Substitution;
              *newSubst = here->GetSubstitutionForParent(pSubst);
              pSubst = newSubst;
              here = pParent->GetClass();
              _ASSERT(here != NULL);
              pParent = pParent->GetParentMethodTable();
          }
          while (entryEEClass != here);
        }

        // Get sig of entry in hash chain
        pEntry->m_pDesc->GetSig(&pHashMethodSig, &cHashMethodSig);

        // Note instantiation info
        {
            HRESULT hr = MetaSig::CompareMethodSigsNT(*ppMemberSignature, *pcMemberSignature, pModule, NULL,
                                                      pHashMethodSig, cHashMethodSig, pEntry->m_pDesc->GetModule(), pSubst);
            if (FAILED(hr))
            {
                if(pSubst) pSubst->DeleteChain();
                return hr;
            }
            if (hr == S_OK)
            {
                // Found a match
                *ppMethodDesc = pEntry->m_pDesc;
                // Check the constraints are consistent,
                // and return the result to the caller.
                // We do this here to avoid recalculating pSubst.
                *pMethodConstraintsMatch =
                MetaSig::CompareMethodConstraints(pModule, mdToken,
                                                  pSubst, pEntry->m_pDesc->GetModule(), pEntry->m_pDesc->GetMemberDef());
                if(pSubst) pSubst->DeleteChain();
                return S_OK;
            }


        }
        if(pSubst) pSubst->DeleteChain();
        // Advance to next item in the hash chain which has the same name
        do
        {
            pEntry = pEntry->m_pNext; // Next entry in the hash chain

            if (pEntry == NULL)
                return S_OK; // End of hash chain, no match found
        } while ((pEntry->m_dwHashValue != dwNameHashValue) || (strcmp(pEntry->m_pKey, pszMemberName) != 0));
    }

    return S_OK;
}

//*******************************************************************************
// Enumerator to traverse the interface declarations of a type, automatically building
// a substitution chain on the stack.
class InterfaceImplEnum
{
    Module* m_pModule;
    HENUMInternalHolder   hEnumInterfaceImpl;
    const Substitution *m_pSubstChain;
    Substitution m_CurrSubst;
    mdTypeDef m_CurrTok;
public:
    InterfaceImplEnum(Module *pModule, mdTypeDef cl, const Substitution *pSubstChain)
        : hEnumInterfaceImpl(pModule->GetMDImport())
    {
        WRAPPER_CONTRACT;
        m_pModule = pModule;
        hEnumInterfaceImpl.EnumInit(mdtInterfaceImpl, cl);
        m_pSubstChain = pSubstChain;
    }

    BOOL Next()
    {
        WRAPPER_CONTRACT;
        mdInterfaceImpl ii;
        if (!m_pModule->GetMDImport()->EnumNext(&hEnumInterfaceImpl, &ii))
            return FALSE;

        m_CurrTok = m_pModule->GetMDImport()->GetTypeOfInterfaceImpl(ii);
        m_CurrSubst = Substitution(m_CurrTok, m_pModule, m_pSubstChain);
        return TRUE;
    }
    const Substitution *CurrentSubst() const { LEAF_CONTRACT; return &m_CurrSubst; }
    mdTypeDef CurrentToken() const { LEAF_CONTRACT; return m_CurrTok; }
};

//*******************************************************************************
//
// Given an interface map to fill out, expand pNewInterface (and its sub-interfaces) into it, increasing
// pdwInterfaceListSize as appropriate, and avoiding duplicates.
//
/* static */
void MethodTableBuilder::ExpandApproxInterface(bmtInterfaceInfo *bmtInterface,
                              const Substitution *pNewInterfaceSubstChain,
                              MethodTable *pNewInterface,
                              WORD flags)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    // We expand the tree of inherited interfaces into a set by adding the
    // current node BEFORE expanding the parents of the current node.
    // ****** This must be consistent with ExpandExactInterface *******
    // ****** This must be consistent with BuildInteropVTable_ExpandApproxInterface *******

    // The interface list contains the fully expanded set of interfaces from the parent then
    // we start adding all the interfaces we declare. We need to know which interfaces
    // we declare but do not need duplicates of the ones we declare. This means we can
    // duplicate our parent entries.

    // Is it already present in the list?
    for (DWORD i = 0; i < bmtInterface->dwInterfaceMapSize; i++)
    {
        if (MetaSig::CompareTypeDefsUnderSubstitutions(bmtInterface->pInterfaceMap[i].m_pMethodTable,
                                                       pNewInterface,
                                                       bmtInterface->ppInterfaceSubstitutionChains[i],
                                                       pNewInterfaceSubstChain))
        {
            bmtInterface->pInterfaceMap[i].m_wFlags |= flags;
            return; // found it, don't add it again
        }
    }

    if (pNewInterface->GetNumVirtuals() > bmtInterface->dwLargestInterfaceSize)
        bmtInterface->dwLargestInterfaceSize = pNewInterface->GetNumVirtuals();

    DWORD n = bmtInterface->dwInterfaceMapSize;
    // Add it and each sub-interface
    // Save a copy of ths substitution chain for use later in loading and for subsequent comparisons
    bmtInterface->pInterfaceMap[n].m_pMethodTable = pNewInterface;
    bmtInterface->pInterfaceMap[n].SetInteropStartSlot(MethodTable::NO_SLOT);
    bmtInterface->pInterfaceMap[n].m_wFlags = flags;

    bmtInterface->ppInterfaceSubstitutionChains[n] = (Substitution *) GetThread()->m_MarshalAlloc.Alloc(sizeof(Substitution) * pNewInterfaceSubstChain->GetLength());
    pNewInterfaceSubstChain->CopyToArray(bmtInterface->ppInterfaceSubstitutionChains[n]);
    bmtInterface->dwInterfaceMapSize++;

    ExpandApproxDeclaredInterfaces(bmtInterface,
                            pNewInterface->GetModule(),
                            pNewInterface->GetCl(),
                            pNewInterfaceSubstChain,
                            flags & ~InterfaceInfo_t::interface_declared_on_class);

}

//*******************************************************************************
/* static */
void MethodTableBuilder::ExpandApproxDeclaredInterfaces(bmtInterfaceInfo *bmtInterface,
                                       Module *pModule,
                                       mdToken typeDef,
                                       const Substitution *pSubstChain,
                                       WORD flags)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    InterfaceImplEnum ie(pModule, typeDef, pSubstChain);
    while (ie.Next())
    {
        MethodTable *pGenericIntf = ClassLoader::LoadApproxTypeThrowing(pModule, ie.CurrentToken(), NULL, NULL).GetMethodTable();
        CONSISTENCY_CHECK(pGenericIntf->IsInterface());

        ExpandApproxInterface(bmtInterface, ie.CurrentSubst(), pGenericIntf, flags);
    }

}

//*******************************************************************************
/* static */
void MethodTableBuilder::ExpandApproxInherited(bmtInterfaceInfo *bmtInterface,
                                               MethodTable *pApproxMT,
                                               const Substitution *pSubstChain)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    // Expand interfaces in superclasses first.  Interfaces inherited from parents
    // must have identical indexes as in the parent.
    MethodTable *pParentOfParent = pApproxMT->GetParentMethodTable();

    if (pParentOfParent)
    {
        Substitution parentSubst = pApproxMT->GetClass()->GetSubstitutionForParent(pSubstChain);
        ExpandApproxInherited(bmtInterface,pParentOfParent,&parentSubst);
    }

    ExpandApproxDeclaredInterfaces(bmtInterface,
                                   pApproxMT->GetModule(),pApproxMT->GetCl(),
                                   pSubstChain,InterfaceInfo_t::interface_implemented_on_parent);

}

//*******************************************************************************
//
// Fill out a fully expanded interface map, such that if we are declared to implement I3, and I3 extends I1,I2,
// then I1,I2 are added to our list if they are not already present.
//
void MethodTableBuilder::LoadApproxInterfaceMap(BuildingInterfaceInfo_t *pBuildingInterfaceList,
                                                MethodTable *pApproxParentMT)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    bmtInterface->dwInterfaceMapSize = 0;

    // First inherit all the parent's interfaces.  This is important, because our interface map must
    // list the interfaces in identical order to our parent.
    //
    // <NICE> we should document the reasons why.  One reason is that DispatchMapTypeIDs can be indexes
    // into the list </NICE>
    if (pApproxParentMT)
    {
        Substitution parentSubst = GetHalfBakedClass()->GetSubstitutionForParent(NULL);

        MethodTableBuilder::ExpandApproxInherited(bmtInterface,
                                                  pApproxParentMT,
                                                  &parentSubst);
    }

    // Now add in any freshly declared interfaces, possibly augmenting the flags
    MethodTableBuilder::ExpandApproxDeclaredInterfaces(bmtInterface,
                                                       GetModule(), GetCl(),
                                                       NULL,
                                                       InterfaceInfo_t::interface_declared_on_class);

}


//*******************************************************************************
DispatchMapTypeID MethodTableBuilder::ComputeDispatchMapTypeID(MethodTable *pDeclInftMT, const Substitution *pDeclIntfSubst)
{
    WRAPPER_CONTRACT;
    if (!pDeclInftMT->IsInterface())
    {
        return DispatchMapTypeID::ThisClassID();
    }
    else
    {
        for (DWORD idx = 0; idx < bmtInterface->dwInterfaceMapSize; idx++)
        {
            if (MetaSig::CompareTypeDefsUnderSubstitutions(bmtInterface->pInterfaceMap[idx].m_pMethodTable,
                                                           pDeclInftMT,
                                                           bmtInterface->ppInterfaceSubstitutionChains[idx],
                                                           pDeclIntfSubst)) {
                return DispatchMapTypeID::InterfaceClassID(idx);
            }
        }
        return DispatchMapTypeID::InterfaceNotImplementedID();
    }

}

//*******************************************************************************
DispatchMapTypeID MethodTable::ComputeDispatchMapTypeID(MethodTable *pExactIntfMT, MethodTable *pExactImplMT)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pExactIntfMT));
        PRECONDITION(CheckPointer(pExactImplMT));
    } CONTRACTL_END;

    if (!pExactIntfMT->IsInterface())
    {
        return DispatchMapTypeID::ThisClassID();
    }
    else
    {
        InterfaceMapIterator it = pExactImplMT->IterateInterfaceMap();
        while (it.Next())
        {
            if (g_pConfig->ExactInterfaceCalls())
            {
                if (it.InterfaceEquals(pExactIntfMT))
                {
                    return DispatchMapTypeID::InterfaceClassID(it.GetIndex());
                }
            }
            else
            {
                if (it.GetInterface()->GetCanonicalMethodTable() == pExactIntfMT->GetCanonicalMethodTable())
                {
                    return DispatchMapTypeID::InterfaceClassID(it.GetIndex());
                }
            }
        }
        return DispatchMapTypeID::InterfaceNotImplementedID();
    }
}


//*******************************************************************************
/*static*/
VOID DECLSPEC_NORETURN MethodTableBuilder::BuildMethodTableThrowException(HRESULT hr,
                                             const bmtErrorInfo & bmtError)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    LPCUTF8 pszClassName, pszNameSpace;
    bmtError.pModule->GetMDImport()->GetNameOfTypeDef(bmtError.cl, &pszClassName, &pszNameSpace);

    if ((! bmtError.dMethodDefInError || bmtError.dMethodDefInError == mdMethodDefNil) &&
        bmtError.szMethodNameForError == NULL) {
        if (hr == E_OUTOFMEMORY)
            COMPlusThrowOM();
        else
            bmtError.pModule->GetAssembly()->ThrowTypeLoadException(pszNameSpace, pszClassName,
                                                                    bmtError.resIDWhy);
    }
    else {
        LPCUTF8 szMethodName;
        if(bmtError.szMethodNameForError == NULL)
            szMethodName = (bmtError.pModule->GetMDImport())->GetNameOfMethodDef(bmtError.dMethodDefInError);
        else
            szMethodName = bmtError.szMethodNameForError;

        bmtError.pModule->GetAssembly()->ThrowTypeLoadException(pszNameSpace, pszClassName,
                                                                szMethodName, bmtError.resIDWhy);
    }

}

//*******************************************************************************
void MethodTableBuilder::SetBMTData(
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
    bmtThreadContextStaticInfo *bmtTCSInfo)
{
    LEAF_CONTRACT;
    this->bmtDomain = bmtDomain;
    this->bmtError = bmtError;
    this->bmtProp = bmtProp;
    this->bmtVT = bmtVT;
    this->bmtParent = bmtParent;
    this->bmtInterface = bmtInterface;
    this->bmtMetaData = bmtMetaData;
    this->bmtMFDescs = bmtMFDescs;
    this->bmtFP = bmtFP;
    this->bmtInternal = bmtInternal;
    this->bmtGCSeries = bmtGCSeries;
    this->bmtMethodImpl = bmtMethodImpl;
    this->bmtGenerics = bmtGenerics;
    this->bmtEnumMF = bmtEnumMF;
    this->bmtTCSInfo = bmtTCSInfo;
}

//*******************************************************************************
void MethodTableBuilder::NullBMTData()
{
    LEAF_CONTRACT;
    this->bmtDomain = NULL;
    this->bmtError = NULL;
    this->bmtProp = NULL;
    this->bmtVT = NULL;
    this->bmtParent = NULL;
    this->bmtInterface = NULL;
    this->bmtMetaData = NULL;
    this->bmtMFDescs = NULL;
    this->bmtFP = NULL;
    this->bmtInternal = NULL;
    this->bmtGCSeries = NULL;
    this->bmtMethodImpl = NULL;
    this->bmtGenerics = NULL;
    this->bmtEnumMF = NULL;
    this->bmtTCSInfo = NULL;
}

//*******************************************************************************
//
// Builds the method table, allocates MethodDesc, handles overloaded members, attempts to compress
// interface storage.  All dependent classes must already be resolved!
//
VOID MethodTableBuilder::BuildMethodTableThrowing(BaseDomain *bmtDomain,
                                                  Module *pLoaderModule,
                                                  Module *pModule,
                                                  mdToken cl,
                                                  BuildingInterfaceInfo_t *pBuildingInterfaceList,
                                                  const LayoutRawFieldInfo *pLayoutRawFieldInfos,
                                                  MethodTable *pParentMethodTable,
                                                  const bmtGenericsInfo *bmtGenericsInfo,
                                                  PCCOR_SIGNATURE parentInst,
                                                  WORD wNumInterfaces,
                                                  AllocMemTracker *pamTracker)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(GetHalfBakedClass()));
        PRECONDITION(CheckPointer(bmtGenericsInfo));
    }
    CONTRACTL_END;

    // pThread is only used in certain scenarios.  Avoid Rotor unused var error
    Thread *pThread = GetThread();

    pModule->EnsureLibraryLoaded();

    // The following structs, defined as private members of MethodTableBuilder, contain the necessary local
    // parameters needed for BuildMethodTable

    // Look at the struct definitions for a detailed list of all parameters available
    // to BuildMethodTable.

    bmtErrorInfo bmtError;
    bmtProperties bmtProp;
    bmtVtable bmtVT;
    bmtParentInfo bmtParent;
    bmtInterfaceInfo bmtInterface;
    bmtMetaDataInfo bmtMetaData;
    bmtMethAndFieldDescs bmtMFDescs;
    bmtFieldPlacement bmtFP;
    bmtInternalInfo bmtInternal;
    bmtGCSeriesInfo bmtGCSeries;
    bmtMethodImplInfo bmtMethodImpl;
    bmtThreadContextStaticInfo bmtTCSInfo;
    this->bmtGenerics = bmtGenericsInfo;

    //Initialize structs

    bmtProp.fMarshaledByRef = false;

    bmtError.resIDWhy = IDS_CLASSLOAD_GENERAL;          // Set the reason and the offending method def. If the method information
    bmtError.pThrowable = NULL;
    bmtError.pModule  = pModule;
    bmtError.cl       = GetCl();

    bmtInterface.dwInterfaceMapSize = wNumInterfaces;

    bmtInternal.pInternalImport = pModule->GetMDImport();
    bmtInternal.pModule = pModule;
    bmtInternal.cl = GetCl();

    bmtEnumMethAndFields bmtEnumMF(bmtInternal.pInternalImport);

    bmtParent.parentSubst = Substitution(pModule,parentInst,NULL);
    DWORD dwAttrClass;
    bmtInternal.pInternalImport->GetTypeDefProps(
        bmtInternal.cl,
        &dwAttrClass,
        &(bmtParent.token)
    );

    SetBMTData(
        bmtDomain,
        &bmtError,
        &bmtProp,
        &bmtVT,
        &bmtParent,
        &bmtInterface,
        &bmtMetaData,
        &bmtMFDescs,
        &bmtFP,
        &bmtInternal,
        &bmtGCSeries,
        &bmtMethodImpl,
        bmtGenericsInfo,
        &bmtEnumMF,
        &bmtTCSInfo);

    // put the interior stack probe after all the stack-allocted goop above.  We check compare our this pointer to the SP on
    // the dtor to determine if we are being called on an EH path or not.
    INTERIOR_STACK_PROBE_FOR(pThread, 8);

    // If not NULL, it means there are some by-value fields, and this contains an entry for each instance or static field,
    // which is NULL if not a by value field, and points to the EEClass of the field if a by value field.  Instance fields
    // come first, statics come second.
    NewArrayHolder<EEClass*>pByValueClassCache(NULL);

    // If not NULL, it means there are some by-value fields, and this contains an entry for each inst

#ifdef _DEBUG
    LPCUTF8 className;
    LPCUTF8 nameSpace;
    bmtInternal.pInternalImport->GetNameOfTypeDef(bmtInternal.cl, &className, &nameSpace);

    unsigned fileNameSize = 0;
    LPCWSTR fileName = NULL;
    // Personal choice: I do not want to see the full filename in every class name
    // (i.e. System.IO.TextWriter[E:\WINDOWS\Microsoft.NET\Framework\v1.2.AMD64dbg\mscorlib.dll] )
    if (pModule->IsPEFile()) {
        fileName = pModule->GetFile()->GetDebugName();
        if (fileName != 0)
            fileNameSize = (unsigned int) wcslen(fileName) + 2;
    }

    {
        size_t len = sizeof(char)*(strlen(className) + strlen(nameSpace) + fileNameSize + 2);
        char *name = (char*) pamTracker->Track(bmtDomain->GetHighFrequencyHeap()->AllocMem(len));
        strcpy_s(name, len, nameSpace);
        if (strlen(nameSpace) > 0) {
            name[strlen(nameSpace)] = '.';
            name[strlen(nameSpace) + 1] = '\0';
        }
        strcat_s(name, len, className);

        if (fileNameSize != 0 && g_pConfig->ShouldAppendFileNameToTypeName()) {
            char* ptr = name + strlen(name);
            *ptr++ = '[';
            while(*fileName != 0)
                *ptr++ = char(*fileName++);
            *ptr++ = ']';
            *ptr++ = 0;
        }
        GetHalfBakedClass()->SetDebugClassName(name);
    }

    if (g_pConfig->ShouldBreakOnClassBuild(className)) {
        _ASSERTE(!"BreakOnClassBuild");
        GetHalfBakedClass()->m_fDebuggingClass = TRUE;
    }
#endif // _DEBUG

    DWORD i;

#ifdef _DEBUG
    LPCUTF8 pszDebugName,pszDebugNamespace;

    pModule->GetMDImport()->GetNameOfTypeDef(bmtInternal.cl, &pszDebugName, &pszDebugNamespace);
#endif // _DEBUG


#ifdef _DEBUG
    if (bmtGenerics->HasInstantiation())
    {
        StackSString debugName(SString::Utf8, GetDebugClassName());
        TypeString::AppendInst(debugName, bmtGenerics->GetNumGenericArgs(), bmtGenerics->GetInstantiation(), TypeString::FormatBasic);
        StackScratchBuffer buff;
        const char* pDebugNameUTF8 = debugName.GetUTF8(buff);
        size_t len = strlen(pDebugNameUTF8)+1;
        char *name = (char*) pamTracker->Track(bmtDomain->GetLowFrequencyHeap()->AllocMem(len));
        strcpy_s(name, len, pDebugNameUTF8);
        GetHalfBakedClass()->SetDebugClassName(name);
    }
#endif // _DEBUG

    // If this is mscorlib, then don't perform some sanity checks on the layout
    bmtProp.fNoSanityChecks = ((g_pObjectClass != NULL) && pModule == g_pObjectClass->GetModule());

#ifdef _DEBUG
    StackSString debugName(SString::Utf8, pszDebugName);
    if (bmtGenerics->HasInstantiation())
    {
        TypeString::AppendInst(debugName, bmtGenerics->GetNumGenericArgs(), bmtGenerics->GetInstantiation(), TypeString::FormatBasic);
    }

    LOG((LF_CLASSLOADER, LL_INFO1000, "Loading class \"%s%s%S\" from module \"%ws\" in domain 0x%x %s\n",
        *pszDebugNamespace ? pszDebugNamespace : "",
        *pszDebugNamespace ? NAMESPACE_SEPARATOR_STR : "",
        debugName.GetUnicode(),
        pModule->GetDebugName(),
        pModule->GetDomain(),
        (pModule->IsSystem()) ? "System Domain" : ""
    ));
#endif // _DEBUG

    // Interfaces have a parent class of Object, but we don't really want to inherit all of
    // Object's virtual methods, so pretend we don't have a parent class - at the bottom of this
    // function we reset the parent class
    if (IsInterface())
    {
        pParentMethodTable = NULL;
    }

    unsigned totalDeclaredFieldSize=0;

    bmtParent.pParentMethodTable = pParentMethodTable;

    // Check to see if the class is an valuetype
    if(pParentMethodTable != NULL
    && ((g_pEnumClass != NULL && pParentMethodTable == g_pValueTypeClass) ||
        pParentMethodTable == g_pEnumClass))
    {
        SetValueClass();

        HRESULT hr = bmtInternal.pInternalImport->GetCustomAttributeByName(bmtInternal.cl,
                                                                g_CompilerServicesUnsafeValueTypeAttribute,
                                                                NULL, NULL);
        IfFailThrow(hr);
        if (hr == S_OK)
        {
            SetUnsafeValueClass();
        }
    }

    // Check to see if the class is an enumeration
    if(pParentMethodTable != NULL && pParentMethodTable == g_pEnumClass)
    {
        SetEnum();

        // Ensure we don't have generic enums, or at least enums that have a 
        // different number of type parameters from their enclosing class.
        // The goal is to ensure that the enum's values can't depend on the 
        // type parameters in any way.  And we don't see any need for an
        // enum to have additional type parameters.
        if (bmtGenerics->GetNumGenericArgs() != 0)
        {
            // Nested enums can have generic type parameters from their enclosing class.
            // CLS rules require type parameters to be propogated to nested types.
            // Note that class G<T> { enum E { } } will produce "G`1+E<T>".
            // We want to disallow class G<T> { enum E<T, U> { } }
            // Perhaps the IL equivalent of class G<T> { enum E { } } should be legal.
            if (!IsNested())
                BuildMethodTableThrowException(IDS_CLASSLOAD_ENUM_EXTRA_GENERIC_TYPE_PARAM);

            bool fWrongNumberOfTypeParams = false;
            mdTypeDef tdEnclosing = mdTypeDefNil;

            HRESULT hr = bmtInternal.pInternalImport->GetNestedClassProps(GetCl(), &tdEnclosing);
            if (FAILED(hr))
                ThrowHR(hr, BFA_UNABLE_TO_GET_NESTED_PROPS);

            HENUMInternal   hEnumGenericPars;
            if (FAILED(bmtInternal.pInternalImport->EnumInit(mdtGenericParam, tdEnclosing, &hEnumGenericPars)))
                GetAssembly()->ThrowTypeLoadException(bmtInternal.pInternalImport, tdEnclosing, IDS_CLASSLOAD_BADFORMAT);

            // Iterate past end of generic type params
            for (unsigned j = 0; j < bmtGenerics->GetNumGenericArgs() + 1; j++)
            {
                mdGenericParam tkTyPar = mdGenericParamNil;
                bmtInternal.pInternalImport->EnumNext(&hEnumGenericPars, &tkTyPar);
                // Check if we got back null?
                if (IsNilToken(tkTyPar)) {
                    fWrongNumberOfTypeParams = (j < bmtGenerics->GetNumGenericArgs());
                    break;
                }
                /*
                else if (j == bmtGenerics->GetNumGenericArgs()) {
                    fWrongNumberOfTypeParams = true;
                }
                */
            }
            bmtInternal.pInternalImport->EnumClose(&hEnumGenericPars);
            
            if (fWrongNumberOfTypeParams)
                BuildMethodTableThrowException(IDS_CLASSLOAD_ENUM_EXTRA_GENERIC_TYPE_PARAM);
        }
    }


    bmtParent.pParentMethodTable = pParentMethodTable;

    if (bmtParent.pParentMethodTable)
    {

    }
    else if (! (IsInterface() ) )
    {
        if(g_pObjectClass != NULL)
        {
            if(g_pObjectClass->GetAssembly()->GetManifestFile()->Equals(GetAssembly()->GetManifestFile()) &&
                !IsGlobalClass())
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_PARENTNULL);
            }
        }
    }


    // Set the contextful or marshalbyref flag if necessary
    SetContextfulOrByRef();

    // NOTE: This appears to be the earliest point during class loading that other classes MUST be loaded
    // resolve unresolved interfaces, determine an upper bound on the size of the interface map,
    // and determine the size of the largest interface (in # slots)
    ResolveInterfaces(pBuildingInterfaceList);

    // Compute parent class and interface module dependencies
    ComputeModuleDependencies();

    // Enumerate this class's members
    EnumerateMethodImpls();

    // Enumerate this class's members
    EnumerateClassMembers();

    // This will allocate the working versions of the VTable and NonVTable in bmtVT
    AllocateWorkingSlotTables();

    // Allocate a MethodDesc* for each method (needed later when doing interfaces), and a FieldDesc* for each field
    AllocateMethodFieldDescs(pamTracker);

    // Go thru all fields and initialize their FieldDescs.
    InitializeFieldDescs(bmtDomain, GetApproxFieldDescListRaw(), pLayoutRawFieldInfos, &bmtInternal, bmtGenerics,
        &bmtMetaData, &bmtEnumMF, &bmtError,
        &pByValueClassCache, &bmtMFDescs, &bmtFP, &bmtTCSInfo,
        &totalDeclaredFieldSize, &bmtParent);

    void *pv = pThread->m_MarshalAlloc.Alloc(sizeof(DispatchMapBuilder));
    bmtVT.pDispatchMapBuilder = new (pv) DispatchMapBuilder(&pThread->m_MarshalAlloc);

    // Determine vtable placement for each member in this class
    PlaceMembers((DWORD) wNumInterfaces, pBuildingInterfaceList, pamTracker);


    //
    // If we are a class, then there may be some unplaced vtable methods (which are by definition
    // interface methods, otherwise they'd already have been placed).  Place as many unplaced methods
    // as possible, in the order preferred by interfaces.  However, do not allow any duplicates - once
    // a method has been placed, it cannot be placed again - if we are unable to neatly place an interface,
    // create duplicate slots for it starting at dwCurrentDuplicateVtableSlot.  Fill out the interface
    // map for all interfaces as they are placed.
    //
    // If we are an interface, then all methods are already placed.  Fill out the interface map for
    // interfaces as they are placed.
    //
    if (!IsInterface())
    {
        PlaceVtableMethods((DWORD) wNumInterfaces, pBuildingInterfaceList);
        PlaceMethodImpls(pamTracker);
        // Now that interface method implementation have been fully resolved,
        // we need to make sure that type constraints are also met.
        ValidateInterfaceMethodConstraints();
    }

    // If we're a value class, we want to create duplicate slots
    // and MethodDescs for all methods in the vtable
    // section (i.e. not non-virtual instance methods or statics).
    //
    // This does not do non-virtual instance methods, which means
    // we have no BoxedEntryPointStubs available for these in the methodtable.
    // These are stored in the AssociatedMethodHash (see FindOrCreateAssociatedMethod).
    //
    //        if (!bmtProp.fContainsGenericVariables)
    ChangeValueClassVirtualsToBoxedEntryPointsAndCreateUnboxedEntryPoints(pamTracker);


    // Verify that we have not overflowed the number of slots.
    if (!FitsInU2((UINT64)bmtVT.dwCurrentVtableSlot + (UINT64)bmtVT.dwCurrentNonVtableSlot))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_TOO_MANY_METHODS);
    }

    // Place all non vtable methods
    for (i = 0; i < bmtVT.dwCurrentNonVtableSlot; i++)
    {
        MethodDesc *pMD = bmtVT.pNonVtableMD[i];

        _ASSERTE(pMD->GetSlot() == i);
        pMD->SetSlot(pMD->GetSlot() + (WORD) bmtVT.dwCurrentVtableSlot);

        bmtVT.SetMethodDescForSlot(pMD->GetSlot(), pMD);
    }

    if (bmtVT.wDefaultCtorSlot != MethodTable::NO_SLOT)
        bmtVT.wDefaultCtorSlot += (WORD) bmtVT.dwCurrentVtableSlot;

    if (bmtVT.wCCtorSlot != MethodTable::NO_SLOT)
        bmtVT.wCCtorSlot += (WORD) bmtVT.dwCurrentVtableSlot;

    bmtVT.dwCurrentNonVtableSlot += bmtVT.dwCurrentVtableSlot;

    // ensure we didn't overflow the temporary vtable
    _ASSERTE(bmtVT.dwCurrentNonVtableSlot <= bmtVT.dwMaxVtableSize);

    // Allocate dictionary layout used by all compatible instantiations
    GetHalfBakedClass()->m_pDictLayout = NULL;
    if (IsSharedByGenericInstantiations() && !bmtGenerics->fContainsGenericVariables)
    {
        // We use the number of methods as a heuristic for the number of slots in the dictionary
        // attached to shared class method tables.
        // If there are no declared methods then we have no slots, and we will never do any token lookups
        //
        // Heuristics
        //  - Classes with a small number of methods (2-3) tend to be more likely to use new slots,
        //    i.e. further methods tend to reuse slots from previous methods.
        //      = treat all classes with only 2-3 methods as if they have an extra method.
        //  - Classes with more generic parameters tend to use more slots.
        //      = multiply by 1.5 for 2 params or more

        DWORD numMethodsAdjusted =
            (bmtEnumMF.dwNumDeclaredNonAbstractMethods == 0)
            ? 0
            : (bmtEnumMF.dwNumDeclaredNonAbstractMethods < 3)
            ? 3
            : bmtEnumMF.dwNumDeclaredNonAbstractMethods;
        _ASSERTE(bmtGenerics->GetNumGenericArgs() != 0);
        DWORD nTypeFactorBy2 =  (bmtGenerics->GetNumGenericArgs() == 1) ? 2 : 3;

        DWORD estNumTypeSlots = (numMethodsAdjusted * nTypeFactorBy2 + 2) / 3;
        DWORD numTypeSlots = estNumTypeSlots;

        if (numTypeSlots > 0)
            GetHalfBakedClass()->m_pDictLayout = DictionaryLayout::Allocate(numTypeSlots, bmtDomain);
    }


    // We decide here if we need a dynamic entry for our statics. We need it here because
    // the offsets of our fields will depend on this. For the dynamic case (which requires
    // an extra indirection (indirect depending of methodtable) we'll allocate the slot
    // in setupmethodtable
    if (((pModule->IsReflection() || bmtGenerics->HasInstantiation()) &&
        (bmtVT.wCCtorSlot != MethodTable::NO_SLOT || bmtEnumMF.dwNumStaticFields !=0))
        )
    {
        // We will need a dynamic id
        bmtProp.fDynamicStatics = TRUE;

        if (bmtGenerics->HasInstantiation())
        {
            bmtProp.fGenericsStatics = TRUE;
        }
    }
    else
    {
            SetModuleDynamicID(MODULE_NON_DYNAMIC_STATICS);
    }

    // Place static fields
    PlaceStaticFields();

#if _DEBUG
    if (GetNumStaticFields() > 0)
    {
        LOG((LF_CODESHARING,
                LL_INFO10000,
                "Placing %d %sshared statics (%d handles) for class %s.\n",
                GetNumStaticFields(), pModule->IsCompiledDomainNeutral() ? "" : "un", GetNumHandleStatics(),
                pszDebugName));
    }
#endif // _DEBUG

//#define NumStaticFieldsOfSize $$$$$
//#define StaticFieldStart $$$$$

    if (IsBlittable())
    {
        SetNumGCPointerSeries(0);
        bmtFP.NumInstanceGCPointerFields = 0;

        _ASSERTE(HasLayout());
        SetNumInstanceFieldBytes(((LayoutEEClass*)GetHalfBakedClass())->GetLayoutInfo()->m_cbNativeSize);
    }
    else if (IsManagedSequential())
    {
        SetNumGCPointerSeries(0);
        bmtFP.NumInstanceGCPointerFields = 0;

        _ASSERTE(HasLayout());
        SetNumInstanceFieldBytes(((LayoutEEClass*)GetHalfBakedClass())->GetLayoutInfo()->m_cbManagedSize);
    }
    else
    {
        _ASSERTE(!IsBlittable());
        // HandleExplicitLayout fails for the GenericTypeDefinition when
        // it will succeed for some particular instantiations.
        // Thus we only do explicit layout for real instantiations, e.g. C<int>, not
        // the open types such as the GenericTypeDefinition C<!0> or any
        // of the "fake" types involving generic type variables which are
        // used for reflection and verification, e.g. C<List<!0>>.
        if (!bmtGenerics->fContainsGenericVariables && HasExplicitFieldOffsetLayout())
        {
            HandleExplicitLayout(pByValueClassCache);
        }
        else
        {
            // Place instance fields
            PlaceInstanceFields(pByValueClassCache);
        }
    }

        // We enforce that all value classes have non-zero size
    if (IsValueClass() && GetNumInstanceFieldBytes() == 0)
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_ZEROSIZE);
    }


    // If the class is serializable we scan it for VTS (Version Tolerant
    // Serialization) event methods or NotSerialized or OptionalField
    // fields. Any such info found will be attached to the method as
    // optional data later.
    if (IsTdSerializable(GetAttrClass()))
        ScanTypeForVtsInfo();

    // Now setup the method table
    SetupMethodTable2(pamTracker, pLoaderModule);

    MethodTable *pMT = GetHalfBakedMethodTable();

    if (bmtGenerics->pVarianceInfo != NULL)
    {
        pMT->SetHasVariance();
    }

    if (bmtFP.fHasFixedAddressValueTypes)
    {
        // To make things simpler, if the class has any field with this requirement, we'll set
        // all the statics to have this property. This allows us to only need to persist one bit
        // for the ngen case.
        pMT->SetFixedAddressStaticVTs();
    }

    if (IsValueClass() && (GetNumInstanceFieldBytes() != totalDeclaredFieldSize || HasOverLayedField()))
    {
        pMT->SetNotTightlyPacked();
    }

#ifdef _DEBUG
    pMT->SetDebugClassName(GetDebugClassName());
#endif // _DEBUG




    if (HasExplicitFieldOffsetLayout())
        // Perform relevant GC calculations for tdexplicit
        HandleGCForExplicitLayout();
    else
        // Perform relevant GC calculations for value classes
        HandleGCForValueClasses(pByValueClassCache);

    if (pMT->ContainsPointers())
    {
        CGCDesc* gcDesc = CGCDesc::GetCGCDescFromMT(pMT);
        qsort(gcDesc->GetLowestSeries(), (int)gcDesc->GetNumSeries(), sizeof(CGCDescSeries), compareCGCDescSeries);
    }

    if (MethodTable::ComputeIsPreInit(bmtGenerics->fContainsGenericVariables,
                                        pMT->HasClassConstructor(),
                                        (bmtFP.NumStaticGCBoxedFields > 0),
                                        bmtProp.fDynamicStatics))
    {
        // Mark the class as needing no static initialization, i.e. we've done all necessary
        // initialization at load time.
        pMT->SetClassPreInited();
    }

    pMT->MaybeSetHasFinalizer();

#if CHECK_APP_DOMAIN_LEAKS
    // Figure out if we're domain agile..
    // Note that this checks a bunch of field directly on the class & method table,
    // so it needs to come late in the game.
    SetAppDomainAgileAttribute();
#endif // CHECK_APP_DOMAIN_LEAKS

    // Allocate dynamic slot if necessary
    if (bmtProp.fDynamicStatics)
    {
        if (bmtProp.fGenericsStatics)
        {
            FieldDesc* pStaticFieldDescs = NULL;

            if (GetNumStaticFields() != 0)
            {
                pStaticFieldDescs = pMT->GetApproxFieldDescListRaw() + pMT->GetNumIntroducedInstanceFields();
            }

            pMT->SetupGenericsStaticsInfo(pStaticFieldDescs);
        }
        else
        {
            // Get an id for the dynamic class. We store it in the class because
            // no class that is persisted in ngen should have it (ie, if the class is ngened
            SetModuleDynamicID(GetModule()->AllocateDynamicEntry(pMT));
        }
    }

    //
    // if there are context or thread static set the info in the method table optional members
    //
    if (bmtTCSInfo.dwThreadStaticsSize != 0 || bmtTCSInfo.dwContextStaticsSize != 0)
    {
        if ((bmtTCSInfo.dwThreadStaticsSize != (WORD)bmtTCSInfo.dwThreadStaticsSize) ||
            (bmtTCSInfo.dwContextStaticsSize != (WORD)bmtTCSInfo.dwContextStaticsSize)) {
            BuildMethodTableThrowException(IDS_EE_TOOMANYFIELDS);
        }

        // this is responsible for setting the flag and allocation in the loader heap
        pMT->SetupThreadOrContextStatics(pamTracker, (WORD)bmtTCSInfo.dwThreadStaticsSize, (WORD)bmtTCSInfo.dwContextStaticsSize);
    }

    // If we have a non-interface class, then do inheritance security
    // checks on it. The check starts by checking for inheritance
    // permission demands on the current class. If these first checks
    // succeeded, then the cached declared method list is scanned for
    // methods that have inheritance permission demands.
    VerifyInheritanceSecurity();


    // Check for the RemotingProxy Attribute
    if (IsContextful())
    {
        _ASSERTE(g_pObjectClass);
        // Skip mscorlib marshal-by-ref classes since they all
        // are assumed to have the default proxy attribute
        if (!(pModule == g_pObjectClass->GetModule()))
        {
            CheckForRemotingProxyAttrib();
        }
    }

    if (IsContextful() || HasRemotingProxyAttribute())
    {
        // Contextful and classes that have a remoting proxy attribute
        // (whether they are MarshalByRef or ContextFul) always take the slow
        // path of managed activation
        pMT->SetRequiresManagedActivation();
    }

    // structs with GC poitners MUST be pointer sized aligned because the GC assumes it
    if (IsValueClass() && pMT->ContainsPointers() &&  GetNumInstanceFieldBytes() % sizeof(void*) != 0)
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_BADFORMAT);
    }

    if (IsInterface())
    {
        // Reset parent class
        pMT->SetParentMethodTable (g_pObjectClass);
    }

#ifdef _DEBUG
    // Reset the debug method names for BoxedEntryPointStubs
    // so they reflect the very best debug information for the methods
    {
        DeclaredMethodIterator methIt(*this);
        while (methIt.Next())
        {
            if (methIt.GetUnboxedMethodDesc() != NULL)
            {
                {
                    MethodDesc *pMD = methIt.GetUnboxedMethodDesc();
                    StackSString name(SString::Utf8);
                    TypeString::AppendMethodDebug(name, pMD);
                    StackScratchBuffer buff;
                    const char* pDebugNameUTF8 = name.GetUTF8(buff);
                    size_t len = strlen(pDebugNameUTF8)+1;
                    pMD->m_pszDebugMethodName = (char*) pamTracker->Track(GetDomain()->GetLowFrequencyHeap()->AllocMem(len));
                    _ASSERTE(pMD->m_pszDebugMethodName);
                    strcpy_s((char *) pMD->m_pszDebugMethodName, len, pDebugNameUTF8);
                }

                {
                    MethodDesc *pMD = methIt.GetMethodDesc();

                    StackSString name(SString::Utf8);
                    TypeString::AppendMethodDebug(name, pMD);
                    StackScratchBuffer buff;
                    const char* pDebugNameUTF8 = name.GetUTF8(buff);
                    size_t len = strlen(pDebugNameUTF8)+1;
                    pMD->m_pszDebugMethodName = (char*) pamTracker->Track(GetDomain()->GetLowFrequencyHeap()->AllocMem(len));
                    _ASSERTE(pMD->m_pszDebugMethodName);
                    strcpy_s((char *) pMD->m_pszDebugMethodName, len, pDebugNameUTF8);
                }
            }
        }
    }
#endif // _DEBUG

    // Make sure the object cloner won't attempt to blit types that aren't serializable.
    if (!IsTdSerializable(GetAttrClass()) && !IsEnum())
        SetCannotBeBlittedByObjectCloner();

    //If this is a value type, then propagate the UnsafeValueTypeAttribute from
    //its instance members to this type.
    if (IsValueClass() && !IsUnsafeValueClass())
    {
        ApproxFieldDescIterator fields(GetHalfBakedMethodTable(),
                                       ApproxFieldDescIterator::INSTANCE_FIELDS,
                                       FALSE);
        FieldDesc * current;
        while (NULL != (current = fields.Next()))
        {
            CONSISTENCY_CHECK(!current->IsStatic());
            if (current->GetFieldType() == ELEMENT_TYPE_VALUETYPE)
            {
                TypeHandle th = current->LookupApproxFieldTypeHandle();
                CONSISTENCY_CHECK(!th.IsNull());
                if (th.AsMethodTable()->GetClass()->IsUnsafeValueClass())
                {
                    SetUnsafeValueClass();
                    break;
                }
            }
        }
    }

    // Grow the typedef ridmap in advance as we can't afford to
    // fail once we set the resolve bit
    pModule->EnsureTypeDefCanBeStored(bmtInternal.cl);

    // Grow the tables in advance so that RID map filling cannot fail
    // once we're past the commit point.
    EnsureRIDMapsCanBeFilled();

    {
        // NOTE. NOTE!! the EEclass can now be accessed by other threads.
        // Do NOT place any initialization after this point.
        // You may NOT fail the call after this point.
        FAULT_FORBID();
        CANNOTTHROWCOMPLUSEXCEPTION();

        /*
        pamTracker->SuppressRelease();
        */
    }

#ifdef _DEBUG
    if (g_pConfig->ShouldDumpOnClassLoad(pszDebugName))
    {
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Method table summary for '%s':\n", pszDebugName));
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Number of static fields: %d\n", bmtEnumMF.dwNumStaticFields));
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Number of instance fields: %d\n", bmtEnumMF.dwNumInstanceFields));
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Number of static obj ref fields: %d\n", bmtEnumMF.dwNumStaticObjRefFields));
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Number of static boxed fields: %d\n", bmtEnumMF.dwNumStaticBoxedFields));
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Number of declared fields: %d\n", NumDeclaredFields()));
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Number of declared methods: %d\n", NumDeclaredMethods()));
        LOG((LF_CLASSLOADER, LL_ALWAYS, "Number of declared non-abstract methods: %d\n", bmtEnumMF.dwNumDeclaredNonAbstractMethods));
        pMT->DebugDumpVtable(pszDebugName, false);
        GetHalfBakedClass()->DebugDumpFieldLayout(pszDebugName, false);
        GetHalfBakedClass()->DebugDumpGCDesc(pszDebugName, false);
    }
#endif // _DEBUG

    STRESS_LOG3(LF_CLASSLOADER,  LL_INFO1000, "MethodTableBuilder: finished method table for module %p token %x = %pT \n",
        pModule,
        GetCl(),
        GetHalfBakedMethodTable());


    // Make sure that nobody access what were stack-allocated structures in this method.
    NullBMTData();

    END_INTERIOR_STACK_PROBE;
}

//*******************************************************************************
// Resolve unresolved interfaces, determine an upper bound on the size of the interface map,
// and determine the size of the largest interface (in # slots)
//
VOID MethodTableBuilder::ResolveInterfaces(BuildingInterfaceInfo_t *pBuildingInterfaceList)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtDomain));
        PRECONDITION(CheckPointer(bmtInterface));
        PRECONDITION(CheckPointer(bmtVT));
        PRECONDITION(CheckPointer(bmtParent));
    }
    CONTRACTL_END;

    DWORD i;
    Thread *pThread = GetThread();

    // resolve unresolved interfaces, determine an upper bound on the size of the interface map,
    // and determine the size of the largest interface (in # slots)
    bmtInterface->dwMaxExpandedInterfaces = 0; // upper bound on max # interfaces implemented by this class

    // First look through the interfaces explicitly declared by this class
    for (i = 0; i < bmtInterface->dwInterfaceMapSize; i++)
    {
        MethodTable *pInterface = pBuildingInterfaceList[i].m_pMethodTable;

        if (IsSerializerRelatedInterface(pInterface))
            SetCannotBeBlittedByObjectCloner();

        bmtInterface->dwMaxExpandedInterfaces += (1+ pInterface->GetNumInterfaces());

    }

    // Now look at interfaces inherited from the parent
    if (bmtParent->pParentMethodTable != NULL)
    {
        //@GENERICS: for our purposes here we use the generic parent because all its interfaces will be preloaded
        MethodTable::InterfaceMapIterator it = bmtParent->pParentMethodTable->IterateInterfaceMap();
        while (it.Next())
        {
            MethodTable *pMT = it.GetInterface();

            bmtInterface->dwMaxExpandedInterfaces += (1+pMT->GetNumInterfaces());
        }
    }

    bmtInterface->ppInterfaceSubstitutionChains = (Substitution**) pThread->m_MarshalAlloc.Alloc(sizeof(Substitution *) * bmtInterface->dwMaxExpandedInterfaces);

    // Create a fully expanded map of all interfaces we implement
    bmtInterface->pInterfaceMap = (InterfaceInfo_t *) pThread->m_MarshalAlloc.Alloc(sizeof(InterfaceInfo_t) * bmtInterface->dwMaxExpandedInterfaces);

    // # slots of largest interface
    bmtInterface->dwLargestInterfaceSize = 0;

    LoadApproxInterfaceMap(pBuildingInterfaceList,
                           bmtParent->pParentMethodTable);

    _ASSERTE(bmtInterface->dwInterfaceMapSize <= bmtInterface->dwMaxExpandedInterfaces);

    if (bmtInterface->dwLargestInterfaceSize > 0)
    {
        // This is needed later - for each interface, we get the MethodDesc pointer for each
        // method.  We need to be able to persist at most one interface at a time, so we
        // need enough memory for the largest interface.
        bmtInterface->ppInterfaceMethodDescList = (MethodDesc**)
            pThread->m_MarshalAlloc.Alloc(bmtInterface->dwLargestInterfaceSize * sizeof(MethodDesc*));

        bmtInterface->ppInterfaceDeclMethodDescList = (MethodDesc**)
            pThread->m_MarshalAlloc.Alloc(bmtInterface->dwLargestInterfaceSize * sizeof(MethodDesc*));
    }

    MethodTable *pParentClass = (IsInterface() || bmtParent->pParentMethodTable == NULL) ? NULL : bmtParent->pParentMethodTable;

    // For all the new interfaces we bring in, sum the methods
    bmtInterface->dwTotalNewInterfaceMethods = 0;
    if (pParentClass != NULL)
    {
        for (i = bmtParent->pParentMethodTable->GetNumInterfaces(); i < (bmtInterface->dwInterfaceMapSize); i++)
            bmtInterface->dwTotalNewInterfaceMethods +=
                bmtInterface->pInterfaceMap[i].m_pMethodTable->GetNumVirtuals();
    }

    // Inherit parental slot counts
    if (pParentClass != NULL)
    {
        bmtVT->dwCurrentVtableSlot         = bmtParent->pParentMethodTable->GetNumVirtuals();
        bmtParent->dwNumParentInterfaces   = bmtParent->pParentMethodTable->GetNumInterfaces();
        bmtParent->NumParentPointerSeries  = pParentClass->GetClass()->m_wNumGCPointerSeries;

        if (pParentClass->HasFieldsWhichMustBeInited())
            SetHasFieldsWhichMustBeInited();
        if (pParentClass->CannotBeBlittedByObjectCloner())
            SetCannotBeBlittedByObjectCloner();
    }
    else
    {
        bmtVT->dwCurrentVtableSlot         = 0;
        bmtParent->dwNumParentInterfaces   = 0;
        bmtParent->NumParentPointerSeries  = 0;
    }

    bmtVT->dwCurrentNonVtableSlot      = 0;

    bmtInterface->pppInterfaceImplementingMD = (MethodDesc ***) pThread->m_MarshalAlloc.Alloc(sizeof(MethodDesc *) * bmtInterface->dwMaxExpandedInterfaces);
    memset(bmtInterface->pppInterfaceImplementingMD, 0, sizeof(MethodDesc *) * bmtInterface->dwMaxExpandedInterfaces);

    bmtInterface->pppInterfaceDeclaringMD = (MethodDesc ***) pThread->m_MarshalAlloc.Alloc(sizeof(MethodDesc *) * bmtInterface->dwMaxExpandedInterfaces);
    memset(bmtInterface->pppInterfaceDeclaringMD, 0, sizeof(MethodDesc *) * bmtInterface->dwMaxExpandedInterfaces);

}

//*******************************************************************************
VOID MethodTableBuilder::ComputeModuleDependencies()
{
    // Add dependencies for parents up the chain

    MethodTable *pParent = bmtParent->pParentMethodTable;
    while (pParent != NULL)
    {
        Module *pParentModule = pParent->GetModule();

        if (pParentModule != bmtInternal->pModule)
            bmtInternal->pModule->AddClassDependency(pParentModule, &GetHalfBakedClass()->m_classDependencies);

        pParent = pParent->GetParentMethodTable();
    }

}

//*******************************************************************************
/* static */
int __cdecl MethodTableBuilder::bmtMetaDataInfo::MethodImplTokenPair::Compare(
        const void *elem1,
        const void *elem2)
{
    STATIC_CONTRACT_LEAF;
    MethodImplTokenPair *e1 = (MethodImplTokenPair *)elem1;
    MethodImplTokenPair *e2 = (MethodImplTokenPair *)elem2;
    if (e1->methodBody < e2->methodBody) return -1;
    else if (e1->methodBody > e2->methodBody) return 1;
    else if (e1->methodDecl < e2->methodDecl) return -1;
    else if (e1->methodDecl > e2->methodDecl) return 1;
    else return 0;
}

//*******************************************************************************
/* static */
BOOL MethodTableBuilder::bmtMetaDataInfo::MethodImplTokenPair::Equal(
        const MethodImplTokenPair *elem1,
        const MethodImplTokenPair *elem2)
{
    STATIC_CONTRACT_LEAF;
    return ((elem1->methodBody == elem2->methodBody) &&
            (elem1->methodDecl == elem2->methodDecl));
}

//*******************************************************************************
VOID    MethodTableBuilder::EnumerateMethodImpls()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END


    HRESULT hr = S_OK;
    IMDInternalImport *pMDInternalImport = bmtInternal->pInternalImport;
    DWORD rid, maxRidMD, maxRidMR;
    hr = pMDInternalImport->EnumMethodImplInit(GetCl(),
                                               &(bmtEnumMF->hEnumBody),
                                               &(bmtEnumMF->hEnumDecl));

    if (FAILED(hr))
    {
        BuildMethodTableThrowException(hr, *bmtError);
    }

    // We have successfully opened the enum, make sure to close when we're done.
    bmtEnumMF->fNeedToCloseEnumMethodImpl = true;

    // This gets the count out of the metadata interface.
    bmtEnumMF->dwNumberMethodImpls = pMDInternalImport->EnumMethodImplGetCount(&(bmtEnumMF->hEnumBody), &(bmtEnumMF->hEnumDecl));

    // This is the first pass. In this we will simply enumerate the token pairs and fill in
    // the data structures. In addition, we'll sort the list and eliminate duplicates.
    if (bmtEnumMF->dwNumberMethodImpls > 0)
    {
        //
        // Allocate the structures to keep track of the token pairs
        //
        DWORD cbAllocSize = 0;
        if (!ClrSafeInt<DWORD>::multiply(bmtEnumMF->dwNumberMethodImpls, sizeof(bmtMetaDataInfo::MethodImplTokenPair), cbAllocSize))
            ThrowHR(COR_E_OVERFLOW);

        bmtMetaData->rgMethodImplTokens = (bmtMetaDataInfo::MethodImplTokenPair *)
            GetThread()->m_MarshalAlloc.Alloc(cbAllocSize);

        // Iterate through each MethodImpl declared on this class
        for (DWORD i = 0; i < bmtEnumMF->dwNumberMethodImpls; i++)
        {
            // Grab the next set of body/decl tokens
            if(!pMDInternalImport->EnumMethodImplNext(&(bmtEnumMF->hEnumBody),
                                                      &(bmtEnumMF->hEnumDecl),
                                                      &bmtMetaData->rgMethodImplTokens[i].methodBody,
                                                      &bmtMetaData->rgMethodImplTokens[i].methodDecl))
            {
                // In the odd case that the enumerator fails before we've reached the total reported
                // entries, let's reset the count and just break out. (Should we throw?)
                bmtEnumMF->dwNumberMethodImpls = i;
                break;
            }
        }

        // No need to do any sorting or duplicate elimination if there's not two or more methodImpls
        if (bmtEnumMF->dwNumberMethodImpls > 1)
        {
            // Now sort
            qsort(bmtMetaData->rgMethodImplTokens,
                  bmtEnumMF->dwNumberMethodImpls,
                  sizeof(bmtMetaDataInfo::MethodImplTokenPair),
                  &bmtMetaDataInfo::MethodImplTokenPair::Compare);

            // Now eliminate duplicates
            for (DWORD i = 0; i < bmtEnumMF->dwNumberMethodImpls - 1; i++)
            {
                CONSISTENCY_CHECK((i + 1) < bmtEnumMF->dwNumberMethodImpls);

                bmtMetaDataInfo::MethodImplTokenPair *e1 = &bmtMetaData->rgMethodImplTokens[i];
                bmtMetaDataInfo::MethodImplTokenPair *e2 = &bmtMetaData->rgMethodImplTokens[i + 1];

                // If the pair are equal, eliminate the first one, and reduce the total count by one.
                if (bmtMetaDataInfo::MethodImplTokenPair::Equal(e1, e2))
                {
                    DWORD dwCopyNum = bmtEnumMF->dwNumberMethodImpls - (i + 1);
                    memcpy(e1, e2, dwCopyNum * sizeof(bmtMetaDataInfo::MethodImplTokenPair));
                    bmtEnumMF->dwNumberMethodImpls--;
                    CONSISTENCY_CHECK(bmtEnumMF->dwNumberMethodImpls > 0);
                }
            }
        }
    }

    if(bmtEnumMF->dwNumberMethodImpls)
    {
        //
        // Allocate the structures to keep track of the impl matches
        //
        bmtMetaData->pMethodDeclSubsts = (Substitution*) GetThread()->m_MarshalAlloc.Alloc(
            bmtEnumMF->dwNumberMethodImpls * sizeof(Substitution));

        bmtMethodImpl->rgEntries = (bmtMethodImplInfo::Entry *) GetThread()->m_MarshalAlloc.Alloc(
            bmtEnumMF->dwNumberMethodImpls * sizeof(bmtMethodImplInfo::Entry));

        // These are used for verification
        maxRidMD = pMDInternalImport->GetCountWithTokenKind(mdtMethodDef);
        maxRidMR = pMDInternalImport->GetCountWithTokenKind(mdtMemberRef);

        // Iterate through each MethodImpl declared on this class
        for (DWORD i = 0; i < bmtEnumMF->dwNumberMethodImpls; i++)
        {
            PCCOR_SIGNATURE pSigDecl=NULL,pSigBody = NULL;
            ULONG           cbSigDecl, cbSigBody;
            mdToken tkParent;

            mdToken theBody, theDecl;
            Substitution theDeclSubst(bmtInternal->pModule, NULL, NULL); // this can get updated later below.

            theBody = bmtMetaData->rgMethodImplTokens[i].methodBody;
            theDecl = bmtMetaData->rgMethodImplTokens[i].methodDecl;

            // IMPLEMENTATION LIMITATION: currently, we require that the body of a methodImpl
            // belong to the current type. This is because we need to allocate a different
            // type of MethodDesc for bodies that are part of methodImpls.
            if (TypeFromToken(theBody) != mdtMethodDef)
            {
                Module* pModule;
                mdToken theNewBody;
                hr = FindMethodDeclarationForMethodImpl(theBody,
                                                        &theNewBody,
                                                        TRUE,
                                                        &pModule);
                if (FAILED(hr))
                {
                    BuildMethodTableThrowException(hr, IDS_CLASSLOAD_MI_ILLEGAL_BODY, mdMethodDefNil);
                }
                _ASSERTE(pModule == bmtInternal->pModule);
                theBody = theNewBody;

                // Make sure to update the stored token with the resolved token.
                bmtMetaData->rgMethodImplTokens[i].methodBody = theBody;
            }

            if (TypeFromToken(theBody) != mdtMethodDef)
            {
                BuildMethodTableThrowException(BFA_METHODDECL_NOT_A_METHODDEF);
            }
            CONSISTENCY_CHECK(theBody == bmtMetaData->rgMethodImplTokens[i].methodBody);

            //
            // Now that the tokens of Decl and Body are obtained, do the MD validation
            //

            rid = RidFromToken(theDecl);

            // Perform initial rudimentary validation of the token. Full token verification
            // will be done in TestMethodImpl when placing the methodImpls.
            if (TypeFromToken(theDecl) == mdtMethodDef)
            {
                // Decl must be valid token
                if ((rid == 0)||(rid > maxRidMD))
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_MI_ILLEGAL_TOKEN_DECL);
                }
                // Get signature and length
                pSigDecl = pMDInternalImport->GetSigOfMethodDef(theDecl,&cbSigDecl);
            }

            // The token is not a MethodDef (likely a MemberRef)
            else
            {
                // Decl must be valid token
                if ((TypeFromToken(theDecl) != mdtMemberRef) || (rid == 0) || (rid > maxRidMR))
                {
                    bmtError->resIDWhy = IDS_CLASSLOAD_MI_ILLEGAL_TOKEN_DECL;
                    BuildMethodTableThrowException(IDS_CLASSLOAD_MI_ILLEGAL_TOKEN_DECL);
                }

                // Get signature and length
                LPCSTR szDeclName;
                szDeclName = pMDInternalImport->GetNameAndSigOfMemberRef(theDecl,&pSigDecl,&cbSigDecl);

                // Get parent
                hr = pMDInternalImport->GetParentToken(theDecl,&tkParent);
                if (FAILED(hr))
                    BuildMethodTableThrowException(hr, *bmtError);

                theDeclSubst = Substitution(tkParent, bmtInternal->pModule, NULL);
            }

            // Perform initial rudimentary validation of the token. Full token verification
            // will be done in TestMethodImpl when placing the methodImpls.
            {
                // Body must be valid token
                rid = RidFromToken(theBody);
                if ((rid == 0)||(rid > maxRidMD))
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_MI_ILLEGAL_TOKEN_BODY);
                }
                // Body's parent must be this class
                hr = pMDInternalImport->GetParentToken(theBody,&tkParent);
                if (FAILED(hr))
                    BuildMethodTableThrowException(hr, *bmtError);
                if(tkParent != GetCl())
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_MI_ILLEGAL_BODY);
                }
            }
            // Decl's and Body's signatures must match
            if(pSigDecl && cbSigDecl)
            {
                if((pSigBody = pMDInternalImport->GetSigOfMethodDef(theBody,&cbSigBody)) != NULL && cbSigBody)
                {
                    // Can't use memcmp because there may be two AssemblyRefs
                    // in this scope, pointing to the same assembly, etc.).
                    if (!MetaSig::CompareMethodSigs(pSigDecl,
                                                    cbSigDecl,
                                                    bmtInternal->pModule,
                                                    &theDeclSubst,
                                                    pSigBody,
                                                    cbSigBody,
                                                    bmtInternal->pModule,
                                                    NULL))
                    {
                        BuildMethodTableThrowException(IDS_CLASSLOAD_MI_BODY_DECL_MISMATCH);
                    }
                }
                else
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_MI_MISSING_SIG_BODY);
                }
            }
            else
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_MI_MISSING_SIG_DECL);
            }

            bmtMetaData->pMethodDeclSubsts[i] = theDeclSubst;

        }
    }
}

//*******************************************************************************
// Retrieve or add the TokenRange node for a particular token and nodelist.
/*static*/ MethodTableBuilder::bmtTokenRangeNode *MethodTableBuilder::GetTokenRange(mdToken tok, bmtTokenRangeNode **ppHead)
{
    CONTRACT (MethodTableBuilder::bmtTokenRangeNode *) {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        POSTCONDITION(CheckPointer(RETVAL));
    } CONTRACT_END;

    BYTE tokrange = ::GetTokenRange(tok);
    bmtTokenRangeNode *pWalk = *ppHead;
    while (pWalk)
    {
        if (pWalk->tokenHiByte == tokrange)
        {
            RETURN pWalk;
        }
        pWalk = pWalk->pNext;
    }

    // If we got here, this is the first time we've seen this token range.
    bmtTokenRangeNode *pNewNode = (bmtTokenRangeNode*)(GetThread()->m_MarshalAlloc.Alloc(sizeof(bmtTokenRangeNode)));
    pNewNode->tokenHiByte = tokrange;
    pNewNode->cMethods = 0;
    pNewNode->dwCurrentChunk = 0;
    pNewNode->dwCurrentIndex = 0;
    pNewNode->pNext = *ppHead;
    *ppHead = pNewNode;
    RETURN pNewNode;
}

//*******************************************************************************
//
// Find a method declaration that must reside in the scope passed in. This method cannot be called if
// the reference travels to another scope.
//
// Protect against finding a declaration that lives within
// us (the type being created)
//
HRESULT MethodTableBuilder::FindMethodDeclarationForMethodImpl(
            mdToken  pToken,       // Token that is being located (MemberRef or MemberDef)
            mdToken* pDeclaration, // Method definition for Member
            BOOL fSameClass,       // Does the declaration need to be in this class
            Module** pModule)      // Module that the Method Definitions is part of
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    HRESULT hr = S_OK;

    IMDInternalImport *pMDInternalImport = bmtInternal->pInternalImport;

//      // We are currently assumming that most MethodImpls will be used
//      // to define implementation for methods defined on an interface
//      // or base type. Therefore, we try to load entry first. If that
//      // indicates the member is on our type then we check meta data.
//    MethodDesc* pMethod = NULL;
//      hr = GetDescFromMemberRef(bmtInternal->pModule,
//  pToken,
//                                        GetCl(),
//                                        (void**) (&pMethod),
//                                        bmtError->pThrowable);
//      if(FAILED(hr) && !pThrowableAvailable(bmtError->pThrowable)) { // it was us we were find

    *pModule = bmtInternal->pModule;
    PCCOR_SIGNATURE pSig;  // Signature of Member
    DWORD           cSig;
    LPCUTF8         szMember = NULL;
    // The token should be a member ref or def. If it is a ref then we need to travel
    // back to us hopefully.
    if(TypeFromToken(pToken) == mdtMemberRef) {
        // Get the parent
        mdToken typeref = pMDInternalImport->GetParentOfMemberRef(pToken);
        GOTPARENT:
        // If parent is a method def then this is a varags method
        if (TypeFromToken(typeref) == mdtMethodDef) {
            mdTypeDef typeDef;
            hr = pMDInternalImport->GetParentToken(typeref, &typeDef);

            // Make sure it is a typedef
            if (TypeFromToken(typeDef) != mdtTypeDef) {
                BAD_FORMAT_NOTHROW_ASSERT(!"MethodDef without TypeDef as Parent");
                IfFailRet(COR_E_TYPELOAD);
            }
            BAD_FORMAT_NOTHROW_ASSERT(typeDef == GetCl());
            // This is the real method we are overriding
            *pDeclaration = mdtMethodDef;
        }
        else if (TypeFromToken(typeref) == mdtTypeSpec) {
            // Added so that method impls can refer to instantiated interfaces or classes
            pSig = pMDInternalImport->GetSigFromToken(typeref, &cSig);
            CorElementType elemType = (CorElementType) *pSig++;

            // If this is a generic inst, we expect that the next elem is ELEMENT_TYPE_CLASS,
            // which is handled in the case below.
            if (elemType == ELEMENT_TYPE_GENERICINST) {
                elemType = (CorElementType) *pSig++;
                BAD_FORMAT_NOTHROW_ASSERT(elemType == ELEMENT_TYPE_CLASS);
            }

            // This covers E_T_GENERICINST and E_T_CLASS typespec formats. We don't expect
            // any other kinds to come through here.
            if (elemType == ELEMENT_TYPE_CLASS) {
                CorSigUncompressToken(pSig, &typeref);
            }
            else {
                // This is an unrecognized signature format.
                BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT,
                                               IDS_CLASSLOAD_MI_BAD_SIG,
                                               mdMethodDefNil);
            }
            goto GOTPARENT;
        }
        else {
            // Verify that the ref points back to us
            mdToken tkDef = mdTokenNil;

            // We only get here when we know the token does not reference a type
            // in a different scope.
            if(TypeFromToken(typeref) == mdtTypeRef) {
                LPCUTF8 pszNameSpace;
                LPCUTF8 pszClassName;

                pMDInternalImport->GetNameOfTypeRef(typeref, &pszNameSpace, &pszClassName);
                mdToken tkRes = pMDInternalImport->GetResolutionScopeOfTypeRef(typeref);
                hr = pMDInternalImport->FindTypeDef(pszNameSpace,
                                                    pszClassName,
                                                    (TypeFromToken(tkRes) == mdtTypeRef) ? tkRes : mdTokenNil,
                                                    &tkDef);
                if(FAILED(hr)) {
                    IfFailRet(COR_E_TYPELOAD);
                }
            }
            // We get a typedef when the parent of the token is a typespec to the type.
            else if (TypeFromToken(typeref) == mdtTypeDef) {
                tkDef = typeref;
            }
            else {
                CONSISTENCY_CHECK_MSGF(FALSE, ("Invalid methodimpl signature in class %s.", GetDebugClassName()));
                BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT,
                                               IDS_CLASSLOAD_MI_BAD_SIG,
                                               mdMethodDefNil);
            }

            // If we required that the typedef be the same type as the current class,
            // and it doesn't match, we need to return a failure result.
            if(fSameClass && tkDef != GetCl()) {
                IfFailRet(COR_E_TYPELOAD);
            }

            szMember = pMDInternalImport->GetNameAndSigOfMemberRef(pToken,
                                                                   &pSig,
                                                                   &cSig);
            if(isCallConv(MetaSig::GetCallingConventionInfo(*pModule, pSig),
                          IMAGE_CEE_CS_CALLCONV_FIELD)) {
                return VLDTR_E_MR_BADCALLINGCONV;
            }

            hr = pMDInternalImport->FindMethodDef(tkDef,
                                                  szMember,
                                                  pSig,
                                                  cSig,
                                                  pDeclaration);
            IfFailRet(hr);
        }
    }
    else if(TypeFromToken(pToken) == mdtMethodDef) {
        mdTypeDef typeDef;

        // Verify that we are the parent
        hr = pMDInternalImport->GetParentToken(pToken, &typeDef);
        IfFailRet(hr);

        if(typeDef != GetCl())
        {
            IfFailRet(COR_E_TYPELOAD);
        }

        *pDeclaration = pToken;
    }
    else {
        IfFailRet(COR_E_TYPELOAD);
    }
    return hr;
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Enumerate this class's members
//
VOID    MethodTableBuilder::EnumerateClassMembers()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(bmtInternal));
        PRECONDITION(CheckPointer(bmtEnumMF));
        PRECONDITION(CheckPointer(bmtMFDescs));
        PRECONDITION(CheckPointer(bmtProp));
        PRECONDITION(CheckPointer(bmtMetaData));
        PRECONDITION(CheckPointer(bmtVT));
        PRECONDITION(CheckPointer(bmtError));
    }
    CONTRACTL_END;

    HRESULT hr = S_OK;
    DWORD i;
    Thread *pThread = GetThread();
    IMDInternalImport *pMDInternalImport = bmtInternal->pInternalImport;
    mdToken tok;
    DWORD dwMemberAttrs;
    BOOL fIsClassEnum = IsEnum();
    BOOL fIsClassInterface = IsInterface();
    BOOL fIsClassValueType = IsValueClass();
    BOOL fIsClassNotAbstract = (IsTdAbstract(GetAttrClass()) == 0);
    PCCOR_SIGNATURE pMemberSignature;
    ULONG           cMemberSignature;

    //
    // Run through the method list and calculate the following:
    // # methods.
    // # "other" methods (i.e. static or private)
    // # non-other methods
    //

    bmtVT->dwMaxVtableSize     = 0; // we'll fix this later to be the real upper bound on vtable size
    bmtMetaData->cMethods = 0;

    hr = pMDInternalImport->EnumInit(mdtMethodDef, GetCl(), &(bmtEnumMF->hEnumMethod));
    if (FAILED(hr))
    {
        _ASSERTE(!"Cannot count memberdefs");
        if (FAILED(hr))
        {
            BuildMethodTableThrowException(hr, *bmtError);
        }
    }
    bmtEnumMF->fNeedToCloseEnumMethod = true;

    DWORD cbAllocSize = 0;

    // Allocate an array to contain the method tokens as well as information about the methods.
    bmtMetaData->cMethAndGaps = pMDInternalImport->EnumGetCount(&(bmtEnumMF->hEnumMethod));
    if (!ClrSafeInt<DWORD>::multiply(bmtMetaData->cMethAndGaps, sizeof(mdToken), cbAllocSize))
        BuildMethodTableThrowException(COR_E_OVERFLOW);
    bmtMetaData->pMethods = (mdToken*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);
    if (!ClrSafeInt<DWORD>::multiply(bmtMetaData->cMethAndGaps, sizeof(ULONG), cbAllocSize))
        BuildMethodTableThrowException(COR_E_OVERFLOW);
    bmtMetaData->pMethodRVA = (ULONG*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);
    if (!ClrSafeInt<DWORD>::multiply(bmtMetaData->cMethAndGaps, sizeof(DWORD), cbAllocSize))
        BuildMethodTableThrowException(COR_E_OVERFLOW);
    bmtMetaData->pMethodAttrs = (DWORD*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);
    bmtMetaData->pMethodImplFlags = (DWORD*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);
    bmtMetaData->pMethodClassifications = (DWORD*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);
    if (!ClrSafeInt<DWORD>::multiply(bmtMetaData->cMethAndGaps, sizeof(LPSTR), cbAllocSize))
        BuildMethodTableThrowException(COR_E_OVERFLOW);
    bmtMetaData->pstrMethodName = (LPCSTR*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);
    if (!ClrSafeInt<DWORD>::multiply(bmtMetaData->cMethAndGaps, sizeof(BYTE), cbAllocSize))
        BuildMethodTableThrowException(COR_E_OVERFLOW);
    bmtMetaData->pMethodImpl = (BYTE*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);
    bmtMetaData->pMethodType = (BYTE*)pThread->m_MarshalAlloc.Alloc(cbAllocSize);

    enum { SeenCtor = 1, SeenInvoke = 2, SeenBeginInvoke = 4, SeenEndInvoke = 8};
    unsigned delegateMethodsSeen = 0;

    for (i = 0; i < bmtMetaData->cMethAndGaps; i++)
    {
        ULONG dwMethodRVA;
        DWORD dwImplFlags;
        DWORD Classification;
        LPSTR strMethodName;

        //
        // Go to the next method and retrieve its attributes.
        //

        pMDInternalImport->EnumNext(&(bmtEnumMF->hEnumMethod), &tok);
        DWORD   rid = RidFromToken(tok);
        if ((rid == 0)||(rid > pMDInternalImport->GetCountWithTokenKind(mdtMethodDef)))
        {
            BuildMethodTableThrowException(BFA_METHOD_TOKEN_OUT_OF_RANGE);
        }

        dwMemberAttrs = pMDInternalImport->GetMethodDefProps(tok);
        if (IsMdRTSpecialName(dwMemberAttrs) || IsMdVirtual(dwMemberAttrs) || IsAnyDelegateClass())
        {
            strMethodName = (LPSTR)pMDInternalImport->GetNameOfMethodDef(tok);
            if(IsStrLongerThan(strMethodName,MAX_CLASS_NAME))
            {
                BuildMethodTableThrowException(BFA_METHOD_NAME_TOO_LONG);
            }
        }
        else
            strMethodName = NULL;

        HENUMInternal hEnumTyPars;
        hr = pMDInternalImport->EnumInit(mdtGenericParam, tok, &hEnumTyPars);
        if (FAILED(hr))
        {
            BuildMethodTableThrowException(hr, *bmtError);
        }


        WORD numGenericMethodArgs = (WORD) pMDInternalImport->EnumGetCount(&hEnumTyPars);

        // We do not want to support context-bound objects with generic methods.
        if (IsContextful() && numGenericMethodArgs > 0)
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_CONTEXT_BOUND_GENERIC_METHOD);
        }

        if (numGenericMethodArgs != 0)
        {
            HENUMInternalHolder hEnumGenericPars(pMDInternalImport);

            hEnumGenericPars.EnumInit(mdtGenericParam, tok);

            for (unsigned methIdx = 0; methIdx < numGenericMethodArgs; methIdx++)
            {
                mdGenericParam tkTyPar;
                pMDInternalImport->EnumNext(&hEnumGenericPars, &tkTyPar);
                DWORD flags;
                pMDInternalImport->GetGenericParamProps(tkTyPar, NULL, &flags, NULL, NULL, NULL);

                if (0 != (flags & ~(gpVarianceMask | gpSpecialConstraintMask)))
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_BADFORMAT);
                }
                switch (flags & gpVarianceMask)
                {
                    case gpNonVariant:
                        break;

                    case gpCovariant: // intentional fallthru
                    case gpContravariant:
                        BuildMethodTableThrowException(VLDTR_E_GP_ILLEGAL_VARIANT_MVAR);
                        break;

                    default:
                        BuildMethodTableThrowException(IDS_CLASSLOAD_BADFORMAT);

                }

            }
            pMDInternalImport->EnumClose(&hEnumGenericPars);

        }

        

        //
        // We need to check if there are any gaps in the vtable. These are
        // represented by methods with the mdSpecial flag and a name of the form
        // _VTblGap_nnn (to represent nnn empty slots) or _VTblGap (to represent a
        // single empty slot).
        //

        if (IsMdRTSpecialName(dwMemberAttrs))
        {
            // The slot is special, but it might not be a vtable spacer. To
            // determine that we must look at the name.
            if (strncmp(strMethodName, "_VtblGap", 8) == 0)
            {

                LPCSTR pos = strMethodName + 8;

                // Skip optional number.
                while (IS_DIGIT(*pos))
                    pos++;

                WORD n = 0;

                // Check for presence of count.
                if (*pos == '\0')
                    n = 1;
                else
                {
                    if (*pos != '_')
                    {
                        BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT,
                                                       IDS_CLASSLOAD_BADSPECIALMETHOD,
                                                       mdMethodDefNil);
                    }

                    // Skip '_'.
                    pos++;

                    // Read count.
                    while (IS_DIGIT(*pos))
                    {
                        _ASSERTE(n < 6552);
                        n *= 10;
                        n += DIGIT_TO_INT(*pos);
                        pos++;
                    }

                    // Check for end of name.
                    if (*pos != '\0')
                    {
                        BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT,
                                                       BFA_METHOD_NAME_NOT_TERMINATED,
                                                       mdMethodDefNil);
                    }
                }

                continue;
            }

        }


        //
        // This is a real method so add it to the enumeration of methods. We now need to retrieve
        // information on the method and store it for later use.
        //
        pMDInternalImport->GetMethodImplProps(tok, &dwMethodRVA, &dwImplFlags);
        //
        // But first - minimal flags validity checks
        //
        // No methods in Enums!
        if(fIsClassEnum)
        {
            BuildMethodTableThrowException(BFA_METHOD_IN_A_ENUM);
        }
        // RVA : 0
        if(dwMethodRVA != 0)
        {
            if(IsMdAbstract(dwMemberAttrs))
            {
                BuildMethodTableThrowException(BFA_ABSTRACT_METHOD_WITH_RVA);
            }
            if(IsMiRuntime(dwImplFlags))
            {
                BuildMethodTableThrowException(BFA_RUNTIME_METHOD_WITH_RVA);
            }
            if(IsMiInternalCall(dwImplFlags))
            {
                BuildMethodTableThrowException(BFA_INTERNAL_METHOD_WITH_RVA);
            }
        }

        // Abstract / not abstract
        if(IsMdAbstract(dwMemberAttrs))
        {
            if(fIsClassNotAbstract)
            {
                BuildMethodTableThrowException(BFA_AB_METHOD_IN_AB_CLASS);
            }
            if(!IsMdVirtual(dwMemberAttrs))
            {
                BuildMethodTableThrowException(BFA_NONVIRT_AB_METHOD);
            }
        }
        else if(fIsClassInterface && strMethodName &&
                (strcmp(strMethodName, COR_CCTOR_METHOD_NAME)))
        {
            BuildMethodTableThrowException(BFA_NONAB_NONCCTOR_METHOD_ON_INT);
        }

        // Virtual / not virtual
        if(IsMdVirtual(dwMemberAttrs))
        {
            if(IsMdPinvokeImpl(dwMemberAttrs))
            {
                BuildMethodTableThrowException(BFA_VIRTUAL_PINVOKE_METHOD);
            }
            if(IsMdStatic(dwMemberAttrs))
            {
                BuildMethodTableThrowException(BFA_VIRTUAL_STATIC_METHOD);
            }
            if(strMethodName && (0==strcmp(strMethodName, COR_CTOR_METHOD_NAME)))
            {
                BuildMethodTableThrowException(BFA_VIRTUAL_INSTANCE_CTOR);
            }
        }

        // Some interface checks.
        if (IsInterface())
        {
            if (IsMdVirtual(dwMemberAttrs))
            {
                if (!IsMdAbstract(dwMemberAttrs))
                {
                    BuildMethodTableThrowException(BFA_VIRTUAL_NONAB_INT_METHOD);
                }
            }
            else
            {
                // Instance field/method
                if (!IsMdStatic(dwMemberAttrs))
                {
                    BuildMethodTableThrowException(BFA_NONVIRT_INST_INT_METHOD);
                }
            }
        }

        // No synchronized methods in ValueTypes
        if(fIsClassValueType && IsMiSynchronized(dwImplFlags))
        {
            BuildMethodTableThrowException(BFA_SYNC_METHOD_IN_VT);
        }

        // Global methods:
        if(IsGlobalClass())
        {
            if(!IsMdStatic(dwMemberAttrs))
            {
                BuildMethodTableThrowException(BFA_NONSTATIC_GLOBAL_METHOD);
            }
            if (strMethodName)
            {
                if(0==strcmp(strMethodName, COR_CTOR_METHOD_NAME))
                {
                    BuildMethodTableThrowException(BFA_GLOBAL_INST_CTOR);
                }
            }
        }
        //@GENERICS:
        // Generic methods or methods in generic classes
        // may not be part of a COM Import class, PInvoke, internal call.
        if ((bmtGenerics->GetNumGenericArgs() != 0 || numGenericMethodArgs != 0) &&
            (
             IsMdPinvokeImpl(dwMemberAttrs) ||
             IsMiInternalCall(dwImplFlags)))
        {
            BuildMethodTableThrowException(BFA_BAD_PLACE_FOR_GENERIC_METHOD);
        }

        // Generic methods may not be marked "runtime".  However note that
        // methods in generic delegate classes are, hence we don't apply this to
        // methods in generic classes in general.
        if (numGenericMethodArgs != 0 && IsMiRuntime(dwImplFlags))
        {
            BuildMethodTableThrowException(BFA_GENERIC_METHOD_RUNTIME_IMPL);
        }


        // Signature validation
        pMemberSignature = pMDInternalImport->GetSigOfMethodDef(tok,&cMemberSignature);
        hr = validateTokenSig(tok,pMemberSignature,cMemberSignature,dwMemberAttrs,pMDInternalImport);
        if (FAILED(hr))
        {
            BuildMethodTableThrowException(hr, BFA_BAD_SIGNATURE, mdMethodDefNil);
        }

        // Check the appearance of covariant and contravariant in the method signature
        // Note that variance is only supported for interfaces
        if (bmtGenerics->pVarianceInfo != NULL)
        {
            SigPointer sp(pMemberSignature);
            ULONG callConv;
            IfFailThrow(sp.GetCallingConvInfo(&callConv));
            
            if (callConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
                IfFailThrow(sp.GetData(NULL));

            DWORD numArgs;
            IfFailThrow(sp.GetData(&numArgs));

            // Return type behaves covariantly
            if (!GetHalfBakedClass()->CheckVarianceInSig(bmtGenerics->GetNumGenericArgs(), bmtGenerics->pVarianceInfo, sp, gpCovariant))
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_VARIANCE_IN_METHOD_RESULT, tok);
            }
            sp.SkipExactlyOne();
            for (DWORD j = 0; j < numArgs; j++)
            {
                // Argument types behave contravariantly
                if (!GetHalfBakedClass()->CheckVarianceInSig(bmtGenerics->GetNumGenericArgs(), bmtGenerics->pVarianceInfo,
                                        sp, gpContravariant))
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_VARIANCE_IN_METHOD_ARG, tok);
                }
                sp.SkipExactlyOne();
            }
        }

        //
        // Determine the method's classification.
        //

        if (IsReallyMdPinvokeImpl(dwMemberAttrs) || IsMiInternalCall(dwImplFlags))
        {
            hr = NDirect::HasNAT_LAttribute(pMDInternalImport, tok);
            if (FAILED(hr))
            {
                BuildMethodTableThrowException(hr, IDS_CLASSLOAD_BADPINVOKE, tok);
            }

            if (hr == S_FALSE)
            {
                if (dwMethodRVA == 0)
                    Classification = mcFCall;
                else
                    Classification = mcNDirect;
            }
            else
                Classification = mcNDirect;
        }
        else if (IsMiRuntime(dwImplFlags))
        {
                // currently the only runtime implemented functions are delegate instance methods
            if (!IsAnyDelegateClass() || IsMdStatic(dwMemberAttrs) || IsMdAbstract(dwMemberAttrs))
            {
                BuildMethodTableThrowException(BFA_BAD_RUNTIME_IMPL);
            }

            unsigned newDelegateMethodSeen = 0;

            if (IsMdRTSpecialName(dwMemberAttrs))   // .ctor
            {
                if (strcmp(strMethodName, COR_CTOR_METHOD_NAME) != 0 || IsMdVirtual(dwMemberAttrs))
                {
                    BuildMethodTableThrowException(BFA_BAD_FLAGS_ON_DELEGATE);
                }
                newDelegateMethodSeen = SeenCtor;
                Classification = mcFCall;
            }
            else
            {
                if (strcmp(strMethodName, "Invoke") == 0)
                    newDelegateMethodSeen = SeenInvoke;
                else if (strcmp(strMethodName, "BeginInvoke") == 0)
                    newDelegateMethodSeen = SeenBeginInvoke;
                else if (strcmp(strMethodName, "EndInvoke") == 0)
                    newDelegateMethodSeen = SeenEndInvoke;
                else
                {
                    BuildMethodTableThrowException(BFA_UNKNOWN_DELEGATE_METHOD);
                }
                Classification = mcEEImpl;
            }

            // If we get here we have either set newDelegateMethodSeen or we have thrown a BMT exception
            _ASSERTE(newDelegateMethodSeen != 0);

            if ((delegateMethodsSeen & newDelegateMethodSeen) != 0)
            {
                BuildMethodTableThrowException(BFA_DUPLICATE_DELEGATE_METHOD);
            }

            delegateMethodsSeen |= newDelegateMethodSeen;
        }
        else if (numGenericMethodArgs != 0)
        {
            //We use an instantiated method desc to represent a generic method
            Classification = mcInstantiated;
        }
        else if (fIsClassInterface)
        {
            // This codepath is used by remoting
            Classification = mcIL;
        }
        else
        {
            Classification = mcIL;
        }


#ifdef _DEBUG
        // We don't allow stack based declarative security on ecalls, fcalls and
        // other special purpose methods implemented by the EE (the interceptor
        // we use doesn't play well with non-jitted stubs).
        if ((Classification == mcFCall || Classification == mcEEImpl) &&
            (IsMdHasSecurity(dwMemberAttrs) || IsTdHasSecurity(GetAttrClass())))
        {
            DWORD dwSecFlags;
            DWORD dwNullDeclFlags;

            if (IsTdHasSecurity(GetAttrClass()) &&
                SUCCEEDED(Security::GetDeclarationFlags(pMDInternalImport, GetCl(), &dwSecFlags, &dwNullDeclFlags)))
            {
                CONSISTENCY_CHECK_MSG(!(dwSecFlags & ~dwNullDeclFlags & DECLSEC_RUNTIME_ACTIONS),
                                      "Cannot add stack based declarative security to a class containing an ecall/fcall/special method.");
            }
            if (IsMdHasSecurity(dwMemberAttrs) &&
                SUCCEEDED(Security::GetDeclarationFlags(pMDInternalImport, tok, &dwSecFlags, &dwNullDeclFlags)))
            {
                CONSISTENCY_CHECK_MSG(!(dwSecFlags & ~dwNullDeclFlags & DECLSEC_RUNTIME_ACTIONS),
                                      "Cannot add stack based declarative security to an ecall/fcall/special method.");
            }
        }
#endif // _DEBUG

        // Generic methods should always be mcInstantiated
        if (!((numGenericMethodArgs == 0) || ((Classification & mdcClassification) == mcInstantiated)))
        {
            BuildMethodTableThrowException(BFA_GENERIC_METHODS_INST);
        }
        // count how many overrides this method does All methods bodies are defined
        // on this type so we can just compare the tok with the body token found
        // from the overrides.
        for(DWORD impls = 0; impls < bmtEnumMF->dwNumberMethodImpls; impls++) {
            if(bmtMetaData->rgMethodImplTokens[impls].methodBody == tok) {
                Classification |= mdcMethodImpl;
                break;
            }
        }

        // For delegates we don't allow any non-runtime implemented bodies
        // for any of the four special methods
        if (IsAnyDelegateClass() && !IsMiRuntime(dwImplFlags))
        {
            if ((strcmp(strMethodName, COR_CTOR_METHOD_NAME) == 0) ||
                (strcmp(strMethodName, "Invoke")             == 0) || 
                (strcmp(strMethodName, "BeginInvoke")        == 0) || 
                (strcmp(strMethodName, "EndInvoke")          == 0)   )  
            {
                BuildMethodTableThrowException(BFA_ILLEGAL_DELEGATE_METHOD);
            }
        }

        //
        // Compute the type & other info
        //

        // Set the index into the storage locations
        BYTE impl;
        if (Classification & mdcMethodImpl)
            impl = METHOD_IMPL;
        else
            impl = METHOD_IMPL_NOT;

        BYTE type;
        if ((Classification & mdcClassification)  == mcNDirect)
        {
            type = METHOD_TYPE_NDIRECT;
        }
        else if ((Classification & mdcClassification) == mcFCall)
        {
            type = METHOD_TYPE_FCALL;
        }
        else if ((Classification & mdcClassification) == mcEEImpl)
        {
            type = METHOD_TYPE_EEIMPL;
        }
        else if ((Classification & mdcClassification) == mcInstantiated)
        {
            type = METHOD_TYPE_INSTANTIATED;
        }
        else
        {
            type = METHOD_TYPE_NORMAL;
        }

        //
        // Store the method and the information we have gathered on it in the metadata info structure.
        //

        bmtMetaData->SetMethodData(NumDeclaredMethods(),
                                   tok,
                                   dwMemberAttrs,
                                   dwMethodRVA,
                                   dwImplFlags,
                                   Classification,
                                   strMethodName,
                                   impl,
                                   type);

        IncNumDeclaredMethods();

        //
        // Update the count of the various types of methods.
        //

        bmtVT->dwMaxVtableSize++;

        // Increment the number of non-abstract declared methods
        if (!IsMdAbstract(dwMemberAttrs))
        {
            bmtEnumMF->dwNumDeclaredNonAbstractMethods++;
        }

        // Increment the number of IL instance methods
        if (type == METHOD_TYPE_NORMAL && !IsMdStatic(dwMemberAttrs))
        {
          bmtEnumMF->dwNumILInstanceMethods++;
        }

        // Increment the number of MethodDescs for this type of method
        bmtMFDescs->sets[type][impl].dwNumMethodDescs++;

        // If the method requires an unboxing method, record this fact
        BOOL hasUnboxing = (IsValueClass()
                            && !IsMdStatic(dwMemberAttrs)
                            && type != METHOD_TYPE_INSTANTIATED
                            && IsMdVirtual(dwMemberAttrs)
                            && !IsMdRTSpecialName(dwMemberAttrs));


        if (hasUnboxing)
        {
            bmtEnumMF->dwNumUnboxingMethods++;
            bmtMFDescs->sets[type][impl].dwNumBoxedEntryPointMDs++;
        }

        // Update the token ranges for this type of method to be used for MethodDescChunk allocation
        GetTokenRange(tok, &(bmtMetaData->ranges[type][impl]))->cMethods += (hasUnboxing ? 2 : 1);
    }

    // Check to see that we have all of the required delegate methods (ECMA 13.6 Delegates)
    if (IsAnyDelegateClass())
    {
        // Do we have all four special delegate methods 
        // or just the two special delegate methods 
        if ((delegateMethodsSeen != (SeenCtor | SeenInvoke | SeenBeginInvoke | SeenEndInvoke)) &&
            (delegateMethodsSeen != (SeenCtor | SeenInvoke)) )
        {
            BuildMethodTableThrowException(BFA_MISSING_DELEGATE_METHOD);
        }
    }

    if (i != bmtMetaData->cMethAndGaps)
    {
        BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_METHOD_COUNT, mdTokenNil);
    }

    pMDInternalImport->EnumReset(&(bmtEnumMF->hEnumMethod));


    //
    // Run through the field list and calculate the following:
    // # static fields
    // # static fields that contain object refs.
    // # instance fields
    //

    bmtEnumMF->dwNumStaticFields        = 0;
    bmtEnumMF->dwNumStaticObjRefFields  = 0;
    bmtEnumMF->dwNumStaticBoxedFields   = 0;
    bmtEnumMF->dwNumInstanceFields      = 0;

    hr = pMDInternalImport->EnumInit(mdtFieldDef, GetCl(), &(bmtEnumMF->hEnumField));
    if (FAILED(hr))
    {
        BuildMethodTableThrowException(hr, *bmtError);
    }
    bmtMetaData->cFields = pMDInternalImport->EnumGetCount(&(bmtEnumMF->hEnumField));
    bmtEnumMF->fNeedToCloseEnumField = true;

    // Retrieve the fields and store them in a temp array.
    bmtMetaData->pFields = (mdToken*)pThread->m_MarshalAlloc.Alloc(bmtMetaData->cFields * sizeof(mdToken));
    bmtMetaData->pFieldAttrs = (DWORD*)pThread->m_MarshalAlloc.Alloc(bmtMetaData->cFields * sizeof(DWORD));

    DWORD   dwFieldLiteralInitOnly = fdLiteral | fdInitOnly;

    for (i = 0; pMDInternalImport->EnumNext(&(bmtEnumMF->hEnumField), &tok); i++)
    {
        //
        // Retrieve the attributes of the field.
        //
        DWORD   rid = tok & 0x00FFFFFF;
        if ((rid == 0)||(rid > pMDInternalImport->GetCountWithTokenKind(mdtFieldDef)))
        {
            BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, BFA_BAD_FIELD_TOKEN, mdTokenNil);
        }

        dwMemberAttrs = pMDInternalImport->GetFieldDefProps(tok);


        //
        // Store the field and its attributes in the bmtMetaData structure for later use.
        //

        bmtMetaData->pFields[i] = tok;
        bmtMetaData->pFieldAttrs[i] = dwMemberAttrs;

        if((dwMemberAttrs & fdFieldAccessMask)==fdFieldAccessMask)
        {
            BuildMethodTableThrowException(BFA_INVALID_FIELD_ACC_FLAGS);
        }
        if((dwMemberAttrs & dwFieldLiteralInitOnly)==dwFieldLiteralInitOnly)
        {
            BuildMethodTableThrowException(BFA_FIELD_LITERAL_AND_INIT);
        }

            // can only have static global fields
        if(IsGlobalClass())
        {
            if(!IsMdStatic(dwMemberAttrs))
            {
                BuildMethodTableThrowException(BFA_NONSTATIC_GLOBAL_FIELD);
            }
        }

        //
        // Update the count of the various types of fields.
        //

        if (IsFdStatic(dwMemberAttrs))
        {
            if (!IsFdLiteral(dwMemberAttrs))
            {
                bmtEnumMF->dwNumStaticFields++;
            }
        }
        else
        {
            bmtEnumMF->dwNumInstanceFields++;
            if(fIsClassInterface)
            {
                BuildMethodTableThrowException(BFA_INSTANCE_FIELD_IN_INT);
            }
        }
    }

    if (i != bmtMetaData->cFields)
    {
        BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_FIELD_COUNT, mdTokenNil);
    }

    if(fIsClassEnum && (bmtEnumMF->dwNumInstanceFields==0))
    {
        BuildMethodTableThrowException(BFA_INSTANCE_FIELD_IN_ENUM);
    }

    bmtEnumMF->dwNumDeclaredFields = bmtEnumMF->dwNumStaticFields + bmtEnumMF->dwNumInstanceFields;
}

//*******************************************************************************
//
// Used by AllocateMethodFieldDescs
// Thus used by BuildMethodTable
//
// Allocates the chunks used to contain the method descs.
//
MethodDescChunk **  MethodTableBuilder::AllocateMDChunks(
                                  bmtTokenRangeNode *pTokenRanges, DWORD type, DWORD impl, DWORD *pNumChunks, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(pTokenRanges));
    }
    CONTRACTL_END;


    static const DWORD classifications[METHOD_TYPE_COUNT][METHOD_IMPL_COUNT] =
    {
        { mcIL, mcIL | mdcMethodImpl },
        { mcFCall, mcFCall | mdcMethodImpl },
        { mcEEImpl, mcEEImpl | mdcMethodImpl },
        { mcNDirect, mcNDirect | mdcMethodImpl }
        , { mcInstantiated, mcInstantiated | mdcMethodImpl }
    };

    DWORD Classification = classifications[type][impl];

    bmtTokenRangeNode *pTR = pTokenRanges;
    *pNumChunks = 0;
    while (pTR)
    {
        // Note: Since dwCurrentChunk isn't being used at this stage, we'll steal it to store
        // away the chunk count.
        // After this function, we'll set it to its intended value.
        pTR->dwCurrentChunk = MethodDescChunk::GetChunkCount(pTR->cMethods, Classification);
        (*pNumChunks) += pTR->dwCurrentChunk;
        pTR = pTR->pNext;
    }

    MethodDescChunk **pItfMDChunkList = (MethodDescChunk**)GetThread()->m_MarshalAlloc.Alloc((*pNumChunks) * sizeof(MethodDescChunk*));

    // Allocate the chunks for the method descs.
    pTR = pTokenRanges;
    DWORD chunkIdx = 0;
    while (pTR)
    {
        DWORD NumChunks = pTR->dwCurrentChunk;
        DWORD dwMDAllocs = pTR->cMethods;
        pTR->dwCurrentChunk = chunkIdx;
        for (DWORD i = 0; i < NumChunks; i++)
        {
            DWORD dwElems = min(dwMDAllocs, MethodDescChunk::GetMaxMethodDescs(Classification));
            MethodDescChunk *pChunk = MethodDescChunk::CreateChunk(bmtDomain->GetHighFrequencyHeap(),
                                                                   dwElems,
                                                                   Classification,
                                                                   pTR->tokenHiByte,
                                                                   NULL,
                                                                   pamTracker);

            pItfMDChunkList[chunkIdx++] = pChunk;
            dwMDAllocs -= dwElems;
        }
        pTR = pTR->pNext;
    }
    return pItfMDChunkList;

}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Determines the maximum size of the vtable and allocates the temporary storage arrays
// Also copies the parent's vtable into the working vtable.
//
VOID    MethodTableBuilder::AllocateWorkingSlotTables()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtDomain));
        PRECONDITION(CheckPointer(bmtMFDescs));
        PRECONDITION(CheckPointer(bmtMetaData));
        PRECONDITION(CheckPointer(bmtVT));
        PRECONDITION(CheckPointer(bmtEnumMF));
        PRECONDITION(CheckPointer(bmtInterface));
        PRECONDITION(CheckPointer(bmtFP));
        PRECONDITION(CheckPointer(bmtParent));

    }
    CONTRACTL_END;

    DWORD i;
    Thread *pThread = GetThread();

    // Allocate a MethodDesc* for each method (needed later when doing interfaces), and a FieldDesc* for each field
    bmtMFDescs->ppMethodDescList = (MethodDesc **) pThread->m_MarshalAlloc.Alloc(NumDeclaredMethods() * sizeof(MethodDesc *));
    ZeroMemory(bmtMFDescs->ppMethodDescList, NumDeclaredMethods() * sizeof(MethodDesc *));

    bmtMFDescs->ppFieldDescList = (FieldDesc **) pThread->m_MarshalAlloc.Alloc(bmtMetaData->cFields * sizeof(FieldDesc *));
    ZeroMemory(bmtMFDescs->ppFieldDescList, bmtMetaData->cFields * sizeof(FieldDesc *));

    // Create a temporary function table (we don't know how large the vtable will be until the very end,
    // since duplicated interfaces are stored at the end of it).  Calculate an upper bound.
    //
    // Upper bound is: The parent's class vtable size, plus every method declared in
    //                 this class, plus the size of every interface we implement
    //
    // In the case of value classes, we add # InstanceMethods again, since we have boxed and unboxed versions
    // of every vtable method.
    //
    if (IsValueClass())
    {
        bmtVT->dwMaxVtableSize += NumDeclaredMethods();
        bmtMFDescs->ppUnboxMethodDescList =
            (MethodDesc **) pThread->m_MarshalAlloc.Alloc(NumDeclaredMethods() * sizeof(MethodDesc*));
        ZeroMemory(bmtMFDescs->ppUnboxMethodDescList, NumDeclaredMethods() * sizeof(MethodDesc*));
    }

    // sanity check
    _ASSERTE(bmtParent->pParentMethodTable == NULL || (bmtInterface->dwInterfaceMapSize - bmtParent->pParentMethodTable->GetNumInterfaces()) >= 0);

    // add parent vtable size
    bmtVT->dwMaxVtableSize += bmtVT->dwCurrentVtableSlot;

    for (i = 0; i < bmtInterface->dwInterfaceMapSize; i++)
    {
        // We double the interface size because we may end up duplicating the Interface for MethodImpls
        bmtVT->dwMaxVtableSize += (bmtInterface->pInterfaceMap[i].m_pMethodTable->GetNumVirtuals() * 2);
    }

    // Allocate the temporary vtable
    bmtVT->pVtable = (SLOT *) pThread->m_MarshalAlloc.Alloc(bmtVT->dwMaxVtableSize * sizeof(SLOT));
    ZeroMemory(bmtVT->pVtable, bmtVT->dwMaxVtableSize * sizeof(SLOT));
    bmtVT->pVtableMD = (MethodDesc**) pThread->m_MarshalAlloc.Alloc(bmtVT->dwMaxVtableSize * sizeof(SLOT));
    ZeroMemory(bmtVT->pVtableMD, bmtVT->dwMaxVtableSize * sizeof(MethodDesc*));

    // Allocate the temporary non-vtable
    bmtVT->pNonVtableMD = (MethodDesc**) pThread->m_MarshalAlloc.Alloc(sizeof(MethodDesc*) * NumDeclaredMethods());
    ZeroMemory(bmtVT->pNonVtableMD, sizeof(MethodDesc*) * NumDeclaredMethods());

    if (bmtParent->pParentMethodTable != NULL)
    {
        if (bmtParent->pParentMethodTable->IsZapped())
        {

            for (unsigned j=0; j<bmtParent->pParentMethodTable->GetNumVirtuals(); j++)
            {
                bmtParent->pParentMethodTable->GetRestoredSlot(j);
            }
        }

        // Copy parent's vtable into our "temp" vtable
        {
            MethodTable::MethodIterator it(bmtParent->pParentMethodTable);
            for (;it.IsValid() && it.IsVirtual(); it.Next()) {                
                DWORD slot = it.GetSlotNumber();
                bmtVT->pVtable[slot] = (SLOT) it.GetTarget().GetTarget();
                bmtVT->pVtableMD[slot] = NULL; // MethodDescs are resolved lazily
            }
            bmtVT->pParentMethodTable = bmtParent->pParentMethodTable;
        }

    }

    if (NumDeclaredMethods() > 0)
    {
        bmtParent->ppParentMethodDescBuf = (MethodDesc **)
            pThread->m_MarshalAlloc.Alloc(2 * NumDeclaredMethods() *
                                          sizeof(MethodDesc*));

        bmtParent->ppParentMethodDescBufPtr = bmtParent->ppParentMethodDescBuf;
    }
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Allocate a MethodDesc* for each method (needed later when doing interfaces), and a FieldDesc* for each field
//
VOID    MethodTableBuilder::AllocateMethodFieldDescs(AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtDomain));
        PRECONDITION(CheckPointer(bmtMFDescs));
        PRECONDITION(CheckPointer(bmtMetaData));
        PRECONDITION(CheckPointer(bmtVT));
        PRECONDITION(CheckPointer(bmtEnumMF));
        PRECONDITION(CheckPointer(bmtFP));
        PRECONDITION(CheckPointer(bmtParent));

    }
    CONTRACTL_END;

    DWORD i;

    // We'll be counting the # fields of each size as we go along
    for (i = 0; i <= MAX_LOG2_PRIMITIVE_FIELD_SIZE; i++)
    {
        bmtFP->NumStaticFieldsOfSize[i]    = 0;
        bmtFP->NumInstanceFieldsOfSize[i]  = 0;
    }

    //
    // Allocate blocks of MethodDescs and FieldDescs for all declared methods and fields
    //
        // In order to avoid allocating a field pointing back to the method
        // table in every single method desc, we allocate memory in the
        // following manner:
        //   o  Field descs get a single contiguous block.
        //   o  Method descs of different sizes (normal vs NDirect) are
        //      allocated in different MethodDescChunks.
        //   o  Each method desc chunk starts with a header, and has
        //      at most MAX_ method descs (if there are more
        //      method descs of a given size, multiple chunks are allocated).
        // This way method descs can use an 8-bit offset field to locate the
        // pointer to their method table.
    //

        // Allocate fields first.
    if (NumDeclaredFields() > 0)
        {
            GetHalfBakedClass()->m_pFieldDescList = (FieldDesc *)pamTracker->Track(
            bmtDomain->GetHighFrequencyHeap()->AllocMem(NumDeclaredFields() *
                                                                   sizeof(FieldDesc)));
        INDEBUG(GetClassLoader()->m_dwDebugFieldDescs += NumDeclaredFields();)
        INDEBUG(GetClassLoader()->m_dwFieldDescData += (NumDeclaredFields() * sizeof(FieldDesc));)
        }
    else
    {
        // No fields or methods
        GetHalfBakedClass()->m_pFieldDescList = NULL;
    }

    if (NumDeclaredMethods() > 0)
    {
        for (DWORD impl=0; impl<METHOD_IMPL_COUNT; impl++)
        {
            for (DWORD type=0; type<METHOD_TYPE_COUNT; type++)
            {
                bmtMethodDescSet *set = &bmtMFDescs->sets[type][impl];

                DWORD dwAllocs = set->dwNumMethodDescs + set->dwNumBoxedEntryPointMDs;
                if (dwAllocs > 0)
                {
                    set->pChunkList = AllocateMDChunks(
                                                  bmtMetaData->ranges[type][impl],
                                                  type, impl,
                                                  &set->dwChunks,
                                                  pamTracker);
                }
#ifdef _DEBUG
                GetClassLoader()->m_dwDebugMethods += dwAllocs;
                for (UINT j=0; j<set->dwChunks; j++)
                    GetClassLoader()->m_dwMethodDescData += set->pChunkList[j]->Sizeof();
#endif // _DEBUG
            }
        }
    }
}

//*******************************************************************************
//
// Heuristic to determine if we should have instances of this class 8 byte aligned
//
BOOL MethodTableBuilder::ShouldAlign8(DWORD dwR8Fields, DWORD dwTotalFields)
{
    LEAF_CONTRACT;

    return dwR8Fields*2>dwTotalFields && dwR8Fields>=2;
}

//*******************************************************************************
BOOL MethodTableBuilder::IsSelfReferencingStaticValueTypeField(mdToken     dwByValueClassToken,
                                                               bmtInternalInfo* bmtInternal,
                                                               const bmtGenericsInfo *bmtGenerics,
                                                               PCCOR_SIGNATURE pMemberSignature,
                                                               DWORD       cMemberSignature)
{
    CONTRACTL
    {
        THROWS; // see note below as to why this is NOTHROW
        WRAPPER(GC_TRIGGERS);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;

    if (dwByValueClassToken != this->GetCl())
    {
        return FALSE;
    }

    if (!bmtGenerics->HasInstantiation())
    {
        return TRUE;
    }

    // <NOTE>See notes in InitializeFieldDescs as to why we bail out here </NOTE>

    if (bmtInternal->pModule->IsEditAndContinueEnabled() && GetThread() == NULL)
    {
        return FALSE;
    }

    // The value class is generic.  Check that the signature of the field
    // is _exactly_ equivalent to VC<!0, !1, !2, ...>.  Do this by consing up a fake
    // signature.
    DWORD nGenericArgs = bmtGenerics->GetNumGenericArgs();
    CONSISTENCY_CHECK(nGenericArgs != 0);
    unsigned int nFakeSig = 1 + 1 + 4 + 4 + (1+4) * nGenericArgs;

    BYTE *pFakeSigMem = (BYTE *) _alloca(nFakeSig);

    BYTE *pFakeSigMemMax = pFakeSigMem + nFakeSig;
    PCCOR_SIGNATURE pFieldSig = pMemberSignature + 1; // skip the CALLCONV_FIELD
    PCCOR_SIGNATURE pFakeSig = (PCCOR_SIGNATURE) pFakeSigMem;
    PCCOR_SIGNATURE pFakeSigMax = (PCCOR_SIGNATURE) pFakeSigMemMax;
    /* 1 */ pFakeSigMem += CorSigCompressElementTypeSafe(ELEMENT_TYPE_GENERICINST,pFakeSigMem, pFakeSigMemMax);
    /* 1 */ pFakeSigMem += CorSigCompressElementTypeSafe(ELEMENT_TYPE_VALUETYPE,pFakeSigMem, pFakeSigMemMax);
    /* 4 */ pFakeSigMem += CorSigCompressTokenSafe(dwByValueClassToken,pFakeSigMem, pFakeSigMemMax);
    /* max 4 */ pFakeSigMem += CorSigCompressDataSafe(nGenericArgs, pFakeSigMem, pFakeSigMemMax);
    for (unsigned int typearg = 0; typearg < nGenericArgs; typearg++)
    {
        /* 1 */ pFakeSigMem += CorSigCompressElementTypeSafe(ELEMENT_TYPE_VAR,pFakeSigMem, pFakeSigMemMax);
        /* max 4 */ pFakeSigMem += CorSigCompressDataSafe(typearg, pFakeSigMem, pFakeSigMemMax);
    }
    return MetaSig::CompareElementType(pFakeSig,             pFieldSig,
                                       pFakeSigMax,          pMemberSignature + cMemberSignature,
                                       bmtInternal->pModule, bmtInternal->pModule,
                                       NULL,                 NULL,
                                       NULL,                 NULL);

}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Go thru all fields and initialize their FieldDescs.
//
VOID    MethodTableBuilder::InitializeFieldDescs(BaseDomain *bmtDomain,
                                                 FieldDesc *pFieldDescList,
                                                 const LayoutRawFieldInfo* pLayoutRawFieldInfos,
                                                 bmtInternalInfo* bmtInternal,
                                                 const bmtGenericsInfo* bmtGenerics,
                                                 bmtMetaDataInfo* bmtMetaData,
                                                 bmtEnumMethAndFields* bmtEnumMF,
                                                 bmtErrorInfo* bmtError,
                                                 EEClass*** pByValueClassCache,
                                                 bmtMethAndFieldDescs* bmtMFDescs,
                                                 bmtFieldPlacement* bmtFP,
                                                 bmtThreadContextStaticInfo* pbmtTCSInfo,
                                                 unsigned* totalDeclaredSize,
                                                 bmtParentInfo* bmtParent)
{
    CONTRACTL
    {
        // InitializeFieldDescs/IsSelfReferencingStaticValueTypeField
        // should by rights be THROWS because
        //      (a) they call the loader and
        //      (b) IsSelfReferencingStaticValueTypeField calls signature comparison functions which in
        //           turn call the loader.
        // We could simply propagate the exceptions
        // if it weren't for the fact that InitializeFieldDescs is used by EnC to initialize
        // freshly added field descriptors, and thus it is called from the debugger thread
        // which cannot handle these exceptions.
        //
        // So further below we explicitly catch exceptions at the calls to the loader
        // functions (actually we call the non-throwing entry points to the loader),
        // and also avoid calling the loader at all when executing the EnC paths.
        THROWS;
        WRAPPER(GC_TRIGGERS);
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtInternal));
        PRECONDITION(CheckPointer(bmtGenerics));
        PRECONDITION(CheckPointer(bmtMetaData));
        PRECONDITION(CheckPointer(bmtEnumMF));
        PRECONDITION(CheckPointer(bmtError));
        PRECONDITION(CheckPointer(pByValueClassCache));
        PRECONDITION(CheckPointer(bmtMFDescs));
        PRECONDITION(CheckPointer(bmtFP));
        PRECONDITION(CheckPointer(totalDeclaredSize));
        PRECONDITION(CheckPointer(bmtParent, NULL_OK));
    }
    CONTRACTL_END;

    DWORD i;
    IMDInternalImport *pInternalImport = bmtInternal->pInternalImport; // to avoid multiple dereferencings

    FieldMarshaler *pNextFieldMarshaler = NULL;
    if (HasLayout())
    {
        pNextFieldMarshaler = (FieldMarshaler*)(GetLayoutInfo()->GetFieldMarshalers());
    }


//========================================================================
// BEGIN:
//    Go thru all fields and initialize their FieldDescs.
//========================================================================

    DWORD   dwCurrentDeclaredField = 0;
    DWORD   dwCurrentStaticField   = 0;
    DWORD   dwThreadStaticsOffset   = 0;
    DWORD   dwContextStaticsOffset  = 0;
    DWORD   dwR8Fields              = 0;        // Number of R8's the class has

#ifdef RVA_FIELD_VALIDATION_ENABLED
    Module* pMod = bmtInternal->pModule;
#endif
    for (i = 0; i < bmtMetaData->cFields; i++)
    {
        PCCOR_SIGNATURE pMemberSignature;
        DWORD       cMemberSignature;
        DWORD       dwMemberAttrs;

        dwMemberAttrs = bmtMetaData->pFieldAttrs[i];

        // We don't store static final primitive fields in the class layout
        if (IsFdLiteral(dwMemberAttrs))
            continue;

        if(!IsFdPublic(dwMemberAttrs))
            SetHasNonPublicFields();

        if (IsFdNotSerialized(dwMemberAttrs))
            SetCannotBeBlittedByObjectCloner();

        pMemberSignature = pInternalImport->GetSigOfFieldDef(bmtMetaData->pFields[i], &cMemberSignature);
        // Signature validation
        IfFailThrow(validateTokenSig(bmtMetaData->pFields[i],pMemberSignature,cMemberSignature,dwMemberAttrs,pInternalImport));

        FieldDesc * pFD;
        DWORD       dwLog2FieldSize = 0;
        BOOL        bCurrentFieldIsGCPointer = FALSE;
        mdToken     dwByValueClassToken = 0;
        EEClass *   pByValueClass = NULL;
        BOOL        fIsByValue = FALSE;
        BOOL        fIsThreadStatic = FALSE;
        BOOL        fIsContextStatic = FALSE;
        BOOL        fHasRVA = FALSE;

        MetaSig fsig(pMemberSignature, 
                     cMemberSignature, 
                     bmtInternal->pModule, 
                     &bmtGenerics->typeContext, 
                     FALSE, 
                     MetaSig::sigField);
        CorElementType ElementType = fsig.NextArg();


        // Get type
        if (!isCallConv(fsig.GetCallingConvention(), IMAGE_CEE_CS_CALLCONV_FIELD))
        {
            IfFailThrow(COR_E_TYPELOAD);
        }

        // Determine if a static field is special i.e. RVA based, local to
        // a thread or a context
        if(IsFdStatic(dwMemberAttrs))
        {
            if(IsFdHasFieldRVA(dwMemberAttrs))
            {
                fHasRVA = TRUE;
            }

            HRESULT hr;

            hr = pInternalImport->GetCustomAttributeByName(bmtMetaData->pFields[i],
                                                           g_ThreadStaticAttributeClassName,
                                                           NULL, NULL);
            IfFailThrow(hr);
            if (hr == S_OK)
            {
                fIsThreadStatic = TRUE;
            }

            hr = pInternalImport->GetCustomAttributeByName(bmtMetaData->pFields[i],
                                                           g_ContextStaticAttributeClassName,
                                                           NULL, NULL);
            IfFailThrow(hr);
            if (hr == S_OK)
            {
                fIsContextStatic = TRUE;
            }


            if(ElementType == ELEMENT_TYPE_VALUETYPE)
            {
                hr = pInternalImport->GetCustomAttributeByName(bmtMetaData->pFields[i],
                                                               g_CompilerServicesFixedAddressValueTypeAttribute,
                                                               NULL, NULL);
                IfFailThrow(hr);
                if (hr == S_OK)
                {
                    bmtFP->fHasFixedAddressValueTypes = true;
                }
            }


            // Do some sanity checks that we are not mixing context and thread
            // relative statics.
            if (fIsThreadStatic && fIsContextStatic)
            {
                IfFailThrow(COR_E_TYPELOAD);
            }
            if (fHasRVA && (fIsThreadStatic || fIsContextStatic))
            {
                IfFailThrow(COR_E_TYPELOAD);
            }
        }

    GOT_ELEMENT_TYPE:
        // Type to store in FieldDesc - we don't want to have extra case statements for
        // ELEMENT_TYPE_STRING, SDARRAY etc., so we convert all object types to CLASS.
        // Also, BOOLEAN, CHAR are converted to U1, I2.
        CorElementType FieldDescElementType = ElementType;

        switch (ElementType)
        {
        case ELEMENT_TYPE_I1:
        case ELEMENT_TYPE_U1:
        {
            dwLog2FieldSize = 0;
            break;
        }

        case ELEMENT_TYPE_I2:
        case ELEMENT_TYPE_U2:
        {
            dwLog2FieldSize = 1;
            break;
        }

        case ELEMENT_TYPE_I4:
        case ELEMENT_TYPE_U4:
        IN_WIN32(case ELEMENT_TYPE_I:)
        IN_WIN32(case ELEMENT_TYPE_U:)
        case ELEMENT_TYPE_R4:
            {
                dwLog2FieldSize = 2;
                break;
            }

        case ELEMENT_TYPE_BOOLEAN:
            {
                //                FieldDescElementType = ELEMENT_TYPE_U1;
                dwLog2FieldSize = 0;
                break;
            }

        case ELEMENT_TYPE_CHAR:
            {
                //                FieldDescElementType = ELEMENT_TYPE_U2;
                dwLog2FieldSize = 1;
                break;
            }

        case ELEMENT_TYPE_R8:
            dwR8Fields++;
            // Fall through

        case ELEMENT_TYPE_I8:
        case ELEMENT_TYPE_U8:
        IN_WIN64(case ELEMENT_TYPE_I:)
        IN_WIN64(case ELEMENT_TYPE_U:)
            {
               dwLog2FieldSize = 3;
                break;
            }

        case ELEMENT_TYPE_FNPTR:
        case ELEMENT_TYPE_PTR:   // ptrs are unmanaged scalars, for layout
            {
                dwLog2FieldSize = LOG2_PTRSIZE;
                break;
            }

        // Class type variable (method type variables aren't allowed in fields)
        // These only occur in open types used for verification/reflection.
        case ELEMENT_TYPE_VAR:
        case ELEMENT_TYPE_MVAR:
           // deliberate drop through - do fake field layout
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_SZARRAY:      // single dim, zero
        case ELEMENT_TYPE_ARRAY:        // all other arrays
        case ELEMENT_TYPE_CLASS: // objectrefs
        case ELEMENT_TYPE_OBJECT:
            {
                dwLog2FieldSize = LOG2_PTRSIZE;
                bCurrentFieldIsGCPointer = TRUE;
                FieldDescElementType = ELEMENT_TYPE_CLASS;

                if (IsFdStatic(dwMemberAttrs) == 0)
                {
                    SetHasFieldsWhichMustBeInited();
                    if (ElementType != ELEMENT_TYPE_STRING)
                        SetCannotBeBlittedByObjectCloner();
                }
                else
                {
                    // Increment the number of static fields that contain object references.
                    bmtEnumMF->dwNumStaticObjRefFields++;
                }
                break;
            }

        case ELEMENT_TYPE_VALUETYPE: // a byvalue class field
            {

                dwByValueClassToken = fsig.GetArgProps().PeekValueTypeTokenClosed(&bmtGenerics->typeContext);
                fIsByValue = TRUE;

                // By-value class
                BAD_FORMAT_NOTHROW_ASSERT(dwByValueClassToken != 0);

#ifndef RVA_FIELD_VALIDATION_ENABLED
                if (fHasRVA)
                    break;
#endif // !RVA_FIELD_VALIDATION_ENABLED

                if (this->IsValueClass())
                {

                    BOOL selfref = IsSelfReferencingStaticValueTypeField(dwByValueClassToken,
                                                                          bmtInternal,
                                                                          bmtGenerics,
                                                                          pMemberSignature,
                                                                          cMemberSignature);

                    if (selfref)
                    {
                        // immediately self-referential machines must be static.
                        if (!IsFdStatic(dwMemberAttrs))
                        {
                            bmtError->resIDWhy = IDS_CLASSLOAD_VALUEINSTANCEFIELD;
                            COMPlusThrowHR(COR_E_TYPELOAD);
                        }
                        pByValueClass = GetHalfBakedClass();
                    }
                    else
                    {

                        // We also check the TypeRef case, though this is in theory invalid IL (using a typeRef
                        // to refer to something defined in the same module) and no compilers should be producing it.
                        // <NICE> Get rid of this </NICE>

                        if (IsFdStatic(dwMemberAttrs) && (TypeFromToken(dwByValueClassToken) == mdtTypeRef))
                        {
                            // It's a typeref - check if it's a class that has a static field of itself
                            mdTypeDef ValueCL;

                            LPCUTF8 pszNameSpace;
                            LPCUTF8 pszClassName;
                            pInternalImport->GetNameOfTypeRef(dwByValueClassToken, &pszNameSpace, &pszClassName);
                            if(IsStrLongerThan((char*)pszClassName,MAX_CLASS_NAME)
                                || IsStrLongerThan((char*)pszNameSpace,MAX_CLASS_NAME)
                                || (strlen(pszClassName)+strlen(pszNameSpace)+1 >= MAX_CLASS_NAME))
                            {
                                COMPlusThrowHR(COR_E_TYPELOAD, BFA_TYPEREG_NAME_TOO_LONG);
                            }
                            mdToken tkRes = pInternalImport->GetResolutionScopeOfTypeRef(dwByValueClassToken);
                            if(TypeFromToken(tkRes) == mdtTypeRef)
                            {
                                DWORD rid = RidFromToken(tkRes);
                                if((rid==0)||(rid > pInternalImport->GetCountWithTokenKind(mdtTypeRef)))
                                {
                                    COMPlusThrowHR(COR_E_TYPELOAD, BFA_BAD_TYPEREF_TOKEN);
                                }
                            }
                            else tkRes = mdTokenNil;

                            if (SUCCEEDED(pInternalImport->FindTypeDef(pszNameSpace,
                                                                       pszClassName,
                                                                       tkRes,
                                                                       &ValueCL)))
                            {
                                if (ValueCL == GetCl())
                                        pByValueClass = GetHalfBakedClass();
                            }
                        } // If field is static typeref
                    } // If field is self-referencing
                } // If 'this' is a value class

                // It's not self-referential so try to load it
                if (pByValueClass == NULL)
                {

                    // <NOTE>See notes above on why this function is NOTHROW.
                    // We avoid the call to the loader here because EnC calls InitializeFieldDescs
                    // from the debugger thread, which is an unmanaged thread (GetThread() returns NULL).
                    // We can't typically invoke the loader from unmanaged threads.</NOTE>

                    if (bmtInternal->pModule->IsEditAndContinueEnabled() && GetThread() == NULL)
                    {
                        COMPlusThrowHR(E_FAIL);
                    }

                    // We load the approximate type of the field to avoid recursion problems.
                    // MethodTable::DoFullyLoad() will later load it fully
                    pByValueClass = fsig.GetArgProps().GetTypeHandleThrowing(bmtInternal->pModule,
                                                                             &bmtGenerics->typeContext,
                                                                             ClassLoader::LoadTypes,
                                                                             CLASS_LOAD_APPROXPARENTS,
                                                                             TRUE
                                                                             ).GetClass();

                }

                // IF it is an enum, strip it down to its underlying type

                if (pByValueClass->IsEnum())
                {
                    BAD_FORMAT_NOTHROW_ASSERT((pByValueClass == GetHalfBakedClass() && bmtEnumMF->dwNumInstanceFields == 1)
                             || pByValueClass->GetNumInstanceFields() == 1);      // enums must have exactly one field
                    FieldDesc* enumField = pByValueClass->m_pFieldDescList;
                    BAD_FORMAT_NOTHROW_ASSERT(!enumField->IsStatic());   // no real static fields on enums
                    ElementType = enumField->GetFieldType();
                    BAD_FORMAT_NOTHROW_ASSERT(ElementType != ELEMENT_TYPE_VALUETYPE);
                    fIsByValue = FALSE; // we're going to treat it as the underlying type now
                    goto GOT_ELEMENT_TYPE;
                }
                else if ( (pByValueClass->IsValueClass() == FALSE) &&
                          (pByValueClass != g_pEnumClass->GetClass()) )
                {
                    BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_MUST_BE_BYVAL, mdTokenNil);
                }

                // If it is an illegal type, say so
                if (pByValueClass->ContainsStackPtr())
                {
                    BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_FIELD, mdTokenNil);
                }

                // If a class has a field of type ValueType with non-public fields in it,
                // the class must "inherit" this characteristic
                if (pByValueClass->HasNonPublicFields())
                {
                    SetHasNonPublicFields();
                }

#ifdef RVA_FIELD_VALIDATION_ENABLED
                if (fHasRVA)
                {
                    dwLog2FieldSize = IsFdStatic(dwMemberAttrs) ? LOG2_PTRSIZE : 0;
                    break;
                }
#endif // RVA_FIELD_VALIDATION_ENABLED

                if (IsFdStatic(dwMemberAttrs) == 0)
                {
                    if (pByValueClass->HasFieldsWhichMustBeInited())
                        SetHasFieldsWhichMustBeInited();
                    if (pByValueClass->CannotBeBlittedByObjectCloner())
                        SetCannotBeBlittedByObjectCloner();
                }
                else
                {
                    // Increment the number of static fields that contain object references.
                    if (!IsFdHasFieldRVA(dwMemberAttrs))
                        bmtEnumMF->dwNumStaticBoxedFields++;
                }

                // Need to create by value class cache.  For E&C, this pointer will get
                // cached indefinately and not cleaned up as the parent descriptors are
                // in the low frequency heap.  Use new with the intent of leaking
                // this pointer and avoiding the assert                                  .
                if (*pByValueClassCache == NULL)
                {
                    *pByValueClassCache = new EEClass * [bmtEnumMF->dwNumInstanceFields + bmtEnumMF->dwNumStaticFields];

                    memset (*pByValueClassCache, 0, (bmtEnumMF->dwNumInstanceFields + bmtEnumMF->dwNumStaticFields) * sizeof(EEClass **));
                }

                // Static fields come after instance fields in this list
                if (IsFdStatic(dwMemberAttrs))
                {
                    if (!pByValueClass->HasInstantiation())
                    {
                        (*pByValueClassCache)[bmtEnumMF->dwNumInstanceFields + dwCurrentStaticField] = pByValueClass;
                    }
                    // make sure to record the correct size for static field
                    // layout
                    dwLog2FieldSize = LOG2_PTRSIZE; // handle
                }
                else
                {
                    (*pByValueClassCache)[dwCurrentDeclaredField] = pByValueClass;
                    dwLog2FieldSize = 0; // unused
                }

                break;
            }
        default:
            {
                BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_FIELD, mdTokenNil);
            }
        }


        // Static fields are not packed
        if (IsFdStatic(dwMemberAttrs) && (dwLog2FieldSize < 2))
            dwLog2FieldSize = 2;

        if (!IsFdStatic(dwMemberAttrs))
        {
            pFD = &pFieldDescList[dwCurrentDeclaredField];
            *totalDeclaredSize += (1 << dwLog2FieldSize);
        }
        else /* (dwMemberAttrs & mdStatic) */
        {
            pFD = &pFieldDescList[bmtEnumMF->dwNumInstanceFields + dwCurrentStaticField];
        }

        bmtMFDescs->ppFieldDescList[i] = pFD;

        const LayoutRawFieldInfo *pLayoutFieldInfo;

        pLayoutFieldInfo    = NULL;

        if (HasLayout())
        {
            const LayoutRawFieldInfo *pwalk = pLayoutRawFieldInfos;
            while (pwalk->m_MD != mdFieldDefNil)
            {
                if (pwalk->m_MD == bmtMetaData->pFields[i])
                {

                    pLayoutFieldInfo = pwalk;
                    CopyMemory(pNextFieldMarshaler,
                               &(pwalk->m_FieldMarshaler),
                               MAXFIELDMARSHALERSIZE);

                    pNextFieldMarshaler->SetFieldDesc(pFD);
                    pNextFieldMarshaler->SetExternalOffset(pwalk->m_offset);

                    ((BYTE*&)pNextFieldMarshaler) += MAXFIELDMARSHALERSIZE;
                    break;
                }
                pwalk++;
            }
        }

        LPCSTR pszFieldName = NULL;
#ifdef _DEBUG
        pszFieldName = pInternalImport->GetNameOfFieldDef(bmtMetaData->pFields[i]);
#endif
        // Initialize contents
        pFD->Init(
                  bmtMetaData->pFields[i],
                  FieldDescElementType,
                  dwMemberAttrs,
                  IsFdStatic(dwMemberAttrs),
                  fHasRVA,
                  fIsThreadStatic,
                  fIsContextStatic,
                  pszFieldName
                  );

        // Check if the ValueType field containing non-publics is overlapped
        if(HasExplicitFieldOffsetLayout()
            && pLayoutFieldInfo
            && pLayoutFieldInfo->m_fIsOverlapped
            && pByValueClass
            && pByValueClass->HasNonPublicFields())
        {
            if (!Security::CanSkipVerification(GetAssembly()->GetDomainAssembly()))
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_BADOVERLAP);
            }

            //SetHasNonVerifiablyOverLayedFields();
        }

        if (fIsByValue)
        {
            if (!IsFdStatic(dwMemberAttrs) &&
                (IsBlittable() || HasExplicitFieldOffsetLayout()))
            {
                pFD->m_pMTOfEnclosingClass =
                    (MethodTable *)(DWORD_PTR)(*pByValueClassCache)[dwCurrentDeclaredField]->GetNumInstanceFieldBytes();

                if (pLayoutFieldInfo)
                    IfFailThrow(pFD->SetOffset(pLayoutFieldInfo->m_offset));
                else
                    pFD->SetOffset(FIELD_OFFSET_VALUE_CLASS);
            }
            else if (!IsFdStatic(dwMemberAttrs) && IsManagedSequential())
            {
                pFD->m_pMTOfEnclosingClass =
                    (MethodTable *)(DWORD_PTR)(*pByValueClassCache)[dwCurrentDeclaredField]->GetNumInstanceFieldBytes();

                IfFailThrow(pFD->SetOffset(pLayoutFieldInfo->m_managedOffset));
            }
            else
            {
                // static value class fields hold a handle, which is ptr sized
                // (instance field layout ignores this value)
                pFD->m_pMTOfEnclosingClass = (MethodTable *) LOG2_PTRSIZE;
                pFD->SetOffset(FIELD_OFFSET_VALUE_CLASS);
            }
        }
        else
        {
            // Use the field's MethodTable to temporarily store the field's size
            pFD->m_pMTOfEnclosingClass = (MethodTable *)(size_t)dwLog2FieldSize;

            // -1 means that this field has not yet been placed
            // -2 means that this is a GC Pointer field not yet places
            if ((IsBlittable() || HasExplicitFieldOffsetLayout()) && !(IsFdStatic(dwMemberAttrs)))
                IfFailThrow(pFD->SetOffset(pLayoutFieldInfo->m_offset));
            else if (IsManagedSequential() && !(IsFdStatic(dwMemberAttrs)))
                IfFailThrow(pFD->SetOffset(pLayoutFieldInfo->m_managedOffset));
            else if (bCurrentFieldIsGCPointer)
                pFD->SetOffset(FIELD_OFFSET_UNPLACED_GC_PTR);
            else
                pFD->SetOffset(FIELD_OFFSET_UNPLACED);
        }

        if (!IsFdStatic(dwMemberAttrs))
        {
            if (!fIsByValue)
            {
                if (++bmtFP->NumInstanceFieldsOfSize[dwLog2FieldSize] == 1)
                    bmtFP->FirstInstanceFieldOfSize[dwLog2FieldSize] = dwCurrentDeclaredField;
            }

            dwCurrentDeclaredField++;

            if (bCurrentFieldIsGCPointer)
                bmtFP->NumInstanceGCPointerFields++;
        }
        else /* static fields */
        {

            // Static fields are stored in the vtable after the vtable and interface slots.  We don't
            // know how large the vtable will be, so we will have to fixup the slot number by
            // <vtable + interface size> later.
            dwCurrentStaticField++;
            if(fHasRVA)
            {
#ifdef RVA_FIELD_VALIDATION_ENABLED
                // Check if we place ObjectRefs into RVA field
                if((FieldDescElementType==ELEMENT_TYPE_CLASS)
                    ||((FieldDescElementType==ELEMENT_TYPE_VALUETYPE)
                        &&pByValueClass->HasFieldsWhichMustBeInited()))
                {
                    BAD_FORMAT_NOTHROW_ASSERT(!"ObjectRef in an RVA field");
                    BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_FIELD, mdTokenNil);
                }
                // Check if we place ValueType with non-public fields into RVA field
                if((FieldDescElementType==ELEMENT_TYPE_VALUETYPE)
                        &&pByValueClass->HasNonPublicFields())
                {
                    GCX_COOP();
                    if (!Security::CanHaveRVA(pFD, GetAssembly()))
                    {
                        BAD_FORMAT_NOTHROW_ASSERT(!"ValueType with non-public fields as a type of an RVA field");
                        BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_FIELD, mdTokenNil);
                    }
                }
#endif // RVA_FIELD_VALIDATION_ENABLED
                    // Set the field offset
                    DWORD rva;
                    IfFailThrow(pInternalImport->GetFieldRVA(pFD->GetMemberDef(), &rva));
#ifdef RVA_FIELD_VALIDATION_ENABLED
                    // Ensure that the IL image is loaded. Note that this assembly may
                    // have an ngen image, but this type may have failed to load during ngen.
                    pMod->GetFile()->LoadLibrary(FALSE);

                    DWORD fldSize = (FieldDescElementType == ELEMENT_TYPE_VALUETYPE) ?
                            pByValueClass->GetNumInstanceFieldBytes() :
                            GetSizeForCorElementType(FieldDescElementType);
                    if (!pMod->CheckRvaField(rva, fldSize))
                    {
                        GCX_COOP();
                        if (!Security::CanHaveRVA(pFD, GetAssembly()))

                        {
                            BAD_FORMAT_NOTHROW_ASSERT(!"Illegal RVA of a mapped field");
                            BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_FIELD, mdTokenNil);
                        }
                    }
#endif // RVA_FIELD_VALIDATION_ENABLED
                    IfFailThrow(pFD->SetOffsetRVA(rva));
#ifdef RVA_FIELD_OVERLAPPING_VALIDATION_ENABLED
                    // Check if the field overlaps with known RVA fields
                    BYTE*   pbModuleBase = pMod->GetILBase();
                    DWORD       dwSizeOfThisField = FieldDescElementType==ELEMENT_TYPE_VALUETYPE ?
                        pByValueClass->GetNumInstanceFieldBytes() : GetSizeForCorElementType(FieldDescElementType);
                    BYTE* FDfrom = pbModuleBase + pFD->GetOffset_NoLogging();
                    BYTE* FDto = FDfrom + dwSizeOfThisField;

                    ULONG j;
                    if(g_drRVAField)
                    {
                        for(j=1; j < g_ulNumRVAFields; j++)
                        {
                            if((*g_drRVAField)[j].pbStart >= FDto) continue;
                            if((*g_drRVAField)[j].pbEnd <= FDfrom) continue;
                        }
                    }
                    else
                    {
                        g_drRVAField = new (nothrow) DynamicArray<RVAFSE>;

                        if (g_drRVAField == NULL)
                            return (E_OUTOFMEMORY);
                    }

                    (*g_drRVAField)[g_ulNumRVAFields].pbStart = FDfrom;
                    (*g_drRVAField)[g_ulNumRVAFields].pbEnd = FDto;
                    g_ulNumRVAFields++;
#endif // RVA_FIELD_OVERLAPPING_VALIDATION_ENABLED
                    ;

            }
            else if (fIsThreadStatic)
            {
                DWORD size = 1 << dwLog2FieldSize;

#if defined(ALIGN_ACCESS)
                dwThreadStaticsOffset = (DWORD)ALIGN_UP(dwThreadStaticsOffset, size);
#endif

                IfFailThrow(pFD->SetOffset(dwThreadStaticsOffset)); // offset is the bucket index

                dwThreadStaticsOffset += size;
            }
            else if (fIsContextStatic)
            {
                DWORD size = 1 << dwLog2FieldSize;

#if defined(ALIGN_ACCESS)
                dwContextStaticsOffset = (DWORD)ALIGN_UP(dwContextStaticsOffset, size);
#endif

                IfFailThrow(pFD->SetOffset(dwContextStaticsOffset)); // offset is the bucket index

                dwContextStaticsOffset += size;
            }
            else
            {
                bmtFP->NumStaticFieldsOfSize[dwLog2FieldSize]++;

                if (bCurrentFieldIsGCPointer)
                    bmtFP->NumStaticGCPointerFields++;

                if (fIsByValue)
                    bmtFP->NumStaticGCBoxedFields++;
            }
        }
    }    

    EEClass *pParent = NULL;
    if (bmtParent)
        pParent = (bmtParent->pParentMethodTable) ? bmtParent->pParentMethodTable->GetClass() : NULL;
    else
    {
        pParent = GetParentClass();
    }

    DWORD dwNumInstanceFields = dwCurrentDeclaredField + (pParent ? pParent->m_wNumInstanceFields : 0);
    DWORD dwNumStaticFields = bmtEnumMF->dwNumStaticFields;
    if (dwNumInstanceFields != (WORD)dwNumInstanceFields || dwNumStaticFields != (WORD)dwNumStaticFields)
    {
        BuildMethodTableThrowException(IDS_EE_TOOMANYFIELDS);
    }
    GetHalfBakedClass()->SetNumInstanceFields((WORD)dwNumInstanceFields);
    GetHalfBakedClass()->SetNumStaticFields((WORD)dwNumStaticFields);

    if (ShouldAlign8(dwR8Fields, dwNumInstanceFields))
    {
        SetAlign8Candidate();
    }

    if (pbmtTCSInfo)
    {
        pbmtTCSInfo->dwThreadStaticsSize = dwThreadStaticsOffset;
        pbmtTCSInfo->dwContextStaticsSize = dwContextStaticsOffset;
    }

    //========================================================================
    // END:
    //    Go thru all fields and initialize their FieldDescs.
    //========================================================================

    return;

}

//*******************************************************************************
BOOL MethodTableBuilder::TestOverrideForAccessibility(Assembly *pParentAssembly,
                                                      Assembly *pChildAssembly,
                                                      DWORD     dwParentAttrs)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    BOOL isSameAssembly = (pChildAssembly == pParentAssembly);

    // AKA "strict bit". This means that overridability is tightly bound to accessibility.
    if (IsMdCheckAccessOnOverride(dwParentAttrs))
    {
        // Same Assembly
        if (isSameAssembly || pParentAssembly->GrantsFriendAccessTo(pChildAssembly))
        {
            // We are not allowed to override private members
            if ((dwParentAttrs & mdMemberAccessMask) <= mdPrivate)
            {
                return FALSE;
            }
        }
        // Cross-Assembly
        else
        {
            // If the method marks itself as check visibility the the method must be
            // public, FamORAssem, or family
            if((dwParentAttrs & mdMemberAccessMask) <= mdAssem)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

//*******************************************************************************
VOID MethodTableBuilder::TestOverRide(DWORD dwParentAttrs,
                                      DWORD dwMemberAttrs,
                                      Module *pModule,
                                      Module *pParentModule,
                                      mdToken method)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(IsMdVirtual(dwParentAttrs));
        PRECONDITION(IsMdVirtual(dwMemberAttrs));
    } CONTRACTL_END;

    Assembly *pAssembly = pModule->GetAssembly();
    Assembly *pParentAssembly = pParentModule->GetAssembly();

    BOOL isSameModule = (pModule == pParentModule);
    BOOL isSameAssembly = (pAssembly == pParentAssembly);

    // Virtual methods cannot be static
    if (IsMdStatic(dwMemberAttrs))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_STATICVIRTUAL, method);
    }

    if (!TestOverrideForAccessibility(pParentAssembly, pAssembly, dwParentAttrs))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_MI_ACCESS_FAILURE, method);
    }

    //
    // Refer to Partition II, 9.3.3 for more information on what is permitted.
    //

    enum WIDENING_STATUS
    {
        e_NO,       // NO
        e_YES,      // YES
        e_SA,       // YES, but only when same assembly
        e_NSA,      // YES, but only when NOT same assembly
        e_SM,       // YES, but only when same module
    };

    C_ASSERT(mdPrivateScope == 0x00);
    C_ASSERT(mdPrivate      == 0x01);
    C_ASSERT(mdFamANDAssem  == 0x02);
    C_ASSERT(mdAssem        == 0x03);
    C_ASSERT(mdFamily       == 0x04);
    C_ASSERT(mdFamORAssem   == 0x05);
    C_ASSERT(mdPublic       == 0x06);

    static const DWORD dwCount = mdPublic - mdPrivateScope + 1;
    static const WIDENING_STATUS rgWideningTable[dwCount][dwCount] =

    //               |        Base type
    // Subtype       |        mdPrivateScope  mdPrivate   mdFamANDAssem   mdAssem     mdFamily    mdFamORAssem    mdPublic
    // --------------+-------------------------------------------------------------------------------------------------------
    /*mdPrivateScope | */ { { e_SM,           e_NO,       e_NO,           e_NO,       e_NO,       e_NO,           e_NO    },
    /*mdPrivate      | */   { e_SM,           e_YES,      e_NO,           e_NO,       e_NO,       e_NO,           e_NO    },
    /*mdFamANDAssem  | */   { e_SM,           e_YES,      e_SA,           e_NO,       e_NO,       e_NO,           e_NO    },
    /*mdAssem        | */   { e_SM,           e_YES,      e_SA,           e_SA,       e_NO,       e_NO,           e_NO    },
    /*mdFamily       | */   { e_SM,           e_YES,      e_YES,          e_NO,       e_YES,      e_NSA,          e_NO    },
    /*mdFamORAssem   | */   { e_SM,           e_YES,      e_YES,          e_SA,       e_YES,      e_YES,          e_NO    },
    /*mdPublic       | */   { e_SM,           e_YES,      e_YES,          e_YES,      e_YES,      e_YES,          e_YES   } };

    DWORD idxParent = (dwParentAttrs & mdMemberAccessMask) - mdPrivateScope;
    DWORD idxMember = (dwMemberAttrs & mdMemberAccessMask) - mdPrivateScope;
    CONSISTENCY_CHECK(idxParent < dwCount);
    CONSISTENCY_CHECK(idxMember < dwCount);

    WIDENING_STATUS entry = rgWideningTable[idxMember][idxParent];

    if (entry == e_NO ||
        (entry == e_SA && !isSameAssembly && !pParentAssembly->GrantsFriendAccessTo(pAssembly)) ||
        (entry == e_NSA && isSameAssembly) ||
        (entry == e_SM && !isSameModule)
       )
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_REDUCEACCESS, method);
    }

    return;
}

//*******************************************************************************
VOID MethodTableBuilder::TestMethodImpl(Module *pDeclModule,
                                        Module *pImplModule,
                                        mdToken tokDecl,
                                        mdToken tokImpl)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(TypeFromToken(tokDecl) == mdtMethodDef);
        PRECONDITION(TypeFromToken(tokImpl) == mdtMethodDef);
        PRECONDITION(CheckPointer(pDeclModule));
        PRECONDITION(CheckPointer(pImplModule));
    }
    CONTRACTL_END

    BOOL isSameModule = pDeclModule->Equals(pImplModule);
    Assembly *pDeclAssembly = pDeclModule->GetAssembly();
    Assembly *pImplAssembly = pImplModule->GetAssembly();
    IMDInternalImport *pIMDDecl = pDeclModule->GetMDImport();
    IMDInternalImport *pIMDImpl = pImplModule->GetMDImport();
    DWORD dwDeclAttrs = pIMDDecl->GetMethodDefProps(tokDecl);
    DWORD dwImplAttrs = pIMDImpl->GetMethodDefProps(tokImpl);

    HRESULT hr = COR_E_TYPELOAD;

    if (!IsMdVirtual(dwDeclAttrs))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_MI_NONVIRTUAL_DECL);
    }
    if (!IsMdVirtual(dwImplAttrs))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_MI_MUSTBEVIRTUAL);
    }
    // Virtual methods cannot be static
    if (IsMdStatic(dwDeclAttrs))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_STATICVIRTUAL);
    }
    if (IsMdStatic(dwImplAttrs))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_STATICVIRTUAL);
    }
    if (IsMdFinal(dwDeclAttrs))
    {
        BuildMethodTableThrowException(IDS_CLASSLOAD_MI_FINAL_DECL);
    }

    // Since MethodImpl's do not affect the visibility of the Decl method, there's
    // no need to check.

    // If Decl's parent is other than this class, Decl must not be private
    mdTypeDef tkImplParent = mdTypeDefNil;
    mdTypeDef tkDeclParent = mdTypeDefNil;

    if (FAILED(hr = pIMDDecl->GetParentToken(tokDecl, &tkDeclParent)))
    {
        BuildMethodTableThrowException(hr, *bmtError);
    }
    if (FAILED(hr = pIMDImpl->GetParentToken(tokImpl, &tkImplParent)))
    {
        BuildMethodTableThrowException(hr, *bmtError);
    }

    // Make sure that we test for accessibility restrictions only if the decl is
    // not within our own type, as we are allowed to methodImpl a private with the
    // strict bit set if it is in our own type.
    if (!isSameModule || tkDeclParent != tkImplParent)
    {
        if (!TestOverrideForAccessibility(pDeclAssembly, pImplAssembly, dwDeclAttrs))
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_MI_ACCESS_FAILURE, tokImpl);
        }

        // Decl's parent must not be tdSealed
        mdToken tkGrandParentDummyVar;
        DWORD dwDeclTypeAttrs;
        pIMDDecl->GetTypeDefProps(tkDeclParent, &dwDeclTypeAttrs, &tkGrandParentDummyVar);
        if (IsTdSealed(dwDeclTypeAttrs))
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_MI_SEALED_DECL);
        }
    }

    return;
}

#if defined(_DEBUG) && !defined(STUB_DISPATCH_ALL)
//*******************************************************************************
//
// If a derived class is being created, and it does not override a virtual method
// from the parent class, it will use the same MethodDesc prestub as the parent.
// This functions tracks such inherited methods.
//
// Note that the tracking does not completely reflect the class hierarchy - if the
// method has already been jitted, the deriving class will use the jitted code directly
// instead of using the prestub
//
void    MethodTableBuilder::MarkInheritedVirtualMethods(MethodTable *childMT, MethodTable * parentMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(parentMT));
    }
    CONTRACTL_END
    CONSISTENCY_CHECK(CheckPointer(parentMT));

    unsigned parentVirtMethods = parentMT->GetNumVirtuals();

    // Walk all the methods in the parent's vtable

    for (unsigned i = 0; i < parentVirtMethods; i++)
    {
        // Does the childVtable use the same

        if (childMT->GetSlot(i) == parentMT->GetSlot(i))
        {
            MethodDesc* pMD = childMT->GetUnknownMethodDescForSlot(i);

            // The permitted prestub calls count is persisted in the ngen image.
            // We have to simulate the increments as if the method got jitted.
            // This is necesary since we are running code during ngen.
            pMD->IncPermittedPrestubCalls();

            if (!DoesSlotCallPrestub((BYTE *)childMT->GetSlot(i)))
                pMD->IncPrestubCalls();
        }
    }
}
#endif // defined(_DEBUG) && !defined(STUB_DISPATCH_ALL)

//*******************************************************************************
void MethodTableBuilder::SetSecurityFlagsOnMethod(MethodDesc* pParentMethodDesc,
                                                  MethodDesc* pNewMD,
                                                  mdToken tokMethod,
                                                  DWORD dwMemberAttrs,
                                                  bmtInternalInfo* bmtInternal,
                                                  bmtMetaDataInfo* bmtMetaData)
{
    if (!Security::IsSecurityOn())
        return;

    DWORD dwMethDeclFlags = 0;
    DWORD dwMethNullDeclFlags = 0;
    DWORD dwClassDeclFlags = 0xffffffff;
    DWORD dwClassNullDeclFlags = 0xffffffff;

    if ( IsMdHasSecurity(dwMemberAttrs) || IsTdHasSecurity(GetAttrClass()) || pNewMD->IsNDirect() )
    {
        // Disable inlining for any function which does runtime declarative
        // security actions.
        DWORD dwRuntimeSecurityFlags = (pNewMD->GetSecurityFlagsDuringClassLoad(bmtInternal->pInternalImport,
                                     tokMethod,
                                     GetCl(),
                                     &dwClassDeclFlags,
                                     &dwClassNullDeclFlags,
                                     &dwMethDeclFlags,
                                        &dwMethNullDeclFlags) & DECLSEC_RUNTIME_ACTIONS);
        if (dwRuntimeSecurityFlags)
        {
            // If we get here it means
            // - We have some "runtime" actions on this method. We dont care about "linktime" demands
            // - If this is a pinvoke method, then the unmanaged code access demand has not been suppressed
            pNewMD->SetNotInline(true);

            pNewMD->SetInterceptedForDeclSecurity(true);

            pNewMD->SetInterceptedForDeclSecurityCASDemandsOnly(
                MethodSecurityDescriptor::IsDeclSecurityCASDemandsOnly(dwRuntimeSecurityFlags, tokMethod, bmtInternal->pInternalImport ));

        }
    }

    if ( IsMdHasSecurity(dwMemberAttrs) )
    {
        // We only care about checks that are not empty...
        dwMethDeclFlags &= ~dwMethNullDeclFlags;

        if ( dwMethDeclFlags & (DECLSEC_LINK_CHECKS|DECLSEC_NONCAS_LINK_DEMANDS) )
        {
            pNewMD->SetRequiresLinktimeCheck();
        }

        if ( dwMethDeclFlags & (DECLSEC_INHERIT_CHECKS|DECLSEC_NONCAS_INHERITANCE) )
        {
            pNewMD->SetRequiresInheritanceCheck();
            if (IsInterface())
            {
                GetHalfBakedClass()->SetSomeMethodsRequireInheritanceCheck();
            }
        }
    }

    // Linktime checks on a method override those on a class.
    // If the method has an empty set of linktime checks,
    // then don't require linktime checking for this method.
    if ( RequiresLinktimeCheck() && !(dwMethNullDeclFlags & DECLSEC_LINK_CHECKS) )
    {
        pNewMD->SetRequiresLinktimeCheck();
    }

    if ( pParentMethodDesc != NULL &&
        (pParentMethodDesc->RequiresInheritanceCheck() ||
        pParentMethodDesc->ParentRequiresInheritanceCheck()) )
    {
        pNewMD->SetParentRequiresInheritanceCheck();
    }

    // Methods on an interface that includes an UnmanagedCode check
    // suppression attribute are assumed to be interop methods. We ask
    // for linktime checks on these.
    // Also place linktime checks on all P/Invoke calls.
    if ((IsInterface() &&
        (bmtInternal->pInternalImport->GetCustomAttributeByName(GetCl(),
                                                                COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                                NULL,
                                                                NULL) == S_OK ||
         bmtInternal->pInternalImport->GetCustomAttributeByName(pNewMD->GetMemberDef(),
                                                                COR_SUPPRESS_UNMANAGED_CODE_CHECK_ATTRIBUTE_ANSI,
                                                                NULL,
                                                                NULL) == S_OK) ) ||
        pNewMD->IsNDirect() ||
        (pNewMD->IsComPlusCall() && !IsInterface()))
    {
        pNewMD->SetRequiresLinktimeCheck();
    }

    // All public methods on public types will do a link demand of
    // full trust, unless AllowUntrustedCaller attribute is set
    if (
#ifdef _DEBUG
        g_pConfig->Do_AllowUntrustedCaller_Checks() &&
#endif
        !pNewMD->RequiresLinktimeCheck())
    {
        // If the method is public (visible outside it's assembly),
        // and the type is public and the assembly
        // is not marked with AllowUntrustedCaller attribute, do
        // a link demand for full trust on all callers note that
        // this won't be effective on virtual overrides. The caller
        // can allways do a virtual call on the base type / interface

        if (Security::MethodIsVisibleOutsideItsAssembly(
                dwMemberAttrs, GetAttrClass()))
        {
            _ASSERTE(GetClassLoader());
            _ASSERTE(GetAssembly());

            // See if the Assembly has AllowUntrustedCallerChecks CA
            // Pull this page in last

            if (!GetAssembly()->AllowUntrustedCaller())
                pNewMD->SetRequiresLinktimeCheck();
        }
    }

    // If it's a delegate BeginInvoke, we need to do a HostProtection check for synchronization
    if(IsAnyDelegateClass())
    {
        DelegateEEClass* pDelegateClass = (DelegateEEClass*)GetHalfBakedClass();
        if(pNewMD == pDelegateClass->m_pBeginInvokeMethod)
            pNewMD->SetRequiresLinktimeCheck();
    }
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Determine vtable placement for each member in this class
//
VOID MethodTableBuilder::PlaceMembers(DWORD numDeclaredInterfaces,
                                      BuildingInterfaceInfo_t *pBuildingInterfaceList,
                                      AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtInternal));
        PRECONDITION(CheckPointer(bmtMetaData));
        PRECONDITION(CheckPointer(bmtError));
        PRECONDITION(CheckPointer(bmtProp));
        PRECONDITION(CheckPointer(bmtInterface));
        PRECONDITION(CheckPointer(bmtParent));
        PRECONDITION(CheckPointer(bmtMFDescs));
        PRECONDITION(CheckPointer(bmtEnumMF));
        PRECONDITION(CheckPointer(bmtMethodImpl));
        PRECONDITION(CheckPointer(bmtVT));
    }
    CONTRACTL_END;

#ifdef _DEBUG
    LPCUTF8 pszDebugName,pszDebugNamespace;
    bmtInternal->pModule->GetMDImport()->GetNameOfTypeDef(GetCl(), &pszDebugName, &pszDebugNamespace);
#endif // _DEBUG

    HRESULT hr = S_OK;

    bmtVT->wCCtorSlot = MethodTable::NO_SLOT;
    bmtVT->wDefaultCtorSlot = MethodTable::NO_SLOT;

    DeclaredMethodIterator it(*this);
    while (it.Next())
    {
        PCCOR_SIGNATURE pMemberSignature = NULL;
        DWORD       cMemberSignature = 0;
        DWORD       dwParentAttrs;


        // for IL code that is implemented here must have a valid code RVA
        // this came up due to a linker bug where the ImplFlags/DescrOffset were
        // being set to null and we weren't coping with it
        if (it.RVA() == 0)
        {
            if((it.ImplFlags() == 0 || IsMiIL(it.ImplFlags()) || IsMiOPTIL(it.ImplFlags())) &&
               !IsMiRuntime(it.ImplFlags()) &&
               !IsMdAbstract(it.Attrs()) &&
               !IsReallyMdPinvokeImpl(it.Attrs()) &&
               !IsMiInternalCall(it.ImplFlags()) &&
               !(bmtInternal->pModule)->IsReflection() &&
               !(IsInterface() && !IsMdStatic(it.Attrs())) &&
               bmtDomain->IsExecutable())
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_MISSINGMETHODRVA, it.Token());
            }
        }
        else
        {
            if (!GetModule()->CheckIL(it.RVA()))
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_MISSINGMETHODRVA, it.Token());
            }
        }

        // If this member is a method which overrides a parent method, it will be set to non-NULL
        MethodDesc *pParentMethodDesc = NULL;

        BOOL        fIsInitMethod = FALSE;

        BOOL        fIsCCtor = FALSE;
        BOOL        fIsDefaultCtor = FALSE;

#ifdef _DEBUG
        if(GetHalfBakedClass()->m_fDebuggingClass && g_pConfig->ShouldBreakOnMethod(it.Name()))
            _ASSERTE(!"BreakOnMethodName");
#endif // _DEBUG

        // constructors and class initialisers are special
        if (IsMdRTSpecialName(it.Attrs()))
        {
            if (IsMdStatic(it.Attrs()))
            {
                // The only rtSpecialName static method allowed is the .cctor
                if(strcmp(it.Name(), COR_CCTOR_METHOD_NAME) != 0)
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
                }
                else
                {
                    // Validate that we have the correct signature for the .cctor
                    pMemberSignature = it.GetSig(&cMemberSignature);
                    PCCOR_SIGNATURE pbBinarySig;
                    ULONG           cbBinarySig;

                    // .cctor must return void, have default call conv, and have no args
                    unsigned cconv,nargs;
                    pbBinarySig = pMemberSignature;
                    cconv = CorSigUncompressData(pbBinarySig);
                    nargs = CorSigUncompressData(pbBinarySig);
                    if((*pbBinarySig != ELEMENT_TYPE_VOID)||(nargs!=0)||(cconv != IMAGE_CEE_CS_CALLCONV_DEFAULT))
                    {
                        BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
                    }
                    else
                    {
                        gsig_SM_RetVoid.GetBinarySig(&pbBinarySig, &cbBinarySig);

                        // No substitutions for type parameters as the method is static
                        if (MetaSig::CompareMethodSigs(pbBinarySig, cbBinarySig,
                                                       SystemDomain::SystemModule(), NULL,
                                                       pMemberSignature, cMemberSignature, bmtInternal->pModule, NULL))
                        {
                            fIsCCtor = TRUE;
                        }
                        else
                        {
                            BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
                        }
                    }
                }
            }
            else
            {
                // Verify the name for a constructor.
                if(strcmp(it.Name(), COR_CTOR_METHOD_NAME) != 0)
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
                }
                else
                {
                    // See if this is a default constructor.  If so, remember it for later.
                    pMemberSignature = it.GetSig(&cMemberSignature);
                    PCCOR_SIGNATURE pbBinarySig;
                    ULONG           cbBinarySig;
                    // .ctor must return void
                    pbBinarySig = pMemberSignature;
                    CorSigUncompressData(pbBinarySig); // get call conv out of the way
                    CorSigUncompressData(pbBinarySig); // get num args out of the way

                    if(*pbBinarySig != ELEMENT_TYPE_VOID)
                    {
                        BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
                    }
                    else
                    {
                        gsig_IM_RetVoid.GetBinarySig(&pbBinarySig, &cbBinarySig);

                        if (MetaSig::CompareMethodSigs(pbBinarySig, cbBinarySig,
                                                        SystemDomain::SystemModule(), NULL,
                                                        pMemberSignature, cMemberSignature, bmtInternal->pModule, NULL))
                            fIsDefaultCtor = TRUE;
                    }

                    fIsInitMethod = TRUE;
                }
            }
        }
        // The method does not have the special marking
        else
        {
            if (IsMdVirtual(it.Attrs()))
            {
                // Hash that a method with this name exists in this class
                // Note that ctors and static ctors are not added to the table
                DWORD dwHashName = HashStringA(it.Name());
                BOOL fMethodConstraintsMatch = FALSE;

                // If the member is marked with a new slot we do not need to find it
                // in the parent
                if (!IsMdNewSlot(it.Attrs()))
                {
                    // If we're not doing sanity checks, then assume that any method declared static
                    // does not attempt to override some virtual parent.
                    if (!IsMdStatic(it.Attrs()) && bmtParent->pParentMethodTable != NULL)
                    {
                        // Attempt to find the method with this name and signature in the parent class.
                        // This method may or may not create pParentMethodHash (if it does not already exist).
                        // It also may or may not fill in pMemberSignature/cMemberSignature.
                        // An error is only returned when we can not create the hash.
                        // NOTE: This operation touches metadata
                        {
                            HRESULT hrTmp;
                            hrTmp = LoaderFindMethodInClass(
                                                          it.Name(),
                                                          bmtInternal->pModule,
                                                          it.Token(),
                                                          &pParentMethodDesc,
                                                          &pMemberSignature, &cMemberSignature,
                                                          dwHashName, &fMethodConstraintsMatch);
                            if (FAILED(hrTmp))
                            {
                                BuildMethodTableThrowException(hrTmp, *bmtError);
                            }
                        }
                        if (pParentMethodDesc != NULL)
                        {
                            dwParentAttrs = pParentMethodDesc->GetAttrs();

                            if (!IsMdVirtual(dwParentAttrs))
                            {
                                BuildMethodTableThrowException(BFA_NONVIRT_NO_SEARCH, it.Token());
                            }

                            CONSISTENCY_CHECK(!fIsInitMethod);

                            // if we end up pointing at a slot that is final we are not allowed to override it.
                            if(IsMdFinal(dwParentAttrs))
                            {
                                BuildMethodTableThrowException(IDS_CLASSLOAD_MI_FINAL_DECL, it.Token());
                            }
                            else if(!bmtProp->fNoSanityChecks)
                            {
                                TestOverRide(dwParentAttrs, it.Attrs(), GetModule(), pParentMethodDesc->GetModule(), it.Token());
                            }

                            if (!fMethodConstraintsMatch)
                            {
                                BuildMethodTableThrowException(
                                        IDS_CLASSLOAD_CONSTRAINT_MISMATCH_ON_IMPLICIT_OVERRIDE,
                                        it.Token());
                            }

                            if (g_pConfig->ShouldRejectSafeHandleFinalizers() &&
                                bmtParent->pParentMethodTable->HasCriticalFinalizer() && 0 == strcmp("Finalize", it.Name()))
                            {
                                bool isSafeHandle = false;
                                // Is this a subclass of SafeHandle?
                                MethodTable * currMT = bmtParent->pParentMethodTable;
                                while(currMT != NULL)
                                {
                                    if (currMT == g_Mscorlib.FetchClass(CLASS__SAFE_HANDLE))
                                    {
                                        isSafeHandle = true;
                                        break;
                                    }
                                    currMT = currMT->GetParentMethodTable();
                                }

                                if (isSafeHandle)
                                {
                                    BuildMethodTableThrowException(IDS_CLASSLOAD_SH_SUBCLASS_FINALIZER, it.Token());
                                }
                            }
                        }
                    }
                }
            }
        }



        // Now we know the classification we can allocate the correct type of
        // method desc and perform any classification specific initialization.

        bmtTokenRangeNode *pTR = GetTokenRange(it.Token(),
                                               &(bmtMetaData->ranges[it.MethodType()][it.MethodImpl()])); // throws
        CONSISTENCY_CHECK(pTR->cMethods != 0);

        bmtMethodDescSet *set = &bmtMFDescs->sets[it.MethodType()][it.MethodImpl()];
        CONSISTENCY_CHECK(CheckPointer(set));

        MethodDescChunk *pChunk = set->pChunkList[pTR->dwCurrentChunk];

        // The MethodDesc we allocate for this method
        MethodDesc *pNewMD = pChunk->GetMethodDescAt(pTR->dwCurrentIndex);

        LPCSTR pName = it.Name();
        if (pName == NULL)
            pName = bmtInternal->pInternalImport->GetNameOfMethodDef(it.Token());

        // Update counters to prepare for next method desc allocation.
        pTR->dwCurrentIndex++;
        if (pTR->dwCurrentIndex == MethodDescChunk::GetMaxMethodDescs(it.Classification()))
        {
            pTR->dwCurrentChunk++;
            pTR->dwCurrentIndex = 0;
        }

#ifdef _DEBUG
        LPCUTF8 pszDebugMethodName = bmtInternal->pInternalImport->GetNameOfMethodDef(it.Token());
        size_t len = strlen(pszDebugMethodName) + 1;
        LPCUTF8 pszDebugMethodNameCopy = (char*) pamTracker->Track(bmtDomain->GetLowFrequencyHeap()->AllocMem(len));
        strcpy_s((char *) pszDebugMethodNameCopy, len, pszDebugMethodName);

#endif // _DEBUG

        // Do the init specific to each classification of MethodDesc & assign some common fields
        InitMethodDesc(bmtDomain,
                       pNewMD,
                       it.Classification(),
                       it.Token(),
                       it.ImplFlags(),
                       it.Attrs(),
                       FALSE,
                       it.RVA(),
                       bmtInternal->pInternalImport,
                       pName,
#ifdef _DEBUG
                       pszDebugMethodNameCopy,
                       GetDebugClassName(),
                       "", // FIX this happens on global methods, give better info
#endif // _DEBUG
                       pamTracker
                      );

        CONSISTENCY_CHECK(CheckPointer(bmtParent->ppParentMethodDescBufPtr));
        CONSISTENCY_CHECK(((bmtParent->ppParentMethodDescBufPtr - bmtParent->ppParentMethodDescBuf)
                           / sizeof(MethodDesc*)) < NumDeclaredMethods());
        *(bmtParent->ppParentMethodDescBufPtr++) = pParentMethodDesc;
        *(bmtParent->ppParentMethodDescBufPtr++) = pNewMD;


        // Declarative Security
        SetSecurityFlagsOnMethod(pParentMethodDesc, pNewMD, it.Token(), it.Attrs(), bmtInternal, bmtMetaData);

        it.SetMethodDesc(pNewMD);
        it.SetParentMethodDesc(pParentMethodDesc);

        // Make sure that fcalls have a 0 rva.  This is assumed by the prejit fixup logic
        if ((it.Classification() & ~mdcMethodImpl) == mcFCall && it.RVA() != 0)
        {
            BuildMethodTableThrowException(BFA_ECALLS_MUST_HAVE_ZERO_RVA, it.Token());
        }

        if (!IsMdVirtual(it.Attrs()))
        {
            // non-vtable method
            CONSISTENCY_CHECK(bmtVT->pNonVtableMD[bmtVT->dwCurrentNonVtableSlot] == NULL);

            bmtVT->pNonVtableMD[bmtVT->dwCurrentNonVtableSlot] = pNewMD; // Not prestub addr
            pNewMD->SetSlot((WORD) bmtVT->dwCurrentNonVtableSlot);

            if (fIsDefaultCtor)
                bmtVT->wDefaultCtorSlot = (WORD) bmtVT->dwCurrentNonVtableSlot;
            else if (fIsCCtor)
                bmtVT->wCCtorSlot = (WORD) bmtVT->dwCurrentNonVtableSlot;

            bmtVT->dwCurrentNonVtableSlot++;
        }
        else
        {
            pNewMD->SetSlot(MethodTable::NO_SLOT); // mark it initially as unplaced

            // vtable method
            if (IsInterface())
            {
                CONSISTENCY_CHECK(pParentMethodDesc == NULL);

                // if we're an interface, our slot number is fixed
                CONSISTENCY_CHECK(bmtVT->GetMethodDescForSlot(bmtVT->dwCurrentVtableSlot) == NULL);
                bmtVT->SetMethodDescForSlot(bmtVT->dwCurrentVtableSlot, pNewMD);
                pNewMD->SetSlot((WORD) bmtVT->dwCurrentVtableSlot);
                bmtVT->dwCurrentVtableSlot++;
            }
            else if (pParentMethodDesc != NULL)
            {
                WORD slotNumber = pParentMethodDesc->GetSlot();
                // No need for placeholder MDs with stub dispatch.
                CONSISTENCY_CHECK(!pParentMethodDesc->IsInterface());
                // we are overriding a parent method, so place this method now
                bmtVT->SetMethodDescForSlot(slotNumber, pNewMD);
                pNewMD->SetSlot(slotNumber);
                CONSISTENCY_CHECK(!bmtVT->pDispatchMapBuilder->Contains(
                    DispatchMapTypeID::ThisClassID(), pParentMethodDesc->GetSlot()));
                CONSISTENCY_CHECK(bmtParent->pParentMethodTable == NULL || slotNumber < bmtParent->pParentMethodTable->GetNumVirtuals());
                // We add the override entry to the mapping.
                bmtVT->pDispatchMapBuilder->InsertMDMapping(
                    DispatchMapTypeID::ThisClassID(),
                    pParentMethodDesc->GetSlot(),
                    pNewMD,
                    FALSE);
            }

            else
            {
                bmtVT->SetMethodDescForSlot(bmtVT->dwCurrentVtableSlot, pNewMD);
                pNewMD->SetSlot((WORD) bmtVT->dwCurrentVtableSlot);
                bmtVT->dwCurrentVtableSlot++;
            }

        }

        // If this method serves as the BODY of a MethodImpl specification, then
        // we should iterate all the MethodImpl's for this class and see just how many
        // of them this method participates in as the BODY.
        if(it.Classification() & mdcMethodImpl) {
            for(DWORD m = 0; m < bmtEnumMF->dwNumberMethodImpls; m++) {
                if(it.Token() == bmtMetaData->rgMethodImplTokens[m].methodBody) {
                    MethodDesc* pDeclMD = NULL;
                    BOOL fIsMethod;
                    mdToken mdDecl = bmtMetaData->rgMethodImplTokens[m].methodDecl;
                    DWORD dwDeclAttrs;
                    Substitution *pDeclSubst = &bmtMetaData->pMethodDeclSubsts[m];

                    // Get the parent
                    mdToken tkParent = mdTypeDefNil;
                    if (TypeFromToken(mdDecl) == mdtMethodDef || TypeFromToken(mdDecl) == mdtMemberRef)
                    {
                        if (FAILED(hr = bmtInternal->pInternalImport->GetParentToken(mdDecl,&tkParent)))
                        {
                            BuildMethodTableThrowException(hr, *bmtError);
                        }
                    }

                    // The DECL has been declared within the class
                    // that we're currently building.
                    if (GetCl() == tkParent)
                    {
                        hr = S_OK;

                        if(pThrowableAvailable(bmtError->pThrowable))
                            *(bmtError->pThrowable) = NULL;

                        if(TypeFromToken(mdDecl) != mdtMethodDef)
                        {
                            Module* pModule;
                            if (FAILED(hr = FindMethodDeclarationForMethodImpl(
                                                mdDecl, &mdDecl, FALSE, &pModule)))
                            {
                                BuildMethodTableThrowException(hr, *bmtError);
                            }
                            // Remember, the decl is in the same type as the impl.
                            CONSISTENCY_CHECK(pModule == bmtInternal->pModule);
                        }
                        dwDeclAttrs = bmtInternal->pInternalImport->GetMethodDefProps(mdDecl);
                    }
                    else
                    {
                        pDeclMD = (MethodDesc*) MemberLoader::GetDescFromMemberDefOrRefThrowing(
                                                       bmtInternal->pModule,
                                                       mdDecl,
                                                       &fIsMethod,
                                                       &bmtGenerics->typeContext, // type context
                                                       FALSE, // don't demand generic method args
                                                       0,
                                                       NULL,
                                                       FALSE /*allowInstParam*/,
                                                       CLASS_LOAD_APPROXPARENTS);

                        _ASSERTE(pDeclMD != NULL);

                        // We found a non-method, so throw.
                        if (!fIsMethod)
                        {
                            BuildMethodTableThrowException(IDS_CLASSLOAD_MI_DECLARATIONNOTFOUND, it.Token());
                        }
                        mdDecl = mdTokenNil;
                        dwDeclAttrs = pDeclMD->GetAttrs();
                    }

                    // Make sure the impl and decl are virtaul
                    if (!IsMdVirtual(dwDeclAttrs))
                    {
                        BuildMethodTableThrowException(IDS_CLASSLOAD_MI_MUSTBEVIRTUAL, it.Token());
                    }
                    if (!IsMdVirtual(it.Attrs()))
                    {
                        BuildMethodTableThrowException(IDS_CLASSLOAD_MI_VIRTUALMISMATCH, it.Token());
                    }

                    bmtMethodImpl->AddMethod(pNewMD,
                                             pDeclMD,
                                             mdDecl,
                                             pDeclSubst);
                }
            }
        }

        // check for proper use of the Managed and native flags
        if (IsMiManaged(it.ImplFlags()))
        {
            if (IsMiIL(it.ImplFlags()) || IsMiRuntime(it.ImplFlags())) // IsMiOPTIL(it.ImplFlags()) no longer supported
            {
                // No need to set code address, pre stub used automatically.
            }
            else
            {
                if (IsMiNative(it.ImplFlags()))
                {
                    // For now simply disallow managed native code if you turn this on you have to at least
                    // insure that we have SkipVerificationPermission or equivalent
                    BuildMethodTableThrowException(BFA_MANAGED_NATIVE_NYI, it.Token());
                }
                else
                {
                    BuildMethodTableThrowException(BFA_BAD_IMPL_FLAGS, it.Token());
                }
            }
        }
        else
        {
            if (IsMiNative(it.ImplFlags()) && IsGlobalClass())
            {
                // global function unmanaged entrypoint via IJW thunk was handled
                // above.
            }
            else
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_BAD_UNMANAGED_RVA, it.Token());
            }
            if (it.Classification() != mcNDirect)
            {
                BuildMethodTableThrowException(BFA_BAD_UNMANAGED_ENTRY_POINT);
            }
        }

        // Turn off inlining for any calls
        // that are marked in the metadata as not being inlineable.
        if(IsMiNoInlining(it.ImplFlags()))
        {
            pNewMD->SetNotInline(true);
        }

        // Vararg methods are not allowed inside generic classes
        // and nor can they be generic methods.
        if (bmtGenerics->GetNumGenericArgs() > 0 || ((it.Classification() & mdcClassification) == mcInstantiated) )
        {
            // We've been trying to avoid asking for the signature - now we need it
            if (pMemberSignature == NULL)
            {
                pMemberSignature = it.GetSig(&cMemberSignature);
            }

            if (MetaSig::IsVarArg(GetModule(), pMemberSignature))
            {
                BuildMethodTableThrowException(BFA_GENCODE_NOT_BE_VARARG);
            }
        }


    } /* end ... for each member */
}

//*******************************************************************************
// InitMethodDesc takes a pointer to space that's already allocated for the
// particular type of MethodDesc, and initializes based on the other info.
// This factors logic between PlaceMembers (the regular code path) & AddMethod
// (Edit & Continue (EnC) code path) so we don't have to maintain separate copies.
VOID MethodTableBuilder::InitMethodDesc(BaseDomain *bmtDomain,
                                        MethodDesc *pNewMD, // This is should actually be of the correct
                                        // sub-type, based on Classification
                                        DWORD Classification,
                                        mdToken tok,
                                        DWORD dwImplFlags,
                                        DWORD dwMemberAttrs,
                                        BOOL  fEnC,
                                        DWORD RVA,          // Only needed for NDirect case
                                        IMDInternalImport *pIMDII,  // Needed for NDirect, EEImpl(Delegate) cases
                                        LPCSTR pMethodName, // Only needed for mcEEImpl (Delegate) case
#ifdef _DEBUG
                                        LPCUTF8 pszDebugMethodName,
                                        LPCUTF8 pszDebugClassName,
                                        LPUTF8 pszDebugMethodSignature,
#endif // _DEBUG
                                        AllocMemTracker *pamTracker
                                       )
{
    CONTRACTL
    {
        THROWS;
        WRAPPER(GC_TRIGGERS);
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    LOG((LF_CORDB, LL_EVERYTHING, "EEC::IMD: pNewMD:0x%x for tok:0x%x (%s::%s)\n",
        pNewMD, tok, pszDebugClassName, pszDebugMethodName));

    // Now we know the classification we can perform any classification specific initialization.

    // The method desc is zero inited by the caller.

    switch (Classification & mdcClassification)
    {
    case mcNDirect:
        {
        // NDirect specific initialization.
        NDirectMethodDesc *pNewNMD = (NDirectMethodDesc*)pNewMD;

        // Allocate writeable data
        pNewNMD->ndirect.m_pWriteableData = (NDirectWriteableData*)
            pamTracker->Track(bmtDomain->GetHighFrequencyHeap()->AllocMem(sizeof(NDirectWriteableData)));

#ifdef HAS_NDIRECT_IMPORT_PRECODE
        pNewNMD->ndirect.m_pImportThunkGlue = Precode::Allocate(PRECODE_NDIRECT_IMPORT, pNewMD, FALSE,
            bmtDomain, pamTracker)->AsNDirectImportPrecode();
#else
        pNewNMD->GetNDirectImportThunkGlue()->Init(pNewNMD, bmtDomain);
#endif


        pNewNMD->GetWriteableData()->m_pNDirectTarget = pNewNMD->GetNDirectImportThunkGlue()->GetEntrypoint();
        }
        break;

    case mcFCall:
        break;

    case mcEEImpl:
        // For the Invoke method we will set a standard invoke method.
        BAD_FORMAT_NOTHROW_ASSERT(IsAnyDelegateClass());

        // For the asserts, either the pointer is NULL (since the class hasn't
        // been constructed yet), or we're in EnC mode, meaning that the class
        // does exist, but we may be re-assigning the field to point to an
        // updated MethodDesc

        // It is not allowed for EnC to replace one of the runtime builtin methods

        if (strcmp(pMethodName, "Invoke") == 0)
            {
            BAD_FORMAT_NOTHROW_ASSERT(NULL == ((DelegateEEClass*)GetHalfBakedClass())->m_pInvokeMethod);
            ((DelegateEEClass*)GetHalfBakedClass())->m_pInvokeMethod = pNewMD;
        }
        else if (strcmp(pMethodName, "BeginInvoke") == 0)
        {
            BAD_FORMAT_NOTHROW_ASSERT(NULL == ((DelegateEEClass*)GetHalfBakedClass())->m_pBeginInvokeMethod);
            ((DelegateEEClass*)GetHalfBakedClass())->m_pBeginInvokeMethod = pNewMD;
        }
        else if (strcmp(pMethodName, "EndInvoke") == 0)
        {
            BAD_FORMAT_NOTHROW_ASSERT(NULL == ((DelegateEEClass*)GetHalfBakedClass())->m_pEndInvokeMethod);
            ((DelegateEEClass*)GetHalfBakedClass())->m_pEndInvokeMethod = pNewMD;
        }
        else
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
        }

        // StoredSig specific intialization
        {
            StoredSigMethodDesc *pNewSMD = (StoredSigMethodDesc*) pNewMD;;
            DWORD cSig;
            PCCOR_SIGNATURE pSig = pIMDII->GetSigOfMethodDef(tok, &cSig);
            pNewSMD->SetStoredMethodSig(pSig, cSig);
        }
        break;

    case mcIL:
        break;

    case mcInstantiated:
        {
            // Initialize the typical instantiation.
            InstantiatedMethodDesc* pNewIMD = (InstantiatedMethodDesc*) pNewMD;
            pNewIMD->SetupGenericMethodDefinition(pIMDII,bmtDomain,GetModule(),
                                                    tok,IsTypicalTypeDefinition());
        }
        break;


    default:
        BAD_FORMAT_NOTHROW_ASSERT(!"Failed to set a method desc classification");
    }

    // Check the method desc's classification.
    _ASSERTE(pNewMD->GetClassification() == (Classification & mdcClassification));
    _ASSERTE(!pNewMD->IsMethodImpl() == !(Classification & mdcMethodImpl));

    pNewMD->SetMemberDef(tok);

    if (IsMdStatic(dwMemberAttrs))
        pNewMD->SetStatic();

    // Set suppress unmanaged code access permission attribute

    pNewMD->ComputeSuppressUnmanagedCodeAccessAttr(pIMDII);

#ifdef _DEBUG
    // Mark as many methods as synchronized as possible.
    //
    // Note that this can easily cause programs to deadlock, and that
    // should not be treated as a bug in the program.

    static ConfigDWORD stressSynchronized;
    DWORD stressSynchronizedVal = stressSynchronized.val(L"stressSynchronized", 0);

    bool isStressSynchronized =  stressSynchronizedVal &&
        pNewMD->IsIL() &&
        ((g_pValueTypeClass != NULL && g_pEnumClass != NULL &&
          !IsValueClass()) || // Can not synchronize on byref "this"
          IsMdStatic(dwMemberAttrs)) && // IsStatic() blows up in _DEBUG as pNewMD is not fully inited
        g_pObjectClass != NULL; // Ignore Object:* since "this" could be a boxed object

    // stressSynchronized=1 turns off the stress in the system domain to reduce
    // the chances of spurious deadlocks. Deadlocks in user code can still occur.
    // stressSynchronized=2 will probably cause more deadlocks, and is not recommended
    if (stressSynchronizedVal == 1 && GetAssembly()->IsSystem())
        isStressSynchronized = false;

    if (IsMiSynchronized(dwImplFlags) || isStressSynchronized)
#else // !_DEBUG
    if (IsMiSynchronized(dwImplFlags))
#endif // !_DEBUG
        pNewMD->SetSynchronized();


#ifdef _DEBUG
    pNewMD->m_pszDebugMethodName = (LPUTF8)pszDebugMethodName;
    pNewMD->m_pszDebugClassName  = (LPUTF8)pszDebugClassName;
    pNewMD->m_pDebugMethodTable  = GetHalfBakedMethodTable();

    if (pszDebugMethodSignature == NULL)
        pNewMD->m_pszDebugMethodSignature = FormatSig(pNewMD, bmtDomain, pamTracker);
    else
        pNewMD->m_pszDebugMethodSignature = pszDebugMethodSignature;
    pNewMD->InitPrestubCallChecking();
#endif // _DEBUG
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
VOID MethodTableBuilder::AddMethodImplDispatchMapping(
                        DispatchMapTypeID   typeID,
                        UINT32              slotNumber,
                        MethodDesc*         pMDImpl,
                        BOOL                fIsVirtual)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    // Look for an existing entry in the map.
    DispatchMapBuilder::Iterator it(bmtVT->pDispatchMapBuilder);
    if (bmtVT->pDispatchMapBuilder->Find(typeID, slotNumber, it))
    {
        // Throw if this entry has already previously been MethodImpl'd.
        if (it.IsMethodImpl())
        {
            // NOTE: This is where we check for duplicate overrides. This is the easiest place to check
            //       because duplicate overrides could in fact have separate MemberRefs to the same
            //       member and so just comparing tokens at the very start would not be enough.
            if (it.GetTargetMD() != pMDImpl)
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_MI_MULTIPLEOVERRIDES, pMDImpl->GetMemberDef());
            }
        }
        // This is the first MethodImpl. That's ok.
        else
        {
            it.SetTarget(pMDImpl);
            it.SetIsMethodImpl(TRUE);
        }
    }
    // A mapping for this interface method does not exist, so insert it.
    else
    {
        bmtVT->pDispatchMapBuilder->InsertMDMapping(
            typeID, slotNumber, pMDImpl, fIsVirtual, TRUE);
    }

    // Save the entry into the vtable as well, if it isn't an interface methodImpl
    if (typeID == DispatchMapTypeID::ThisClassID())
    {
        bmtVT->SetMethodDescForSlot(slotNumber, pMDImpl);
    }
}

//*******************************************************************************
VOID    MethodTableBuilder::MethodImplCompareSignatures(
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
                            DWORD               dwConstraintErrorCode)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(TypeFromToken(mdDecl) == mdtMethodDef);
        PRECONDITION(TypeFromToken(mdImpl) == mdtMethodDef);
    } CONTRACTL_END;

    // Get the signature for the IMPL, if unavailable
    if(*ppImplSignature == NULL)
    {
        *ppImplSignature = pImportImpl->GetSigOfMethodDef(mdImpl, pcImplSignature);
    }

    // Get the signature for the DECL
    PCCOR_SIGNATURE pDeclSignature = NULL;
    DWORD           cDeclSignature = 0;
    pDeclSignature = pImportDecl->GetSigOfMethodDef(mdDecl, &cDeclSignature);

    // Compare the signatures
    HRESULT hr = MetaSig::CompareMethodSigsNT(
                                   pDeclSignature,
                                   cDeclSignature,
                                   pModuleDecl,
                                   pSubstDecl,
                                   *ppImplSignature,
                                   *pcImplSignature,
                                   pModuleImpl,
                                   pSubstImpl);

    // S_FALSE means the comparison was successful, but the signatures do not match
    if (hr == S_FALSE)
    {
        hr = COR_E_TYPELOAD;
    }

    // Throw if the signatures do not match
    if(FAILED(hr))
    {
        LOG((LF_CLASSLOADER, LL_INFO1000, "BADSIG placing MethodImpl: %x\n", mdDecl));
        BuildMethodTableThrowException(hr, IDS_CLASSLOAD_MI_BADSIGNATURE, mdDecl);
    }

    //now compare the method constraints
    if (!MetaSig::CompareMethodConstraints(pModuleImpl,mdImpl, pSubstDecl,pModuleDecl,mdDecl))
    {
        BuildMethodTableThrowException(dwConstraintErrorCode, mdImpl);
    }
}

//*******************************************************************************
// We should have collected all the method impls. Cycle through them creating the method impl
// structure that holds the information about which slots are overridden.
VOID    MethodTableBuilder::PlaceMethodImpls(AllocMemTracker *pamTracker)

{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    HRESULT hr = S_OK;

    if(bmtMethodImpl->pIndex == 0)
        return;

    DWORD pIndex = 0;
    MethodDesc* next = bmtMethodImpl->GetBodyMethodDesc(pIndex);

    // Allocate some temporary storage. The number of overrides for a single method impl
    // cannot be greater then the number of vtable slots.
    DWORD* slots = (DWORD*) GetThread()->m_MarshalAlloc.Alloc((bmtVT->dwCurrentVtableSlot) * sizeof(DWORD));
    MethodDesc **replaced = (MethodDesc**) GetThread()->m_MarshalAlloc.Alloc((bmtVT->dwCurrentVtableSlot) * sizeof(MethodDesc*));

    while(next != NULL)
    {
        DWORD slotIndex = 0;
        MethodDesc* body;

        // The signature for the body of the method impl. We cache the signature until all
        // the method impl's using the same body are done.
        PCCOR_SIGNATURE pBodySignature = NULL;
        DWORD           cBodySignature = 0;

        // Get the MethodImpl storage
        CONSISTENCY_CHECK(next->IsMethodImpl());
        MethodImpl* pImpl = next->GetMethodImpl();

        // The impls are sorted according to the method descs for the body of the method impl.
        // Loop through the impls until the next body is found. When a single body
        // has been done move the slots implemented and method descs replaced into the storage
        // found on the body method desc.
        do { // collect information until we reach the next body
            body = next;

            // Get the declaration part of the method impl. It will either be a token
            // (declaration is on this type) or a method desc.
            MethodDesc* pDecl = bmtMethodImpl->GetDeclarationMethodDesc(pIndex);
            if(pDecl == NULL)
            {

                // The declaration is on this type to get the token.
                mdMethodDef mdef = bmtMethodImpl->GetDeclarationToken(pIndex);

                if (bmtMethodImpl->IsBody(mdef))
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_MI_MULTIPLEOVERRIDES,
                                                   mdef);
                }

                // Throws
                hr = PlaceLocalDeclaration(mdef,
                                           body,
                                           slots,             // Adds override to the slot and replaced arrays.
                                           replaced,
                                           &slotIndex,        // Increments count
                                           &pBodySignature,   // Fills in the signature
                                           &cBodySignature);
            }
            else
            {
                // Do not use pDecl->IsInterface here as that asks the method table and the MT may not yet be set up.
                if(pDecl->IsInterface())
                {
                    // Throws
                    hr = PlaceInterfaceDeclaration(pDecl,
                                                   body,
                                                   bmtMethodImpl->GetDeclarationSubst(pIndex),
                                                   slots,
                                                   replaced,
                                                   &slotIndex,        // Increments count
                                                   &pBodySignature,   // Fills in the signature
                                                   &cBodySignature);
                }
                else
                {
                    // Throws
                    hr = PlaceParentDeclaration(pDecl,
                                                body,
                                                bmtMethodImpl->GetDeclarationSubst(pIndex),
                                                slots,
                                                replaced,
                                                &slotIndex,        // Increments count
                                                &pBodySignature,   // Fills in the signature
                                                &cBodySignature);
                }
            }

            pIndex++;
            // we hit the end of the list so leave
            if(pIndex == bmtMethodImpl->pIndex)
                next = NULL;
            else
                next = bmtMethodImpl->GetBodyMethodDesc(pIndex);
        } while(next == body) ;

        // Use the number of overrides to
        // push information on to the method desc. We store the slots that
        // are overridden and the method desc that is replaced. That way
        // when derived classes need to determine if the method is to be
        // overridden then it can check the name against the replaced
        // method desc not the bodies name.
        if(slotIndex == 0)
        {
/*
            BuildMethodTableThrowException(IDS_CLASSLOAD_MI_DECLARATIONNOTFOUND, body->GetMemberDef());
*/
            body->ResetMethodImpl();
        }
        else
        {
            hr = S_OK;

            pImpl->SetSize(bmtDomain->GetHighFrequencyHeap(), pamTracker, slotIndex);

            // Gasp we do a bubble sort. Should change this to a qsort..
            for (DWORD i = 0; i < slotIndex; i++) {
                for (DWORD j = i+1; j < slotIndex; j++)
                {
                    if (slots[j] < slots[i])
                    {
                        MethodDesc* mTmp = replaced[i];
                        replaced[i] = replaced[j];
                        replaced[j] = mTmp;

                        DWORD sTmp = slots[i];
                        slots[i] = slots[j];
                        slots[j] = sTmp;
                    }
                }
            }

            // Go and set the method impl
            pImpl->SetData(slots, replaced);

        }
    }  // while(next != NULL)
}

//*******************************************************************************
HRESULT MethodTableBuilder::PlaceLocalDeclaration(mdMethodDef      mdef,
                                       MethodDesc*      pMDBody,
                                       DWORD*           slots,
                                       MethodDesc**     replaced,
                                       DWORD*           pSlotIndex,
                                       PCCOR_SIGNATURE* ppBodySignature,
                                       DWORD*           pcBodySignature)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(bmtVT->pDispatchMapBuilder));
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    ////////////////////////////////////////////////////////////////////////////////
    // First, find the method matching the token. Need to search for the MethodDesc
    // that corresponds to this token since we need to know what slot the token has
    // been assigned to for updating the vtable.

    MethodDesc *pMDDecl = NULL;

    {
        DeclaredMethodIterator methIt(*this);
        while (methIt.Next())
        {
            MethodDesc *pMD = methIt.GetMethodDesc();
            PREFIX_ASSUME(pMD != NULL);
            if ((pMD->GetMemberDef() == mdef))
            {
                pMDDecl = pMD;
                break;
            }
        }
    }

    PREFIX_ASSUME(pMDDecl != NULL);

    ///////////////////////////////
    // Verify the signatures match

    MethodImplCompareSignatures(
        pMDDecl->GetMemberDef(),
        bmtInternal->pInternalImport,
        bmtInternal->pModule,
        NULL,
        pMDBody->GetMemberDef(),
        bmtInternal->pInternalImport,
        bmtInternal->pModule,
        NULL,
        ppBodySignature,
        pcBodySignature,
        IDS_CLASSLOAD_CONSTRAINT_MISMATCH_ON_LOCAL_METHOD_IMPL);

    ///////////////////////////////
    // Validate the method impl.

    TestMethodImpl(
        bmtInternal->pModule,
        bmtInternal->pModule,
        pMDDecl->GetMemberDef(),
        pMDDecl->GetMemberDef());

    // Don't allow overrides for any of the four special runtime implemented delegate methods
    if (IsAnyDelegateClass())
    {
        IMDInternalImport *pMDInternalImport = bmtInternal->pInternalImport;
        _ASSERTE(mdef == pMDDecl->GetMemberDef());
        LPSTR strMethodName = (LPSTR)pMDInternalImport->GetNameOfMethodDef( mdef );

        if ((strcmp(strMethodName, COR_CTOR_METHOD_NAME) == 0) ||
            (strcmp(strMethodName, "Invoke")             == 0) || 
            (strcmp(strMethodName, "BeginInvoke")        == 0) || 
            (strcmp(strMethodName, "EndInvoke")          == 0)   )  
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_MI_CANNOT_OVERRIDE, mdef);
        }
    }

    ///////////////////
    // Add the mapping

    // Call helper to add it. Will throw if decl is already MethodImpl'd
    AddMethodImplDispatchMapping(DispatchMapTypeID::ThisClassID(),
                                 pMDDecl->GetSlot(),
                                 pMDBody,
                                 TRUE);

    // We implement this slot, record it
    slots[*pSlotIndex] = pMDDecl->GetSlot();
    replaced[*pSlotIndex] = pMDDecl;

    // increment the counter
    (*pSlotIndex)++;

    ////////////
    // Success!
    return S_OK;

}

//*******************************************************************************
HRESULT MethodTableBuilder::PlaceInterfaceDeclaration(MethodDesc*       pDeclMD,
                                                      MethodDesc*       pImplMD,
                                                      const Substitution *pDeclSubst,
                                                      DWORD*            slots,
                                                      MethodDesc**      replaced,
                                                      DWORD*            pSlotIndex,
                                                      PCCOR_SIGNATURE*  ppBodySignature,
                                                      DWORD*            pcBodySignature)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(return E_OUTOFMEMORY;);
        PRECONDITION(CheckPointer(pDeclMD));
        PRECONDITION(pDeclMD->IsInterface());
        PRECONDITION(CheckPointer(bmtVT->pDispatchMapBuilder));
    } CONTRACTL_END;


    MethodTable *pDeclMT = pDeclMD->GetMethodTable();

    //////////////////////////////////////////////////////////////
    // First make sure the interface is implemented by this class
    {
        // Iterate all the interfaces in the map to find a match
        BOOL fInterfaceFound = FALSE;
        for (UINT32 i = 0; i < bmtInterface->dwInterfaceMapSize && !fInterfaceFound; i++)
        {
            MethodTable *pInterface = bmtInterface->pInterfaceMap[i].m_pMethodTable;

            if (MetaSig::CompareTypeDefsUnderSubstitutions(pInterface,
                                                           pDeclMT,
                                                           bmtInterface->ppInterfaceSubstitutionChains[i],
                                                           pDeclSubst))
            {
                fInterfaceFound = TRUE;
                break;
            }
        }
        // Throw if this class does not implement the interface of pDecl.
        if (!fInterfaceFound)
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_MI_NOTIMPLEMENTED, pDeclMD->GetName());
        }
    }

    ///////////////////////////////
    // Verify the signatures match

    MethodImplCompareSignatures(
        pDeclMD->GetMemberDef(),
        pDeclMD->GetModule()->GetMDImport(),
        pDeclMD->GetModule(),
        pDeclSubst,
        pImplMD->GetMemberDef(),
        bmtInternal->pModule->GetMDImport(),
        bmtInternal->pModule,
        NULL,
        ppBodySignature,
        pcBodySignature,
        IDS_CLASSLOAD_CONSTRAINT_MISMATCH_ON_INTERFACE_METHOD_IMPL);

    ///////////////////////////////
    // Validate the method impl.

    TestMethodImpl(
        pDeclMD->GetModule(),
        bmtInternal->pModule,
        pDeclMD->GetMemberDef(),
        pImplMD->GetMemberDef());

    ///////////////////
    // Add the mapping

    DispatchMapTypeID dispatchMapTypeID =
        ComputeDispatchMapTypeID(pDeclMT, pDeclSubst);
    CONSISTENCY_CHECK(dispatchMapTypeID.IsImplementedInterface());

    // Call helper to add it. Will throw if decl is already MethodImpl'd
    AddMethodImplDispatchMapping(dispatchMapTypeID,
                                 pDeclMD->GetSlot(),
                                 pImplMD,
                                 TRUE);


    ////////////
    // Success!
    return S_OK;

}

//*******************************************************************************
HRESULT MethodTableBuilder::PlaceParentDeclaration(
                                        MethodDesc*       pMDDecl,
                                        MethodDesc*       pMDImpl,
                                        const Substitution *pDeclSubst,
                                        DWORD*            slots,
                                        MethodDesc**      replaced,
                                        DWORD*            pSlotIndex,
                                        PCCOR_SIGNATURE*  ppImplSignature,
                                        DWORD*            pcImplSignature)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(bmtVT->pDispatchMapBuilder));
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    MethodTable *pMTDecl = pMDDecl->GetMethodTable();

    // We're given a pMDDecl, but that could be for some parent way up which was overridden later on,
    // so we translate to the most current decl for this slot so we make sure to take into consideration
    // all the current attributes of the slot.
    MethodDesc *pMDParentImpl = bmtParent->pParentMethodTable->GetUnknownMethodDescForSlot(pMDDecl->GetSlot());
    pMDDecl = pMDParentImpl->GetDeclMethodDesc(pMDDecl->GetSlot());

    ////////////////////////////////////////////////////////////////
    // Verify that the class of the declaration is in our heirarchy

    {
        MethodTable* pParent = bmtParent->pParentMethodTable;
        Substitution* pParentSubst = &bmtParent->parentSubst;
        Substitution* newSubst = NULL;
        while(pParent != NULL) {
            _ASSERT(pParent != NULL);
            if (MetaSig::CompareTypeDefsUnderSubstitutions(pParent,
                                                           pMTDecl,
                                                           pParentSubst,
                                                           pDeclSubst))
            {
                break;
            }

            // Move on to the next parent...
            newSubst = new Substitution;
            *newSubst = pParent->GetClass()->GetSubstitutionForParent(pParentSubst);
            pParentSubst = newSubst;
            pParent = pParent->GetParentMethodTable();
        }
        if(newSubst != NULL) // there was at least 1 allocation
        {
            for(newSubst = pParentSubst;newSubst->GetNext()!=&bmtParent->parentSubst;
                                        newSubst=(Substitution*)(newSubst->GetNext()));
            memset(newSubst,0,sizeof(Substitution));  // destroy link to bmtParent->parentSubst
            pParentSubst->DeleteChain();  // delete all chain up to and including newSubst
        }

        if(pParent == NULL) {
            BuildMethodTableThrowException(IDS_CLASSLOAD_MI_NOTIMPLEMENTED,
                                           pMDDecl->GetName());
        }
    }


    /////////////////////////////////////////
    // Verify that the signatures match

    MethodImplCompareSignatures(
        pMDDecl->GetMemberDef(),
        pMDDecl->GetModule()->GetMDImport(),
        pMDDecl->GetModule(),
        pDeclSubst,
        pMDImpl->GetMemberDef(),
        bmtInternal->pInternalImport,
        bmtInternal->pModule,
        NULL,
        ppImplSignature,
        pcImplSignature,
        IDS_CLASSLOAD_CONSTRAINT_MISMATCH_ON_PARENT_METHOD_IMPL);

    ////////////////////////////////
    // Verify rules of method impls

    TestMethodImpl(
        pMDDecl->GetModule(),
        bmtInternal->pModule,
        pMDDecl->GetMemberDef(),
        pMDImpl->GetMemberDef());

    ///////////////////
    // Add the mapping

    // Call helper to add it. Will throw if DECL is already MethodImpl'd
    AddMethodImplDispatchMapping(DispatchMapTypeID::ThisClassID(),
                                 pMDDecl->GetSlot(),
                                 pMDImpl,
                                 TRUE);

    // We implement this slot, record it
    slots[*pSlotIndex] = pMDDecl->GetSlot();
    replaced[*pSlotIndex] = pMDDecl;

    // increment the counter
    (*pSlotIndex)++;

    ////////////
    // Success!
    return S_OK;


}

//*******************************************************************************
// This will validate that all interface methods that were matched during
// layout also validate against type constraints.

VOID MethodTableBuilder::ValidateInterfaceMethodConstraints()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    DispatchMapBuilder::Iterator it(bmtVT->pDispatchMapBuilder);
    while (it.IsValid())
    {
        if (it.GetTypeID() != DispatchMapTypeID::ThisClassID())
        {
            InterfaceInfo_t *pItfInfo = &bmtInterface->pInterfaceMap[it.GetTypeID().GetInterfaceNum()];

            // Grab the substitution for this interface
            Substitution *pSubst = bmtInterface->ppInterfaceSubstitutionChains[it.GetTypeID().GetInterfaceNum()];

            // Grab the method token
            MethodTable *pMTItf = pItfInfo->m_pMethodTable;
            CONSISTENCY_CHECK(CheckPointer(pMTItf->GetMethodDescForSlot(it.GetSlotNumber())));
            mdMethodDef mdTok = pItfInfo->m_pMethodTable->GetMethodDescForSlot(it.GetSlotNumber())->GetMemberDef();

            // Default to the current module. The code immediately below determines if this
            // assumption is incorrect.
            Module *          pTargetModule          = bmtInternal->pModule;

            // Get the module of the target method. Get it through the chunk to
            // avoid triggering the assert that MethodTable is non-NULL. It may
            // be null since it may belong to the type we're building right now.
            MethodDesc *      pTargetMD              = it.GetTargetMD();
            MethodDescChunk * pTargetChunk           = pTargetMD->GetMethodDescChunk();
            MethodTable *     pTargetMT              = pTargetChunk->GetMethodTable();

            // If pTargetMT is null, this indicates that the target MethodDesc belongs
            // to the current type. Otherwise, the MethodDesc MUST be owned by a parent
            // of the type we're building.
            BOOL              fTargetIsOwnedByParent = pTargetMT != NULL;

            // If the method is owned by a parent, we need to use the parent's module
            if (fTargetIsOwnedByParent)
            {
                pTargetModule = pTargetMT->GetModule();
            }

            // Now compare the method constraints.
            if (!MetaSig::CompareMethodConstraints(pTargetModule,
                                                   pTargetMD->GetMemberDef(),
                                                   pSubst,
                                                   pMTItf->GetModule(),
                                                   mdTok))
            {
                LOG((LF_CLASSLOADER, LL_INFO1000,
                     "BADCONSTRAINTS on interface method implementation: %x\n", pTargetMD));
                // This exception will be due to an implicit implementation, since explicit errors
                // will be detected in MethodImplCompareSignatures (for now, anyway).
                CONSISTENCY_CHECK(!it.IsMethodImpl());
                DWORD idsError = it.IsMethodImpl() ?
                                 IDS_CLASSLOAD_CONSTRAINT_MISMATCH_ON_INTERFACE_METHOD_IMPL :
                                 IDS_CLASSLOAD_CONSTRAINT_MISMATCH_ON_IMPLICIT_IMPLEMENTATION;
                if (fTargetIsOwnedByParent)
                {
                    DefineFullyQualifiedNameForClass();
                    LPCUTF8 szClassName = GetFullyQualifiedNameForClassNestedAware(pTargetMD->GetClass());
                    LPCUTF8 szMethodName = pTargetMD->GetName();

                    CQuickBytes qb;
                    // allocate enough room for "<class>.<method>\0"
                    size_t cchFullName = strlen(szClassName) + 1 + strlen(szMethodName) + 1;
                    LPUTF8 szFullName = (LPUTF8) qb.AllocThrows(cchFullName);
                    strcpy_s(szFullName, cchFullName, szClassName);
                    strcat_s(szFullName, cchFullName, ".");
                    strcat_s(szFullName, cchFullName, szMethodName);
                    
                    BuildMethodTableThrowException(idsError, szFullName);
                }
                else
                {
                    BuildMethodTableThrowException(idsError, pTargetMD->GetMemberDef());
                }
            }
        }

        // Move to the next entry
        it.Next();
    }
}

//
// Used by BuildMethodTable
//
// If we're a value class, we want to create duplicate slots and MethodDescs for all methods in the vtable
// section (i.e. not non-virtual instance methods or statics).
// In the name of uniformity it would be much nicer
// if we created _all_ value class BoxedEntryPointStubs at this point.
// However, non-virtual instance methods only require unboxing
// stubs in the rare case that we create a delegate to such a
// method, and thus it would be inefficient to create them on
// loading: after all typical structs will have many non-virtual
// instance methods.
//
// Unboxing stubs for non-virtual instance methods are created
// in MethodDesc::FindOrCreateAssociatedMethodDesc.

VOID MethodTableBuilder::ChangeValueClassVirtualsToBoxedEntryPointsAndCreateUnboxedEntryPoints(AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // If we're a value class, we want to create duplicate slots and MethodDescs for all methods in the vtable
    // section (i.e. not privates or statics).

    if (IsValueClass())
    {
        DeclaredMethodIterator it(*this);
        while (it.Next())
        {
            MethodDesc *pMD;

            pMD = it.GetMethodDesc();
            if (pMD == NULL)
                continue;

            if (IsMdStatic(it.Attrs()) ||
                !IsMdVirtual(it.Attrs()) ||
                (it.Classification() & mdcClassification) == mcInstantiated ||
                IsMdRTSpecialName(it.Attrs()))
                continue;

            bmtTokenRangeNode *pTR = GetTokenRange(it.Token(),
                                                   &(bmtMetaData->ranges[it.MethodType()][it.MethodImpl()]));

            bmtMethodDescSet *set = &bmtMFDescs->sets[it.MethodType()][it.MethodImpl()];

            MethodDescChunk * pChunk = set->pChunkList[pTR->dwCurrentChunk];

            MethodDesc *pNewMD = pChunk->GetMethodDescAt(pTR->dwCurrentIndex);

            // <NICE> memcpy operations on data structures like MethodDescs are extremely fragile
            // and should not be used.  We should go to the effort of having proper constructors
            // in the MethodDesc class. </NICE>
            memcpy(pNewMD, pMD, pChunk->GetMethodDescSize() - METHOD_PREPAD);

            // Reset the chunk index
            pNewMD->SetChunkIndex(pChunk, pTR->dwCurrentIndex);

            pNewMD->SetMemberDef(pMD->GetMemberDef());

            // Update counters to prepare for next method desc allocation.
            pTR->dwCurrentIndex++;
            if (pTR->dwCurrentIndex == MethodDescChunk::GetMaxMethodDescs(it.Classification()))
            {
                pTR->dwCurrentChunk++;
                pTR->dwCurrentIndex = 0;
            }

            bmtMFDescs->ppUnboxMethodDescList[it.CurrentIndex()] = pNewMD;

            pNewMD->SetSlot((WORD) bmtVT->dwCurrentNonVtableSlot);

            // Change the original MD in the vtable section
            // to be a stub method which takes a BOXed this pointer.
            pMD->SetIsUnboxingStub();

            bmtVT->pNonVtableMD[ bmtVT->dwCurrentNonVtableSlot ] = pNewMD; // not pre-stub addr, refer to statics above
            bmtVT->dwCurrentNonVtableSlot++;
        }
    }

}

//*******************************************************************************
//
// Used by BuildMethodTable
//
//
// If we are a class, then there may be some unplaced vtable methods (which are by definition
// interface methods, otherwise they'd already have been placed).  Place as many unplaced methods
// as possible, in the order preferred by interfaces.  However, do not allow any duplicates - once
// a method has been placed, it cannot be placed again - if we are unable to neatly place an interface,
// create duplicate slots for it starting at dwCurrentDuplicateVtableSlot.  Fill out the interface
// map for all interfaces as they are placed.
//
// If we are an interface, then all methods are already placed.  Fill out the interface map for
// interfaces as they are placed.
//
// BEHAVIOUR (based on Partition II: 11.2, not including MethodImpls)
//   C is current class, P is a parent class, I is the interface being implemented
//
//   FOREACH interface I implemented by this class C
//     FOREACH method I::M
//       IF I is EXPLICITLY implemented by C
//         IF some method C::M matches I::M
//           USE C::M as implementation for I::M
//         ELIF we inherit a method P::M that matches I::M
//           USE P::M as implementation for I::M
//         ENDIF
//       ELSE
//         IF I::M lacks implementation
//           IF some method C::M matches I::M
//             USE C::M as implementation for I::M
//           ELIF we inherit a method P::M that matches I::M
//             USE P::M as implementation for I::M
//           ENDIF
//         ENDIF
//       ENDIF
//     ENDFOR
//   ENDFOR
//

VOID    MethodTableBuilder::PlaceVtableMethods(
                                    DWORD numDeclaredInterfaces,
                                    BuildingInterfaceInfo_t *pBuildingInterfaceList)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    BOOL fParentInterface;

    for (DWORD dwCurInterface = 0;
         dwCurInterface < bmtInterface->dwInterfaceMapSize;
         dwCurInterface++)
    {
        // Default to not being implemented by the current class
        fParentInterface = FALSE;

        // Keep track of the current interface
        InterfaceInfo_t *pCurItfInfo = &(bmtInterface->pInterfaceMap[dwCurInterface]);
        // The interface we are attempting to place
        MethodTable* pInterface = pCurItfInfo->m_pMethodTable;
        Substitution* pIntfSubst = bmtInterface->ppInterfaceSubstitutionChains[dwCurInterface];

        // Check if this type is allowed to implement this interface.
        {
            Assembly *pItfAssembly = pInterface->GetAssembly();
            Assembly *pThisAssembly = bmtInternal->pModule->GetAssembly();
            if(pCurItfInfo->IsDeclaredOnClass() &&
               !pInterface->IsExternallyVisible() &&
               pItfAssembly != pThisAssembly &&
               !pItfAssembly->GrantsFriendAccessTo(pThisAssembly))
            {
                BuildMethodTableThrowException(IDS_CLASSLOAD_INTERFACE_NO_ACCESS);
            }
        }

        if (pCurItfInfo->IsImplementedByParent())
        {
            if (!pCurItfInfo->IsDeclaredOnClass())
            {
                fParentInterface = TRUE;
            }
        }

        // For each method declared in this interface
        MethodTable::MethodIterator it(pInterface);
        MethodTable::MethodDataWrapper hParentData;
        if (!pCurItfInfo->IsDeclaredOnClass())
        {
            CONSISTENCY_CHECK(CheckPointer(bmtParent->pParentMethodTable));
            // NOTE: This override does not cache the resulting MethodData object
            hParentData = MethodTable::GetMethodData(
                ComputeDispatchMapTypeID(pInterface, pIntfSubst),
                pInterface,
                bmtParent->pParentMethodTable);
        }

        for (;it.IsValid() && it.IsVirtual(); it.Next())
        {
            MethodDesc **ppImplementingMD = &bmtInterface->ppInterfaceMethodDescList[it.GetSlotNumber()];
            MethodDesc **ppDeclaringMD = &bmtInterface->ppInterfaceDeclMethodDescList[it.GetSlotNumber()];

            // Provide default values
            *ppImplementingMD = NULL;
            *ppDeclaringMD = NULL;

            // See if we have info gathered while placing members
            if (bmtInterface->pppInterfaceImplementingMD[dwCurInterface] && bmtInterface->pppInterfaceImplementingMD[dwCurInterface][it.GetSlotNumber()] != NULL)
            {
                *ppImplementingMD = bmtInterface->pppInterfaceImplementingMD[dwCurInterface][it.GetSlotNumber()];
                *ppDeclaringMD = bmtInterface->pppInterfaceDeclaringMD[dwCurInterface][it.GetSlotNumber()];
                continue;
            }

            MethodDesc *pItfMD =  it.GetDeclMethodDesc();
            CONSISTENCY_CHECK(CheckPointer(pItfMD));

            if (!pCurItfInfo->IsDeclaredOnClass())
            {
                if (!hParentData->GetImplSlot(it.GetSlotNumber()).IsNull())
                {
                    // If this interface is not explicitly declared on this class, and the interface slot has already been
                    // given an implementation, then the only way to provide a new implementation is through an override
                    // or through a MethodImpl.
                    continue;
                }
            }

            LPCUTF8     pszInterfaceMethodName  = pItfMD->GetNameOnNonArrayClass();
            PCCOR_SIGNATURE pInterfaceMethodSig;
            DWORD       cInterfaceMethodSig;
            BOOL        fFoundMatchInBuildingClass = FALSE;

            pItfMD->GetSig(&pInterfaceMethodSig, &cInterfaceMethodSig);

            //
            // First, try to find the method explicitly declared in our class
            //

            DeclaredMethodIterator methIt(*this);
            while (methIt.Next())
            {
                // Note that non-publics can legally be exposed via an interface, but only
                // through methodImpls.
                if (IsMdVirtual(methIt.Attrs()) && IsMdPublic(methIt.Attrs()))
                {
                    if (methIt.Name() == NULL)
                    {
                        BuildMethodTableThrowException(IDS_CLASSLOAD_NOMETHOD_NAME);
                    }

#ifdef _DEBUG
                    if(GetHalfBakedClass()->m_fDebuggingClass && g_pConfig->ShouldBreakOnMethod(methIt.Name()))
                        _ASSERTE(!"BreakOnMethodName");
#endif // _DEBUG

                    if (strcmp(methIt.Name(),pszInterfaceMethodName) == 0)
                    {
                        PCCOR_SIGNATURE pMemberSignature;
                        DWORD       cMemberSignature;

                        pMemberSignature = methIt.GetSig(&cMemberSignature);

                        if (MetaSig::CompareMethodSigs(
                            pMemberSignature,
                            cMemberSignature,
                            bmtInternal->pModule,
                            NULL,
                            pInterfaceMethodSig,
                            cInterfaceMethodSig,
                            pItfMD->GetModule(),
                            pIntfSubst))
                        {
                            fFoundMatchInBuildingClass = TRUE;
                            *ppImplementingMD = methIt.GetMethodDesc();
                            *ppDeclaringMD = pItfMD;
                            break;
                        }
                    }
                }
            } // end ... try to find method

            CONSISTENCY_CHECK(it.GetSlotNumber() < bmtInterface->dwLargestInterfaceSize);

            if (!fFoundMatchInBuildingClass)
            {
                // If this interface has been layed out by our parent then
                // we do not need to define a new method desc for it. This
                // is needed only for APPHACK behaviour defined above.
                if(fParentInterface)
                {
                    *ppImplementingMD = NULL;
                    *ppDeclaringMD = NULL;
                    continue;
                }
                // We will use the interface implemenation if we do not find one in the
                // parent. It will have to be overriden by the a method impl unless the
                // class is abstract or it is a special COM type class.

                MethodDesc* pParentMD = NULL;
                if(bmtParent->pParentMethodTable != NULL)
                {
                    CONSISTENCY_CHECK_MSG(!(GetHalfBakedClass()->m_fDebuggingClass &&
                                             g_pConfig->ShouldBreakOnMethod(pszInterfaceMethodName)),
                                           "BreakOnMethodName");

                    // Check the parent class.
                    pParentMD = bmtParent->pParentMethodTable->GetClass()->FindMethod(
                                                         pszInterfaceMethodName,
                                                         pInterfaceMethodSig,
                                                         cInterfaceMethodSig,
                                                         pItfMD->GetModule(),
                                                         pIntfSubst,
                                                         EEClass::FM_ForInterface,
                                                         &bmtParent->parentSubst);
                }

                // If the found method is virtual and public, we can use it. Otherwise, by definition
                // of Partition II, chapter 11.2, we are not allowed to use this method.
                if(pParentMD != NULL)
                {
                    CONSISTENCY_CHECK(IsMdVirtual(pParentMD->GetAttrs()));
                    CONSISTENCY_CHECK(IsMdPublic(pParentMD->GetAttrs()));
                    *ppImplementingMD = pParentMD;
                    *ppDeclaringMD = pItfMD;
                }
                else
                {
                    *ppImplementingMD = pItfMD;
                    *ppDeclaringMD = NULL;
                }
            }
        }

        //
        // Now that all matches for the current interface have been determined,
        // add these matches to the dispatch map (for stub dispatch) or to the
        // vtable (for vtable interface dispatch).
        //

        it.MoveToBegin();
        for (; it.IsValid() && it.IsVirtual(); it.Next())
        {
            // The entry can be null if the interface was previously
            // laid out by a parent and we did not have a method
            // that subclassed the interface.

            // Get the MethodDesc which was allocated for the method
            MethodDesc *pMD = bmtInterface->ppInterfaceMethodDescList[it.GetSlotNumber()];
            MethodDesc *pMDDecl = bmtInterface->ppInterfaceDeclMethodDescList[it.GetSlotNumber()];

            // Now fill out the stub dispatch implementation table.
            if (pMD != NULL && pMDDecl != NULL)
            {
                DispatchMapTypeID dispatchMapTypeID = DispatchMapTypeID::InterfaceClassID(dwCurInterface);
                CONSISTENCY_CHECK(dispatchMapTypeID.IsImplementedInterface());
#ifdef _DEBUG
                DispatchMapTypeID dispatchMapTypeID2 = ComputeDispatchMapTypeID(pInterface, pIntfSubst);
                CONSISTENCY_CHECK(dispatchMapTypeID2.IsImplementedInterface());
                CONSISTENCY_CHECK(dispatchMapTypeID == dispatchMapTypeID2);
                CONSISTENCY_CHECK(!bmtVT->pDispatchMapBuilder->Contains(dispatchMapTypeID, pMDDecl->GetSlot()));
#endif // _DEBUG
                bmtVT->pDispatchMapBuilder->InsertMDMapping(dispatchMapTypeID, pMDDecl->GetSlot(), pMD, TRUE);
            }
        }
    }
}


//*******************************************************************************
//
// Used by BuildMethodTable
//
// Place static fields
//
VOID MethodTableBuilder::PlaceStaticFields()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    DWORD i;

     //===============================================================
    // BEGIN: Place static fields
    //===============================================================
    LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Placing statics for %s\n", this->GetDebugClassName()));

    //
    // Place gc refs and value types first, as they need to have handles created for them.
    // (Placing them together allows us to easily create the handles when Restoring the class,
    // and when initializing new DLS for the class.)
    //

    DWORD   dwCumulativeStaticFieldPos = 0 ;
    DWORD   dwCumulativeStaticGCFieldPos = 0;
    DWORD   dwCumulativeStaticBoxFieldPos = 0;

    // We don't need to do any calculations for the gc refs or valuetypes, as they're
    // guaranteed to be aligned in ModuleStaticsInfo
    bmtFP->NumStaticFieldsOfSize[LOG2_PTRSIZE]
      -= bmtFP->NumStaticGCBoxedFields + bmtFP->NumStaticGCPointerFields;

    // Place fields, largest first, padding so that each group is aligned to its natural size
    for (i = MAX_LOG2_PRIMITIVE_FIELD_SIZE; (signed long) i >= 0; i--)
    {
        // Fields of this size start at the next available location
        bmtFP->StaticFieldStart[i] = dwCumulativeStaticFieldPos;
        dwCumulativeStaticFieldPos += (bmtFP->NumStaticFieldsOfSize[i] << i);

        // Reset counters for the loop after this one
        bmtFP->NumStaticFieldsOfSize[i]    = 0;
    }


    if (dwCumulativeStaticFieldPos > FIELD_OFFSET_LAST_REAL_OFFSET)
        BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);

    DWORD dwNumHandleStatics = bmtFP->NumStaticGCBoxedFields + bmtFP->NumStaticGCPointerFields;
    if (dwNumHandleStatics != (WORD)dwNumHandleStatics) {
        BuildMethodTableThrowException(IDS_EE_TOOMANYFIELDS);
    }
    SetNumHandleStatics(dwNumHandleStatics);

    SetNumBoxedStatics(bmtFP->NumStaticGCBoxedFields);


    // Tell the module to give us the offsets we'll be using and commit space for us
    // if necessary
    DWORD dwNonGCOffset, dwGCOffset;
    GetModule()->GetOffsetsForStaticData(bmtInternal->cl,
                                         bmtProp->fDynamicStatics,
                                         GetNumHandleStatics(), dwCumulativeStaticFieldPos,
                                         &dwGCOffset, &dwNonGCOffset);

    // Allocate boxed statics first
    dwCumulativeStaticGCFieldPos = bmtFP->NumStaticGCBoxedFields<<LOG2_PTRSIZE;

    FieldDesc *pFieldDescList = GetHalfBakedClass()->GetApproxFieldDescListRaw();
    // Place static fields
    for (i = 0; i < bmtEnumMF->dwNumStaticFields; i++)
    {
        DWORD dwIndex       = bmtEnumMF->dwNumInstanceFields+i; // index in the FieldDesc list
        DWORD dwFieldSize   = (DWORD)(size_t)pFieldDescList[dwIndex].m_pMTOfEnclosingClass; // log2(field size)
        DWORD dwOffset      = (DWORD) pFieldDescList[dwIndex].m_dwOffset; // offset or type of field

        switch (dwOffset)
        {
        case FIELD_OFFSET_UNPLACED_GC_PTR:
            pFieldDescList[dwIndex].SetOffset(dwCumulativeStaticGCFieldPos + dwGCOffset);
            dwCumulativeStaticGCFieldPos += 1<<LOG2_PTRSIZE;
            LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Field placed at GC offset 0x%x\n", pFieldDescList[dwIndex].GetOffset_NoLogging()));

            break;

        case FIELD_OFFSET_VALUE_CLASS:
            pFieldDescList[dwIndex].SetOffset(dwCumulativeStaticBoxFieldPos + dwGCOffset);
            dwCumulativeStaticBoxFieldPos += 1<<LOG2_PTRSIZE;
            LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Field placed at GC offset 0x%x\n", pFieldDescList[dwIndex].GetOffset_NoLogging()));

            break;

        case FIELD_OFFSET_UNPLACED:
            pFieldDescList[dwIndex].SetOffset(bmtFP->StaticFieldStart[dwFieldSize] +
                                                (bmtFP->NumStaticFieldsOfSize[dwFieldSize] << dwFieldSize) +
                                                + dwNonGCOffset);
            bmtFP->NumStaticFieldsOfSize[dwFieldSize]++;
            LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Field placed at non GC offset 0x%x\n", pFieldDescList[dwIndex].GetOffset_NoLogging()));
            break;

        default:
            // RVA field
            break;
        }

        LOG((LF_CLASSLOADER, LL_INFO1000000, "Offset of %s: %i\n", pFieldDescList[dwIndex].m_debugName, pFieldDescList[dwIndex].GetOffset_NoLogging()));
    }

    if (bmtProp->fDynamicStatics)
    {
        _ASSERTE(dwNonGCOffset == 0 ||  // no statics at all
                 dwNonGCOffset == DomainLocalModule::DynamicEntry::GetOffsetOfDataBlob()); // We need space to point to the GC statics
        bmtVT->dwNonGCStaticFieldBytes = dwCumulativeStaticFieldPos;
    }
    else
    {
        bmtVT->dwNonGCStaticFieldBytes = MODULE_NON_DYNAMIC_STATICS; // Non dynamics shouldnt be using this
    }
    LOG((LF_CLASSLOADER, LL_INFO10000, "STATICS: Static field bytes needed (-1 is normal for non dynamic case)%i\n", bmtVT->dwNonGCStaticFieldBytes));
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Place instance fields
//
VOID    MethodTableBuilder::PlaceInstanceFields(EEClass** pByValueClassCache)
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END;



    DWORD i;

        //===============================================================
        // BEGIN: Place instance fields
        //===============================================================

        FieldDesc *pFieldDescList = GetHalfBakedClass()->GetApproxFieldDescListRaw();
        DWORD   dwCumulativeInstanceFieldPos;

        // Instance fields start right after the parent
        dwCumulativeInstanceFieldPos    = (bmtParent->pParentMethodTable != NULL) ? bmtParent->pParentMethodTable->GetClass()->GetNumInstanceFieldBytes() : 0;

        // place small fields first if the parent have a number of field bytes that is not aligned
        if (!IS_ALIGNED(dwCumulativeInstanceFieldPos, DATA_ALIGNMENT))
        {
            for (i = 0; i < MAX_LOG2_PRIMITIVE_FIELD_SIZE; i++) {
                DWORD j;

                if (IS_ALIGNED(dwCumulativeInstanceFieldPos, 1<<(i+1)))
                    continue;

                // check whether there are any bigger fields
                for (j = i + 1; j <= MAX_LOG2_PRIMITIVE_FIELD_SIZE; j++) {
                    if (bmtFP->NumInstanceFieldsOfSize[j] != 0)
                        break;
                }
                // nothing to gain if there are no bigger fields
                if (j > MAX_LOG2_PRIMITIVE_FIELD_SIZE)
                    break;

                // check whether there are any small enough fields
                for (j = i; (signed long) j >= 0; j--) {
                    if (bmtFP->NumInstanceFieldsOfSize[j] != 0)
                        break;
                }
                // nothing to play with if there are no smaller fields
                if ((signed long) j < 0)
                    break;
                // eventually go back and use the smaller field as filling
                i = j;

                CONSISTENCY_CHECK(bmtFP->NumInstanceFieldsOfSize[i] != 0);

                j = bmtFP->FirstInstanceFieldOfSize[i];

                // Avoid reordering of gcfields
                if (i == LOG2SLOT) {
                    for ( ; j < bmtEnumMF->dwNumInstanceFields; j++) {
                        if ((pFieldDescList[j].GetOffset_NoLogging() == FIELD_OFFSET_UNPLACED) &&
                            (pFieldDescList[j].m_pMTOfEnclosingClass == (MethodTable *)(size_t)i))
                            break;
                    }

                    // out of luck - can't reorder gc fields
                    if (j >= bmtEnumMF->dwNumInstanceFields)
                        break;
                }

                // Place the field
                dwCumulativeInstanceFieldPos = (DWORD)ALIGN_UP(dwCumulativeInstanceFieldPos, 1 << i);

                pFieldDescList[j].SetOffset(dwCumulativeInstanceFieldPos);
                dwCumulativeInstanceFieldPos += (1 << i);

                // We've placed this field now, so there is now one less of this size field to place
                if (--bmtFP->NumInstanceFieldsOfSize[i] == 0)
                    continue;

                // We are done in this round if we haven't picked the first field
                if (bmtFP->FirstInstanceFieldOfSize[i] != j)
                    continue;

                // Update FirstInstanceFieldOfSize[i] to point to the next such field
                for (j = j+1; j < bmtEnumMF->dwNumInstanceFields; j++)
                {
                    // The log of the field size is stored in the method table
                    if (pFieldDescList[j].m_pMTOfEnclosingClass == (MethodTable *)(size_t)i)
                    {
                        bmtFP->FirstInstanceFieldOfSize[i] = j;
                        break;
                    }
                }
                _ASSERTE(j < bmtEnumMF->dwNumInstanceFields);
            }
        }

        // Place fields, largest first
        for (i = MAX_LOG2_PRIMITIVE_FIELD_SIZE; (signed long) i >= 0; i--)
        {
            if (bmtFP->NumInstanceFieldsOfSize[i] == 0)
                continue;

            // Align instance fields if we aren't already
            dwCumulativeInstanceFieldPos = (DWORD)ALIGN_UP(dwCumulativeInstanceFieldPos, min(1 << i, DATA_ALIGNMENT));

            // Fields of this size start at the next available location
            bmtFP->InstanceFieldStart[i] = dwCumulativeInstanceFieldPos;
            dwCumulativeInstanceFieldPos += (bmtFP->NumInstanceFieldsOfSize[i] << i);

            // Reset counters for the loop after this one
            bmtFP->NumInstanceFieldsOfSize[i]  = 0;
        }


        // Make corrections to reserve space for GC Pointer Fields
        //
        // The GC Pointers simply take up the top part of the region associated
        // with fields of that size (GC pointers can be 64 bit on certain systems)
        if (bmtFP->NumInstanceGCPointerFields)
        {
            bmtFP->GCPointerFieldStart = bmtFP->InstanceFieldStart[LOG2SLOT];
            bmtFP->InstanceFieldStart[LOG2SLOT] = bmtFP->InstanceFieldStart[LOG2SLOT] + (bmtFP->NumInstanceGCPointerFields << LOG2SLOT);
            bmtFP->NumInstanceGCPointerFields = 0;     // reset to zero here, counts up as pointer slots are assigned below
        }

        // Place instance fields - be careful not to place any already-placed fields
        for (i = 0; i < bmtEnumMF->dwNumInstanceFields; i++)
        {
            DWORD dwFieldSize   = (DWORD)(size_t)pFieldDescList[i].m_pMTOfEnclosingClass;
            DWORD dwOffset;

            dwOffset = pFieldDescList[i].GetOffset_NoLogging();

            // Don't place already-placed fields
            if ((dwOffset == FIELD_OFFSET_UNPLACED || dwOffset == FIELD_OFFSET_UNPLACED_GC_PTR || dwOffset == FIELD_OFFSET_VALUE_CLASS))
            {
                if (dwOffset == FIELD_OFFSET_UNPLACED_GC_PTR)
                {
                    pFieldDescList[i].SetOffset(bmtFP->GCPointerFieldStart + (bmtFP->NumInstanceGCPointerFields << LOG2SLOT));
                    bmtFP->NumInstanceGCPointerFields++;
                }
                else if (pFieldDescList[i].IsByValue() == FALSE) // it's a regular field
                {
                    pFieldDescList[i].SetOffset(bmtFP->InstanceFieldStart[dwFieldSize] + (bmtFP->NumInstanceFieldsOfSize[dwFieldSize] << dwFieldSize));
                    bmtFP->NumInstanceFieldsOfSize[dwFieldSize]++;
                }
            }
        }

        WORD wNumGCPointerSeries;
        // Save Number of pointer series
        if (bmtFP->NumInstanceGCPointerFields)
            wNumGCPointerSeries = bmtParent->NumParentPointerSeries + 1;
        else
            wNumGCPointerSeries = bmtParent->NumParentPointerSeries;

        // Place by value class fields last
        // Update the number of GC pointer series
        for (i = 0; i < bmtEnumMF->dwNumInstanceFields; i++)
        {
            if (pFieldDescList[i].IsByValue())
            {

                EEClass *pByValueClass = pByValueClassCache[i];

                    // value classes could have GC pointers in them, which need to be pointer-size aligned
                    // so do this if it has not been done already

#if !defined(_WIN64) && (DATA_ALIGNMENT > 4)
                dwCumulativeInstanceFieldPos = (DWORD)ALIGN_UP(dwCumulativeInstanceFieldPos,
                    (pByValueClass->GetNumInstanceFieldBytes() >= DATA_ALIGNMENT) ? DATA_ALIGNMENT : sizeof(void*));
#else // !(!defined(_WIN64) && (DATA_ALIGNMENT > 4))
                dwCumulativeInstanceFieldPos = (DWORD)ALIGN_UP(dwCumulativeInstanceFieldPos,
                    sizeof(void*));
#endif // !(!defined(_WIN64) && (DATA_ALIGNMENT > 4))

                pFieldDescList[i].SetOffset(dwCumulativeInstanceFieldPos);
                dwCumulativeInstanceFieldPos += pByValueClass->GetAlignedNumInstanceFieldBytes();

                // Add pointer series for by-value classes
                wNumGCPointerSeries += pByValueClass->m_wNumGCPointerSeries;
            }
        }

            // Can be unaligned
        DWORD dwNumInstanceFieldBytes = dwCumulativeInstanceFieldPos;

        if (IsValueClass())
        {
                 // Like C++ we enforce that there can be no 0 length structures.
                // Thus for a value class with no fields, we 'pad' the length to be 1
            if (dwNumInstanceFieldBytes == 0)
                dwNumInstanceFieldBytes = 1;

                // The JITs like to copy full machine words,
                //  so if the size is bigger than a void* round it up to minAlign
                // and if the size is smaller than void* round it up to next power of two
            unsigned minAlign;

            if (dwNumInstanceFieldBytes > sizeof(void*)) {
                minAlign = sizeof(void*);
            }
            else {
                minAlign = 1;
                while (minAlign < dwNumInstanceFieldBytes)
                    minAlign *= 2;
            }

            dwNumInstanceFieldBytes = (dwNumInstanceFieldBytes + minAlign-1) & ~(minAlign-1);
        }

        if (dwNumInstanceFieldBytes > FIELD_OFFSET_LAST_REAL_OFFSET) {
            BuildMethodTableThrowException(IDS_CLASSLOAD_FIELDTOOLARGE);
        }

        SetNumInstanceFieldBytes(dwNumInstanceFieldBytes);
        SetNumGCPointerSeries(wNumGCPointerSeries);


        //===============================================================
        // END: Place instance fields
        //===============================================================
}

//*******************************************************************************
// this accesses the field size which is temporarily stored in m_pMTOfEnclosingClass
// during class loading. Don't use any other time
DWORD MethodTableBuilder::GetFieldSize(FieldDesc *pFD)
{

    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

        // We should only be calling this while this class is being built.
    _ASSERTE(GetHalfBakedMethodTable() == 0);
    BAD_FORMAT_NOTHROW_ASSERT(! pFD->IsByValue() || HasExplicitFieldOffsetLayout());

    if (pFD->IsByValue())
        return (DWORD)(size_t)(pFD->m_pMTOfEnclosingClass);
    return (1 << (DWORD)(size_t)(pFD->m_pMTOfEnclosingClass));
}




//----------------------------------------------------------------------------------------------
// This class is a helper for HandleExplicitLayout. To make it harder to introduce security holes
// into this function, we will manage all updates to the class's trust level through the ExplicitClassTrust
// class. This abstraction enforces the rule that the overall class is only as trustworthy as
// the least trustworthy field.
//----------------------------------------------------------------------------------------------
class ExplicitClassTrust : private ExplicitFieldTrust
{
    public:
        ExplicitClassTrust()
        {
            LEAF_CONTRACT;
            m_trust = kMaxTrust;   // Yes, we start out with maximal trust. This reflects that explicit layout structures with no fields do represent no risk.
        }

        VOID AddField(TrustLevel fieldTrust)
        {
            LEAF_CONTRACT;
            m_trust = min(m_trust, fieldTrust);
        }

        BOOL IsLegal()
        {
            LEAF_CONTRACT;
            return m_trust >= kLegal;
        }

        BOOL IsVerifiable()
        {
            LEAF_CONTRACT;
            return m_trust >= kVerifiable;
        }

        BOOL IsNonOverLayed()
        {
            LEAF_CONTRACT;
            return m_trust >= kNonOverLayed;
        }

        TrustLevel GetTrustLevel()
        {
            LEAF_CONTRACT;
            return m_trust;
        }

    private:
        TrustLevel      m_trust;
};


//----------------------------------------------------------------------------------------------
// This class is a helper for HandleExplicitLayout. To make it harder to introduce security holes
// into this function, this class will collect trust information about individual fields to be later
// aggregated into the overall class level.
//
// This abstraction enforces the rule that all fields are presumed guilty until explicitly declared
// safe by calling SetTrust(). If you fail to call SetTrust before leaving the block, the destructor
// will automatically cause the entire class to be declared illegal (and you will get an assert
// telling you to fix this bug.)
//----------------------------------------------------------------------------------------------
class ExplicitFieldTrustHolder : private ExplicitFieldTrust
{
    public:
        ExplicitFieldTrustHolder(ExplicitClassTrust *pExplicitClassTrust)
        {
            LEAF_CONTRACT;
            m_pExplicitClassTrust = pExplicitClassTrust;
#ifdef _DEBUG
            m_trustDeclared       = FALSE;
#endif
            m_fieldTrust          = kNone;
        }

        VOID SetTrust(TrustLevel fieldTrust)
        {
            LEAF_CONTRACT;

            _ASSERTE(fieldTrust >= kNone && fieldTrust <= kMaxTrust);
            _ASSERTE(!m_trustDeclared && "You should not set the trust value more than once.");

#ifdef _DEBUG
            m_trustDeclared = TRUE;
#endif
            m_fieldTrust = fieldTrust;
        }

        ~ExplicitFieldTrustHolder()
        {
            LEAF_CONTRACT;
            // If no SetTrust() was ever called, we will default to kNone (i.e. declare the entire type
            // illegal.) It'd be nice to assert here but since this case can be legitimately reached
            // on exception unwind, we cannot.
            m_pExplicitClassTrust->AddField(m_fieldTrust);
        }


    private:
        ExplicitClassTrust* m_pExplicitClassTrust;
        TrustLevel          m_fieldTrust;
#ifdef _DEBUG
        BOOL                m_trustDeclared;                // Debug flag to detect multiple Sets. (Which we treat as a bug as this shouldn't be necessary.)
#endif
};






//*******************************************************************************
// make sure that no object fields are overlapped incorrectly and define the
// GC pointer series for the class. We are assuming that this class will always be laid out within
// its enclosing class by the compiler in such a way that offset 0 will be the correct alignment
// for object ref fields so we don't need to try to align it
VOID MethodTableBuilder::HandleExplicitLayout(EEClass **pByValueClassCache)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    // need to calculate instance size as can't use nativeSize or anything else that
    // has been previously calculated.
    UINT instanceSliceSize = 0;
    DWORD firstObjectOverlapOffset = ((DWORD)(-1));


    UINT i;
    for (i=0; i < bmtMetaData->cFields; i++) {
        FieldDesc *pFD = bmtMFDescs->ppFieldDescList[i];
        if (!pFD)
            continue;
        if (pFD->IsStatic())
            continue;

        UINT fieldExtent = 0;
        if (!ClrSafeInt<UINT>::addition(pFD->GetOffset_NoLogging(), GetFieldSize(pFD), fieldExtent))
            BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
        if (fieldExtent > instanceSliceSize)
            instanceSliceSize = fieldExtent;
    }

    CQuickBytes qb;
    // Helping out prefix
    PREFIX_ASSUME(sizeof(BYTE) == 1);
    BYTE *pFieldLayout = (BYTE*) qb.AllocThrows(instanceSliceSize * sizeof(BYTE));
    for (i=0; i < instanceSliceSize; i++)
        pFieldLayout[i] = empty;

    // go through each field and look for invalid layout
    // (note that we are more permissive than what Ecma allows. We only disallow the minimum set necessary to
    // close security holes.)
    //
    // This is what we implment:
    //
    // 1. Verify that every OREF is on a valid alignment
    // 2. Verify that OREFs only overlap with other OREFs.
    // 3. If an OREF does overlap with another OREF, the class is marked unverifiable.
    // 4. If an overlap of any kind occurs, the class will be marked NotTightlyPacked (affects ValueType.Equals()).
    //
    char emptyObject[sizeof(void*)];
    char isObject[sizeof(void*)];
    for (i = 0; i < sizeof(void*); i++)
    {
        emptyObject[i] = empty;
        isObject[i]    = oref;
    }


    ExplicitClassTrust explicitClassTrust;

    UINT valueClassCacheIndex = ((UINT)(-1));
    UINT badOffset = 0;
    FieldDesc *pFD = NULL;
    for (i=0; i < bmtMetaData->cFields; i++)
    {
        // Note about this loop body:
        //
        // This loop is coded to make it as hard as possible to allow a field to be trusted when it shouldn't.
        //
        // Every path in this loop body must lead to an explicit decision as to whether the field nonoverlaps,
        // overlaps in a verifiable fashion, overlaps in a nonverifiable fashion or overlaps in a completely illegal fashion.
        //
        // It must call fieldTrust.SetTrust() with the appropriate result. If you don't call it, fieldTrust's destructor
        // will intentionally default to kNone and mark the entire class illegal.
        //
        // If your result is anything but kNone (class is illegal), you must also explicitly "continue" the loop.
        // There is a "break" at end of this loop body that will abort the loop if you don't do this. And
        // if you don't finish iterating through all the fields, this function will automatically mark the entire
        // class illegal. This rule is a vestige of an earlier version of this function.

        // This object's dtor will aggregate the trust decision for this field into the trust level for the class as a whole.
        ExplicitFieldTrustHolder fieldTrust(&explicitClassTrust);

        pFD = bmtMFDescs->ppFieldDescList[i];
        if (pFD == NULL || pFD->IsStatic())
        {
            fieldTrust.SetTrust(ExplicitFieldTrust::kNonOverLayed);
            continue;
        }

        // "i" indexes all fields, valueClassCacheIndex indexes non-static fields only. Don't get them confused!
        valueClassCacheIndex++;

        if (CorTypeInfo::IsObjRef(pFD->GetFieldType()))
        {
            if (pFD->GetOffset_NoLogging() & ((ULONG)sizeof(OBJECTREF) - 1))
            {
                badOffset = pFD->GetOffset_NoLogging();
                fieldTrust.SetTrust(ExplicitFieldTrust::kNone);

                // If we got here, OREF field was not pointer aligned. THROW.
                break;
            }
            // check if overlaps another object
            if (memcmp((void *)&pFieldLayout[pFD->GetOffset_NoLogging()], (void *)isObject, sizeof(isObject)) == 0)
            {
                // If we got here, an OREF overlapped another OREF. We permit this but mark the class unverifiable.
                fieldTrust.SetTrust(ExplicitFieldTrust::kLegal);

                if (firstObjectOverlapOffset == ((DWORD)(-1)))
                {
                    firstObjectOverlapOffset = pFD->GetOffset_NoLogging();
                }

                continue;
            }
            // check if is empty at this point
            if (memcmp((void *)&pFieldLayout[pFD->GetOffset_NoLogging()], (void *)emptyObject, sizeof(emptyObject)) == 0)
            {
                // If we got here, this OREF is overlapping no other fields (yet). Record that these bytes now contain an OREF.
                memset((void *)&pFieldLayout[pFD->GetOffset_NoLogging()], oref, sizeof(isObject));
                fieldTrust.SetTrust(ExplicitFieldTrust::kNonOverLayed);
                continue;
            }

            // If we got here, the OREF overlaps a non-OREF. THROW.
            badOffset = pFD->GetOffset_NoLogging();
            fieldTrust.SetTrust(ExplicitFieldTrust::kNone);
            break;
        }
        else
        {
            UINT fieldSize;
            if (pFD->IsByValue())
            {
                EEClass *pByValue = pByValueClassCache[valueClassCacheIndex];
                if (pByValue->GetMethodTable()->ContainsPointers())
                {
                    if ((pFD->GetOffset_NoLogging() & ((ULONG)sizeof(void*) - 1)) == 0)
                    {
                        ExplicitFieldTrust::TrustLevel trust;
                        DWORD firstObjectOverlapOffsetInsideValueClass = ((DWORD)(-1));
                        trust = CheckValueClassLayout(pByValue, &pFieldLayout[pFD->GetOffset_NoLogging()], &firstObjectOverlapOffsetInsideValueClass);
                        fieldTrust.SetTrust(trust);
                        if (firstObjectOverlapOffsetInsideValueClass != ((DWORD)(-1)))
                        {
                            if (firstObjectOverlapOffset == ((DWORD)(-1)))
                            {
                                firstObjectOverlapOffset = pFD->GetOffset_NoLogging() + firstObjectOverlapOffsetInsideValueClass;
                            }
                        }

                        if (trust != ExplicitFieldTrust::kNone)
                        {
                            continue;
                        }
                        else
                        {
                            // If we got here, then an OREF inside the valuetype illegally overlapped a non-OREF field. THROW.
                            badOffset = pFD->GetOffset_NoLogging();
                            break;
                        }
                    }
                    // If we got here, then a valuetype containing an OREF was misaligned.
                    badOffset = pFD->GetOffset_NoLogging();
                    fieldTrust.SetTrust(ExplicitFieldTrust::kNone);
                    break;
                }
                // no pointers so fall through to do standard checking
                fieldSize = pByValue->m_dwNumInstanceFieldBytes;
            }
            else
            {
                // field size temporarily stored in pInterface field
                fieldSize = GetFieldSize(pFD);
            }

            // If we got here, we are trying to place a non-OREF (or a valuetype composed of non-OREFs.)
            // Look for any orefs under this field
            BYTE *loc;
            if ((loc = (BYTE*)memchr((void*)&pFieldLayout[pFD->GetOffset_NoLogging()], oref, fieldSize)) == NULL)
            {
                // If we have a nonoref in the range then we are doing an overlay
                if(memchr((void*)&pFieldLayout[pFD->GetOffset_NoLogging()], nonoref, fieldSize))
                {
                    fieldTrust.SetTrust(ExplicitFieldTrust::kVerifiable);
                }
                else
                {
                    fieldTrust.SetTrust(ExplicitFieldTrust::kNonOverLayed);
                }
                memset((void*)&pFieldLayout[pFD->GetOffset_NoLogging()], nonoref, fieldSize);
                continue;
            }

            // If we got here, we tried to place a non-OREF (or a valuetype composed of non-OREFs)
            // on top of an OREF. THROW.
            badOffset = (UINT)(loc - pFieldLayout);
            fieldTrust.SetTrust(ExplicitFieldTrust::kNone);
            break;
            // anything else is an error
        }

        // We have to comment out this assert because otherwise, the compiler refuses to build because the _ASSERT is unreachable
        // (Thanks for nothing, compiler, that's what the assert is trying to enforce!) But the intent of the assert is correct.
        //_ASSERTE(!"You aren't supposed to be here. Some path inside the loop body did not execute an explicit break or continue.");


        // If we got here, some code above failed to execute an explicit "break" or "continue." This is a bug! To be safe,
        // we will put a catchall "break" here which will cause the typeload to abort (albeit with a probably misleading
        // error message.)
        break;
    } // for(;;)

    // We only break out of the loop above if we detected an error.
    if (i < bmtMetaData->cFields || !explicitClassTrust.IsLegal())
    {
        ThrowFieldLayoutError(GetCl(),
                              bmtInternal->pModule,
                              badOffset,
                              IDS_CLASSLOAD_EXPLICIT_LAYOUT);
    }

    if(!explicitClassTrust.IsVerifiable())
    {
        GCX_COOP();

        if (!Security::CanSkipVerification(GetAssembly()->GetDomainAssembly()))
        {
            ThrowFieldLayoutError(GetCl(),
                                  bmtInternal->pModule,
                                  firstObjectOverlapOffset,
                                  IDS_CLASSLOAD_UNVERIFIABLE_FIELD_LAYOUT);
        }

        //SetHasNonVerifiablyOverLayedFields();
    }

    if(!explicitClassTrust.IsNonOverLayed())
    {
        SetHasOverLayedFields();
    }

    FindPointerSeriesExplicit(instanceSliceSize, pFieldLayout);

    // Fixup the offset to include parent as current offsets are relative to instance slice
    // Could do this earlier, but it's just easier to assume instance relative for most
    // of the earlier calculations

    // Instance fields start right after the parent
    size_t dwInstanceSliceOffset    = InstanceSliceOffsetForExplicit(bmtGCSeries->numSeries != 0, bmtParent->pParentMethodTable);

    size_t numInstanceFieldBytes = dwInstanceSliceOffset + instanceSliceSize;
    if (numInstanceFieldBytes < dwInstanceSliceOffset || ((size_t)(DWORD)numInstanceFieldBytes) != numInstanceFieldBytes)
    {
        // addition overflow or cast truncation
        BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
    }

    if (IsValueClass())
    {
        ULONG tmpclstotalsize = 0;
        GetMDImport()->GetClassTotalSize(GetCl(), &tmpclstotalsize);

        if (tmpclstotalsize)
        {
            size_t clstotalsize = (size_t)tmpclstotalsize;
            if (clstotalsize != tmpclstotalsize)
            {
                // addition overflow or cast truncation
                BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
            }

            // size must be large enough to accomodate layout. If not, we use the layout size instead.
            if (clstotalsize < numInstanceFieldBytes)
            {
                clstotalsize = numInstanceFieldBytes;
            }
            numInstanceFieldBytes = clstotalsize;   // use the size they told us
        }

    }

    // The GC requires that all valuetypes containing orefs be sized to a multiple of sizeof(void*).
    if (bmtGCSeries->numSeries != 0)
    {
        size_t tmp = ALIGN_UP(numInstanceFieldBytes, sizeof(void*));
        if (tmp < numInstanceFieldBytes)
        {
            // Integer overflow
            BuildMethodTableThrowException(IDS_CLASSLOAD_GENERAL);
        }
        numInstanceFieldBytes = tmp;
    }

    // Set the total size
    SetNumInstanceFieldBytes((DWORD)numInstanceFieldBytes);

    for (i=0; i < bmtMetaData->cFields; i++) {
        FieldDesc *pTempFD = bmtMFDescs->ppFieldDescList[i];
        if (pTempFD == NULL || pTempFD->IsStatic())
        {
            continue;
        }
        HRESULT hr = pTempFD->SetOffset(pTempFD->GetOffset_NoLogging() + (DWORD)dwInstanceSliceOffset);
        if (FAILED(hr))
        {
            BuildMethodTableThrowException(hr, *bmtError);
        }
    }
}

//*******************************************************************************
// Check that the class type parameters are used consistently in this signature blob
// in accordance with their variance annotations
// The signature is assumed to be well-formed but indices and arities might not be correct
BOOL EEClass::CheckVarianceInSig(DWORD numGenericArgs, BYTE* pVarianceInfo, SigPointer psig, CorGenericParamAttr position)
{
    CONTRACT(BOOL)
    {
        THROWS;
        MODE_ANY;
        GC_TRIGGERS;
        INSTANCE_CHECK;
        PRECONDITION(position == gpNonVariant || position == gpCovariant || position == gpContravariant);
    }
    CONTRACT_END

    if (pVarianceInfo == NULL)
        RETURN TRUE;

    CorElementType typ;
    IfFailThrow(psig.GetElemType(&typ));

    switch(typ) {
        case ELEMENT_TYPE_STRING:
        case ELEMENT_TYPE_U:
        case ELEMENT_TYPE_I:
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
        case ELEMENT_TYPE_OBJECT:
        case ELEMENT_TYPE_TYPEDBYREF:
        case ELEMENT_TYPE_MVAR:
        case ELEMENT_TYPE_CLASS:
        case ELEMENT_TYPE_VALUETYPE:
            RETURN TRUE;

        case ELEMENT_TYPE_VAR:
        {
            DWORD index;
            IfFailThrow(psig.GetData(&index));

            // This will be checked later anyway; so give up and don't indicate a variance failure
            if (index < 0 || index >= numGenericArgs)
                RETURN TRUE;

            // Non-variant parameters are allowed to appear anywhere
            if (pVarianceInfo[index] == gpNonVariant)
                RETURN TRUE;

            // Covariant and contravariant parameters can *only* appear in resp. covariant and contravariant positions
            RETURN ((CorGenericParamAttr) (pVarianceInfo[index]) == position);
        }

        case ELEMENT_TYPE_GENERICINST:
        {
            IfFailThrow(psig.GetElemType(&typ));
            mdTypeRef typeref;
            IfFailThrow(psig.GetToken(&typeref));

            // The number of type parameters follows
            DWORD ntypars;
            IfFailThrow(psig.GetData(&ntypars));

            // If this is a value type, or position == gpNonVariant, then
            // we're disallowing covariant and contravariant completely
            if (typ == ELEMENT_TYPE_VALUETYPE || position == gpNonVariant)
            {
                for (unsigned i = 0; i < ntypars; i++)
                {
                    if (!CheckVarianceInSig(numGenericArgs, pVarianceInfo, psig, gpNonVariant))
                        RETURN FALSE;

                    psig.SkipExactlyOne();
                }
            }
            // Otherwise we need to take notice of the variance annotation on each type parameter to the generic type
            else
            {
                mdTypeDef typeDef;
                Module* pDefModule;
                // This will also be resolved later; so, give up and don't indicate a variance failure
                if (!ClassLoader::ResolveTokenToTypeDefThrowing(GetModule(), typeref, &pDefModule, &typeDef))
                    RETURN TRUE;

                HENUMInternal   hEnumGenericPars;
                if (FAILED(pDefModule->GetMDImport()->EnumInit(mdtGenericParam, typeDef, &hEnumGenericPars)))
                    pDefModule->GetAssembly()->ThrowTypeLoadException(pDefModule->GetMDImport(), typeDef, IDS_CLASSLOAD_BADFORMAT);

                for (unsigned i = 0; i < ntypars; i++)
                {
                    mdGenericParam tkTyPar;
                    pDefModule->GetMDImport()->EnumNext(&hEnumGenericPars, &tkTyPar);
                    DWORD flags;
                    pDefModule->GetMDImport()->GetGenericParamProps(tkTyPar, NULL, &flags, NULL, NULL, NULL);
                    CorGenericParamAttr genPosition = (CorGenericParamAttr) (flags & gpVarianceMask);
                    // If the surrounding context is contravariant then we need to flip the variance of this parameter
                    if (position == gpContravariant)
                    {
                        genPosition = genPosition == gpCovariant ? gpContravariant
                                    : genPosition == gpContravariant ? gpCovariant
                                    : gpNonVariant;
                    }
                    if (!CheckVarianceInSig(numGenericArgs, pVarianceInfo, psig, genPosition))
                        RETURN FALSE;

                    psig.SkipExactlyOne();
                }
                pDefModule->GetMDImport()->EnumClose(&hEnumGenericPars);
            }

            RETURN TRUE;
        }

        // Arrays behave covariantly
        case ELEMENT_TYPE_ARRAY:
        case ELEMENT_TYPE_SZARRAY:
            RETURN CheckVarianceInSig(numGenericArgs, pVarianceInfo, psig, position);

        // Pointers behave non-variantly
        case ELEMENT_TYPE_BYREF:
        case ELEMENT_TYPE_PTR:
            RETURN CheckVarianceInSig(numGenericArgs, pVarianceInfo, psig, gpNonVariant);

        case ELEMENT_TYPE_FNPTR:
            {
                // Calling convention
                IfFailThrow(psig.GetData(NULL));

                // Get arg count;
                ULONG cArgs;
                IfFailThrow(psig.GetData(&cArgs));

                // Conservatively, assume non-variance of function pointer types
                if (!CheckVarianceInSig(numGenericArgs, pVarianceInfo, psig, gpNonVariant))
                    RETURN FALSE;

                IfFailThrow(psig.SkipExactlyOne());

                for (unsigned i = 0; i < cArgs; i++)
                {
                    if (!CheckVarianceInSig(numGenericArgs, pVarianceInfo, psig, gpNonVariant))
                        RETURN FALSE;

                    IfFailThrow(psig.SkipExactlyOne());
                }

                RETURN TRUE;
            }

        default:
            THROW_BAD_FORMAT(IDS_CLASSLOAD_BAD_VARIANCE_SIG, this);
    }

    RETURN FALSE;
}

//*******************************************************************************
// make sure that no object fields are overlapped incorrectly, returns S_FALSE if
// there overlap but nothing illegal, S_OK if there is no overlap
/*static*/ ExplicitFieldTrust::TrustLevel MethodTableBuilder::CheckValueClassLayout(EEClass *pClass, BYTE *pFieldLayout, DWORD *pFirstObjectOverlapOffset)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    *pFirstObjectOverlapOffset = (DWORD)(-1);

    // Build a layout of the value class. Don't know the sizes of all the fields easily, but
    // do know a) vc is already consistent so don't need to check it's overlaps and
    // b) size and location of all objectrefs. So build it by setting all non-oref
    // then fill in the orefs later
    UINT fieldSize = pClass->GetNumInstanceFieldBytes();
    CQuickBytes qb;
    BYTE *vcLayout = (BYTE*) qb.AllocThrows(fieldSize * sizeof(BYTE));

    memset((void*)vcLayout, nonoref, fieldSize);
    // use pointer series to locate the orefs
    _ASSERTE(pClass->m_wNumGCPointerSeries > 0);
    CGCDescSeries *pSeries = ((CGCDesc*) pClass->GetMethodTable())->GetLowestSeries();

    for (UINT j = 0; j < pClass->GetNumGCPointerSeries(); j++)
    {
        CONSISTENCY_CHECK(pSeries <= CGCDesc::GetCGCDescFromMT(pClass->GetMethodTable())->GetHighestSeries());

        memset((void*)&vcLayout[pSeries->GetSeriesOffset()-sizeof(Object)], oref, pSeries->GetSeriesSize() + pClass->GetMethodTable()->GetBaseSize());
        pSeries++;
    }


    ExplicitClassTrust explicitClassTrust;

    for (UINT i=0; i < fieldSize; i++) {

        ExplicitFieldTrustHolder fieldTrust(&explicitClassTrust);

        if (vcLayout[i] == oref) {
            switch (pFieldLayout[i]) {
                // oref <--> empty
                case empty:
                    pFieldLayout[i] = oref;
                    fieldTrust.SetTrust(ExplicitFieldTrust::kNonOverLayed);
                    break;

                // oref <--> nonoref
                case nonoref:
                    fieldTrust.SetTrust(ExplicitFieldTrust::kNone);
                    break;

                // oref <--> oref
                case oref:
                    fieldTrust.SetTrust(ExplicitFieldTrust::kLegal);
                    if ((*pFirstObjectOverlapOffset) == ((DWORD)(-1)))
                    {
                        *pFirstObjectOverlapOffset = (DWORD)i;
                    }
                    break;

                default:
                    _ASSERTE(!"Can't get here.");
                }
        } else if (vcLayout[i] == nonoref) {
            switch (pFieldLayout[i]) {
                // nonoref <--> empty
                case empty:
                    pFieldLayout[i] = nonoref;
                    fieldTrust.SetTrust(ExplicitFieldTrust::kNonOverLayed);
                    break;

                // nonoref <--> nonoref
                case nonoref:
                    fieldTrust.SetTrust(ExplicitFieldTrust::kVerifiable);
                    break;

                // nonoref <--> oref
                case oref:
                    fieldTrust.SetTrust(ExplicitFieldTrust::kNone);
                    break;

                default:
                    _ASSERTE(!"Can't get here.");
            }
        } else {
            _ASSERTE(!"Can't get here.");
        }
    }

    return explicitClassTrust.GetTrustLevel();
}







//*******************************************************************************
void MethodTableBuilder::FindPointerSeriesExplicit(UINT instanceSliceSize,
                                                   BYTE *pFieldLayout)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    // allocate a structure to track the series. We know that the worst case is a oref-non-oref-non
    // so would the number of series is total instance size div 2 div size of oref.
    // But watch out for the case where we have e.g. an instanceSlizeSize of 4.
    DWORD sz = (instanceSliceSize + (2 * sizeof(OBJECTREF)) - 1);
    bmtGCSeries->pSeries = new bmtGCSeriesInfo::Series[sz/2/sizeof(OBJECTREF)];

    BYTE *loc = pFieldLayout;
    BYTE *layoutEnd = pFieldLayout + instanceSliceSize;
    while (loc < layoutEnd) {
        loc = (BYTE*)memchr((void*)loc, oref, layoutEnd-loc);
        if (!loc)
            break;
        BYTE *cur = loc;
        while(*cur == oref && cur < layoutEnd)
            cur++;
        // so we have a GC series at loc for cur-loc bytes
        bmtGCSeries->pSeries[bmtGCSeries->numSeries].offset = (DWORD)(loc - pFieldLayout);
        bmtGCSeries->pSeries[bmtGCSeries->numSeries].len = (DWORD)(cur - loc);

        CONSISTENCY_CHECK(IS_ALIGNED(cur - loc, sizeof(size_t)));

        bmtGCSeries->numSeries++;
        loc = cur;
    }

    SetNumGCPointerSeries(bmtGCSeries->numSeries + (bmtParent->pParentMethodTable ? bmtParent->pParentMethodTable->GetClass()->m_wNumGCPointerSeries : 0));
}

//*******************************************************************************
VOID MethodTableBuilder::HandleGCForExplicitLayout()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        FORBID_FAULT;
    }
    CONTRACTL_END


    if (! bmtGCSeries->numSeries)
    {
        delete [] bmtGCSeries->pSeries;
        bmtGCSeries->pSeries = NULL;

        return;
    }

    MethodTable *pMT = GetHalfBakedMethodTable();
    pMT->SetContainsPointers();

    // Copy the pointer series map from the parent
    CGCDesc::Init( (PVOID) pMT, pMT->GetNumGCPointerSeries() );
    EEClass *pParentClass = bmtParent->pParentMethodTable ? bmtParent->pParentMethodTable->GetClass() : NULL;
    if (pParentClass && pParentClass->m_wNumGCPointerSeries > 0)
    {
        size_t ParentGCSize = CGCDesc::ComputeSize(pParentClass->m_wNumGCPointerSeries);
        memcpy( (PVOID) (((BYTE*) pMT) - ParentGCSize),  (PVOID) (((BYTE*) bmtParent->pParentMethodTable) - ParentGCSize), ParentGCSize - sizeof(UINT) );

    }

    // Build the pointer series map for this pointers in this instance
    CGCDescSeries *pSeries = ((CGCDesc*)pMT)->GetLowestSeries();
    for (UINT i=0; i < bmtGCSeries->numSeries; i++) {
        // See gcdesc.h for an explanation of why we adjust by subtracting BaseSize
        BAD_FORMAT_NOTHROW_ASSERT(pSeries <= CGCDesc::GetCGCDescFromMT(pMT)->GetHighestSeries());

        pSeries->SetSeriesSize( (size_t) bmtGCSeries->pSeries[i].len - (size_t) pMT->GetBaseSize() );
        pSeries->SetSeriesOffset(bmtGCSeries->pSeries[i].offset + sizeof(Object) + InstanceSliceOffsetForExplicit(TRUE, bmtParent->pParentMethodTable));
        pSeries++;
    }
    delete [] bmtGCSeries->pSeries;
    bmtGCSeries->pSeries = NULL;
}

static BOOL InsertMethodTable(MethodTable *pNew, MethodTable **pArray, DWORD *pNumAssigned)
{
    LEAF_CONTRACT;

    for (DWORD j = 0; j < (*pNumAssigned); j++)
    {
        if (pNew == pArray[j])
        {
#ifdef _DEBUG
            LOG((LF_CLASSLOADER, LL_INFO1000, "GENERICS: Found duplicate interface %s (%p) at position %d out of %d\n", pNew->GetDebugClassName(), pNew, j, *pNumAssigned));
#endif
            return pNew->HasInstantiation(); // bail out - we found a duplicate instantiated interface
        }
        else
        {
#ifdef _DEBUG
            LOG((LF_CLASSLOADER, LL_INFO1000, "  GENERICS: InsertMethodTable ignored interface %s (%p) at position %d out of %d\n", pArray[j]->GetDebugClassName(), pArray[j], j, *pNumAssigned));
#endif
        }
    }
    LOG((LF_CLASSLOADER, LL_INFO1000, "GENERICS: Inserting interface %s (%p) at position %d\n", pNew->GetDebugClassName(), pNew, *pNumAssigned));
    pArray[(*pNumAssigned)++] = pNew;
    return FALSE;
}


BOOL ClassLoader::LoadExactParentAndInterfacesTransitively(MethodTable *pMT)
{

    CONTRACT(BOOL)
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pMT));
        POSTCONDITION(pMT->CheckLoadLevel(CLASS_LOAD_EXACTPARENTS));
    }
    CONTRACT_END;


    TypeHandle thisTH(pMT);
    SigTypeContext typeContext(thisTH);
    IMDInternalImport* pInternalImport = pMT->GetMDImport();
    MethodTable *pParentMT = pMT->GetParentMethodTable();

    if (!pMT->IsArray())
    {
        // Fill in exact parent if it's instantiated
        mdToken crExtends;
        pInternalImport->GetTypeDefProps(pMT->GetCl(),
                                         NULL,
                                         &crExtends);

        BOOL parentChanged = FALSE;
        if (!IsNilToken(crExtends) && TypeFromToken(crExtends) == mdtTypeSpec)
        {
            TypeHandle newParent = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pMT->GetModule(), crExtends, &typeContext,
                                                                               ClassLoader::ThrowIfNotFound,
                                                                               ClassLoader::FailIfUninstDefOrRef,
                                                                               ClassLoader::LoadTypes,
                                                                               CLASS_LOAD_EXACTPARENTS,
                                                                               TRUE);

            MethodTable* pNewParentMT = newParent.AsMethodTable();
            if (pNewParentMT != pParentMT)
            {
#ifdef _DEBUG
                LOG((LF_CLASSLOADER, LL_INFO1000, "GENERICS: Replaced approximate parent %s with exact parent %s from token %x\n", pParentMT->GetDebugClassName(), pNewParentMT->GetDebugClassName(), crExtends));
#endif
                pMT->SetParentMethodTable (pNewParentMT);
                pParentMT = pNewParentMT;
                parentChanged = TRUE;
            }
        }
        else if (pParentMT != NULL)
        {
            EnsureLoaded(pParentMT, CLASS_LOAD_EXACTPARENTS);
        }

    }

    // Restore action, not in MethodTable::Restore because we may have had approx parents at that point
    if (pMT->IsZapped())
    {
        for (MethodTable *pChain = pMT; pChain != NULL; pChain = pChain->GetParentMethodTable())
        {
            if (pChain->HasInstantiation())
            {
                _ASSERTE(pMT->GetPerInstInfo() != NULL);

                // Copy down all inherited dictionary pointers which we
                // could not embed.
                DWORD dictNum = pChain->GetClass()->GetNumDicts()-1;

                Dictionary **ppDict = pMT->GetPerInstInfo() + dictNum;
                if (*ppDict == NULL)
                {
                    *ppDict = pChain->GetPerInstInfo()[dictNum];
                }
            }
        }
    }

    BOOL hasInstantiatedInterfaces = FALSE;
    MethodTable::InterfaceMapIterator it = pMT->IterateInterfaceMap();
    while (it.Next())
    {
        if (it.GetInterface()->HasInstantiation())
        {
            hasInstantiatedInterfaces = TRUE;
            break;
        }
    }

    // If we have some instantiated interfaces, then we have lots more work to do...

    // In the worst case we have to use the metadata to
    //  (a) load the exact interfaces and determine the order in which they
    //      go.  We do those by re-running the interface layout algorithm
    //      and using metadata-comparisons to place interfaces in the list.
    //  (b) do a check to see if any ambiguity in the interface dispatch map is introduced
    //      by the instantiation
    //
    // However, we can do something simpler: we just use
    // the loaded interface method tables to determine ordering.  This can be done
    // if there are no duplicate instantiated interfaces in the interface
    // set.

    if (hasInstantiatedInterfaces)
    {

        // Exact interface instantiation loading TECHNIQUE 1.
        // (a) For interfaces inherited from an instantiated parent class, just copy down from exact parent
        // (b) Grab newly declared interfaces by loading and then copying down all their inherited parents
        // (c) But check for any exact duplicates along the way
        // (d) If no duplicates then we can use the computed interface map we've created
        // (e) If duplicates found then use the slow metadata-based technique
        MethodTable **pExactMTs = (MethodTable**) _alloca(sizeof(MethodTable *) * pMT->GetNumInterfaces());
        DWORD nAssigned = 0;
        BOOL duplicates = false;
        if (pParentMT != NULL)
        {
            MethodTable::InterfaceMapIterator parentIt = pParentMT->IterateInterfaceMap();
            while (parentIt.Next())
            {
            LOG((LF_CLASSLOADER, LL_INFO1000, "PHASEDLOAD: Calling InsertMethodTable on imap for %s with interface %s and %d assigned so far\n", pMT->GetDebugClassName(), parentIt.GetInterface()->GetDebugClassName(), nAssigned));

                duplicates |= InsertMethodTable(parentIt.GetInterface(), pExactMTs, &nAssigned);
            }
        }
        InterfaceImplEnum ie(pMT->GetModule(), pMT->GetCl(), NULL);
        while (ie.Next())
        {
            MethodTable *pNewIntfMT = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pMT->GetModule(),
                                                                                  ie.CurrentToken(),
                                                                                  &typeContext,
                                                                                  ClassLoader::ThrowIfNotFound,
                                                                                  ClassLoader::FailIfUninstDefOrRef,
                                                                                  ClassLoader::LoadTypes,
                                                                                  CLASS_LOAD_EXACTPARENTS,
                                                                                  TRUE).GetMethodTable();

            LOG((LF_CLASSLOADER, LL_INFO1000, "PHASEDLOAD: Calling InsertMethodTable on imap for %s with interface %s and %d assigned so far\n", pMT->GetDebugClassName(), pNewIntfMT->GetDebugClassName(), nAssigned));
            duplicates |= InsertMethodTable(pNewIntfMT, pExactMTs, &nAssigned);
            MethodTable::InterfaceMapIterator intIt = pNewIntfMT->IterateInterfaceMap();
            while (intIt.Next())
            {
            LOG((LF_CLASSLOADER, LL_INFO1000, "PHASEDLOAD: Calling InsertMethodTable on imap for %s with interface %s and %d assigned so far\n", pMT->GetDebugClassName(), intIt.GetInterface()->GetDebugClassName(), nAssigned));
                duplicates |= InsertMethodTable(intIt.GetInterface(), pExactMTs, &nAssigned);
            }
        }
#ifdef _DEBUG
        duplicates |= EEConfig::GetConfigDWORD(L"AlwaysUseMetadataInterfaceMapLayout", FALSE);
#endif
        CONSISTENCY_CHECK(duplicates || nAssigned == pMT->GetNumInterfaces());

        if (duplicates)
        {
            //
            //
            //

            Thread *pThread = GetThread();
            CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

            // ***********************************************************
            // ****** This must be consistent with ExpandApproxInterface etc. *******
            //
            // The correlation to ExpandApproxInterfaces etc. simply drops out by how we
            // traverse interfaces.
            // ***********************************************************

            MethodTableBuilder::bmtExactInterfaceInfo bmtExactInterface;
            bmtExactInterface.ppInterfaceSubstitutionChains = (Substitution**) pThread->m_MarshalAlloc.Alloc(sizeof(Substitution *) * pMT->GetNumInterfaces());
            bmtExactInterface.pExactMTs = pExactMTs;
            bmtExactInterface.nAssigned = 0;
            bmtExactInterface.typeContext = typeContext;

            // Do the interfaces inherited from a parent class
            if (pParentMT != NULL && pParentMT->GetNumInterfaces() > 0)
            {
                Substitution parentSubst = pMT->GetClass()->GetSubstitutionForParent(NULL);
                MethodTableBuilder::ExpandExactInheritedInterfaces(&bmtExactInterface,pParentMT,&parentSubst);
            }
            _ASSERTE(pParentMT->GetNumInterfaces() == bmtExactInterface.nAssigned);

            MethodTableBuilder::bmtInterfaceAmbiguityCheckInfo bmtCheckInfo;
            bmtCheckInfo.pMT = pMT;
            bmtCheckInfo.ppInterfaceSubstitutionChains = (Substitution**) pThread->m_MarshalAlloc.Alloc(sizeof(Substitution *) * pMT->GetNumInterfaces());
            bmtCheckInfo.ppExactDeclaredInterfaces = (MethodTable**) pThread->m_MarshalAlloc.Alloc(sizeof(MethodTable *) * pMT->GetNumInterfaces());
            bmtCheckInfo.nAssigned = 0;
            bmtCheckInfo.typeContext = typeContext;
            MethodTableBuilder::InterfacesAmbiguityCheck(&bmtCheckInfo, pMT->GetModule(), pMT->GetCl(), NULL);

            // OK, there is no ambiguity amongst the instantiated interfaces declared on this class.
            MethodTableBuilder::ExpandExactDeclaredInterfaces(&bmtExactInterface, pMT->GetModule(), pMT->GetCl(), NULL);
            CONSISTENCY_CHECK(bmtExactInterface.nAssigned == pMT->GetNumInterfaces());
        }





        // OK, if we've got this far then pExactMTs should now hold the array of exact instantiated interfaces.
        MethodTable::InterfaceMapIterator thisIt = pMT->IterateInterfaceMap();
        DWORD i = 0;
        while (thisIt.Next())
        {
#ifdef _DEBUG
            MethodTable*pOldMT = thisIt.GetInterface();
            MethodTable *pNewMT = pExactMTs[i];
            CONSISTENCY_CHECK(pOldMT->HasSameTypeDefAs(pNewMT));
#endif
            thisIt.SetInterface(pExactMTs[i]);
            i++;
        }

    }
    // We can now mark this type as having exact parents
    pMT->SetHasExactParent();

    RETURN FALSE;
}

// CLASS_LOAD_EXACTPARENTS phase of loading:
// * Load the base class at exact instantiation
// * Recurse LoadExactParents up parent hierarchy
// * Load explicitly declared interfaces on this class at exact instantiation
// * Fixup vtable (STUB_DISPATCH off)
//
/*static*/
void ClassLoader::LoadExactParents(MethodTable *pMT)
{

    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pMT));
        POSTCONDITION(pMT->CheckLoadLevel(CLASS_LOAD_EXACTPARENTS));
    }
    CONTRACT_END;

    if (!pMT->IsCanonicalMethodTable())
    {
        EnsureLoaded(TypeHandle(pMT->GetCanonicalMethodTable()), CLASS_LOAD_EXACTPARENTS);
    }
    LoadExactParentAndInterfacesTransitively(pMT);

    // Copy down inherited dictionary pointers
    MethodTable* pParentMT = pMT->GetParentMethodTable();
    if (pMT->GetPerInstInfo() && pParentMT && pParentMT->GetPerInstInfo())
    {
        memcpy(pMT->GetPerInstInfo(), pParentMT->GetPerInstInfo(),
               sizeof(TypeHandle*) * pParentMT->GetNumDicts());
    }

    //fix up wrongly-inherited method descriptors </STRIP>
    for (DWORD i = 0; i < pMT->GetNumParentVirtuals(); i++)
    {
        MethodDesc* pMD = pMT->GetUnknownMethodDescForSlot(i);
        if (pMD->GetCanonicalMethodTable() != pMT->GetCanonicalMethodTable())
        {
            pMT->SetSlot(i, pParentMT->GetRestoredSlot(i));
        }
    }

    RETURN;
}

/* static */
void MethodTableBuilder::LoadExactInterfaceMap(MethodTable *pMT)
{

    CONTRACT_VOID
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pMT));
    }
    CONTRACT_END;


    TypeHandle thisTH(pMT);
    SigTypeContext typeContext(thisTH);
    MethodTable *pParentMT = pMT->GetParentMethodTable();

    BOOL hasInstantiatedInterfaces = FALSE;
    MethodTable::InterfaceMapIterator it = pMT->IterateInterfaceMap();
    while (it.Next())
    {
        if (it.GetInterface()->HasInstantiation())
        {
            hasInstantiatedInterfaces = TRUE;
            break;
        }
    }

    // If we have some instantiated interfaces, then we have lots more work to do...

    // In the worst case we have to use the metadata to
    //  (a) load the exact interfaces and determine the order in which they
    //      go.  We do those by re-running the interface layout algorithm
    //      and using metadata-comparisons to place interfaces in the list.
    //  (b) do a check to see if any ambiguity in the interface dispatch map is introduced
    //      by the instantiation
    //
    // However, we can do something simpler: we just use
    // the loaded interface method tables to determine ordering.  This can be done
    // if there are no duplicate instantiated interfaces in the interface
    // set.

    if (hasInstantiatedInterfaces)
    {

        // Exact interface instantiation loading TECHNIQUE 1.
        // (a) For interfaces inherited from an instantiated parent class, just copy down from exact parent
        // (b) Grab newly declared interfaces by loading and then copying down all their inherited parents
        // (c) But check for any exact duplicates along the way
        // (d) If no duplicates then we can use the computed interface map we've created
        // (e) If duplicates found then use the slow metadata-based technique
        MethodTable **pExactMTs = (MethodTable**) _alloca(sizeof(MethodTable *) * pMT->GetNumInterfaces());
        DWORD nAssigned = 0;
        BOOL duplicates = false;
        if (pParentMT != NULL)
        {
            MethodTable::InterfaceMapIterator parentIt = pParentMT->IterateInterfaceMap();
            while (parentIt.Next())
            {
                duplicates |= InsertMethodTable(parentIt.GetInterface(), pExactMTs, &nAssigned);
            }
        }
        InterfaceImplEnum ie(pMT->GetModule(), pMT->GetCl(), NULL);
        while (ie.Next())
        {
            MethodTable *pNewIntfMT = ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pMT->GetModule(),
                                                                                  ie.CurrentToken(),
                                                                                  &typeContext,
                                                                                  ClassLoader::ThrowIfNotFound,
                                                                                  ClassLoader::FailIfUninstDefOrRef,
                                                                                  ClassLoader::LoadTypes,
                                                                                  CLASS_LOAD_EXACTPARENTS,
                                                                                  TRUE).GetMethodTable();

            duplicates |= InsertMethodTable(pNewIntfMT, pExactMTs, &nAssigned);
            MethodTable::InterfaceMapIterator intIt = pNewIntfMT->IterateInterfaceMap();
            while (intIt.Next())
            {
                duplicates |= InsertMethodTable(intIt.GetInterface(), pExactMTs, &nAssigned);
            }
        }
#ifdef _DEBUG
        duplicates |= EEConfig::GetConfigDWORD(L"AlwaysUseMetadataInterfaceMapLayout", FALSE);
#endif
        CONSISTENCY_CHECK(duplicates || nAssigned == pMT->GetNumInterfaces());
        if (duplicates)
        {

            //
            //
            //

            Thread *pThread = GetThread();
            CheckPointHolder cph(pThread->m_MarshalAlloc.GetCheckpoint()); //hold checkpoint for autorelease

            // ***********************************************************
            // ****** This must be consistent with ExpandApproxInterface etc. *******
            //
            // The correlation to ExpandApproxInterfaces etc. simply drops out by how we
            // traverse interfaces.
            // ***********************************************************

            bmtExactInterfaceInfo bmtExactInterface;
            bmtExactInterface.ppInterfaceSubstitutionChains = (Substitution**) pThread->m_MarshalAlloc.Alloc(sizeof(Substitution *) * pMT->GetNumInterfaces());
            bmtExactInterface.pExactMTs = pExactMTs;
            bmtExactInterface.nAssigned = 0;
            bmtExactInterface.typeContext = typeContext;

            // Do the interfaces inherited from a parent class
            if (pParentMT != NULL && pParentMT->GetNumInterfaces() > 0)
            {
                Substitution parentSubst = pMT->GetClass()->GetSubstitutionForParent(NULL);
                ExpandExactInheritedInterfaces(&bmtExactInterface,pParentMT,&parentSubst);
            }
            _ASSERTE(pParentMT->GetNumInterfaces() == bmtExactInterface.nAssigned);

            bmtInterfaceAmbiguityCheckInfo bmtCheckInfo;
            bmtCheckInfo.pMT = pMT;
            bmtCheckInfo.ppInterfaceSubstitutionChains = (Substitution**) pThread->m_MarshalAlloc.Alloc(sizeof(Substitution *) * pMT->GetNumInterfaces());
            bmtCheckInfo.ppExactDeclaredInterfaces = (MethodTable**) pThread->m_MarshalAlloc.Alloc(sizeof(MethodTable *) * pMT->GetNumInterfaces());
            bmtCheckInfo.nAssigned = 0;
            bmtCheckInfo.typeContext = typeContext;
            MethodTableBuilder::InterfacesAmbiguityCheck(&bmtCheckInfo, pMT->GetModule(), pMT->GetCl(), NULL);

            // OK, there is no ambiguity amongst the instantiated interfaces declared on this class.
            MethodTableBuilder::ExpandExactDeclaredInterfaces(&bmtExactInterface, pMT->GetModule(), pMT->GetCl(), NULL);
            CONSISTENCY_CHECK(bmtExactInterface.nAssigned == pMT->GetNumInterfaces());
        }

        // OK, if we've got this far then pExactMTs should now hold the array of exact instantiated interfaces.
        MethodTable::InterfaceMapIterator thisIt = pMT->IterateInterfaceMap();
        DWORD i = 0;
        while (thisIt.Next())
        {
#ifdef _DEBUG
            MethodTable*pOldMT = thisIt.GetInterface();
            MethodTable *pNewMT = pExactMTs[i];
            CONSISTENCY_CHECK(pOldMT->HasSameTypeDefAs(pNewMT));
#endif // _DEBUG
            thisIt.SetInterface(pExactMTs[i]);
            i++;
        }
    }
    RETURN;
}

//*******************************************************************************
void MethodTableBuilder::ExpandExactInheritedInterfaces(bmtExactInterfaceInfo *bmtInfo,
                                                        MethodTable *pMT,
                                                        const Substitution *pSubstChain)
{
    WRAPPER_CONTRACT;

    MethodTable *pParentMT = pMT->GetParentMethodTable();
    if (pParentMT)
    {
        Substitution parentSubst = pMT->GetClass()->GetSubstitutionForParent(pSubstChain);
        ExpandExactInheritedInterfaces(bmtInfo,pParentMT,&parentSubst);
    }
    ExpandExactDeclaredInterfaces(bmtInfo,pMT->GetModule(),pMT->GetCl(),pSubstChain);
}

//*******************************************************************************
/* static */
void MethodTableBuilder::ExpandExactDeclaredInterfaces(bmtExactInterfaceInfo *bmtInfo,
                                                       Module *pModule,
                                                       mdToken typeDef,
                                                       const Substitution *pSubstChain)
{
    WRAPPER_CONTRACT;

    InterfaceImplEnum ie(pModule, typeDef, pSubstChain);
    while (ie.Next())
    {
        MethodTable *pInterface =
            ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pModule, ie.CurrentToken(),
                                                        &bmtInfo->typeContext,
                                                        ClassLoader::ThrowIfNotFound,
                                                        ClassLoader::FailIfUninstDefOrRef,
                                                        ClassLoader::LoadTypes,
                                                        CLASS_LOAD_EXACTPARENTS,
                                                        TRUE,
                                                        pSubstChain).GetMethodTable();

        ExpandExactInterface(bmtInfo, ie.CurrentSubst(), pInterface);
    }
}

//*******************************************************************************
void MethodTableBuilder::ExpandExactInterface(bmtExactInterfaceInfo *bmtInfo,
                                              const Substitution *pItfSubstChain,
                                              MethodTable *pIntf)
{
    WRAPPER_CONTRACT;

    // Is it already present according to the "generic" layout of the interfaces.
    // Note we use exactly the same algorithm as when we
    // determined the layout of the interface map for the "generic" version of the class.
    for (DWORD i = 0; i < bmtInfo->nAssigned; i++)
    {
        if (MetaSig::CompareTypeDefsUnderSubstitutions(bmtInfo->pExactMTs[i],
                                                       pIntf,
                                                       bmtInfo->ppInterfaceSubstitutionChains[i],
                                                       pItfSubstChain))
            return; // found it, don't add it again
    }

    // Add it and each sub-interface
    // Also save the substitution chain for future comparisons during this run of ExpandExactInterfaces
    DWORD n = bmtInfo->nAssigned;
    bmtInfo->pExactMTs[n] = pIntf;
    bmtInfo->ppInterfaceSubstitutionChains[n] = (Substitution *) GetThread()->m_MarshalAlloc.Alloc(sizeof(Substitution) * pItfSubstChain->GetLength());
    pItfSubstChain->CopyToArray(bmtInfo->ppInterfaceSubstitutionChains[n]);
    bmtInfo->nAssigned++;

    ExpandExactDeclaredInterfaces(bmtInfo, pIntf->GetModule(), pIntf->GetCl(), pItfSubstChain);
}

//*******************************************************************************
/* static */
void MethodTableBuilder::InterfacesAmbiguityCheck(bmtInterfaceAmbiguityCheckInfo *bmtCheckInfo,
                                                  Module *pModule,
                                                  mdToken typeDef,
                                                  const Substitution *pSubstChain)
{

    WRAPPER_CONTRACT;

    InterfaceImplEnum ie(pModule, typeDef, pSubstChain);
    while (ie.Next())
    {
        MethodTable *pInterface =
            ClassLoader::LoadTypeDefOrRefOrSpecThrowing(pModule, ie.CurrentToken(),
                                                        &bmtCheckInfo->typeContext,
                                                        ClassLoader::ThrowIfNotFound,
                                                        ClassLoader::FailIfUninstDefOrRef,
                                                        ClassLoader::LoadTypes,
                                                        CLASS_LOAD_EXACTPARENTS,
                                                        TRUE,
                                                        pSubstChain).GetMethodTable();
        InterfaceAmbiguityCheck(bmtCheckInfo, ie.CurrentSubst(), pInterface);
    }
}

//*******************************************************************************
void MethodTableBuilder::InterfaceAmbiguityCheck(bmtInterfaceAmbiguityCheckInfo *bmtCheckInfo,
                                                 const Substitution *pItfSubstChain,
                                                 MethodTable *pIntf)
{
    WRAPPER_CONTRACT;

    for (DWORD i = 0; i < bmtCheckInfo->nAssigned; i++)
    {
        if (MetaSig::CompareTypeDefsUnderSubstitutions(bmtCheckInfo->ppExactDeclaredInterfaces[i],
                                                       pIntf,
                                                       bmtCheckInfo->ppInterfaceSubstitutionChains[i],
                                                       pItfSubstChain))
            return; // found it, don't add it again
    }

    // OK, so it isn't a duplicate based on the generic IL, now check if the instantiation
    // makes it a duplicate.
    for (DWORD i = 0; i < bmtCheckInfo->nAssigned; i++)
    {
        if (bmtCheckInfo->ppExactDeclaredInterfaces[i] == pIntf)
        {
                bmtCheckInfo->pMT->GetModule()->GetAssembly()->ThrowTypeLoadException(bmtCheckInfo->pMT->GetModule()->GetMDImport(),
                                                                                      bmtCheckInfo->pMT->GetCl(),
                                                                                      IDS_CLASSLOAD_OVERLAPPING_INTERFACES);
        }
    }

    DWORD n = bmtCheckInfo->nAssigned;
    bmtCheckInfo->ppExactDeclaredInterfaces[n] = pIntf;
    bmtCheckInfo->ppInterfaceSubstitutionChains[n] = (Substitution *) GetThread()->m_MarshalAlloc.Alloc(sizeof(Substitution) * pItfSubstChain->GetLength());
    pItfSubstChain->CopyToArray(bmtCheckInfo->ppInterfaceSubstitutionChains[n]);

    bmtCheckInfo->nAssigned++;
    InterfacesAmbiguityCheck(bmtCheckInfo,pIntf->GetModule(),pIntf->GetCl(),pItfSubstChain);
}

//*******************************************************************************
int __cdecl CompareUnknownSlotAddressSlotNumbers(const void *md1, const void *md2)
{
    WRAPPER_CONTRACT;
    MethodDesc *pMD1 = MethodTable::GetUnknownMethodDescForSlotAddress(*((SLOT*)md1));
    MethodDesc *pMD2 = MethodTable::GetUnknownMethodDescForSlotAddress(*((SLOT*)md2));
    return (int)pMD1->GetSlot() - (int)pMD2->GetSlot();
}

//*******************************************************************************
// Private helper method used by the code below to check whether the given
// method is annotated to be a VTS event callback.
BOOL MethodTableBuilder::CheckForVtsEventMethod(IMDInternalImport  *pImport,
                                                MethodDesc         *pMD,
                                                DWORD               dwAttrs,
                                                LPCUTF8             szAttrName,
                                                MethodDesc        **ppMethodDesc)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    // For each method with an attriubte we need to check that:
    //   o  The method is not static, virtual, abstract or generic.
    //   o  The signature is correct.
    //   o  No other method on the same type is marked with the same
    //      attribute.

    if (pImport->GetCustomAttributeByName(pMD->GetMemberDef(),
                                          szAttrName,
                                          NULL,
                                          NULL) == S_OK)
    {
        if (IsMdStatic(dwAttrs) ||
            IsMdVirtual(dwAttrs) ||
            IsMdAbstract(dwAttrs) ||
            pMD->IsGenericMethodDefinition())
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_INVALID_VTS_METHOD, pMD->GetMemberDef());
        }

        // Check whether we've seen one of these methods before.
        if (*ppMethodDesc != NULL)
        {
            BuildMethodTableThrowException(IDS_CLASSLOAD_TOO_MANY_VTS_METHODS, szAttrName);
        }

        // Check the signature, it should be "void M(StreamingContext)".
        DWORD           cbSig;
        PCCOR_SIGNATURE pSig = pImport->GetSigOfMethodDef(pMD->GetMemberDef(), &cbSig);

        // Should be an instance method with no generic type parameters.
        if (CorSigUncompressCallingConv(pSig) != IMAGE_CEE_CS_CALLCONV_HASTHIS)
            goto BadSignature;

        // Should have one argument.
        if (CorSigUncompressData(pSig) != 1)
            goto BadSignature;

        // And a return type of void.
        if (*pSig++ != (BYTE)ELEMENT_TYPE_VOID)
            goto BadSignature;

        // The argument should be a value type.
        if (*pSig++ != (BYTE)ELEMENT_TYPE_VALUETYPE)
            goto BadSignature;

        // Now the tricky bit: we want to verify the value type is
        // StreamingContext, but we don't want to simply load the type since it
        // might be any other arbitrary type and cause recursive loading
        // problems. SO we manually check the type via the metadata APIs
        // instead.
        mdToken tkType = CorSigUncompressToken(pSig);
        LPCUTF8 szType;
        LPCUTF8 szNamespace;

        // Compute type name and namespace.
        if (TypeFromToken(tkType) == mdtTypeDef)
        {
            pImport->GetNameOfTypeDef(tkType, &szType, &szNamespace);
        }
        else
        {
            _ASSERTE(TypeFromToken(tkType) == mdtTypeRef);
            pImport->GetNameOfTypeRef(tkType, &szNamespace, &szType);
        }

        // Do the names match?
        if (strcmp(szType, g_Mscorlib.GetClassName(CLASS__STREAMING_CONTEXT)) != 0 ||
            strcmp(szNamespace, g_Mscorlib.GetClassNameSpace(CLASS__STREAMING_CONTEXT)))
            goto BadSignature;

        // For typedefs we can directly check whether the current module is
        // part of mscorlib. For refs we have to dig deeper (into the token
        // resolution scope).
        if (TypeFromToken(tkType) == mdtTypeDef)
        {
            if (bmtError->pModule->GetAssembly()->GetManifestModule() != SystemDomain::SystemAssembly()->GetManifestModule())
                goto BadSignature;
        }
        else
        {
            // The scope needs to be an assembly ref.
            mdToken tkScope = pImport->GetResolutionScopeOfTypeRef(tkType);
            if (TypeFromToken(tkScope) != mdtAssemblyRef)
                goto BadSignature;

            // Fetch the name and public key or public key token.
            BYTE *pbPublicKeyOrToken;
            DWORD cbPublicKeyOrToken;
            LPCSTR szAssembly;
            DWORD dwAssemblyFlags;
            pImport->GetAssemblyRefProps(tkScope,
                                         (const void**)&pbPublicKeyOrToken,
                                         &cbPublicKeyOrToken,
                                         &szAssembly,
                                         NULL, // AssemblyMetaDataInternal: we don't care about version, culture etc.
                                         NULL, // Hash value pointer, obsolete information
                                         NULL, // Byte count for above
                                         &dwAssemblyFlags);

            // Validate the name.
            if (stricmpUTF8(szAssembly, g_psBaseLibraryName) != 0)
                goto BadSignature;

            // And the public key or token, whichever was burned into the reference by the compiler. For mscorlib this is the ECMA key or
            // token.
            if (IsAfPublicKeyToken(dwAssemblyFlags))
            {
                if (cbPublicKeyOrToken != sizeof(g_rbNeutralPublicKeyToken) ||
                    memcmp(pbPublicKeyOrToken, g_rbNeutralPublicKeyToken, cbPublicKeyOrToken) != 0)
                    goto BadSignature;
            }
            else
            {
                if (cbPublicKeyOrToken != sizeof(g_rbNeutralPublicKey) ||
                    memcmp(pbPublicKeyOrToken, g_rbNeutralPublicKey, cbPublicKeyOrToken) != 0)
                    goto BadSignature;
            }
        }

        // We managed to pass all tests; record this method.
        *ppMethodDesc = pMD;

        return TRUE;
    }

    return FALSE;

 BadSignature:
    BuildMethodTableThrowException(IDS_CLASSLOAD_INVALID_VTS_SIG, pMD->GetMemberDef());
}

//*******************************************************************************
// Names of the various VTS custom attributes
#define VTS_ON_SERIALIZING_ATTRIBUTE    "System.Runtime.Serialization.OnSerializingAttribute"
#define VTS_ON_SERIALIZED_ATTRIBUTE     "System.Runtime.Serialization.OnSerializedAttribute"
#define VTS_ON_DESERIALIZING_ATTRIBUTE  "System.Runtime.Serialization.OnDeserializingAttribute"
#define VTS_ON_DESERIALIZED_ATTRIBUTE   "System.Runtime.Serialization.OnDeserializedAttribute"
#define VTS_OPTIONAL_FIELD_ATTRIBUTE    "System.Runtime.Serialization.OptionalFieldAttribute"

//*******************************************************************************
// Look for VTS event methods or fields with interesting serialization
// attributes on this type (only called for serializable types).
VOID MethodTableBuilder::ScanTypeForVtsInfo()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(IsTdSerializable(GetAttrClass()));
    }
    CONTRACTL_END;

    DWORD i;

    // Scan all the non-virtual, non-abstract, non-generic instance methods for
    // one of the special custom attributes indicating a VTS event method.
    DeclaredMethodIterator it(*this);
    while (it.Next())
    {
        if (CheckForVtsEventMethod(bmtInternal->pInternalImport,
                                   it.GetMethodDesc(),
                                   it.Attrs(),
                                   VTS_ON_SERIALIZING_ATTRIBUTE,
                                   &bmtMFDescs->pOnSerializingMethod))
            bmtMFDescs->fNeedsRemotingVtsInfo = true;

        if (CheckForVtsEventMethod(bmtInternal->pInternalImport,
                                   it.GetMethodDesc(),
                                   it.Attrs(),
                                   VTS_ON_SERIALIZED_ATTRIBUTE,
                                   &bmtMFDescs->pOnSerializedMethod))
            bmtMFDescs->fNeedsRemotingVtsInfo = true;

        if (CheckForVtsEventMethod(bmtInternal->pInternalImport,
                                   it.GetMethodDesc(),
                                   it.Attrs(),
                                   VTS_ON_DESERIALIZING_ATTRIBUTE,
                                   &bmtMFDescs->pOnDeserializingMethod))
            bmtMFDescs->fNeedsRemotingVtsInfo = true;

        if (CheckForVtsEventMethod(bmtInternal->pInternalImport,
                                   it.GetMethodDesc(),
                                   it.Attrs(),
                                   VTS_ON_DESERIALIZED_ATTRIBUTE,
                                   &bmtMFDescs->pOnDeserializedMethod))
            bmtMFDescs->fNeedsRemotingVtsInfo = true;
    }

    // Scan all the instance fields introduced on this type for NotSerialized or
    // OptionalField attributes.
    DWORD dwNumIntroducedInstanceFields = bmtEnumMF->dwNumInstanceFields;
    FieldDesc *pFieldDescList = GetHalfBakedClass()->GetApproxFieldDescListRaw();
    for (i = 0; i < dwNumIntroducedInstanceFields; i++)
    {
        FieldDesc *pFD = &pFieldDescList[i];

        if (IsFdNotSerialized(bmtInternal->pInternalImport->GetFieldDefProps(pFD->GetMemberDef())))
            bmtMFDescs->SetFieldNotSerialized(i, dwNumIntroducedInstanceFields);

        if (bmtInternal->pInternalImport->GetCustomAttributeByName(pFD->GetMemberDef(),
                                                                   VTS_OPTIONAL_FIELD_ATTRIBUTE,
                                                                   NULL,
                                                                   NULL) == S_OK)
            bmtMFDescs->SetFieldOptionallySerialized(i, dwNumIntroducedInstanceFields);
    }
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Setup the method table
//
VOID   MethodTableBuilder::SetupMethodTable2(AllocMemTracker *pamTracker, Module* pLoaderModule)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtVT));
        PRECONDITION(CheckPointer(bmtInterface));
        PRECONDITION(CheckPointer(bmtInternal));
        PRECONDITION(CheckPointer(bmtProp));
        PRECONDITION(CheckPointer(bmtMFDescs));
        PRECONDITION(CheckPointer(bmtEnumMF));
        PRECONDITION(CheckPointer(bmtError));
        PRECONDITION(CheckPointer(bmtMetaData));
        PRECONDITION(CheckPointer(bmtParent));
        PRECONDITION(CheckPointer(bmtGenerics));
    }
    CONTRACTL_END;

    // When calling GetUnknownMethodDescForSlotAddress below (which is called by
    // bmtVTable::GetMethodDescForSlot()), this calls into MNativeJitManager, which
    // has a NOTHROW contract on all of its methods. But deep in the bowels of this
    // code there is a BEGIN_ENTRYPOINT_VOIDRET in MNativeJitManager::JitCodeToMethodInfo,
    // which probes for enough stack space, and on failure SKIPS CODE AND DOESN'T
    // INFORM THE CALLER OF FAILURE. As such, the call to GetMethodDescForSlot was
    // returning NULL instead of some sort of SO or other failure code. So, to cover
    // this case we probe for the default number of pages plus extra to cover the
    // stack used between here and the call to MNativeJitManager::JitCodeToMethodInfo.
    BEGIN_SO_INTOLERANT_CODE_FOR(GetThread(), DefaultEntryProbeAmount() + 2)

    DWORD i;

    EEClass *pClass = GetHalfBakedClass();

    DWORD cbDict = bmtGenerics->HasInstantiation() ?  DictionaryLayout::GetFirstDictionaryBucketSize(bmtGenerics->GetNumGenericArgs(), pClass->GetDictionaryLayout()) : 0;

    BOOL fHasThreadOrContextStatics = (bmtTCSInfo) ? (bmtTCSInfo->dwThreadStaticsSize | bmtTCSInfo->dwContextStaticsSize) : FALSE;

    BOOL fNeedsRemotableMethodInfo = (bmtProp->fMarshaledByRef || IsInterface() || g_pObjectClass == NULL);

    // Now setup the method table
    // interface map is allocated along with the method table
    MethodTable *pMT =
        MethodTable::AllocateNewMT(pClass,
                                   bmtVT->dwCurrentNonVtableSlot,
                                   pClass->m_wNumGCPointerSeries ? (DWORD)CGCDesc::ComputeSize(pClass->m_wNumGCPointerSeries) : 0,
                                   bmtInterface->dwInterfaceMapSize,
                                   bmtGenerics->GetNumGenericArgs(),
                                   bmtGenerics->numDicts,
                                   cbDict,
                                   GetClassLoader(),
                                   bmtDomain,
                                   IsInterface(),
                                   bmtProp->fGenericsStatics,
                                   fNeedsRemotableMethodInfo,
                                   bmtMFDescs->fNeedsRemotingVtsInfo,
                                   fHasThreadOrContextStatics,
                                   pamTracker);

    pClass->m_pMethodTable = pMT;

#ifdef _DEBUG
    pMT->SetDebugClassName(GetDebugClassName());
#endif

    pMT->SetIsNotFullyLoaded();
    pMT->SetHasApproxParent();

    // Need to do this before anyone inquires about optional data offsets or sizes.
    if (bmtProp->fGenericsStatics)
        pMT->SetHasGenericsStaticsInfo();

    if (bmtMFDescs->fNeedsRemotingVtsInfo)
        pMT->SetHasRemotingVtsInfo();

    if (fHasThreadOrContextStatics)
        pMT->SetHasThreadOrContextStatics();

    if (bmtProp->fDynamicStatics)
        pMT->SetDynamicStatics();

    if (bmtProp->fMarshaledByRef)
        pMT->SetMarshaledByRef();

    if (IsInterface())
        pMT->SetIsInterface();

    // Must be done early because various methods test HasInstantiation() and ContainsGenericVariables()
    if (bmtGenerics->GetNumGenericArgs() != 0)
    {
        pMT->SetHasInstantiation(bmtGenerics->genericsKind == EEClass::VMFLAG_GENERIC_TYPICALINST);
        if (bmtGenerics->fContainsGenericVariables)
            pMT->SetContainsGenericVariables();

    }
    pMT->SetDictInfo(bmtGenerics->numDicts, bmtGenerics->GetNumGenericArgs());

    CONSISTENCY_CHECK(pMT->GetNumGenericArgs() == bmtGenerics->GetNumGenericArgs());
    CONSISTENCY_CHECK(pMT->GetNumDicts() == bmtGenerics->numDicts);
    CONSISTENCY_CHECK(pMT->HasInstantiation() == bmtGenerics->HasInstantiation());
    CONSISTENCY_CHECK(pMT->HasInstantiation() == (pMT->GetInstantiation() != NULL));
    _ASSERTE(bmtInternal->pModule != NULL);
    pMT->SetModule (bmtInternal->pModule);
    pMT->SetLoaderModule(pLoaderModule);
    pMT->SetInternalCorElementType (ELEMENT_TYPE_CLASS);
    SetNonGCStaticFieldBytes (bmtVT->dwNonGCStaticFieldBytes);
    pMT->SetNumVirtuals(bmtVT->dwCurrentVtableSlot);
    pMT->SetClassConstructorSlot(GetClassConstructorSlot());



    PSecurityProperties psp = GetSecurityProperties();
    // Check whether we have any runtime actions such as Demand, Assert etc
    // that can result in methods needing the security stub. We dont care about Linkdemands etc
    if ( !psp->GetRuntimeActions() && !psp->GetNullRuntimeActions())
        pMT->SetNoSecurityProperties();

    pMT->SetCompiledDomainNeutral(bmtDomain->IsSharedDomain());


    pMT->SetClassConstructorSlot (bmtVT->wCCtorSlot);
    pMT->SetDefaultConstructorSlot (bmtVT->wDefaultCtorSlot);

    // Push pointer to method table into the head of each of the method desc
    // chunks we allocated earlier, so that method descs can map back to method
    // tables.
    for (DWORD impl=0; impl<METHOD_IMPL_COUNT; impl++)
    {
        for (DWORD type=0; type<METHOD_TYPE_COUNT; type++)
        {
            bmtMethodDescSet *set = &bmtMFDescs->sets[type][impl];
            for (i=0; i<set->dwChunks; i++)
            {
                set->pChunkList[i]->SetMethodTable(pMT);
            }
        }
    }

#ifdef _DEBUG
    {
        DeclaredMethodIterator it(*this);
        while (it.Next())
        {
            if (it.GetMethodDesc() != NULL)
            {
                it.GetMethodDesc()->m_pDebugMethodTable = pMT;
                it.GetMethodDesc()->m_pszDebugMethodSignature = FormatSig(it.GetMethodDesc(), bmtDomain, pamTracker);
        }
            if (it.GetUnboxedMethodDesc() != NULL)
            {
                it.GetUnboxedMethodDesc()->m_pDebugMethodTable = pMT;
                it.GetUnboxedMethodDesc()->m_pszDebugMethodSignature = FormatSig(it.GetUnboxedMethodDesc(), bmtDomain, pamTracker);
            }
        }
    }
#endif // _DEBUG

    // Note that for value classes, the following calculation is only appropriate
    // when the instance is in its "boxed" state.
    if (!IsInterface())
    {
        DWORD baseSize = (DWORD) MAX(GetNumInstanceFieldBytes() + ObjSizeOf(Object), MIN_OBJECT_SIZE);
        baseSize = (baseSize + ALLOC_ALIGN_CONSTANT) & ~ALLOC_ALIGN_CONSTANT;  // m_BaseSize must be aligned
        pMT->SetBaseSize(baseSize);
        pMT->SetComponentSize(0);
    }
    else
    {
    }
    _ASSERTE((pMT->IsInterface() == 0) == (IsInterface() == 0));

    if (HasLayout())
    {
        pMT->SetNativeSize(GetLayoutInfo()->GetNativeSize());
    }

#ifdef _DEBUG
    for (i = 0; i < bmtVT->dwCurrentNonVtableSlot; i++)
    {
        _ASSERTE(bmtVT->GetMethodDescForSlot(i) != NULL);
    }
#endif // _DEBUG

    FieldDesc *pFieldDescList = pClass->GetApproxFieldDescListRaw();
    // Set all field slots to point to the newly created MethodTable
    for (i = 0; i < (bmtEnumMF->dwNumStaticFields + bmtEnumMF->dwNumInstanceFields); i++)
    {
        pFieldDescList[i].m_pMTOfEnclosingClass = pMT;
    }

    // Fill in type parameters before looking up exact parent or fetching the types of any field descriptors!
    // This must come before the use of GetFieldType in the value class representation optimization below.
    if (bmtGenerics->GetNumGenericArgs() != 0)
    {
      // Space has already been allocated for the instantiation but the parameters haven't been filled in
      TypeHandle *pInstDest = pMT->GetInstantiation();
      TypeHandle *pInst = bmtGenerics->GetInstantiation();
      CONSISTENCY_CHECK(pInst != NULL);
      CONSISTENCY_CHECK(pInstDest != NULL);

      // So fill them in...
      for (DWORD j = 0; j < bmtGenerics->GetNumGenericArgs(); j++)
        pInstDest[j] = pInst[j];
    }

    if (IsValueClass())
    {
        pMT->SetInternalCorElementType (ELEMENT_TYPE_VALUETYPE);
        if (IsEnum())
        {
            if (GetNumInstanceFields() != 1 ||
                !CorTypeInfo::IsPrimitiveType(pFieldDescList[0].GetFieldType()))
            {
                BuildMethodTableThrowException(COR_E_BADIMAGEFORMAT, IDS_CLASSLOAD_BAD_FIELD, mdTokenNil);
            }
            CONSISTENCY_CHECK(!pFieldDescList->IsStatic());
            pMT->SetInternalCorElementType (pFieldDescList->GetFieldType());
        }
        else
        {
            LPCUTF8 name, nameSpace;

#ifdef _X86_
            CorElementType normalizedType = ELEMENT_TYPE_END;
            if (pClass->ComputeInternalCorElementTypeForValueType(&normalizedType))
            {
                CONSISTENCY_CHECK(ELEMENT_TYPE_END != normalizedType);
                pMT->SetInternalCorElementType(normalizedType);

#ifdef _DEBUG
                bmtInternal->pModule->GetMDImport()->GetNameOfTypeDef(GetCl(), &name, &nameSpace);
                LOG((LF_CLASSLOADER, LL_INFO10000, "%s::%s marked as primitive type %i\n", nameSpace, name, normalizedType));
#endif // _DEBUG

            }
#endif // _X86_

            // Check if it is a primitive type or other special type
            if (!IsNested() && bmtInternal->pModule->IsSystem())    // we are in mscorlib
            {
                bmtInternal->pModule->GetMDImport()->GetNameOfTypeDef(GetCl(), &name, &nameSpace);

                if (strcmp(nameSpace, "System") == 0) {

                    CorElementType type = CorTypeInfo::FindPrimitiveType(nameSpace, name);
                    if (type == ELEMENT_TYPE_END)
                    {
                        // Mark the special types that have embeded stack poitners in them
                        if (strcmp(name, "ArgIterator") == 0 || strcmp(name, "RuntimeArgumentHandle") == 0)
                            pClass->SetContainsStackPtr();
#ifndef _X86_
                        pMT->SetInternalCorElementType (ELEMENT_TYPE_VALUETYPE);

                        if ((strcmp(name, g_RuntimeTypeHandleName) == 0)   ||
                            (strcmp(name, g_RuntimeMethodHandleName) == 0) ||
                            (strcmp(name, g_RuntimeFieldHandleName) == 0)  ||
                            (strcmp(name, g_RuntimeArgumentHandleName) == 0))
                        {
                            pMT->SetInternalCorElementType (ELEMENT_TYPE_I);
                        }
                                                
#endif // !_X86_

#ifdef ALIGN_ACCESS
                        // This is required because native layout of System.Decimal causes it to be aligned
                        // differently to the layout of the native DECIMAL structure, which will cause
                        // data misalignent exceptions if Decimal is embedded in another type. 
                        if (strcmp(name, "Decimal") == 0)
                        {
                            EEClassLayoutInfo* pLayout = pClass->GetLayoutInfo();
                            pLayout->m_LargestAlignmentRequirementOfAllMembers        = sizeof(ULONGLONG);
                            pLayout->m_ManagedLargestAlignmentRequirementOfAllMembers = sizeof(ULONGLONG);
                        }
#endif // ALIGN_ACCESS
                    }
                    else
                    {
                        pMT->SetInternalCorElementType(type, TRUE /* isTruePrimitive */ );
                        pClass->SetIsTruePrimitive();
                        _ASSERTE(pMT->IsTruePrimitive());

#ifdef _DEBUG
                        bmtInternal->pModule->GetMDImport()->GetNameOfTypeDef(GetCl(), &name, &nameSpace);
                        LOG((LF_CLASSLOADER, LL_INFO10000, "%s::%s marked as primitive type %i\n", nameSpace, name, type));
#endif // _DEBUG

                        if (pMT->GetInternalCorElementType() == ELEMENT_TYPE_TYPEDBYREF)
                            pClass->SetContainsStackPtr();
                    }
                }
            }
        }
    }

    // Now fill in the real interface map with the approximate interfaces
    if (bmtInterface->dwInterfaceMapSize > 0)
    {
        InterfaceInfo_t *pInterfaces = pMT->GetInterfaceMap();

        CONSISTENCY_CHECK(CheckPointer(pInterfaces));
        // Copy from temporary interface map
        memcpy(pInterfaces, bmtInterface->pInterfaceMap,
               bmtInterface->dwInterfaceMapSize * sizeof(InterfaceInfo_t));

    }

    for (i = 0; i < bmtVT->dwCurrentNonVtableSlot; i++)
    {
        TADDR addr = (TADDR)bmtVT->pVtable[i];
        MethodDesc* pMD = bmtVT->pVtableMD[i];

        if (addr == NULL)
        {
            _ASSERTE(pMD != NULL);

            if (pMD->GetMethodTable() == pMT)
            {
                if (pMD->ComputeMayHaveNativeCode())
                    pMD->SetMayHaveNativeCode();

                pMD->GetMethodDescChunk()->
                    EnsureTemporaryEntryPointsCreated(bmtDomain, pamTracker);
                addr = pMD->GetTemporaryEntryPoint();

                _ASSERTE(!pMD->HasNonVtableSlot());
            }
            else
            {
                addr = pMD->HasStableEntryPoint() ?
                    pMD->GetStableEntryPoint() : pMD->GetTemporaryEntryPoint();
            }
        }
        _ASSERTE(addr != NULL);
        pMT->SetSlot(i,(SLOT)addr);
        if (pMD != NULL && pMD->GetMethodTable() == pMT && pMD->GetSlot() == i)
        {
            if (pMD->RequiresStableEntryPoint())
            {
                // The rest of the system assumes that certain methods always have stable entrypoints.
                // Create them now.
                pMD->GetOrCreatePrecode();
            }
        }
    }

    pMT->SetParentMethodTable (bmtParent->pParentMethodTable);

#if _DEBUG
    // bmtInterface now contains old, invalid information....
    bmtInterface->pInterfaceMap= (InterfaceInfo_t *) (UINT_PTR) 0xbaddf00d;
#endif // _DEBUG


    // If we have any entries, then finalize them and allocate the object in class loader heap
    DispatchMap                 *pDispatchMap        = NULL;
    DispatchMapBuilder          *pDispatchMapBuilder = bmtVT->pDispatchMapBuilder;
    CONSISTENCY_CHECK(CheckPointer(pDispatchMapBuilder));

    if (pDispatchMapBuilder->Count() > 0)
    {
        // Create a map in stacking memory.
        BYTE *pbMap;
        UINT32 cbMap;
        DispatchMap::CreateEncodedMapping(pMT,
                                          pDispatchMapBuilder,
                                          pDispatchMapBuilder->GetAllocator(),
                                          &pbMap,
                                          &cbMap);

        // Now finalize the impltable and allocate the block in the low frequency loader heap
        size_t objSize = (size_t) DispatchMap::GetObjectSize(cbMap);
        void *pv = pamTracker->Track(bmtDomain->GetLowFrequencyHeap()->AllocMem(objSize));
        _ASSERTE(pv != NULL);

        // Use placement new
        pDispatchMap = new (pv) DispatchMap(
            pbMap, cbMap);
        pMT->SetDispatchMap(pDispatchMap);

#ifdef LOGGING
        g_sdStats.m_cDispatchMap++;
        g_sdStats.m_cbDispatchMap += (UINT32) objSize;
        LOG((LF_LOADER, LL_INFO1000, "SD: Dispatch map for %s: %d bytes for map, %d bytes total for object.\n",
            pMT->GetDebugClassName(), cbMap, objSize));
#endif // LOGGING

    }


    BOOL fCheckForMissingMethod = !IsAbstract() && !IsInterface();



    if (!IsInterface())
    {
        // Propagate inheritance.

        // NOTE: In the world of unfolded interface this was used to propagate overrides into
        //       the unfolded interface vtables to make sure that overrides of virtual methods
        //       also overrode the interface methods that they contributed to. This had the
        //       unfortunate side-effect of also overwriting regular vtable slots that had been
        //       methodimpl'd and as a result changed the meaning of methodimpl from "substitute
        //       the body of method A with the body of method B" to "unify the slots of methods
        //       A and B". But now compilers have come to rely on this side-effect and it can
        //       not be brought back to its originally intended behaviour.

        for (i = 0; i < bmtVT->dwCurrentVtableSlot; i++)
        {
            // For now only propagate inheritance for method desc that are not interface MD's.
            // This is not sufficient but InterfaceImpl's will complete the picture.
            MethodDesc* pMD = pMT->GetUnknownMethodDescForSlot(i);
            CONSISTENCY_CHECK(CheckPointer(pMD));

            CONSISTENCY_CHECK(!pMD->GetClass()->IsInterface());

            // Do not use pMD->IsInterface here as that asks the method table and the
            // MT may not yet be set up.
            if(
                pMD->GetSlot() != i)
            {
                INDEBUG(MethodDesc *pMDNew; pMDNew = pMT->GetUnknownMethodDescForSlot(pMD->GetSlot());)
                pMT->SetSlot(i,pMT->GetSlot(pMD->GetSlot()));

                // Update the pMD to the new method desc we just copied over ourselves with. This will
                // be used in the check for missing method block below.
                pMD = pMT->GetUnknownMethodDescForSlot(pMD->GetSlot());

                // This method is now duplicate
                pMD->SetDuplicate();
                INDEBUG(g_dupMethods++;)
            }

            if (fCheckForMissingMethod)
            {
                // Do not use pMD->IsInterface here as that asks the method table and the
                // MT may not yet be set up.
                if (
                    pMD->IsAbstract())
                {
                    BuildMethodTableThrowException(IDS_CLASSLOAD_NOTIMPLEMENTED, pMD->GetNameOnNonArrayClass());
                }
                    // we check earlier to make certain only abstract methods have RVA != 0
                _ASSERTE(!(pMD->GetModule()->IsPEFile() && pMD->IsIL() && pMD->GetRVA() == 0));
            }
        }
    }

    // GetMethodData by default will cache its result. However, in the case that we're
    // building a MethodTable, we aren't guaranteed that this type is going to successfully
    // load and so caching it would result in errors down the road since the memory and
    // type occupying the same memory location would very likely be incorrect. The second
    // argument specifies that GetMethodData should not cache the returned object.
    MethodTable::MethodDataWrapper hMTData(MethodTable::GetMethodData(pMT, FALSE));

    // Since interfaces aren't laid out in the vtable for stub dispatch, what we need to do
    // is try to find an implementation for every interface contract by iterating through
    // the interfaces not declared on a parent.
    if (fCheckForMissingMethod)
    {
        BOOL fParentIsAbstract = FALSE;
        if (bmtParent->pParentMethodTable != NULL) {
            fParentIsAbstract = bmtParent->pParentMethodTable->IsAbstract();
        }
        // If the parent is abstract, we need to check that each virtual method is implemented
        if (fParentIsAbstract) {
            // NOTE: Uses hMTData to avoid caching a MethodData object for the type being built.
            MethodTable::MethodIterator it(hMTData);
            for (; it.IsValid() && it.IsVirtual(); it.Next()) {
                MethodDesc *pMD = it.GetMethodDesc();
                if (pMD->IsAbstract()) {
                    MethodDesc *pDeclMD = it.GetDeclMethodDesc();
                    BuildMethodTableThrowException(IDS_CLASSLOAD_NOTIMPLEMENTED, pDeclMD->GetNameOnNonArrayClass());
                }
            }
        }
        MethodTable::InterfaceMapIterator intIt = pMT->IterateInterfaceMap();
        while (intIt.Next())
        {
            if (fParentIsAbstract || !intIt.IsImplementedByParent())
            {
                // Since the type is not completely loaded, we must explicitly determine and provide
                // the interface ID for the iterator.
                // NOTE: This override does not cache the resulting MethodData object.
                MethodTable::MethodDataWrapper hData(MethodTable::GetMethodData(
                    DispatchMapTypeID::InterfaceClassID(intIt.GetIndex()), intIt.GetInterface(), pMT));
                MethodTable::MethodIterator it(hData);
                for (; it.IsValid() && it.IsVirtual(); it.Next()) {
                    if (it.GetTarget().IsNull()) {
                        MethodDesc *pMD = it.GetDeclMethodDesc();
                        BuildMethodTableThrowException(IDS_CLASSLOAD_NOTIMPLEMENTED, pMD->GetNameOnNonArrayClass());
                    }
                }
            }
        }
    }

#ifdef _DEBUG
    {
        for (UINT32 i = 0; i < bmtVT->dwCurrentNonVtableSlot; i++)
        {
            _ASSERTE(bmtVT->GetMethodDescForSlot(i) != NULL);
        }
    }
#endif // _DEBUG



    // If this class uses any VTS (Version Tolerant Serialization) features
    // (event callbacks or OptionalField attributes) we've previously cached the
    // additional information in the bmtMFDescs structure. Now it's time to add
    // this information as an optional extension to the MethodTable.
    if (bmtMFDescs->fNeedsRemotingVtsInfo)
    {
        DWORD dwNumIntroducedInstanceFields = bmtEnumMF->dwNumInstanceFields;
        PTR_RemotingVtsInfo pInfo = pMT->AllocateRemotingVtsInfo(bmtDomain, pamTracker, dwNumIntroducedInstanceFields);

        pInfo->m_pCallbacks[RemotingVtsInfo::VTS_CALLBACK_ON_SERIALIZING] = bmtMFDescs->pOnSerializingMethod;
        pInfo->m_pCallbacks[RemotingVtsInfo::VTS_CALLBACK_ON_SERIALIZED] = bmtMFDescs->pOnSerializedMethod;
        pInfo->m_pCallbacks[RemotingVtsInfo::VTS_CALLBACK_ON_DESERIALIZING] = bmtMFDescs->pOnDeserializingMethod;
        pInfo->m_pCallbacks[RemotingVtsInfo::VTS_CALLBACK_ON_DESERIALIZED] = bmtMFDescs->pOnDeserializedMethod;

        for (i = 0; i < dwNumIntroducedInstanceFields; i++)
        {
            if (bmtMFDescs->prfNotSerializedFields && bmtMFDescs->prfNotSerializedFields[i])
                pInfo->SetIsNotSerialized(i);
            if (bmtMFDescs->prfOptionallySerializedFields && bmtMFDescs->prfOptionallySerializedFields[i])
                pInfo->SetIsOptionallySerialized(i);
        }

    }

    if (fNeedsRemotableMethodInfo)
        pMT->SetupRemotableMethodInfo(bmtDomain, pamTracker);

#if defined(_DEBUG) && !defined(STUB_DISPATCH_ALL)
    if (bmtParent->pParentMethodTable)
        MarkInheritedVirtualMethods(pMT, bmtParent->pParentMethodTable);
#endif // defined(_DEBUG) && !defined(STUB_DISPATCH_ALL)

    

    END_SO_INTOLERANT_CODE

}

//*******************************************************************************
bool EEClass::ComputeInternalCorElementTypeForValueType(CorElementType* pInternalTypeOut)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pInternalTypeOut, NULL_OK));
    } CONTRACTL_END;

    if (GetNumInstanceFields() == 1 &&
        (!HasLayout() || GetNumInstanceFieldBytes() == sizeof(void*)) // Don't do the optimization
                                                                      // if we're getting specified
                                                                      // anything but the trivial
                                                                      // layout
        )
    {
        CorElementType type = m_pFieldDescList->GetFieldType();

        if (type == ELEMENT_TYPE_VALUETYPE)
        {
            TypeHandle fldHnd = m_pFieldDescList->GetApproxFieldTypeHandleThrowing();
            CONSISTENCY_CHECK(!fldHnd.IsNull());

            type = fldHnd.GetInternalCorElementType();
        }

        switch (type)
        {
            case ELEMENT_TYPE_I:
            case ELEMENT_TYPE_U:
            case ELEMENT_TYPE_I4:
            case ELEMENT_TYPE_U4:
            case ELEMENT_TYPE_PTR:
            {
                if (pInternalTypeOut)
                {
                    *pInternalTypeOut = type;
                }
                return true;
            }

            default:
                break;
        }
    }

    return false;
}

INT32 __stdcall IsDefined(Module *pModule, mdToken token, TypeHandle attributeClass)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
    }
    CONTRACTL_END;

    BOOL isDefined = FALSE;

    IMDInternalImport *pInternalImport = pModule->GetMDImport();
    BOOL isSealed = FALSE;

    HRESULT         hr;
    HENUMInternalHolderNoRef   hEnum(pInternalImport);
    TypeHandle caTH;

    // Get the enum first but don't get any values
    hr = pInternalImport->EnumInit(mdtCustomAttribute, token, &hEnum);
    if (SUCCEEDED(hr))
    {
        ULONG cMax = pInternalImport->EnumGetCount(&hEnum);
        if (cMax)
        {
            // we have something to look at


            if (!attributeClass.IsNull())
                isSealed = attributeClass.GetClass()->IsSealed();

            // Loop through the Attributes and look for the requested one
            mdCustomAttribute cv;
            while (pInternalImport->EnumNext(&hEnum, &cv))
            {
                //
                // fetch the ctor
                mdToken     tkCtor;
                pInternalImport->GetCustomAttributeProps(cv, &tkCtor);

                mdToken tkType = TypeFromToken(tkCtor);
                if(tkType != mdtMemberRef && tkType != mdtMethodDef)
                    continue; // we only deal with the ctor case

                //
                // get the info to load the type, so we can check whether the current
                // attribute is a subtype of the requested attribute
                hr = pInternalImport->GetParentToken(tkCtor, &tkType);
                if (FAILED(hr))
                {
                    _ASSERTE(!"GetParentToken Failed, bogus metadata");
                    COMPlusThrow(kInvalidProgramException);
                }
                _ASSERTE(TypeFromToken(tkType) == mdtTypeRef || TypeFromToken(tkType) == mdtTypeDef);
                // load the type
                if (isSealed)
                {
                    caTH=ClassLoader::LoadTypeDefOrRefThrowing(pModule, tkType,
                                                               ClassLoader::ReturnNullIfNotFound,
                                                               ClassLoader::FailIfUninstDefOrRef,
                                                               TypeFromToken(tkType) == mdtTypeDef ? tdAllTypes : tdNoTypes);
                }
                else
                {
                    caTH = ClassLoader::LoadTypeDefOrRefThrowing(pModule, tkType,
                                                                 ClassLoader::ReturnNullIfNotFound,
                                                                 ClassLoader::FailIfUninstDefOrRef);
                }
                if (caTH.IsNull())
                    continue;

                // a null class implies all custom attribute
                if (!attributeClass.IsNull())
                {
                    if (isSealed)
                    {
                        if (attributeClass != caTH)
                            continue;
                    }
                    else
                    {
                        if (!caTH.CanCastTo(attributeClass))
                            continue;
                    }
                }

                //
                // if we are here we got one
                isDefined = TRUE;
                break;
            }
        }

    }
    else
    {
        _ASSERTE(!"EnumInit Failed");
        COMPlusThrow(kInvalidProgramException);
    }

    return isDefined;
}

//*******************************************************************************
VOID MethodTableBuilder::CheckForRemotingProxyAttrib()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    GCX_COOP();

    // See if our parent class has a proxy attribute
    _ASSERTE(g_pObjectClass != NULL);

    if (!bmtParent->pParentMethodTable->GetClass()->HasRemotingProxyAttribute())
    {
        // Call the metadata api to look for a proxy attribute on this type
        // Note: the api does not check for inherited attributes

        // Set the flag is the type has a non-default proxy attribute
        if(IsDefined(
            bmtInternal->pModule,
            bmtInternal->cl,
            TypeHandle(CRemotingServices::GetProxyAttributeClass())))
        {
            SetHasRemotingProxyAttribute();
        }
    }
    else
    {
        // parent has proxyAttribute ... mark this class as having one too!
        SetHasRemotingProxyAttribute();
    }
}



//*******************************************************************************
//
// Used by BuildMethodTable
//
// Set the contextful or marshaledbyref flag on the attributes of the class
//
VOID MethodTableBuilder::SetContextfulOrByRef()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(bmtInternal));

    }
    CONTRACTL_END;

    // Check whether these classes are the root classes of contextful
    // and marshalbyref classes i.e. System.ContextBoundObject and
    // System.MarshalByRefObject respectively.

    // Extract the class name
    LPCUTF8 pszClassName = NULL;
    LPCUTF8 pszNameSpace = NULL;
    bmtInternal->pModule->GetMDImport()->GetNameOfTypeDef(GetCl(), &pszClassName, &pszNameSpace);

    StackSString ssFullyQualifiedName;
    ns::MakePath(ssFullyQualifiedName,
                 StackSString(SString::Utf8, pszNameSpace),
                 StackSString(SString::Utf8, pszClassName));

    // Compare
    if(ssFullyQualifiedName.Equals(SL(g_ContextBoundObjectClassName)))
    {
        // Set the contextful and marshalbyref flag
        SetContextfull();
        bmtProp->fMarshaledByRef = true;
    }

    else if(ssFullyQualifiedName.Equals(SL(g_MarshalByRefObjectClassName)))
    {
        // Set the marshalbyref flag
        bmtProp->fMarshaledByRef = true;
    }

    else
    {
        // First check whether the parent class is contextful or
        // marshalbyref
        EEClass* pParent = (bmtParent->pParentMethodTable) ? bmtParent->pParentMethodTable->GetClass() : NULL;
        if(pParent)
        {
            if(pParent->IsContextful())
            {
                // Set the contextful and marshalbyref flag
                SetContextfull();
                bmtProp->fMarshaledByRef = true;

                // While these could work with a bit of work in the JIT,
                // we will not support generic context-bound objects in V2.0.
                if (bmtGenerics->GetNumGenericArgs() > 0)
                    BuildMethodTableThrowException(IDS_CLASSLOAD_GENERIC_CONTEXT_BOUND_OBJECT);
            }

            else if (pParent->IsMarshaledByRef())
                // Set the marshalbyref flag
                bmtProp->fMarshaledByRef = true;
        }
    }

}

#if CHECK_APP_DOMAIN_LEAKS
//*******************************************************************************
void EEClass::GetPredefinedAgility(Module *pModule, mdTypeDef td,
                                   BOOL *pfIsAgile, BOOL *pfCheckAgile)
{

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    //
    // There are 4 settings possible:
    // IsAgile  CheckAgile
    // F        F               (default)   Use normal type logic to determine agility
    // T        F               "Proxy"     Treated as agile even though may not be.
    // F        T               "Maybe"     Not agile, but specific instances can be made agile.
    // T        T               "Force"     All instances are forced agile, even though not typesafe.
    //
    // Also, note that object arrays of agile or maybe agile types are made maybe agile.
    //

    static const struct PredefinedAgility
    {
        const char  *name;
        BOOL        isAgile;
        BOOL        checkAgile;
    }
    agility[] =
    {
        // The Thread and its LocalDataStore leak across context boundaries.
        // We manage the leaks manually
        { g_ThreadClassName,                    TRUE,   FALSE },
        { g_LocalDataStoreClassName,            TRUE,   FALSE },
        { g_LocalDataStoreMgrClassName,       FALSE, TRUE },
        // The SharedStatics class is a container for process-wide data
        { g_SharedStaticsClassName,             FALSE,  TRUE },

        { "System.ActivationArguments",         FALSE,  TRUE },

        // Make all containers maybe agile
        { "System.Collections.*",               FALSE,  TRUE },
        { "System.Collections.Generic.*",               FALSE,  TRUE },

        // Make all globalization objects agile
        // We have CultureInfo objects on thread.  Because threads leak across
        // app domains, we have to be prepared for CultureInfo to leak across.
        // CultureInfo exposes all of the other globalization objects, so we
        // just make the entire namespace app domain agile.
        { "System.Globalization.*",             FALSE,  TRUE },

        // Remoting structures for legally smuggling messages across app domains
        { "System.Runtime.Remoting.Messaging.SmuggledMethodCallMessage", FALSE,  TRUE },
        { "System.Runtime.Remoting.Messaging.SmuggledMethodReturnMessage", FALSE,  TRUE },
        { "System.Runtime.Remoting.Messaging.SmuggledObjRef", FALSE, TRUE},
        { "System.Runtime.Remoting.ObjRef", FALSE,  TRUE },
        { "System.Runtime.Remoting.ChannelInfo", FALSE,  TRUE },
        { "System.Runtime.Remoting.Channels.CrossAppDomainData", FALSE,  TRUE },

        // Remoting cached data structures are all in mscorlib
        { "System.Runtime.Remoting.Metadata.RemotingCachedData",       FALSE,  TRUE },
        { "System.Runtime.Remoting.Metadata.RemotingMethodCachedData", FALSE,  TRUE },
        { "System.Runtime.Remoting.Metadata.RemotingTypeCachedData", FALSE,  TRUE },
        { g_ReflectionMemberInfoName,                            FALSE,  TRUE },
        { g_TypeClassName,                                       FALSE,  TRUE },
        { g_ReflectionClassName,                                 FALSE,  TRUE },
        { g_ReflectionConstructorInfoName,                       FALSE,  TRUE },
        { g_ReflectionConstructorName,                           FALSE,  TRUE },
        { "System.Reflection.EventInfo",                         FALSE,  TRUE },
        { g_ReflectionEventInfoName,                             FALSE,  TRUE },
        { g_ReflectionFieldInfoName,                             FALSE,  TRUE },
        { g_ReflectionFieldName,                                 FALSE,  TRUE },
        { g_MethodBaseName,                                      FALSE,  TRUE },
        { g_ReflectionMethodName,                                FALSE,  TRUE },
        { g_ReflectionPropertyInfoName,                          FALSE,  TRUE },
        { g_ReflectionPropInfoName,                              FALSE,  TRUE },
        { g_ReflectionParamInfoName,                             FALSE,  TRUE },
        { "System.Runtime.Remoting.Metadata.SoapAttribute",      FALSE,  TRUE },
        { "System.Runtime.Remoting.Metadata.SoapFieldAttribute", FALSE,  TRUE },
        { "System.Runtime.Remoting.Metadata.SoapMethodAttribute",FALSE,  TRUE },
        { "System.Runtime.Remoting.Metadata.SoapParameterAttribute", FALSE,  TRUE },
        { "System.Runtime.Remoting.Metadata.SoapTypeAttribute",  FALSE,  TRUE },
        { "System.Reflection.Cache.InternalCache",               FALSE,  TRUE },
        { "System.Reflection.Cache.InternalCacheItem",           FALSE,  TRUE },
        { "System.RuntimeType+RuntimeTypeCache",               FALSE,  TRUE },

        { "System.RuntimeType+RuntimeTypeCache+MemberInfoCache`1+Filter",               FALSE,  TRUE },
        { "System.Reflection.CerArrayList`1",               FALSE,  TRUE },
        { "System.Reflection.CerHashtable`2",               FALSE,  TRUE },

        { "System.RuntimeType+RuntimeTypeCache+MemberInfoCache", FALSE,  TRUE },
        { "System.Reflection.RtFieldInfo",               FALSE,  TRUE },
        { "System.Reflection.MdFieldInfo",               FALSE,  TRUE },
        { "System.Signature",               FALSE,  TRUE },
        { "System.Reflection.MetadataImport",     FALSE,  TRUE },
        { "System.SignatureStruct",               FALSE,  TRUE },

        // LogSwitches are agile even though we can't prove it
        { "System.Diagnostics.LogSwitch",       FALSE,  TRUE },

        // There is a process global PermissionTokenFactory
        { "System.Security.PermissionToken",    FALSE,  TRUE },
        { g_PermissionTokenFactoryName,         FALSE,  TRUE },


        // Mark all the exceptions we throw agile.  This makes
        // most BVTs pass even though exceptions leak
        //
        // Note that making exception checked automatically
        // makes a bunch of subclasses checked as well.
        //
        // Pre-allocated exceptions
        { g_ExceptionClassName,                 FALSE,  TRUE },
        { g_OutOfMemoryExceptionClassName,      FALSE,  TRUE },
        { g_StackOverflowExceptionClassName,    FALSE,  TRUE },
        { g_ExecutionEngineExceptionClassName,  FALSE,  TRUE },

        // SecurityDocument contains pointers and other agile types
        { "System.Security.SecurityDocument",    TRUE, TRUE },

        // BinaryFormatter smuggles these across appdomains.
        { "System.Runtime.Serialization.Formatters.Binary.BinaryObjectWithMap", TRUE, FALSE},
        { "System.Runtime.Serialization.Formatters.Binary.BinaryObjectWithMapTyped", TRUE, FALSE},

        { NULL }
    };

    if (pModule == SystemDomain::SystemModule())
    {
        while (TRUE)
        {
        LPCUTF8 pszName;
        LPCUTF8 pszNamespace;
            HRESULT     hr;
            mdTypeDef   tdEnclosing;

        pModule->GetMDImport()->GetNameOfTypeDef(td, &pszName, &pszNamespace);

            const PredefinedAgility *p = agility;
        while (p->name != NULL)
        {
            SIZE_T length = strlen(pszNamespace);
            if (strncmp(pszNamespace, p->name, length) == 0
                && (strcmp(pszName, p->name + length + 1) == 0
                    || strcmp("*", p->name + length + 1) == 0))
            {
                *pfIsAgile = p->isAgile;
                *pfCheckAgile = p->checkAgile;
                return;
            }

            p++;
        }

            // Perhaps we have a nested type like 'bucket' that is supposed to be
            // agile or checked agile by virtue of being enclosed in a type like
            // hashtable, which is itself inside "System.Collections".
            tdEnclosing = mdTypeDefNil;
            hr = pModule->GetMDImport()->GetNestedClassProps(td, &tdEnclosing);
            if (SUCCEEDED(hr))
            {
                BAD_FORMAT_NOTHROW_ASSERT(tdEnclosing != td && TypeFromToken(tdEnclosing) == mdtTypeDef);
                td = tdEnclosing;
            }
            else
                break;
        }
    }

    *pfIsAgile = FALSE;
    *pfCheckAgile = FALSE;
}

//*******************************************************************************
void EEClass::SetAppDomainAgileAttribute()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        //        PRECONDITION(!IsAppDomainAgilityDone());
    }
    CONTRACTL_END

    //
    // The most general case for provably a agile class is
    // (1) No instance fields of non-sealed or non-agile types
    // (2) Class is in system domain (its type must be not unloadable
    //      & loaded in all app domains)
    // (3) The class can't have a finalizer
    // (4) The class can't be a COMClass
    //

    _ASSERTE(!IsAppDomainAgilityDone());

    BOOL    fCheckAgile     = FALSE;
    BOOL    fAgile          = FALSE;
    BOOL    fFieldsAgile    = TRUE;
    WORD        nFields         = 0;

    if (!GetModule()->IsSystem())
    {
        //
        // No types outside of the system domain can even think about
        // being agile
        //

        goto exit;
    }

    if (m_pMethodTable->IsComObjectType())
    {
        //
        // No COM type is agile, as there is domain specific stuff in the sync block
        //

        goto exit;
    }

    if (m_pMethodTable->IsInterface())
    {
        //
        // Don't mark interfaces agile
        //

        goto exit;
    }

    if (TypeHandle(m_pMethodTable).ContainsGenericVariables())
    {
        // Types containing formal type parameters aren't agile
        goto exit;
    }

    //
    // See if we need agile checking in the class
    //

    GetPredefinedAgility(GetModule(), GetCl(),
                         &fAgile, &fCheckAgile);

    if (m_pMethodTable->HasFinalizer())
    {
        if (!fAgile && !fCheckAgile)
        {
            //
            // If we're finalizable, we need domain affinity.  Otherwise, we may appear
            // to a particular app domain not to call the finalizer (since it may run
            // in a different domain.)
            //
            // Note: do not change this assumption. The eager finalizaton code for
            // appdomain unloading assumes that no obects other than those in mscorlib
            // can be agile and finalizable                      
            //
            goto exit;
        }
        else
        {

            // Note that a finalizable object will be considered potentially agile if it has one of the two
            // predefined agility bits set. This will cause an assert in the eager finalization code if you add
            // a finalizer to such a class - we don't want to have them as we can't run them eagerly and running
            // them after we've cleared the roots/handles means it can't do much safely. Right now thread is the
            // only one we allow.                                                                                                  
            _ASSERTE(g_pThreadClass == NULL || m_pMethodTable->IsAgileAndFinalizable());
        }
    }

    //
    // Now see if the type is "naturally agile" - that is, it's type structure
    // guarantees agility.
    //

    if (GetParentClass() != NULL)
    {
        //
        // Make sure our parent was computed.  This should only happen
        // when we are prejitting - otherwise it is computed for each
        // class as its loaded.
        //

        _ASSERTE(GetParentClass()->IsAppDomainAgilityDone());

        if (!GetParentClass()->IsAppDomainAgile())
        {
            fFieldsAgile = FALSE;
            if (fCheckAgile)
                _ASSERTE(GetParentClass()->IsCheckAppDomainAgile());
        }

        //
        // To save having to list a lot of trivial (layout-wise) subclasses,
        // automatically check a subclass if its parent is checked and
        // it introduces no new fields.
        //

        if (!fCheckAgile
            && GetParentClass()->IsCheckAppDomainAgile()
            && GetNumInstanceFields() == GetParentClass()->GetNumInstanceFields())
            fCheckAgile = TRUE;
    }

    nFields = GetNumInstanceFields()
        - (GetParentClass() == NULL ? 0 : GetParentClass()->GetNumInstanceFields());

    if (fFieldsAgile || fCheckAgile)
    {
        FieldDesc *pFD = m_pFieldDescList;
        FieldDesc *pFDEnd = pFD + nFields;
        while (pFD < pFDEnd)
        {
            switch (pFD->GetFieldType())
            {
            case ELEMENT_TYPE_CLASS:
                {
                    //
                    // There is a bit of a problem in computing the classes which are naturally agile -
                    // we don't want to load types of non-value type fields.  So for now we'll
                    // err on the side of conservatism and not allow any non-value type fields other than
                    // the forced agile types listed above.
                    //

                    MetaSig sig(pFD);
                    CorElementType type = sig.NextArg();
                    SigPointer sigPtr = sig.GetArgProps();

                    //
                    // Don't worry about strings
                    //

                    if (type == ELEMENT_TYPE_STRING)
                        break;

                    // Find our field's token so we can proceed cautiously
                    mdToken token = mdTokenNil;

                    if (type == ELEMENT_TYPE_CLASS)
                        IfFailThrow(sigPtr.GetToken(&token));

                    //
                    // First, a special check to see if the field is of our own type.
                    //

                    if (token == GetCl() && IsSealed())
                        break;

                    //
                    // Now, look for the field's TypeHandle.
                    //

                    TypeHandle th;
                        th = pFD->LookupFieldTypeHandle();

                    //
                    // See if the referenced type is agile.  Note that there is a reasonable
                    // chance that the type hasn't been loaded yet.  If this is the case,
                    // we just have to assume that it's not agile, since we can't trigger
                    // extra loads here (for fear of circular recursion.)
                    //
                    // If you have an agile class which runs into this problem, you can solve it by
                    // setting the type manually to be agile.
                    //

                    if (th.IsNull()
                        || !th.IsAppDomainAgile()
                        || (th.IsUnsharedMT()
                            && !th.GetClass()->IsSealed()))
                    {
                        //
                        // Treat the field as non-agile.
                        //

                        fFieldsAgile = FALSE;
                        if (fCheckAgile)
                            pFD->SetDangerousAppDomainAgileField();
                    }
                }

                break;

            case ELEMENT_TYPE_VALUETYPE:
                {
                    TypeHandle th;

                    th = pFD->GetApproxFieldTypeHandleThrowing();

                    _ASSERTE(!th.IsNull());

                    if (!th.IsAppDomainAgile())
                    {
                        fFieldsAgile = FALSE;
                        if (fCheckAgile)
                            pFD->SetDangerousAppDomainAgileField();
                    }
                }

                break;

            default:
                break;
            }

            pFD++;
        }
    }

    if (fFieldsAgile || fAgile)
        SetAppDomainAgile();

    if (fCheckAgile && !fFieldsAgile)
        SetCheckAppDomainAgile();

exit:
    LOG((LF_CLASSLOADER, LL_INFO1000, "CLASSLOADER: AppDomainAgileAttribute for %s is %d\n", GetDebugClassName(), IsAppDomainAgile()));
    SetAppDomainAgilityDone();
}
#endif // CHECK_APP_DOMAIN_LEAKS

#ifdef DEBUGGING_SUPPORTED
//*******************************************************************************
//
// Debugger notification
//
BOOL TypeHandle::NotifyDebuggerLoad(AppDomain *pDomain, BOOL attaching) const
{
    WRAPPER_CONTRACT;

    if (IsIntrospectionOnly())
    {
        return FALSE;
    }

    if (!CORDebuggerAttached())
    {
        return FALSE;
    }

    return g_pDebugInterface->LoadClass(
        *this, GetCl(), GetModule(), pDomain, GetAssembly()->IsSystem(), attaching);
}

//*******************************************************************************
void TypeHandle::NotifyDebuggerUnload(AppDomain *pDomain) const
{
    WRAPPER_CONTRACT;

    if (IsIntrospectionOnly())
        return;

    if (!pDomain->IsDebuggerAttached())
        return;

    g_pDebugInterface->UnloadClass(GetCl(), GetModule(), pDomain, FALSE);
}
#endif // DEBUGGING_SUPPORTED

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Perform relevant GC calculations for value classes
//
VOID MethodTableBuilder::HandleGCForValueClasses(EEClass** pByValueClassCache)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    DWORD i, j;

    EEClass *pClass = GetHalfBakedClass();
    MethodTable *pMT = GetHalfBakedMethodTable();

    FieldDesc *pFieldDescList = pClass->GetApproxFieldDescListRaw();

    // Note that for value classes, the following calculation is only appropriate
    // when the instance is in its "boxed" state.
    if (pClass->m_wNumGCPointerSeries > 0)
    {
        CGCDescSeries *pSeries;
        CGCDescSeries *pHighest;

        pMT->SetContainsPointers();

        // Copy the pointer series map from the parent
        CGCDesc::Init( (PVOID) pMT, pClass->GetNumGCPointerSeries() );
        if (GetParentClass() && (GetParentClass()->m_wNumGCPointerSeries > 0))
        {
            size_t ParentGCSize = CGCDesc::ComputeSize(GetParentClass()->m_wNumGCPointerSeries);
            memcpy( (PVOID) (((BYTE*) pMT) - ParentGCSize),
                    (PVOID) (((BYTE*) GetParentClass()->m_pMethodTable) - ParentGCSize),
                    ParentGCSize - sizeof(size_t)   // sizeof(size_t) is the NumSeries count
                  );

        }

        // Build the pointer series map for this pointers in this instance
        pSeries = ((CGCDesc*)pMT)->GetLowestSeries();
        if (bmtFP->NumInstanceGCPointerFields)
        {
            // See gcdesc.h for an explanation of why we adjust by subtracting BaseSize
            pSeries->SetSeriesSize( (size_t) (bmtFP->NumInstanceGCPointerFields * sizeof(OBJECTREF)) - (size_t) pMT->GetBaseSize());
            pSeries->SetSeriesOffset(bmtFP->GCPointerFieldStart+sizeof(Object));
            pSeries++;
        }

        // Insert GC info for fields which are by-value classes
        for (i = 0; i < bmtEnumMF->dwNumInstanceFields; i++)
        {
            if (pFieldDescList[i].IsByValue())
            {
                EEClass     *pByValueClass = pByValueClassCache[i];
                MethodTable *pByValueMT = pByValueClass->GetMethodTable();
                CGCDescSeries *pByValueSeries;

                // The by value class may have more than one pointer series
                DWORD       dwNumByValueSeries = pByValueClass->m_wNumGCPointerSeries;

                if (dwNumByValueSeries > 0)
                {
                    // Offset of the by value class in the class we are building, does NOT include Object
                    DWORD       dwCurrentOffset = pFieldDescList[i].GetOffset_NoLogging();

                    pByValueSeries = ((CGCDesc*) pByValueMT)->GetLowestSeries();

                    for (j = 0; j < dwNumByValueSeries; j++)
                    {
                        size_t cbSeriesSize;
                        size_t cbSeriesOffset;

                        _ASSERTE(pSeries <= CGCDesc::GetCGCDescFromMT(pMT)->GetHighestSeries());

                        cbSeriesSize = pByValueSeries->GetSeriesSize();

                        // Add back the base size of the by value class, since it's being transplanted to this class
                        cbSeriesSize += pByValueMT->GetBaseSize();

                        // Subtract the base size of the class we're building
                        cbSeriesSize -= pMT->GetBaseSize();

                        // Set current series we're building
                        pSeries->SetSeriesSize(cbSeriesSize);

                        // Get offset into the value class of the first pointer field (includes a +Object)
                        cbSeriesOffset = pByValueSeries->GetSeriesOffset();

                        // Add it to the offset of the by value class in our class
                        cbSeriesOffset += dwCurrentOffset;

                        pSeries->SetSeriesOffset(cbSeriesOffset); // Offset of field
                        pSeries++;
                        pByValueSeries++;
                    }
                }
            }
        }

        // Adjust the inherited series - since the base size has increased by "# new field instance bytes", we need to
        // subtract that from all the series (since the series always has BaseSize subtracted for it - see gcdesc.h)
        pHighest = CGCDesc::GetCGCDescFromMT(pMT)->GetHighestSeries();
        while (pSeries <= pHighest)
        {
            CONSISTENCY_CHECK(CheckPointer(GetParentClass()));
            pSeries->SetSeriesSize( pSeries->GetSeriesSize() - ((size_t) pMT->GetBaseSize() - (size_t) GetParentClass()->GetMethodTable()->GetBaseSize()) );
            pSeries++;
        }

        _ASSERTE(pSeries-1 <= CGCDesc::GetCGCDescFromMT(pMT)->GetHighestSeries());
    }

}

//*******************************************************************************
//
// Helper method for VerifyInheritanceSecurity
//
VOID MethodTableBuilder::VerifyClassInheritanceSecurityHelper(
                                    EEClass *pParentCls,
                                    EEClass *pChildCls)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pParentCls));
        PRECONDITION(CheckPointer(pChildCls));
    } CONTRACTL_END;
    //@ASSUMPTION: The current class has been resolved to the point that
    // we can construct a reflection object on the class or its methods.
    // This is required for the security checks.

    // Check the entire parent chain for inheritance permission demands.
    while (pParentCls != NULL)
    {
        if (pParentCls->RequiresInheritanceCheck())
        {
            // This method throws on failure.
            Security::ClassInheritanceCheck(pChildCls, pParentCls);
        }

        pParentCls = pParentCls->GetParentClass();
    }
}

//*******************************************************************************
//
// Helper method for VerifyInheritanceSecurity
//
VOID MethodTableBuilder::VerifyMethodInheritanceSecurityHelper(
                                                       MethodDesc *pParentMD,
                                                       MethodDesc *pChildMD)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pParentMD));
        PRECONDITION(CheckPointer(pChildMD));
    } CONTRACTL_END;

    // If no inheritance checks are required, just return.
    if (!pParentMD->RequiresInheritanceCheck() &&
        !pParentMD->ParentRequiresInheritanceCheck())
    {
        return;
    }

    DWORD dwSlot = pParentMD->GetSlot();

#ifdef _DEBUG
    // Get the name and signature for the method so we can find the new parent method desc.
    // We use the parent MethodDesc for this because the child could actually have a very
    // different name in the case that the child is MethodImpling the parent.

    // Get the name.
    LPCUTF8            szName;
    szName = pParentMD->GetName();

    // Get the signature.
    PCCOR_SIGNATURE    pSignature;
    DWORD              cSignature;
    pParentMD->GetSig(&pSignature, &cSignature);
    Module            *pModule = pParentMD->GetModule();
#endif

    do
    {
        if (pParentMD->RequiresInheritanceCheck())
        {
            Security::MethodInheritanceCheck(pChildMD, pParentMD);
        }

        if (pParentMD->ParentRequiresInheritanceCheck())
        {
            MethodTable *pGrandParentMT = pParentMD->GetMethodTable()->GetParentMethodTable();
            CONSISTENCY_CHECK(CheckPointer(pGrandParentMT));

            // Find this method in the parent.
            // If it does exist in the parent, it would be at the same vtable slot.
            if (dwSlot >= pGrandParentMT->GetNumVirtuals())
            {
                // Parent does not have this many vtable slots, so it doesn't exist there
                pParentMD = NULL;
            }
            else
            {
                // It is in the vtable of the parent
                pParentMD = pGrandParentMT->GetUnknownMethodDescForSlot(dwSlot);
                _ASSERTE(pParentMD != NULL);

#ifdef _DEBUG
                _ASSERTE(pParentMD == pGrandParentMT->GetClass()->FindMethod(
                    szName,
                    pSignature,
                    cSignature,
                    pModule));
#endif
            }
        }
        else
        {
            pParentMD = NULL;
        }
    } while (pParentMD != NULL);
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// If we have a non-interface class, then do inheritance security
// checks on it. The check starts by checking for inheritance
// permission demands on the current class. If these first checks
// succeeded, then the cached declared method list is scanned for
// methods that have inheritance permission demands.
//

void MethodTableBuilder::VerifyInheritanceSecurity()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    // If we have a non-interface class, then do inheritance security
    // checks on it. The check starts by checking for inheritance
    // permission demands on the current class. If these first checks
    // succeeded, then the cached declared method list is scanned for
    // methods that have inheritance permission demands.
    if (!IsInterface() && (bmtInternal->pModule->IsSystem() == FALSE) &&
        Security::IsSecurityOn())
    {
        //We need to disable preemptive GC if there's any chance that it could still be
        //active.  The inheritance checks might allocate objects.
        GCX_COOP();

        if (GetParentClass() != NULL)
        {
            // Check the parent for inheritance permission demands.
            VerifyClassInheritanceSecurityHelper(GetParentClass(), GetHalfBakedClass());

            // Iterate all the declared methods and check each of them for inheritance demands
            DeclaredMethodIterator mIt(*this);
            while (mIt.Next())
            {
                _ASSERTE(mIt.GetMethodDesc() != NULL);

                if (mIt.GetParentMethodDesc() != NULL)
                {
                    VerifyMethodInheritanceSecurityHelper(mIt.GetParentMethodDesc(), mIt.GetMethodDesc());
                }

                // If this method is a MethodImpl, we need to verify that all
                // decls are allowed to be overridden.
                if (mIt.GetMethodDesc()->IsMethodImpl())
                {
                    // Iterate through each decl that this method is an impl for and
                    // test that inheritance demands are met.
                    MethodImpl *pMethodImpl = mIt.GetMethodDesc()->GetMethodImpl();
                    for (DWORD iCurImpl = 0; iCurImpl < pMethodImpl->GetSize(); iCurImpl++)
                    {
                        MethodDesc *pDeclMD = pMethodImpl->GetImplementedMDs()[iCurImpl];
                        _ASSERTE(pDeclMD != NULL);
                        // We deal with interfaces below, so don't duplicate work
                        if (!pDeclMD->IsInterface())
                        {
                            VerifyMethodInheritanceSecurityHelper(pDeclMD, mIt.GetMethodDesc());
                        }
                    }
                }
            }
        }

        // Now we need to verify that we are meeting all inheritance demands
        // that were placed on interfaces and their methods. The logic is as
        // follows: for each method contributing an implementation to this type,
        // if a method it could contribute to any interface described in the
        // interface map, check that both method-level and type-level inheritance
        // demands are met (only need to check type-level once per interface).
        {
            // Iterate through each interface
            MethodTable *pMT = GetHalfBakedClass()->GetMethodTable();
            MethodTable::InterfaceMapIterator itfIt = pMT->IterateInterfaceMap();
            while (itfIt.Next())
            {
                // Get current interface details
                EEClass *pCurItfCls = itfIt.GetInterface()->GetClass();
                CONSISTENCY_CHECK(CheckPointer(pCurItfCls));

                if (pCurItfCls->RequiresInheritanceCheck() ||
                    pCurItfCls->SomeMethodsRequireInheritanceCheck())
                {
                    // Keep track of whether or not type-level inheritance demands
                    // have been evaluated for each interface.
                    BOOL fMustEvaluateTypeLevelInheritanceDemand = itfIt.IsDeclaredOnClass();

                    // Now iterate through every method contributing any implementation
                    // and if it lies within the interface vtable, then we must evaluate demands
                    // NOTE: Avoid caching the MethodData object for the type being built.
                    MethodTable::MethodDataWrapper
                        hItfImplData(MethodTable::GetMethodData(itfIt.GetInterface(), pMT, FALSE));
                    MethodTable::MethodIterator methIt(hItfImplData);
                    for (;methIt.IsValid(); methIt.Next())
                    {
                        MethodDesc *pMDImpl = methIt.GetMethodDesc();
                        if (pMDImpl->GetMethodTable() == pMT)
                        {
                            // Check security on the interface for this method in its default slot placement
                            VerifyMethodInheritanceSecurityHelper(methIt.GetDeclMethodDesc(), pMDImpl);

                            fMustEvaluateTypeLevelInheritanceDemand = TRUE;
                        }
                    }

                    // If any previous methods contributed to this interface's implementation, that means we
                    // need to check the type-level inheritance for the interface.
                    if (fMustEvaluateTypeLevelInheritanceDemand)
                    {
                        VerifyClassInheritanceSecurityHelper(pCurItfCls, pMT->GetClass());
                    }
                }
            }
        }
    }
}

//*******************************************************************************
//
// Used by BuildMethodTable
//
// Before we make the final leap, make sure we've allocated all memory needed to
// fill out the RID maps.
//
VOID MethodTableBuilder::EnsureRIDMapsCanBeFilled()
{

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    DWORD i;


    // Rather than call Ensure***CanBeStored() hundreds of times, we
    // will call it once on the largest token we find. This relies
    // on an invariant that RidMaps don't use some kind of sparse
    // allocation.

    {
        mdMethodDef largest = mdMethodDefNil;

        DeclaredMethodIterator it(*this);
        while (it.Next())
        {
            if (it.Token() > largest)
            {
                largest = it.Token();
            }
        }
        if ( largest != mdMethodDefNil )
        {
            bmtInternal->pModule->EnsureMethodDefCanBeStored(largest);
        }
    }

    {
        mdFieldDef largest = mdFieldDefNil;

        for (i = 0; i < bmtMetaData->cFields; i++)
        {
            if (bmtMetaData->pFields[i] > largest)
            {
                largest = bmtMetaData->pFields[i];
            }
            }
            if ( largest != mdFieldDefNil )
            {
                bmtInternal->pModule->EnsureFieldDefCanBeStored(largest);
            }
        }


}

//*******************************************************************************
// Given the (generics-shared or generics-exact) value class method, find the
// (generics-shared) unboxing Stub for the given method .  We search the vtable.
//
// This is needed when creating a delegate to an instance method in a value type
MethodDesc* EEClass::GetBoxedEntryPointMD(MethodDesc *pMD)
{
    CONTRACT (MethodDesc *) {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(IsValueClass());
        PRECONDITION(!pMD->ContainsGenericVariables());
        PRECONDITION(!pMD->IsUnboxingStub());
        POSTCONDITION(RETVAL->IsUnboxingStub());
    } CONTRACT_END;

    RETURN MethodDesc::FindOrCreateAssociatedMethodDesc(pMD,
                                                        pMD->GetMethodTable(),
                                                        TRUE /* get unboxing entry point */,
                                                        pMD->GetNumGenericMethodArgs(),
                                                        pMD->GetMethodInstantiation(),
                                                        FALSE /* no allowInstParam */ );

}

//*******************************************************************************
// Given the unboxing value class method, find the non-unboxing method
// This is used when generating the code for an BoxedEntryPointStub.
MethodDesc* MethodTable::GetUnboxedEntryPointMD(MethodDesc *pMD)
{
    CONTRACT (MethodDesc *) {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(IsValueClass());
        // reflection needs to call this for methods in non instantiated classes,
        // so move the assert to the caller when needed
        //PRECONDITION(!pMD->ContainsGenericVariables());
        PRECONDITION(pMD->IsUnboxingStub());
        POSTCONDITION(!RETVAL->IsUnboxingStub());
    } CONTRACT_END;

    BOOL allowInstParam = (pMD->GetNumGenericMethodArgs() == 0);
    RETURN MethodDesc::FindOrCreateAssociatedMethodDesc(pMD,
                                                        this,
                                                        FALSE /* don't get unboxing entry point */,
                                                        pMD->GetNumGenericMethodArgs(),
                                                        pMD->GetMethodInstantiation(),
                                                        allowInstParam);
}


//*******************************************************************************
// Given the unboxing value class method, find the non-unboxing method
// This is used when generating the code for an BoxedEntryPointStub.
MethodDesc* MethodTable::GetExistingUnboxedEntryPointMD(MethodDesc *pMD)
{
    CONTRACT (MethodDesc *) {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(IsValueClass());
        // reflection needs to call this for methods in non instantiated classes,
        // so move the assert to the caller when needed
        //PRECONDITION(!pMD->ContainsGenericVariables());
        PRECONDITION(pMD->IsUnboxingStub());
        POSTCONDITION(!RETVAL->IsUnboxingStub());
    } CONTRACT_END;

    BOOL allowInstParam = (pMD->GetNumGenericMethodArgs() == 0);
    RETURN MethodDesc::FindOrCreateAssociatedMethodDesc(pMD,
                                                        this,
                                                        FALSE /* don't get unboxing entry point */,
                                                        pMD->GetNumGenericMethodArgs(),
                                                        pMD->GetMethodInstantiation(),
                                                        allowInstParam,
                                                        FALSE, /* forceRemotableMethod */
                                                        FALSE  /* allowCreate */
                                                       );
}

#endif // !DACCESS_COMPILE


//*******************************************************************************
BOOL EEClass::FM_ShouldSkipMethod(DWORD dwAttrs, FM_Flags flags)
{
    LEAF_CONTRACT;

    // If we have any special selection flags, then we need to check a little deeper.
    if (flags & FM_SpecialVirtualMask)
    {
        if (((flags & FM_ExcludeVirtual) && IsMdVirtual(dwAttrs)) ||
            ((flags & FM_ExcludeNonVirtual) && !IsMdVirtual(dwAttrs)))
        {
            return TRUE;
        }
    }

    // This makes for quick shifting in determining if an access mask bit matches
    C_ASSERT((FM_ExcludePrivateScope >> 0x3) == 0x1);

    if (flags & FM_SpecialAccessMask)
    {
        DWORD dwAccess = dwAttrs & mdMemberAccessMask;
        if ((1 << dwAccess) & ((DWORD)(flags & FM_SpecialAccessMask) >> 0x3))
        {
            return TRUE;
        }
    }

    // No exclusion for this method
    return FALSE;
}

//*******************************************************************************
// Finds a method by name and signature, where scope is the scope in which the
// signature is defined.
MethodDesc *EEClass::FindMethod(LPCUTF8 pszName,
                                PCCOR_SIGNATURE pSignature, DWORD cSignature,
                                Module* pModule,
                                const Substitution *pSigSubst,        // = NULL
                                FM_Flags flags,                       // = FM_Default
                                const Substitution *pDefSubst)        // = NULL
{

    CONTRACT (MethodDesc *) {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(!IsThunking());
        MODE_ANY;
    } CONTRACT_END;


#ifndef DACCESS_COMPILE

    // Retrive the right comparition function to use.
    UTF8StringCompareFuncPtr StrCompFunc = FM_GetStrCompFunc(flags);

    // Statistically it's most likely for a method to be found in non-vtable portion of this class's members, then in the
    // vtable of this class's declared members, then in the inherited portion of the vtable, so we search backwards.

    // For value classes, if it's a value class method, we want to return the duplicated MethodDesc, not the one in the vtable
    // section.  We'll find the one in the duplicate section before the one in the vtable section, so we're ok.

    // Search non-vtable portion of this class first

    g_IBCLogger.LogEEClassAndMethodTableAccess(this);

    MethodTable::MethodIterator it(GetMethodTable());

    // Move the iterator to the appropriate starting point
    it.MoveToEnd();

    // Iterate through the methods of the current type searching for a match.
    for (; it.IsValid(); it.Prev())
    {
        MethodDesc *pCurDeclMD = it.GetDeclMethodDesc();
        MethodTable *pCurDeclMT = pCurDeclMD->GetMethodTable();
        CONSISTENCY_CHECK(!IsInterface() || pCurDeclMT == GetMethodTable());

        {
            if (FM_ShouldSkipMethod(pCurDeclMD->GetAttrs(), flags))
            {
                continue;
            }

            BOOL fIgnoreMethodDueToInstantiationCheck = pCurDeclMT->HasInstantiation() && pCurDeclMT != GetMethodTable();
            if (
                !fIgnoreMethodDueToInstantiationCheck
                // This is done last since it is the most expensive of the IF statement.
                && StrCompFunc(pszName, pCurDeclMD->GetName()) == 0
               )
            {
                PCCOR_SIGNATURE pCurMethodSig;
                DWORD       cCurMethodSig;

                pCurDeclMD->GetSig(&pCurMethodSig, &cCurMethodSig);
                if (MetaSig::CompareMethodSigs(pSignature, cSignature, pModule, pSigSubst,pCurMethodSig,
                                               cCurMethodSig, pCurDeclMD->GetModule(), pDefSubst))
                {
                    RETURN pCurDeclMD;
                }
            }
        }
    }

    // No inheritance on value types or interfaces
    if (IsValueClass() || IsInterface())
    {
        RETURN NULL;
    }

    // Recurse up the hierarchy if the method was not found.
    CONSISTENCY_CHECK(GetMethodTable()->CheckLoadLevel(CLASS_LOAD_APPROXPARENTS));

    MethodTable *pParentMT = GetMethodTable()->GetParentMethodTable();
    if (pParentMT != NULL)
    {
        Substitution subst2 = GetSubstitutionForParent(pDefSubst);
        MethodDesc *md = pParentMT->GetClass()->FindMethod(
            pszName, pSignature, cSignature, pModule, pSigSubst, flags, &subst2);

        if (md)
        {
            if (IsMdInstanceInitializer(md->GetAttrs(), pszName))
            {
                md = NULL;
            }
        }

        RETURN md;
    }

    RETURN NULL;
#else // DACCESS_COMPILE
    DacNotImpl();
    RETURN NULL;
#endif // DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE

//*******************************************************************************
// This will return the MethodDesc that implements the interface method <pInterface,slotNum>.
MethodDesc *EEClass::FindMethodForInterfaceSlot(MethodTable *pInterface, WORD slotNum)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION(CheckPointer(pInterface));
        PRECONDITION(pInterface->IsInterface());
        PRECONDITION(slotNum < pInterface->GetNumMethods());
    } CONTRACTL_END;

    MethodDesc *pMDRet = NULL;
    MethodTable *pMT = GetMethodTable();

    DispatchSlot ds(pMT->FindDispatchSlot(pInterface->GetTypeID(), (UINT32)slotNum));
    if (!ds.IsNull()) {
        pMDRet = ds.GetMethodDesc();
    }

    CONSISTENCY_CHECK(CheckPointer(pMDRet));
    return pMDRet;
}

#endif // #ifndef DACCESS_COMPILE

//*******************************************************************************
MethodDesc *EEClass::FindMethod(LPCUTF8 pwzName, LPHARDCODEDMETASIG pwzSignature, FM_Flags flags)
    {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(!IsThunking());
        MODE_ANY;
         SO_TOLERANT;
    } CONTRACTL_END;

    PCCOR_SIGNATURE pBinarySig;
    ULONG       cbBinarySigLength;
    pwzSignature->GetBinarySig(&pBinarySig, &cbBinarySigLength );

    return FindMethod(pwzName, pBinarySig, cbBinarySigLength, SystemDomain::SystemModule(), NULL, flags);
}

//*******************************************************************************
MethodDesc *EEClass::FindMethod(mdMethodDef mb)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(!IsThunking());
        MODE_ANY;
    } CONTRACTL_END;

    // We have the EEClass (this) and so lets just look this up in the ridmap.
    MethodDesc *pMD     = NULL;
    Module     *pModule = GetModule();
    PREFIX_ASSUME(pModule != NULL);

    if (TypeFromToken(mb) == mdtMemberRef)
        pMD = pModule->LookupMemberRefAsMethod(mb);
    else
        pMD = pModule->LookupMethodDef(mb);

    if (pMD != NULL)
        pMD->CheckRestore();

    return pMD;
}

//*******************************************************************************
MethodDesc *EEClass::FindMethodByName(LPCUTF8 pszName, FM_Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(!IsThunking());
        PRECONDITION(!IsArrayClass());
        MODE_ANY;
        SO_TOLERANT;
    } CONTRACTL_END;

#ifndef DACCESS_COMPILE

    // Caching of MethodDescs (impl and decl) for MethodTable slots provided significant
    // performance gain in some reflection emit scenarios.
    MethodTable::AllowMethodDataCaching();

    // Retrive the right comparition function to use.
    UTF8StringCompareFuncPtr StrCompFunc = FM_GetStrCompFunc(flags);

    // Scan all classes in the hierarchy, starting at the current class and
    // moving back up towards the base.
    MethodTable *pMT = GetMethodTable();
    while (pMT != NULL)
    {
        // Iterate through the methods searching for a match.
        MethodTable::MethodIterator it(pMT);
        it.MoveToEnd();
        for (; it.IsValid(); it.Prev())
        {
            MethodDesc *pCurMD = it.GetDeclMethodDesc();

            if (pCurMD != NULL)
            {
                // If we're working from the end of the vtable, we'll cover all the non-virtuals
                // first, and so if we're supposed to ignore virtuals (see setting of the flag
                // below) then we can just break out of the loop and go to the parent.
                if ((flags & FM_ExcludeVirtual) && pCurMD->IsVirtual())
                {
                    break;
                }

                if (FM_ShouldSkipMethod(pCurMD->GetAttrs(), flags))
                {
                    continue;
                }

                if (StrCompFunc(pszName, pCurMD->GetNameOnNonArrayClass()) == 0)
                {
                    MethodDesc* pRetMD = it.GetMethodDesc();
                    pRetMD->CheckRestore();
                    return pRetMD;
                }
            }
        }

        // Check the parent type for a matching method.
        pMT = pMT->GetParentMethodTable();

        // There is no need to check virtuals for parent types, since by definition they have the same name.
        flags = (FM_Flags)(flags | FM_ExcludeVirtual);
    }

    return NULL;
#else // DACCESS_COMPILE
    DacNotImpl();
    RETURN NULL;
#endif // DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE
//*******************************************************************************
MethodDesc *EEClass::FindPropertyMethod(LPCUTF8 pszName, EnumPropertyMethods Method, FM_Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
        PRECONDITION(Method < 2);
    } CONTRACTL_END;

    // The format strings for the getter and setter. These must stay in synch with the
    // EnumPropertyMethods enum defined in class.h
    static const LPCUTF8 aFormatStrings[] =
    {
        "get_%s",
        "set_%s"
    };

    CQuickBytes qbMethName;
    size_t len = strlen(pszName) + strlen(aFormatStrings[Method]) + 1;
    LPUTF8 strMethName = (LPUTF8) qbMethName.AllocThrows(len);
    sprintf_s(strMethName, len, aFormatStrings[Method], pszName);

    return FindMethodByName(strMethName, flags);
}

//*******************************************************************************
MethodDesc *EEClass::FindEventMethod(LPCUTF8 pszName, EnumEventMethods Method, FM_Flags flags)
    {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(Method < 3);
    } CONTRACTL_END;

    // The format strings for the getter and setter. These must stay in synch with the
    // EnumPropertyMethods enum defined in class.h
    static const LPCUTF8 aFormatStrings[] =
    {
        "add_%s",
        "remove_%s",
        "raise_%s"
    };

    CQuickBytes qbMethName;
    size_t len = strlen(pszName) + strlen(aFormatStrings[Method]) + 1;
    LPUTF8 strMethName = (LPUTF8) qbMethName.AllocThrows(len);
    sprintf_s(strMethName, len, aFormatStrings[Method], pszName);

    return FindMethodByName(strMethName, flags);
}


#endif // #ifndef DACCESS_COMPILE

FieldDesc *EEClass::FindField(LPCUTF8 pszName, PCCOR_SIGNATURE pSignature, DWORD cSignature, Module* pModule, BOOL bCaseSensitive)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END



    DWORD       i;
    DWORD       dwFieldDescsToScan;
    IMDInternalImport *pInternalImport = GetMDImport(); // All explicitly declared fields in this class will have the same scope

    CONSISTENCY_CHECK(GetMethodTable()->CheckLoadLevel(CLASS_LOAD_APPROXPARENTS));

    // Retrive the right comparition function to use.
    UTF8StringCompareFuncPtr StrCompFunc = bCaseSensitive ? strcmp : stricmpUTF8;

    // The following assert is very important, but we need to special case it enough
    // to allow us access to the legitimate fields of a context proxy object.
    CONSISTENCY_CHECK(!IsThunking() ||
             !strcmp(pszName, "actualObject") ||
             !strcmp(pszName, "contextID") ||
             !strcmp(pszName, "_rp") ||
             !strcmp(pszName, "_stubData") ||
             !strcmp(pszName, "_pMT") ||
             !strcmp(pszName, "_pInterfaceMT") ||
             !strcmp(pszName, "_stub"));

    // Array classes don't have fields, and don't have metadata
    if (IsArrayClass())
        return NULL;

    MethodTable *pParentMT = GetMethodTable()->GetParentMethodTable();

    // Scan the FieldDescs of this class
    if (pParentMT != NULL)
        dwFieldDescsToScan = m_wNumInstanceFields - pParentMT->GetClass()->m_wNumInstanceFields + m_wNumStaticFields;
    else
        dwFieldDescsToScan = m_wNumInstanceFields + m_wNumStaticFields;

    for (i = 0; i < dwFieldDescsToScan; i++)
    {
        LPCUTF8     szMemberName;
        FieldDesc * pFD = &GetFieldDescListPtr()[i];
        PREFIX_ASSUME(pFD!=NULL);
        mdFieldDef  mdField = pFD->GetMemberDef();

        // Check is valid FieldDesc, and not some random memory
        INDEBUGIMPL(pFD->GetApproxEnclosingMethodTable()->SanityCheck());

        szMemberName = pInternalImport->GetNameOfFieldDef(mdField);

        if (StrCompFunc(szMemberName, pszName) != 0)
        {
            continue;
        }

        if (pSignature != NULL)
        {
            PCCOR_SIGNATURE pMemberSig;
            DWORD       cMemberSig;

            pMemberSig = pInternalImport->GetSigOfFieldDef(
                mdField,
                &cMemberSig
            );

            if (!MetaSig::CompareFieldSigs(
                    pMemberSig,
                    cMemberSig,
                    GetModule(),
                    pSignature,
                    cSignature,
                    pModule))
                {
                continue;
            }
        }

        return pFD;
    }

    return NULL;
}

#ifndef DACCESS_COMPILE

//*******************************************************************************
MethodDesc *EEClass::FindConstructor(LPHARDCODEDMETASIG pwzSignature)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END

    PCCOR_SIGNATURE pBinarySig;
    ULONG       cbBinarySigLength;

    pwzSignature->GetBinarySig(&pBinarySig, &cbBinarySigLength);

    return FindConstructor(pBinarySig, cbBinarySigLength, SystemDomain::SystemModule());
}

//*******************************************************************************
MethodDesc *EEClass::FindConstructor(PCCOR_SIGNATURE pSignature,DWORD cSignature, Module* pModule)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;
    }
    CONTRACTL_END


    // Array classes don't have metadata
    if (IsArrayClass())
        return NULL;

    MethodTable::MethodIterator it(this->GetMethodTable());
    for (; it.IsValid(); it.Next()) {
        if (it.IsVirtual()) {
            continue;
        }

            MethodDesc *pCurMethod = it.GetMethodDesc();
            if (pCurMethod == NULL) {
            continue;
            }

            DWORD dwCurMethodAttrs = pCurMethod->GetAttrs();
            if(!IsMdRTSpecialName(dwCurMethodAttrs)) {
            continue;
            }

        // Don't want class initializers.
            if (IsMdStatic(dwCurMethodAttrs)) {
            continue;
            }

        // Find only the constructor for for this object
        _ASSERTE(pCurMethod->GetMethodTable() == this->GetMethodTable());

            PCCOR_SIGNATURE pCurMethodSig;
            DWORD cCurMethodSig;
        pCurMethod->GetSig(&pCurMethodSig, &cCurMethodSig);
            if (MetaSig::CompareMethodSigs(pSignature, cSignature, pModule, NULL, pCurMethodSig, cCurMethodSig, pCurMethod->GetModule(), NULL)) {
                return pCurMethod;
        }
    }

    return NULL;
}



#endif // !DACCESS_COMPILE

//*******************************************************************************
//
// Helper routines for the macros defined at the top of this class.
// You probably should not use these functions directly.
//
SString &EEClass::_GetFullyQualifiedNameForClassNestedAware(SString &ssBuf)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    } CONTRACTL_END;

    ssBuf.Clear();

    LPCUTF8 pszNamespace;
    LPCUTF8 pszName;
    pszName = GetFullyQualifiedNameInfo(&pszNamespace);
    if (pszName == NULL)
    {
        return ssBuf;
    }

    StackSString ssName(SString::Utf8, pszName);

    mdTypeDef mdEncl = GetCl();
    IMDInternalImport *pImport = GetMDImport();

    // Check if the type is nested
    DWORD dwAttr;
    pImport->GetTypeDefProps(GetCl(), &dwAttr, NULL);

    if (IsTdNested(dwAttr))
    {
        StackSString ssFullyQualifiedName;
        StackSString ssPath;

        // Build the nesting chain.
        while (SUCCEEDED(pImport->GetNestedClassProps(mdEncl, &mdEncl)))
        {
            LPCUTF8 szEnclName;
            LPCUTF8 szEnclNameSpace;
            pImport->GetNameOfTypeDef(mdEncl,
                                      &szEnclName,
                                      &szEnclNameSpace);

            ns::MakePath(ssPath, StackSString(SString::Utf8, szEnclNameSpace), StackSString(SString::Utf8, szEnclName));
            ns::MakeNestedTypeName(ssFullyQualifiedName, ssPath, ssName);

            ssName = ssFullyQualifiedName;
        }
    }

    ns::MakePath(ssBuf, StackSString(SString::Utf8, pszNamespace), ssName);

    return ssBuf;
}

//*******************************************************************************
SString &EEClass::_GetFullyQualifiedNameForClass(SString &ssBuf)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    ssBuf.Clear();

    if (IsArrayClass())
    {
        TypeDesc::ConstructName(GetMethodTable()->GetInternalCorElementType(),
                                GetMethodTable()->GetApproxArrayElementTypeHandle_NoLogging(),
                                GetMethodTable()->GetRank(),
                                ssBuf);
    }
    else if (!IsNilToken(GetCl()))
    {
        LPCUTF8 szNamespace;
        LPCUTF8 szName;
        GetMDImport()->GetNameOfTypeDef(GetCl(), &szName, &szNamespace);

        ns::MakePath(ssBuf,
                     StackSString(SString::Utf8, szNamespace),
                     StackSString(SString::Utf8, szName));
    }

    return ssBuf;
}

//
//
LPCUTF8 EEClass::GetFullyQualifiedNameInfo(LPCUTF8 *ppszNamespace)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (IsArrayClass())
    {
        *ppszNamespace = NULL;
        return NULL;
    }
    else
    {
        LPCUTF8 szName;
        GetMDImport()->GetNameOfTypeDef(GetCl(), &szName, ppszNamespace);
        return szName;
    }
}

//*******************************************************************************
DWORD EEClass::GetInstAndDictSize()
{
    LEAF_CONTRACT;
    if (!HasInstantiation())
        return 0;
    else
        return DictionaryLayout::GetFirstDictionaryBucketSize(GetNumGenericArgs(), GetDictionaryLayout());
}

#ifndef DACCESS_COMPILE


//*******************************************************************************
void EEClass::DebugRecursivelyDumpInstanceFields(LPCUTF8 pszClassName, BOOL debug)
{
    WRAPPER_CONTRACT;  // It's a dev helper, who cares about contracts

    EX_TRY
    {
        StackSString ssBuff;

        DWORD cParentInstanceFields;
        DWORD i;

        CONSISTENCY_CHECK(GetMethodTable()->CheckLoadLevel(CLASS_LOAD_APPROXPARENTS));

        MethodTable *pParentMT = GetMethodTable()->GetParentMethodTable();
        if (pParentMT != NULL)
        {
            cParentInstanceFields = pParentMT->GetClass()->m_wNumInstanceFields;
            DefineFullyQualifiedNameForClass();
            LPCUTF8 name = GetFullyQualifiedNameForClass(pParentMT->GetClass());
            pParentMT->GetClass()->DebugRecursivelyDumpInstanceFields(name, debug);
        }
        else
        {
            cParentInstanceFields = 0;
        }

        // Are there any new instance fields declared by this class?
        if (m_wNumInstanceFields > cParentInstanceFields)
        {
            // Display them
            if(debug) {
                ssBuff.Printf(L"%S:\n", pszClassName);
                WszOutputDebugString(ssBuff.GetUnicode());
            }
            else {
                 LOG((LF_CLASSLOADER, LL_ALWAYS, "%s:\n", pszClassName));
            }

            for (i = 0; i < (m_wNumInstanceFields-cParentInstanceFields); i++)
            {
                FieldDesc *pFD = &m_pFieldDescList[i];
                // printf("offset %s%3d %s\n", pFD->IsByValue() ? "byvalue " : "", pFD->GetOffset_NoLogging(), pFD->GetName());
                if(debug) {
                    ssBuff.Printf(L"offset %3d %S\n", pFD->GetOffset_NoLogging(), pFD->GetName());
                    WszOutputDebugString(ssBuff.GetUnicode());
                }
                else {
                    LOG((LF_CLASSLOADER, LL_ALWAYS, "offset %3d %s\n", pFD->GetOffset_NoLogging(), pFD->GetName()));
                }
            }
        }
    }
    EX_CATCH
    {
        if(debug)
        {
            WszOutputDebugString(L"<Exception Thrown>\n");
        }
        else
        {
             LOG((LF_CLASSLOADER, LL_ALWAYS, "<Exception Thrown>\n"));
        }
    }
    EX_END_CATCH(SwallowAllExceptions);
}

//*******************************************************************************
void EEClass::DebugDumpFieldLayout(LPCUTF8 pszClassName, BOOL debug)
{
    WRAPPER_CONTRACT;   // It's a dev helper, who cares about contracts

    if (m_wNumStaticFields == 0 && m_wNumInstanceFields == 0)
        return;

    EX_TRY
    {
        StackSString ssBuff;

        DWORD   i;
        DWORD   cParentInstanceFields;

        CONSISTENCY_CHECK(GetMethodTable()->CheckLoadLevel(CLASS_LOAD_APPROXPARENTS));

        if (GetParentClass() != NULL)
            cParentInstanceFields = GetParentClass()->m_wNumInstanceFields;
        else
            cParentInstanceFields = 0;

        if(debug) {
            ssBuff.Printf(L"Field layout for '%S':\n\n", pszClassName);
            WszOutputDebugString(ssBuff.GetUnicode());
        }
        else {
            LOG((LF_CLASSLOADER, LL_ALWAYS, "Field layout for '%s':\n\n", pszClassName));
        }

        if (m_wNumStaticFields > 0)
        {
            if(debug) {
                WszOutputDebugString(L"Static fields (stored at vtable offsets)\n");
                WszOutputDebugString(L"----------------------------------------\n");
            }
            else {
                LOG((LF_CLASSLOADER, LL_ALWAYS, "Static fields (stored at vtable offsets)\n"));
                LOG((LF_CLASSLOADER, LL_ALWAYS, "----------------------------------------\n"));
            }

            for (i = 0; i < m_wNumStaticFields; i++)
            {
                FieldDesc *pFD = &m_pFieldDescList[(m_wNumInstanceFields-cParentInstanceFields) + i];
                if(debug) {
                    ssBuff.Printf(L"offset %3d %S\n", pFD->GetOffset_NoLogging(), pFD->GetName());
                    WszOutputDebugString(ssBuff.GetUnicode());
            }
                else {
                    LOG((LF_CLASSLOADER, LL_ALWAYS, "offset %3d %s\n", pFD->GetOffset_NoLogging(), pFD->GetName()));
        }
            }
        }

        if (m_wNumInstanceFields > 0)
        {
            if (m_wNumStaticFields) {
                if(debug) {
                    WszOutputDebugString(L"\n");
                }
                else {
                    LOG((LF_CLASSLOADER, LL_ALWAYS, "\n"));
                }
            }

            if(debug) {
                WszOutputDebugString(L"Instance fields\n");
                WszOutputDebugString(L"---------------\n");
            }
            else {
                LOG((LF_CLASSLOADER, LL_ALWAYS, "Instance fields\n"));
                LOG((LF_CLASSLOADER, LL_ALWAYS, "---------------\n"));
            }

            DebugRecursivelyDumpInstanceFields(pszClassName, debug);
        }

        if(debug) {
            WszOutputDebugString(L"\n");
        }
        else {
            LOG((LF_CLASSLOADER, LL_ALWAYS, "\n"));
        }
    }
    EX_CATCH
    {
        if(debug)
        {
            WszOutputDebugString(L"<Exception Thrown>\n");
        }
        else
        {
             LOG((LF_CLASSLOADER, LL_ALWAYS, "<Exception Thrown>\n"));
        }
    }
    EX_END_CATCH(SwallowAllExceptions);
}

//*******************************************************************************
void EEClass::DebugDumpGCDesc(LPCUTF8 pszClassName, BOOL debug)
{
    WRAPPER_CONTRACT;   // It's a dev helper, who cares about contracts

    EX_TRY
    {
        StackSString ssBuff;

        if(debug) {
            ssBuff.Printf(L"GC description for '%S':\n\n", pszClassName);
            WszOutputDebugString(ssBuff.GetUnicode());
        }
        else {
            LOG((LF_CLASSLOADER, LL_ALWAYS, "GC description for '%s':\n\n", pszClassName));
        }

        if (GetMethodTable()->ContainsPointers())
        {
            CGCDescSeries *pSeries;
            CGCDescSeries *pHighest;

            if(debug) {
                WszOutputDebugString(L"GCDesc:\n");
            } else {
                LOG((LF_CLASSLOADER, LL_ALWAYS, "GCDesc:\n"));
            }

            pSeries  = CGCDesc::GetCGCDescFromMT(GetMethodTable())->GetLowestSeries();
            pHighest = CGCDesc::GetCGCDescFromMT(GetMethodTable())->GetHighestSeries();

            while (pSeries <= pHighest)
            {
                if(debug) {
                    ssBuff.Printf(L"   offset %5d (%d w/o Object), size %5d (%5d w/o BaseSize subtr)\n",
                        pSeries->GetSeriesOffset(),
                        pSeries->GetSeriesOffset() - sizeof(Object),
                        pSeries->GetSeriesSize(),
                        pSeries->GetSeriesSize() + GetMethodTable()->GetBaseSize() );
                    WszOutputDebugString(ssBuff.GetUnicode());
                }
                else {
                    LOG((LF_CLASSLOADER, LL_ALWAYS, "   offset %5d (%d w/o Object), size %5d (%5d w/o BaseSize subtr)\n",
                         pSeries->GetSeriesOffset(),
                         pSeries->GetSeriesOffset() - sizeof(Object),
                         pSeries->GetSeriesSize(),
                         pSeries->GetSeriesSize() + GetMethodTable()->GetBaseSize()
                         ));
                }
                pSeries++;
            }


            if(debug) {
                WszOutputDebugString(L"\n");
            } else {
                LOG((LF_CLASSLOADER, LL_ALWAYS, "\n"));
            }
        }
    }
    EX_CATCH
    {
        if(debug)
        {
            WszOutputDebugString(L"<Exception Thrown>\n");
        }
        else
        {
             LOG((LF_CLASSLOADER, LL_ALWAYS, "<Exception Thrown>\n"));
        }
    }
    EX_END_CATCH(SwallowAllExceptions);
}


// For X86, INSTALL_COMPLUS_EXCEPTION_HANDLER grants us sufficient protection to call into
// managed code.
//
// But on 64-bit, the personality routine will not pop frames or trackers as exceptions unwind
// out of managed code.  Instead, we rely on explicit cleanup like CLRException::HandlerState::CleanupTry
// or UMThunkUnwindFrameChainHandler.
//
// So most callers should call through CallDescrWorkerWithHandler (or a wrapper like MethodDesc::Call)
// and get the platform-appropriate exception handling.  A few places try to optimize by calling direct
// to managed methods (see ArrayInitializeWorker or FastCallFinalize).  This sort of thing is
// dangerous.  You have to worry about marking yourself as a legal managed caller and you have to
// worry about how exceptions will be handled on a WIN64EXCEPTIONS plan.  It is generally only suitable
// for X86.

//*******************************************************************************
extern "C" ARG_SLOT CallDescrWorkerWithHandler(
                LPVOID                      pSrcEnd,
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
                UINT32                      fpReturnSize,
                LPVOID                      pTarget,
                BOOL                        fCriticalCall)
{
    ARG_SLOT retval = 0;
    BEGIN_CALL_TO_MANAGEDEX(fCriticalCall ? EEToManagedCriticalCall : EEToManagedDefault);
    retval = CallDescrWorker(pSrcEnd,
                             numStackSlots,
#ifdef CALLDESCR_ARGREGS
                             pArgumentRegisters,
#endif
#ifdef CALLDESCR_REGTYPEMAP
                             dwRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF
                             pRetBuff,
                             cbRetBuff,
#endif // CALLDESCR_RETBUF
                             fpReturnSize,
                             pTarget);

    END_CALL_TO_MANAGED();
    return retval;
}


#if !defined(_WIN64) && defined(_DEBUG)

//*******************************************************************************
// assembly code, in i386/asmhelpers.asm
extern "C" ARG_SLOT __stdcall CallDescrWorkerInternal(
                LPVOID                      pSrcEnd,
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


extern "C" ARG_SLOT __stdcall CallDescrWorker(
                LPVOID                      pSrcEnd,
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
                LPVOID                      pTarget)
{
    //
    // This function must not have a contract ... it's caller has pushed an FS:0 frame (COMPlusFrameHandler) that must
    // be the first handler on the stack. The contract causes, at a minimum, a C++ exception handler to be pushed to
    // handle the destruction of the contract object. If there is an exception in the managed code called from here,
    // and that exception is handled in that same block of managed code, then the COMPlusFrameHandler will actually
    // unwind the C++ handler before branching to the catch clause in managed code. That essentially causes an
    // out-of-order destruction of the contract object, resulting in very odd crashes later.
    //

    TRIGGERSGC_NOSTOMP(); // Can't stomp object refs because they are args to the function

    ARG_SLOT retValue;

    // Save a copy of dangerousObjRefs in table.
    Thread* curThread;
    DWORD_PTR ObjRefTable[OBJREF_TABSIZE];

    curThread = GetThread();
    _ASSERTE(curThread != NULL);

    C_ASSERT(sizeof(curThread->dangerousObjRefs) == sizeof(ObjRefTable));
    memcpy(ObjRefTable, curThread->dangerousObjRefs, sizeof(ObjRefTable));

    _ASSERTE(curThread->PreemptiveGCDisabled());  // Jitted code expects to be in cooperative mode

    // If current thread owns spinlock or unbreakalble lock, it can not call managed code.
    _ASSERTE(!curThread->HasUnbreakableLock() &&
             (curThread->m_StateNC & Thread::TSNC_OwnsSpinLock) == 0);

    retValue = (ARG_SLOT) CallDescrWorkerInternal (
                    pSrcEnd,
                    numStackSlots,
#ifdef CALLDESCR_ARGREGS
                    pArgumentRegisters,
#endif
#ifdef CALLDESCR_REGTYPEMAP
                    dwRegTypeMap,
#endif
#ifdef CALLDESCR_RETBUF
                    pRetBuff,
                    cbRetBuff,
#endif // CALLDESCR_RETBUF
                    fpRetSize,
                    pTarget);

    // Restore dangerousObjRefs when we return back to EE after call
    memcpy(curThread->dangerousObjRefs, ObjRefTable, sizeof(ObjRefTable));

    TRIGGERSGC();

    ENABLESTRESSHEAP();

    return retValue;
}
#endif // !defined(_WIN64) && defined(_DEBUG)



//*******************************************************************************
Substitution EEClass::GetSubstitutionForParent(const Substitution *pSubst)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    mdToken crExtends;
    DWORD dwAttrClass;

    if (IsArrayClass())
        return Substitution(GetModule(), NULL, pSubst);

    GetMDImport()->GetTypeDefProps(
        GetCl(),
        &dwAttrClass,
        &crExtends
    );

    return Substitution(crExtends, GetModule(), pSubst);
}

#endif // !DACCESS_COMPILE

//*******************************************************************************
Assembly* EEClass::GetAssembly()
{
    WRAPPER_CONTRACT;
    return GetModule()->GetAssembly();
}

//*******************************************************************************
BaseDomain* EEClass::GetDomain()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

#ifndef DACCESS_COMPILE
    Module *pZapModule = GetZapModule();
    if (pZapModule)
    {
        return pZapModule->GetDomain();
    }
    else
    {
        if (!IsGenericTypeDefinition() && HasInstantiation())
        {
            _ASSERTE(m_pMethodTable != NULL && "Cannot call EEClass::GetDomain so early for an instantiated type: use bmtDomain instead?");
            return BaseDomain::ComputeBaseDomain(GetAssembly()->GetDomain(),
                                                 GetNumGenericArgs(),
                                                 GetCanonicalInstantiation());
        }
        else
            return GetAssembly()->GetDomain();
    }
#else // DACCESS_COMPILE
    return NULL;
#endif // DACCESS_COMPILE
}


#ifndef DACCESS_COMPILE


//*******************************************************************************
void EEClass::AddChunk (MethodDescChunk* pNewChunk)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    pNewChunk->SetNextChunk(GetChunks());
    SetChunks(pNewChunk);
}

//*******************************************************************************
struct TempEnumValue
{
    LPCUTF8 name;
    UINT64 value;
};

//*******************************************************************************
class TempEnumValueSorter : public CQuickSort<TempEnumValue>
{
  public:
    TempEnumValueSorter(TempEnumValue *pArray, SSIZE_T iCount)
      : CQuickSort<TempEnumValue>(pArray, iCount) { LEAF_CONTRACT; }

    int Compare(TempEnumValue *pFirst, TempEnumValue *pSecond)
    {
        LEAF_CONTRACT;

        if (pFirst->value == pSecond->value)
            return 0;
        if (pFirst->value > pSecond->value)
            return 1;
        else
            return -1;
    }
};

//*******************************************************************************
int EnumEEClass::GetEnumLogSize()
{
    WRAPPER_CONTRACT;

    switch (GetMethodTable()->GetInternalCorElementType())
    {
    case ELEMENT_TYPE_I1:
    case ELEMENT_TYPE_U1:
    case ELEMENT_TYPE_BOOLEAN:
        return 0;

    case ELEMENT_TYPE_I2:
    case ELEMENT_TYPE_U2:
    case ELEMENT_TYPE_CHAR:
        return 1;

    case ELEMENT_TYPE_I4:
    case ELEMENT_TYPE_U4:
    case ELEMENT_TYPE_R4:
    IN_WIN32(case ELEMENT_TYPE_I:)
    IN_WIN32(case ELEMENT_TYPE_U:)
        return 2;

    case ELEMENT_TYPE_I8:
    case ELEMENT_TYPE_U8:
    case ELEMENT_TYPE_R8:
    IN_WIN64(case ELEMENT_TYPE_I:)
    IN_WIN64(case ELEMENT_TYPE_U:)
        return 3;

    default:
        BAD_FORMAT_NOTHROW_ASSERT(!"Illegal enum type");
        return 0;
    }
}

//*******************************************************************************
HRESULT EnumEEClass::BuildEnumTables()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return E_OUTOFMEMORY;);
    }
    CONTRACTL_END

    HRESULT hr;

    BAD_FORMAT_NOTHROW_ASSERT(IsEnum());

    // Note about synchronization:
    // This routine is synchronized OK without any locking since it's idempotent. (although it
    // may leak in races.)
    // Right now we'll be satisfied with this - external code can lock if appropriate.

    if (EnumTablesBuilt())
        return S_OK;

    IMDInternalImport *pImport = GetMDImport();

    HENUMInternal fields;
    IfFailRet(pImport->EnumInit(mdtFieldDef, GetCl(), &fields));

    //
    // Note that we're fine treating signed types as unsigned, because all we really
    // want to do is sort them based on a convenient strong ordering.
    //

    int logSize = GetEnumLogSize();
    int size = 1<<logSize;

    ULONG fieldCount = pImport->EnumGetCount(&fields)-1; // Omit one for __value field

    if (fieldCount > 0)
    {
        CQuickArray<TempEnumValue> temps;

        if (FAILED(temps.ReSizeNoThrow(fieldCount)))
            return E_OUTOFMEMORY;

        TempEnumValue *pTemps = temps.Ptr();

        // The following is not portable code - it assumes that the address of all union members
        // is the same.
        C_ASSERT(offsetof(MDDefaultValue, m_byteValue) == offsetof(MDDefaultValue, m_usValue));
        C_ASSERT(offsetof(MDDefaultValue, m_ulValue) == offsetof(MDDefaultValue, m_ullValue));

        mdFieldDef field;
        int nTotalInstanceFields = 0;
        while (pImport->EnumNext(&fields, &field))
        {
            if (IsFdStatic(pImport->GetFieldDefProps(field)))
            {
                pTemps->name = pImport->GetNameOfFieldDef(field);

                MDDefaultValue defaultValue;
                IfFailRet(pImport->GetDefaultValue(field, &defaultValue));
                switch (logSize)
                {
                case 0:
                    pTemps->value = defaultValue.m_byteValue;
                    break;
                case 1:
                    pTemps->value = defaultValue.m_usValue;
                    break;
                case 2:
                    pTemps->value = defaultValue.m_ulValue;
                    break;
                case 3:
                    pTemps->value = defaultValue.m_ullValue;
                    break;
                }
                pTemps++;
            }
            else
            {
                nTotalInstanceFields++;
            }
        }

        BAD_FORMAT_NOTHROW_ASSERT((nTotalInstanceFields == 1) && "Zero or Multiple instance fields in an enum!");

        //
        // Check to see if we are already sorted.  This may seem extraneous, but is
        // actually probably the normal case.
        //

        BOOL sorted = TRUE;

        pTemps = temps.Ptr();
        TempEnumValue *pTempsEnd = pTemps + fieldCount - 1;
        while (pTemps < pTempsEnd)
        {
            if (pTemps[0].value > pTemps[1].value)
            {
                sorted = FALSE;
                break;
            }
            pTemps++;
        }

        if (!sorted)
        {
            TempEnumValueSorter sorter(temps.Ptr(), fieldCount);
            sorter.Sort();
        }

        // Last chance to exit race without leaking!
        if (EnumTablesBuilt())
            return S_OK;

        // Overflow check
        if (fieldCount > 0x7fffffff)
            return E_INVALIDARG;
        AllocMemHolder<LPCUTF8> pNames (GetDomain()->GetHighFrequencyHeap()->AllocMem_NoThrow(fieldCount * sizeof(LPCUTF8)));
        if (!pNames)
        {
            return E_OUTOFMEMORY;
        }

        ULONG cbAllocSize = 0;
        if (!ClrSafeInt<ULONG>::multiply(fieldCount, size, cbAllocSize))
            return E_INVALIDARG;
        AllocMemHolder<BYTE> pValues (GetDomain()->GetHighFrequencyHeap()->AllocMem_NoThrow(cbAllocSize));
        if (!pValues)
        {
            return E_OUTOFMEMORY;
        }

        pTemps = temps.Ptr();
        pTempsEnd = pTemps + fieldCount;

        LPCUTF8 *pn = pNames;
        BYTE *pv = pValues;

        while (pTemps < pTempsEnd)
        {
            *pn++ = pTemps->name;
            switch (logSize)
            {
            case 0:
                *pv++ = (BYTE) pTemps->value;
                break;

            case 1:
                *(USHORT*)pv = (USHORT) pTemps->value;
                pv += sizeof(USHORT);
                break;

            case 2:
                *(UINT*)pv = (UINT) pTemps->value;
                pv += sizeof(UINT);
                break;

            case 3:
                *(UINT64*)pv = (UINT64) pTemps->value;
                pv += sizeof(UINT64);
                break;
            }
            pTemps++;
        }

        _ASSERTE( 0 == ( ((UINT_PTR)&m_names) & (sizeof(LPVOID)-1) ));
        if (NULL == InterlockedCompareExchangePointer((volatile PVOID *)&m_names, (PVOID)pNames, NULL))
        {
            pNames.SuppressRelease();
        }

        _ASSERTE( 0 == ( ((UINT_PTR)&m_values) & (sizeof(LPVOID)-1) ));
        if (NULL == InterlockedCompareExchangePointer((volatile PVOID *)&m_values, (PVOID)pValues, NULL))
        {
            pValues.SuppressRelease();
        }


        pImport->EnumClose(&fields);
    }

    m_countPlusOne = fieldCount+1;

    return S_OK;
}


#endif // !DACCESS_COMPILE

//*******************************************************************************
// ApproxFieldDescIterator is used to iterate over fields in a given class.
// It does not includes EnC fields, and not inherited fields.
// <NICE> ApproxFieldDescIterator is only used to iterate over static fields in one place,
// and this will probably change anyway.  After
// we clean this up we should make ApproxFieldDescIterator work
// over instance fields only </NICE>
ApproxFieldDescIterator::ApproxFieldDescIterator()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_iteratorType = 0;
    m_pClass = NULL;
    m_currField = -1;
    m_totalFields = 0;
}

//*******************************************************************************
void ApproxFieldDescIterator::Init(MethodTable *pMT, int iteratorType, BOOL fixupEnC)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    m_iteratorType = iteratorType;
    m_pClass = pMT->GetClass();
    m_currField = -1;

    // This gets non-EnC fields.
    m_totalFields = m_pClass->GetNumIntroducedInstanceFields();

    if (!(iteratorType & (int)INSTANCE_FIELDS))
    {
        // if not handling instances then skip them by setting curr to last one
        m_currField = m_pClass->GetNumIntroducedInstanceFields() - 1;
    }

    if (iteratorType & (int)STATIC_FIELDS)
    {
        m_totalFields += m_pClass->GetNumStaticFields();
    }
}

//*******************************************************************************
FieldDesc* ApproxFieldDescIterator::Next()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // This will iterate through all non-inherited and non-EnC fields.
    ++m_currField;
    if (m_currField >= m_totalFields)
    {
        return NULL;
    }

    return (m_pClass->GetFieldDescListPtr()) + m_currField;
}

//*******************************************************************************
bool
DeepFieldDescIterator::NextClass()
{
    WRAPPER_CONTRACT;

    if (m_curClass <= 0)
    {
        return false;
    }

    if (m_numClasses <= 0) {
        _ASSERTE(m_numClasses > 0);
        return false;
    }

    EEClass* cls;

    //
    // If we're in the cache just grab the cache entry.
    //
    // If we're deeper in the hierarchy than the
    // portion we cached we need to take the
    // deepest cache entry and search down manually.
    //

    if (--m_curClass < m_numClasses)
    {
        cls = m_classes[m_curClass];
    }
    else
    {
        cls = m_classes[m_numClasses - 1];
        int depthDiff = m_curClass - m_numClasses + 1;
        while (depthDiff--)
        {
            cls = cls->GetParentClass();
        }
    }

    m_fieldIter.Init(cls->GetMethodTable(), m_fieldIter.GetIteratorType());
    return true;
}

//*******************************************************************************
void
DeepFieldDescIterator::Init(MethodTable* pMT, int iteratorType,
                            bool includeParents)
{
    WRAPPER_CONTRACT;

    EEClass* lastClass = NULL;
    int numClasses;

    //
    // Walk up the parent chain, collecting
    // parent pointers and counting fields.
    //

    numClasses = 0;
    m_numClasses = 0;
    m_deepTotalFields = 0;
    m_lastNextFromParentClass = false;

    EEClass *cls = pMT->GetClass();
    while (cls)
    {
        if (m_numClasses < (int)NumItems(m_classes))
        {
            m_classes[m_numClasses++] = cls;
        }

        if ((iteratorType & ApproxFieldDescIterator::INSTANCE_FIELDS) != 0)
        {
            m_deepTotalFields += cls->GetNumIntroducedInstanceFields();
        }
        if ((iteratorType & ApproxFieldDescIterator::STATIC_FIELDS) != 0)
        {
            m_deepTotalFields += cls->GetNumStaticFields();
        }

        numClasses++;
        lastClass = cls;

        if (includeParents)
        {
            cls = cls->GetParentClass();
        }
        else
        {
            break;
        }
    }

    // Start the per-class field iterator on the base-most parent.
    if (numClasses)
    {
        m_curClass = numClasses - 1;
        m_fieldIter.Init(lastClass->GetMethodTable(), iteratorType);
    }
    else
    {
        m_curClass = 0;
    }
}

//*******************************************************************************
FieldDesc*
DeepFieldDescIterator::Next()
{
    WRAPPER_CONTRACT;

    FieldDesc* field;

    do
    {
        m_lastNextFromParentClass = m_curClass > 0;

        field = m_fieldIter.Next();

        if (!field && !NextClass())
        {
            return NULL;
        }
    }
    while (!field);

    return field;
}

//*******************************************************************************
bool
DeepFieldDescIterator::Skip(int numSkip)
{
    WRAPPER_CONTRACT;

    while (numSkip >= m_fieldIter.CountRemaining())
    {
        numSkip -= m_fieldIter.CountRemaining();

        if (!NextClass())
        {
            return false;
        }
    }

    while (numSkip--)
    {
        m_fieldIter.Next();
    }

    return true;
}

#ifdef DACCESS_COMPILE

//*******************************************************************************
void
EEClass::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();
    EMEM_OUT(("MEM: %p EEClass\n", PTR_HOST_TO_TADDR(this)));
    if (flags != CLRDATA_ENUM_MEM_MINI)
    {
    if (m_pModule.IsValid())
    {
        m_pModule->EnumMemoryRegions(flags, true);
    }
    if (m_pMethodTable.IsValid())
    {
        m_pMethodTable->EnumMemoryRegions(flags);
    }
        PTR_MethodDescChunk chunk = GetChunks();
        while (chunk.IsValid())
        {
            chunk->EnumMemoryRegions(flags);
            chunk = chunk->m_next;
        }
    }
    if (GetFieldDescListPtr().IsValid() &&
        (IsValueClass() ||
         (m_pMethodTable.IsValid() && GetMethodTable()->IsRestored())))
    {
        // add one to make sos's code happy.
        DacEnumMemoryRegion((TADDR)m_pFieldDescList_UseAccessor,
                            (GetNumIntroducedInstanceFields() +
                             GetNumStaticFields() + 1) *
                            sizeof(FieldDesc));
    }

}

#endif // DACCESS_COMPILE

//*******************************************************************************
void MethodTableBuilder::bmtMethodImplInfo::AddMethod(MethodDesc* pImplDesc, MethodDesc* pDesc, mdToken mdDecl, Substitution *pDeclSubst)
{
    LEAF_CONTRACT;
    _ASSERTE((pDesc == NULL || mdDecl == mdTokenNil) && (pDesc != NULL || mdDecl != mdTokenNil));
    rgEntries[pIndex].pDeclDesc = pDesc;
    rgEntries[pIndex].declToken = mdDecl;
    rgEntries[pIndex].declSubst = *pDeclSubst;
    rgEntries[pIndex].pBodyDesc = pImplDesc;
    pIndex++;
}

//*******************************************************************************
// Returns TRUE if tok acts as a body for any methodImpl entry. FALSE, otherwise.
BOOL MethodTableBuilder::bmtMethodImplInfo::IsBody(mdToken tok)
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(TypeFromToken(tok) == mdtMethodDef);
    for (DWORD i = 0; i < pIndex; i++) {
        if (GetBodyMethodDesc(i)->GetMemberDef() == tok) {
            return TRUE;
        }
    }
    return FALSE;
}


//-------------------------------------------------------------------------------
// Make best-case effort to obtain an image name for use in an error message.
//
// This routine must expect to be called before the this object is fully loaded.
// It can return an empty if the name isn't available or the object isn't initialized
// enough to get a name, but it mustn't crash.
//-------------------------------------------------------------------------------
void EEClass::GetPathForErrorMessages(SString & result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    Module *pModule = GetModule();
    if (pModule)
    {
        pModule->GetPathForErrorMessages(result);
    }
    else
    {
        result = L"";
    }
}


//-------------------------------------------------------------------------------
// Make best-case effort to obtain an image name for use in an error message.
//
// This routine must expect to be called before the this object is fully loaded.
// It can return an empty if the name isn't available or the object isn't initialized
// enough to get a name, but it mustn't crash.
//-------------------------------------------------------------------------------
void MethodTableBuilder::GetPathForErrorMessages(SString & result)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    EEClass *pClass = m_pHalfBakedClass;
    if (pClass)
    {
        pClass->GetPathForErrorMessages(result);
    }
    else
    {
        result = L"";
    }
}




