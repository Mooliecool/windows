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
// File: MethodImpl.CPP
//
// ===========================================================================
//
// ===========================================================================
//

#include "common.h"
#include "methodimpl.h"

DWORD MethodImpl::FindSlotIndex(DWORD slot)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
        PRECONDITION(CheckPointer(GetSlots()));
    } CONTRACTL_END;

    DWORD dwSize = GetSize();
    if(dwSize == 0) {
        return INVALID_INDEX;
    }

    // Simple binary search
    PTR_DWORD rgSlots = GetSlots();
    INT32     l       = 0;
    INT32     r       = dwSize - 1;
    INT32     pivot;

    while(1) {
        pivot =  (l + r) / 2;

        if(rgSlots[pivot] == slot) {
            break; // found it
        }
        else if(rgSlots[pivot] < slot) {
            l = pivot + 1;
        }
        else {
            r = pivot - 1;
        }

        if(l > r) {
            return INVALID_INDEX; // Not here
        }
    }

    CONSISTENCY_CHECK(pivot >= 0);
    return (DWORD)pivot;
}

PTR_MethodDesc MethodImpl::FindMethodDesc(DWORD slot, PTR_MethodDesc defaultReturn)
{
    CONTRACTL
    {
        if (FORBIDGC_LOADER_USE_ENABLED()) NOTHROW; else THROWS;
        if (FORBIDGC_LOADER_USE_ENABLED()) GC_NOTRIGGER; else GC_TRIGGERS;
        if (FORBIDGC_LOADER_USE_ENABLED()) FORBID_FAULT; else { INJECT_FAULT(COMPlusThrowOM()); }
        MODE_ANY;
    }
    CONTRACTL_END

    DWORD slotIndex = FindSlotIndex(slot);
    if (slotIndex == INVALID_INDEX) {
        return defaultReturn;
    }


    PTR_MethodDesc result = pImplementedMD[slotIndex]; // The method descs are not offset by one

    // Prejitted images may leave NULL in this table if
    // the methoddesc is declared in another module.
    // In this case we need to manually compute & restore it
    // from the slot number.

    if (result == NULL)
#ifndef DACCESS_COMPILE 
        result = RestoreSlot(slotIndex, defaultReturn->GetMethodTable());
#else // DACCESS_COMPILE
        DacNotImpl();
#endif // DACCESS_COMPILE

    return result;
}

#ifndef DACCESS_COMPILE 

MethodDesc *MethodImpl::RestoreSlot(DWORD index, MethodTable *pMT)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
        PRECONDITION(CheckPointer(pdwSlots));
    }
    CONTRACTL_END

    MethodDesc *result;

    PREFIX_ASSUME(pdwSlots != NULL);
    DWORD slot = GetSlots()[index];

    // Since the overridden method is in a different module, we
    // are guaranteed that it is from a different class.  It is
    // either an override of a parent virtual method or parent-implemented
    // interface, or of an interface that this class has introduced.

    // In the former 2 cases, the slot number will be in the parent's
    // vtable section, and we can retrieve the implemented MethodDesc from
    // there.  In the latter case, we can search through our interface
    // map to determine which interface it is from.

    MethodTable *pParentMT = pMT->GetParentMethodTable();
    CONSISTENCY_CHECK(pParentMT != NULL && slot < pParentMT->GetNumVirtuals());
    {
        result = pParentMT->GetMethodDescForSlot(slot);
    }

    _ASSERTE(result != NULL);

    // Don't worry about races since we would all be setting the same result
    pImplementedMD[index] = result;

    return result;
}

#endif // !DACCESS_COMPILE

MethodDesc* MethodImpl::GetFirstImplementedMD(MethodDesc* pContainer)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        FORBID_FAULT;
    }
    CONTRACTL_END

// This is only used for recovering interface MDs
    CONSISTENCY_CHECK_MSG(FALSE, "Stub Dispatch forbidden code");
    return NULL;
}

#ifndef DACCESS_COMPILE 

///////////////////////////////////////////////////////////////////////////////////////
void MethodImpl::SetSize(LoaderHeap *pHeap, AllocMemTracker *pamTracker, DWORD size)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(pdwSlots==NULL && pImplementedMD==NULL);
        INJECT_FAULT(ThrowOutOfMemory());
    } CONTRACTL_END;

    if(size > 0) {
        // An array of DWORDs, the first entry representing count, and the rest representing slot numbers
        size_t cbCountAndSlots =   sizeof(DWORD)          // DWORD for the total count of slots
                                 + size * sizeof(DWORD);  // DWORD each for the slot numbers

        // MethodDesc* for each of the implemented methods
        size_t cbMethodDescs = size * sizeof(MethodDesc *);

        // Need to align-up the slot entries so that the MethodDesc* array starts on a pointer boundary.
        size_t cbTotal = ALIGN_UP(cbCountAndSlots, sizeof(MethodDesc*)) + cbMethodDescs;

        // Allocate the memory.
        LPBYTE pAllocData = (BYTE*)pamTracker->Track(pHeap->AllocMem(cbTotal));

        // Set the count and slot array
        pdwSlots = (DWORD*)pAllocData;

        // Set the MethodDesc* array. Make sure to adjust for alignment.
        pImplementedMD = (MethodDesc**)ALIGN_UP(pAllocData + cbCountAndSlots, sizeof(MethodDesc*));

        // Store the count in the first entry
        *pdwSlots = size;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
void MethodImpl::SetData(DWORD* slots, MethodDesc** md)
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(CheckPointer(this));
        PRECONDITION(CheckPointer(pdwSlots));
    } CONTRACTL_END;

    DWORD dwSize = *pdwSlots;
    memcpy(&(pdwSlots[1]), slots, dwSize*sizeof(DWORD));
    memcpy(pImplementedMD, md, dwSize*sizeof(MethodDesc*));
}



#endif // !DACCESS_COMPILE

#ifdef DACCESS_COMPILE 

void
MethodImpl::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{
    CONSISTENCY_CHECK_MSG(FALSE, "Stub Dispatch forbidden code");
}

#endif // DACCESS_COMPILE

#ifndef DACCESS_COMPILE 
MethodImpl::Iterator::Iterator(MethodDesc *pMD) : m_pMD(pMD), m_pImpl(NULL), m_iCur(0)
{
    WRAPPER_CONTRACT;
    if (pMD->IsMethodImpl())
    {
        m_pImpl = pMD->GetMethodImpl();
    }
}
#endif // !DACCESS_COMPILE

