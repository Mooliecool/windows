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

Public NotInheritable Class SequentialWorkflow
    Inherits SequentialWorkflowActivity

    Private amountValue As Integer
    Private statusValue As StatusType

    Public Property Amount() As Integer
        Get
            Amount = amountValue
        End Get
        Set(ByVal value As Integer)
            amountValue = value
        End Set
    End Property

    Public Property Status() As StatusType
        Get
            Status = statusValue
        End Get
        Set(ByVal value As StatusType)
            statusValue = value
        End Set
    End Property

    ' When the order is < 500 it sets the Status parameter value to "Approved"
    Private Sub OnApproved(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Status = StatusType.Approved
    End Sub

    ' When the order is >= 500 it sets the Status parameter value to "Rejected"
    Private Sub OnRejected(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Status = StatusType.Rejected
    End Sub

    ' Code condition to evaluate whether to take the first branch of IfElseBranch Activity
    Private Sub IsApproved(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
        e.Result = Amount < 500
    End Sub
End Class

Public Enum StatusType
    None
    Approved
    Rejected
End Enum