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
// File: pch.h
// 
// ===========================================================================

#ifndef _PCH_H_
#define _PCH_H_

#include "ole2.h"

#include "rotor_pal.h"
#include "rotor_palrt.h"

#include "umisc.h"

#include "corhdr.h"
#include "corsym.h"
#include "palclr.h"

#define NEW( x ) ( ::new (nothrow) x )
#define DELETE( x ) ( ::delete(x) )
#define DELETEARRAY( x ) (::delete[] (x))

#include "symwrite.h"
#include "symread.h"
#include "symbinder.h"

#endif
