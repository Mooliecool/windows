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
#ifndef __DBGALLOC_H_INCLUDED
#define __DBGALLOC_H_INCLUDED

//
// DbgAlloc.h
//
//  Routines layered on top of allocation primitives to provide debugging
//  support.
//

#include "switches.h"

void * __stdcall DbgAlloc(size_t n, void **ppvCallstack, BOOL isArray = FALSE);
void __stdcall DbgFree(void *b, void **ppvCallstack, BOOL isArray = FALSE);
unsigned __stdcall DbgAllocSnapshot();
void __stdcall DbgAllocReport(__in_z __in_opt char *pString = NULL,
                              BOOL fDone = TRUE,
                              BOOL fDoPrintf = TRUE,
                              unsigned snapShot = 0);

#define CDA_DECL_CALLSTACK()
#define CDA_GET_CALLSTACK() NULL


#endif
