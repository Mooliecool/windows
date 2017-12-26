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
/*++---------------------------------------------------------------------------------------

Module Name:

    hash.h

Abstract:

    Fast hash table classes,
--*/

#ifndef _HASH_H_ 
#define _HASH_H_

#ifndef ASSERT 
#define ASSERT _ASSERTE
#endif


#include "crst.h"


const unsigned int HASHTABLE_LOOKUP_PROBES_DATA = 20;

//-------------------------------------------------------
//  enums for special Key values used in hash table
//
enum
{
    EMPTY  = 0,
    DELETED = 1,
    INVALIDENTRY = ~0
};

typedef ULONG_PTR UPTR;

//------------------------------------------------------------------------------
// classes in use
//------------------------------------------------------------------------------
class Bucket;
class HashMap;
class SyncHashMap;

//-------------------------------------------------------
//  class Bucket
//  used by hash table implementation
//
typedef DPTR(class Bucket) PTR_Bucket;
class Bucket
{
public:
    UPTR m_rgKeys[4];
    UPTR m_rgValues[4];
private:
    BYTE m_bCollision;
    BYTE m_bHasFreeSlot;
public:

    void SetValue (UPTR value, UPTR i)
    {
        LEAF_CONTRACT;

        m_rgValues[i] = value;
    }

    UPTR GetValue (UPTR i)
    {
        LEAF_CONTRACT;

        return m_rgValues[i];
    }

    UPTR IsCollision() // useful sentinel for fast fail of lookups
    {
        LEAF_CONTRACT;

        return m_bCollision;
    }

    void SetCollision()
    {
        LEAF_CONTRACT;

        m_bCollision = TRUE;
        m_bHasFreeSlot = FALSE;
    }

    BOOL HasFreeSlots()
    {
        WRAPPER_CONTRACT;

        // check for free slots available in the bucket
        // either there is no collision or a free slot has been during
        // compaction
        return (!IsCollision() || m_bHasFreeSlot);
    }

    void SetFreeSlots()
    {
        LEAF_CONTRACT;

        m_bHasFreeSlot = TRUE;
    }

    BOOL InsertValue(const UPTR key, const UPTR value);
};


//------------------------------------------------------------------------------
// bool (*CompareFnPtr)(UPTR,UPTR); pointer to a function that takes 2 UPTRs
// and returns a boolean, provide a function with this signature to the HashTable
// to use for comparing Values during lookup
//------------------------------------------------------------------------------
typedef  BOOL (*CompareFnPtr)(UPTR,UPTR);

class Compare
{
protected:
    Compare()
    {
        LEAF_CONTRACT;

        m_ptr = NULL;
    }
public:
    CompareFnPtr m_ptr;

    Compare(CompareFnPtr ptr)
    {
        LEAF_CONTRACT;

        _ASSERTE(ptr != NULL);
        m_ptr = ptr;
    }

    virtual UPTR CompareHelper(UPTR val1, UPTR storedval)
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

        return (*m_ptr)(val1,storedval);
    }
};

class ComparePtr : public Compare
{
public:
    ComparePtr (CompareFnPtr ptr)
    {
        LEAF_CONTRACT;

        _ASSERTE(ptr != NULL);
        m_ptr = ptr;
    }

    virtual UPTR CompareHelper(UPTR val1, UPTR storedval)
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

        storedval <<=1;
        return (*m_ptr)(val1,storedval);
    }
};

