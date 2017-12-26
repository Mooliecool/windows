//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.Net;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using System.ServiceModel;

namespace Microsoft.Samples.NetHttpBinding
{
    class Client
    {
        static void Main(string[] args)
        {
            // allow server certificate that doesn't match the host in the endpoint address
            ServicePointManager.ServerCertificateValidationCallback = new RemoteCertificateValidationCallback(IgnoreSubjectNameMismatch);

            string uri = "https://" + Environment.MachineName + ":8443/TestService/BinaryEncoderOverHTTPS";
            if (args.Length > 0)
            {
                uri = args[0];
            }
            EndpointAddress address = new EndpointAddress(uri);
            NetHttpBinding binding = new NetHttpBinding();
            ChannelFactory<IEchoService> factory = new ChannelFactory<IEchoService>(binding, address);
            factory.Open();
            IEchoService service = factory.CreateChannel();
            Console.WriteLine(service.Echo("'Test string passed for calling service using code'"));

            Console.WriteLine("called service successfully using binding in code");

            factory = new ChannelFactory<IEchoService>("EchoServer");
            factory.Open();
            service = factory.CreateChannel();

            Console.WriteLine(service.Echo("'Test string passed for calling service using code'"));
            Console.WriteLine("called service successfully using binding from config. Press enter to exit...");

            Console.ReadLine();
        }

        static bool IgnoreSubjectNameMismatch(object obj, X509Certificate certificate, X509Chain chain, SslPolicyErrors errors)
        {
            if (certificate == null)    
            {
                return false;
            }
            return (errors == SslPolicyErrors.RemoteCertificateNameMismatch || errors == SslPolicyErrors.None);
        }
    }
}
