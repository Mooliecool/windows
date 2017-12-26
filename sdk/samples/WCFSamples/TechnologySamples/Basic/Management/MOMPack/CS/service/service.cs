using System;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    public interface ICalculator
    {
        [OperationContract]
        double Add(double n1, double n2);
        [OperationContract]
        double Subtract(double n1, double n2);
        [OperationContract]
        double Multiply(double n1, double n2);
        [OperationContract]
        double Divide(double n1, double n2);
    }

    // Service class which implements the service contract.
    public class CalculatorService : ICalculator
    {
        public CalculatorService()
        {
        }

        public double Add(double n1, double n2)
        {
            double result = n1 + n2;

            return result;
        }

        public double Subtract(double n1, double n2)
        {
            double result = n1 - n2;

            return result;
        }

        public double Multiply(double n1, double n2)
        {
            double result = n1 * n2;

            return result;
        }

        public double Divide(double n1, double n2)
        {
            double result = n1 / n2;

            return result;
        }
    }
}