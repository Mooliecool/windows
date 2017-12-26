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
#ifndef MEMORYREPORT_H_
#define MEMORYREPORT_H_

#include "utilcode.h"
#include "rangetree.h"
#include "clrhost.h"
#include "shash.h"
#include <stdio.h>
#include <wchar.h>

#ifndef DACCESS_COMPILE
#endif



inline void ___noop(...) { };

#define MEMORY_REPORT_CONTEXT_SCOPE ___noop
#define MEMORY_REPORT_ATTRIBUTION_SCOPE ___noop
#define MEMORY_REPORT_SET_ATTRIBUTION ___noop
#define MEMORY_REPORT_CLEAR() do {} while (FALSE)
#define MEMORY_REPORT_ALLOCATE(b,s,t,n) do {} while (FALSE)
#define MEMORY_REPORT_FREE(s,t) do {} while (FALSE)
#define MEMORY_REPORT_FREE_RANGE(b,s,t) do {} while (FALSE)


#endif // MEMORYREPORT_H_
