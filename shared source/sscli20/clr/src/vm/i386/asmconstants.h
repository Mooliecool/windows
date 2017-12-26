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
// asmconstants.h -
//
// This header defines field offsets and constants used by assembly code
// Be sure to rebuild clr/src/vm/ceemain.cpp after changing this file, to
// ensure that the constants match the expected C/C++ values

#ifndef _X86_ 
#error this file should only be used on an X86 platform
#endif

#include "../../inc/switches.h"

#ifndef ASMCONSTANTS_C_ASSERT
#define ASMCONSTANTS_C_ASSERT(cond)
#endif

#ifndef ASMCONSTANTS_RUNTIME_ASSERT
#define ASMCONSTANTS_RUNTIME_ASSERT(cond)
#endif

//***************************************************************************
 #define HAS_TRACK_CXX_EXCEPTION_CODE_HACK 0



// from clr/src/vm/remoting.h
#define TP_OFFSET_STUBDATA    0x8
#define TP_OFFSET_MT          0xc
#define TP_OFFSET_STUB        0x14

// CONTEXT from rotor_pal.h
#define CONTEXT_Edi 0x9c
ASMCONSTANTS_C_ASSERT(CONTEXT_Edi == offsetof(CONTEXT,Edi))

#define CONTEXT_Esi 0xa0
ASMCONSTANTS_C_ASSERT(CONTEXT_Esi == offsetof(CONTEXT,Esi))

#define CONTEXT_Ebx 0xa4
ASMCONSTANTS_C_ASSERT(CONTEXT_Ebx == offsetof(CONTEXT,Ebx))

#define CONTEXT_Edx 0xa8
ASMCONSTANTS_C_ASSERT(CONTEXT_Edx == offsetof(CONTEXT,Edx))

#define CONTEXT_Eax 0xb0
ASMCONSTANTS_C_ASSERT(CONTEXT_Eax == offsetof(CONTEXT,Eax))

#define CONTEXT_Ebp 0xb4
ASMCONSTANTS_C_ASSERT(CONTEXT_Ebp == offsetof(CONTEXT,Ebp))

#define CONTEXT_Eip 0xb8
ASMCONSTANTS_C_ASSERT(CONTEXT_Eip == offsetof(CONTEXT,Eip))

#define CONTEXT_Esp 0xc4
ASMCONSTANTS_C_ASSERT(CONTEXT_Esp == offsetof(CONTEXT,Esp))

// SYSTEM_INFO from rotor_pal.h
#define SYSTEM_INFO_dwNumberOfProcessors 20 
ASMCONSTANTS_C_ASSERT(SYSTEM_INFO_dwNumberOfProcessors == offsetof(SYSTEM_INFO,dwNumberOfProcessors))

// EHContext from clr/src/vm/i386/cgencpu.h
#define EHContext_Eax 0x00
ASMCONSTANTS_C_ASSERT(EHContext_Eax == offsetof(EHContext,Eax))

#define EHContext_Ebx 0x04
ASMCONSTANTS_C_ASSERT(EHContext_Ebx == offsetof(EHContext,Ebx))

#define EHContext_Ecx 0x08
ASMCONSTANTS_C_ASSERT(EHContext_Ecx == offsetof(EHContext,Ecx))

#define EHContext_Edx 0x0c
ASMCONSTANTS_C_ASSERT(EHContext_Edx == offsetof(EHContext,Edx))

#define EHContext_Esi 0x10
ASMCONSTANTS_C_ASSERT(EHContext_Esi == offsetof(EHContext,Esi))

#define EHContext_Edi 0x14
ASMCONSTANTS_C_ASSERT(EHContext_Edi == offsetof(EHContext,Edi))

#define EHContext_Ebp 0x18
ASMCONSTANTS_C_ASSERT(EHContext_Ebp == offsetof(EHContext,Ebp))

#define EHContext_Esp 0x1c
ASMCONSTANTS_C_ASSERT(EHContext_Esp == offsetof(EHContext,Esp))

#define EHContext_Eip 0x20
ASMCONSTANTS_C_ASSERT(EHContext_Eip == offsetof(EHContext,Eip))


// from clr/src/fjit/helperframe.h
#define MachState__pEdi           0
ASMCONSTANTS_C_ASSERT(MachState__pEdi == offsetof(MachState, _pEdi))

#define MachState__edi            4
ASMCONSTANTS_C_ASSERT(MachState__edi == offsetof(MachState, _edi))

#define MachState__pEsi           8
ASMCONSTANTS_C_ASSERT(MachState__pEsi == offsetof(MachState, _pEsi))

#define MachState__esi            12
ASMCONSTANTS_C_ASSERT(MachState__esi == offsetof(MachState, _esi))

#define MachState__pEbx           16
ASMCONSTANTS_C_ASSERT(MachState__pEbx == offsetof(MachState, _pEbx))

#define MachState__ebx            20
ASMCONSTANTS_C_ASSERT(MachState__ebx == offsetof(MachState, _ebx))

#define MachState__pEbp           24
ASMCONSTANTS_C_ASSERT(MachState__pEbp == offsetof(MachState, _pEbp))

