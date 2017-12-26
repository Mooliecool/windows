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
    partial class CustomerWorkflow
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
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding1 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind2 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding2 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind3 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding3 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo1 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.ChannelToken endpoint1 = new System.Workflow.Activities.ChannelToken();
            System.Workflow.ComponentModel.ActivityBind activitybind4 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding4 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind5 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding5 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind6 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding6 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.Activities.TypedOperationInfo typedoperationinfo2 = new System.Workflow.Activities.TypedOperationInfo();
            System.Workflow.Activities.WorkflowServiceAttributes workflowserviceattributes1 = new System.Workflow.Activities.WorkflowServiceAttributes();
            this.DoReviewOrder = new System.Workflow.Activities.CodeActivity();
            this.ReceiveOrderDetails = new System.Workflow.Activities.ReceiveActivity();
            this.DoDisplayStatus = new System.Workflow.Activities.CodeActivity();
            this.SendSubmitOrder = new System.Workflow.Activities.SendActivity();
            // 
            // DoReviewOrder
            // 
            this.DoReviewOrder.Name = "DoReviewOrder";
            this.DoReviewOrder.ExecuteCode += new System.EventHandler(this.ReviewOrder);
            // 
            // ReceiveOrderDetails
            // 
            this.ReceiveOrderDetails.Activities.Add(this.DoReviewOrder);
            this.ReceiveOrderDetails.Name = "ReceiveOrderDetails";
            activitybind1.Name = "CustomerWorkflow";
            activitybind1.Path = "orderDetails";
            workflowparameterbinding1.ParameterName = "po";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            activitybind2.Name = "CustomerWorkflow";
            activitybind2.Path = "shippingQuote";
            workflowparameterbinding2.ParameterName = "quote";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            activitybind3.Name = "CustomerWorkflow";
            activitybind3.Path = "customerAck";
            workflowparameterbinding3.ParameterName = "(ReturnValue)";
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind3)));
            this.ReceiveOrderDetails.ParameterBindings.Add(workflowparameterbinding1);
            this.ReceiveOrderDetails.ParameterBindings.Add(workflowparameterbinding2);
            this.ReceiveOrderDetails.ParameterBindings.Add(workflowparameterbinding3);
            typedoperationinfo1.ContractType = typeof(Microsoft.WorkflowServices.Samples.IOrderDetails);
            typedoperationinfo1.Name = "OrderDetails";
            this.ReceiveOrderDetails.ServiceOperationInfo = typedoperationinfo1;
            // 
            // DoDisplayStatus
            // 
            this.DoDisplayStatus.Name = "DoDisplayStatus";
            this.DoDisplayStatus.ExecuteCode += new System.EventHandler(this.DisplayStatus);
            // 
            // SendSubmitOrder
            // 
            endpoint1.EndpointName = "SupplierEndPoint";
            endpoint1.Name = "SupplierEndPoint";
            endpoint1.OwnerActivityName = "CustomerWorkflow";
            this.SendSubmitOrder.ChannelToken = endpoint1;
            this.SendSubmitOrder.Name = "SendSubmitOrder";
            activitybind4.Name = "CustomerWorkflow";
            activitybind4.Path = "order";
            workflowparameterbinding4.ParameterName = "po";
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind4)));
            activitybind5.Name = "CustomerWorkflow";
            activitybind5.Path = "contextToSend";
            workflowparameterbinding5.ParameterName = "context";
            workflowparameterbinding5.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind5)));
            activitybind6.Name = "CustomerWorkflow";
            activitybind6.Path = "supplierAck";
            workflowparameterbinding6.ParameterName = "(ReturnValue)";
            workflowparameterbinding6.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind6)));
            this.SendSubmitOrder.ParameterBindings.Add(workflowparameterbinding4);
            this.SendSubmitOrder.ParameterBindings.Add(workflowparameterbinding5);
            this.SendSubmitOrder.ParameterBindings.Add(workflowparameterbinding6);
            typedoperationinfo2.ContractType = typeof(Microsoft.WorkflowServices.Samples.IOrder);
            typedoperationinfo2.Name = "SubmitOrder";
            this.SendSubmitOrder.ServiceOperationInfo = typedoperationinfo2;
            this.SendSubmitOrder.BeforeSend += new System.EventHandler<System.Workflow.Activities.SendActivityEventArgs>(this.PrepareOrder);
            // 
            // CustomerWorkflow
            // 
            this.Activities.Add(this.SendSubmitOrder);
            this.Activities.Add(this.DoDisplayStatus);
            this.Activities.Add(this.ReceiveOrderDetails);
            this.Name = "CustomerWorkflow";
            this.SetValue(System.Workflow.Activities.ReceiveActivity.WorkflowServiceAttributesProperty, workflowserviceattributes1);
            this.CanModifyActivities = false;

		}

		#endregion

        private CodeActivity DoDisplayStatus;
        private CodeActivity DoReviewOrder;
        private ReceiveActivity ReceiveOrderDetails;
        private SendActivity SendSubmitOrder;











    }
}
