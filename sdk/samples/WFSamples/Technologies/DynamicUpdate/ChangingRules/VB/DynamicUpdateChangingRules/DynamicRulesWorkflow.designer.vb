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
Imports System.Workflow.Activities.Rules

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class DynamicRulesWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim ruleconditionreference1 As System.Workflow.Activities.Rules.RuleConditionReference = New System.Workflow.Activities.Rules.RuleConditionReference
        Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
        Me.getVPApproval = New System.Workflow.Activities.CodeActivity
        Me.getManagerApproval = New System.Workflow.Activities.CodeActivity
        Me.vpApprovalBranch = New System.Workflow.Activities.IfElseBranchActivity
        Me.managerApprovalBranch = New System.Workflow.Activities.IfElseBranchActivity
        Me.delayActivity = New System.Workflow.Activities.DelayActivity
        Me.ifElseActivity = New System.Workflow.Activities.IfElseActivity
        Me.initAmount = New System.Workflow.Activities.CodeActivity
        Me.mainSequence = New System.Workflow.Activities.SequenceActivity
        Me.whileLoop = New System.Workflow.Activities.WhileActivity
        '
        'getVPApproval
        '
        Me.getVPApproval.Name = "getVPApproval"
        AddHandler Me.getVPApproval.ExecuteCode, AddressOf Me.OnVPApproval
        '
        'getManagerApproval
        '
        Me.getManagerApproval.Name = "getManagerApproval"
        AddHandler Me.getManagerApproval.ExecuteCode, AddressOf Me.OnManagerApproval
        '
        'vpApprovalBranch
        '
        Me.vpApprovalBranch.Activities.Add(Me.getVPApproval)
        Me.vpApprovalBranch.Name = "vpApprovalBranch"
        '
        'managerApprovalBranch
        '
        Me.managerApprovalBranch.Activities.Add(Me.getManagerApproval)
        ruleconditionreference1.ConditionName = "Check"
        Me.managerApprovalBranch.Condition = ruleconditionreference1
        Me.managerApprovalBranch.Name = "managerApprovalBranch"
        '
        'delayActivity
        '
        Me.delayActivity.Name = "delayActivity"
        Me.delayActivity.TimeoutDuration = System.TimeSpan.Parse("00:00:02")
        '
        'ifElseActivity
        '
        Me.ifElseActivity.Activities.Add(Me.managerApprovalBranch)
        Me.ifElseActivity.Activities.Add(Me.vpApprovalBranch)
        Me.ifElseActivity.Name = "ifElseActivity"
        '
        'initAmount
        '
        Me.initAmount.Name = "initAmount"
        AddHandler Me.initAmount.ExecuteCode, AddressOf Me.OnInitAmount
        '
        'mainSequence
        '
        Me.mainSequence.Activities.Add(Me.initAmount)
        Me.mainSequence.Activities.Add(Me.ifElseActivity)
        Me.mainSequence.Activities.Add(Me.delayActivity)
        Me.mainSequence.Name = "mainSequence"
        '
        'whileLoop
        '
        Me.whileLoop.Activities.Add(Me.mainSequence)
        AddHandler codecondition1.Condition, AddressOf Me.Rerun
        Me.whileLoop.Condition = codecondition1
        Me.whileLoop.Name = "whileLoop"
        '
        'DynamicRulesWorkflow
        '
        Me.Activities.Add(Me.whileLoop)
        Me.Name = "DynamicRulesWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private mainSequence As System.Workflow.Activities.SequenceActivity
    Private initAmount As System.Workflow.Activities.CodeActivity
    Private ifElseActivity As System.Workflow.Activities.IfElseActivity
    Private managerApprovalBranch As System.Workflow.Activities.IfElseBranchActivity
    Private vpApprovalBranch As System.Workflow.Activities.IfElseBranchActivity
    Private getManagerApproval As System.Workflow.Activities.CodeActivity
    Private getVPApproval As System.Workflow.Activities.CodeActivity
    Private delayActivity As System.Workflow.Activities.DelayActivity
    Private whileLoop As System.Workflow.Activities.WhileActivity

End Class
