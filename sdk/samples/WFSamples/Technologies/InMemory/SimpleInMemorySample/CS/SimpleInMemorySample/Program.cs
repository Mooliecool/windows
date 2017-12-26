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
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.Runtime;
using System.Workflow.Runtime.Hosting;

namespace Microsoft.Samples.Workflow.SimpleInMemorySample
{
    class Program
    {
        static void Main(string[] args)
        {
            using(WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                AutoResetEvent waitHandle = new AutoResetEvent(false);

                WorkflowCompiler compiler = new WorkflowCompiler();
                WorkflowCompilerParameters compilerParameters = new WorkflowCompilerParameters();
                compilerParameters.GenerateInMemory = true;

                String[] workflowFilenames = GetWorkflowFilenames();
                
                WorkflowCompilerResults results = compiler.Compile(compilerParameters, workflowFilenames);
                if (results.Errors.Count > 0)
                {
                    Console.WriteLine("Errors occurred while building the workflow:");
                    foreach (WorkflowCompilerError compilerError in results.Errors)
                    {
                        Console.WriteLine(compilerError.Line.ToString() + "," + compilerError.Column.ToString() + " : " + compilerError.ErrorText);
                    }

                    return;
                }

                Type workflowType = results.CompiledAssembly.GetType("Microsoft.Samples.Workflow.SimpleInMemorySample.SequentialWorkflow");

                workflowRuntime.WorkflowCompleted += delegate(object sender, WorkflowCompletedEventArgs e) 
                {
                    string orderStatus = e.OutputParameters["Status"].ToString();
                    Console.WriteLine("Order was " + orderStatus);
                    waitHandle.Set();
                };
                workflowRuntime.WorkflowTerminated += delegate(object sender, WorkflowTerminatedEventArgs e)
                {
                    Console.WriteLine(e.Exception.Message);
                    waitHandle.Set();
                };

                Dictionary<string, object> parameters = new Dictionary<string, object>();
                parameters.Add("Amount", 300);

                WorkflowInstance instance = workflowRuntime.CreateWorkflow(workflowType, parameters);
                instance.Start();

                waitHandle.WaitOne();

                workflowRuntime.StopRuntime();
            }
            
        }
        private static String[] GetWorkflowFilenames()
        {
            String workflowFilename = @"\SequentialWorkflow.cs";
            String workflowDesignerFilename = @"\SequentialWorkflow.designer.cs";
            String applicationPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().GetName().CodeBase);
            String codeDirectory = Directory.GetParent(Directory.GetParent(applicationPath.Substring(6)).ToString()).ToString();
            String workflowFullPath = codeDirectory + workflowFilename;
            String workflowDesignerFullPath = codeDirectory + workflowDesignerFilename;
            return new String[] { workflowFullPath, workflowDesignerFullPath };
        }
    }
}
