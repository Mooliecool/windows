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
 ** Class: IFieldInfo
 **
 **
 ** Purpose: Interface For Returning FieldNames and FieldTypes
 **
 **
 ===========================================================*/

namespace System.Runtime.Serialization.Formatters {

	using System.Runtime.Remoting;
	using System.Runtime.Serialization;
	using System.Security.Permissions;
	using System;

[System.Runtime.InteropServices.ComVisible(true)]
    public interface IFieldInfo
    {
    	// Name of parameters, if null the default param names will be used
		String[] FieldNames 
		{
		    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
		    get;
		    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
		    set;
		}
		Type[] FieldTypes 
		{
		    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
		    get;
		    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
		    set;
		}		
    }
}
