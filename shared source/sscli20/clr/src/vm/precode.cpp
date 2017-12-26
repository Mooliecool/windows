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
// precode.cpp
//
// Stub that runs before the actual native code
//

#include "common.h"



//==========================================================================================
// class Precode
//==========================================================================================
BOOL Precode::IsValidType(PrecodeType t)
{
    LEAF_CONTRACT;
    switch (t) {
    case PRECODE_STUB:
#ifdef HAS_NDIRECT_IMPORT_PRECODE
    case PRECODE_NDIRECT_IMPORT:
#endif // HAS_NDIRECT_IMPORT_PRECODE
#ifdef HAS_REMOTING_PRECODE
    case PRECODE_REMOTING:
#endif // HAS_REMOTING_PRECODE
#ifdef HAS_FIXUP_PRECODE
    case PRECODE_FIXUP:
#endif // HAS_FIXUP_PRECODE
#ifdef HAS_THISPTR_RETBUF_PRECODE
    case PRECODE_THISPTR_RETBUF:
#endif // HAS_THISPTR_RETBUF_PRECODE
        return TRUE;
    default:
        return FALSE;
    }
}

SIZE_T Precode::SizeOf(PrecodeType t)
{
    LEAF_CONTRACT;

    switch (t)
    {
    case PRECODE_STUB:
        return sizeof(StubPrecode);
#ifdef HAS_NDIRECT_IMPORT_PRECODE
    case PRECODE_NDIRECT_IMPORT:
        return sizeof(NDirectImportPrecode);
#endif // HAS_NDIRECT_IMPORT_PRECODE
#ifdef HAS_REMOTING_PRECODE
    case PRECODE_REMOTING:
        return sizeof(RemotingPrecode);
#endif // HAS_REMOTING_PRECODE
#ifdef HAS_FIXUP_PRECODE
    case PRECODE_FIXUP:
        return sizeof(FixupPrecode);
#endif // HAS_FIXUP_PRECODE
#ifdef HAS_THISPTR_RETBUF_PRECODE
    case PRECODE_THISPTR_RETBUF:
        return sizeof(ThisPtrRetBufPrecode);
#endif // HAS_THISPTR_RETBUF_PRECODE
    default:
        UnexpectedPrecodeType();
        break;
    }
    return 0;
}

PTR_TADDR Precode::GetAddrOfNativeCodeSlot()
{
    WRAPPER_CONTRACT;

    MethodDesc* pMD = GetMethodDesc();

#ifndef DACCESS_COMPILE
    // This function works on precodes associated with the method desc only. The native code slot 
    // is not guaranteed to be there otherwise.
    _ASSERTE(pMD->GetPrecode() == this);
#endif // DACCESS_COMPILE

    if (!pMD->MayHaveNativeCode())
        return NULL;

    return (PTR_TADDR)(PTR_HOST_TO_TADDR(this) + SizeOf());
}

// Note: This is immediate target of the precode. It does not follow jump stub if there is one.
TADDR Precode::GetTarget()
{
    LEAF_CONTRACT;

    TADDR target = NULL;

    switch (GetType()) {
    case PRECODE_STUB:
        target = AsStubPrecode()->GetTarget();
        break;
#ifdef HAS_REMOTING_PRECODE
    case PRECODE_REMOTING:
        target = AsRemotingPrecode()->GetTarget();
        break;
#endif // HAS_REMOTING_PRECODE
#ifdef HAS_FIXUP_PRECODE
    case PRECODE_FIXUP:
        target = AsFixupPrecode()->GetTarget();
        break;
#endif // HAS_FIXUP_PRECODE
#ifdef HAS_THISPTR_RETBUF_PRECODE
    case PRECODE_THISPTR_RETBUF:
        target = AsThisPtrRetBufPrecode()->GetTarget();
        break;
#endif // HAS_THISPTR_RETBUF_PRECODE
    default:
        UnexpectedPrecodeType();
        break;
    }
    return target;
}

