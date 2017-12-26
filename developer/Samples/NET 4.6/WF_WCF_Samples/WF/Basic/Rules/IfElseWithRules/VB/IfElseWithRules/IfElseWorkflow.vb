'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------
Imports System
Imports System.ComponentModel
Imports System.CodeDom
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.Samples.Rules.IfElseWithRules
    Public Class IfElseWorkflow
        Inherits SequentialWorkflowActivity

        ' This field is populated by the incoming workflow parameter and is used by the condition
        Private orderValueValue As Integer

        Public Property OrderValue() As Integer
            Get
                Return Me.orderValueValue
            End Get
            Set(ByVal value As Integer)
                Me.orderValueValue = value
            End Set
        End Property

        Private Sub ManagerApprovalHandler(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Get Manager Approval")
        End Sub

        Private Sub VPApprovalHandler(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Get VP Approval")
        End Sub
    End Class
End Namespace
