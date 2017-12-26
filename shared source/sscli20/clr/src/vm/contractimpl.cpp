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
// File: CONTRACTIMPL.CPP
//
// CONTRACTIMPL.CPP
// ===========================================================================

#include "common.h"  // Precompiled header

#include "contractimpl.h"
#include "virtualcallstub.h"
#include "decodemd.h"



#ifdef LOGGING 
//----------------------------------------------------------------------------
StubDispatchStats g_sdStats = {0};
#endif // LOGGING

#ifndef DACCESS_COMPILE 

//----------------------------------------------------------------------------
MethodDesc *DispatchSlot::GetMethodDesc()
{
    WRAPPER_CONTRACT;
    if (IsNull())
        return NULL;
    else
        return MethodTable::GetUnknownMethodDescForSlotAddress((SLOT)GetTarget());
}

#endif // !DACCESS_COMPILE


#ifndef DACCESS_COMPILE 



//------------------------------------------------------------------------
void TypeIDMap::Init(UINT32 idStartValue, UINT32 idIncrementValue)
{
    WRAPPER_CONTRACT;
    LockOwner lock = {&m_lock, IsOwnerOfCrst};
    m_hashMap.Init(11, TRUE, &lock);
    m_idProvider.Init(idStartValue, idIncrementValue);
    m_entryCount = 0;
}


#endif // !DACCESS_COMPILE


//------------------------------------------------------------------------
// Returns the ID of the type if found. If not found, returns INVALID_TYPE_ID
UINT32 TypeIDMap::LookupTypeID(PTR_MethodTable pMT)
{
    CONTRACTL {
        NOTHROW;
        SO_TOLERANT;
        PRECONDITION(CheckPointer(GetThread()));
        if (GetThread()->PreemptiveGCDisabled()) { GC_NOTRIGGER; } else { GC_TRIGGERS; }
    } CONTRACTL_END;

    UINT32 id = (UINT32) m_hashMap.LookupValue((UPTR)PTR_TO_TADDR(pMT), 0);
    return id;
}

//------------------------------------------------------------------------
// Returns the ID of the type if found. If not found, returns INVALID_TYPE_ID
PTR_MethodTable TypeIDMap::LookupType(UINT32 id)
{
    CONTRACTL {
        NOTHROW;
        PRECONDITION(CheckPointer(GetThread()));
        if (GetThread()->PreemptiveGCDisabled()) { GC_NOTRIGGER; } else { GC_TRIGGERS; }
        PRECONDITION(id <= MAX_TYPE_ID);
    } CONTRACTL_END;

    if (!m_idProvider.OwnsID(id))
        return NULL;

    UPTR ret = m_hashMap.LookupValue((UPTR)id, 0);
    if (ret == static_cast<UPTR>(INVALIDENTRY))
        return NULL;

    ret <<= 1;

    return PTR_MethodTable(ret);
}

//------------------------------------------------------------------------
// Returns the ID of the type if found. If not found, assigns the ID and
// returns the new ID.
UINT32 TypeIDMap::GetTypeID(PTR_MethodTable pMT)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        PRECONDITION((UINT_PTR)MAX_TYPE_ID < (UINT_PTR)pMT);
    } CONTRACTL_END;

    // Lookup the value.
    UINT32 id = LookupTypeID(pMT);
#ifndef DACCESS_COMPILE 
    // If the value is not in the table, take the lock, get a new ID, and
    // insert the new pair.
    if (id == INVALID_TYPE_ID) {
        // Take the lock
        CrstPreempHolder lh(&m_lock);
        // Check to see if someone beat us to the punch
        id = LookupTypeID(pMT);
        if (id != INVALID_TYPE_ID) {
            return id;
        }
        // Get the next ID
        id = GetNextID();
        CONSISTENCY_CHECK(id <= MAX_TYPE_ID);
        // Insert the pair, with lookups in both directions
        CONSISTENCY_CHECK((((UPTR)pMT) & 0x1) == 0);
        m_hashMap.InsertValue((UPTR)id, ((UPTR)pMT) >> 1);
        m_hashMap.InsertValue((UPTR)pMT, (UPTR)id);
        m_entryCount++;
        CONSISTENCY_CHECK(GetThread()->GetDomain()->IsCompilationDomain() ||
                          LookupType(id) == pMT);
    }
