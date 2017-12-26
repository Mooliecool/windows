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
//
// TypeFilter defines a delegate that is as a callback function for filtering
//	a list of Types.
//
// Date: March 98
//
namespace System.Reflection {
    
    // Define the delegate
	[Serializable()]
[System.Runtime.InteropServices.ComVisible(true)]
    public delegate bool TypeFilter(Type m, Object filterCriteria);
}
