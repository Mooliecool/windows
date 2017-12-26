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

// STUBLINK.H -
//
// A StubLinker object provides a way to link several location-independent
// code sources into one executable stub, resolving references,
// and choosing the shortest possible instruction size. The StubLinker
// abstracts out the notion of a "reference" so it is completely CPU
// independent. This StubLinker is intended not only to create method
// stubs but to create the PCode-marshaling stubs for Native/Direct.
//
// A StubLinker's typical life-cycle is:
//
//   1. Create a new StubLinker (it accumulates state for the stub being
//      generated.)
//   2. Emit code bytes and references (requiring fixups) into the StubLinker.
//   3. Call the Link() method to produce the final stub.
//   4. Destroy the StubLinker.
//
// StubLinkers are not multithread-aware: they're intended to be
// used entirely on a single thread. Also, StubLinker's report errors
// using COMPlusThrow. StubLinker's do have a destructor: to prevent
// C++ object unwinding from clashing with COMPlusThrow,
// you must use COMPLUSCATCH to ensure the StubLinker's cleanup in the
// event of an exception: the following code would do it:
//
//  StubLinker stublink;
//  Inner();
//
//
//  // Have to separate into inner function because VC++ forbids
//  // mixing __try & local objects in the same function.
//  void Inner() {
//      COMPLUSTRY {
//          ... do stuff ...
//          pLinker->Link();
//      } COMPLUSCATCH {
//      }
//  }
//


// This file should only be included via the platform-specific cgencpu.h.
#include "cgensys.h"

#ifndef __stublink_h__
#define __stublink_h__

#include "crst.h"
#include "util.hpp"
#include "eecontract.h"

//-------------------------------------------------------------------------
// Forward refs
//-------------------------------------------------------------------------
class  InstructionFormat;
class  Stub;
class  InterceptStub;
class  CheckDuplicatedStructLayouts;
class  CodeBasedStubCache;
struct CodeLabel;

struct CodeLabel;
struct CodeRun;
struct LabelRef;
struct CodeElement;
struct IntermediateUnwindInfo;
struct IntermediateUnwindInfoPrologueList;



#ifdef STUBLINKER_GENERATES_UNWIND_INFO

typedef DPTR(struct StubUnwindInfoHeaderSuffix) PTR_StubUnwindInfoHeaderSuffix;
struct StubUnwindInfoHeaderSuffix
{
    USHORT nEntryPoints:2;  // Currently this never exceeds 2.
    USHORT nTotalSlots:14;
};

// Variable-sized struct that preceeds a Stub when the stub requires unwind
// information.  Followed by a StubUnwindInfoHeaderSuffix.
typedef DPTR(struct StubUnwindInfoHeader) PTR_StubUnwindInfoHeader;
struct StubUnwindInfoHeader
{
    PTR_StubUnwindInfoHeader pNext;
    struct {
        RUNTIME_FUNCTION FunctionEntry;
        UNWIND_INFO UnwindInfo;  // variable length
    } rgEntryPoints[1];  // variable length

    // Computes the size needed for this variable-sized struct.  Note that if
    // there is more than one entry point, the total number of slots should
    // already include padding slots to ensure that the RUNTIME_FUNCTION
    // structs are ULONG-aligned.
    static SIZE_T ComputeSize (UCHAR nTotalSlots, UCHAR nEntryPoints, SIZE_T cbPrefix);

    StubUnwindInfoHeaderSuffix *GetSuffix (PBYTE pbSegmentBaseAddress);

    void Init ();

    bool IsRegistered ();
};

// List of stub address ranges, in increasing address order.
struct StubUnwindInfoHeapSegment
{
    PBYTE pbBaseAddress;
    SIZE_T cbSegment;
    StubUnwindInfoHeader *pUnwindHeaderList;
    StubUnwindInfoHeapSegment *pNext;
};

VOID UnregisterUnwindInfoInLoaderHeap (UnlockedLoaderHeap *pHeap);

#endif // STUBLINKER_GENERATES_UNWIND_INFO


enum StubStyle
{
    kNoTripStubStyle = 0,       // stub doesn't rendezvous the thread on return
    kInterceptorStubStyle = 1,  // stub does not does  return but

 // Add more stub styles here...
};


//-------------------------------------------------------------------------
// A non-multithreaded object that fixes up and emits one executable stub.
//-------------------------------------------------------------------------
class StubLinker
{
    public:
        //---------------------------------------------------------------
        // Construction
        //---------------------------------------------------------------
        StubLinker();


        //---------------------------------------------------------------
        // Create a new undefined label. Label must be assigned to a code
        // location using EmitLabel() prior to final linking.
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        CodeLabel* NewCodeLabel();

        //---------------------------------------------------------------
        // Create a new undefined label for which we want the absolute
        // address, not offset. Label must be assigned to a code
        // location using EmitLabel() prior to final linking.
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        CodeLabel* NewAbsoluteCodeLabel();

        //---------------------------------------------------------------
        // Combines NewCodeLabel() and EmitLabel() for convenience.
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        CodeLabel* EmitNewCodeLabel();


        //---------------------------------------------------------------
        // Returns final location of label as an offset from the start
        // of the stub. Can only be called after linkage.
        //---------------------------------------------------------------
        UINT32 GetLabelOffset(CodeLabel *pLabel);

