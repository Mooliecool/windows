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
// stublink.cpp
//

#include "common.h"

#include "threads.h"
#include "excep.h"
#include "stublink.h"
#include "perfcounters.h"
#include "stubgen.h"
#include "stublink.inl"
#include "memoryreport.h"


#ifndef DACCESS_COMPILE


//************************************************************************
// CodeElement
//
// There are two types of CodeElements: CodeRuns (a stream of uninterpreted
// code bytes) and LabelRefs (an instruction containing
// a fixup.)
//************************************************************************
struct CodeElement
{
    enum CodeElementType {
        kCodeRun  = 0,
        kLabelRef = 1,
    };


    CodeElementType     m_type;  // kCodeRun or kLabelRef
    CodeElement        *m_next;  // ptr to next CodeElement

    // Used as workspace during Link(): holds the offset relative to
    // the start of the final stub.
    UINT                m_globaloffset;
    UINT                m_dataoffset;
};


//************************************************************************
// CodeRun: A run of uninterrupted code bytes.
//************************************************************************

#ifdef _DEBUG
#define CODERUNSIZE 3
#else
#define CODERUNSIZE 32
#endif

struct CodeRun : public CodeElement
{
    UINT    m_numcodebytes;       // how many bytes are actually used
    BYTE    m_codebytes[CODERUNSIZE];
};

//************************************************************************
// LabelRef: An instruction containing an embedded label reference
//************************************************************************
struct LabelRef : public CodeElement
{
    // provides platform-specific information about the instruction
    InstructionFormat    *m_pInstructionFormat;

    // a variation code (interpretation is specific to the InstructionFormat)
    //  typically used to customize an instruction (e.g. with a condition
    //  code.)
    UINT                 m_variationCode;


    CodeLabel           *m_target;

    // Workspace during the link phase
    UINT                 m_refsize;


    // Pointer to next LabelRef
    LabelRef            *m_nextLabelRef;
};


//************************************************************************
// IntermediateUnwindInfo
//************************************************************************

#ifdef STUBLINKER_GENERATES_UNWIND_INFO


// List of unwind operations, queued in StubLinker::m_pUnwindInfoList.
struct IntermediateUnwindInfo
{
    IntermediateUnwindInfo *pNext;
    CodeRun *pCodeRun;
    UINT LocalOffset;
};

struct IntermediateUnwindInfoPrologueList : IntermediateUnwindInfo
{
    IntermediateUnwindInfoPrologueList *pNextPrologue;
};


// Overload an unwind operator that we'll never use.  This marks the start of
// a new entry point.
#define CLR_UWOP_NEW_PROLOGUE UWOP_PUSH_MACHFRAME


// Initial value for StubLinker::m_pUnwindInfoList.  This normalizes the
// multiple prologue/list end cases in StubLinker::EmitUnwindInfo.
IntermediateUnwindInfoPrologueList g_StubLinkerUnwindInfoListInitializer;
CodeRun g_StubLinkerUnwindInfoListInitializerCodeRun;


StubUnwindInfoHeapSegment *g_StubHeapSegments;
CrstStatic g_StubUnwindInfoHeapSegmentsCrst;
#ifdef _DEBUG  // for unit test
void *__DEBUG__g_StubHeapSegments = &g_StubHeapSegments;
#endif


//
// Callback registered via RtlInstallFunctionTableCallback.  Called by
// RtlpLookupDynamicFunctionEntry to locate RUNTIME_FUNCTION entry for a PC
// found within a portion of a heap that contains stub code.
//
PRUNTIME_FUNCTION
FindStubFunctionEntry (
    IN ULONG64 ControlPc,
    IN PVOID Context
    )
{
    CONSISTENCY_CHECK(DYNFNTABLE_STUB == IdentifyDynamicFunctionTableTypeFromContext(Context));

    StubUnwindInfoHeapSegment *pStubHeapSegment = (StubUnwindInfoHeapSegment*)DecodeDynamicFunctionTableContext(Context);

    //
    // The RUNTIME_FUNCTION entry contains ULONG offsets relative to the
    // segment base.  Stub::EmitUnwindInfo ensures that this cast is valid.
    //
    ULONG RelativeAddress = (ULONG)((BYTE*)ControlPc - pStubHeapSegment->pbBaseAddress);

    LOG((LF_STUBS, LL_INFO100000, "ControlPc %p, RelativeAddress 0x%x, pStubHeapSegment %p, pStubHeapSegment->pbBaseAddress %p\n",
            ControlPc,
            RelativeAddress,
            pStubHeapSegment,
            pStubHeapSegment->pbBaseAddress));

    //
    // Search this segment's list of stubs for an entry that includes the
    // segment-relative offset.
    //
    for (StubUnwindInfoHeader *pHeader = pStubHeapSegment->pUnwindHeaderList;
         pHeader;
         pHeader = pHeader->pNext)
    {
        StubUnwindInfoHeaderSuffix *pSuffix = pHeader->GetSuffix(pStubHeapSegment->pbBaseAddress);
        CONSISTENCY_CHECK(pSuffix->nEntryPoints > 0);

        // The entry points are in increasing address order.
        if (RelativeAddress >= pHeader->rgEntryPoints[0].FunctionEntry.BeginAddress)
        {
            RUNTIME_FUNCTION *pCurFunction = &pHeader->rgEntryPoints[0].FunctionEntry;
            RUNTIME_FUNCTION *pPrevFunction = NULL;

            for (UINT iEntryPoint = 0; iEntryPoint < pSuffix->nEntryPoints; iEntryPoint++)
            {
                LOG((LF_STUBS, LL_INFO100000, "pCurFunction %p, pCurFunction->BeginAddress 0x%x, pCurFunction->EndAddress 0x%x\n",
                        pCurFunction,
                        pCurFunction->BeginAddress,
                        pCurFunction->EndAddress));

                CONSISTENCY_CHECK(pCurFunction->EndAddress > pCurFunction->BeginAddress);
                CONSISTENCY_CHECK(!pPrevFunction || pPrevFunction->EndAddress <= pCurFunction->BeginAddress);

                // The entry points are in increasing address order.  They're
                // also contiguous, so after we're sure it's after the start of
                // the first function (checked above), we only need to test
                // the end address.
                if (RelativeAddress < pCurFunction->EndAddress)
                {
                    CONSISTENCY_CHECK(RelativeAddress >= pCurFunction->BeginAddress);

                    return pCurFunction;
                }

                //
                // Skip UNWIND_INFO field corresponding to the RUNTIME_FUNCTION.
                //
                UNWIND_INFO *pUnwindInfo = (UNWIND_INFO*)(  (BYTE*)pCurFunction
                                                          + FIELD_OFFSET(StubUnwindInfoHeader, rgEntryPoints[0].UnwindInfo)
                                                          - FIELD_OFFSET(StubUnwindInfoHeader, rgEntryPoints[0].FunctionEntry));
                _ASSERTE(pUnwindInfo == (UNWIND_INFO*)(pStubHeapSegment->pbBaseAddress + RUNTIME_FUNCTION__GetUnwindInfoAddress(pCurFunction)));

                //
                // Align on ULONG boundary.
                //
                C_ASSERT(sizeof(ULONG) == 2*sizeof(UNWIND_CODE));

                UINT nSlots = pUnwindInfo->CountOfUnwindCodes;
                nSlots += (nSlots & 1);

                //
                // If there's a personality routine or chained unwind info,
                // there's a ULONG offset appended.
                //
                if (pUnwindInfo->Flags & (UNW_FLAG_EHANDLER | UNW_FLAG_UHANDLER | UNW_FLAG_CHAININFO))
                    nSlots += 2;

                pPrevFunction = pCurFunction;
                pCurFunction = (RUNTIME_FUNCTION*)&pUnwindInfo->UnwindCode[nSlots];
                _ASSERTE(IS_ALIGNED(pCurFunction, sizeof(ULONG)));
            }

            //LOG((LF_STUBS, LL_INFO100000, "pCurFunction %p, pSuffix %p\n", pCurFunction, pSuffix));
            _ASSERTE((SIZE_T)ALIGN_UP(pCurFunction, sizeof(void*)) == (SIZE_T)(pSuffix+1));
        }
    }

    //
    // Return NULL to indicate that there is no RUNTIME_FUNCTION/unwind
    // information for this offset.
    //
    return NULL;
}


