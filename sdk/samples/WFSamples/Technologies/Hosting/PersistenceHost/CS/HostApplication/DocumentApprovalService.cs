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
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace Microsoft.Samples.Workflow.PersistenceHost
{
    // Local service
    class DocumentApprovalService : IDocumentApproval
    {
        private Mainform mainForm;

        public DocumentApprovalService(Mainform main)
        {
            this.mainForm = main;
        }
        // Method to raise a DocumentRequested event to host
        public void RequestDocumentApproval(Guid instanceId, String approver)
        {
            mainForm.DocumentRequested(instanceId, approver);
        }

        // Method to raise a DocumentRequested event to workflow
        public void ApproveDocument(Guid instanceId, String approver)
        {
            EventHandler<DocumentEventArgs> documentApproved = this.DocumentApproved;
            if (documentApproved != null)
                documentApproved(null, new DocumentEventArgs(instanceId, approver));
        }

        // Received document approval
        public event EventHandler<DocumentEventArgs> DocumentApproved;
    }
}
