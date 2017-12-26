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
//  util.cpp
//
//  This contains a bunch of C++ utility classes.
//
//*****************************************************************************
#include "stdafx.h"                     // Precompiled header key.
#include "utilcode.h"
#include "metadata.h"
#include "ndpversion.h"
#include "ex.h"
#include "pedecoder.h"


char g_RTMVersion[]= "v1.0.3705";

//********** Code. ************************************************************


//
//
// CHashTable
//
//

#ifndef DACCESS_COMPILE

//*****************************************************************************
// This is the second part of construction where we do all of the work that
// can fail.  We also take the array of structs here because the calling class
// presumably needs to allocate it in its NewInit.
//*****************************************************************************
HRESULT CHashTable::NewInit(            // Return status.
    BYTE        *pcEntries,             // Array of structs we are managing.
    ULONG      iEntrySize)             // Size of the entries.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    _ASSERTE(iEntrySize >= sizeof(FREEHASHENTRY));

    // Allocate the bucket chain array and init it.
    if ((m_piBuckets = new (nothrow) ULONG [m_iBuckets]) == NULL)
        return (OutOfMemory());
    memset(m_piBuckets, 0xff, m_iBuckets * sizeof(ULONG));

    // Save the array of structs we are managing.
    m_pcEntries = (TADDR)pcEntries;
    m_iEntrySize = iEntrySize;
    return (S_OK);
}

BYTE *CHashTable::Add(                  // New entry.
    ULONG      iHash,                  // Hash value of entry to add.
    ULONG      iIndex)                 // Index of struct in m_pcEntries.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    HASHENTRY   *psEntry;               // The struct we are adding.

    // Get a pointer to the entry we are adding.
    psEntry = EntryPtr(iIndex);

    // Compute the hash value for the entry.
    iHash %= m_iBuckets;

    _ASSERTE(m_piBuckets[iHash] != iIndex &&
        (m_piBuckets[iHash] == UINT32_MAX || EntryPtr(m_piBuckets[iHash])->iPrev != iIndex));

    // Setup this entry.
    psEntry->iPrev = UINT32_MAX;
    psEntry->iNext = m_piBuckets[iHash];

    // Link it into the hash chain.
    if (m_piBuckets[iHash] != UINT32_MAX)
        EntryPtr(m_piBuckets[iHash])->iPrev = iIndex;
    m_piBuckets[iHash] = iIndex;
    return ((BYTE *) psEntry);
}

void CHashTable::Delete(
    ULONG      iHash,                  // Hash value of entry to delete.
    ULONG      iIndex)                 // Index of struct in m_pcEntries.
{
    WRAPPER_CONTRACT;
    
    HASHENTRY   *psEntry;               // Struct to delete.
    
    // Get a pointer to the entry we are deleting.
    psEntry = EntryPtr(iIndex);
    Delete(iHash, psEntry);
}

void CHashTable::Delete(
    ULONG      iHash,                  // Hash value of entry to delete.
    HASHENTRY   *psEntry)               // The struct to delete.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    // Compute the hash value for the entry.
    iHash %= m_iBuckets;

    _ASSERTE(psEntry->iPrev != psEntry->iNext || psEntry->iPrev == UINT32_MAX);

    // Fix the predecessor.
    if (psEntry->iPrev == UINT32_MAX)
        m_piBuckets[iHash] = psEntry->iNext;
    else
        EntryPtr(psEntry->iPrev)->iNext = psEntry->iNext;

    // Fix the successor.
    if (psEntry->iNext != UINT32_MAX)
        EntryPtr(psEntry->iNext)->iPrev = psEntry->iPrev;
}

//*****************************************************************************
// The item at the specified index has been moved, update the previous and
// next item.
//*****************************************************************************
void CHashTable::Move(
    ULONG      iHash,                  // Hash value for the item.
    ULONG      iNew)                   // New location.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    HASHENTRY   *psEntry;               // The struct we are deleting.

    psEntry = EntryPtr(iNew);
    _ASSERTE(psEntry->iPrev != iNew && psEntry->iNext != iNew);

    if (psEntry->iPrev != UINT32_MAX)
        EntryPtr(psEntry->iPrev)->iNext = iNew;
    else
        m_piBuckets[iHash % m_iBuckets] = iNew;
    if (psEntry->iNext != UINT32_MAX)
        EntryPtr(psEntry->iNext)->iPrev = iNew;
}

#endif // #ifndef DACCESS_COMPILE

//*****************************************************************************
// Search the hash table for an entry with the specified key value.
//*****************************************************************************
BYTE *CHashTable::Find(                 // Index of struct in m_pcEntries.
    ULONG      iHash,                  // Hash value of the item.
    BYTE        *pcKey)                 // The key to match.
{
    CONTRACTL
    {
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    
    ULONG      iNext;
    HASHENTRY   *psEntry;

    iNext = m_piBuckets[iHash % m_iBuckets];

    // Search until we hit the end.
    
#ifdef _DEBUG
    unsigned count = 0;
#endif

    while (iNext != UINT32_MAX)
    {
        // Compare the keys.
        psEntry = EntryPtr(iNext);

#ifdef _DEBUG
        count++;
#endif
        if (!Cmp(pcKey, psEntry))
        {
#ifdef _DEBUG
            if (count > m_maxSearch)
                m_maxSearch = count;
#endif

            return ((BYTE *) psEntry);
        }

        // Advance to the next item in the chain.
        iNext = psEntry->iNext;
    }

    // We couldn't find it.
    return (0);
}

//*****************************************************************************
// Search the hash table for the next entry with the specified key value.
//*****************************************************************************
ULONG CHashTable::FindNext(            // Index of struct in m_pcEntries.
    BYTE        *pcKey,                 // The key to match.
    ULONG      iIndex)                 // Index of previous match.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    ULONG      iNext;
    HASHENTRY   *psEntry;

    iNext = EntryPtr(iIndex)->iNext;

    // Search until we hit the end.
    while (iNext != UINT32_MAX)
    {
        // Compare the keys.
        psEntry = EntryPtr(iNext);
        if (!Cmp(pcKey, psEntry))
            return (iNext);

        // Advance to the next item in the chain.
        iNext = psEntry->iNext;
    }

    // We couldn't find it.
    return (UINT32_MAX);
}

//*****************************************************************************
// Returns the next entry in the list.
//*****************************************************************************
BYTE *CHashTable::FindNextEntry(        // The next entry, or0 for end of list.
    HASHFIND    *psSrch)                // Search object.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    HASHENTRY   *psEntry;

    for (;;)
    {
        // See if we already have one to use and if so, use it.
        if (psSrch->iNext != UINT32_MAX)
        {
            psEntry = EntryPtr(psSrch->iNext);
            psSrch->iNext = psEntry->iNext;
            return ((BYTE *) psEntry);
        }

        // Advance to the next bucket.
        if (psSrch->iBucket < m_iBuckets)
            psSrch->iNext = m_piBuckets[psSrch->iBucket++];
        else
            break;
    }

    // There were no more entries to be found.
    return (0);
}

#ifdef DACCESS_COMPILE

void
CHashTable::EnumMemoryRegions(CLRDataEnumMemoryFlags flags,
                              ULONG numEntries)
{
    // This class may be embedded so do not enum 'this'.
    DacEnumMemoryRegion(m_pcEntries,
                        (ULONG)numEntries * m_iEntrySize);
    DacEnumMemoryRegion(PTR_TO_TADDR(m_piBuckets),
                        (ULONG)m_iBuckets * sizeof(ULONG));
}

#endif // #ifdef DACCESS_COMPILE

//
//
// CClosedHashBase
//
//

//*****************************************************************************
// Delete the given value.  This will simply mark the entry as deleted (in
// order to keep the collision chain intact).  There is an optimization that
// consecutive deleted entries leading up to a free entry are themselves freed
// to reduce collisions later on.
//*****************************************************************************
void CClosedHashBase::Delete(
    void        *pData)                 // Key value to delete.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    BYTE        *ptr;

    // Find the item to delete.
    if ((ptr = Find(pData)) == 0)
    {
        // You deleted something that wasn't there, why?
        _ASSERTE(0);
        return;
    }

    // One less active entry.
    --m_iCount;

    // For a perfect system, there are no collisions so it is free.
    if (m_bPerfect)
    {
        SetStatus(ptr, FREE);
        return;
    }

    // Mark this entry deleted.
    SetStatus(ptr, DELETED);

    // If the next item is free, then we can go backwards freeing
    // deleted entries which are no longer part of a chain.  This isn't
    // 100% great, but it will reduce collisions.
    BYTE        *pnext;
    if ((pnext = ptr + m_iEntrySize) > EntryPtr(m_iSize - 1))
        pnext = &m_rgData[0];
    if (Status(pnext) != FREE)
        return;
    
    // We can now free consecutive entries starting with the one
    // we just deleted, up to the first non-deleted one.
    while (Status(ptr) == DELETED)
    {
        // Free this entry.
        SetStatus(ptr, FREE);

        // Check the one before it, handle wrap around.
        if ((ptr -= m_iEntrySize) < &m_rgData[0])
            ptr = EntryPtr(m_iSize - 1);
    }
}


//*****************************************************************************
// Iterates over all active values, passing each one to pDeleteLoopFunc.
// If pDeleteLoopFunc returns TRUE, the entry is deleted. This is safer
// and faster than using FindNext() and Delete().
//*****************************************************************************
void CClosedHashBase::DeleteLoop(
    DELETELOOPFUNC pDeleteLoopFunc,     // Decides whether to delete item
    void *pCustomizer)                  // Extra value passed to deletefunc.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    int i;

    if (m_rgData == 0)
    {
        return;
    }

    for (i = 0; i < m_iSize; i++)
    {
        BYTE *pEntry = EntryPtr(i);
        if (Status(pEntry) == USED)
        {
            if (pDeleteLoopFunc(pEntry, pCustomizer))
            {
                SetStatus(pEntry, m_bPerfect ? FREE : DELETED);
                --m_iCount;  // One less active entry
            }
        }
    }

    if (!m_bPerfect)
    {
        // Now free DELETED entries that are no longer part of a chain.
        for (i = 0; i < m_iSize; i++)
        {
            if (Status(EntryPtr(i)) == FREE)
            {
                break;
            }
        }
        if (i != m_iSize)
        {
            int iFirstFree = i;
    
            do
            {
                if (i-- == 0)
                {
                    i = m_iSize - 1;
                }
                while (Status(EntryPtr(i)) == DELETED)
                {
                    SetStatus(EntryPtr(i), FREE);
                    if (i-- == 0)
                    {
                        i = m_iSize - 1;
                    }
                }
    
                while (Status(EntryPtr(i)) != FREE)
                {
                    if (i-- == 0)
                    {
                        i = m_iSize - 1;
                    }
                }
    
            }
            while (i != iFirstFree);
        }
    }

}

//*****************************************************************************
// Lookup a key value and return a pointer to the element if found.
//*****************************************************************************
BYTE *CClosedHashBase::Find(            // The item if found, 0 if not.
    void        *pData)                 // The key to lookup.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    unsigned long iHash;                // Hash value for this data.
    int         iBucket;                // Which bucke to start at.
    int         i;                      // Loop control.

    // Safety check.
    if (!m_rgData || m_iCount == 0)
        return (0);

    // Hash to the bucket.
    iHash = Hash(pData);
    iBucket = iHash % m_iBuckets;

    // For a perfect table, the bucket is the correct one.
    if (m_bPerfect)
    {
        // If the value is there, it is the correct one.
        if (Status(EntryPtr(iBucket)) != FREE)
            return (EntryPtr(iBucket));
        return (0);
    }

    // Walk the bucket list looking for the item.
    for (i=iBucket;  Status(EntryPtr(i)) != FREE;  )
    {
        // Don't look at deleted items.
        if (Status(EntryPtr(i)) == DELETED)
        {
            // Handle wrap around.
            if (++i >= m_iSize)
                i = 0;
            continue;
        }

        // Check this one.
        if (Compare(pData, EntryPtr(i)) == 0)
            return (EntryPtr(i));

        // If we never collided while adding items, then there is
        // no point in scanning any further.
        if (!m_iCollisions)
            return (0);

        // Handle wrap around.
        if (++i >= m_iSize)
            i = 0;
    }
    return (0);
}



//*****************************************************************************
// Look for an item in the table.  If it isn't found, then create a new one and
// return that.
//*****************************************************************************
BYTE *CClosedHashBase::FindOrAdd(       // The item if found, 0 if not.
    void        *pData,                 // The key to lookup.
    bool        &bNew)                  // true if created.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    unsigned long iHash;                // Hash value for this data.
    int         iBucket;                // Which bucke to start at.
    int         i;                      // Loop control.

    // If we haven't allocated any memory, or it is too small, fix it.
    if (!m_rgData || ((m_iCount + 1) > (m_iSize * 3 / 4) && !m_bPerfect))
    {
        if (!ReHash())
            return (0);
    }

    // Assume we find it.
    bNew = false;

    // Hash to the bucket.
    iHash = Hash(pData);
    iBucket = iHash % m_iBuckets;

    // For a perfect table, the bucket is the correct one.
    if (m_bPerfect)
    {
        // If the value is there, it is the correct one.
        if (Status(EntryPtr(iBucket)) != FREE)
            return (EntryPtr(iBucket));
        i = iBucket;
    }
    else
    {
        // Walk the bucket list looking for the item.
        for (i=iBucket;  Status(EntryPtr(i)) != FREE;  )
        {
            // Don't look at deleted items.
            if (Status(EntryPtr(i)) == DELETED)
            {
                // Handle wrap around.
                if (++i >= m_iSize)
                    i = 0;
                continue;
            }

            // Check this one.
            if (Compare(pData, EntryPtr(i)) == 0)
                return (EntryPtr(i));

            // One more to count.
            ++m_iCollisions;

            // Handle wrap around.
            if (++i >= m_iSize)
                i = 0;
        }
    }

    // We've found an open slot, use it.
    _ASSERTE(Status(EntryPtr(i)) == FREE);
    bNew = true;
    ++m_iCount;
    return (EntryPtr(i));
}

