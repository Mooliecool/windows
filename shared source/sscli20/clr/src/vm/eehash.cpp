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
// File: eehash.cpp
//
#include "common.h"
#include "excep.h"
#include "eehash.h"
#include "securityattributes.h"
#include "comstring.h"
#include "stringliteralmap.h"
#include "clsload.hpp"
#include "comnlsinfo.h"
#include "memoryreport.h"
#include "typectxt.h"
#include "genericdict.h"

#ifndef DACCESS_COMPILE

// ============================================================================
// UTF8 string hash table helper.
// ============================================================================
EEHashEntry_t * EEUtf8HashTableHelper::AllocateEntry(LPCUTF8 pKey, BOOL bDeepCopy, void *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    MEMORY_REPORT_CONTEXT_SCOPE("EEUtf8Hash");

    EEHashEntry_t *pEntry;

    if (bDeepCopy)
    {
        DWORD StringLen = (DWORD)strlen(pKey);
        pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(LPUTF8) + StringLen + 1];
        if (!pEntry)
            return NULL;

        memcpy(pEntry->Key + sizeof(LPUTF8), pKey, StringLen + 1); 
        *((LPUTF8*)pEntry->Key) = (LPUTF8)(pEntry->Key + sizeof(LPUTF8));
    }
    else
    {
        pEntry = (EEHashEntry_t *) new (nothrow)BYTE[SIZEOF_EEHASH_ENTRY + sizeof(LPUTF8)];
        if (pEntry)
            *((LPCUTF8*)pEntry->Key) = pKey;
    }

    return pEntry;
}


void EEUtf8HashTableHelper::DeleteEntry(EEHashEntry_t *pEntry, void *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    delete [] (BYTE*)pEntry;
}


BOOL EEUtf8HashTableHelper::CompareKeys(EEHashEntry_t *pEntry, LPCUTF8 pKey)
{
    LEAF_CONTRACT;

    LPCUTF8 pEntryKey = *((LPCUTF8*)pEntry->Key);
    return (strcmp(pEntryKey, pKey) == 0) ? TRUE : FALSE;
}


DWORD EEUtf8HashTableHelper::Hash(LPCUTF8 pKey)
{
    LEAF_CONTRACT;

    DWORD dwHash = 0;

    while (*pKey != 0)
    {
        dwHash = (dwHash << 5) + (dwHash >> 5) + (*pKey);
        pKey++;
    }

    return dwHash;
}


LPCUTF8 EEUtf8HashTableHelper::GetKey(EEHashEntry_t *pEntry)
{
    LEAF_CONTRACT;

    return *((LPCUTF8*)pEntry->Key);
}


// ============================================================================
// Unicode string hash table helper.
// ============================================================================
EEHashEntry_t * EEUnicodeHashTableHelper::AllocateEntry(EEStringData *pKey, BOOL bDeepCopy, void *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    MEMORY_REPORT_CONTEXT_SCOPE("EEUnicodeHash");

    EEHashEntry_t *pEntry;

    if (bDeepCopy)
    {
        pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(EEStringData) + ((pKey->GetCharCount() + 1) * sizeof(WCHAR))];
        if (pEntry) {
            EEStringData *pEntryKey = (EEStringData *)(&pEntry->Key);
            pEntryKey->SetIsOnlyLowChars (pKey->GetIsOnlyLowChars());
            pEntryKey->SetCharCount (pKey->GetCharCount());
            pEntryKey->SetStringBuffer ((LPWSTR) ((LPBYTE)pEntry->Key + sizeof(EEStringData)));
            memcpy((LPWSTR)pEntryKey->GetStringBuffer(), pKey->GetStringBuffer(), pKey->GetCharCount() * sizeof(WCHAR)); 
        }
    }
    else
    {
        pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(EEStringData)];
        if (pEntry) {
            EEStringData *pEntryKey = (EEStringData *) pEntry->Key;
            pEntryKey->SetIsOnlyLowChars (pKey->GetIsOnlyLowChars());
            pEntryKey->SetCharCount (pKey->GetCharCount());
            pEntryKey->SetStringBuffer (pKey->GetStringBuffer());
        }
    }

    return pEntry;
}


void EEUnicodeHashTableHelper::DeleteEntry(EEHashEntry_t *pEntry, void *pHeap)
{
    LEAF_CONTRACT;

    delete [] (BYTE*)pEntry;
}


BOOL EEUnicodeHashTableHelper::CompareKeys(EEHashEntry_t *pEntry, EEStringData *pKey)
{
    LEAF_CONTRACT;

    EEStringData *pEntryKey = (EEStringData*) pEntry->Key;

    // Same buffer, same string.
    if (pEntryKey->GetStringBuffer() == pKey->GetStringBuffer())
        return TRUE;

    // Length not the same, never a match.
    if (pEntryKey->GetCharCount() != pKey->GetCharCount())
        return FALSE;

    // Compare the entire thing.
    // We'll deliberately ignore the bOnlyLowChars field since this derived from the characters
    return !memcmp(pEntryKey->GetStringBuffer(), pKey->GetStringBuffer(), pEntryKey->GetCharCount() * sizeof(WCHAR));
}


DWORD EEUnicodeHashTableHelper::Hash(EEStringData *pKey)
{
    LEAF_CONTRACT;

    return (HashBytes((const BYTE *) pKey->GetStringBuffer(), pKey->GetCharCount()*sizeof(WCHAR)));
}


EEStringData *EEUnicodeHashTableHelper::GetKey(EEHashEntry_t *pEntry)
{
    LEAF_CONTRACT;

    return (EEStringData*)pEntry->Key;
}

