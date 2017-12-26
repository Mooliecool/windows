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
//
// switches.h switch configuration of common runtime features
//

#define STRESS_HEAP
#define STRESS_THREAD
#define THREAD_DELAY
#define GC_SIZE
#define RETAIN_VM
#define VERIFY_HEAP


#define LOP_FRIENDLY_FRAMES

// Special flags used by JIT64 team for "Zapster".
// Zapster creates stand alone ngen-images that can be installed in the NIC later
#define ZAPSTER


// Define this to use VSD for all interface calls
#define STUB_DISPATCH

// Define this to use VSD for all virtual and interface calls (must define STUB_DISPATCH as well)
//#define STUB_DISPATCH_ALL

#ifdef _X86_
// Define this on x86 to test out indirection stubs
// #define USES_INDIRECT_STUB

// Define this on x86 to test out storing the MethodTable hash in the MethodTable
// #define PRECOMPUTE_METHODTABLE_HASH
#endif //_X86_

#if defined(_X86_) // || defined(_AMD64_)
#define METHOD_PRECODE                          1   // supports precode
#endif




#if defined(_DEBUG) && !defined(DACCESS_COMPILE)
    #define LOGGING
#endif




#define EMIT_FIXUPS

// Right now only AMD64 has support for direct GetThread/GetAppDomain calls
#define USE_INDIRECT_GET_THREAD_APPDOMAIN   1

#if defined(_X86_)
    #define PAGE_SIZE               0x1000
    #define CPU_HAS_FP_SUPPORT      1
    #define USE_UPPER_ADDRESS       0

#elif defined(_PPC_)
    #define PAGE_SIZE               0x1000
    #define CPU_HAS_FP_SUPPORT      1
    #define USE_UPPER_ADDRESS       0

#else // !defined(_AMD64_)
    #error Please add a new #elif clause and define all portability macros for the new platform
#endif // !defined(_AMD64_)

#ifndef OS_PAGE_SIZE
#define OS_PAGE_SIZE PAGE_SIZE
#endif

