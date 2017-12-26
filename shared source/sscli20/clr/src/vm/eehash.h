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
//emp
// File: eehash.h
//
// Provides hash table functionality needed in the EE - intended to be replaced later with better
// algorithms, but which have the same interface.
//
// Two requirements are:
//
// 1. Any number of threads can be reading the hash table while another thread is writing, without error.
// 2. Only one thread can write at a time.
// 3. When calling ReplaceValue(), a reader will get the old value, or the new value, but not something
//    in between.
// 4. DeleteValue() is an unsafe operation - no other threads can be in the hash table when this happens.
//
#ifndef _EE_HASH_H
#define _EE_HASH_H

#include "exceptmacros.h"
#include "syncclean.hpp"
#include "memoryreport.h"

#include "util.hpp"




class AllocMemTracker;
class ClassLoader;
struct LockOwner;
class NameHandle;
struct PsetCacheEntry;
class SigTypeContext;


// The "blob" you get to store in the hash table

typedef void* HashDatum;

// The heap that you want the allocation to be done in

typedef void* AllocationHeap;


// One of these is present for each element in the table.
// Update the SIZEOF_EEHASH_ENTRY macro below if you change this
// struct

typedef struct EEHashEntry EEHashEntry_t;
typedef DPTR(EEHashEntry_t) PTR_EEHashEntry_t;
struct EEHashEntry
{
    PTR_EEHashEntry_t   pNext;
    DWORD               dwHashValue;
    HashDatum           Data;
    BYTE                Key[1]; // The key is stored inline
};

// The key[1] is a place holder for the key
// SIZEOF_EEHASH_ENTRY is the size of struct up to (and not including) the key
#define SIZEOF_EEHASH_ENTRY (offsetof(EEHashEntry,Key[0]))


// Struct to hold a client's iteration state
struct EEHashTableIteration;

class GCHeap;

// Generic hash table.

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
class EEHashTableBase
{
public:


    BOOL            Init(DWORD dwNumBuckets, LockOwner *pLock, AllocationHeap pHeap = 0,BOOL CheckThreadSafety = TRUE);

    void            InsertValue(KeyType pKey, HashDatum Data, BOOL bDeepCopyKey = bDefaultCopyIsDeep);
    void            InsertKeyAsValue(KeyType pKey, BOOL bDeepCopyKey = bDefaultCopyIsDeep); 
    BOOL            DeleteValue(KeyType pKey);
    BOOL            ReplaceValue(KeyType pKey, HashDatum Data);
    BOOL            ReplaceKey(KeyType pOldKey, KeyType pNewKey);
    void            ClearHashTable();
    void            EmptyHashTable();
    BOOL            IsEmpty();
    void            Destroy();

    // Reader functions. Please place any functions that can be called from the 
    // reader threads here.
    BOOL            GetValue(KeyType pKey, HashDatum *pData);
    BOOL            GetValue(KeyType pKey, HashDatum *pData, DWORD hashValue);

    
    // A fast inlinable flavor of GetValue that can return false instead of the actual item
    // if there is race with updating of the hashtable. Callers of GetValueSpeculative
    // should fall back to the slow GetValue if GetValueSpeculative returns false.
    // Assumes that we are in cooperative mode already. For performance-sensitive codepaths.
    BOOL            GetValueSpeculative(KeyType pKey, HashDatum *pData);

    DWORD           GetHash(KeyType Key);
    DWORD           GetCount();
    
    // Walk through all the entries in the hash table, in meaningless order, without any
    // synchronization.
    //
    //           IterateStart()
    //           while (IterateNext())
    //              IterateGetKey();
    //
    // This is guaranteed to be DeleteValue-friendly if you advance the iterator before
    // deletig, i.e. if used in the following pattern:
    //
    // IterateStart();
    // BOOL keepGoing = IterateNext();
    // while(keepGoing)
    // {
    //      key = IterateGetKey();
    //      keepGoing = IterateNext();
    //     ...
    //         DeleteValue(key);
    //       ..
    //  }
    void            IterateStart(EEHashTableIteration *pIter);
    BOOL            IterateNext(EEHashTableIteration *pIter);
    KeyType         IterateGetKey(EEHashTableIteration *pIter);
    HashDatum       IterateGetValue(EEHashTableIteration *pIter);
#ifdef _DEBUG
    void  SuppressSyncCheck()
    {
        LEAF_CONTRACT;
        m_CheckThreadSafety=FALSE;
    }
#endif
protected:
    BOOL            GrowHashTable();
    EEHashEntry_t * FindItem(KeyType pKey);
    EEHashEntry_t * FindItem(KeyType pKey, DWORD hashValue);

    // A fast inlinable flavor of FindItem that can return null instead of the actual item
    // if there is race with updating of the hashtable. Callers of FindItemSpeculative
    // should fall back to the slow FindItem if FindItemSpeculative returns null.
    // Assumes that we are in cooperative mode already. For performance-sensitive codepaths.
    EEHashEntry_t * FindItemSpeculative(KeyType pKey, DWORD hashValue);

    // Double buffer to fix the race condition of growhashtable (the update
    // of m_pBuckets and m_dwNumBuckets has to be atomic, so we double buffer
    // the structure and access it through a pointer, which can be updated
    // atomically. The union is in order to not change the SOS macros.
    
    struct BucketTable
    {
        DPTR(PTR_EEHashEntry_t) m_pBuckets;    // Pointer to first entry for each bucket  
        DWORD            m_dwNumBuckets;
    } m_BucketTable[2];
    typedef DPTR(BucketTable) PTR_BucketTable;

