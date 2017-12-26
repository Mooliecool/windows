using System;
using System.Collections.Generic;
using System.Net;
using System.ServiceModel;
using System.Text;

using Microsoft.Samples.Channels;

namespace Microsoft.ServiceModel.Samples.Service
{
    class Service
    {
        static readonly UriBuilder uriBuilder = new UriBuilder("http://" + Environment.MachineName + ":8000/TestService");
        static void Main(string[] args)
        {
            NetHttpBinding httpsBinding = new NetHttpBinding();
            NetHttpBinding httpBinding = new NetHttpBinding(NetHttpSecurityMode.TransportCredentialOnly);
            ServiceHost serviceHost = new ServiceHost(typeof(EchoService), GetBaseAddress("http", 8000), GetBaseAddress("https", 8443));
            serviceHost.AddServiceEndpoint(typeof(IEchoService), httpBinding, "BinaryEncoderOverHTTP");
            serviceHost.AddServiceEndpoint(typeof(IEchoService), httpsBinding, "BinaryEncoderOverHTTPS");
            serviceHost.Open();

            Console.WriteLine("Service started at: " + serviceHost.ChannelDispatchers[0].Listener.Uri.AbsoluteUri);
            Console.WriteLine("Service started at: " + serviceHost.ChannelDispatchers[1].Listener.Uri.AbsoluteUri);
            Console.WriteLine("Press enter to exit...");
            Console.ReadLine();
        }
        static Uri GetBaseAddress(string scheme, int port)
        {
            uriBuilder.Scheme = scheme;
            uriBuilder.Port = port;
            return uriBuilder.Uri;
        }
    }
    class EchoService : IEchoService
    {
        #region IEchoService Members

        public string Echo(string message)
        {
            Console.WriteLine("Echo called with: " + message);
            
            Console.WriteLine("The client is hitting endpoint: {0}", OperationContext.Current.IncomingMessageProperties.Via.AbsoluteUri);
            ServiceSecurityContext securityContext = OperationContext.Current.ServiceSecurityContext;
            Console.WriteLine("The client is: {0}", securityContext.IsAnonymous ? "anonymous" : securityContext.PrimaryIdentity.Name);
            return message;
        }

        #endregion
    }
}