#else // DACCESS_COMPILE
    if (id == INVALID_TYPE_ID)
        DacError(E_FAIL);
#endif // DACCESS_COMPILE
    // Return the ID for this type.
    return id;
}

#ifndef DACCESS_COMPILE 

//------------------------------------------------------------------------
// If TRUE, it points to a matching entry.
// If FALSE, it is at the insertion point.
BOOL DispatchMapBuilder::Find(DispatchMapTypeID typeID, UINT32 slotNumber, Iterator &it)
{
    WRAPPER_CONTRACT;
    for (;it.IsValid(); it.Next()) {
        if (typeID == it.GetTypeID()) {
            if (slotNumber == it.GetSlotNumber()) {
                return TRUE;
            }
            if (slotNumber < it.GetSlotNumber()) {
                return FALSE;
            }
        }
        else if (typeID < it.GetTypeID()) {
            return FALSE;
        }
    }

    return FALSE;
}

//------------------------------------------------------------------------
// If TRUE, contains such an entry.
// If FALSE, no such entry exists.
BOOL DispatchMapBuilder::Contains(DispatchMapTypeID typeID, UINT32 slotNumber)
{
    WRAPPER_CONTRACT;
    Iterator it(this);
    return Find(typeID, slotNumber, it);
}

//------------------------------------------------------------------------
BOOL DispatchMapBuilder::InsertMDMapping(DispatchMapTypeID typeID,
                                         UINT32 slotNumber,
                                         MethodDesc *pMDTarget,
                                         BOOL fVirtualMapping,
                                         BOOL fIsMethodImpl)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    return Insert(typeID, slotNumber, 0, 0, pMDTarget, fVirtualMapping, fIsMethodImpl);
}

//--------------------------------------------------------------------
UINT32 DispatchMapBuilder::Iterator::GetTargetSlot()
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(IsValid());
    CONSISTENCY_CHECK(IsVirtualMapping());
    if (GetTargetMD() != NULL) {
        return EntryNode()->m_pMDTarget->GetSlot();
    }
    else {
        return EntryNode()->m_targetSlotOrIndex;
    }
}


//------------------------------------------------------------------------
DispatchMapBuilderNode *DispatchMapBuilder::NewEntry()
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        INJECT_FAULT(COMPlusThrowOM());
    } CONTRACTL_END;

    return new (m_pAllocator->Alloc(sizeof(DispatchMapBuilderNode))) DispatchMapBuilderNode;
}

//------------------------------------------------------------------------
BOOL DispatchMapBuilder::Insert(DispatchMapTypeID typeID,
                                UINT32 slotNumber,
                                UINT32 chainDelta,
                                UINT32 targetSlotOrIndex,
                                MethodDesc *pMDTarget,
                                BOOL fVirtualMapping,
                                BOOL fIsMethodImpl)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CONSISTENCY_CHECK(!(!fVirtualMapping && typeID.IsThisClass()) ||
                      (pMDTarget == NULL || pMDTarget->GetSlot() == slotNumber));

    // Find a matching entry, or move the iterator to insertion point.
    Iterator it(this);
    BOOL fFound = Find(typeID, slotNumber, it);

    // If we find an existing matching entry, fail.
    if (fFound) {
        return FALSE;
    }

    // Create and initialize a new entry
    DispatchMapBuilderNode *pNew = NewEntry();
    pNew->Init(typeID, slotNumber, chainDelta, targetSlotOrIndex, pMDTarget, fVirtualMapping);
    pNew->SetIsMethodImpl(fIsMethodImpl);

    // Insert at the point of the iterator
    pNew->m_next = NULL;
    if (it.IsValid())
        pNew->m_next = it.EntryNode();
    *(it.EntryNodePtr()) = pNew;
    m_cEntries++;

    return TRUE;
}


// ===========================================================================
// ===========================================================================

