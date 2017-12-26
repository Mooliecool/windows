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
#include "common.h"

#include "syncclean.hpp"
#include "virtualcallstub.h"

Bucket volatile* SyncClean::m_HashMap = NULL;
EEHashEntry volatile ** SyncClean::m_EEHashTable;

void SyncClean::Terminate()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CleanUp();
}

void SyncClean::AddHashMap (Bucket *bucket)
{
    WRAPPER_CONTRACT;

    if (!g_fEEStarted) {
        delete [] bucket;
        return;
    }
    _ASSERTE (GetThread() == NULL || GetThread()->PreemptiveGCDisabled());

    Bucket * pTempBucket = NULL;
    do
    {
        pTempBucket = (Bucket *)m_HashMap;
        NextObsolete (bucket) = pTempBucket;
    }
    while (FastInterlockCompareExchangePointer ((PVOID *)&m_HashMap, bucket, pTempBucket) != pTempBucket);
}

void SyncClean::AddEEHashTable (EEHashEntry** entry)
{
    WRAPPER_CONTRACT;

    if (!g_fEEStarted) {
        delete [] (entry-1);
        return;
    }
    _ASSERTE (GetThread() == NULL || GetThread()->PreemptiveGCDisabled());

    EEHashEntry * pTempHashEntry = NULL;
    do
    {
        pTempHashEntry = (EEHashEntry*)m_EEHashTable;
        entry[-1] = pTempHashEntry;
    }
    while (FastInterlockCompareExchangePointer ((PVOID *)&m_EEHashTable, entry, pTempHashEntry) != pTempHashEntry);
}

void SyncClean::CleanUp ()
{
    LEAF_CONTRACT;

    // Only GC thread can call this.
    _ASSERTE (g_fProcessDetach ||
              (GCHeap::IsGCInProgress()  && GetThread() == GCHeap::GetGCHeap()->GetGCThread()));
    if (m_HashMap)
    {
        Bucket * pTempBucket = (Bucket *)FastInterlockExchangePointer ((PVOID *)&m_HashMap, NULL);
        
        while (pTempBucket) 
        {
            Bucket* pNextBucket = NextObsolete (pTempBucket);
            delete [] pTempBucket;
            pTempBucket = pNextBucket;
        }
    }

    if (m_EEHashTable)
    {
        EEHashEntry ** pTempHashEntry = (EEHashEntry **)FastInterlockExchangePointer ((PVOID *)&m_EEHashTable, NULL);

        while (pTempHashEntry) {
            EEHashEntry **pNextHashEntry = (EEHashEntry **)pTempHashEntry[-1];
            pTempHashEntry --;
            delete [] pTempHashEntry;
            pTempHashEntry = pNextHashEntry;
        }        
    }    

    // Give others we want to reclaim during the GC sync point a chance to do it
    VirtualCallStubManager::ReclaimAll();
}