void UnregisterUnwindInfoInLoaderHeapCallback (PVOID pvAllocationBase, SIZE_T cbReserved)
{
    WRAPPER_CONTRACT;

    //
    // There may be multiple StubUnwindInfoHeapSegment's associated with a region.
    //

    LOG((LF_STUBS, LL_INFO1000, "Looking for stub unwind info for LoaderHeap segment %p size %p\n", pvAllocationBase, cbReserved));
    
    CrstHolder crst(&g_StubUnwindInfoHeapSegmentsCrst);

    StubUnwindInfoHeapSegment *pStubHeapSegment;
    for (StubUnwindInfoHeapSegment **ppPrevStubHeapSegment = &g_StubHeapSegments;
         pStubHeapSegment = *ppPrevStubHeapSegment; )
    {
        LOG((LF_STUBS, LL_INFO10000, "    have unwind info for address %p size %p\n", pStubHeapSegment->pbBaseAddress, pStubHeapSegment->cbSegment));

        // If heap region ends before stub segment
        if ((BYTE*)pvAllocationBase + cbReserved <= pStubHeapSegment->pbBaseAddress)
        {
            // The list is ordered, so address range is between segments
            break;
        }

        // The given heap segment base address may fall within a prereserved
        // region that was given to the heap when the heap was constructed, so
        // pvAllocationBase may be > pbBaseAddress.  Also, there could be
        // multiple segments for each heap region, so pvAllocationBase may be
        // < pbBaseAddress.  So...there is no meaningful relationship between
        // pvAllocationBase and pbBaseAddress.

        // If heap region starts before end of stub segment
        if ((BYTE*)pvAllocationBase < pStubHeapSegment->pbBaseAddress + pStubHeapSegment->cbSegment)
        {
            _ASSERTE((BYTE*)pvAllocationBase + cbReserved <= pStubHeapSegment->pbBaseAddress + pStubHeapSegment->cbSegment);

            DeleteEEFunctionTable(pStubHeapSegment);

            *ppPrevStubHeapSegment = pStubHeapSegment->pNext;

            delete pStubHeapSegment;
        }
        else
        {
            ppPrevStubHeapSegment = &pStubHeapSegment->pNext;
        }
    }
}


VOID UnregisterUnwindInfoInLoaderHeap (UnlockedLoaderHeap *pHeap)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        PRECONDITION(pHeap->m_fPermitStubsWithUnwindInfo);
    }
    CONTRACTL_END;

    pHeap->EnumPageRegions(&UnregisterUnwindInfoInLoaderHeapCallback);

#ifdef _DEBUG
    pHeap->m_fStubUnwindInfoUnregistered = TRUE;
#endif // _DEBUG
}


class StubUnwindInfoSegmentBoundaryReservationList
{
    struct ReservationList
    {
        ReservationList *pNext;

        static ReservationList *FromStub (Stub *pStub)
        {
            return (ReservationList*)(pStub+1);
        }

        Stub *GetStub ()
        {
            return (Stub*)this - 1;
        }
    };

    ReservationList *m_pList;
    
public:

    StubUnwindInfoSegmentBoundaryReservationList ()
    {
        LEAF_CONTRACT;
        
        m_pList = NULL;
    }

    ~StubUnwindInfoSegmentBoundaryReservationList ()
    {
        LEAF_CONTRACT;
        
        ReservationList *pList = m_pList;
        while (pList)
        {
            ReservationList *pNext = pList->pNext;

            pList->GetStub()->DecRef();

            pList = pNext;
        }
    }

    void AddStub (Stub *pStub)
    {
        LEAF_CONTRACT;
        
        ReservationList *pList = ReservationList::FromStub(pStub);

        pList->pNext = m_pList;
        m_pList = pList;
    }
};


#endif // STUBLINKER_GENERATES_UNWIND_INFO


//************************************************************************
// StubLinker
//************************************************************************

//---------------------------------------------------------------
// Construction
//---------------------------------------------------------------
StubLinker::StubLinker()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    m_pCodeElements     = NULL;
    m_pFirstCodeLabel   = NULL;
    m_pFirstLabelRef    = NULL;
    m_pPatchLabel       = NULL;
    m_pReturnLabel      = NULL;
    m_returnStackSize   = 0;
    m_stackSize         = 0;
    m_fDataOnly         = FALSE;
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    m_pUnwindInfoList   = &g_StubLinkerUnwindInfoListInitializer;
    m_nUnwindSlots      = 0;
    m_fHaveFramePointer = FALSE;
    m_nEntryPoints      = 1;
    m_nLastEntryPointSlots = 0;
    m_LastPrologueEntry = &g_StubLinkerUnwindInfoListInitializer;
#ifdef _DEBUG
    m_pUnwindInfoCheckLabel = NULL;
#endif
#endif // STUBLINKER_GENERATES_UNWIND_INFO
}



//---------------------------------------------------------------
// Append code bytes.
//---------------------------------------------------------------
VOID StubLinker::EmitBytes(const BYTE *pBytes, UINT numBytes)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CodeElement *pLastCodeElement = GetLastCodeElement();
    while (numBytes != 0) {

        if (pLastCodeElement != NULL &&
            pLastCodeElement->m_type == CodeElement::kCodeRun) {
            CodeRun *pCodeRun = (CodeRun*)pLastCodeElement;
            UINT numbytessrc  = numBytes;
            UINT numbytesdst  = CODERUNSIZE - pCodeRun->m_numcodebytes;
            if (numbytesdst <= numbytessrc) {
                CopyMemory(&(pCodeRun->m_codebytes[pCodeRun->m_numcodebytes]),
                           pBytes,
                           numbytesdst);
                ClrFlushInstructionCache(&(pCodeRun->m_codebytes[pCodeRun->m_numcodebytes]),numbytesdst);
                pCodeRun->m_numcodebytes = CODERUNSIZE;
                pLastCodeElement = NULL;
                pBytes += numbytesdst;
                numBytes -= numbytesdst;
            } else {
                CopyMemory(&(pCodeRun->m_codebytes[pCodeRun->m_numcodebytes]),
                           pBytes,
                           numbytessrc);
                ClrFlushInstructionCache(&(pCodeRun->m_codebytes[pCodeRun->m_numcodebytes]),numbytessrc);
                pCodeRun->m_numcodebytes += numbytessrc;
                pBytes += numbytessrc;
                numBytes = 0;
            }

        } else {
            pLastCodeElement = AppendNewEmptyCodeRun();
        }
    }
}


