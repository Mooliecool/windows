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
// ===========================================================================
// File: JITinterfaceX86.CPP
//
// ===========================================================================

// This contains JITinterface routines that are tailored for
// X86 platforms. Non-X86 versions of these can be found in
// JITinterfaceGen.cpp

#include "common.h"
#include "jitinterface.h"
#include "eeconfig.h"
#include "excep.h"
#include "comstring.h"
#include "comdelegate.h"
#include "remoting.h" // create context bound and remote class instances
#include "field.h"
#include "ecall.h"
#include "asmconstants.h"
#include "virtualcallstub.h"

#if defined(_DEBUG) && !defined (WRITE_BARRIER_CHECK) 
#define WRITE_BARRIER_CHECK 1
#endif



// To test with MON_DEBUG off, comment out the following line. DO NOT simply define
// to be 0 as the checks are for #ifdef not #if 0.
#ifdef _DEBUG 
#define MON_DEBUG 1
#endif

class generation;
extern "C" generation generation_table[];

extern "C" void __stdcall JIT_WriteBarrierReg_PreGrow();// JIThelp.asm/JIThelp.s
extern "C" void __stdcall JIT_WriteBarrierReg_PostGrow();// JIThelp.asm/JIThelp.s

// This needs to be placed in the PE .text section (it needs to be executable)
#ifdef _MSC_VER
#pragma code_seg(push, ".text")
__declspec(allocate(".text")) JIT_Writeable_Thunks JIT_Writeable_Thunks_Buf; 
#pragma code_seg(pop)
#else // _MSC_VER
const JIT_Writeable_Thunks JIT_Writeable_Thunks_Buf = {{{0}}};
#endif // _MSC_VER

JIT_WriteBarrierReg_Thunk *JIT_WriteBarrierReg_Buf;

// Mark beginning of thunk buffer for EH checking
BYTE *JIT_WriteBarrier_Buf_Start = (BYTE *)JIT_Writeable_Thunks_Buf.WriteBarrierReg;
// End of thunk buffer
BYTE *JIT_WriteBarrier_Buf_Last = (BYTE *)JIT_Writeable_Thunks_Buf.WriteBarrierReg + 8*sizeof(JIT_WriteBarrierReg_Thunk);

#ifdef HARDBOUND_DYNAMIC_CALLS
BYTE* GetDynamicFCallThunk(int index)
{
    LEAF_CONTRACT;
    _ASSERTE(index < (int)COUNTOF(JIT_Writeable_Thunks_Buf.DynamicFCall));
    return (BYTE*)JIT_Writeable_Thunks_Buf.DynamicFCall[index].Bytes;
}

BYTE* GetDynamicHCallThunk(int index)
{
    LEAF_CONTRACT;
    _ASSERTE(index < (int)COUNTOF(JIT_Writeable_Thunks_Buf.DynamicHCall));
    return (BYTE*)JIT_Writeable_Thunks_Buf.DynamicHCall[index].Bytes;
}
#endif

#ifdef _DEBUG 
extern "C" void __stdcall WriteBarrierAssert(BYTE* ptr, Object* obj)
{
    STATIC_CONTRACT_SO_TOLERANT;
    WRAPPER_CONTRACT;

    static BOOL fVerifyHeap = -1;

    if (fVerifyHeap == -1)
        fVerifyHeap = g_pConfig->GetHeapVerifyLevel() & EEConfig::HEAPVERIFY_GC;

    if (fVerifyHeap)
    {
        obj->Validate(FALSE);
        if(GCHeap::GetGCHeap()->IsHeapPointer(ptr))
        {
            Object* pObj = *(Object**)ptr;
            _ASSERTE (pObj == NULL || GCHeap::GetGCHeap()->IsHeapPointer(pObj));
        }
    }
    else
    {
        _ASSERTE((g_lowest_address <= ptr && ptr < g_highest_address) ||
             ((size_t)ptr < MAX_UNCHECKED_OFFSET_FOR_NULL_OBJECT));
    }
}

#endif // _DEBUG



FCDECL1(Object*, JIT_NewFast, CORINFO_CLASS_HANDLE typeHnd_);


HCIMPL1(Object*, JIT_NewCrossContextHelper, CORINFO_CLASS_HANDLE typeHnd_)
{
    CONTRACTL {
        SO_TOLERANT;
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
    } CONTRACTL_END;

    TypeHandle typeHnd(typeHnd_);

    OBJECTREF newobj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame

    _ASSERTE(typeHnd.IsUnsharedMT());                                   // we never use this helper for arrays
    MethodTable *pMT = typeHnd.AsMethodTable();
    pMT->CheckRestore();

    // Remoting services determines if the current context is appropriate
    // for activation. If the current context is OK then it creates an object
    // else it creates a proxy.
    // Note: 3/20/03 Added fIsNewObj flag to indicate that CreateProxyOrObject
    // is being called from Jit_NewObj ... the fIsCom flag is FALSE by default -
    // which used to be the case before this change as well.
    newobj = CRemotingServices::CreateProxyOrObject(pMT,FALSE /*fIsCom*/,TRUE/*fIsNewObj*/);

    HELPER_METHOD_FRAME_END();
    return(OBJECTREFToObject(newobj));
}
HCIMPLEND

HCIMPL1(Object*, AllocObjectWrapper, MethodTable *pMT)
{
     CONTRACTL {
        THROWS;
        DISABLED(GC_TRIGGERS);      // currently disabled because of FORBIDGC in HCIMPL
        SO_TOLERANT;
    } CONTRACTL_END;

    OBJECTREF newObj = NULL;
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB(Frame::FRAME_ATTR_RETURNOBJ);    // Set up a frame
    newObj = FastAllocateObject(pMT);
    HELPER_METHOD_FRAME_END();
    return OBJECTREFToObject(newObj);
}
HCIMPLEND



/*********************************************************************/
// Caller has to be an EBP frame, callee-saved registers (EDI, ESI, EBX) have
// to be saved on the stack just below the stack arguments,
// enregistered arguements are in correct registers, remaining args pushed
// on stack, followed by target address and count of stack arguments.
// So the stack will look like TailCallArgs
#if defined(_MSC_VER) 
#pragma warning(push)
#pragma warning(disable : 4200 )  // zero-sized array
#endif // defined(_MSC_VER)
struct TailCallArgs
{
    MachState           machState;
    ArgumentRegisters   argRegs;
    Thread*             pThread;

    DWORD       dwRetAddr;
    union {
        DWORD           dwTargetAddr;
        const BYTE***   pIndirection;   // for VSD case
    };

    unsigned    calleeSavedRegs     : 1; // Indicates that the tailcall helper should restore callee saved regs
    unsigned    virtualStubDispatch : 1; // Indicates that pIndirection is a pointer to a stub dispatch indirect cell.

    DWORD       nNewStackArgs;
    DWORD       nOldStackArgs;
    DWORD       newStackArgs[0];
};
#if defined(_MSC_VER) 
#pragma warning(pop)
#endif

extern "C" void __stdcall JIT_TailCallHelper(TailCallArgs * args);
extern "C" void __stdcall JIT_TailCallReturnFromVSD();

extern "C" BYTE g_TailCanSkipTripForGC;
BYTE g_TailCanSkipTripForGC;

extern "C" void* g_TailCallFrameVptr;
void* g_TailCallFrameVptr;

