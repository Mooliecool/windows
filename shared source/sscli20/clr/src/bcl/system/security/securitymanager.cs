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
// SecurityManager.cs
//
// The SecurityManager class provides a general purpose API for interacting
// with the security system.
//

namespace System.Security {
    using System;
    using System.Security.Util;
    using System.Security.Policy;
    using System.Security.Permissions;
    using System.Collections;
    using System.Runtime.CompilerServices;
    using System.Text;
    using System.Threading;
    using System.Reflection;
    using System.IO;
    using System.Globalization;
    using System.Runtime.Versioning;

    [Serializable]
    [System.Runtime.InteropServices.ComVisible(true)]
    public enum PolicyLevelType
    {
        User = 0,
        Machine = 1,
        Enterprise = 2,
        AppDomain = 3
    }

    [System.Runtime.InteropServices.ComVisible(true)]
    static public class SecurityManager {
        private static Type securityPermissionType = null;
        private static SecurityPermission executionSecurityPermission = null;

        // This flag enables checking for execution rights on start-up (slow)
        private const int CheckExecutionRightsDisabledFlag  = 0x00000100;

        // -1 if no decision has been made yet
        // 0 if we don't need to check
        // 1 if we do.
        private static int checkExecution = -1;

        private static PolicyManager polmgr = new PolicyManager();
        internal static PolicyManager PolicyManager {
            get {
                return polmgr;
            }
        }

        //
        // Public APIs
        //

        public static bool IsGranted( IPermission perm )
        {
            if (perm == null)
                return true;

            PermissionSet granted, denied;
            StackCrawlMark stackMark = StackCrawlMark.LookForMyCaller;
            _GetGrantedPermissions( out granted, out denied, ref stackMark );
            return granted.Contains( perm ) && (denied == null || !denied.Contains( perm ));
        }