//----------------------------------------------------------------------------
DispatchMap::DispatchMap(
                         BYTE *pMap,
                         UINT32 cbMap)
{
    LEAF_CONTRACT;
    CONSISTENCY_CHECK(CheckPointer(pMap));

    memcpyNoGCRefs(m_rgMap, pMap, cbMap);
}

//----------------------------------------------------------------------------
// This mapping consists of a list of the following entries.
// <type, [<slot, scope, (index | slot)>]>. This is implemented as
//
// flag:  0 if the map is a part of a JIT'd module
//        1 if the map is a part of an NGEN'd module.
// count: number of types that have entries
// {
//   type:  The ID current type being mapped
//   count: Number of subentries for the current type
//   bool:  Whether or not the target slot/index values can be negative.
//   {
//     slot:       The slot of type that is being mapped
//     scope:      Determines if this is a virtual or non-virtual mapping
//     index/slot: If scope is virtual, then this is a slot mapping for the
//                 current type. In such a case, the implementation search is
//                 modified to <this, slot> and the search is restarted from
//                 the initial type. If the scope is non-virtual, the value is
//                 an index that represents an implementation within
//                 implementation table of the type that contains this slot
//                 mapping.
//   }
// }
void DispatchMap::CreateEncodedMapping(MethodTable                 *pMT,
                                       DispatchMapBuilder          *pMapBuilder,
                                       StackingAllocator           *pAllocator,
                                       BYTE                       **ppbMap,
                                       UINT32                      *pcbMap)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        INJECT_FAULT(COMPlusThrowOM());
        PRECONDITION(CheckPointer(pMT));
        PRECONDITION(CheckPointer(pMapBuilder));
        PRECONDITION(CheckPointer(pAllocator));
        PRECONDITION(CheckPointer(ppbMap));
        PRECONDITION(CheckPointer(pcbMap));
    } CONTRACTL_END;

    /////////////////////////////////
    // Phase 1 - gather entry counts

    UINT32 cNumTypes = 0;
    UINT32 cNumEntries = 0;

    {
        DispatchMapBuilder::Iterator it(pMapBuilder);
        // We don't want to record overrides or methodImpls in the dispatch map since
        // we have vtables to track this information.
        it.SkipThisTypeEntries();
        if (it.IsValid()) {
            DispatchMapTypeID curType = DispatchMapTypeID::FromUINT32(INVALIDENTRY);
            do {
                cNumEntries++;
                if (curType != it.GetTypeID()) {
                    cNumTypes++;
                    curType = it.GetTypeID();
                }
            } while (it.Next());
        }
    }

    /////////////////////////////////
    // Phase 2 - allocate space

    // Now that we have stats about the overall absolute maximum map size, we can allocate
    // some working space for createing the encoded map in.
    // Sizes: flag==UINT32, typeID==UINT32, slot==UINT32, scope==UINT16, index/slot==UINT32

    UINT32 cbMap = sizeof(UINT32) +
                   cNumTypes * sizeof(UINT32) +
                   cNumEntries * (sizeof(UINT32) + sizeof(UINT16) + sizeof(UINT32));

    BYTE *pbMap = (BYTE*) pAllocator->Alloc((unsigned)cbMap);

    /////////////////////////////////
    // Phase 3 - encode the map

    {
        // Create the encoder over the newly allocated memory
        Encoder e(pbMap);
        // Encode the count of type entries
        e.Encode((unsigned)cNumTypes);
        // Start encoding the map
        DispatchMapBuilder::Iterator it(pMapBuilder);
        it.SkipThisTypeEntries();
        if (it.IsValid()) {
            INT32 curType = -1;
            INT32 prevType;
            INT32 deltaType;
            do {
                // Encode the type ID
                prevType = curType;
                curType = (INT32) it.GetTypeID().ToUINT32();
                deltaType = curType - prevType - ENCODING_TYPE_DELTA;
                CONSISTENCY_CHECK(0 <= deltaType);
                e.Encode((unsigned)deltaType);
                // Variables for slot delta calculations
                BOOL fHasNegatives = FALSE;
                // Source slot
                INT32 curSlot = -1;
                INT32 prevSlot = -1;
                // Target slot for virtual mappings
                INT32 curTargetSlot = -1;
                INT32 prevTargetSlot = -1;
                // Target index for non-virtual mappings
                INT32 curTargetIndex = -1;
                // Count and encode the number of sub entries for this type
                UINT32 cSubEntries = 0;
                DispatchMapBuilder::Iterator subIt(it);
                do {
                    if (subIt.IsVirtualMapping()) {
                        prevTargetSlot = curTargetSlot;
                        curTargetSlot = (INT32)subIt.GetTargetSlot();
                        INT32 deltaTargetSlot =  curTargetSlot - prevTargetSlot - ENCODING_TARGET_SLOT_DELTA;
                        if (deltaTargetSlot < 0) {
                            fHasNegatives = TRUE;
                        }
                    }
                    CONSISTENCY_CHECK(subIt.IsVirtualMapping());
                    cSubEntries++;
                } while (subIt.Next() && subIt.GetTypeID().ToUINT32() == (UINT32)curType);
                e.Encode((unsigned)cSubEntries);
                e.Encode((unsigned)fHasNegatives);
                e.ContainsNegatives(fHasNegatives);
                // Iterate each subentry and encode it
                curTargetSlot = -1;
                curTargetIndex = -1;
                do {
                    // Only virtual targets can be mapped virtually.
                    CONSISTENCY_CHECK(!it.IsVirtualMapping() ||
                                      (it.GetTargetMD() == NULL || it.GetTargetMD()->IsVirtual()));
                    // Encode the slot
                    prevSlot = curSlot;
                    curSlot = it.GetSlotNumber();
                    INT32 deltaSlot = curSlot - prevSlot - ENCODING_SLOT_DELTA;
                    CONSISTENCY_CHECK(0 <= deltaSlot);
                    e.Encode((unsigned)deltaSlot);
                    // Calculate and encode the scope and target slot/index delta
                    UINT32 scope;
                    INT32 delta;
                    CONSISTENCY_CHECK(it.IsVirtualMapping());
                    {
                        // If it's a virtual mapping, the scope is the this class
                        scope = SCOPE_VIRTUAL;
                        prevTargetSlot = curTargetSlot;
                        curTargetSlot = (INT32)it.GetTargetSlot();
                        delta = curTargetSlot - prevTargetSlot - ENCODING_TARGET_SLOT_DELTA;
                    }
                    if (fHasNegatives) {
                        e.EncodeSigned((signed)scope);
                        e.EncodeSigned((signed)delta);
                    }
                    else {
                        CONSISTENCY_CHECK(0 <= delta);
                        e.Encode((unsigned)scope);
                        e.Encode((unsigned)delta);
                    }
                } while (it.Next() && it.GetTypeID().ToUINT32() == (UINT32)curType);
            } while (it.IsValid());
        }
        // Finish and finalize the map, and set the out params.
        e.Done();
        *pcbMap = e.Contents(ppbMap);
}

