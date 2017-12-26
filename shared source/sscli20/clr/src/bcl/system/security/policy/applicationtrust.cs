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
// ApplicationTrust.cs
//
// This class encapsulates security decisions about an application.
//

namespace System.Security.Policy {
    using System.Collections;
    using System.Deployment.Internal.Isolation;
    using System.Deployment.Internal.Isolation.Manifest;
    using System.Globalization;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Runtime.Serialization.Formatters.Binary;
    using System.Security.Permissions;
    using System.Security.Util;
    using System.Text;
    using System.Threading;

    [System.Runtime.InteropServices.ComVisible(true)]
    public enum ApplicationVersionMatch {
        MatchExactVersion,
        MatchAllVersions
    }

    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ApplicationTrust : ISecurityEncodable {
        private ApplicationIdentity m_appId;
        private bool m_appTrustedToRun;
        private bool m_persist;
        private object m_extraInfo;
        private SecurityElement m_elExtraInfo;
        private PolicyStatement m_psDefaultGrant;
        private StrongName[] m_fullTrustAssemblies;

        public ApplicationTrust (ApplicationIdentity applicationIdentity) : this () {
            ApplicationIdentity = applicationIdentity;
        }

        public ApplicationTrust () : this (new PermissionSet(PermissionState.None)) {}
        internal ApplicationTrust (PermissionSet defaultGrantSet) : this (defaultGrantSet, null) {}
        internal ApplicationTrust (PermissionSet defaultGrantSet, StrongName[] fullTrustAssemblies) {
            DefaultGrantSet = new PolicyStatement(defaultGrantSet);
            FullTrustAssemblies = fullTrustAssemblies;
        }

        public ApplicationIdentity ApplicationIdentity {
            get {
                return m_appId;
            }
            set {
                if (value == null)
                    throw new ArgumentNullException(Environment.GetResourceString("Argument_InvalidAppId"));
                m_appId = value;
            }
        }

        public PolicyStatement DefaultGrantSet {
            get {
                if (m_psDefaultGrant == null)
                    return new PolicyStatement(new PermissionSet(PermissionState.None));
                return m_psDefaultGrant;
            }
            set {
                if (value == null)
                    m_psDefaultGrant = null;
                else
                    m_psDefaultGrant = value;
            }
        }

        internal StrongName[] FullTrustAssemblies {
            get {
                return m_fullTrustAssemblies;
            }
            set {
                m_fullTrustAssemblies = value;
            }
        }

        public bool IsApplicationTrustedToRun {
            get {
                return m_appTrustedToRun;
            }
            set {
                m_appTrustedToRun = value;
            }
        }

        public bool Persist {
            get {
                return m_persist;
            }
            set {
                m_persist = value;
            }
        }

        public object ExtraInfo {
            get {
                if (m_elExtraInfo != null) {
                    m_extraInfo = ObjectFromXml(m_elExtraInfo);
                    m_elExtraInfo = null;
                }
                return m_extraInfo;
            }
            set {
                m_elExtraInfo = null;
                m_extraInfo = value;
            }
        }

        public SecurityElement ToXml () {
            SecurityElement elRoot = new SecurityElement("ApplicationTrust");
            elRoot.AddAttribute("version", "1");

            if (m_appId != null)
                elRoot.AddAttribute("FullName", SecurityElement.Escape(m_appId.FullName));
            if (m_appTrustedToRun)
                elRoot.AddAttribute("TrustedToRun", "true");
            if (m_persist)
                elRoot.AddAttribute("Persist", "true");

            if (m_psDefaultGrant != null) {
                SecurityElement elDefaultGrant = new SecurityElement("DefaultGrant");
                elDefaultGrant.AddChild(m_psDefaultGrant.ToXml());
                elRoot.AddChild(elDefaultGrant);
            }
            if (m_fullTrustAssemblies != null) {
                SecurityElement elFullTrustAssemblies = new SecurityElement("FullTrustAssemblies");
                for (int index = 0; index < m_fullTrustAssemblies.Length; index++) {
                    if (m_fullTrustAssemblies[index] != null) 
                        elFullTrustAssemblies.AddChild(m_fullTrustAssemblies[index].ToXml());
                }
                elRoot.AddChild(elFullTrustAssemblies);
            }
            if (ExtraInfo != null)
                elRoot.AddChild(ObjectToXml("ExtraInfo", ExtraInfo));

            return elRoot;
        }

        public void FromXml (SecurityElement element) {
            if (element == null)
                throw new ArgumentNullException("element");
            if (String.Compare(element.Tag, "ApplicationTrust", StringComparison.Ordinal) != 0)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidXML"));

            m_psDefaultGrant = null;
            m_fullTrustAssemblies = null;
            m_appTrustedToRun = false;

            string isAppTrustedToRun = element.Attribute("TrustedToRun");
            if (isAppTrustedToRun != null && String.Compare(isAppTrustedToRun, "true", StringComparison.Ordinal) == 0)
                m_appTrustedToRun = true;
            string persist = element.Attribute("Persist");
            if (persist != null && String.Compare(persist, "true", StringComparison.Ordinal) == 0)
                m_persist = true;

            string fullName = element.Attribute("FullName");
            if (fullName != null && fullName.Length > 0)
                m_appId = new ApplicationIdentity(fullName);

            SecurityElement elDefaultGrant = element.SearchForChildByTag("DefaultGrant");
            if (elDefaultGrant != null) {
                SecurityElement elDefaultGrantPS = elDefaultGrant.SearchForChildByTag("PolicyStatement");
                if (elDefaultGrantPS != null) {
                    PolicyStatement ps = new PolicyStatement(null);
                    ps.FromXml(elDefaultGrantPS);
                    m_psDefaultGrant = ps;
                }
            }

            SecurityElement elFullTrustAssemblies = element.SearchForChildByTag("FullTrustAssemblies");
            if (elFullTrustAssemblies != null && elFullTrustAssemblies.InternalChildren != null) {
                m_fullTrustAssemblies = new StrongName[elFullTrustAssemblies.Children.Count];
                IEnumerator enumerator = elFullTrustAssemblies.Children.GetEnumerator();
                int index = 0;
                while (enumerator.MoveNext()) {
                    m_fullTrustAssemblies[index] = new StrongName();
                    m_fullTrustAssemblies[index].FromXml(enumerator.Current as SecurityElement);
                    index++;
                }
            }

            m_elExtraInfo = element.SearchForChildByTag("ExtraInfo");
        }

        private static SecurityElement ObjectToXml (string tag, Object obj) {
            BCLDebug.Assert(obj != null, "You need to pass in an object");

            ISecurityEncodable encodableObj = obj as ISecurityEncodable;

            SecurityElement elObject;
            if (encodableObj != null) {
                elObject = encodableObj.ToXml();
                if (!elObject.Tag.Equals(tag))
                    throw new ArgumentException(Environment.GetResourceString("Argument_InvalidXML"));
            }

            MemoryStream stream = new MemoryStream();
            BinaryFormatter formatter = new BinaryFormatter();
            formatter.Serialize(stream, obj);
            byte[] array = stream.ToArray();

            elObject = new SecurityElement(tag);
            elObject.AddAttribute("Data", Hex.EncodeHexString(array));
            return elObject;
        }


        private static Object ObjectFromXml (SecurityElement elObject) {
            BCLDebug.Assert(elObject != null, "You need to pass in a security element");

            if (elObject.Attribute("class") != null) {
                ISecurityEncodable encodableObj = XMLUtil.CreateCodeGroup(elObject) as ISecurityEncodable;
                if (encodableObj != null) {
                    encodableObj.FromXml(elObject);
                    return encodableObj;
                }
            }

            string objectData = elObject.Attribute("Data");
            MemoryStream stream = new MemoryStream(Hex.DecodeHexString(objectData));
            BinaryFormatter formatter = new BinaryFormatter();
            return formatter.Deserialize(stream);
        }
    }

