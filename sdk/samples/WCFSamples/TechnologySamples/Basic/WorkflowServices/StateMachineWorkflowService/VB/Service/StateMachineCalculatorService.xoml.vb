' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.WorkflowServices.Samples
    <Serializable()> _
    Partial Public Class StateMachineCalculatorService
        Inherits StateMachineWorkflowActivity
        <NonSerialized()> _
        Public inputValue As Integer = Nothing
        Public currentValue As Integer = Nothing

        Private Sub Add(ByVal sender As Object, ByVal e As EventArgs)
            currentValue += inputValue
        End Sub

        Private Sub Subtract(ByVal sender As Object, ByVal e As EventArgs)
            currentValue -= inputValue
        End Sub

        Private Sub Multiply(ByVal sender As Object, ByVal e As EventArgs)
            currentValue *= inputValue
        End Sub

        Private Sub Divide(ByVal sender As Object, ByVal e As EventArgs)
            currentValue /= inputValue
        End Sub
    End Class

End Namespace