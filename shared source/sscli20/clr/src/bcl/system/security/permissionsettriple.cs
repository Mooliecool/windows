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
/*=============================================================================
**
** Class: PermissionSetTriple
**
** Purpose: Container class for holding an AppDomain's Grantset and Refused sets.
**          Also used for CompressedStacks which brings in the third PermissionSet.
**          Hence, the name PermissionSetTriple. 
**
=============================================================================*/

namespace System.Security
{
    using IEnumerator = System.Collections.IEnumerator;
    using System.Security;
    using System.Security.Permissions;
    using System.Runtime.InteropServices;


    [Serializable()]
    sealed internal class PermissionSetTriple
    {
        unsafe static private RuntimeMethodHandle s_emptyRMH = new RuntimeMethodHandle(null);
        static private PermissionToken s_zoneToken;
        static private PermissionToken s_urlToken;
        internal PermissionSet AssertSet;
        internal PermissionSet GrantSet;
        internal PermissionSet RefusedSet;

        internal PermissionSetTriple()
        {
            Reset();
        }
        internal PermissionSetTriple(PermissionSetTriple triple)
        {
            this.AssertSet = triple.AssertSet;
            this.GrantSet = triple.GrantSet;
            this.RefusedSet = triple.RefusedSet;
        }
        internal void Reset()
        {
            AssertSet = null;
            GrantSet = null;
            RefusedSet = null;
        }
        internal bool IsEmpty()
        {
            return (AssertSet == null && GrantSet == null && RefusedSet == null);
        }

        private PermissionToken ZoneToken
        {
            get
            {
                if (s_zoneToken == null)
                    s_zoneToken =  PermissionToken.GetToken(typeof(ZoneIdentityPermission));
                return s_zoneToken;
            }
        }            
        private PermissionToken UrlToken
        {
            get
            {
                if (s_urlToken == null)
                    s_urlToken =  PermissionToken.GetToken(typeof(UrlIdentityPermission));
                return s_urlToken;
            }
        }            
        internal bool Update(PermissionSetTriple psTriple, out PermissionSetTriple retTriple)
        {
            retTriple = null;
            // Special case: unrestricted assert. Note: dcs.Assert.IsUnrestricted => dcs.Grant.IsUnrestricted
            if (psTriple.AssertSet != null && psTriple.AssertSet.IsUnrestricted())
            {
                return true; // stop construction
            }
            retTriple = UpdateAssert(psTriple.AssertSet);
            UpdateGrant(psTriple.GrantSet);
            UpdateRefused(psTriple.RefusedSet);
            return false;
        }

        internal PermissionSetTriple UpdateAssert(PermissionSet in_a)
        {
            PermissionSetTriple retTriple = null;
            if (in_a != null)
            {
                BCLDebug.Assert((!in_a.IsUnrestricted()), "Cannot be unrestricted here");
                // if we're already asserting in_a, nothing to do
                if (in_a.IsSubsetOf(AssertSet))
                    return null;

                PermissionSet retPs;
                if (GrantSet != null)
                    retPs = in_a.Intersect(GrantSet); // Restrict the assert to what we've already been granted
                else
                {
                    GrantSet = new PermissionSet(true);
                    retPs = in_a.Copy(); // Currently unrestricted Grant: assert the whole assert set
                }
                bool bFailedToCompress;
                // removes anything that is already in the refused set from the assert set
                retPs = PermissionSet.RemoveRefusedPermissionSet(retPs, RefusedSet, out bFailedToCompress); 
                if (!bFailedToCompress)
                    bFailedToCompress = PermissionSet.IsIntersectingAssertedPermissions(retPs, AssertSet);
                if (bFailedToCompress)
                {
                    retTriple = new PermissionSetTriple(this);
                    this.Reset();
                    this.GrantSet = retTriple.GrantSet.Copy();
                }

                if (AssertSet == null)
                    AssertSet = retPs;
                else
                    AssertSet.InplaceUnion(retPs);

            }
            return retTriple;
        }
        internal void UpdateGrant(PermissionSet in_g, out ZoneIdentityPermission z,out UrlIdentityPermission u)
        {
            z = null;
            u = null;
            if (in_g != null)
            {
                if (GrantSet == null)
                    GrantSet = in_g.Copy();
                else
                    GrantSet.InplaceIntersect(in_g);
                
                z = (ZoneIdentityPermission)in_g.GetPermission(ZoneToken);
                u = (UrlIdentityPermission)in_g.GetPermission(UrlToken);
            }
        }

