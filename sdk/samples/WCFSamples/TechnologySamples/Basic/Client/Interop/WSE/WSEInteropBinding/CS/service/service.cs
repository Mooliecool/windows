    
//  Copyright (c) Microsoft Corporation.  All Rights Reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Security.Principal;
using System.Security.Cryptography.X509Certificates;
using System.Configuration;
using System.Runtime.Serialization;
using System.Xml.Serialization;

using System.ServiceModel;
using System.ServiceModel.Security;
using System.ServiceModel.Dispatcher;
using System.ServiceModel.Description;
using System.ServiceModel.Channels;

namespace Microsoft.ServiceModel.Samples
{
    [ServiceContract(Namespace = "http://Microsoft.ServiceModel.Samples")]
    interface ICalculator
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
    // Added code to write output to the console window
    [ServiceBehavior(IncludeExceptionDetailInFaults = true)]
    class CalculatorService : ICalculator
    {

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

        static void Main(string[] args)
        {
            
            //Create a ServiceHost for the CalculatorService type
            using (ServiceHost serviceHost = new ServiceHost(typeof(CalculatorService)))
            {
                //Create and configure the WseHttpBinding
                WseHttpBinding binding = new WseHttpBinding();
                binding.SecurityAssertion = WseSecurityAssertion.AnonymousForCertificate;
                binding.EstablishSecurityContext = true;
                binding.RequireDerivedKeys = true;
                binding.MessageProtectionOrder = MessageProtectionOrder.SignBeforeEncrypt;

                serviceHost.AddServiceEndpoint(typeof(ICalculator), binding, "");
                ServiceCredentials creds = new ServiceCredentials();
                creds.ServiceCertificate.SetCertificate(StoreLocation.LocalMachine,
                                                        StoreName.My,
                                                        X509FindType.FindBySubjectName,
                                                        "localhost");
                serviceHost.Description.Behaviors.Add(creds);
                serviceHost.Open();

                // The service can now be accessed.
                Console.WriteLine("Press enter to terminate the service");
                // Uncomment the following to see the account the service is running under
                //Console.WriteLine("The service is running in the following account: {0}", WindowsIdentity.GetCurrent().Name);

                for (int i = 0; i < serviceHost.ChannelDispatchers.Count; ++i)
                {
                    ChannelDispatcher channelDispatcher = serviceHost.ChannelDispatchers[i] as ChannelDispatcher;
                    if (channelDispatcher != null)
                    {
                        for (int j=0; j<channelDispatcher.Endpoints.Count; ++j)
                        {
                            EndpointDispatcher endpointDispatcher = channelDispatcher.Endpoints[j];
                            Console.WriteLine("The service is listening on {0}", endpointDispatcher.EndpointAddress.Uri);
                        }
                    }
                }

                Console.WriteLine("The service is ready");
                Console.ReadLine();
            }
        }
    } 
}


