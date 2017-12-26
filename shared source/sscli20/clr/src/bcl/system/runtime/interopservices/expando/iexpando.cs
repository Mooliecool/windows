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
// IExpando is an interface which allows Objects implemeningt this interface 
//	support the ability to modify the object by adding and removing members, 
//	represented by MemberInfo objects.
//
// Date: March 98
//
// The IExpando Interface.
namespace System.Runtime.InteropServices.Expando {
    
	using System;
	using System.Reflection;

    [Guid("AFBF15E6-C37C-11d2-B88E-00A0C9B471B8")]
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IExpando : IReflect
    {
    	// Add a new Field to the reflection object.  The field has
    	// name as its name.
        FieldInfo AddField(String name);

    	// Add a new Property to the reflection object.  The property has
    	// name as its name.
	    PropertyInfo AddProperty(String name);

    	// Add a new Method to the reflection object.  The method has 
    	// name as its name and method is a delegate
    	// to the method.  
        MethodInfo AddMethod(String name, Delegate method);

    	// Removes the specified member.
        void RemoveMember(MemberInfo m);
    }
}
