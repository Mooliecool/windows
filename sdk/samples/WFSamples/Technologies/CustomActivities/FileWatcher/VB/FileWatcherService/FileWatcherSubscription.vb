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
Imports System.IO

Friend Class FileWatcherSubscription
    Private fileSystemWatcherValue As FileSystemWatcher
    Private workflowInstanceIdValue As Guid
    Private queueNameValue As IComparable

    Friend Sub New(ByVal fileSystemWatcher As FileSystemWatcher, _
    ByVal workflowInstanceId As Guid, ByVal queueName As IComparable)
        Me.fileSystemWatcherValue = fileSystemWatcher
        Me.workflowInstanceIdValue = workflowInstanceId
        Me.queueNameValue = queueName
    End Sub

    Friend ReadOnly Property FileSystemWatcher() As FileSystemWatcher
        Get
            Return fileSystemWatcherValue
        End Get
    End Property

    Friend ReadOnly Property WorkflowInstanceId() As Guid
        Get
            Return workflowInstanceIdValue
        End Get
    End Property

    Friend ReadOnly Property QueueName() As IComparable
        Get
            Return queueNameValue
        End Get
    End Property

End Class