void EEUnicodeHashTableHelper::ReplaceKey(EEHashEntry_t *pEntry, EEStringData *pNewKey)
{
    LEAF_CONTRACT;

    ((EEStringData*)pEntry->Key)->SetStringBuffer (pNewKey->GetStringBuffer());
    ((EEStringData*)pEntry->Key)->SetCharCount (pNewKey->GetCharCount());
    ((EEStringData*)pEntry->Key)->SetIsOnlyLowChars (pNewKey->GetIsOnlyLowChars());
}

// ============================================================================
// Unicode stringliteral hash table helper.
// ============================================================================
EEHashEntry_t * EEUnicodeStringLiteralHashTableHelper::AllocateEntry(EEStringData *pKey, BOOL bDeepCopy, void *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    // We assert here because we expect that the heap is not null for EEUnicodeStringLiteralHash table. 
    // If someone finds more uses of this kind of hashtable then remove this asserte. 
    // Also note that in case of heap being null we go ahead and use new /delete which is EXPENSIVE
    // But for production code this might be ok if the memory is fragmented then thers a better chance 
    // of getting smaller allocations than full pages.
    _ASSERTE (pHeap);

    MEMORY_REPORT_CONTEXT_SCOPE("EEStringLiteralHash");

    if (pHeap)
        return (EEHashEntry_t *) ((MemoryPool*)pHeap)->AllocateElementNoThrow ();
    else
        return (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY];
}


void EEUnicodeStringLiteralHashTableHelper::DeleteEntry(EEHashEntry_t *pEntry, void *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    // We assert here because we expect that the heap is not null for EEUnicodeStringLiteralHash table. 
    // If someone finds more uses of this kind of hashtable then remove this asserte. 
    // Also note that in case of heap being null we go ahead and use new /delete which is EXPENSIVE
    // But for production code this might be ok if the memory is fragmented then thers a better chance 
    // of getting smaller allocations than full pages.
    _ASSERTE (pHeap);

    if (pHeap)
        ((MemoryPool*)pHeap)->FreeElement(pEntry);
    else
        delete [] (BYTE*)pEntry;
}


BOOL EEUnicodeStringLiteralHashTableHelper::CompareKeys(EEHashEntry_t *pEntry, EEStringData *pKey)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

    BOOL bMatch = TRUE;
    WCHAR *thisChars;
    int thisLength;

    GCX_COOP();
    
    StringLiteralEntry *pHashData = (StringLiteralEntry *)pEntry->Data;
    STRINGREF *pStrObj = (STRINGREF*)(pHashData->GetStringObject());
    
    RefInterpretGetStringValuesDangerousForGC((STRINGREF)*pStrObj, &thisChars, &thisLength);

    // Length not the same, never a match.
    if ((unsigned int)thisLength != pKey->GetCharCount())
        bMatch = FALSE;

    // Compare the entire thing.
    // We'll deliberately ignore the bOnlyLowChars field since this derived from the characters
    bMatch = !memcmp(thisChars, pKey->GetStringBuffer(), thisLength * sizeof(WCHAR));

    return bMatch;
}


DWORD EEUnicodeStringLiteralHashTableHelper::Hash(EEStringData *pKey)
{
    LEAF_CONTRACT;

    return (HashBytes((const BYTE *) pKey->GetStringBuffer(), pKey->GetCharCount()));
}

// ============================================================================
// Permission set hash table helper.
// ============================================================================

EEHashEntry_t * EEPsetHashTableHelper::AllocateEntry(PsetCacheEntry *pKey, BOOL bDeepCopy, void *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    MEMORY_REPORT_CONTEXT_SCOPE("EEPSetHash");

    _ASSERTE(!bDeepCopy);
    return (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY];
}

void EEPsetHashTableHelper::DeleteEntry(EEHashEntry_t *pEntry, void *pHeap)
{
    LEAF_CONTRACT;

    delete [] (BYTE*)pEntry;
}

BOOL EEPsetHashTableHelper::CompareKeys(EEHashEntry_t *pEntry, PsetCacheEntry *pKey)
{
    LEAF_CONTRACT;

    return pKey->IsEquiv(&SecurityAttributes::s_rCachedPsets[(DWORD)(size_t)pEntry->Data]);
}

DWORD EEPsetHashTableHelper::Hash(PsetCacheEntry *pKey)
{
    LEAF_CONTRACT;

    return pKey->Hash();
}

PsetCacheEntry * EEPsetHashTableHelper::GetKey(EEHashEntry_t *pEntry)
{
    LEAF_CONTRACT;

    return &SecurityAttributes::s_rCachedPsets[(size_t)pEntry->Data];
}


// ============================================================================
// Instantiation hash table helper.
// ============================================================================

EEHashEntry_t *EEInstantiationHashTableHelper::AllocateEntry(const SigTypeContext *pKey, BOOL bDeepCopy, AllocationHeap pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END
    
    EEHashEntry_t *pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(SigTypeContext)];
    if (!pEntry)
        return NULL;
    *((SigTypeContext*)pEntry->Key) = *pKey;

    return pEntry;
}

void EEInstantiationHashTableHelper::DeleteEntry(EEHashEntry_t *pEntry, AllocationHeap pHeap)
{
    LEAF_CONTRACT;

    delete [] (BYTE*)pEntry;
}

BOOL EEInstantiationHashTableHelper::CompareKeys(EEHashEntry_t *pEntry, const SigTypeContext *pKey)
{
    LEAF_CONTRACT;

    SigTypeContext *pThis = (SigTypeContext*)&pEntry->Key;
    return SigTypeContext::Equal(pThis, pKey);
}