    [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags=SecurityPermissionFlag.ControlPolicy)]
    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ApplicationTrustCollection : ICollection {
        private const string ApplicationTrustProperty = "ApplicationTrust";
        private const string InstallerIdentifier = "{60051b8f-4f12-400a-8e50-dd05ebd438d1}";
        private static Guid ClrPropertySet = new Guid("c989bb7a-8385-4715-98cf-a741a8edb823");

        // The CLR specific constant install reference.
        private static object s_installReference = null;
        private static StoreApplicationReference InstallReference {
            get {
                if (s_installReference == null) {
                    Interlocked.CompareExchange(ref s_installReference,
                                                new StoreApplicationReference(
                                                    IsolationInterop.GUID_SXS_INSTALL_REFERENCE_SCHEME_OPAQUESTRING,
                                                    InstallerIdentifier,
                                                    null),
                                                null);
                }
                return (StoreApplicationReference) s_installReference;
            }
        }

        private readonly object m_syncRoot = new object();

        private object m_appTrusts = null;
        private ArrayList AppTrusts {
            get {
                if (m_appTrusts == null) {
                    ArrayList appTrusts = new ArrayList();
                    if (m_storeBounded) {
                        RefreshStorePointer();
                        // enumerate the user store and populate the collection
                        StoreDeploymentMetadataEnumeration deplEnum = m_pStore.EnumInstallerDeployments(IsolationInterop.GUID_SXS_INSTALL_REFERENCE_SCHEME_OPAQUESTRING, InstallerIdentifier, ApplicationTrustProperty, null);
                        foreach (IDefinitionAppId defAppId in deplEnum) {
                            StoreDeploymentMetadataPropertyEnumeration metadataEnum = m_pStore.EnumInstallerDeploymentProperties(IsolationInterop.GUID_SXS_INSTALL_REFERENCE_SCHEME_OPAQUESTRING, InstallerIdentifier, ApplicationTrustProperty, defAppId);
                            foreach (StoreOperationMetadataProperty appTrustProperty in metadataEnum) {
                                string appTrustXml = appTrustProperty.Value;
                                if (appTrustXml != null && appTrustXml.Length > 0) {
                                    SecurityElement seTrust = SecurityElement.FromString(appTrustXml);
                                    ApplicationTrust appTrust = new ApplicationTrust();
                                    appTrust.FromXml(seTrust);
                                    appTrusts.Add(appTrust);
                                }
                            }
                        }
                    }
                    Interlocked.CompareExchange(ref m_appTrusts, appTrusts, null);
                }
                return m_appTrusts as ArrayList;
            }
        }

        private bool m_storeBounded = false;
        private Store m_pStore = null; // Component store interface pointer.

        // Only internal constructors are exposed.
        internal ApplicationTrustCollection () : this(false) {}
        internal ApplicationTrustCollection (bool storeBounded) {
            m_storeBounded = storeBounded;
        }

        private void RefreshStorePointer () {
            // Refresh store pointer.
            if (m_pStore != null)
                Marshal.ReleaseComObject(m_pStore.InternalStore);
            m_pStore = IsolationInterop.GetUserStore();
        }

        public int Count {
            get {
                return AppTrusts.Count;
            }
        }

        public ApplicationTrust this[int index] {
            get {
                return AppTrusts[index] as ApplicationTrust;
            }
        }

        public ApplicationTrust this[string appFullName] {
            get {
                ApplicationIdentity identity = new ApplicationIdentity(appFullName);
                ApplicationTrustCollection appTrusts = Find(identity, ApplicationVersionMatch.MatchExactVersion);
                if (appTrusts.Count > 0)
                    return appTrusts[0];
                return null;
            }
        }

        private void CommitApplicationTrust(ApplicationIdentity applicationIdentity, string trustXml) {
            StoreOperationMetadataProperty[] properties = new StoreOperationMetadataProperty[] {
                    new StoreOperationMetadataProperty(ClrPropertySet, ApplicationTrustProperty, trustXml)
                };

            IEnumDefinitionIdentity idenum = applicationIdentity.Identity.EnumAppPath();
            IDefinitionIdentity[] asbId = new IDefinitionIdentity[1];
            IDefinitionIdentity deplId = null;
            if (idenum.Next(1, asbId) == 1)
                deplId = asbId[0];

            IDefinitionAppId defAppId = IsolationInterop.AppIdAuthority.CreateDefinition();
            defAppId.SetAppPath(1, new IDefinitionIdentity[] {deplId});
            defAppId.put_Codebase(applicationIdentity.CodeBase);

            using (StoreTransaction storeTxn = new StoreTransaction()) {
                storeTxn.Add(new StoreOperationSetDeploymentMetadata(defAppId, InstallReference, properties));
                RefreshStorePointer();
                m_pStore.Transact(storeTxn.Operations);
            }

            m_appTrusts = null; // reset the app trusts in the collection.
        }

        public int Add (ApplicationTrust trust) {
            if (trust == null)
                throw new ArgumentNullException("trust");
            if (trust.ApplicationIdentity == null)
                throw new ArgumentException(Environment.GetResourceString("Argument_ApplicationTrustShouldHaveIdentity"));

            // Add the trust decision of the application to the fusion store.
            if (m_storeBounded) {
                CommitApplicationTrust(trust.ApplicationIdentity, trust.ToXml().ToString());
                return -1;
            } else {
                return AppTrusts.Add(trust);
            }
        }

        public void AddRange (ApplicationTrust[] trusts) {
            if (trusts == null)
                throw new ArgumentNullException("trusts");

            int i=0;
            try {
                for (; i<trusts.Length; i++) {
                    Add(trusts[i]);
                }
            } catch {
                for (int j=0; j<i; j++) {
                    Remove(trusts[j]);
                }
                throw;
            }
        }

        public void AddRange (ApplicationTrustCollection trusts) {
            if (trusts == null)
                throw new ArgumentNullException("trusts");

            int i = 0;
            try {
                foreach (ApplicationTrust trust in trusts) {
                    Add(trust);
                    i++;
                }
            } catch {
                for (int j=0; j<i; j++) {
                    Remove(trusts[j]);
                }
                throw;
            }
        }

        public ApplicationTrustCollection Find (ApplicationIdentity applicationIdentity, ApplicationVersionMatch versionMatch) {
            ApplicationTrustCollection collection = new ApplicationTrustCollection(false);
            foreach (ApplicationTrust trust in this) {
                if (CmsUtils.CompareIdentities(trust.ApplicationIdentity, applicationIdentity, versionMatch))
                    collection.Add(trust);
            }
            return collection;
        }

        public void Remove (ApplicationIdentity applicationIdentity, ApplicationVersionMatch versionMatch) {
            ApplicationTrustCollection collection = Find(applicationIdentity, versionMatch);
            RemoveRange(collection);
        }

        public void Remove (ApplicationTrust trust) {
            if (trust == null)
                throw new ArgumentNullException("trust");
            if (trust.ApplicationIdentity == null)
                throw new ArgumentException(Environment.GetResourceString("Argument_ApplicationTrustShouldHaveIdentity"));

            // Remove the trust decision of the application from the fusion store.
            if (m_storeBounded) {
                CommitApplicationTrust(trust.ApplicationIdentity, null);
            } else {
                AppTrusts.Remove(trust);
            }
        }

        public void RemoveRange (ApplicationTrust[] trusts) {
            if (trusts == null)
                throw new ArgumentNullException("trusts");

            int i=0;
            try {
                for (; i<trusts.Length; i++) {
                    Remove(trusts[i]);
                }
            } catch {
                for (int j=0; j<i; j++) {
                    Add(trusts[j]);
                }
                throw;
            }
        }

        public void RemoveRange (ApplicationTrustCollection trusts) {
            if (trusts == null)
                throw new ArgumentNullException("trusts");

            int i = 0;
            try {
                foreach (ApplicationTrust trust in trusts) {
                    Remove(trust);
                    i++;
                }
            } catch {
                for (int j=0; j<i; j++) {
                    Add(trusts[j]);
                }
                throw;
            }
        }

        public void Clear() {
            // remove all trust decisions in the collection.
            ArrayList trusts = this.AppTrusts;
            if (m_storeBounded) {
                foreach (ApplicationTrust trust in trusts) {
                    if (trust.ApplicationIdentity == null)
                        throw new ArgumentException(Environment.GetResourceString("Argument_ApplicationTrustShouldHaveIdentity"));

                    // Remove the trust decision of the application from the fusion store.
                    CommitApplicationTrust(trust.ApplicationIdentity, null);
                }
            }
            trusts.Clear();
        }

        public ApplicationTrustEnumerator GetEnumerator() {
            return new ApplicationTrustEnumerator(this);
        }

        /// <internalonly/>
        IEnumerator IEnumerable.GetEnumerator() {
            return new ApplicationTrustEnumerator(this);
        }

        /// <internalonly/>
        void ICollection.CopyTo(Array array, int index) {
            if (array == null)
                throw new ArgumentNullException("array");
            if (array.Rank != 1)
                throw new ArgumentException(Environment.GetResourceString("Arg_RankMultiDimNotSupported"));
            if (index < 0 || index >= array.Length)
                throw new ArgumentOutOfRangeException("index", Environment.GetResourceString("ArgumentOutOfRange_Index"));
            if (array.Length - index < this.Count)
                throw new ArgumentException(Environment.GetResourceString("Argument_InvalidOffLen"));

            for (int i=0; i < this.Count; i++) {
                array.SetValue(this[i], index++);
            }
        }

        public void CopyTo (ApplicationTrust[] array, int index) {
            ((ICollection)this).CopyTo(array, index);
        }

        public bool IsSynchronized {
            get {
                return false;
            }
        }

        public object SyncRoot {
            get {
                return this;
            }
        }
    }

    [System.Runtime.InteropServices.ComVisible(true)]
    public sealed class ApplicationTrustEnumerator : IEnumerator {
        private ApplicationTrustCollection m_trusts;
        private int m_current;

        private ApplicationTrustEnumerator() {}
        internal ApplicationTrustEnumerator(ApplicationTrustCollection trusts) {
            m_trusts = trusts;
            m_current = -1;
        }

        public ApplicationTrust Current {
            get {
                return m_trusts[m_current];
            }
        }

        /// <internalonly/>
        object IEnumerator.Current {
            get {
                return (object) m_trusts[m_current];
            }
        }

        public bool MoveNext() {
            if (m_current == ((int) m_trusts.Count - 1))
                return false;
            m_current++;
            return true;
        }

        public void Reset() {
            m_current = -1;
        }
    }
}
