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
    class CalculatorCallbackHandler : ICalculatorCallback
    {
        public void EchoFault(Message fault)
        {
            MessageFault messageFault = MessageFault.CreateFault(fault, int.MaxValue);
            Console.WriteLine("Client: Got '{0}' as a fault", messageFault.Reason);
        }

        public void Equals(string result)
        {
            Console.WriteLine("Client: Got {0} as result", result);
        }
    }

    class Client
    {
        static void Main(string[] args)
        {
            InstanceContext instance = new InstanceContext(new CalculatorCallbackHandler());

            CalculatorClient client = new CalculatorClient(instance);

            Console.WriteLine("Calling AddTo");
            client.AddTo(2);
            Console.WriteLine("Calling SubtractFrom");
            client.SubtractFrom(1);
            Console.WriteLine("Calling MultiplyBy");
            client.MultiplyBy(22);
            Console.WriteLine("Calling DivideBy");
            client.DivideBy(7);
            Console.WriteLine("Calling GetResult");
            client.GetResult();
            Console.ReadLine();

            //Closing the client gracefully closes the connection and cleans up resources
            client.Close();
        }
    }
}
