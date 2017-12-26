//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.WorkflowServices.Samples
{
	public partial class SequentialCalculatorClient : SequentialWorkflowActivity
	{
        Random random = new Random();
        public int outputValue = default(int);
        public int returnValue = default(int);

        private void OnBeforeSend(object sender, SendActivityEventArgs e)
        {
            outputValue = random.Next(1, 100);
            Console.WriteLine(e.SendActivity.Name + "(" + outputValue.ToString() + ")");
        }

        private void OnAfterResponse(object sender, SendActivityEventArgs e)
        {
            Console.WriteLine("[" + returnValue.ToString() + "]");
        }
    }
}
