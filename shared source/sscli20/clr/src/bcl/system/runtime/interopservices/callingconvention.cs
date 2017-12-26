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
    // Used for the CallingConvention named argument to the DllImport attribute
	[Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
	public enum CallingConvention
    {
        Winapi          = 1,
        Cdecl           = 2,
        StdCall         = 3,
        ThisCall        = 4,
        FastCall        = 5,
    }
	
}
