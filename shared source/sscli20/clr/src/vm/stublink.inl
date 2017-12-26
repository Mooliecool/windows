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
// StubLink.inl
//
// Defines inline functions for StubLinker
//

#ifndef __STUBLINK_INL__
#define __STUBLINK_INL__

#include "stublink.h"
#include "eeconfig.h"


#ifdef STUBLINKER_GENERATES_UNWIND_INFO

inline //static
SIZE_T StubUnwindInfoHeader::ComputeSize (UCHAR nTotalSlots, UCHAR nEntryPoints, SIZE_T cbPrefix)
{
    LEAF_CONTRACT;

    return ALIGN_UP(  FIELD_OFFSET(StubUnwindInfoHeader, rgEntryPoints[0])
                    + nEntryPoints * (sizeof(UNWIND_INFO) + sizeof(RUNTIME_FUNCTION))
                    // UNWIND_INFO includes 1 UNWIND_CODE
                    + (nTotalSlots - nEntryPoints) * sizeof(UNWIND_CODE)
                    + sizeof(StubUnwindInfoHeaderSuffix)
                    + cbPrefix, sizeof(void*)) - cbPrefix;
}


inline
StubUnwindInfoHeaderSuffix *StubUnwindInfoHeader::GetSuffix (PBYTE pbSegmentBaseAddress)
{
    WRAPPER_CONTRACT;

    TADDR pbEntryPoint = (TADDR)
        (pbSegmentBaseAddress + rgEntryPoints[0].FunctionEntry.BeginAddress);
    Stub *pStub = Stub::RecoverStub(pbEntryPoint);
    return pStub->GetUnwindInfoHeaderSuffix();
}


#ifndef DACCESS_COMPILE

inline
void StubUnwindInfoHeader::Init ()
{
    LEAF_CONTRACT;
    
    pNext = (StubUnwindInfoHeader*)(SIZE_T)1;
}


inline
bool StubUnwindInfoHeader::IsRegistered ()
{
    LEAF_CONTRACT;

    return pNext != (StubUnwindInfoHeader*)(SIZE_T)1;
}

#endif // #ifndef DACCESS_COMPILE

#endif // STUBLINKER_GENERATES_UNWIND_INFO


inline
void StubLinker::Push(UINT size)
{
    LEAF_CONTRACT;

    m_stackSize += size;
    UnwindAllocStack(size);
}


inline
void StubLinker::Pop(UINT size)
{
    LEAF_CONTRACT;

    m_stackSize -= size;
}


inline
VOID StubLinker::EmitUnwindInfoCheck()
{
#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
    if (g_pConfig->IsStubLinkerUnwindInfoVerificationOn())
    {
        if (!m_pUnwindInfoCheckLabel)
            m_pUnwindInfoCheckLabel = NewCodeLabel();
        EmitUnwindInfoCheckWorker(m_pUnwindInfoCheckLabel);
    }
#endif
}


#ifndef STUBLINKER_GENERATES_UNWIND_INFO

inline VOID StubLinker::UnwindSavedReg (UCHAR reg, ULONG SPRelativeOffset) {LEAF_CONTRACT;}
inline VOID StubLinker::UnwindAllocStack (ULONG FrameSizeIncrement) {LEAF_CONTRACT;}
inline VOID StubLinker::UnwindSetFramePointer (UCHAR reg) {LEAF_CONTRACT;}
inline VOID StubLinker::UnwindBeginNewEntryPoint () {LEAF_CONTRACT;}

inline VOID StubLinker::UnwindPushedReg (UCHAR reg)
{
    LEAF_CONTRACT;

    m_stackSize += sizeof(void*);
}

#endif // !STUBLINKER_GENERATES_UNWIND_INFO


#endif // !__STUBLINK_INL__

