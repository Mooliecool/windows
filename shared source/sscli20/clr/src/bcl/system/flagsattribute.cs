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
namespace System {
    
	using System;
    // Custom attribute to indicate that the enum
    // should be treated as a bitfield (or set of flags).
    // An IDE may use this information to provide a richer
    // development experience.
    [AttributeUsage(AttributeTargets.Enum, Inherited = false),Serializable] 
[System.Runtime.InteropServices.ComVisible(true)]
	public class  FlagsAttribute : Attribute
    {
    	public FlagsAttribute()
    	{
    	}
    }
}
