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
// File: typehash.cpp
//
#include "common.h"
#include "excep.h"
#include "typehash.h"
#include "eeconfig.h"
#include "generics.h"
#include "typestring.h"
#include "typedesc.h"
#include "typekey.h"

#ifndef DACCESS_COMPILE

// ============================================================================
// Class hash table methods
// ============================================================================
/* static */
EETypeHashTable *EETypeHashTable::Create(BaseDomain *pDomain, Module *pModule, DWORD dwNumBuckets, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    size_t size = sizeof(EETypeHashTable);

    BYTE *              pMem;
    EETypeHashTable *  pThis;

    _ASSERTE( dwNumBuckets >= 0 );
    S_SIZE_T cbAllocSize = S_SIZE_T( dwNumBuckets ) 
                            * S_SIZE_T( sizeof(EETypeHashEntry_t*) )
                            + S_SIZE_T( size );
    if( cbAllocSize.IsOverflow() ) 
    {
        ThrowHR(E_INVALIDARG);
    }

    pMem = (BYTE *)pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem( cbAllocSize.Value() ));   
    pThis = (EETypeHashTable *) pMem;

#ifdef _DEBUG
    pThis->m_dwDebugMemory = (DWORD)(size + dwNumBuckets*sizeof(EETypeHashEntry_t*));
    pThis->m_dwSealCount = 0;
#endif

    pThis->m_dwNumBuckets = dwNumBuckets;
    pThis->m_dwNumEntries = 0;
    pThis->m_pBuckets = (EETypeHashEntry_t**) (pMem + size);
    pThis->m_pModule = pModule;
    pThis->m_pDomain = pDomain;

    return pThis;
}

BaseDomain *EETypeHashTable::GetDomain()
{
    WRAPPER_CONTRACT;

    if (m_pDomain)
    {
        return m_pDomain;
    }
    else
    {
        _ASSERTE(m_pModule != NULL);
        return m_pModule->GetDomain();
    }
}

#endif // #ifdef DACCESS_COMPILE

void EETypeHashTable::Iterator::Reset()
{
    WRAPPER_CONTRACT;

    if (m_pTable)
    {
#ifdef _DEBUG
        m_pTable->Unseal();
#endif
        m_pTable = NULL;
    }

    Init();
}

void EETypeHashTable::Iterator::Init()
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    if (m_pTable)
        m_pTable->Seal(); // The table cannot be changing while it is being iterated
#endif

    m_pCurBucket = NULL;
}

EETypeHashTable::Iterator::Iterator()
{
    WRAPPER_CONTRACT; 
    m_pTable = NULL;
    Init(); 
}

EETypeHashTable::Iterator::Iterator(EETypeHashTable * pTable)
{
    WRAPPER_CONTRACT;
    m_pTable = pTable;
    Init();
}

EETypeHashTable::Iterator::~Iterator()
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    if (m_pTable)
        m_pTable->Unseal(); 
#endif
}

BOOL EETypeHashTable::FindNext(Iterator *it, EETypeHashEntry **ppEntry)
{
    LEAF_CONTRACT;

    *ppEntry = NULL;

    // Fresh iterator
    if (it->m_pCurBucket == NULL)
    {
        it->m_pCurBucket = m_pBuckets;
        it->m_pCurEntry = *(it->m_pCurBucket);
    }
    // Move to the next entry
    else 
    {
        if (it->m_pCurEntry != NULL)
        {
            it->m_pCurEntry = it->m_pCurEntry->pNext;
        }
    }

    if (it->m_pCurEntry == NULL)
    {
        if (it->m_pCurBucket == (m_pBuckets + m_dwNumBuckets))
            return FALSE;

        while (++(it->m_pCurBucket) < (m_pBuckets + m_dwNumBuckets))
        {
            if (*(it->m_pCurBucket) != NULL)
            {
                it->m_pCurEntry = *(it->m_pCurBucket);
                *ppEntry = *(it->m_pCurBucket);
                return TRUE;
            }
        }
    }
    else
    {
        *ppEntry = it->m_pCurEntry;
        return TRUE;
    }

    return FALSE;
}