        //---------------------------------------------------------------
        // Append code bytes.
        //---------------------------------------------------------------
        VOID EmitBytes(const BYTE *pBytes, UINT numBytes);
        VOID Emit8 (unsigned __int8  u8);
        VOID Emit16(unsigned __int16 u16);
        VOID Emit32(unsigned __int32 u32);
        VOID Emit64(unsigned __int64 u64);
        VOID EmitPtr(const VOID *pval);

        //---------------------------------------------------------------
        // Emit a UTF8 string
        //---------------------------------------------------------------
        VOID EmitUtf8(LPCUTF8 pUTF8)
        {
            WRAPPER_CONTRACT;

            LPCUTF8 p = pUTF8;
            while (*(p++)) {
                //nothing
            }
            EmitBytes((const BYTE *)pUTF8, (unsigned int)(p-pUTF8-1));
        }

        //---------------------------------------------------------------
        // Append an instruction containing a reference to a label.
        //
        //      target             - the label being referenced.
        //      instructionFormat  - a platform-specific InstructionFormat object
        //                           that gives properties about the reference.
        //      variationCode      - uninterpreted data passed to the pInstructionFormat methods.
        //---------------------------------------------------------------
        VOID EmitLabelRef(CodeLabel* target, const InstructionFormat & instructionFormat, UINT variationCode);


        //---------------------------------------------------------------
        // Sets the label to point to the current "instruction pointer"
        // It is invalid to call EmitLabel() twice on
        // the same label.
        //---------------------------------------------------------------
        VOID EmitLabel(CodeLabel* pCodeLabel);

        //---------------------------------------------------------------
        // Emits the patch label for the stub.
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        void EmitPatchLabel();

        //---------------------------------------------------------------
        // Emits the return label for the stub.
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        void EmitReturnLabel();

        //---------------------------------------------------------------
        // Create a new label to an external address.
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        CodeLabel* NewExternalCodeLabel(LPVOID pExternalAddress);

        //---------------------------------------------------------------
        // Push and Pop can be used to keep track of stack growth.
        // These should be adjusted by opcodes written to the stream.
        //
        // Note that popping & pushing stack size as opcodes are emitted
        // is naive & may not be accurate in many cases,
        // so complex stubs may have to manually adjust the stack size.
        // However it should work for the vast majority of cases we care
        // about.
        //---------------------------------------------------------------
        void Push(UINT size);
        void Pop(UINT size);

        INT GetStackSize() { LEAF_CONTRACT; return m_stackSize; }
        void SetStackSize(SHORT size) { LEAF_CONTRACT; m_stackSize = size; }

        void SetDataOnly(BOOL fDataOnly = TRUE) { LEAF_CONTRACT; m_fDataOnly = fDataOnly; }

        //===========================================================================
        // Unwind information

        // Records location of preserved or parameter register
        VOID UnwindSavedReg (UCHAR reg, ULONG SPRelativeOffset);
        VOID UnwindPushedReg (UCHAR reg);

        // Records "sub rsp, xxx"
        VOID UnwindAllocStack (ULONG FrameSizeIncrement);

        // Records frame pointer register
        VOID UnwindSetFramePointer (UCHAR reg);

        // Records location of an alternate entry point.
        VOID UnwindBeginNewEntryPoint ();

        // In DEBUG, emits a call to m_pUnwindInfoCheckLabel (via
        // EmitUnwindInfoCheckWorker).  Code at that label will call to a
        // helper that will attempt to RtlVirtualUnwind through the stub.  The
        // helper will preserve ALL registers.
        VOID EmitUnwindInfoCheck();

#if defined(_DEBUG) && defined(STUBLINKER_GENERATES_UNWIND_INFO)
protected:

        // Injects a call to the given label.
        virtual VOID EmitUnwindInfoCheckWorker (CodeLabel *pCheckLabel) { _ASSERTE(!"override me"); }

        // Emits a call to a helper that will attempt to RtlVirtualUnwind
        // through the stub.  The helper will preserve ALL registers.
        virtual VOID EmitUnwindInfoCheckSubfunction() { _ASSERTE(!"override me"); }
#endif

public:

        //---------------------------------------------------------------
        // Generate the actual stub. The returned stub has a refcount of 1.
        // No other methods (other than the destructor) should be called
        // after calling Link().
        //
        // fMC Set to true if the stub is a multicast delegate, false otherwise
        //
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        Stub *Link(UINT *pcbSize = NULL, BOOL fMC = FALSE) { WRAPPER_CONTRACT; return Link(NULL, pcbSize, fMC); }
        Stub *Link(LoaderHeap *heap, UINT *pcbSize = NULL, BOOL fMC = FALSE);

        //---------------------------------------------------------------
        // Generate the actual stub. The returned stub has a refcount of 1.
        // No other methods (other than the destructor) should be called
        // after calling Link(). The linked stub must have its increment
        // increased by one prior to calling this method. This method
        // does not increment the reference count of the interceptee.
        //
        // Throws COM+ exception on failure.
        //---------------------------------------------------------------
        Stub *LinkInterceptor(Stub* interceptee, void *pRealAddr)
            { WRAPPER_CONTRACT; return LinkInterceptor(NULL,interceptee, pRealAddr); }
        Stub *LinkInterceptor(LoaderHeap *heap, Stub* interceptee, void *pRealAddr);