//*****************************************************************************
// This helper actually does the add for you.
//*****************************************************************************
BYTE *CClosedHashBase::DoAdd(void *pData, BYTE *rgData, int &iBuckets, int iSize, 
            int &iCollisions, int &iCount)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    unsigned long iHash;                // Hash value for this data.
    int         iBucket;                // Which bucke to start at.
    int         i;                      // Loop control.

    // Hash to the bucket.
    iHash = Hash(pData);
    iBucket = iHash % iBuckets;

    // For a perfect table, the bucket is free.
    if (m_bPerfect)
    {
        i = iBucket;
        _ASSERTE(Status(EntryPtr(i, rgData)) == FREE);
    }
    // Need to scan.
    else
    {
        // Walk the bucket list looking for a slot.
        for (i=iBucket;  Status(EntryPtr(i, rgData)) != FREE;  )
        {
            // Handle wrap around.
            if (++i >= iSize)
                i = 0;

            // If we made it this far, we collided.
            ++iCollisions;
        }
    }

    // One more item in list.
    ++iCount;

    // Return the new slot for the caller.
    return (EntryPtr(i, rgData));
}

//*****************************************************************************
// This function is called either to init the table in the first place, or
// to rehash the table if we ran out of room.
//*****************************************************************************
bool CClosedHashBase::ReHash()          // true if successful.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    // Allocate memory if we don't have any.
    if (!m_rgData)
    {
        if ((m_rgData = new (nothrow) BYTE [m_iSize * m_iEntrySize]) == 0)
            return (false);
        InitFree(&m_rgData[0], m_iSize);
        return (true);
    }

    // We have entries already, allocate a new table.
    BYTE        *rgTemp, *p;
    int         iBuckets = m_iBuckets * 2 - 1;
    int         iSize = iBuckets + 7;
    int         iCollisions = 0;
    int         iCount = 0;

    if ((rgTemp = new (nothrow) BYTE [iSize * m_iEntrySize]) == 0)
        return (false);
    InitFree(&rgTemp[0], iSize);
    m_bPerfect = false;

    // Rehash the data.
    for (int i=0;  i<m_iSize;  i++)
    {
        // Only copy used entries.
        if (Status(EntryPtr(i)) != USED)
            continue;
        
        // Add this entry to the list again.
        VERIFY((p = DoAdd(GetKey(EntryPtr(i)), rgTemp, iBuckets,
                iSize, iCollisions, iCount)));
        memmove(p, EntryPtr(i), m_iEntrySize);
    }
    
    // Reset internals.
    delete [] m_rgData;
    m_rgData = rgTemp;
    m_iBuckets = iBuckets;
    m_iSize = iSize;
    m_iCollisions = iCollisions;
    m_iCount = iCount;
    return (true);
}


//
//
// CStructArray
//
//


//*****************************************************************************
// Returns a pointer to the (iIndex)th element of the array, shifts the elements 
// in the array if the location is already full. The iIndex cannot exceed the count 
// of elements in the array.
//*****************************************************************************
void *CStructArray::InsertThrowing(
    int         iIndex)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;
    
    _ASSERTE(iIndex >= 0);
    
    // We can not insert an element further than the end of the array.
    if (iIndex > m_iCount)
        return (NULL);
    
    // The array should grow, if we can't fit one more element into the array.
    Grow(1);

    // The pointer to be returned.
    BYTE *pcList = m_pList + iIndex * m_iElemSize;

    // See if we need to slide anything down.
    if (iIndex < m_iCount)
        memmove(pcList + m_iElemSize, pcList, (m_iCount - iIndex) * m_iElemSize);
    ++m_iCount;
    return(pcList);
}

//*****************************************************************************
// Non-throwing variant
//*****************************************************************************
void *CStructArray::Insert(int iIndex)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    void *result = NULL;
    EX_TRY
    {
        result = InsertThrowing(iIndex);
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);
    
    return result;
}


//*****************************************************************************
// Allocate a new element at the end of the dynamic array and return a pointer
// to it.
//*****************************************************************************
void *CStructArray::AppendThrowing()
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;
    
    // The array should grow, if we can't fit one more element into the array.
    Grow(1);

    return (m_pList + m_iCount++ * m_iElemSize);
}


//*****************************************************************************
// Non-throwing variant
//*****************************************************************************
void *CStructArray::Append()
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    void *result = NULL;
    EX_TRY
    {
        result = AppendThrowing();
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    return result;
}


//*****************************************************************************
// Allocate enough memory to have at least iCount items.  This is a one shot
// check for a block of items, instead of requiring singleton inserts.  It also
// avoids realloc headaches caused by growth, since you can do the whole block
// in one piece of code.  If the array is already large enough, this is a no-op.
//*****************************************************************************
void CStructArray::AllocateBlockThrowing(int iCount)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;
    
    if (m_iSize < m_iCount+iCount)
        Grow(iCount);
    m_iCount += iCount;
}

//*****************************************************************************
// Non-throwing variant
//*****************************************************************************
int CStructArray::AllocateBlock(int iCount)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    int result = FALSE;
    EX_TRY
    {
        AllocateBlockThrowing(iCount);
        result = TRUE;
    }
    EX_CATCH
    {
    }
    EX_END_CATCH(SwallowAllExceptions);

    return result;
}


//*****************************************************************************
// Deletes the specified element from the array.
//*****************************************************************************
void CStructArray::Delete(
    int         iIndex)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    _ASSERTE(iIndex >= 0);

    // See if we need to slide anything down.
    if (iIndex < --m_iCount)
    {
        BYTE *pcList = m_pList + iIndex * m_iElemSize;
        memmove(pcList, pcList + m_iElemSize, (m_iCount - iIndex) * m_iElemSize);
    }
}


//*****************************************************************************
// Grow the array if it is not possible to fit iCount number of new elements.
//*****************************************************************************
void CStructArray::Grow(
    int         iCount)
{
    CONTRACTL {
        THROWS;
    } CONTRACTL_END;
    
    BYTE        *pTemp;                 // temporary pointer used in realloc.
    int         iGrow;

    if (m_iSize < m_iCount+iCount)
    {
        if (m_pList == NULL)
        {
            iGrow = max(m_iGrowInc, iCount);

            m_pList = new BYTE[iGrow * m_iElemSize];
            m_iSize = iGrow;
            m_bFree = true;
        }
        else
        {
            // Adjust grow size as a ratio to avoid too many reallocs.
            if (m_iSize / m_iGrowInc >= 3)
            {   // Don't overflow and go negative.
                int newinc = m_iGrowInc * 2;
                if (newinc > m_iGrowInc)
                    m_iGrowInc = newinc;
            }

            iGrow = max(m_iGrowInc, iCount);

            // try to allocate memory for reallocation.
            if (m_bFree)
            {   // We already own memory.
                pTemp = new BYTE[(m_iSize+iGrow) * m_iElemSize];
                memcpy (pTemp, m_pList, m_iSize * m_iElemSize);
                delete [] m_pList;
            }
            else
            {   // We don't own memory; get our own.
                pTemp = new BYTE[(m_iSize+iGrow) * m_iElemSize];
                memcpy(pTemp, m_pList, m_iSize * m_iElemSize);
                m_bFree = true;
            }
            m_pList = pTemp;
            m_iSize += iGrow;
        }
    }
}


//*****************************************************************************
// Free the memory for this item.
//*****************************************************************************
void CStructArray::Clear()
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    // Free the chunk of memory.
    if (m_bFree && m_pList != NULL)
        delete [] m_pList;

    m_pList = NULL;
    m_iSize = 0;
    m_iCount = 0;
}


//*****************************************************************************
// CHistogram

CHistogram::CHistogram(unsigned count)
{
    WRAPPER_CONTRACT;
    Init(count, 1);
}

CHistogram::CHistogram(unsigned count, unsigned bucketSize)
  : m_dwBuckets(count),
    m_dwBucketSize(bucketSize),
    m_iEntries(0)
{
    WRAPPER_CONTRACT;
    Init(count, bucketSize);
}

void CHistogram::Init(unsigned count, unsigned bucketSize)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;
    
    m_dwBuckets = count;
    m_dwBucketSize = bucketSize;
    m_pBuckets = new unsigned[count + 1];
    memset(m_pBuckets, 0, (count + 1) * sizeof(m_pBuckets[0]));
    
    m_iEntries = 0;
}
CHistogram::~CHistogram()
{
    WRAPPER_CONTRACT;
    
    delete [] m_pBuckets;
}

void CHistogram::RecordEntry(unsigned val)
{
    LEAF_CONTRACT;
    
    m_iEntries++;
    
    unsigned index = val / m_dwBucketSize;
    if (index >= m_dwBuckets)
        index = m_dwBuckets;
        
    m_pBuckets[index]++;
}

void CHistogram::Print(bool fShowMinMax, bool fShowEmptyBuckets)
{
    LEAF_CONTRACT;
    
    printf("Total number of entries : %d\n", m_iEntries);
    
    unsigned minIndex = 0, maxIndex = 0;
    
    for (unsigned i = 0; i <= m_dwBuckets; i++)
    {
        unsigned count = m_pBuckets[i];

        if (count > m_pBuckets[maxIndex])
            maxIndex = i;
        if (count < m_pBuckets[minIndex])
            minIndex = i;
        
        if (!fShowEmptyBuckets && count == 0)
            continue;
        
        printf("%02d - %02d : %d\n",
               i * m_dwBucketSize, 
               (i == m_dwBuckets) ? 0 : (i + 1) * m_dwBucketSize,
               count);
    }

    if (fShowMinMax)
    {
        printf("Min at %02d-%02d : %d\n",
               minIndex * m_dwBucketSize, 
               (minIndex == m_dwBuckets) ? 0 : (minIndex + 1) * m_dwBucketSize,
               m_pBuckets[minIndex]);
        printf("Max at %02d-%02d : %d\n",
               maxIndex * m_dwBucketSize, 
               (maxIndex == m_dwBuckets) ? 0 : (maxIndex + 1) * m_dwBucketSize,
               m_pBuckets[maxIndex]);
    }
}

//*****************************************************************************
// Convert a string of hex digits into a hex value of the specified # of bytes.
//*****************************************************************************
HRESULT GetHex(                         // Return status.
    LPCSTR      szStr,                  // String to convert.
    int         size,                   // # of bytes in pResult.
    void        *pResult)               // Buffer for result.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    int         count = size * 2;       // # of bytes to take from string.
    unsigned long Result = 0;           // Result value.
    char          ch;

    _ASSERTE(size == 1 || size == 2 || size == 4);

    while (count-- && (ch = *szStr++) != '\0')
    {
        switch (ch)
        {
            case '0': case '1': case '2': case '3': case '4': 
            case '5': case '6': case '7': case '8': case '9': 
            Result = 16 * Result + (ch - '0');
            break;

            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            Result = 16 * Result + 10 + (ch - 'A');
            break;

            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            Result = 16 * Result + 10 + (ch - 'a');
            break;

            default:
            return (E_FAIL);
        }
    }

    // Set the output.
    switch (size)
    {
        case 1:
        *((BYTE *) pResult) = (BYTE) Result;
        break;

        case 2:
        *((WORD *) pResult) = (WORD) Result;
        break;

        case 4:
        *((DWORD *) pResult) = Result;
        break;

        default:
        _ASSERTE(0);
        break;
    }
    return (S_OK);
}

//*****************************************************************************
// Convert hex value into a wide string of hex digits 
//*****************************************************************************
HRESULT GetStr (DWORD hHexNum, __out_ecount(cbHexNum * 2) LPWSTR szHexNum, DWORD cbHexNum)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    _ASSERTE (szHexNum);
    cbHexNum *= 2; // each nibble is a char
    while (cbHexNum)
    {
        DWORD thisHexDigit = hHexNum % 16;
        hHexNum /= 16;
        cbHexNum--;
        if (thisHexDigit < 10)
            *(szHexNum+cbHexNum) = (BYTE)(thisHexDigit + L'0');
        else
            *(szHexNum+cbHexNum) = (BYTE)(thisHexDigit - 10 + L'A');
    }
    return S_OK;
}

