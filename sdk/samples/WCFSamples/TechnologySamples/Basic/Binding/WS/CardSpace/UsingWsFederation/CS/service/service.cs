
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ServiceModel;
using System.ServiceModel.Security.Tokens;
using System.IdentityModel;
using System.IdentityModel.Policy;
using System.IdentityModel.Claims;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface ISecureCalculator
    {
        [OperationContract]
        double Add(double n1, double n2);
        [OperationContract]
        double Subtract(double n1, double n2);
        [OperationContract]
        double Multiply(double n1, double n2);
        [OperationContract]
        double Divide(double n1, double n2);
        [OperationContract]
        string GetIdentity();

    }

    // Service class which implements the service contract.
    public class CalculatorService : ISecureCalculator
    {
        public double Add(double n1, double n2)
        {
            return n1 + n2;
        }

        public double Subtract(double n1, double n2)
        {
            return n1 - n2;
        }

        public double Multiply(double n1, double n2)
        {
            return n1 * n2;
        }

        public double Divide(double n1, double n2)
        {
            return n1 / n2;
        }

        
        public string GetIdentity()
        {
            string identity = String.Empty;

            AuthorizationContext ctx = OperationContext.Current.ServiceSecurityContext.AuthorizationContext;
            
            foreach (ClaimSet claimSet in ctx.ClaimSets)
            {
                foreach (Claim claim in claimSet)
                {
                    identity += "[" + claim.Resource as string + "] ";
                }
            }
            return identity;
        }

    }

}
