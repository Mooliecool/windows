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

#ifndef __ExInfo_h__
#define __ExInfo_h__

#include "exstatecommon.h"

typedef DPTR(class ExInfo) PTR_ExInfo;
class ExInfo
{
    friend class ThreadExceptionState;
    friend class ClrDataExceptionState;
   
public:
    
    BOOL    IsHeapAllocated() 
    { 
        LEAF_CONTRACT; 
        return m_StackAddress != (void *) this; 
    }

    void CopyAndClearSource(ExInfo *from);

    void UnwindExInfo(VOID* limit);

public:
    OBJECTHANDLE    m_hThrowable;       // thrown exception
    PTR_Frame       m_pSearchBoundary;  // topmost frame for current managed frame group
    DWORD           m_ExceptionCode;    // After a catch of a COM+ exception, pointers/context are trashed.

    union 
    {
        EXCEPTION_REGISTRATION_RECORD* m_pBottomMostHandler; // most recent EH record registered
        EXCEPTION_REGISTRATION_RECORD* m_pCatchHandler;      // reg frame for catching handler
    };
    LPVOID              m_dEsp;             // Esp when  fault occured, OR esp to restore on endcatch

    StackTraceInfo      m_StackTraceInfo;

    PTR_ExInfo          m_pPrevNestedInfo;  // pointer to nested info if are handling nested exception

    size_t*             m_pShadowSP;        // Zero this after endcatch

    EXCEPTION_RECORD        m_ExceptionRecord;
    PTR_CONTEXT             m_pContext;

    // We have a rare case where (re-entry to the EE from an unmanaged filter) where we
    // need to create a new ExInfo ... but don't have a nested handler for it.  The handlers
    // use stack addresses to figure out their correct lifetimes.  This stack location is
    // used for that.  For most records, it will be the stack address of the ExInfo ... but
    // for some records, it will be a pseudo stack location -- the place where we think
    // the record should have been (except for the re-entry case).
    //
    //
    void* m_StackAddress; // A pseudo or real stack location for this record.

public:

    DebuggerExState     m_DebuggerExState;
    EHClauseInfo        m_EHClauseInfo;
    ExceptionFlags      m_ExceptionFlags;

#ifdef DACCESS_COMPILE
    void EnumMemoryRegions(CLRDataEnumMemoryFlags flags);
#endif

    void Init();
    ExInfo() DAC_EMPTY();

    void DestroyExceptionHandle();

private:    
    // Don't allow this
    ExInfo& operator=(const ExInfo &from);
};


#endif // __ExInfo_h__
