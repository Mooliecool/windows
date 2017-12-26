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
** Class: PermissionListSet.cs
**
** Purpose: Holds state about A/G/R permissionsets in a callstack or appdomain
**          (Replacement for PermissionListSet)
**
=============================================================================*/

namespace System.Security
{
    using System.Globalization;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.Security;
    using System.Security.Permissions;
    using System.Threading;
    using System.Collections;

    [Serializable()]
    sealed internal class PermissionListSet
    {
        // Only internal (public) methods are creation methods and demand evaluation methods.
        // Scroll down to the end to see them.
        private PermissionSetTriple m_firstPermSetTriple;
        private ArrayList m_permSetTriples;
        private ArrayList m_zoneList;
        private ArrayList m_originList;

        internal PermissionListSet() {}

        private void EnsureTriplesListCreated()
        {
            if (m_permSetTriples == null)
            {
                m_permSetTriples = new ArrayList();
                if (m_firstPermSetTriple != null)
                {
                    m_permSetTriples.Add(m_firstPermSetTriple);
                    m_firstPermSetTriple = null;
                }
            }
        }

        internal void UpdateDomainPLS (PermissionListSet adPLS) {
            if (adPLS != null && adPLS.m_firstPermSetTriple != null)
                UpdateDomainPLS(adPLS.m_firstPermSetTriple.GrantSet, adPLS.m_firstPermSetTriple.RefusedSet);
        }

        internal void UpdateDomainPLS (PermissionSet grantSet, PermissionSet deniedSet) {
            BCLDebug.Assert(m_permSetTriples == null, "m_permSetTriples != null");
            if (m_firstPermSetTriple == null)
                m_firstPermSetTriple = new PermissionSetTriple();

            // update the grant and denied sets
            m_firstPermSetTriple.UpdateGrant(grantSet);
            m_firstPermSetTriple.UpdateRefused(deniedSet);
        }

        private void Terminate(PermissionSetTriple currentTriple)
        {
            UpdateTripleListAndCreateNewTriple(currentTriple, null);
        }

        private void Terminate(PermissionSetTriple currentTriple, PermissionListSet pls)
        {
            this.UpdateZoneAndOrigin(pls);
            this.UpdatePermissions(currentTriple, pls);
            this.UpdateTripleListAndCreateNewTriple(currentTriple, null);
        }

        private bool Update(PermissionSetTriple currentTriple, PermissionListSet pls)
        {
            this.UpdateZoneAndOrigin(pls);
            return this.UpdatePermissions(currentTriple, pls);
        }

        private bool Update(PermissionSetTriple currentTriple, FrameSecurityDescriptor fsd)
        {
           // check imperative
           bool fHalt = Update2(currentTriple, fsd, false);
           if (!fHalt)            
           {
                // then declarative
                fHalt = Update2(currentTriple, fsd, true);
           }
           return fHalt;
        }
        
        private bool Update2(PermissionSetTriple currentTriple, FrameSecurityDescriptor fsd, bool fDeclarative)
        {
            // Deny
            PermissionSet deniedPset = fsd.GetDenials(fDeclarative);
            if (deniedPset != null)
            {
                currentTriple.UpdateRefused(deniedPset);
            }

            // permit only
            PermissionSet permitOnlyPset = fsd.GetPermitOnly(fDeclarative);
            if (permitOnlyPset != null)
            {
                currentTriple.UpdateGrant(permitOnlyPset);
            }

            // Assert all possible
            if (fsd.GetAssertAllPossible())
                return true; // halt the construction
                
            // Assert
            PermissionSet assertPset = fsd.GetAssertions(fDeclarative);
            if (assertPset != null)
            {
                if (assertPset.IsUnrestricted())
                    return true; // halt the construction
                PermissionSetTriple retTriple = currentTriple.UpdateAssert(assertPset);
                if (retTriple != null)
                {
                    EnsureTriplesListCreated();
                    m_permSetTriples.Add(retTriple);
                }
            }
                
            return false;
        }
        private void Update(PermissionSetTriple currentTriple, PermissionSet in_g, PermissionSet in_r)
        {
            ZoneIdentityPermission z;
            UrlIdentityPermission u;
            currentTriple.UpdateGrant(in_g, out z, out u);
            currentTriple.UpdateRefused(in_r);
            AppendZoneOrigin(z, u);
        }

        // Called from the VM for HG CS construction        
        private void Update(PermissionSet in_g)
        {
            if (m_firstPermSetTriple == null)
                m_firstPermSetTriple = new PermissionSetTriple();
            Update(m_firstPermSetTriple, in_g, null);
        }
        
