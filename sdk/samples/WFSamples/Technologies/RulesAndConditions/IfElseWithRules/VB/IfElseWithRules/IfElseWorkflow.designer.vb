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
Imports System.Workflow.Activities.Rules

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class IfElseWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim ruleconditionreference1 As System.Workflow.Activities.Rules.RuleConditionReference = New System.Workflow.Activities.Rules.RuleConditionReference
        Me.VPApproval = New System.Workflow.Activities.CodeActivity
        Me.ManagerApproval = New System.Workflow.Activities.CodeActivity
        Me.ifElseBranch2 = New System.Workflow.Activities.IfElseBranchActivity
        Me.ifElseBranch1 = New System.Workflow.Activities.IfElseBranchActivity
        Me.ifElse1 = New System.Workflow.Activities.IfElseActivity
        '
        'VPApproval
        '
        Me.VPApproval.Name = "VPApproval"
        AddHandler Me.VPApproval.ExecuteCode, AddressOf Me.VPApprovalHandler
        '
        'ManagerApproval
        '
        Me.ManagerApproval.Name = "ManagerApproval"
        AddHandler Me.ManagerApproval.ExecuteCode, AddressOf Me.ManagerApprovalHandler
        '
        'ifElseBranch2
        '
        Me.ifElseBranch2.Activities.Add(Me.VPApproval)
        Me.ifElseBranch2.Name = "ifElseBranch2"
        '
        'ifElseBranch1
        '
        Me.ifElseBranch1.Activities.Add(Me.ManagerApproval)
        ruleconditionreference1.ConditionName = "CheckOrderValue"
        Me.ifElseBranch1.Condition = ruleconditionreference1
        Me.ifElseBranch1.Name = "ifElseBranch1"
        '
        'ifElse1
        '
        Me.ifElse1.Activities.Add(Me.ifElseBranch1)
        Me.ifElse1.Activities.Add(Me.ifElseBranch2)
        Me.ifElse1.Name = "ifElse1"
        '
        'IfElseWorkflow
        '
        Me.Activities.Add(Me.ifElse1)
        Me.Name = "IfElseWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents ifElse1 As System.Workflow.Activities.IfElseActivity
    Private WithEvents ifElseBranch1 As System.Workflow.Activities.IfElseBranchActivity
    Private WithEvents ManagerApproval As System.Workflow.Activities.CodeActivity
    Private WithEvents ifElseBranch2 As System.Workflow.Activities.IfElseBranchActivity
    Private WithEvents VPApproval As System.Workflow.Activities.CodeActivity

End Class