DWORD EEInstantiationHashTableHelper::Hash(const SigTypeContext *pKey)
{
    LEAF_CONTRACT;

    DWORD dwHash = 5381;
    DWORD i;

    for (i = 0; i < pKey->m_classInstCount; i++)
        dwHash = ((dwHash << 5) + dwHash) ^ (unsigned int)(SIZE_T)pKey->m_classInst[i].AsPtr();

    for (i = 0; i < pKey->m_methInstCount; i++)
        dwHash = ((dwHash << 5) + dwHash) ^ (unsigned int)(SIZE_T)pKey->m_methInst[i].AsPtr();

    return dwHash;
}

const SigTypeContext *EEInstantiationHashTableHelper::GetKey(EEHashEntry_t *pEntry)
{
    LEAF_CONTRACT;

    return (const SigTypeContext*)&pEntry->Key;
}



// ============================================================================
// ComComponentInfo hash table helper.
// ============================================================================

EEHashEntry_t *EEClassFactoryInfoHashTableHelper::AllocateEntry(ClassFactoryInfo *pKey, BOOL bDeepCopy, void *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        INJECT_FAULT(return NULL;);
    }
    CONTRACTL_END

    MEMORY_REPORT_CONTEXT_SCOPE("EEClassFactoryInfoHash");

    EEHashEntry_t *pEntry;
    DWORD StringLen = 0;

    _ASSERTE(bDeepCopy && "Non deep copy is not supported by the EEComCompInfoHashTableHelper");

    if (pKey->m_strServerName)
        StringLen = (DWORD)wcslen(pKey->m_strServerName) + 1;
    pEntry = (EEHashEntry_t *) new (nothrow) BYTE[SIZEOF_EEHASH_ENTRY + sizeof(ClassFactoryInfo) + StringLen * sizeof(WCHAR)];
    if (pEntry) {
        memcpy(pEntry->Key + sizeof(ClassFactoryInfo), pKey->m_strServerName, StringLen * sizeof(WCHAR)); 
        ((ClassFactoryInfo*)pEntry->Key)->m_strServerName = pKey->m_strServerName ? (WCHAR*)(pEntry->Key + sizeof(ClassFactoryInfo)) : NULL;
        ((ClassFactoryInfo*)pEntry->Key)->m_clsid = pKey->m_clsid;
    }

    return pEntry;
}

void EEClassFactoryInfoHashTableHelper::DeleteEntry(EEHashEntry_t *pEntry, void *pHeap)
{
    LEAF_CONTRACT;

    delete [] (BYTE*) pEntry;
}

BOOL EEClassFactoryInfoHashTableHelper::CompareKeys(EEHashEntry_t *pEntry, ClassFactoryInfo *pKey)
{
    LEAF_CONTRACT;

    // First check the GUIDs.
    if (((ClassFactoryInfo*)pEntry->Key)->m_clsid != pKey->m_clsid)
        return FALSE;

    // Next do a trivial comparition on the server name pointer values.
    if (((ClassFactoryInfo*)pEntry->Key)->m_strServerName == pKey->m_strServerName)
        return TRUE;

    // If the pointers are not equal then if one is NULL then the server names are different.
    if (!((ClassFactoryInfo*)pEntry->Key) || !pKey->m_strServerName)
        return FALSE;

    // Finally do a string comparition of the server names.
    return wcscmp(((ClassFactoryInfo*)pEntry->Key)->m_strServerName, pKey->m_strServerName) == 0;
}

DWORD EEClassFactoryInfoHashTableHelper::Hash(ClassFactoryInfo *pKey)
{
    LEAF_CONTRACT;

    DWORD dwHash = 0;
    BYTE *pGuidData = (BYTE*)&pKey->m_clsid;

    for (unsigned int i = 0; i < sizeof(GUID); i++)
    {
        dwHash = (dwHash << 5) + (dwHash >> 5) + (*pGuidData);
        pGuidData++;
    }

    if (pKey->m_strServerName)
    {
        WCHAR *pSrvNameData = pKey->m_strServerName;

        while (*pSrvNameData != 0)
        {
            dwHash = (dwHash << 5) + (dwHash >> 5) + (*pSrvNameData);
            pSrvNameData++;
        }
    }

    return dwHash;
}

ClassFactoryInfo *EEClassFactoryInfoHashTableHelper::GetKey(EEHashEntry_t *pEntry)
{
    LEAF_CONTRACT;

    return (ClassFactoryInfo*)pEntry->Key;
}


// ============================================================================
// Class hash table methods
// ============================================================================

TypeHandle EEClassHashEntry::GetEntry()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;
    
    _ASSERTE(!(((ULONG_PTR)Data) & EECLASSHASH_TYPEHANDLE_DISCR));
    
    TypeHandle type = TypeHandle::FromPtr(Data);
    _ASSERTE (type.AsMethodTable());

    return type;
    
}

/*static*/ EEClassHashTable *EEClassHashTable::Create(Module *pModule, DWORD dwNumBuckets, BOOL bCaseInsensitive, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());

    }
    CONTRACTL_END

    BYTE *              pMem;
    EEClassHashTable *  pThis;
    size_t              size = sizeof(EEClassHashTable);

    // Prefast overflow sanity check.
    _ASSERTE( dwNumBuckets >= 0 );
    S_SIZE_T allocSize = S_SIZE_T( dwNumBuckets ) 
                                        * S_SIZE_T( sizeof(EEClassHashEntry_t*) )
                                        + S_SIZE_T( size );
    if( allocSize.IsOverflow() )
    {
        COMPlusThrowOM();
    }

    pMem = (BYTE *) pamTracker->Track(pModule->GetAssembly()->GetLowFrequencyHeap()->AllocMem(allocSize.Value()));
    // Don't need to memset() since this was ClrVirtualAlloc()'d memory
    pThis = (EEClassHashTable *) pMem;

