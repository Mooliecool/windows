'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class FileWatcherWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.Delay1 = New System.Workflow.Activities.DelayActivity
        Me.Event2 = New FileSystemEvent
        Me.eventDriven2 = New System.Workflow.Activities.EventDrivenActivity
        Me.eventDriven1 = New System.Workflow.Activities.EventDrivenActivity
        Me.Event3 = New FileSystemEvent
        Me.listen1 = New System.Workflow.Activities.ListenActivity
        Me.Event1 = New FileSystemEvent
        '
        'Delay1
        '
        Me.Delay1.Name = "Delay1"
        Me.Delay1.TimeoutDuration = System.TimeSpan.Parse("00:00:20")
        '
        'Event2
        '
        Me.Event2.IncludeSubdirectories = False
        Me.Event2.Name = "Event2"
        Me.Event2.NotifyFilter = CType(((System.IO.NotifyFilters.FileName Or System.IO.NotifyFilters.DirectoryName) _
                    Or System.IO.NotifyFilters.LastWrite), System.IO.NotifyFilters)
        Me.Event2.Path = "C:\temp2"
        AddHandler Me.Event2.FileWatcherBefore, AddressOf Me.BeforeFileSystemEvent
        AddHandler Me.Event2.FileWatcherEventHandler, AddressOf Me.AfterFileSystemEvent
        '
        'eventDriven2
        '
        Me.eventDriven2.Activities.Add(Me.Delay1)
        Me.eventDriven2.Name = "eventDriven2"
        '
        'eventDriven1
        '
        Me.eventDriven1.Activities.Add(Me.Event2)
        Me.eventDriven1.Name = "eventDriven1"
        '
        'Event3
        '
        Me.Event3.IncludeSubdirectories = False
        Me.Event3.Name = "Event3"
        Me.Event3.NotifyFilter = CType(((System.IO.NotifyFilters.FileName Or System.IO.NotifyFilters.DirectoryName) _
                    Or System.IO.NotifyFilters.LastWrite), System.IO.NotifyFilters)
        Me.Event3.Path = "C:\temp"
        AddHandler Me.Event3.FileWatcherBefore, AddressOf Me.BeforeFileSystemEvent
        AddHandler Me.Event3.FileWatcherEventHandler, AddressOf Me.AfterFileSystemEvent
        '
        'listen1
        '
        Me.listen1.Activities.Add(Me.eventDriven1)
        Me.listen1.Activities.Add(Me.eventDriven2)
        Me.listen1.Name = "listen1"
        '
        'Event1
        '
        Me.Event1.IncludeSubdirectories = False
        Me.Event1.Name = "Event1"
        Me.Event1.NotifyFilter = CType(((System.IO.NotifyFilters.FileName Or System.IO.NotifyFilters.DirectoryName) _
                    Or System.IO.NotifyFilters.LastWrite), System.IO.NotifyFilters)
        Me.Event1.Path = "C:\temp"
        AddHandler Me.Event1.FileWatcherBefore, AddressOf Me.BeforeFileSystemEvent
        AddHandler Me.Event1.FileWatcherEventHandler, AddressOf Me.AfterFileSystemEvent
        '
        'FileWatcherWorkflow
        '
        Me.Activities.Add(Me.Event1)
        Me.Activities.Add(Me.listen1)
        Me.Activities.Add(Me.Event3)
        Me.Name = "FileWatcherWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private eventDriven2 As System.Workflow.Activities.EventDrivenActivity
    Private eventDriven1 As System.Workflow.Activities.EventDrivenActivity
    Private listen1 As System.Workflow.Activities.ListenActivity
    Private Event2 As FileSystemEvent
    Private Delay1 As System.Workflow.Activities.DelayActivity
    Private Event3 As FileSystemEvent
    Private Event1 As FileSystemEvent

End Class
