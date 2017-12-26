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
// PolicyManager.cs
//

namespace System.Security {
    using System.Collections;
    using System.Globalization;
    using System.IO;
    using System.Runtime.CompilerServices;
    using System.Runtime.Versioning;
    using System.Security.Util;
    using System.Security.Policy;
    using System.Security.Permissions;
    using System.Text;
    using System.Threading;

    internal class PolicyManager {
        // Only parse the system CAS policy levels when needed. In particular,
        // we do not use these when the AppDomain is homogeneous for example.
        private object m_policyLevels;
        private IList PolicyLevels {
            get {
                if (m_policyLevels == null) {
                    ArrayList policyLevels = new ArrayList();

                    string enterpriseConfig = PolicyLevel.GetLocationFromType(System.Security.PolicyLevelType.Enterprise);
                    policyLevels.Add(new PolicyLevel(System.Security.PolicyLevelType.Enterprise, enterpriseConfig, ConfigId.EnterprisePolicyLevel));

                    string machineConfig = PolicyLevel.GetLocationFromType(System.Security.PolicyLevelType.Machine);
                    policyLevels.Add(new PolicyLevel(System.Security.PolicyLevelType.Machine, machineConfig, ConfigId.MachinePolicyLevel));

                    // The user directory could be null if the user does not have a user profile for example.
                    if (Config.UserDirectory != null) {
                        string userConfig = PolicyLevel.GetLocationFromType(System.Security.PolicyLevelType.User);
                        policyLevels.Add(new PolicyLevel(System.Security.PolicyLevelType.User, userConfig, ConfigId.UserPolicyLevel));
                    }
                    Interlocked.CompareExchange(ref m_policyLevels, policyLevels, null);
                }
                return m_policyLevels as ArrayList;
            }
        }

        internal PolicyManager() {}

        internal void AddLevel (PolicyLevel level) {
            PolicyLevels.Add(level);
        }

        [SecurityPermissionAttribute(SecurityAction.Demand, Flags=SecurityPermissionFlag.ControlPolicy)]
        internal IEnumerator PolicyHierarchy() {
            return PolicyLevels.GetEnumerator();
        }

        internal PermissionSet Resolve (Evidence evidence) {
            // The host might have implemented a ResolvePolicy method in their HostSecurityManager,
            // so take that into account for non-GAC assemblies.
            if (!IsGacAssembly(evidence)) {
                HostSecurityManager securityManager = AppDomain.CurrentDomain.HostSecurityManager;
                if ((securityManager.Flags & HostSecurityManagerOptions.HostResolvePolicy) == HostSecurityManagerOptions.HostResolvePolicy)
                    return securityManager.ResolvePolicy(evidence);
            }
            return ResolveHelper(evidence);
        }

        internal PermissionSet ResolveHelper (Evidence evidence) {
            PermissionSet grant = null;
            if (IsGacAssembly(evidence)) {
                // GAC assemblies get FullTrust
                grant = new PermissionSet(PermissionState.Unrestricted);
            }
            else {
                {
                    // Resolve the CAS policy levels on the machine
                    grant = CodeGroupResolve(evidence, false);
                }
            }
            return grant;
        }

