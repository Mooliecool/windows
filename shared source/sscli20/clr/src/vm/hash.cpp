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
/*++

Module Name:

    synchash.cpp

--*/

#include "common.h"

#include "hash.h"

#include "excep.h"

#include "syncclean.hpp"

//---------------------------------------------------------------------
//  Array of primes, used by hash table to choose the number of buckets
//

const unsigned int g_rgNumPrimes = 71;
const DWORD g_rgPrimes[g_rgNumPrimes]={
5,11,17,23,29,37,47,59,71,89,107,131,163,197,239,293,353,431,521,631,761,919,
1103,1327,1597,1931,2333,2801,3371,4049,4861,5839,7013,8419,10103,12143,14591,
17519,21023,25229,30293,36353,43627,52361,62851,75431,90523, 108631, 130363,
156437, 187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403,
968897, 1162687, 1395263, 1674319, 2009191, 2411033, 2893249, 3471899, 4166287,
4999559, 5999471, 7199369
};


const unsigned int SLOTS_PER_BUCKET = 4;

#ifndef DACCESS_COMPILE 

void *PtrHashMap::operator new(size_t size, LoaderHeap *pHeap)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT; //return NULL;

    return pHeap->AllocMem(size);
}

void PtrHashMap::operator delete(void *p)
{
}


//-----------------------------------------------------------------
// Bucket methods

BOOL Bucket::InsertValue(const UPTR key, const UPTR value)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;  //return FALSE;

    _ASSERTE(key != EMPTY);
    _ASSERTE(key != DELETED);

    if (!HasFreeSlots())
        return false; //no free slots

    // might have a free slot
    for (UPTR i = 0; i < SLOTS_PER_BUCKET; i++)
    {
        //@NOTE we can't reuse DELETED slots
        if (m_rgKeys[i] == EMPTY)
        {
            SetValue (value, i);

            // On multiprocessors we should make sure that
            // the value is propagated before we proceed.
            // inline memory barrier call, refer to
            // function description at the beginning of this
            MemoryBarrier();

            m_rgKeys[i] = key;
            return true;
        }
    }       // for i= 0; i < SLOTS_PER_BUCKET; loop

    SetCollision(); // otherwise set the collision bit
    return false;
}

#endif // !DACCESS_COMPILE

//---------------------------------------------------------------------
//  inline Bucket* HashMap::Buckets()
//  get the pointer to the bucket array
inline
PTR_Bucket HashMap::Buckets()
{
    LEAF_CONTRACT;

#ifndef DACCESS_COMPILE 
    _ASSERTE (!g_fEEStarted || !m_fAsyncMode || GetThread() == NULL || GetThread()->PreemptiveGCDisabled());
#endif
    return m_rgBuckets + 1;
}

//---------------------------------------------------------------------
//  inline size_t HashMap::GetSize(PTR_Bucket rgBuckets)
//  get the number of buckets
inline
size_t HashMap::GetSize(PTR_Bucket rgBuckets)
{
    LEAF_CONTRACT;
    PTR_size_t pSize = PTR_size_t(PTR_TO_TADDR(rgBuckets+int(-1)));
    return pSize[0];
}

//---------------------------------------------------------------------
//  inline size_t HashMap::HashFunction(UPTR key, UINT numBuckets, UINT &seed, UINT &incr)
//  get the first & second hash function.
//   H(key, i) = h1(key) + i*h2(key, hashSize);  0 <= i < numBuckets
//   h2 must return a value >= 1 and < numBuckets.
inline
void HashMap::HashFunction(const UPTR key, const UINT numBuckets, UINT &seed, UINT &incr)
{
    LEAF_CONTRACT;
    // First hash function
    // We commonly use pointers, which are 4 byte aligned, so the two least
    // significant bits are often 0, then we mod this value by something like
    // 11.  We can get a better distribution for pointers by dividing by 4.
    seed = key >> 2;
    // Second hash function
    incr = (UINT)(1 + (((key >> 5) + 1) % ((UINT)numBuckets - 1)));
    _ASSERTE(incr > 0 && incr < numBuckets);
}


#ifndef DACCESS_COMPILE 