    // In a function we MUST only read this value ONCE, as the writer thread can change
    // the value asynchronously. We make this member volatile the compiler won't do copy propagation 
    // optimizations that can make this read happen more than once. Note that we  only need 
    // this property for the readers. As they are the ones that can have
    // this variable changed (note also that if the variable was enregistered we wouldn't
    // have any problem)
    // BE VERY CAREFUL WITH WHAT YOU DO WITH THIS VARIABLE AS USING IT BADLY CAN CAUSE 
    // RACING CONDITIONS
    PTR_BucketTable 
#ifndef DACCESS_COMPILE        
    volatile   
#endif
    m_pVolatileBucketTable;

    
    DWORD                   m_dwNumEntries;
    AllocationHeap          m_Heap;
    volatile LONG         m_bGrowing;     
#ifdef _DEBUG
    LPVOID          m_lockData;
    FnLockOwner     m_pfnLockOwner;

    EEThreadId      m_writerThreadId;
    BOOL            m_CheckThreadSafety;

#endif

#ifdef _DEBUG_IMPL
    // A thread must own a lock for a hash if it is a writer.
    BOOL OwnLock()
    {
        WRAPPER_CONTRACT;
        if (m_CheckThreadSafety == FALSE)
            return TRUE;

        if (m_pfnLockOwner == NULL) {
            return m_writerThreadId.IsSameThread();
        }
        else {
            BOOL ret = m_pfnLockOwner(m_lockData);
            if (!ret) {
                if (GCHeap::IsGCInProgress()  && 
                    (dbgOnly_IsSpecialEEThread() || GetThread() == GCHeap::GetGCHeap()->GetGCThread())) {
                    ret = TRUE;
                }
            }
            return ret;
        }
    }
#endif  // _DEBUG_IMPL
};

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
class EEHashTable : public EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>
{
public:
    EEHashTable()
    {
        LEAF_CONTRACT;
        m_BucketTable[0].m_pBuckets     = NULL;
        m_BucketTable[0].m_dwNumBuckets = 0;
        m_BucketTable[1].m_pBuckets     = NULL;
        m_BucketTable[1].m_dwNumBuckets = 0;
#ifndef DACCESS_COMPILE    
        m_pVolatileBucketTable = NULL;
#endif
        m_dwNumEntries = 0;
        m_bGrowing = 0;    
#ifdef _DEBUG
        m_lockData = NULL;
        m_pfnLockOwner = NULL;
#endif
    }

    ~EEHashTable()
    {
        WRAPPER_CONTRACT;
        Destroy();
    }
};

/* to be used as static variable - no constructor/destructor, assumes zero 
   initialized memory */
template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
class EEHashTableStatic : public EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>
{
};

#ifndef DACCESS_COMPILE
template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
void EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::Destroy()
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    if (m_pVolatileBucketTable && m_pVolatileBucketTable->m_pBuckets != NULL)
    {
        DWORD i;

        for (i = 0; i < m_pVolatileBucketTable->m_dwNumBuckets; i++)
        {
            EEHashEntry_t *pEntry, *pNext;

            for (pEntry = m_pVolatileBucketTable->m_pBuckets[i]; pEntry != NULL; pEntry = pNext)
            {
                pNext = pEntry->pNext;
                Helper::DeleteEntry(pEntry, m_Heap);
            }
        }

        delete[] (m_pVolatileBucketTable->m_pBuckets-1);

		m_pVolatileBucketTable = NULL;
    }
	
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
void EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::ClearHashTable()
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    
    GCX_COOP_NO_THREAD_BROKEN();

    if (m_pVolatileBucketTable->m_pBuckets != NULL)
    {
        DWORD i;

        for (i = 0; i < m_pVolatileBucketTable->m_dwNumBuckets; i++)
        {
            EEHashEntry_t *pEntry, *pNext;

            for (pEntry = m_pVolatileBucketTable->m_pBuckets[i]; pEntry != NULL; pEntry = pNext)
            {
                pNext = pEntry->pNext;
                Helper::DeleteEntry(pEntry, m_Heap);
            }
        }

        delete[] (m_pVolatileBucketTable->m_pBuckets-1);
        m_pVolatileBucketTable->m_pBuckets = NULL;
    }
   
    m_pVolatileBucketTable->m_dwNumBuckets = 0;
    m_dwNumEntries = 0;
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
void EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::EmptyHashTable()
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE (OwnLock());
    
    GCX_COOP_NO_THREAD_BROKEN();
    
    if (m_pVolatileBucketTable->m_pBuckets != NULL)
    {
        DWORD i;

        for (i = 0; i < m_pVolatileBucketTable->m_dwNumBuckets; i++)
        {
            EEHashEntry_t *pEntry, *pNext;

            for (pEntry = m_pVolatileBucketTable->m_pBuckets[i]; pEntry != NULL; pEntry = pNext)
            {
                pNext = pEntry->pNext;
                Helper::DeleteEntry(pEntry, m_Heap);
            }

            m_pVolatileBucketTable->m_pBuckets[i] = NULL;
        }
    }

    m_dwNumEntries = 0;
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>

BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::Init(DWORD dwNumBuckets, LockOwner *pLock, AllocationHeap pHeap, BOOL CheckThreadSafety)
{
    CONTRACTL
    {
        WRAPPER(NOTHROW);
        WRAPPER(GC_NOTRIGGER);
        INJECT_FAULT(return FALSE;);

#ifndef DACCESS_COMPILE  
        PRECONDITION(m_pVolatileBucketTable == NULL && "EEHashTable::Init() called twice.");
#endif

    }
    CONTRACTL_END

    m_pVolatileBucketTable = &m_BucketTable[0];

    m_pVolatileBucketTable->m_pBuckets = new (nothrow) EEHashEntry_t*[dwNumBuckets+1];
    if (m_pVolatileBucketTable->m_pBuckets == NULL)
        return FALSE;
    
    memset(m_pVolatileBucketTable->m_pBuckets, 0, (dwNumBuckets+1)*sizeof(EEHashEntry_t*));
    // The first slot links to the next list.
    m_pVolatileBucketTable->m_pBuckets ++;

    m_pVolatileBucketTable->m_dwNumBuckets = dwNumBuckets;

    m_Heap = pHeap;

#ifdef _DEBUG
    if (pLock == NULL) {
        m_lockData = NULL;
        m_pfnLockOwner = NULL;
    }
    else {
        m_lockData = pLock->lock;
        m_pfnLockOwner = pLock->lockOwnerFunc;
    }

    if (m_pfnLockOwner == NULL) {
        m_writerThreadId.SetThreadId();
    }
    m_CheckThreadSafety = CheckThreadSafety;
#endif
    
    return TRUE;
}