        internal PermissionSet CodeGroupResolve (Evidence evidence, bool systemPolicy) {
            PermissionSet grant = null;
            PolicyStatement policy;
            PolicyLevel currentLevel = null;

            IEnumerator levelEnumerator = PolicyLevels.GetEnumerator();

            char[] serializedEvidence = MakeEvidenceArray(evidence, false);
            int count = evidence.Count;

            bool legacyIgnoreSystemPolicy = (AppDomain.CurrentDomain.GetData("IgnoreSystemPolicy") != null);
            bool testApplicationLevels = false;
            while (levelEnumerator.MoveNext())
            {
                currentLevel = (PolicyLevel)levelEnumerator.Current;
                if (systemPolicy) {
                    if (currentLevel.Type == PolicyLevelType.AppDomain)
                        continue;
                } else if (legacyIgnoreSystemPolicy && currentLevel.Type != PolicyLevelType.AppDomain)
                    continue;

                policy = currentLevel.Resolve(evidence, count, serializedEvidence);

                // If the grant is "AllPossible", the intersection is just the other permission set.
                // Otherwise, do an inplace intersection (since we know we can alter the grant set since
                // it is a copy of the first policy statement's permission set).

                if (grant == null)
                    grant = policy.PermissionSet;
                else
                    grant.InplaceIntersect(policy.GetPermissionSetNoCopy());

                if (grant == null || grant.FastIsEmpty())
                {
                    break;
                }
                else if ((policy.Attributes & PolicyStatementAttribute.LevelFinal) == PolicyStatementAttribute.LevelFinal)
                {
                    if (currentLevel.Type != PolicyLevelType.AppDomain)
                    {
                        testApplicationLevels = true;
                    }
                    break;
                }
            }

            if (grant != null && testApplicationLevels)
            {
                PolicyLevel appDomainLevel = null;

                for (int i = PolicyLevels.Count - 1; i >= 0; --i)
                {
                    currentLevel = (PolicyLevel) PolicyLevels[i];
                    if (currentLevel.Type == PolicyLevelType.AppDomain)
                    {
                        appDomainLevel = currentLevel;
                        break;
                    }
                }

                if (appDomainLevel != null)
                {
                    policy = appDomainLevel.Resolve(evidence, count, serializedEvidence);
                    grant.InplaceIntersect(policy.GetPermissionSetNoCopy());
                }
            }

            if (grant == null)
                grant = new PermissionSet(PermissionState.None);

            // Each piece of evidence can possibly create an identity permission that we
            // need to add to our grant set.  Therefore, for all pieces of evidence that
            // implement the IIdentityPermissionFactory interface, ask it for its
            // adjoining identity permission and add it to the grant.

            if (!CodeAccessSecurityEngine.DoesFullTrustMeanFullTrust() || !grant.IsUnrestricted()) {
                IEnumerator enumerator = evidence.GetHostEnumerator();
                while (enumerator.MoveNext())
                {
                    Object obj = enumerator.Current;
                    IIdentityPermissionFactory factory = obj as IIdentityPermissionFactory;
                    if (factory != null)
                    {
                        IPermission perm = factory.CreateIdentityPermission( evidence );
                        if (perm != null)
                            grant.AddPermission( perm );
                    }
                }
            }

            grant.IgnoreTypeLoadFailures = true;
            return grant;
        }

        internal static bool IsGacAssembly (Evidence evidence) {
            return new GacMembershipCondition().Check(evidence);
        }


        internal IEnumerator ResolveCodeGroups (Evidence evidence) {
            ArrayList accumList = new ArrayList();
            IEnumerator levelEnumerator = PolicyLevels.GetEnumerator();

            while (levelEnumerator.MoveNext())
            {
                CodeGroup temp = ((PolicyLevel)levelEnumerator.Current).ResolveMatchingCodeGroups(evidence);
                if (temp != null)
                    accumList.Add(temp);
            }

            return accumList.GetEnumerator(0, accumList.Count);
        }

        internal void Save () {
            EncodeLevel(Environment.GetResourceString("Policy_PL_Enterprise"));
            EncodeLevel(Environment.GetResourceString("Policy_PL_Machine"));
            EncodeLevel(Environment.GetResourceString("Policy_PL_User"));
        }

        private void EncodeLevel (string label) {
            for (int i = 0; i < PolicyLevels.Count; ++i)
            {
                PolicyLevel currentLevel = (PolicyLevel) PolicyLevels[i];
                if (currentLevel.Label.Equals(label))
                {
                    EncodeLevel(currentLevel);
                    return;
                }
            }
        }

        internal static void EncodeLevel (PolicyLevel level) {
            SecurityElement elConf = new SecurityElement("configuration");
            SecurityElement elMscorlib = new SecurityElement("mscorlib");
            SecurityElement elSecurity = new SecurityElement("security");
            SecurityElement elPolicy = new SecurityElement("policy");

            elConf.AddChild(elMscorlib);
            elMscorlib.AddChild(elSecurity);
            elSecurity.AddChild(elPolicy);
            elPolicy.AddChild(level.ToXml());

            try
            {
                StringBuilder sb = new StringBuilder();
                Encoding encoding = Encoding.UTF8;

                SecurityElement format = new SecurityElement("xml");
                format.m_type = SecurityElementType.Format;
                format.AddAttribute("version", "1.0");
                format.AddAttribute("encoding", encoding.WebName);
                sb.Append(format.ToString());
                sb.Append(elConf.ToString());

                byte[] data = encoding.GetBytes(sb.ToString());

                // Write out the new config.
                if (level.Path == null || !Config.SaveDataByte(level.Path, data, 0, data.Length))
                    throw new PolicyException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Policy_UnableToSave"), level.Label));
            }
            catch (Exception e)
            {
                if (e is PolicyException)
                    throw e;
                else
                    throw new PolicyException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Policy_UnableToSave"), level.Label), e);
            }
            catch
            {
                throw new PolicyException(String.Format(CultureInfo.CurrentCulture, Environment.GetResourceString("Policy_UnableToSave"), level.Label));
            }

            Config.ResetCacheData(level.ConfigId);
            if (CanUseQuickCache(level.RootCodeGroup))
                Config.SetQuickCache(level.ConfigId, GenerateQuickCache(level));
        }

