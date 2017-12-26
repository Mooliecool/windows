using System;
using System.Collections.Generic;
using System.Text;
using System.ServiceModel;

namespace Microsoft.ServiceModel.Samples
{
    class Host
    {
        static void Main(string[] args)
        {
            using (ServiceHost host = new ServiceHost(typeof(CalculatorService)))
            {
                // Create a new instance of the WFServiceHostExtension for managing the WorkflowRuntime
                WFServiceHostExtension wfServiceExtension = new WFServiceHostExtension();

                // Add the Extension to the collection from the Service Host
                host.Extensions.Add(wfServiceExtension);

                host.Open();

                Console.WriteLine("The service is ready.");
                Console.WriteLine("Press <ENTER> to terminate service.");
                Console.ReadLine();
            }
        }
    }
}