//---------------------------------------------------------------
// Append code bytes.
//---------------------------------------------------------------
VOID StubLinker::Emit8 (unsigned __int8  val)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CodeRun *pCodeRun = GetLastCodeRunIfAny();
    if (pCodeRun && (CODERUNSIZE - pCodeRun->m_numcodebytes) >= sizeof(val)) {
        *((unsigned __int8 *)(pCodeRun->m_codebytes + pCodeRun->m_numcodebytes)) = val;
        pCodeRun->m_numcodebytes += sizeof(val);
    } else {
        EmitBytes((BYTE*)&val, sizeof(val));
    }
}

//---------------------------------------------------------------
// Append code bytes.
//---------------------------------------------------------------
VOID StubLinker::Emit16(unsigned __int16 val)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CodeRun *pCodeRun = GetLastCodeRunIfAny();
    if (pCodeRun && (CODERUNSIZE - pCodeRun->m_numcodebytes) >= sizeof(val)) {
        SET_UNALIGNED_16(pCodeRun->m_codebytes + pCodeRun->m_numcodebytes, val);
        pCodeRun->m_numcodebytes += sizeof(val);
    } else {
        EmitBytes((BYTE*)&val, sizeof(val));
    }
}

//---------------------------------------------------------------
// Append code bytes.
//---------------------------------------------------------------
VOID StubLinker::Emit32(unsigned __int32 val)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CodeRun *pCodeRun = GetLastCodeRunIfAny();
    if (pCodeRun && (CODERUNSIZE - pCodeRun->m_numcodebytes) >= sizeof(val)) {
        SET_UNALIGNED_32(pCodeRun->m_codebytes + pCodeRun->m_numcodebytes,  val);
        pCodeRun->m_numcodebytes += sizeof(val);
    } else {
        EmitBytes((BYTE*)&val, sizeof(val));
    }
}

//---------------------------------------------------------------
// Append code bytes.
//---------------------------------------------------------------
VOID StubLinker::Emit64(unsigned __int64 val)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CodeRun *pCodeRun = GetLastCodeRunIfAny();
    if (pCodeRun && (CODERUNSIZE - pCodeRun->m_numcodebytes) >= sizeof(val)) {
        SET_UNALIGNED_64(pCodeRun->m_codebytes + pCodeRun->m_numcodebytes, val);
        pCodeRun->m_numcodebytes += sizeof(val);
    } else {
        EmitBytes((BYTE*)&val, sizeof(val));
    }
}

//---------------------------------------------------------------
// Append pointer value.
//---------------------------------------------------------------
VOID StubLinker::EmitPtr(const VOID *val)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CodeRun *pCodeRun = GetLastCodeRunIfAny();
    if (pCodeRun && (CODERUNSIZE - pCodeRun->m_numcodebytes) >= sizeof(val)) {
        SET_UNALIGNED_PTR(pCodeRun->m_codebytes + pCodeRun->m_numcodebytes, (UINT_PTR)val);
        pCodeRun->m_numcodebytes += sizeof(val);
    } else {
        EmitBytes((BYTE*)&val, sizeof(val));
    }
}


//---------------------------------------------------------------
// Create a new undefined label. Label must be assigned to a code
// location using EmitLabel() prior to final linking.
// Throws COM+ exception on failure.
//---------------------------------------------------------------
CodeLabel* StubLinker::NewCodeLabel()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("StubLinker");

    CodeLabel *pCodeLabel = (CodeLabel*)(m_quickHeap.Alloc(sizeof(CodeLabel)));
    _ASSERTE(pCodeLabel); // QuickHeap throws exceptions rather than returning NULL
    pCodeLabel->m_next       = m_pFirstCodeLabel;
    pCodeLabel->m_fExternal  = FALSE;
    pCodeLabel->m_fAbsolute = FALSE;
    pCodeLabel->i.m_pCodeRun = NULL;
    m_pFirstCodeLabel = pCodeLabel;
    return pCodeLabel;


}

CodeLabel* StubLinker::NewAbsoluteCodeLabel()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    CodeLabel *pCodeLabel = NewCodeLabel();
    pCodeLabel->m_fAbsolute = TRUE;
    return pCodeLabel;
}


//---------------------------------------------------------------
// Sets the label to point to the current "instruction pointer".
// It is invalid to call EmitLabel() twice on
// the same label.
//---------------------------------------------------------------
VOID StubLinker::EmitLabel(CodeLabel* pCodeLabel)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(!(pCodeLabel->m_fExternal));       //can't emit an external label
    _ASSERTE(pCodeLabel->i.m_pCodeRun == NULL);  //must only emit label once
    CodeRun *pLastCodeRun = GetLastCodeRunIfAny();
    if (!pLastCodeRun) {
        pLastCodeRun = AppendNewEmptyCodeRun();
    }
    pCodeLabel->i.m_pCodeRun    = pLastCodeRun;
    pCodeLabel->i.m_localOffset = pLastCodeRun->m_numcodebytes;
}


//---------------------------------------------------------------
// Combines NewCodeLabel() and EmitLabel() for convenience.
// Throws COM+ exception on failure.
//---------------------------------------------------------------
CodeLabel* StubLinker::EmitNewCodeLabel()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CodeLabel* label = NewCodeLabel();
    EmitLabel(label);
    return label;
}


//---------------------------------------------------------------
// Creates & emits the patch offset label for the stub
//---------------------------------------------------------------
VOID StubLinker::EmitPatchLabel()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    //
    // Note that it's OK to have re-emit the patch label,
    // just use the later one.
    //

    m_pPatchLabel = EmitNewCodeLabel();
}

//---------------------------------------------------------------
// Creates & emits the return offset label for the stub
//---------------------------------------------------------------
VOID StubLinker::EmitReturnLabel()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    //
    // Note that it's OK to have re-emit the patch label,
    // just use the later one.
    //

    m_pReturnLabel = EmitNewCodeLabel();
    m_returnStackSize = m_stackSize;
}


//---------------------------------------------------------------
// Returns final location of label as an offset from the start
// of the stub. Can only be called after linkage.
//---------------------------------------------------------------
UINT32 StubLinker::GetLabelOffset(CodeLabel *pLabel)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(!(pLabel->m_fExternal));
    return pLabel->i.m_localOffset + pLabel->i.m_pCodeRun->m_globaloffset;
}