    private:
        CodeElement   *m_pCodeElements;     // stored in *reverse* order
        CodeLabel     *m_pFirstCodeLabel;   // linked list of CodeLabels
        LabelRef      *m_pFirstLabelRef;    // linked list of references
        CodeLabel     *m_pPatchLabel;       // label of stub patch offset
                                            // currently just for multicast
                                            // frames.
        CodeLabel     *m_pReturnLabel;      // label of stub return offset
        SHORT         m_returnStackSize;    // label of stub stack size
                                            // @ return label
        SHORT         m_stackSize;          // count of pushes/pops
        CQuickHeap    m_quickHeap;          // throwaway heap for
                                            //   labels, and
                                            //   internals.
        BOOL          m_fDataOnly;          // the stub contains only data - does not need FlushInstructionCache
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
        IntermediateUnwindInfo *m_pUnwindInfoList;
        UINT          m_nUnwindSlots;       // number of slots to allocate at end, == UNWIND_INFO::CountOfCodes
        BOOL          m_fHaveFramePointer;  // indicates stack operations no longer need to be recorded
        UINT          m_nEntryPoints;       // number of prologues
        UINT          m_nLastEntryPointSlots;  // number of slots used at end of last entry point
        IntermediateUnwindInfoPrologueList *m_LastPrologueEntry;
#ifdef _DEBUG
        CodeLabel     *m_pUnwindInfoCheckLabel;  // subfunction to call to unwind info check helper.
                                                 // On AMD64, the prologue is restricted to 256
                                                 // bytes, so this reduces the size of the injected
                                                 // code from 14 to 5 bytes.
#endif
#endif

        CodeRun *AppendNewEmptyCodeRun();


        // Returns pointer to last CodeElement or NULL.
        CodeElement *GetLastCodeElement()
        {
            LEAF_CONTRACT;
            return m_pCodeElements;
        }

        // Appends a new CodeElement.
        VOID AppendCodeElement(CodeElement *pCodeElement);


        // Calculates the size of the stub code that is allocate
        // immediately after the stub object. Returns the
        // total size. GlobalSize contains the size without
        // that data part.
        virtual int CalculateSize(int* globalsize);

        // Writes out the code element into memory following the
        // stub object.
        bool EmitStub(Stub* pStub, int globalsize);

        CodeRun *GetLastCodeRunIfAny();

};

//************************************************************************
// CodeLabel
//************************************************************************
struct CodeLabel
{
    // Link pointer for StubLink's list of labels
    CodeLabel       *m_next;

    // if FALSE, label refers to some code within the same stub
    // if TRUE, label refers to some externally supplied address.
    BOOL             m_fExternal;

    // if TRUE, means we want the actual address of the label and
    // not an offset to it
    BOOL             m_fAbsolute;

    union {

        // Internal
        struct {
            // Indicates the position of the label, expressed
            // as an offset into a CodeRun.
            CodeRun         *m_pCodeRun;
            UINT             m_localOffset;

        } i;


        // External
        struct {
            LPVOID           m_pExternalAddress;
        } e;
    };
};




//-------------------------------------------------------------------------
// An executable stub. These can only be created by the StubLinker().
// Each stub has a reference count (which is maintained in a thread-safe
// manner.) When the ref-count goes to zero, the stub automatically
// cleans itself up.
//-------------------------------------------------------------------------
typedef DPTR(class Stub) PTR_Stub;
typedef DPTR(PTR_Stub) PTR_PTR_Stub;
class Stub
{
    friend class CheckDuplicatedStructLayouts;
    friend class CheckAsmOffsets;

    protected:
    enum
    {
        MULTICAST_DELEGATE_BIT = 0x80000000,
        CALL_SITE_BIT          = 0x40000000,
        LOADER_HEAP_BIT        = 0x20000000,
        INTERCEPT_BIT          = 0x10000000,
        UNWIND_INFO_BIT        = 0x08000000,
        METHODDESC_CALL_BIT    = 0x04000000,

        PATCH_OFFSET_MASK      = METHODDESC_CALL_BIT - 1,
        MAX_PATCH_OFFSET       = PATCH_OFFSET_MASK + 1,
    };

    C_ASSERT(PATCH_OFFSET_MASK < METHODDESC_CALL_BIT);

    // CallSiteInfo is allocated before the stub when
    // the CALL_SITE_BIT is set
    struct CallSiteInfo
    {
        USHORT  returnOffset;
        USHORT  stackSize;
    };
    typedef DPTR(CallSiteInfo) PTR_CallSiteInfo;

    public:
        //-------------------------------------------------------------------
        // Inc the refcount.
        //-------------------------------------------------------------------
        VOID IncRef();


        //-------------------------------------------------------------------
        // Dec the refcount.
        // Returns true if the count went to zero and the stub was deleted
        //-------------------------------------------------------------------
        BOOL DecRef();


        //-------------------------------------------------------------------
        // ForceDelete
        //
        // Forces a stub to free itself. This routine forces the refcount
        // to 1, then does a DecRef. It is not threadsafe, and thus can
        // only be used in shutdown scenarios.
        //-------------------------------------------------------------------
        VOID ForceDelete();