MethodDesc* Precode::GetMethodDesc()
{
    LEAF_CONTRACT;

    TADDR pMD = NULL;

    switch (GetType())
    {
    case PRECODE_STUB:
        pMD = AsStubPrecode()->GetMethodDesc();
        break;
#ifdef HAS_NDIRECT_IMPORT_PRECODE
    case PRECODE_NDIRECT_IMPORT:
        pMD = AsNDirectImportPrecode()->GetMethodDesc();
        break;
#endif // HAS_NDIRECT_IMPORT_PRECODE
#ifdef HAS_REMOTING_PRECODE
    case PRECODE_REMOTING:
        pMD = AsRemotingPrecode()->GetMethodDesc();
        break;
#endif // HAS_REMOTING_PRECODE
#ifdef HAS_FIXUP_PRECODE
    case PRECODE_FIXUP:
        pMD = AsFixupPrecode()->GetMethodDesc();
        break;
#endif // HAS_FIXUP_PRECODE
#ifdef HAS_THISPTR_RETBUF_PRECODE
    case PRECODE_THISPTR_RETBUF:
        pMD = AsThisPtrRetBufPrecode()->GetMethodDesc();
        break;
#endif // HAS_THISPTR_RETBUF_PRECODE
    default:
        UnexpectedPrecodeType();
        break;
    }

    return (PTR_MethodDesc)pMD;
}

BOOL Precode::IsPointingToPrestub(TADDR target)
{
    WRAPPER_CONTRACT;

    if (IsPointingTo(target, (TADDR)ThePreStub()->GetEntryPoint()))
        return TRUE;


    return FALSE;
}

#ifndef DACCESS_COMPILE

Precode* Precode::Allocate(PrecodeType t, MethodDesc* pMD, BOOL fMayHaveNativeCode,
    BaseDomain* pDomain, AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    SIZE_T size = SizeOf(t) + (fMayHaveNativeCode ? sizeof(TADDR) : 0);
    Precode* pPrecode = (Precode*)pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocAlignedMem(size, AlignOf(t), NULL));
    pPrecode->Init(t, pMD, pDomain);
    return pPrecode;
}

void Precode::Init(PrecodeType t, MethodDesc* pMD, BaseDomain* pDomain)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    switch (t) {
    case PRECODE_STUB:
        ((StubPrecode*)this)->Init(pMD, pDomain);
        break;
#ifdef HAS_NDIRECT_IMPORT_PRECODE
    case PRECODE_NDIRECT_IMPORT:
        ((NDirectImportPrecode*)this)->Init(pMD, pDomain);
        break;
#endif // HAS_NDIRECT_IMPORT_PRECODE
#ifdef HAS_REMOTING_PRECODE
    case PRECODE_REMOTING:
        ((RemotingPrecode*)this)->Init(pMD, pDomain);
        break;
#endif // HAS_REMOTING_PRECODE
#ifdef HAS_FIXUP_PRECODE
    case PRECODE_FIXUP:
        ((FixupPrecode*)this)->Init(pMD, pDomain);
        break;
#endif // HAS_FIXUP_PRECODE
#ifdef HAS_THISPTR_RETBUF_PRECODE
    case PRECODE_THISPTR_RETBUF:
        ((ThisPtrRetBufPrecode*)this)->Init(pMD, pDomain);
        break;
#endif // HAS_THISPTR_RETBUF_PRECODE
    default:
        UnexpectedPrecodeType();
        break;
    }

    _ASSERTE(IsValidType(GetType()));
}