//---------------------------------------------------------------
// Create a new label to an external address.
// Throws COM+ exception on failure.
//---------------------------------------------------------------
CodeLabel* StubLinker::NewExternalCodeLabel(LPVOID pExternalAddress)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;   

        PRECONDITION(CheckPointer(pExternalAddress));
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("StubLinker");

    CodeLabel *pCodeLabel = (CodeLabel*)(m_quickHeap.Alloc(sizeof(CodeLabel)));
    _ASSERTE(pCodeLabel); // QuickHeap throws exceptions rather than returning NULL
    pCodeLabel->m_next       = m_pFirstCodeLabel;
    pCodeLabel->m_fExternal          = TRUE;
    pCodeLabel->m_fAbsolute  = FALSE;
    pCodeLabel->e.m_pExternalAddress = pExternalAddress;
    m_pFirstCodeLabel = pCodeLabel;
    return pCodeLabel;
}




//---------------------------------------------------------------
// Append an instruction containing a reference to a label.
//
//      target          - the label being referenced.
//      instructionFormat         - a platform-specific InstructionFormat object
//                        that gives properties about the reference.
//      variationCode   - uninterpreted data passed to the pInstructionFormat methods.
//---------------------------------------------------------------
VOID StubLinker::EmitLabelRef(CodeLabel* target, const InstructionFormat & instructionFormat, UINT variationCode)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("StubLinker");

    LabelRef *pLabelRef = (LabelRef *)(m_quickHeap.Alloc(sizeof(LabelRef)));
    _ASSERTE(pLabelRef);      // m_quickHeap throws an exception rather than returning NULL
    pLabelRef->m_type               = LabelRef::kLabelRef;
    pLabelRef->m_pInstructionFormat = (InstructionFormat*)&instructionFormat;
    pLabelRef->m_variationCode      = variationCode;
    pLabelRef->m_target             = target;

    pLabelRef->m_nextLabelRef = m_pFirstLabelRef;
    m_pFirstLabelRef = pLabelRef;

    AppendCodeElement(pLabelRef);


}





//---------------------------------------------------------------
// Internal helper routine.
//---------------------------------------------------------------
CodeRun *StubLinker::GetLastCodeRunIfAny()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    CodeElement *pLastCodeElem = GetLastCodeElement();
    if (pLastCodeElem == NULL || pLastCodeElem->m_type != CodeElement::kCodeRun) {
        return NULL;
    } else {
        return (CodeRun*)pLastCodeElem;
    }
}


//---------------------------------------------------------------
// Internal helper routine.
//---------------------------------------------------------------
CodeRun *StubLinker::AppendNewEmptyCodeRun()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("StubLinker");

    CodeRun *pNewCodeRun = (CodeRun*)(m_quickHeap.Alloc(sizeof(CodeRun)));
    _ASSERTE(pNewCodeRun); // QuickHeap throws exceptions rather than returning NULL
    pNewCodeRun->m_type = CodeElement::kCodeRun;
    pNewCodeRun->m_numcodebytes = 0;
    AppendCodeElement(pNewCodeRun);
    return pNewCodeRun;

}

//---------------------------------------------------------------
// Internal helper routine.
//---------------------------------------------------------------
VOID StubLinker::AppendCodeElement(CodeElement *pCodeElement)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    pCodeElement->m_next = m_pCodeElements;
    m_pCodeElements = pCodeElement;
}



//---------------------------------------------------------------
// Is the current LabelRef's size big enough to reach the target?
//---------------------------------------------------------------
static BOOL LabelCanReach(LabelRef *pLabelRef)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    InstructionFormat *pIF  = pLabelRef->m_pInstructionFormat;

    if (pLabelRef->m_target->m_fExternal)
    {
        return pLabelRef->m_pInstructionFormat->CanReach(
                pLabelRef->m_refsize, pLabelRef->m_variationCode, TRUE, (INT_PTR)pLabelRef->m_target->e.m_pExternalAddress);
    }
    else
    {
        UINT targetglobaloffset = pLabelRef->m_target->i.m_pCodeRun->m_globaloffset +
                                  pLabelRef->m_target->i.m_localOffset;
        UINT srcglobaloffset = pLabelRef->m_globaloffset +
                               pIF->GetHotSpotOffset(pLabelRef->m_refsize,
                                                     pLabelRef->m_variationCode);
        INT offset = (INT)(targetglobaloffset - srcglobaloffset);

        return pLabelRef->m_pInstructionFormat->CanReach(
            pLabelRef->m_refsize, pLabelRef->m_variationCode, FALSE, offset);
    }
}

//---------------------------------------------------------------
// Generate the actual stub. The returned stub has a refcount of 1.
// No other methods (other than the destructor) should be called
// after calling Link().
//
// Throws COM+ exception on failure.
//---------------------------------------------------------------
Stub *StubLinker::LinkInterceptor(LoaderHeap *pHeap, Stub* interceptee, void *pRealAddr)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("StubLinker");

    int globalsize = 0;
    int size = CalculateSize(&globalsize);

    _ASSERTE(pHeap);
    _ASSERTE(pHeap->IsExecutable());

    StubHolder<Stub> pStub;

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    StubUnwindInfoSegmentBoundaryReservationList ReservedStubs;

    for (;;)
#endif
    {
        pStub = InterceptStub::NewInterceptedStub(pHeap, size, interceptee,
                                                    pRealAddr,
                                                    m_pReturnLabel != NULL
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
                                                    , m_nUnwindSlots,
                                                    m_nEntryPoints
#endif
                                                    );
        if (!pStub) {
            COMPlusThrowOM();
        }

        bool fSuccess; fSuccess = EmitStub(pStub, globalsize);

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
        if (fSuccess)
        {
            break;
        }
        else
        {
            ReservedStubs.AddStub(pStub);
            pStub.SuppressRelease();
        }
#else
        CONSISTENCY_CHECK_MSG(fSuccess, ("EmitStub should always return true"));
#endif
    }

    return pStub.Extract();
}

//---------------------------------------------------------------
// Generate the actual stub. The returned stub has a refcount of 1.
// No other methods (other than the destructor) should be called
// after calling Link().
//
// Throws COM+ exception on failure.
//---------------------------------------------------------------
Stub *StubLinker::Link(LoaderHeap *pHeap, UINT *pcbSize /* = NULL*/, BOOL fMC)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    MEMORY_REPORT_CONTEXT_SCOPE("StubLinker");

    int globalsize = 0;
    int size = CalculateSize(&globalsize);
    if (pcbSize) {
        *pcbSize = size;
    }

    _ASSERTE(!pHeap || pHeap->IsExecutable());

    StubHolder<Stub> pStub;

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    StubUnwindInfoSegmentBoundaryReservationList ReservedStubs;

    for (;;)
#endif
    {
        pStub = Stub::NewStub(
                pHeap,
                size,
                FALSE,
                m_pReturnLabel != NULL,
                fMC
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
                , m_nUnwindSlots,
                m_nEntryPoints
#endif
                );
        ASSERT(pStub != NULL);

        bool fSuccess; fSuccess = EmitStub(pStub, globalsize);

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
        if (fSuccess)
        {
            break;
        }
        else
        {
            ReservedStubs.AddStub(pStub);
            pStub.SuppressRelease();
        }
#else
        CONSISTENCY_CHECK_MSG(fSuccess, ("EmitStub should always return true"));
#endif
    }

    return pStub.Extract();
}