void __stdcall JIT_TailCallHelper(TailCallArgs * args)
{
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
        SO_TOLERANT;
    } CONTRACTL_END;


    BEGIN_SO_INTOLERANT_CODE(args->pThread);
    
    BEGINFORBIDGC();

    { // Scope for SkipTrip label

    // Note: We depend on g_TailCanSkipTripForGC to set g_TailCallFrameVptr
    if (!g_TailCallFrameVptr)
        g_TailCallFrameVptr = (void*)TailCallFrame::GetFrameVtable();

    if (!g_TailCanSkipTripForGC && g_pConfig->GetGCStressLevel() == 0)
        g_TailCanSkipTripForGC = true;

    bool shouldTrip = args->pThread->CatchAtSafePoint() != 0;

#ifdef STRESS_HEAP 
    bool gcStress = g_pConfig->GetGCStressLevel() && g_pConfig->GetGCconcurrent();
#ifdef _DEBUG 
    // We dont want to do a GC every time through here. So trip the thread only if FastGCStressLevel
    // as FastGCStressLevel can keep StressHeap() in check.
    gcStress |= ((g_pConfig->GetGCStressLevel() & EEConfig::GCSTRESS_TRANSITION) != 0) && g_pConfig->FastGCStressLevel();
#endif // _DEBUG
    shouldTrip |= gcStress;
#endif // STRESS_HEAP

    if (!shouldTrip)
        goto SkipTrip;

    /* We will rendezvous with the EE. Set up frame to protect the arguments */
    MethodDesc *pMD = NULL;
    // The target is actually a pointer to a struct for stub dispatch
    if (args->virtualStubDispatch)
    {
        const BYTE * pSiteTarget = NULL;

        pSiteTarget = *((BYTE **) (LONG_PTR) args->dwTargetAddr);


        size_t token = VirtualCallStubManager::GetTokenFromStub((TADDR)pSiteTarget);

        Object* pObj = OBJECTREFToObject(OBJECTREF((UINT_PTR)args->argRegs.ECX));
        // We should never get NULL here - the JIT generates NULL check.
        _ASSERTE(pObj != NULL);

        pMD = VirtualCallStubManager::GetRepresentativeMethodDescFromToken(token, pObj->GetTrueMethodTable());
    }
    else
        pMD = Entry2MethodDesc((BYTE *)(size_t)args->dwTargetAddr, NULL);

    // The return address is separated from the stack arguments by the
    // extra arguments passed to JIT_TailCall(). Put them together
    // while creating the helper frame. When done, we will undo this.

    DWORD oldArgs = args->nOldStackArgs;        // temp
    _ASSERTE(offsetof(TailCallArgs, nOldStackArgs) + sizeof(void*) ==
                offsetof(TailCallArgs,newStackArgs));
    args->nOldStackArgs = args->dwRetAddr;      // move dwRetAddr near newStackArgs[]
    _ASSERTE(args->machState.pRetAddr() == (TADDR*)(TADDR)0xDDDDDDDD);
    args->machState.SetRetAddr((TADDR*)&args->nOldStackArgs);

    ENDFORBIDGC();

    FrameWithCookie<HelperMethodFrame> helperFrame(&args->machState, pMD, &args->argRegs);

    // Rendezvous with the EE
#ifdef _DEBUG 
    BOOL GCOnTransition = FALSE;
    if (g_pConfig->FastGCStressLevel()) {
        GCOnTransition = GC_ON_TRANSITIONS (FALSE);
    }
#endif // _DEBUG
    CommonTripThread();

#ifdef _DEBUG 
    if (g_pConfig->FastGCStressLevel()) {
        GC_ON_TRANSITIONS (GCOnTransition);
    }
#endif // _DEBUG

#ifdef STRESS_HEAP 
    if (gcStress)
        GCHeap::GetGCHeap()->StressHeap();
#endif // STRESS_HEAP

    // Pop the frame

    helperFrame.Pop();

    BEGINFORBIDGC();

    // Undo move of dwRetAddr from close to newStackArgs[]

    args->dwRetAddr = args->nOldStackArgs;
    args->nOldStackArgs = oldArgs;
#ifdef _DEBUG 
    args->machState.SetRetAddr((TADDR*)(TADDR)0xDDDDDDDD);
#endif

    } // Scope for SkipTrip label

SkipTrip: ;


    ENDFORBIDGC();

    END_SO_INTOLERANT_CODE;
}

#ifdef MAXALLOC 
extern "C" BOOL __stdcall CheckAllocRequest(size_t n)
{
    WRAPPER_CONTRACT;
    BOOL fRequest;
    //BEGIN_ENTRYPOINT_THROWS;
    fRequest = GetGCAllocManager()->CheckRequest(n);
    //END_ENTRYPOINT_THROWS;
    return fRequest;
}

extern "C" void __stdcall UndoAllocRequest()
{
    WRAPPER_CONTRACT;
    //BEGIN_ENTRYPOINT_THROWS;
    GetGCAllocManager()->UndoRequest();
    //END_ENTRYPOINT_THROWS;
}
#endif // MAXALLOC

#if CHECK_APP_DOMAIN_LEAKS 
HCIMPL1(void *, SetObjectAppDomain, Object *pObject)
{

    CONTRACTL
    {
        DEBUG_ONLY;
        WRAPPER(THROWS);
        WRAPPER(GC_TRIGGERS);
        ENTRY_POINT;
        MODE_ANY;
    }
    CONTRACTL_END;

    ThreadPreventAbortHolder preventAbort(TRUE);
    HELPER_METHOD_FRAME_BEGIN_RET_ATTRIB_NOPOLL(Frame::FRAME_ATTR_CAPTURE_DEPTH_2|Frame::FRAME_ATTR_EXACT_DEPTH|Frame::FRAME_ATTR_POSSIBLE_MD);
    pObject->SetAppDomain();
    HELPER_METHOD_FRAME_END();

    return pObject;
}
HCIMPLEND
#endif // CHECK_APP_DOMAIN_LEAKS

    // emit code that adds MIN_OBJECT_SIZE to reg if reg is unaligned thus making it aligned
void JIT_TrialAlloc::EmitAlignmentRoundup(CPUSTUBLINKER *psl, X86Reg testAlignReg, X86Reg adjReg, Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    _ASSERTE((MIN_OBJECT_SIZE & 7) == 4);   // want to change alignment

    CodeLabel *AlreadyAligned = psl->NewCodeLabel();

    // test reg, 7
    psl->Emit16(0xC0F7 | (testAlignReg << 8));
    psl->Emit32(0x7);

    // jz alreadyAligned
    if (flags & ALIGN8OBJ)
    {
        psl->X86EmitCondJump(AlreadyAligned, X86CondCode::kJNZ);
    }
    else
    {
        psl->X86EmitCondJump(AlreadyAligned, X86CondCode::kJZ);
    }

    psl->X86EmitAddReg(adjReg, MIN_OBJECT_SIZE);
    // AlreadyAligned:
    psl->EmitLabel(AlreadyAligned);
}

    // if 'reg' is unaligned, then set the dummy object at EAX and increment EAX past
    // the dummy object
void JIT_TrialAlloc::EmitDummyObject(CPUSTUBLINKER *psl, X86Reg alignTestReg, Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CodeLabel *AlreadyAligned = psl->NewCodeLabel();

    // test reg, 7
    psl->Emit16(0xC0F7 | (alignTestReg << 8));
    psl->Emit32(0x7);

    // jz alreadyAligned
    if (flags & ALIGN8OBJ)
    {
        psl->X86EmitCondJump(AlreadyAligned, X86CondCode::kJNZ);
    }
    else
    {
        psl->X86EmitCondJump(AlreadyAligned, X86CondCode::kJZ);
    }

    // Make the fake object
    // mov EDX, [g_pObjectClass]
    psl->Emit16(0x158B);
    psl->Emit32((int)(size_t)&g_pObjectClass);

    // mov [EAX], EDX
    psl->X86EmitOffsetModRM(0x89, kEDX, kEAX, 0);

#if CHECK_APP_DOMAIN_LEAKS 
    EmitSetAppDomain(psl);
#endif

    // add EAX, MIN_OBJECT_SIZE
    psl->X86EmitAddReg(kEAX, MIN_OBJECT_SIZE);

    // AlreadyAligned:
    psl->EmitLabel(AlreadyAligned);
}

void JIT_TrialAlloc::EmitCore(CPUSTUBLINKER *psl, CodeLabel *noLock, CodeLabel *noAlloc, Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    // Upon entry here, ecx contains the method we are to try allocate memory for
    // Upon exit, eax contains the allocated memory, edx is trashed, and ecx undisturbed

    if (flags & MP_ALLOCATOR)
    {
#ifdef MAXALLOC 
        if (flags & SIZE_IN_EAX)
        {
            // save size for later
            psl->X86EmitPushReg(kEAX);
        }
        else
        {
            // load size from method table
            psl->X86EmitIndexRegLoad(kEAX, kECX, offsetof(MethodTable, m_BaseSize));
        }

        // save regs
        psl->X86EmitPushRegs((1<<kECX));

        // CheckAllocRequest(size);
        psl->X86EmitPushReg(kEAX);
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) CheckAllocRequest), 0);

        // test eax, eax
        psl->Emit16(0xc085);

        // restore regs
        psl->X86EmitPopRegs((1<<kECX));

        CodeLabel *AllocRequestOK = psl->NewCodeLabel();

        if (flags & SIZE_IN_EAX)
            psl->X86EmitPopReg(kEAX);

        // jnz             AllocRequestOK
        psl->X86EmitCondJump(AllocRequestOK, X86CondCode::kJNZ);

        if (flags & SIZE_IN_EAX)
            psl->X86EmitZeroOutReg(kEAX);

        // ret
        psl->X86EmitReturn(0);

        // AllocRequestOK:
        psl->EmitLabel(AllocRequestOK);
