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
// File: hash.cpp
//
//*****************************************************************************
#include "stdafx.h"

/* ------------------------------------------------------------------------- *
 * Hash Table class
 * ------------------------------------------------------------------------- */

CordbHashTable::~CordbHashTable()
{
    HASHFIND    find;

    for (CordbHashEntry *entry = (CordbHashEntry *) FindFirstEntry(&find);
         entry != NULL;
         entry = (CordbHashEntry *) FindNextEntry(&find))
        entry->pBase->InternalRelease();
}

HRESULT CordbHashTable::UnsafeAddBase(CordbBase *pBase)
{
    AssertIsProtected();
    DbgIncChangeCount();

    HRESULT hr = S_OK;

    if (!m_initialized)
    {
        HRESULT res = NewInit(m_iBuckets, sizeof(CordbHashEntry), 0xffff);

        if (res != S_OK)
        {
            return res;
        }

        m_initialized = true;
    }

    CordbHashEntry *entry = (CordbHashEntry *) Add(HASH((ULONG_PTR)pBase->m_id));

    if (entry == NULL)
    {
        hr = E_FAIL;
    }
    else
    {
        entry->pBase = pBase;
        m_count++;
        pBase->InternalAddRef();
    }
    return hr;
}
bool CordbHashTable::IsInitialized()
{
    return m_initialized;
}

CordbBase *CordbHashTable::UnsafeGetBase(ULONG_PTR id, BOOL fFab)
{
    AssertIsProtected();

    CordbHashEntry *entry = NULL;

    if (!m_initialized)
        return (NULL);

    entry = (CordbHashEntry *) Find(HASH((ULONG_PTR)id), KEY((BYTE *)id));
    return (entry ? entry->pBase : NULL);
}

HRESULT CordbHashTable::UnsafeSwapBase(CordbBase *pOldBase, CordbBase *pNewBase)
{
    if (!m_initialized)
        return E_FAIL;

    AssertIsProtected();
    DbgIncChangeCount();

    ULONG_PTR id = (ULONG_PTR)pOldBase->m_id;

    CordbHashEntry *entry
      = (CordbHashEntry *) Find(HASH((ULONG_PTR)id), KEY((BYTE *)(ULONG_PTR)id));

    if (entry == NULL)
    {
        return E_FAIL;
    }

    _ASSERTE(entry->pBase == pOldBase);
    entry->pBase = pNewBase;

    // release the hash table's reference to the old base and transfer it
    // to the new one.
    pOldBase->InternalRelease();
    pNewBase->InternalAddRef();

    return S_OK;
}


CordbBase *CordbHashTable::UnsafeRemoveBase(ULONG_PTR id)
{
    AssertIsProtected();

    if (!m_initialized)
        return NULL;

    DbgIncChangeCount();


    CordbHashEntry *entry
      = (CordbHashEntry *) Find(HASH((ULONG_PTR)id), KEY((BYTE *)(ULONG_PTR)id));

    if (entry == NULL)
    {
        return NULL;
    }

    CordbBase *base = entry->pBase;

    Delete(HASH((ULONG_PTR)id), (HASHENTRY *) entry);
    m_count--;
    base->InternalRelease();

    return base;
}

CordbBase *CordbHashTable::UnsafeFindFirst(HASHFIND *find)
{
    AssertIsProtected();
    return UnsafeUnlockedFindFirst(find);
}

CordbBase *CordbHashTable::UnsafeUnlockedFindFirst(HASHFIND *find)
{
    CordbHashEntry *entry = (CordbHashEntry *) FindFirstEntry(find);

    if (entry == NULL)
        return NULL;
    else
        return entry->pBase;
}

CordbBase *CordbHashTable::UnsafeFindNext(HASHFIND *find)
{
    AssertIsProtected();
    return UnsafeUnlockedFindNext(find);
}

CordbBase *CordbHashTable::UnsafeUnlockedFindNext(HASHFIND *find)
{
    CordbHashEntry *entry = (CordbHashEntry *) FindNextEntry(find);

    if (entry == NULL)
        return NULL;
    else
        return entry->pBase;
}