//------------------------------------------------------------------------------
// Class HashMap
// Fast Hash table, for concurrent use,
// stores a 4 byte Key and a 4 byte Value for each slot.
// Duplicate keys are allowed, (keys are compared as 4 byte UPTRs)
// Duplicate values are allowed,(values are compared using comparison fn. provided)
// but if no comparison function is provided then the values should be unique
//
// Lookup's don't require to take locks, unless you specify fAsyncMode.
// Insert and Delete operations require locks
// Inserting a duplicate value will assert in DEBUG mode, the PROPER way to perform inserts
// is to take a lock, do a lookup and if the lookup fails then Insert
//
// In async mode, deleted slots are not immediately reclaimed (until a rehash), and
// accesses to the hash table cause a transition to cooperative GC mode, and reclamation of old
// hash maps (after a rehash) are deferred until GC time.
// In sync mode, none of this is necessary; however calls to LookupValue must be synchronized as well.
//
// Algorithm:
//   The Hash table is an array of buckets, each bucket can contain 4 key/value pairs
//   Special key values are used to identify EMPTY and DELETED slots
//   Hash function uses the current size of the hash table and a SEED based on the key
//   to choose the bucket, seed starts of being the key and gets refined every time
//   the hash function is re-applied.
//
//   Inserts choose an empty slot in the current bucket for new entries, if the current bucket
//   is full, then the seed is refined and a new bucket is choosen, if an empty slot is not found
//   after 8 retries, the hash table is expanded, this causes the current array of buckets to
//   be put in a free list and a new array of buckets is allocated and all non-deleted entries
//   from the old hash table are rehashed to the new array
//   The old arrays are reclaimed during Compact phase, which should only be called during GC or
//   any other time it is guaranteed that no Lookups are taking place.
//   Concurrent Insert and Delete operations need to be serialized
//
//   Delete operations, mark the Key in the slot as DELETED, the value is not removed and inserts
//   don't reuse these slots, they get reclaimed during expansion and compact phases.
//
//------------------------------------------------------------------------------

#if defined (_MSC_VER) && _MSC_VER <= 1300 
template void DoNothing(HashMap *);
template BOOL CompareDefault(HashMap *,HashMap *);
typedef Holder<HashMap *, DoNothing, DoNothing> HACKHashMapSyncAccessHolder;
#endif // defined (_MSC_VER) && _MSC_VER <= 1300

class HashMap
{
    friend class SyncHashMap;

public:

    //@constructor
    HashMap() DAC_EMPTY();
    //destructor
    ~HashMap() DAC_EMPTY();

    // Init
    void Init(BOOL fAsyncMode, LockOwner *pLock)
    {
        WRAPPER_CONTRACT;

        Init(0, (Compare *)NULL,fAsyncMode, pLock);
    }
    // Init
    void Init(DWORD cbInitialSize, BOOL fAsyncMode, LockOwner *pLock)
    {
        WRAPPER_CONTRACT;

        Init(cbInitialSize, (Compare*)NULL, fAsyncMode, pLock);
    }
    // Init
    void Init(CompareFnPtr ptr, BOOL fAsyncMode, LockOwner *pLock)
    {
        WRAPPER_CONTRACT;

        Init(0, ptr, fAsyncMode, pLock);
    }

    // Init method
    void Init(DWORD cbInitialSize, CompareFnPtr ptr, BOOL fAsyncMode, LockOwner *pLock);


    //Init method
    void Init(DWORD cbInitialSize, Compare* pCompare, BOOL fAsyncMode, LockOwner *pLock);

    // check to see if the value is already in the Hash Table
    // key should be > DELETED
    // if provided, uses the comparison function ptr to compare values
    // returns INVALIDENTRY if not found
    UPTR LookupValue(UPTR key, UPTR value);

    // Insert if the value is not already present
    // it is illegal to insert duplicate values in the hash map
    // do a lookup to verify the value is not already present

    void InsertValue(UPTR key, UPTR value);

    // Replace the value if present
    // returns the previous value, or INVALIDENTRY if not present
    // does not insert a new value under any circumstances

    UPTR ReplaceValue(UPTR key, UPTR value);

    // mark the entry as deleted and return the stored value
    // returns INVALIDENTRY, if not found
    UPTR DeleteValue (UPTR key, UPTR value);

    // for unique keys, use this function to get the value that is
    // stored in the hash table, returns INVALIDENTRY if key not found
    UPTR Gethash(UPTR key);

    // Called only when all threads are frozed, like during GC
    // for a SINGLE user mode, call compact after every delete
    // operation on the hash table
    void Compact();

