//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Threading;
using System.Workflow.Runtime;

namespace Microsoft.Rules.Samples
{
    class Program
    {

        static void Main(string[] args)
        {

            // Start the engine.
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                AutoResetEvent waitHandle = new AutoResetEvent(false);
                workflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e)
                {
                    Console.WriteLine("\nWorkflow Completed");
                    waitHandle.Set();
                };

                workflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                {
                    Console.WriteLine(e.Exception.Message);
                    waitHandle.Set();
                };

                string anotherOrder = "y";
                while (!anotherOrder.ToLower().StartsWith("n"))
                {
                    // Prompt the user for inputs.
                    Console.Write("\nPlease enter your name: ");
                    string customerName = Console.ReadLine();
                    Console.WriteLine("\nWhat would you like to purchase?");
                    Console.WriteLine("\t(1) Vista Ultimate DVD");
                    Console.WriteLine("\t(2) Vista Ultimate Upgrade DVD");
                    Console.WriteLine("\t(3) Vista Home Premium DVD");
                    Console.WriteLine("\t(4) Vista Home Premium Upgrade DVD");
                    Console.WriteLine("\t(5) Vista Home Basic DVD");
                    Console.WriteLine("\t(6) Vista Home Basic Upgrade DVD");
                    int itemNum = 0;

                    // Make sure an integeral value has been entered for the item number.
                    bool validItemNum = false;
                    while (validItemNum == false)
                    {
                        try
                        {
                            Console.Write("\nPlease enter an item number: ");
                            itemNum = Int32.Parse(Console.ReadLine());  // throw if the input is not an integer
                            validItemNum = true;
                        }
                        catch (FormatException)
                        {
                            Console.WriteLine(" => Please enter an integer for the item number!");
                        }
                    }

                    // Make sure an integeral value has been entered for the zip code.
                    string zipCode = "";
                    bool validZip = false;
                    while (validZip == false)
                    {
                        try
                        {
                            Console.Write("\nPlease enter your 5-Digit zip code: ");
                            zipCode = Console.ReadLine();
                            Int32.Parse(zipCode);  // throw if the input is not an integer
                            validZip = true;
                        }
                        catch (FormatException)
                        {
                            Console.WriteLine(" => Please enter an integer for the zip code!");
                        }
                    }

                    Console.WriteLine();

                    // Populate the dictionary with the information the user has just entered.
                    Dictionary<string, object> parameters = new Dictionary<string, object>();
                    parameters.Add("CustomerName", customerName);
                    parameters.Add("ItemNum", itemNum);
                    parameters.Add("ZipCode", zipCode);

                    // Load the workflow type and create th workflow.
                    WorkflowInstance instance = workflowRuntime.CreateWorkflow(typeof(Workflow), parameters);
                    waitHandle.Reset();
                    instance.Start();

                    waitHandle.WaitOne();

                    Console.Write("Another Order? (Y/N): ");
                    anotherOrder = Console.ReadLine();
                }
            }
        }
    }
}
