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
Partial Public Class SimplePolicyWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim rulesetreference1 As System.Workflow.Activities.Rules.RuleSetReference = New System.Workflow.Activities.Rules.RuleSetReference
        Me.simpleDiscountPolicy = New System.Workflow.Activities.PolicyActivity
        '
        'simpleDiscountPolicy
        '
        Me.simpleDiscountPolicy.Name = "simpleDiscountPolicy"
        rulesetreference1.RuleSetName = "DiscountRuleSet"
        Me.simpleDiscountPolicy.RuleSetReference = rulesetreference1
        '
        'SimplePolicyWorkflow
        '
        Me.Activities.Add(Me.simpleDiscountPolicy)
        Me.Name = "SimplePolicyWorkflow"
        AddHandler Completed, AddressOf Me.OnWorkflowCompleted
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents discountPolicy As System.Workflow.Activities.PolicyActivity
    Private WithEvents simpleDiscountPolicy As System.Workflow.Activities.PolicyActivity

End Class
