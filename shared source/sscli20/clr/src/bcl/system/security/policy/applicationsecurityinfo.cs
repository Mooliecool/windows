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
// ApplicationSecurityInfo.cs
//
// The application security info holds all the security related information pertinent
// to the application. In some sense, it is the CLR public representation of the security
// information held in the manifest.
//

namespace System.Security.Policy {
    using System.Collections;
    using System.Deployment.Internal.Isolation;
    using System.Deployment.Internal.Isolation.Manifest;
    using System.Globalization;
    using System.Runtime.InteropServices;
    using System.Security.Cryptography;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Security.Util;
    using System.Threading;
    using System.Runtime.Versioning;

    [SecurityPermissionAttribute(SecurityAction.Assert, Flags = SecurityPermissionFlag.UnmanagedCode)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ApplicationSecurityInfo {
        private ActivationContext m_context;
        private object m_appId;
        private object m_deployId;
        private object m_defaultRequest;
        private object m_appEvidence;

        internal ApplicationSecurityInfo () {}

        //
        // Public.
        //

        public ApplicationSecurityInfo (ActivationContext activationContext) {
            if (activationContext == null)
                throw new ArgumentNullException("activationContext");
            m_context = activationContext;
        }

        public ApplicationId ApplicationId {
            get {
                if (m_appId == null && m_context != null) {
                    ICMS appManifest = m_context.ApplicationComponentManifest;
                    ApplicationId appId = ParseApplicationId(appManifest);
                    Interlocked.CompareExchange(ref m_appId, appId, null);
                }
                return m_appId as ApplicationId;
            }
            set {
                if (value == null)
                    throw new ArgumentNullException("value");
                m_appId = value;
            }
        }

        public ApplicationId DeploymentId {
            get {
                if (m_deployId == null && m_context != null) {
                    ICMS deplManifest = m_context.DeploymentComponentManifest;
                    ApplicationId deplId = ParseApplicationId(deplManifest);
                    Interlocked.CompareExchange(ref m_deployId, deplId, null);
                }
                return m_deployId as ApplicationId;
            }
            set {
                if (value == null)
                    throw new ArgumentNullException("value");
                m_deployId = value;
            }
        }

        public PermissionSet DefaultRequestSet {
            [ResourceExposure(ResourceScope.None)]
            [ResourceConsumption(ResourceScope.Machine, ResourceScope.Machine)]
            get {
                if (m_defaultRequest == null) {
                    PermissionSet defaultRequest = new PermissionSet(PermissionState.None);
                    if (m_context != null) {
                        // read the default request from the app manifest.
                        ICMS appManifest = m_context.ApplicationComponentManifest;
                        string defaultPSetId = ((IMetadataSectionEntry) appManifest.MetadataSectionEntry).defaultPermissionSetID;
                        object permissionSetObj = null;
                        if (defaultPSetId != null && defaultPSetId.Length > 0) {
                            ((ISectionWithStringKey) appManifest.PermissionSetSection).Lookup(defaultPSetId, out permissionSetObj);
                            IPermissionSetEntry defaultPSet = permissionSetObj as IPermissionSetEntry;
                            if (defaultPSet != null) {
                                SecurityElement seDefaultPS = SecurityElement.FromString(defaultPSet.AllData.XmlSegment);
                                string unrestricted = seDefaultPS.Attribute("temp:Unrestricted");
                                if (unrestricted != null)
                                    seDefaultPS.AddAttribute("Unrestricted", unrestricted);
                                defaultRequest = new PermissionSet(PermissionState.None);
                                defaultRequest.FromXml(seDefaultPS);

                                // Look for "SameSite" request.
                                string sameSite = seDefaultPS.Attribute("SameSite");
                                if (String.Compare(sameSite, "Site", StringComparison.OrdinalIgnoreCase) == 0) {
                                    NetCodeGroup netCodeGroup = new NetCodeGroup(new AllMembershipCondition());
                                    Url url = new Url(m_context.Identity.CodeBase);
                                    PolicyStatement ps = netCodeGroup.CalculatePolicy(url.GetURLString().Host, url.GetURLString().Scheme, url.GetURLString().Port);
                                    if (ps != null) {
                                        PermissionSet netPermissionSet = ps.PermissionSet;
                                        if (netPermissionSet != null)
                                            defaultRequest.InplaceUnion(netPermissionSet);
                                    }
                                    if (String.Compare("file:", 0, m_context.Identity.CodeBase, 0, 5, StringComparison.OrdinalIgnoreCase) == 0) {
                                        FileCodeGroup fileCodeGroup = new FileCodeGroup(new AllMembershipCondition(), FileIOPermissionAccess.Read | FileIOPermissionAccess.PathDiscovery);
                                        ps = fileCodeGroup.CalculatePolicy(url);
                                        if (ps != null) {
                                            PermissionSet filePermissionSet = ps.PermissionSet;
                                            if (filePermissionSet != null)
                                                defaultRequest.InplaceUnion(filePermissionSet);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Interlocked.CompareExchange(ref m_defaultRequest, defaultRequest, null);
                }
                return m_defaultRequest as PermissionSet;
            }
            set {
                if (value == null)
                    throw new ArgumentNullException("value");
                m_defaultRequest = value;
            }
        }

        public Evidence ApplicationEvidence {
            get {
                if (m_appEvidence == null) {
                    Evidence appEvidence = new Evidence();
                    if (m_context != null) {
                        appEvidence = new Evidence();
                        Url deploymentUrl = new Url(m_context.Identity.CodeBase);
                        appEvidence.AddHost(deploymentUrl);
                        appEvidence.AddHost(Zone.CreateFromUrl(m_context.Identity.CodeBase));
                        if (String.Compare("file:", 0, m_context.Identity.CodeBase, 0, 5, StringComparison.OrdinalIgnoreCase) != 0)
                            appEvidence.AddHost(Site.CreateFromUrl(m_context.Identity.CodeBase));
                        appEvidence.AddHost(new StrongName(new StrongNamePublicKeyBlob(DeploymentId.m_publicKeyToken),
                                                           DeploymentId.Name,
                                                           DeploymentId.Version));
                    }
                    Interlocked.CompareExchange(ref m_appEvidence, appEvidence, null);
                }
                return m_appEvidence as Evidence;
            }
            set {
                if (value == null)
                    throw new ArgumentNullException("value");
                m_appEvidence = value;
            }
        }

        //
        // Internal.
        //

        private static ApplicationId ParseApplicationId (ICMS manifest) {
            if (manifest.Identity == null)
                return null;

            return new ApplicationId(Hex.DecodeHexString(manifest.Identity.GetAttribute("", "publicKeyToken")),
                                     manifest.Identity.GetAttribute("", "name"),
                                     new Version(manifest.Identity.GetAttribute("", "version")),
                                     manifest.Identity.GetAttribute("", "processorArchitecture"),
                                     manifest.Identity.GetAttribute("", "culture"));
        }
    }
}