        //-------------------------------------------------------------------
        // Used for throwing out unused stubs from stub caches. This
        // method cannot be 100% accurate due to race conditions. This
        // is ok because stub cache management is robust in the face
        // of missed or premature cleanups.
        //-------------------------------------------------------------------
        BOOL HeuristicLooksOrphaned()
        {
            LEAF_CONTRACT;
            _ASSERTE(m_signature == kUsedStub);
            return (m_refcount == 1);
        }

        //-------------------------------------------------------------------
        // Used by the debugger to help step through stubs
        //-------------------------------------------------------------------
        BOOL IsIntercept()
        {
            LEAF_CONTRACT;
            return (m_patchOffset & INTERCEPT_BIT) != 0;
        }

        BOOL IsMulticastDelegate()
        {
            LEAF_CONTRACT;
            return (m_patchOffset & MULTICAST_DELEGATE_BIT) != 0;
        }

        //-------------------------------------------------------------------
        // For stubs which execute user code, a patch offset needs to be set
        // to tell the debugger how far into the stub code the debugger has
        // to step until the frame is set up.
        //-------------------------------------------------------------------
        USHORT GetPatchOffset()
        {
            LEAF_CONTRACT;

            return (USHORT)(m_patchOffset & PATCH_OFFSET_MASK);
        }

        void SetPatchOffset(USHORT offset)
        {
            LEAF_CONTRACT;
            _ASSERTE(GetPatchOffset() == 0);
            m_patchOffset |= offset;
            _ASSERTE(GetPatchOffset() == offset);
        }

        TADDR GetPatchAddress()
        {
            WRAPPER_CONTRACT;

            return (TADDR)GetEntryPointInternal() + GetPatchOffset();
        }                

        //-------------------------------------------------------------------
        // For stubs which call unmanaged code, the stub should publish
        // information about the unmanaged call site.  Specifically,
        //  * returnOffset - offset into the stub of the return address
        //      from the call
        //  * stackSize - offset on the stack (from the end of the frame)
        //      where the return address is pushed during the call
        //-------------------------------------------------------------------

        BOOL HasCallSiteInfo()
        {
            LEAF_CONTRACT;
            return (m_patchOffset & CALL_SITE_BIT) != 0;
        }

        CallSiteInfo *GetCallSiteInfo()
        {
            STATIC_CONTRACT_NOTHROW;
            STATIC_CONTRACT_GC_NOTRIGGER;
            STATIC_CONTRACT_FAULT;

            _ASSERTE(HasCallSiteInfo());

            TADDR info = PTR_HOST_TO_TADDR(this);

            if (IsIntercept())
            {
                info -= 2 * sizeof(TADDR);
            }

            info -= sizeof(CallSiteInfo);

            return PTR_CallSiteInfo(info);
        }

        USHORT GetCallSiteReturnOffset()
        {
            WRAPPER_CONTRACT;
            return GetCallSiteInfo()->returnOffset;
        }

        TADDR GetCallSiteReturnAddress()
        {
            WRAPPER_CONTRACT;

            return (TADDR)GetEntryPointInternal() + GetCallSiteReturnOffset();
        }                

        void SetCallSiteReturnOffset(USHORT offset)
        {
            WRAPPER_CONTRACT;
            _ASSERTE(offset < USHRT_MAX);
            GetCallSiteInfo()->returnOffset = offset;
        }

        USHORT GetCallSiteStackSize()
        {
            WRAPPER_CONTRACT;
            return GetCallSiteInfo()->stackSize;
        }

        void SetCallSiteStackSize(USHORT stackSize)
        {
            WRAPPER_CONTRACT;
            _ASSERTE(stackSize < USHRT_MAX);
            GetCallSiteInfo()->stackSize = stackSize;
        }

        //-------------------------------------------------------------------
        // Unwind information.
        //-------------------------------------------------------------------

#ifdef STUBLINKER_GENERATES_UNWIND_INFO

        BOOL HasUnwindInfo()
        {
            LEAF_CONTRACT;
            return (m_patchOffset & UNWIND_INFO_BIT) != 0;
        }

        StubUnwindInfoHeaderSuffix *GetUnwindInfoHeaderSuffix()
        {
            CONTRACTL
            {
                NOTHROW;
                GC_NOTRIGGER;
                FORBID_FAULT;
            }
            CONTRACTL_END

            _ASSERTE(HasUnwindInfo());

            TADDR info = PTR_HOST_TO_TADDR(this);

            if (IsIntercept())
            {
                info -= 2 * sizeof(TADDR);
            }

            if (HasCallSiteInfo())
            {
                info -= sizeof(CallSiteInfo);
            }

            return PTR_StubUnwindInfoHeaderSuffix
                (info - sizeof(StubUnwindInfoHeaderSuffix));
        }

        StubUnwindInfoHeader *GetUnwindInfoHeader()
        {
            CONTRACTL
            {
                NOTHROW;
                GC_NOTRIGGER;
                FORBID_FAULT;
            }
            CONTRACTL_END

            StubUnwindInfoHeaderSuffix *pSuffix = GetUnwindInfoHeaderSuffix();

            TADDR suffixEnd = PTR_HOST_TO_TADDR(pSuffix) + sizeof(*pSuffix);
            TADDR cbPrefix = PTR_HOST_TO_TADDR(this) - suffixEnd;

            return PTR_StubUnwindInfoHeader(suffixEnd -
                                            StubUnwindInfoHeader::ComputeSize(pSuffix->nTotalSlots, pSuffix->nEntryPoints, cbPrefix));
        }

#endif // STUBLINKER_GENERATES_UNWIND_INFO

