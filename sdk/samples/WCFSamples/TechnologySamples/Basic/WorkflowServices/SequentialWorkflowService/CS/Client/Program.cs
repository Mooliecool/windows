//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;

namespace Microsoft.WorkflowServices.Samples
{
    class Program
    {
        static void Main(string[] args)
        {
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                // Add ChannelManager.
                ChannelManagerService channelmgr = new ChannelManagerService();
                workflowRuntime.AddService(channelmgr);
                
                AutoResetEvent waitHandle = new AutoResetEvent(false);
                workflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e) { Console.WriteLine("WorkflowCompleted: " + e.WorkflowInstance.InstanceId.ToString()); waitHandle.Set(); };
                workflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e) { Console.WriteLine("WorkflowTerminated: " + e.Exception.Message); waitHandle.Set(); };

                while (true)
                {
                    WorkflowInstance instance = workflowRuntime.CreateWorkflow(typeof(Microsoft.WorkflowServices.Samples.SequentialCalculatorClient));
                    Console.WriteLine("Start SequentialCalculatorClient.");
                    instance.Start();
                    waitHandle.WaitOne();
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.WriteLine("Do another calculation? (Y)");
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Press <enter> to exit.");
                    Console.ResetColor();
                    string input = Console.ReadLine();
                    if (input.Length == 0 || input[0] != 'Y')
                        break;
                    waitHandle.Reset();
                }
            }
        }
    }
}
