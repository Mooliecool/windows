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
using System.Workflow.ComponentModel;

namespace Microsoft.Samples.Workflow.RaiseEventToLoadWorkflow
{
    [Serializable]
    public class DocumentEventArgs : ExternalDataEventArgs
    {
        private Guid documentId;

        public DocumentEventArgs(Guid instanceId)
            : base(instanceId)
        {
            this.documentId = instanceId;
        }

        public Guid DocumentId
        {
            get { return this.documentId; }
            set { this.documentId = value; }
        }
    }

    [ExternalDataExchange]
    public interface IDocumentApproval
    {
        // Send document for approval
        void RequestDocumentApproval(Guid documentId, String approver);
        
        // Received document for approval
        event EventHandler<DocumentEventArgs> DocumentApproved;
    }
}
