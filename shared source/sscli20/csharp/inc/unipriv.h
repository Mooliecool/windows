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

// unipriv.h -- UniLib private internal header

#ifdef _MSC_VER
#pragma once
#endif

#ifndef __UNIPRIV_H__
#define __UNIPRIV_H__

#undef UASSERT

#define __UBREAK__ DebugBreak();
#define UASSERT(exp) _ASSERTE(exp)

#endif // __UNIPRIV_H__
