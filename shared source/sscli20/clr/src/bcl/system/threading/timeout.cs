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

namespace System.Threading {
	using System.Threading;
	using System;
    // A constant used by methods that take a timeout (Object.Wait, Thread.Sleep
    // etc) to indicate that no timeout should occur.
    //
    //This class has only static members and does not require serialization.
[System.Runtime.InteropServices.ComVisible(true)]
    public static class Timeout
    {
    
        public const int Infinite = -1;
    }

}
