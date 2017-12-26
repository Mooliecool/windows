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
Imports System.Collections.Generic
Imports System.Text
Imports System.Workflow.Activities

<Serializable()> _
Public Class DocumentEventArgs
    Inherits ExternalDataEventArgs

    Private approverValue As String

    Public Sub New(ByVal instanceId As Guid, ByVal approver As String)
        MyBase.new(instanceId)
        Me.approverValue = approver
    End Sub

    Public Property Approver() As String
        Get
            Return Me.approverValue
        End Get
        Set(ByVal value As String)
            Me.approverValue = value
        End Set
    End Property
End Class

