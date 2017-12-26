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
// Intrinsic.h
//
// Force several very useful functions to be intrinsic, which means that the
// compiler will generate code inline for the functions instead of generating
// a call to the function.
//
//*****************************************************************************
#ifndef __intrinsic_h__
#define __intrinsic_h__

#ifdef _MSC_VER
#pragma intrinsic(memcmp)
#pragma intrinsic(memcpy)
#pragma intrinsic(memset)
#pragma intrinsic(strcmp)
#pragma intrinsic(strcpy)
#pragma intrinsic(strlen)
#endif  // defined(_MSC_VER)

#endif // __intrinsic_h__