//*****************************************************************************
// Convert a GUID into a pointer to a Wide char string
//*****************************************************************************
int GuidToLPWSTR(                  // Return status.
    GUID        Guid,                  // The GUID to convert.
    __out_ecount(cchGuid) LPWSTR      szGuid,                // String into which the GUID is stored
    DWORD       cchGuid)                // Count in wchars
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    int         i;
    
    // successive fields break the GUID into the form DWORD-WORD-WORD-WORD-WORD.DWORD 
    // covering the 128-bit GUID. The string includes enclosing braces, which are an OLE convention.

    if (cchGuid < 39) // 38 chars + 1 null terminating.
        return 0;

    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
    // ^
    szGuid[0]  = L'{';

    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
    //  ^^^^^^^^
    if (FAILED (GetStr(Guid.Data1, szGuid+1 , 4))) return 0;

    szGuid[9]  = L'-';
    
    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
    //           ^^^^
    if (FAILED (GetStr(Guid.Data2, szGuid+10, 2))) return 0;

    szGuid[14] = L'-';
    
    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
    //                ^^^^
    if (FAILED (GetStr(Guid.Data3, szGuid+15, 2))) return 0;

    szGuid[19] = L'-';
    
    // Get the last two fields (which are byte arrays).
    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
    //                     ^^^^
    for (i=0; i < 2; ++i)
        if (FAILED(GetStr(Guid.Data4[i], szGuid + 20 + (i * 2), 1)))
            return (0);

    szGuid[24] = L'-';
    
    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
    //                          ^^^^^^^^^^^^
    for (i=0; i < 6; ++i)
        if (FAILED(GetStr(Guid.Data4[i+2], szGuid + 25 + (i * 2), 1)))
            return (0);

    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
    //                                      ^
    szGuid[37] = L'}';
    szGuid[38] = L'\0';

    // Removing this check for VSWhidbey pri0 449695.
    // The code below requires ole32 static linking, but we can't have this in
    // mscoree.dll. 

    return 39;
}

//*****************************************************************************
// Convert a pointer to a string into a GUID.
//*****************************************************************************
HRESULT LPCSTRToGuid(                   // Return status.
    LPCSTR      szGuid,                 // String to convert.
    GUID        *psGuid)                // Buffer for converted GUID.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    int         i;

    // Verify the surrounding syntax.
    if (strlen(szGuid) != 38 || szGuid[0] != '{' || szGuid[9] != '-' ||
        szGuid[14] != '-' || szGuid[19] != '-' || szGuid[24] != '-' || szGuid[37] != '}')
        return (E_FAIL);

    // Parse the first 3 fields.
    if (FAILED(GetHex(szGuid+1, 4, &psGuid->Data1))) return (E_FAIL);
    if (FAILED(GetHex(szGuid+10, 2, &psGuid->Data2))) return (E_FAIL);
    if (FAILED(GetHex(szGuid+15, 2, &psGuid->Data3))) return (E_FAIL);

    // Get the last two fields (which are byte arrays).
    for (i=0; i < 2; ++i)
        if (FAILED(GetHex(szGuid + 20 + (i * 2), 1, &psGuid->Data4[i])))
            return (E_FAIL);
    for (i=0; i < 6; ++i)
        if (FAILED(GetHex(szGuid + 25 + (i * 2), 1, &psGuid->Data4[i+2])))
            return (E_FAIL);
    return (S_OK);
}

//*****************************************************************************
// Parse a string that is a list of strings separated by the specified
// character.  This eliminates both leading and trailing whitespace.  Two
// important notes: This modifies the supplied buffer and changes the szEnv
// parameter to point to the location to start searching for the next token.
// This also skips empty tokens (e.g. two adjacent separators).  szEnv will be
// set to NULL when no tokens remain.  NULL may also be returned if no tokens
// exist in the string.
//*****************************************************************************
char *StrTok(                           // Returned token.
    __deref_opt_inout_opt char *&szEnv, // Location to start searching.
    char        ch)                     // Separator character.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    char        *tok;                   // Returned token.
    char        *next;                  // Used to find separator.

    do
    {
        // Handle the case that we have thrown away a bunch of white space.
        if (szEnv == NULL) return(NULL);

        // Skip leading whitespace.
        while (*szEnv == ' ' || *szEnv == '\t') ++szEnv;

        // Parse the next component.
        tok = szEnv;
        if ((next = strchr(szEnv, ch)) == NULL)
            szEnv = NULL;
        else
        {
            szEnv = next+1;

            // Eliminate trailing white space.
            while (--next >= tok && (*next == ' ' || *next == '\t'));
            *++next = '\0';
        }
    }
    while (*tok == '\0');
    return (tok);
}


//
//
// Global utility functions.
//
//



#ifdef _DEBUG
// Always write regardless of registry.
int _cdecl DbgWriteEx(LPCTSTR szFmt, ...)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    static WCHAR rcBuff[1024];
    va_list     marker;

    va_start(marker, szFmt);
    _vsnwprintf_s(rcBuff, _countof(rcBuff), _TRUNCATE, szFmt, marker);
    va_end(marker);
    WszOutputDebugString(rcBuff);
    return (lstrlenW(rcBuff));
}
#endif


// Writes a wide, formatted string to the standard output.

int _cdecl PrintfStdOut(LPCWSTR szFmt, ...)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    WCHAR rcBuff[1024];
    CHAR  sMbsBuff[1024 * sizeof(WCHAR)];
    va_list     marker;
    DWORD       cWritten;
    int         cChars;
    

    va_start(marker, szFmt);
    _vsnwprintf_s(rcBuff, _countof(rcBuff), _TRUNCATE, szFmt, marker);

    va_end(marker);
    cChars = lstrlenW(rcBuff);
    int cBytes = WszWideCharToMultiByte(CP_ACP, 0, rcBuff, -1, sMbsBuff, sizeof(sMbsBuff)-1, NULL, NULL);
    
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), sMbsBuff, cBytes-1, &cWritten, NULL);
    
    return (cChars);
}


#if USE_UPPER_ADDRESS
static const BYTE * s_nextCodeStartAllocHint = (const BYTE *) CODEHEAP_START_ADDRESS;
#endif

//
// Use this function to reset the s_nextCodeStartAllocHint
// after unloading an AppDomain
//
void ResetNextCodeStartAllocHint()
{
#if USE_UPPER_ADDRESS
    s_nextCodeStartAllocHint = (const BYTE *) CODEHEAP_START_ADDRESS;
#endif
}
    
//
// Use this function to setup pMinAddr and pMaxAddr  
// when you need to do a ClrVirtualAllocWithinRange and
// the range just needs to be in the executable code area,
// but is not otherwise constrained.
//
void InitArgsForAllocInExecutableRange(const BYTE *&pMinAddr,
                                       const BYTE *&pMaxAddr)
{
    // Default values 
    pMinAddr = (BYTE *) BOT_MEMORY;
    pMaxAddr = (BYTE *) TOP_MEMORY;

#if USE_UPPER_ADDRESS
    //
    // If we are using the UPPER_ADDRESS space (on Win64)
    // then for any code heap that doesn't specify an address
    // range using [pMinAddr..pMaxAddr] we place it in the
    // upper address space
    // This enables us to avoid having to use long JumpStubs
    // to reach the code for our ngen-ed images.
    // Which are also placed in the UPPER_ADDRESS space.
    // If GetForceRelocs is enabled we don't constrain the pMinAddr
    //
    
#if defined(_DEBUG)
    // If GetForceRelocs is enabled we don't constrain the pMinAddr
    if (!PEDecoder::GetForceRelocs())
#endif
    {
        pMinAddr = (const BYTE *) CLR_UPPER_ADDRESS_MIN;
        pMaxAddr = (const BYTE *) CLR_UPPER_ADDRESS_MAX;
    }
#endif
}

//
// Returns true if this range is the standard executable range
// that is returned by  InitArgsForAllocInExecutableRange()
// returns false otherwise
//
bool IsStandardExecutableRange(const BYTE *pMinAddr,
                               const BYTE *pMaxAddr)
{
#if USE_UPPER_ADDRESS
    if ((pMinAddr != BOT_MEMORY) || (pMaxAddr != TOP_MEMORY))
    {
        const BYTE * pMinExe;
        const BYTE * pMaxExe;
        
        InitArgsForAllocInExecutableRange(pMinExe, pMaxExe);
        
        return (pMinAddr == pMinExe) && (pMaxAddr == pMaxExe);
    }
#endif
    return false;
}


//
// Allocate free memory that will be used for executable code
// Handles the special requirements that we have on 64-bit platforms
// where we want the executable memory to be located near mscorwks
//
BYTE * ClrVirtualAllocExecutable(SIZE_T dwSize, 
                                 DWORD flAllocationType,
                                 DWORD flProtect)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;

#if USE_UPPER_ADDRESS
    //
    // If we are using the UPPER_ADDRESS space (on Win64)
    // then for any heap that will contain executable code
    // we will place it in the upper address space
    //
    // This enables us to avoid having to use JumpStubs
    // to reach the code for our ngen-ed images on x64,
    // since they are also placed in the UPPER_ADDRESS space.
    //
    const BYTE * pStart = s_nextCodeStartAllocHint;
    const BYTE * pMinAddr;
    const BYTE * pMaxAddr;
 
    InitArgsForAllocInExecutableRange(pMinAddr, pMaxAddr);

    HRESULT hr = ClrVirtualAllocWithinRange (pStart, pMinAddr, pMaxAddr,
                                             dwSize, 
                                             MEM_RESERVE, PAGE_NOACCESS);
    if (hr == E_OUTOFMEMORY)
    {
        //
        // If we ran out of memory in the UPPER_ADDRESS region we fall back 
        // and allocate executable memory in any region
        //
        return (BYTE *) ClrVirtualAlloc (NULL, dwSize, MEM_RESERVE, PAGE_NOACCESS);
    }
    else
    {
        IfFailThrow(hr);
        return (BYTE *) pStart;
    }
#else
    return (BYTE *) ClrVirtualAlloc (NULL, dwSize, MEM_RESERVE, PAGE_NOACCESS);
#endif
}

// Allocate free memory within the range [pMinAddr..pMaxAddr] using
// ClrVirtualQuery to find free memory and ClrVirtualAlloc to allocate it.
//
// If pStart in non-NULL we start there and work up to pMaxAddr and failing 
// that we start a second scan at pMinAddr and work upwards back to pStart.
//
// If pStart is NULL we start at pMinAddr and work upwards to pMaxAddr.
// The result is returned in pStart (it is a reference parameter)
//
// It is recommended that callers use flAllocationType of MEM_RESERVE
// rather than MEM_COMMIT.  Callers also should have dwSize set to a multiple
// of sysInfo.dwAllocationGranularity (64k).  That way they can reserve
// as large region and commit smaller sized pages from that region until
// it fills up.  
//
// This functions returns S_OK upon success and writes the address of the 
// allocated memory to pStart
// It returns E_OUTOFMEMORY when it fails to allocate memory 
// It returns E_INVALIDARG when the arguments to thsi function are invalid
//
// Note that this function replaces FindFreeSpaceWithinRange
//
HRESULT ClrVirtualAllocWithinRange(const BYTE *&pStart,
                                   const BYTE *pMinAddr,
                                   const BYTE *pMaxAddr,
                                   SIZE_T dwSize, 
                                   DWORD flAllocationType,
                                   DWORD flProtect)
{
    CONTRACTL
    {
        NOTHROW;
        PRECONDITION(dwSize != 0);
        PRECONDITION((flAllocationType == MEM_RESERVE) || (flAllocationType == MEM_COMMIT));
    }
    CONTRACTL_END;

    const BYTE *pResult = NULL;
    //
    // First lets normalize the pMinAddr and pMaxAddr values
    //
    // If pMinAddr is NULL then set it to BOT_MEMORY
    if ((pMinAddr == 0) || (pMinAddr < (BYTE *) BOT_MEMORY))
    {
        pMinAddr = (BYTE *) BOT_MEMORY;
    }

    // If pMaxAddr is NULL then set it to TOP_MEMORY
    if ((pMaxAddr == 0) || (pMaxAddr > (BYTE *) TOP_MEMORY))
    {
        pMaxAddr = (BYTE *) TOP_MEMORY;
    }

    // If pMinAddr is BOT_MEMORY and pMaxAddr is TOP_MEMORY
    // then we can call ClrVirtualAlloc instead 
    if ((pMinAddr == (BYTE *) BOT_MEMORY) && (pMaxAddr == (BYTE *) TOP_MEMORY))
    {
        pResult = (const BYTE*) ClrVirtualAlloc((BYTE *)pStart, dwSize, flAllocationType, flProtect);

        // Success!
        if (pResult != NULL) 
        {
            pStart = pResult;  // write back the result to pStart 
            return S_OK;
        }
        else
        {
            return E_OUTOFMEMORY;
        }
    }

    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);    

    // If we're given 0 (in a free build only), default to sysInfo.dwPageSize
    if (dwSize == 0)
    {
        dwSize = sysInfo.dwPageSize;
    }

    // Align to sysInfo.dwAllocationGranularity (64k). 
    // See docs on VirtualAllocEx for reasons.
    SIZE_T  dwReserveSize = ALIGN_UP(dwSize, sysInfo.dwAllocationGranularity);
    SIZE_T  dwCommitSize  = 0;

    if (flAllocationType == MEM_COMMIT)
    {
        // Align to sysInfo.dwPageSize (4k). 
        dwCommitSize = ALIGN_UP(dwSize, sysInfo.dwPageSize);
    }

    // If pMaxAddr is not greater than pMinAddr we can not make an allocation
    if (pMaxAddr <= pMinAddr)
    {
        pStart = NULL;
        return E_INVALIDARG;
    }

#if USE_UPPER_ADDRESS
    // If pStart is NULL and our s_nextCodeStartAllocHint is 
    // in the range [pMinAddr.. pMaxAddr], the use it as pStart
    if ((pStart == NULL) && 
        (pMinAddr <= s_nextCodeStartAllocHint) && 
        (pMaxAddr >= s_nextCodeStartAllocHint))
    {
        pStart = s_nextCodeStartAllocHint;
    }