#ifdef _DEBUG 
    // Let's verify the mapping
    {
        EncodedMapIterator itMap(*ppbMap);
        DispatchMapBuilder::Iterator itBuilder(pMapBuilder);
        itBuilder.SkipThisTypeEntries();

        while (itMap.IsValid()) {
            CONSISTENCY_CHECK(itBuilder.IsValid());
            DispatchMapEntry *pEntryMap = itMap.Entry();
            CONSISTENCY_CHECK(pEntryMap->GetTypeID() == itBuilder.GetTypeID());
            CONSISTENCY_CHECK(pEntryMap->IsVirtuallyMapped() == itBuilder.IsVirtualMapping());
            {
                CONSISTENCY_CHECK(pEntryMap->GetTargetSlotNumber() == itBuilder.GetTargetSlot());
            }
            CONSISTENCY_CHECK(pEntryMap->IsVirtuallyMapped());
            CONSISTENCY_CHECK(itBuilder.IsVirtualMapping());
            itMap.Next();
            itBuilder.Next();
        }

        CONSISTENCY_CHECK(!itBuilder.IsValid());
    }
#endif // _DEBUG
}



#endif // !DACCESS_COMPILE

//------------------------------------------------------------------------
UINT32 DispatchMap::GetMapSize()
{
    WRAPPER_CONTRACT;
    EncodedMapIterator it(this, NULL, e_IT_FLAG_DISABLE_ZAP_ID_MAP);
    for (; it.IsValid(); it.Next()) { }
    CONSISTENCY_CHECK(PTR_HOST_TO_TADDR(it.m_d.End()) > PTR_HOST_MEMBER_TADDR(DispatchMap,this,m_rgMap));
    return (UINT32)(PTR_HOST_TO_TADDR(it.m_d.End()) - PTR_HOST_MEMBER_TADDR(DispatchMap,this,m_rgMap));
}

