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
namespace System.Security.Permissions
{

    using System.Security.Util;
    using System.IO;
    using System.Security.Policy;
    using System.Runtime.Remoting.Activation;
    using System.Text;
    using System.Runtime.Serialization.Formatters;
    using System.Threading;
    using System.Runtime.InteropServices;
    using System.Runtime.Remoting;
    using System.Runtime.Serialization;
    using System.Security.Cryptography.X509Certificates;
    using System.Runtime.Versioning;
    
    [Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public enum SecurityAction
    {
        // Demand permission of all caller
        Demand = 2,

        // Assert permission so callers don't need
        Assert = 3,

        // Deny permissions so checks will fail
        Deny = 4,

        // Reduce permissions so check will fail
        PermitOnly = 5,

        // Demand permission of caller
        LinkDemand = 6,
    
        // Demand permission of a subclass
        InheritanceDemand = 7,

        // Request minimum permissions to run
        RequestMinimum = 8,

        // Request optional additional permissions
        RequestOptional = 9,

        // Refuse to be granted these permissions
        RequestRefuse = 10,
    }


    [Serializable(), AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class SecurityAttribute : System.Attribute
    {
        /// <internalonly/>
        internal SecurityAction m_action;
        /// <internalonly/>
        internal bool m_unrestricted;

        protected SecurityAttribute( SecurityAction action ) 
        {
            m_action = action;
        }

        public SecurityAction Action
        {
            get { return m_action; }
            set { m_action = value; }
        }

        public bool Unrestricted
        {
            get { return m_unrestricted; }
            set { m_unrestricted = value; }
        }

        abstract public IPermission CreatePermission();

        internal static unsafe IntPtr FindSecurityAttributeTypeHandle(String typeName)
        {
            PermissionSet.s_fullTrust.Assert();
            Type t = Type.GetType(typeName, false, false);
            if(t == null)
                return IntPtr.Zero;
            IntPtr typeHandle = t.TypeHandle.Value;
            return typeHandle;
        }
    }

    [Serializable(), AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class CodeAccessSecurityAttribute : SecurityAttribute
    {
        protected CodeAccessSecurityAttribute( SecurityAction action )
            : base( action )
        {
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class EnvironmentPermissionAttribute : CodeAccessSecurityAttribute
    {
        private String m_read = null;
        private String m_write = null;
    
        public EnvironmentPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public String Read {
            get { return m_read; }
            set { m_read = value; }
        }
    
        public String Write {
            get { return m_write; }
            set { m_write = value; }
        }

        public String All {
            get { throw new NotSupportedException( Environment.GetResourceString( "NotSupported_GetMethod" ) ); }
            set { m_write = value; m_read = value; }
        }

        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new EnvironmentPermission(PermissionState.Unrestricted);
            }
            else
            {
                EnvironmentPermission perm = new EnvironmentPermission(PermissionState.None);
                if (m_read != null)
                    perm.SetPathList( EnvironmentPermissionAccess.Read, m_read );
                if (m_write != null)
                    perm.SetPathList( EnvironmentPermissionAccess.Write, m_write );
                return perm;
            }
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class FileDialogPermissionAttribute : CodeAccessSecurityAttribute
    {
        private FileDialogPermissionAccess m_access;

        public FileDialogPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public bool Open
        {
            get { return (m_access & FileDialogPermissionAccess.Open) != 0; }
            set { m_access = value ? m_access | FileDialogPermissionAccess.Open : m_access & ~FileDialogPermissionAccess.Open; }
        }
            
        public bool Save
        {
            get { return (m_access & FileDialogPermissionAccess.Save) != 0; }
            set { m_access = value ? m_access | FileDialogPermissionAccess.Save : m_access & ~FileDialogPermissionAccess.Save; }
        }

        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new FileDialogPermission( PermissionState.Unrestricted );
            }
            else
            {
                return new FileDialogPermission( m_access );
            }
        }
    }


    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class FileIOPermissionAttribute : CodeAccessSecurityAttribute
    {
        private String m_read = null;
        private String m_write = null;
        private String m_append = null;
        private String m_pathDiscovery = null;
        private String m_viewAccess = null;
        private String m_changeAccess = null;
        [OptionalField(VersionAdded = 2)] private FileIOPermissionAccess m_allLocalFiles = FileIOPermissionAccess.NoAccess;
        [OptionalField(VersionAdded = 2)] private FileIOPermissionAccess m_allFiles = FileIOPermissionAccess.NoAccess;
    
        public FileIOPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public String Read {
            get { return m_read; }
            set { m_read = value; }
        }
    
        public String Write {
            get { return m_write; }
            set { m_write = value; }
        }

        public String Append {
            get { return m_append; }
            set { m_append = value; }
        }

        public String PathDiscovery {
            get { return m_pathDiscovery; }
            set { m_pathDiscovery = value; }
        }

        public String ViewAccessControl {
            get { return m_viewAccess; }
            set { m_viewAccess = value; }
        }

        public String ChangeAccessControl {
            get { return m_changeAccess; }
            set { m_changeAccess = value; }
        }

        [Obsolete("Please use the ViewAndModify property instead.")]
        public String All {
            set { m_read = value; m_write = value; m_append = value; m_pathDiscovery = value; }
            get { throw new NotSupportedException( Environment.GetResourceString( "NotSupported_GetMethod" ) ); }
        }

        // Read, Write, Append, PathDiscovery, but no ACL-related permissions
        public String ViewAndModify {
            get { throw new NotSupportedException( Environment.GetResourceString( "NotSupported_GetMethod" ) ); }
            set { m_read = value; m_write = value; m_append = value; m_pathDiscovery = value; }
        }

        public FileIOPermissionAccess AllFiles {
            get { return m_allFiles; }
            set { m_allFiles = value; }
        }

        public FileIOPermissionAccess AllLocalFiles {
            get { return m_allLocalFiles; }
            set { m_allLocalFiles = value; }
        }

        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new FileIOPermission(PermissionState.Unrestricted);
            }
            else
            {
                FileIOPermission perm = new FileIOPermission(PermissionState.None);
                if (m_read != null)
                    perm.SetPathList( FileIOPermissionAccess.Read, m_read );
                if (m_write != null)
                    perm.SetPathList( FileIOPermissionAccess.Write, m_write );
                if (m_append != null)
                    perm.SetPathList( FileIOPermissionAccess.Append, m_append );
                if (m_pathDiscovery != null)
                    perm.SetPathList( FileIOPermissionAccess.PathDiscovery, m_pathDiscovery );

                perm.AllFiles = m_allFiles;
                perm.AllLocalFiles = m_allLocalFiles;
                return perm;
            }
        }
    }


    // PrincipalPermissionAttribute currently derives from
    // CodeAccessSecurityAttribute, even though it's not related to code access
    // security. This is because compilers are currently looking for
    // CodeAccessSecurityAttribute as a direct parent class rather than
    // SecurityAttribute as the root class.
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Class, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class PrincipalPermissionAttribute : CodeAccessSecurityAttribute
    {
        private String m_name = null;
        private String m_role = null;
        private bool m_authenticated = true;
    
        public PrincipalPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }
        
        public String Name
        {
            get { return m_name; }
            set { m_name = value; }
        }
        
        public String Role
        {
            get { return m_role; }
            set { m_role = value; }
        }
        
        public bool Authenticated
        {
            get { return m_authenticated; }
            set { m_authenticated = value; }
        }
        
        
        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new PrincipalPermission( PermissionState.Unrestricted );
            }
            else
            {
                return new PrincipalPermission( m_name, m_role, m_authenticated );
            }
        }
    }
                


    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class ReflectionPermissionAttribute : CodeAccessSecurityAttribute
    {
        private ReflectionPermissionFlag m_flag = ReflectionPermissionFlag.NoFlags;

        public ReflectionPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public ReflectionPermissionFlag Flags {
            get { return m_flag; }
            set { m_flag = value; }
        }

        [Obsolete("This API has been deprecated. http://go.microsoft.com/fwlink/?linkid=14202")]
        public bool TypeInformation {
#pragma warning disable 618
            get { return (m_flag & ReflectionPermissionFlag.TypeInformation) != 0; }
            set { m_flag = value ? m_flag | ReflectionPermissionFlag.TypeInformation : m_flag & ~ReflectionPermissionFlag.TypeInformation; }
#pragma warning restore 618
        }

        public bool MemberAccess {
            get { return (m_flag & ReflectionPermissionFlag.MemberAccess) != 0; }
            set { m_flag = value ? m_flag | ReflectionPermissionFlag.MemberAccess : m_flag & ~ReflectionPermissionFlag.MemberAccess; }
        }

        public bool ReflectionEmit {
            get { return (m_flag & ReflectionPermissionFlag.ReflectionEmit) != 0; }
            set { m_flag = value ? m_flag | ReflectionPermissionFlag.ReflectionEmit : m_flag & ~ReflectionPermissionFlag.ReflectionEmit; }
        }

        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new ReflectionPermission( PermissionState.Unrestricted );
            }
            else
            {
                return new ReflectionPermission( m_flag );
            }
        }
    }

    
    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class SecurityPermissionAttribute : CodeAccessSecurityAttribute
    {
        private SecurityPermissionFlag m_flag = SecurityPermissionFlag.NoFlags;
    
        public SecurityPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public SecurityPermissionFlag Flags {
            get { return m_flag; }
            set { m_flag = value; }
        }

        public bool Assertion {
            get { return (m_flag & SecurityPermissionFlag.Assertion) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.Assertion : m_flag & ~SecurityPermissionFlag.Assertion; }
        }

        public bool UnmanagedCode {
            get { return (m_flag & SecurityPermissionFlag.UnmanagedCode) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.UnmanagedCode : m_flag & ~SecurityPermissionFlag.UnmanagedCode; }
        }

        public bool SkipVerification {
            get { return (m_flag & SecurityPermissionFlag.SkipVerification) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.SkipVerification : m_flag & ~SecurityPermissionFlag.SkipVerification; }
        }

        public bool Execution {
            get { return (m_flag & SecurityPermissionFlag.Execution) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.Execution : m_flag & ~SecurityPermissionFlag.Execution; }
        }

        public bool ControlThread {
            get { return (m_flag & SecurityPermissionFlag.ControlThread) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.ControlThread : m_flag & ~SecurityPermissionFlag.ControlThread; }
        }
    
        public bool ControlEvidence {
            get { return (m_flag & SecurityPermissionFlag.ControlEvidence) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.ControlEvidence : m_flag & ~SecurityPermissionFlag.ControlEvidence; }
        }
    
        public bool ControlPolicy {
            get { return (m_flag & SecurityPermissionFlag.ControlPolicy) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.ControlPolicy : m_flag & ~SecurityPermissionFlag.ControlPolicy; }
        }
    
        public bool SerializationFormatter {
            get { return (m_flag & SecurityPermissionFlag.SerializationFormatter) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.SerializationFormatter : m_flag & ~SecurityPermissionFlag.SerializationFormatter; }
        }

        public bool ControlDomainPolicy {
            get { return (m_flag & SecurityPermissionFlag.ControlDomainPolicy) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.ControlDomainPolicy : m_flag & ~SecurityPermissionFlag.ControlDomainPolicy; }
        }
    
        public bool ControlPrincipal {
            get { return (m_flag & SecurityPermissionFlag.ControlPrincipal) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.ControlPrincipal : m_flag & ~SecurityPermissionFlag.ControlPrincipal; }
        }

        public bool ControlAppDomain {
            get { return (m_flag & SecurityPermissionFlag.ControlAppDomain) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.ControlAppDomain : m_flag & ~SecurityPermissionFlag.ControlAppDomain; }
        } 

        public bool RemotingConfiguration {
            get { return (m_flag & SecurityPermissionFlag.RemotingConfiguration) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.RemotingConfiguration : m_flag & ~SecurityPermissionFlag.RemotingConfiguration; }
        }

[System.Runtime.InteropServices.ComVisible(true)]
        public bool Infrastructure {
            get { return (m_flag & SecurityPermissionFlag.Infrastructure) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.Infrastructure : m_flag & ~SecurityPermissionFlag.Infrastructure; }
        }
    
        public bool BindingRedirects {
            get { return (m_flag & SecurityPermissionFlag.BindingRedirects) != 0; }
            set { m_flag = value ? m_flag | SecurityPermissionFlag.BindingRedirects : m_flag & ~SecurityPermissionFlag.BindingRedirects; }
        }

        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new SecurityPermission( PermissionState.Unrestricted );
            }
            else
            {
                return new SecurityPermission( m_flag );
            }
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class UIPermissionAttribute : CodeAccessSecurityAttribute
    {
        private UIPermissionWindow m_windowFlag = UIPermissionWindow.NoWindows;
        private UIPermissionClipboard m_clipboardFlag = UIPermissionClipboard.NoClipboard;
    
        public UIPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public UIPermissionWindow Window {
            get { return m_windowFlag; }
            set { m_windowFlag = value; }
        }

        public UIPermissionClipboard Clipboard {
            get { return m_clipboardFlag; }
            set { m_clipboardFlag = value; }
        }
    
        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new UIPermission( PermissionState.Unrestricted );
            }
            else
            {
                return new UIPermission( m_windowFlag, m_clipboardFlag );
            }
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class ZoneIdentityPermissionAttribute : CodeAccessSecurityAttribute
    {
        private SecurityZone m_flag = SecurityZone.NoZone;
    
        public ZoneIdentityPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public SecurityZone Zone {
            get { return m_flag; }
            set { m_flag = value; }
        }
    
        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new ZoneIdentityPermission(PermissionState.Unrestricted);
            }
            else
            {
                return new ZoneIdentityPermission( m_flag );
            }
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class StrongNameIdentityPermissionAttribute : CodeAccessSecurityAttribute
    {
        private String m_name = null;
        private String m_version = null;
        private String m_blob = null;

        public StrongNameIdentityPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public String Name
        {
            get { return m_name; }
            set { m_name = value; }
        }
        
        public String Version
        {
            get { return m_version; }
            set { m_version = value; }
        }
        
        public String PublicKey
        {
            get { return m_blob; }
            set { m_blob = value; }
        }

        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new StrongNameIdentityPermission( PermissionState.Unrestricted );
            }
            else
            {
                if (m_blob == null && m_name == null && m_version == null)
                    return new StrongNameIdentityPermission( PermissionState.None );
            
                if (m_blob == null)
                    throw new ArgumentException( Environment.GetResourceString("ArgumentNull_Key"));
                    
                StrongNamePublicKeyBlob blob = new StrongNamePublicKeyBlob( m_blob );
                
                if (m_version == null || m_version.Equals(String.Empty))
                    return new StrongNameIdentityPermission( blob, m_name, null );
                else    
                    return new StrongNameIdentityPermission( blob, m_name, new Version( m_version ) );
            }
        }
    }


    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class SiteIdentityPermissionAttribute : CodeAccessSecurityAttribute
    {
        private String m_site = null;
    
        public SiteIdentityPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public String Site {
            get { return m_site; }
            set { m_site = value; }
        }
    
        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new SiteIdentityPermission( PermissionState.Unrestricted );
            }
            else
            {
                if (m_site == null)
                    return new SiteIdentityPermission( PermissionState.None );
            
                return new SiteIdentityPermission( m_site );
            }
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable] sealed public class UrlIdentityPermissionAttribute : CodeAccessSecurityAttribute
    {
        private String m_url = null;
    
        public UrlIdentityPermissionAttribute( SecurityAction action )
            : base( action )
        {
        }

        public String Url {
            get { return m_url; }
            set { m_url = value; }
        }
    
        public override IPermission CreatePermission()
        {
            if (m_unrestricted)
            {
                return new UrlIdentityPermission( PermissionState.Unrestricted );
            }
            else
            {
                if (m_url == null)
                    return new UrlIdentityPermission( PermissionState.None );
                    
                return new UrlIdentityPermission( m_url );
            }
        }
    }
    

    [Serializable(), AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor
     | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly,
    AllowMultiple=true, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class IsolatedStoragePermissionAttribute : CodeAccessSecurityAttribute
    {
        /// <internalonly/>
        internal long m_userQuota;
        /// <internalonly/>
        internal IsolatedStorageContainment m_allowed;
        protected IsolatedStoragePermissionAttribute(SecurityAction action) : base(action)
        {
        }

        // properties
        public long UserQuota {
            set{
                m_userQuota = value;
            }
            get{
                return m_userQuota;
            }
        }
        public IsolatedStorageContainment UsageAllowed {
            set{
                m_allowed = value;
            }
            get{
                return m_allowed;
            }
        }

    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor
     | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly,
    AllowMultiple=true, Inherited=false)]
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class IsolatedStorageFilePermissionAttribute : IsolatedStoragePermissionAttribute
    {
        public IsolatedStorageFilePermissionAttribute(SecurityAction action) : base(action)
        {

        }
        public override IPermission CreatePermission()
        {
            IsolatedStorageFilePermission p;
            if (m_unrestricted) {
                p = new IsolatedStorageFilePermission
                        (PermissionState.Unrestricted);
            } else {
                p = new IsolatedStorageFilePermission(PermissionState.None);
                p.UserQuota      = m_userQuota;
                p.UsageAllowed   = m_allowed;
            }
            return p;
        }
    }

    [AttributeUsage(AttributeTargets.Method | AttributeTargets.Constructor | AttributeTargets.Class | AttributeTargets.Struct | AttributeTargets.Assembly, AllowMultiple = true, Inherited = false )] 
