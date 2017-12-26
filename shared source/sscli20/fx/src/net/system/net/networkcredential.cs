//------------------------------------------------------------------------------
// <copyright file="NetworkCredential.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>
//------------------------------------------------------------------------------

namespace System.Net {

    using System.IO;
    using System.Runtime.InteropServices;
    using System.Security.Cryptography;
    using System.Security.Permissions;
    using System.Text;
    using System.Threading;
    using Microsoft.Win32;


    /// <devdoc>
    ///    <para>Provides credentials for password-based
    ///       authentication schemes such as basic, digest, NTLM and Kerberos.</para>
    /// </devdoc>
    public class NetworkCredential : ICredentials,ICredentialsByHost {

        private static EnvironmentPermission m_environmentUserNamePermission;
        private static EnvironmentPermission m_environmentDomainNamePermission;
        private static readonly object lockingObject = new object();
        private string m_userName;
        private string m_password;
        private string m_domain;


        public NetworkCredential() {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.Net.NetworkCredential'/>
        ///       class with name and password set as specified.
        ///    </para>
        /// </devdoc>
        public NetworkCredential(string userName, string password)
        : this(userName, password, string.Empty) {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.Net.NetworkCredential'/>
        ///       class with name and password set as specified.
        ///    </para>
        /// </devdoc>
        public NetworkCredential(string userName, string password, string domain) : this(userName, password, domain, true) {
        }

        /// <devdoc>
        ///    <para>
        ///       Initializes a new instance of the <see cref='System.Net.NetworkCredential'/>
        ///       class with name and password set as specified.
        ///    </para>
        /// </devdoc>


        internal NetworkCredential(string userName, string password, string domain, bool encrypt) {
            UserName = userName;
            Password = password;
            Domain = domain;
        }


        void InitializePart1() {
            if (m_environmentUserNamePermission == null) {
                lock(lockingObject) {
                    if (m_environmentUserNamePermission == null) {
                        m_environmentDomainNamePermission = new EnvironmentPermission(EnvironmentPermissionAccess.Read, "USERDOMAIN");
                        m_environmentUserNamePermission = new EnvironmentPermission(EnvironmentPermissionAccess.Read, "USERNAME");
                    }
                }
            }
        }


        void InitializePart2(){

        }

        /// <devdoc>
        ///    <para>
        ///       The user name associated with this credential.
        ///    </para>
        /// </devdoc>
        public string UserName {
            get {
                InitializePart1();
                m_environmentUserNamePermission.Demand();
                return InternalGetUserName();
            }
            set {
                m_userName = value;
                // GlobalLog.Print("NetworkCredential::set_UserName: m_userName: \"" + m_userName + "\"" );
            }
        }

        /// <devdoc>
        ///    <para>
        ///       The password for the user name.
        ///    </para>
        /// </devdoc>
        public string Password {
            get {
                ExceptionHelper.UnmanagedPermission.Demand();
                return InternalGetPassword();
            }
            set {
                m_password = value;
//                GlobalLog.Print("NetworkCredential::set_Password: m_password: \"" + m_password + "\"" );
            }
        }

        /// <devdoc>
        ///    <para>
        ///       The machine name that verifies
        ///       the credentials. Usually this is the host machine.
        ///    </para>
        /// </devdoc>
        public string Domain {
            get {
                InitializePart1();
                m_environmentDomainNamePermission.Demand();
                return InternalGetDomain();
            }
            set {
                m_domain = value;
//                GlobalLog.Print("NetworkCredential::set_Domain: m_domain: \"" + m_domain + "\"" );
            }
        }

        internal string InternalGetUserName() {
            // GlobalLog.Print("NetworkCredential::get_UserName: returning \"" + m_userName + "\"");
            return m_userName;
        }

        internal string InternalGetPassword() {
            // GlobalLog.Print("NetworkCredential::get_Password: returning \"" + m_password + "\"");
            return m_password;
        }

        internal string InternalGetDomain() {

            // GlobalLog.Print("NetworkCredential::get_Domain: returning \"" + m_domain + "\"");
            return m_domain;
        }

        internal string InternalGetDomainUserName() {
            string domainUserName = InternalGetDomain();
            if (domainUserName.Length != 0)
                domainUserName += "\\";
            domainUserName += InternalGetUserName();
            return domainUserName;
        }

        /// <devdoc>
        ///    <para>
        ///       Returns an instance of the NetworkCredential class for a Uri and
        ///       authentication type.
        ///    </para>
        /// </devdoc>
        public NetworkCredential GetCredential(Uri uri, String authType) {
            return this;
        }

        public NetworkCredential GetCredential(string host, int port, String authenticationType) {
            return this;
        }

        internal bool IsEqualTo(object compObject) {
            if ((object)compObject == null)
                return false;
            if ((object)this == (object)compObject)
                return true;
            NetworkCredential compCred = compObject as NetworkCredential;
            if ((object)compCred == null)
                return false;
            return(InternalGetUserName() == compCred.InternalGetUserName() &&
                   InternalGetPassword() == compCred.InternalGetPassword() &&
                   InternalGetDomain()  == compCred.InternalGetDomain());
        }

        
    } // class NetworkCredential
} // namespace System.Net
