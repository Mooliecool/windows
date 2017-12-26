//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
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
	public partial class ClientWorkflow: StateMachineWorkflowActivity
	{
        Random generator = new Random();

        [NonSerialized] // input for IHostForwardContract.BeginWork
        public string ReturnUri = default(string);
        [NonSerialized] // input for IHostForwardContract.SubmitWorkItem
        public string SubmitWorkItemInput = default(string);
        [NonSerialized] // output for IForwardContract.BeginWorkflow
        public EndpointAddress10 ReturnAddress = default(EndpointAddress10);

        public int WorkItemCount = 0;
        public int WorkItemValue = 0;
        public string WorkItemLastPart = default(string);
        public WorkItem WorkItem = null;
        
        private void SetReturnAddress(object sender, EventArgs e)
        {
            EndpointAddress epr = ContextManager.CreateEndpointAddress(ReturnUri, this.ReceiveWorkItemComplete);
            ReturnAddress = EndpointAddress10.FromEndpointAddress(epr);
            Dictionary<string, string> context = epr.Headers[0].GetValue<Dictionary<string, string>>();

            StringBuilder contextString = new StringBuilder();

            foreach (KeyValuePair<string, string> pair in context)
            {
                contextString.Append(pair.Key);
                contextString.Append(":");
                contextString.Append(pair.Value);
            }
            
            DebugOutput("[ClientWorkflow:SetReturnAddress] " + contextString.ToString());
        }

        private void GenerateWorkItemCount(object sender, EventArgs e)
        {
            WorkItemCount = generator.Next(1, 8);
            WorkItemLastPart = SubmitWorkItemInput + "_Completed";
            DebugOutput("[ClientWorkflow:GenerateWorkItemCount] " + WorkItemCount.ToString());
        }

        private void GenerateNextWorkItemValue(object sender, EventArgs e)
        {
            WorkItemCount--;
            WorkItemValue = generator.Next(1000, 9999);
            DebugOutput("[ClientWorkflow:GenerateNextWorkItemValue] " + WorkItemCount.ToString() + ", " + WorkItemValue.ToString());
        }

        private void DebugOutput(string output)
        {
            Console.WriteLine(output);
        }
	}

}
