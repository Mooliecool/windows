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
/*
 * GCCOMMON.CPP 
 *
 * Code common to both SVR and WKS gcs
 */

#include "common.h"

SVAL_IMPL_INIT(DWORD,GCHeap,gcHeapType,GCHeap::GC_HEAP_INVALID);

SPTR_IMPL(Thread,GCHeap,FinalizerThread);
SPTR_IMPL(Thread,GCHeap,GcThread);

/* global versions of the card table and brick table */ 
GPTR_IMPL(DWORD,g_card_table);

/* absolute bounds of the GC memory */
GPTR_IMPL_INIT(BYTE,g_lowest_address,0);
GPTR_IMPL_INIT(BYTE,g_highest_address,0);

#ifndef DACCESS_COMPILE

BYTE* g_ephemeral_low = (BYTE*)1; 
BYTE* g_ephemeral_high = (BYTE*)~0;


volatile LONG m_GCLock = -1;

inline
BOOL power_of_two_p (size_t integer)
{
    STATIC_CONTRACT_LEAF;
    return !(integer & (integer-1));
}

#if defined(_X86_) || defined(_WIN64)

// implemented in assembly
// extern "C" HCIMPL2_RAW(VOID, JIT_CheckedWriteBarrier, Object **dst, Object *refUNSAFE)
// extern "C" HCIMPL2_RAW(VOID, JIT_WriteBarrier, Object **dst, Object *refUNSAFE)

#else // _X86_ || _WIN64

// This function is a JIT helper, but it must NOT use HCIMPL2 because it
// modifies Thread state that will not be restored if an exception occurs
// inside of memset.  A normal EH unwind will not occur.
extern "C" HCIMPL2_RAW(VOID, JIT_CheckedWriteBarrier, Object **dst, Object *refUNSAFE)
{
    // Must use static contract here, because if an AV occurs, a normal EH
    // unwind will not occur, and destructors will not run.
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    // no HELPER_METHOD_FRAME because we are MODE_COOPERATIVE, GC_NOTRIGGER
    
    GCHeap::GetGCHeap()->CheckedWriteBarrierHelper(dst, refUNSAFE);   // can throw
}
HCIMPLEND_RAW

// This function is a JIT helper, but it must NOT use HCIMPL2 because it
// modifies Thread state that will not be restored if an exception occurs
// inside of memset.  A normal EH unwind will not occur.
extern "C" HCIMPL2_RAW(VOID, JIT_WriteBarrier, Object **dst, Object *refUNSAFE)
{
    // Must use static contract here, because if an AV occurs, a normal EH
    // unwind will not occur, and destructors will not run.
    STATIC_CONTRACT_MODE_COOPERATIVE;
    STATIC_CONTRACT_THROWS;
    STATIC_CONTRACT_GC_NOTRIGGER;
    
    // no HELPER_METHOD_FRAME because we are MODE_COOPERATIVE, GC_NOTRIGGER
    
    GCHeap::GetGCHeap()->WriteBarrierHelper(dst, refUNSAFE);  // can throw
}
HCIMPLEND_RAW

#endif // _X86_ || _WIN64

#endif // !DACCESS_COMPILE