int StubLinker::CalculateSize(int* pGlobalSize)
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    _ASSERTE(pGlobalSize);

#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
    if (m_pUnwindInfoCheckLabel)
    {
        EmitLabel(m_pUnwindInfoCheckLabel);
        EmitUnwindInfoCheckSubfunction();
        m_pUnwindInfoCheckLabel = NULL;
    }
#endif

#ifdef _DEBUG
    // Don't want any undefined labels
    for (CodeLabel *pCodeLabel = m_pFirstCodeLabel;
         pCodeLabel != NULL;
         pCodeLabel = pCodeLabel->m_next) {
        if ((!(pCodeLabel->m_fExternal)) && pCodeLabel->i.m_pCodeRun == NULL) {
            _ASSERTE(!"Forgot to define a label before asking StubLinker to link.");
        }
    }
#endif //_DEBUG

    //-------------------------------------------------------------------
    // Tentatively set all of the labelref sizes to their smallest possible
    // value.
    //-------------------------------------------------------------------
    for (LabelRef *pLabelRef = m_pFirstLabelRef;
         pLabelRef != NULL;
         pLabelRef = pLabelRef->m_nextLabelRef) {

        for (UINT bitmask = 1; bitmask <= InstructionFormat::kMax; bitmask = bitmask << 1) {
            if (pLabelRef->m_pInstructionFormat->m_allowedSizes & bitmask) {
                pLabelRef->m_refsize = bitmask;
                break;
            }
        }

    }

    UINT globalsize;
    UINT datasize;
    BOOL fSomethingChanged;
    do {
        fSomethingChanged = FALSE;


        // Layout each code element.
        globalsize = 0;
        datasize = 0;
        CodeElement *pCodeElem;
        for (pCodeElem = m_pCodeElements; pCodeElem; pCodeElem = pCodeElem->m_next) {

            switch (pCodeElem->m_type) {
                case CodeElement::kCodeRun:
                    globalsize += ((CodeRun*)pCodeElem)->m_numcodebytes;
                    break;

                case CodeElement::kLabelRef: {
                    LabelRef *pLabelRef = (LabelRef*)pCodeElem;
                    globalsize += pLabelRef->m_pInstructionFormat->GetSizeOfInstruction( pLabelRef->m_refsize,
                                                                                         pLabelRef->m_variationCode );
                    datasize += pLabelRef->m_pInstructionFormat->GetSizeOfData( pLabelRef->m_refsize,
                                                                                         pLabelRef->m_variationCode );
                    }
                    break;

                default:
                    _ASSERTE(0);
            }

            pCodeElem->m_globaloffset = 0 - globalsize;

            // also record the data offset. Note the link-list we walk is in
            // *reverse* order so we visit the last instruction first
            // so what we record now is in fact the offset from the *end* of
            // the data block. We fix it up later.
            pCodeElem->m_dataoffset = 0 - datasize;
        }

        // Now fix up the global offsets.
        for (pCodeElem = m_pCodeElements; pCodeElem; pCodeElem = pCodeElem->m_next) {
            pCodeElem->m_globaloffset += globalsize;
            pCodeElem->m_dataoffset += datasize;
        }


        // Now, iterate thru the LabelRef's and check if any of them
        // have to be resized.
        for (LabelRef *pLabelRef = m_pFirstLabelRef;
             pLabelRef != NULL;
             pLabelRef = pLabelRef->m_nextLabelRef) {


            if (!LabelCanReach(pLabelRef)) {
                fSomethingChanged = TRUE;

                // Find the next largest size.
                // (we could be smarter about this and eliminate intermediate
                // sizes based on the tentative offset.)
                for (UINT bitmask = pLabelRef->m_refsize << 1; bitmask <= InstructionFormat::kMax; bitmask = bitmask << 1) {
                    if (pLabelRef->m_pInstructionFormat->m_allowedSizes & bitmask) {
                        pLabelRef->m_refsize = bitmask;
                        break;
                    }
                }
#ifdef _DEBUG
                if (pLabelRef->m_refsize > InstructionFormat::kMax) {
                    _ASSERTE(!"Stub instruction cannot reach target: must choose a different instruction!");
                }
#endif
            }
        }


    } while (fSomethingChanged); // Keep iterating until all LabelRef's can reach


    // We now have the correct layout write out the stub.

    // Compute stub code+data size after aligning data correctly
    if(globalsize % DATA_ALIGNMENT)
        globalsize += (DATA_ALIGNMENT - (globalsize % DATA_ALIGNMENT));

    *pGlobalSize = globalsize;
    return globalsize + datasize;
}

bool StubLinker::EmitStub(Stub* pStub, int globalsize)
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
        SO_TOLERANT;
    }
    CONTRACTL_END;

    BYTE *pCode = (BYTE*)(pStub->GetEntryPoint());
    BYTE *pData = pCode+globalsize; // start of data area
    {
        int lastCodeOffset = 0;

        // Write out each code element.
        for (CodeElement* pCodeElem = m_pCodeElements; pCodeElem; pCodeElem = pCodeElem->m_next) {
            int currOffset = 0;

            switch (pCodeElem->m_type) {
                case CodeElement::kCodeRun:
                    CopyMemory(pCode + pCodeElem->m_globaloffset,
                               ((CodeRun*)pCodeElem)->m_codebytes,
                               ((CodeRun*)pCodeElem)->m_numcodebytes);
                    currOffset = pCodeElem->m_globaloffset + ((CodeRun *)pCodeElem)->m_numcodebytes;
                    break;

                case CodeElement::kLabelRef: {
                    LabelRef *pLabelRef = (LabelRef*)pCodeElem;
                    InstructionFormat *pIF  = pLabelRef->m_pInstructionFormat;
                    __int64 fixupval;

                    LPBYTE srcglobaladdr = pCode +
                                           pLabelRef->m_globaloffset +
                                           pIF->GetHotSpotOffset(pLabelRef->m_refsize,
                                                                 pLabelRef->m_variationCode);
                    LPBYTE targetglobaladdr;
                    if (!(pLabelRef->m_target->m_fExternal)) {
                        targetglobaladdr = pCode +
                                           pLabelRef->m_target->i.m_pCodeRun->m_globaloffset +
                                           pLabelRef->m_target->i.m_localOffset;
                    } else {
                        targetglobaladdr = (LPBYTE)(pLabelRef->m_target->e.m_pExternalAddress);
                    }
                    if ((pLabelRef->m_target->m_fAbsolute)) {
                        fixupval = (__int64)(size_t)targetglobaladdr;
                    } else
                        fixupval = (__int64)(targetglobaladdr - srcglobaladdr);

                    pLabelRef->m_pInstructionFormat->EmitInstruction(
                        pLabelRef->m_refsize,
                        fixupval,
                        pCode + pCodeElem->m_globaloffset,
                        pLabelRef->m_variationCode,
                        pData + pCodeElem->m_dataoffset);

                    currOffset =
                        pCodeElem->m_globaloffset +
                        pLabelRef->m_pInstructionFormat->GetSizeOfInstruction( pLabelRef->m_refsize,
                                                                               pLabelRef->m_variationCode );
                    }
                    break;

                default:
                    _ASSERTE(0);
            }
            lastCodeOffset = (currOffset > lastCodeOffset) ? currOffset : lastCodeOffset;
        }

        // Fill in zeros at the end, if necessary
        if (lastCodeOffset < globalsize)
            ZeroMemory(pCode + lastCodeOffset, globalsize - lastCodeOffset);
    }

    // Fill in patch offset, if we have one
    // Note that these offsets are relative to the start of the stub,
    // not the code, so you'll have to add sizeof(Stub) to get to the
    // right spot.
    if (m_pPatchLabel != NULL)
    {
        pStub->SetPatchOffset(GetLabelOffset(m_pPatchLabel));

        LOG((LF_CORDB, LL_INFO100, "SL::ES: patch offset:0x%x\n",
            pStub->GetPatchOffset()));
    }


    if (m_pReturnLabel != NULL)
    {
        pStub->SetCallSiteReturnOffset(GetLabelOffset(m_pReturnLabel));
        pStub->SetCallSiteStackSize(m_returnStackSize);
    }

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    if (m_nUnwindSlots)
    {
        if (!EmitUnwindInfo(pStub, globalsize))
            return false;
    }