#endif

    // If we are given a non-Null pStart value then we will 
    // use it as the starting point for our scan
    //
    if (pStart != NULL)
    {
        // We can't allocate any memory below BOT_MEMORY 
        if (pStart < (BYTE *) BOT_MEMORY)
        {
            pStart = (BYTE *) BOT_MEMORY;
        }
        
        // First make sure that the pStart value is valid
        if ((pStart < pMinAddr) || (pStart > pMaxAddr))
        {
            // the pStart that we were given is outside of [pMinAddr..pMaxAddr]
            return E_INVALIDARG;
        }

        // We will do two scans:
        // First we scan [pStart .. pMaxAddr]
        // Then we scan  [pMinAddr .. pStart]
    }
    else
    {
        // We will do one scan: [pMinAddr .. pMaxAddr]
        pStart = pMinAddr;
    }

    MEMORY_BASIC_INFORMATION mbInfo;
    memset((void *)&mbInfo, 0, sizeof(mbInfo));

    // Setup tryAddr with the value that we computed above for pStart.
    BYTE *tryAddr = (BYTE *)pStart;

    do {
        // Now scan memory and try to find a free block of the size requested.
        while ((tryAddr + dwReserveSize) <= (BYTE *) pMaxAddr)
        {
            // Align to 64k. See docs for VirtualAllocEx and lpAddress and 64k alignment for reasons.
            tryAddr = (BYTE*) ALIGN_UP(tryAddr, sysInfo.dwAllocationGranularity);
            
            // Use VirtualQuery to find out if this address is MEM_FREE
            //
            if (!ClrVirtualQuery((LPCVOID)tryAddr, &mbInfo, sizeof(mbInfo)))
                break;
            
            // Is there enough memory free from this start location?
            // Note with FEATURE_PAL, ClrVirtualQuery returns mbInfo.RegionSize == 0 when the size of the region is unknown
            if ( (mbInfo.State == MEM_FREE)  &&
                 ((mbInfo.RegionSize >= (SIZE_T) dwReserveSize) || (mbInfo.RegionSize == 0)) )
            {
                // Try reserving the memory using VirtualAlloc now
                pResult = (const BYTE*) ClrVirtualAlloc(tryAddr, dwReserveSize, MEM_RESERVE, flProtect);
                
                if (pResult != NULL) 
                {
                    // If the caller wanted us to commit the memory then we can do so now.
                    if (flAllocationType == MEM_COMMIT)
                    {
                        // Try committing the memory using VirtualAlloc now
                        pResult = (const BYTE*) ClrVirtualAlloc(tryAddr, dwCommitSize, MEM_COMMIT, flProtect);
                        if (pResult == NULL)
                        {
                            return E_OUTOFMEMORY;
                        }
                    }

#if USE_UPPER_ADDRESS
                    // If our pMinAddr constraint is the CLR_UPPER_ADDRESS_MIN
                    // the set s_nextCodeStartAllocHint to the next address above 
                    // the allocation that we just made.
                    if (pMinAddr == (const BYTE *) CLR_UPPER_ADDRESS_MIN)
                    {
                        s_nextCodeStartAllocHint = pResult + dwReserveSize;
                    }
#endif

                    pStart = pResult;  // write back the result to pStart 
                    return S_OK;
                }
                
                // We could fail in a race.  Just move on to next region and continue trying
                tryAddr = tryAddr + sysInfo.dwAllocationGranularity;
            }
            else
            {
                // Try another section of memory
                tryAddr = max(tryAddr + sysInfo.dwAllocationGranularity,
                              (BYTE*) mbInfo.BaseAddress + mbInfo.RegionSize);
            }
        }
        // Our tryAddr reached pMaxAddr

        if (pStart == pMinAddr)
        {
            // We have completed our second scan from [pMinAddr .. pStart]
            // or we have completed thefull scan from [pMinAddr .. pMaxAddr]
            break;
        }
        else
        {
            // Setup the second scan from [pMinAddr .. pStart]
            pMaxAddr = pStart;
            pStart   = pMinAddr;
            tryAddr  = (BYTE *) pStart;
        }
    } while(true);       

    // We have finished cycling through the range and we didn't find any free memory
    pStart = NULL;
    return E_OUTOFMEMORY;
}

//******************************************************************************
// Returns the number of processors that a process has been configured to run on
//******************************************************************************
int GetCurrentProcessCpuCount()
{
    CONTRACTL
    {
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACTL_END;
    

    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;

}

/**************************************************************************/
void ConfigMethodSet::init(__in_z LPWSTR keyName)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;
    
    // make sure that the memory was zero initialized
    _ASSERTE(m_inited == 0 || m_inited == 1);

    OnUnicodeSystem();
    LPWSTR str = REGUTIL::GetConfigString(keyName);
    if (str) 
    {
        m_list.Insert(str);
        REGUTIL::FreeConfigString(str);
    }
    m_inited = 1;
}

/**************************************************************************/
bool ConfigMethodSet::contains(LPCUTF8 methodName, LPCUTF8 className, PCCOR_SIGNATURE sig)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    _ASSERTE(m_inited == 1);

    if (m_list.IsEmpty())
        return false;
    return(m_list.IsInList(methodName, className, sig));
}

/**************************************************************************/
void ConfigDWORD::init(__in_z LPWSTR keyName, DWORD defaultVal)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    // make sure that the memory was zero initialized
    _ASSERTE(m_inited == 0 || m_inited == 1);

    OnUnicodeSystem();
    m_value = REGUTIL::GetConfigDWORD(keyName, defaultVal);
    m_inited = 1;
}


/**************************************************************************/
void ConfigString::init(__in_z LPWSTR keyName)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    // make sure that the memory was zero initialized
    _ASSERTE(m_inited == 0 || m_inited == 1);

    OnUnicodeSystem();
    // Note: m_value will be leaking
    m_value = REGUTIL::GetConfigString(keyName);
    m_inited = 1;
}

//=============================================================================
// AssemblyNamesList
//=============================================================================
// The string should be of the form
// MyAssembly
// MyAssembly;mscorlib;System
// MyAssembly;mscorlib System

AssemblyNamesList::AssemblyNamesList(LPWSTR list)
{
    CONTRACTL {
        THROWS;
    } CONTRACTL_END;

    WCHAR prevChar = '?'; // dummy
    LPWSTR nameStart = NULL; // start of the name currently being processed. NULL if no current name
    AssemblyName ** ppPrevLink = &m_pNames;
    
    for (LPWSTR listWalk = list; prevChar != '\0'; prevChar = *listWalk, listWalk++)
    {
        WCHAR curChar = *listWalk;
        
        if (iswspace(curChar) || curChar == ';' || curChar == '\0' )
        {
            //
            // Found white-space
            //
            
            if (nameStart)
            {
                // Found the end of the current name
                
                AssemblyName * newName = new AssemblyName();
                size_t nameLen = listWalk - nameStart;
                
                MAKE_UTF8PTR_FROMWIDE(temp, nameStart);
                newName->m_assemblyName = new char[nameLen + 1];
                memcpy(newName->m_assemblyName, temp, nameLen * sizeof(newName->m_assemblyName[0]));
                newName->m_assemblyName[nameLen] = '\0';

                *ppPrevLink = newName;
                ppPrevLink = &newName->m_next;

                nameStart = NULL;
            }
        }
        else if (!nameStart)
        {
            //
            // Found the start of a new name
            //
            
            nameStart = listWalk;
        }
    }

    _ASSERTE(!nameStart); // cannot be in the middle of a name
    *ppPrevLink = NULL;
}

AssemblyNamesList::~AssemblyNamesList()
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;

    for (AssemblyName * pName = m_pNames; pName; /**/)
    {
        AssemblyName * cur = pName;
        pName = pName->m_next;

        delete [] cur->m_assemblyName;
        delete cur;
    }
}

bool AssemblyNamesList::IsInList(LPCUTF8 assemblyName)
{
    if (IsEmpty())
        return false;
    
    for (AssemblyName * pName = m_pNames; pName; pName = pName->m_next)
    {
        if (strcmp(pName->m_assemblyName, assemblyName) == 0)
            return true;
    }

    return false;
}

//=============================================================================
// MethodNamesList
//=============================================================================
//  str should be of the form :
// "foo1 MyNamespace.MyClass:foo3 *:foo4 foo5(x,y,z)"
// "MyClass:foo2 MyClass:*" will match under _DEBUG
//

void MethodNamesListBase::Insert(__in_z LPWSTR str)
{
    CONTRACTL {
        THROWS;
    } CONTRACTL_END;

    enum State { NO_NAME, CLS_NAME, FUNC_NAME, ARG_LIST }; // parsing state machine

    const char   SEP_CHAR = ' ';     // current character use to separate each entry
//  const char   SEP_CHAR = ';';     // better  character use to separate each entry

    WCHAR lastChar = '?'; // dummy
    LPWSTR nameStart = NULL; // while walking over the classname or methodname, this points to start
    MethodName nameBuf; // Buffer used while parsing the current entry
    MethodName** lastName = &pNames; // last entry inserted into the list
    bool         bQuote   = false;

    for(State state = NO_NAME; lastChar != '\0'; str++)
    {
        lastChar = *str;

        switch(state)
        {
        case NO_NAME:
            if (*str != SEP_CHAR)
            {
                nameStart = str;
                state = CLS_NAME; // we have found the start of the next entry
            }
            break;

        case CLS_NAME:
            if (*nameStart == '"')
            {
                while (*str && *str!='"')
                {
                    str++;
                }
                nameStart++;
                bQuote=true;
            }

            if (*str == ':')
            {
                if (*nameStart == '*' && !bQuote)
                {
                    // Is the classname string a wildcard. Then set it to NULL
                    nameBuf.className = NULL;
                }
                else
                {
                    int len = (int)(str - nameStart);

                    // Take off the quote
                    if (bQuote) { len--; bQuote=false; }
                    
                    nameBuf.className = new char[len + 1];
                    MAKE_UTF8PTR_FROMWIDE(temp, nameStart);
                    memcpy(nameBuf.className, temp, len*sizeof(nameBuf.className[0]));
                    nameBuf.className[len] = '\0';
                }
                if (str[1] == ':')      // Accept class::name syntax too
                    str++;
                nameStart = str + 1;
                state = FUNC_NAME;
            }
            else if (*str == '\0' || *str == SEP_CHAR || *str == '(')
            {
                /* This was actually a method name without any class */
                nameBuf.className = NULL;
                goto DONE_FUNC_NAME;
            }
            break;

        case FUNC_NAME:
            if (*nameStart == '"')
            {
                while ( (nameStart==str)    ||
                        (*str && *str!='"'))
                {
                    str++;
                }
                       
                nameStart++;
                bQuote=true;
            }

            if (*str == '\0' || *str == SEP_CHAR || *str == '(')
            {
            DONE_FUNC_NAME:
                _ASSERTE(*str == '\0' || *str == SEP_CHAR || *str == '(');

                if (*nameStart == '*' && !bQuote)
                {
                    // Is the name string a wildcard. Then set it to NULL
                    nameBuf.methodName = NULL;
                }
                else
                {
                    int len = (int)(str - nameStart);

                    // Take off the quote
                    if (bQuote) { len--; bQuote=false; }

                    nameBuf.methodName = new char[len + 1];
                    MAKE_UTF8PTR_FROMWIDE(temp, nameStart);
                    memcpy(nameBuf.methodName, temp, len*sizeof(nameBuf.methodName[0]));
                    nameBuf.methodName[len] = '\0';
                }

                if (*str == '\0' || *str == SEP_CHAR)
                {
                    nameBuf.numArgs = -1;
                    goto DONE_ARG_LIST;
                }
                else
                {
                    _ASSERTE(*str == '(');
                    nameBuf.numArgs = -1;
                    state = ARG_LIST;
                }
            }
            break;

        case ARG_LIST:
            if (*str == '\0' || *str == ')')
            {
                if (nameBuf.numArgs == -1)
                    nameBuf.numArgs = 0;

            DONE_ARG_LIST:
                _ASSERTE(*str == '\0' || *str == SEP_CHAR || *str == ')');

                // We have parsed an entire method name.
                // Create a new entry in the list for it

                MethodName * newName = new MethodName();
                *newName = nameBuf;
                newName->next = NULL;
                *lastName = newName;
                lastName = &newName->next;
                state = NO_NAME;
            }
            else
            {
                if (*str != SEP_CHAR && nameBuf.numArgs == -1)
                    nameBuf.numArgs = 1;
                if (*str == ',')
                    nameBuf.numArgs++;
            }
            break;

        default: _ASSERTE(!"Bad state"); break;
        }
    }
}

/**************************************************************/

void MethodNamesListBase::Destroy() 
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    for(MethodName * pName = pNames; pName; /**/)
    {
        if (pName->className)
            delete [] pName->className;
        if (pName->methodName)
            delete [] pName->methodName;

        MethodName * curName = pName;
        pName = pName->next;
        delete curName;
    }
}