#ifdef _DEBUG
    pThis->m_dwDebugMemory = (DWORD)allocSize.Value();
#endif

    pThis->m_dwNumBuckets = dwNumBuckets;
    pThis->m_dwNumEntries = 0;
    pThis->m_pBuckets = (EEClassHashEntry_t**) (pMem + size);
    pThis->m_pModule    = pModule;
    pThis->m_bCaseInsensitive = bCaseInsensitive;
    pThis->m_dwNumHotBuckets = 0;
    pThis->m_hotBucketIndex = NULL;
    pThis->m_hotBucketChain = NULL;

    if (bCaseInsensitive)
    {
        // Must allow lazy init of NLS helpers now
        InternalCasingHelper::InitTable();
    }

    return pThis;
}





EEClassHashEntry_t *EEClassHashTable::AllocNewEntry(AllocMemTracker *pamTracker)
{

    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM(););
        MODE_ANY;

        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("EEClassHash");

    _ASSERTE(m_pModule);
    EEClassHashEntry_t *pTmp;
    DWORD dwSizeofEntry;

    dwSizeofEntry = sizeof(EEClassHashEntry_t);
    pTmp = (EEClassHashEntry_t *) pamTracker->Track(m_pModule->GetAssembly()->GetLowFrequencyHeap()->AllocMem(dwSizeofEntry));

    return pTmp;
}

//
// If the entry/bucket ratio grows too large, this routine attempts to increase the number of buckets and rehash all the entries.
//
VOID EEClassHashTable::RetuneHashTable(AllocMemTracker *pamTracker)
{    

    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;

        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());
    }
    CONTRACTL_END;


    if  (m_dwNumEntries > m_dwNumBuckets*2)
    {
        _ASSERTE(m_pModule);
        // Make the new bucket table 4 times bigger
        DWORD dwNewNumBuckets = m_dwNumBuckets * 4;
        DWORD dwAllocSize;

        TaggedMemAllocPtr taggedPtr;
        EEClassHashEntry_t **pNewBuckets;


        {
            // If we can't increase the number of buckets, we lose perf but not correctness. So we won't report this error to our caller.
            FAULT_NOT_FATAL();
            
            // Prefast overflow sanity check before the allocations.
            if (!ClrSafeInt<DWORD>::multiply(dwNewNumBuckets, sizeof(pNewBuckets[0]), dwAllocSize))
                return;

            taggedPtr = m_pModule->GetAssembly()->GetLowFrequencyHeap()->AllocMem_NoThrow(dwAllocSize);
            if (!(m_pModule->IsTenured()))
            {
                // Our Module is still in the creation stage (or we are the manifest module of an assembly that's still untenured)
                // so it's still possible this hash table may suffer a premature death. Thus, we must report our AllocMem to our tracker.
                //
                // Otherwise, we must *not* report as we will be publishing this new bucketlist into the hashtable immediately and that's
                // not an operation we can backout.
                if (!pamTracker->Track_NoThrow(taggedPtr))
                {
                    return;
                }
            }
            pNewBuckets = (EEClassHashEntry_t**)(void*)taggedPtr;
        }

        if (!pNewBuckets)
            return;
        
        // Don't need to memset() since this was ClrVirtualAlloc()'d memory
        // memset(pNewBuckets, 0, dwNewNumBuckets*sizeof(pNewBuckets[0]));
    
        // Run through the old table and transfer all the entries
    
        // Be sure not to mess with the integrity of the old table while
        // we are doing this, as there can be concurrent readers!  Note that
        // it is OK if the concurrent reader misses out on a match, though -
        // they will have to acquire the lock on a miss & try again.
    
        for (DWORD i = 0; i < m_dwNumBuckets; i++)
        {
            EEClassHashEntry_t * pEntry = m_pBuckets[i];
    
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
                DWORD dwNewBucket = (DWORD)(pEntry->dwHashValue % dwNewNumBuckets);
                EEClassHashEntry_t * pNextEntry  = pEntry->pNext;
    
                // Insert at head of bucket if non-nested, at end if nested
                if (pEntry->pEncloser && pNewBuckets[dwNewBucket]) {
                    EEClassHashEntry_t *pCurrent = pNewBuckets[dwNewBucket];
                    while (pCurrent->pNext)
                        pCurrent = pCurrent->pNext;
                    
                    pCurrent->pNext  = pEntry;
                    pEntry->pNext = NULL;
                }
                else {
                    pEntry->pNext = pNewBuckets[dwNewBucket];
                    pNewBuckets[dwNewBucket] = pEntry;
                }
    
                pEntry = pNextEntry;
            }
        }
    
        // Finally, store the new number of buckets and the new bucket table
        m_dwNumBuckets = dwNewNumBuckets;
        m_pBuckets = pNewBuckets;

        m_dwNumHotBuckets = 0;
    }
}

#endif // #ifndef DACCESS_COMPILE