#endif // MAXALLOC

        if (flags & (ALIGN8 | SIZE_IN_EAX | ALIGN8OBJ))
        {
            if (flags & ALIGN8OBJ)
            {
                // mov             eax, [ecx]MethodTable.m_BaseSize
                psl->X86EmitIndexRegLoad(kEAX, kECX, offsetof(MethodTable, m_BaseSize));
            }

            psl->X86EmitPushReg(kEBX);  // we need a spare register
        }
        else
        {
            // mov             eax, [ecx]MethodTable.m_BaseSize
            psl->X86EmitIndexRegLoad(kEAX, kECX, offsetof(MethodTable, m_BaseSize));
        }

        assert( ((flags & ALIGN8)==0     ||  // EAX loaded by else statement
                 (flags & SIZE_IN_EAX)   ||  // EAX already comes filled out
                 (flags & ALIGN8OBJ)     )   // EAX loaded in the if (flags & ALIGN8OBJ) statement
                 && "EAX should contain size for allocation and it doesnt!!!");

        // Fetch current thread into EDX, preserving EAX and ECX
        psl->X86EmitTLSFetch(GetThreadTLSIndex(), kEDX, (1<<kEAX)|(1<<kECX));

        // Try the allocation.


        if (flags & (ALIGN8 | SIZE_IN_EAX | ALIGN8OBJ))
        {
            // MOV EBX, [edx]Thread.m_alloc_context.alloc_ptr
            psl->X86EmitOffsetModRM(0x8B, kEBX, kEDX, offsetof(Thread, m_alloc_context) + offsetof(alloc_context, alloc_ptr));
            // add EAX, EBX
            psl->Emit16(0xC303);
            if (flags & ALIGN8)
                EmitAlignmentRoundup(psl, kEBX, kEAX, flags);      // bump EAX up size by 12 if EBX unaligned (so that we are aligned)
        }
        else
        {
            // add             eax, [edx]Thread.m_alloc_context.alloc_ptr
            psl->X86EmitOffsetModRM(0x03, kEAX, kEDX, offsetof(Thread, m_alloc_context) + offsetof(alloc_context, alloc_ptr));
        }

        // cmp             eax, [edx]Thread.m_alloc_context.alloc_limit
        psl->X86EmitOffsetModRM(0x3b, kEAX, kEDX, offsetof(Thread, m_alloc_context) + offsetof(alloc_context, alloc_limit));

        // ja              noAlloc
        psl->X86EmitCondJump(noAlloc, X86CondCode::kJA);

        // Fill in the allocation and get out.

        // mov             [edx]Thread.m_alloc_context.alloc_ptr, eax
        psl->X86EmitIndexRegStore(kEDX, offsetof(Thread, m_alloc_context) + offsetof(alloc_context, alloc_ptr), kEAX);

        if (flags & (ALIGN8 | SIZE_IN_EAX | ALIGN8OBJ))
        {
            // mov EAX, EBX
            psl->Emit16(0xC38B);
            // pop EBX
            psl->X86EmitPopReg(kEBX);

            if (flags & ALIGN8)
                EmitDummyObject(psl, kEAX, flags);
        }
        else
        {
            // sub             eax, [ecx]MethodTable.m_BaseSize
            psl->X86EmitOffsetModRM(0x2b, kEAX, kECX, offsetof(MethodTable, m_BaseSize));
        }

        // mov             dword ptr [eax], ecx
        psl->X86EmitIndexRegStore(kEAX, 0, kECX);
    }
    else
    {
        // Take the GC lock (there is no lock prefix required - we will use JIT_TrialAllocSFastMP on an MP System).
        // inc             dword ptr [m_GCLock]
        psl->Emit16(0x05ff);
        psl->Emit32((int)(size_t)&m_GCLock);

        // jnz             NoLock
        psl->X86EmitCondJump(noLock, X86CondCode::kJNZ);

        if (flags & SIZE_IN_EAX)
        {
            // mov edx, eax
            psl->Emit16(0xd08b);
        }
        else
        {
            // mov             edx, [ecx]MethodTable.m_BaseSize
            psl->X86EmitIndexRegLoad(kEDX, kECX, offsetof(MethodTable, m_BaseSize));
        }

#ifdef MAXALLOC 
        // save regs
        psl->X86EmitPushRegs((1<<kEDX)|(1<<kECX));

        // CheckAllocRequest(size);
        psl->X86EmitPushReg(kEDX);
        psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) CheckAllocRequest), 0);

        // test eax, eax
        psl->Emit16(0xc085);

        // restore regs
        psl->X86EmitPopRegs((1<<kEDX)|(1<<kECX));

        CodeLabel *AllocRequestOK = psl->NewCodeLabel();

        // jnz             AllocRequestOK
        psl->X86EmitCondJump(AllocRequestOK, X86CondCode::kJNZ);

        // ret
        psl->X86EmitReturn(0);

        // AllocRequestOK:
        psl->EmitLabel(AllocRequestOK);
#endif // MAXALLOC

        // mov             eax, dword ptr [generation_table]
        psl->Emit8(0xA1);
        psl->Emit32((int)(size_t)&generation_table);

        // Try the allocation.
        // add             edx, eax
        psl->Emit16(0xd003);

        if (flags & (ALIGN8 | ALIGN8OBJ))
            EmitAlignmentRoundup(psl, kEAX, kEDX, flags);      // bump up EDX size by 12 if EAX unaligned (so that we are aligned)

        // cmp             edx, dword ptr [generation_table+4]
        psl->Emit16(0x153b);
        psl->Emit32((int)(size_t)&generation_table + 4);

        // ja              noAlloc
        psl->X86EmitCondJump(noAlloc, X86CondCode::kJA);

        // Fill in the allocation and get out.
        // mov             dword ptr [generation_table], edx
        psl->Emit16(0x1589);
        psl->Emit32((int)(size_t)&generation_table);

        if (flags & (ALIGN8 | ALIGN8OBJ))
            EmitDummyObject(psl, kEAX, flags);

        // mov             dword ptr [eax], ecx
        psl->X86EmitIndexRegStore(kEAX, 0, kECX);

        // mov             dword ptr [m_GCLock], 0FFFFFFFFh
        psl->Emit16(0x05C7);
        psl->Emit32((int)(size_t)&m_GCLock);
        psl->Emit32(0xFFFFFFFF);
    }


#ifdef INCREMENTAL_MEMCLR 
    _ASSERTE(!"NYI");
#endif // INCREMENTAL_MEMCLR
}

#if CHECK_APP_DOMAIN_LEAKS 
void JIT_TrialAlloc::EmitSetAppDomain(CPUSTUBLINKER *psl)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    if (!g_pConfig->AppDomainLeaks())
        return;

    // At both entry & exit, eax contains the allocated object.
    // ecx is preserved, edx is not.

    //
    // Add in a call to SetAppDomain.  (Note that this
    // probably would have been easier to implement by just not using
    // the generated helpers in a checked build, but we'd lose code
    // coverage that way.)
    //

    // Save ECX over function call
    psl->X86EmitPushReg(kECX);

    // mov object to ECX
    // mov ecx, eax
    psl->Emit16(0xc88b);

    // SetObjectAppDomain pops its arg & returns object in EAX
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) SetObjectAppDomain), 4);

    psl->X86EmitPopReg(kECX);
}

#endif // CHECK_APP_DOMAIN_LEAKS


