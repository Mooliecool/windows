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
// ParameterAttributes is an enum defining the attributes that may be 
//	associated with a Parameter.  These are defined in CorHdr.h.
//
// Date: Aug 99
//
namespace System.Reflection {
    
	using System;
	// This Enum matchs the CorParamAttr defined in CorHdr.h
	[Flags, Serializable]  
[System.Runtime.InteropServices.ComVisible(true)]
    public enum ParameterAttributes
    {
    	None	  =	  0x0000,	  // no flag is specified
        In        =   0x0001,     // Param is [In]    
        Out       =   0x0002,     // Param is [Out]   
        Lcid      =   0x0004,     // Param is [lcid]  
        Retval    =   0x0008,     // Param is [Retval]    
        Optional  =   0x0010,     // Param is optional 

		// Reserved flags for Runtime use only.
		ReservedMask              =   0xf000,
		HasDefault                =   0x1000,     // Param has default value.
		HasFieldMarshal           =   0x2000,     // Param has FieldMarshal.
		Reserved3                 =   0x4000,     // reserved bit
		Reserved4                 =   0x8000      // reserved bit 
    }
}
