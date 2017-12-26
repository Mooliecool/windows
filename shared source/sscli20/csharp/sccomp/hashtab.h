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
// File: hashtab.h
//
// Hash table templates.
// ===========================================================================

#ifndef __hashtab_h__
#define __hashtab_h__

/***************************************************************************************************
    Base for hash table classes. Implements lookup, grow, etc. This doesn't know anything about the
    value or the actual data stored in each entry.

    Some methods take additional template parameters:

        Key - key information to pass through to Table methods.
        EntryData - the actual entry type. This should be derived from Entry.

    * Table - the derived type. Table should derive from HashTableBase<Table> and should
        provide the following:

            bool EqualEntryKey(EntryData * pend, Key key, uint hash);
            uint GetEntryHash(EntryData * pend);
            void * AllocBlockZero(size_t cb); // To allocate buckets.
            void FreeBlock(void * pv, int cb); // To free buckets.
            void FreeAllEntries();

        These must be accessible to HashTableBase (typically be declaring it a friend).
***************************************************************************************************/
template<typename Table> class HashTableBase {
protected:
    struct Entry {
        Entry * penNext;
    };

    Entry ** m_prgpen;
    int m_cpenHash;
    int m_cpenTot;

    HashTableBase() {
        m_prgpen = NULL;
        m_cpenHash = 0;
        m_cpenTot = 0;
    }

    ~HashTableBase() {
        ASSERT(!m_prgpen); // Term should have been called!
    }

    void Init(int cpenInit) { // Must be a power of two.
        ASSERT(!m_prgpen && !m_cpenHash && !m_cpenTot);
        ASSERT(1 < cpenInit && (cpenInit & (cpenInit - 1)) == 0);
        m_cpenHash = cpenInit;
        m_prgpen = (Entry **)Ptbl()->AllocBlockZero(SizeMul(m_cpenHash, sizeof(Entry *)));
    }

    void Term() {
        if (m_prgpen) {
            Ptbl()->FreeAllEntries();
            Ptbl()->FreeBlock(m_prgpen, m_cpenHash * sizeof(Entry *));
        }
        m_prgpen = NULL;
        m_cpenHash = 0;
        m_cpenTot = 0;
    }

    Table * Ptbl() {
        ASSERT(static_cast<HashTableBase *>(reinterpret_cast<Table *>(this)) == this);
        return (Table *)this;
    }

    template<typename EntryData, typename Key>
    EntryData * FindEntry(Key key, uint hash) {
        ASSERT(static_cast<Entry *>(reinterpret_cast<EntryData *>(NULL)) == NULL);
        for (Entry * pen = m_prgpen[hash & (m_cpenHash - 1)]; pen; pen = pen->penNext) {
            if (Ptbl()->EqualEntryKey(reinterpret_cast<EntryData *>(pen), key, hash))
                return reinterpret_cast<EntryData *>(pen);
        }
        return NULL;
    }

    template<typename EntryData, typename Key>
    EntryData * FindEntry(Key key, uint hash, Entry *** pppenHere) {
        ASSERT(static_cast<Entry *>(reinterpret_cast<EntryData *>(NULL)) == NULL);
        *pppenHere = &m_prgpen[hash & (m_cpenHash - 1)]; // Insert new ones at head.
        for (Entry ** ppen = *pppenHere; *ppen; ppen = &(*ppen)->penNext) {
            if (Ptbl()->EqualEntryKey(reinterpret_cast<EntryData *>(*ppen), key, hash)) {
                *pppenHere = ppen; // So the entry can be deleted.
                return reinterpret_cast<EntryData *>(*ppen);
            }
        }
        return NULL;
    }

    template<typename EntryData>
    void InsertEntry(Entry * penNew, Entry ** ppenHere) {
        ASSERT(static_cast<Entry *>(reinterpret_cast<EntryData *>(NULL)) == NULL);
        ASSERT(!penNew->penNext);

        penNew->penNext = *ppenHere;
        *ppenHere = penNew;

        if (m_cpenTot++ >= 2 * m_cpenHash)
            GrowTable<EntryData>();
    }

    template<typename EntryData>
    EntryData * FirstEntry() {
        ASSERT(static_cast<Entry *>(reinterpret_cast<EntryData *>(NULL)) == NULL);
        for (int ipen = 0; ipen < m_cpenHash; ipen++) {
            if (m_prgpen[ipen])
                return reinterpret_cast<EntryData *>(m_prgpen[ipen]);
        }
        return NULL;
    }

    template<typename EntryData, typename Key>
    EntryData * FindNextEntry(Key key, uint hash) {
        ASSERT(static_cast<Entry *>(reinterpret_cast<EntryData *>(NULL)) == NULL);

        int ipen = hash & (m_cpenHash - 1);

        if (m_prgpen[ipen]) {
            // Look for the entry.
            Entry * pen = m_prgpen[ipen];
            for (;;) {
                if (Ptbl()->EqualEntryKey(reinterpret_cast<EntryData *>(pen), key, hash))
                    break;
                pen = pen->penNext;
                if (!pen) {
                    // Entry wasn't found, so return the first entry in this bucket.
                    return reinterpret_cast<EntryData *>(m_prgpen[ipen]);
                }
            }
            ASSERT(pen);

            if (pen->penNext)
                return reinterpret_cast<EntryData *>(pen->penNext);
            // Go to next bucket.
        }

        while (++ipen < m_cpenHash) {
            if (m_prgpen[ipen])
                return reinterpret_cast<EntryData *>(m_prgpen[ipen]);
        }

        return NULL;
    }

private:
    template<typename EntryData>
    void GrowTable() {
        ASSERT(static_cast<Entry *>(reinterpret_cast<EntryData *>(NULL)) == NULL);
        ASSERT(m_cpenHash > 0 && (m_cpenHash & (m_cpenHash - 1)) == 0); // Power of 2.
        ASSERT(m_cpenTot >= 2 * m_cpenHash);

        // Go from average density 2 to average density 1/2.
        int cpenHashNew = m_cpenHash * 4;
        Entry ** prgpenOld = m_prgpen;
        Entry ** prgpenNew = (Entry **)Ptbl()->AllocBlockZero(SizeMul(cpenHashNew, sizeof(Entry *)));

        for (Entry ** ppen = prgpenOld + m_cpenHash; ppen-- > prgpenOld; ) {
            while (*ppen) {
                Entry * pen = *ppen;
                ASSERT((Ptbl()->GetEntryHash(reinterpret_cast<EntryData *>(pen)) & (m_cpenHash - 1)) == (uint)(ppen - prgpenOld));

                // Remove it from the old list.
                *ppen = pen->penNext;

                // Add it to the new bucket.
                Entry ** ppenNew = prgpenNew + (Ptbl()->GetEntryHash(reinterpret_cast<EntryData *>(pen)) & (cpenHashNew - 1));
                pen->penNext = *ppenNew;
                *ppenNew = pen;
            }
        }

        Ptbl()->FreeBlock(prgpenOld, m_cpenHash * sizeof(Entry *));

        m_prgpen = prgpenNew;
        m_cpenHash = cpenHashNew;
    }
};