VOID EEClassHashTable::ConstructKeyFromData(EEClassHashEntry_t *pEntry, // IN  : Entry to compare
                                               ConstructKeyCallback *pCallback) // This class will process the output
{
    // We can't write a CONTRACT for this guy.
    //
    // This function itself may not introduce GC's or exceptions. However, it passes
    // its work to a callback function passed by the caller, and that callback
    // can do whatever it wants.

    LPUTF8 Key[2];
    Key[0] = Key[1] = NULL;

    {
        MAYBE_CANNOTTHROWCOMPLUSEXCEPTION(!m_bCaseInsensitive);
        GCX_NOTRIGGER();
        MAYBE_FAULT_FORBID(!m_bCaseInsensitive); // Only the case-insensitive table can OOM!

#ifdef _DEBUG_IMPL
        _ASSERTE(!(m_bCaseInsensitive && FORBIDGC_LOADER_USE_ENABLED()));
#endif
    
        // cqb - If m_bCaseInsensitive is true for the hash table, the bytes in Key will be allocated
        // from cqb. This is to prevent wasting bytes in the Loader Heap. Thusly, it is important to note that
        // in this case, the lifetime of Key is bounded by the lifetime of cqb, which will free the memory
        // it allocated on destruction.
        
        _ASSERTE(m_pModule);
        LPSTR        pszName = NULL;
        LPSTR        pszNameSpace = NULL;
        IMDInternalImport *pInternalImport = NULL;
        
        HashDatum Data = NULL;
        if (!m_bCaseInsensitive)
            Data = pEntry->Data;
        else
            Data = (PTR_EEClassHashEntry((TADDR)pEntry->Data))->Data;
    
        // Lower bit is a discriminator.  If the lower bit is NOT SET, it means we have
        // a TypeHandle, otherwise, we have a mdtTypedef/mdtExportedType.
        if ((((TADDR) Data) & EECLASSHASH_TYPEHANDLE_DISCR) == 0)
        {
            TypeHandle pType = TypeHandle::FromPtr(Data);
            _ASSERTE (pType.GetMethodTable());
            MethodTable *pMT = pType.GetMethodTable();
            _ASSERTE(pMT);
            pMT->GetMDImport()->GetNameOfTypeDef(pMT->GetCl(), (LPCSTR *)&pszName, (LPCSTR *)&pszNameSpace);
        }
        else // We have a mdtoken
        {
            // call the lightweight version first
            mdToken mdtUncompressed = UncompressModuleAndClassDef(Data);
            if (TypeFromToken(mdtUncompressed) == mdtExportedType)
            {
                m_pModule->GetClassLoader()->GetAssembly()->GetManifestImport()->GetExportedTypeProps(mdtUncompressed, 
                                                                                    (LPCSTR *)&pszNameSpace,
                                                                                    (LPCSTR *)&pszName, 
                                                                                    NULL,   //mdImpl
                                                                                    NULL,   // type def
                                                                                    NULL);  // flags
            }
            else
            {
                _ASSERTE(TypeFromToken(mdtUncompressed) == mdtTypeDef);
    
                Module *    pUncompressedModule;
                mdTypeDef   UncompressedCl;
                UncompressModuleAndNonExportClassDef(Data, &pUncompressedModule, &UncompressedCl);
                _ASSERTE (pUncompressedModule && "Uncompressed token of unexpected type");
                pInternalImport = pUncompressedModule->GetMDImport();
                _ASSERTE(pInternalImport && "Uncompressed token has no MD import");
                pInternalImport->GetNameOfTypeDef(UncompressedCl, (LPCSTR *)&pszName, (LPCSTR *)&pszNameSpace);
            }
        }
        
        if (!m_bCaseInsensitive)
        {
            Key[0] = pszNameSpace;
            Key[1] = pszName;
        }
        else
        {
            // Scanner limitation: does not recognize this to be valid:
            //
            //  CONTRACT { if (!foo) YOU_CANNOT_DO_X }
            //
            //  if (foo)
            //  {
            //      DO_X;
            //  }
            //
            SCAN_IGNORE_THROW;
            SCAN_IGNORE_FAULT;

#ifndef DACCESS_COMPILE
            // We can call the nothrow version here because we fulfilled the requirement of calling
            // InitTables() in the "new" method.
            INT32 iNSLength = InternalCasingHelper::InvariantToLowerNoThrow(NULL, 0, pszNameSpace);
            if (!iNSLength)
            {
                COMPlusThrowOM();
            }

            INT32 iNameLength = InternalCasingHelper::InvariantToLowerNoThrow(NULL, 0, pszName);
            if (!iNameLength)
            {
                COMPlusThrowOM();
            }

            // Prefast overflow sanity check before alloc.
            INT32 iAllocSize;
            if (!ClrSafeInt<INT32>::addition(iNSLength, iNameLength, iAllocSize))
                COMPlusThrowOM();
            LPUTF8 pszOutNameSpace = (LPUTF8) _alloca(iAllocSize);
            if (iNSLength == 1)
            {
                *pszOutNameSpace = '\0';
            }
            else
            {
                if (!InternalCasingHelper::InvariantToLowerNoThrow(pszOutNameSpace, iNSLength, pszNameSpace))
                {
                    COMPlusThrowOM();
                }
            }
            LPUTF8 pszOutName = (LPUTF8) pszOutNameSpace + iNSLength;

            if (!InternalCasingHelper::InvariantToLowerNoThrow(pszOutName, iNameLength, pszName))
            {
                COMPlusThrowOM();
            }
            Key[0] = pszOutNameSpace;
            Key[1] = pszOutName;
#else
            DacNotImpl();
#endif // #ifndef DACCESS_COMPILE
        }
    }

    pCallback->UseKeys(Key);
}

#ifndef DACCESS_COMPILE

EEClassHashEntry_t *EEClassHashTable::InsertValue(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum Data, EEClassHashEntry_t *pEncloser, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;

        INJECT_FAULT(COMPlusThrowOM(););
        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());
    }
    CONTRACTL_END;


    RetuneHashTable(pamTracker);

    _ASSERTE(pszNamespace != NULL);
    _ASSERTE(pszClassName != NULL);
    _ASSERTE(m_dwNumBuckets != 0);
    _ASSERTE(m_pModule);

    DWORD           dwHash = Hash(pszNamespace, pszClassName);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;

    return InsertValueHelper(AllocNewEntry(pamTracker), pszNamespace, pszClassName, Data, pEncloser, dwHash, dwBucket);
}

