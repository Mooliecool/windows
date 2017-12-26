' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.ComponentModel
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.WorkflowServices.Samples
    Partial Public Class SequentialCalculatorClient
        Inherits SequentialWorkflowActivity
        Private random As New Random()
        Public outputValue As Integer = Nothing
        Public returnValue As Integer = Nothing

        Private Sub OnBeforeSend(ByVal sender As Object, ByVal e As SendActivityEventArgs)
            outputValue = random.[Next](1, 100)
            Console.WriteLine(e.SendActivity.Name + "(" + outputValue.ToString() + ")")
        End Sub

        Private Sub OnAfterResponse(ByVal sender As Object, ByVal e As SendActivityEventArgs)
            Console.WriteLine("[" + returnValue.ToString() + "]")
        End Sub
    End Class
End Namespace