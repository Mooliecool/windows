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
Imports System.Workflow.Activities

<Serializable()> _
Public Class OrderEventArgs
    Inherits ExternalDataEventArgs

    Private orderIdValue As String

    Public Sub New(ByVal instanceId As Guid, ByVal orderId As String)
        MyBase.New(instanceId)
        orderIdValue = orderId
    End Sub

    Public Property OrderId() As String
        Get
            Return orderIdValue
        End Get
        Set(ByVal value As String)
            orderIdValue = value
        End Set
    End Property
End Class