static DWORD HashTypeHandle(DWORD level, TypeHandle t);

// Calculate hash value for a type def or instantiated type def
static DWORD HashPossiblyInstantiatedType(DWORD level, mdTypeDef token, DWORD numGenericArgs, TypeHandle *genericArgs)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(TypeFromToken(token) == mdtTypeDef);
        PRECONDITION(CheckPointer(genericArgs, NULL_OK));
    }
    CONTRACTL_END

    INT_PTR dwHash = 5381;
    
    dwHash = ((dwHash << 5) + dwHash) ^ token;
    if (genericArgs != NULL)
    {
        dwHash = ((dwHash << 5) + dwHash) ^ numGenericArgs;

        // Hash two levels of the hiearchy. A simple nesting of generics instantiations is 
        // pretty common in generic collections, e.g.: ICollection<KeyValuePair<TKey, TValue>>
        if (level < 2)
        {
            // Hash n type parameters
            for (DWORD i = 0; i < numGenericArgs; i++)
            {
                dwHash = ((dwHash << 5) + dwHash) ^ HashTypeHandle(level+1, genericArgs[i]);
            }
        }
    }

    return dwHash;
}

// Calculate hash value for a function pointer type
static DWORD HashFnPtrType(DWORD level, BYTE callConv, DWORD numArgs, TypeHandle *retAndArgTypes)
{
    WRAPPER_CONTRACT;
    INT_PTR dwHash = 5381;
    
    dwHash = ((dwHash << 5) + dwHash) ^ ELEMENT_TYPE_FNPTR;
    dwHash = ((dwHash << 5) + dwHash) ^ callConv;
    dwHash = ((dwHash << 5) + dwHash) ^ numArgs;
    if (level < 1)
    {
        for (DWORD i = 0; i <= numArgs; i++)
        {
            dwHash = ((dwHash << 5) + dwHash) ^ HashTypeHandle(level+1, retAndArgTypes[i]);
        }
    }

    return dwHash;
}

// Calculate hash value for an array/pointer/byref type
static DWORD HashParamType(DWORD level, CorElementType kind, TypeHandle typeParam)
{
    WRAPPER_CONTRACT;
    INT_PTR dwHash = 5381;
    
    dwHash = ((dwHash << 5) + dwHash) ^ kind;
    dwHash = ((dwHash << 5) + dwHash) ^ HashTypeHandle(level, typeParam);

    return dwHash;
}

// Calculate hash value from type handle
static DWORD HashTypeHandle(DWORD level, TypeHandle t)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(t));
        PRECONDITION(!t.IsEncodedFixup());
    }
    CONTRACTL_END;

    DWORD retVal = 0;
    
    INTERIOR_STACK_PROBE_NOTHROW_CHECK_THREAD(goto Exit;);

    if (t.HasTypeParam())
    {
        retVal =  HashParamType(level, t.GetInternalCorElementType(), t.GetTypeParam());
    }
    else if (t.IsGenericVariable())
    {
        retVal = ((TypeVarTypeDesc*) t.AsTypeDesc())->GetToken();
    }
    else if (t.HasInstantiation())
    {
        retVal = HashPossiblyInstantiatedType(level, t.GetCl(), t.GetNumGenericArgs(), t.GetInstantiation());
    }
    else if (t.IsFnPtrType())
    {
        FnPtrTypeDesc* pTD = t.AsFnPtrType();
        retVal = HashFnPtrType(level, pTD->GetCallConv(), pTD->GetNumArgs(), pTD->GetRetAndArgTypesPointer());
    }
    else
        retVal = HashPossiblyInstantiatedType(level, t.GetCl(), 0, NULL);

#ifndef TOTALLY_DISBLE_STACK_GUARDS
Exit: 
    ;
#endif
    END_INTERIOR_STACK_PROBE;
    
    return retVal;
}


