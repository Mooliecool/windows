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
// ReflectionPermission.cs
//

namespace System.Security.Permissions
{
    using System;
    using System.IO;
    using System.Security.Util;
    using System.Text;
    using System.Runtime.Remoting;
    using System.Security;
    using System.Reflection;
    using System.Globalization;

    [Flags,Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum ReflectionPermissionFlag
    {
        NoFlags = 0x00,
        [Obsolete("This API has been deprecated. http://go.microsoft.com/fwlink/?linkid=14202")]
        TypeInformation = 0x01,
        MemberAccess = 0x02,
        ReflectionEmit = 0x04,
        AllFlags = 0x07
    }

[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class ReflectionPermission
           : CodeAccessPermission, IUnrestrictedPermission, IBuiltInPermission
    {

        private ReflectionPermissionFlag m_flags;

        //
        // Public Constructors
        //
        
        public ReflectionPermission(PermissionState state)
        {
            if (state == PermissionState.Unrestricted)
            {
                SetUnrestricted( true );
            }
            else if (state == PermissionState.None)
            {
                SetUnrestricted( false );
                Reset();
            }
            else
            {
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidPermissionState"));
            }
        }    
        
         // Parameters:
         //
        public ReflectionPermission(ReflectionPermissionFlag flag)
        {
            VerifyAccess(flag);
            
            SetUnrestricted(false);
            m_flags = flag;
        }
    
        //------------------------------------------------------
        //
        // PRIVATE AND PROTECTED MODIFIERS 
        //
        //------------------------------------------------------
        
        
        private void SetUnrestricted(bool unrestricted)
        {
            if (unrestricted)
            {
                m_flags = ReflectionPermissionFlag.AllFlags;
            }
            else
            {
                Reset();
            }
        }
        
        
        private void Reset()
        {
            m_flags = ReflectionPermissionFlag.NoFlags;
        }    
        
     
        public ReflectionPermissionFlag Flags
        {
            set
            {
                VerifyAccess(value);
            
                m_flags = value;
            }
            
            get
            {
                return m_flags;
            }
        }
        
            
    
    
        //
        // CodeAccessPermission implementation
        //
        
        public bool IsUnrestricted()
        {
            return m_flags == ReflectionPermissionFlag.AllFlags;
        }
        
        //
        // IPermission implementation
        //
        
        public override IPermission Union(IPermission other)
        {
            if (other == null)
            {
                return this.Copy();
            }
            else if (!VerifyType(other))
            {
                throw new 
                    ArgumentException(
                                    String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_WrongType"), this.GetType().FullName)
                                     );
            }
            
            ReflectionPermission operand = (ReflectionPermission)other;
    
            if (this.IsUnrestricted() || operand.IsUnrestricted())
            {
                return new ReflectionPermission( PermissionState.Unrestricted );
            }
            else
            {
                ReflectionPermissionFlag flag_union = (ReflectionPermissionFlag)(m_flags | operand.m_flags);
                return(new ReflectionPermission(flag_union));
            }
        }  
        
        
        
        public override bool IsSubsetOf(IPermission target)
        {
            if (target == null)
            {
                return m_flags == ReflectionPermissionFlag.NoFlags;
            }

            try
            {
                ReflectionPermission operand = (ReflectionPermission)target;
                if (operand.IsUnrestricted())
                    return true;
                else if (this.IsUnrestricted())
                    return false;
                else
                    return (((int)this.m_flags) & ~((int)operand.m_flags)) == 0;
            }
            catch (InvalidCastException)
            {
                throw new 
                    ArgumentException(
                                    String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_WrongType"), this.GetType().FullName)
                                     );
            }                

        }
        
        public override IPermission Intersect(IPermission target)
        {
            if (target == null)
                return null;
            else if (!VerifyType(target))
            {
                throw new 
                    ArgumentException(
                                    String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Argument_WrongType"), this.GetType().FullName)
                                     );
            }

            ReflectionPermission operand = (ReflectionPermission)target;

            ReflectionPermissionFlag newFlags = operand.m_flags & this.m_flags;
            
            if (newFlags == ReflectionPermissionFlag.NoFlags)
                return null;
            else
                return new ReflectionPermission( newFlags );
        }
    
        public override IPermission Copy()
        {
            if (this.IsUnrestricted())
            {
                return new ReflectionPermission(PermissionState.Unrestricted);
            }
            else
            {
                return new ReflectionPermission((ReflectionPermissionFlag)m_flags);
            }
        }
        
        
        //
        // IEncodable Interface 
    
        private
        void VerifyAccess(ReflectionPermissionFlag type)
        {
            if ((type & ~ReflectionPermissionFlag.AllFlags) != 0)
                throw new ArgumentException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Arg_EnumIllegalVal"), (int)type));
        }
        
    
        //------------------------------------------------------
        //
        // PUBLIC ENCODING METHODS
        //
        //------------------------------------------------------
        
        private const String _strHeaderTypeInformation  = "TypeInformation";
        private const String _strHeaderMemberAccess  = "MemberAccess";
        private const String _strHeaderReflectionEmit  = "ReflectionEmit";
    
        public override SecurityElement ToXml()
        {
            SecurityElement esd = CodeAccessPermission.CreatePermissionElement( this, "System.Security.Permissions.ReflectionPermission" );
            if (!IsUnrestricted())
            {
                esd.AddAttribute( "Flags", XMLUtil.BitFieldEnumToString( typeof( ReflectionPermissionFlag ), m_flags ) );
                }
            else
            {
                esd.AddAttribute( "Unrestricted", "true" );
            }
            return esd;
        }
    
        public override void FromXml(SecurityElement esd)
        {
            CodeAccessPermission.ValidateElement( esd, this );
            if (XMLUtil.IsUnrestricted( esd ))
            {
                m_flags = ReflectionPermissionFlag.AllFlags;
                return;
            }
           
            Reset () ;
            SetUnrestricted (false) ;
    
            String flags = esd.Attribute( "Flags" );
            if (flags != null)
                m_flags = (ReflectionPermissionFlag)Enum.Parse( typeof( ReflectionPermissionFlag ), flags );
        }

        /// <internalonly/>
        int IBuiltInPermission.GetTokenIndex()
        {
            return ReflectionPermission.GetTokenIndex();
        }

        internal static int GetTokenIndex()
        {
            return BuiltInPermissionIndex.ReflectionPermissionIndex;
        }

       
    }

}