// Does not handle duplicates!

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
void EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::InsertValue(KeyType pKey, HashDatum Data, BOOL bDeepCopyKey)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    _ASSERTE (OwnLock());
    
    GCX_COOP_NO_THREAD_BROKEN();

    _ASSERTE(m_pVolatileBucketTable->m_dwNumBuckets != 0);

    if  (m_dwNumEntries > m_pVolatileBucketTable->m_dwNumBuckets*2)
    {
        if (!GrowHashTable()) COMPlusThrowOM();
    }

    DWORD dwHash = (DWORD)Helper::Hash(pKey);
    DWORD dwBucket = dwHash % m_pVolatileBucketTable->m_dwNumBuckets;
    EEHashEntry_t * pNewEntry;

    pNewEntry = Helper::AllocateEntry(pKey, bDeepCopyKey, m_Heap);
    if (!pNewEntry)
    {
        COMPlusThrowOM();
    }

    // Fill in the information for the new entry.
    pNewEntry->pNext        = m_pVolatileBucketTable->m_pBuckets[dwBucket];
    pNewEntry->Data         = Data;
    pNewEntry->dwHashValue  = dwHash;

    // Insert at head of bucket
    // need volatile write to avoid write reordering problem in IA
    *(EEHashEntry_t* volatile *)(&m_pVolatileBucketTable->m_pBuckets[dwBucket]) = pNewEntry;

    m_dwNumEntries++;
}


// Similar to the above, except that the HashDatum is a pointer to key.
template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
void EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::InsertKeyAsValue(KeyType pKey, BOOL bDeepCopyKey)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        INJECT_FAULT(COMPlusThrowOM(););
    }
    CONTRACTL_END

    _ASSERTE (OwnLock());
    
    GCX_COOP_NO_THREAD_BROKEN();

    _ASSERTE(m_pVolatileBucketTable->m_dwNumBuckets != 0);

    if  (m_dwNumEntries > m_pVolatileBucketTable->m_dwNumBuckets*2)
    {
        if (!GrowHashTable()) COMPlusThrowOM();
    }

    DWORD           dwHash = Helper::Hash(pKey);
    DWORD           dwBucket = dwHash % m_pVolatileBucketTable->m_dwNumBuckets;
    EEHashEntry_t * pNewEntry;

    pNewEntry = Helper::AllocateEntry(pKey, bDeepCopyKey, m_Heap);
    if (!pNewEntry)
    {         
        COMPlusThrowOM();
    }

    // Fill in the information for the new entry.
    pNewEntry->pNext        = m_pVolatileBucketTable->m_pBuckets[dwBucket];
    pNewEntry->dwHashValue  = dwHash;
    pNewEntry->Data         = *((LPUTF8 *)pNewEntry->Key);

    // Insert at head of bucket
    // need volatile write to avoid write reordering problem in IA
    *(EEHashEntry_t* volatile *)(&m_pVolatileBucketTable->m_pBuckets[dwBucket]) = pNewEntry;

    m_dwNumEntries++;
}


template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::DeleteValue(KeyType pKey)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE (OwnLock());

    Thread *pThread = GetThread();
    GCX_MAYBE_COOP_NO_THREAD_BROKEN(pThread ? !(pThread->m_StateNC & Thread::TSNC_UnsafeSkipEnterCooperative) : FALSE);

    _ASSERTE(m_pVolatileBucketTable->m_dwNumBuckets != 0);

    DWORD           dwHash = Helper::Hash(pKey);
    DWORD           dwBucket = dwHash % m_pVolatileBucketTable->m_dwNumBuckets;
    EEHashEntry_t * pSearch;
    EEHashEntry_t **ppPrev = &m_pVolatileBucketTable->m_pBuckets[dwBucket];

    for (pSearch = m_pVolatileBucketTable->m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext)
    {
        if (pSearch->dwHashValue == dwHash && Helper::CompareKeys(pSearch, pKey))
        {
            *ppPrev = pSearch->pNext;
            Helper::DeleteEntry(pSearch, m_Heap);

            // Do we ever want to shrink?
            m_dwNumEntries--;

            return TRUE;
        }

        ppPrev = &pSearch->pNext;
    }

    return FALSE;
}


