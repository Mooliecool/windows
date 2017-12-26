
//-----------------------------------------------------------------
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.
//-----------------------------------------------------------------

using System;
using System.ServiceModel;

namespace Microsoft.Samples.AdvancedFilters
{
    // Define a service contract.
    [ServiceContract(Namespace = "http://Microsoft.Samples.AdvancedFilters")]
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
    [ServiceBehavior(AddressFilterMode = AddressFilterMode.Any)]
    public class CalculatorService : ICalculator
    {
        public double Add(double n1, double n2)
        {
            Console.WriteLine("Regular Calc: Add");
            return n1 + n2;
        }

        public double Subtract(double n1, double n2)
        {
            Console.WriteLine("Regular Calc: Subtract");
            return n1 - n2;
        }

        public double Multiply(double n1, double n2)
        {
            Console.WriteLine("Regular Calc: Multiply");
            return n1 * n2;
        }

        public double Divide(double n1, double n2)
        {
            Console.WriteLine("Regular Calc: Divide");
            return n1 / n2;
        }

        // Host the service within this EXE console application.
        public static void Main()
        {
            // Create a ServiceHost for the CalculatorService type.
            using (ServiceHost serviceHost =
                new ServiceHost(typeof(CalculatorService)))
            {
                // Open the ServiceHost to create listeners         
                // and start listening for messages.
                serviceHost.Open();

                // The service can now be accessed.
                Console.WriteLine("The regular calculator service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");
                Console.WriteLine();
                Console.ReadLine();
            }
        }
    }





}
