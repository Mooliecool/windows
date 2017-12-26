/****************************** Module Header ******************************\
* Module Name:	Program.cs
* Project:		CSAzureServiceBusWCFDS
* Copyright (c) Microsoft Corporation.
* 
* This Console Application hosts WCF Data Services and expose it via Service Bus
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

namespace NorthwindDataService
{
    using System;
    using System.Globalization;
    using System.ServiceModel;
    using Microsoft.ServiceBus;
    using System.ServiceModel.Web;
    using Microsoft.ServiceBus.Description;
    using System.ServiceModel.Dispatcher;
    using System.ServiceModel.Description;
    using System.ServiceModel.Channels;
    using System.IO;
    using System.Xml;
    using System.Text;
  
    internal sealed class Program
    {
        private static void Main()
        {
            Console.Write("Your Service Namespace Domain (ex. https://<DOMAIN>.servicebus.windows.net/): ");
            string serviceNamespaceDomain = Console.ReadLine();

            // By setting EndToEndWebHttpSecurityMode.Transport we use HTTPS.
            // If you want to use HTTP please set EndToEndWebHttpSecurityMode.None.
            // In this sample we need to authenticate client via Access Control Service so
            // RelayClientAuthenticationType.RelayAccessToken is set. You can set RelayClientAuthenticationType.None
            // If you don't want to authenticate client via Access Control Service.
            WebHttpRelayBinding binding = new WebHttpRelayBinding(EndToEndWebHttpSecurityMode.Transport, RelayClientAuthenticationType.RelayAccessToken);
            // Replace above code with the following one to test in browser
            // WebHttpRelayBinding binding = new WebHttpRelayBinding(EndToEndWebHttpSecurityMode.Transport, RelayClientAuthenticationType.None);

            // Initialize ServiceHost using custom binding
            Uri address = ServiceBusEnvironment.CreateServiceUri("https", serviceNamespaceDomain, "DataService");
            WebServiceHost host = new WebServiceHost(typeof(NorthwindDataService), address);
            host.AddServiceEndpoint("System.Data.Services.IRequestHandler", binding, address);
            var eb=new TransportClientEndpointBehavior(){ CredentialType=TransportClientCredentialType.SharedSecret};
            eb.Credentials.SharedSecret.IssuerName="owner";
            eb.Credentials.SharedSecret.IssuerSecret = "[Your Secret]";
            host.Description.Endpoints[0].Behaviors.Add(eb);

            // The following behavior is used to work around exception caused by PUT/POST 
            // requests when exposing via Service Bus
            MyBehavior mb = new MyBehavior();
            host.Description.Endpoints[0].Behaviors.Add(mb);

            // Start service
            host.Open();
            Console.WriteLine("Test the following URI in browser: ");
            Console.WriteLine(address + "Customers");
            Console.WriteLine("Use the following URI if you want to generate client proxy for this service");
            Console.WriteLine(address);
            Console.WriteLine();
            Console.WriteLine("Press [Enter] to exit");
            Console.ReadLine();

            host.Close();
        }

        class MyInspector : IDispatchMessageInspector 
        {

            #region IDispatchMessageInspector Members

            public object AfterReceiveRequest(ref System.ServiceModel.Channels.Message request, IClientChannel channel, InstanceContext instanceContext)
            {
                // Workaround for Service Bus scenario for PUT&POST
                MessageBuffer buffer = request.CreateBufferedCopy(int.MaxValue);
                Message copy = buffer.CreateMessage();
                MemoryStream ms = new MemoryStream();
                Encoding encoding = Encoding.UTF8;
                XmlWriterSettings writerSettings = new XmlWriterSettings { Encoding = encoding };
                XmlDictionaryWriter writer = XmlDictionaryWriter.CreateDictionaryWriter(XmlWriter.Create(ms));
                copy.WriteBodyContents(writer);
                writer.Flush();
                string messageBodyString = encoding.GetString(ms.ToArray());
                messageBodyString = @"<?xml version=""1.0"" encoding=""utf-8""?><Binary>" + 
                    Convert.ToBase64String(Encoding.UTF8.GetBytes(messageBodyString)) + "</Binary>";
                ms = new MemoryStream(encoding.GetBytes(messageBodyString));
                XmlReader bodyReader = XmlReader.Create(ms);
                Message originalMessage = request;
                request = Message.CreateMessage(originalMessage.Version, null, bodyReader);
                request.Headers.CopyHeadersFrom(originalMessage);
             
                if (!request.Properties.ContainsKey(WebBodyFormatMessageProperty.Name))
                {
                    request.Properties.Add(WebBodyFormatMessageProperty.Name, new WebBodyFormatMessageProperty(WebContentFormat.Raw));
                }

                return null;
                
            }

            public void BeforeSendReply(ref System.ServiceModel.Channels.Message reply, object correlationState)
            {
                
            }

            #endregion
        }
        class MyBehavior : IEndpointBehavior 
        {

            #region IEndpointBehavior Members

            public void AddBindingParameters(ServiceEndpoint endpoint, System.ServiceModel.Channels.BindingParameterCollection bindingParameters)
            {
               
            }

            public void ApplyClientBehavior(ServiceEndpoint endpoint, ClientRuntime clientRuntime)
            {
               
            }

            public void ApplyDispatchBehavior(ServiceEndpoint endpoint, EndpointDispatcher endpointDispatcher)
            {
                endpointDispatcher.DispatchRuntime.MessageInspectors.Add(new MyInspector());
            }

            public void Validate(ServiceEndpoint endpoint)
            {
               
            }

            #endregion
        }
    }
}