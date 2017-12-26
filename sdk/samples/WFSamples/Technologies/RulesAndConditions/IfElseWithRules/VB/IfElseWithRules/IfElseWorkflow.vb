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
Imports System.CodeDom
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

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