#define MachState__ebp            28
ASMCONSTANTS_C_ASSERT(MachState__ebp == offsetof(MachState, _ebp))

#define MachState__esp            32
ASMCONSTANTS_C_ASSERT(MachState__esp == offsetof(MachState, _esp))

#define MachState__pRetAddr       36
ASMCONSTANTS_C_ASSERT(MachState__pRetAddr == offsetof(MachState, _pRetAddr))

#define LazyMachState_captureEbp  40
ASMCONSTANTS_C_ASSERT(LazyMachState_captureEbp == offsetof(LazyMachState, captureEbp))

#define LazyMachState_captureEsp  44
ASMCONSTANTS_C_ASSERT(LazyMachState_captureEsp == offsetof(LazyMachState, captureEsp))

#define LazyMachState_captureEip  48
ASMCONSTANTS_C_ASSERT(LazyMachState_captureEip == offsetof(LazyMachState, captureEip))


// The size of the overall stack frame for NDirectGenericStubWorker,
// including the parameters, return address, preserved registers, and
// local variables, but not including any __alloca'd memory.
//
// The size counts two arguments, the return value, and one preserved register
// for a total of 16 bytes, plus 16 bytes of locals
#define NDirectGenericWorkerFrameSize (16+16+8)

#define SIZEOF_TailCallFrame 32
ASMCONSTANTS_C_ASSERT(SIZEOF_TailCallFrame == sizeof(TailCallFrame))

#define SIZEOF_GSCookie 4

// ICodeManager::SHADOW_SP_IN_FILTER from clr/src/inc/eetwain.h
#define SHADOW_SP_IN_FILTER_ASM 0x1
ASMCONSTANTS_C_ASSERT(SHADOW_SP_IN_FILTER_ASM == ICodeManager::SHADOW_SP_IN_FILTER)

// from clr/src/inc/corinfo.h
#define CORINFO_NullReferenceException_ASM 0
ASMCONSTANTS_C_ASSERT(CORINFO_NullReferenceException_ASM == CORINFO_NullReferenceException)

#define CORINFO_IndexOutOfRangeException_ASM 3
ASMCONSTANTS_C_ASSERT(CORINFO_IndexOutOfRangeException_ASM == CORINFO_IndexOutOfRangeException)

#define CORINFO_OverflowException_ASM 4
ASMCONSTANTS_C_ASSERT(CORINFO_OverflowException_ASM == CORINFO_OverflowException)

#define CORINFO_SynchronizationLockException_ASM 5
ASMCONSTANTS_C_ASSERT(CORINFO_SynchronizationLockException_ASM == CORINFO_SynchronizationLockException)

#define CORINFO_ArrayTypeMismatchException_ASM 6
ASMCONSTANTS_C_ASSERT(CORINFO_ArrayTypeMismatchException_ASM == CORINFO_ArrayTypeMismatchException)

#define CORINFO_ArgumentNullException_ASM 8
ASMCONSTANTS_C_ASSERT(CORINFO_ArgumentNullException_ASM == CORINFO_ArgumentNullException)

#define CORINFO_ArgumentException_ASM 9
ASMCONSTANTS_C_ASSERT(CORINFO_ArgumentException_ASM == CORINFO_ArgumentException)

// from clr/src/vm/ml.h
#define MLHF_THISCALL_ASM          0x0100
ASMCONSTANTS_C_ASSERT(MLHF_THISCALL_ASM == MLHF_THISCALL)

#define MLHF_THISCALLHIDDENARG_ASM 0x0200
ASMCONSTANTS_C_ASSERT(MLHF_THISCALLHIDDENARG_ASM == MLHF_THISCALLHIDDENARG)

// from clr/src/vm/threads.h
    #define Thread_m_Context    0x3c
    ASMCONSTANTS_C_ASSERT(Thread_m_Context == offsetof(Thread, m_Context))

#define Thread_m_State      0x04
ASMCONSTANTS_C_ASSERT(Thread_m_State == offsetof(Thread, m_State))

#define Thread_m_pFrame     0x0C
ASMCONSTANTS_C_ASSERT(Thread_m_pFrame == offsetof(Thread, m_pFrame))

#define Thread_m_dwLockCount 0x18
ASMCONSTANTS_C_ASSERT(Thread_m_dwLockCount == offsetof(Thread, m_dwLockCount))

#define Thread_m_ThreadId 0x1c
ASMCONSTANTS_C_ASSERT(Thread_m_ThreadId == offsetof(Thread, m_ThreadId))

#define TS_CatchAtSafePoint_ASM 0x5F
ASMCONSTANTS_C_ASSERT(Thread::TS_CatchAtSafePoint == TS_CatchAtSafePoint_ASM)

#define TS_AbortRequested_ASM 0x00000001
ASMCONSTANTS_C_ASSERT(Thread::TS_AbortRequested == TS_AbortRequested_ASM)





// from clr/src/vm/syncblk.h
#define SizeOfSyncTableEntry_ASM 8
ASMCONSTANTS_C_ASSERT(sizeof(SyncTableEntry) == SizeOfSyncTableEntry_ASM)

