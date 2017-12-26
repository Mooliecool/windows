
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.IdentityModel.Selectors;

namespace Microsoft.ServiceModel.Samples.CardSpace
{
    class Client
    {
        static void Main()
        {
            try
            {
                // Create a client with given client endpoint configuration
                SecureCalculatorClient client = new SecureCalculatorClient();

                Console.WriteLine("Press <ENTER> to invoke the service...");
                Console.ReadLine();

                // Display Identity.
                Console.WriteLine("Claims requested by the service = {0}", client.GetIdentity());

                // Call the Add service operation.
                double value1 = 100.00D;
                double value2 = 15.99D;
                double result = client.Add(value1, value2);
                Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result);

                // Call the Subtract service operation.
                value1 = 145.00D;
                value2 = 76.54D;
                result = client.Subtract(value1, value2);
                Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result);

                // Call the Multiply service operation.
                value1 = 9.00D;
                value2 = 81.25D;
                result = client.Multiply(value1, value2);
                Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result);

                // Call the Divide service operation.
                value1 = 22.00D;
                value2 = 7.00D;
                result = client.Divide(value1, value2);
                Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result);

                //Closing the client gracefully closes the connection and cleans up resources
                client.Close();
            }
            catch (UserCancellationException ex)
            {
                Console.WriteLine(ex.Message);
            }
            catch (UntrustedRecipientException ex)
            {
                Console.WriteLine(ex.Message);
            }
            catch (ServiceNotStartedException ex)
            {
                Console.WriteLine(ex.Message);
            }
            catch (TimeoutException ex)
            {
                Console.WriteLine(ex.Message);
            }
            catch (System.ServiceModel.Security.SecurityNegotiationException ex)
            {
                Console.WriteLine(ex.Message);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }


            Console.WriteLine();
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }

    }
}