// Calculate hash value from key
static DWORD HashTypeKey(TypeKey* pKey)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pKey));
    }
    CONTRACTL_END;

    if (pKey->GetKind() == ELEMENT_TYPE_CLASS)
    {
        return HashPossiblyInstantiatedType(0, pKey->GetTypeToken(), pKey->GetNumGenericArgs(), pKey->GetInstantiation());
    }
    else if (pKey->GetKind() == ELEMENT_TYPE_FNPTR)
    {
        return HashFnPtrType(0, pKey->GetCallConv(), pKey->GetNumArgs(), pKey->GetRetAndArgTypes());
    }
    else
    {
        return HashParamType(0, pKey->GetKind(), pKey->GetElementType());
    }
}

// Look up a value in the hash table 
//
// The logic is subtle: type handles in the hash table may not be
// restored, but we need to compare components of the types (rank and
// element type for arrays, generic type and instantiation for
// instantiated types) against pKey
// 
// We avoid restoring types during search by cracking the signature
// encoding used by the zapper for out-of-module types e.g. in the
// instantiation of an instantiated type.
EETypeHashEntry_t *EETypeHashTable::FindItem(TypeKey* pKey)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(pKey));
        PRECONDITION(m_dwNumBuckets != 0);
    }
    CONTRACTL_END

    DWORD           dwHash = HashTypeKey(pKey);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;
    EETypeHashEntry_t * pSearch;
    CorElementType kind = pKey->GetKind();

    if (kind == ELEMENT_TYPE_CLASS) {
        TypeHandle *genericArgs = pKey->GetInstantiation();     
        DWORD numGenericArgs = pKey->GetNumGenericArgs();
        for (pSearch = m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext) 
        {
            if (pSearch->dwHashValue == dwHash && CompareInstantiatedType(pSearch->data, pKey->GetModule(), pKey->GetTypeToken(), numGenericArgs, genericArgs))
                return pSearch;
        }
    }
    else if (kind == ELEMENT_TYPE_FNPTR) 
    {
        BYTE callConv = pKey->GetCallConv();
        DWORD numArgs = pKey->GetNumArgs();
        TypeHandle *retAndArgTypes = pKey->GetRetAndArgTypes();
        for (pSearch = m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext)
        {
            if (pSearch->dwHashValue == dwHash && CompareFnPtrType(pSearch->data, callConv, numArgs, retAndArgTypes))
                return pSearch;
        }
    }
    else
    {
        for (pSearch = m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext)
        {
            if (pSearch->dwHashValue == dwHash && 
                //HACK: If we encounter an unrestored MethodTable, then it
                //isn't the type for which we are looking (plus, it will crash
                //in GetSignatureCorElementType).  However, TypeDescs can be
                //accessed when unrestored.  Also, they are accessed in that
                //manner at startup when we're loading the global types (i.e.
                //System.Object).
                (pSearch->data.IsTypeDesc() || pSearch->data.IsRestored()) &&
                pSearch->data.GetSignatureCorElementType() == kind &&
                pSearch->data.GetTypeParam() == pKey->GetElementType() &&
                pSearch->data.IsTypeDesc() == !pKey->IsTemplateMethodTable())
            {
                if (kind != ELEMENT_TYPE_ARRAY)
                    return pSearch;
                else if (pKey->IsTemplateMethodTable())
                {
                    if (pSearch->data.AsMethodTable()->GetRank() == pKey->GetRank())
                        return pSearch;
                }
                else
                {
                    ArrayTypeDesc *pATD = pSearch->data.AsArray();
                    
                    {
                        if (pATD->GetRank() == pKey->GetRank())
                            return pSearch;
                    }
                }
            }
        }
    }

    return NULL;
}