    // Remove all entries from the hash tablex
    void Clear();

#ifdef DACCESS_COMPILE 
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    // inline helper, in non HASHTABLE_PROFILE mode becomes a NO-OP
    void        ProfileLookup(UPTR ntry, UPTR retValue);
    // data members used for profiling


protected:
    // static helper function
    static UPTR PutEntry (Bucket* rgBuckets, UPTR key, UPTR value);
private:

    DWORD       GetNearestIndex(DWORD cbInitialSize);

#ifdef _DEBUG 
    static void            Enter(HashMap *);        // check valid to enter
    static void            Leave(HashMap *);        // check valid to leave

    typedef Holder<HashMap *, HashMap::Enter, HashMap::Leave> SyncAccessHolder;
    BOOL            m_fInSyncCode; // test for non-synchronus access
#else // !_DEBUG
    // in non DEBUG mode use a no-op helper
    typedef NoOpBaseHolder<HashMap *> SyncAccessHolder;
#endif // !_DEBUG

    // compute the new size, based on the number of free slots
    // available, compact or expand
    UPTR            NewSize();
    // create a new bucket array and rehash the non-deleted entries
    void            Rehash();
    static size_t   GetSize(PTR_Bucket rgBuckets);
    static void     SetSize(Bucket* rgBuckets, size_t size);
    PTR_Bucket      Buckets();
    UPTR            CompareValues(const UPTR value1, const UPTR value2);

    // For double hashing, compute the second hash function once, then add.
    // H(key, i) = H1(key) + i * H2(key), where 0 <= i < numBuckets
    static void     HashFunction(const UPTR key, const UINT numBuckets, UINT &seed, UINT &incr);

    Compare*        m_pCompare;         // compare object to be used in lookup
    SIZE_T          m_iPrimeIndex;      // current size (index into prime array)
    PTR_Bucket      m_rgBuckets;        // array of buckets

    // track the number of inserts and deletes
    SIZE_T          m_cbPrevSlotsInUse;
    SIZE_T          m_cbInserts;
    SIZE_T          m_cbDeletes;
    // mode of operation, synchronus or single user
    BYTE            m_fAsyncMode;

#ifdef _DEBUG 
    LPVOID          m_lockData;
    FnLockOwner     m_pfnLockOwner;
    EEThreadId      m_writerThreadId;
#endif // _DEBUG

#ifdef _DEBUG 
    // A thread must own a lock for a hash if it is a writer.
    BOOL OwnLock();
#endif // _DEBUG

public:
    ///---------Iterator----------------

    // Iterator,
    class Iterator
    {
        PTR_Bucket m_pBucket;
        PTR_Bucket m_pSentinel;
        int        m_id;
        BOOL       m_fEnd;

    public:

        // Constructor
        Iterator(Bucket* pBucket) :
            m_pBucket(PTR_Bucket(PTR_HOST_TO_TADDR(pBucket))),
            m_id(-1), m_fEnd(false)
        {

            WRAPPER_CONTRACT;

            if (!m_pBucket) {
                m_fEnd = true;
                return;
            }
            size_t cbSize = (PTR_size_t(m_pBucket))[0];
            m_pBucket++;
            m_pSentinel = m_pBucket+cbSize;
            MoveNext(); // start
        }

        Iterator(const Iterator& iter)
        {
            LEAF_CONTRACT;

            m_pBucket = iter.m_pBucket;
            m_pSentinel = iter.m_pSentinel;
            m_id    = iter.m_id;
            m_fEnd = iter.m_fEnd;

        }

        //destructor
        ~Iterator(){ LEAF_CONTRACT; };

        // friend operator==
        friend bool operator == (const Iterator& lhs, const Iterator& rhs)
        {
            LEAF_CONTRACT;

            return (lhs.m_pBucket == rhs.m_pBucket && lhs.m_id == rhs.m_id);
        }
        // operator =
        inline Iterator& operator= (const Iterator& iter)
        {
            LEAF_CONTRACT;

            m_pBucket = iter.m_pBucket;
            m_pSentinel = iter.m_pSentinel;
            m_id    = iter.m_id;
            m_fEnd = iter.m_fEnd;
            return *this;
        }