template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::ReplaceValue(KeyType pKey, HashDatum Data)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE (OwnLock());
    
    EEHashEntry_t *pItem = FindItem(pKey);

    if (pItem != NULL)
    {
        // Required to be atomic
        pItem->Data = Data;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::ReplaceKey(KeyType pOldKey, KeyType pNewKey)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE (OwnLock());
    
    EEHashEntry_t *pItem = FindItem(pOldKey);

    if (pItem != NULL)
    {
        Helper::ReplaceKey (pItem, pNewKey);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif // !DACCESS_COMPILE

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
DWORD EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::GetHash(KeyType pKey)
{
    WRAPPER_CONTRACT;
    return Helper::Hash(pKey);
}


template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::GetValue(KeyType pKey, HashDatum *pData)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    EEHashEntry_t *pItem = FindItem(pKey);

    if (pItem != NULL)
    {
        *pData = pItem->Data;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::GetValue(KeyType pKey, HashDatum *pData, DWORD hashValue)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    EEHashEntry_t *pItem = FindItem(pKey, hashValue);

    if (pItem != NULL)
    {
        *pData = pItem->Data;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
FORCEINLINE BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::GetValueSpeculative(KeyType pKey, HashDatum *pData)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
#ifdef MODE_COOPERATIVE     // This header file sees contract.h, not eecontract.h - what a kludge!
        MODE_COOPERATIVE;
#endif
        SO_TOLERANT;
    }
    CONTRACTL_END

    EEHashEntry_t *pItem = FindItemSpeculative(pKey, Helper::Hash(pKey));

    if (pItem != NULL)
    {
        *pData = pItem->Data;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
EEHashEntry_t *EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::FindItem(KeyType pKey)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

    return FindItem(pKey, Helper::Hash(pKey));
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
EEHashEntry_t *EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::FindItem(KeyType pKey, DWORD dwHash)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
        SO_TOLERANT;
    }
    CONTRACTL_END

#ifndef DACCESS_COMPILE    
   GCX_COOP_NO_THREAD_BROKEN();
#endif

    // Atomic transaction. In any other point of this method or ANY of the callees of this function you can not read
    // from m_pVolatileBucketTable!!!!!!! A racing condition would occur.
    DWORD dwOldNumBuckets;

#ifndef DACCESS_COMPILE    
    DWORD nTry = 0;
#endif
    do 
    {       
        BucketTable* pBucketTable=m_pVolatileBucketTable;
        dwOldNumBuckets = pBucketTable->m_dwNumBuckets;
       
        _ASSERTE(pBucketTable->m_dwNumBuckets != 0);

        DWORD           dwBucket = dwHash % pBucketTable->m_dwNumBuckets;
        EEHashEntry_t * pSearch;

        for (pSearch = pBucketTable->m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext)
        {
            if (pSearch->dwHashValue == dwHash && Helper::CompareKeys(pSearch, pKey))
                return pSearch;
        }

        // There is a race in EEHash Table: when we grow the hash table, we will nuke out 
        // the old bucket table. Readers might be looking up in the old table, they can 
        // fail to find an existing entry. The workaround is to retry the search process 
        // if we are called grow table during the search process.
#ifndef DACCESS_COMPILE
        nTry ++;
        if (nTry == 20) {
            __SwitchToThread(0);
            nTry = 0;
        }
#endif // #ifndef DACCESS_COMPILE
    }
    while ( m_bGrowing || dwOldNumBuckets != m_pVolatileBucketTable->m_dwNumBuckets);
    
    return NULL;
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
FORCEINLINE EEHashEntry_t *EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::FindItemSpeculative(KeyType pKey, DWORD dwHash)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
#ifdef MODE_COOPERATIVE     // This header file sees contract.h, not eecontract.h - what a kludge!
        MODE_COOPERATIVE;
#endif
        SO_TOLERANT;
    }
    CONTRACTL_END

    // Atomic transaction. In any other point of this method or ANY of the callees of this function you can not read
    // from m_pVolatileBucketTable!!!!!!! A racing condition would occur.
    DWORD dwOldNumBuckets;

    BucketTable* pBucketTable=m_pVolatileBucketTable;
    dwOldNumBuckets = pBucketTable->m_dwNumBuckets;
    
    _ASSERTE(pBucketTable->m_dwNumBuckets != 0);

    DWORD           dwBucket = dwHash % pBucketTable->m_dwNumBuckets;
    EEHashEntry_t * pSearch;

    for (pSearch = pBucketTable->m_pBuckets[dwBucket]; pSearch; pSearch = pSearch->pNext)
    {
        if (pSearch->dwHashValue == dwHash && Helper::CompareKeys(pSearch, pKey))
            return pSearch;
    }
    
    return NULL;
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::IsEmpty()
{
    LEAF_CONTRACT;
    return m_dwNumEntries == 0;
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
DWORD EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::GetCount()
{
    LEAF_CONTRACT;
    return m_dwNumEntries;
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::GrowHashTable()
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        INJECT_FAULT(return FALSE;);
    }
    CONTRACTL_END

    _ASSERTE(!g_fEEStarted || GetThread()->PreemptiveGCDisabled());

    // Make the new bucket table 4 times bigger
    DWORD dwNewNumBuckets = m_pVolatileBucketTable->m_dwNumBuckets * 4;

    // On resizes, we still have an array of old pointers we need to worry about.
    // We can't free these old pointers, for we may hit a race condition where we're
    // resizing and reading from the array at the same time. We need to keep track of these
    // old arrays of pointers, so we're going to use the last item in the array to "link"
    // to previous arrays, so that they may be freed at the end.
    
    EEHashEntry_t **pNewBuckets = new (nothrow) EEHashEntry_t*[dwNewNumBuckets+1];

    if (pNewBuckets == NULL)
    {
        return FALSE;
    }
    
    memset(pNewBuckets, 0, (dwNewNumBuckets+1)*sizeof(EEHashEntry_t*));
    // The first slot is linked to next list.
    pNewBuckets ++;

    // Run through the old table and transfer all the entries

    // Be sure not to mess with the integrity of the old table while
    // we are doing this, as there can be concurrent readers!  Note that
    // it is OK if the concurrent reader misses out on a match, though -
    // they will have to acquire the lock on a miss & try again.
    FastInterlockExchange( (LONG *) &m_bGrowing, 1);
    for (DWORD i = 0; i < m_pVolatileBucketTable->m_dwNumBuckets; i++)
    {
        EEHashEntry_t * pEntry = m_pVolatileBucketTable->m_pBuckets[i];

        // Try to lock out readers from scanning this bucket.  This is
        // obviously a race which may fail. However, note that it's OK
        // if somebody is already in the list - it's OK if we mess
        // with the bucket groups, as long as we don't destroy
        // anything.  The lookup function will still do appropriate
        // comparison even if it wanders aimlessly amongst entries
        // while we are rearranging things.  If a lookup finds a match
        // under those circumstances, great.  If not, they will have
        // to acquire the lock & try again anyway.

        m_pVolatileBucketTable->m_pBuckets[i] = NULL;

        while (pEntry != NULL)
        {
            DWORD           dwNewBucket = pEntry->dwHashValue % dwNewNumBuckets;
            EEHashEntry_t * pNextEntry   = pEntry->pNext;

            pEntry->pNext = pNewBuckets[dwNewBucket];
            pNewBuckets[dwNewBucket] = pEntry;
            pEntry = pNextEntry;
        }
    }


    // Finally, store the new number of buckets and the new bucket table
    BucketTable* pNewBucketTable = (m_pVolatileBucketTable == &m_BucketTable[0]) ?
                    &m_BucketTable[1]:
                    &m_BucketTable[0];

    pNewBucketTable->m_pBuckets = pNewBuckets;
    pNewBucketTable->m_dwNumBuckets = dwNewNumBuckets;

    // Add old table to the to free list. Note that the SyncClean thing will only 
    // delete the buckets at a safe point
    SyncClean::AddEEHashTable (m_pVolatileBucketTable->m_pBuckets);
    
    // Swap the double buffer, this is an atomic operation (the assignment)
    m_pVolatileBucketTable = pNewBucketTable;

    FastInterlockExchange( (LONG *) &m_bGrowing, 0);                                                                                                        
    

    return TRUE;

}


// Walk through all the entries in the hash table, in meaningless order, without any
// synchronization.
//
//           IterateStart()
//           while (IterateNext())
//              GetKey();
//
template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
void EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::
            IterateStart(EEHashTableIteration *pIter)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE_IMPL(OwnLock());
    pIter->m_dwBucket = -1;
    pIter->m_pEntry = NULL;

#ifdef _DEBUG
    pIter->m_pTable = this;
#endif
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
BOOL EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::
            IterateNext(EEHashTableIteration *pIter)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE_IMPL(OwnLock());

    Thread *pThread = GetThread();
    GCX_MAYBE_COOP_NO_THREAD_BROKEN(pThread ? !(pThread->m_StateNC & Thread::TSNC_UnsafeSkipEnterCooperative) : FALSE);
    
    _ASSERTE(pIter->m_pTable == (void *) this);

    // If we haven't started iterating yet, or if we are at the end of a particular
    // chain, advance to the next chain.
    while (pIter->m_pEntry == NULL || pIter->m_pEntry->pNext == NULL)
    {
        if (++pIter->m_dwBucket >= m_pVolatileBucketTable->m_dwNumBuckets)
        {
            // advanced beyond the end of the table.
            _ASSERTE(pIter->m_dwBucket == m_pVolatileBucketTable->m_dwNumBuckets);   // client keeps asking?
            return FALSE;
        }
        pIter->m_pEntry = m_pVolatileBucketTable->m_pBuckets[pIter->m_dwBucket];

        // If this bucket has no chain, keep advancing.  Otherwise we are done
        if (pIter->m_pEntry)
            return TRUE;
    }

    // We are within a chain.  Advance to the next entry
    pIter->m_pEntry = pIter->m_pEntry->pNext;

    _ASSERTE(pIter->m_pEntry);
    return TRUE;
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
KeyType EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::
            IterateGetKey(EEHashTableIteration *pIter)
{
    CONTRACTL
    {
        WRAPPER(THROWS);
        WRAPPER(GC_NOTRIGGER);
        FORBID_FAULT;
    }
    CONTRACTL_END

    _ASSERTE(pIter->m_pTable == (void *) this);
    _ASSERTE(pIter->m_dwBucket < m_pVolatileBucketTable->m_dwNumBuckets && pIter->m_pEntry);
    return Helper::GetKey(pIter->m_pEntry);
}

template <class KeyType, class Helper, BOOL bDefaultCopyIsDeep>
HashDatum EEHashTableBase<KeyType, Helper, bDefaultCopyIsDeep>::
            IterateGetValue(EEHashTableIteration *pIter)
{
    LEAF_CONTRACT;

    _ASSERTE(pIter->m_pTable == (void *) this);
    _ASSERTE(pIter->m_dwBucket < m_pVolatileBucketTable->m_dwNumBuckets && pIter->m_pEntry);
    return pIter->m_pEntry->Data;
}

class EEIntHashTableHelper
{
public:
    static EEHashEntry_t *AllocateEntry(int iKey, BOOL bDeepCopy, AllocationHeap pHeap = 0)
    {
        CONTRACTL
        {
            WRAPPER(THROWS);
            WRAPPER(GC_NOTRIGGER);
            INJECT_FAULT(return NULL;);
        }
        CONTRACTL_END
    
        _ASSERTE(!bDeepCopy && "Deep copy is not supported by the EEPtrHashTableHelper");

        EEHashEntry_t *pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(int)];
        if (!pEntry)
            return NULL;
        *((int*) pEntry->Key) = iKey;

        return pEntry;
    }

    static void DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap pHeap = 0)
    {
        LEAF_CONTRACT;

        // Delete the entry.
        delete [] (BYTE*) pEntry;
    }

    static BOOL CompareKeys(EEHashEntry_t *pEntry, int iKey)
    {
        LEAF_CONTRACT;

        return *((int*)pEntry->Key) == iKey;
    }

    static DWORD Hash(int iKey)
    {
        LEAF_CONTRACT;

        return (DWORD)iKey;
    }

    static int GetKey(EEHashEntry_t *pEntry)
    {
        LEAF_CONTRACT;

        return *((int*) pEntry->Key);
    }
};
typedef EEHashTable<int, EEIntHashTableHelper, FALSE> EEIntHashTable;

