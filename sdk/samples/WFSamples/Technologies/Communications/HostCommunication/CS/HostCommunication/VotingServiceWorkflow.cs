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

namespace Microsoft.Samples.Workflow.Communication.HostCommunication
{
    public sealed partial class VotingServiceWorkflow : SequentialWorkflowActivity
    {
        private string alias;

        public VotingServiceWorkflow()
        {
            InitializeComponent();
        }

        private void OnRejected(object sender, ExternalDataEventArgs e)
        {
            Console.WriteLine("Proposal Rejected by {0}.", this.alias);
        }

        private void OnApproved(object sender, ExternalDataEventArgs e)
        {
            Console.WriteLine("Proposal Approved by {0}.", this.alias);
        }

        public string Alias
        {
            get
            {
                return this.alias;
            }
            set
            {
                this.alias = value;
            }
        }
    }
}
