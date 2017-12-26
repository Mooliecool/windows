//----------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
//----------------------------------------------------------------

using System;
using System.ServiceModel;
using System.ServiceModel.Activities;

namespace Microsoft.Samples.WF.ManagementEndpoint
{
    class Program
    {
        static void Main()
        {
            Console.WriteLine("Client starting...");

            IWorkflowCreation creationClient = new ChannelFactory<IWorkflowCreation>(new BasicHttpBinding(), "http://localhost/DataflowControl.xaml/Creation").CreateChannel();

            // Start a new instance of the workflow
            Guid instanceId = creationClient.CreateSuspended(null);
            WorkflowControlClient controlClient = new WorkflowControlClient(
                new BasicHttpBinding(), 
                new EndpointAddress(new Uri("http://localhost/DataflowControl.xaml")));
            controlClient.Unsuspend(instanceId);
            Console.WriteLine("Client exiting...");
        }
    }
}
