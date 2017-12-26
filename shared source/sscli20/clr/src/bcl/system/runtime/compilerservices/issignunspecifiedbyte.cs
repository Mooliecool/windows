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
namespace System.Runtime.CompilerServices
{
	// C++ recognizes three char types: signed char, unsigned char, and char.
	// When a char is neither signed nor unsigned, it is a naked char.
	// This modopt indicates that the modified instance is a naked char.
	//
	// Any compiler could use this to indicate that the user has not specified
	// Sign behavior for the given byte.
    public static class IsSignUnspecifiedByte 
    {
    }
}