        // operator ++
        inline void operator++ ()
        {
            WRAPPER_CONTRACT;

            _ASSERTE(!m_fEnd); // check we are not alredy at end
            MoveNext();
        }
        // operator --



        //accessors : GetDisc() , returns the discriminator
        inline UPTR GetKey()
        {
            LEAF_CONTRACT;

            _ASSERTE(!m_fEnd); // check we are not alredy at end
            return m_pBucket->m_rgKeys[m_id];
        }
        //accessors : SetDisc() , sets the discriminator


        //accessors : GetValue(),
        // returns the pointer that corresponds to the discriminator
        inline UPTR GetValue()
        {
            WRAPPER_CONTRACT;

            _ASSERTE(!m_fEnd); // check we are not alredy at end
            return m_pBucket->GetValue(m_id);
        }


        // end(), check if the iterator is at the end of the bucket
        inline BOOL end() const
        {
            LEAF_CONTRACT;

            return m_fEnd;
        }

    protected:

        void MoveNext()
        {
            LEAF_CONTRACT;

            for (m_pBucket = m_pBucket;m_pBucket < m_pSentinel; m_pBucket++)
            {   //loop thru all buckets
                for (m_id = m_id+1; m_id < 4; m_id++)
                {   //loop through all slots
                    if (m_pBucket->m_rgKeys[m_id] > DELETED)
                    {
                        return;
                    }
                }
                m_id  = -1;
            }
            m_fEnd = true;
        }

    };

    inline Bucket* firstBucket()
    {
        WRAPPER_CONTRACT;

        return m_rgBuckets;
    }

    // return an iterator, positioned at the beginning of the bucket
    inline Iterator begin()
    {
        WRAPPER_CONTRACT;

        return Iterator(m_rgBuckets);
    }
};

//------------------------------------------------------------------------------
// Class SyncHashMap, helper
// this class is a wrapper for the above HashMap class, and shows how the above
// class should be used for concurrent access,
// some of the rules to follow when using the above hash table
// Insert and delete operations need to take a lock,
// Lookup operations don't require a lock
// Insert operations, after taking the lock, should verify the value about to be inserted
// is not already in the hash table

class SyncHashMap
{
    HashMap         m_HashMap;
    Crst            m_lock;

    UPTR FindValue(UPTR key, UPTR value)
    {
        WRAPPER_CONTRACT;

        return m_HashMap.LookupValue(key,value);;
    }

public:
    SyncHashMap()
        : m_lock("HashMap", CrstSyncHashLock, CrstFlags(CRST_REENTRANCY | CRST_UNSAFE_ANYMODE))
    {
        LEAF_CONTRACT;

    }

    void Init(DWORD cbInitialSize, CompareFnPtr ptr)
    {
        WRAPPER_CONTRACT;

        //comparison function,
        // to be used when duplicate keys are allowed
        LockOwner lock = {&m_lock, IsOwnerOfCrst};
        m_HashMap.Init(cbInitialSize, ptr,true,&lock);
    }

    UPTR DeleteValue (UPTR key, UPTR value)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FORBID_FAULT;