typedef struct PtrPlusInt
{
	void* pValue;
	int iValue;
} *PPtrPlusInt;

class EEPtrPlusIntHashTableHelper
{
public:
    static EEHashEntry_t *AllocateEntry(PtrPlusInt ppiKey, BOOL bDeepCopy, AllocationHeap pHeap = 0)
    {
        CONTRACTL
        {
            WRAPPER(THROWS);
            WRAPPER(GC_NOTRIGGER);
            INJECT_FAULT(return NULL;);
        }
        CONTRACTL_END
    
        _ASSERTE(!bDeepCopy && "Deep copy is not supported by the EEPtrPlusIntHashTableHelper");

        EEHashEntry_t *pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(PtrPlusInt)];
        if (!pEntry)
            return NULL;
        *((PPtrPlusInt) pEntry->Key) = ppiKey;

        return pEntry;
    }

    static void DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap pHeap = 0)
    {
        LEAF_CONTRACT;

        // Delete the entry.
        delete [] (BYTE*) pEntry;
    }

    static BOOL CompareKeys(EEHashEntry_t *pEntry, PtrPlusInt ppiKey)
    {
        LEAF_CONTRACT;

        return (((PPtrPlusInt)pEntry->Key)->pValue == ppiKey.pValue) &&
               (((PPtrPlusInt)pEntry->Key)->iValue == ppiKey.iValue);
    }

    static DWORD Hash(PtrPlusInt ppiKey)
    {
        LEAF_CONTRACT;

		return (DWORD)ppiKey.iValue ^ 
#ifdef _X86_
        	(DWORD)(size_t) ppiKey.pValue;
#else
        	(DWORD)(((size_t) ppiKey.pValue) >> 3);
#endif
    }

    static PtrPlusInt GetKey(EEHashEntry_t *pEntry)
    {
        LEAF_CONTRACT;

        return *((PPtrPlusInt) pEntry->Key);
    }
};

