//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections.Generic;
using System.Drawing;
using System.ServiceModel;
using System.Text;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.WorkflowServices.Samples
{
	public partial class ServiceWorkflow: StateMachineWorkflowActivity
	{
        public const string ReverseEndpoint = "ReverseEndpoint";
        [NonSerialized] // input of IForwardContract.BeginWorkflow
        public EndpointAddress10 ReturnAddress = default(EndpointAddress10);
        [NonSerialized] // input of IForwardContract.BeginWorkItem
        public string BeginWorkItemInput = default(System.String);
        [NonSerialized] // input of IForwardContract.ContinueWorkItem
        public int ContinueWorkItemInput = default(System.Int32);
        [NonSerialized] // input of IFowardContract.CompleteWorkItem
        public string CompleteWorkItemInput = default(System.String);

        public WorkItem CurrentWorkItem = null;
        public List<WorkItem> CompletedWorkItems = new List<WorkItem>(3);

        private void ApplyReturnAddress(object sender, EventArgs e)
        {   // apply ReturnAddress to ReverseEndpoint
            EndpointAddress epr = ReturnAddress.ToEndpointAddress();
            ContextManager.ApplyEndpointAddress(this.SendWorkItemComplete, epr);
            Dictionary<string, string> context = epr.Headers[0].GetValue<Dictionary<string,string>>();

            StringBuilder contextString = new StringBuilder();

            foreach(KeyValuePair<string, string> pair in context)
            {
                contextString.Append(pair.Key);
                contextString.Append(":");
                contextString.Append(pair.Value);
            }
            DebugOutput("[ServiceWorkflow:ApplyReturnAddress] " + contextString.ToString());
            
        }

        private void CreateWorkItem(object sender, EventArgs e)
        {   // create a new CurrentWorkItem
            CurrentWorkItem = new WorkItem();
            CurrentWorkItem.FirstPart = BeginWorkItemInput;
            DebugOutput("[ServiceWorkflow:CreateWorkItem] " + BeginWorkItemInput);
        }

        private void AddPartsToWorkItem(object sender, EventArgs e)
        {   // add parts to CurrentWorkItem
            if (CurrentWorkItem.PartsList == null)
                CurrentWorkItem.PartsList = new List<int>();
            CurrentWorkItem.PartsList.Add(ContinueWorkItemInput);
            DebugOutput("[ServiceWorkflow:AddPartsToWorkItem] " + ContinueWorkItemInput);
        }

        private void AddCompletedWorkItem(object sender, EventArgs e)
        {   // complete CurrentWorkItem
            CurrentWorkItem.LastPart = CompleteWorkItemInput;
            CompletedWorkItems.Add(CurrentWorkItem);
            DebugOutput("[ServiceWorkflow:AddCompletedWorkItem] " + CompleteWorkItemInput);
        }

        private void RemoveCompletedWorkItem(object sender, EventArgs e)
        {   // remove completed WorkItem
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine("WorkItemComplete: " + CompletedWorkItems[0].FirstPart);
            Console.ResetColor();
            CompletedWorkItems.RemoveAt(0);
            DebugOutput("[ServiceWorkflow:RemoveCompletedWorkItem] " + CompletedWorkItems.Count.ToString());
        }

        private void DebugOutput(string output)
        {
            Console.WriteLine(output);
        }

    }

}