/**************************************************************/
bool MethodNamesListBase::IsInList(LPCUTF8 methName, LPCUTF8 clsName, PCCOR_SIGNATURE sig) 
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    sig++;      // Skip calling convention
    int numArgs = CorSigUncompressData(sig);    

    // Try to match all the entries in the list

    for(MethodName * pName = pNames; pName; pName = pName->next)
    {
        // If numArgs is valid, check for mismatch
        if (pName->numArgs != -1 && pName->numArgs != numArgs)
            continue;

        // If methodName is valid, check for mismatch
        if (pName->methodName) {
            if (strcmp(pName->methodName, methName) != 0) {

                const char* ptr = strchr(methName, ':');
                if (ptr != 0 && ptr[1] == ':' && strcmp(&ptr[2], pName->methodName) == 0) {
                    unsigned clsLen = (unsigned)(ptr - methName);
                    if (pName->className == 0 || strncmp(pName->className, methName, clsLen) == 0)
                        return true;
                }
                continue;
            }
        }


        // check for class Name exact match
        if (pName->className == 0 || strcmp(pName->className, clsName) == 0)
            return true;

        // check for suffix wildcard like System.*
        unsigned len = (unsigned)strlen(pName->className);
        if (len > 0 && pName->className[len-1] == '*' && strncmp(pName->className, clsName, len-1) == 0)
            return true;

#ifdef _DEBUG
            // Maybe className doesnt include namespace. Try to match that
        LPCUTF8 onlyClass = ns::FindSep(clsName);
        if (onlyClass && strcmp(pName->className, onlyClass+1) == 0)
            return true;
#endif
    }
    return(false);
}

//=============================================================================
// Signature Validation Functions (scaled down version from MDValidator
//=============================================================================

//*****************************************************************************
// This function validates that the given Method signature is consistent as per
// the compression scheme.
//*****************************************************************************
HRESULT validateSigCompression(
    mdToken     tk,                     // [IN] Token whose signature needs to be validated.
    PCCOR_SIGNATURE pbSig,              // [IN] Signature.
    ULONG       cbSig)                  // [IN] Size in bytes of the signature.
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    ULONG       ulCurByte = 0;          // Current index into the signature.
    ULONG       ulSize;                 // Size of uncompressed data at each point.

    // Check for NULL signature.
    if (!pbSig || !cbSig) return VLDTR_E_SIGNULL;

    // Walk through the signature.  At each point make sure there is enough
    // room left in the signature based on the encoding in the current byte.
    while (cbSig - ulCurByte)
    {
        _ASSERTE(ulCurByte <= cbSig);
        // Get next chunk of uncompressed data size.
        if ((ulSize = CorSigUncompressedDataSize(pbSig)) > (cbSig - ulCurByte)) return VLDTR_E_SIGNODATA;
        // Go past this chunk.
        ulCurByte += ulSize;
        CorSigUncompressData(pbSig);
    }
    return S_OK;
}   // validateSigCompression()

//*****************************************************************************
// This function validates one argument given an offset into the signature
// where the argument begins.  This function assumes that the signature is well
// formed as far as the compression scheme is concerned.
//*****************************************************************************
HRESULT validateOneArg(
    mdToken     tk,                     // [IN] Token whose signature needs to be validated.
    PCCOR_SIGNATURE &pbSig,             // [IN] Pointer to the beginning of argument.
    ULONG       cbSig,                  // [IN] Size in bytes of the full signature.
    ULONG       *pulCurByte,            // [IN/OUT] Current offset into the signature..
    ULONG       *pulNSentinels,         // [IN/OUT] Number of sentinels
    IMDInternalImport*  pImport,        // [IN] Internal MD Import interface ptr
    BOOL        bNoVoidAllowed)         // [IN] Flag indicating whether "void" is disallowed for this arg

{
    CONTRACTL
    {
        NOTHROW;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    ULONG       ulElementType;          // Current element type being processed.
    ULONG       ulElemSize;             // Size of the element type.
    mdToken     token;                  // Embedded token.
    ULONG       ulArgCnt;               // Argument count for function pointer.
    ULONG       ulIndex;                // Index for type parameters
    ULONG       ulRank;                 // Rank of the array.
    ULONG       ulSizes;                // Count of sized dimensions of the array.
    ULONG       ulLbnds;                // Count of lower bounds of the array.
    ULONG       ulTkSize;               // Token size.
    ULONG       ulCallConv;

    HRESULT     hr = S_OK;              // Value returned.
    BOOL        bRepeat = TRUE;         // MODOPT and MODREQ belong to the arg after them

    _ASSERTE (pulCurByte);

    BEGIN_SO_INTOLERANT_CODE_NO_THROW_CHECK_THREAD(return COR_E_STACKOVERFLOW);
    while(bRepeat)
    {
        bRepeat = FALSE;
        // Validate that the argument is not missing.
        _ASSERTE(*pulCurByte <= cbSig);
        if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSARG);

        // Get the element type.
        *pulCurByte += (ulElemSize = CorSigUncompressedDataSize(pbSig));
        ulElementType = CorSigUncompressData(pbSig);

        // Walk past all the modifier types.
        while (ulElementType & ELEMENT_TYPE_MODIFIER)
        {
            _ASSERTE(*pulCurByte <= cbSig);
            if(ulElementType == ELEMENT_TYPE_SENTINEL)
            {
                if(pulNSentinels) *pulNSentinels+=1;
                if(TypeFromToken(tk) != mdtMemberRef) IfFailGo(VLDTR_E_SIG_SENTINMETHODDEF);
                if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_LASTSENTINEL);
            }
            if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSELTYPE);
            *pulCurByte += (ulElemSize = CorSigUncompressedDataSize(pbSig));
            ulElementType = CorSigUncompressData(pbSig);
        }

        switch (ulElementType)
        {
            case ELEMENT_TYPE_VOID:
                if(bNoVoidAllowed) IfFailGo(VLDTR_E_SIG_BADVOID);

            case ELEMENT_TYPE_BOOLEAN:
            case ELEMENT_TYPE_CHAR:
            case ELEMENT_TYPE_I1:
            case ELEMENT_TYPE_U1:
            case ELEMENT_TYPE_I2:
            case ELEMENT_TYPE_U2:
            case ELEMENT_TYPE_I4:
            case ELEMENT_TYPE_U4:
            case ELEMENT_TYPE_I8:
            case ELEMENT_TYPE_U8:
            case ELEMENT_TYPE_R4:
            case ELEMENT_TYPE_R8:
            case ELEMENT_TYPE_STRING:
            case ELEMENT_TYPE_OBJECT:
            case ELEMENT_TYPE_TYPEDBYREF:
            case ELEMENT_TYPE_U:
            case ELEMENT_TYPE_I:
                break;
            case ELEMENT_TYPE_PTR:
                // Validate the referenced type.
                if(FAILED(hr = validateOneArg(tk, pbSig, cbSig, pulCurByte,pulNSentinels,pImport,FALSE))) IfFailGo(hr);
                break;
            case ELEMENT_TYPE_BYREF:  //fallthru
                if(TypeFromToken(tk)==mdtFieldDef) IfFailGo(VLDTR_E_SIG_BYREFINFIELD);
            case ELEMENT_TYPE_PINNED:
            case ELEMENT_TYPE_SZARRAY:
                // Validate the referenced type.
                if(FAILED(hr = validateOneArg(tk, pbSig, cbSig, pulCurByte,pulNSentinels,pImport,TRUE))) IfFailGo(hr);
                break;
            case ELEMENT_TYPE_CMOD_OPT:
            case ELEMENT_TYPE_CMOD_REQD:
                bRepeat = TRUE; // go on validating, we're not done with this arg
            case ELEMENT_TYPE_VALUETYPE: //fallthru
            case ELEMENT_TYPE_CLASS:
                // See if the token is missing.
                _ASSERTE(*pulCurByte <= cbSig);
                if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSTKN);
                // See if the token is a valid token.
                ulTkSize = CorSigUncompressedDataSize(pbSig);
                token = CorSigUncompressToken(pbSig);
                // Token validation .
                if(pImport)
                {
                    ULONG   rid = RidFromToken(token);
                    ULONG   typ = TypeFromToken(token);
                    ULONG   maxrid = pImport->GetCountWithTokenKind(typ);
                    if(typ == mdtTypeDef) maxrid++;
                    if((rid==0)||(rid > maxrid)) IfFailGo(VLDTR_E_SIG_TKNBAD);
                }
                *pulCurByte += ulTkSize;
                break;

            case ELEMENT_TYPE_FNPTR: 
                // Validate that calling convention is present.
                _ASSERTE(*pulCurByte <= cbSig);
                if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSFPTR);
                // Consume calling convention.
                *pulCurByte += CorSigUncompressedDataSize(pbSig);
                ulCallConv = CorSigUncompressData(pbSig);
                if(((ulCallConv & IMAGE_CEE_CS_CALLCONV_MASK) >= IMAGE_CEE_CS_CALLCONV_MAX) 
                    ||((ulCallConv & IMAGE_CEE_CS_CALLCONV_EXPLICITTHIS)
                    &&(!(ulCallConv & IMAGE_CEE_CS_CALLCONV_HASTHIS)))) IfFailGo(VLDTR_E_MD_BADCALLINGCONV);

                // Validate that argument count is present.
                _ASSERTE(*pulCurByte <= cbSig);
                if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSFPTRARGCNT);
                // Consume argument count.
                *pulCurByte += CorSigUncompressedDataSize(pbSig);
                ulArgCnt = CorSigUncompressData(pbSig);

                // FNPTR signature must follow the rules of MethodDef
                // Validate and consume return type.
                IfFailGo(validateOneArg(mdtMethodDef, pbSig, cbSig, pulCurByte,NULL,pImport,FALSE));

                // Validate and consume the arguments.
                while(ulArgCnt--)
                {
                    IfFailGo(validateOneArg(mdtMethodDef, pbSig, cbSig, pulCurByte,NULL,pImport,TRUE));
                }
                break;

            case ELEMENT_TYPE_ARRAY:
                // Validate and consume the base type.
                IfFailGo(validateOneArg(tk, pbSig, cbSig, pulCurByte,pulNSentinels,pImport,TRUE));

                // Validate that the rank is present.
                _ASSERTE(*pulCurByte <= cbSig);
                if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSRANK);
                // Consume the rank.
                *pulCurByte += CorSigUncompressedDataSize(pbSig);
                ulRank = CorSigUncompressData(pbSig);

                // Process the sizes.
                if (ulRank)
                {
                    // Validate that the count of sized-dimensions is specified.
                    _ASSERTE(*pulCurByte <= cbSig);
                    if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSNSIZE);
                    // Consume the count of sized dimensions.
                    *pulCurByte += CorSigUncompressedDataSize(pbSig);
                    ulSizes = CorSigUncompressData(pbSig);

                    // Loop over the sizes.
                    while(ulSizes--)
                    {
                        // Validate the current size.
                        _ASSERTE(*pulCurByte <= cbSig);
                        if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSSIZE);
                        // Consume the current size.
                        *pulCurByte += CorSigUncompressedDataSize(pbSig);
                        CorSigUncompressData(pbSig);
                    }

                    // Validate that the count of lower bounds is specified.
                    _ASSERTE(*pulCurByte <= cbSig);
                    if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSNLBND);
                    // Consume the count of lower bound.
                    *pulCurByte += CorSigUncompressedDataSize(pbSig);
                    ulLbnds = CorSigUncompressData(pbSig);

                    // Loop over the lower bounds.
                    while(ulLbnds--)
                    {
                        // Validate the current lower bound.
                        _ASSERTE(*pulCurByte <= cbSig);
                        if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSLBND);
                        // Consume the current size.
                        *pulCurByte += CorSigUncompressedDataSize(pbSig);
                        CorSigUncompressData(pbSig);
                    }
                }
                break;
                case ELEMENT_TYPE_VAR:
                case ELEMENT_TYPE_MVAR:
                    // Validate that index is present.
                    _ASSERTE(*pulCurByte <= cbSig);
                    if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSFPTRARGCNT);
        
                    // Consume index.
                    *pulCurByte += CorSigUncompressedDataSize(pbSig);
                    ulIndex = CorSigUncompressData(pbSig);

                    break;

                case ELEMENT_TYPE_GENERICINST:
                    // Validate the generic type.
                    IfFailGo(validateOneArg(tk, pbSig, cbSig, pulCurByte,pulNSentinels,pImport,TRUE));

                    // Validate that parameter count is present.
                    _ASSERTE(*pulCurByte <= cbSig);
                    if (cbSig == *pulCurByte) IfFailGo(VLDTR_E_SIG_MISSFPTRARGCNT);
                    // Consume parameter count.
                    *pulCurByte += CorSigUncompressedDataSize(pbSig);
                    ulArgCnt = CorSigUncompressData(pbSig);


                    while(ulArgCnt--)
                    {
                        IfFailGo(validateOneArg(tk, pbSig, cbSig, pulCurByte,NULL,pImport,TRUE));
                    }
                    break;

            case ELEMENT_TYPE_SENTINEL: // this case never works because all modifiers are skipped before switch
                if(TypeFromToken(tk) == mdtMethodDef) IfFailGo(VLDTR_E_SIG_SENTINMETHODDEF);
                break;

            default:
                IfFailGo(VLDTR_E_SIG_BADELTYPE);
                break;
        }   // switch (ulElementType)
    } // end while(bRepeat)
ErrExit:

    END_SO_INTOLERANT_CODE;
    return hr;
}

