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
	// The CLR data marshaler has some behaviors that are incompatible with
	// C++. Specifically, C++ treats boolean variables as byte size, whereas 
	// the marshaller treats them as 4-byte size.  Similarly, C++ treats
	// wchar_t variables as 4-byte size, whereas the marshaller treats them
	// as single byte size under certain conditions.  In order to work around
	// such issues, the C++ compiler will emit a type that the marshaller will
	// marshal using the correct sizes.  In addition, the compiler will place
	// this modopt onto the variables to indicate that the specified type is
	// not the true type.  Any compiler that needed to deal with similar
	// marshalling incompatibilities could use this attribute as well.
	//
	// Indicates that the modified instance differs from its true type for
	// correct marshalling.
    public static class CompilerMarshalOverride
    {
    }
}
