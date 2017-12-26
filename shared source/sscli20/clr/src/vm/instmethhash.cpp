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
// File: InstMethHash.cpp
//
#include "common.h"
#include "excep.h"
#include "instmethhash.h"
#include "eeconfig.h"
#include "generics.h"
#include "typestring.h"

#ifndef DACCESS_COMPILE

// ============================================================================
// Class hash table methods
// ============================================================================
/* static */ InstMethodHashTable *InstMethodHashTable::Create(BaseDomain *pDomain, Module *pModule, DWORD dwNumBuckets, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    size_t size = sizeof(InstMethodHashTable);

    _ASSERTE( dwNumBuckets >= 0 );
    S_SIZE_T allocSize = S_SIZE_T( dwNumBuckets ) 
                            * S_SIZE_T( sizeof(InstMethodHashEntry_t*) )
                            + S_SIZE_T( size );
    if( allocSize.IsOverflow() )
    {
        ThrowHR(E_INVALIDARG);
    }

    InstMethodHashTable *pThis;

    BYTE* pMem = (BYTE *)pamTracker->Track(pDomain->GetLowFrequencyHeap()->AllocMem( allocSize.Value() ));
    pThis = (InstMethodHashTable *) pMem;

#ifdef _DEBUG
    pThis->m_dwDebugMemory = (DWORD)allocSize.Value();
    pThis->m_dwSealCount = 0;
#endif

    pThis->m_dwNumBuckets = dwNumBuckets;
    pThis->m_dwNumEntries = 0;
    pThis->m_pBuckets = (InstMethodHashEntry_t**) (pMem + size);
    pThis->m_pModule = pModule;
    pThis->m_pDomain = pDomain;

    return pThis;
}

BaseDomain *InstMethodHashTable::GetDomain()
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

// Calculate a hash value for a method-desc key
static DWORD Hash(TypeHandle declaringType, mdMethodDef token, DWORD numGenericArgs, TypeHandle *genericArgs)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    INT_PTR dwHash = 5381;
    
    dwHash = ((dwHash << 5) + dwHash) ^ (INT_PTR) declaringType.GetCl();
    dwHash = ((dwHash << 5) + dwHash) ^ (INT_PTR) token;
    
    for (DWORD i = 0; i < numGenericArgs; i++)
    {
        dwHash = ((dwHash << 5) + dwHash) ^ (INT_PTR) (genericArgs[i].GetSize());
    }

    return  (DWORD)dwHash;
}

MethodDesc* InstMethodHashTable::FindMethodDesc(TypeHandle declaringType, 
                                                mdMethodDef token, 
                                                BOOL unboxingStub, 
                                                DWORD numGenericArgs, 
                                                TypeHandle* genericArgs, 
                                                BOOL getSharedNotStub)
{ 
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(declaringType));
        PRECONDITION(m_dwNumBuckets != 0);
    }
    CONTRACTL_END

    DWORD dwHash = Hash(declaringType, token, numGenericArgs, genericArgs);
    DWORD dwBucket = dwHash % m_dwNumBuckets;
    InstMethodHashEntry_t* pSearch = m_pBuckets[dwBucket];
    TypeHandle *inst = NULL;

    while (pSearch)
    {
        if (pSearch->dwHashValue == dwHash)
        {
            MethodDesc *pMD = pSearch->data;
            if (pMD->GetMemberDef() == token && 
                pMD->GetNumGenericMethodArgs() == numGenericArgs &&
                (((pSearch->wKeyFlags & InstMethodHashEntry::RequiresInstArg) == 0) == (getSharedNotStub == 0)) &&
                (((pSearch->wKeyFlags & InstMethodHashEntry::UnboxingStub) == 0) == (unboxingStub == 0)))
            {
                // Note pMD->GetMethodTable() might not be restored at this point
                if (pMD->GetMethodTable() != declaringType.GetMethodTable())
                {
                    if (m_pModule == NULL)
                        goto NotEq;

                        goto NotEq;
                }
                
                inst = pMD->GetMethodInstantiation();
                for (DWORD i = 0; i < numGenericArgs; i++)
                {
                    if (inst[i] != genericArgs[i]) 
                    {
                        if (m_pModule == NULL)
                            goto NotEq;
                            goto NotEq;
                    }               
                }     
                return pMD;
            NotEq:;
            }
                
        }
        pSearch = pSearch->pNext;
    }

    return NULL;
}

BOOL InstMethodHashTable::ContainsMethodDesc(MethodDesc* pMD)
{
    WRAPPER_CONTRACT;

    return FindMethodDesc(
        pMD->GetMethodTable(), pMD->GetMemberDef(), pMD->IsUnboxingStub(),
        pMD->GetNumGenericMethodArgs(), pMD->GetMethodInstantiation(), pMD->RequiresInstArg()) != NULL;
}

#endif // #ifndef DACCESS_COMPILE

void InstMethodHashTable::Iterator::Reset()
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

void InstMethodHashTable::Iterator::Init()
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    if (m_pTable)
        m_pTable->Seal(); // The table cannot be changing while it is being iterated
#endif

    m_pCurBucket = NULL;
}

