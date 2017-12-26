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
// CrtWrap.h
//
// Wrapper code for the C runtime library.
//
//*****************************************************************************
#ifndef __CrtWrap_h__
#define __CrtWrap_h__


#include "rotor_palrt.h"

#ifdef REDEFINE_NO_CRT
#undef REDEFINE_NO_CRT
#define NO_CRT 1
#endif

#endif // __CrtWrap_h__