BOOL Precode::SetTargetInterlocked(TADDR target, BOOL fRequiresMethodDescCallingConvention /*=TRUE*/)
{
    WRAPPER_CONTRACT;

    TADDR expected = GetTarget();
    BOOL ret = FALSE;

    if (!IsPointingToPrestub(expected))
        return FALSE;

    g_IBCLogger.LogMethodPrecodeWriteAccess(GetMethodDesc());

    switch (GetType())
    {
    case PRECODE_STUB:
        ret = AsStubPrecode()->SetTargetInterlocked(target, expected, fRequiresMethodDescCallingConvention);
        break;

#ifdef HAS_REMOTING_PRECODE
    case PRECODE_REMOTING:
        ret = AsRemotingPrecode()->SetTargetInterlocked(target, expected);
        break;
#endif // HAS_REMOTING_PRECODE

#ifdef HAS_FIXUP_PRECODE
    case PRECODE_FIXUP:
        ret = AsFixupPrecode()->SetTargetInterlocked(target, expected);
        break;
#endif // HAS_FIXUP_PRECODE

#ifdef HAS_THISPTR_RETBUF_PRECODE
    case PRECODE_THISPTR_RETBUF:
        ret = AsThisPtrRetBufPrecode()->SetTargetInterlocked(target, expected);
        break;
#endif // HAS_THISPTR_RETBUF_PRECODE

    default:
        UnexpectedPrecodeType();
        break;
    }

    if (ret) {
        FlushInstructionCache(GetCurrentProcess(),this,SizeOf());
    }

    _ASSERTE(!IsPointingToPrestub());
    return ret;
}

void Precode::Reset()
{
    WRAPPER_CONTRACT;

    MethodDesc* pMD = GetMethodDesc();
    Init(GetType(), pMD, pMD->GetDomainForAllocation());
    if (pMD->MayHaveNativeCode())
        SetNativeCode(NULL);
    FlushInstructionCache(GetCurrentProcess(),this,SizeOf());
}

/* static */ TADDR Precode::AllocateTemporaryEntryPoints(MethodDescChunk* pChunk,
    BaseDomain* pDomain, AllocMemTracker *pamTracker)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    MethodDesc* pFirstMD = pChunk->GetFirstMethodDesc();

    int count = pChunk->GetCount();

    BOOL fForcedPrecode = pFirstMD->RequiresStableEntryPoint(count > 1);
    BOOL fNativeCodeSlots = fForcedPrecode && pFirstMD->ComputeMayHaveNativeCode(count > 1);

    PrecodeType t = PRECODE_STUB;

#if defined(HAS_FIXUP_PRECODE) && !defined(HAS_COMPACT_ENTRYPOINTS)
    // Default to faster fixup precode if possible
    if (!fForcedPrecode)
    {        
        t = PRECODE_FIXUP;
    }
#endif // HAS_FIXUP_PRECODE && !HAS_COMPACT_ENTRYPOINTS

    SIZE_T oneSize = SizeOfTemporaryEntryPoint(t, fNativeCodeSlots);
    SIZE_T allSize = oneSize * count;

#ifdef HAS_COMPACT_ENTRYPOINTS
    if (!fForcedPrecode && allSize > MethodDescChunk::SizeOfCompactEntryPoints(count))
        return NULL;
#endif

    TADDR temporaryEntryPoints = (TADDR)pamTracker->Track(pDomain->GetHighFrequencyHeap()->AllocAlignedMem(allSize, AlignOf(t), NULL));

    TADDR entryPoint = temporaryEntryPoints;
    for (int i = 0; i < count; i++)
    {
        MethodDesc* pMD = pChunk->GetMethodDescAt(i);

        ((Precode*)entryPoint)->Init(t, pMD, pDomain);
        _ASSERTE((Precode*)entryPoint == GetPrecodeForTemporaryEntryPoint(temporaryEntryPoints, i, fNativeCodeSlots));

        entryPoint += oneSize;
    }

    if (fNativeCodeSlots)
        pChunk->SetHasNativeCodeSlots();

    return temporaryEntryPoints;
}


#endif // !DACCESS_COMPILE


#ifdef DACCESS_COMPILE
void Precode::EnumMemoryRegions(CLRDataEnumMemoryFlags flags)
{   
    DacEnumMemoryRegion(GetStart(), 
        SizeOf() + (GetMethodDesc()->MayHaveNativeCode() ? sizeof(TADDR) : 0));
}
#endif

