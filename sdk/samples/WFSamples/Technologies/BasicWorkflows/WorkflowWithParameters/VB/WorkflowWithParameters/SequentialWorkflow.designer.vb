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
Partial Public Class SequentialWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
        Me.ifElseActivity = New System.Workflow.Activities.IfElseActivity
        Me.approveIfElseBranch = New System.Workflow.Activities.IfElseBranchActivity
        Me.rejecteIfElseBranch = New System.Workflow.Activities.IfElseBranchActivity
        Me.Approve = New System.Workflow.Activities.CodeActivity
        Me.Reject = New System.Workflow.Activities.CodeActivity
        '
        'ifElseActivity
        '
        Me.ifElseActivity.Activities.Add(Me.approveIfElseBranch)
        Me.ifElseActivity.Activities.Add(Me.rejecteIfElseBranch)
        Me.ifElseActivity.Name = "ifElseActivity"
        '
        'approveIfElseBranch
        '
        Me.approveIfElseBranch.Activities.Add(Me.Approve)
        AddHandler codecondition1.Condition, AddressOf Me.IsApproved
        Me.approveIfElseBranch.Condition = codecondition1
        Me.approveIfElseBranch.Name = "approveIfElseBranch"
        '
        'rejecteIfElseBranch
        '
        Me.rejecteIfElseBranch.Activities.Add(Me.Reject)
        Me.rejecteIfElseBranch.Name = "rejecteIfElseBranch"
        '
        'Approve
        '
        Me.Approve.Name = "Approve"
        AddHandler Me.Approve.ExecuteCode, AddressOf Me.OnApproved
        '
        'Reject
        '
        Me.Reject.Name = "Reject"
        AddHandler Me.Reject.ExecuteCode, AddressOf Me.OnRejected
        '
        'SequentialWorkflow
        '
        Me.Activities.Add(Me.ifElseActivity)
        Me.Name = "SequentialWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents ifElseActivity As System.Workflow.Activities.IfElseActivity
    Private WithEvents approveIfElseBranch As System.Workflow.Activities.IfElseBranchActivity
    Private WithEvents Approve As System.Workflow.Activities.CodeActivity
    Private WithEvents rejecteIfElseBranch As System.Workflow.Activities.IfElseBranchActivity
    Private WithEvents Reject As System.Workflow.Activities.CodeActivity

End Class
