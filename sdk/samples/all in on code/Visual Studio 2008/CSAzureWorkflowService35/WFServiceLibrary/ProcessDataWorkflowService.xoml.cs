/****************************** Module Header ******************************\
* Module Name:	ProcessDataWorkflowService.xoml.cs
* Project:		WFServiceLibrary
* Copyright (c) Microsoft Corporation.
* 
* This is the code behind of the workflow.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Linq;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace WFServiceLibrary
{
	public partial class ProcessDataWorkflowService : SequentialWorkflowActivity
	{
		// DependencyProperties.
		public static DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(int), typeof(ProcessDataWorkflowService));

		public int Value
		{
			get { return (int)this.GetValue(ProcessDataWorkflowService.ValueProperty); }
			set { this.SetValue(ProcessDataWorkflowService.ValueProperty, value); }
		}

		public static DependencyProperty ReturnValueProperty = DependencyProperty.Register("ReturnValue", typeof(string), typeof(ProcessDataWorkflowService));

		public string ReturnValue
		{
			get { return (string)this.GetValue(ProcessDataWorkflowService.ReturnValueProperty); }
			set { this.SetValue(ProcessDataWorkflowService.ReturnValueProperty, value); }
		}

		// Assign the return values.
		private void AssignSmallValue_ExecuteCode(object sender, EventArgs e)
		{
			this.ReturnValue = "You've entered a small value.";
		}

		private void AssignLargeValue_ExecuteCode(object sender, EventArgs e)
		{
			this.ReturnValue = "You've entered a large value.";
		}
	}
}
