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
// MemberTypes is an bit mask marking each type of Member that is defined as
//	a subclass of MemberInfo.  These are returned by MemberInfo.MemberType and 
//	are useful in switch statements.
//
// Date: July 99
//
namespace System.Reflection {
    
	using System;
    // This Enum matchs the CorTypeAttr defined in CorHdr.h
	[Serializable()]
	[Flags()]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum MemberTypes
    {
    	// The following are the known classes which extend MemberInfo
    	Constructor		= 0x01,
    	Event			= 0x02,
    	Field			= 0x04,
    	Method			= 0x08,
    	Property		= 0x10,
    	TypeInfo		= 0x20,
    	Custom			= 0x40,
    	NestedType		= 0x80,
    	All				= Constructor | Event | Field | Method | Property | TypeInfo | NestedType,
    }
}
