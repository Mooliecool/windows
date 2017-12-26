//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Security.Cryptography.X509Certificates;
using System.ServiceModel;
using System.ServiceModel.Security;
using System.ServiceModel.Channels;

namespace Microsoft.ServiceModel.Samples
{
    // The service contract is defined in generatedClient.cs, generated from the service by
    // the svcutil tool.

    class Program
    {
        static void Main(string[] args)
        {
            CallWcfService();
            // If the WSE 3.0 SDK is installed, uncomment this code, compile the WSSecurityAnonymous 
            // WSE 3.0 Quickstart sample and use the configured WseHttpBinding to call the service.
            // Passing "true" indicates that a WSE 3.0 Policy file will be read
            // to configure the WseHttpBinding. e.g. CallWseService(true);
            //
            //CallWseService(true);
        }

        static void CallWcfService()
        {
            EndpointAddress address = new EndpointAddress(new Uri("http://localhost:8000/servicemodelsamples/service"),
                                                          EndpointIdentity.CreateDnsIdentity("localhost"));

            WseHttpBinding binding = new WseHttpBinding();
            binding.SecurityAssertion = WseSecurityAssertion.AnonymousForCertificate;
            binding.EstablishSecurityContext = true;
            binding.RequireDerivedKeys = true;
            binding.MessageProtectionOrder = MessageProtectionOrder.SignBeforeEncrypt;

            // Use the calculator client in generatedClient.cs
            CalculatorClient client = new CalculatorClient(binding, address);
            client.ClientCredentials.ServiceCertificate.SetDefaultCertificate(
                                                                 StoreLocation.CurrentUser,
                                                                 StoreName.TrustedPeople,
                                                                 X509FindType.FindBySubjectName,
                                                                 "localhost");
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

            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }

        static void CallWseService(bool usePolicyFile)
        {
            EndpointAddress address = new EndpointAddress(new Uri("http://localhost/WSSecurityAnonymousPolicy/WSSecurityAnonymousService.asmx"),
                                                          EndpointIdentity.CreateDnsIdentity("WSE2QuickStartServer"));

            WseHttpBinding binding = new WseHttpBinding();
            if (!usePolicyFile)
            {
                binding.SecurityAssertion = WseSecurityAssertion.AnonymousForCertificate;
                binding.EstablishSecurityContext = false;
                binding.RequireDerivedKeys = true;
                binding.MessageProtectionOrder = MessageProtectionOrder.SignBeforeEncrypt;
            }
            else
                binding.LoadPolicy("wse3policyCache.config", "ServerPolicy");

            // Use the WSSecurityAnonymousServiceSoapClient client in wseclient.cs
            WSSecurityAnonymousServiceSoapClient client = new WSSecurityAnonymousServiceSoapClient(binding, address);

            // Need to supply the credentials depending on the type of WseSecurityAssertion used.
            // Anonymous only requires server certificate. UsernameForCertificate would also require
            // a username and password to be supplied.
            client.ClientCredentials.ServiceCertificate.SetDefaultCertificate(
                                                                 StoreLocation.LocalMachine,
                                                                 StoreName.My,
                                                                 X509FindType.FindBySubjectDistinguishedName,
                                                                 "CN=WSE2QuickStartServer");
            string[] symbols = new string[] { "FABRIKAM", "CONTOSO" };
            StockQuote[] quotes = client.StockQuoteRequest(symbols);

            client.Close();

            // Success!
            foreach (StockQuote quote in quotes)
            {
                Console.WriteLine("");
                Console.WriteLine("Symbol: " + quote.Symbol);
                Console.WriteLine("\tName:\t\t\t" + quote.Name);
                Console.WriteLine("\tLast Price:\t\t" + quote.Last);
                Console.WriteLine("\tPrevious Change:\t" + quote.PreviousChange + "%");
            }

            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }
    }
}
