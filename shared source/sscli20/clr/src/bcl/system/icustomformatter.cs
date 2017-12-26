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
/*============================================================
**
** Interface:  ICustomFormatter
**
**
** Purpose: Marks a class as providing special formatting
**
**
===========================================================*/
namespace System {
    
	using System;
	using System.Runtime.Serialization;
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ICustomFormatter
    {
	// Interface does not need to be marked with the serializable attribute
    	String Format (String format, Object arg, IFormatProvider formatProvider);
    	
    }
}
