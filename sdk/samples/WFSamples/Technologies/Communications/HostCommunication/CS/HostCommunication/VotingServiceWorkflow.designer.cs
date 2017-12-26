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
    public sealed partial class VotingServiceWorkflow
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
            this.createBallotCallExternalMethodActivity = new System.Workflow.Activities.CallExternalMethodActivity();
            this.waitForResponseListenActivity = new System.Workflow.Activities.ListenActivity();
            this.waitForApprovalEventDrivenActivity = new System.Workflow.Activities.EventDrivenActivity();
            this.waitForRejectionEventDrivenActivity = new System.Workflow.Activities.EventDrivenActivity();
            this.approvedHandleExternalEventActivity = new System.Workflow.Activities.HandleExternalEventActivity();
            this.rejectedHandleExternalEventActivity = new System.Workflow.Activities.HandleExternalEventActivity();
            // 
            // createBallotCallExternalMethodActivity
            // 
            this.createBallotCallExternalMethodActivity.InterfaceType = typeof(Microsoft.Samples.Workflow.Communication.HostCommunication.IVotingService);
            this.createBallotCallExternalMethodActivity.MethodName = "CreateBallot";
            this.createBallotCallExternalMethodActivity.Name = "createBallotCallExternalMethodActivity";
            activitybind1.Name = "VotingServiceWorkflow";
            activitybind1.Path = "Alias";
            workflowparameterbinding1.ParameterName = "alias";
            workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, ((System.Workflow.ComponentModel.ActivityBind)(activitybind1)));
            this.createBallotCallExternalMethodActivity.ParameterBindings.Add(workflowparameterbinding1);
            // 
            // waitForResponseListenActivity
            // 
            this.waitForResponseListenActivity.Activities.Add(this.waitForApprovalEventDrivenActivity);
            this.waitForResponseListenActivity.Activities.Add(this.waitForRejectionEventDrivenActivity);
            this.waitForResponseListenActivity.Name = "waitForResponseListenActivity";
            // 
            // waitForApprovalEventDrivenActivity
            // 
            this.waitForApprovalEventDrivenActivity.Activities.Add(this.approvedHandleExternalEventActivity);
            this.waitForApprovalEventDrivenActivity.Name = "waitForApprovalEventDrivenActivity";
            // 
            // waitForRejectionEventDrivenActivity
            // 
            this.waitForRejectionEventDrivenActivity.Activities.Add(this.rejectedHandleExternalEventActivity);
            this.waitForRejectionEventDrivenActivity.Name = "waitForRejectionEventDrivenActivity";
            // 
            // approvedHandleExternalEventActivity
            // 
            this.approvedHandleExternalEventActivity.EventName = "ApprovedProposal";
            this.approvedHandleExternalEventActivity.InterfaceType = typeof(Microsoft.Samples.Workflow.Communication.HostCommunication.IVotingService);
            this.approvedHandleExternalEventActivity.Name = "approvedHandleExternalEventActivity";
            this.approvedHandleExternalEventActivity.Roles = null;
            this.approvedHandleExternalEventActivity.Invoked += new System.EventHandler<ExternalDataEventArgs>(this.OnApproved);
            // 
            // rejectedHandleExternalEventActivity
            // 
            this.rejectedHandleExternalEventActivity.EventName = "RejectedProposal";
            this.rejectedHandleExternalEventActivity.InterfaceType = typeof(Microsoft.Samples.Workflow.Communication.HostCommunication.IVotingService);
            this.rejectedHandleExternalEventActivity.Name = "rejectedHandleExternalEventActivity";
            this.rejectedHandleExternalEventActivity.Roles = null;
            this.rejectedHandleExternalEventActivity.Invoked += new System.EventHandler<ExternalDataEventArgs>(this.OnRejected);
            // 
            // VotingServiceWorkflow
            // 
            this.Activities.Add(this.createBallotCallExternalMethodActivity);
            this.Activities.Add(this.waitForResponseListenActivity);
            this.Name = "VotingServiceWorkflow";
            this.CanModifyActivities = false;
        }

        #endregion

        private CallExternalMethodActivity createBallotCallExternalMethodActivity;
        private ListenActivity waitForResponseListenActivity;
        private EventDrivenActivity waitForApprovalEventDrivenActivity;
        private EventDrivenActivity waitForRejectionEventDrivenActivity;
        private HandleExternalEventActivity approvedHandleExternalEventActivity;
        private HandleExternalEventActivity rejectedHandleExternalEventActivity;

    }
}