//---------------------------------------------------------------------
//  inline void HashMap::SetSize(Bucket *rgBuckets, size_t size)
//  set the number of buckets
inline
void HashMap::SetSize(Bucket *rgBuckets, size_t size)
{
    LEAF_CONTRACT;
    ((size_t*)rgBuckets)[0] = size;
}

//---------------------------------------------------------------------
//  HashMap::HashMap()
//  constructor, initialize all values
//
HashMap::HashMap()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    m_rgBuckets = NULL;
    m_pCompare = NULL;  // comparsion object
    m_cbInserts = 0;        // track inserts
    m_cbDeletes = 0;        // track deletes
    m_cbPrevSlotsInUse = 0; // track valid slots present during previous rehash

    //Debug data member
#ifdef _DEBUG 
    m_fInSyncCode = false;
#endif
    // profile data members
#ifdef _DEBUG 
    m_lockData = NULL;
    m_pfnLockOwner = NULL;
#endif // _DEBUG
}

//---------------------------------------------------------------------
//  void HashMap::Init(unsigned cbInitialSize, CompareFnPtr ptr, bool fAsyncMode)
//  set the initial size of the hash table and provide the comparison
//  function pointer
//
void HashMap::Init(DWORD cbInitialSize, CompareFnPtr ptr, BOOL fAsyncMode, LockOwner *pLock)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    Compare* pCompare = NULL;
    if (ptr != NULL)
    {
        pCompare = new Compare(ptr);
    }
    Init(cbInitialSize, pCompare, fAsyncMode, pLock);
}

DWORD HashMap::GetNearestIndex(DWORD cbInitialSize)
{
    LEAF_CONTRACT;

    DWORD lowIndex = 0;
    DWORD highIndex = sizeof(g_rgPrimes)/sizeof(unsigned) - 1;
    DWORD midIndex = (highIndex + 1) / 2;

    if (cbInitialSize <= g_rgPrimes[0])
        return 0;

    if (cbInitialSize >= g_rgPrimes[highIndex])
        return highIndex;

    while (true)
    {
        if (cbInitialSize < g_rgPrimes[midIndex])
        {
            highIndex = midIndex;
        }
        else
        {
            if (cbInitialSize == g_rgPrimes[midIndex])
                return midIndex;
            lowIndex = midIndex;
        }
        midIndex = lowIndex + (highIndex - lowIndex + 1)/2;
        if (highIndex == midIndex)
        {
            _ASSERTE(g_rgPrimes[highIndex] >= cbInitialSize);
            _ASSERTE(highIndex < g_rgNumPrimes);
            return highIndex;
        }
    }
}

//---------------------------------------------------------------------
//  void HashMap::Init(unsigned cbInitialSize, Compare* pCompare, bool fAsyncMode)
//  set the initial size of the hash table and provide the comparison
//  function pointer
//
void HashMap::Init(DWORD cbInitialSize, Compare* pCompare, BOOL fAsyncMode, LockOwner *pLock)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    DWORD size = g_rgPrimes[m_iPrimeIndex = GetNearestIndex(cbInitialSize)];
    m_rgBuckets = new Bucket[size+1];

    memset (m_rgBuckets, 0, (size+1)*sizeof(Bucket));
    SetSize(m_rgBuckets, size);

    m_pCompare = pCompare;

    m_fAsyncMode = fAsyncMode;

    // assert null comparison returns true
    //ASSERT(
    //      m_pCompare == NULL ||
    //      (m_pCompare->CompareHelper(0,0) != 0)
    //    );


#ifdef _DEBUG 
    if (pLock == NULL) {
        m_lockData = NULL;
        m_pfnLockOwner = NULL;
    }
    else
    {
        m_lockData = pLock->lock;
        m_pfnLockOwner = pLock->lockOwnerFunc;
    }
    if (m_pfnLockOwner == NULL) {
        m_writerThreadId.SetThreadId();
    }
#endif // _DEBUG
}

//---------------------------------------------------------------------
//  void PtrHashMap::Init(unsigned cbInitialSize, CompareFnPtr ptr, bool fAsyncMode)
//  set the initial size of the hash table and provide the comparison
//  function pointer
//
void PtrHashMap::Init(DWORD cbInitialSize, CompareFnPtr ptr, BOOL fAsyncMode, LockOwner *pLock)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    ComparePtr *compare = NULL;
    if (ptr != NULL)
        compare = new ComparePtr(ptr);

    m_HashMap.Init(cbInitialSize, compare, fAsyncMode, pLock);
}

