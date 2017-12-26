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

#include "rotor_pal.h"
#include "rotor_palrt.h"
#include "syms.h"
#include "libs.h"

#define SSCOREE_LIB_START(NAME) {MAKEDLLNAME (TEXT (#NAME))},

LibEntry g_Libs[] = {
#include "sscoree_shims.h"
};

#define SSCOREE_SHIM_FUNC(FUNC) {#FUNC, NULL},
#define SSCOREE_LIB_END(NAME) {"__" #NAME "__end", NULL},

// See the comment at the top of syms.h for more on g_Syms

SymEntry g_Syms[] = {
#include "sscoree_shims.h"
};
