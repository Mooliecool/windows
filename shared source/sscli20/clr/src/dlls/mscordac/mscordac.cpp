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

// just includes some references to the global functions we export, so that ld
// will link them (and all of their dependants) in.

#if defined(FEATURE_PAL) && defined (PLATFORM_UNIX)

extern "C" int OutOfProcessFunctionTableCallback();
extern "C" int CLRDataCreateInstance();

void
UNUSED_MSCORDAC_REFSYMBOLS()
{
    OutOfProcessFunctionTableCallback();
    CLRDataCreateInstance();
}

#endif // FEATURE_PAL && PLATFORM_UNIX