//---------------------------------------------------------------------
//  HashMap::~HashMap()
//  destructor, free the current array of buckets
//
HashMap::~HashMap()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    // free the current table
    Clear();
    // compare object
    if (NULL != m_pCompare)
        delete m_pCompare;
}


//---------------------------------------------------------------------
//  HashMap::Clear()
//  Remove all elements from table
//
void HashMap::Clear()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    // free the current table
    delete [] m_rgBuckets;

    m_rgBuckets = NULL;
}


//---------------------------------------------------------------------
//  UPTR   HashMap::CompareValues(const UPTR value1, const UPTR value2)
//  compare values with the function pointer provided
//
#ifndef _DEBUG 
inline
#endif
UPTR   HashMap::CompareValues(const UPTR value1, const UPTR value2)
{
    WRAPPER_CONTRACT;

#ifndef _DEBUG 
    CONTRACTL
    {
        DISABLED(THROWS);       // This is not a bug, we cannot decide, since the function ptr called may be either.
        DISABLED(GC_NOTRIGGER); // This is not a bug, we cannot decide, since the function ptr called may be either.
    }
    CONTRACTL_END;
#endif // !_DEBUG

    /// NOTE:: the ordering of arguments are random
    return (m_pCompare == NULL || m_pCompare->CompareHelper(value1,value2));
}

//---------------------------------------------------------------------
//  bool HashMap::Enter()
//  bool HashMap::Leave()
//  check  valid use of the hash table in synchronus mode

#ifdef _DEBUG 
void HashMap::Enter(HashMap *map)
{
    LEAF_CONTRACT;

    // check proper concurrent use of the hash table
    if (map->m_fInSyncCode)
        ASSERT(0); // oops multiple access to sync.-critical code
    map->m_fInSyncCode = true;
}

void HashMap::Leave(HashMap *map)
{
    LEAF_CONTRACT;

    // check proper concurrent use of the hash table
    if (map->m_fInSyncCode == false)
        ASSERT(0); // oops multiple access to sync.-critical code
    map->m_fInSyncCode = false;
}
#endif // _DEBUG

#endif // !DACCESS_COMPILE

//---------------------------------------------------------------------
//  void HashMap::ProfileLookup(unsigned ntry)
//  profile helper code
void HashMap::ProfileLookup(UPTR ntry, UPTR retValue)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

#ifndef DACCESS_COMPILE 
#endif // !DACCESS_COMPILE
}

#ifndef DACCESS_COMPILE 

//---------------------------------------------------------------------
//  void HashMap::InsertValue (UPTR key, UPTR value)
//  Insert into hash table, if the number of retries
//  becomes greater than threshold, expand hash table
//
void HashMap::InsertValue (UPTR key, UPTR value)
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    _ASSERTE (OwnLock());

    GCX_MAYBE_COOP_NO_THREAD_BROKEN(m_fAsyncMode);

    ASSERT(m_rgBuckets != NULL);

    // check proper use in synchronous mode
    SyncAccessHolder holder(this);   // no-op in NON debug code


    ASSERT (key > DELETED);

    Bucket* rgBuckets = Buckets();
    UPTR cbSize = GetSize(rgBuckets);

    UINT seed, incr;
    HashFunction(key, cbSize, seed, incr);

    for (UPTR ntry =0; ntry < 8; ntry++)
    {
        Bucket* pBucket = &rgBuckets[seed % cbSize];
        if(pBucket->InsertValue(key,value))
        {
            goto LReturn;
        }

        seed += incr;
    } // for ntry loop

    // We need to expand to keep lookup short
    Rehash();

    // Try again
    PutEntry (Buckets(), key,value);

LReturn: // label for return

    m_cbInserts++;

    #ifdef _DEBUG 
        ASSERT (m_pCompare != NULL || value == LookupValue (key,value));
        // check proper concurrent use of the hash table in synchronous mode
    #endif // _DEBUG

    return;
}
#endif // !DACCESS_COMPILE

