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
    using System.Globalization;
    using System.Threading;
    using System.Reflection;
    using System.Collections;
    using System.Runtime.CompilerServices;
    using System.Security.Permissions;

    internal class SecurityRuntime
    {
        private SecurityRuntime(){}

        // Returns the security object for the caller of the method containing
        // 'stackMark' on its frame.
        //
        // THE RETURNED OBJECT IS THE LIVE RUNTIME OBJECT. BE CAREFUL WITH IT!
        //
        // Internal only, do not doc.
        // 
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal static extern 
        FrameSecurityDescriptor GetSecurityObjectForFrame(ref StackCrawlMark stackMark,
                                                          bool create);

        // Constants used to return status to native
        internal const bool StackContinue  = true;
        internal const bool StackHalt      = false;
        
        // Returns the number of negative overrides(deny/permitonly) in this secDesc
        private static int OverridesHelper(FrameSecurityDescriptor secDesc)
        {
            // check imperative
            int count = OverridesHelper2(secDesc, false);
            // add declarative
            count += OverridesHelper2(secDesc, true);
            return count;
            
        }
        private static int OverridesHelper2(FrameSecurityDescriptor secDesc, bool fDeclarative)
        {
            PermissionSet permSet;
            int count = 0;
            
            permSet = secDesc.GetPermitOnly(fDeclarative);
            if (permSet != null)
                count++;
            permSet = secDesc.GetDenials(fDeclarative);
            if (permSet != null)
                count++;
            return count;
        }

        // this method is a big perf hit, so don't call unnecessarily
        internal static MethodInfo GetMethodInfo(RuntimeMethodHandle rmh)
        {
            if(rmh.IsNullHandle())
                return null;

#if _DEBUG
            try
            {
#endif
                // Assert here because reflection will check grants and if we fail the check,
                // there will be an infinite recursion that overflows the stack.
                PermissionSet.s_fullTrust.Assert();
                RuntimeTypeHandle rth = rmh.GetDeclaringType();
                return(System.RuntimeType.GetMethodBase(rth, rmh) as MethodInfo);
#if _DEBUG
            }
            catch(Exception)
            {
                return null;
            }
#endif
        }

        private static bool FrameDescSetHelper(FrameSecurityDescriptor secDesc,
                                               PermissionSet demandSet,
                                               out PermissionSet alteredDemandSet,
                                               RuntimeMethodHandle rmh)
        {
            return secDesc.CheckSetDemand(demandSet, out alteredDemandSet, rmh);
        }
        
        private static bool FrameDescHelper(FrameSecurityDescriptor secDesc,
                                               IPermission demandIn, 
                                               PermissionToken permToken,
                                               RuntimeMethodHandle rmh)
        {
            return secDesc.CheckDemand((CodeAccessPermission) demandIn, permToken, rmh);
        }
        
        //
        // API for PermissionSets
        //
        
        internal static void Assert(PermissionSet permSet, ref StackCrawlMark stackMark)
        {
            // Note: if the "AssertPermission" is not a permission that implements IUnrestrictedPermission
            // you need to change the fourth parameter to a zero.
            FrameSecurityDescriptor secObj = CodeAccessSecurityEngine.CheckNReturnSO(
                                                CodeAccessSecurityEngine.AssertPermissionToken,
                                                CodeAccessSecurityEngine.AssertPermission,
                                                ref stackMark,
                                                1,
                                                1 );
    
            BCLDebug.Assert(secObj != null || !SecurityManager._IsSecurityOn(),"Failure in SecurityRuntime.Assert() - secObj != null");
            if (secObj == null)
            {
                if (SecurityManager._IsSecurityOn())
                // Security: REQ_SQ flag is missing. Bad compiler ?
                throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
            }
            else
            {
                if (secObj.HasImperativeAsserts())
                    throw new SecurityException( Environment.GetResourceString( "Security_MustRevertOverride" ) );

                secObj.SetAssert(permSet);
            }
        }
    
        internal static void AssertAllPossible(ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj =
                SecurityRuntime.GetSecurityObjectForFrame(ref stackMark, true);
    
            BCLDebug.Assert(secObj != null || !SecurityManager._IsSecurityOn(),"Failure in SecurityRuntime.AssertAllPossible() - secObj != null");
            if (secObj == null)
            {
                if (SecurityManager._IsSecurityOn())
                    // Security: REQ_SQ flag is missing. Bad compiler ?
                    throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
            }
            else
            {
                if (secObj.GetAssertAllPossible())
                    throw new SecurityException( Environment.GetResourceString( "Security_MustRevertOverride" ) );

                secObj.SetAssertAllPossible();
            }
        }
    
        internal static void Deny(PermissionSet permSet, ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj =
                SecurityRuntime.GetSecurityObjectForFrame(ref stackMark, true);
    
            BCLDebug.Assert(secObj != null || !SecurityManager._IsSecurityOn(),"Failure in SecurityRuntime.Deny() - secObj != null");
            if (secObj == null)
            {
                if (SecurityManager._IsSecurityOn())
                // Security: REQ_SQ flag is missing. Bad compiler ?
                throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
            }
            else
            {
                if (secObj.HasImperativeDenials())
                    throw new SecurityException( Environment.GetResourceString( "Security_MustRevertOverride" ) );

                secObj.SetDeny(permSet);
            }
        }
    
        internal static void PermitOnly(PermissionSet permSet, ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj =
                SecurityRuntime.GetSecurityObjectForFrame(ref stackMark, true);
    
            BCLDebug.Assert(secObj != null || !SecurityManager._IsSecurityOn(),"Failure in SecurityRuntime.PermitOnly() - secObj != null");
            if (secObj == null)
            {
                if (SecurityManager._IsSecurityOn())
                // Security: REQ_SQ flag is missing. Bad compiler ?
                throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
            }
            else
            {
                if (secObj.HasImperativeRestrictions())
                    throw new SecurityException( Environment.GetResourceString( "Security_MustRevertOverride" ) );

                secObj.SetPermitOnly(permSet);
            }
        }
    
        //
        // Revert API
        //
        
        internal static void RevertAssert(ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj = GetSecurityObjectForFrame(ref stackMark, false);
            if (secObj != null)
            {
                secObj.RevertAssert();
            }
            else if (SecurityManager._IsSecurityOn())
                throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
        }

        internal static void RevertDeny(ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj = GetSecurityObjectForFrame(ref stackMark, false);
            if (secObj != null)
            {
                secObj.RevertDeny();
            }
            else if (SecurityManager._IsSecurityOn())
                throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
        }
        
        internal static void RevertPermitOnly(ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj = GetSecurityObjectForFrame(ref stackMark, false);
            if (secObj != null)
            {
                secObj.RevertPermitOnly();
            }
            else if (SecurityManager._IsSecurityOn())
                throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
        }
        
        internal static void RevertAll(ref StackCrawlMark stackMark)
        {
            FrameSecurityDescriptor secObj = GetSecurityObjectForFrame(ref stackMark, false);
            if (secObj != null)
            {
                secObj.RevertAll();
            }
            else if (SecurityManager._IsSecurityOn())
                throw new ExecutionEngineException( Environment.GetResourceString( "ExecutionEngine_MissingSecurityDescriptor" ) );
        }
    }
}


