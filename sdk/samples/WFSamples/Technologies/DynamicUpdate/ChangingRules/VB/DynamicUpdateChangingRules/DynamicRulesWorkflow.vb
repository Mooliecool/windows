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
Imports System.CodeDom
Imports System.ComponentModel
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Public Class DynamicRulesWorkflow
    Inherits SequentialWorkflowActivity

    Dim i As Integer = 0
    Private amountValue As Integer

    Public Property Amount() As Integer
        Get
            Amount = amountValue
        End Get
        Set(ByVal value As Integer)
            amountValue = value
        End Set
    End Property

    Private Sub Rerun(ByVal sender As System.Object, ByVal e As ConditionalEventArgs)
        e.Result = (i < 2)
        i = i + 1
    End Sub

    Private Sub OnInitAmount(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Loop " + i.ToString())
    End Sub

    Private Sub OnManagerApproval(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("  Get Manager Approval")
    End Sub

    Private Sub OnVPApproval(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("  Get VP Approval")
    End Sub
End Class
