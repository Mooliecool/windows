//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System.Runtime.Serialization;
using System.ServiceModel;
using System.ServiceModel.Web;

namespace Microsoft.Ajax.Samples
{
   // Define a service contract.
    [ServiceContract(Namespace = "WCTMService")]
    public interface ICalculator
    {
        [OperationContract]
        [WebInvoke(ResponseFormat = WebMessageFormat.Json, BodyStyle = WebMessageBodyStyle.Wrapped)]
        MathResult DoMath(double n1, double n2);
    }

    public class CalculatorService : ICalculator
    {
        public MathResult DoMath(double n1, double n2)
        {
            MathResult mr = new MathResult();
            mr.sum = n1 + n2;
            mr.difference = n1 - n2;
            mr.product = n1 * n2;
            mr.quotient = n1 / n2;
            return mr;
        }
    }

    [DataContract]
    public class MathResult
    {
        [DataMember]
        public double sum;

        [DataMember]
        public double difference;

        [DataMember]
        public double product;

        [DataMember]
        public double quotient;
    }
}