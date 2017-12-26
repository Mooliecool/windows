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
Imports System.Workflow.ComponentModel
Imports System.Workflow.Activities

' Class defines the message passed between the local service and the workflow
' The Serializable attribute is a required attribute
' indicating that a class can be serialized 
<Serializable()> _
Public Class OrderEventArgs
    Inherits ExternalDataEventArgs
    Private idValue As String

    Public Sub New(ByVal instanceId As Guid, ByVal id As String)
        MyBase.New(instanceId)
        idValue = id
    End Sub

    ' Gets or sets an order id value
    ' Sets by the workflow to pass an order id
    Public Property OrderId() As String
        Get
            Return idValue
        End Get
        Set(ByVal Value As String)
            idValue = Value
        End Set
    End Property
End Class

' The ExternalDataExchange attribute is a required attribute
' indicating that the local service participates in data exchange with a workflow
<ExternalDataExchange()> _
Public Interface IOrderService
    Sub CreateOrder(ByVal id As String)

    Event OrderApproved As EventHandler(Of OrderEventArgs)
    Event OrderRejected As EventHandler(Of OrderEventArgs)
End Interface
