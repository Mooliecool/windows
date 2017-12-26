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
namespace System.Reflection.Emit {
    
	using System;
    // This Enum matchs the CorFieldAttr defined in CorHdr.h
	[Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public enum PEFileKinds
    {
        Dll			=   0x0001,  
		ConsoleApplication = 0x0002,
		WindowApplication = 0x0003,
    }
}
