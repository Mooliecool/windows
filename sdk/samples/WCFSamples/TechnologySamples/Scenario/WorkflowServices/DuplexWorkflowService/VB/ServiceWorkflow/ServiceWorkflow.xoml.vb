' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections.Generic
Imports System.Drawing
Imports System.ServiceModel
Imports System.Text
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.WorkflowServices.Samples
    <Serializable()> _
    Partial Public Class ServiceWorkflow
        Inherits StateMachineWorkflowActivity
        Public Const ReverseEndpoint As String = "ReverseEndpoint"

        ' input of IForwardContract.BeginWorkflow
        <NonSerialized()> _
        Public ReturnAddress As EndpointAddress10 = Nothing

        ' input of IForwardContract.BeginWorkItem
        <NonSerialized()> _
        Public BeginWorkItemInput As String = Nothing

        ' input of IForwardContract.ContinueWorkItem
        <NonSerialized()> _
        Public ContinueWorkItemInput As Integer = Nothing

        ' input of IFowardContract.CompleteWorkItem
        <NonSerialized()> _
        Public CompleteWorkItemInput As String = Nothing

        Public CurrentWorkItem As WorkItem = Nothing
        Public CompletedWorkItems As New List(Of WorkItem)(3)

        Private Sub ApplyReturnAddress(ByVal sender As Object, ByVal e As EventArgs)
            ' apply ReturnAddress to ReverseEndpoint
            Dim epr As EndpointAddress = ReturnAddress.ToEndpointAddress()
            ContextManager.ApplyEndpointAddress(CType(GetActivityByName("SendWorkItemComplete"), SendActivity), epr)

            Dim context As Dictionary(Of String, String) = epr.Headers(0).GetValue(Of Dictionary(Of String, String))()
            Dim contextString As StringBuilder = New StringBuilder()

            For Each pair As KeyValuePair(Of String, String) In context
                contextString.Append(pair.Key)
                contextString.Append(":")
                contextString.Append(pair.Value)
            Next

            DebugOutput("[ServiceWorkflow:ApplyReturnAddress] " + contextString.ToString())

        End Sub

        Private Sub CreateWorkItem(ByVal sender As Object, ByVal e As EventArgs)
            ' create a new CurrentWorkItem
            CurrentWorkItem = New WorkItem()
            CurrentWorkItem.FirstPart = BeginWorkItemInput
            DebugOutput("[ServiceWorkflow:CreateWorkItem] " + BeginWorkItemInput)
        End Sub

        Private Sub AddPartsToWorkItem(ByVal sender As Object, ByVal e As EventArgs)
            ' add parts to CurrentWorkItem
            If CurrentWorkItem.PartsList Is Nothing Then
                CurrentWorkItem.PartsList = New List(Of Integer)()
            End If
            CurrentWorkItem.PartsList.Add(ContinueWorkItemInput)
            DebugOutput("[ServiceWorkflow:AddPartsToWorkItem] " + CType(ContinueWorkItemInput, String))
        End Sub

        Private Sub AddCompletedWorkItem(ByVal sender As Object, ByVal e As EventArgs)
            ' complete CurrentWorkItem
            CurrentWorkItem.LastPart = CompleteWorkItemInput
            CompletedWorkItems.Add(CurrentWorkItem)
            DebugOutput("[ServiceWorkflow:AddCompletedWorkItem] " + CompleteWorkItemInput)
        End Sub

        Private Sub RemoveCompletedWorkItem(ByVal sender As Object, ByVal e As EventArgs)
            ' remove completed WorkItem
            Console.ForegroundColor = ConsoleColor.Green
            Console.WriteLine("WorkItemComplete: " + CompletedWorkItems(0).FirstPart)
            Console.ResetColor()
            CompletedWorkItems.RemoveAt(0)
            DebugOutput("[ServiceWorkflow:RemoveCompletedWorkItem] " + CompletedWorkItems.Count.ToString())
        End Sub

        Private Sub DebugOutput(ByVal output As String)
            Console.WriteLine(output)
        End Sub

    End Class

End Namespace