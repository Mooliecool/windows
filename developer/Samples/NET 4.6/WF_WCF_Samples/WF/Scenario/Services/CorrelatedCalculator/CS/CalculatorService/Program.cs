//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel.Activities;
using System.ServiceModel.Description;

namespace Microsoft.Samples.CorrelatedCalculator.CalculatorService
{
    class Program
    {
        static void Main(string[] args)
        {            
            string baseAddress = "http://localhost:8080/CalculatorService";
            
            using (WorkflowServiceHost host = new WorkflowServiceHost(new Calculator(), new Uri(baseAddress)))
            {
                host.Description.Behaviors.Add(new ServiceMetadataBehavior() { HttpGetEnabled = true });
                
                // no application endpoints are defined explicitly
                // the Config Simplification feature adds a default endpoint with BasicHttpBinding at baseAddress + Receive.ServiceContractName.LocalName
                
                host.Open();
                Console.WriteLine("CalculatorService waiting at: {0}", baseAddress);
                Console.WriteLine("Press [ENTER] to exit");
                Console.ReadLine();
                host.Close();
            }
        }
    }
}
