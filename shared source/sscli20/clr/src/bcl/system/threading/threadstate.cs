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
/*=============================================================================
**
** Class: ThreadState
**
**
** Purpose: Enum to represent the different thread states
**
**
=============================================================================*/

namespace System.Threading {

	[Serializable(),Flags]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum ThreadState
    {   
        /*=========================================================================
        ** Constants for thread states.
        =========================================================================*/
		Running = 0,
        StopRequested = 1,
        SuspendRequested = 2,
        Background = 4,
        Unstarted = 8,
        Stopped = 16,
        WaitSleepJoin = 32,
        Suspended = 64,
		AbortRequested = 128,
		Aborted = 256
    }
}
