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

#ifndef __SSCOREE_SYMS_H__
#define __SSCOREE_SYMS_H__

#include "libs.h"


// sscoree keeps a single symbol table. This table a symbol for each function
// and library defined in sscoree_shims.h. The library symbols are named
// __libname__end and are there so that SscoreeShimGetProcAddress() can find
// in which library a given symbol index is.
//
// Since the symbol name in this table is only used for assertion purposes,
// it's no big sin if, somehow, __libname__end happens to collide with another
// symbol.
//
// It's important to ensure that the g_Syms[] array ALSO has entries for those
// library symbols. g_Syms[] must be kept in sync with the ShimmedSym enum.
//

#define SSCOREE_LIB_END(NAME) SHIMLIB_ ## NAME,

#define SSCOREE_SHIM_FUNC(FUNC) \
    SHIMSYM_ ## FUNC,

typedef enum {
#include "sscoree_shims.h"
    SHIMSYM_MAX_SYMBOL, /* must be last */
} ShimmedSym;

#define SYM_INDEX_VALID(SymIndex)                                       \
    ((SymIndex) >= (ShimmedSym)0 && (SymIndex) < SHIMSYM_MAX_SYMBOL)

typedef struct {
    LPCSTR      Name;
    FARPROC     Proc;
} SymEntry;

extern SymEntry g_Syms[];

#endif // __SSCOREE_SYMS_H__
