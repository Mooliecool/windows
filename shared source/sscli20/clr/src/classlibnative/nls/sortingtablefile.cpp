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

#include <winnls.h>

#include "nlstable.h"
#include "globalizationassembly.h"
#include "sortingtablefile.h"
#include "sortingtable.h"

#include "excep.h"
#include "holder.h"

/** 
 * There are two data table for NativeCompareInfo.  One is sortkey.nlp, which contains the default
 * sortkey information.
 * The other one is sorttbls.nlp, which contains other sorting information for all cultures.
 *
 */

const CHAR  SortingTable::m_szSortKeyFileName[]       = "sortkey.nlp";
const CHAR  SortingTable::m_szSortTableFileName[]     = "sorttbls.nlp";
const WCHAR SortingTable::m_szExceptionSectionName[]  = L"NLS_00000000_Exception_Table_3_2";

SortingTable::SortingTable(NativeGlobalizationAssembly* pNativeGlobalizationAssembly) :
    m_NumReverseDW(0), m_NumDblCompression(0), m_NumIdeographLcid(0), m_NumExpansion(0),
    m_NumCompression(0), m_NumException(0), m_NumMultiWeight(0),
    m_pReverseDW(NULL),  m_pDblCompression(NULL),  m_pIdeographLcid(NULL), m_pExpansion(NULL),
    m_pCompressHdr(NULL), m_pCompression(NULL),
    m_pExceptHdr(NULL), m_pException(NULL), m_pMultiWeight(NULL),
    m_pNativeGlobalizationAssembly(pNativeGlobalizationAssembly),
    m_pDefaultSortKeyTable(NULL),
    m_pSortTable(NULL)
{
    WRAPPER_CONTRACT;
    ZeroMemory(m_pNativeCompareInfoCache, sizeof(m_pNativeCompareInfoCache));
    
    // Get the necessay information that is global to all cultures.
    GetSortInformation();
}

SortingTable::~SortingTable() {
    LEAF_CONTRACT;
}


/*===========================InitializeNativeCompareInfo=============================
**Action: Ensure that the correct sorting table for a given locale has been allocated.
**        This function is called from within a synchronized method from managed, so
**        there should never be more than one thread in here at any one time.  If
**        the table can't be found in the cache, we allocate another one and put it
**        into the cache.
**        The end result is that a NativeCompareInfo instance for a particular culture will not 
**        be created twice.
**Returns: The pointer to the created NativeCompareInfo.  
**        The side effect is to either allocate the table or do nothing if the
**         correct table already exists.
**Arguments:  nLcid -- the lcid for which we're creating the table.
**Exceptions: OutOfMemory if new fails.
**            OutOfMemoryException if we can't find the resource in the SortingTable
**            constructor.
==============================================================================*/
NativeCompareInfo* SortingTable::InitializeNativeCompareInfo(INT32 nLcid) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;
    
    MEMORY_REPORT_CONTEXT_SCOPE("NLS");

    //The cultureID should have been checked when the CompareInfo was created, but
    //we'll double check it here.

    //
    // m_SortingTableOffset[PRIMARYLANGID(nLcid)] points to the slot for a certain primary language.
    // SUBLANGID(nLcid) provides the index within this slot.
    //

    // We access a global variable (m_pNativeCompareInfoCache), so this is why this method should be
    // synchronized.
    int slot = nLcid % COMPARE_INFO_HASH_PRIME;

    NativeCompareInfo* pTable = m_pNativeCompareInfoCache[slot];

    if (pTable == NULL) {
        //
        // This entry is empty.  Create a NativeCompareInfo corresponding to the nLcid.
        //
        // An exception will be throw if OOM happens in the following line.
        NewHolder<NativeCompareInfo> pTemp(new NativeCompareInfo(nLcid, this));

        if (!(pTemp->InitSortingData())) {
            // Failed to initialize sorting data
            return (NULL);
        }
        pTemp.SuppressRelease();

        m_pNativeCompareInfoCache[slot] = pTemp;

        return (m_pNativeCompareInfoCache[slot]);
    } 
    //
    // Search through the list of NativeCompareInfo in this entry until find one matching the nLcid.
    // If one cannot be found, create a new one and link it with the previous node in this entry.
    //
    NativeCompareInfo* pPrevTable;
    do {
        if (pTable->m_nLcid == nLcid) {
            //
            // The NativeCompareInfo instance for this nLcid has been created, so our mission
            // is done here.
            //
            return (pTable);
        }
        pPrevTable = pTable;
        pTable = pTable->m_pNext;
    } while (pTable != NULL);

    //
    // The NativeCompareInfo for this nLcid has not been created yet.  Create one and link
    // it with the previous node.
    //
    // An exception will be throw if OOM happens in the following line.
    NewHolder<NativeCompareInfo> pTemp(new NativeCompareInfo(nLcid, this));

    if (!(pTemp->InitSortingData())) {
        // Failed to initialize sorting data
        return (NULL);
    }

    pTemp.SuppressRelease();
    pPrevTable->m_pNext = pTemp;
            
    return(pTemp);
}


