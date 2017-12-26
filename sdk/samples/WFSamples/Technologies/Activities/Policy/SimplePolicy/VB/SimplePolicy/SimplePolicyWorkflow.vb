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

