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

namespace System.Security {
    using System;
    using System.Threading;
    using System.Security.Util;
    using System.Collections;
    using System.Runtime.CompilerServices;
    using System.Security.Permissions;
    using System.Reflection;
    using System.Globalization;
    using System.Security.Policy;
    using System.Runtime.Versioning;

    // Used in DemandInternal, to remember the result of previous demands
    // KEEP IN SYNC WITH DEFINITIONS IN SECURITYPOLICY.H
    [Serializable]
    internal enum PermissionType
    {
        // special flags
        SecurityUnmngdCodeAccess    = 0,
        SecuritySkipVerification    = 1,
        ReflectionTypeInfo          = 2,
        SecurityAssert              = 3,
        ReflectionMemberAccess      = 4,
        SecuritySerialization       = 5,
        ReflectionEmit              = 6,
        FullTrust                   = 7,
        SecurityBindingRedirects    = 8,

        // special permissions
        UIPermission                = 9,
        EnvironmentPermission       = 10,
        FileDialogPermission        = 11,
        FileIOPermission            = 12,
        ReflectionPermission        = 13,
        SecurityPermission          = 14,

        // additional special flags
        SecurityControlEvidence     = 16,
        SecurityControlPrincipal    = 17
    }

    internal class CodeAccessSecurityEngine
    {

        internal static SecurityPermission AssertPermission; 
        internal static PermissionToken AssertPermissionToken; 

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void SpecialDemand(PermissionType whatPermission, ref StackCrawlMark stackMark);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool DoesFullTrustMeanFullTrust();

        [System.Diagnostics.Conditional( "_DEBUG" )]
        [ResourceExposure(ResourceScope.None)]
        [ResourceConsumption(ResourceScope.Process, ResourceScope.Process)]
        private static void DEBUG_OUT( String str )
        {
#if _DEBUG        
            if (debug)
            {
                if (to_file)
                {
                    System.Text.StringBuilder sb = new System.Text.StringBuilder();
                    sb.Append( str );
                    sb.Append ((char)13) ;
                    sb.Append ((char)10) ;
                    PolicyManager._DebugOut( file, sb.ToString() );
                }
                else
                    Console.WriteLine( str );
             }
#endif             
        }
        
#if _DEBUG
        private static bool debug = false;
        private static readonly bool to_file = false;
        private const String file = "d:\\foo\\debug.txt";
#endif  

        private CodeAccessSecurityEngine(){}
        // static default constructor. This will be called before any of the static members are accessed.
        static CodeAccessSecurityEngine()
        {
            AssertPermission = new SecurityPermission(SecurityPermissionFlag.Assertion);
            AssertPermissionToken = PermissionToken.GetToken(AssertPermission);
        }

        private static void ThrowSecurityException(Assembly asm, PermissionSet granted, PermissionSet refused, RuntimeMethodHandle rmh, SecurityAction action, Object demand, IPermission permThatFailed)
        {
            AssemblyName asmName = null;
            Evidence asmEvidence = null;
            if (asm != null)
            {
                // Assert here because reflection will check grants and if we fail the check,
                // there will be an infinite recursion that overflows the stack.
                PermissionSet.s_fullTrust.Assert();
                asmName = asm.GetName();
                if(asm != Assembly.GetExecutingAssembly()) // this condition is to avoid having to marshal mscorlib's evidence (which is always in teh default domain) to the current domain
                    asmEvidence = asm.Evidence;
            }
            throw SecurityException.MakeSecurityException(asmName, asmEvidence, granted, refused, rmh, action, demand, permThatFailed);
        }

        private static void ThrowSecurityException(Object assemblyOrString, PermissionSet granted, PermissionSet refused, RuntimeMethodHandle rmh, SecurityAction action, Object demand, IPermission permThatFailed)
        {
            BCLDebug.Assert((assemblyOrString == null || assemblyOrString is Assembly || assemblyOrString is String), "Must pass in an Assembly object or String object here");
            
            if (assemblyOrString == null || assemblyOrString is Assembly)
                ThrowSecurityException((Assembly)assemblyOrString, granted, refused, rmh, action, demand, permThatFailed);
            else
            {
                AssemblyName asmName = new AssemblyName((String)assemblyOrString);
                throw SecurityException.MakeSecurityException(asmName, null, granted, refused, rmh, action, demand, permThatFailed);
            }
        }

