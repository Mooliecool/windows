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

namespace Microsoft.Samples.Workflow.RaiseEventToLoadWorkflow
{
    public partial class DocumentApprovalWorkflow
    {
        [System.Diagnostics.DebuggerNonUserCode()]
        private void InitializeComponent()
        {
            this.CanModifyActivities = true;
            //
            // SendDocument method invoke
            //
            this.sendDocument = new CallExternalMethodActivity();
            this.sendDocument.Name = "sendDocument";
            this.sendDocument.InterfaceType = typeof(IDocumentApproval);
            this.sendDocument.MethodName = "RequestDocumentApproval";
            this.sendDocument.MethodInvoking += sendDocumentMethodInvoking;
            //
            // DocumentApproved event
            //
            this.documentApproved = new HandleExternalEventActivity();
            this.documentApproved.Name = "documentApproved";
            this.documentApproved.InterfaceType = typeof(IDocumentApproval);
            this.documentApproved.EventName = "DocumentApproved";
            this.documentApproved.Roles = null;
            this.documentApproved.Invoked += documentApprovedInvoked;

            //
            // workflow
            //
            this.Activities.Add(this.sendDocument);
            this.Activities.Add(this.documentApproved);
            this.Name = "DocumentApprovalWorkflow";
            this.CanModifyActivities = false;
        }
        private HandleExternalEventActivity documentApproved;
        private CallExternalMethodActivity sendDocument;
    }
}
