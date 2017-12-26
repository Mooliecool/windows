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

'This sample demonstrates a canonical purchase order workflow that
' contains a single IfElse activity with two branches.  Each branch
' contains a single code activity.  Since the condition always
' evaluates to true the left branch is always taken and the order
' is approved.
Public Class SequentialWorkflow
    Inherits SequentialWorkflowActivity

    Private amountValue As Integer
    Private statusValue As StatusType

    ' Code condition to evaluate whether to take the first branch of IfElseBranch Activity
    Private Sub IsApproved(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
        e.Result = (Amount < 500)
    End Sub

    ' When the order is < 500 it sets the Status parameter value to "Approved"
    Private Sub OnApproved(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Status = StatusType.Approved
    End Sub

    ' When the order is >= 500 it sets the Status parameter value to "Rejected"
    Private Sub OnRejected(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Status = StatusType.Rejected
    End Sub

    Public Property Amount() As Integer
        Get
            Return Me.amountValue
        End Get
        Set(ByVal value As Integer)
            Me.amountValue = value
        End Set
    End Property

    Public Property Status() As StatusType
        Get
            Return Me.statusValue
        End Get

        Set(ByVal value As StatusType)
            Me.statusValue = value
        End Set
    End Property
End Class

Public Enum StatusType
    None
    Approved
    Rejected
End Enum