        // Here is the managed portion of the QuickCache code.  It
        // is mainly concerned with detecting whether it is valid
        // for us to use the quick cache, and then calculating the
        // proper mapping of partial evidence to partial mapping.
        //
        // The choice of the partial evidence sets is fairly arbitrary
        // and in this case is tailored to give us meaningful
        // results from default policy.
        //
        // The choice of whether or not we can use the quick cache
        // is far from arbitrary.  There are a number of conditions that must
        // be true for the QuickCache to produce valid result.  These
        // are:
        // 
        // * equivalent evidence objects must produce the same
        //   grant set (i.e. it must be independent of time of day,
        //   space on the harddisk, other "external" factors, and
        //   cannot be random).
        //
        // * evidence must be used positively (i.e. if evidence A grants
        //   permission X, then evidence A+B must grant at least permission
        //   X).
        //
        // In particular for our implementation, this means that we
        // limit the classes that can be used by policy to just
        // the ones defined within mscorlib and that there are
        // no Exclusive bits set on any code groups.

        internal static bool CanUseQuickCache (CodeGroup group) {
            ArrayList list = new ArrayList();

            list.Add(group);

            for (int i = 0; i < list.Count; ++i)
            {
                group = (CodeGroup)list[i];

                IUnionSemanticCodeGroup unionGroup = group as IUnionSemanticCodeGroup;

                if (unionGroup != null)
                {
                    if (!TestPolicyStatement(group.PolicyStatement))
                        return false;
                }
                else
                {
                    return false;
                }

                IMembershipCondition cond = group.MembershipCondition;
                if (cond != null && !(cond is IConstantMembershipCondition))
                {
                    return false;
                }

                IList children = group.Children;

                if (children != null && children.Count > 0)
                {
                    IEnumerator enumerator = children.GetEnumerator();

                    while (enumerator.MoveNext())
                    {
                        list.Add(enumerator.Current);
                    }
                }
            }

            return true;
        }

        private static bool TestPolicyStatement (PolicyStatement policy) {
            if (policy == null)
                return true;
            return (policy.Attributes & PolicyStatementAttribute.Exclusive) == 0;
        }

        private static QuickCacheEntryType GenerateQuickCache (PolicyLevel level) {
            QuickCacheEntryType[] FullTrustMap = new QuickCacheEntryType[]
                { QuickCacheEntryType.FullTrustZoneMyComputer,
                  QuickCacheEntryType.FullTrustZoneIntranet,
                  QuickCacheEntryType.FullTrustZoneInternet,
                  QuickCacheEntryType.FullTrustZoneTrusted,
                  QuickCacheEntryType.FullTrustZoneUntrusted };

            QuickCacheEntryType accumulator = (QuickCacheEntryType)0;

            Evidence noEvidence = new Evidence();

            PermissionSet policy = null;

            try
            {
                policy = level.Resolve( noEvidence ).PermissionSet;
                if (policy.IsUnrestricted())
                    accumulator |= QuickCacheEntryType.FullTrustAll;
            }
            catch (PolicyException)
            {
            }

            Array zones = Enum.GetValues( typeof( SecurityZone ) );

            for (int i = 0; i < zones.Length; ++i)
            {
                if (((SecurityZone)zones.GetValue( i )) == SecurityZone.NoZone)
                    continue;

                Evidence zoneEvidence = new Evidence();
                zoneEvidence.AddHost( new Zone( (SecurityZone)zones.GetValue( i ) ) );

                PermissionSet zonePolicy = null;

                try
                {
                    zonePolicy = level.Resolve( zoneEvidence ).PermissionSet;
                    if (zonePolicy.IsUnrestricted())
                        accumulator |= FullTrustMap[i];
                }
                catch (PolicyException)
                {
                }
            }

            return accumulator;
        }

        internal static char[] MakeEvidenceArray (Evidence evidence, bool verbose) {
            // We only support caching on our built-in evidence types (excluding hash b/c it would
            // make our caching scheme just match up the same assembly from the same location which
            // doesn't gain us anything).

            IEnumerator enumerator = evidence.GetEnumerator();
            int requiredLength = 0;

            while (enumerator.MoveNext())
            {
                IBuiltInEvidence obj = enumerator.Current as IBuiltInEvidence;

                if (obj == null)
                    return null;

                requiredLength += obj.GetRequiredSize(verbose);
            }

            enumerator.Reset();
            
            char[] buffer = new char[requiredLength];

            int position = 0;

            while (enumerator.MoveNext())
            {
                position = ((IBuiltInEvidence)enumerator.Current).OutputToBuffer(buffer, position, verbose);
            }

            return buffer;
        }

#if _DEBUG
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int _DebugOut(String file, String message);
#endif
    }
}