BOOL EETypeHashTable::CompareInstantiatedType(TypeHandle t, Module *pModule, mdTypeDef token, DWORD numGenericArgs, TypeHandle *genericArgs)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(t));
        PRECONDITION(CheckPointer(pModule));
        PRECONDITION(CheckPointer(genericArgs));
    }
    CONTRACTL_END

    if (!t.HasInstantiation())
        return FALSE;
    
    // Even the EEClass pointer might be encoded
    EEClass *pClass = t.GetClass();
    TypeHandle* genericArgs2 = t.GetInstantiation();
    {
        // First check that the typedef tokens match
        if (pClass->GetCl() != token)
            return FALSE;

        Module *pGenericModuleIfLoaded = pClass->GetModule();

        // Now check that the modules match 
        if (!pGenericModuleIfLoaded ||
            PTR_HOST_TO_TADDR(pGenericModuleIfLoaded) !=
            PTR_HOST_TO_TADDR(pModule))
            return FALSE;

    }

    // Now check the instantiations. Some type arguments might be encoded.
    _ASSERTE(genericArgs2 != NULL);
    for (DWORD i = 0; i < numGenericArgs; i++)
    {
        if (genericArgs[i] != genericArgs2[i]) 
        {
                return FALSE;
        }                    
    }
    
    return TRUE;
}

BOOL EETypeHashTable::CompareFnPtrType(TypeHandle t, BYTE callConv, DWORD numArgs, TypeHandle *retAndArgTypes)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(t));
        PRECONDITION(CheckPointer(retAndArgTypes));
    }
    CONTRACTL_END

    if (!t.IsFnPtrType())
        return FALSE;
    
#ifndef DACCESS_COMPILE

    FnPtrTypeDesc* pTD = t.AsFnPtrType();

    if (pTD->GetNumArgs() != numArgs || pTD->GetCallConv() != callConv)
        return FALSE;

    // Now check the return and argument types. Some type arguments might be encoded.
    TypeHandle *retAndArgTypes2 = pTD->GetRetAndArgTypesPointer();
    for (DWORD i = 0; i <= numArgs; i++)
    {
        if (retAndArgTypes[i] != retAndArgTypes2[i]) 
        {
                return FALSE;
        }
    }
    
    return TRUE;

#else
    DacNotImpl();
    return FALSE;
#endif // #ifndef DACCESS_COMPILE
}

TypeHandle EETypeHashTable::GetValue(TypeKey *pKey)
{
    WRAPPER_CONTRACT;

    EETypeHashEntry_t *pItem = FindItem(pKey);

    if (pItem)
    {
        g_IBCLogger.LogTypeHashTableAccess(&pItem->data);
        return pItem->data;
    }
    else
        return TypeHandle();
}

#ifndef DACCESS_COMPILE

BOOL EETypeHashTable::ContainsValue(TypeHandle th)
{
    WRAPPER_CONTRACT;

    TypeKey typeKey = th.GetTypeKey();
    return !GetValue(&typeKey).IsNull();
}

// Insert a value not already in the hash table
VOID EETypeHashTable::InsertValue(TypeHandle data)
{
    CONTRACTL
    {
        INSTANCE_CHECK;
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(m_dwSealCount == 0);
        PRECONDITION(CheckPointer(data));
        PRECONDITION(!data.IsEncodedFixup());
        PRECONDITION(!data.IsGenericTypeDefinition()); // Generic type defs live in typedef table (availableClasses)
        PRECONDITION(data.HasInstantiation() || data.HasTypeParam() || data.IsFnPtrType()); // It's an instantiated type or an array/ptr/byref type
        PRECONDITION(m_dwNumBuckets != 0);
        PRECONDITION(!m_pModule || m_pModule->IsTenured()); // Destruct won't destruct m_pAvailableParamTypes for non-tenured modules - so make sure no one tries to insert one before the Module has been tenured
    }
    CONTRACTL_END

    // log inserts because they are generally preceded by unsuccessful lookups that we have trouble logging
    g_IBCLogger.LogTypeHashTableWriteAccess(&data);

    DWORD           dwHash = HashTypeHandle(0, data);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;
    EETypeHashEntry_t * pNewEntry;

    pNewEntry = (EETypeHashEntry_t *)(void*) GetDomain()->GetLowFrequencyHeap()->AllocMem(sizeof(EETypeHashEntry));
#ifdef _DEBUG
    m_dwDebugMemory += sizeof(EETypeHashEntry);
#endif

    pNewEntry->data         = data;
    pNewEntry->dwHashValue  = dwHash;
    pNewEntry->pNext     = m_pBuckets[dwBucket];

    // Make sure that all writes are visible before publishing the entry
    MemoryBarrier();

    // Final atomic action; we allow multiple readers concurrent with a single writer
    m_pBuckets[dwBucket] = pNewEntry;

    m_dwNumEntries++;
    if  (m_dwNumEntries > m_dwNumBuckets*2)
    {
        FAULT_NOT_FATAL();  // Return value from GrowHashTable intentionally ignored: OOM failure here affects perf, not correctnesss.
        GrowHashTable();
    }

    return;
}

