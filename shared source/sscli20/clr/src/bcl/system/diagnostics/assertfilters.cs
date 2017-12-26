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
namespace System.Diagnostics {
    
    /*
     * FailDebug indicates the debugger should be invoked
     * FailIgnore indicates the failure should be ignored & the 
     *			program continued
     * FailTerminate indicates that the program should be terminated
     * FailContinue indicates that no decision is made - 
     *		the previous Filter should be invoked
     */
	using System;
	[Serializable()]
    internal enum AssertFilters
    {
        FailDebug           = 0,
    	FailIgnore          = 1,
    	FailTerminate       = 2,
    	FailContinueFilter  = 3,
    }
}
