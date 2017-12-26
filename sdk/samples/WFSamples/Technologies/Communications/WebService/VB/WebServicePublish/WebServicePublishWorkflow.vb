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

Public NotInheritable Class WebServicePublishWorkflow
    Inherits SequentialWorkflowActivity

    Private purchaseOrderIdValue As String
    Private poStatusValue As String

    Public Property POStatus() As String
        Get
            Return poStatusValue
        End Get
        Set(ByVal value As String)
            poStatusValue = value
        End Set
    End Property

    Public Property PurchaseOrderId() As String
        Get
            Return purchaseOrderIdValue
        End Get
        Set(ByVal value As String)
            purchaseOrderIdValue = value
        End Set
    End Property

    Public Sub New()
        MyBase.New()
        InitializeComponent()
        PurchaseOrderId = "Id"
        POStatus = "InitialStatus"
    End Sub

    Private Sub OnWebServiceInputReceived(ByVal sender As Object, ByVal e As EventArgs) Handles webServiceReceive1.InputReceived
        poStatusValue = String.Format("Approved {0}", PurchaseOrderId)
    End Sub
End Class
