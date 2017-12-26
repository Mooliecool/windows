//---------------------------------------------------------------------
//  This file is part of the Windows Workflow Foundation SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//---------------------------------------------------------------------

using System;
using System.ComponentModel.Design.Serialization;
using System.Threading;
using System.Workflow.Activities;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.Runtime;
using System.Xml;

namespace Microsoft.Samples.Workflow.CustomSerialization
{
    class Program
    {
        const string workflowFilename = "workflow.xaml";

        static void Main(string[] args)
        {

            Validator validator = (Validator)Activator.CreateInstance(typeof(Validator));
            WorkflowMarkupSerializer serializer = new WorkflowMarkupSerializer();
            DesignerSerializationManager serializationManager = new DesignerSerializationManager();

            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                AutoResetEvent waitHandle = new AutoResetEvent(false);
                workflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e)
                {
                    Console.WriteLine("Workflow completed.");
                    waitHandle.Set();
                };
                workflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                {
                    Console.WriteLine(e.Exception.Message);
                    waitHandle.Set();
                };

                //
                // Create queue workflow programmatically
                //
                Console.WriteLine("Creating workflow.");
                SequentialWorkflowActivity workflow = new SequentialWorkflowActivity();
                workflow.Name = "Programmatically created workflow";
                workflow.Description = "Programmatically created workflow for XAML activation";

                // Add Queue activity
                QueueActivity queueActivity = new QueueActivity();
                queueActivity.Name = "QueueActivity1";

                // Enqueue data
                queueActivity.NameQueue.Enqueue("Queue item 1");
                queueActivity.NameQueue.Enqueue("Queue item 2");
                queueActivity.NameQueue.Enqueue("Queue item 3");
                workflow.Activities.Add(queueActivity);

                // Add Stack activity
                StackActivity stackActivity = new StackActivity();
                stackActivity.Name = "StackActivity1";

                // Push data
                stackActivity.NameStack.Push("Stack item 1");
                stackActivity.NameStack.Push("Stack item 2");
                stackActivity.NameStack.Push("Stack item 3");
                workflow.Activities.Add(stackActivity);

                //
                // Serialize workflow to XAML file
                //
                Console.WriteLine("Serializing workflow to file.");
                using (XmlWriter writer = XmlWriter.Create(workflowFilename))
                {
                    using (serializationManager.CreateSession())
                    {
                        serializer.Serialize(serializationManager, writer, workflow);
                        if (serializationManager.Errors.Count > 0)
                        {
                            Console.WriteLine(String.Format("There were {0} errors during serialization", serializationManager.Errors.Count));
                            return;
                        }
                    }
                }

                //
                // Create workflow instance from file
                //
                Console.WriteLine("Deserializing workflow from file.");
                WorkflowInstance workflowInstance;
                using (XmlReader reader = XmlReader.Create(workflowFilename))
                {
                    try
                    {
                        workflowInstance = workflowRuntime.CreateWorkflow(reader);
                    }
                    catch (WorkflowValidationFailedException e)
                    {
                        Console.WriteLine("Validation errors found.  Exiting.");
                        foreach (ValidationError validationError in e.Errors)
                        {
                            Console.WriteLine(validationError.ErrorText);
                        }
                        return;
                    }
                }

                //
                // Execute workflow
                //
                Console.WriteLine("Starting workflow.");
                workflowInstance.Start();
                waitHandle.WaitOne();
                workflowRuntime.StopRuntime();
            }
        }
    }
}