        //-------------------------------------------------------------------
        // Returns pointer to the start of the allocation containing this Stub.
        //-------------------------------------------------------------------
        TADDR GetAllocationBase()
        {
            CONTRACTL
            {
                NOTHROW;
                GC_NOTRIGGER;
                FORBID_FAULT;
            }
            CONTRACTL_END

            TADDR info = PTR_HOST_TO_TADDR(this);
            SIZE_T cbPrefix = 0;

            if (IsIntercept())
            {
                cbPrefix += 2 * sizeof(TADDR);
            }

            if (HasCallSiteInfo())
            {
                cbPrefix += sizeof(CallSiteInfo);
            }

#ifdef STUBLINKER_GENERATES_UNWIND_INFO
            if (HasUnwindInfo())
            {
                StubUnwindInfoHeaderSuffix *pSuffix =
                    PTR_StubUnwindInfoHeaderSuffix(info - cbPrefix -
                                                   sizeof(*pSuffix));

                cbPrefix += StubUnwindInfoHeader::ComputeSize(pSuffix->nTotalSlots, pSuffix->nEntryPoints, cbPrefix);
            }
#endif // STUBLINKER_GENERATES_UNWIND_INFO

            return info - cbPrefix;
        }

        //-------------------------------------------------------------------
        // Return executable entrypoint after checking the ref count.
        //-------------------------------------------------------------------
        const BYTE *GetEntryPoint()
        {
            WRAPPER_CONTRACT;

            _ASSERTE(m_signature == kUsedStub);
            _ASSERTE(m_refcount > 0);

            return GetEntryPointInternal();
        }

        //-------------------------------------------------------------------
        // Return the Stub as in GetEntryPoint and size of the stub+code in bytes
        //   WARNING: Depending on the stub kind this may be just Stub size as 
        //            not all stubs have the info about the code size.
        //            It's the caller responsibility to determine that
        //-------------------------------------------------------------------
        static Stub* RecoverStubAndSize(TADDR pEntryPoint, DWORD *pSize)
        {
            CONTRACT(Stub*)
            {
                NOTHROW;
                GC_NOTRIGGER;
                MODE_ANY;

                PRECONDITION(pEntryPoint && pSize);
            }
            CONTRACT_END;

            Stub *pStub = Stub::RecoverStub(pEntryPoint);
            *pSize = sizeof(Stub) + pStub->m_numCodeBytes;
            RETURN pStub;
        }


        //-------------------------------------------------------------------
        // Reverse GetEntryPoint.
        //-------------------------------------------------------------------
        static Stub* RecoverStub(TADDR pEntryPoint)
        {
            STATIC_CONTRACT_NOTHROW;
            STATIC_CONTRACT_GC_NOTRIGGER;

            Stub *pStub = PTR_Stub(pEntryPoint - sizeof(*pStub));

#if !defined(DACCESS_COMPILE)
            _ASSERTE(pStub->m_signature == kUsedStub);
            _ASSERTE((TADDR)pStub->GetEntryPoint() == pEntryPoint);
#elif defined(_DEBUG)
            if (pStub->m_signature != kUsedStub ||
                (TADDR)pStub->GetEntryPoint() != pEntryPoint)
            {
                DacError(E_INVALIDARG);
            }
#endif
            return pStub;
        }


        static UINT32 GetOffsetOfEntryPoint()
        {
            LEAF_CONTRACT;
            return (UINT32)sizeof(Stub);
        }

        //-------------------------------------------------------------------
        // Returns TRUE if entry point is not inside the Stub allocation.
        //-------------------------------------------------------------------
        BOOL HasExternalEntryPoint() const
        {
            LEAF_CONTRACT;
            _ASSERTE(m_signature == kUsedStub);


            return FALSE;
        }

        // -------------------------------------------------------------------
        // Call this if the stub's entry point expects a MethodDesc* to be
        // passed to it somehow.
        //
        // On IA64, this means it will transition through a MethodEntryChunk.
        //
        // On x86, this means it must be called through the MethodDesc
        // pre-pad, so that at the entry point of the stub, the MethodDesc*
        // appears to be the return address.
        //
        // On AMD64, for native stubs (i.e. our assembly helpers), this means
        // the same thing as on x86.  However, for IL stubs, it means that
        // we need to pass the MethodDesc* in r10.
        // -------------------------------------------------------------------
        void SetRequiresMethodDescCallingConvention ()
        {
            WRAPPER_CONTRACT;

            // Multicast stubs automatically require this and don't need to be
            // explicitly marked.
            _ASSERTE(!IsMulticastDelegate());

            m_patchOffset |= METHODDESC_CALL_BIT;
        }