/*=============================SortingTableShutdown=============================
**Action: Clean up any statically allocated resources during EE Shutdown.  We need
**        to clean the SortTable                                              and then walk our
**        cache cleaning up any SortingTables.
**Returns: True.  Eventually designed for error checking, but we don't do any right now.
**Arguments:  None
**Exceptions: None.
==============================================================================*/


/*============================GetSortInformation============================
**Action: Get the information that is global to all locales.  The information includes:
**        1. reverse diacritic information: which locales uses diacritic.
**        2. double compression information: which locales uses double compression.
**        3. ideographic locale exception: the mapping of ideographic locales (CJK) to extra sorting files.
**        4. expansion information: expansion characters and their expansion forms.
**        5. compression information:
**        6. exception information: which locales has exception, and their exception entries.
**        7. multiple weight information: what is this?
**        This operation must happen at most once per instance of the runtime.  We
**        guarantee this by allocating it in the class initializer of System.CompareInfo.
**Returns: Void.  The side effect is to allocate the cache as a member of SortingTable.
**Arguments:  None
**Exceptions: None.
==============================================================================*/
void SortingTable::GetSortInformation()
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;


    PCOMPRESS_HDR pCompressHdr;   // ptr to compression header
    PEXCEPT_HDR pExceptHdr;       // ptr to exception header
    LPWORD pBaseAddr;             // ptr to the current location in the data file.

    m_pSortTable = pBaseAddr = (LPWORD)m_pNativeGlobalizationAssembly->GetResource(m_szSortTableFileName);

    //
    //  Get Reverse Diacritic Information.
    //
    m_NumReverseDW   = *((LPDWORD)pBaseAddr);
    _ASSERTE(m_NumReverseDW > 0);
    m_pReverseDW     = (PREVERSE_DW)(pBaseAddr + REV_DW_HEADER);

    pBaseAddr += REV_DW_HEADER + (m_NumReverseDW * (sizeof(REVERSE_DW) / sizeof(WORD)));

    //
    //  Get Double Compression Information.
    //
    m_NumDblCompression = *((LPDWORD)pBaseAddr);
    _ASSERTE(m_NumDblCompression > 0);
    m_pDblCompression   = (PDBL_COMPRESS)(pBaseAddr + DBL_COMP_HEADER);
    pBaseAddr += DBL_COMP_HEADER + (m_NumDblCompression * (sizeof(DBL_COMPRESS) / sizeof(WORD)));

    //
    //  Get Ideograph Lcid Exception Information.
    //
    m_NumIdeographLcid = *((LPDWORD)pBaseAddr);
    _ASSERTE(m_NumIdeographLcid > 0);
    m_pIdeographLcid   = (PIDEOGRAPH_LCID)(pBaseAddr + IDEO_LCID_HEADER);
    pBaseAddr += IDEO_LCID_HEADER + (m_NumIdeographLcid * (sizeof(IDEOGRAPH_LCID) / sizeof(WORD)));

    //
    //  Get Expansion Information.
    //
    m_NumExpansion   = *((LPDWORD)pBaseAddr);
    _ASSERTE(m_NumExpansion > 0);
    m_pExpansion     = (PEXPAND)(pBaseAddr + EXPAND_HEADER);
    pBaseAddr += EXPAND_HEADER + (m_NumExpansion * (sizeof(EXPAND) / sizeof(WORD)));

    //
    //  Get Compression Information.
    //
    m_NumCompression = *((LPDWORD)pBaseAddr);
    _ASSERTE(m_NumCompression > 0);
    m_pCompressHdr   = (PCOMPRESS_HDR)(pBaseAddr + COMPRESS_HDR_OFFSET);
    m_pCompression   = (PCOMPRESS)(pBaseAddr + COMPRESS_HDR_OFFSET +
                                 (m_NumCompression * (sizeof(COMPRESS_HDR) /
                                         sizeof(WORD))));

    pCompressHdr = m_pCompressHdr;
    pBaseAddr = (LPWORD)(m_pCompression) +
                        (pCompressHdr[m_NumCompression - 1]).Offset;

    pBaseAddr += (((pCompressHdr[m_NumCompression - 1]).Num2) *
                  (sizeof(COMPRESS_2) / sizeof(WORD)));

    pBaseAddr += (((pCompressHdr[m_NumCompression - 1]).Num3) *
                  (sizeof(COMPRESS_3) / sizeof(WORD)));

    //
    //  Get Exception Information.
    //
    m_NumException = *((LPDWORD)pBaseAddr);
    _ASSERTE(m_NumException > 0);
    m_pExceptHdr   = (PEXCEPT_HDR)(pBaseAddr + EXCEPT_HDR_OFFSET);
    m_pException   = (PEXCEPT)(pBaseAddr + EXCEPT_HDR_OFFSET +
                               (m_NumException * (sizeof(EXCEPT_HDR) /
                                       sizeof(WORD))));
    pExceptHdr = m_pExceptHdr;
    pBaseAddr = (LPWORD)(m_pException) +
                        (pExceptHdr[m_NumException - 1]).Offset;
    pBaseAddr += (((pExceptHdr[m_NumException - 1]).NumEntries) *
                  (sizeof(EXCEPT) / sizeof(WORD)));

    //
    //  Get Multiple Weights Information.
    //
    m_NumMultiWeight = *pBaseAddr;
    _ASSERTE(m_NumMultiWeight > 0);
    m_pMultiWeight   = (PMULTI_WT)(pBaseAddr + MULTI_WT_HEADER);

    pBaseAddr += (MULTI_WT_HEADER + m_NumMultiWeight * sizeof(MULTI_WT)/sizeof(WORD));

    //
    // Get Jamo Index Table.
    //
    
    m_NumJamoIndex = (DWORD)(*pBaseAddr);   // The Jamo Index table size is (Num) bytes.
    m_pJamoIndex = (PJAMO_TABLE)(pBaseAddr + JAMO_INDEX_HEADER);
    
    pBaseAddr += (m_NumJamoIndex * sizeof(JAMO_TABLE) / sizeof(WORD) + JAMO_INDEX_HEADER);
    
    //
    // Get Jamo Composition state machine table.
    //
    m_NumJamoComposition = (DWORD)(*pBaseAddr);
    m_pJamoComposition = (PJAMO_COMPOSE_STATE)(pBaseAddr + JAMO_COMPOSITION_HEADER);

}