//---------------------------------------------------------------------
//  UPTR HashMap::LookupValue(UPTR key, UPTR value)
//  Lookup value in the hash table, use the comparison function
//  to verify the values match
//
UPTR HashMap::LookupValue(UPTR key, UPTR value)
{
    CONTRACTL
    {
        DISABLED(THROWS);       // This is not a bug, we cannot decide, since the function ptr called may be either.
        DISABLED(GC_NOTRIGGER); // This is not a bug, we cannot decide, since the function ptr called may be either.
        SO_TOLERANT;
    }
    CONTRACTL_END;

    SCAN_IGNORE_THROW;          // See contract above.
    SCAN_IGNORE_TRIGGER;        // See contract above.

#ifndef DACCESS_COMPILE 
    _ASSERTE (m_fAsyncMode || OwnLock());

    GCX_MAYBE_COOP_NO_THREAD_BROKEN(m_fAsyncMode);

    ASSERT(m_rgBuckets != NULL);
    // This is necessary in case some other thread
    // replaces m_rgBuckets
    ASSERT (key > DELETED);

#endif // !DACCESS_COMPILE

    PTR_Bucket rgBuckets = Buckets(); //atomic fetch
    UPTR  cbSize = GetSize(rgBuckets);

    UINT seed, incr;
    HashFunction(key, cbSize, seed, incr);

    UPTR ntry;
    for(ntry =0; ntry < cbSize; ntry++)
    {
        PTR_Bucket pBucket = rgBuckets+(seed % cbSize);
        for (unsigned int i = 0; i < SLOTS_PER_BUCKET; i++)
        {
            if (pBucket->m_rgKeys[i] == key) // keys match
            {

                // inline memory barrier call, refer to
                // function description at the beginning of this
                MemoryBarrier();

                UPTR storedVal = pBucket->GetValue(i);
                // if compare function is provided
                // dupe keys are possible, check if the value matches,
// Not using compare function in DAC build.
#ifndef DACCESS_COMPILE 
                if (CompareValues(value,storedVal))
#endif
                {
                    ProfileLookup(ntry,storedVal); //no-op in non HASHTABLE_PROFILE code

                    // return the stored value
                    return storedVal;
                }
            }
        }

        seed += incr;
        if(!pBucket->IsCollision())
            break;
    }   // for ntry loop

    // not found
    ProfileLookup(ntry,INVALIDENTRY); //no-op in non HASHTABLE_PROFILE code

    return INVALIDENTRY;
}

#ifndef DACCESS_COMPILE 

//---------------------------------------------------------------------
//  UPTR HashMap::ReplaceValue(UPTR key, UPTR value)
//  Replace existing value in the hash table, use the comparison function
//  to verify the values match
//
UPTR HashMap::ReplaceValue(UPTR key, UPTR value)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE(OwnLock());

    GCX_MAYBE_COOP_NO_THREAD_BROKEN(m_fAsyncMode);

    ASSERT(m_rgBuckets != NULL);
    // This is necessary in case some other thread
    // replaces m_rgBuckets
    ASSERT (key > DELETED);


    Bucket* rgBuckets = Buckets(); //atomic fetch
    UPTR  cbSize = GetSize(rgBuckets);

    UINT seed, incr;
    HashFunction(key, cbSize, seed, incr);

    UPTR ntry;
    for(ntry =0; ntry < cbSize; ntry++)
    {
        Bucket* pBucket = &rgBuckets[seed % cbSize];
        for (unsigned int i = 0; i < SLOTS_PER_BUCKET; i++)
        {
            if (pBucket->m_rgKeys[i] == key) // keys match
            {

                // inline memory barrier call, refer to
                // function description at the beginning of this
                MemoryBarrier();

                UPTR storedVal = pBucket->GetValue(i);
                // if compare function is provided
                // dupe keys are possible, check if the value matches,
                if (CompareValues(value,storedVal))
                {
                    ProfileLookup(ntry,storedVal); //no-op in non HASHTABLE_PROFILE code

                    pBucket->SetValue(value, i);

                    // On multiprocessors we should make sure that
                    // the value is propagated before we proceed.
                    // inline memory barrier call, refer to
                    // function description at the beginning of this
                    MemoryBarrier();

                    // return the previous stored value
                    return storedVal;
                }
            }
        }

        seed += incr;
        if(!pBucket->IsCollision())
            break;
    }   // for ntry loop

    // not found
    ProfileLookup(ntry,INVALIDENTRY); //no-op in non HASHTABLE_PROFILE code

    return INVALIDENTRY;
}