typedef EEHashTable<PtrPlusInt, EEPtrPlusIntHashTableHelper, FALSE> EEPtrPlusIntHashTable;

// UTF8 string hash table. The UTF8 strings are NULL terminated.

class EEUtf8HashTableHelper
{
public:
    static EEHashEntry_t * AllocateEntry(LPCUTF8 pKey, BOOL bDeepCopy, AllocationHeap Heap);
    static void            DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap Heap);
    static BOOL            CompareKeys(EEHashEntry_t *pEntry, LPCUTF8 pKey);
    static DWORD           Hash(LPCUTF8 pKey);
    static LPCUTF8         GetKey(EEHashEntry_t *pEntry);
};

typedef EEHashTable<LPCUTF8, EEUtf8HashTableHelper, TRUE> EEUtf8StringHashTable;


// Unicode String hash table - the keys are UNICODE strings which may
// contain embedded nulls.  An EEStringData struct is used for the key
// which contains the length of the item.  Note that this string is
// not necessarily null terminated and should never be treated as such.
const DWORD ONLY_LOW_CHARS_MASK = 0x80000000;

class EEStringData
{
private:
    LPCWSTR         szString;           // The string data.
    DWORD           cch;                // Characters in the string.
#ifdef _DEBUG
    BOOL            bDebugOnlyLowChars;      // Does the string contain only characters less than 0x80?
    DWORD           dwDebugCch;
#endif // _DEBUG

public:
    // explicilty initialize cch to 0 because SetCharCount uses cch
    EEStringData() : cch(0)
    {
        LEAF_CONTRACT;

        SetStringBuffer(NULL);
        SetCharCount(0);
        SetIsOnlyLowChars(FALSE);
    };
    EEStringData(DWORD cchString, LPCWSTR str) : cch(0)
    { 
        LEAF_CONTRACT;

        SetStringBuffer(str);
        SetCharCount(cchString);
        SetIsOnlyLowChars(FALSE);
    };
    EEStringData(DWORD cchString, LPCWSTR str, BOOL onlyLow) : cch(0)
    { 
        LEAF_CONTRACT;

        SetStringBuffer(str);
        SetCharCount(cchString);
        SetIsOnlyLowChars(onlyLow);
    };
    inline ULONG GetCharCount() const
    { 
        LEAF_CONTRACT;

        _ASSERTE ((cch & ~ONLY_LOW_CHARS_MASK) == dwDebugCch);
        return (cch & ~ONLY_LOW_CHARS_MASK); 
    }
    inline void SetCharCount(ULONG _cch)
    {
        LEAF_CONTRACT;

#ifdef _DEBUG
        dwDebugCch = _cch;
#endif // _DEBUG
        cch = ((DWORD)_cch) | (cch & ONLY_LOW_CHARS_MASK);
    }
    inline LPCWSTR GetStringBuffer() const
    { 
        LEAF_CONTRACT;

        return (szString); 
    }
    inline void SetStringBuffer(LPCWSTR _szString)
    {
        LEAF_CONTRACT;

        szString = _szString;
    }
    inline BOOL GetIsOnlyLowChars() const 
    { 
        LEAF_CONTRACT;

        _ASSERTE(bDebugOnlyLowChars == ((cch & ONLY_LOW_CHARS_MASK) ? TRUE : FALSE));
        return ((cch & ONLY_LOW_CHARS_MASK) ? TRUE : FALSE); 
    }
    inline void SetIsOnlyLowChars(BOOL bIsOnlyLowChars)
    {
        LEAF_CONTRACT;

#ifdef _DEBUG
        bDebugOnlyLowChars = bIsOnlyLowChars;
#endif // _DEBUG
        bIsOnlyLowChars ? (cch |= ONLY_LOW_CHARS_MASK) : (cch &= ~ONLY_LOW_CHARS_MASK);        
    }
};

