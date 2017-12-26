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

NativeGlobalizationAssembly* NativeGlobalizationAssembly::m_pHead = NULL;
NativeGlobalizationAssembly* NativeGlobalizationAssembly::m_pCurrent = NULL;
/*=================================AddToList==========================
**Action: Add the newly created NativeGlobalizationAssembly into a linked list.
**Returns: Void
**Arguments:
**      pNGA    the newly created NativeGlobalizationAssembly
**Exceptions: None.
**Notes:
**      When a new instance of NativeGlobalizationAssembly is created, you should 
**      call this method to add the instance to a linked list.
**      When the runtime is shutdown, we will use this linked list to shutdown
**      every instances of NativeGlobalizationAssembly.
============================================================================*/
void NativeGlobalizationAssembly::AddToList(NativeGlobalizationAssembly* pNGA) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(pNGA));
    } CONTRACTL_END;

    if (m_pHead == NULL) {
        // This is the first node of the linked list.
        m_pCurrent = m_pHead = pNGA;
    } else {
        // Otherwise, link the previous node to the current node.
        m_pCurrent->m_pNext = pNGA;
        m_pCurrent = pNGA;
    }
}

/*==========================FindGlobalizationAssembly===========================
**Action:
**Returns:
**Arguments:
**Exceptions:
==============================================================================*/
NativeGlobalizationAssembly *NativeGlobalizationAssembly::FindGlobalizationAssembly(Assembly *targetAssembly) {
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_COOPERATIVE;
        PRECONDITION(CheckPointer(targetAssembly));
    } CONTRACTL_END;
    NativeGlobalizationAssembly *pNode = m_pHead;
    while (pNode!=NULL) {
        if (targetAssembly==pNode->m_pAssembly) {
            return pNode;
        }
        pNode = pNode->m_pNext;
        //Remove this assert in vNext. 
        //However, if you see it in this version, it means that we're allocating
        //too much memory.
        _ASSERTE(pNode==NULL);  
    }
    return NULL;
}

/*=================================ShutDown==========================
**Action: Enumerate every node (which contains instance of NativeGlobalizationAssembly) in the linked list,
**      and call proper shutdown methods for every instance.
**Returns: None.
**Arguments:    None.
**Exceptions:   None.
**Notes:
**      When runtime shutdowns, you should call this methods to clean up every instance of 
============================================================================*/




NativeGlobalizationAssembly::NativeGlobalizationAssembly(Assembly* pAssembly) :
    NLSTable(pAssembly)
{
    WRAPPER_CONTRACT;

    MEMORY_REPORT_CONTEXT_SCOPE("NLS");

    // For now, we create SortingTable by default.
    // This is beause SortingTable is the only NLS+ data table to support Assembly versioning.
    // However, if we have more classes (CultureInfo, RegionInfo, etc.) to support versioning.
    // We should create SortingTable on demand.
    m_pSortingTable = new SortingTable(this);
    m_pNext = NULL;
}
