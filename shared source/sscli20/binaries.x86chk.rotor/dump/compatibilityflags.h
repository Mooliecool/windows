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

// Unmanaged enum with compatibility flags. See compatibilityflagsdef.h for more details.

#ifndef __COMPATIBILITYFLAGS_H__
#define __COMPATIBILITYFLAGS_H__

enum CompatibilityFlag {
#define COMPATFLAGDEF(name) compat##name,
#include "compatibilityflagsdef.h"
    compatCount,
};

#endif // __COMPATIBILITYFLAGS_H__