class EEUnicodeHashTableHelper
{
public:
    static EEHashEntry_t * AllocateEntry(EEStringData *pKey, BOOL bDeepCopy, AllocationHeap Heap);
    static void            DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap Heap);
    static BOOL            CompareKeys(EEHashEntry_t *pEntry, EEStringData *pKey);
    static DWORD           Hash(EEStringData *pKey);
    static EEStringData *  GetKey(EEHashEntry_t *pEntry);
    static void            ReplaceKey(EEHashEntry_t *pEntry, EEStringData *pNewKey);
};

typedef EEHashTable<EEStringData *, EEUnicodeHashTableHelper, TRUE> EEUnicodeStringHashTable;


class EEUnicodeStringLiteralHashTableHelper
{
public:
    static EEHashEntry_t * AllocateEntry(EEStringData *pKey, BOOL bDeepCopy, AllocationHeap Heap);
    static void            DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap Heap);
    static BOOL            CompareKeys(EEHashEntry_t *pEntry, EEStringData *pKey);
    static DWORD           Hash(EEStringData *pKey);
    static void            ReplaceKey(EEHashEntry_t *pEntry, EEStringData *pNewKey);
};

typedef EEHashTable<EEStringData *, EEUnicodeStringLiteralHashTableHelper, TRUE> EEUnicodeStringLiteralHashTable;

// Permission set hash table.

class EEPsetHashTableHelper
{
public:
    static EEHashEntry_t * AllocateEntry(PsetCacheEntry *pKey, BOOL bDeepCopy, AllocationHeap Heap);
    static void            DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap Heap);
    static BOOL            CompareKeys(EEHashEntry_t *pEntry, PsetCacheEntry *pKey);
    static DWORD           Hash(PsetCacheEntry *pKey);
    static PsetCacheEntry *GetKey(EEHashEntry_t *pEntry);
};

typedef EEHashTableStatic<PsetCacheEntry *, EEPsetHashTableHelper, FALSE> EEPsetHashTable;


// Generic pointer hash table helper.

template <class KeyPointerType>
class EEPtrHashTableHelper
{
public:
    static EEHashEntry_t *AllocateEntry(KeyPointerType pKey, BOOL bDeepCopy, AllocationHeap Heap)
    {
        CONTRACTL
        {
            WRAPPER(THROWS);
            WRAPPER(GC_NOTRIGGER);
            INJECT_FAULT(return FALSE;);
        }
        CONTRACTL_END
    
        MEMORY_REPORT_CONTEXT_SCOPE("EEPtrHash");

        _ASSERTE(!bDeepCopy && "Deep copy is not supported by the EEPtrHashTableHelper");
        _ASSERTE(sizeof(KeyPointerType) == sizeof(void *) && "KeyPointerType must be a pointer type");

        EEHashEntry_t *pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(KeyPointerType)];
        if (!pEntry)
            return NULL;
        *((KeyPointerType*)pEntry->Key) = pKey;

        return pEntry;
    }

    static void DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap Heap)
    {
        LEAF_CONTRACT;

        // Delete the entry.
        delete [] (BYTE*) pEntry;
    }

    static BOOL CompareKeys(EEHashEntry_t *pEntry, KeyPointerType pKey)
    {
        LEAF_CONTRACT;

        KeyPointerType pEntryKey = *((KeyPointerType*)pEntry->Key);
        return pEntryKey == pKey;
    }

    static DWORD Hash(KeyPointerType pKey)
    {
        LEAF_CONTRACT;

#ifdef _X86_
        return (DWORD)(size_t) pKey;
#else
        return (DWORD)(((size_t) pKey) >> 3);
#endif
    }

    static KeyPointerType GetKey(EEHashEntry_t *pEntry)
    {
        LEAF_CONTRACT;

        return *((KeyPointerType*)pEntry->Key);
    }
};

typedef EEHashTable<void *, EEPtrHashTableHelper<void *>, FALSE> EEPtrHashTable;
typedef DPTR(EEPtrHashTable) PTR_EEPtrHashTable;


// Define a hash of generic instantiations (represented by a SigTypeContext).
class EEInstantiationHashTableHelper
{
public:
    static EEHashEntry_t *AllocateEntry(const SigTypeContext *pKey, BOOL bDeepCopy, AllocationHeap pHeap = 0);
    static void DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap pHeap = 0);
    static BOOL CompareKeys(EEHashEntry_t *pEntry, const SigTypeContext *pKey);
    static DWORD Hash(const SigTypeContext *pKey);
    static const SigTypeContext *GetKey(EEHashEntry_t *pEntry);
};
typedef EEHashTable<const SigTypeContext*, EEInstantiationHashTableHelper, FALSE> EEInstantiationHashTable;

// ComComponentInfo hashtable.

struct ClassFactoryInfo
{
    GUID     m_clsid;
    WCHAR   *m_strServerName;
};

class EEClassFactoryInfoHashTableHelper
{
public:
    static EEHashEntry_t *AllocateEntry(ClassFactoryInfo *pKey, BOOL bDeepCopy, AllocationHeap Heap);
    static void DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap Heap);
    static BOOL CompareKeys(EEHashEntry_t *pEntry, ClassFactoryInfo *pKey);
    static DWORD Hash(ClassFactoryInfo *pKey);
    static ClassFactoryInfo *GetKey(EEHashEntry_t *pEntry);
};

typedef EEHashTable<ClassFactoryInfo *, EEClassFactoryInfoHashTableHelper, TRUE> EEClassFactoryInfoHashTable;


// One of these is present for each element in the table

typedef DPTR(struct EEClassHashEntry) PTR_EEClassHashEntry;
typedef struct EEClassHashEntry
{
    PTR_EEClassHashEntry pNext;
    PTR_EEClassHashEntry pEncloser;     // stores nested class
    DWORD               dwHashValue;    // Hash of the name of the type

    #define EECLASSHASH_TYPEHANDLE_DISCR ((ULONG_PTR)(UINT)0x00000001)
    #define EECLASSHASH_MDEXPORT_DISCR   ((ULONG_PTR)(UINT)0x80000000)
    #define EECLASSHASH_ALREADYSEEN      ((ULONG_PTR)(UINT)0x40000000)
    HashDatum           Data;           // Either the token (if EECLASSHASH_TYPEHANDLE_DISCR), or the type handle
    
#ifdef _DEBUG
    PTR_CUTF8           DebugKey[2];    // Name of the type
#endif // _DEBUG

    TypeHandle           GetEntry();     // Get the type handle (the token must have been resolved)
} EEClassHashEntry_t;