#endif // STUBLINKER_GENERATES_UNWIND_INFO

    if (!m_fDataOnly) 
    {
        FlushInstructionCache(GetCurrentProcess(), pCode, globalsize);
    }

    _ASSERTE(m_fDataOnly || DbgIsExecutableVM(pCode, globalsize));

    return true;
}




#endif // #ifndef DACCESS_COMPILE

#ifndef DACCESS_COMPILE

//-------------------------------------------------------------------
// ForceDelete
//
// Forces a stub to free itself. This routine forces the refcount
// to 1, then does a DecRef. It is not threadsafe, and thus can
// only be used in shutdown scenarios.
//-------------------------------------------------------------------
VOID Stub::ForceDelete()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    m_refcount = 1;
    DecRef();
}

//-------------------------------------------------------------------
// Inc the refcount.
//-------------------------------------------------------------------
VOID Stub::IncRef()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(m_signature == kUsedStub);
    FastInterlockIncrement((LONG*)&m_refcount);
}

//-------------------------------------------------------------------
// Dec the refcount.
//-------------------------------------------------------------------
BOOL Stub::DecRef()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    _ASSERTE(m_signature == kUsedStub);
    int count = FastInterlockDecrement((LONG*)&m_refcount);
    if (count <= 0) {
#ifdef _DEBUG
        if ((m_patchOffset & LOADER_HEAP_BIT) == 0)
        {
            m_signature = kFreedStub;
            FillMemory(this+1, m_numCodeBytes, 0xcc);
        }
#endif

        if(m_patchOffset & INTERCEPT_BIT) {
            ((InterceptStub*)this)->ReleaseInterceptedStub();
            ((InterceptStub*)this)->DeleteStub();
        }
        else
            DeleteStub();

        return TRUE;
    }
    return FALSE;
}

VOID Stub::DeleteStub()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    COUNTER_ONLY(GetPrivatePerfCounters().m_Interop.cStubs--);

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    if (HasUnwindInfo())
    {
        StubUnwindInfoHeader *pHeader = GetUnwindInfoHeader();

        //
        // Check if the stub has been linked into a StubUnwindInfoHeapSegment.
        //

        if (pHeader->IsRegistered())
        {
            CrstHolder crst(&g_StubUnwindInfoHeapSegmentsCrst);

            //
            // Find the segment containing the stub.
            //
            StubUnwindInfoHeapSegment **ppPrevSegment = &g_StubHeapSegments;
            StubUnwindInfoHeapSegment *pSegment = *ppPrevSegment;

            if (pSegment)
            {
                PBYTE pbCode = (PBYTE)GetEntryPointInternal();

                for (StubUnwindInfoHeapSegment *pNextSegment = pSegment->pNext;
                     pNextSegment;
                     ppPrevSegment = &pSegment->pNext, pSegment = pNextSegment, pNextSegment = pSegment->pNext)
                {
                    // The segments are sorted by pbBaseAddress.
                    if (pbCode < pNextSegment->pbBaseAddress)
                        break;
                }
            }

            // The stub was marked as registered, so a segment should exist.
            _ASSERTE(pSegment);

            if (pSegment)
            {
                //
                // Find this stub's location in the segment's list.
                //
                StubUnwindInfoHeader *pCurHeader;
                StubUnwindInfoHeader **ppPrevHeaderList;
                for (ppPrevHeaderList = &pSegment->pUnwindHeaderList;
                     pCurHeader = *ppPrevHeaderList;
                     ppPrevHeaderList = &pCurHeader->pNext)
                {
                    if (pHeader == pCurHeader)
                        break;
                }

                // The stub was marked as registered, so we should find it in the segment's list.
                _ASSERTE(pCurHeader);

                if (pCurHeader)
                {
                    //
                    // Remove the stub from the segment's list.
                    //
                    *ppPrevHeaderList = pHeader->pNext;

                    //
                    // If the segment's list is now empty, delete the segment.
                    //
                    if (!pSegment->pUnwindHeaderList)
                    {
                        DeleteEEFunctionTable(pSegment);

                        *ppPrevSegment = pSegment->pNext;

                        delete pSegment;
                    }
                }
            }
        }
    }
#endif

    // a size of 0 is a signal to Nirvana to flush the entire cache 
    //FlushInstructionCache(GetCurrentProcess(),0,0);

    if ((m_patchOffset & LOADER_HEAP_BIT) == 0) {
        DeleteExecutable((BYTE*)GetAllocationBase());
    }
}




//-------------------------------------------------------------------
// Stub allocation done here.
//-------------------------------------------------------------------
/*static*/ Stub* Stub::NewStub(
        LoaderHeap *pHeap,
        UINT numCodeBytes,
        BOOL intercept,
        BOOL callSiteInfo,
        BOOL fMC
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
        , UINT nUnwindInfoSlots,
        UINT nEntryPoints
#endif
        )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    _ASSERTE(!nUnwindInfoSlots || !pHeap || pHeap->m_fPermitStubsWithUnwindInfo);
