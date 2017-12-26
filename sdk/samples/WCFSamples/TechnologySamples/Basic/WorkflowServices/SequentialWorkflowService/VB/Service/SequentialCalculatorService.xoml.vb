' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Collections.ObjectModel
Imports System.IdentityModel.Claims
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.WorkflowServices.Samples
    <Serializable()> _
    Partial Public Class SequentialCalculatorService
        Inherits SequentialWorkflowActivity
        <NonSerialized()> _
        Public inputValue As Integer = Nothing
        Public currentValue As Integer = Nothing
        Public powerOn As Boolean = True

        Private Sub PowerOff(ByVal sender As Object, ByVal e As EventArgs)
            powerOn = False
        End Sub

        Private Sub Add(ByVal sender As Object, ByVal e As EventArgs)
            currentValue += inputValue
            Output("Add")
        End Sub

        Private Sub Subtract(ByVal sender As Object, ByVal e As EventArgs)
            currentValue -= inputValue
            Output("Subtract")
        End Sub

        Private Sub Multiply(ByVal sender As Object, ByVal e As EventArgs)
            currentValue *= inputValue
            Output("Multiply")
        End Sub

        Private Sub Divide(ByVal sender As Object, ByVal e As EventArgs)
            currentValue /= inputValue
            Output("Divide")
        End Sub

        Private Sub Output(ByVal operation As String)
            Console.WriteLine(operation + "(" + inputValue.ToString() + ")")
            Console.WriteLine("[" + currentValue.ToString() + "]")
        End Sub

        Private Function ExtractCallerName(ByVal claimSets As ReadOnlyCollection(Of ClaimSet)) As String
            Dim owner As String = String.Empty
            For Each claims As ClaimSet In claimSets
                For Each claim As Claim In claims
                    If claim.ClaimType.Equals(ClaimTypes.Name) AndAlso claim.Right.Equals(Rights.PossessProperty) Then
                        owner = claim.Resource.ToString()
                        Exit For
                    End If
                Next
            Next
            Return owner
        End Function


    End Class
End Namespace