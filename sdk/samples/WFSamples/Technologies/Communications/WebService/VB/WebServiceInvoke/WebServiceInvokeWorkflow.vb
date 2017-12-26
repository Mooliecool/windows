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
Imports System.Workflow.Activities

Public Class WebServiceInvokeWorkflow
    Inherits SequentialWorkflowActivity

    Private purchaseOrderIdValue As String

    Public Property PurchaseOrderId() As String
        Get
            Return purchaseOrderIdValue
        End Get
        Set(ByVal value As String)
            purchaseOrderIdValue = value
        End Set
    End Property

    Private poStatusValue As String

    Public Property POStatus() As String
        Get
            Return poStatusValue
        End Get
        Set(ByVal value As String)
            poStatusValue = value
        End Set
    End Property

    Private Sub OnExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("PO Status: {0}", POStatus)
    End Sub

    Private Sub OnWebServiceInvoking(ByVal sender As Object, ByVal e As InvokeWebServiceEventArgs)
        PurchaseOrderId = "Laptop Computer"
        Console.WriteLine("Ordering item: {0}", PurchaseOrderId)
    End Sub
End Class
