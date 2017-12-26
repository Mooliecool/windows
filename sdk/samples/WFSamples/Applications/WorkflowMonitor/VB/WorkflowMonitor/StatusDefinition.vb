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

' Class to store workflow instance information - create one per workflow instance in the database
Friend Class WorkflowStatusInfo
    Private idValue As String
    Private nameValue As String
    Private statusValue As String
    Private createdDateTimeValue As String
    Private instanceIdValue As Guid
    Private listViewItemValue As ListViewItem

    Friend Sub New(ByVal id As String, ByVal name As String, ByVal status As String, ByVal createdDateTime As String, ByVal instanceId As Guid, ByVal listViewItem As ListViewItem)
        Me.idValue = id
        Me.nameValue = name
        Me.statusValue = status
        Me.createdDateTimeValue = createdDateTime
        Me.instanceIdValue = instanceId
        Me.listViewItemValue = listViewItem
    End Sub

    Friend WriteOnly Property Status() As String
        Set(ByVal Value As String)
            statusValue = Value
        End Set
    End Property

    Friend ReadOnly Property WorkflowListViewItem() As ListViewItem
        Get
            Return listViewItemValue
        End Get
    End Property

    Friend ReadOnly Property InstanceId() As Guid
        Get
            Return instanceIdValue
        End Get
    End Property
End Class

' Class to store activity information - create one per activity for the selected workflow
Friend Class ActivityStatusInfo
    Private nameValue As String
    Private statusValue As String

    Friend Sub New(ByVal name As String, ByVal status As String)
        Me.nameValue = name
        Me.statusValue = status
    End Sub

    Friend ReadOnly Property Name() As String
        Get
            Return nameValue
        End Get
    End Property

    Friend ReadOnly Property Status() As String
        Get
            Return statusValue
        End Get
    End Property
End Class
