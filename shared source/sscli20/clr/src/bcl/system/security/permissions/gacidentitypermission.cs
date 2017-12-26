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
// GacIdentityPermission.cs
// 

namespace System.Security.Permissions
{
    using System;
    using SecurityElement = System.Security.SecurityElement;
    using System.Globalization;

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )]
    [System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class GacIdentityPermissionAttribute : CodeAccessSecurityAttribute
    {
        public GacIdentityPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public override IPermission CreatePermission()
        {
            return new GacIdentityPermission();
        }
    }


    [System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class GacIdentityPermission : CodeAccessPermission, IBuiltInPermission
    {
        //------------------------------------------------------
        //
        // PUBLIC CONSTRUCTORS
        //
        //------------------------------------------------------

        public GacIdentityPermission(PermissionState state)
        {
            if (state == PermissionState.Unrestricted)
            {
                if(!CodeAccessSecurityEngine.DoesFullTrustMeanFullTrust())
                    throw new ArgumentException(Environment.GetResourceString("Argument_UnrestrictedIdentityPermission"));
            }
            else if (state == PermissionState.None)
            {

            }
            else
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidPermissionState"));
            }
        }

        public GacIdentityPermission()
        {
        }

        //------------------------------------------------------
        //
        // IPERMISSION IMPLEMENTATION
        //
        //------------------------------------------------------


        public override IPermission Copy()
        {
            return new GacIdentityPermission();
        }

        public override bool IsSubsetOf(IPermission target)
        {
            if (target == null)
                return false;
            if (!(target is GacIdentityPermission))
                throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_WrongType"), this.GetType().FullName));
            return true;
        }

        public override IPermission Intersect(IPermission target)
        {
            if (target == null)
                return null;
            if (!(target is GacIdentityPermission))
                throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_WrongType"), this.GetType().FullName));
            return this.Copy();
        }

        public override IPermission Union(IPermission target)
        {
            if (target == null)
                return this.Copy();
            if (!(target is GacIdentityPermission))
                throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_WrongType"), this.GetType().FullName));
            return this.Copy();
        }

        public override SecurityElement ToXml()
        {
            SecurityElement securityElement = CodeAccessPermission.CreatePermissionElement( this, "System.Security.Permissions.GacIdentityPermission" );
            return securityElement;
        }

        public override void FromXml(SecurityElement securityElement)
        {
            CodeAccessPermission.ValidateElement(securityElement, this);
        }

        /// <internalonly/>
        int IBuiltInPermission.GetTokenIndex()
        {
            return GacIdentityPermission.GetTokenIndex();
        }

        internal static int GetTokenIndex()
        {
            return BuiltInPermissionIndex.GacIdentityPermissionIndex;
        }
    }
}
