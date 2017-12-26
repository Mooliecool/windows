'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'This source code is intended only as a supplement to Microsoft
'Development Tools and/or on-line documentation.  See these other
'materials for detailed information regarding Microsoft code samples.
' 
'THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.CodeDom
Imports System.ComponentModel
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules
Imports Microsoft.VisualBasic

Public Class DiscountPolicyWorkflow
    Inherits SequentialWorkflowActivity

    ' set orderValue and customerType to change the discount calculated by the policy
    Private orderValue As Double = 60000.0
    Private customerTypeValue As CustomerType = CustomerType.Residential
    Private discount As Double = 0
    Private calculatedTotal As Double
    Private errorValue As String = String.Empty

    Private Sub OnWorkflowCompleted(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(vbCrLf + "Order value = {0:c}", orderValue)
        Console.WriteLine("Customer type = {0}", customerTypeValue)
        Console.WriteLine("Calculated discount = {0}%", discount)
        Console.WriteLine("Calculated total = {0:c}", calculatedTotal)
        If errorValue.Length > 0 Then
            Console.WriteLine("Discount policy error: {0}", errorValue)
        End If
    End Sub

    ' The RuleWrite attribute indicates that this method updates the calculatedTotal field.
    ' The engine uses this information to reevaluate any conditions that use calculatedTotal
    ' whenever this method is called
    <RuleWrite("calculatedTotal")> _
    Public Sub CalculateTotal(ByVal currentOrderValue As Double, ByVal currentDiscount As Double)
        Me.calculatedTotal = currentOrderValue * (1.0 - currentDiscount / 100.0)
    End Sub

    Public Enum CustomerType
        Empty = 0
        Residential = 1
        Business = 2
    End Enum
End Class