void JIT_TrialAlloc::EmitNoAllocCode(CPUSTUBLINKER *psl, Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

#ifdef MAXALLOC 
    psl->X86EmitPushRegs((1<<kEAX)|(1<<kEDX)|(1<<kECX));
    // call            UndoAllocRequest
    psl->X86EmitCall(psl->NewExternalCodeLabel((LPVOID) UndoAllocRequest), 0);
    psl->X86EmitPopRegs((1<<kEAX)|(1<<kEDX)|(1<<kECX));
#endif // MAXALLOC
    if (flags & MP_ALLOCATOR)
    {
        if (flags & (ALIGN8|SIZE_IN_EAX))
            psl->X86EmitPopReg(kEBX);
    }
    else
    {
        // mov             dword ptr [m_GCLock], 0FFFFFFFFh
        psl->Emit16(0x05c7);
        psl->Emit32((int)(size_t)&m_GCLock);
        psl->Emit32(0xFFFFFFFF);
    }
}

void *JIT_TrialAlloc::GenAllocSFast(Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CPUSTUBLINKER sl;

    CodeLabel *noLock  = sl.NewCodeLabel();
    CodeLabel *noAlloc = sl.NewCodeLabel();
    CodeLabel *notRestored = NULL;
        
    if (flags & CHKRESTORE)
    {
        // CHKRESTORE & SIZE_IN_EAX are mutually exclusive.
        _ASSERTE((flags & SIZE_IN_EAX) == 0);

        notRestored = sl.NewCodeLabel();

        _ASSERTE(((MethodTableWriteableData::enum_flag_Unrestored & 0xffffff00) == 0) &&
                (offsetof(MethodTableWriteableData,m_dwFlags) == 0) &&
                (sizeof(MethodTableWriteableData::enum_flag_Unrestored) == sizeof(DWORD)));

        sl.X86EmitIndexRegLoad(kEAX, kECX, offsetof(MethodTable, m_pWriteableData));

        // test    byte ptr [eax],MethodTableWriteableData::enum_flag_Unrestored
        sl.Emit16(0x00f6);
        sl.Emit8(MethodTableWriteableData::enum_flag_Unrestored);

        // jnz             notRestored
        sl.X86EmitCondJump(notRestored, X86CondCode::kJNZ);
    }

    // Emit the main body of the trial allocator, be it SP or MP
    EmitCore(&sl, noLock, noAlloc, flags);

#if CHECK_APP_DOMAIN_LEAKS 
    EmitSetAppDomain(&sl);
#endif

    // Here we are at the end of the success case - just emit a ret
    sl.X86EmitReturn(0);

    // Come here in case of no space
    sl.EmitLabel(noAlloc);

    // Release the lock in the uniprocessor case
    EmitNoAllocCode(&sl, flags);

    // Come here in case of failure to get the lock
    sl.EmitLabel(noLock);

    // Jump to the framed helper
    sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) JIT_NewFast));

    if (flags & CHKRESTORE)
    {
        _ASSERTE(notRestored);

        // Come here in case the type isn't restored
        sl.EmitLabel(notRestored);

        // Jump to the framed helper
        sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) JIT_New));
    }

    Stub *pStub = sl.Link(SystemDomain::System()->GetHighFrequencyHeap());

    return (void *)pStub->GetEntryPoint();
}


void *JIT_TrialAlloc::GenBox(Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CPUSTUBLINKER sl;

    CodeLabel *noLock  = sl.NewCodeLabel();
    CodeLabel *noAlloc = sl.NewCodeLabel();

    // Save address of value to be boxed
    sl.X86EmitPushReg(kEBX);
    sl.Emit16(0xda8b);

    // Save the MethodTable ptr
    sl.X86EmitPushReg(kECX);

    // mov             ecx, [ecx]MethodTable.m_pWriteableData
    sl.X86EmitOffsetModRM(0x8b, kECX, kECX, offsetof(MethodTable, m_pWriteableData));

    // Check whether the class has not been initialized
    // test [ecx]MethodTableWriteableData.m_dwFlags,MethodTableWriteableData::enum_flag_Unrestored
    sl.X86EmitOffsetModRM(0xf7, (X86Reg)0x0, kECX, offsetof(MethodTableWriteableData, m_dwFlags));
    sl.Emit32(MethodTableWriteableData::enum_flag_Unrestored);

    // Restore the MethodTable ptr in ecx
    sl.X86EmitPopReg(kECX);

    // jne              noAlloc
    sl.X86EmitCondJump(noAlloc, X86CondCode::kJNE);

    // Emit the main body of the trial allocator
    EmitCore(&sl, noLock, noAlloc, flags);

#if CHECK_APP_DOMAIN_LEAKS 
    EmitSetAppDomain(&sl);
#endif

    // Here we are at the end of the success case

    // Check whether the object contains pointers
    // test [ecx]MethodTable.m_wFlags,MethodTable::enum_flag_ContainsPointers
    sl.X86EmitOffsetModRM(0xf7, (X86Reg)0x0, kECX, offsetof(MethodTable, m_wFlags));
    sl.Emit32(MethodTable::enum_flag_ContainsPointers);

    CodeLabel *pointerLabel = sl.NewCodeLabel();

    // jne              pointerLabel
    sl.X86EmitCondJump(pointerLabel, X86CondCode::kJNE);

    // We have no pointers - emit a simple inline copy loop

    // mov             ecx, [ecx]MethodTable.m_BaseSize
    sl.X86EmitOffsetModRM(0x8b, kECX, kECX, offsetof(MethodTable, m_BaseSize));

    // sub ecx,12
    sl.X86EmitSubReg(kECX, 12);

    CodeLabel *loopLabel = sl.NewCodeLabel();

    sl.EmitLabel(loopLabel);

    // mov edx,[ebx+ecx]
    sl.X86EmitOp(0x8b, kEDX, kEBX, 0, kECX, 1);

    // mov [eax+ecx+4],edx
    sl.X86EmitOp(0x89, kEDX, kEAX, 4, kECX, 1);

    // sub ecx,4
    sl.X86EmitSubReg(kECX, 4);

    // jg loopLabel
    sl.X86EmitCondJump(loopLabel, X86CondCode::kJGE);

    sl.X86EmitPopReg(kEBX);

    sl.X86EmitReturn(0);

    // Arrive at this label if there are pointers in the object
    sl.EmitLabel(pointerLabel);

    // Do call to CopyValueClassUnchecked(object, data, pMT)

    // Pass pMT (still in ECX)
    sl.X86EmitPushReg(kECX);

    // Pass data (still in EBX)
    sl.X86EmitPushReg(kEBX);

    // Save the address of the object just allocated
    // mov ebx,eax
    sl.Emit16(0xD88B);

    // Pass address of first user byte in the newly allocated object
    sl.X86EmitAddReg(kEAX, 4);
    sl.X86EmitPushReg(kEAX);

    // call CopyValueClass
    sl.X86EmitCall(sl.NewExternalCodeLabel((LPVOID) CopyValueClassUnchecked), 12);

    // Restore the address of the newly allocated object and return it.
    // mov eax,ebx
    sl.Emit16(0xC38B);

    sl.X86EmitPopReg(kEBX);

    sl.X86EmitReturn(0);

    // Come here in case of no space
    sl.EmitLabel(noAlloc);

    // Release the lock in the uniprocessor case
    EmitNoAllocCode(&sl, flags);

    // Come here in case of failure to get the lock
    sl.EmitLabel(noLock);

    // Restore the address of the value to be boxed
    // mov edx,ebx
    sl.Emit16(0xD38B);

    // pop ebx
    sl.X86EmitPopReg(kEBX);

    // Jump to the slow version of JIT_Box
    sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) JIT_Box));

    Stub *pStub = sl.Link(SystemDomain::System()->GetHighFrequencyHeap());

    return (void *)pStub->GetEntryPoint();
}


HCIMPL2_RAW(Object*, UnframedAllocateObjectArray, /*TypeHandle*/PVOID ElementType, DWORD cElements)
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    return OBJECTREFToObject( AllocateObjectArray(cElements, TypeHandle::FromPtr(ElementType), FALSE) );
HCIMPLEND_RAW


HCIMPL2_RAW(Object*, UnframedAllocatePrimitiveArray, CorElementType type, DWORD cElements)
    CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    return OBJECTREFToObject( AllocatePrimitiveArray(type, cElements, FALSE) );
HCIMPLEND_RAW


