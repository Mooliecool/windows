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
using System.Threading;
using System.Workflow.Runtime;
using System.Workflow.Activities.Rules;
using System.Workflow.ComponentModel;
using System.CodeDom;   

namespace Microsoft.Samples.Workflow.ChangingRules
{
    class Program
    {
        static AutoResetEvent waitHandle = new AutoResetEvent(false);
        static bool wasChanged = false;

        static void Main()
        {
            using (WorkflowRuntime workflowRuntime = new WorkflowRuntime())
            {
                try
                {
                    Int32 orderAmount = 14000;
                    Console.WriteLine("Order amount = {0:c}, approved amount = {1:c}", orderAmount, 10000);

                    // Start the engine.
                    workflowRuntime.StartRuntime();

                    // Load the workflow type.
                    Type type = typeof(Microsoft.Samples.Workflow.ChangingRules.DynamicRulesWorkflow);

                    workflowRuntime.WorkflowCompleted += OnWorkflowCompleted;
                    workflowRuntime.WorkflowIdled += OnWorkflowIdle;
                    workflowRuntime.WorkflowTerminated += OnWorkflowTerminated;
                    workflowRuntime.ServicesExceptionNotHandled += OnExceptionNotHandled;

                    // The "OrderValueParameter" parameter is used to determine which branch of the IfElse should be executed
                    // a value less than 10,000 will execute branch 1 - Get Manager Approval; any other value will execute branch 2 - Get VP Approval
                    Dictionary<string, object> parameters = new Dictionary<string, object>();
                    parameters.Add("Amount", orderAmount);
                    WorkflowInstance workflowInstance = workflowRuntime.CreateWorkflow(type, parameters);
                    workflowInstance.Start();

                    waitHandle.WaitOne();
                }
                catch (Exception e)
                {
                    Console.WriteLine("Exception \n\tSource: {0} \n\tMessage: {1}", e.Source, e.Message);
                }
                finally
                {
                    workflowRuntime.StopRuntime();
                    Console.WriteLine("Workflow runtime stopped, program exiting...");
                }
            }
        }

        static void OnExceptionNotHandled(object sender, ServicesExceptionNotHandledEventArgs e)
        {
            Console.WriteLine("Unhandled Workflow Exception ");
            Console.WriteLine("  Type: " + e.GetType().ToString());
            Console.WriteLine("  Message: " + e.Exception.Message);
        }

        static void OnWorkflowTerminated(object sender, WorkflowTerminatedEventArgs e)
        {
            Console.WriteLine(e.Exception.Message);
            waitHandle.Set();
        }

        static void OnWorkflowIdle(object sender, WorkflowEventArgs e)
        {
            if (wasChanged)
                return;

            wasChanged = true;

            WorkflowInstance workflowInstance = e.WorkflowInstance;

            Int32 newAmount = 15000;

            Console.WriteLine("Dynamically change approved amount to {0:c}",  newAmount);

            // Dynamic update of order rule
            WorkflowChanges workflowchanges = new WorkflowChanges(workflowInstance.GetWorkflowDefinition());
            
            CompositeActivity transient = workflowchanges.TransientWorkflow;
            RuleDefinitions ruleDefinitions = (RuleDefinitions)transient.GetValue(RuleDefinitions.RuleDefinitionsProperty);
            RuleConditionCollection conditions = ruleDefinitions.Conditions;
            RuleExpressionCondition condition1 = (RuleExpressionCondition)conditions["Check"];
            (condition1.Expression as CodeBinaryOperatorExpression).Right = new CodePrimitiveExpression(newAmount);

            workflowInstance.ApplyWorkflowChanges(workflowchanges);
        }

        static void OnWorkflowCompleted(object sender, WorkflowCompletedEventArgs instance)
        {
            Console.WriteLine("Workflow completed.");
            waitHandle.Set();
        }
    }
}
