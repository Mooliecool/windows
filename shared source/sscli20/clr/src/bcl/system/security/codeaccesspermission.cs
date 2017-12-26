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
namespace System.Security
{
	using System.IO;
    using System.Threading;
	using System.Security;
	using System.Security.Util;
	using System.Security.Permissions;
	using System.Collections;
	using System.Text;
	using System;
	using IUnrestrictedPermission = System.Security.Permissions.IUnrestrictedPermission;

    [Serializable, SecurityPermissionAttribute( SecurityAction.InheritanceDemand, ControlEvidence = true, ControlPolicy = true )]
[System.Runtime.InteropServices.ComVisible(true)]
    abstract public class CodeAccessPermission
        : IPermission, ISecurityEncodable, IStackWalk
    {
        // Static methods for manipulation of stack
        public static void RevertAssert()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            SecurityRuntime.RevertAssert(ref stackMark);
        }

        public static void RevertDeny()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            SecurityRuntime.RevertDeny(ref stackMark);
        }

        public static void RevertPermitOnly()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            SecurityRuntime.RevertPermitOnly(ref stackMark);
        }

        public static void RevertAll()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            SecurityRuntime.RevertAll(ref stackMark);
        }

        //
        // Standard implementation of IPermission methods for
        // code-access permissions.
        //

        // Mark this method as requiring a security object on the caller's frame
        // so the caller won't be inlined (which would mess up stack crawling).
        [DynamicSecurityMethodAttribute()]
        public void Demand()
        {
            if (!this.CheckDemand( null ))
            {
                StackCrawlMark stackMark = StackCrawlMark.LookForMyCallersCaller;
                CodeAccessSecurityEngine.Check(this, ref stackMark);
            }
        }

        [DynamicSecurityMethodAttribute()]
        internal static void DemandInternal(PermissionType permissionType)
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCallersCaller;
            CodeAccessSecurityEngine.SpecialDemand(permissionType, ref stackMark);
        }

        // Metadata for this method should be flaged with REQ_SQ so that
        // EE can allocate space on the stack frame for FrameSecurityDescriptor

        [DynamicSecurityMethodAttribute()]
        public void Assert()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            CodeAccessSecurityEngine.Assert(this, ref stackMark);
        }

        [DynamicSecurityMethodAttribute()]
        static internal void AssertAllPossible()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            SecurityRuntime.AssertAllPossible(ref stackMark);
        }
    
        // Metadata for this method should be flaged with REQ_SQ so that
        // EE can allocate space on the stack frame for FrameSecurityDescriptor

        [DynamicSecurityMethodAttribute()]
        public void Deny()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            CodeAccessSecurityEngine.Deny(this, ref stackMark);
        }
        
        // Metadata for this method should be flaged with REQ_SQ so that
        // EE can allocate space on the stack frame for FrameSecurityDescriptor

        [DynamicSecurityMethodAttribute()]
        public void PermitOnly()
        {
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            CodeAccessSecurityEngine.PermitOnly(this, ref stackMark);
        }

        // IPermission interfaces

        // We provide a default implementation of Union here.
        // Any permission that doesn't provide its own representation 
        // of Union will get this one and trigger CompoundPermission
        // We can take care of simple cases here...

        public virtual IPermission Union(IPermission other) {
            // The other guy could be null
            if (other == null) return(this.Copy());
            
            // otherwise we don't support it.
            throw new NotSupportedException(Environment.GetResourceString( "NotSupported_SecurityPermissionUnion" ));
        }
        
        static internal SecurityElement CreatePermissionElement( IPermission perm, String permname )
        {
            SecurityElement root = new SecurityElement( "IPermission" );
            XMLUtil.AddClassAttribute( root, perm.GetType(), permname );
            // If you hit this assert then most likely you are trying to change the name of this class. 
            // This is ok as long as you change the hard coded string above and change the assert below.
            BCLDebug.Assert( perm.GetType().FullName.Equals( permname ), "Incorrect class name passed in! Was: " + permname + " Should be " + perm.GetType().FullName);

            root.AddAttribute( "version", "1" );
            return root;
        }
        
        static internal void ValidateElement( SecurityElement elem, IPermission perm )
        {
            if (elem == null)
                throw new ArgumentNullException( "elem" );
                
            if (!XMLUtil.IsPermissionElement( perm, elem ))
                throw new ArgumentException( Environment.GetResourceString( "Argument_NotAPermissionElement"));
                
            String version = elem.Attribute( "version" );
            
            if (version != null && !version.Equals( "1" ))
                throw new ArgumentException( Environment.GetResourceString( "Argument_InvalidXMLBadVersion") );
        }

        abstract public SecurityElement ToXml();
        abstract public void FromXml( SecurityElement elem );

        //
        // Unimplemented interface methods 
        // (as a reminder only)
        //

        public override String ToString()
        {
            return ToXml().ToString();
        }

        //
        // HELPERS FOR IMPLEMENTING ABSTRACT METHODS
        //

        //
        // Protected helper
        //

        internal bool VerifyType(IPermission perm)
        {
            // if perm is null, then obviously not of the same type
            if ((perm == null) || (perm.GetType() != this.GetType())) {
                return(false);
            } else {
                return(true);
            }
        }

        // The IPermission Interface
        public abstract IPermission Copy();
        public abstract IPermission Intersect(IPermission target);
        public abstract bool IsSubsetOf(IPermission target);

        [System.Runtime.InteropServices.ComVisible(false)]
        public override bool Equals(Object obj)
        {
            IPermission perm = obj as IPermission;
            if(obj != null && perm == null)
                return false;
            try {
                if(!this.IsSubsetOf(perm))
                    return false;
                if(perm != null && !perm.IsSubsetOf(this))
                    return false;
            }
            catch (ArgumentException)
            {
                // Any argument exception implies inequality
                // Note that we require a try/catch block here because we have to deal with
                // custom permissions that may throw exceptions indiscriminately.
                return false;
            }
            return true;
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public override int GetHashCode()
        {
            // This implementation is only to silence a compiler warning.
            return base.GetHashCode();
        }


        internal bool CheckDemand(CodeAccessPermission grant)
        {
            BCLDebug.Assert( grant == null || grant.GetType().Equals( this.GetType() ), "CheckDemand not defined for permissions of different type" );
            return IsSubsetOf( grant );
        }

        internal bool CheckPermitOnly(CodeAccessPermission permitted)
        {
            BCLDebug.Assert( permitted == null || permitted.GetType().Equals( this.GetType() ), "CheckPermitOnly not defined for permissions of different type" );
            return IsSubsetOf( permitted );
        }

        internal bool CheckDeny(CodeAccessPermission denied)
        {
            BCLDebug.Assert( denied == null || denied.GetType().Equals( this.GetType() ), "CheckDeny not defined for permissions of different type" );
            IPermission intersectPerm = Intersect(denied);
            return (intersectPerm == null || intersectPerm.IsSubsetOf(null));
        }

        internal bool CheckAssert(CodeAccessPermission asserted)
        {
            BCLDebug.Assert( asserted == null || asserted.GetType().Equals( this.GetType() ), "CheckPermitOnly not defined for permissions of different type" );
            return IsSubsetOf( asserted );
        }

        internal bool CanUnrestrictedOverride()
        {
            return CanUnrestrictedOverride(this);
        }

        internal static bool CanUnrestrictedOverride(IPermission ip)
        {
            if(CodeAccessSecurityEngine.DoesFullTrustMeanFullTrust())
                return true;
            if(ip is IUnrestrictedPermission)
                return true;
            return false;
        }
    }
}


