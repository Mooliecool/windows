#region Using directives

using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;
using Microsoft.ServiceModel.Samples;

#endregion

namespace Microsoft.ServiceModel.Samples
{
    class Client
    {
        static void Main(string[] args)
        {

            EchoableClient client = new EchoableClient();
            
            try
            {
                string response = client.Echo("Hello");
                Console.WriteLine("Client: Got {0} in response", response);
            }
            catch (FaultException<WorkflowAborted> ex)
            {
                Console.WriteLine("Client: Got {0} as a fault", ex.Detail.Message);
            }

            //Closing the client gracefully closes the connection and cleans up resources
            client.Close();

            Console.WriteLine();
            Console.WriteLine("Press <ENTER> to terminate client.");
            Console.ReadLine();
        }
    }
}