/*============================GetDefaultSortKeyTable============================
**Action: Allocates the default sortkey table if it hasn't already been allocated.
**        This allocates resources, so it needs to be called in a synchronized fasion.
**        We guarantee this by making the managed method that accesses this codepath
**        synchronized.  If you're calling this from someplace besides SortingTable::SortingTable
**        or SortingTable::GetExceptionSortKeyTable, make sure that you haven't broken
**        any invariants.
**Returns:    A pointer to the default sorting table.
**Arguments:  None
**Exceptions: GetResource can throw an OutOfMemoryException if the needed data file can't
**            be found.
==============================================================================*/
PSORTKEY SortingTable::GetDefaultSortKeyTable() {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
    } CONTRACTL_END;

    if (m_pDefaultSortKeyTable == NULL)
    {
        //
        // Skip the first DWORD since it is the semaphore value.
        //
        m_pDefaultSortKeyTable = (PSORTKEY)((LPWORD)m_pNativeGlobalizationAssembly->GetResource(
            m_szSortKeyFileName) + SORTKEY_HEADER);
    }

    return (m_pDefaultSortKeyTable);
}

PSORTKEY SortingTable::GetSortKey(int nLcid, HANDLE* phSortKey) {
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(phSortKey));
    } CONTRACTL_END;

    PEXCEPT_HDR pExceptHdr;       // ptr to exception header
    PEXCEPT pExceptTbl;           // ptr to exception table
    PVOID pIdeograph;             // ptr to ideograph exception table

    // No handle to start with
    *phSortKey = NULL;

    // If this is not a fast compare locale, try to find if it has exception pointers.
    if (!IS_FAST_COMPARE_LOCALE(nLcid) 
        && FindExceptionPointers(nLcid, &pExceptHdr, &pExceptTbl, &pIdeograph)) {
        // Yes, exceptions exist.  Get the table with exceptions.
        return (GetExceptionSortKeyTable(nLcid, pExceptHdr, pExceptTbl, pIdeograph, phSortKey));
    }
    
    //
    //  No exceptions for locale, so attach the default sortkey
    //  table pointer to the this locale.
    //
    return (GetDefaultSortKeyTable());
}


