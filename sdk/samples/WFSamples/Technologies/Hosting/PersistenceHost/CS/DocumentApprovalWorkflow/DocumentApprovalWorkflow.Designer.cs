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

namespace Microsoft.Samples.Workflow.PersistenceHost
{
    partial class DocumentApprovalWorkflow
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
            this.DocumentApproved = new System.Workflow.Activities.HandleExternalEventActivity();
            this.SendDocument = new System.Workflow.Activities.CallExternalMethodActivity();
            // 
            // DocumentApproved
            // 
            this.DocumentApproved.EventName = "DocumentApproved";
            this.DocumentApproved.InterfaceType = typeof(Microsoft.Samples.Workflow.PersistenceHost.IDocumentApproval);
            this.DocumentApproved.Name = "DocumentApproved";
            // 
            // SendDocument
            // 
            this.SendDocument.InterfaceType = typeof(Microsoft.Samples.Workflow.PersistenceHost.IDocumentApproval);
            this.SendDocument.MethodName = "RequestDocumentApproval";
            this.SendDocument.Name = "SendDocument";
            activitybind1.Name = "DocumentApprovalWorkflow";
            activitybind1.Path = "InstanceId";
            workflowparameterbinding1.ParameterName = "documentId";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            activitybind2.Name = "DocumentApprovalWorkflow";
            activitybind2.Path = "Approver";
            workflowparameterbinding2.ParameterName = "approver";
            workflowparameterbinding2.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind2)));
            this.SendDocument.ParameterBindings.Add(workflowparameterbinding1);
            this.SendDocument.ParameterBindings.Add(workflowparameterbinding2);
            // 
            // DocumentApprovalWorkflow
            // 
            this.Activities.Add(this.SendDocument);
            this.Activities.Add(this.DocumentApproved);
            this.Name = "DocumentApprovalWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private HandleExternalEventActivity DocumentApproved;
        private CallExternalMethodActivity SendDocument;






    }
}
