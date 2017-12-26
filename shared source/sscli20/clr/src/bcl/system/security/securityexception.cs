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
** Class: SecurityException
**
**
** Purpose: Exception class for security
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
    using System.Security.Policy;
    using System.IO;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Globalization;
    using System.Security.Util;

    [System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] public class SecurityException : SystemException
    {
        private String m_debugString; // NOTE: If you change the name of this field, you'll have to update SOS as well!
        private SecurityAction m_action;
        [NonSerialized] private Type m_typeOfPermissionThatFailed;
        private String m_permissionThatFailed;
        private String m_demanded;
        private String m_granted;
        private String m_refused;
        private String m_denied;
        private String m_permitOnly;
        private AssemblyName m_assemblyName;
        private byte[] m_serializedMethodInfo;
        private String m_strMethodInfo;
        private SecurityZone m_zone;
        private String m_url;

        private const String ActionName = "Action";
        private const String FirstPermissionThatFailedName = "FirstPermissionThatFailed";
        private const String DemandedName = "Demanded";
        private const String GrantedSetName = "GrantedSet";
        private const String RefusedSetName = "RefusedSet";
        private const String DeniedName = "Denied";
        private const String PermitOnlyName = "PermitOnly";
        private const String Assembly_Name = "Assembly";
        private const String MethodName_Serialized = "Method";
        private const String MethodName_String = "Method_String";
        private const String ZoneName = "Zone";
        private const String UrlName = "Url";

        internal static string GetResString(string sResourceName)
        {
            PermissionSet.s_fullTrust.Assert();
            return Environment.GetResourceString(sResourceName);
        }

        internal static Exception MakeSecurityException(AssemblyName asmName, Evidence asmEvidence, PermissionSet granted, PermissionSet refused, RuntimeMethodHandle rmh, SecurityAction action, Object demand, IPermission permThatFailed)
        {
            // See if we need to throw a HostProtectionException instead
            HostProtectionPermission hostProtectionPerm = permThatFailed as HostProtectionPermission;
            if(hostProtectionPerm != null)
                return new HostProtectionException(GetResString("HostProtection_HostProtection"), HostProtectionPermission.protectedResources, hostProtectionPerm.Resources);

            // Produce relevant strings
            String message = "";
            MethodInfo method = null;
            try
            {
                if(granted == null && refused == null && demand == null)
                {
                    message = GetResString("Security_NoAPTCA");
                }
                else
                {
                    if(demand != null && demand is IPermission)
                        message = String.Format(CultureInfo.InvariantCulture,  GetResString("Security_Generic"), demand.GetType().AssemblyQualifiedName );
                    else if (permThatFailed != null)
                        message = String.Format(CultureInfo.InvariantCulture, GetResString("Security_Generic"), permThatFailed.GetType().AssemblyQualifiedName);
                    else
                        message = GetResString("Security_GenericNoType");
                }

                method = SecurityRuntime.GetMethodInfo(rmh);
            }
            catch(Exception e)
            {
                // Environment.GetResourceString will throw if we are ReadyForAbort (thread abort).  (We shouldn't do a BCLDebug.Assert in this case or it will lock up the thread.)
                if(e is System.Threading.ThreadAbortException)
                throw;
            }

/*            catch(System.Threading.ThreadAbortException)
            {
                // Environment.GetResourceString will throw if we are ReadyForAbort (thread abort).  (We shouldn't do a BCLDebug.Assert in this case or it will lock up the thread.)
                throw;
            }
            catch
            {
            }
*/
            // make the exception object
            return new SecurityException(message, asmName, granted, refused, method, action, demand, permThatFailed, asmEvidence);
        }

        private static byte[] ObjectToByteArray(Object obj)
        {
            if(obj == null)
                return null;
            MemoryStream stream = new MemoryStream();
            BinaryFormatter formatter = new BinaryFormatter();
            try {
                formatter.Serialize(stream, obj);
                byte[] array = stream.ToArray();
                return array;
            } catch (NotSupportedException) {
                // Serialization of certain methods is not supported (namely
                // global methods, since they have no representation outside of
                // a module scope).
                return null;
            }
        }

        private static Object ByteArrayToObject(byte[] array)
        {
            if(array == null || array.Length == 0)
                return null;
            MemoryStream stream = new MemoryStream(array);
            BinaryFormatter formatter = new BinaryFormatter();
            Object obj = formatter.Deserialize(stream);
            return obj;
        }

        public SecurityException() 
            : base(GetResString("Arg_SecurityException"))
        {
            SetErrorCode(System.__HResults.COR_E_SECURITY);
        }
    
        public SecurityException(String message) 
            : base(message)
        {
            // This is the constructor that gets called if you Assert but don't have permission to Assert.  (So don't assert in here.)
            SetErrorCode(System.__HResults.COR_E_SECURITY);
        }

        public SecurityException(String message, Type type ) 
            : base(message)
        {
            PermissionSet.s_fullTrust.Assert();
            SetErrorCode(System.__HResults.COR_E_SECURITY);
            m_typeOfPermissionThatFailed = type;
        }

        // *** Don't use this constructor internally ***
        public SecurityException(String message, Type type, String state ) 
            : base(message)
        {
            PermissionSet.s_fullTrust.Assert();
            SetErrorCode(System.__HResults.COR_E_SECURITY);
            m_typeOfPermissionThatFailed = type;
            m_demanded = state;
        }

        public SecurityException(String message, Exception inner) 
            : base(message, inner)
        {
    		SetErrorCode(System.__HResults.COR_E_SECURITY);
        }

        // *** Don't use this constructor internally ***
        internal SecurityException( PermissionSet grantedSetObj, PermissionSet refusedSetObj )
            : base(GetResString("Arg_SecurityException"))
        {
            PermissionSet.s_fullTrust.Assert();
            SetErrorCode(System.__HResults.COR_E_SECURITY);
            if (grantedSetObj != null)
                m_granted = grantedSetObj.ToXml().ToString();
            if (refusedSetObj != null)
                m_refused = refusedSetObj.ToXml().ToString();
        }
    
        // *** Don't use this constructor internally ***
        internal SecurityException( String message, PermissionSet grantedSetObj, PermissionSet refusedSetObj )
            : base(message)
        {
            PermissionSet.s_fullTrust.Assert();
            SetErrorCode(System.__HResults.COR_E_SECURITY);
            if (grantedSetObj != null)
                m_granted = grantedSetObj.ToXml().ToString();
            if (refusedSetObj != null)
                m_refused = refusedSetObj.ToXml().ToString();
        }

        protected SecurityException(SerializationInfo info, StreamingContext context) : base (info, context)
        {
            if (info==null)
                throw new ArgumentNullException("info");

            try
            {
                m_action = (SecurityAction)info.GetValue(ActionName, typeof(SecurityAction));
                m_permissionThatFailed = (String)info.GetValueNoThrow(FirstPermissionThatFailedName, typeof(String));
                m_demanded = (String)info.GetValueNoThrow(DemandedName, typeof(String));
                m_granted = (String)info.GetValueNoThrow(GrantedSetName, typeof(String));
                m_refused = (String)info.GetValueNoThrow(RefusedSetName, typeof(String));
                m_denied = (String)info.GetValueNoThrow(DeniedName, typeof(String));
                m_permitOnly = (String)info.GetValueNoThrow(PermitOnlyName, typeof(String));
                m_assemblyName = (AssemblyName)info.GetValueNoThrow(Assembly_Name, typeof(AssemblyName));
                m_serializedMethodInfo = (byte[])info.GetValueNoThrow(MethodName_Serialized, typeof(byte[]));
                m_strMethodInfo = (String)info.GetValueNoThrow(MethodName_String, typeof(String));
                m_zone = (SecurityZone)info.GetValue(ZoneName, typeof(SecurityZone));
                m_url = (String)info.GetValueNoThrow(UrlName, typeof(String));
            }
            catch 
            {
                m_action = 0;
                m_permissionThatFailed = "";
                m_demanded = "";
                m_granted = "";
                m_refused = "";
                m_denied = "";
                m_permitOnly = "";
                m_assemblyName = null;
                m_serializedMethodInfo = null;
                m_strMethodInfo = null;
                m_zone = SecurityZone.NoZone;
                m_url = "";
            }
        }

        // ------------------------------------------
        // | For failures due to insufficient grant |
        // ------------------------------------------
        public SecurityException(string message, AssemblyName assemblyName, PermissionSet grant, PermissionSet refused, MethodInfo method, SecurityAction action, Object demanded, IPermission permThatFailed, Evidence evidence)
            : base(message)
        {
            PermissionSet.s_fullTrust.Assert();
            SetErrorCode(System.__HResults.COR_E_SECURITY);
            Action = action;
            if(permThatFailed != null)
                m_typeOfPermissionThatFailed = permThatFailed.GetType();
            FirstPermissionThatFailed = permThatFailed;
            Demanded = demanded;
            m_granted = (grant == null ? "" : grant.ToXml().ToString());
            m_refused = (refused == null ? "" : refused.ToXml().ToString());
            m_denied = "";
            m_permitOnly = "";
            m_assemblyName = assemblyName;
            Method = method;
            m_url = "";
            m_zone = SecurityZone.NoZone;
            if(evidence != null)
            {
                Url url = (Url)evidence.FindType(typeof(Url));
                if(url != null)
                    m_url = url.GetURLString().ToString();
                Zone zone = (Zone)evidence.FindType(typeof(Zone));
                if(zone != null)
                    m_zone = zone.SecurityZone;
            }
            m_debugString = this.ToString(true, false);
        }

        // ------------------------------------------
        // | For failures due to deny or PermitOnly |
        // ------------------------------------------
        public SecurityException(string message, Object deny, Object permitOnly, MethodInfo method, Object demanded, IPermission permThatFailed)
            : base(message)
        {
            PermissionSet.s_fullTrust.Assert();
            SetErrorCode(System.__HResults.COR_E_SECURITY);
            Action = SecurityAction.Demand;
            if(permThatFailed != null)
                m_typeOfPermissionThatFailed = permThatFailed.GetType();
            FirstPermissionThatFailed = permThatFailed;
            Demanded = demanded;
            m_granted = "";
            m_refused = "";
            DenySetInstance = deny;
            PermitOnlySetInstance = permitOnly;
            m_assemblyName = null;
            Method = method;
            m_zone = SecurityZone.NoZone;
            m_url = "";
            m_debugString = this.ToString(true, false);
        }











        [System.Runtime.InteropServices.ComVisible(false)]
        public SecurityAction Action
        {
            get
            {
                return m_action;
            }

            set
            {
                m_action = value;
            }
        }

        public Type PermissionType
        {
            get
            {
                if(m_typeOfPermissionThatFailed == null)
                {
                    Object ob = XMLUtil.XmlStringToSecurityObject(m_permissionThatFailed);
                    if(ob == null)
                        ob = XMLUtil.XmlStringToSecurityObject(m_demanded);
                    if(ob != null)
                        m_typeOfPermissionThatFailed = ob.GetType();
                }
                return m_typeOfPermissionThatFailed;
            }

            set
            {
                m_typeOfPermissionThatFailed = value;
            }
        }

        public IPermission FirstPermissionThatFailed
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return (IPermission)XMLUtil.XmlStringToSecurityObject(m_permissionThatFailed);
            }

            set
            {
                m_permissionThatFailed = XMLUtil.SecurityObjectToXmlString(value);
            }
        }

        public String PermissionState
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return m_demanded;
            }

            set
            {
                m_demanded = value;
            }
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public Object Demanded
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return XMLUtil.XmlStringToSecurityObject(m_demanded);
            }

            set
            {
                m_demanded = XMLUtil.SecurityObjectToXmlString(value);
            }
        }

        public String GrantedSet
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return m_granted;
            }

            set
            {
                m_granted = value;
            }
        }

        public String RefusedSet
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return m_refused;
            }

            set
            {
                m_refused = value;
            }
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public Object DenySetInstance
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return XMLUtil.XmlStringToSecurityObject(m_denied);
            }

            set
            {
                m_denied = XMLUtil.SecurityObjectToXmlString(value);
            }
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public Object PermitOnlySetInstance
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return XMLUtil.XmlStringToSecurityObject(m_permitOnly);
            }

            set
            {
                m_permitOnly = XMLUtil.SecurityObjectToXmlString(value);
            }
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public AssemblyName FailedAssemblyInfo
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return m_assemblyName;
            }

            set
            {
                m_assemblyName = value;
            }
            }

        private MethodInfo getMethod()
        {
            return (MethodInfo)ByteArrayToObject(m_serializedMethodInfo);
        }

        [System.Runtime.InteropServices.ComVisible(false)]
        public MethodInfo Method
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return getMethod();
            }

            set
            {
                RuntimeMethodInfo m = value as RuntimeMethodInfo;
                m_serializedMethodInfo = ObjectToByteArray(m);
                if (m != null)
                {
                    m_strMethodInfo = m.ToString();
                }
            }
        }

        public SecurityZone Zone
        {
            get
            {
                return m_zone;
            }

            set
            {
                m_zone = value;
            }
        }

        public String Url
        {
            [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy)]
            get
            {
                return m_url;
            }

            set
            {
                m_url = value;
            }
        }

        private void ToStringHelper(StringBuilder sb, String resourceString, Object attr)
        {
            if (attr == null)
                return;
            String attrString = attr as String;
            if (attrString == null)
                attrString = attr.ToString();
            if (attrString.Length == 0)
                return;
            sb.Append(Environment.NewLine);
            sb.Append(GetResString(resourceString));
            sb.Append(Environment.NewLine);
            sb.Append(attrString);
        }

        private String ToString(bool includeSensitiveInfo, bool includeBaseInfo)
        {
            PermissionSet.s_fullTrust.Assert();
            StringBuilder sb = new StringBuilder();

            if(includeBaseInfo)
            sb.Append(base.ToString());
            if(Action > 0)
                ToStringHelper(sb, "Security_Action", Action);
            ToStringHelper(sb, "Security_TypeFirstPermThatFailed", PermissionType);
            if(includeSensitiveInfo)
            {
                ToStringHelper(sb, "Security_FirstPermThatFailed", m_permissionThatFailed);
                ToStringHelper(sb, "Security_Demanded", m_demanded);
                ToStringHelper(sb, "Security_GrantedSet", m_granted);
                ToStringHelper(sb, "Security_RefusedSet", m_refused);
                ToStringHelper(sb, "Security_Denied", m_denied);
                ToStringHelper(sb, "Security_PermitOnly", m_permitOnly);
                ToStringHelper(sb, "Security_Assembly", m_assemblyName);
                ToStringHelper(sb, "Security_Method", m_strMethodInfo);
            }
            if(m_zone != SecurityZone.NoZone)
                ToStringHelper(sb, "Security_Zone", m_zone);
            if(includeSensitiveInfo)
                ToStringHelper(sb, "Security_Url", m_url);
            return sb.ToString();
        }

        private bool CanAccessSensitiveInfo()
        {
            bool retVal = false;
            try
            {
                new SecurityPermission(SecurityPermissionFlag.ControlEvidence | SecurityPermissionFlag.ControlPolicy).Demand();
                retVal = true;
            }
            catch(SecurityException)
            {
            }
            return retVal;
            }

        public override String ToString() 
        {
            return ToString(CanAccessSensitiveInfo(), true);
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.SerializationFormatter)]
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            if (info==null)
                throw new ArgumentNullException("info");

            base.GetObjectData( info, context );

            info.AddValue(ActionName, m_action, typeof(SecurityAction));
            info.AddValue(FirstPermissionThatFailedName, m_permissionThatFailed, typeof(String));
            info.AddValue(DemandedName, m_demanded, typeof(String));
            info.AddValue(GrantedSetName, m_granted, typeof(String));
            info.AddValue(RefusedSetName, m_refused, typeof(String));
            info.AddValue(DeniedName, m_denied, typeof(String));
            info.AddValue(PermitOnlyName, m_permitOnly, typeof(String));
            info.AddValue(Assembly_Name, m_assemblyName, typeof(AssemblyName));
            info.AddValue(MethodName_Serialized, m_serializedMethodInfo, typeof(byte[]));
            info.AddValue(MethodName_String, m_strMethodInfo, typeof(String));
            info.AddValue(ZoneName, m_zone, typeof(SecurityZone));
            info.AddValue(UrlName, m_url, typeof(String));
        }
    }
}