//---------------------------------------------------------------------
//  UPTR HashMap::DeleteValue (UPTR key, UPTR value)
//  if found mark the entry deleted and return the stored value
//
UPTR HashMap::DeleteValue (UPTR key, UPTR value)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    _ASSERTE (OwnLock());

    GCX_MAYBE_COOP_NO_THREAD_BROKEN(m_fAsyncMode);

    // check proper use in synchronous mode
    SyncAccessHolder holoder(this);  //no-op in non DEBUG code

    ASSERT(m_rgBuckets != NULL);
    // This is necessary in case some other thread
    // replaces m_rgBuckets
    ASSERT (key > DELETED);


    Bucket* rgBuckets = Buckets();
    UPTR  cbSize = GetSize(rgBuckets);

    UINT seed, incr;
    HashFunction(key, cbSize, seed, incr);

    UPTR ntry;
    for(ntry =0; ntry < cbSize; ntry++)
    {
        Bucket* pBucket = &rgBuckets[seed % cbSize];
        for (unsigned int i = 0; i < SLOTS_PER_BUCKET; i++)
        {
            if (pBucket->m_rgKeys[i] == key) // keys match
            {
                // inline memory barrier call, refer to
                // function description at the beginning of this
                MemoryBarrier();

                UPTR storedVal = pBucket->GetValue(i);
                // if compare function is provided
                // dupe keys are possible, check if the value matches,
                if (CompareValues(value,storedVal))
                {
                    if(m_fAsyncMode)
                    {
                        pBucket->m_rgKeys[i] = DELETED; // mark the key as DELETED
                    }
                    else
                    {
                        pBucket->m_rgKeys[i] = EMPTY;// otherwise mark the entry as empty
                        pBucket->SetFreeSlots();
                    }
                    m_cbDeletes++;  // track the deletes

                    ProfileLookup(ntry,storedVal); //no-op in non HASHTABLE_PROFILE code

                    // return the stored value
                    return storedVal;
                }
            }
        }

        seed += incr;
        if(!pBucket->IsCollision())
            break;
    }   // for ntry loop

    // not found
    ProfileLookup(ntry,INVALIDENTRY); //no-op in non HASHTABLE_PROFILE code

#ifdef _DEBUG 
    ASSERT (m_pCompare != NULL || (UPTR) INVALIDENTRY == LookupValue (key,value));
    // check proper concurrent use of the hash table in synchronous mode
#endif // _DEBUG

    return INVALIDENTRY;
}


//---------------------------------------------------------------------
//  UPTR HashMap::Gethash (UPTR key)
//  use this for lookups with unique keys
// don't need to pass an input value to perform the lookup
//
UPTR HashMap::Gethash (UPTR key)
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    return LookupValue(key,NULL);
}


//---------------------------------------------------------------------
//  UPTR PutEntry (Bucket* rgBuckets, UPTR key, UPTR value)
//  helper used by expand method below

UPTR HashMap::PutEntry (Bucket* rgBuckets, UPTR key, UPTR value)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    }
    CONTRACTL_END

    ASSERT (value > 0);
    ASSERT (key > DELETED);

    UPTR size = GetSize(rgBuckets);
    UINT seed, incr;
    HashFunction(key, size, seed, incr);

    UPTR ntry;
    for (ntry =0; ntry < size; ntry++)
    {
        Bucket* pBucket = &rgBuckets[seed % size];
        if(pBucket->InsertValue(key,value))
        {
            return ntry;
        }

        seed += incr;
    } // for ntry loop
    _ASSERTE(!"Hash table insert failed.  Bug in PutEntry or the code that resizes the hash table?");
    return INVALIDENTRY;
}