        // -------------------------------------------------------------------
        // Returns TRUE if the stub's entry point expects a MethodDesc* to be
        // passed to it somehow.
        // -------------------------------------------------------------------
        BOOL RequiresMethodDescCallingConvention() const
        {
            LEAF_CONTRACT;
            _ASSERTE(m_signature == kUsedStub);

            // Multicast stubs always require the MethodDesc.  We must check
            // both bits because for multicast stubs, METHODDESC_CALL_BIT
            // overlaps one of the multicast bit fields.
            //
            //  MULTICAST_DELEGATE_BIT  METHODDESC_CALL_BIT     result
            //  0                       0                       FALSE
            //  0                       1                       TRUE
            //  1                       don't care              TRUE
            //
            return 0 != (m_patchOffset & (MULTICAST_DELEGATE_BIT | METHODDESC_CALL_BIT));
        }

        //-------------------------------------------------------------------
        // This is the guy that creates stubs.
        // fMC: Set to true if the stub is a multicast delegate, false otherwise
        //-------------------------------------------------------------------
        static Stub* NewStub(LoaderHeap *pLoaderHeap, UINT numCodeBytes,
                             BOOL intercept = FALSE, BOOL callSiteInfo = FALSE,
                             BOOL fMC = FALSE
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
                             , UINT nUnwindInfoSlots = 0,
                             UINT nEntryPoints = 1
#endif
                             );


        //-------------------------------------------------------------------
        // One-time init
        //-------------------------------------------------------------------
        static void Init();

    protected:
        // fMC: Set to true if the stub is a multicast delegate, false otherwise
        void SetupStub(int numCodeBytes, BOOL fIntercepted, BOOL fLoaderHeap,
                       BOOL callSiteInfo, BOOL fMC
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
                       , UINT nUnwindInfoSlots
#endif
                       );
        void DeleteStub();

        //-------------------------------------------------------------------
        // Return executable entrypoint without checking the ref count.
        //-------------------------------------------------------------------
        inline const BYTE *GetEntryPointInternal()
        {
            LEAF_CONTRACT;

            {
                // StubLink always puts the entrypoint first.
                return (const BYTE *)(PTR_HOST_TO_TADDR(this) + sizeof(*this));
            }
        }

        ULONG   m_refcount;
        ULONG   m_patchOffset;

        UINT    m_numCodeBytes;
#ifdef _DEBUG
        enum {
            kUsedStub  = 0x42555453,     // 'STUB'
            kFreedStub = 0x46555453,     // 'STUF'
        };


        UINT32  m_signature;
        Stub*   m_Next;
#else 
#endif // _DEBUG


        
#ifdef _DEBUG
        Stub()      // Stubs are created by NewStub(), not "new". Hide the
        { LEAF_CONTRACT; }          //  constructor to enforce this.
#endif

};


/*
 * The InterceptStub hides a reference to the real stub at a negative offset.
 * When this stub is deleted it decrements the real stub cleaning it up as
 * well. The InterceptStub is created by the Stublinker.
 *
 *                                                                      
 *        The stubs are linked - GetInterceptedStub will return either
 *        a pointer to the next intercept stub (if there is one), or NULL,
 *        indicating end-of-chain.  GetRealAddr will return the address of
 *        the "real" code, which may, in fact, be another thunk (for example),
 *        and thus should be traced as well.
 */

typedef DPTR(class InterceptStub) PTR_InterceptStub;
class InterceptStub : public Stub
{
    friend class Stub;
    public:
        //-------------------------------------------------------------------
        // This is the guy that creates stubs.
        //-------------------------------------------------------------------
        static Stub* NewInterceptedStub(LoaderHeap *pHeap,
                                        UINT numCodeBytes,
                                        Stub* interceptee,
                                        void* pRealAddr,
                                        BOOL callSiteInfo = FALSE
#ifdef STUBLINKER_GENERATES_UNWIND_INFO
                                        , UINT nUnwindInfoSlots = 0,
                                        UINT nEntryPoints = 1
#endif
                                        );

        //---------------------------------------------------------------
        // Expose key offsets and values for stub generation.
        //---------------------------------------------------------------
        int GetNegativeOffset()
        {
            LEAF_CONTRACT;
            return sizeof(TADDR) + GetNegativeOffsetRealAddr();
        }

        PTR_PTR_Stub GetInterceptedStub()
        {
            LEAF_CONTRACT;
            return PTR_PTR_Stub(PTR_HOST_TO_TADDR(this) -
                                GetNegativeOffset());
        }

        int GetNegativeOffsetRealAddr()
        {
            LEAF_CONTRACT;
            return sizeof(TADDR);
        }

        PTR_TADDR GetRealAddr()
        {
            LEAF_CONTRACT;
            return PTR_TADDR(PTR_HOST_TO_TADDR(this) -
                             GetNegativeOffsetRealAddr());
        }


protected:
        void DeleteStub();
        void ReleaseInterceptedStub();

#ifdef _DEBUG
        InterceptStub()  // Intercept stubs are only created by NewInterceptStub .
        { LEAF_CONTRACT; }
#endif
};