        private static void LazyCheckSetHelper(PermissionSet demands,
                                               IntPtr asmSecDesc,
                                               RuntimeMethodHandle rmh,
                                               Assembly assembly,
                                               SecurityAction action)
        {
            if (demands.CanUnrestrictedOverride())
                return;

            PermissionSet grants;
            PermissionSet refused;

            _GetGrantedPermissionSet( asmSecDesc, out grants, out refused );

            CheckSetHelper( grants, refused, demands, rmh, (Object)assembly, action, true );
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern void _GetGrantedPermissionSet(IntPtr secDesc, 
                                                           out PermissionSet grants,
                                                           out PermissionSet refused );

        internal static void CheckSetHelper(CompressedStack cs,
                                           PermissionSet grants,
                                           PermissionSet refused,
                                           PermissionSet demands,
                                           RuntimeMethodHandle rmh,
                                           Assembly asm,
                                           SecurityAction action)
        {
            if (cs != null)
                cs.CheckSetDemand(demands, rmh);
            else
                CheckSetHelper(grants, refused, demands, rmh, (Object)asm, action, true);
        }

        internal static bool CheckSetHelper(PermissionSet grants,
                                           PermissionSet refused,
                                           PermissionSet demands,
                                           RuntimeMethodHandle rmh,
                                           Object assemblyOrString,
                                           SecurityAction action,
                                           bool throwException)
        {

            BCLDebug.Assert(demands != null, "Should not reach here with a null demand set");
    #if _DEBUG
            if (debug)
            {
                DEBUG_OUT("Granted: ");
                DEBUG_OUT(grants.ToXml().ToString());
                DEBUG_OUT("Refused: ");
                DEBUG_OUT(refused != null ? refused.ToXml().ToString() : "<null>");
                DEBUG_OUT("Demanded: ");
                DEBUG_OUT(demands!=null ? demands.ToXml().ToString() : "<null>");
            }
    #endif

            IPermission permThatFailed = null;
            if (grants != null)
                grants.CheckDecoded(demands);
            if (refused != null)
                refused.CheckDecoded(demands);

            bool bThreadSecurity = SecurityManager._SetThreadSecurity(false);

            try
            {

                // Check grant set
                if (!demands.CheckDemand(grants, out permThatFailed))
                {
                    if (throwException)
                        ThrowSecurityException(assemblyOrString, grants, refused, rmh, action, demands, permThatFailed);
                    else
                        return false;
                }

                // Check refused set
                if (!demands.CheckDeny(refused, out permThatFailed))
                {
                    if (throwException)
                        ThrowSecurityException(assemblyOrString, grants, refused, rmh, action, demands, permThatFailed);
                    else
                        return false;
                }
            }
            catch (SecurityException)
            {
                throw;
            }
            catch (Exception)
            {
                // Any exception besides a security exception in this code means that
                // a permission was unable to properly handle what we asked of it.
                // We will define this to mean that the demand failed.
                if (throwException)
                    ThrowSecurityException(assemblyOrString, grants, refused, rmh, action, demands, permThatFailed);
                else
                    return false;
            }
            catch
            {
                return false;
            }
            finally
            {
                if (bThreadSecurity)
                    SecurityManager._SetThreadSecurity(true);
            }
            return true;
        }

        internal static void CheckHelper(CompressedStack cs,
                                        PermissionSet grantedSet,
                                        PermissionSet refusedSet,
                                        CodeAccessPermission demand, 
                                        PermissionToken permToken,
                                        RuntimeMethodHandle rmh,
                                        Assembly asm,
                                        SecurityAction action)
        {
            if (cs != null)
                cs.CheckDemand(demand, permToken, rmh);
            else
                CheckHelper(grantedSet, refusedSet, demand, permToken, rmh, (Object)asm, action, true);
        }

        internal static bool CheckHelper(PermissionSet grantedSet,
                                        PermissionSet refusedSet,
                                        CodeAccessPermission demand, 
                                        PermissionToken permToken,
                                        RuntimeMethodHandle rmh,
                                        Object assemblyOrString,
                                        SecurityAction action,
                                        bool throwException)
        {
            // We should never get here with a null demand
            BCLDebug.Assert(demand != null, "Should not reach here with a null demand");
            
    #if _DEBUG
            if (debug)
            {
                DEBUG_OUT("Granted: ");
                DEBUG_OUT(grantedSet.ToXml().ToString());
                DEBUG_OUT("Refused: ");
                DEBUG_OUT(refusedSet != null ? refusedSet.ToXml().ToString() : "<null>");
                DEBUG_OUT("Demanded: ");
                DEBUG_OUT(demand.ToString());
            }
    #endif

            if (permToken == null)
                permToken = PermissionToken.GetToken(demand);

            if (grantedSet != null)
                grantedSet.CheckDecoded(permToken.m_index);
            if (refusedSet != null)
                refusedSet.CheckDecoded(permToken.m_index);

            // If PermissionSet is null, then module does not have Permissions... Fail check.

            bool bThreadSecurity = SecurityManager._SetThreadSecurity(false);

            try
            {
                if (grantedSet == null)
                {
                    if (throwException)
                        ThrowSecurityException(assemblyOrString, grantedSet, refusedSet, rmh, action, demand, demand);
                    else
                        return false;
                }
                
                else if (!grantedSet.IsUnrestricted() || !demand.CanUnrestrictedOverride())
                {
                    // If we aren't unrestricted, there is a refused set, or our permission is not of the unrestricted
                    // variety, we need to do the proper callback.

                    BCLDebug.Assert(demand != null,"demand != null");

                    // Find the permission of matching type in the permission set.

                    CodeAccessPermission grantedPerm = 
                                (CodeAccessPermission)grantedSet.GetPermission(permToken);

                    // Make sure the demand has been granted
                    if (!demand.CheckDemand( grantedPerm ))
                    {
                        if (throwException)
                            ThrowSecurityException(assemblyOrString, grantedSet, refusedSet, rmh, action, demand, demand);
                        else
                            return false;
                    }
                }

                // Make the sure the permission is not refused.

                if (refusedSet != null)
                {
                    CodeAccessPermission refusedPerm = 
                        (CodeAccessPermission)refusedSet.GetPermission(permToken);
                    if (refusedPerm != null)
                    {
                        if (!refusedPerm.CheckDeny(demand))
                        {
        #if _DEBUG
                            if (debug)
                                DEBUG_OUT( "Permission found in refused set" );
        #endif
                                if (throwException)
                                    ThrowSecurityException(assemblyOrString, grantedSet, refusedSet, rmh, action, demand, demand);
                                else
                                    return false;

                        }
                    }

                    if (refusedSet.IsUnrestricted() && demand.CanUnrestrictedOverride())
                    {
                        if (throwException)
                            ThrowSecurityException(assemblyOrString, grantedSet, refusedSet, rmh, action, demand, demand);
                        else
                            return false;
                    }
                }
            }
            catch (SecurityException)
            {
                throw;
            }
            catch (Exception)
            {
                // Any exception besides a security exception in this code means that
                // a permission was unable to properly handle what we asked of it.
                // We will define this to mean that the demand failed.
                if (throwException)
                    ThrowSecurityException(assemblyOrString, grantedSet, refusedSet, rmh, action, demand, demand);
                else
                    return false;
            }
            catch
            {
                return false;
            }
            finally
            {
                if (bThreadSecurity)
                    SecurityManager._SetThreadSecurity(true);
            }

            DEBUG_OUT( "Check passed" );
            return true;
        }


        internal static void GetZoneAndOriginHelper( CompressedStack cs, PermissionSet grantSet, PermissionSet refusedSet, ArrayList zoneList, ArrayList originList )
        {
            if (cs != null)
                cs.GetZoneAndOrigin(zoneList, originList, PermissionToken.GetToken(typeof(ZoneIdentityPermission)), PermissionToken.GetToken(typeof(UrlIdentityPermission)));
            else
        {
            ZoneIdentityPermission zone = (ZoneIdentityPermission)grantSet.GetPermission( typeof( ZoneIdentityPermission ) );
            UrlIdentityPermission url = (UrlIdentityPermission)grantSet.GetPermission( typeof( UrlIdentityPermission ) );

            if (zone != null)
                zoneList.Add( zone.SecurityZone );

            if (url != null)
                originList.Add( url.Url );
            }
        }

        internal static void GetZoneAndOrigin( ref StackCrawlMark mark, out ArrayList zone, out ArrayList origin )
        {
            zone = new ArrayList();
            origin = new ArrayList();

            GetZoneAndOriginInternal( zone, origin, ref mark);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void GetZoneAndOriginInternal(ArrayList zoneList, 
                                  ArrayList originList, 
                                  ref StackCrawlMark stackMark);

        internal static void CheckAssembly( Assembly asm, CodeAccessPermission demand )
        {
            BCLDebug.Assert( asm != null, "Must pass in a good assembly" );
            BCLDebug.Assert( demand != null, "Must pass in a good demand" );
            if (SecurityManager._IsSecurityOn())
            {
                PermissionSet granted, refused;
                asm.nGetGrantSet( out granted, out refused );
                CheckHelper( granted, refused, demand, PermissionToken.GetToken(demand), RuntimeMethodHandle.EmptyHandle, asm, SecurityAction.Demand, true );
            }
        }

        // Check - Used to initiate a code-access security check.
        // This method invokes a stack walk after skipping to the frame
        // referenced by stackMark.
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        private static extern void Check (Object demand,
                                  ref StackCrawlMark stackMark, 
                                  bool isPermSet);

  
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool QuickCheckForAllDemands();
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern bool AllDomainsHomogeneousWithNoStackModifiers();

        internal static void Check(CodeAccessPermission cap, ref StackCrawlMark stackMark)
        {
            Check(cap,
                  ref stackMark,
                  false);
        }


        internal static void Check(PermissionSet permSet, ref StackCrawlMark stackMark)
        {
            Check(permSet,
                 ref stackMark,
                 true);
        }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern FrameSecurityDescriptor CheckNReturnSO(PermissionToken permToken, 
                                                                    CodeAccessPermission demand, 
                                                                    ref StackCrawlMark stackMark,
                                                                    int unrestrictedOverride, 
                                                                    int create );

        internal static void Assert(CodeAccessPermission cap, ref StackCrawlMark stackMark)
        {
            // Make sure the caller of assert has the permission to assert
            //WARNING: The placement of the call here is just right to check
            //         the appropriate frame.
            
            // Note: if the "AssertPermission" is not a permission that implements IUnrestrictedPermission
            // you need to change the last parameter to a zero.
            BCLDebug.Assert(AssertPermissionToken != null && AssertPermission != null, "Assert Permission not setup correctly");
            FrameSecurityDescriptor secObj = CheckNReturnSO(AssertPermissionToken,
                                                            AssertPermission,
                                                            ref stackMark,
                                                            1,
                                                            1 );
            if (secObj == null)
            {
                if (SecurityManager._IsSecurityOn())
                    // Security: REQ_SQ flag is missing. Bad compiler ?
                    // This can happen when you create delegates over functions that need the REQ_SQ 
                    throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
            }
            else
            {
                if (secObj.HasImperativeAsserts())
                    throw new SecurityException( Environment.GetResourceString( "Security_MustRevertOverride" ) );

                secObj.SetAssert(cap);
            }
        }

        internal static void Deny(CodeAccessPermission cap, ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj =
                SecurityRuntime.GetSecurityObjectForFrame(ref stackMark, true);
            if (secObj == null)
            {
                if (SecurityManager._IsSecurityOn())
                    // Security: REQ_SQ flag is missing. Bad compiler ?
                    // This can happen when you create delegates over functions that need the REQ_SQ 
                    throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
            }
            else
            {
                if (secObj.HasImperativeDenials())
                    throw new SecurityException( Environment.GetResourceString( "Security_MustRevertOverride" ) );

                secObj.SetDeny(cap);
        }
        }
        
        internal static void PermitOnly(CodeAccessPermission cap, ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj =
                SecurityRuntime.GetSecurityObjectForFrame(ref stackMark, true);
            if (secObj == null)
            {
                if (SecurityManager._IsSecurityOn())
                    // Security: REQ_SQ flag is missing. Bad compiler ?
                    // This can happen when you create delegates over functions that need the REQ_SQ 
                    throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
            }
            else
            {
                if (secObj.HasImperativeRestrictions())
                    throw new SecurityException( Environment.GetResourceString( "Security_MustRevertOverride" ) );

                secObj.SetPermitOnly(cap);
            }
        }
        
        // Update the PLS used for optimization in the AppDomain: called from the VM
        private static PermissionListSet UpdateAppDomainPLS(PermissionListSet adPLS, PermissionSet grantedPerms, PermissionSet refusedPerms) {
            if (adPLS == null) {
                adPLS = new PermissionListSet();
                adPLS.UpdateDomainPLS(grantedPerms, refusedPerms);
                return adPLS;
            } else {
                PermissionListSet newPLS = new PermissionListSet();
                newPLS.UpdateDomainPLS(adPLS);
                newPLS.UpdateDomainPLS(grantedPerms, refusedPerms);
                return newPLS;
            }
        }
    }
}
