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
// CallingConventions is a set of Bits representing the calling conventions
//	in the system.
//
// Date: Aug 99
//
namespace System.Reflection {
	using System.Runtime.InteropServices;
	using System;
    [Flags, Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum CallingConventions
    {
		//NOTE: If you change this please update COMMember.cpp.  These
		//	are defined there.
    	Standard		= 0x0001,
    	VarArgs			= 0x0002,
    	Any				= Standard | VarArgs,
        HasThis         = 0x0020,
        ExplicitThis    = 0x0040,
    }
}