//-------------------------------------------------------------------------
// Each platform encodes the "branch" instruction in a different
// way. We use objects derived from InstructionFormat to abstract this
// information away. InstructionFormats don't contain any variable data
// so they should be allocated statically.
//
// Note that StubLinker does not create or define any InstructionFormats.
// The client does.
//
// The following example shows how to define a InstructionFormat for the
// X86 jump near instruction which takes on two forms:
//
//   EB xx        jmp  rel8    ;; SHORT JMP (signed 8-bit offset)
//   E9 xxxxxxxx  jmp  rel32   ;; NEAR JMP (signed 32-bit offset)
//
// InstructionFormat's provide StubLinker the following information:
//
//   RRT.m_allowedSizes
//
//     What are the possible sizes that the reference can
//     take? The X86 jump can take either an 8-bit or 32-bit offset
//     so this value is set to (k8|k32). StubLinker will try to
//     use the smallest size possible.
//
//
//   RRT.m_fTreatSizesAsSigned
//     Sign-extend or zero-extend smallsizes offsets to the platform
//     code pointer size? For x86, this field is set to TRUE (rel8
//     is considered signed.)
//
//
//   UINT RRT.GetSizeOfInstruction(refsize, variationCode)
//     Returns the total size of the instruction in bytes for a given
//     refsize. For this example:
//
//          if (refsize==k8) return 2;
//          if (refsize==k32) return 5;
//
//   UINT RRT.GetSizeOfData(refsize, variationCode)
//     Returns the total size of the seperate data area (if any) that the
//     instruction needs in bytes for a given refsize. For this example
//     on the SH3
//          if (refsize==k32) return 4; else return 0;
//
//   The default implem of this returns 0, so CPUs that don't have need
//   for a seperate constant area don't have to worry about it.
//
//
//   BOOL CanReach(refsize, variationcode, fExternal, offset)
//     Returns whether the instruction with the given variationcode &
//     refsize can reach the given offset. In the case of External
//     calls, fExternal is set and offset is the target address. In this case an
//     implementation should return TRUE only if refsize is big enough to fit a
//     full machine-sized pointer to anywhere in the address space.
//
//
//   VOID RRT.EmitInstruction(UINT     refsize,
//                            __int64  fixedUpReference,
//                            BYTE    *pOutBuffer,
//                            UINT     variationCode,
//                            BYTE    *pDataBuffer)
//
//     Given a chosen size (refsize) and the final offset value
//     computed by StubLink (fixedUpReference), write out the
//     instruction into the provided buffer (guaranteed to be
//     big enough provided you told the truth with GetSizeOfInstruction()).
//     If needed (e.g. on SH3) a data buffer is also passed in for
//     storage of constants.
//
//     For x86 jmp near:
//
//          if (refsize==k8) {
//              pOutBuffer[0] = 0xeb;
//              pOutBuffer[1] = (__int8)fixedUpReference;
//          } else if (refsize == k32) {
//              pOutBuffer[0] = 0xe9;
//              *((__int32*)(1+pOutBuffer)) = (__int32)fixedUpReference;
//          } else {
//              CRASH("Bad input.");
//          }
//
// VOID RRT.GetHotSpotOffset(UINT refsize, UINT variationCode)
//
//     The reference offset is always relative to some IP: this
//     method tells StubLinker where that IP is relative to the
//     start of the instruction. For X86, the offset is always
//     relative to the start of the *following* instruction so
//     the correct implementation is:
//
//          return GetSizeOfInstruction(refsize, variationCode);
//
//     Actually, InstructionFormat() provides a default implementation of this
//     method that does exactly this so X86 need not override this at all.
//
//
// The extra "variationCode" argument is an __int32 that StubLinker receives
// from EmitLabelRef() and passes uninterpreted to each RRT method.
// This allows one RRT to handle a family of related instructions,
// for example, the family of conditional jumps on the X86.
//
//-------------------------------------------------------------------------
class InstructionFormat
{
    private:
        enum
        {
        // if you want to add a size, insert it in-order (e.g. a 18-bit size would
        // go between k16 and k32) and shift all the higher values up. All values
        // must be a power of 2 since the get ORed together.

            _k8,
#ifdef INSTRFMT_K9
            _k9,
#endif
#ifdef INSTRFMT_K13
            _k13,
#endif
            _k16,
#ifdef INSTRFMT_K24
            _k24,
#endif
#ifdef INSTRFMT_K26
            _k26,
#endif
            _k32,
#ifdef INSTRFMT_K64SMALL
            _k64Small,
#endif
#ifdef INSTRFMT_K64
            _k64,
#endif
            _kAllowAlways,
        };

    public:

        enum
        {
            k8          = (1 << _k8),
#ifdef INSTRFMT_K9
            k9          = (1 << _k9),
#endif
#ifdef INSTRFMT_K13
            k13         = (1 << _k13),
#endif
            k16         = (1 << _k16),
#ifdef INSTRFMT_K24
            k24         = (1 << _k24),
#endif
#ifdef INSTRFMT_K26
            k26         = (1 << _k26),
#endif
            k32         = (1 << _k32),
#ifdef INSTRFMT_K64SMALL
            k64Small    = (1 << _k64Small),
#endif
#ifdef INSTRFMT_K64
            k64         = (1 << _k64),
#endif
            kAllowAlways= (1 << _kAllowAlways),
            kMax = kAllowAlways,
        };

        const UINT m_allowedSizes;         // OR mask using above "k" values
        InstructionFormat(UINT allowedSizes) : m_allowedSizes(allowedSizes)
        {
            LEAF_CONTRACT;
        }