[System.Runtime.InteropServices.ComVisible(true)]
    [Serializable()] sealed public class PermissionSetAttribute : CodeAccessSecurityAttribute
    {
        private String m_file;
        private String m_name;
        private bool m_unicode;
        private String m_xml;
        private String m_hex;

        public PermissionSetAttribute( SecurityAction action )
            : base( action )
        {
            m_unicode = false;
        }

        public String File {
            get { return m_file; }
            [ResourceExposure(ResourceScope.Machine)]
            [ResourceConsumption(ResourceScope.Machine)]
            set { m_file = value; }
        }
    
        public bool UnicodeEncoded {
            get { return m_unicode; }
            set { m_unicode = value; }
        }
        
        public String Name {
            get { return m_name; }
            set { m_name = value; }
        }
        
        public String XML {
            get { return m_xml; }
            set { m_xml = value; }
        }       

        public String Hex {
            get { return m_hex; }
            set { m_hex = value; }
        }

        public override IPermission CreatePermission()
        {
            return null;
        }

        private PermissionSet BruteForceParseStream(Stream stream)
        {
            Encoding[] encodings = new Encoding[] { Encoding.UTF8, Encoding.ASCII, Encoding.Unicode };

            StreamReader reader = null;
            Exception exception = null;

            for (int i = 0; reader == null && i < encodings.Length; ++i)
            {
                try
                {
                    stream.Position = 0;
                    reader = new StreamReader( stream, encodings[i] );

                    return ParsePermissionSet( new Parser(reader) );
                }
                catch (Exception e1)
                {
                    if (exception == null)
                        exception = e1;
                }
            }

            throw exception;
        }

        private PermissionSet ParsePermissionSet(Parser parser)
        {
            SecurityElement e = parser.GetTopElement();
            PermissionSet permSet = new PermissionSet( PermissionState.None );
            permSet.FromXml( e );

            return permSet;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public PermissionSet CreatePermissionSet()
        {
            if (m_unrestricted)
                return new PermissionSet( PermissionState.Unrestricted );
            else if (m_name != null)
                return PolicyLevel.GetBuiltInSet( m_name );
            else if (m_xml != null)
                return ParsePermissionSet( new Parser(m_xml.ToCharArray()) );
            else if (m_hex != null)
                return BruteForceParseStream( new MemoryStream(Util.Hex.DecodeHexString(m_hex)) );
            else if (m_file != null)
                return BruteForceParseStream( new FileStream( m_file, FileMode.Open, FileAccess.Read) );
            else
                return new PermissionSet( PermissionState.None );
        }
    }
}
