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
//  IMembershipCondition.cs
//
//  Interface that all MembershipConditions must implement
//

namespace System.Security.Policy {
    
	using System;
[System.Runtime.InteropServices.ComVisible(true)]
    public interface IMembershipCondition : ISecurityEncodable, ISecurityPolicyEncodable
    {
        bool Check( Evidence evidence );
    
        IMembershipCondition Copy();
        
        String ToString();

        bool Equals( Object obj );
        
    }
}