//
// This function gets called whenever the class hash table seems way too small.
// Its task is to allocate a new bucket table that is a lot bigger, and transfer
// all the entries to it.
// 
BOOL EETypeHashTable::GrowHashTable()
{

    CONTRACTL
    {
        INSTANCE_CHECK;
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END



    // Make the new bucket table 2 times bigger plus a little bit so the number has some lower bits set (for the benefit of the %
    // operation later used to index)
    DWORD dwNewNumBuckets = m_dwNumBuckets * 2 + 11;
    EETypeHashEntry_t **pNewBuckets = (EETypeHashEntry_t **)(void*)GetDomain()->GetLowFrequencyHeap()->AllocMem_NoThrow(dwNewNumBuckets*sizeof(pNewBuckets[0]));
    if (!pNewBuckets)
    {
        return FALSE;
    }

    // Don't need to memset() since this was ClrVirtualAlloc()'d memory
    // memset(pNewBuckets, 0, dwNewNumBuckets*sizeof(pNewBuckets[0]));

    // Run through the old table and transfer all the entries

    // Be sure not to mess with the integrity of the old table while
    // we are doing this, as there can be concurrent readers!  Note that
    // it is OK if the concurrent reader misses out on a match, though -
    // they will have to acquire the lock on a miss & try again.

    for (DWORD i = 0; i < m_dwNumBuckets; i++)
    {
        EETypeHashEntry_t * pEntry = m_pBuckets[i];

        // Try to lock out readers from scanning this bucket.  This is
        // obviously a race which may fail. However, note that it's OK
        // if somebody is already in the list - it's OK if we mess
        // with the bucket groups, as long as we don't destroy
        // anything.  The lookup function will still do appropriate
        // comparison even if it wanders aimlessly amongst entries
        // while we are rearranging things.  If a lookup finds a match
        // under those circumstances, great.  If not, they will have
        // to acquire the lock & try again anyway.

        m_pBuckets[i] = NULL;
        while (pEntry != NULL)
        {
            DWORD dwNewBucket = pEntry->dwHashValue % dwNewNumBuckets;
            EETypeHashEntry_t * pNextEntry  = pEntry->pNext;

            pEntry->pNext = pNewBuckets[dwNewBucket];
            pNewBuckets[dwNewBucket] = pEntry;

            pEntry = pNextEntry;
        }
    }

    // Make sure that all writes are visible before publishing the new array
    MemoryBarrier();
    m_pBuckets = pNewBuckets;

    // The new number of buckets has to be published last
    MemoryBarrier();
    m_dwNumBuckets = dwNewNumBuckets;

    return TRUE;
}


#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
EETypeHashTable::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();
    
    DacEnumMemoryRegion(PTR_TO_TADDR(m_pBuckets),
                        m_dwNumBuckets * sizeof(PTR_EEClassHashEntry));
    if (m_pBuckets.IsValid())
    {
        for (DWORD i = 0; i < m_dwNumBuckets; i++)
        {
            PTR_EETypeHashEntry entry = m_pBuckets[i];
            while (entry.IsValid())
            {
                entry.EnumMem();
                entry->data.EnumMemoryRegions(flags);
                entry = entry->pNext;
            }
        }
    }

    if (m_pModule && m_pModule.IsValid())
    {
        m_pModule->EnumMemoryRegions(flags, true);
    }
}

#endif // #ifdef DACCESS_COMPILE
