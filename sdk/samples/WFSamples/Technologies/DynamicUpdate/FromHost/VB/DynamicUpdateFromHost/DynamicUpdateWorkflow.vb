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

Public Class DynamicUpdateWorkflow
    Inherits SequentialWorkflowActivity

    Private amountValue As Integer

    Public Property Amount() As Integer
        Get
            Amount = amountValue
        End Get
        Set(ByVal value As Integer)
            amountValue = value
        End Set
    End Property

    Private Sub OnPORequest(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(String.Format("  PO Request Amount of {0:c}", Amount))
    End Sub

    Private Sub OnCreditCheckDelay(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("  Background check")
    End Sub

    Private Sub OnPOCreated(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("  Processing PO")
    End Sub
End Class
