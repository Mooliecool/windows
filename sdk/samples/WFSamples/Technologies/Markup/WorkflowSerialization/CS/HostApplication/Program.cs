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


namespace Microsoft.Samples.Workflow.WorkflowSerialization
{
    class Program
    {
        static void Main(string[] args)
        {
            const string workflowFilename = "workflow.xoml";

            //
            // Create and configure workflow runtime
            //
            using(WorkflowRuntime workflowRuntime = new WorkflowRuntime())
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
                // Create workflow programmatically
                //
                Console.WriteLine("Creating workflow.");
                SequentialWorkflowActivity workflow = new SequentialWorkflowActivity();
                workflow.Name = "Programmatically created workflow";
                workflow.Description = "Programmatically created workflow for XAML activation";
                ConsoleActivity activity = new ConsoleActivity();
                activity.Name = "ConsoleActivity1";
                activity.StringToWrite = "Sample String";
                workflow.Activities.Add(activity);
                
                //
                // Serialize workflow to XAML file
                //
                Console.WriteLine("Serializing workflow to file.");
                WorkflowMarkupSerializer serializer = new WorkflowMarkupSerializer();
                using (XmlWriter writer = XmlWriter.Create(workflowFilename))
                {
                    DesignerSerializationManager serializationManager = new DesignerSerializationManager();
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
                // Deserialize workflow from file
                //
                WorkflowInstance deserializedWorkflow = null;
                Console.WriteLine("Deserializing workflow from file.");
                try
                {

                    using (XmlReader reader = XmlReader.Create(workflowFilename))
                    {
                        deserializedWorkflow = workflowRuntime.CreateWorkflow(reader);
                    }
                }
                catch (WorkflowValidationFailedException exp)
                {
                    ValidationErrorCollection list = exp.Errors;
                    foreach (ValidationError err in list)
                    {
                        Console.WriteLine(err.ErrorText);
                    }
                    return;
                }


                //
                // Start workflow
                //
                Console.WriteLine("Starting workflow.");
                deserializedWorkflow.Start();
           
                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
        }
    }
}