void *JIT_TrialAlloc::GenAllocArray(Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CPUSTUBLINKER sl;

    CodeLabel *noLock  = sl.NewCodeLabel();
    CodeLabel *noAlloc = sl.NewCodeLabel();

    // We were passed a type descriptor in ECX, which contains the (shared)
    // array method table and the element type.

    // If this is the allocator for use from unmanaged code, ECX contains the
    // element type descriptor, or the CorElementType.

    // We need to save ECX for later

    // push ecx
    sl.X86EmitPushReg(kECX);

    // The element count is in EDX - we need to save it for later.

    // push edx
    sl.X86EmitPushReg(kEDX);

    if (flags & NO_FRAME)
    {
        if (flags & OBJ_ARRAY)
        {
            // mov ecx, [g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT]]
            sl.Emit16(0x0d8b);
            sl.Emit32((int)(size_t)&g_pPredefinedArrayTypes[ELEMENT_TYPE_OBJECT]);
        }
        else
        {
            // mov ecx,[g_pPredefinedArrayTypes+ecx*4]
            sl.Emit8(0x8b);
            sl.Emit16(0x8d0c);
            sl.Emit32((int)(size_t)&g_pPredefinedArrayTypes);

            // test ecx,ecx
            sl.Emit16(0xc985);

            // je noLock
            sl.X86EmitCondJump(noLock, X86CondCode::kJZ);
        }

        // we need to load the true method table from the type desc
        sl.X86EmitIndexRegLoad(kECX, kECX, offsetof(ArrayTypeDesc,m_TemplateMT));
    }
    else
    {
        // we need to load the true method table from the type desc
        sl.X86EmitIndexRegLoad(kECX, kECX, offsetof(ArrayTypeDesc,m_TemplateMT)-2);
    }

    // Instead of doing elaborate overflow checks, we just limit the number of elements
    // to (LARGE_OBJECT_SIZE - 256)/LARGE_ELEMENT_SIZE or less. As the jit will not call
    // this fast helper for element sizes bigger than LARGE_ELEMENT_SIZE, this will
    // avoid avoid all overflow problems, as well as making sure big array objects are
    // correctly allocated in the big object heap.

    // cmp edx,(LARGE_OBJECT_SIZE - 256)/LARGE_ELEMENT_SIZE
    sl.Emit16(0xfa81);


        // The large object heap is 8 byte aligned, so for double arrays we
        // want to bias toward putting things in the large object heap
    unsigned maxElems =  (LARGE_OBJECT_SIZE - 256)/LARGE_ELEMENT_SIZE;

    if ((flags & ALIGN8) && g_pConfig->GetDoubleArrayToLargeObjectHeap() < maxElems)
        maxElems = g_pConfig->GetDoubleArrayToLargeObjectHeap();
    sl.Emit32(maxElems);


    // jae noLock - seems tempting to jump to noAlloc, but we haven't taken the lock yet
    sl.X86EmitCondJump(noLock, X86CondCode::kJAE);

    if (flags & OBJ_ARRAY)
    {
        // In this case we know the element size is sizeof(void *), or 4 for x86
        // This helps us in two ways - we can shift instead of multiplying, and
        // there's no need to align the size either

        _ASSERTE(sizeof(void *) == 4);

        // mov eax, [ecx]MethodTable.m_BaseSize
        sl.X86EmitIndexRegLoad(kEAX, kECX, offsetof(MethodTable, m_BaseSize));

        // lea eax, [eax+edx*4]
        sl.X86EmitOp(0x8d, kEAX, kEAX, 0, kEDX, 4);
    }
    else
    {
        // movzx eax, [ECX]MethodTable.m_wFlags /* component size */
        sl.Emit8(0x0f);
        sl.X86EmitOffsetModRM(0xb7, kEAX, kECX, offsetof(MethodTable, m_wFlags /* component size */));

        // mul eax, edx
        sl.Emit16(0xe2f7);

        // add eax, [ecx]MethodTable.m_BaseSize
        sl.X86EmitOffsetModRM(0x03, kEAX, kECX, offsetof(MethodTable, m_BaseSize));
    }

#if DATA_ALIGNMENT == 4 
    if (flags & OBJ_ARRAY)
    {
        // No need for rounding in this case - element size is 4, and m_BaseSize is guaranteed
        // to be a multiple of 4.
    }
    else
#endif // DATA_ALIGNMENT == 4
    {
        // round the size to a multiple of 4

        // add eax, 3
        sl.X86EmitAddReg(kEAX, (DATA_ALIGNMENT-1));

        // and eax, ~3
        sl.Emit16(0xe083);
        sl.Emit8(~(DATA_ALIGNMENT-1));
    }

    flags = (Flags)(flags | SIZE_IN_EAX);

    // Emit the main body of the trial allocator, be it SP or MP
    EmitCore(&sl, noLock, noAlloc, flags);

    // Here we are at the end of the success case - store element count
    // and possibly the element type descriptor and return

    // pop edx - element count
    sl.X86EmitPopReg(kEDX);

    // pop ecx - array type descriptor
    sl.X86EmitPopReg(kECX);

    // mov             dword ptr [eax]ArrayBase.m_NumComponents, edx
    sl.X86EmitIndexRegStore(kEAX, offsetof(ArrayBase,m_NumComponents), kEDX);

    if (flags & OBJ_ARRAY)
    {
        // need to store the element type descriptor

        if ((flags & NO_FRAME) == 0)
        {
            // mov ecx, [ecx]ArrayTypeDescriptor.m_Arg
            sl.X86EmitIndexRegLoad(kECX, kECX, offsetof(ArrayTypeDesc,m_Arg)-2);
        }

        // mov [eax]PtrArray.m_ElementType, ecx
        sl.X86EmitIndexRegStore(kEAX, offsetof(PtrArray,m_ElementType), kECX);
    }

#if CHECK_APP_DOMAIN_LEAKS 
    EmitSetAppDomain(&sl);
#endif

    // no stack parameters
    sl.X86EmitReturn(0);

    // Come here in case of no space
    sl.EmitLabel(noAlloc);

    // Release the lock in the uniprocessor case
    EmitNoAllocCode(&sl, flags);

    // Come here in case of failure to get the lock
    sl.EmitLabel(noLock);

    // pop edx - element count
    sl.X86EmitPopReg(kEDX);

    // pop ecx - array type descriptor
    sl.X86EmitPopReg(kECX);

    if (flags & NO_FRAME)
    {
        if (flags & OBJ_ARRAY)
        {
            // Jump to the unframed helper
            sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) UnframedAllocateObjectArray));
        }
        else
        {
            // Jump to the unframed helper
            sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) UnframedAllocatePrimitiveArray));
        }
    }
    else
    {
        // Jump to the framed helper
        sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) JIT_NewArr1));
    }

    Stub *pStub = sl.Link(SystemDomain::System()->GetHighFrequencyHeap());

    return (void *)pStub->GetEntryPoint();
}


