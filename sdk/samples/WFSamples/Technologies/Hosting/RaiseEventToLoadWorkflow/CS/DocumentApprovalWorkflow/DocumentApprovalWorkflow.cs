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
    public partial class DocumentApprovalWorkflow: SequentialWorkflowActivity
    {
        public DocumentApprovalWorkflow()
        {
            this.InitializeComponent();
        }

        void sendDocumentMethodInvoking(object sender, EventArgs e)
        {
            Console.WriteLine("Workflow: Request document approval\n");
        }

        void documentApprovedInvoked(object sender, EventArgs e)
        {
            Console.WriteLine("Workflow: Received document approval\n");
        }

       
    }
}
