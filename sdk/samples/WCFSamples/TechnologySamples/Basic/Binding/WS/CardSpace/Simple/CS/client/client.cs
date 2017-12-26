
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.IdentityModel.Selectors;
using System.ServiceModel.Security;

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
                Console.WriteLine("Identity - (Private Personal ID) = {0}", client.GetIdentity());

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
                Console.WriteLine("1:" + ex.Message);
            }
            catch (UntrustedRecipientException ex)
            {
                Console.WriteLine("2:" + ex.Message);
            }
            catch (ServiceNotStartedException ex)
            {
                Console.WriteLine("3:" + ex.Message);
            }
            catch (TimeoutException ex)
            {
                Console.WriteLine("4:" + ex.Message);
            }
            catch (SecurityNegotiationException ex)
            {
                Console.WriteLine("5:" + ex.Message);
            }
            catch (IdentityValidationException ex)
            {
                Console.WriteLine("7:" + ex.Message);
                Console.WriteLine(ex.StackTrace);
            }
            catch (Exception ex)
            {
                Console.WriteLine("6:" + ex.GetType().Name + ex.Message);
            }


            Console.WriteLine();
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }

    }
}