PSORTKEY SortingTable::GetExceptionSortKeyTable(
    int         nLCID,              // LCID
    PEXCEPT_HDR pExceptHdr,         // ptr to exception header
    PEXCEPT     pExceptTbl,         // ptr to exception table
    PVOID       pIdeograph,         // ptr to ideograph exception table
    HANDLE *    pMapHandle          // ptr to the handle for the file mapping.

)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pExceptHdr, NULL_OK));
        PRECONDITION(CheckPointer(pExceptTbl, NULL_OK));
        PRECONDITION(CheckPointer(pIdeograph, NULL_OK));
        PRECONDITION(CheckPointer(pMapHandle)); 
    } CONTRACTL_END;

    _ASSERTE(pMapHandle);

    WCHAR sectionName[COUNTOF(m_szExceptionSectionName)];
    wcscpy_s(sectionName, COUNTOF(sectionName), m_szExceptionSectionName);

    // Move to end of 00000000 in padded name and copy hex digits of LCID into
    // the 000000000 space from right to left.
    WCHAR* temp = sectionName + 11;
    while (nLCID != 0)
    {
        if ((nLCID & 0xf) >= 10)  
            *(temp--) = 'A' - 10 + (nLCID & 0xf);
        else
            *(temp--) = '0' + (nLCID & 0xf);
        nLCID>>=4;
    }
      
    int defaultLen = sizeof(SORTKEY) * (65536 + SORTKEY_HEADER) + 4; //This evaluates to 64K Unicode Characters.  Add 4 for size of flag

    LPWORD pBaseAddr = (LPWORD)NLSTable::OpenOrCreateMemoryMapping(sectionName, defaultLen, pMapHandle);

    if (pBaseAddr == NULL || ((BYTE*)(pBaseAddr))[defaultLen-4] == 0xff)
        return (PSORTKEY)(pBaseAddr);

    // Copy it
    CopyMemory((LPVOID)pBaseAddr, (LPVOID)GetDefaultSortKeyTable(), defaultLen);

    //
    //  Copy exception information to the table.
    //
    CopyExceptionInfo( (PSORTKEY)(pBaseAddr),
                       pExceptHdr,
                       pExceptTbl,
                       pIdeograph);

    ((BYTE*)(pBaseAddr))[defaultLen-4] = 0xff;    // Flag so we don't build more sections
    
    return ((PSORTKEY)pBaseAddr);
}

////////////////////////////////////////////////////////////////////////////
//
//  FindExceptionPointers
//
//  Checks to see if any exceptions exist for the given locale id.  If
//  exceptions exist, then TRUE is returned and the pointer to the exception
//  header and the pointer to the exception table are stored in the given
//  parameters.
//
////////////////////////////////////////////////////////////////////////////