        static private bool CheckExecution()
        {
            if (checkExecution == -1)
                checkExecution = (GetGlobalFlags() & CheckExecutionRightsDisabledFlag) != 0 ? 0 : 1;

            if (checkExecution == 1)
            {
                if (securityPermissionType == null)
                {
                    securityPermissionType = typeof( System.Security.Permissions.SecurityPermission );
                    executionSecurityPermission = new SecurityPermission( SecurityPermissionFlag.Execution );
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <internalonly/>
        [StrongNameIdentityPermissionAttribute(SecurityAction.LinkDemand, Name = "System.Windows.Forms", PublicKey = "0x00000000000000000400000000000000" )]
        static public void GetZoneAndOrigin( out ArrayList zone, out ArrayList origin )
        {
            StackCrawlMark mark = StackCrawlMark.LookForMyCaller;

            if (_IsSecurityOn())
            {
                CodeAccessSecurityEngine.GetZoneAndOrigin( ref mark, out zone, out origin );
            }
            else
            {
                zone = null;
                origin = null;
            }
        }

        [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlPolicy )]
        [ResourceExposure(ResourceScope.Machine)]
        [ResourceConsumption(ResourceScope.Machine)]
        static public PolicyLevel LoadPolicyLevelFromFile(string path, PolicyLevelType type)
        {
            if (path == null)
               throw new ArgumentNullException( "path" );

            // We need to retain V1.x compatibility by throwing the same exception type.
            if (!File.InternalExists(path))
                throw new ArgumentException( Environment.GetResourceString("Argument_PolicyFileDoesNotExist"));

            String fullPath = Path.GetFullPath( path );

            FileIOPermission perm = new FileIOPermission( PermissionState.None );
            perm.AddPathList( FileIOPermissionAccess.Read, fullPath );
            perm.AddPathList( FileIOPermissionAccess.Write, fullPath );
            perm.Demand();

            using (FileStream stream = new FileStream(path, FileMode.Open, FileAccess.Read)) {
                using (StreamReader reader = new StreamReader(stream)) {
                    return LoadPolicyLevelFromStringHelper(reader.ReadToEnd(), path, type);
                }
            }
        }

        [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlPolicy )]
        static public PolicyLevel LoadPolicyLevelFromString(string str, PolicyLevelType type)
        {
            return LoadPolicyLevelFromStringHelper(str, null, type);
        }

        private static PolicyLevel LoadPolicyLevelFromStringHelper (string str, string path, PolicyLevelType type)
        {
            if (str == null)
                throw new ArgumentNullException( "str" );

            PolicyLevel level = new PolicyLevel(type, path);

            Parser parser = new Parser( str );
            SecurityElement elRoot = parser.GetTopElement();
            if (elRoot == null)
                throw new ArgumentException( String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Policy_BadXml" ), "configuration" ) );

            SecurityElement elMscorlib = elRoot.SearchForChildByTag( "mscorlib" );
            if (elMscorlib == null)
                throw new ArgumentException( String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Policy_BadXml" ), "mscorlib" ) );

            SecurityElement elSecurity = elMscorlib.SearchForChildByTag( "security" );
            if (elSecurity == null)
                throw new ArgumentException( String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Policy_BadXml" ), "security" ) );

            SecurityElement elPolicy = elSecurity.SearchForChildByTag( "policy" );
            if (elPolicy == null)
                throw new ArgumentException( String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Policy_BadXml" ), "policy" ) );

            SecurityElement elPolicyLevel = elPolicy.SearchForChildByTag( "PolicyLevel" );
            if (elPolicyLevel != null)
                level.FromXml( elPolicyLevel );
            else
                throw new ArgumentException( String.Format( CultureInfo.CurrentCulture, Environment.GetResourceString( "Policy_BadXml" ), "PolicyLevel" ) );

            return level;
        }

        [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlPolicy )]
        static public void SavePolicyLevel( PolicyLevel level )
        {
            PolicyManager.EncodeLevel( level );
        }

        static
        private PermissionSet ResolvePolicy( Evidence evidence,
                                             PermissionSet reqdPset,
                                             PermissionSet optPset,
                                             PermissionSet denyPset,
                                             out PermissionSet denied,
                                             out int securitySpecialFlags,
                                             bool checkExecutionPermission )
        {
            CodeAccessPermission.AssertAllPossible();

            PermissionSet granted = ResolvePolicy( evidence,
                                                   reqdPset,
                                                   optPset,
                                                   denyPset,
                                                   out denied,
                                                   checkExecutionPermission );

            securitySpecialFlags = SecurityManager.GetSpecialFlags(granted, denied);
            return granted;
        }

        static public PermissionSet ResolvePolicy(Evidence evidence,
                           PermissionSet reqdPset,
                           PermissionSet optPset,
                           PermissionSet denyPset,
                           out PermissionSet denied)
        {
            return ResolvePolicy( evidence, reqdPset, optPset, denyPset, out denied, true );
        }

        static private PermissionSet ResolvePolicy(Evidence evidence,
                           PermissionSet reqdPset,
                           PermissionSet optPset,
                           PermissionSet denyPset,
                           out PermissionSet denied,
                           bool checkExecutionPermission)
        {
            PermissionSet requested = null;
            PermissionSet optional;
            PermissionSet allowed;

            Exception savedException = null;

            // We don't want to recurse back into here as a result of a
            // stackwalk during resolution. So simply assert full trust (this
            // implies that custom permissions cannot use any permissions that
            // don't implement IUnrestrictedPermission.
            // PermissionSet.s_fullTrust.Assert();

            // The requested set is the union of the minimal request and the
            // optional request. Minimal request defaults to empty, optional
            // is "AllPossible" (includes any permission that can be defined)
            // which is symbolized by null.
            optional = optPset;

            if (reqdPset == null)
                requested = optional;
            else
                // If optional is null, the requested set becomes null/"AllPossible".
                requested = optional == null ? null : reqdPset.Union(optional);

            // Make sure that the right to execute is requested (if this feature is
            // enabled).

            if (requested != null && !requested.IsUnrestricted() && CheckExecution())
                requested.AddPermission( executionSecurityPermission );

            // If we aren't passed any evidence, just make an empty object
            // If we are passed evidence, copy it before passing it
            // to the policy manager.
            // Note: this is not a deep copy, the pieces of evidence within the
            // Evidence object can still be altered and affect the originals.

            if (evidence == null)
                evidence = new Evidence();
            else
                evidence = evidence.ShallowCopy();

            evidence.AddHost(new PermissionRequestEvidence(reqdPset, optPset, denyPset));
            allowed = polmgr.Resolve(evidence);

            // Intersect the grant with the RequestOptional
            if (requested != null)
                allowed.InplaceIntersect(requested);

            // Check that we were granted the right to execute.
            if (checkExecutionPermission && CheckExecution())
            {
                if (!allowed.Contains( executionSecurityPermission ) ||
                    (denyPset != null && denyPset.Contains( executionSecurityPermission )))
                {
                    throw new PolicyException(Environment.GetResourceString( "Policy_NoExecutionPermission" ),
                                              System.__HResults.CORSEC_E_NO_EXEC_PERM,
                                              savedException );
                }
            }

            // Check that we were granted at least the minimal set we asked for. Do
            // this before pruning away any overlap with the refused set so that
            // users have the flexability of defining minimal permissions that are
            // only expressable as set differences (e.g. allow access to "C:\" but
            // disallow "C:\Windows").
            if (reqdPset != null && !reqdPset.IsSubsetOf(allowed))
            {
                throw new PolicyException(Environment.GetResourceString( "Policy_NoRequiredPermission" ),
                                          System.__HResults.CORSEC_E_MIN_GRANT_FAIL,
                                          savedException );
            }

            // Remove any granted permissions that are safe subsets of some denied
            // permission. The remaining denied permissions (if any) are returned
            // along with the modified grant set for use in checks.
            if (denyPset != null)
            {
                denied = denyPset.Copy();
                allowed.MergeDeniedSet(denied);
                if (denied.IsEmpty())
                    denied = null;
            }
            else
                denied = null;

            allowed.IgnoreTypeLoadFailures = true;

            return allowed;
        }

        static public PermissionSet ResolvePolicy( Evidence evidence )
        {
            // If we aren't passed any evidence, just make an empty object
            // If we are passed evidence, copy it before passing it
            // to the policy manager.
            // Note: this is not a deep copy, the pieces of evidence within the
            // Evidence object can still be altered and affect the originals.

            if (evidence == null)
                evidence = new Evidence();
            else
                evidence = evidence.ShallowCopy();

            evidence.AddHost(new PermissionRequestEvidence(null, null, null));
            return polmgr.Resolve(evidence);
        }

        static public PermissionSet ResolvePolicy( Evidence[] evidences )
        {
            if (evidences == null || evidences.Length == 0)
                evidences = new Evidence[] { null };

            PermissionSet retval = ResolvePolicy( evidences[0] );
            if (retval == null)
                return null;

            for (int i = 1; i < evidences.Length; ++i)
            {
                retval = retval.Intersect( ResolvePolicy( evidences[i] ) );
                if (retval == null || retval.IsEmpty())
                    return retval;
            }

            return retval;
        }

        //
        // This method resolves the policy for the specified evidence, but it
        // ignores the AppDomain level even when one is available in the current policy.
        //

        public static PermissionSet ResolveSystemPolicy (Evidence evidence) {
            if (PolicyManager.IsGacAssembly(evidence))
                return new PermissionSet(PermissionState.Unrestricted);

            return polmgr.CodeGroupResolve(evidence, true);
        }

        static public IEnumerator ResolvePolicyGroups(Evidence evidence)
        {
            return polmgr.ResolveCodeGroups(evidence);
        }

        public static IEnumerator PolicyHierarchy()
        {
            return polmgr.PolicyHierarchy();
        }

        [SecurityPermissionAttribute( SecurityAction.Demand, Flags = SecurityPermissionFlag.ControlPolicy )]
        public static void SavePolicy()
        {
            polmgr.Save();
            SaveGlobalFlags();
        }

        static public bool CheckExecutionRights
        {
            get
            {
                return (GetGlobalFlags() & CheckExecutionRightsDisabledFlag) != CheckExecutionRightsDisabledFlag;
            }

            set
            {
                if (value)
                {
                    checkExecution = 1;
                    SetGlobalFlags( CheckExecutionRightsDisabledFlag, 0 );
                }
                else
                {
                    new SecurityPermission( SecurityPermissionFlag.ControlPolicy ).Demand();

                    checkExecution = 0;
                    SetGlobalFlags( CheckExecutionRightsDisabledFlag, CheckExecutionRightsDisabledFlag );
                }
            }
        }

        [Obsolete("Because security can no longer be turned off permanently, setting the SecurityEnabled property no longer has any effect. Reading the property will still indicate whether security has been turned off temporarily.")]
        public static bool SecurityEnabled
        {
            get {
                return _IsSecurityOn();
            }

            set {
                // The setter for this property is now a no-op since caspol.exe is 
                // the only supported way of turning security off, and it does it always
                // in a temporary way, so there is no need to turn security back on.
            }
        }

        private static int[][] s_BuiltInPermissionIndexMap = {
            new int[] { BuiltInPermissionIndex.EnvironmentPermissionIndex, (int) PermissionType.EnvironmentPermission },
            new int[] { BuiltInPermissionIndex.FileDialogPermissionIndex, (int) PermissionType.FileDialogPermission },
            new int[] { BuiltInPermissionIndex.FileIOPermissionIndex, (int) PermissionType.FileIOPermission },
            new int[] { BuiltInPermissionIndex.ReflectionPermissionIndex, (int) PermissionType.ReflectionPermission },
            new int[] { BuiltInPermissionIndex.SecurityPermissionIndex, (int) PermissionType.SecurityPermission },
            new int[] { BuiltInPermissionIndex.UIPermissionIndex, (int) PermissionType.UIPermission }
        };

        private static CodeAccessPermission[] s_UnrestrictedSpecialPermissionMap = {
            new EnvironmentPermission(PermissionState.Unrestricted),
            new FileDialogPermission(PermissionState.Unrestricted),
            new FileIOPermission(PermissionState.Unrestricted),
            new ReflectionPermission(PermissionState.Unrestricted),
            new SecurityPermission(PermissionState.Unrestricted),
            new UIPermission(PermissionState.Unrestricted)
        };

        private static int GetSpecialFlags (PermissionSet grantSet, PermissionSet deniedSet) {
            if ((grantSet != null && grantSet.IsUnrestricted()) && (deniedSet == null || deniedSet.IsEmpty())) {
                return -1;
            }
            else {
                SecurityPermission securityPermission = null;
                SecurityPermissionFlag securityPermissionFlags = SecurityPermissionFlag.NoFlags;
                ReflectionPermission reflectionPermission = null;
                ReflectionPermissionFlag reflectionPermissionFlags = ReflectionPermissionFlag.NoFlags;

                CodeAccessPermission[] specialPermissions = new CodeAccessPermission[6];
                if (grantSet != null) {
                    if (grantSet.IsUnrestricted()) {
                        securityPermissionFlags = SecurityPermissionFlag.AllFlags;
                        reflectionPermissionFlags = ReflectionPermissionFlag.AllFlags;
                        for (int i = 0; i < specialPermissions.Length; i++) {
                            specialPermissions[i] = s_UnrestrictedSpecialPermissionMap[i];
                        }
                    }
                    else {
                        securityPermission = grantSet.GetPermission(BuiltInPermissionIndex.SecurityPermissionIndex) as SecurityPermission;
                        if (securityPermission != null)
                            securityPermissionFlags = securityPermission.Flags;
                        reflectionPermission = grantSet.GetPermission(BuiltInPermissionIndex.ReflectionPermissionIndex) as ReflectionPermission;
                        if (reflectionPermission != null)
                            reflectionPermissionFlags = reflectionPermission.Flags;
                        for (int i = 0; i < specialPermissions.Length; i++) {
                            specialPermissions[i] = grantSet.GetPermission(s_BuiltInPermissionIndexMap[i][0]) as CodeAccessPermission;
                        }
                    }
                }

                if (deniedSet != null) {
                    if (deniedSet.IsUnrestricted()) {
                        securityPermissionFlags = SecurityPermissionFlag.NoFlags;
                        reflectionPermissionFlags = ReflectionPermissionFlag.NoFlags;
                        for (int i = 0; i < s_BuiltInPermissionIndexMap.Length; i++) {
                            specialPermissions[i] = null;
                        }
                    }
                    else {
                        securityPermission = deniedSet.GetPermission(BuiltInPermissionIndex.SecurityPermissionIndex) as SecurityPermission;
                        if (securityPermission != null)
                            securityPermissionFlags &= ~securityPermission.Flags;
                        reflectionPermission = deniedSet.GetPermission(BuiltInPermissionIndex.ReflectionPermissionIndex) as ReflectionPermission;
                        if (reflectionPermission != null)
                            reflectionPermissionFlags &= ~reflectionPermission.Flags;
                        for (int i = 0; i < s_BuiltInPermissionIndexMap.Length; i++) {
                            CodeAccessPermission deniedSpecialPermission = deniedSet.GetPermission(s_BuiltInPermissionIndexMap[i][0]) as CodeAccessPermission;
                            if (deniedSpecialPermission != null && !deniedSpecialPermission.IsSubsetOf(null))
                                specialPermissions[i] = null; // we don't care about the exact value here.
                        }
                    }
                }
                int flags = MapToSpecialFlags(securityPermissionFlags, reflectionPermissionFlags);
                if (flags != -1) {
                    for (int i = 0; i < specialPermissions.Length; i++) {
                        if (specialPermissions[i] != null && ((IUnrestrictedPermission) specialPermissions[i]).IsUnrestricted())
                            flags |= (1 << (int) s_BuiltInPermissionIndexMap[i][1]);
                    }
                }
                return flags;
            }
        }

        private static int MapToSpecialFlags (SecurityPermissionFlag securityPermissionFlags, ReflectionPermissionFlag reflectionPermissionFlags) {
            int flags = 0;
            if ((securityPermissionFlags & SecurityPermissionFlag.UnmanagedCode) == SecurityPermissionFlag.UnmanagedCode)
                flags |= (1 << (int) PermissionType.SecurityUnmngdCodeAccess);
            if ((securityPermissionFlags & SecurityPermissionFlag.SkipVerification) == SecurityPermissionFlag.SkipVerification)
                flags |= (1 << (int) PermissionType.SecuritySkipVerification);
            if ((securityPermissionFlags & SecurityPermissionFlag.Assertion) == SecurityPermissionFlag.Assertion)
                flags |= (1 << (int) PermissionType.SecurityAssert);
            if ((securityPermissionFlags & SecurityPermissionFlag.SerializationFormatter) == SecurityPermissionFlag.SerializationFormatter)
                flags |= (1 << (int) PermissionType.SecuritySerialization);
            if ((securityPermissionFlags & SecurityPermissionFlag.BindingRedirects) == SecurityPermissionFlag.BindingRedirects)
                flags |= (1 << (int) PermissionType.SecurityBindingRedirects);
            if ((securityPermissionFlags & SecurityPermissionFlag.ControlEvidence) == SecurityPermissionFlag.ControlEvidence)
                flags |= (1 << (int) PermissionType.SecurityControlEvidence);
            if ((securityPermissionFlags & SecurityPermissionFlag.ControlPrincipal) == SecurityPermissionFlag.ControlPrincipal)
                flags |= (1 << (int) PermissionType.SecurityControlPrincipal);

            if ((reflectionPermissionFlags & ReflectionPermissionFlag.MemberAccess) == ReflectionPermissionFlag.MemberAccess)
                flags |= (1 << (int) PermissionType.ReflectionMemberAccess);
            if ((reflectionPermissionFlags & ReflectionPermissionFlag.ReflectionEmit) == ReflectionPermissionFlag.ReflectionEmit)
                flags |= (1 << (int) PermissionType.ReflectionEmit);
            return flags;
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool _IsSameType(String strLeft, String strRight);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool _SetThreadSecurity(bool bThreadSecurity);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool _IsSecurityOn();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern int GetGlobalFlags();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void SetGlobalFlags( int mask, int flags );
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void SaveGlobalFlags();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void _GetGrantedPermissions(out PermissionSet granted, out PermissionSet denied, ref StackCrawlMark stackmark);
    }
}
