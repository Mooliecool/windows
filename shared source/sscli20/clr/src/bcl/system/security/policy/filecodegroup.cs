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
// FileCodeGroup.cs
//
// Representation for code groups used for the policy mechanism.
//

namespace System.Security.Policy {
    using System;
    using System.Collections;
    using System.Globalization;
    using System.Security.Permissions;
    using System.Security.Util;
    using System.Runtime.Serialization;
    using System.Runtime.Versioning;

    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    sealed public class FileCodeGroup : CodeGroup, IUnionSemanticCodeGroup {
        private FileIOPermissionAccess m_access;

        internal FileCodeGroup() : base() {}

        public FileCodeGroup(IMembershipCondition membershipCondition, FileIOPermissionAccess access)
            : base(membershipCondition, (PolicyStatement)null) {
            m_access = access;
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        public override PolicyStatement Resolve(Evidence evidence) {
            if (evidence == null)
                throw new ArgumentNullException("evidence");

            if (this.MembershipCondition.Check(evidence)) {
                PolicyStatement thisPolicy = CalculateAssemblyPolicy(evidence);

                IEnumerator enumerator = this.Children.GetEnumerator();
                while (enumerator.MoveNext()) {
                    PolicyStatement childPolicy = ((CodeGroup)enumerator.Current).Resolve(evidence);

                    if (childPolicy != null) {
                        if (((thisPolicy.Attributes & childPolicy.Attributes) & PolicyStatementAttribute.Exclusive) == PolicyStatementAttribute.Exclusive)
                            throw new PolicyException(Environment.GetResourceString("Policy_MultipleExclusive"));

                        thisPolicy.GetPermissionSetNoCopy().InplaceUnion(childPolicy.GetPermissionSetNoCopy());
                        thisPolicy.Attributes = thisPolicy.Attributes | childPolicy.Attributes;
                    }
                }

                return thisPolicy;
            }
            else {
                return null;
            }
        }

        /// <internalonly/>
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        PolicyStatement IUnionSemanticCodeGroup.InternalResolve(Evidence evidence) {
            if (evidence == null)
                throw new ArgumentNullException("evidence");

            if (this.MembershipCondition.Check(evidence)) {
                return CalculateAssemblyPolicy(evidence);
            }

            return null;
        }

        public override CodeGroup ResolveMatchingCodeGroups(Evidence evidence) {
            if (evidence == null)
                throw new ArgumentNullException("evidence");

            if (this.MembershipCondition.Check(evidence)) {
                CodeGroup retGroup = this.Copy();
                retGroup.Children = new ArrayList();
                IEnumerator enumerator = this.Children.GetEnumerator();
                while (enumerator.MoveNext()) {
                    CodeGroup matchingGroups = ((CodeGroup)enumerator.Current).ResolveMatchingCodeGroups(evidence);
                    // If the child has a policy, we are done.
                    if (matchingGroups != null)
                        retGroup.AddChild(matchingGroups);
                }
                return retGroup;
            }
            else {
                return null;
            }
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        internal PolicyStatement CalculatePolicy(Url url) {
            URLString urlString = url.GetURLString();
            if (String.Compare(urlString.Scheme, "file", StringComparison.OrdinalIgnoreCase) != 0)
                return null;

            string directory = urlString.GetDirectoryName();
            PermissionSet permSet = new PermissionSet(PermissionState.None);
            permSet.SetPermission(new FileIOPermission(m_access, System.IO.Path.GetFullPath(directory)));

            return new PolicyStatement(permSet, PolicyStatementAttribute.Nothing);
        }

        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        private PolicyStatement CalculateAssemblyPolicy(Evidence evidence) {
            PolicyStatement thisPolicy = null;

            IEnumerator evidenceEnumerator = evidence.GetHostEnumerator();
            while (evidenceEnumerator.MoveNext()) {
                Url url = evidenceEnumerator.Current as Url;
                if (url != null)
                    thisPolicy = CalculatePolicy(url);
            }

            if (thisPolicy == null)
                thisPolicy = new PolicyStatement(new PermissionSet(false), PolicyStatementAttribute.Nothing);
            return thisPolicy;
        }

        public override CodeGroup Copy() {
            FileCodeGroup group = new FileCodeGroup(this.MembershipCondition, this.m_access);
            group.Name = this.Name;
            group.Description = this.Description;

            IEnumerator enumerator = this.Children.GetEnumerator();
            while (enumerator.MoveNext()) {
                group.AddChild((CodeGroup)enumerator.Current);
            }
            return group;
        }

        public override string MergeLogic {
            get {
                return Environment.GetResourceString("MergeLogic_Union");
            }
        }

        public override string PermissionSetName {
            get {
                return String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("FileCodeGroup_PermissionSet"), XMLUtil.BitFieldEnumToString(typeof(FileIOPermissionAccess), m_access));
            }
        }

        public override string AttributeString {
            get {
                return null;
            }
        }

        protected override void CreateXml(SecurityElement element, PolicyLevel level) {
            element.AddAttribute("Access", XMLUtil.BitFieldEnumToString(typeof(FileIOPermissionAccess), m_access));
        }

        protected override void ParseXml(SecurityElement e, PolicyLevel level) {
            string access = e.Attribute("Access");
            if (access != null)
                m_access = (FileIOPermissionAccess) Enum.Parse(typeof(FileIOPermissionAccess), access);
            else
                m_access = FileIOPermissionAccess.NoAccess;
        }

        public override bool Equals(Object o) {
            FileCodeGroup that = (o as FileCodeGroup);
            if (that != null && base.Equals(that)) {
                if (this.m_access == that.m_access)
                    return true;
            }
            return false;
        }

        public override int GetHashCode() {
            return base.GetHashCode() + m_access.GetHashCode();
        }

        internal override string GetTypeName() {
            return "System.Security.Policy.FileCodeGroup";
        }
    }
}