#define SyncBlockIndexOffset_ASM 4
ASMCONSTANTS_C_ASSERT(sizeof(ObjHeader) - offsetof(ObjHeader, m_SyncBlockValue) == SyncBlockIndexOffset_ASM)

#ifndef __GNUC__
#define SyncTableEntry_m_SyncBlock 0
ASMCONSTANTS_C_ASSERT(offsetof(SyncTableEntry, m_SyncBlock) == SyncTableEntry_m_SyncBlock)

#define SyncBlock_m_Monitor 0
ASMCONSTANTS_C_ASSERT(offsetof(SyncBlock, m_Monitor) == SyncBlock_m_Monitor)

#define AwareLock_m_MonitorHeld 0
ASMCONSTANTS_C_ASSERT(offsetof(AwareLock, m_MonitorHeld) == AwareLock_m_MonitorHeld)
#else
// The following 3 offsets have value of 0, and must be
// defined to be an empty string. Otherwise, gas may generate assembly
// code with 0 displacement if 0 is left in the displacement field
// of an instruction.
#define SyncTableEntry_m_SyncBlock // 0
ASMCONSTANTS_C_ASSERT(offsetof(SyncTableEntry, m_SyncBlock) == 0)

#define SyncBlock_m_Monitor // 0
ASMCONSTANTS_C_ASSERT(offsetof(SyncBlock, m_Monitor) == 0)

#define AwareLock_m_MonitorHeld // 0
ASMCONSTANTS_C_ASSERT(offsetof(AwareLock, m_MonitorHeld) == 0)
#endif // !__GNUC__

#define AwareLock_m_HoldingThread 8
ASMCONSTANTS_C_ASSERT(offsetof(AwareLock, m_HoldingThread) == AwareLock_m_HoldingThread)

#define AwareLock_m_Recursion 4
ASMCONSTANTS_C_ASSERT(offsetof(AwareLock, m_Recursion) == AwareLock_m_Recursion)

#define BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM 0x08000000
ASMCONSTANTS_C_ASSERT(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM == BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX)

#define BIT_SBLK_SPIN_LOCK_ASM 0x10000000
ASMCONSTANTS_C_ASSERT(BIT_SBLK_SPIN_LOCK_ASM == BIT_SBLK_SPIN_LOCK)

#define SBLK_MASK_LOCK_THREADID_ASM 0x000003FF   // special value of 0 + 1023 thread ids
ASMCONSTANTS_C_ASSERT(SBLK_MASK_LOCK_THREADID_ASM == SBLK_MASK_LOCK_THREADID)

#define SBLK_MASK_LOCK_RECLEVEL_ASM 0x0000FC00   // 64 recursion levels
ASMCONSTANTS_C_ASSERT(SBLK_MASK_LOCK_RECLEVEL_ASM == SBLK_MASK_LOCK_RECLEVEL)

#define SBLK_LOCK_RECLEVEL_INC_ASM 0x00000400   // each level is this much higher than the previous one
ASMCONSTANTS_C_ASSERT(SBLK_LOCK_RECLEVEL_INC_ASM == SBLK_LOCK_RECLEVEL_INC)

#define BIT_SBLK_IS_HASHCODE_ASM 0x04000000
ASMCONSTANTS_C_ASSERT(BIT_SBLK_IS_HASHCODE_ASM == BIT_SBLK_IS_HASHCODE)

#define MASK_SYNCBLOCKINDEX_ASM  0x03ffffff // ((1<<SYNCBLOCKINDEX_BITS)-1)
ASMCONSTANTS_C_ASSERT(MASK_SYNCBLOCKINDEX_ASM == MASK_SYNCBLOCKINDEX)

// BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM + BIT_SBLK_SPIN_LOCK_ASM + 
// SBLK_MASK_LOCK_THREADID_ASM + SBLK_MASK_LOCK_RECLEVEL_ASM
#define SBLK_COMBINED_MASK_ASM 0x1800ffff
ASMCONSTANTS_C_ASSERT(SBLK_COMBINED_MASK_ASM == (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + BIT_SBLK_SPIN_LOCK + SBLK_MASK_LOCK_THREADID + SBLK_MASK_LOCK_RECLEVEL))

// BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_ASM + BIT_SBLK_SPIN_LOCK_ASM
#define BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_SPIN_LOCK_ASM 0x18000000
ASMCONSTANTS_C_ASSERT(BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX_SPIN_LOCK_ASM == (BIT_SBLK_IS_HASH_OR_SYNCBLKINDEX + BIT_SBLK_SPIN_LOCK))

// BIT_SBLK_IS_HASHCODE + BIT_SBLK_SPIN_LOCK
#define BIT_SBLK_IS_HASHCODE_OR_SPIN_LOCK_ASM 0x14000000
ASMCONSTANTS_C_ASSERT(BIT_SBLK_IS_HASHCODE_OR_SPIN_LOCK_ASM == (BIT_SBLK_IS_HASHCODE + BIT_SBLK_SPIN_LOCK))

#undef ASMCONSTANTS_C_ASSERT
#undef ASMCONSTANTS_RUNTIME_ASSERT
