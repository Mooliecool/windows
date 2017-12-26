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
//  AllMembershipCondition.cs
//
//  Simple IMembershipCondition implementation that always passes
//

namespace System.Security.Policy {
    
    using System;
    using System.Security;
    using System.Security.Util;
    using System.Security.Permissions;
    using System.Collections;
    
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class AllMembershipCondition : IMembershipCondition, IConstantMembershipCondition
    {
        public AllMembershipCondition()
        {
        }
        
        public bool Check( Evidence evidence )
        {
            return true;
        }
        
        public IMembershipCondition Copy()
        {
            return new AllMembershipCondition();
        }
        
        public override String ToString()
        {
            return Environment.GetResourceString( "All_ToString" );
        }
        
        public SecurityElement ToXml()
        {
            return ToXml( null );
        }
    
        public void FromXml( SecurityElement e )
        {
            FromXml( e, null );
        }
        
        public SecurityElement ToXml( PolicyLevel level )
        {
            SecurityElement root = new SecurityElement( "IMembershipCondition" );
            System.Security.Util.XMLUtil.AddClassAttribute( root, this.GetType(), "System.Security.Policy.AllMembershipCondition" );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( this.GetType().FullName.Equals( "System.Security.Policy.AllMembershipCondition" ), "Class name changed!" );

            root.AddAttribute( "version", "1" );
            
            return root;
        }
    
        public void FromXml( SecurityElement e, PolicyLevel level )
        {
            if (e == null)
                throw new ArgumentNullException("e");
        
            if (!e.Tag.Equals( "IMembershipCondition" ))
            {
                throw new ArgumentException( Environment.GetResourceString( "Argument_MembershipConditionElement" ) );
            }
            
        }
        
        public override bool Equals( Object o )
        {
            return (o is AllMembershipCondition);
        }
        
        public override int GetHashCode()
        {
            return typeof( AllMembershipCondition ).GetHashCode();
        }
    }
}
