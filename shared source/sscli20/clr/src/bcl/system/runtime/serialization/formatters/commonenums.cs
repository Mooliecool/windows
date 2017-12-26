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
 ** Class: FormatterEnums
 **
 **
 ** Purpose: Soap XML Formatter Enums
 **
 **
 ===========================================================*/

namespace System.Runtime.Serialization.Formatters {
	using System.Threading;
	using System.Runtime.Remoting;
	using System.Runtime.Serialization;
	using System;
    // Enums which specify options to the XML and Binary formatters
    // These will be public so that applications can use them
	[Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum FormatterTypeStyle
    {
    	TypesWhenNeeded = 0, // Types are outputted only for Arrays of Objects, Object Members of type Object, and ISerializable non-primitive value types
    	TypesAlways = 0x1, // Types are outputted for all Object members and ISerialiable object members.
		XsdString = 0x2     // Strings are outputed as xsd rather then SOAP-ENC strings. No string ID's are transmitted
    }

	[Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
	public enum FormatterAssemblyStyle
	{
		Simple = 0,
		Full = 1,
	}

[System.Runtime.InteropServices.ComVisible(true)]
    public enum TypeFilterLevel {
        Low = 0x2,
        Full = 0x3
    }    
}