BOOL SortingTable::FindExceptionPointers(
    LCID nLcid,
    PEXCEPT_HDR *ppExceptHdr,
    PEXCEPT *ppExceptTbl,
    PVOID *ppIdeograph)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(ppExceptHdr));
        PRECONDITION(CheckPointer(ppExceptTbl));
        PRECONDITION(CheckPointer(ppIdeograph));
    } CONTRACTL_END;

    DWORD ctr;                         // loop counter
    PEXCEPT_HDR pHdr;                  // ptr to exception header
    BOOL bFound = FALSE;               // if an exception is found

    PIDEOGRAPH_LCID pIdeoLcid;         // ptr to ideograph lcid entry

    //
    //  Initialize pointers.
    //
    *ppExceptHdr = NULL;
    *ppExceptTbl = NULL;
    *ppIdeograph = NULL;

    //
    //  Need to search down the exception header for the given nLcid.
    //
    pHdr = m_pExceptHdr;
    for (ctr = m_NumException; ctr > 0; ctr--, pHdr++)
    {
        if (pHdr->Locale == (DWORD)nLcid)
        {
            //
            //  Found the locale id, so set the pointers.
            //
            *ppExceptHdr = pHdr;
            *ppExceptTbl = (PEXCEPT)(((LPWORD)(m_pException)) +
                                     pHdr->Offset);

            //
            //  Set the return code to show that an exception has been
            //  found.
            //
            bFound = TRUE;
            break;
        }
    }

    //
    //  Need to search down the ideograph lcid exception list for the
    //  given locale.
    //
    pIdeoLcid = m_pIdeographLcid;
    for (ctr = m_NumIdeographLcid; ctr > 0; ctr--, pIdeoLcid++)
    {
        if (pIdeoLcid->Locale == (DWORD)nLcid)
        {
            //
            //  Found the locale id, so create/open and map the section
            //  for the appropriate file.
            //

            *ppIdeograph = m_pNativeGlobalizationAssembly->GetResource((WCHAR *) pIdeoLcid->pFileName);

            //
            //  Set the return code to show that an exception has been
            //  found.
            //
            bFound = TRUE;
            break;
        }
    }

    //
    //  Return the appropriate value.
    //
    return (bFound);
}


////////////////////////////////////////////////////////////////////////////
//
//  CopyExceptionInfo
//
//  Copies the exception information to the given sortkey table.
//
////////////////////////////////////////////////////////////////////////////

void SortingTable::CopyExceptionInfo(
    PSORTKEY pSortkey,
    PEXCEPT_HDR pExceptHdr,
    PEXCEPT pExceptTbl,
    PVOID pIdeograph)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pSortkey));
        PRECONDITION(CheckPointer(pExceptHdr, NULL_OK));
        PRECONDITION(CheckPointer(pExceptTbl, NULL_OK));
        PRECONDITION(CheckPointer(pIdeograph, NULL_OK));
    } CONTRACTL_END;
    DWORD ctr;                    // loop counter
    PIDEOGRAPH_EXCEPT_HDR pHdrIG; // ptr to ideograph exception header
    PIDEOGRAPH_EXCEPT pEntryIG;   // ptr to ideograph exception entry
    PEXCEPT pEntryIGEx;           // ptr to ideograph exception entry ex


    //
    //  For each entry in the exception table, copy the information to the
    //  sortkey table.
    //
    if (pExceptTbl)
    {
        for (ctr = pExceptHdr->NumEntries; ctr > 0; ctr--, pExceptTbl++)
        {
            (pSortkey[pExceptTbl->UCP]).UW.Unicode = pExceptTbl->Weights.UW.Unicode;
            (pSortkey[pExceptTbl->UCP]).Diacritic  = pExceptTbl->Weights.Diacritic;
            (pSortkey[pExceptTbl->UCP]).Case       = pExceptTbl->Weights.Case;
        }
    }

    //
    //  For each entry in the ideograph exception table, copy the
    //  information to the sortkey table.
    //
    if (pIdeograph)
    {
        pHdrIG = (PIDEOGRAPH_EXCEPT_HDR)pIdeograph;
        ctr = pHdrIG->NumEntries;

        if (pHdrIG->NumColumns == 2)
        {
            pEntryIG = (PIDEOGRAPH_EXCEPT)( ((LPBYTE)pIdeograph) +
                                            sizeof(IDEOGRAPH_EXCEPT_HDR) );
            for (; ctr > 0; ctr--, pEntryIG++)
            {
                (pSortkey[pEntryIG->UCP]).UW.Unicode = pEntryIG->Unicode;
            }
        }
        else
        {
            pEntryIGEx = (PEXCEPT)( ((LPBYTE)pIdeograph) +
                                    sizeof(IDEOGRAPH_EXCEPT_HDR) );
            for (; ctr > 0; ctr--, pEntryIGEx++)
            {
                (pSortkey[pEntryIGEx->UCP]).UW.Unicode = pEntryIGEx->Weights.UW.Unicode;
                (pSortkey[pEntryIGEx->UCP]).Diacritic  = pEntryIGEx->Weights.Diacritic;
                (pSortkey[pEntryIGEx->UCP]).Case       = pEntryIGEx->Weights.Case;
            }
        }
    }
}