/***************************************************************************************************
    Implements AllocBlockZero and FreeBlock using the PAGEHEAP of the symbol manager.
    Initialize size of the bucket array is one page.
***************************************************************************************************/
template<typename Table, typename SymMgr> class HashTableBaseSymMgr : public HashTableBase<Table> {
protected:
    friend class HashTableBase<Table>;

    SymMgr * m_pmgr;

    HashTableBaseSymMgr() {
        m_pmgr = NULL;
    }

    void * AllocBlockZero(size_t cb) {
        ASSERT(cb > 0);
        void * pv = m_pmgr->host()->GetPageHeap()->AllocPages(cb);
        if (!pv)
            m_pmgr->host()->NoMemory();
        memset(pv, 0, cb);
        return pv;
    }

    void FreeBlock(void * pv, int cb) {
        ASSERT(pv && cb);
        m_pmgr->host()->GetPageHeap()->FreePages(ProtectedEntityFlags::Other, pv, cb);
    }

    void FreeAllEntries() { }

public:

    void Init(SymMgr * pmgr) {
        m_pmgr = pmgr;
        HashTableBase<Table>::Init((int)(PAGEHEAP::pageSize / sizeof(typename HashTableBase<Table>::Entry *)));
    }
    void Term() { HashTableBase<Table>::Term(); }
};

#endif // !__hashtab_h__
