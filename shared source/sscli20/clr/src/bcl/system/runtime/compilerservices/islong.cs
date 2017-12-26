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
	// The C++ standard indicates that a long is always 4-bytes, whereas the
	// size of an integer is system dependent (not exceedign sizeof(long)).
	// The CLR does not offer a mechanism for encoding this distinction,
	// but it is critically important for maintaining language level type
	// safety.
	//
	// Indicates that the modified integer is a standard C++ long.
    // Could also be called IsAlternateIntegerType or something else.
	public static class IsLong
    {
    }
}
