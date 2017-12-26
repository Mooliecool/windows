'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities

Namespace Microsoft.Samples.Rules.AdvancedPolicy
    <Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
    Partial Public Class DiscountPolicyWorkflow

        'NOTE: The following procedure is required by the Workflow Designer
        'It can be modified using the Workflow Designer.  
        'Do not modify it using the code editor.
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
            Me.CanModifyActivities = True
            Dim rulesetreference1 As System.Workflow.Activities.Rules.RuleSetReference = New System.Workflow.Activities.Rules.RuleSetReference()
            Me.advancedDiscountPolicy = New System.Workflow.Activities.PolicyActivity()
            '
            'advancedDiscountPolicy
            '
            Me.advancedDiscountPolicy.Name = "advancedDiscountPolicy"
            rulesetreference1.RuleSetName = "DiscountRuleSet"
            Me.advancedDiscountPolicy.RuleSetReference = rulesetreference1
            '
            'DiscountPolicyWorkflow
            '
            Me.Activities.Add(Me.advancedDiscountPolicy)
            Me.Name = "DiscountPolicyWorkflow"
            AddHandler Completed, AddressOf Me.OnWorkflowCompleted
            Me.CanModifyActivities = False

        End Sub
        Private advancedDiscountPolicy As System.Workflow.Activities.PolicyActivity

    End Class
End Namespace
