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

Imports Microsoft.VisualBasic
Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities

Public Class PurchaseOrderWorkflow
    Inherits SequentialWorkflowActivity

    ' The event handler that executes on MethodInvoking event of the CreatePO activity
    Private Sub OnBeforeCreateOrder(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(vbLf + "PO Workflow created")
    End Sub

    ' The event handler that executes on Invoked event of the ApprovePO activity
    Private Sub OnApprovePO(ByVal sender As System.Object, ByVal e As ExternalDataEventArgs)
        Console.WriteLine(vbLf + "Purchase Order Approved")
    End Sub

    ' The event handler that executes on Invoked event of the RejectPO activity
    Private Sub OnRejectPO(ByVal sender As System.Object, ByVal e As ExternalDataEventArgs)
        Console.WriteLine(vbLf + "Purchase Order Rejected")
    End Sub

    ' The event handler that executes on ExecuteCode event of the Timeout activity
    Private Sub OnTimeout(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(vbLf + "Purchase Order Workflow timed out")
    End Sub
End Class
