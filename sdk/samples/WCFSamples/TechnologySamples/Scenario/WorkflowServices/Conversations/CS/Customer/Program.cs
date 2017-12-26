//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ServiceModel;
using System.ServiceModel.Description;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;

namespace Microsoft.WorkflowServices.Samples
{
    class Program
    {
        static void Main(string[] args)
        {
            WorkflowServiceHost host = new WorkflowServiceHost(typeof(CustomerWorkflow));
            
            host.Description.Behaviors.Find<WorkflowRuntimeBehavior>().WorkflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e) { Console.WriteLine("WorkflowTerminated: " + e.Exception.Message); };
            host.Description.Behaviors.Find<WorkflowRuntimeBehavior>().WorkflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e) { Console.WriteLine("WorkflowCompleted."); };
            host.Open();

            Console.WriteLine("Role: Customer");
            Console.WriteLine("Press <enter> to submit order.");
            Console.ReadLine();

            WorkflowInstance workflow = host.Description.Behaviors.Find<WorkflowRuntimeBehavior>().WorkflowRuntime.CreateWorkflow(typeof(CustomerWorkflow));
            workflow.Start();

            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Press <enter> to exit");
            Console.ResetColor();
            Console.ReadLine();
            host.Close();
        }
    }
}