        private void UpdateZoneAndOrigin(PermissionListSet pls)
        {
            if (pls != null)
            {
                if (this.m_zoneList == null && pls.m_zoneList != null && pls.m_zoneList.Count > 0)
                    this.m_zoneList = new ArrayList();
                UpdateArrayList(this.m_zoneList, pls.m_zoneList);
                if (this.m_originList == null && pls.m_originList != null && pls.m_originList.Count > 0)
                    this.m_originList = new ArrayList();                
                UpdateArrayList(this.m_originList, pls.m_originList);
            }
        }

        private bool UpdatePermissions(PermissionSetTriple currentTriple, PermissionListSet pls)
        {
            if (pls != null)
            {
                if (pls.m_permSetTriples != null)
                {
                    // DCS has an AGR List. So we need to add the AGR List
                    UpdateTripleListAndCreateNewTriple(currentTriple,pls.m_permSetTriples);
                }
                else
                {
                    // Common case: One AGR set
                    
                    PermissionSetTriple tmp_psTriple = pls.m_firstPermSetTriple;
                    PermissionSetTriple retTriple;
                    // First try and update currentTriple. Return value indicates if we can stop construction
                    if (currentTriple.Update(tmp_psTriple, out retTriple))
                        return true;
                    // If we got a non-null retTriple, what it means is that compression failed,
                    // and we now have 2 triples to deal with: retTriple and currentTriple.
                    // retTriple has to be appended first. then currentTriple.
                    if (retTriple != null)
                    {
                        EnsureTriplesListCreated();
                        // we just created a new triple...add the previous one (returned) to the list
                        m_permSetTriples.Add(retTriple);
                    }
                }
            }
            else
            {
                // pls can be null only outside the loop in CreateCompressedState
                UpdateTripleListAndCreateNewTriple(currentTriple, null);
            }
            

            return false;
            
        }


        private void UpdateTripleListAndCreateNewTriple(PermissionSetTriple currentTriple, ArrayList tripleList)
        {
            if (!currentTriple.IsEmpty())
            {
                if (m_firstPermSetTriple == null && m_permSetTriples == null)
                {
                    m_firstPermSetTriple = new PermissionSetTriple(currentTriple);
                }
                else
                {
                    EnsureTriplesListCreated();
                    m_permSetTriples.Add(new PermissionSetTriple(currentTriple));
                }
                currentTriple.Reset();
            }
            if (tripleList != null)
            {
                EnsureTriplesListCreated();
                m_permSetTriples.AddRange(tripleList);
            }
        }

        private static void UpdateArrayList(ArrayList current, ArrayList newList)
        {
            if (newList == null)
                return;

            for(int i=0;i < newList.Count; i++)
            {
                if (!current.Contains(newList[i]))
                    current.Add(newList[i]);
            }
        
        }

        private void AppendZoneOrigin(ZoneIdentityPermission z, UrlIdentityPermission u)
        {

            if (z != null)
            {
                if (m_zoneList == null)
                    m_zoneList = new ArrayList();
                m_zoneList.Add( z.SecurityZone );
            }

            if (u != null)
            {
                if (m_originList == null)
                    m_originList = new ArrayList();
                m_originList.Add( u.Url );
            }
        }

[System.Runtime.InteropServices.ComVisible(true)]
        // public(internal) interface begins...
        // Creation functions
        static internal PermissionListSet CreateCompressedState(CompressedStack cs, CompressedStack innerCS)
        {
            // function that completes the construction of the compressed stack if not done so already (bottom half for demand evaluation)
            
            bool bHaltConstruction = false;
            if (cs.CompressedStackHandle == null)
                return null; //  FT case or Security off
   
            PermissionListSet pls = new PermissionListSet();
            PermissionSetTriple currentTriple = new PermissionSetTriple();
            int numDomains = CompressedStack.GetDCSCount(cs.CompressedStackHandle);
            for (int i=numDomains-1; (i >= 0 && !bHaltConstruction) ; i--)
            {
                DomainCompressedStack dcs = CompressedStack.GetDomainCompressedStack(cs.CompressedStackHandle, i);
                if (dcs == null)
                    continue; // we hit a FT Domain
                if (dcs.PLS == null)
                {
                    // We failed on some DCS
                    throw new SecurityException(String.Format(CultureInfo.InvariantCulture, Environment.GetResourceString("Security_Generic")));
                }
                pls.UpdateZoneAndOrigin(dcs.PLS);
                pls.Update(currentTriple, dcs.PLS); 
                bHaltConstruction = dcs.ConstructionHalted;
            }
            if (!bHaltConstruction)
            {
                PermissionListSet tmp_pls = null;
                // Construction did not halt. 
                if (innerCS != null)
                {
                    innerCS.CompleteConstruction(null);
                    tmp_pls = innerCS.PLS;
                }
                pls.Terminate(currentTriple, tmp_pls);
            }
            else
            {
                pls.Terminate(currentTriple);
            }

            return pls;
        }

