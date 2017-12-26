'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class VotingServiceWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim workflowparameterbinding1 As System.Workflow.ComponentModel.WorkflowParameterBinding = New System.Workflow.ComponentModel.WorkflowParameterBinding
        Me.createBallotCallExternalMethodActivity = New System.Workflow.Activities.CallExternalMethodActivity
        Me.waitForResponseListenActivity = New System.Workflow.Activities.ListenActivity
        Me.waitForApprovalEventDrivenActivity = New System.Workflow.Activities.EventDrivenActivity
        Me.waitForRejectionEventDrivenActivity = New System.Workflow.Activities.EventDrivenActivity
        Me.approvedHandleExternalEventActivity = New System.Workflow.Activities.HandleExternalEventActivity
        Me.rejectedHandleExternalEventActivity = New System.Workflow.Activities.HandleExternalEventActivity
        '
        'createBallotCallExternalMethodActivity
        '
        Me.createBallotCallExternalMethodActivity.InterfaceType = GetType(IVotingService)
        Me.createBallotCallExternalMethodActivity.MethodName = "CreateBallot"
        Me.createBallotCallExternalMethodActivity.Name = "createBallotCallExternalMethodActivity"
        activitybind1.Name = "VotingServiceWorkflow"
        activitybind1.Path = "VoterName"
        workflowparameterbinding1.ParameterName = "voterName"
        workflowparameterbinding1.SetBinding(System.Workflow.ComponentModel.WorkflowParameterBinding.ValueProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        Me.createBallotCallExternalMethodActivity.ParameterBindings.Add(workflowparameterbinding1)
        '
        'waitForResponseListenActivity
        '
        Me.waitForResponseListenActivity.Activities.Add(Me.waitForApprovalEventDrivenActivity)
        Me.waitForResponseListenActivity.Activities.Add(Me.waitForRejectionEventDrivenActivity)
        Me.waitForResponseListenActivity.Name = "waitForResponseListenActivity"
        '
        'waitForApprovalEventDrivenActivity
        '
        Me.waitForApprovalEventDrivenActivity.Activities.Add(Me.approvedHandleExternalEventActivity)
        Me.waitForApprovalEventDrivenActivity.Name = "waitForApprovalEventDrivenActivity"
        '
        'waitForRejectionEventDrivenActivity
        '
        Me.waitForRejectionEventDrivenActivity.Activities.Add(Me.rejectedHandleExternalEventActivity)
        Me.waitForRejectionEventDrivenActivity.Name = "waitForRejectionEventDrivenActivity"
        '
        'approvedHandleExternalEventActivity
        '
        Me.approvedHandleExternalEventActivity.EventName = "ApprovedProposal"
        Me.approvedHandleExternalEventActivity.InterfaceType = GetType(IVotingService)
        Me.approvedHandleExternalEventActivity.Name = "approvedHandleExternalEventActivity"
        Me.approvedHandleExternalEventActivity.Roles = Nothing
        AddHandler Me.approvedHandleExternalEventActivity.Invoked, AddressOf Me.OnApproved
        '
        'rejectedHandleExternalEventActivity
        '
        Me.rejectedHandleExternalEventActivity.EventName = "RejectedProposal"
        Me.rejectedHandleExternalEventActivity.InterfaceType = GetType(IVotingService)
        Me.rejectedHandleExternalEventActivity.Name = "rejectedHandleExternalEventActivity"
        Me.rejectedHandleExternalEventActivity.Roles = Nothing
        AddHandler Me.rejectedHandleExternalEventActivity.Invoked, AddressOf Me.OnRejected
        '
        'VotingServiceWorkflow
        '
        Me.Activities.Add(Me.createBallotCallExternalMethodActivity)
        Me.Activities.Add(Me.waitForResponseListenActivity)
        Me.Name = "VotingServiceWorkflow"
        Me.CanModifyActivities = False
    End Sub
    Private WithEvents createBallotCallExternalMethodActivity As System.Workflow.Activities.CallExternalMethodActivity
    Private WithEvents waitForResponseListenActivity As System.Workflow.Activities.ListenActivity
    Private WithEvents waitForApprovalEventDrivenActivity As System.Workflow.Activities.EventDrivenActivity
    Private WithEvents waitForRejectionEventDrivenActivity As System.Workflow.Activities.EventDrivenActivity
    Private WithEvents approvedHandleExternalEventActivity As System.Workflow.Activities.HandleExternalEventActivity
    Private WithEvents rejectedHandleExternalEventActivity As System.Workflow.Activities.HandleExternalEventActivity
End Class
