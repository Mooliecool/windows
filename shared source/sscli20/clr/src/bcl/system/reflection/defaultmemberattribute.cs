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
// DefaultMemberAttribute is defines the Member of a Type that is the "default"
//	member used by Type.InvokeMember.  The default member is simply a name given
//	to a type.
//
// 
// 
//
namespace System.Reflection {
    
    using System;

    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Interface),Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public sealed class DefaultMemberAttribute : Attribute
    {
		// The name of the member
    	private String m_memberName;

		// You must provide the name of the member, this is required
    	public DefaultMemberAttribute(String memberName) {
    		m_memberName = memberName;
    	}

		// A get accessor to return the name from the attribute.
		// NOTE: There is no setter because the name must be provided
		//	to the constructor.  The name is not optional.
        public String MemberName {
    		get {return m_memberName;}
    	}
    }
}
