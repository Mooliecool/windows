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
//*****************************************************************************
// Threadpool.h
//
// Class factories are used by the plumbing in COM to activate new objects.  
// This module contains the class factory code to instantiate the debugger
// objects described in <cordb.h>.
//
//*****************************************************************************
#ifndef __Threadpool__h__
#define __Threadpool__h__

#define WAIT_SINGLE_EXECUTION      0x00000001
#define WAIT_FREE_CONTEXT          0x00000002
#define WAIT_INTERNAL_COMPLETION   0x00000004

#define QUEUE_ONLY                 0x00000000  // do not attempt to call on the thread
#define CALL_OR_QUEUE              0x00000001  // call on the same thread if not too busy, else queue

#endif