/* ------------------------------------------------------------------------- *
 * Hash Table Enumerator class
 * ------------------------------------------------------------------------- */

CordbHashTableEnum::CordbHashTableEnum(
    CordbBase * pOwnerObj, NeuterList * pOwnerList,
    CordbHashTable *table,
    REFIID guid
)
  : CordbBase(pOwnerObj->GetProcess(), 0, enumCordbHashTableEnum),
    m_pOwnerObj(pOwnerObj),
    m_pOwnerNeuterList(pOwnerList),
    m_table(table),
    m_started(false),
    m_done(false),
    m_guid(guid),
    m_iCurElt(0),
    m_count(0),
    m_fCountInit(FALSE)
{
    // If no neuter-list supplied, then our owner is manually managing us.
    // It also means we can't be cloned.
    if (m_pOwnerNeuterList != NULL)
    {
        m_pOwnerNeuterList->Add(this);
    }
#ifdef _DEBUG
    m_DbgChangeCount = m_table->GetChangeCount();
#endif
}

// Only for cloning.
// Copy constructor makes life easy & fun!
CordbHashTableEnum::CordbHashTableEnum(CordbHashTableEnum *cloneSrc)
  : CordbBase(cloneSrc->m_pOwnerObj->GetProcess(), 0, enumCordbHashTableEnum),
    m_pOwnerObj(cloneSrc->m_pOwnerObj),
    m_pOwnerNeuterList(cloneSrc->m_pOwnerNeuterList),
    m_started(cloneSrc->m_started),
    m_done(cloneSrc->m_done),
    m_hashfind(cloneSrc->m_hashfind),
    m_guid(cloneSrc->m_guid),
    m_iCurElt(cloneSrc->m_iCurElt),
    m_count(cloneSrc->m_count),
    m_fCountInit(cloneSrc->m_fCountInit)
{
    // We can get cloned at any time, so our owner can't manually control us,
    // so we need explicit access to a neuter list.
    _ASSERTE(m_pOwnerNeuterList != NULL);

    m_table = cloneSrc->m_table;

    // Add to neuter list
    if (m_pOwnerObj->GetProcess() != NULL)
    {
        // Normal case. For things enumerating stuff within a CordbProcess tree.
        m_pOwnerNeuterList->Add(this);
    }
    else
    {
        m_pOwnerNeuterList->UnsafeAdd(this);
    }


#ifdef _DEBUG
    m_DbgChangeCount = cloneSrc->m_DbgChangeCount;
#endif
}

CordbHashTableEnum::~CordbHashTableEnum()
{
    _ASSERTE(this->IsNeutered());

    _ASSERTE(m_table == NULL);
    _ASSERTE(m_pOwnerObj == NULL);
    _ASSERTE(m_pOwnerNeuterList == NULL);
}

void CordbHashTableEnum::Neuter(NeuterTicket ticket)
{
    m_table = NULL;
    m_pOwnerObj = NULL;
    m_pOwnerNeuterList = NULL;

    CordbBase::Neuter(ticket);
}


HRESULT CordbHashTableEnum::Reset()
{
    HRESULT hr = S_OK;

    m_started = false;
    m_done = false;

    return hr;
}

HRESULT CordbHashTableEnum::Clone(ICorDebugEnum **ppEnum)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    AssertValid();

    VALIDATE_POINTER_TO_OBJECT(ppEnum, ICorDebugEnum **);

    HRESULT hr;
    hr = S_OK;

    CordbHashTableEnum *e;

    CordbProcess * pProc = GetProcess();

    if (pProc != NULL)
    {
        ATT_REQUIRE_STOPPED_MAY_FAIL(pProc);
        e = new (nothrow) CordbHashTableEnum(this);
    }
    else
    {
        e = new (nothrow) CordbHashTableEnum(this);

    }

    if (e == NULL)
    {
        (*ppEnum) = NULL;
        hr = E_OUTOFMEMORY;
        goto LExit;
    }

    e->QueryInterface(m_guid, (void **) ppEnum);