#ifdef DACCESS_COMPILE 

//------------------------------------------------------------------------
void DispatchMap::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    WRAPPER_CONTRACT;

    DAC_ENUM_DTHIS();

    EMEM_OUT(("MEM: %p DispatchMap\n", PTR_HOST_TO_TADDR(this)));


    DacEnumMemoryRegion(PTR_HOST_MEMBER_TADDR(DispatchMap,this,m_rgMap), GetMapSize());
}

#endif // DACCESS_COMPILE

//--------------------------------------------------------------------
void DispatchMap::EncodedMapIterator::Invalidate()
{
    LEAF_CONTRACT;
    m_numTypes = 0;
    m_curType = 0;
    m_numEntries = 0;
    m_curEntry = 0;
}


//--------------------------------------------------------------------
void DispatchMap::EncodedMapIterator::Init(BYTE *pbMap)
{
    CONTRACTL {
        GC_NOTRIGGER;
        NOTHROW;
        INSTANCE_CHECK;
        PRECONDITION(CheckPointer(pbMap, NULL_OK));
    } CONTRACTL_END;

    if (pbMap != NULL)
    {
        // Initialize the map decoder
        m_d.Init(pbMap);
        m_numTypes = m_d.Next();
        m_curType = -1;
        m_curTypeId = DispatchMapTypeID::FromUINT32(static_cast<UINT32>(-1));
        m_numEntries = 0;
        m_curEntry = -1;
        m_curTargetSlot = static_cast<UINT32>(-1);
        m_curTargetIndex = static_cast<UINT32>(-1);
    }
    else
    {
        Invalidate();
    }

    Next();
}

//--------------------------------------------------------------------
DispatchMap::EncodedMapIterator::EncodedMapIterator(MethodTable *pMT, UINT32 flags)
{
    CONTRACTL {
        GC_NOTRIGGER;
        NOTHROW;
        INSTANCE_CHECK;
    } CONTRACTL_END;

    m_flags = flags;

    if (pMT->HasDispatchMap())
    {
        DispatchMap *pMap = pMT->GetDispatchMap();
        Init(PTR_BYTE(PTR_HOST_MEMBER_TADDR(DispatchMap, pMap, m_rgMap)));
    }
    else
    {
        Init(NULL);
    }
}

//--------------------------------------------------------------------
// This should be used only when a dispatch map needs to be used
// separately from its MethodTable.
DispatchMap::EncodedMapIterator::EncodedMapIterator(DispatchMap *pMap, Module *pModule, UINT32 flags)
    : m_flags(flags)
{
    WRAPPER_CONTRACT;
    Init(pMap != NULL ? PTR_BYTE(PTR_HOST_MEMBER_TADDR(DispatchMap,pMap,m_rgMap)) : NULL);
}

//--------------------------------------------------------------------
DispatchMap::EncodedMapIterator::EncodedMapIterator(BYTE *pbMap, UINT32 flags)
{
    WRAPPER_CONTRACT;

    m_flags = flags;

    Init(pbMap);
}