#endif // STUBLINKER_GENERATES_UNWIND_INFO

    COUNTER_ONLY(GetPrivatePerfCounters().m_Interop.cStubs++);

    MEMORY_REPORT_CONTEXT_SCOPE("StubLinker");

    SIZE_T cbIntercept = sizeof(Stub *) + sizeof(void*);
    SIZE_T cbPrefixBeforeUnwindInfo = 0;

    if (intercept)
    {
        if (!ClrSafeInt<SIZE_T>::addition(cbPrefixBeforeUnwindInfo, cbIntercept, cbPrefixBeforeUnwindInfo))
            return NULL;
    }
    
    if (callSiteInfo)
    {
        if (!ClrSafeInt<SIZE_T>::addition(cbPrefixBeforeUnwindInfo, sizeof(CallSiteInfo), cbPrefixBeforeUnwindInfo))
            return NULL;
    }

    if (ALIGN_UP(cbPrefixBeforeUnwindInfo, CODE_SIZE_ALIGN) < cbPrefixBeforeUnwindInfo)
        return NULL;
    
    cbPrefixBeforeUnwindInfo = ALIGN_UP(cbPrefixBeforeUnwindInfo, CODE_SIZE_ALIGN);


    SIZE_T size = 0;
    if (!ClrSafeInt<SIZE_T>::addition(cbPrefixBeforeUnwindInfo, sizeof(Stub), size) ||
        !ClrSafeInt<SIZE_T>::addition(size, numCodeBytes, size))
        return NULL;

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    if (nUnwindInfoSlots)
    {
        SIZE_T addSize = StubUnwindInfoHeader::ComputeSize(nUnwindInfoSlots, nEntryPoints, cbPrefixBeforeUnwindInfo);

        if (!ClrSafeInt<SIZE_T>::addition(size, addSize, size))
            return NULL;
    }
#endif


    BYTE *pBlock;
    if (pHeap == NULL)
        pBlock = new (executable) BYTE[size];
    else
        pBlock = (BYTE*)(void*) pHeap->AllocAlignedMem(size, CODE_SIZE_ALIGN, NULL);


#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    if (nUnwindInfoSlots)
    {
        ((StubUnwindInfoHeader*)pBlock)->Init();

        pBlock += StubUnwindInfoHeader::ComputeSize(nUnwindInfoSlots, nEntryPoints, cbPrefixBeforeUnwindInfo);

        StubUnwindInfoHeaderSuffix *pSuffix = (StubUnwindInfoHeaderSuffix*)pBlock - 1;

        pSuffix->nEntryPoints = nEntryPoints;
        pSuffix->nTotalSlots = nUnwindInfoSlots;
    }
#endif

    pBlock += cbPrefixBeforeUnwindInfo;

    Stub* pStub = (Stub*) pBlock;

    pStub->SetupStub(
            numCodeBytes,
            intercept,
            pHeap != NULL,
            callSiteInfo,
            fMC
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
            , nUnwindInfoSlots
#endif
            );

    return pStub;
}


void Stub::SetupStub(int numCodeBytes, BOOL fIntercepted, BOOL fLoaderHeap,
                     BOOL fCallSiteInfo, BOOL fMulticast
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
                     , UINT nUnwindInfoSlots
#endif
                     )
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef _DEBUG
    m_signature = kUsedStub;
#endif
    m_numCodeBytes = numCodeBytes;

    m_refcount = 1;
    m_patchOffset = 0;
    if(fIntercepted)
        m_patchOffset |= INTERCEPT_BIT;
    if(fLoaderHeap)
        m_patchOffset |= LOADER_HEAP_BIT;
    if(fMulticast)
        m_patchOffset |= MULTICAST_DELEGATE_BIT;
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    if(nUnwindInfoSlots)
        m_patchOffset |= UNWIND_INFO_BIT;
#endif
    if(fCallSiteInfo)
    {
        m_patchOffset |= CALL_SITE_BIT;

        CallSiteInfo *info = GetCallSiteInfo();
        info->returnOffset = 0;
        info->stackSize = 0;
    }


}

//-------------------------------------------------------------------
// One-time init
//-------------------------------------------------------------------
/*static*/ void Stub::Init()
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
    g_StubUnwindInfoHeapSegmentsCrst.Init("g_StubUnwindInfoHeapSegmentsCrst", CrstStubUnwindInfoHeapSegments, CRST_UNSAFE_ANYMODE);

    ZeroMemory(&g_StubLinkerUnwindInfoListInitializerCodeRun, sizeof(g_StubLinkerUnwindInfoListInitializerCodeRun));
    ZeroMemory(&g_StubLinkerUnwindInfoListInitializer, sizeof(g_StubLinkerUnwindInfoListInitializer));

    g_StubLinkerUnwindInfoListInitializer.rgUnwindCode[0].UnwindOp = CLR_UWOP_NEW_PROLOGUE;
    g_StubLinkerUnwindInfoListInitializer.pCodeRun = &g_StubLinkerUnwindInfoListInitializerCodeRun;
#endif
}



//-------------------------------------------------------------------
// Stub allocation done here.
//-------------------------------------------------------------------
/*static*/ Stub* InterceptStub::NewInterceptedStub(LoaderHeap *pHeap,
                                                   UINT numCodeBytes,
                                                   Stub* interceptee,
                                                   void* pRealAddr,
                                                   BOOL callSiteInfo
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
                                                   , UINT nUnwindInfoSlots,
                                                   UINT nEntryPoints
#endif
                                                   )
{
    CONTRACTL
    {
        THROWS;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    InterceptStub *pStub = (InterceptStub *) NewStub(
            pHeap,
            numCodeBytes,
            TRUE,
            callSiteInfo,
            FALSE
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
            , nUnwindInfoSlots,
            nEntryPoints
#endif
            );

    if (pStub == NULL)
        return NULL;

    *pStub->GetInterceptedStub() = interceptee;
    *pStub->GetRealAddr() = (TADDR)pRealAddr;

    LOG((LF_CORDB, LL_INFO10000, "For Stub 0x%x, set intercepted stub to 0x%x\n",
        pStub, interceptee));

    return pStub;
}

//-------------------------------------------------------------------
// Delete the stub
//-------------------------------------------------------------------
void InterceptStub::DeleteStub()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    /* Allocated on the heap
    if(m_patchOffset & CALL_SITE_BIT)
        delete [] ((BYTE*)this - GetNegativeOffset() - sizeof(CallSiteInfo));
    else
        delete [] ((BYTE*)this - GetNegativeOffset());
    */
}

//-------------------------------------------------------------------
// Release the stub that is owned by this stub
//-------------------------------------------------------------------
void InterceptStub::ReleaseInterceptedStub()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    Stub** intercepted = GetInterceptedStub();
    // If we own the stub then decrement it. It can be null if the
    // linked stub is actually a jitted stub.
    if(*intercepted)
        (*intercepted)->DecRef();
}

//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------
ArgBasedStubCache::ArgBasedStubCache(UINT fixedSlots)
        : m_numFixedSlots(fixedSlots),
          m_crst("ArgBasedSlotCache", CrstArgBasedStubCache)
{
    WRAPPER_CONTRACT;

    m_aStub = new Stub * [m_numFixedSlots];
    _ASSERTE(m_aStub != NULL);

    for (unsigned __int32 i = 0; i < m_numFixedSlots; i++) {
        m_aStub[i] = NULL;
    }
    m_pSlotEntries = NULL;
}


