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
// File:  poppack.h
// 
// ===========================================================================
/*
Abstract:

    This file turns packing of structures off.  (That is, it enables
    automatic alignment of structure fields.)  An include file is needed
    because various compilers do this in different ways.

    poppack.h is the complement to pshpack?.h.  An inclusion of poppack.h
    MUST ALWAYS be preceded by an inclusion of one of pshpack?.h, in one-to-one
    correspondence.

    For Microsoft compatible compilers, this file uses the pop option
    to the pack pragma so that it can restore the previous saved by the
    pshpack?.h include file.

*/

#if ! (defined(lint) || defined(RC_INVOKED))
#if ( _MSC_VER >= 800 && !defined(_M_I86)) || defined(_PUSHPOP_SUPPORTED)
#pragma warning(disable:4103)
#if !(defined( MIDL_PASS )) || defined( __midl )
#pragma pack(pop)
#else
#pragma pack()
#endif
#else
#pragma pack()
#endif
#endif // ! (defined(lint) || defined(RC_INVOKED))
