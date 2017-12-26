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
// ===========================================================================
// File: symkind.h
//
// Define the different symbol kinds. Only non-abstract symbol kinds are here.
// ===========================================================================

#ifndef __symkind_h__
#define __symkind_h__

enum SYMKIND {

    #define SYMBOLDEF(kind, global, local) SK_ ## kind,
    #include "symkinds.h"

    SK_LIM
};


// The kinds of aggregates.
namespace AggKind {
    enum _Enum {
        Unknown,

        Class,
        Delegate,
        Interface,
        Struct,
        Enum,

        Lim
    };
};
DECLARE_ENUM_TYPE(AggKind);

#endif // __symkind_h__