void *JIT_TrialAlloc::GenAllocString(Flags flags)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CPUSTUBLINKER sl;

    CodeLabel *noLock  = sl.NewCodeLabel();
    CodeLabel *noAlloc = sl.NewCodeLabel();

    // We were passed the number of characters in ECX

    // push ecx
    sl.X86EmitPushReg(kECX);

    // mov eax, ecx
    sl.Emit16(0xc18b);

    // we need to load the method table for string from the global

    // mov ecx, [g_pStringMethodTable]
    sl.Emit16(0x0d8b);
    sl.Emit32((int)(size_t)&g_pStringClass);

    // Instead of doing elaborate overflow checks, we just limit the number of elements
    // to (LARGE_OBJECT_SIZE - 256)/sizeof(WCHAR) or less.
    // This will avoid avoid all overflow problems, as well as making sure
    // big string objects are correctly allocated in the big object heap.

    _ASSERTE(sizeof(WCHAR) == 2);

    // cmp edx,(LARGE_OBJECT_SIZE - 256)/sizeof(WCHAR)
    sl.Emit16(0xf881);
    sl.Emit32((LARGE_OBJECT_SIZE - 256)/sizeof(WCHAR));

    // jae noLock - seems tempting to jump to noAlloc, but we haven't taken the lock yet
    sl.X86EmitCondJump(noLock, X86CondCode::kJAE);

    // mov edx, [ecx]MethodTable.m_BaseSize
    sl.X86EmitIndexRegLoad(kEDX, kECX, offsetof(MethodTable,m_BaseSize));

    // Calculate the final size to allocate.
    // We need to calculate baseSize + cnt*2, then round that up by adding 3 and anding ~3.

    // lea eax, [edx+eax*2+5]
    sl.X86EmitOp(0x8d, kEAX, kEDX, 2 + (DATA_ALIGNMENT-1), kEAX, 2);

    // and eax, ~3
    sl.Emit16(0xe083);
    sl.Emit8(~(DATA_ALIGNMENT-1));

    flags = (Flags)(flags | SIZE_IN_EAX);

    // Emit the main body of the trial allocator, be it SP or MP
    EmitCore(&sl, noLock, noAlloc, flags);

    // Here we are at the end of the success case - store element count
    // and possibly the element type descriptor and return

    // pop ecx - element count
    sl.X86EmitPopReg(kECX);

    // mov             dword ptr [eax]ArrayBase.m_StringLength, ecx
    sl.X86EmitIndexRegStore(kEAX, offsetof(StringObject,m_StringLength), kECX);

    // inc ecx
    sl.Emit8(0x41);

    // mov             dword ptr [eax]ArrayBase.m_ArrayLength, ecx
    sl.X86EmitIndexRegStore(kEAX, offsetof(StringObject,m_ArrayLength), kECX);

#if CHECK_APP_DOMAIN_LEAKS 
    EmitSetAppDomain(&sl);
#endif

    // no stack parameters
    sl.X86EmitReturn(0);

    // Come here in case of no space
    sl.EmitLabel(noAlloc);

    // Release the lock in the uniprocessor case
    EmitNoAllocCode(&sl, flags);

    // Come here in case of failure to get the lock
    sl.EmitLabel(noLock);

    // pop ecx - element count
    sl.X86EmitPopReg(kECX);

    if (flags & NO_FRAME)
    {
        // Jump to the unframed helper
        sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) UnframedAllocateString));
    }
    else
    {
        // Jump to the framed helper
        sl.X86EmitNearJump(sl.NewExternalCodeLabel((LPVOID) FramedAllocateString));
    }

    Stub *pStub = sl.Link(SystemDomain::System()->GetHighFrequencyHeap());

    return (void *)pStub->GetEntryPoint();
}


FastStringAllocatorFuncPtr fastStringAllocator = UnframedAllocateString;

FastObjectArrayAllocatorFuncPtr fastObjectArrayAllocator = UnframedAllocateObjectArray;

FastPrimitiveArrayAllocatorFuncPtr fastPrimitiveArrayAllocator = UnframedAllocatePrimitiveArray;

// For this helper,
// If bCCtorCheck == true
//          ECX contains the domain neutral module ID
//          EDX contains the class domain ID, and the
// else
//          ECX contains the domain neutral module ID
//          EDX is junk
// shared static base is returned in EAX.

// "init" should be the address of a routine which takes an argument of
// the module domain ID, the class domain ID, and returns the static base pointer
void EmitFastGetSharedStaticBase(CPUSTUBLINKER *psl, CodeLabel *init, bool bCCtorCheck, bool bGCStatic)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CodeLabel *DoInit = 0;
    if (bCCtorCheck)
    {
        DoInit = psl->NewCodeLabel();
    }

    // Check tag
    CodeLabel *cctorCheck = psl->NewCodeLabel();

    // mov eax, ecx
    psl->Emit8(0x89);
    psl->Emit8(0xc8);

    // test eax, 1
    psl->Emit8(0xa9);
    psl->Emit32(1);

    // jz cctorCheck
    psl->X86EmitCondJump(cctorCheck, X86CondCode::kJZ);

    // mov eax GetAppDomain()
    psl->X86EmitTLSFetch(GetAppDomainTLSIndex(), kEAX, (1<<kECX)|(1<<kEDX));

    // mov eax [eax->m_sDomainLocalBlock.m_pModuleSlots]
    psl->X86EmitIndexRegLoad(kEAX, kEAX, (__int32) AppDomain::GetOffsetOfModuleSlotsPointer());

    // Note: weird address arithmetic effectively does:
    // shift over 1 to remove tag bit (which is always 1), then multiply by 4.
    // mov eax [eax + ecx*2 - 2]
    psl->X86EmitOp(0x8b, kEAX, kEAX, -2, kECX, 2);

    // cctorCheck:
    psl->EmitLabel(cctorCheck);


    if (bCCtorCheck)
    {
        // test [eax + edx + offsetof(DomainLocalModule, m_pDataBlob], INITIALIZED_FLAG       // Is class inited
        _ASSERTE(FitsInI1(DomainLocalModule::INITIALIZED_FLAG));
        _ASSERTE(FitsInI1(DomainLocalModule::GetOffsetOfDataBlob()));

        BYTE testClassInit[] = { 0xF6, 0x44, 0x10,
            (BYTE) DomainLocalModule::GetOffsetOfDataBlob(), (BYTE)DomainLocalModule::INITIALIZED_FLAG };

        psl->EmitBytes(testClassInit, sizeof(testClassInit));

        // jz  init                                    // no, init it
        psl->X86EmitCondJump(DoInit, X86CondCode::kJZ);
    }

    if (bGCStatic)
    {
        // Indirect to get the pointer to the first GC Static
        psl->X86EmitIndexRegLoad(kEAX, kEAX, (__int32) DomainLocalModule::GetOffsetOfGCStaticPointer());
    }

    // ret
    psl->X86EmitReturn(0);

    if (bCCtorCheck)
    {
        // DoInit:
        psl->EmitLabel(DoInit);

        // push edx (must be preserved)
        psl->X86EmitPushReg(kEDX);

        // call init
        psl->X86EmitCall(init, 0);

        // pop edx
        psl->X86EmitPopReg(kEDX);

        // ret
        psl->X86EmitReturn(0);
    }

}

void *GenFastGetSharedStaticBase(bool bCheckCCtor, bool bGCStatic)
{
    CONTRACTL {
        THROWS;
        GC_NOTRIGGER;
    } CONTRACTL_END;

    CPUSTUBLINKER sl;

    CodeLabel *init;
    if (bGCStatic)
    {
        init = sl.NewExternalCodeLabel((LPVOID) JIT_GetSharedGCStaticBase);
    }
    else
    {
        init = sl.NewExternalCodeLabel((LPVOID) JIT_GetSharedNonGCStaticBase);
    }

    EmitFastGetSharedStaticBase(&sl, init, bCheckCCtor, bGCStatic);

    Stub *pStub = sl.Link(SystemDomain::System()->GetHighFrequencyHeap());

    return (void*) pStub->GetEntryPoint();
}


extern "C" void __stdcall JIT_MonStart();
extern "C" void __stdcall JIT_MonLast();

inline void PatchLockPrefix(BYTE* pFunction, int offset)
{
    BYTE* pLockPrefix = pFunction + offset;

    _ASSERTE(*pLockPrefix == 0x90
        && "Initialization failure while stomping instructions for multi-processor machines: the instruction at the given offset was not a compare-exchange instruction");

    _ASSERTE((BYTE*)JIT_MonStart <= pLockPrefix && pLockPrefix < (BYTE*)JIT_MonLast
        && "Initialization failure while stomping instructions for multi-processor machines: the low/high address calculation for ClrVirtualProtect did not correctly capture the loation we are stomping");

    *pLockPrefix = 0xF0;
}

