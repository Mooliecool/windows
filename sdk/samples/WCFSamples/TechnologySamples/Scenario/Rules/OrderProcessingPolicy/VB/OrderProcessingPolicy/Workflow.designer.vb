' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Reflection
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.Rules.Samples
	Partial Class Workflow
#Region "Designer generated code"

        ''' <summary> 
        ''' Required method for Designer support - do not modify 
        ''' the contents of this method with the code editor.
        ''' </summary>
        <System.Diagnostics.DebuggerNonUserCode()> _
        Private Sub InitializeComponent()
Me.CanModifyActivities = True
Dim rulesetreference1 As System.Workflow.Activities.Rules.RuleSetReference = New System.Workflow.Activities.Rules.RuleSetReference
Me.policyActivity1 = New System.Workflow.Activities.PolicyActivity
'
'policyActivity1
'
Me.policyActivity1.Name = "policyActivity1"
rulesetreference1.RuleSetName = "ValidateOrders"
Me.policyActivity1.RuleSetReference = rulesetreference1
'
'Workflow
'
Me.Activities.Add(Me.policyActivity1)
Me.Name = "Workflow"
Me.CanModifyActivities = False

End Sub
        Private policyActivity1 As System.Workflow.Activities.PolicyActivity

#End Region







	End Class
End Namespace