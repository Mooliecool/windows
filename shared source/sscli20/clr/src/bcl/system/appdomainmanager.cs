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

//
// An AppDomainManager gives a hosting application the chance to 
// participate in the creation and control the settings of new AppDomains.
//

namespace System {
    using System.Collections;
    using System.Globalization;
    using System.IO;
    using System.Reflection;
    using System.Runtime.CompilerServices;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Threading;
    using System.Runtime.Hosting;
    
    [Flags]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum AppDomainManagerInitializationOptions {
        None             = 0x0000,
        RegisterWithHost = 0x0001
    }

    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
    [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public class AppDomainManager : MarshalByRefObject {
        public AppDomainManager () {}

        public virtual AppDomain CreateDomain (string friendlyName,
                                               Evidence securityInfo,
                                               AppDomainSetup appDomainInfo) {
            return CreateDomainHelper(friendlyName, securityInfo, appDomainInfo);
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
        [SecurityPermissionAttribute(SecurityAction.Demand, ControlAppDomain = true)]
        protected static AppDomain CreateDomainHelper (string friendlyName,
                                                       Evidence securityInfo,
                                                       AppDomainSetup appDomainInfo) {
            if (friendlyName == null)
                throw new ArgumentNullException(Environment.GetResourceString("ArgumentNull_String"));

            // If evidence is provided, we check to make sure that is allowed.
            if (securityInfo != null)
                new SecurityPermission(SecurityPermissionFlag.ControlEvidence).Demand();

            return AppDomain.nCreateDomain(friendlyName,
                                           appDomainInfo,
                                           securityInfo,
                                           securityInfo == null ? AppDomain.CurrentDomain.InternalEvidence : null,
                                           AppDomain.CurrentDomain.GetSecurityDescriptor());
        }

        public virtual void InitializeNewDomain (AppDomainSetup appDomainInfo) {
            // By default, InitializeNewDomain does nothing.
        }

        private AppDomainManagerInitializationOptions m_flags = AppDomainManagerInitializationOptions.None;
        public AppDomainManagerInitializationOptions InitializationFlags {
            get {
                return m_flags;
            }
            set {
                m_flags = value;
            }
        }


        public virtual HostSecurityManager HostSecurityManager {
            get {
                return null;
            }
        }

        public virtual HostExecutionContextManager HostExecutionContextManager {
            get {
                // By default, the AppDomainManager returns the HostExecutionContextManager.
                return HostExecutionContextManager.GetInternalHostExecutionContextManager();
            }
        }

        private Assembly m_entryAssembly = null;
        public virtual Assembly EntryAssembly {
            get {
                // The default AppDomainManager sets the EntryAssembly depending on whether the
                // AppDomain is a manifest application domain or not. In the first case, we parse
                // the application manifest to find out the entry point assembly and return that assembly.
                // In the second case, we maintain the old behavior by calling nGetEntryAssembly().
                if (m_entryAssembly == null) {
                    AppDomain domain = AppDomain.CurrentDomain;
                        m_entryAssembly = nGetEntryAssembly();
                }
                return m_entryAssembly;
            }
        }

        internal static AppDomainManager CurrentAppDomainManager {
            get {
                return AppDomain.CurrentDomain.DomainManager;
            }
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern void nRegisterWithHost();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern Assembly nGetEntryAssembly();
    }
}