HRESULT validateTokenSig(
    mdToken             tk,                     // [IN] Token whose signature needs to be validated.
    PCCOR_SIGNATURE     pbSig,                  // [IN] Signature.
    ULONG               cbSig,                  // [IN] Size in bytes of the signature.
    DWORD               dwFlags,                // [IN] Method flags.
    IMDInternalImport*  pImport)               // [IN] Internal MD Import interface ptr
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    ULONG       ulCurByte = 0;          // Current index into the signature.
    ULONG       ulCallConv;             // Calling convention.
    ULONG       ulArgCount = 1;         // Count of arguments (1 because of the return type)
    ULONG       ulTyArgCount = 0;         // Count of type arguments
    ULONG       ulArgIx = 0;            // Starting index of argument (standalone sig: 1)
    ULONG       i;                      // Looping index.
    HRESULT     hr = S_OK;              // Value returned.
    ULONG       ulNSentinels = 0;

    _ASSERTE(TypeFromToken(tk) == mdtMethodDef ||
             TypeFromToken(tk) == mdtMemberRef ||
             TypeFromToken(tk) == mdtSignature ||
             TypeFromToken(tk) == mdtFieldDef);

    // Validate the signature is well-formed with respect to the compression
    // scheme.  If this fails, no further validation needs to be done.
    if ( FAILED(hr = validateSigCompression(tk, pbSig, cbSig))) return hr;

    // Validate the calling convention.
    ulCurByte += CorSigUncompressedDataSize(pbSig);
    ulCallConv = CorSigUncompressData(pbSig);
    i = ulCallConv & IMAGE_CEE_CS_CALLCONV_MASK;
    switch(TypeFromToken(tk))
    {
        case mdtMethodDef: // MemberRefs have no flags available
            // If HASTHIS is set on the calling convention, the method should not be static.
            if ((ulCallConv & IMAGE_CEE_CS_CALLCONV_HASTHIS) &&
                IsMdStatic(dwFlags)) return VLDTR_E_MD_THISSTATIC;

            // If HASTHIS is not set on the calling convention, the method should be static.
            if (!(ulCallConv & IMAGE_CEE_CS_CALLCONV_HASTHIS) &&
                !IsMdStatic(dwFlags)) return VLDTR_E_MD_NOTTHISNOTSTATIC;
            // fall thru to callconv check;

        case mdtMemberRef:
            if(i == IMAGE_CEE_CS_CALLCONV_FIELD) return validateOneArg(tk, pbSig, cbSig, &ulCurByte,NULL,pImport,TRUE);

            // EXPLICITTHIS and native call convs are for stand-alone sigs only (for calli)
            if((i != IMAGE_CEE_CS_CALLCONV_DEFAULT)&&( i != IMAGE_CEE_CS_CALLCONV_VARARG)
                || (ulCallConv & IMAGE_CEE_CS_CALLCONV_EXPLICITTHIS)) return VLDTR_E_MD_BADCALLINGCONV;
            break;

        case mdtSignature:
            if(i != IMAGE_CEE_CS_CALLCONV_LOCAL_SIG) // then it is function sig for calli
            {
                if((i >= IMAGE_CEE_CS_CALLCONV_MAX) 
                    ||((ulCallConv & IMAGE_CEE_CS_CALLCONV_EXPLICITTHIS)
                    &&(!(ulCallConv & IMAGE_CEE_CS_CALLCONV_HASTHIS)))) return VLDTR_E_MD_BADCALLINGCONV;
            }
            else
                ulArgIx = 1;        // Local variable signatures don't have a return type 
            break;

        case mdtFieldDef:
            if(i != IMAGE_CEE_CS_CALLCONV_FIELD) return VLDTR_E_MD_BADCALLINGCONV;
            return validateOneArg(tk, pbSig, cbSig, &ulCurByte,NULL,pImport,TRUE);
    }
    // Is there any sig left for arguments?
    _ASSERTE(ulCurByte <= cbSig);
    if (cbSig == ulCurByte) return VLDTR_E_MD_NOARGCNT;

    // Get the type argument count
    if (ulCallConv & IMAGE_CEE_CS_CALLCONV_GENERIC)
    {
      ulCurByte += CorSigUncompressedDataSize(pbSig);
      ulTyArgCount += CorSigUncompressData(pbSig);
    }

    // Get the argument count.
    ulCurByte += CorSigUncompressedDataSize(pbSig);
    ulArgCount += CorSigUncompressData(pbSig);

    // Validate the return type and the arguments.
    // (at this moment ulArgCount = num.args+1, ulArgIx = (standalone sig. ? 1 :0); )
    for(; ulArgIx < ulArgCount; ulArgIx++)
    {
        if(FAILED(hr = validateOneArg(tk, pbSig, cbSig, &ulCurByte,&ulNSentinels,pImport, (ulArgIx!=0)))) return hr;
    }
    

    if((ulNSentinels != 0) && ((ulCallConv & IMAGE_CEE_CS_CALLCONV_MASK) != IMAGE_CEE_CS_CALLCONV_VARARG ))
        return VLDTR_E_SIG_SENTMUSTVARARG;
    if(ulNSentinels > 1) return VLDTR_E_SIG_MULTSENTINELS;
    return S_OK;
}   // validateTokenSig()


CHAR g_VersionBase[] = "v1.";
CHAR g_DevelopmentVersion[] = "x86";
CHAR g_RetString[] = "retail";
CHAR g_ComplusString[] = "COMPLUS";

WCHAR g_VersionBaseW[] = L"v1.";
WCHAR g_DevelopmentVersionW[] = L"x86";
WCHAR g_RetStringW[] = L"retail";
WCHAR g_ComplusStringW[] = L"COMPLUS";

//*****************************************************************************
// Determine the version number of the runtime that was used to build the
// specified image. The pMetadata pointer passed in is the pointer to the
// metadata contained in the image.
//*****************************************************************************
static BOOL IsReallyRTM(LPCWSTR szVersion)
{
    LEAF_CONTRACT;
    if (szVersion==NULL)
        return FALSE;


    size_t lgth = sizeof(g_VersionBaseW) / sizeof(WCHAR) - 1;
    size_t foundLgth = wcslen(szVersion);

    // Have normal version, v1.*
    if ( (foundLgth >= lgth+2) &&
         !wcsncmp(szVersion, g_VersionBaseW, lgth) ) {

        // v1.0.* means RTM
        if (szVersion[lgth+1] == L'.') {
            if (szVersion[lgth] == L'0')
               return TRUE;
        }
        
        // Check for dev version (v1.x86ret, v1.x86fstchk...)
        else if(!wcsncmp(szVersion+lgth, g_DevelopmentVersionW,
                         (sizeof(g_DevelopmentVersionW) / sizeof(WCHAR) - 1)))
            return TRUE;
    }
    // Some weird version...
    else if( (!wcscmp(szVersion, g_RetStringW)) ||
             (!wcscmp(szVersion, g_ComplusStringW)) )
        return TRUE;
    return FALSE;   


}

void AdjustImageRuntimeVersion(SString* pVersion)
{
    CONTRACTL
    {
        THROWS;
    }
    CONTRACTL_END;
    
    if (IsReallyRTM(*pVersion))
    {

        //
        // AN ANSI buffer can be opened by supplying n-1 chars of intended 
        // buffer (excluding the \0 character).
        //

        size_t bufLen = sizeof(g_RTMVersion) / sizeof(char) + 1;
        strcpy_s(pVersion->OpenANSIBuffer((COUNT_T) (bufLen-1)), bufLen, g_RTMVersion);
        pVersion->CloseBuffer();
    }
};


HRESULT GetImageRuntimeVersionString(PVOID pMetaData, LPCSTR* pString)
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    _ASSERTE(pString);
    STORAGESIGNATURE* pSig = (STORAGESIGNATURE*) pMetaData;

    // Verify the signature. 

    // If signature didn't match, you shouldn't be here.
    if (pSig->GetSignature() != STORAGE_MAGIC_SIG)
        return CLDB_E_FILE_CORRUPT;

    // The version started in version 1.1
    if (pSig->GetMajorVer() < 1)
        return CLDB_E_FILE_OLDVER;

    if (pSig->GetMajorVer() == 1 && pSig->GetMinorVer() < 1)
        return CLDB_E_FILE_OLDVER;
    
    // Header data starts after signature.
    *pString = (LPCSTR) pSig->pVersion;
    if(*pString) {
        size_t lgth = sizeof(g_VersionBase) / sizeof(char) - 1;
        size_t foundLgth = strlen(*pString);

        // Have normal version, v1.*
        if ( (foundLgth >= lgth+2) &&
             !strncmp(*pString, g_VersionBase, lgth) ) {

            // v1.0.* means RTM
            if ((*pString)[lgth+1] == '.') {
                if ((*pString)[lgth] == '0')
                    *pString = g_RTMVersion;
            }
            
            // Check for dev version (v1.x86ret, v1.x86fstchk...)
            else if(!strncmp(&(*pString)[lgth], g_DevelopmentVersion,
                             (sizeof(g_DevelopmentVersion) / sizeof(char) - 1)))
                *pString = g_RTMVersion;
        }

        // Some weird version...
        else if( (!strcmp(*pString, g_RetString)) ||
                 (!strcmp(*pString, g_ComplusString)) )
            *pString = g_RTMVersion;
    }

    return S_OK;
}


//*****************************************************************************
// Convert a UTF8 string to Unicode, into a CQuickArray<WCHAR>.
//*****************************************************************************
HRESULT Utf2Quick(
    LPCUTF8     pStr,                   // The string to convert.
    CQuickArray<WCHAR> &rStr,           // The QuickArray<WCHAR> to convert it into.
    int         iCurLen)                // Inital characters in the array to leave (default 0).
{
    CONTRACTL
    {
        NOTHROW;
    }
    CONTRACTL_END;
    
    HRESULT     hr = S_OK;              // A result.
    int         iReqLen;                // Required additional length.
    int         iActLen;
    int         bAlloc = 0;             // If non-zero, allocation was required.

    // Attempt the conversion.
    iReqLen = WszMultiByteToWideChar(CP_UTF8, 0, pStr, -1, rStr.Ptr()+iCurLen, (int)(rStr.MaxSize()-iCurLen));
    // If the buffer was too small, determine what is required.
    if (iReqLen == 0) 
        bAlloc = iReqLen = WszMultiByteToWideChar(CP_UTF8, 0, pStr, -1, 0, 0);
    // Resize the buffer.  If the buffer was large enough, this just sets the internal
    //  counter, but if it was too small, this will attempt a reallocation.  Note that 
    //  the length includes the terminating L'/0'.
    IfFailGo(rStr.ReSizeNoThrow(iCurLen+iReqLen));
    // If we had to realloc, then do the conversion again, now that the buffer is 
    //  large enough.
    if (bAlloc) {
        iActLen = WszMultiByteToWideChar(CP_UTF8, 0, pStr, -1, rStr.Ptr()+iCurLen, (int)(rStr.MaxSize())-iCurLen);
        _ASSERTE(iReqLen == iActLen);
    }
ErrExit:
    return hr;
} // HRESULT Utf2Quick()


//*****************************************************************************
//  Extract the movl 64-bit unsigned immediate from an IA64 bundle
//  (Format X2)
//*****************************************************************************
UINT64 GetIA64Imm64(UINT64 * pBundle)
{
    WRAPPER_CONTRACT;
    
    UINT64 temp0 = PTR_UINT64(pBundle)[0];
    UINT64 temp1 = PTR_UINT64(pBundle)[1];

    return GetIA64Imm64(temp0, temp1);
}

UINT64 GetIA64Imm64(UINT64 qword0, UINT64 qword1)
{
    LEAF_CONTRACT;
    
    UINT64 imm64 = 0;
    
#ifdef _DEBUG_IMPL
    //
    // make certain we're decoding a movl opcode, with template 4 or 5
    //
    UINT64    templa = (qword0 >>  0) & 0x1f;
    UINT64    opcode = (qword1 >> 60) & 0xf;
    
    _ASSERTE((opcode == 0x6) && ((templa == 0x4) || (templa == 0x5)));
#endif        

    imm64  = (qword1 >> 59) << 63;       //  1 i
    imm64 |= (qword1 << 41) >>  1;       // 23 high bits of imm41 
    imm64 |= (qword0 >> 46) << 22;       // 18 low  bits of imm41
    imm64 |= (qword1 >> 23) & 0x200000;  //  1 ic
    imm64 |= (qword1 >> 29) & 0x1F0000;  //  5 imm5c
    imm64 |= (qword1 >> 43) & 0xFF80;    //  9 imm9d
    imm64 |= (qword1 >> 36) & 0x7F;      //  7 imm7b

    return imm64;
}

//*****************************************************************************
//  Deposit the movl 64-bit unsigned immediate into an IA64 bundle
//  (Format X2)
//*****************************************************************************
void PutIA64Imm64(UINT64 * pBundle, UINT64 imm64)
{
    LEAF_CONTRACT;
    
#ifdef _DEBUG_IMPL
    //
    // make certain we're decoding a movl opcode, with template 4 or 5
    //
    UINT64    templa = (pBundle[0] >>  0) & 0x1f;
    UINT64    opcode = (pBundle[1] >> 60) & 0xf ;
    
    _ASSERTE((opcode == 0x6) && ((templa == 0x4) || (templa == 0x5)));
#endif        

    const UINT64 mask0 = UI64(0x00003FFFFFFFFFFF);
    const UINT64 mask1 = UI64(0xF000080FFF800000);

    /* Clear all bits used as part of the imm64 */
    pBundle[0] &= mask0;
    pBundle[1] &= mask1;

    UINT64 temp0;
    UINT64 temp1;

    temp1  = (imm64 >> 63)      << 59;  //  1 i
    temp1 |= (imm64 & 0xFF80)   << 43;  //  9 imm9d
    temp1 |= (imm64 & 0x1F0000) << 29;  //  5 imm5c
    temp1 |= (imm64 & 0x200000) << 23;  //  1 ic
    temp1 |= (imm64 & 0x7F)     << 36;  //  7 imm7b
    temp1 |= (imm64 <<  1)      >> 41;  // 23 high bits of imm41
    temp0  = (imm64 >> 22)      << 46;  // 18 low bits of imm41

    /* Or in the new bits used in the imm64 */
    pBundle[0] |= temp0;
    pBundle[1] |= temp1;
    FlushInstructionCache(GetCurrentProcess(),pBundle,16);
}

