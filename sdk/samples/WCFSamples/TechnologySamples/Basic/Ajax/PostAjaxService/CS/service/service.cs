//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System.ServiceModel;
using System.ServiceModel.Web;

namespace Microsoft.Ajax.Samples 
{
    // Define a service contract.
    [ServiceContract(Namespace = "PostAjaxService")]
    public interface ICalculator
    {
        [OperationContract]
        [WebInvoke]
        double Add(double n1, double n2);
        [OperationContract]
        [WebInvoke]
        double Subtract(double n1, double n2);
        [OperationContract]
        [WebInvoke]
        double Multiply(double n1, double n2);
        [OperationContract]
        [WebInvoke]
        double Divide(double n1, double n2);
    }

    public class CalculatorService : ICalculator
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
    }

}
