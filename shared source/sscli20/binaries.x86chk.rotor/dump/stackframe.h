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
#ifndef __STACKFRAME_H
#define __STACKFRAME_H

#include "regdisp.h"


struct StackFrame
{
    const static UINT_PTR maxVal = (UINT_PTR)(INT_PTR)-1;
    StackFrame() : SP(NULL) IA64_ARG(BSP(maxVal))
    {
    }

    StackFrame(UINT_PTR sp IA64_ARG(UINT_PTR bsp))
    {
        SP = sp;
        IA64_ONLY(BSP = bsp);
    }

    void Clear()
    {
        SP = NULL;
        IA64_ONLY(BSP = maxVal);
    }

    void SetMaxVal()
    {
        SP = maxVal;
        IA64_ONLY(BSP = NULL);
    }

    bool IsNull()
    {
        return (SP == NULL) IA64_ONLY(&& (BSP == maxVal));
    }

    bool IsMaxVal()
    {
        return (SP == maxVal) IA64_ONLY(&& (BSP == NULL));
    }

    bool operator==(StackFrame sf)
    {
        return (SP == sf.SP) IA64_ONLY(&& (BSP == sf.BSP));
    }

    bool operator!=(StackFrame sf)
    {
        return (SP != sf.SP) IA64_ONLY(|| (BSP != sf.BSP));
    }

    bool operator<(StackFrame sf)
    {
        return (SP < sf.SP) IA64_ONLY(|| ((SP == sf.SP) && (BSP > sf.BSP)));
    }

    bool operator<=(StackFrame sf)
    {
        return (SP <= sf.SP) IA64_ONLY(&& (BSP >= sf.BSP));
    }

    bool operator>(StackFrame sf)
    {
        return (SP > sf.SP) IA64_ONLY(|| ((SP == sf.SP) && (BSP < sf.BSP)));
    }

    bool operator>=(StackFrame sf)
    {
        return (SP >= sf.SP) IA64_ONLY(&& (BSP <= sf.BSP));
    }

    static inline StackFrame FromEstablisherFrame(UINT_PTR EstablisherFrame)
    {
        return StackFrame(EstablisherFrame);
    }

    static inline StackFrame FromRegDisplay(REGDISPLAY* pRD)
    {
        return StackFrame((UINT_PTR)GetRegdisplaySP(pRD));
    }

    UINT_PTR SP;
    IA64_ONLY(UINT_PTR BSP);
};

#endif  // __STACKFRAME_H
