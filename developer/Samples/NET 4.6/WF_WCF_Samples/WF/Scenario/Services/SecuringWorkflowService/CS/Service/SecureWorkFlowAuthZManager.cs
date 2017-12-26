//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.IdentityModel.Claims;
using System.IdentityModel.Policy;
using System.ServiceModel;

namespace Microsoft.Samples.WF.SecuringWorkFlow
{

    class SecureWorkFlowAuthZManager : ServiceAuthorizationManager
    {
        //if our custom claim is in the claimsets we will return true, otherwise it will return false
        //which will result in an Access Denied error
        protected override bool CheckAccessCore(OperationContext operationContext)
        {
            //list out all claims for this call
            Debug.WriteLine("Claims");
            AuthorizationContext ac = operationContext.ServiceSecurityContext.AuthorizationContext;
            foreach (ClaimSet claimSet in ac.ClaimSets)
            {
                Debug.WriteLine("ClaimSet: " + claimSet.GetType().ToString());
                Debug.WriteLine("--------------------------");
                foreach (Claim clm in claimSet)
                {
                    string strClaimdata = "ClaimType: " + clm.ClaimType + Environment.NewLine +
                        "Resource: " + clm.Resource.ToString() + Environment.NewLine +
                        "Right: " + clm.Right + Environment.NewLine;
                    Debug.WriteLine(strClaimdata + Environment.NewLine);
                }
                Debug.WriteLine("");
            }

            //look for our claim
            foreach (ClaimSet claimSet in ac.ClaimSets)
            {
                foreach (Claim clm in claimSet.FindClaims(Constants.PossessesMagicCharacterType, Rights.PossessProperty))
                {
                    //We found our claim and can now acess the workflow
                    return true;
                }
            }

            //We didn't find the claim and thus cannot access the workflow
            return false;
        }
    }
}
