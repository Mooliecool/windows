'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.Samples.Workflow.SimplePolicy
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Public Class SimplePolicyWorkflow

        'NOTE: The following procedure is required by the Workflow Designer
        'It can be modified using the Workflow Designer.  
        'Do not modify it using the code editor.
        <System.Diagnostics.DebuggerNonUserCode()> _
            Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim rulesetreference1 As System.Workflow.Activities.Rules.RuleSetReference = New System.Workflow.Activities.Rules.RuleSetReference()
            Me.simpleDiscountPolicy = New System.Workflow.Activities.PolicyActivity()
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
        Private simpleDiscountPolicy As System.Workflow.Activities.PolicyActivity

    End Class
End Namespace
