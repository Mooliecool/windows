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
// ICustomAttributeProvider is an interface that is implemented by reflection
//	objects which support custom attributes.
//
// Date: July 99
//
namespace System.Reflection {
    
    using System;

	// Interface does not need to be marked with the serializable attribute
[System.Runtime.InteropServices.ComVisible(true)]
    public interface ICustomAttributeProvider
    {

    	// Return an array of custom attributes identified by Type
    	Object[] GetCustomAttributes(Type attributeType, bool inherit);


    	// Return an array of all of the custom attributes (named attributes are not included)
    	Object[] GetCustomAttributes(bool inherit);

    
		// Returns true if one or more instance of attributeType is defined on this member. 
		bool IsDefined (Type attributeType, bool inherit);
	
    }
}
