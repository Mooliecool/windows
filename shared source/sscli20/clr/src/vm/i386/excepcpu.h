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
// EXCEPX86.H -
//
// This header file is optionally included from Excep.h if the target platform is x86
//

#ifndef __excepx86_h__
#define __excepx86_h__

#include "corerror.h"  // HResults for the COM+ Runtime

#include "../dlls/mscorrc/resource.h"

#define THROW_CONTROL_FOR_THREAD_FUNCTION  ThrowControlForThread

#define STATUS_CLR_GCCOVER_CODE         STATUS_PRIVILEGED_INSTRUCTION

class Thread;


#define INSTALL_EXCEPTION_HANDLING_RECORD(record)               \
    {                                                           \
        PEXCEPTION_REGISTRATION_RECORD __record = (record);     \
        __record->pvFilterParameter = __record;                 \
        __record->dwFlags = PAL_EXCEPTION_FLAGS_UNWINDONLY;     \
        __record->typeOfHandler = PALExceptFilter;              \
        PAL_TryHelper(__record);                                \
    }

#define UNINSTALL_EXCEPTION_HANDLING_RECORD(record)             \
    {                                                           \
        PEXCEPTION_REGISTRATION_RECORD __record = (record);     \
        PAL_EndTryHelper(__record, 0);                          \
    }


// stackOverwriteBarrier is used to detect overwriting of stack which will mess up handler registration
#if defined(_DEBUG)
#define DECLARE_CPFH_EH_RECORD(pCurThread) \
    FrameHandlerExRecordWithBarrier *___pExRecordWithBarrier = (FrameHandlerExRecordWithBarrier *)_alloca(sizeof(FrameHandlerExRecordWithBarrier)); \
    for (int ___i =0; ___i < STACK_OVERWRITE_BARRIER_SIZE; ___i++) \
        ___pExRecordWithBarrier->m_StackOverwriteBarrier[___i] = STACK_OVERWRITE_BARRIER_VALUE; \
    FrameHandlerExRecord *___pExRecord = &(___pExRecordWithBarrier->m_ExRecord); \
    ___pExRecord->m_ExReg.Handler = (PEXCEPTION_ROUTINE)COMPlusFrameHandler; \
    ___pExRecord->m_pEntryFrame = (pCurThread)->GetFrame();

#else
#define DECLARE_CPFH_EH_RECORD(pCurThread) \
    FrameHandlerExRecord *___pExRecord = (FrameHandlerExRecord *)_alloca(sizeof(FrameHandlerExRecord)); \
    ___pExRecord->m_ExReg.Handler = (PEXCEPTION_ROUTINE)COMPlusFrameHandler; \
    ___pExRecord->m_pEntryFrame = (pCurThread)->GetFrame();

#endif

PEXCEPTION_REGISTRATION_RECORD GetCurrentSEHRecord();
PEXCEPTION_REGISTRATION_RECORD GetFirstCOMPlusSEHRecord(Thread*);

// Determine the address of the instruction that made the current call. For X86, pass
// esp where it contains the return address and will adjust back 5 bytes for the call
inline
LPVOID GetAdjustedCallAddress(LPVOID esp)
{
    LEAF_CONTRACT;
    return (*(BYTE**)esp - 5);
}

#endif // __excepx86_h__
