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

Public Class VotingServiceWorkflow
    Inherits SequentialWorkflowActivity

    Private voterNameValue As String

    Public Property VoterName() As System.String
        Get
            Return Me.voterNameValue
        End Get
        Set(ByVal value As System.String)
            Me.voterNameValue = value
        End Set
    End Property

    Private Sub OnApproved(ByVal sender As System.Object, ByVal e As ExternalDataEventArgs)
        Console.WriteLine("Proposal Approved by {0}.", voterNameValue)
    End Sub

    Private Sub OnRejected(ByVal sender As System.Object, ByVal e As ExternalDataEventArgs)
        Console.WriteLine("Proposal Rejected by {0}.", voterNameValue)
    End Sub
End Class