#ifdef _DEBUG
class ConstructKeyCallbackValidate : public EEClassHashTable::ConstructKeyCallback
{
public:
    virtual void UseKeys(LPUTF8 *Key)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_DEBUG_ONLY;
        _ASSERTE (strcmp(pNewEntry->DebugKey[1], Key[1]) == 0);
        _ASSERTE (strcmp(pNewEntry->DebugKey[0], Key[0]) == 0);
    }
    
    EEClassHashEntry_t *pNewEntry;
    
};
#endif // _DEBUG

// This entrypoint lets the caller separate the allocation of the entrypoint from the actual insertion into the hashtable. (This lets us
// do multiple insertions without having to worry about an OOM occuring inbetween.)
//
// The newEntry must have been allocated using AllocEntry. It must not be referenced by any other entity (other than a holder or tracker)
// If this function throws, the caller is responsible for freeing the entry.
EEClassHashEntry_t *EEClassHashTable::InsertValueUsingPreallocatedEntry(EEClassHashEntry_t *pNewEntry, LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum Data, EEClassHashEntry_t *pEncloser)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;

        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());
    }
    CONTRACTL_END;

    DWORD           dwHash = Hash(pszNamespace, pszClassName);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;

    return InsertValueHelper(pNewEntry, pszNamespace, pszClassName, Data, pEncloser, dwHash, dwBucket);
}


EEClassHashEntry_t *EEClassHashTable::InsertValueHelper(EEClassHashEntry_t *pNewEntry, LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum Data, EEClassHashEntry_t *pEncloser, DWORD dwHash, DWORD dwBucket)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;

        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());
    }
    CONTRACTL_END;


    MEMORY_REPORT_CONTEXT_SCOPE("EEClassHash");

    // Fill the data structure before we put it in the list
    pNewEntry->pEncloser = pEncloser;
    pNewEntry->Data         = Data;
    pNewEntry->dwHashValue  = dwHash;

#ifdef _DEBUG
    LPCUTF8         Key[2] = { pszNamespace, pszClassName };
    memcpy(pNewEntry->DebugKey, Key, sizeof(LPCUTF8)*2);
#endif

    // Insert at head of bucket if non-nested, at end if nested
    if (pEncloser && m_pBuckets[dwBucket]) {
        EEClassHashEntry_t *pCurrent = m_pBuckets[dwBucket];
        while (pCurrent->pNext)
            pCurrent = pCurrent->pNext;

        pNewEntry->pNext = NULL;
        pCurrent->pNext  = pNewEntry;
    }
    else {
        pNewEntry->pNext     = m_pBuckets[dwBucket];
        m_pBuckets[dwBucket] = pNewEntry;
    }

#ifdef _DEBUG
    {
        // We may OOM during this debug check - if so, just pass the test. We cannot OOM out of this function.

        EX_TRY
        {
            ConstructKeyCallbackValidate cback;
    
            cback.pNewEntry = pNewEntry;
    

            FAULT_NOT_FATAL();
            // now verify that we can indeed get the namespace, name from this data
            ConstructKeyFromData (pNewEntry, &cback);
        }
        EX_CATCH
        {
            _ASSERTE(GET_EXCEPTION()->IsTransient());
        }
        EX_END_CATCH(SwallowAllExceptions)
    }
#endif
    m_dwNumEntries++;

    return pNewEntry;
}

EEClassHashEntry_t *EEClassHashTable::InsertValueIfNotFound(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum *pData, EEClassHashEntry_t *pEncloser, BOOL IsNested, BOOL *pbFound, AllocMemTracker *pamTracker)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());
    }
    CONTRACTL_END;

    RetuneHashTable(pamTracker);

    _ASSERTE(m_pModule);
    _ASSERTE(pszNamespace != NULL);
    _ASSERTE(pszClassName != NULL);
    _ASSERTE(m_dwNumBuckets != 0);
    _ASSERTE(m_pModule);

    DWORD           dwHash = Hash(pszNamespace, pszClassName);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;

    EEClassHashEntry_t * pNewEntry = FindItemHelper (pszNamespace, pszClassName, IsNested, dwHash, dwBucket);
    if (pNewEntry)
    {
        *pData = pNewEntry->Data;
        *pbFound = TRUE;
        return pNewEntry;
    }
    

    // Reached here implies that we didn't find the entry and need to insert it 
    *pbFound = FALSE;

    return InsertValueHelper(AllocNewEntry(pamTracker), pszNamespace, pszClassName, *pData, pEncloser, dwHash, dwBucket);
}

#endif // #ifndef DACCESS_COMPILE

EEClassHashEntry_t *EEClassHashTable::FindItem(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, BOOL IsNested)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(m_pModule);
    _ASSERTE(pszNamespace != NULL);
    _ASSERTE(pszClassName != NULL);
    _ASSERTE(m_dwNumBuckets != 0);

    DWORD           dwHash = Hash(pszNamespace, pszClassName);
    DWORD           dwBucket = dwHash % m_dwNumBuckets;

    return FindItemHelper (pszNamespace, pszClassName, IsNested, dwHash, dwBucket);
}

EEClassHashEntry_t *EEClassHashTable::FindItemHelper(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, BOOL IsNested, DWORD dwHash, DWORD dwBucket)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    PTR_EEClassHashEntry pSearch = NULL;

