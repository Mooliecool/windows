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
** Class: ThreadPriority
**
**
** Purpose: Enums for the priorities of a Thread
**
**
=============================================================================*/

namespace System.Threading {
	using System.Threading;

	[Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum ThreadPriority
    {   
        /*=========================================================================
        ** Constants for thread priorities.
        =========================================================================*/
        Lowest = 0,
        BelowNormal = 1,
        Normal = 2,
        AboveNormal = 3,
        Highest = 4
    
    }
}