        CrstHolder ch(&m_lock);
        UPTR retVal = m_HashMap.DeleteValue(key,value);
        return retVal;
    }

    UPTR InsertValue(UPTR key, UPTR value)
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FAULT;


        CrstHolder ch(&m_lock);
        UPTR storedVal = FindValue(key,value); // check to see if someone beat us to it
        //UPTR storedVal = 0;
        if (storedVal == (UPTR) INVALIDENTRY) // value not found
        {       // go ahead and insert
            m_HashMap.InsertValue(key,value);
            storedVal = value;
        }
        return storedVal; // the value currently in the hash table
    }

    // For cases where 'value' we lookup by is not the same 'value' as we store.
    UPTR InsertValue(UPTR key, UPTR storeValue, UPTR lookupValue)
    {
        STATIC_CONTRACT_THROWS;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FAULT;

        CrstHolder ch(&m_lock);
        UPTR storedVal = FindValue(key,lookupValue); // check to see if someone beat us to it
        //UPTR storedVal = 0;
        if (storedVal == (UPTR) INVALIDENTRY) // value not found
        {       // go ahead and insert
            m_HashMap.InsertValue(key,storeValue);
            storedVal = storeValue;
        }
        return storedVal; // the value currently in the hash table
    }

    UPTR ReplaceValue(UPTR key, UPTR value)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FORBID_FAULT;

        CrstHolder ch(&m_lock);
        UPTR storedVal = ReplaceValue(key,value);
        return storedVal; // the value currently in the hash table
    }


    // lookup value in the hash table, uses the compare function to verify the values
    // match, returns the stored value if found, otherwise returns NULL
    UPTR LookupValue(UPTR key, UPTR value)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FORBID_FAULT;

        UPTR retVal = FindValue(key,value);
        if (retVal == EMPTY)
            return LookupValueSync(key,value);
        return retVal;
    }

    UPTR LookupValueSync(UPTR key, UPTR value)
    {
        STATIC_CONTRACT_NOTHROW;
        STATIC_CONTRACT_GC_TRIGGERS;
        STATIC_CONTRACT_FORBID_FAULT;

        CrstHolder ch(&m_lock);


        UPTR retVal  = FindValue(key,value);
        return retVal;
    }

    // for unique keys, use this function to get the value that is
    // stored in the hash table, returns 0 if key not found
    UPTR GetHash(UPTR key)
    {
        WRAPPER_CONTRACT;

        return m_HashMap.Gethash(key);
    }

    void Compact()
    {
        WRAPPER_CONTRACT;

        m_HashMap.Compact();
    }

    // Not protected by a lock ! Right now used only at shutdown, where this is ok
    inline HashMap::Iterator begin()
    {
        WRAPPER_CONTRACT;

        _ASSERTE(g_fEEShutDown);
        return HashMap::Iterator(m_HashMap.m_rgBuckets);
    }


};


//---------------------------------------------------------------------------------------
// class PtrHashMap
//  Wrapper class for using Hash table to store pointer values
//  HashMap class requires that high bit is always reset
//  The allocator used within the runtime, always allocates objects 8 byte aligned
//  so we can shift right one bit, and store the result in the hash table
class PtrHashMap
{
    HashMap         m_HashMap;

public:
#ifndef DACCESS_COMPILE 
    void *operator new(size_t size, LoaderHeap *pHeap);
    void operator delete(void *p);
#endif // !DACCESS_COMPILE

    // Init
    void Init(BOOL fAsyncMode, LockOwner *pLock)
    {
        WRAPPER_CONTRACT;

        Init(0,NULL,fAsyncMode,pLock);
    }
    // Init
    void Init(DWORD cbInitialSize, BOOL fAsyncMode, LockOwner *pLock)
    {
        WRAPPER_CONTRACT;

        Init(cbInitialSize, NULL, fAsyncMode,pLock);
    }
    // Init
    void Init(CompareFnPtr ptr, BOOL fAsyncMode, LockOwner *pLock)
    {
        WRAPPER_CONTRACT;

        Init(0, ptr, fAsyncMode,pLock);
    }

    // Init method
    void Init(DWORD cbInitialSize, CompareFnPtr ptr, BOOL fAsyncMode, LockOwner *pLock);

    // check to see if the value is already in the Hash Table
    LPVOID LookupValue(UPTR key, LPVOID pv)
    {
        WRAPPER_CONTRACT;

        _ASSERTE (key > DELETED);

        // gmalloc allocator, always allocates 8 byte aligned
        // so we can shift out the lowest bit
        // ptr right shift by 1
        UPTR value = (UPTR)pv;
        _ASSERTE((value & 0x1) == 0);
        value>>=1;
        UPTR val =  m_HashMap.LookupValue (key, value);
        if (val != (UPTR) INVALIDENTRY)
        {
            val<<=1;
        }
        return (LPVOID)val;
    }

