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
#ifndef __new__hpp
#define __new__hpp

struct NoThrow { int x; };
extern const NoThrow nothrow;

void * __cdecl operator new(size_t n, const NoThrow&);
void * __cdecl operator new[](size_t n, const NoThrow&);

#ifdef _DEBUG
void DisableThrowCheck();
#endif

#endif