LExit:
    return hr;
}

HRESULT CordbHashTableEnum::GetCount(ULONG *pcelt)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    AssertValid();

    VALIDATE_POINTER_TO_OBJECT(pcelt, ULONG *);

    *pcelt = m_table->GetCount();

    return S_OK;
}

HRESULT CordbHashTableEnum::PrepForEnum(CordbBase **pBase)
{
    HRESULT hr = S_OK;

    if (!m_started)
    {
        (*pBase) = m_table->UnsafeUnlockedFindFirst(&m_hashfind);
        m_started = true;
    }
    else
    {
        (*pBase) = m_table->UnsafeUnlockedFindNext(&m_hashfind);
    }

    return hr;
}

HRESULT CordbHashTableEnum::SetupModuleEnum(void)
{
    return S_OK;
}


HRESULT CordbHashTableEnum::AdvancePreAssign(CordbBase **pBase)
{
    HRESULT hr = S_OK;
    return hr;
}

HRESULT CordbHashTableEnum::AdvancePostAssign(CordbBase **pBase,
                                              CordbBase     **b,
                                              CordbBase   **bEnd)
{
    CordbBase *base;

    if (pBase == NULL)
        pBase = &base;

    // If we're looping like normal, or we're in skip
    if ( ((b < bEnd) || ((b ==bEnd)&&(b==NULL)))
       )
    {
        (*pBase) = m_table->UnsafeUnlockedFindNext(&m_hashfind);
        if (*pBase == NULL)
           m_done = true;
    }

    return S_OK;
}

// This is an public function implementing all of the ICorDebugXXXEnum interfaces.
HRESULT CordbHashTableEnum::Next(ULONG celt,
                                 CordbBase *bases[],
                                 ULONG *pceltFetched)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    AssertValid();

    VALIDATE_POINTER_TO_OBJECT_ARRAY(bases,
                                     CordbBase *,
                                     celt,
                                     true,
                                     true);
    VALIDATE_POINTER_TO_OBJECT_OR_NULL(pceltFetched,
                                       ULONG *);

    if ((pceltFetched == NULL) && (celt != 1))
    {
        return E_INVALIDARG;
    }

    if (celt == 0)
    {
        if (pceltFetched != NULL)
        {
            *pceltFetched = 0;
        }
        return S_OK;
    }

    HRESULT         hr      = S_OK;
    CordbBase      *base    = NULL;
    CordbBase     **b       = bases;
    CordbBase     **bEnd    = bases + celt;

    hr = PrepForEnum(&base);
    if (FAILED(hr))
    {
        goto LError;
    }

    while (b < bEnd && !m_done)
    {
        hr = AdvancePreAssign(&base);
        if (FAILED(hr))
        {
            goto LError;
        }

        if (base == NULL)
        {
            m_done = true;
        }
        else
        {
            if (m_guid == IID_ICorDebugProcessEnum)
            {
                *b = (CordbBase*)(ICorDebugProcess*)(CordbProcess*)base;
            }
            else if (m_guid == IID_ICorDebugBreakpointEnum)
            {
                *b = (CordbBase*)(ICorDebugBreakpoint*)(CordbBreakpoint*)base;
            }
            else if (m_guid == IID_ICorDebugStepperEnum)
            {
                *b = (CordbBase*)(ICorDebugStepper*)(CordbStepper*)base;
            }
            else if (m_guid == IID_ICorDebugModuleEnum)
            {
                *b = (CordbBase*)(ICorDebugModule*)(CordbModule*)base;
            }
            else if (m_guid == IID_ICorDebugThreadEnum)
            {
                *b = (CordbBase*)(ICorDebugThread*)(CordbThread*)base;
            }
            else if (m_guid == IID_ICorDebugAppDomainEnum)
            {
                *b = (CordbBase*)(ICorDebugAppDomain*)(CordbAppDomain*)base;
            }
            else if (m_guid == IID_ICorDebugAssemblyEnum)
            {
                *b = (CordbBase*)(ICorDebugAssembly*)(CordbAssembly*)base;
            }
            else
            {
                *b = (CordbBase*)(IUnknown*)base;
            }

            if (*b)
            {
                // 'b' is not a valid CordbBase ptr.
                base->ExternalAddRef();
                b++;
            }

            hr = AdvancePostAssign(&base, b, bEnd);
            if (FAILED(hr))
            {
                goto LError;
            }
        }
    }

