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
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.ActiveDirectoryRoles
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
            System.Workflow.ComponentModel.ActivityBind activitybind1 = new System.Workflow.ComponentModel.ActivityBind();
            this.InitiatePO = new System.Workflow.Activities.HandleExternalEventActivity();
            this.SetupRoles = new System.Workflow.Activities.CodeActivity();
            activitybind1.Name = "PurchaseOrderWorkflow";
            activitybind1.Path = "POInitiators";
            // 
            // InitiatePO
            // 
            this.InitiatePO.EventName = "InitiatePurchaseOrder";
            this.InitiatePO.InterfaceType = typeof(Microsoft.Samples.Workflow.ActiveDirectoryRoles.IStartPurchaseOrder);
            this.InitiatePO.Name = "InitiatePO";
            this.InitiatePO.Invoked += new System.EventHandler<System.Workflow.Activities.ExternalDataEventArgs>(this.OnPOInitiated);
            this.InitiatePO.SetBinding(System.Workflow.Activities.HandleExternalEventActivity.RolesProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            // 
            // SetupRoles
            // 
            this.SetupRoles.Name = "SetupRoles";
            this.SetupRoles.ExecuteCode += new System.EventHandler(this.OnSetupRoles);
            // 
            // PurchaseOrderWorkflow
            // 
            this.Activities.Add(this.SetupRoles);
            this.Activities.Add(this.InitiatePO);
            this.Name = "PurchaseOrderWorkflow";
            this.CanModifyActivities = false;

        }

        #endregion

        private HandleExternalEventActivity InitiatePO;
        private CodeActivity SetupRoles;

    }
}
