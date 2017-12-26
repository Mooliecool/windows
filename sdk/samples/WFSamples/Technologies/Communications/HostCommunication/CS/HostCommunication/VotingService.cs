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
using System.Threading;
using System.Windows.Forms;
using System.Workflow.Runtime;
using System.Workflow.Activities;

namespace Microsoft.Samples.Workflow.Communication.HostCommunication
{
    // Class defines the message passed between the local service and the workflow
    [Serializable]
    internal class VotingServiceEventArgs : ExternalDataEventArgs
    {
        private string aliasValue;

        public VotingServiceEventArgs(Guid instanceID, string alias)
            : base(instanceID)
        {
            this.aliasValue = alias;
        }

        public string Alias
        {
            get 
            {
                return this.aliasValue;
            }
        }
    }

    // Workflow communication interface which defines the contract
    // between a local service and a workflow
    [ExternalDataExchange]
    internal interface IVotingService
    {
        event EventHandler<VotingServiceEventArgs> ApprovedProposal;
        event EventHandler<VotingServiceEventArgs> RejectedProposal;

        void CreateBallot(string alias);
    }

    // Local service that implements the contract on the host side
    // i.e. it implements the methods and calls the events, which are
    // implemented by the workflow
    internal class VotingServiceImpl : IVotingService
    {
        public event EventHandler<VotingServiceEventArgs> ApprovedProposal;
        public event EventHandler<VotingServiceEventArgs> RejectedProposal;

        // Called by the workflow to create a new ballot, this method
        // creates a new thread which shows a voting dialog to the user
        public void CreateBallot(string alias)
        {
            Console.WriteLine("Ballot created for {0}.", alias);
            ShowVotingDialog(new VotingServiceEventArgs(WorkflowEnvironment.WorkflowInstanceId, alias));
        }

        public void ShowVotingDialog(VotingServiceEventArgs votingEventArgs)
        {
            DialogResult result;
            string alias = votingEventArgs.Alias;

            // Show the voting dialog to the user and depending on the response
            // raise the ApproveProposal or RejectProposal event back to the workflow
            result = MessageBox.Show(string.Format("Approve Proposal, {0}?", alias), string.Format("{0} Ballot", alias), MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                EventHandler<VotingServiceEventArgs> approvedProposal = this.ApprovedProposal;
                if (approvedProposal != null)
                    approvedProposal(null, votingEventArgs);
            }
            else
            {
                EventHandler<VotingServiceEventArgs> rejectedProposal = this.RejectedProposal;
                if (rejectedProposal != null)
                    rejectedProposal(null, votingEventArgs);
            }
        }
    }
}