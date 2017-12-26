//  Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Collections;
using System.Drawing;
using System.Reflection;
using System.Workflow.ComponentModel.Compiler;
using System.Workflow.ComponentModel.Serialization;
using System.Workflow.ComponentModel;
using System.Workflow.ComponentModel.Design;
using System.Workflow.Runtime;
using System.Workflow.Activities;
using System.Workflow.Activities.Rules;

namespace Microsoft.WorkflowServices.Samples
{
	partial class ShipperWorkflow
	{
		#region Designer generated code
		
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
        [System.Diagnostics.DebuggerNonUserCode]
		private void InitializeComponent()
		{
            this.CanModifyActivities = true;
            System.Workflow.Activities.ChannelToken endpoint1 = new System.Workflow.Activities.ChannelToken();
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding1 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo1 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.ComponentModel.ActivityBind activitybind2 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding2 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind3 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding3 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind4 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding4 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo2 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.WorkflowServiceAttributes workflowserviceattributes1 = new System.Workflow.Activities.WorkflowServiceAttributes();
            this.DoAcceptQuoteRequest = new System.Workflow.Activities.CodeActivity();
            this.SendShippingQuote = new System.Workflow.Activities.SendActivity();
            this.Delay = new System.Workflow.Activities.DelayActivity();
            this.ReceiveRequestShippingQuote = new System.Workflow.Activities.ReceiveActivity();
            // 
            // DoAcceptQuoteRequest
            // 
            this.DoAcceptQuoteRequest.Name = "DoAcceptQuoteRequest";
            this.DoAcceptQuoteRequest.ExecuteCode += new System.EventHandler(this.AcceptQuoteRequest);
            // 
            // SendShippingQuote
            // 
            endpoint1.EndpointName = "SupplierEndpoint";
            endpoint1.Name = "SupplierEndpoint";
            this.SendShippingQuote.ChannelToken = endpoint1;
            this.SendShippingQuote.Name = "SendShippingQuote";
            activitybind1.Name = "ShipperWorkflow";
            activitybind1.Path = "quote";
            workflowparameterbinding1.ParameterName = "quote";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            this.SendShippingQuote.ParameterBindings.Add(workflowparameterbinding1);
            typedoperationinfo1.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingQuote);
            typedoperationinfo1.Name = "ShippingQuote";
            this.SendShippingQuote.ServiceOperationInfo = typedoperationinfo1;
            this.SendShippingQuote.BeforeSend += new System.EventHandler<System.Workflow.Activities.SendActivityEventArgs>(this.PrepareQuote);
            // 
            // Delay
            // 
            this.Delay.Name = "Delay";
            this.Delay.TimeoutDuration = System.TimeSpan.Parse("00:00:04");
            // 
            // ReceiveRequestShippingQuote
            // 
            this.ReceiveRequestShippingQuote.Activities.Add(this.DoAcceptQuoteRequest);
            this.ReceiveRequestShippingQuote.CanCreateInstance = true;
            this.ReceiveRequestShippingQuote.Name = "ReceiveRequestShippingQuote";
            activitybind2.Name = "ShipperWorkflow";
            activitybind2.Path = "supplierAck";
            workflowparameterbinding2.ParameterName = "(ReturnValue)";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            activitybind3.Name = "ShipperWorkflow";
            activitybind3.Path = "order";
            workflowparameterbinding3.ParameterName = "po";
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind3)));
            activitybind4.Name = "ShipperWorkflow";
            activitybind4.Path = "supplierContext";
            workflowparameterbinding4.ParameterName = "context";
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind4)));
            this.ReceiveRequestShippingQuote.ParameterBindings.Add(workflowparameterbinding2);
            this.ReceiveRequestShippingQuote.ParameterBindings.Add(workflowparameterbinding3);
            this.ReceiveRequestShippingQuote.ParameterBindings.Add(workflowparameterbinding4);
            typedoperationinfo2.ContractType = typeof(Microsoft.WorkflowServices.Samples.IShippingRequest);
            typedoperationinfo2.Name = "RequestShippingQuote";
            this.ReceiveRequestShippingQuote.ServiceOperationInfo = typedoperationinfo2;
            // 
            // ShipperWorkflow
            // 
            this.Activities.Add(this.ReceiveRequestShippingQuote);
            this.Activities.Add(this.Delay);
            this.Activities.Add(this.SendShippingQuote);
            this.Name = "ShipperWorkflow";
            this.SetValue(System.Workflow.Activities.ReceiveActivity.WorkflowServiceAttributesProperty, workflowserviceattributes1);
            this.CanModifyActivities = false;

		}

		#endregion

        private DelayActivity Delay;
        private CodeActivity DoAcceptQuoteRequest;
        private SendActivity SendShippingQuote;
        private ReceiveActivity ReceiveRequestShippingQuote;








    }
}
