
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Channels;
using System.ServiceModel.Description;
using System.ServiceModel.Dispatcher;
using System.Security.Cryptography;


namespace Microsoft.ServiceModel.Samples
{
    public static class CustomHeader
    {
        public static readonly String HeaderName = "InstanceId";
        public static readonly String HeaderNamespace = "http://Microsoft.ServiceModel.Samples/Sharing";
    }

    //The service contract is defined in generatedProxy.cs, generated from the service by the svcutil tool.

    //Client implementation code.
    class Client
    {
        static RNGCryptoServiceProvider randomNumberGenerator = new RNGCryptoServiceProvider();

        static string NewInstanceId()
        {
            byte[] random = new byte[256 / 8];
            randomNumberGenerator.GetBytes(random);
            return Convert.ToBase64String(random);
        }

        static void Main()
        {
            // Create a client to default endpoint configuration
            CalculatorInstanceClient client1 = new CalculatorInstanceClient();

            //Create a new 1028 bit strong InstanceContextId that we want the server to associate 
            //the InstanceContext that will process all message's from this client.
            String uniqueId = NewInstanceId();

            MessageHeader Client1InstanceContextHeader = MessageHeader.CreateHeader(
                CustomHeader.HeaderName,
                CustomHeader.HeaderNamespace,
                uniqueId);

            try
            {
                using (new OperationContextScope(client1.InnerChannel))
                {
                    //Add the header as a header to the scope so it gets sent for each message.
                    OperationContext.Current.OutgoingMessageHeaders.Add(Client1InstanceContextHeader);
                    DoCalculations(client1);
                }

                // Create a second client
                CalculatorInstanceClient client2 = new CalculatorInstanceClient();

                //We want this to communicate with InstanceContext created by Proxy1

                using (new OperationContextScope(client2.InnerChannel))
                {
                    //Add the same header that client1 used so we will connect to that same InstanceContext
                    try
                    {
                        OperationContext.Current.OutgoingMessageHeaders.Add(Client1InstanceContextHeader);
                        DoCalculations(client2);

                        //Closing the client gracefully closes the connection and cleans up resources
                        client2.Close();
                    }
                    catch (CommunicationException)
                    {
                        client2.Abort();
                    }
                    catch (TimeoutException)
                    {
                        client2.Abort();
                    }
                    catch (Exception)
                    {
                        client2.Abort();
                        throw;
                    }
                }

                client1.Close();
            }
            catch (CommunicationException)
            {
                client1.Abort();
            }
            catch (TimeoutException)
            {
                client1.Abort();
            }
            catch (Exception)
            {
                client1.Abort();
                throw;
            }

            Console.WriteLine();
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }

        static void DoCalculations(CalculatorInstanceClient client)
        {
            // Call the Add service operation.
            double value1 = 100.00D;
            double value2 = 15.99D;
            double result = client.Add(value1, value2);
            Console.WriteLine("Add({0},{1}) = {2}", value1, value2, result);
            Console.Write("InstanceId: {0}", client.GetInstanceId());
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount());

            // Call the Subtract service operation.
            value1 = 145.00D;
            value2 = 76.54D;
            result = client.Subtract(value1, value2);
            Console.WriteLine("Subtract({0},{1}) = {2}", value1, value2, result);
            Console.Write("InstanceId: {0}", client.GetInstanceId());
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount());

            // Call the Multiply service operation.
            value1 = 9.00D;
            value2 = 81.25D;
            result = client.Multiply(value1, value2);
            Console.WriteLine("Multiply({0},{1}) = {2}", value1, value2, result);
            Console.Write("InstanceId: {0}", client.GetInstanceId());
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount());

            // Call the Divide service operation.
            value1 = 22.00D;
            value2 = 7.00D;
            result = client.Divide(value1, value2);
            Console.WriteLine("Divide({0},{1}) = {2}", value1, value2, result);
            Console.Write("InstanceId: {0}", client.GetInstanceId());
            Console.WriteLine(" , OperationCount: {0}", client.GetOperationCount());
        }

    }
}