//---------------------------------------------------------------------
//
//  UPTR HashMap::NewSize()
//  compute the new size based on the number of free slots
//
inline
UPTR HashMap::NewSize()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

    ASSERT(m_cbInserts >= m_cbDeletes);
    UPTR cbValidSlots = m_cbInserts-m_cbDeletes;
    UPTR cbNewSlots = m_cbInserts > m_cbPrevSlotsInUse ? m_cbInserts - m_cbPrevSlotsInUse : 0;

    ASSERT(cbValidSlots >=0 );
    if (cbValidSlots == 0)
        return g_rgPrimes[0]; // Minimum size for this hash table.

    UPTR cbTotalSlots = (m_fAsyncMode) ? (UPTR)(cbValidSlots*3/2+cbNewSlots*.6) : cbValidSlots*3/2;

    //UPTR cbTotalSlots = cbSlotsInUse*3/2+m_cbDeletes;

    UPTR iPrimeIndex;
    for (iPrimeIndex = 0; iPrimeIndex < g_rgNumPrimes; iPrimeIndex++)
    {
        if (g_rgPrimes[iPrimeIndex] > cbTotalSlots)
        {
            return iPrimeIndex;
        }
    }
    ASSERT(iPrimeIndex == g_rgNumPrimes);
    ASSERT(0 && !"Hash table walked beyond end of primes array");
    return g_rgNumPrimes - 1;
}

//---------------------------------------------------------------------
//  void HashMap::Rehash()
//  Rehash the hash table, create a new array of buckets and rehash
// all non deleted values from the previous array
//
void HashMap::Rehash()
{
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FAULT;

    GCX_MAYBE_COOP_NO_THREAD_BROKEN(m_fAsyncMode);

    _ASSERTE (!g_fEEStarted || !m_fAsyncMode || GetThread() == NULL || GetThread()->PreemptiveGCDisabled());
    _ASSERTE (OwnLock());

    DWORD cbNewSize = g_rgPrimes[m_iPrimeIndex = NewSize()];

    ASSERT(m_iPrimeIndex < 70);

    Bucket* rgBuckets = Buckets();
    UPTR cbCurrSize =   GetSize(rgBuckets);

    Bucket* rgNewBuckets = (Bucket*) new BYTE[((cbNewSize + 1)*sizeof (Bucket))];
    memset (rgNewBuckets, 0, (cbNewSize + 1)*sizeof (Bucket));
    SetSize(rgNewBuckets, cbNewSize);

    // current valid slots
    UPTR cbValidSlots = m_cbInserts-m_cbDeletes;
    m_cbInserts = cbValidSlots; // reset insert count to the new valid count
    m_cbPrevSlotsInUse = cbValidSlots; // track the previous delete count
    m_cbDeletes = 0;            // reset delete count
    // rehash table into it

    if (cbValidSlots) // if there are valid slots to be rehashed
    {
        for (unsigned long nb = 0; nb < cbCurrSize; nb++)
        {
            for (unsigned int i = 0; i < SLOTS_PER_BUCKET; i++)
            {
                UPTR key =rgBuckets[nb].m_rgKeys[i];
                if (key > DELETED)
                {
                    PutEntry (rgNewBuckets+1, key, rgBuckets[nb].GetValue (i));

                        // check if we can bail out
                    if (--cbValidSlots == 0)
                        goto LDone; // break out of both the loops
                }
            } // for i =0 thru SLOTS_PER_BUCKET
        } //for all buckets
    }


LDone:

    Bucket* pObsoleteTables = m_rgBuckets;

    // memory barrier, to replace the pointer to array of bucket
    MemoryBarrier();

    // replace the old array with the new one.
    m_rgBuckets = rgNewBuckets;


#ifdef _DEBUG 

    unsigned nb;
    if (m_fAsyncMode)
    {
        // for all non deleted keys in the old table, make sure the corresponding values
        // are in the new lookup table

        for (nb = 1; nb <= ((size_t*)pObsoleteTables)[0]; nb++)
        {
            for (unsigned int i =0; i < SLOTS_PER_BUCKET; i++)
            {
                if (pObsoleteTables[nb].m_rgKeys[i] > DELETED)
                {
                    UPTR value = pObsoleteTables[nb].GetValue (i);
                    // make sure the value is present in the new table
                    ASSERT (m_pCompare != NULL || value == LookupValue (pObsoleteTables[nb].m_rgKeys[i], value));
                }
            }
        }
    }

    // make sure there are no deleted entries in the new lookup table
    // if the compare function provided is null, then keys must be unique
    for (nb = 0; nb < cbNewSize; nb++)
    {
        for (unsigned int i = 0; i < SLOTS_PER_BUCKET; i++)
        {
            UPTR keyv = Buckets()[nb].m_rgKeys[i];
            ASSERT (keyv != DELETED);
            if (m_pCompare == NULL && keyv != EMPTY)
            {
                ASSERT ((Buckets()[nb].GetValue (i)) == Gethash (keyv));
            }
        }
    }
#endif // _DEBUG

    if (m_fAsyncMode)
    {
        // If we are allowing asynchronous reads, we must delay bucket cleanup until GC time.
        SyncClean::AddHashMap (pObsoleteTables);
    }
    else
    {
        Bucket* pBucket = pObsoleteTables;
        while (pBucket) {
            Bucket* pNextBucket = NextObsolete(pBucket);
            delete [] pBucket;
            pBucket = pNextBucket;
        }
    }

}