//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------
ArgBasedStubCache::~ArgBasedStubCache()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
    }
    CONTRACTL_END;

    for (unsigned __int32 i = 0; i < m_numFixedSlots; i++) {
        Stub *pStub = m_aStub[i];
        if (pStub) {
            pStub->DecRef();
        }
    }
    // a size of 0 is a signal to Nirvana to flush the entire cache 
    // not sure if this is needed, but should have no CLR perf impact since size is 0.
    FlushInstructionCache(GetCurrentProcess(),0,0);

    SlotEntry **ppSlotEntry = &m_pSlotEntries;
    SlotEntry *pCur;
    while (NULL != (pCur = *ppSlotEntry)) {
        Stub *pStub = pCur->m_pStub;
        pStub->DecRef();
        *ppSlotEntry = pCur->m_pNext;
        delete pCur;
    }
    delete [] m_aStub;
}



//-------------------------------------------------------------------
// Queries/retrieves a previously cached stub.
//
// If there is no stub corresponding to the given index,
//   this function returns NULL.
//
// Otherwise, this function returns the stub after
//   incrementing its refcount.
//-------------------------------------------------------------------
Stub *ArgBasedStubCache::GetStub(UINT_PTR key)
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    Stub *pStub;

    CrstPreempHolder ch(&m_crst);

    if (key < m_numFixedSlots) {
        pStub = m_aStub[key];
    } else {
        pStub = NULL;
        for (SlotEntry *pSlotEntry = m_pSlotEntries;
             pSlotEntry != NULL;
             pSlotEntry = pSlotEntry->m_pNext) {

            if (pSlotEntry->m_key == key) {
                pStub = pSlotEntry->m_pStub;
                break;
            }
        }
    }
    if (pStub) {
        pStub->IncRef();
    }
    return pStub;
}


//-------------------------------------------------------------------
// Tries to associate a stub with a given index. This association
// may fail because some other thread may have beaten you to it
// just before you make the call.
//
// If the association succeeds, "pStub" is installed, and it is
// returned back to the caller. The stub's refcount is incremented
// twice (one to reflect the cache's ownership, and one to reflect
// the caller's ownership.)
//
// If the association fails because another stub is already installed,
// then the incumbent stub is returned to the caller and its refcount
// is incremented once (to reflect the caller's ownership.)
//
// If the association fails due to lack of memory, NULL is returned
// and no one's refcount changes.
//
// This routine is intended to be called like this:
//
//    Stub *pCandidate = MakeStub();  // after this, pCandidate's rc is 1
//    Stub *pWinner = cache->SetStub(idx, pCandidate);
//    pCandidate->DecRef();
//    pCandidate = 0xcccccccc;     // must not use pCandidate again.
//    if (!pWinner) {
//          OutOfMemoryError;
//    }
//    // If the association succeeded, pWinner's refcount is 2 and so
//    // is pCandidate's (because it *is* pWinner);.
//    // If the association failed, pWinner's refcount is still 2
//    // and pCandidate got destroyed by the last DecRef().
//    // Either way, pWinner is now the official index holder. It
//    // has a refcount of 2 (one for the cache's ownership, and
//    // one belonging to this code.)
//-------------------------------------------------------------------
Stub* ArgBasedStubCache::AttemptToSetStub(UINT_PTR key, Stub *pStub)
{
    CONTRACTL
    {
        THROWS;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    CrstPreempHolder ch(&m_crst);

    if (key < m_numFixedSlots) {
        if (m_aStub[key]) {
            pStub = m_aStub[key];
        } else {
            m_aStub[key] = pStub;
            pStub->IncRef();   // IncRef on cache's behalf
        }
    } else {
        SlotEntry *pSlotEntry;
        for (pSlotEntry = m_pSlotEntries;
             pSlotEntry != NULL;
             pSlotEntry = pSlotEntry->m_pNext) {

            if (pSlotEntry->m_key == key) {
                pStub = pSlotEntry->m_pStub;
                break;
            }
        }
        if (!pSlotEntry) {
            pSlotEntry = new SlotEntry;
            pSlotEntry->m_pStub = pStub;
            pStub->IncRef();   // IncRef on cache's behalf
            pSlotEntry->m_key = key;
            pSlotEntry->m_pNext = m_pSlotEntries;
            m_pSlotEntries = pSlotEntry;
        }
    }
    if (pStub) {
        pStub->IncRef();  // IncRef because we're returning it to caller
    }
    return pStub;
}



//-------------------------------------------------------------------
// This goes through and eliminates cache entries for stubs
// that look unused based on their refcount. Eliminating the
// cache entry does not necessarily destroy the stub (the
// cache only undoes its initial IncRef.)
//-------------------------------------------------------------------
VOID ArgBasedStubCache::FreeUnusedStubs()
{
    CONTRACTL
    {
        NOTHROW;
        GC_TRIGGERS;
        MODE_ANY;
    }
    CONTRACTL_END;

    CrstPreempHolder ch(&m_crst);

    for (unsigned __int32 i = 0; i < m_numFixedSlots; i++) {
        Stub *pStub = m_aStub[i];
        if (pStub && pStub->HeuristicLooksOrphaned()) {
            pStub->DecRef();
            m_aStub[i] = NULL;
        }
    }
    
    // a size of 0 is a signal to Nirvana to flush the entire cache 
    FlushInstructionCache(GetCurrentProcess(),0,0);

    SlotEntry **ppSlotEntry = &m_pSlotEntries;
    SlotEntry *pCur;
    while (NULL != (pCur = *ppSlotEntry)) {
        Stub *pStub = pCur->m_pStub;
        if (pStub && pStub->HeuristicLooksOrphaned()) {
            pStub->DecRef();
            *ppSlotEntry = pCur->m_pNext;
            delete pCur;
        } else {
            ppSlotEntry = &(pCur->m_pNext);
        }
    }
}


#ifdef _DEBUG
// Diagnostic dump
VOID ArgBasedStubCache::Dump()
{
    CONTRACTL
    {
        NOTHROW;
        GC_NOTRIGGER;
        MODE_ANY;
    }
    CONTRACTL_END;

    printf("--------------------------------------------------------------\n");
    printf("ArgBasedStubCache dump (%lu fixed entries):\n", m_numFixedSlots);
    for (UINT32 i = 0; i < m_numFixedSlots; i++) {

        printf("  Fixed slot %lu: ", (ULONG)i);
        Stub *pStub = m_aStub[i];
        if (!pStub) {
            printf("empty\n");
        } else {
            printf("%lxh   - refcount is %lu\n",
                   (size_t)(pStub->GetEntryPoint()),
                   (ULONG)( *( ( ((ULONG*)(pStub->GetEntryPoint())) - 1))));
        }
    }

    for (SlotEntry *pSlotEntry = m_pSlotEntries;
         pSlotEntry != NULL;
         pSlotEntry = pSlotEntry->m_pNext) {

        printf("  Dyna. slot %lu: ", (ULONG)(pSlotEntry->m_key));
        Stub *pStub = pSlotEntry->m_pStub;
        printf("%lxh   - refcount is %lu\n",
               (size_t)(pStub->GetEntryPoint()),
               (ULONG)( *( ( ((ULONG*)(pStub->GetEntryPoint())) - 1))));

    }


    printf("--------------------------------------------------------------\n");
}
#endif



#endif // #ifndef DACCESS_COMPILE