    // Insert if the value is not already present
    // it is illegal to insert duplicate values in the hash map
    // users should do a lookup to verify the value is not already present

    void InsertValue(UPTR key, LPVOID pv)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(key > DELETED);

        // gmalloc allocator, always allocates 8 byte aligned
        // so we can shift out the lowest bit
        // ptr right shift by 1
        UPTR value = (UPTR)pv;
        _ASSERTE((value & 0x1) == 0);
        value>>=1;
        m_HashMap.InsertValue (key, value);
    }

    // Replace the value if present
    // returns the previous value, or INVALIDENTRY if not present
    // does not insert a new value under any circumstances

    LPVOID ReplaceValue(UPTR key, LPVOID pv)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(key > DELETED);

        // gmalloc allocator, always allocates 8 byte aligned
        // so we can shift out the lowest bit
        // ptr right shift by 1
        UPTR value = (UPTR)pv;
        _ASSERTE((value & 0x1) == 0);
        value>>=1;
        UPTR val = m_HashMap.ReplaceValue (key, value);
        if (val != (UPTR) INVALIDENTRY)
        {
            val<<=1;
        }
        return (LPVOID)val;
    }

    // mark the entry as deleted and return the stored value
    // returns INVALIDENTRY if not found
    LPVOID DeleteValue (UPTR key,LPVOID pv)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(key > DELETED);

        UPTR value = (UPTR)pv;
        _ASSERTE((value & 0x1) == 0);
        value >>=1 ;
        UPTR val = m_HashMap.DeleteValue(key, value);
        if (val != (UPTR) INVALIDENTRY)
        {
            val <<= 1;
        }
        return (LPVOID)val;
    }

    // for unique keys, use this function to get the value that is
    // stored in the hash table, returns INVALIDENTRY if key not found
    LPVOID Gethash(UPTR key)
    {
        WRAPPER_CONTRACT;

        _ASSERTE(key > DELETED);

        UPTR val = m_HashMap.Gethash(key);
        if (val != (UPTR) INVALIDENTRY)
        {
            val <<= 1;
        }
        return (LPVOID)val;
    }

    void Compact()
    {
        WRAPPER_CONTRACT;

        m_HashMap.Compact();
    }

    void Clear()
    {
        WRAPPER_CONTRACT;

        m_HashMap.Clear();
    }

    class PtrIterator
    {
        HashMap::Iterator iter;

    public:
        PtrIterator(HashMap& hashMap) : iter(hashMap.begin())
        {
            LEAF_CONTRACT;
        }
        PtrIterator(Bucket* bucket) : iter(bucket)
        {
            LEAF_CONTRACT;
        }

        ~PtrIterator()
        {
            LEAF_CONTRACT;
        }

        BOOL end()
        {
            WRAPPER_CONTRACT;

            return iter.end();
        }

        LPVOID GetValue()
        {
            WRAPPER_CONTRACT;

            UPTR val = iter.GetValue();
            if (val != (UPTR) INVALIDENTRY)
            {
                val <<= 1;
            }
            return (LPVOID)val;
        }

        void operator++()
        {
            WRAPPER_CONTRACT;

            iter.operator++();
        }
    };

    inline Bucket* firstBucket()
    {
        WRAPPER_CONTRACT;

        return m_HashMap.firstBucket();
    }

    // return an iterator, positioned at the beginning of the bucket
    inline PtrIterator begin()
    {
        WRAPPER_CONTRACT;

        return PtrIterator(m_HashMap);
    }

#ifdef DACCESS_COMPILE 
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
    {
        m_HashMap.EnumMemoryRegions(flags);
    }
#endif // DACCESS_COMPILE
};

//---------------------------------------------------------------------
//  inline Bucket*& NextObsolete (Bucket* rgBuckets)
//  get the next obsolete bucket in the chain
inline
Bucket*& NextObsolete (Bucket* rgBuckets)
{
    LEAF_CONTRACT;

    return *(Bucket**)&((size_t*)rgBuckets)[1];
}

#endif // !_HASH_H_
