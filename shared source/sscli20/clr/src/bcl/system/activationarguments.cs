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

namespace System.Runtime.Hosting {
    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ActivationArguments {
        private ActivationArguments () {}

        // This boolean is used to smuggle the information about whether
        // AppDomainSetup was constructed from an ActivationContext.
        private bool m_useFusionActivationContext = false;
        internal bool UseFusionActivationContext {
            get {
                return m_useFusionActivationContext;
            }
        }

        // This is used to indicate whether the instance is to be activated
        // during the new domain's initialization. CreateInstanceHelper sets
        // this flag to true; CreateDomainHelper never activates the application.
        private bool m_activateInstance = false;
        internal bool ActivateInstance {
            get {
                return m_activateInstance;
            }
            set {
                m_activateInstance = value;
            }
        }

        private string m_appFullName;
        internal string ApplicationFullName {
            get {
                return m_appFullName;
            }
        }

        private string[] m_appManifestPaths;
        internal string[] ApplicationManifestPaths {
            get {
                return m_appManifestPaths;
            }
        }


        private string[] m_activationData;
        public string[] ActivationData {
            get {
                return m_activationData;
            }
        }
    }
}
