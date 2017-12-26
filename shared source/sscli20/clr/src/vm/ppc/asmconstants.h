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

#ifndef _PPC_
#error this file should only be used on an PPC platform
#endif // _PPC_

//-----------------------------------------------------------------------------

#ifndef ASMCONSTANTS_C_ASSERT
#define ASMCONSTANTS_C_ASSERT(cond)
#endif

#ifndef ASMCONSTANTS_RUNTIME_ASSERT 
#define ASMCONSTANTS_RUNTIME_ASSERT(cond)
#endif

// from clr/src/vm/remoting.h
#define TP_OFFSET_STUBDATA    0x8
#define TP_OFFSET_MT          0xc
#define TP_OFFSET_STUB        0x14

// from clr/src/vm/ppc/gmscpu.h
#define MachState__Regs     0
ASMCONSTANTS_C_ASSERT(MachState__Regs == offsetof(MachState, _Regs))

#define MachState__pRegs    (4*NUM_CALLEESAVED_REGISTERS)
ASMCONSTANTS_C_ASSERT(MachState__pRegs == offsetof(MachState, _pRegs))

#define MachState__cr       (8*NUM_CALLEESAVED_REGISTERS)
ASMCONSTANTS_C_ASSERT(MachState__cr == offsetof(MachState, _cr))

#define MachState__sp       (MachState__cr+4)
ASMCONSTANTS_C_ASSERT(MachState__sp == offsetof(MachState, _sp))

#define MachState__pRetAddr (MachState__sp+4)
ASMCONSTANTS_C_ASSERT(MachState__pRetAddr == offsetof(MachState, _pRetAddr))

#define MachState_size      (8*NUM_CALLEESAVED_REGISTERS + 12)
ASMCONSTANTS_C_ASSERT(MachState_size == sizeof(MachState))

#define LazyMachState_captureSp     (MachState__pRetAddr+4)
ASMCONSTANTS_C_ASSERT(LazyMachState_captureSp == offsetof(LazyMachState, captureSp))

#define LazyMachState_captureSp2    (LazyMachState_captureSp+4)
ASMCONSTANTS_C_ASSERT(LazyMachState_captureSp2 == offsetof(LazyMachState, captureSp2))

#define LazyMachState_capturePC     (LazyMachState_captureSp2+4)
ASMCONSTANTS_C_ASSERT(LazyMachState_capturePC == offsetof(LazyMachState, capturePC))


// ArgumentRegisters from clr/src/vm/ppc/cgencpu.h
#define ArgumentRegisters_r 0
ASMCONSTANTS_C_ASSERT(ArgumentRegisters_r == offsetof(ArgumentRegisters, r))

#define ArgumentRegisters_f 32
ASMCONSTANTS_C_ASSERT(ArgumentRegisters_f == offsetof(ArgumentRegisters, f))

#define ArgumentRegisters_size (8*NUM_FLOAT_ARGUMENT_REGISTERS+4*NUM_ARGUMENT_REGISTERS)
ASMCONSTANTS_C_ASSERT(ArgumentRegisters_size == sizeof(ArgumentRegisters))

// CalleeSavedRegisters from clr/src/vm/ppc/cgencpu.h
#define CalleeSavedRegisters_cr 0
ASMCONSTANTS_C_ASSERT(CalleeSavedRegisters_cr == offsetof(CalleeSavedRegisters, cr))

#define CalleeSavedRegisters_r  4
ASMCONSTANTS_C_ASSERT(CalleeSavedRegisters_r == offsetof(CalleeSavedRegisters, r))

#define CalleeSavedRegisters_f  (4*(NUM_CALLEESAVED_REGISTERS+1))
ASMCONSTANTS_C_ASSERT(CalleeSavedRegisters_f == offsetof(CalleeSavedRegisters, f))

#define CalleeSavedRegisters_size (8*NUM_FLOAT_CALLEESAVED_REGISTERS+4*NUM_CALLEESAVED_REGISTERS+4)
ASMCONSTANTS_C_ASSERT(CalleeSavedRegisters_size == sizeof(CalleeSavedRegisters))


// EHContext from clr/src/vm/ppc/cgencpu.h
#define EHContext_R     0
ASMCONSTANTS_C_ASSERT(EHContext_R == offsetof(EHContext,R))

#define EHContext_F     (4*32)
ASMCONSTANTS_C_ASSERT(EHContext_F == offsetof(EHContext,F))

#define EHContext_CR    (EHContext_F+8*NUM_FLOAT_CALLEESAVED_REGISTERS)
ASMCONSTANTS_C_ASSERT(EHContext_CR == offsetof(EHContext,CR))


// FaultingExceptionFrame from clr/src/vm/frames.h
#define FaultingExceptionFrame_m_regs       40
ASMCONSTANTS_C_ASSERT(FaultingExceptionFrame_m_regs == offsetof(FaultingExceptionFrame, m_regs))

#define FaultingExceptionFrame_m_SavedSP    16
ASMCONSTANTS_C_ASSERT(FaultingExceptionFrame_m_SavedSP == offsetof(FaultingExceptionFrame, m_SavedSP))


// The size of the overall stack frame for NDirectGenericStubWorker,
// including the parameters, return address, preserved registers, and
// local variables, but not including any __alloca'd memory.
// 
#define NDirectGenericWorkerFrameSize   48

// ICodeManager::SHADOW_SP_IN_FILTER from clr/src/inc/eetwain.h
#define SHADOW_SP_IN_FILTER_ASM 0x1
ASMCONSTANTS_C_ASSERT(SHADOW_SP_IN_FILTER_ASM == ICodeManager::SHADOW_SP_IN_FILTER);

// from clr/src/vm/threads.h
#define Thread_m_Context    0x3C
ASMCONSTANTS_C_ASSERT(Thread_m_Context == offsetof(Thread, m_Context));