        internal void UpdateGrant(PermissionSet in_g)
        {
            if (in_g != null)
            {
                if (GrantSet == null)
                    GrantSet = in_g.Copy();
                else
                    GrantSet.InplaceIntersect(in_g);
            }
        }
        internal void UpdateRefused(PermissionSet in_r)
        {
            if (in_r != null)
            {
                if (RefusedSet == null)
                    RefusedSet = in_r.Copy();
                else
                    RefusedSet.InplaceUnion(in_r);
            }
        } 

        
        static bool CheckAssert(PermissionSet pSet, CodeAccessPermission demand, PermissionToken permToken)
        {
            if (pSet != null)
            {
                pSet.CheckDecoded(demand, permToken);

                CodeAccessPermission perm = (CodeAccessPermission)pSet.GetPermission(demand);
            
                // If the assert set does contain the demanded permission, halt the stackwalk

                try
                {
                    if ((pSet.IsUnrestricted() && demand.CanUnrestrictedOverride()) || demand.CheckAssert(perm))
                    {
                        return SecurityRuntime.StackHalt;
                    }
                }
                catch (ArgumentException)
                {
                }
            }
            return SecurityRuntime.StackContinue;
        }

        static bool CheckAssert(PermissionSet assertPset, PermissionSet demandSet, out PermissionSet newDemandSet)
        {
            newDemandSet = null;
            if (assertPset!= null)
            {
                assertPset.CheckDecoded(demandSet);
                // If this frame asserts a superset of the demand set we're done
            
                if (demandSet.CheckAssertion(assertPset))
                    return SecurityRuntime.StackHalt;
                PermissionSet.RemoveAssertedPermissionSet(demandSet, assertPset, out newDemandSet);
            }
            return SecurityRuntime.StackContinue;
        }

        
        internal bool CheckDemand(CodeAccessPermission demand, PermissionToken permToken, RuntimeMethodHandle rmh)
        {
            if (CheckAssert(AssertSet, demand, permToken) == SecurityRuntime.StackHalt)
                return SecurityRuntime.StackHalt;

            CodeAccessSecurityEngine.CheckHelper(GrantSet, RefusedSet, demand, permToken, rmh, null, SecurityAction.Demand, true);

            return SecurityRuntime.StackContinue;
        }
        internal bool CheckSetDemand(PermissionSet demandSet , out PermissionSet alteredDemandset, RuntimeMethodHandle rmh)
        {
            alteredDemandset = null;
            
            if (CheckAssert(AssertSet, demandSet, out alteredDemandset) == SecurityRuntime.StackHalt)
                return SecurityRuntime.StackHalt;
            if (alteredDemandset != null)
                demandSet = alteredDemandset; // note that this does not modify demandSet external to this function.
            CodeAccessSecurityEngine.CheckSetHelper(GrantSet, RefusedSet, demandSet, rmh, null, SecurityAction.Demand, true);

            return SecurityRuntime.StackContinue;

        }
        
        internal bool CheckDemandNoThrow(CodeAccessPermission demand, PermissionToken permToken)
        {
            BCLDebug.Assert(AssertSet == null, "AssertSet not null");

            return CodeAccessSecurityEngine.CheckHelper(GrantSet, RefusedSet, demand, permToken, s_emptyRMH, null, SecurityAction.Demand, false);
        }
        internal bool CheckSetDemandNoThrow(PermissionSet demandSet)
        {
            BCLDebug.Assert(AssertSet == null, "AssertSet not null");

            return CodeAccessSecurityEngine.CheckSetHelper(GrantSet, RefusedSet, demandSet, s_emptyRMH, null, SecurityAction.Demand, false);
        }        
    }
}


