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

namespace Microsoft.Samples.Workflow.WebService
{
    public sealed partial class WebServicePublishWorkflow
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
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding1 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            System.Workflow.ComponentModel.ActivityBind activitybind2 = new System.Workflow.ComponentModel.ActivityBind();
            System.Workflow.ComponentModel.WorkflowParameterBinding workflowparameterbinding2 = new System.Workflow.ComponentModel.WorkflowParameterBinding();
            this.webServiceResponse1 = new System.Workflow.Activities.WebServiceOutputActivity();
            this.webServiceReceive1 = new System.Workflow.Activities.WebServiceInputActivity();
            // 
            // webServiceResponse1
            // 
            this.webServiceResponse1.InputActivityName = "webServiceReceive1";
            this.webServiceResponse1.Name = "webServiceResponse1";
            activitybind1.Name = "WebServicePublishWorkflow";
            activitybind1.Path = "POStatus";
            workflowparameterbinding1.ParameterName = "orderStatus";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            this.webServiceResponse1.ParameterBindings.Add(workflowparameterbinding1);
            // 
            // webServiceReceive1
            // 
            this.webServiceReceive1.InterfaceType = typeof(Microsoft.Samples.Workflow.WebService.IPurchaseOrder);
            this.webServiceReceive1.IsActivating = true;
            this.webServiceReceive1.MethodName = "CreateOrder";
            this.webServiceReceive1.Name = "webServiceReceive1";
            activitybind2.Name = "WebServicePublishWorkflow";
            activitybind2.Path = "PurchaseOrderId";
            workflowparameterbinding2.ParameterName = "id";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            this.webServiceReceive1.ParameterBindings.Add(workflowparameterbinding2);
            this.webServiceReceive1.InputReceived += new System.EventHandler(this.OnWebServiceInputReceived);
            // 
            // WebServicePublishWorkflow
            // 
            this.Activities.Add(this.webServiceReceive1);
            this.Activities.Add(this.webServiceResponse1);
            this.Name = "WebServicePublishWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private WebServiceInputActivity webServiceReceive1;
        private WebServiceOutputActivity webServiceResponse1;

    }
}