        virtual UINT GetSizeOfInstruction(UINT refsize, UINT variationCode) = 0;
        virtual VOID EmitInstruction(UINT refsize, __int64 fixedUpReference, BYTE *pCodeBuffer, UINT variationCode, BYTE *pDataBuffer) = 0;
        virtual UINT GetHotSpotOffset(UINT refsize, UINT variationCode)
        {
            WRAPPER_CONTRACT;
            // Default implementation: the offset is added to the
            // start of the following instruction.
            return GetSizeOfInstruction(refsize, variationCode);
        }

        virtual UINT GetSizeOfData(UINT refsize, UINT variationCode)
        {
            LEAF_CONTRACT;
            // Default implementation: 0 extra bytes needed (most CPUs)
            return 0;
        }

        virtual BOOL CanReach(UINT refsize, UINT variationCode, BOOL fExternal, INT_PTR offset)
        {
            LEAF_CONTRACT;

            if (fExternal) {
                // For external, we don't have enough info to predict
                // the offset yet so we only accept if the offset size
                // is at least as large as the native pointer size.
                switch(refsize) {
                    case InstructionFormat::k8: // intentional fallthru
                    case InstructionFormat::k16: // intentional fallthru
#ifdef INSTRFMT_K24
                    case InstructionFormat::k24: // intentional fallthru
#endif
#ifdef INSTRFMT_K26
                    case InstructionFormat::k26: // intentional fallthru
#endif
                        return FALSE;           // no 8 or 16-bit platforms

                    case InstructionFormat::k32:
                        return sizeof(LPVOID) <= 4;
#ifdef INSTRFMT_K64
                    case InstructionFormat::k64:
                        return sizeof(LPVOID) <= 8;
#endif
                    case InstructionFormat::kAllowAlways:
                        return TRUE;

                    default:
                        _ASSERTE(0);
                        return FALSE;
                }
            } else {
                switch(refsize)
                {
                    case InstructionFormat::k8:
                        return FitsInI1(offset);

                    case InstructionFormat::k16:
                        return FitsInI2(offset);
                        
#ifdef INSTRFMT_K24
                    case InstructionFormat::k24:
                        return FitsInI2(offset>>8);
#endif

#ifdef INSTRFMT_K26
                    case InstructionFormat::k26:
                        return FitsInI2(offset>>10);
#endif
                    case InstructionFormat::k32:
                        return FitsInI4(offset);
#ifdef INSTRFMT_K64
                    case InstructionFormat::k64:
                        // intentional fallthru
#endif
                    case InstructionFormat::kAllowAlways:
                        return TRUE;
                    default:
                        _ASSERTE(0);
                        return FALSE;

                }
            }
        }
};





//-------------------------------------------------------------------------
// This stub cache associates stubs with an integer key.  For some clients,
// this might represent the size of the argument stack in some cpu-specific
// units (for the x86, the size is expressed in DWORDS.)  For other clients,
// this might take into account the style of stub (e.g. whether it returns
// an object reference or not).
//-------------------------------------------------------------------------
class ArgBasedStubCache
{
    public:
       ArgBasedStubCache(UINT fixedSize = NUMFIXEDSLOTS);
       ~ArgBasedStubCache();

       //-----------------------------------------------------------------
       // Retrieves the stub associated with the given key.
       //-----------------------------------------------------------------
       Stub *GetStub(UINT_PTR key);

       //-----------------------------------------------------------------
       // Tries to associate the stub with the given key.
       // It may fail because another thread might swoop in and
       // do the association before you do. Thus, you must use the
       // return value stub rather than the pStub.
       //-----------------------------------------------------------------
       Stub* AttemptToSetStub(UINT_PTR key, Stub *pStub);


       //-----------------------------------------------------------------
       // Trigger a sweep to garbage-collect stubs.
       //-----------------------------------------------------------------
       VOID FreeUnusedStubs();

       // Suggestions for number of slots
       enum {
 #ifdef _DEBUG
             NUMFIXEDSLOTS = 3,
 #else
             NUMFIXEDSLOTS = 16,
 #endif
       };

#ifdef _DEBUG
       VOID Dump();  //Diagnostic dump
#endif

    private:

       // How many low-numbered keys have direct access?
       UINT      m_numFixedSlots;

       // For 'm_numFixedSlots' low-numbered keys, we store them in an array.
       Stub    **m_aStub;


       struct SlotEntry
       {
           Stub             *m_pStub;
           UINT_PTR         m_key;
           SlotEntry        *m_pNext;
       };

       // High-numbered keys are stored in a sparse linked list.
       SlotEntry            *m_pSlotEntries;


       Crst                  m_crst;
};


//-------------------------------------------------------------------------
// This is just like an ArgBasedStubCache but does not allow for premature
// cleanup of stubs.
//-------------------------------------------------------------------------
class ArgBasedStubRetainer : public ArgBasedStubCache
{
    public:
        //-----------------------------------------------------------------
        // This method is overriden to prevent premature stub deletions.
        //-----------------------------------------------------------------
        VOID FreeUnusedStubs()
        {
            LEAF_CONTRACT;
            _ASSERTE(!"Don't call me, I won't call you.");
        }
};



#define SecurityStubLinker StubLinkerCPU
#define NDirectStubLinker StubLinkerCPU

#define CPUSTUBLINKER StubLinkerCPU

class SecurityStubLinker;
class NDirectStubLinker;
class CPUSTUBLINKER;





#endif // __stublink_h__

