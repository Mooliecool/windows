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

#ifdef _MSC_VER
#pragma once
#endif

#include "vtypeinfo.h"
#include "corerror.h"

class LocalDescr
{
public:
    CorInfoType type;
    CORINFO_CLASS_HANDLE hclass;
};

class VerificationState
{
public:
    bool        blockIsBad;
    bool        *argsInitialized;

    mdToken     delegateMethodRef;     

    UINT        constrainedPrefix:1;
    UINT        readonlyPrefix:1;
    UINT        tailPrefix:1;
    UINT        volatilePrefix:1;
    UINT        unalignedPrefix:1;
    UINT        tailInBlock:1;
    // say we initialize the 'this' pointer in a try.
    // Real successors of the block can assume 'this' is inited.
    // nominal successors cannot.  However if we know 'this' was inited in 
    UINT        thisInitializedThisBlock:1;
    UINT        strongThisInitialized:1;
    const BYTE  *delegateCreateStart;

private:
    UINT        thisInitialized:1;
    vertype*    vstack;
    unsigned    vsp;
    unsigned    maxStack;
    
    VerificationState(){}

public:

    ReaderBase* base;

    inline void setStack(vertype *stackMem);
    
    inline void Init(unsigned maxstack, unsigned numLocals, bool fInitLocals, bool fThisInited);

    inline void print();

    inline void push(vertype typ);

    inline vertype pop();

    inline vertype impStackTop(unsigned n=0);

    // pop an objref which might be an uninitialized 'this' ptr
    // DANGER : do not use except in accordance with Partion 3 1.8.1.4
    //   No operations can be performed on an uninitialized 'this'
    //   except for storing into and loading from the object's fields.
    inline vertype popPossiblyUninit();

    inline unsigned stackLevel() { return vsp; }

    bool isThisInitialized() { return thisInitialized ? true : false; }
    void setThisInitialized() { thisInitialized = 1; }

};


void VerificationState::setStack(vertype *stackMem)
{
    vstack = stackMem;
}
    
void VerificationState::Init(unsigned maxstack, unsigned numLocals, bool fInitLocals, bool fThisInitialized)
{
    vsp = 0;
    maxStack = maxstack;
    delegateMethodRef = mdTokenNil;

    blockIsBad      = false;
    constrainedPrefix = false;
    readonlyPrefix = false;
    tailPrefix      = false;
    volatilePrefix  = false;
    unalignedPrefix = false;
    tailInBlock     = false;
    delegateCreateStart = NULL;
    thisInitializedThisBlock = false;
    
    thisInitialized = fThisInitialized;
    strongThisInitialized = false;

    for (unsigned i=0; i<numLocals; i++)
    {
        argsInitialized[i] = fInitLocals;
    }
}
    
vertype VerificationState::pop() 
{
    vertype ret = popPossiblyUninit();
    base->VerifyAndReportFound((!ret.IsObjRef()) || (!ret.IsUninitialisedObjRef()), ret, MVER_E_STACK_UNINIT);
    return ret;
}

    
// DANGER : do not use except in accordance with Partion 3 1.8.1.4
//   No operations can be performed on an uninitialized 'this'
//   except for storing into and loading from the object's fields.
vertype VerificationState::popPossiblyUninit()
{
    base->GVerifyOrReturn(vsp > 0, MVER_E_STACK_UNDERFLOW);

    vsp--;
    vertype result = vstack[vsp];

    // blank out the thing we just popped
#ifndef NDEBUG
    memset(vstack+vsp, 0xcd, sizeof(vertype));
#endif

    return result;
}
    
void VerificationState::push(vertype typ) 
{
    base->GVerifyOrReturn(vsp <= maxStack-1, MVER_E_STACK_OVERFLOW);
    vstack[vsp] = typ;
    vsp++;
}

vertype VerificationState::impStackTop(unsigned n) 
{
    base->GVerifyOrReturn(vsp > n, MVER_E_STACK_UNDERFLOW);
        
    return vstack[vsp-n-1];
}

#ifdef DEBUG
void VerificationState::print()
{
    int i;
    printf("--verification stack---\n");
    for (i = vsp-1; i>=0; i--)
    {
        printf("%d : ", i);
        vstack[i].Dump();
        printf("\n");
    }
}
#endif




// =========================================================================
// ================ Exceptions
// =========================================================================

class CallAuthorizationException : public ReaderException
{
};

class VerificationException : public ReaderException
{
public:

	DWORD   dwFlags;            // VER_ERR_XXX

    union {
        ReaderBaseNS::OPCODE opcode;
        unsigned long padding1; // to match with idl generated struct size
    };

    union {
        DWORD   dwOffset;       // #of bytes from start of method
        long    uOffset;        // for backward compat with Metadata validator
    };

    union {
        mdToken         token;
        mdToken         Token;  // for backward compat with metadata validator
        BYTE	        bCallConv;
        CorElementType  elem;
        DWORD           dwStackSlot; // positon in the Stack
        unsigned long   padding2;    // to match with idl generated struct size
    };

    union {
        //_VerItem sItem1;
        //_VerItem sItemFound;
        DWORD dwException1;		// Exception Record #
        DWORD dwVarNumber;	    // Variable #
        DWORD dwArgNumber;	    // Argument #
        DWORD dwOperand;        // Operand for the opcode
    };

    union {
        //_VerItem sItem2;
        //_VerItem sItemExpected;
        DWORD dwException2;	    // Exception Record #
    };
};