InstMethodHashTable::Iterator::Iterator()
{
    WRAPPER_CONTRACT; 
    m_pTable = NULL;
    Init(); 
}

InstMethodHashTable::Iterator::Iterator(InstMethodHashTable * pTable)
{
    WRAPPER_CONTRACT;
    m_pTable = pTable;
    Init();
}

InstMethodHashTable::Iterator::~Iterator()
{
    WRAPPER_CONTRACT;

#ifdef _DEBUG
    if (m_pTable)
        m_pTable->Unseal(); 
#endif
}


BOOL InstMethodHashTable::FindNext(Iterator *it, InstMethodHashEntry **ppEntry)
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

#ifndef DACCESS_COMPILE

// Add method desc to the hash table; must not be present already
void InstMethodHashTable::InsertMethodDesc(MethodDesc *pMD)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(m_dwSealCount == 0);
        PRECONDITION(CheckPointer(pMD));
        
        // Generic method definitions (e.g. D.m<U> or C<int>.m<U>) belong in method tables, not here
        PRECONDITION(!pMD->IsGenericMethodDefinition());
    }
    CONTRACTL_END
 
    _ASSERTE(m_dwNumBuckets != 0);

    DWORD           dwHash = Hash(pMD->GetMethodTable(), pMD->GetMemberDef(), pMD->GetNumGenericMethodArgs(), pMD->GetMethodInstantiation());
    DWORD           dwBucket = dwHash % m_dwNumBuckets;

    InstMethodHashEntry_t * pNewEntry = (InstMethodHashEntry_t *)(void*) GetDomain()->GetLowFrequencyHeap()->AllocMem(sizeof(InstMethodHashEntry));
#ifdef _DEBUG
    m_dwDebugMemory += sizeof(InstMethodHashEntry);
#endif

    pNewEntry->pNext        = m_pBuckets[dwBucket];
    pNewEntry->data         = pMD;
    pNewEntry->dwHashValue  = dwHash;
    pNewEntry->wKeyFlags = 0;
    if (pMD->RequiresInstArg())
        pNewEntry->wKeyFlags |= InstMethodHashEntry::RequiresInstArg;
    if (pMD->IsUnboxingStub())
        pNewEntry->wKeyFlags |= InstMethodHashEntry::UnboxingStub;

    // Make sure that all writes are visible before publishing the entry
    MemoryBarrier();
    
    // Final atomic action; we allow multiple readers concurrent with a single writer
    m_pBuckets[dwBucket] = pNewEntry;

    // Now safe for readers, but GrowHashTable must be safe also
    m_dwNumEntries++;
    if  (m_dwNumEntries > m_dwNumBuckets*2)
    {
        GrowHashTable();
    }
}


//
// This function gets called whenever the class hash table seems way too small.
// Its task is to allocate a new bucket table that is a lot bigger, and transfer
// all the entries to it.
// 
void InstMethodHashTable::GrowHashTable()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END


    // Make the new bucket table 4 times bigger
    DWORD dwNewNumBuckets = 0;
    DWORD dwAllocSize = 0;
    
    if (!ClrSafeInt<DWORD>::multiply(m_dwNumBuckets, 4, dwNewNumBuckets) ||
        !ClrSafeInt<DWORD>::multiply(dwNewNumBuckets, sizeof(InstMethodHashEntry_t *), dwAllocSize))
        ThrowHR(COR_E_OVERFLOW);

    InstMethodHashEntry_t **pNewBuckets = (InstMethodHashEntry_t **)(void*)GetDomain()->GetLowFrequencyHeap()->AllocMem(dwAllocSize);

    // Don't need to memset() since this was ClrVirtualAlloc()'d memory
    // memset(pNewBuckets, 0, dwNewNumBuckets*sizeof(pNewBuckets[0]));

    // Run through the old table and transfer all the entries

    // Be sure not to mess with the integrity of the old table while
    // we are doing this, as there can be concurrent readers!  Note that
    // it is OK if the concurrent reader misses out on a match, though -
    // they will have to acquire the lock on a miss & try again.

    for (DWORD i = 0; i < m_dwNumBuckets; i++)
    {
        InstMethodHashEntry_t* pEntry = m_pBuckets[i];

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
            InstMethodHashEntry_t *pNextEntry  = pEntry->pNext;

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
}


#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
InstMethodHashTable::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();
    
    DacEnumMemoryRegion(PTR_TO_TADDR(m_pBuckets),
                        m_dwNumBuckets * sizeof(PTR_InstMethodHashEntry));
    if (m_pBuckets.IsValid())
    {
        for (DWORD i = 0; i < m_dwNumBuckets; i++)
        {
            PTR_InstMethodHashEntry entry = m_pBuckets[i];
            while (entry.IsValid())
            {
                entry.EnumMem();
                if (entry->data.IsValid())
                {
                    entry->data->EnumMemoryRegions(flags);
                }
                entry = entry->pNext;
            }
        }
    }

    if (m_pModule.IsValid())
    {
        m_pModule->EnumMemoryRegions(flags, true);
    }
}

#endif // #ifdef DACCESS_COMPILE
