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
        Public isComplete As Boolean = False
        Public owner As String = Nothing

        Private Sub Add(ByVal sender As Object, ByVal e As EventArgs)
            currentValue += inputValue
            Output("Add")
        End Sub

        Private Sub Complete(ByVal sender As Object, ByVal e As EventArgs)
            isComplete = True
        End Sub

        Private Sub ValidateOwner(ByVal sender As Object, ByVal e As OperationValidationEventArgs)
            If String.IsNullOrEmpty(owner) Then
                owner = ExtractCallerName(e.ClaimSets)
                e.IsValid = True
                Console.WriteLine("Owner: " + owner)
            End If
            If owner.Equals(ExtractCallerName(e.ClaimSets)) Then
                e.IsValid = True
            End If
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

        Private Sub Output(ByVal operation As String)
            Console.WriteLine(operation + "(" + inputValue.ToString() + ")")
            Console.WriteLine("[" + currentValue.ToString() + "]")
        End Sub

    End Class
End Namespace