/*********************************************************************/
// Initialize the part of the JIT helpers that require very little of
// EE infrastructure to be in place.
/*********************************************************************/
void InitJITHelpers1()
{
     CONTRACTL {
        THROWS;
        GC_TRIGGERS;
    } CONTRACTL_END;

    // Init GetThread function
    _ASSERTE(GetThread != NULL);
    SetJitHelperFunction(CORINFO_HELP_GET_THREAD, (void *) GetThread);

    _ASSERTE(g_SystemInfo.dwNumberOfProcessors != 0);

    // All monitor helpers should fit into one page.
    // If you hit this assert on retail build, there is most likely problem with BBT script.
    _ASSERTE_ALL_BUILDS((BYTE*)JIT_MonLast - (BYTE*)JIT_MonStart < PAGE_SIZE);

    // Handle the case that we are on an MP machine.
    if (g_SystemInfo.dwNumberOfProcessors != 1)
    {
        // If we are on a multiproc machine stomp some nop's with lock prefix's
        DWORD   oldProt;

        // I am using virtual protect to cover the entire range that this code falls in.

        if (!ClrVirtualProtect((void *)JIT_MonStart, (BYTE*)JIT_MonLast - (BYTE*)JIT_MonStart,
                               PAGE_EXECUTE_READWRITE, &oldProt))
        {
            _ASSERTE(!"ClrVirtualProtect of code page failed");
            COMPlusThrowWin32();
        }

        // ***** NOTE: you must ensure that both the checked and free versions work if you
        // make any changes here. Do this by undefining MON_DEBUG.
        //
        // ***** NOTE: if you get an AV at one of these points it is due to the fact that the page-protection
        // hasn't been correctly modified above.

        PatchLockPrefix((BYTE*)JIT_MonEnterWorker, 0x3f);
        PatchLockPrefix((BYTE*)JIT_MonEnterWorker, 0x7a);
        PatchLockPrefix((BYTE*)JIT_MonEnterWorker, 0xd6);
        PatchLockPrefix((BYTE*)JIT_MonTryEnter, 0x49);
        PatchLockPrefix((BYTE*)JIT_MonTryEnter, 0x8c);
        PatchLockPrefix((BYTE*)JIT_MonTryEnter, 0xf1);
        PatchLockPrefix((BYTE*)JIT_MonExitWorker, 0x2d);
        PatchLockPrefix((BYTE*)JIT_MonExitWorker, 0x40);
        PatchLockPrefix((BYTE*)JIT_MonExitWorker, 0x87);
        PatchLockPrefix((BYTE*)JIT_MonEnterStatic, 0x0c);
        PatchLockPrefix((BYTE*)JIT_MonExitStatic, 0x21);

        if (!ClrVirtualProtect((void *)JIT_MonStart, (BYTE*)JIT_MonLast - (BYTE*)JIT_MonStart,
                               oldProt, &oldProt))
        {
            _ASSERTE(!"ClrVirtualProtect of code page failed");
            COMPlusThrowWin32();
        }
    }


    JIT_TrialAlloc::Flags flags = GCHeap::UseAllocationContexts() ?
        JIT_TrialAlloc::MP_ALLOCATOR : JIT_TrialAlloc::NORMAL;


    if (!(TrackAllocationsEnabled() || CORProfilerTrackGC() || LoggingOn(LF_GCALLOC, LL_INFO10)
#ifdef _DEBUG 
        || (g_pConfig->ShouldInjectFault(INJECTFAULT_GCHEAP) != 0)
#endif
         )
        )
    {
        // This is the NO logging case.

        // Replace the slow helpers with faster version
        SetJitHelperFunction(CORINFO_HELP_NEWSFAST,
            JIT_TrialAlloc::GenAllocSFast(flags));
        SetJitHelperFunction(CORINFO_HELP_NEWSFAST_ALIGN8,
            JIT_TrialAlloc::GenAllocSFast((JIT_TrialAlloc::Flags)(flags|JIT_TrialAlloc::ALIGN8 | JIT_TrialAlloc::ALIGN8OBJ)));
        SetJitHelperFunction(CORINFO_HELP_NEWSFAST_CHKRESTORE,
            JIT_TrialAlloc::GenAllocSFast((JIT_TrialAlloc::Flags)(flags|JIT_TrialAlloc::CHKRESTORE)));
        SetJitHelperFunction(CORINFO_HELP_BOX,
            JIT_TrialAlloc::GenBox(flags));
        SetJitHelperFunction(CORINFO_HELP_NEWARR_1_OBJ,
            JIT_TrialAlloc::GenAllocArray((JIT_TrialAlloc::Flags)(flags|JIT_TrialAlloc::OBJ_ARRAY)));
        SetJitHelperFunction(CORINFO_HELP_NEWARR_1_VC,
            JIT_TrialAlloc::GenAllocArray(flags));
        SetJitHelperFunction(CORINFO_HELP_NEWARR_1_ALIGN8,
            JIT_TrialAlloc::GenAllocArray((JIT_TrialAlloc::Flags)(flags|JIT_TrialAlloc::ALIGN8)));

        fastObjectArrayAllocator = (FastObjectArrayAllocatorFuncPtr)JIT_TrialAlloc::GenAllocArray((JIT_TrialAlloc::Flags)(flags|JIT_TrialAlloc::NO_FRAME|JIT_TrialAlloc::OBJ_ARRAY));
        fastPrimitiveArrayAllocator = (FastPrimitiveArrayAllocatorFuncPtr)JIT_TrialAlloc::GenAllocArray((JIT_TrialAlloc::Flags)(flags|JIT_TrialAlloc::NO_FRAME));

        ECall::DynamicallyAssignFCallImpl((LPVOID) JIT_TrialAlloc::GenAllocString(flags), ECall::FastAllocateString);

        // generate another allocator for use from unmanaged code (won't need a frame)
        fastStringAllocator = (FastStringAllocatorFuncPtr) JIT_TrialAlloc::GenAllocString((JIT_TrialAlloc::Flags)(flags|JIT_TrialAlloc::NO_FRAME));
        //UnframedAllocateString;
    }

    // Replace static helpers with faster assembly versions
    // This needs to happen outside of FEATURE_PAL because the mainline JIT does not
    // actually pass the classID in the second argument (edx) when there is no
    // .cctor the assembly version take this into account, but the non-assembly
    // versions do not.
    SetJitHelperFunction(CORINFO_HELP_GETSHARED_GCSTATIC_BASE,
        GenFastGetSharedStaticBase(true, true));
    SetJitHelperFunction(CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE,
        GenFastGetSharedStaticBase(true, false));
    SetJitHelperFunction(CORINFO_HELP_GETSHARED_GCSTATIC_BASE_NOCTOR,
        GenFastGetSharedStaticBase(false, true));
    SetJitHelperFunction(CORINFO_HELP_GETSHARED_NONGCSTATIC_BASE_NOCTOR,
        GenFastGetSharedStaticBase(false, false));

    // All write barrier helpers should fit into one page.
    // If you hit this assert on retail build, there is most likely problem with BBT script.
    _ASSERTE_ALL_BUILDS((BYTE*)JIT_WriteBarrierLast - (BYTE*)JIT_WriteBarrierStart < PAGE_SIZE);

    // Make sure we do this last because when we revert the virtual protection above, we could 
    // inadvertently revert the virtual protection for this piece of memory as well.
    // Unprotect the memory range with writeable thunks
    _ASSERTE(DbgIsExecutableVM((void*)&JIT_Writeable_Thunks_Buf, sizeof(JIT_Writeable_Thunks_Buf)));
    DWORD oldProtect;
    if (!ClrVirtualProtect((void*)&JIT_Writeable_Thunks_Buf,
                           sizeof(JIT_Writeable_Thunks_Buf), PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        _ASSERTE(!"ClrVirtualProtect of code page failed");
        COMPlusThrowWin32();
    }

#ifdef _DEBUG
    memset((void*)&JIT_Writeable_Thunks_Buf, X86_INSTR_INT3, sizeof(JIT_Writeable_Thunks_Buf));
#endif

    // Initialize the variable at runtime to prevent compiler from optimizing out access 
    // to const JIT_Writeable_Thunks_Buf
    JIT_WriteBarrierReg_Buf = (JIT_WriteBarrierReg_Thunk*)JIT_Writeable_Thunks_Buf.WriteBarrierReg;

    JIT_WriteBarrierReg_Thunk* writeBarrierBuffer = JIT_WriteBarrierReg_Buf;

    // Copy the write barriers to their final resting place.
    // Note: I use a pfunc temporary here to avoid a WinCE internal compiler error
    for (int reg = 0; reg < 8; reg++)
    {
        BYTE * pfunc = (BYTE *) JIT_WriteBarrierReg_PreGrow;
        memcpy(&writeBarrierBuffer[reg], pfunc, 31);

        // assert the copied code ends in a ret to make sure we got the right length
        _ASSERTE(writeBarrierBuffer[reg].Bytes[30] == 0xC3);

        // We need to adjust registers in a couple of instructions
        // It would be nice to have the template contain all zeroes for
        // the register fields (corresponding to EAX), but that doesn't
        // work because then we get a smaller encoding for the compares
        // that only works for EAX but not the other registers.
        // So we always have to clear the register fields before updating them.

        // First instruction to patch is a mov [edx], reg

        _ASSERTE(writeBarrierBuffer[reg].Bytes[0] == 0x89);
        // Update the reg field (bits 3..5) of the ModR/M byte of this instruction
        writeBarrierBuffer[reg].Bytes[1] &= 0xc7;
        writeBarrierBuffer[reg].Bytes[1] |= reg << 3;

        // Second instruction to patch is cmp reg, imm32 (low bound)

        _ASSERTE(writeBarrierBuffer[reg].Bytes[2] == 0x81);
        // Here the lowest three bits in ModR/M field are the register
        writeBarrierBuffer[reg].Bytes[3] &= 0xf8;
        writeBarrierBuffer[reg].Bytes[3] |= reg;

    }

    // Leave the write barrier buffer writable for StompWriteBarrierEphemeral() and StompWriteBarrierResize()
}

/*********************************************************************/


#define WriteBarrierIsPreGrow() (JIT_WriteBarrierReg_Buf[0].Bytes[10] == 0xc1)


/*********************************************************************/
// When a GC happens, the upper and lower bounds of the ephemeral
// generation change.  This routine updates the WriteBarrier thunks
// with the new values.
void StompWriteBarrierEphemeral()
{
    CONTRACTL {
        NOTHROW;
        GC_NOTRIGGER;
    } CONTRACTL_END;


    BOOL flushICache = FALSE;
    JIT_WriteBarrierReg_Thunk* writeBarrierBuffer = JIT_WriteBarrierReg_Buf;

    // Update the lower bound.
    for (int reg = 0; reg < 8; reg++)
    {
        // assert there is in fact a cmp r/m32, imm32 there
        _ASSERTE(writeBarrierBuffer[reg].Bytes[2] == 0x81);

        // Update the immediate which is the lower bound of the ephemeral generation
        size_t *pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[4];
        //avoid trivial self modifying code
        if (*pfunc != (size_t) g_ephemeral_low)
        {
            flushICache = TRUE;
            *pfunc = (size_t) g_ephemeral_low;
        }
        if (!WriteBarrierIsPreGrow())
        {
            // assert there is in fact a cmp r/m32, imm32 there
            _ASSERTE(writeBarrierBuffer[reg].Bytes[10] == 0x81);

                // Update the upper bound if we are using the PostGrow thunk.
            pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[12];
            //avoid trivial self modifying code
            if (*pfunc != (size_t) g_ephemeral_high)
            {
                flushICache = TRUE;
                *pfunc = (size_t) g_ephemeral_high;
            }
        }
    }
    if (flushICache)
      FlushInstructionCache(GetCurrentProcess(),JIT_WriteBarrierReg_Buf,8*sizeof(JIT_WriteBarrierReg_Thunk));

}

/*********************************************************************/
// When the GC heap grows, the ephemeral generation may no longer
// be after the older generations.  If this happens, we need to switch
// to the PostGrow thunk that checks both upper and lower bounds.
// regardless we need to update the thunk with the
// card_table - lowest_address.
void StompWriteBarrierResize(BOOL bReqUpperBoundsCheck)
{
    CONTRACTL {
        NOTHROW;
        if (GetThread()) {GC_TRIGGERS;} else {GC_NOTRIGGER;}
    } CONTRACTL_END;



    bool bWriteBarrierIsPreGrow = WriteBarrierIsPreGrow();
    bool bStompWriteBarrierEphemeral = false;

    JIT_WriteBarrierReg_Thunk* writeBarrierBuffer = JIT_WriteBarrierReg_Buf;

    BOOL bEESuspended = FALSE;

    for (int reg = 0; reg < 8; reg++)
    {
        size_t *pfunc;

    // Check if we are still using the pre-grow version of the write barrier.
        if (bWriteBarrierIsPreGrow)
        {
            // Check if we need to use the upper bounds checking barrier stub.
            if (bReqUpperBoundsCheck)
            {
                GCX_MAYBE_COOP_NO_THREAD_BROKEN((GetThread()!=NULL));
                if( !IsGCThread() && !bEESuspended) {
                    GCHeap::GetGCHeap()->SuspendEE(GCHeap::SUSPEND_FOR_GC_PREP);
                    bEESuspended = TRUE;
                }

                // Note: I use a pfunc temporary to avoid a WinCE internal compiler error
                pfunc = (size_t *) JIT_WriteBarrierReg_PostGrow;
                memcpy(&writeBarrierBuffer[reg], pfunc, 39);

                // assert the copied code ends in a ret to make sure we got the right length
                _ASSERTE(writeBarrierBuffer[reg].Bytes[38] == 0xC3);

                // We need to adjust registers in a couple of instructions
                // It would be nice to have the template contain all zeroes for
                // the register fields (corresponding to EAX), but that doesn't
                // work because then we get a smaller encoding for the compares
                // that only works for EAX but not the other registers
                // So we always have to clear the register fields before updating them.

                // First instruction to patch is a mov [edx], reg

                _ASSERTE(writeBarrierBuffer[reg].Bytes[0] == 0x89);
                // Update the reg field (bits 3..5) of the ModR/M byte of this instruction
                writeBarrierBuffer[reg].Bytes[1] &= 0xc7;
                writeBarrierBuffer[reg].Bytes[1] |= reg << 3;

                // Second instruction to patch is cmp reg, imm32 (low bound)

                _ASSERTE(writeBarrierBuffer[reg].Bytes[2] == 0x81);
                // Here the lowest three bits in ModR/M field are the register
                writeBarrierBuffer[reg].Bytes[3] &= 0xf8;
                writeBarrierBuffer[reg].Bytes[3] |= reg;

                // Third instruction to patch is another cmp reg, imm32 (high bound)

                _ASSERTE(writeBarrierBuffer[reg].Bytes[10] == 0x81);
                // Here the lowest three bits in ModR/M field are the register
                writeBarrierBuffer[reg].Bytes[11] &= 0xf8;
                writeBarrierBuffer[reg].Bytes[11] |= reg;

                bStompWriteBarrierEphemeral = true;
                // What we're trying to update is the offset field of a

                // cmp offset[edx], 0ffh instruction
                _ASSERTE(writeBarrierBuffer[reg].Bytes[21] == 0x80);
                pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[23];
               *pfunc = (size_t) g_card_table;

                // What we're trying to update is the offset field of a
                // mov offset[edx], 0ffh instruction
                _ASSERTE(writeBarrierBuffer[reg].Bytes[31] == 0xC6);
                pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[33];

            }
            else
            {
                // What we're trying to update is the offset field of a

                // cmp offset[edx], 0ffh instruction
                _ASSERTE(writeBarrierBuffer[reg].Bytes[13] == 0x80);
                pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[15];
               *pfunc = (size_t) g_card_table;

                // What we're trying to update is the offset field of a

                // mov offset[edx], 0ffh instruction
                _ASSERTE(writeBarrierBuffer[reg].Bytes[23] == 0xC6);
                pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[25];
            }
        }
        else
        {
            // What we're trying to update is the offset field of a

            // cmp offset[edx], 0ffh instruction
            _ASSERTE(writeBarrierBuffer[reg].Bytes[21] == 0x80);
            pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[23];
           *pfunc = (size_t) g_card_table;

            // What we're trying to update is the offset field of a
            // mov offset[edx], 0ffh instruction
            _ASSERTE(writeBarrierBuffer[reg].Bytes[31] == 0xC6);
            pfunc = (size_t *) &writeBarrierBuffer[reg].Bytes[33];
        }

        // Stick in the adjustment value.
        *pfunc = (size_t) g_card_table;
    }

    if (bStompWriteBarrierEphemeral)
        StompWriteBarrierEphemeral();
    else
        FlushInstructionCache(GetCurrentProcess(),JIT_WriteBarrierReg_Buf,8*sizeof(JIT_WriteBarrierReg_Thunk));

    if(bEESuspended)
        GCHeap::GetGCHeap()->RestartEE(FALSE, TRUE);
}