//--------------------------------------------------------------------
BOOL DispatchMap::EncodedMapIterator::Next()
{
    CONTRACTL {
        GC_NOTRIGGER;
        NOTHROW;
        INSTANCE_CHECK;
    } CONTRACTL_END;

    if (!IsValid()) {
        return FALSE;
    }

    m_curEntry++;
    if (m_curEntry == m_numEntries) {
        m_curType++;
        if (m_curType == m_numTypes) {
            return FALSE;
        }
        m_curTypeId =
            DispatchMapTypeID::FromUINT32((UINT32)((INT32)m_curTypeId.ToUINT32() + (INT32)m_d.Next() + ENCODING_TYPE_DELTA));
        if (IsThisClassOnly() && !m_curTypeId.IsThisClass()) {
            Invalidate();
            return FALSE;
        }
        m_curEntry = 0;
        m_numEntries = m_d.Next();
        m_fCurTypeHasNegativeEntries = (BOOL)m_d.Next();
        m_curSlot = static_cast<UINT32>(-1);
        m_curTargetSlot = static_cast<UINT32>(-1);
        m_curTargetIndex = static_cast<UINT32>(-1);
        CONSISTENCY_CHECK(m_numEntries != 0);
    }

    // Now gather enough info to initialize the dispatch entry

    // Get the source slot
    m_curSlot = (UINT32)((INT32)m_curSlot + (INT32)m_d.Next() + ENCODING_SLOT_DELTA);

    // Get the target scope (virtual, or non virtual)
    UINT32 scope = (UINT32) m_d.Next();

    // If virtual, get the target virtual slot number
    if (scope == SCOPE_VIRTUAL) {
        m_curTargetSlot =
            (UINT32)((INT32)m_curTargetSlot +
                     ENCODING_TARGET_SLOT_DELTA +
                     (INT32)(m_fCurTypeHasNegativeEntries ? m_d.NextSigned() : m_d.Next()));
        m_e.InitVirtualMapping(m_curTypeId, m_curSlot, m_curTargetSlot);
    }

    // If non-virtual, get the target index for the type indicated by scope
    else {
        m_curTargetIndex =
            (UINT32)((INT32)m_curTargetIndex +
                     ENCODING_TARGET_INDEX_DELTA +
                     (INT32)(m_fCurTypeHasNegativeEntries ? m_d.NextSigned() : m_d.Next()));
        m_e.InitNonVirtualMapping(m_curTypeId, m_curSlot, scope - SCOPE_NON_VIRTUAL_BASE, m_curTargetIndex);
    }

    CONSISTENCY_CHECK(IsValid());
    return TRUE;
}

#ifndef DACCESS_COMPILE 
//--------------------------------------------------------------------
DispatchMap::Iterator::Iterator(MethodTable *pMT, UINT32 flags)
    :
    m_mapIt(pMT, flags)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;
}

//--------------------------------------------------------------------
BOOL DispatchMap::Iterator::IsValid()
{
    WRAPPER_CONTRACT;
    return (
            m_mapIt.IsValid());
}

//--------------------------------------------------------------------
BOOL DispatchMap::Iterator::Next()
{
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(!m_mapIt.Entry()->GetTypeID().IsThisClass());
    if (m_mapIt.IsValid()) {
        m_mapIt.Next();
        CONSISTENCY_CHECK(!m_mapIt.IsValid() || !m_mapIt.Entry()->GetTypeID().IsThisClass());
    }
    return IsValid();
}

//--------------------------------------------------------------------
DispatchMapEntry *DispatchMap::Iterator::Entry()
{
/*
    CONTRACTL {
        INSTANCE_CHECK;
        MODE_ANY;
        if (FORBIDGC_LOADER_USE_ENABLED() || typeID.IsThisClass()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED() || typeID.IsThisClass()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED() || typeID.IsThisClass()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        PRECONDITION(IsValid());
    } CONTRACTL_END;
*/
    WRAPPER_CONTRACT;
    CONSISTENCY_CHECK(IsValid());

    DispatchMapEntry *pEntry = NULL;
    if (m_mapIt.IsValid()) {
        pEntry = m_mapIt.Entry();
    }
    CONSISTENCY_CHECK(CheckPointer(pEntry));
    return pEntry;
}

#endif // !DACCESS_COMPILE


