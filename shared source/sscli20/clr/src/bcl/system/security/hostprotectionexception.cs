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

/*=============================================================================
**
** Class: HostProtectionException
**
** Purpose: Exception class for HostProtection
**
**
=============================================================================*/

namespace System.Security
{
    using System.Security;
    using System;
    using System.Runtime.Serialization;
    using System.Security.Permissions;
    using System.Reflection;
    using System.Text;

    [System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class HostProtectionException : SystemException
    {
        private HostProtectionResource m_protected;
        private HostProtectionResource m_demanded;

        private const String ProtectedResourcesName = "ProtectedResources";
        private const String DemandedResourcesName = "DemandedResources";

        public HostProtectionException() : base()
        {
            m_protected = HostProtectionResource.None;
            m_demanded = HostProtectionResource.None;
        }

        public HostProtectionException(string message) : base(message)
        {
            m_protected = HostProtectionResource.None;
            m_demanded = HostProtectionResource.None;
        }

        public HostProtectionException(string message, Exception e) : base(message, e)
        {
            m_protected = HostProtectionResource.None;
            m_demanded = HostProtectionResource.None;
        }

        protected HostProtectionException(SerializationInfo info, StreamingContext context) : base(info, context)
        {
            if (info==null)
                throw new ArgumentNullException("info");

            m_protected = (HostProtectionResource)info.GetValue(ProtectedResourcesName, typeof(HostProtectionResource));
            m_demanded = (HostProtectionResource)info.GetValue(DemandedResourcesName, typeof(HostProtectionResource));
        }

        public HostProtectionException(string message, HostProtectionResource protectedResources, HostProtectionResource demandedResources)
            : base(message)
        {
            SetErrorCode(__HResults.COR_E_HOSTPROTECTION);
            m_protected = protectedResources;
            m_demanded = demandedResources;
        }

        // Called from the VM to create a HP Exception
        private HostProtectionException(HostProtectionResource protectedResources, HostProtectionResource demandedResources)
            : base(SecurityException.GetResString("HostProtection_HostProtection"))
        {
            SetErrorCode(__HResults.COR_E_HOSTPROTECTION);
            m_protected = protectedResources;
            m_demanded = demandedResources;
        }


        public HostProtectionResource ProtectedResources
        {
            get
            {
                return m_protected;
            }
        }

        public HostProtectionResource DemandedResources
        {
            get
            {
                return m_demanded;
            }
        }

        private String ToStringHelper(String resourceString, Object attr)
        {
            if (attr == null)
                return "";
            StringBuilder sb = new StringBuilder();
            sb.Append(Environment.NewLine);
            sb.Append(Environment.NewLine);
            sb.Append(Environment.GetResourceString( resourceString ));
            sb.Append(Environment.NewLine);
            sb.Append(attr);
            return sb.ToString();
        }

        public override String ToString() 
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(base.ToString());

            sb.Append(ToStringHelper("HostProtection_ProtectedResources", ProtectedResources));
            sb.Append(ToStringHelper("HostProtection_DemandedResources", DemandedResources));

            return sb.ToString();
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)] 		
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info==null)
                throw new ArgumentNullException("info");

            base.GetObjectData( info, context );

            info.AddValue(ProtectedResourcesName, ProtectedResources, typeof(HostProtectionResource));
            info.AddValue(DemandedResourcesName, DemandedResources, typeof(HostProtectionResource));
        }
    }
}
