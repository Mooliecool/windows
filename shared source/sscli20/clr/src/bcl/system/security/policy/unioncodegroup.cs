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
//  UnionCodeGroup.cs
//
//  Representation for code groups used for the policy mechanism
//

namespace System.Security.Policy {
    
    using System;
    using System.Security.Util;
    using System.Security;
    using System.Collections;
    
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    sealed public class UnionCodeGroup : CodeGroup, IUnionSemanticCodeGroup
    {
        internal UnionCodeGroup()
            : base()
        {
        }
        
        internal UnionCodeGroup( IMembershipCondition membershipCondition, PermissionSet permSet )
            : base( membershipCondition, permSet )
        {
        }
        
        public UnionCodeGroup( IMembershipCondition membershipCondition, PolicyStatement policy )
            : base( membershipCondition, policy )
        {
        }
        
        
        public override PolicyStatement Resolve( Evidence evidence )
        {
            if (evidence == null)
                throw new ArgumentNullException("evidence");
                
            if (this.MembershipCondition.Check( evidence ))
            {
                PolicyStatement thisPolicy = this.PolicyStatement;

                IEnumerator enumerator = this.Children.GetEnumerator();

                while (enumerator.MoveNext())
                {
                    PolicyStatement childPolicy = ((CodeGroup)enumerator.Current).Resolve( evidence );

                    if (childPolicy != null)
                    {
                        if (((thisPolicy.Attributes & childPolicy.Attributes) & PolicyStatementAttribute.Exclusive) == PolicyStatementAttribute.Exclusive)
                        {
                            throw new PolicyException( Environment.GetResourceString( "Policy_MultipleExclusive" ) );
                        }

                        thisPolicy.GetPermissionSetNoCopy().InplaceUnion( childPolicy.GetPermissionSetNoCopy() );
                        thisPolicy.Attributes = thisPolicy.Attributes | childPolicy.Attributes;
                    }
                }

                return thisPolicy;
            }           
            else
            {
                return null;
            }
        }        

        /// <internalonly/>
        PolicyStatement IUnionSemanticCodeGroup.InternalResolve( Evidence evidence )
        {
            if (evidence == null)
                throw new ArgumentNullException("evidence");
                
            if (this.MembershipCondition.Check( evidence ))
            {
                return this.PolicyStatement;
            }
            else
            {
                return null;
            }        
        }
        
        public override CodeGroup ResolveMatchingCodeGroups( Evidence evidence )
        {
            if (evidence == null)
                throw new ArgumentNullException("evidence");

            if (this.MembershipCondition.Check( evidence ))
            {
                CodeGroup retGroup = this.Copy();

                retGroup.Children = new ArrayList();

                IEnumerator enumerator = this.Children.GetEnumerator();
                
                while (enumerator.MoveNext())
                {
                    CodeGroup matchingGroups = ((CodeGroup)enumerator.Current).ResolveMatchingCodeGroups( evidence );
                    
                    // If the child has a policy, we are done.
                    
                    if (matchingGroups != null)
                    {
                        retGroup.AddChild( matchingGroups );
                    }
                }

                return retGroup;
                
            }
            else
            {
                return null;
            }
        }

        
        public override CodeGroup Copy()
        {
            UnionCodeGroup group = new UnionCodeGroup();
            
            group.MembershipCondition = this.MembershipCondition;
            group.PolicyStatement = this.PolicyStatement;
            group.Name = this.Name;
            group.Description = this.Description;

            IEnumerator enumerator = this.Children.GetEnumerator();

            while (enumerator.MoveNext())
            {
                group.AddChild( (CodeGroup)enumerator.Current );
            }

            
            return group;
        }
        
        public override String MergeLogic
        {
            get
            {
                return Environment.GetResourceString( "MergeLogic_Union" );
            }
        }
        
        internal override String GetTypeName()
        {
            return "System.Security.Policy.UnionCodeGroup";
        }
	
    }                

}
