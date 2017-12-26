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
/**************************************************************/
/*                       gmscpu.h                             */
/**************************************************************/
/* HelperFrame is defines 'GET_STATE(machState)' macro, which 
   figures out what the state of the machine will be when the 
   current method returns.  It then stores the state in the
   JIT_machState structure.  */

/**************************************************************/

#ifndef __gmsppc_h__
#define __gmsppc_h__

#ifdef _DEBUG
class HelperMethodFrame;
struct MachState;
EXTERN_C MachState* __stdcall 
HelperMethodFrameConfirmState(HelperMethodFrame* frame, 
                              void* rVal[NUM_CALLEESAVED_REGISTERS]);
#endif

    // A MachState indicates the register state of the processor at some point in time (usually
    // just before or after a call is made).  It can be made one of two ways.  Either explicitly
    // (when you for some reason know the values of all the registers), or implicitly using the
    // GET_STATE macros.  

typedef DPTR(struct MachState) PTR_MachState;
struct MachState {
    // Create a machine state explicitly
    MachState(PREGDISPLAY pRD, void ** RetAddr)
    {
        WRAPPER_CONTRACT;
        Init( (TADDR**)pRD->pR, (TADDR)pRD->SP, (TADDR*)RetAddr );
    }

    MachState(void **prVal[NUM_CALLEESAVED_REGISTERS], void* aSp, void** aPRetAddr)
    {
        WRAPPER_CONTRACT;
        Init( (TADDR**)prVal, (TADDR)aSp, (TADDR*)aPRetAddr);
    }

    MachState()
    {
        CONTRACTL {
            NOTHROW;
            GC_NOTRIGGER;
        } CONTRACTL_END;

        INDEBUG(memset(this, 0xCC, sizeof(MachState));)
    }

    void Init(TADDR** prVal, TADDR aSp, TADDR* aPRetAddr);

    typedef void* (*TestFtn)(void*);
    bool   isValid() {
        LEAF_CONTRACT;
        _ASSERTE(PTR_TO_TADDR(_pRetAddr) != INVALID_POINTER_CC);
        return(_pRetAddr != 0);
    }
    TADDR* pPreservedReg(int RegNum) {
        LEAF_CONTRACT;
        _ASSERTE(RegNum >= 0 && RegNum <= NUM_CALLEESAVED_REGISTERS);
        _ASSERTE(PTR_TO_TADDR(_pRegs[RegNum]) != INVALID_POINTER_CC); 
        return(_pRegs[RegNum]);
    }
    TADDR  sp()         { LEAF_CONTRACT; _ASSERTE(isValid()); return(_sp); }
    TADDR* pRetAddr()   { LEAF_CONTRACT; _ASSERTE(isValid()); return(_pRetAddr); }
    TADDR  GetRetAddr() { _ASSERTE(isValid()); return *PTR_TADDR(PTR_TO_TADDR(_pRetAddr)); }
#ifndef DACCESS_COMPILE
    void SetRetAddr(TADDR* addr) { LEAF_CONTRACT; _ASSERTE(isValid()); _pRetAddr = addr; }
#endif

    friend class HelperMethodFrame;
    friend class CheckAsmOffsets;
    friend struct LazyMachState;
#ifdef _DEBUG
    friend MachState* __stdcall HelperMethodFrameConfirmState(
         HelperMethodFrame* frame, 
         void* rVal[NUM_CALLEESAVED_REGISTERS]);
#endif


protected:
    // The state of all the callee saved registers.
    // If the register has been spill to the stack _pRegs[reg]
    // points at this location, otherwise it points
    // at the field _Regs[<REG>] field itself 
    TADDR       _Regs[NUM_CALLEESAVED_REGISTERS];
    PTR_TADDR   _pRegs[NUM_CALLEESAVED_REGISTERS];

    DWORD _cr;          // condition register

    TADDR     _sp;      // stack pointer after the function returns
    PTR_TADDR _pRetAddr;  // The address of the stored IP address (points to the stack)
};

/********************************************************************/
/* This allows you to defer the computation of the Machine state 
   until later.  Note that we don't reuse slots, because we want
   this to be threadsafe without locks */

typedef DPTR(LazyMachState) PTR_LazyMachState;
struct LazyMachState : public MachState {
    // compute the machine state of the processor as it will exist just 
    // after the return after at most'funCallDepth' number of functions.
    // if 'testFtn' is non-NULL, the return address is tested at each
    // return instruction encountered.  If this test returns non-NULL,
    // then stack walking stops (thus you can walk up to the point that the
    // return address matches some criteria

    // Normally this is called with funCallDepth=1 and testFtn = 0 so that 
    // it returns the state of the processor after the function that called 'captureState()'
    void setLazyStateFromUnwind(MachState* copy, TADDR retAddr);
    static TADDR unwindLazyState(LazyMachState* baseState,
                                 MachState* lazyState,
                                 int funCallDepth = 1,
                                 TestFtn testFtn = 0);

    friend class HelperMethodFrame;
    friend class CheckAsmOffsets;
private:
    TADDR               captureSp;    // Stack ptr at time of capture
    TADDR               captureSp2;   // One more stack ptr from the chain - this 
                                      // is to handle alloca
    TADDR               capturePC;    // PC at the time of capture
};

inline void LazyMachState::setLazyStateFromUnwind(MachState* copy,
                                                  TADDR retAddr)
{
    // _pRetAddr has to be the last thing updated when we make the copy (because its
    // is the the _pRetAddr becoming non-zero that flips this from invalid to valid.
    // we assert that it is the last field in the struct.
    C_ASSERT(offsetof(MachState, _pRetAddr) + sizeof(_pRetAddr) == sizeof(MachState));

    memcpy(this, copy, offsetof(MachState, _pRetAddr));

    // this has to be last
    *(TADDR volatile *)&_pRetAddr = PTR_TO_TADDR(copy->_pRetAddr);
}

// Do the initial capture of the machine state.  This is meant to be 
// as light weight as possible, as we may never need the state that 
// we capture.  Thus to complete the process you need to call 
// 'getMachState()', which finishes the process
EXTERN_C int __fastcall LazyMachStateCaptureState(struct LazyMachState *pState);

// CAPTURE_STATE captures just enough register state so that the state of the
// processor can be deterined just after the the routine that has CAPTURE_STATE in
// it returns.
// Note that the return is never taken, is is there for epilog walking
#define CAPTURE_STATE(machState, ret)                       \
    if (LazyMachStateCaptureState(&machState)) ret

#endif
