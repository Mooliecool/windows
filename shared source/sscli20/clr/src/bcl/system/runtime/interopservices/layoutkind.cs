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
    // Used in the StructLayoutAttribute class
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] public enum LayoutKind
    {
        Sequential		= 0, // 0x00000008,
        Explicit		= 2, // 0x00000010,
        Auto			= 3, // 0x00000000,
    }
}
