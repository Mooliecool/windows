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
// File: ListLock.cpp
//
// ===========================================================================
// This file decribes the list lock and deadlock aware list lock.
// ===========================================================================

#include "common.h"
#include "listlock.h"
#include "listlock.inl"

ListLockEntry::ListLockEntry(ListLock *pList, void *pData, const char *description)
  : m_deadlock(description),
    m_pList(pList),
    m_pData(pData),
    m_Crst(description == NULL ? "ListLockEntry" : description,
           CrstListLock,
           (CrstFlags)(CRST_REENTRANCY | (pList->IsHostBreakable()?CRST_HOST_BREAKABLE:0))),
    m_pNext(NULL),
    m_dwRefCount(1),
    m_hrResultCode(S_FALSE),
    m_hInitException(NULL)
{
    WRAPPER_CONTRACT;
}

ListLockEntry *ListLockEntry::Find(ListLock* pLock, LPVOID pPointer, const char *description)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    _ASSERTE(pLock->HasLock());

    ListLockEntry *pEntry = pLock->Find(pPointer);
    if (pEntry==NULL)
    {
        pEntry = new ListLockEntry(pLock, pPointer, description);
        pLock->AddElement(pEntry);
    }
    else
        pEntry->AddRef();

    return pEntry;
};

void ListLockEntry::AddRef()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(this));
    }
    CONTRACTL_END;

    FastInterlockIncrement((LONG*)&m_dwRefCount);
}

void ListLockEntry::Release()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
        PRECONDITION(CheckPointer(this));
    }
    CONTRACTL_END;

    ListLockHolder lock(m_pList);

    if (FastInterlockDecrement((LONG*)&m_dwRefCount) == 0)
    {
        // Remove from list
        m_pList->Unlink(this);
        delete this;
    }
};

