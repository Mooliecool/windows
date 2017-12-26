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

<Serializable()> _
<CLSCompliant(False)> _
Public Class FileWatcherEventArgs
    Inherits EventArgs
    Private changeTypeValue As WatcherChangeTypes
    Private fullPathValue As String

    Private nameValue As String

    Public Sub New(ByVal fileSystemEventArgs As FileSystemEventArgs)
        Me.changeTypeValue = fileSystemEventArgs.ChangeType
        Me.fullPathValue = fileSystemEventArgs.FullPath
        Me.nameValue = fileSystemEventArgs.Name
    End Sub

    Public ReadOnly Property ChangeType() As WatcherChangeTypes
        Get
            Return changeTypeValue
        End Get
    End Property

    Public ReadOnly Property FullPath() As String
        Get
            Return fullPathValue
        End Get
    End Property

    Public ReadOnly Property Name() As String
        Get
            Return nameValue
        End Get
    End Property

End Class