//---------------------------------------------------------------------
//  void HashMap::Compact()
//  delete obsolete tables, try to compact deleted slots by sliding entries
//  in the bucket, note we can slide only if the bucket's collison bit is reset
//  otherwise the lookups will break
//  @perf, use the m_cbDeletes to m_cbInserts ratio to reduce the size of the hash
//   table
//
void HashMap::Compact()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;

    _ASSERTE (OwnLock());

    GCX_MAYBE_COOP_NO_THREAD_BROKEN(m_fAsyncMode);
    ASSERT(m_rgBuckets != NULL);

    // Try to resize if that makes sense (reduce the size of the table), but
    // don't fail the operation simply because we've run out of memory.
    UPTR iNewIndex = NewSize();
    if (iNewIndex != m_iPrimeIndex)
    {
        EX_TRY
        {
            FAULT_NOT_FATAL();
            Rehash();
        }
        EX_CATCH
        {
        }
        EX_END_CATCH(SwallowAllExceptions)
    }

    //compact deleted slots, mark them as EMPTY

    if (m_cbDeletes)
    {
        UPTR cbCurrSize = GetSize(Buckets());
        Bucket *pBucket = Buckets();
        Bucket *pSentinel;

        for (pSentinel = pBucket+cbCurrSize; pBucket < pSentinel; pBucket++)
        {   //loop thru all buckets
            for (unsigned int i = 0; i < SLOTS_PER_BUCKET; i++)
            {   //loop through all slots
                if (pBucket->m_rgKeys[i] == DELETED)
                {
                    pBucket->m_rgKeys[i] = EMPTY;
                    pBucket->SetFreeSlots(); // mark the bucket as containing
                                             // free slots

                    // Need to decrement insert and delete counts at the same
                    // time to preserve correct live count.
                    _ASSERTE(m_cbInserts >= m_cbDeletes);
                    --m_cbInserts;

                    if(--m_cbDeletes == 0) // decrement count
                        return;
                }
            }
        }
    }

}

#ifdef _DEBUG 
// A thread must own a lock for a hash if it is a writer.
BOOL HashMap::OwnLock()
{
    STATIC_CONTRACT_NOTHROW;
    STATIC_CONTRACT_GC_NOTRIGGER;
    STATIC_CONTRACT_FORBID_FAULT;

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
#endif // _DEBUG


#endif // !DACCESS_COMPILE

#ifdef DACCESS_COMPILE 

void
HashMap::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    // Assumed to be embedded, so no this enumeration.

    if (m_rgBuckets.IsValid())
    {
        ULONG32 numBuckets = (ULONG32)GetSize(Buckets()) + 1;
        DacEnumMemoryRegion(PTR_TO_TADDR(m_rgBuckets),
                            numBuckets * sizeof(Bucket));

        for (size_t i = 0; i < numBuckets; i++)
        {
            PTR_Bucket bucket = m_rgBuckets + i;
            if (bucket.IsValid())
            {
                bucket.EnumMem();
            }
        }
    }
}

#endif // DACCESS_COMPILE