#ifndef DACCESS_COMPILE
    //NOTE: Since the m_pBuckets is a superset of m_hotBucketChain, then we
    //      have no need to search the hot list or to emit it into minidumps
    //      for use by DAC.
    for (DWORD i = 0; i < m_dwNumHotBuckets; i++)
    {
        if (m_hotBucketIndex[i] == dwBucket)
        {
            pSearch = m_hotBucketChain[i];
            break;
        }
    }
#endif

    if (pSearch == NULL)
        pSearch = m_pBuckets[dwBucket];

    for ( ; pSearch; pSearch = pSearch->pNext)
    {
        if (pSearch->dwHashValue == dwHash)
        {
            LPCUTF8         Key[2] = { pszNamespace, pszClassName };
            if (CompareKeys(pSearch, Key)) 
            {
                // If (IsNested), then we're looking for a nested class
                // If (pSearch->pEncloser), we've found a nested class
                if ((IsNested != FALSE) == ((pSearch->pEncloser) != NULL))
                {
                    if (m_bCaseInsensitive)
                        g_IBCLogger.LogClassHashTableAccess(&((PTR_EEClassHashEntry((TADDR)pSearch->Data))->Data));
                    else
                        g_IBCLogger.LogClassHashTableAccess(&(pSearch->Data));

                    return pSearch;
                }
                else if (pSearch->pEncloser)
                    return NULL; // searched past non-nested classes
            }
        }
    }

    return NULL;
}

EEClassHashEntry_t *EEClassHashTable::FindNextNestedClass(NameHandle* pName, HashDatum *pData, EEClassHashEntry_t *pBucket)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;


    _ASSERTE(m_pModule);
    _ASSERTE(pName);
    if(pName->GetNameSpace())
    {
        return FindNextNestedClass(pName->GetNameSpace(), pName->GetName(), pData, pBucket);
    }
    else {
        return FindNextNestedClass(pName->GetName(), pData, pBucket);
    }
}


EEClassHashEntry_t *EEClassHashTable::FindNextNestedClass(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum *pData, EEClassHashEntry_t *pBucket)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;


    _ASSERTE(m_pModule);
    DWORD           dwHash = Hash(pszNamespace, pszClassName);
    EEClassHashEntry_t * pSearch = pBucket->pNext;

    for (; pSearch; pSearch = pSearch->pNext)
    {
        if (pSearch->dwHashValue == dwHash)
        {
            LPCUTF8         Key[2] = { pszNamespace, pszClassName };
            if (CompareKeys(pSearch, Key)) 
            {
                *pData = pSearch->Data;
                return pSearch;
            }
        }
    }

    return NULL;
}


EEClassHashEntry_t *EEClassHashTable::FindNextNestedClass(LPCUTF8 pszFullyQualifiedName, HashDatum *pData, EEClassHashEntry_t *pBucket)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    EEClassHashEntry_t * ret = NULL;

    _ASSERTE(m_pModule);
    CHAR zerostring = '\0';
    LPSTR szNamespace = (LPSTR) &zerostring;
    LPCUTF8 p;

    if ((p = ns::FindSep(pszFullyQualifiedName)) != NULL)
    {
        SIZE_T d = p - pszFullyQualifiedName;
        FAULT_NOT_FATAL();
        szNamespace = (LPSTR) new(nothrow) CHAR[d+1];
        if(szNamespace == NULL) return NULL;
        memcpy(szNamespace, pszFullyQualifiedName, d);
        szNamespace[ d ] = '\0';
        p++;
    }
    else
    {
        p = pszFullyQualifiedName;
    }

    ret = FindNextNestedClass(szNamespace, p, pData, pBucket);
    if(szNamespace != (LPCSTR)&zerostring)
        delete [] szNamespace;
    return ret;
}


EEClassHashEntry_t * EEClassHashTable::GetValue(LPCUTF8 pszFullyQualifiedName, HashDatum *pData, BOOL IsNested)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(m_pModule);
    EEClassHashEntry_t * ret = NULL;
    CHAR zerostring = '\0';
    LPSTR szNamespace = (LPSTR) &zerostring;
    LPCUTF8 p;

    p = ns::FindSep(pszFullyQualifiedName);
    if (p != NULL)
    {
        SIZE_T d = p - pszFullyQualifiedName;
        FAULT_NOT_FATAL();
        szNamespace = (LPSTR) new(nothrow) CHAR[d+1];
        if(szNamespace == NULL) return NULL;
        memcpy(szNamespace, pszFullyQualifiedName, d);
        szNamespace[ d ] = '\0';
        p++;
    }
    else
    {
        p = pszFullyQualifiedName;
    }

    ret = GetValue(szNamespace, p, pData, IsNested);
    if(szNamespace != (LPCSTR)&zerostring)
        delete [] szNamespace;
    return ret;
}


EEClassHashEntry_t * EEClassHashTable::GetValue(LPCUTF8 pszNamespace, LPCUTF8 pszClassName, HashDatum *pData, BOOL IsNested)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;


    _ASSERTE(m_pModule);
    EEClassHashEntry_t *pItem = FindItem(pszNamespace, pszClassName, IsNested);
    if (pItem)
        *pData = pItem->Data;

    return pItem;
}


EEClassHashEntry_t * EEClassHashTable::GetValue(NameHandle* pName, HashDatum *pData, BOOL IsNested)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;


    _ASSERTE(pName);
    _ASSERTE(m_pModule);
    if(pName->GetNameSpace() == NULL) {
        return GetValue(pName->GetName(), pData, IsNested);
    }
    else {
        return GetValue(pName->GetNameSpace(), pName->GetName(), pData, IsNested);
    }
}

