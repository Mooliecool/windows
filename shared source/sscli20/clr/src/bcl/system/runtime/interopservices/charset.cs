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
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
namespace System.Runtime.InteropServices {
	using System;
    // Use this in P/Direct function prototypes to specify 
    // which character set to use when marshalling Strings.
    // Using Ansi will marshal the strings as 1 byte char*'s.
    // Using Unicode will marshal the strings as 2 byte wchar*'s.
    // Generally you probably want to use Auto, which does the
    // right thing 99% of the time.
    [Serializable] 
[System.Runtime.InteropServices.ComVisible(true)]
	public enum CharSet
    {
    	None = 1,		// User didn't specify how to marshal strings.
    	Ansi = 2,		// Strings should be marshalled as ANSI 1 byte chars. 
    	Unicode = 3,    // Strings should be marshalled as Unicode 2 byte chars.
    	Auto = 4,		// Marshal Strings in the right way for the target system. 
    }
}