        static internal PermissionListSet CreateCompressedState(IntPtr unmanagedDCS, out bool bHaltConstruction)
        {
            PermissionListSet pls = new PermissionListSet();
            PermissionSetTriple currentTriple = new PermissionSetTriple();

            PermissionSet tmp_g, tmp_r;
            // Construct the descriptor list
            int descCount = DomainCompressedStack.GetDescCount(unmanagedDCS);
            bHaltConstruction = false;
            for(int i=0; (i < descCount && !bHaltConstruction); i++)
            {
                FrameSecurityDescriptor fsd;
                Assembly assembly;
                if (DomainCompressedStack.GetDescriptorInfo(unmanagedDCS, i, out tmp_g, out tmp_r, out assembly, out fsd))
                {
                    // Got an FSD
                    bHaltConstruction = pls.Update(currentTriple, fsd);
                }
                else
                {
                    pls.Update(currentTriple, tmp_g, tmp_r);
                }
                
            }
            if (!bHaltConstruction)
            {
                // domain
                if (!DomainCompressedStack.IgnoreDomain(unmanagedDCS))
                {
                    DomainCompressedStack.GetDomainPermissionSets(unmanagedDCS, out tmp_g, out tmp_r);
                    pls.Update(currentTriple, tmp_g, tmp_r);
                }
            }
            pls.Terminate(currentTriple);


            // return the created object
            return pls;
            
        }
        static internal PermissionListSet CreateCompressedState_HG()
        {
            PermissionListSet pls = new PermissionListSet();
            CompressedStack.GetHomogeneousPLS(pls);
            return pls;
        }

        // Private Demand evaluation functions - only called from the VM
        internal bool CheckDemandNoThrow(CodeAccessPermission demand)
        {
            // AppDomain permissions - no asserts. So there should only be one triple to work with
            BCLDebug.Assert(m_permSetTriples == null && m_firstPermSetTriple != null, "More than one PermissionSetTriple encountered in AD PermissionListSet");
            

            
            PermissionToken permToken = null;
            if (demand != null)
                permToken = PermissionToken.GetToken(demand);

            return m_firstPermSetTriple.CheckDemandNoThrow(demand, permToken);
                

        }
        internal bool CheckSetDemandNoThrow(PermissionSet pSet)
        {
            // AppDomain permissions - no asserts. So there should only be one triple to work with
            BCLDebug.Assert(m_permSetTriples == null && m_firstPermSetTriple != null, "More than one PermissionSetTriple encountered in AD PermissionListSet");

            
            return m_firstPermSetTriple.CheckSetDemandNoThrow(pSet);
        }

        // Demand evauation functions
        internal bool CheckDemand(CodeAccessPermission demand, PermissionToken permToken, RuntimeMethodHandle rmh)
        {
            bool bRet = SecurityRuntime.StackContinue;
            if (m_permSetTriples != null)
            {
                for (int i=0; (i < m_permSetTriples.Count && bRet != SecurityRuntime.StackHalt) ; i++)
                {

                    PermissionSetTriple psTriple = (PermissionSetTriple)m_permSetTriples[i];
                    bRet = psTriple.CheckDemand(demand, permToken, rmh);
                }
            }
            else if (m_firstPermSetTriple != null)
            {
                bRet = m_firstPermSetTriple.CheckDemand(demand, permToken, rmh);
            }

            return SecurityRuntime.StackHalt; //  CS demand check always terminates the stackwalk
        }

        internal bool CheckSetDemand(PermissionSet pset , RuntimeMethodHandle rmh)
        {
            bool bRet = SecurityRuntime.StackContinue;
            PermissionSet demandSet = pset;
            PermissionSet alteredDemandSet;
            if (m_permSetTriples != null)
            {
                for (int i=0; (i < m_permSetTriples.Count && bRet != SecurityRuntime.StackHalt) ; i++)
                {

                    PermissionSetTriple psTriple = (PermissionSetTriple)m_permSetTriples[i];
                    bRet = psTriple.CheckSetDemand(demandSet, out alteredDemandSet, rmh);
                    if (alteredDemandSet != null)
                        demandSet = alteredDemandSet;
                }
            }
            else if (m_firstPermSetTriple != null)
            {
                bRet = m_firstPermSetTriple.CheckSetDemand(demandSet, out alteredDemandSet, rmh);
            }

            return SecurityRuntime.StackHalt; //  CS demand check always terminates the stackwalk            
        }

        internal void GetZoneAndOrigin(ArrayList zoneList, ArrayList originList, PermissionToken zoneToken, PermissionToken originToken)
        {
            if (m_zoneList != null)
                zoneList.AddRange(m_zoneList);
            if (m_originList != null)
                originList.AddRange(m_originList);
        }
    }
}
