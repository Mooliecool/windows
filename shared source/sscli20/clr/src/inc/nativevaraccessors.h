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
//*****************************************************************************
// The following are used to read and write data given NativeVarInfo
// for primitive types. Don't use these for VALUECLASSes.
//*****************************************************************************

#ifndef _NATIVE_VAR_ACCESSORS_H_
#define _NATIVE_VAR_ACCESSORS_H_

#include "corjit.h"

bool operator ==(const ICorDebugInfo::VarLoc &varLoc1,
                 const ICorDebugInfo::VarLoc &varLoc2);

#define MAX_NATIVE_VAR_LOCS 2

struct NativeVarLocation
{
    ULONG64 addr;
    TADDR size;
    bool contextReg;
};

ULONG NativeVarLocations(const ICorDebugInfo::VarLoc &   varLoc, 
                         PCONTEXT                        pCtx,
                         ULONG                           numLocs,
                         NativeVarLocation*              locs);

SIZE_T *NativeVarStackAddr(const ICorDebugInfo::VarLoc &   varLoc, 
                           PCONTEXT                        pCtx);
                        
bool    GetNativeVarVal(const ICorDebugInfo::VarLoc &   varLoc, 
                        PCONTEXT                        pCtx,
                        SIZE_T                      *   pVal1, 
                        SIZE_T                      *   pVal2
                        WIN64_ARG(SIZE_T                cbSize));
                        
bool    SetNativeVarVal(const ICorDebugInfo::VarLoc &   varLoc, 
                        PCONTEXT                        pCtx,
                        SIZE_T                          val1, 
                        SIZE_T                          val2
                        WIN64_ARG(SIZE_T                cbSize));                        
#endif // #ifndef _NATIVE_VAR_ACCESSORS_H_
