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
Imports System.Workflow.ComponentModel

<Serializable()> _
Public Class DocumentEventArgs
    Inherits ExternalDataEventArgs

    Private documentIdValue As Guid

    Public Sub New(ByVal instanceId As Guid)
        MyBase.New(instanceId)
        Me.documentIdValue = instanceId
    End Sub

    Public Property DocumentId() As Guid
        Get
            Return Me.documentIdValue
        End Get
        Set(ByVal Value As Guid)
            Me.documentIdValue = Value
        End Set
    End Property
End Class

<ExternalDataExchange()> _
Public Interface IDocumentApproval
    ' send document for approval
    Sub RequestDocumentApproval(ByVal documentId As Guid, ByVal approver As String)

    ' received document for approval
    Event DocumentApproved As EventHandler(Of DocumentEventArgs)
End Interface
