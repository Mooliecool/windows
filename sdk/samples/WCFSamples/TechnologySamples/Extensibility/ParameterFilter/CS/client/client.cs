
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;

using System.ServiceModel.Channels;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
    // The service contract is defined in generatedClient.cs, generated from the service by the svcutil tool.

    class Client
    {
        static void Main()
        {
            // Create a client with given client endpoint configuration
            CalculatorClient client = new CalculatorClient();

            try
            {
                double value1;
                double value2;
                double result;

                // Call the Multiply service operation with arguments between 1 and 10
                value1 = 2.00D;
                value2 = 5.25D;
                result = client.Multiply(value1, value2);
                Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result);

                // Call the Multiply service operation with arguments outside 1 and 10
                value1 = 9.00D;
                value2 = 81.25D;
                result = client.Multiply(value1, value2);
                Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result);
            }
            catch (FaultException e)
            {
                Console.WriteLine("{0}: {1}", e.GetType(), e.Message);
            }

            //Closing the client gracefully closes the connection and cleans up resources
            client.Close();

            Console.WriteLine();
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }
    }
}