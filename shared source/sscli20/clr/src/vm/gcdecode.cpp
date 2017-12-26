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
#include "common.h"

#ifndef FJITONLY

#define FPO_INTERRUPTIBLE 0

/* Precompiled header nonsense requires that we do it this way  */

/* GCDecoder.cpp is a common source file bewtween VM and JIT/IL */
/* GCDecoder.cpp is located in $COM99/inc                       */

#include "gcdecoder.cpp"

#endif // FJITONLY
