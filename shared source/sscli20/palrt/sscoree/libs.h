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

#ifndef __SSCOREE_LIBS_H__
#define __SSCOREE_LIBS_H__

#define SSCOREE_LIB_START(NAME) LIB_ ## NAME,

typedef enum {
#include "sscoree_shims.h"
    LIB_MAX_LIB, /* must be last */
} ShimmedLib;

#define LIB_INDEX_VALID(LibIndex)                                       \
    ((LibIndex) >= (ShimmedLib)0 && (LibIndex) < LIB_MAX_LIB)

typedef struct {
    LPCTSTR     Name;
    HMODULE     Handle;
} LibEntry;

extern LibEntry g_Libs[];

#endif // __SSCOREE_LIBS_H__