class ConstructKeyCallbackCompare : public EEClassHashTable::ConstructKeyCallback
{
public:
    virtual void UseKeys(LPUTF8 *pKey1)
    {
        LEAF_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        bReturn = ( 
            ((pKey1[0] == pKey2[0]) && (pKey1[1] == pKey2[1])) ||
            ((strcmp (pKey1[0], pKey2[0]) == 0) && (strcmp (pKey1[1], pKey2[1]) == 0))
            );
    }
    
    LPCUTF8 *pKey2;
    BOOL     bReturn;
    
};

// Returns TRUE if two keys are the same string.
//
// The case-insensitive table can throw OOM out of this function. The case-sensitive table can't.
BOOL EEClassHashTable::CompareKeys(EEClassHashEntry_t *pEntry, LPCUTF8 *pKey2)
{
    CONTRACTL
    {
        if (m_bCaseInsensitive) THROWS; else NOTHROW;
        if (m_bCaseInsensitive) GC_TRIGGERS; else GC_NOTRIGGER;
        if (m_bCaseInsensitive) INJECT_FAULT(COMPlusThrowOM();); else FORBID_FAULT;
        MODE_ANY;
    }
    CONTRACTL_END;


    _ASSERTE(m_pModule);
    _ASSERTE (pEntry);
    _ASSERTE (pKey2);

    ConstructKeyCallbackCompare cback;

    cback.pKey2 = pKey2;

    ConstructKeyFromData(pEntry, &cback);

    return cback.bReturn;
}


DWORD EEClassHashTable::Hash(LPCUTF8 pszNamespace, LPCUTF8 pszClassName)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        FORBID_FAULT;
    }
    CONTRACTL_END;


    DWORD dwHash = 5381;
    DWORD dwChar;

    while ((dwChar = *pszNamespace++) != 0)
        dwHash = ((dwHash << 5) + dwHash) ^ dwChar;

    while ((dwChar = *pszClassName++) != 0)
        dwHash = ((dwHash << 5) + dwHash) ^ dwChar;

    return  dwHash;
}

#ifndef DACCESS_COMPILE


/*===========================MakeCaseInsensitiveTable===========================
**Action: Creates a case-insensitive lookup table for class names.  We create a 
**        full path (namespace & class name) in lowercase and then use that as the
**        key in our table.  The hash datum is a pointer to the EEClassHashEntry in this
**        table.
**
!!        You MUST have already acquired the appropriate lock before calling this.!!
**
**Returns:The newly allocated and completed hashtable.
==============================================================================*/

class ConstructKeyCallbackCaseInsensitive : public EEClassHashTable::ConstructKeyCallback
{
public:
    virtual void UseKeys(LPUTF8 *key)
    {
        WRAPPER_CONTRACT;
        STATIC_CONTRACT_SO_TOLERANT;

        //Build the cannonical name (convert it to lowercase).
        //Key[0] is the namespace, Key[1] is class name.
        
        pLoader->CreateCanonicallyCasedKey(key[0], key[1], ppszLowerNameSpace, ppszLowerClsName);
    }
    
    ClassLoader *pLoader;
    LPUTF8      *ppszLowerNameSpace;
    LPUTF8      *ppszLowerClsName;
    
};

EEClassHashTable *EEClassHashTable::MakeCaseInsensitiveTable(Module *pModule, AllocMemTracker *pamTracker) {
    EEClassHashEntry_t *pTempEntry;
    LPUTF8         pszLowerClsName;
    LPUTF8         pszLowerNameSpace;
    unsigned int   iRow;

    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
        INJECT_FAULT(COMPlusThrowOM(););

        PRECONDITION(!FORBIDGC_LOADER_USE_ENABLED());
    }
    CONTRACTL_END;



    _ASSERTE(m_pModule);
    _ASSERTE (pModule == m_pModule);

    //Allocate the table and verify that we actually got one.
    //Initialize this table with the same number of buckets
    //that we had initially.
    EEClassHashTable * pCaseInsTable = EEClassHashTable::Create(pModule, m_dwNumBuckets, TRUE /* bCaseInsensitive */, pamTracker);

    //Walk all of the buckets and insert them into our new case insensitive table
    for (iRow=0; iRow<m_dwNumBuckets; iRow++) {
        pTempEntry = m_pBuckets[iRow];

        while (pTempEntry) {
            ConstructKeyCallbackCaseInsensitive cback;

            cback.pLoader            = pModule->GetClassLoader();
            cback.ppszLowerNameSpace = &pszLowerNameSpace;
            cback.ppszLowerClsName   = &pszLowerClsName;
            ConstructKeyFromData(pTempEntry, &cback);

            //Add the newly created name to our hash table.  The hash datum is a pointer
            //to the entry associated with that name in this hashtable.
            pCaseInsTable->InsertValue(pszLowerNameSpace, pszLowerClsName, (HashDatum)pTempEntry, pTempEntry->pEncloser, pamTracker);
            
            //Get the next entry.
            pTempEntry = pTempEntry->pNext;
        }
    }

    return pCaseInsTable;
}

#endif // #ifndef DACCESS_COMPILE

#ifdef DACCESS_COMPILE

void
EEClassHashTable::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    DAC_ENUM_DTHIS();
    
    DacEnumMemoryRegion(PTR_TO_TADDR(m_pBuckets),
                        m_dwNumBuckets * sizeof(PTR_EEClassHashEntry));
    if (m_pBuckets.IsValid())
    {
        for (DWORD i = 0; i < m_dwNumBuckets; i++)
        {
            PTR_EEClassHashEntry entry = m_pBuckets[i];
            while (entry.IsValid())
            {
                entry.EnumMem();
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
