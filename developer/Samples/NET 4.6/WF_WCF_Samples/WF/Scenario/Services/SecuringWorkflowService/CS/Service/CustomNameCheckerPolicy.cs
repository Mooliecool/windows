//-----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

using System.Collections.Generic;
using System.Diagnostics;
using System.IdentityModel.Claims;
using System.IdentityModel.Policy;
using System.Security.Principal;

namespace Microsoft.Samples.WF.SecuringWorkFlow
{

    class CustomNameCheckerPolicy : IAuthorizationPolicy
    {
        #region IAuthorizationPolicy Members

        //We will add a custom claim to the EvaluationContext if the 'magic character' exists in the username
        public bool Evaluate(EvaluationContext evaluationContext, ref object state)
        {
            //Get the Identities Property
            object obj;
            if (!evaluationContext.Properties.TryGetValue("Identities", out obj))
            {
                Debug.WriteLine("CustomNameCheckerPolicy: Identities is null");
                return false;
            }

            //Get the list of IIdentities
            IList<IIdentity> identities = obj as IList<IIdentity>;
            if (identities == null || identities.Count != 1)
            {
                Debug.WriteLine("CustomNameCheckerPolicy: Identities.count = 0");
                return false;
            }

            //Get the WindowsIdentity
            //Any other type of IIdentity will cause it to return false
            WindowsIdentity wID = identities[0] as WindowsIdentity;
            if (wID == null)
            {
                Debug.WriteLine("CustomNameCheckerPolicy: Not a Windows Identity");
                return false;
            }

            //retrieve the username 
            string[] domainAndUsername =  wID.Name.Split(new char[]{'\\'});
            string userName = domainAndUsername[0];
            if (domainAndUsername.Length > 1)
                userName = domainAndUsername[1];

            //Check to see if the 'magic character' exists in the username
            //Add our claim if it does
            if (userName.ToUpper().Contains(Constants.magicCharacter.ToUpper()))
            {
                List<Claim> claims = new List<Claim>(1);
                Claim magicCharacterClaim = new Claim(Constants.PossessesMagicCharacterType, Constants.magicCharacter, Rights.PossessProperty);
                claims.Add(magicCharacterClaim);
                evaluationContext.AddClaimSet(this, new DefaultClaimSet(claims));
            }

            return true;
        }

        public ClaimSet Issuer
        {
            get { return ClaimSet.System; }
        }

        #endregion

        #region IAuthorizationComponent Members

        public string Id
        {
            get 
            {
                return this.GetType().FullName;
            }
        }

        #endregion
    }
}
