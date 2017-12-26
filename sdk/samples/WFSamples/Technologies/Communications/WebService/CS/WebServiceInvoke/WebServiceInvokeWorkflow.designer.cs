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

namespace Microsoft.Samples.Workflow.Communication.WebService
{
    public sealed partial class WebServiceInvokeWorkflow
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
            this.code1 = new System.Workflow.Activities.CodeActivity();
            this.invokeWebService1 = new System.Workflow.Activities.InvokeWebServiceActivity();
            // 
            // code1
            // 
            this.code1.Name = "code1";
            this.code1.ExecuteCode += new System.EventHandler(this.OnExecuteCode);
            // 
            // invokeWebService1
            // 
            this.invokeWebService1.MethodName = "CreateOrder";
            this.invokeWebService1.Name = "invokeWebService1";
            activitybind1.Name = "WebServiceInvokeWorkflow";
            activitybind1.Path = "POStatus";
            workflowparameterbinding1.ParameterName = "(ReturnValue)";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            activitybind2.Name = "WebServiceInvokeWorkflow";
            activitybind2.Path = "PurchaseOrderId";
            workflowparameterbinding2.ParameterName = "id";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            this.invokeWebService1.ParameterBindings.Add(workflowparameterbinding1);
            this.invokeWebService1.ParameterBindings.Add(workflowparameterbinding2);
            this.invokeWebService1.ProxyClass = typeof(Microsoft.Samples.Workflow.WebService.localhost.WebServicePublishWorkflow_WebService);
            this.invokeWebService1.Invoking += new System.EventHandler<System.Workflow.Activities.InvokeWebServiceEventArgs>(this.OnWebServiceInvoking);
            // 
            // WebServiceInvokeWorkflow
            // 
            this.Activities.Add(this.invokeWebService1);
            this.Activities.Add(this.code1);
            this.Name = "WebServiceInvokeWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private CodeActivity code1;
        private InvokeWebServiceActivity invokeWebService1;

    }
}
