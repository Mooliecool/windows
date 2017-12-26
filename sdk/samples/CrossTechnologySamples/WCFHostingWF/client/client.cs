#region Using directives

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;
using System.ServiceModel;
using System.ServiceModel.Channels;

#endregion

namespace Microsoft.ServiceModel.Samples
{
    class EchoableCallbackHandler : IEchoableCallback
    {
        public void EchoResponse(string response)
        {
            Console.WriteLine("Client: Got {0} in response", response);
        }

        public void EchoFault(Message fault)
        {
            MessageFault messageFault = MessageFault.CreateFault(fault, int.MaxValue);
            Console.WriteLine("Client: Got '{0}' as a fault", messageFault.Reason);
        }
    }

    class Client
    {
        static void Main(string[] args)
        {
            InstanceContext instance = new InstanceContext(new EchoableCallbackHandler());

            EchoableClient client = new EchoableClient(instance);

            client.Echo("Hello");
            Console.ReadLine();

            //Closing the client gracefully closes the connection and cleans up resources
            client.Close();

        }
    }
}
