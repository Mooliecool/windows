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
// MemberFilter is a delegate used to filter Members.  This delegate is used
//	as a callback from Type.FindMembers.
//
// Date: March 98
//
namespace System.Reflection {
    
    // Define the delegate
	[Serializable()] 
[System.Runtime.InteropServices.ComVisible(true)]
    public delegate bool MemberFilter(MemberInfo m, Object filterCriteria);
}
