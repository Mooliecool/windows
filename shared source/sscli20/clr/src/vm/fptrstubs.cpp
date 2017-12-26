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
#include "fptrstubs.h"


// -------------------------------------------------------
// FuncPtr stubs
// -------------------------------------------------------

Precode* FuncPtrStubs::Lookup(MethodDesc * pMD, PrecodeType type)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
    }
    CONTRACTL_END

    CrstHolder ch(&m_hashTableCrst);
    return m_hashTable.Lookup(PrecodeKey(pMD, type));
}


#ifndef DACCESS_COMPILE
//
// FuncPtrStubs
//

FuncPtrStubs::FuncPtrStubs()
    : m_hashTableCrst("FuncPtrStubs", CrstSyncHashLock, CRST_UNSAFE_COOPGC)
{
    WRAPPER_CONTRACT;
}

PrecodeType FuncPtrStubs::GetDefaultType(MethodDesc* pMD)
{
    WRAPPER_CONTRACT;

    PrecodeType type = PRECODE_STUB;
 
#ifdef HAS_FIXUP_PRECODE
    // Use the faster fixup precode if it is available
    type = PRECODE_FIXUP;
#endif // HAS_FIXUP_PRECODE

#ifdef HAS_REMOTING_PRECODE
    if (pMD->IsRemotingInterceptedViaVirtualDispatch())
    {
        type = PRECODE_REMOTING;
    }
#endif // HAS_REMOTING_PRECODE

    return type;
}

//
// Returns an existing stub, or creates a new one
//

PBYTE   FuncPtrStubs::GetFuncPtrStub(MethodDesc * pMD, PrecodeType type)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        SO_INTOLERANT;
        INJECT_FAULT(ThrowOutOfMemory(););
    }
    CONTRACTL_END

    Precode* pPrecode = Lookup(pMD, type);
    if (pPrecode != NULL)
        return (PBYTE)pPrecode->GetEntryPoint();

    TADDR target = NULL;

    if (type != GetDefaultType(pMD))
    {
        // Set the target if precode is not of the default type. We are patching the precodes of the default type only.
        target = (TADDR)pMD->GetMultiCallableAddrOfCode();
    }
    else
    if (pMD->HasStableEntryPoint())
    {
        // Set target
        target = pMD->GetStableEntryPoint();
    }
    else
    {
        // Set the target if method is methodimpled. We would not get to patch it otherwise.
        MethodDesc* pMDImpl = MethodTable::MapMethodDeclToMethodImpl(pMD);

        if (pMDImpl != pMD)
            target = (TADDR)pMDImpl->GetMultiCallableAddrOfCode();
    }

    BOOL fNeedsSlot = FALSE;

#ifdef HAS_REMOTING_PRECODE
    if (type == PRECODE_REMOTING)
    {
        fNeedsSlot = TRUE;
    }
#endif // HAS_REMOTING_PRECODE

    CrstHolder ch(&m_hashTableCrst);

    // Was an entry added in the meantime?
    pPrecode = m_hashTable.Lookup(PrecodeKey(pMD, type));
    if (pPrecode != NULL)
        return (PBYTE)pPrecode->GetEntryPoint();

    AllocMemTracker amt;
    pPrecode = Precode::Allocate(type, pMD, fNeedsSlot, pMD->GetDomain(), &amt);

    if (fNeedsSlot)
    {
        // Fill in the slot with pointer to entrypoint.
        *(PTR_TADDR)(PTR_HOST_TO_TADDR(pPrecode) + pPrecode->SizeOf()) = pPrecode->GetEntryPoint();
    }

    if (target != NULL)
    {
        pPrecode->SetTargetInterlocked(target, FALSE);
    }

    m_hashTable.Add(pPrecode);
    amt.SuppressRelease();
    
    // If we have not set the target, we will take a trip through prestub
    INDEBUG(if (target == NULL) pMD->IncPermittedPrestubCalls());

    return (PBYTE)pPrecode->GetEntryPoint();
}
#endif // DACCESS_COMPILE

