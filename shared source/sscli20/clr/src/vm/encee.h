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
// File: EnC.H
// 

// CEELOAD.H defines the class use to represent the PE file
// ===========================================================================
#ifndef EnC_H 
#define EnC_H

#include "ceeload.h"
#include "field.h"
#include "class.h"


    // If no EnC, then the EnC field iterator is just like the non-Enc iterator.
    typedef ApproxFieldDescIterator EncApproxFieldDescIterator;


#endif // #ifndef EnC_H 
