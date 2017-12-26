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
// File: pendingload.cpp
//
#include "common.h"
#include "excep.h"
#include "pendingload.h"
#include "memoryreport.h"

#ifndef DACCESS_COMPILE


// ============================================================================
// Pending type load hash table methods
// ============================================================================
/*static */ PendingTypeLoadTable* PendingTypeLoadTable::Create(LoaderHeap *pHeap, 
                                                               DWORD dwNumBuckets, 
                                                               AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(CheckPointer(pHeap));
    }
    CONTRACTL_END;

    size_t                  size = sizeof(PendingTypeLoadTable);
    BYTE *                  pMem;
    PendingTypeLoadTable * pThis;

    _ASSERT( dwNumBuckets >= 0 );
    S_SIZE_T allocSize = S_SIZE_T( dwNumBuckets ) 
                                        * S_SIZE_T( sizeof(PendingTypeLoadTable::TableEntry*) )
                                        + S_SIZE_T( size );
    if( allocSize.IsOverflow() )
    {
        ThrowHR(E_INVALIDARG);
    }
    pMem = (BYTE *) pamTracker->Track(pHeap->AllocMem( allocSize.Value() ));

    pThis = (PendingTypeLoadTable *) pMem;

#ifdef _DEBUG
    pThis->m_dwDebugMemory = (DWORD)(size + dwNumBuckets*sizeof(PendingTypeLoadTable::TableEntry*));
#endif

    pThis->m_dwNumBuckets = dwNumBuckets;
    pThis->m_pBuckets = (PendingTypeLoadTable::TableEntry**) (pMem + size);

    // Don't need to memset() since this was ClrVirtualAlloc()'d memory
    // memset(pThis->m_pBuckets, 0, dwNumBuckets*sizeof(PendingTypeLoadTable::TableEntry*));

    return pThis;
}



PendingTypeLoadTable::TableEntry *PendingTypeLoadTable::AllocNewEntry()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT( return NULL; );
    }
    CONTRACTL_END

#ifdef _DEBUG
    m_dwDebugMemory += (DWORD) (sizeof(PendingTypeLoadTable::TableEntry));
#endif

    return (PendingTypeLoadTable::TableEntry *) new (nothrow) BYTE[sizeof(PendingTypeLoadTable::TableEntry)];
}


//
// Does not handle duplicates!
//
BOOL PendingTypeLoadTable::InsertValue(PendingTypeLoadEntry *pData)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT( return FALSE; );
        PRECONDITION(CheckPointer(pData));
        PRECONDITION(FindItem(&pData->GetTypeKey()) == NULL);
    }
    CONTRACTL_END

    _ASSERTE(m_dwNumBuckets != 0);

    DWORD           dwHash = pData->GetTypeKey().ComputeHash();
    DWORD           dwBucket = dwHash % m_dwNumBuckets;
    PendingTypeLoadTable::TableEntry * pNewEntry = AllocNewEntry();
    if (pNewEntry == NULL)
        return FALSE;

    // Insert at head of bucket
    pNewEntry->pNext        = m_pBuckets[dwBucket];
    pNewEntry->pData        = pData;
    pNewEntry->dwHashValue  = dwHash;

    m_pBuckets[dwBucket] = pNewEntry;

    return TRUE;
}


BOOL PendingTypeLoadTable::DeleteValue(TypeKey *pKey)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pKey));
    }
    CONTRACTL_END

    _ASSERTE(m_dwNumBuckets != 0);

    DWORD           dwHash = pKey->ComputeHash();
    DWORD           dwBucket = dwHash % m_dwNumBuckets;
    PendingTypeLoadTable::TableEntry * pSearch;
    PendingTypeLoadTable::TableEntry **ppPrev = &m_pBuckets[dwBucket];

    for (pSearch = m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext)
    {
        TypeKey entryTypeKey = pSearch->pData->GetTypeKey();
        if (pSearch->dwHashValue == dwHash && TypeKey::Equals(pKey, &entryTypeKey))
        {
            *ppPrev = pSearch->pNext;
            delete(pSearch);
            return TRUE;
        }

        ppPrev = &pSearch->pNext;
    }

    return FALSE;
}


PendingTypeLoadTable::TableEntry *PendingTypeLoadTable::FindItem(TypeKey *pKey)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pKey));
    }
    CONTRACTL_END

    _ASSERTE(m_dwNumBuckets != 0);


    DWORD           dwHash = pKey->ComputeHash();
    DWORD           dwBucket = dwHash % m_dwNumBuckets;
    PendingTypeLoadTable::TableEntry * pSearch;

    for (pSearch = m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext)
    {
        TypeKey entryTypeKey = pSearch->pData->GetTypeKey();
        if (pSearch->dwHashValue == dwHash && TypeKey::Equals(pKey, &entryTypeKey))
        {
            return pSearch;
        }
    }

    return NULL;
}


#ifdef _DEBUG
void PendingTypeLoadTable::Dump()
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END

    LOG((LF_CLASSLOADER, LL_INFO10000, "PHASEDLOAD: table contains:\n"));
    for (DWORD i = 0; i < m_dwNumBuckets; i++)
    {
        for (TableEntry *pSearch = m_pBuckets[i]; pSearch; pSearch = pSearch->pNext)
        {
            SString name;
            TypeKey entryTypeKey = pSearch->pData->GetTypeKey();
            TypeString::AppendTypeKeyDebug(name, &entryTypeKey);
            LOG((LF_CLASSLOADER, LL_INFO10000, "  Entry %S with handle %p at level %s\n", name.GetUnicode(), pSearch->pData->m_typeHandle.AsPtr(),
                 pSearch->pData->m_typeHandle.IsNull() ? "not-applicable" : classLoadLevelName[pSearch->pData->m_typeHandle.GetLoadLevel()]));            
        }
    }
}
#endif

PendingTypeLoadEntry* PendingTypeLoadTable::GetValue(TypeKey *pKey)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pKey));
    }
    CONTRACTL_END

    PendingTypeLoadTable::TableEntry *pItem = FindItem(pKey);

    if (pItem != NULL)
    {
        return pItem->pData;
    }
    else
    {
        return NULL;
    }
}

#endif // #ifndef DACCESS_COMPILE

