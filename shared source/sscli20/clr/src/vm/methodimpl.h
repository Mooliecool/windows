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
// method.hpp
//
#ifndef _METHODIMPL_H 
#define _METHODIMPL_H

class MethodDesc;

class MethodImpl
{
    friend class ZapMonitor;

    PTR_DWORD            pdwSlots;       // Maintains the slots in sorted order, the first entry is the size
    DPTR(PTR_MethodDesc) pImplementedMD;

public:

#ifndef DACCESS_COMPILE 
    ///////////////////////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        MethodDesc *m_pMD;
        MethodImpl *m_pImpl;
        DWORD       m_iCur;

    public:
        Iterator(MethodDesc *pMD);
        inline BOOL IsValid()
            { WRAPPER_CONTRACT; return ((m_pImpl != NULL)&& (m_iCur < m_pImpl->GetSize())); }
        inline void Next()
            { WRAPPER_CONTRACT; if (IsValid()) m_iCur++; }
        inline WORD GetSlot()
            { WRAPPER_CONTRACT; CONSISTENCY_CHECK(IsValid()); return m_pImpl->GetSlots()[m_iCur]; }
        inline MethodDesc *GetMethodDesc()
            { WRAPPER_CONTRACT; return m_pImpl->FindMethodDesc(GetSlot(), (PTR_MethodDesc) m_pMD); }
    };

    ///////////////////////////////////////////////////////////////////////////////////////
    inline MethodDesc** GetImplementedMDs()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer(this));
        } CONTRACTL_END;
        return pImplementedMD;
    }
#endif // !DACCESS_COMPILE

    ///////////////////////////////////////////////////////////////////////////////////////
    inline DWORD GetSize()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer(this));
        } CONTRACTL_END;

        if(pdwSlots == NULL)
            return 0;
        else
            return *pdwSlots;
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    inline PTR_DWORD GetSlots()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
            PRECONDITION(CheckPointer(this));
        } CONTRACTL_END;

        if(pdwSlots == NULL)
            return NULL;
        else
            return PTR_DWORD(PTR_HOST_TO_TADDR(pdwSlots) + sizeof(DWORD));
    }

#ifndef DACCESS_COMPILE 

    ///////////////////////////////////////////////////////////////////////////////////////
    void SetSize(LoaderHeap *pHeap, AllocMemTracker *pamTracker, DWORD size);

    ///////////////////////////////////////////////////////////////////////////////////////
    void SetData(DWORD* slots, MethodDesc** md);

#endif // !DACCESS_COMPILE

    MethodDesc* GetFirstImplementedMD(MethodDesc* pContainer);

#ifdef DACCESS_COMPILE 
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif



    // Returns the method desc for the replaced slot;
    PTR_MethodDesc FindMethodDesc(DWORD slot, PTR_MethodDesc defaultReturn);

private:
    static const DWORD INVALID_INDEX = (DWORD)(-1);
    DWORD FindSlotIndex(DWORD slot);
#ifndef DACCESS_COMPILE 
    MethodDesc* RestoreSlot(DWORD slotIndex, MethodTable *pMT);
#endif

};

#endif // !_METHODIMPL_H
