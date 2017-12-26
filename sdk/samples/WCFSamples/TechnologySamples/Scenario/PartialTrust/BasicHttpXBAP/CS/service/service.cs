//  Copyright (c) Microsoft Corporation. All rights reserved.

using System.Runtime.Serialization;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface ICalcPlus
    {
        [OperationContract]
        CalcResult Add(double n1, double n2);

        [OperationContract]
        CalcResult Divide(double n1, double n2);

        [OperationContract]
        CalcResult Multiply(double n1, double n2);

        [OperationContract]
        CalcResult Subtract(double n1, double n2);
    }

    [DataContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public class CalcResult
    {
        [DataMember]
        public readonly double Number;

        [DataMember]
        public readonly string Words;

        // The constructor is not reconstituted on the client
        // in the proxy, but it is useful on the service.
        public CalcResult(double number)
        {
            Number = number;
            Words = NumberUtilities.NumberConverter.ConvertNumberToWords(number);
        }
    }

    // Service class that implements the service contract.
    public class CalcPlusService : ICalcPlus
    {
        public CalcResult Add(double n1, double n2)
        {
            return new CalcResult(n1 + n2);
        }

        public CalcResult Divide(double n1, double n2)
        {
            return new CalcResult(n1 / n2);
        }

        public CalcResult Multiply(double n1, double n2)
        {
            return new CalcResult(n1 * n2);
        }

        public CalcResult Subtract(double n1, double n2)
        {
            return new CalcResult(n1 - n2);
        }
    }

}