//*****************************************************************************
//  Extract the addl 22-bit signed immediate from an IA64 bundle
//  (Format A5)
//*****************************************************************************
INT32 GetIA64Imm22(UINT64 * pBundle, UINT32 slot)
{
    INT32         imm22 = 0;
    UINT64        temp0 = PTR_UINT64(pBundle)[0];
    UINT64        temp1 = PTR_UINT64(pBundle)[1];
    
    if (slot == 0)
    {
        if ((temp0 >> 41) & 1)
            imm22 =              0xFFE00000;   //  1 sign bit
        imm22 |= (temp0 >> 11) & 0x001F0000;   //  5 imm5c
        imm22 |= (temp0 >> 25) & 0x0000FF80;   //  9 imm9d
        imm22 |= (temp0 >> 18) & 0x0000007F;   //  7 imm7b
    }
    else if (slot == 1)
    {
        if ((temp1 >> 18) & 1)
            imm22 =              0xFFE00000;   //  1 sign bit
        imm22 |= (temp1 <<  9) & 0x001F0000;   //  5 imm5c
        imm22 |= (temp1 >>  2) & 0x0000FF80;   //  9 imm9d
        imm22 |= (temp1 <<  5) & 0x00000060;   //  2 imm7b (hi2)
        imm22 |= (temp0 >> 59) & 0x0000001F;   //  5 imm7b (lo5)
    }
    else if (slot == 2)
    {
        if ((temp1 >> 59) & 1)
            imm22 =              0xFFE00000;   //  1 sign bit
        imm22 |= (temp1 >> 32) & 0x001F0000;   //  5 imm5c
        imm22 |= (temp1 >> 43) & 0x0000FF80;   //  9 imm9d
        imm22 |= (temp1 >> 36) & 0x0000007F;   //  7 imm7b
    }

    return imm22;
}

//*****************************************************************************
//  Deposit the addl 22-bit signed immediate into an IA64 bundle
//  (Format A5)
//*****************************************************************************
void  PutIA64Imm22(UINT64 * pBundle, UINT32 slot, INT32 imm22)
{
    if (slot == 0)
    {
        const UINT64 mask0 = UI64(0xFFFFFC000603FFFF);
        /* Clear all bits used as part of the imm22 */
        pBundle[0] &= mask0;

        UINT64 temp0;
        
        temp0  = (UINT64) (imm22 & 0x200000) << 20;     //  1 s
        temp0 |= (UINT64) (imm22 & 0x1F0000) << 11;     //  5 imm5c
        temp0 |= (UINT64) (imm22 & 0x00FF80) << 25;     //  9 imm9d
        temp0 |= (UINT64) (imm22 & 0x00007F) << 18;     //  7 imm7b
        
        /* Or in the new bits used in the imm22 */
        pBundle[0] |= temp0;

    }
    else if (slot == 1)
    {
        const UINT64 mask0 = UI64(0x07FFFFFFFFFFFFFF);
        const UINT64 mask1 = UI64(0xFFFFFFFFFFF8000C);
        /* Clear all bits used as part of the imm22 */
        pBundle[0] &= mask0;
        pBundle[1] &= mask1;
        
        UINT64 temp0;
        UINT64 temp1;
        
        temp1  = (UINT64) (imm22 & 0x200000) >>  4;     //  1 s
        temp1 |= (UINT64) (imm22 & 0x1F0000) >>  9;     //  5 imm5c
        temp1 |= (UINT64) (imm22 & 0x00FF80) <<  2;     //  9 imm9d
        temp1 |= (UINT64) (imm22 & 0x000060) >>  5;     //  2 imm7b (hi2)
        temp0  = (UINT64) (imm22 & 0x00001F) << 59;     //  5 imm7b (hi2)
        
        /* Or in the new bits used in the imm22 */
        pBundle[0] |= temp0;
        pBundle[1] |= temp1;
    }
    else if (slot == 0)
    {
        const UINT64 mask1 = UI64(0xF000180FFFFFFFFF);
        /* Clear all bits used as part of the imm22 */
        pBundle[1] &= mask1;

        UINT64 temp1;
        
        temp1  = (UINT64) (imm22 & 0x200000) << 37;     //  1 s
        temp1 |= (UINT64) (imm22 & 0x1F0000) << 32;     //  5 imm5c
        temp1 |= (UINT64) (imm22 & 0x00FF80) << 43;     //  9 imm9d
        temp1 |= (UINT64) (imm22 & 0x00007F) << 36;     //  7 imm7b
        
        /* Or in the new bits used in the imm22 */
        pBundle[1] |= temp1;
    }
    FlushInstructionCache(GetCurrentProcess(),pBundle,16);
}

//*****************************************************************************
//  Extract the IP-Relative signed 25-bit immediate from an IA64 bundle 
//  (Formats B1, B2 or B3)
//  Note that due to branch target alignment requirements 
//       the lowest four bits in the result will always be zero.
//*****************************************************************************
INT32 GetIA64Rel25(UINT64 * pBundle, UINT32 slot)
{
    WRAPPER_CONTRACT;
    
    UINT64 temp0 = PTR_UINT64(pBundle)[0];
    UINT64 temp1 = PTR_UINT64(pBundle)[1];

    return GetIA64Rel25(temp0, temp1, slot);
}

INT32 GetIA64Rel25(UINT64 qword0, UINT64 qword1, UINT32 slot)
{
    LEAF_CONTRACT;
    
    INT32 imm25 = 0;
    
    if (slot == 2)
    {
        if ((qword1 >> 59) & 1)
            imm25 = 0xFF000000;
        imm25 |= (qword1 >> 32) & 0x00FFFFF0;    // 20 imm20b
    }
    else if (slot == 1)
    {
        if ((qword1 >> 18) & 1)
            imm25 = 0xFF000000;
        imm25 |= (qword1 <<  9) & 0x00FFFE00;    // high 15 of imm20b
        imm25 |= (qword0 >> 55) & 0x000001F0;    // low   5 of imm20b
    }
    else if (slot == 0)
    {
        if ((qword0 >> 41) & 1)
            imm25 = 0xFF000000;
        imm25 |= (qword0 >> 14) & 0x00FFFFF0;    // 20 imm20b
    }

    return imm25;
}

//*****************************************************************************
//  Deposit the IP-Relative signed 25-bit immediate into an IA64 bundle
//  (Formats B1, B2 or B3)
//  Note that due to branch target alignment requirements 
//       the lowest four bits are required to be zero.
//*****************************************************************************
void PutIA64Rel25(UINT64 * pBundle, UINT32 slot, INT32 imm25)
{
    LEAF_CONTRACT;
    
    _ASSERTE((imm25 & 0xF) == 0);

    if (slot == 2)
    {
        const UINT64 mask1 = UI64(0xF700000FFFFFFFFF);
        /* Clear all bits used as part of the imm25 */
        pBundle[1] &= mask1;

        UINT64 temp1;
        
        temp1  = (UINT64) (imm25 & 0x1000000) << 35;     //  1 s
        temp1 |= (UINT64) (imm25 & 0x0FFFFF0) << 32;     // 20 imm20b
        
        /* Or in the new bits used in the imm64 */
        pBundle[1] |= temp1;
    }
    else if (slot == 1)
    {
        const UINT64 mask0 = UI64(0x0EFFFFFFFFFFFFFF);
        const UINT64 mask1 = UI64(0xFFFFFFFFFFFB8000);
        /* Clear all bits used as part of the imm25 */
        pBundle[0] &= mask0;
        pBundle[1] &= mask1;
        
        UINT64 temp0;
        UINT64 temp1;
        
        temp1  = (UINT64) (imm25 & 0x1000000) >>  7;     //  1 s
        temp1 |= (UINT64) (imm25 & 0x0FFFE00) >>  9;     // high 15 of imm20b
        temp0  = (UINT64) (imm25 & 0x00001F0) << 55;     // low   5 of imm20b
        
        /* Or in the new bits used in the imm64 */
        pBundle[0] |= temp0;
        pBundle[1] |= temp1;
    }
    else if (slot == 0)
    {
        const UINT64 mask0 = UI64(0xFFFFFDC00003FFFF);
        /* Clear all bits used as part of the imm25 */
        pBundle[0] &= mask0;

        UINT64 temp0;
        
        temp0  = (UINT64) (imm25 & 0x1000000) << 16;     //  1 s
        temp0 |= (UINT64) (imm25 & 0x0FFFFF0) << 14;     // 20 imm20b
        
        /* Or in the new bits used in the imm64 */
        pBundle[0] |= temp0;

    }
    FlushInstructionCache(GetCurrentProcess(),pBundle,16);
}

//*****************************************************************************
//  Extract the IP-Relative signed 64-bit immediate from an IA64 bundle 
//  (Formats X3 or X4)
//*****************************************************************************
INT64 GetIA64Rel64(UINT64 * pBundle)
{
    WRAPPER_CONTRACT;
    
    UINT64 temp0 = PTR_UINT64(pBundle)[0];
    UINT64 temp1 = PTR_UINT64(pBundle)[1];

    return GetIA64Rel64(temp0, temp1);
}

INT64 GetIA64Rel64(UINT64 qword0, UINT64 qword1)
{
    LEAF_CONTRACT;
    
    INT64 imm64 = 0;
    
#ifdef _DEBUG_IMPL
    //
    // make certain we're decoding a brl opcode, with template 4 or 5
    //
    UINT64       templa = (qword0 >>  0) & 0x1f;
    UINT64       opcode = (qword1 >> 60) & 0xf;
    
    _ASSERTE(((opcode == 0xC) || (opcode == 0xD)) &&
             ((templa == 0x4) || (templa == 0x5)));
#endif        

    imm64  = (qword1 >> 59) << 63;         //  1 i
    imm64 |= (qword1 << 41) >>  1;         // 23 high bits of imm39 
    imm64 |= (qword0 >> 48) << 24;         // 16 low  bits of imm39
    imm64 |= (qword1 >> 32) & 0xFFFFF0;    // 20 imm20b
                                          //  4 bits of zeros
    return imm64;
}

//*****************************************************************************
//  Deposit the IP-Relative signed 64-bit immediate into an IA64 bundle
//  (Formats X3 or X4)
//*****************************************************************************
void PutIA64Rel64(UINT64 * pBundle, INT64 imm64)
{
    LEAF_CONTRACT;
    
#ifdef _DEBUG_IMPL
    //
    // make certain we're decoding a brl opcode, with template 4 or 5
    //
    UINT64    templa = (pBundle[0] >>  0) & 0x1f;
    UINT64    opcode = (pBundle[1] >> 60) & 0xf;
    
    _ASSERTE(((opcode == 0xC) || (opcode == 0xD)) &&
             ((templa == 0x4) || (templa == 0x5)));
    _ASSERTE((imm64 & 0xF) == 0);
#endif        

    const UINT64 mask0 = UI64(0x00003FFFFFFFFFFF);
    const UINT64 mask1 = UI64(0xF700000FFF800000);

    /* Clear all bits used as part of the imm64 */
    pBundle[0] &= mask0;
    pBundle[1] &= mask1;

    UINT64 temp0  = (imm64 & UI64(0x000000FFFF000000)) << 24;  // 16 low  bits of imm39
    UINT64 temp1  = (imm64 & UI64(0x8000000000000000)) >>  4   //  1 i
                  | (imm64 & UI64(0x7FFFFF0000000000)) >> 40   // 23 high bits of imm39 
                  | (imm64 & UI64(0x0000000000FFFFF0)) << 32;  // 20 imm20b

    /* Or in the new bits used in the imm64 */
    pBundle[0] |= temp0;
    pBundle[1] |= temp1;
    FlushInstructionCache(GetCurrentProcess(),pBundle,16);
}


