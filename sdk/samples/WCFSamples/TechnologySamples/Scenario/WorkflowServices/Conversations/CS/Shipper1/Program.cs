//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Workflow.Runtime;

namespace Microsoft.WorkflowServices.Samples
{
    class Program
    {
        static void Main(string[] args)
        {
            WorkflowServiceHost host = new WorkflowServiceHost(typeof(ShipperWorkflow));
            host.Description.Behaviors.Find<WorkflowRuntimeBehavior>().WorkflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e) { Console.WriteLine("WorkflowTerminated: " + e.Exception.Message); };
            host.Description.Behaviors.Find<WorkflowRuntimeBehavior>().WorkflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e) { Console.WriteLine("WorkflowCompleted."); };
            host.Open();

            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine("Role: Shipper");
            Console.WriteLine("Press <enter> to exit");
            Console.ResetColor();
            Console.ReadLine();
            host.Close();
        }
    }
}
