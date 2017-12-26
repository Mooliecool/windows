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
// IApplicationTrustManager.cs
//

namespace System.Security.Policy {

    //
    // Interface that defines an IApplicationTrustManager. An IApplicationTrustManager handles application security decisions
    // when there is no stored policy for that app, be this by prompting the user, checking a web service, or other means.
    //

    [System.Runtime.InteropServices.ComVisible(true)]
    public interface IApplicationTrustManager : ISecurityEncodable {
        ApplicationTrust DetermineApplicationTrust (ActivationContext activationContext, TrustManagerContext context);
    }

    //
    // This enumeration provides a hint to the trust manager as to the UI it should provide for the trust decision.
    //

    [System.Runtime.InteropServices.ComVisible(true)]
    public enum TrustManagerUIContext {
        Install,
        Upgrade,
        Run
    }

    //        

    [System.Runtime.InteropServices.ComVisible(true)]
    public class TrustManagerContext {
        private bool m_ignorePersistedDecision;
        private TrustManagerUIContext m_uiContext;
        private bool m_noPrompt;
        private bool m_keepAlive;
        private bool m_persist;
        private ApplicationIdentity m_appId;

        public TrustManagerContext () : this (TrustManagerUIContext.Run) {}

        public TrustManagerContext (TrustManagerUIContext uiContext) {
            m_ignorePersistedDecision = false;
            m_uiContext = uiContext;
            m_keepAlive = false;
            m_persist = true;
        }

        public virtual TrustManagerUIContext UIContext {
            get {
                return m_uiContext;
            }
            set {
                m_uiContext = value;
            }
        }

        public virtual bool NoPrompt {
            get {
                return m_noPrompt;
            }
            set {
                m_noPrompt = value;
            }
        }

        public virtual bool IgnorePersistedDecision {
            get {
                return m_ignorePersistedDecision;
            }
            set {
                m_ignorePersistedDecision = value;
            }
        }

        public virtual bool KeepAlive {
            get {
                return m_keepAlive;
            }
            set {
                m_keepAlive = value;
            }
        }

        public virtual bool Persist {
            get {
                return m_persist;
            }
            set {
                m_persist = value;
            }
        }

        public virtual ApplicationIdentity PreviousApplicationIdentity {
            get {
                return m_appId;
            }
            set {
                m_appId = value;
            }
        }
    }
}
