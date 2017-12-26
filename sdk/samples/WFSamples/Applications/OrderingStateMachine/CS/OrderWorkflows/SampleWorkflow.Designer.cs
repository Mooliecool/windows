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
using System.Workflow.ComponentModel;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.OrderApplication
{
    public sealed partial class SampleWorkflow 
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
            System.Workflow.ComponentModel.ActivityBind activitybind4 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding4 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind5 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding5 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind6 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding6 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            this.terminateWorkflow = new System.Workflow.ComponentModel.TerminateActivity();
            this.handleExternalEventActivity1 = new System.Workflow.Activities.HandleExternalEventActivity();
            this.setOrderCompletedState = new System.Workflow.Activities.SetStateActivity();
            this.handleOrderShipped = new System.Workflow.Activities.HandleExternalEventActivity();
            this.setOrderOpenState3 = new System.Workflow.Activities.SetStateActivity();
            this.handleOrderUpdated2 = new System.Workflow.Activities.HandleExternalEventActivity();
            this.setOrderProcessedState = new System.Workflow.Activities.SetStateActivity();
            this.handleOrderProcessed = new System.Workflow.Activities.HandleExternalEventActivity();
            this.setOrderOpenState2 = new System.Workflow.Activities.SetStateActivity();
            this.handleOrderUpdated = new System.Workflow.Activities.HandleExternalEventActivity();
            this.setOrderOpenState = new System.Workflow.Activities.SetStateActivity();
            this.handleOrderCreated = new System.Workflow.Activities.HandleExternalEventActivity();
            this.OnOrderCanceled = new System.Workflow.Activities.EventDrivenActivity();
            this.OnOrderShipped = new System.Workflow.Activities.EventDrivenActivity();
            this.OnOrderUpdated2 = new System.Workflow.Activities.EventDrivenActivity();
            this.OnOrderProcessed = new System.Workflow.Activities.EventDrivenActivity();
            this.OnOrderUpdated = new System.Workflow.Activities.EventDrivenActivity();
            this.OnOrderCreated = new System.Workflow.Activities.EventDrivenActivity();
            this.OrderProcessedState = new System.Workflow.Activities.StateActivity();
            this.OrderOpenState = new System.Workflow.Activities.StateActivity();
            this.OrderCompletedState = new System.Workflow.Activities.StateActivity();
            this.WaitingForOrderState = new System.Workflow.Activities.StateActivity();
            // 
            // terminateWorkflow
            // 
            this.terminateWorkflow.Name = "terminateWorkflow";
            // 
            // handleExternalEventActivity1
            // 
            this.handleExternalEventActivity1.EventName = "OrderCanceled";
            this.handleExternalEventActivity1.InterfaceType = typeof(Microsoft.Samples.Workflow.OrderApplication.IOrderService);
            this.handleExternalEventActivity1.Name = "handleExternalEventActivity1";
            activitybind1.Name = "SampleWorkflow";
            activitybind1.Path = "OrderSender";
            workflowparameterbinding1.ParameterName = "sender";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            this.handleExternalEventActivity1.ParameterBindings.Add(workflowparameterbinding1);
            // 
            // setOrderCompletedState
            // 
            this.setOrderCompletedState.Name = "setOrderCompletedState";
            this.setOrderCompletedState.TargetStateName = "OrderCompletedState";
            // 
            // handleOrderShipped
            // 
            this.handleOrderShipped.EventName = "OrderShipped";
            this.handleOrderShipped.InterfaceType = typeof(Microsoft.Samples.Workflow.OrderApplication.IOrderService);
            this.handleOrderShipped.Name = "handleOrderShipped";
            activitybind2.Name = "SampleWorkflow";
            activitybind2.Path = "OrderSender";
            workflowparameterbinding2.ParameterName = "sender";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            this.handleOrderShipped.ParameterBindings.Add(workflowparameterbinding2);
            // 
            // setOrderOpenState3
            // 
            this.setOrderOpenState3.Name = "setOrderOpenState3";
            this.setOrderOpenState3.TargetStateName = "OrderOpenState";
            // 
            // handleOrderUpdated2
            // 
            this.handleOrderUpdated2.EventName = "OrderUpdated";
            this.handleOrderUpdated2.InterfaceType = typeof(Microsoft.Samples.Workflow.OrderApplication.IOrderService);
            this.handleOrderUpdated2.Name = "handleOrderUpdated2";
            activitybind3.Name = "SampleWorkflow";
            activitybind3.Path = "OrderSender";
            workflowparameterbinding3.ParameterName = "sender";
            workflowparameterbinding3.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind3)));
            this.handleOrderUpdated2.ParameterBindings.Add(workflowparameterbinding3);
            // 
            // setOrderProcessedState
            // 
            this.setOrderProcessedState.Name = "setOrderProcessedState";
            this.setOrderProcessedState.TargetStateName = "OrderProcessedState";
            // 
            // handleOrderProcessed
            // 
            this.handleOrderProcessed.EventName = "OrderProcessed";
            this.handleOrderProcessed.InterfaceType = typeof(Microsoft.Samples.Workflow.OrderApplication.IOrderService);
            this.handleOrderProcessed.Name = "handleOrderProcessed";
            activitybind4.Name = "SampleWorkflow";
            activitybind4.Path = "OrderSender";
            workflowparameterbinding4.ParameterName = "sender";
            workflowparameterbinding4.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind4)));
            this.handleOrderProcessed.ParameterBindings.Add(workflowparameterbinding4);
            // 
            // setOrderOpenState2
            // 
            this.setOrderOpenState2.Name = "setOrderOpenState2";
            this.setOrderOpenState2.TargetStateName = "OrderOpenState";
            // 
            // handleOrderUpdated
            // 
            this.handleOrderUpdated.EventName = "OrderUpdated";
            this.handleOrderUpdated.InterfaceType = typeof(Microsoft.Samples.Workflow.OrderApplication.IOrderService);
            this.handleOrderUpdated.Name = "handleOrderUpdated";
            activitybind5.Name = "SampleWorkflow";
            activitybind5.Path = "OrderSender";
            workflowparameterbinding5.ParameterName = "sender";
            workflowparameterbinding5.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind5)));
            this.handleOrderUpdated.ParameterBindings.Add(workflowparameterbinding5);
            // 
            // setOrderOpenState
            // 
            this.setOrderOpenState.Name = "setOrderOpenState";
            this.setOrderOpenState.TargetStateName = "OrderOpenState";
            // 
            // handleOrderCreated
            // 
            this.handleOrderCreated.EventName = "OrderCreated";
            this.handleOrderCreated.InterfaceType = typeof(Microsoft.Samples.Workflow.OrderApplication.IOrderService);
            this.handleOrderCreated.Name = "handleOrderCreated";
            activitybind6.Name = "SampleWorkflow";
            activitybind6.Path = "OrderSender";
            workflowparameterbinding6.ParameterName = "sender";
            workflowparameterbinding6.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind6)));
            this.handleOrderCreated.ParameterBindings.Add(workflowparameterbinding6);
            // 
            // OnOrderCanceled
            // 
            this.OnOrderCanceled.Activities.Add(this.handleExternalEventActivity1);
            this.OnOrderCanceled.Activities.Add(this.terminateWorkflow);
            this.OnOrderCanceled.Name = "OnOrderCanceled";
            // 
            // OnOrderShipped
            // 
            this.OnOrderShipped.Activities.Add(this.handleOrderShipped);
            this.OnOrderShipped.Activities.Add(this.setOrderCompletedState);
            this.OnOrderShipped.Name = "OnOrderShipped";
            // 
            // OnOrderUpdated2
            // 
            this.OnOrderUpdated2.Activities.Add(this.handleOrderUpdated2);
            this.OnOrderUpdated2.Activities.Add(this.setOrderOpenState3);
            this.OnOrderUpdated2.Name = "OnOrderUpdated2";
            // 
            // OnOrderProcessed
            // 
            this.OnOrderProcessed.Activities.Add(this.handleOrderProcessed);
            this.OnOrderProcessed.Activities.Add(this.setOrderProcessedState);
            this.OnOrderProcessed.Name = "OnOrderProcessed";
            // 
            // OnOrderUpdated
            // 
            this.OnOrderUpdated.Activities.Add(this.handleOrderUpdated);
            this.OnOrderUpdated.Activities.Add(this.setOrderOpenState2);
            this.OnOrderUpdated.Name = "OnOrderUpdated";
            // 
            // OnOrderCreated
            // 
            this.OnOrderCreated.Activities.Add(this.handleOrderCreated);
            this.OnOrderCreated.Activities.Add(this.setOrderOpenState);
            this.OnOrderCreated.Name = "OnOrderCreated";
            // 
            // OrderProcessedState
            // 
            this.OrderProcessedState.Activities.Add(this.OnOrderUpdated2);
            this.OrderProcessedState.Activities.Add(this.OnOrderShipped);
            this.OrderProcessedState.Activities.Add(this.OnOrderCanceled);
            this.OrderProcessedState.Name = "OrderProcessedState";
            // 
            // OrderOpenState
            // 
            this.OrderOpenState.Activities.Add(this.OnOrderUpdated);
            this.OrderOpenState.Activities.Add(this.OnOrderProcessed);
            this.OrderOpenState.Name = "OrderOpenState";
            // 
            // OrderCompletedState
            // 
            this.OrderCompletedState.Name = "OrderCompletedState";
            // 
            // WaitingForOrderState
            // 
            this.WaitingForOrderState.Activities.Add(this.OnOrderCreated);
            this.WaitingForOrderState.Name = "WaitingForOrderState";
            // 
            // SampleWorkflow
            // 
            this.Activities.Add(this.WaitingForOrderState);
            this.Activities.Add(this.OrderCompletedState);
            this.Activities.Add(this.OrderOpenState);
            this.Activities.Add(this.OrderProcessedState);
            this.CompletedStateName = "OrderCompletedState";
            this.DynamicUpdateCondition = null;
            this.InitialStateName = "WaitingForOrderState";
            this.Name = "SampleWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private EventDrivenActivity OnOrderCanceled;
        private HandleExternalEventActivity handleExternalEventActivity1;
        private TerminateActivity terminateWorkflow;
        private HandleExternalEventActivity handleOrderCreated;
        private SetStateActivity setOrderOpenState;
        private HandleExternalEventActivity handleOrderUpdated;
        private SetStateActivity setOrderOpenState2;
        private HandleExternalEventActivity handleOrderUpdated2;
        private SetStateActivity setOrderOpenState3;
        private HandleExternalEventActivity handleOrderShipped;
        private SetStateActivity setOrderCompletedState;
        private HandleExternalEventActivity handleOrderProcessed;
        private SetStateActivity setOrderProcessedState;
        private EventDrivenActivity OnOrderCreated;
        private EventDrivenActivity OnOrderUpdated;
        private EventDrivenActivity OnOrderProcessed;
        private EventDrivenActivity OnOrderUpdated2;
        private EventDrivenActivity OnOrderShipped;
        private StateActivity OrderOpenState;
        private StateActivity OrderProcessedState;
        private StateActivity OrderCompletedState;
        private StateActivity WaitingForOrderState;
















































































    }
}
