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
using System.ComponentModel;
using System.Workflow.Activities;


namespace Microsoft.Samples.Workflow.Listen
{
    public sealed partial class PurchaseOrderWorkflow
    {
        #region Designer generated code
        
        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            this.Timeout = new System.Workflow.Activities.CodeActivity();
            this.Delay = new System.Workflow.Activities.DelayActivity();
            this.RejectPO = new System.Workflow.Activities.HandleExternalEventActivity();
            this.ApprovePO = new System.Workflow.Activities.HandleExternalEventActivity();
            this.OnTimeoutEventDriven = new System.Workflow.Activities.EventDrivenActivity();
            this.OnOrderRejectedEventDriven = new System.Workflow.Activities.EventDrivenActivity();
            this.OnOrderApprovedEventDriven = new System.Workflow.Activities.EventDrivenActivity();
            this.POStatusListen = new System.Workflow.Activities.ListenActivity();
            this.CreatePO = new System.Workflow.Activities.CallExternalMethodActivity();
            // 
            // Timeout
            // 
            this.Timeout.Name = "Timeout";
            this.Timeout.ExecuteCode += new System.EventHandler(this.OnTimeout);
            // 
            // Delay
            // 
            this.Delay.Name = "Delay";
            this.Delay.TimeoutDuration = System.TimeSpan.Parse("00:00:05");
            // 
            // RejectPO
            // 
            this.RejectPO.EventName = "OrderRejected";
            this.RejectPO.InterfaceType = typeof(Microsoft.Samples.Workflow.Listen.IOrderService);
            this.RejectPO.Name = "RejectPO";
            this.RejectPO.Invoked += new System.EventHandler<System.Workflow.Activities.ExternalDataEventArgs>(this.OnRejectPO);
            // 
            // ApprovePO
            // 
            this.ApprovePO.EventName = "OrderApproved";
            this.ApprovePO.InterfaceType = typeof(Microsoft.Samples.Workflow.Listen.IOrderService);
            this.ApprovePO.Name = "ApprovePO";
            this.ApprovePO.Invoked += new System.EventHandler<System.Workflow.Activities.ExternalDataEventArgs>(this.OnApprovePO);
            // 
            // OnTimeoutEventDriven
            // 
            this.OnTimeoutEventDriven.Activities.Add(this.Delay);
            this.OnTimeoutEventDriven.Activities.Add(this.Timeout);
            this.OnTimeoutEventDriven.Name = "OnTimeoutEventDriven";
            // 
            // OnOrderRejectedEventDriven
            // 
            this.OnOrderRejectedEventDriven.Activities.Add(this.RejectPO);
            this.OnOrderRejectedEventDriven.Name = "OnOrderRejectedEventDriven";
            // 
            // OnOrderApprovedEventDriven
            // 
            this.OnOrderApprovedEventDriven.Activities.Add(this.ApprovePO);
            this.OnOrderApprovedEventDriven.Name = "OnOrderApprovedEventDriven";
            // 
            // POStatusListen
            // 
            this.POStatusListen.Activities.Add(this.OnOrderApprovedEventDriven);
            this.POStatusListen.Activities.Add(this.OnOrderRejectedEventDriven);
            this.POStatusListen.Activities.Add(this.OnTimeoutEventDriven);
            this.POStatusListen.Name = "POStatusListen";
            // 
            // CreatePO
            // 
            this.CreatePO.InterfaceType = typeof(Microsoft.Samples.Workflow.Listen.IOrderService);
            this.CreatePO.MethodName = "CreateOrder";
            this.CreatePO.Name = "CreatePO";
            this.CreatePO.MethodInvoking += new System.EventHandler(this.OnBeforeCreateOrder);
            // 
            // PurchaseOrderWorkflow
            // 
            this.Activities.Add(this.CreatePO);
            this.Activities.Add(this.POStatusListen);
            this.Name = "PurchaseOrderWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private HandleExternalEventActivity ApprovePO;
        private ListenActivity POStatusListen;
        private EventDrivenActivity OnOrderApprovedEventDriven;
        private EventDrivenActivity OnOrderRejectedEventDriven;
        private HandleExternalEventActivity RejectPO;
        private EventDrivenActivity OnTimeoutEventDriven;
        private DelayActivity Delay;
        private CodeActivity Timeout;
        private CallExternalMethodActivity CreatePO;







    }
}
