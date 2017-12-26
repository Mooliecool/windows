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

#include "common.h"
#include "object.inl"

#ifdef GC_PROFILING
#include "profilepriv.h"
#endif

#include "field.h"
#include "perfcounters.h"
#include "eventtrace.h"
#include "log.h"
#include "eeconfig.h"
#include "excep.h"

#define SERVER_GC 1

// There is no server GC for GC_SMP
#ifndef GC_SMP

namespace SVR { 
using ::Object;
using ::ArrayBase;

#include "gcimpl.h"
#include "gc.cpp"

}

#endif // GC_SMP
