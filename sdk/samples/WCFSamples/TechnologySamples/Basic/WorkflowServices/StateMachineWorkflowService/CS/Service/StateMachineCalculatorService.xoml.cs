//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.WorkflowServices.Samples
{
	public partial class StateMachineCalculatorService: StateMachineWorkflowActivity
	{
        [NonSerialized]
        public int inputValue = default(int);
        public int currentValue = default(int);
        
        private void Add(object sender, EventArgs e)
        {
            currentValue += inputValue;
        }

        private void Subtract(object sender, EventArgs e)
        {
            currentValue -= inputValue;
        }

        private void Multiply(object sender, EventArgs e)
        {
            currentValue *= inputValue;
        }

        private void Divide(object sender, EventArgs e)
        {
            currentValue /= inputValue;
        }
	}

}
