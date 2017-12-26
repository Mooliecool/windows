'----------------------------------------------------------------
' Copyright (c) Microsoft Corporation.  All rights reserved.
'----------------------------------------------------------------

Imports System
Imports System.CodeDom
Imports System.ComponentModel
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

Namespace Microsoft.Samples.Workflow.SimplePolicy
    Public Class SimplePolicyWorkflow
        Inherits SequentialWorkflowActivity

        ' Set orderValue and customerTypeValue to change the discount calculated by the policy
        Private orderValue As Double = 600.0
        Private customerTypeValue As CustomerType = CustomerType.Residential
        Private discount As Double = 0

        Private Sub OnWorkflowCompleted(ByVal sender As System.Object, ByVal e As System.EventArgs)
            Console.WriteLine("Order value = {0:c}", orderValue)
            Console.WriteLine("Customer type = {0}", customerTypeValue)
            Console.WriteLine("Calculated discount = {0}%", discount)
        End Sub

        Public Enum CustomerType
            Empty = 0
            Residential = 1
            Business = 2
        End Enum
    End Class
End Namespace