//---------------------------------------------------------------------
// Splits a command line into argc/argv lists, using the VC7 parsing rules.
//
// This functions interface mimics the CommandLineToArgvW api.
//
// If function fails, returns NULL.
//
// If function suceeds, call delete [] on return pointer when done.
//
//---------------------------------------------------------------------
LPWSTR *SegmentCommandLine(LPCWSTR lpCmdLine, DWORD *pNumArgs)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;


    *pNumArgs = 0;

    int nch = (int)wcslen(lpCmdLine);

    // Calculate the worstcase storage requirement. (One pointer for
    // each argument, plus storage for the arguments themselves.)
    int cbAlloc = (nch+1)*sizeof(LPWSTR) + sizeof(WCHAR)*(nch + 1);
    LPWSTR pAlloc = new (nothrow) WCHAR[cbAlloc / sizeof(WCHAR)];
    if (!pAlloc)
        return NULL;

    LPWSTR *argv = (LPWSTR*) pAlloc;  // We store the argv pointers in the first halt
    LPWSTR  pdst = (LPWSTR)( ((BYTE*)pAlloc) + sizeof(LPWSTR)*(nch+1) ); // A running pointer to second half to store arguments
    LPCWSTR psrc = lpCmdLine;
    WCHAR   c;
    BOOL    inquote;
    BOOL    copychar;
    int     numslash;

    // First, parse the program name (argv[0]). Argv[0] is parsed under
    // special rules. Anything up to the first whitespace outside a quoted
    // subtring is accepted. Backslashes are treated as normal characters.
    argv[ (*pNumArgs)++ ] = pdst;
    inquote = FALSE;
    do {
        if (*psrc == L'"' )
        {
            inquote = !inquote;
            c = *psrc++;
            continue;
        }
        *pdst++ = *psrc;

        c = *psrc++;

    } while ( (c != L'\0' && (inquote || (c != L' ' && c != L'\t'))) );

    if ( c == L'\0' ) {
        psrc--;
    } else {
        *(pdst-1) = L'\0';
    }

    inquote = FALSE;



    /* loop on each argument */
    for(;;)
    {
        if ( *psrc )
        {
            while (*psrc == L' ' || *psrc == L'\t')
            {
                ++psrc;
            }
        }

        if (*psrc == L'\0')
            break;              /* end of args */

        /* scan an argument */
        argv[ (*pNumArgs)++ ] = pdst;

        /* loop through scanning one argument */
        for (;;)
        {
            copychar = 1;
            /* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
               2N+1 backslashes + " ==> N backslashes + literal "
               N backslashes ==> N backslashes */
            numslash = 0;
            while (*psrc == L'\\')
            {
                /* count number of backslashes for use below */
                ++psrc;
                ++numslash;
            }
            if (*psrc == L'"')
            {
                /* if 2N backslashes before, start/end quote, otherwise
                   copy literally */
                if (numslash % 2 == 0)
                {
                    if (inquote)
                    {
                        if (psrc[1] == L'"')
                        {
                            psrc++;    /* Double quote inside quoted string */
                        }
                        else
                        {
                            /* skip first quote char and copy second */
                            copychar = 0;
                        }
                    }
                    else
                    {
                        copychar = 0;       /* don't copy quote */
                    }
                    inquote = !inquote;
                }
                numslash /= 2;          /* divide numslash by two */
            }
    
            /* copy slashes */
            while (numslash--)
            {
                *pdst++ = L'\\';
            }
    
            /* if at end of arg, break loop */
            if (*psrc == L'\0' || (!inquote && (*psrc == L' ' || *psrc == L'\t')))
                break;
    
            /* copy character into argument */
            if (copychar)
            {
                *pdst++ = *psrc;
            }
            ++psrc;
        }

        /* null-terminate the argument */

        *pdst++ = L'\0';          /* terminate string */
    }

    /* We put one last argument in -- a null ptr */
    argv[ (*pNumArgs) ] = NULL;

    _ASSERTE((BYTE*)pdst <= (BYTE*)pAlloc + cbAlloc);
    return argv;
}


LONGLONG  InterlockedCompareExchange64(LONGLONG volatile *pDestination,
                                                             LONGLONG exchange,
                                                             LONGLONG comparand)
{
#if defined(_X86_) && !defined(__GNUC__)
    DWORD *pExchange = (DWORD*)&exchange;
    DWORD *pComparand = (DWORD*)&comparand;
    __asm {
        mov    ecx, pExchange     // set exchange value
        mov    ebx, [ecx]
        mov    ecx, [ecx+4]
            
        mov    edx, pComparand    // set comparand value
        mov    eax, [edx] 
        mov    edx, [edx+4]

        mov    esi, pDestination  // set destination address
        lock   cmpxchg8b qword ptr [esi]    // original value loaded into edx:eax if comparison fails
    }
#else
    static volatile LONG spinLock;
    LONGLONG retVal;
    while (InterlockedCompareExchange(&spinLock, 1, 0) != 0)
        ;
    // now we have the lock
    retVal = *pDestination;
    if (*pDestination == comparand)
    {
        *pDestination = exchange;
    }
    InterlockedExchange(&spinLock, 0);
    return retVal;
#endif
}


volatile PVOID ForbidCallsIntoHostOnThisThread::s_pvOwningFiber = NULL;

void ClrTrace( wchar_t const * format, ... )
{
    va_list args;
    va_start( args, format );
    
    SString outs;
    outs.VPrintf( format, args );

    va_end( args );

    WszOutputDebugString( outs.GetUnicode() );
}


BOOL ThreadWillCreateGuardPage(SIZE_T sizeReservedStack, SIZE_T sizeCommitedStack)
{
    // We need to make sure there will be a reserved but never committed page at the end
    // of the stack. We do here the check NT does when it creates the user stack to decide
    // if there is going to be a guard page. However, that is not enough, as if we only
    // have a guard page, we have nothing to protect us from going pass it. Well, in 
    // fact, there is something that we will protect you, there are certain places 
    // (RTLUnwind) in NT that will check that the current frame is within stack limits. 
    // If we are not it will bomb out. We will also bomb out if we touch the hard guard
    // page.
    // 
    // For situation B, teb->StackLimit is at the beggining of the user stack (ie
    // before updating StackLimit it checks if it was able to create a new guard page,
    // in this case, it can't), which makes the check fail in RtlUnwind. 
    //
    //    Situation A  [ Hard guard page | Guard page | user stack]
    //
    //    Situation B  [ Guard page | User stack ]
    //
    //    Situation C  [ User stack ( no room for guard page) ]
    //
    //    Situation D (W9x) : Guard page or not, w9x has a 64k reserved region below
    //                        the stack, we don't need any checks at all
    //
    // We really want to be in situation A all the time, so we add one more page
    // to our requirements (we require guard page + hard guard)
        
    SYSTEM_INFO sysInfo;
    ::GetSystemInfo(&sysInfo);    

    // OS rounds up sizes the following way to decide if it marks a guard page
    sizeReservedStack = ALIGN(sizeReservedStack, ((size_t)sysInfo.dwAllocationGranularity));   // Allocation granularity
    sizeCommitedStack = ALIGN(sizeCommitedStack, ((size_t)sysInfo.dwPageSize));  // Page Size
 
    // OS wont create guard page, we can't execute managed code safely.
    // We also have to make sure we have a 'hard' guard, thus we add another
    // page to the memory we would need comitted.
    // That is, the following code will check if sizeReservedStack is at least 2 pages 
    // more than sizeCommitedStack.
    return (sizeReservedStack > sizeCommitedStack + ((size_t)sysInfo.dwPageSize));     
}

//The following characters have special sorting weights when combined with other
//characters, which means we can't use our fast sorting algorithm on them.
//Most of these are pretty rare control characters, but apostrophe and hyphen
//are fairly common and force us down the slower path.  This is because we want
//"word sorting", which means that "coop" and "co-op" sort together, instead of
//separately as they would if we were doing a string sort.
//      0x0001   6    3    2   2   0  ;Start Of Heading
//      0x0002   6    4    2   2   0  ;Start Of Text
//      0x0003   6    5    2   2   0  ;End Of Text
//      0x0004   6    6    2   2   0  ;End Of Transmission
//      0x0005   6    7    2   2   0  ;Enquiry
//      0x0006   6    8    2   2   0  ;Acknowledge
//      0x0007   6    9    2   2   0  ;Bell
//      0x0008   6   10    2   2   0  ;Backspace

//      0x000e   6   11    2   2   0  ;Shift Out
//      0x000f   6   12    2   2   0  ;Shift In
//      0x0010   6   13    2   2   0  ;Data Link Escape
//      0x0011   6   14    2   2   0  ;Device Control One
//      0x0012   6   15    2   2   0  ;Device Control Two
//      0x0013   6   16    2   2   0  ;Device Control Three
//      0x0014   6   17    2   2   0  ;Device Control Four
//      0x0015   6   18    2   2   0  ;Negative Acknowledge
//      0x0016   6   19    2   2   0  ;Synchronous Idle
//      0x0017   6   20    2   2   0  ;End Of Transmission Block
//      0x0018   6   21    2   2   0  ;Cancel
//      0x0019   6   22    2   2   0  ;End Of Medium
//      0x001a   6   23    2   2   0  ;Substitute
//      0x001b   6   24    2   2   0  ;Escape
//      0x001c   6   25    2   2   0  ;File Separator
//      0x001d   6   26    2   2   0  ;Group Separator
//      0x001e   6   27    2   2   0  ;Record Separator
//      0x001f   6   28    2   2   0  ;Unit Separator

//      0x0027   6  128    2   2   0  ;Apostrophe-Quote
//      0x002d   6  130    2   2   0  ;Hyphen-Minus

//      0x007f   6   29    2   2   0  ;Delete

const BYTE HighCharHelper::HighCharTable[]= {
    TRUE,     /* 0x0, 0x0 */
    TRUE, /* 0x1, .*/
    TRUE, /* 0x2, .*/
    TRUE, /* 0x3, .*/
    TRUE, /* 0x4, .*/
    TRUE, /* 0x5, .*/
    TRUE, /* 0x6, .*/
    TRUE, /* 0x7, .*/
    TRUE, /* 0x8, .*/
    FALSE, /* 0x9,   */
    FALSE, /* 0xA,  */
    FALSE, /* 0xB, .*/
    FALSE, /* 0xC, .*/
    FALSE, /* 0xD,  */
    TRUE, /* 0xE, .*/
    TRUE, /* 0xF, .*/
    TRUE, /* 0x10, .*/
    TRUE, /* 0x11, .*/
    TRUE, /* 0x12, .*/
    TRUE, /* 0x13, .*/
    TRUE, /* 0x14, .*/
    TRUE, /* 0x15, .*/
    TRUE, /* 0x16, .*/
    TRUE, /* 0x17, .*/
    TRUE, /* 0x18, .*/
    TRUE, /* 0x19, .*/
    TRUE, /* 0x1A, */
    TRUE, /* 0x1B, .*/
    TRUE, /* 0x1C, .*/
    TRUE, /* 0x1D, .*/
    TRUE, /* 0x1E, .*/
    TRUE, /* 0x1F, .*/
    FALSE, /*0x20,  */
    FALSE, /*0x21, !*/
    FALSE, /*0x22, "*/
    FALSE, /*0x23,  #*/
    FALSE, /*0x24,  $*/
    FALSE, /*0x25,  %*/
    FALSE, /*0x26,  &*/
    TRUE,  /*0x27, '*/
    FALSE, /*0x28, (*/
    FALSE, /*0x29, )*/
    FALSE, /*0x2A **/
    FALSE, /*0x2B, +*/
    FALSE, /*0x2C, ,*/
    TRUE,  /*0x2D, -*/
    FALSE, /*0x2E, .*/
    FALSE, /*0x2F, /*/
    FALSE, /*0x30, 0*/
    FALSE, /*0x31, 1*/
    FALSE, /*0x32, 2*/
    FALSE, /*0x33, 3*/
    FALSE, /*0x34, 4*/
    FALSE, /*0x35, 5*/
    FALSE, /*0x36, 6*/
    FALSE, /*0x37, 7*/
    FALSE, /*0x38, 8*/
    FALSE, /*0x39, 9*/
    FALSE, /*0x3A, :*/
    FALSE, /*0x3B, ;*/
    FALSE, /*0x3C, <*/
    FALSE, /*0x3D, =*/
    FALSE, /*0x3E, >*/
    FALSE, /*0x3F, ?*/
    FALSE, /*0x40, @*/
    FALSE, /*0x41, A*/
    FALSE, /*0x42, B*/
    FALSE, /*0x43, C*/
    FALSE, /*0x44, D*/
    FALSE, /*0x45, E*/
    FALSE, /*0x46, F*/
    FALSE, /*0x47, G*/
    FALSE, /*0x48, H*/
    FALSE, /*0x49, I*/
    FALSE, /*0x4A, J*/
    FALSE, /*0x4B, K*/
    FALSE, /*0x4C, L*/
    FALSE, /*0x4D, M*/
    FALSE, /*0x4E, N*/
    FALSE, /*0x4F, O*/
    FALSE, /*0x50, P*/
    FALSE, /*0x51, Q*/
    FALSE, /*0x52, R*/
    FALSE, /*0x53, S*/
    FALSE, /*0x54, T*/
    FALSE, /*0x55, U*/
    FALSE, /*0x56, V*/
    FALSE, /*0x57, W*/
    FALSE, /*0x58, X*/
    FALSE, /*0x59, Y*/
    FALSE, /*0x5A, Z*/
    FALSE, /*0x5B, [*/
    FALSE, /*0x5C, \*/
    FALSE, /*0x5D, ]*/
    FALSE, /*0x5E, ^*/
    FALSE, /*0x5F, _*/
    FALSE, /*0x60, `*/
    FALSE, /*0x61, a*/
    FALSE, /*0x62, b*/
    FALSE, /*0x63, c*/
    FALSE, /*0x64, d*/
    FALSE, /*0x65, e*/
    FALSE, /*0x66, f*/
    FALSE, /*0x67, g*/
    FALSE, /*0x68, h*/
    FALSE, /*0x69, i*/
    FALSE, /*0x6A, j*/
    FALSE, /*0x6B, k*/
    FALSE, /*0x6C, l*/
    FALSE, /*0x6D, m*/
    FALSE, /*0x6E, n*/
    FALSE, /*0x6F, o*/
    FALSE, /*0x70, p*/
    FALSE, /*0x71, q*/
    FALSE, /*0x72, r*/
    FALSE, /*0x73, s*/
    FALSE, /*0x74, t*/
    FALSE, /*0x75, u*/
    FALSE, /*0x76, v*/
    FALSE, /*0x77, w*/
    FALSE, /*0x78, x*/
    FALSE, /*0x79, y*/
    FALSE, /*0x7A, z*/
    FALSE, /*0x7B, {*/
    FALSE, /*0x7C, |*/
    FALSE, /*0x7D, }*/
    FALSE, /*0x7E, ~*/
    TRUE, /*0x7F, */
};


BOOL FileExists(LPCWSTR filename)
{
    WIN32_FIND_DATA data;        
    HANDLE h = WszFindFirstFile(filename, &data);
    if (h == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    ::FindClose(h);

    return TRUE;                
}


    


