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
Imports System.Collections.Generic
Imports System.Threading
Imports System.Windows.Forms
Imports System.Workflow.Activities
Imports System.Workflow.Runtime

' Class defines the message passed between the local service and the workflow
<Serializable()> _
Friend Class VotingServiceEventArgs
    Inherits ExternalDataEventArgs
    Private voterNameValue As String

    Public Sub New(ByVal instanceID As Guid, ByVal voterName As String)
        MyBase.New(instanceID)
        Me.voterNameValue = voterName
    End Sub

    Public ReadOnly Property VoterName() As String
        Get
            Return Me.voterNameValue
        End Get
    End Property
End Class

' Workflow communication interface which defines the contract
' between a local service and a workflow
<ExternalDataExchange()> _
Friend Interface IVotingService
    Event ApprovedProposal As EventHandler(Of VotingServiceEventArgs)
    Event RejectedProposal As EventHandler(Of VotingServiceEventArgs)

    Sub CreateBallot(ByVal voterName As String)
End Interface

' Local service that implements the contract on the host side
' i.e. it implements the methods and calls the events, which are
' implemented by the workflow
Friend Class VotingServiceImpl
    Implements IVotingService

    ' Called by the workflow to create a new ballot, this method
    ' creates a new thread which shows a voting dialog to the user
    Public Sub CreateBallot1(ByVal voterName As String) Implements IVotingService.CreateBallot
        Console.WriteLine("Ballot created for {0}.", voterName)
        ShowVotingDialog(New VotingServiceEventArgs(WorkflowEnvironment.WorkflowInstanceId, voterName))
    End Sub

    Public Sub ShowVotingDialog(ByVal votingEventArgs As VotingServiceEventArgs)
        Dim result As DialogResult
        Dim voterName As String = votingEventArgs.VoterName

        ' Show the voting dialog to the user and depending on the response
        ' raise the ApproveProposal or RejectProposal event back to the workflow
        result = MessageBox.Show(String.Format("Approve Proposal, {0}?", voterName), String.Format("{0} Ballot", voterName), MessageBoxButtons.YesNo)
        If result = DialogResult.Yes Then
            RaiseEvent ApprovedProposal(Nothing, votingEventArgs)
        Else
            RaiseEvent RejectedProposal(Nothing, votingEventArgs)
        End If
    End Sub

    Public Event ApprovedProposal(ByVal sender As Object, ByVal e As VotingServiceEventArgs) Implements IVotingService.ApprovedProposal
    Public Event RejectedProposal(ByVal sender As Object, ByVal e As VotingServiceEventArgs) Implements IVotingService.RejectedProposal
End Class

