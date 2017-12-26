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
// A HostSecurityManager gives a hosting application the chance to 
// participate in the security decisions in the AppDomain.
//

namespace System.Security {
    using System.Collections;
    using System.Reflection;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Runtime.Hosting;

    [Flags, Serializable()]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum HostSecurityManagerOptions {
        None                            = 0x0000,
        HostAppDomainEvidence           = 0x0001,
        HostPolicyLevel                 = 0x0002,
        HostAssemblyEvidence            = 0x0004,
        HostDetermineApplicationTrust   = 0x0008,
        HostResolvePolicy               = 0x0010,
        AllFlags                        = 0x001F
    }

    [Serializable]
    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.Infrastructure)]
    [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags=SecurityPermissionFlag.Infrastructure)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public class HostSecurityManager {
        public HostSecurityManager () {}

        // The host can choose which events he wants to participate in. This property can be set when
        // the host only cares about a subset of the capabilities exposed through the HostSecurityManager.
        public virtual HostSecurityManagerOptions Flags {
            get {
                // We use AllFlags as the default.
                return HostSecurityManagerOptions.AllFlags;
            }
        }

        // provide policy for the AppDomain.
        public virtual PolicyLevel DomainPolicy {
            get {
                return null;
            }
        }

        public virtual Evidence ProvideAppDomainEvidence (Evidence inputEvidence) {
            // The default implementation does not modify the input evidence.
            return inputEvidence;
        }

        public virtual Evidence ProvideAssemblyEvidence (Assembly loadedAssembly, Evidence inputEvidence) {
            // The default implementation does not modify the input evidence.
            return inputEvidence;
        }


        public virtual PermissionSet ResolvePolicy (Evidence evidence) {
            return SecurityManager.PolicyManager.ResolveHelper(evidence);
        }
    }
}
