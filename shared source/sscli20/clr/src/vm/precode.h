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
// precode.h
//
// Stub that runs before the actual native code
//

#ifndef __PRECODE_H__
#define __PRECODE_H__


typedef DPTR(class Precode) PTR_Precode;

#ifndef PRECODE_ALIGNMENT
#define PRECODE_ALIGNMENT sizeof(void*)
#endif

enum PrecodeType {
    PRECODE_INVALID         = InvalidPrecode::Type,
    PRECODE_STUB            = StubPrecode::Type,
#ifdef HAS_NDIRECT_IMPORT_PRECODE
    PRECODE_NDIRECT_IMPORT  = NDirectImportPrecode::Type,
#endif // HAS_NDIRECT_IMPORT_PRECODE
#ifdef HAS_REMOTING_PRECODE
    PRECODE_REMOTING        = RemotingPrecode::Type,
#endif // HAS_REMOTING_PRECODE
#ifdef HAS_FIXUP_PRECODE
    PRECODE_FIXUP           = FixupPrecode::Type,
#endif // HAS_FIXUP_PRECODE
#ifdef HAS_THISPTR_RETBUF_PRECODE
    PRECODE_THISPTR_RETBUF  = ThisPtrRetBufPrecode::Type,
#endif // HAS_THISPTR_RETBUF_PRECODE
};

class Precode {
    friend class ZapMonitor;

    BYTE m_data[SIZEOF_PRECODE_BASE];

    StubPrecode* AsStubPrecode()
    {
        LEAF_CONTRACT;
        return PTR_StubPrecode(PTR_HOST_TO_TADDR(this));
    }

#ifdef HAS_NDIRECT_IMPORT_PRECODE
public:
    // Fake precodes has to be exposed
    NDirectImportPrecode* AsNDirectImportPrecode()
    {
        LEAF_CONTRACT;
        return PTR_NDirectImportPrecode(PTR_HOST_TO_TADDR(this));
    }

private:
#endif // HAS_NDIRECT_IMPORT_PRECODE

#ifdef HAS_REMOTING_PRECODE
    RemotingPrecode* AsRemotingPrecode()
    {
        LEAF_CONTRACT;
        return PTR_RemotingPrecode(PTR_HOST_TO_TADDR(this));
    }
#endif // HAS_REMOTING_PRECODE

#ifdef HAS_FIXUP_PRECODE
    FixupPrecode* AsFixupPrecode()
    {
        LEAF_CONTRACT;
        return PTR_FixupPrecode(PTR_HOST_TO_TADDR(this));
    }
#endif // HAS_FIXUP_PRECODE

#ifdef HAS_THISPTR_RETBUF_PRECODE
    ThisPtrRetBufPrecode* AsThisPtrRetBufPrecode()
    {
        LEAF_CONTRACT;
        return PTR_ThisPtrRetBufPrecode(PTR_HOST_TO_TADDR(this));
    }
#endif // HAS_THISPTR_RETBUF_PRECODE

    TADDR GetStart()
    {
        LEAF_CONTRACT;
        return PTR_HOST_TO_TADDR(this);
    }

    static void UnexpectedPrecodeType()
    {
#ifdef DACCESS_COMPILE
        DacError(E_UNEXPECTED);
#else
        _ASSERTE(!"Unexpected precode type");
#endif
    }

public:
    PrecodeType GetType()
    {
        LEAF_CONTRACT;

#ifdef OFFSETOF_PRECODE_TYPE

        BYTE type = m_data[OFFSETOF_PRECODE_TYPE];
#ifdef _X86_
        if (type == X86_INSTR_MOV_RM_R)
            type = m_data[OFFSETOF_PRECODE_TYPE_MOV_RM_R];
#endif

#ifdef HAS_FIXUP_PRECODE
        if (type == FixupPrecode::TypePrestub)
            type = FixupPrecode::Type;
#endif

        return (PrecodeType)type;

#else // OFFSETOF_PRECODE_TYPE
        return PRECODE_STUB;
#endif // OFFSETOF_PRECODE_TYPE
    }

    static BOOL IsValidType(PrecodeType t);

    static int AlignOf(PrecodeType t)
    {
        int align = PRECODE_ALIGNMENT;

#if defined(_X86_) && defined(HAS_FIXUP_PRECODE)
        // Fixup precodes has to be aligned to allow atomic patching
        if (t == PRECODE_FIXUP)
            align = 8;
#endif // _X86_ && HAS_FIXUP_PRECODE


        return align;
    }

    static SIZE_T SizeOf(PrecodeType t);

    SIZE_T SizeOf()
    {
        WRAPPER_CONTRACT;
        return SizeOf(GetType());
    }

    PTR_TADDR GetAddrOfNativeCodeSlot();

