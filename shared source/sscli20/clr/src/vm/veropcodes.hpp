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
// veropcodes.hpp
//
// Declares the enumeration of the opcodes and the decoding tables.
//
#include "openum.h"

#define HackInlineAnnData  0x7F

#ifdef DECLARE_DATA
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) L##s,

WCHAR *ppOpcodeNameList[] =
{
#include "../inc/opcode.def"
};

#undef OPDEF
#define OPDEF(c,s,pop,push,args,type,l,s1,s2,ctrl) args,

// Whether opcode is Inline0 etc.
BYTE OpcodeData[] =
{
#include "../inc/opcode.def"
     0 /* for CEE_COUNT */
};

#undef OPDEF

#else /* !DECLARE_DATA */

extern WCHAR *ppOpcodeNameList[];

extern BYTE OpcodeData[];

#endif /* DECLARE_DATA */
