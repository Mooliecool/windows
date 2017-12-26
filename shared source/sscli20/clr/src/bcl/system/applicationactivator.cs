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
// ApplicationActivator class is the base class that handles activation of Add-ins.
// There is a single designated instance of the ApplicationActivator in each Appdomain 
// to which all Add-in activation calls are routed to. The AppdomainManager for the 
// current Appdomain could provide its own custom ApplicationActivator, or an instance
// of the default ApplicationActivator is created.
//

namespace System.Runtime.Hosting {
    using System.Deployment.Internal.Isolation;
    using System.Deployment.Internal.Isolation.Manifest;
    using System.IO;
    using System.Reflection;
    using System.Runtime.Remoting;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Threading;

    internal sealed class ManifestRunner {
        private AppDomain m_domain;
        private string m_path;
        private string[] m_args;
        private ApartmentState m_apt;
        private Assembly m_assembly;
        private int m_runResult;

        //
        // We need to assert unmanaged code security permission to be
        // able to call ActivationContext.ApplicationDirectory.
        //

        [SecurityPermissionAttribute(SecurityAction.Assert, Unrestricted=true)]
        internal ManifestRunner (AppDomain domain, ActivationContext activationContext) {
            m_domain = domain;

            string file, parameters;
            CmsUtils.GetEntryPoint(activationContext, out file, out parameters);
            if (parameters == null || parameters.Length == 0)
                m_args = new string[0];
            else
                m_args = parameters.Split(' ');

            m_apt = ApartmentState.Unknown;

            // get the 'merged' application directory path.
            string directoryName = activationContext.ApplicationDirectory;
            m_path = Path.Combine(directoryName, file);
        }

        internal Assembly EntryAssembly {
            [FileIOPermissionAttribute(SecurityAction.Assert, Unrestricted=true)]
            [SecurityPermissionAttribute(SecurityAction.Assert, Unrestricted=true)]
            get {
                if (m_assembly == null)
                    m_assembly = Assembly.LoadFrom(m_path);
                return m_assembly;
            }
        }

        private void NewThreadRunner () {
            m_runResult = Run(false);
        }

        private int RunInNewThread () {
            Thread th = new Thread(new ThreadStart(NewThreadRunner));
            th.SetApartmentState(m_apt);
            th.Start();
            th.Join();
            return m_runResult;
        }

        private int Run (bool checkAptModel) {
            if (checkAptModel && m_apt != ApartmentState.Unknown) {
                if (Thread.CurrentThread.GetApartmentState() != ApartmentState.Unknown && Thread.CurrentThread.GetApartmentState() != m_apt)
                    return RunInNewThread();
                Thread.CurrentThread.SetApartmentState(m_apt);
            }
            return m_domain.nExecuteAssembly(EntryAssembly, m_args);
        }

        internal int ExecuteAsAssembly () {
            Object[] attrs = EntryAssembly.EntryPoint.GetCustomAttributes(typeof(STAThreadAttribute), false);
            if (attrs.Length > 0)
                m_apt = ApartmentState.STA;
            attrs = EntryAssembly.EntryPoint.GetCustomAttributes(typeof(MTAThreadAttribute), false);
            if (attrs.Length > 0)
                if (m_apt == ApartmentState.Unknown)
                    m_apt = ApartmentState.MTA;
                else
                    m_apt = ApartmentState.Unknown;
            return Run(true);
        }
    }

    [System.Runtime.InteropServices.ComVisible(true)]
    public class ApplicationActivator {
        public ApplicationActivator () {}

        public virtual ObjectHandle CreateInstance (ActivationContext activationContext) {
            return CreateInstance(activationContext, null);
        }

        public virtual ObjectHandle CreateInstance (ActivationContext activationContext, string[] activationCustomData) {
            if (activationContext == null)
                throw new ArgumentNullException("activationContext");

            if (CmsUtils.CompareIdentities(AppDomain.CurrentDomain.ActivationContext, activationContext)) {
                ManifestRunner runner = new ManifestRunner(AppDomain.CurrentDomain, activationContext);
                return new ObjectHandle(runner.ExecuteAsAssembly());
            }

            AppDomainSetup adSetup = new AppDomainSetup(new ActivationArguments(activationContext, activationCustomData));
            // we inherit the evidence from the calling domain
            return CreateInstanceHelper(adSetup);
        }

        protected static ObjectHandle CreateInstanceHelper (AppDomainSetup adSetup) {
            if (adSetup.ActivationArguments == null)
                throw new ArgumentException(Environment.GetResourceString("Arg_MissingActivationArguments"));

            adSetup.ActivationArguments.ActivateInstance = true;
            // inherit the caller's domain evidence for the activation.
            Evidence activatorEvidence = AppDomain.CurrentDomain.Evidence;
            // add the application identity as an evidence.
            Evidence appEvidence = CmsUtils.MergeApplicationEvidence(null,
                                                                     adSetup.ActivationArguments.ApplicationIdentity,
                                                                     adSetup.ActivationArguments.ActivationContext,
                                                                     adSetup.ActivationArguments.ActivationData);

            HostSecurityManager securityManager = AppDomain.CurrentDomain.HostSecurityManager;
            ApplicationTrust appTrust = securityManager.DetermineApplicationTrust(appEvidence, activatorEvidence, new TrustManagerContext());
            if (appTrust == null || !appTrust.IsApplicationTrustedToRun)
                throw new PolicyException(Environment.GetResourceString("Policy_NoExecutionPermission"), 
                                          System.__HResults.CORSEC_E_NO_EXEC_PERM,
                                          null);

            ObjRef or = AppDomain.nCreateInstance(adSetup.ActivationArguments.ApplicationIdentity.FullName,
                                                  adSetup,
                                                  appEvidence,
                                                  appEvidence == null ? AppDomain.CurrentDomain.InternalEvidence : null,
                                                  AppDomain.CurrentDomain.GetSecurityDescriptor());
            if (or == null)
                return null;
            return RemotingServices.Unmarshal(or) as ObjectHandle;
        }
    }
}