    TADDR GetNativeCode()
    {
        WRAPPER_CONTRACT;
        PTR_TADDR pSlot = GetAddrOfNativeCodeSlot();
        return (pSlot != NULL) ? *pSlot : NULL;
    }

    // Note: This is immediate target of the precode. It does not follow jump stub if there is one.
    TADDR GetTarget();

    BOOL IsPointingTo(TADDR target, TADDR addr)
    {
        WRAPPER_CONTRACT;

        if (target == addr)
            return TRUE;


        return FALSE;
    }

    BOOL IsPointingToNativeCode()
    {
        WRAPPER_CONTRACT;

#ifdef HAS_REMOTING_PRECODE
        // Remoting precode is special case
        if (GetType() == PRECODE_REMOTING)
            return FALSE;
#endif

        return IsPointingTo(GetTarget(), GetNativeCode());
    }

    BOOL IsPointingToPrestub(TADDR target);

    BOOL IsPointingToPrestub()
    {
        WRAPPER_CONTRACT;
        return IsPointingToPrestub(GetTarget());
    }

    TADDR GetEntryPoint()
    {
        LEAF_CONTRACT;
        return PTR_HOST_TO_TADDR(this);
    }

    MethodDesc* GetMethodDesc();

    static Precode* Allocate(PrecodeType t, MethodDesc* pMD, BOOL fMayHaveNativeCode, 
        BaseDomain* pDomain, AllocMemTracker *pamTracker);
    void Init(PrecodeType t, MethodDesc* pMD, BaseDomain* pDomain);

#ifndef DACCESS_COMPILE
    void SetNativeCode(TADDR addr)
    {
        WRAPPER_CONTRACT;
        PTR_TADDR pSlot = GetAddrOfNativeCodeSlot();
        PREFIX_ASSUME(pSlot != NULL);
        *pSlot = addr;
    }

    BOOL SetNativeCodeInterlocked(TADDR pCode, TADDR pExpected = NULL)
    {
        WRAPPER_CONTRACT;
        PTR_TADDR pSlot = GetAddrOfNativeCodeSlot();
        PREFIX_ASSUME(pSlot != NULL);
        return FastInterlockCompareExchangePointer((void*volatile*)pSlot, (void*)pCode, (void*)pExpected) == (void*)pExpected;
    }

    BOOL SetTargetInterlocked(TADDR target, BOOL fRequiresMethodDescCallingConvention = TRUE);

    // Reset precode to point to prestub
    void Reset();
#endif // DACCESS_COMPILE

    static Precode* GetPrecodeFromEntryPoint(TADDR addr, BOOL fSpeculative = FALSE)
    {
        LEAF_CONTRACT;

#ifdef DACCESS_COMPILE
        // Always use speculative checks with DAC
        fSpeculative = TRUE;
#endif

        // Always do consistency check in debug
        if (fSpeculative INDEBUG(|| TRUE))
        {
            if (!IS_ALIGNED(addr, PRECODE_ALIGNMENT) || !IsValidType(PTR_Precode(addr)->GetType()))
            {
                if (fSpeculative) return NULL;
                _ASSERTE(!"Unexpected code in precode");
            }
        }

        Precode* pPrecode = PTR_Precode(addr);

        if (!fSpeculative)
        {
            g_IBCLogger.LogMethodPrecodeAccess(pPrecode->GetMethodDesc());
        }

        return pPrecode;
    }

    //
    // Precode as temporary entrypoint
    // 

    static SIZE_T SizeOfTemporaryEntryPoint(PrecodeType t, BOOL fNativeCodeSlots)
    {
        LEAF_CONTRACT;
        return ALIGN_UP(SizeOf(t) + (fNativeCodeSlots ? sizeof(TADDR) : 0), AlignOf(t));
    }

    static Precode* GetPrecodeForTemporaryEntryPoint(TADDR temporaryEntryPoints, int index, BOOL fNativeCodeSlots)
    {
        WRAPPER_CONTRACT;
        SIZE_T oneSize = SizeOfTemporaryEntryPoint(PTR_Precode(temporaryEntryPoints)->GetType(), fNativeCodeSlots);
        return PTR_Precode(temporaryEntryPoints + index * oneSize);
    }

    static SIZE_T SizeOfTemporaryEntryPoints(TADDR temporaryEntryPoints, int count, BOOL fNativeCodeSlots)
    {
        WRAPPER_CONTRACT;
        SIZE_T oneSize = SizeOfTemporaryEntryPoint(PTR_Precode(temporaryEntryPoints)->GetType(), fNativeCodeSlots);
        return count * oneSize;
    }

    static TADDR AllocateTemporaryEntryPoints(MethodDescChunk* pChunk,
        BaseDomain* pDomain, AllocMemTracker *pamTracker);


#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif
};


#endif // __PRECODE_H__
