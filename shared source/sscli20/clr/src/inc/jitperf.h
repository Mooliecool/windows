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
//-----------------------------------------------------------------------------
// JitPerf.h
// Internal interface for gathering JIT perfmormance stats. These stats are
// logged (or displayed) in two ways. If PERF_COUNTERS are enabled the 
// perfmon etc. would display the jit stats. If ENABLE_PERF_LOG is enabled
// and PERF_OUTPUT env var is defined then the jit stats are displayed on the 
// stdout. (The jit stats are outputted in a specific format to a file for 
// automated perf tests.)
//-----------------------------------------------------------------------------

#ifndef __JITPERF_H__
#define __JITPERF_H__

#include "mscoree.h"
#include "clrinternal.h"


#define START_JIT_PERF()
#define STOP_JIT_PERF()
#define START_NON_JIT_PERF()
#define STOP_NON_JIT_PERF()
#define JIT_PERF_UPDATE_X86_CODE_SIZE(size)                 

#endif //__JITPERF_H__