// Class name/namespace hashtable.

typedef DPTR(class EEClassHashTable) PTR_EEClassHashTable;
class EEClassHashTable 
{
    friend class ClassLoader;


protected:
    DPTR(PTR_EEClassHashEntry) m_pBuckets;          // Pointer to first entry for each bucket
    DWORD                      m_dwNumBuckets;      // Count of buckets in the array
    DWORD                      m_dwNumEntries;      // Count of total number of entries for all buckets
    BOOL                       m_bCaseInsensitive;  // Default is true FALSE unless we call MakeCaseInsensitiveTable

    // For working-set perf reasons, we identify and collect all frequently-accessed buckets and entries.
    DWORD                      m_dwNumHotBuckets;   // A collection of the buckets that turned out to be hot.
    PTR_DWORD                  m_hotBucketIndex;    // The index value for each bucket (because non-hot buckets are excluded).
    DPTR(PTR_EEClassHashEntry) m_hotBucketChain;    // The hot buckets.

public:
    PTR_Module                 m_pModule;

#ifdef _DEBUG
    DWORD                      m_dwDebugMemory;
#endif

private:
#ifndef DACCESS_COMPILE
    EEClassHashTable();  //Intentionally absent constructor: Use EEClassHashTable::Create() to create hash tables.
    ~EEClassHashTable(); //Intentionally absent destructor: These are loaderheap-allocated hashtables - use allocmem trackers or holders to delete
#endif

public:
    static EEClassHashTable *Create(Module *pModule, DWORD dwNumBuckets, BOOL bCaseInsensitive, AllocMemTracker *pamTracker);
private:
    void               operator delete(void *p);  // Intentionally absent delete operator: These are loaderheap-allocated hashtables - use allocmem trackers or holders to delete  
public:    
    //NOTICE: look at InsertValue() in ClassLoader, that may be the function you want to use. Use this only
    //        when you are sure you want to insert the value in 'this' table. This function does not deal
    //        with case (as often the class loader has to)
    EEClassHashEntry_t *InsertValue(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum Data, EEClassHashEntry_t *pEncloser, AllocMemTracker *pamTracker);
    EEClassHashEntry_t *InsertValueIfNotFound(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum *pData, EEClassHashEntry_t *pEncloser, BOOL IsNested, BOOL *pbFound, AllocMemTracker *pamTracker);
    EEClassHashEntry_t *InsertValueUsingPreallocatedEntry(EEClassHashEntry_t *pStorageForNewEntry, LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum Data, EEClassHashEntry_t *pEncloser);
    EEClassHashEntry_t *GetValue(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum *pData, BOOL IsNested);
    EEClassHashEntry_t *GetValue(LPCUTF8 pszFullyQualifiedName, HashDatum *pData, BOOL IsNested);
    EEClassHashEntry_t *GetValue(NameHandle* pName, HashDatum *pData, BOOL IsNested);
    EEClassHashEntry_t *AllocNewEntry(AllocMemTracker *pamTracker);
    EEClassHashTable   *MakeCaseInsensitiveTable(Module *pModule, AllocMemTracker *pamTracker);
    EEClassHashEntry_t *FindNextNestedClass(NameHandle* pName, HashDatum *pData, EEClassHashEntry_t *pBucket);
    EEClassHashEntry_t *FindNextNestedClass(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum *pData, EEClassHashEntry_t *pBucket);
    EEClassHashEntry_t *FindNextNestedClass(LPCUTF8 pszFullyQualifiedName, HashDatum *pData, EEClassHashEntry_t *pBucket);

    BOOL     CompareKeys(EEClassHashEntry_t *pEntry, LPCUTF8 *pKey2);

    static DWORD    Hash(LPCUTF8 pszNamespace, LPCUTF8 pszClassName);

    class ConstructKeyCallback
    {
        public:
            virtual void UseKeys(LPUTF8 *Key) = 0;
    };


    static HashDatum CompressClassDef(mdToken cl /* either a TypeDef or ExportedType*/);
    VOID UncompressModuleAndClassDef(HashDatum Data, Loader::LoadFlag loadFlag,
                                     Module **ppModule, mdTypeDef *pCL,
                                     mdExportedType *pmdFoundExportedType);
    VOID UncompressModuleAndNonExportClassDef(HashDatum Data, Module **ppModule,
                                              mdTypeDef *pCL);
    static mdToken UncompressModuleAndClassDef(HashDatum Data);



#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif


private:
    EEClassHashEntry_t * FindItem(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, BOOL IsNested);
    EEClassHashEntry_t * FindItemHelper(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, BOOL IsNested, DWORD dwHash, DWORD dwBucket);
    EEClassHashEntry_t * InsertValueHelper(EEClassHashEntry_t *pStorageForNewEntry, LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum Data, EEClassHashEntry_t *pEncloser, DWORD dwHash, DWORD dwBucket);
    VOID                 RetuneHashTable(AllocMemTracker *pamTracker);

    VOID                 ConstructKeyFromData(EEClassHashEntry_t *pEntry, ConstructKeyCallback *pCallback);

};



// Struct to hold a client's iteration state
struct EEHashTableIteration
{
    DWORD              m_dwBucket;
    EEHashEntry_t     *m_pEntry;

#ifdef _DEBUG
    void              *m_pTable;
#endif
};

#endif /* _EE_HASH_H */
