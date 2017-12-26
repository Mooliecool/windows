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
// Interface Map.  This struct returns the mapping of an interface into the actual methods on a class
//	that implement that interface.
//
// Date: March 2000
//
namespace System.Reflection {
    using System;

[System.Runtime.InteropServices.ComVisible(true)]
	public struct InterfaceMapping {
[System.Runtime.InteropServices.ComVisible(true)]
		public Type				TargetType;			// The type implementing the interface
[System.Runtime.InteropServices.ComVisible(true)]
		public Type				InterfaceType;		// The type representing the interface
[System.Runtime.InteropServices.ComVisible(true)]
		public MethodInfo[]		TargetMethods;		// The methods implementing the interface
[System.Runtime.InteropServices.ComVisible(true)]
		public MethodInfo[]		InterfaceMethods;	// The methods defined on the interface
	}
}
