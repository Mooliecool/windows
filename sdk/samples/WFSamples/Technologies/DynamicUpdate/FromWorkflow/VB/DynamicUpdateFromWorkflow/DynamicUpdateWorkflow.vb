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
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel

Public Class DynamicUpdateWorkflow
    Inherits SequentialWorkflowActivity

    Private Const ApprovedAmount As Integer = 1000
    Private amountValue As Integer

    Public Property Amount() As Integer
        Get
            Amount = amountValue
        End Get
        Set(ByVal value As Integer)
            amountValue = value
        End Set
    End Property

    Private Sub OnPORequest(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("  PO Request Amount of {0:c}", Amount)
    End Sub

    Private Sub OnCreditCheckDelay(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("  Background check delay")
    End Sub

    Private Sub OnPOCreated(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("  PO Created")
    End Sub

    Private Sub OnAddApprovalStep(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim invokeApprovalStepWorkflow As New InvokeWorkflowActivity()
        '
        ' Use the WorkflowChanges class to author a dynamic change
        '
        Dim changes As New WorkflowChanges(Me)
        '
        ' setup to invoke ApprovalStepWorkflow type
        '
        Dim type As Type = GetType(ApprovalStepWorkflow)
        invokeApprovalStepWorkflow.Name = "AddApprovalStepWorkflow"
        invokeApprovalStepWorkflow.TargetWorkflow = type
        '
        ' insert invokeApprovalStepWorkflow in ifElse "CheckApproval" transient activity collection
        '
        Dim checkApproval As CompositeActivity = CType(changes.TransientWorkflow.Activities("CheckApproval"), CompositeActivity)
        Dim approvedBranch As CompositeActivity = CType(checkApproval.Activities("Approved"), CompositeActivity)
        approvedBranch.Activities.Add(invokeApprovalStepWorkflow)
        '
        ' apply transient changes to instance
        '
        Me.ApplyWorkflowChanges(changes)
        Console.WriteLine("  Added an InvokeWorkflow activity within the workflow to approve the PO")
    End Sub

    Private Sub AddApprovalStepCondition(ByVal sender As System.Object, ByVal e As ConditionalEventArgs)
        e.Result = (Amount >= ApprovedAmount)
    End Sub
End Class
