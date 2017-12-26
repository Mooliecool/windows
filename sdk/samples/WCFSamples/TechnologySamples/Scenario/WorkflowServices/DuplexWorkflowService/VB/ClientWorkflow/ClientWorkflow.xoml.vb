' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
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
    Partial Public Class ClientWorkflow
        Inherits StateMachineWorkflowActivity
        Private generator As New Random()

        ' input for IHostForwardContract.BeginWork
        <NonSerialized()> _
        Public ReturnUri As String = String.Empty

        ' input for IHostForwardContract.SubmitWorkItem
        <NonSerialized()> _
        Public SubmitWorkItemInput As String = String.Empty

        ' output for IForwardContract.BeginWorkflow
        <NonSerialized()> _
        Public ReturnAddress As EndpointAddress10 = Nothing

        Public WorkItemCount As Integer = 0
        Public WorkItemValue As Integer = 0
        Public WorkItemLastPart As String = Nothing
        Public WorkItem As WorkItem = Nothing

        Private Sub SetReturnAddress(ByVal sender As Object, ByVal e As EventArgs)
            Dim epr As EndpointAddress = ContextManager.CreateEndpointAddress(ReturnUri, CType(GetActivityByName("ReceiveWorkItemComplete"), ReceiveActivity))
            ReturnAddress = EndpointAddress10.FromEndpointAddress(epr)
            Dim context As Dictionary(Of String, String) = epr.Headers(0).GetValue(Of Dictionary(Of String, String))()
            Dim contextString As StringBuilder = New StringBuilder()

            For Each pair As KeyValuePair(Of String, String) In context
                contextString.Append(pair.Key)
                contextString.Append(":")
                contextString.Append(pair.Value)
            Next

            DebugOutput("[ClientWorkflow:SetReturnAddress] " + contextString.ToString())

        End Sub

        Private Sub GenerateWorkItemCount(ByVal sender As Object, ByVal e As EventArgs)
            WorkItemCount = generator.[Next](1, 8)
            WorkItemLastPart = SubmitWorkItemInput + "_Completed"
            DebugOutput("[ClientWorkflow:GenerateWorkItemCount] " + WorkItemCount.ToString())
        End Sub

        Private Sub GenerateNextWorkItemValue(ByVal sender As Object, ByVal e As EventArgs)
            WorkItemCount -= 1
            WorkItemValue = generator.[Next](1000, 9999)
            DebugOutput("[ClientWorkflow:GenerateNextWorkItemValue] " + WorkItemCount.ToString() + ", " + WorkItemValue.ToString())
        End Sub

        Private Sub DebugOutput(ByVal output As String)
            Console.WriteLine(output)
        End Sub
    End Class

End Namespace