LError:
    //
    // If celt == 1, then the pceltFetched parameter is optional.
    //
    if (pceltFetched != NULL)
    {
        *pceltFetched = (ULONG)(b - bases);
    }

    //
    // If we reached the end of the enumeration, but not the end
    // of the number of requested items, we return S_FALSE.
    //
    if (!FAILED(hr) && m_done && (b != bEnd))
    {
        hr = S_FALSE;
    }

    return hr;
}

HRESULT CordbHashTableEnum::Skip(ULONG celt)
{
    PUBLIC_REENTRANT_API_ENTRY(this);
    FAIL_IF_NEUTERED(this);
    AssertValid();

    HRESULT hr = S_OK;

    CordbBase   *base;

    if (celt > 0)
    {
        if (!m_started)
        {
            base = m_table->UnsafeUnlockedFindFirst(&m_hashfind);

            if (base == NULL)
                m_done = true;
            else
                celt--;

            m_started = true;
        }

        while (celt > 0 && !m_done)
        {
            base = m_table->UnsafeUnlockedFindNext(&m_hashfind);

            if (base == NULL)
                m_done = true;
            else
                celt--;
        }
    }

    return hr;
}

HRESULT CordbHashTableEnum::QueryInterface(REFIID id, void **pInterface)
{
    if (id == IID_ICorDebugEnum)
    {
        ExternalAddRef();
        *pInterface = static_cast<ICorDebugEnum *>(static_cast<ICorDebugProcessEnum *>(this));

        return S_OK;
    }
    if (id == IID_IUnknown)
    {
        ExternalAddRef();
        *pInterface = static_cast<IUnknown *>(static_cast<ICorDebugProcessEnum *>(this));

        return S_OK;
    }
    if (id == m_guid)
    {
        ExternalAddRef();

        if (id == IID_ICorDebugProcessEnum)
            *pInterface = static_cast<ICorDebugProcessEnum *>(this);
        else if (id == IID_ICorDebugBreakpointEnum)
            *pInterface = static_cast<ICorDebugBreakpointEnum *>(this);
        else if (id == IID_ICorDebugStepperEnum)
            *pInterface = static_cast<ICorDebugStepperEnum *>(this);
        else if (id == IID_ICorDebugModuleEnum)
            *pInterface = static_cast<ICorDebugModuleEnum *>(this);
        else if (id == IID_ICorDebugThreadEnum)
            *pInterface = static_cast<ICorDebugThreadEnum *>(this);
        else if (id == IID_ICorDebugAppDomainEnum)
            *pInterface = static_cast<ICorDebugAppDomainEnum *>(this);
        else if (id == IID_ICorDebugAssemblyEnum)
            *pInterface = static_cast<ICorDebugAssemblyEnum *>(this);

        return S_OK;
    }

    return E_NOINTERFACE;
}

#ifdef _DEBUG
void CordbHashTableEnum::AssertValid()
{
    //
}


void CordbHashTable::AssertIsProtected()
{
#ifdef RSCONTRACTS
    if (m_pDbgLock != NULL)
    {
        DbgRSThread * pThread = DbgRSThread::GetThread();
        if (pThread->IsInRS())
        {
            CONSISTENCY_CHECK_MSGF(m_pDbgLock->HasLock(), ("Hash table being accessed w/o holding '%s'", m_pDbgLock->Name()));
        }
    }
#endif
}
#endif
