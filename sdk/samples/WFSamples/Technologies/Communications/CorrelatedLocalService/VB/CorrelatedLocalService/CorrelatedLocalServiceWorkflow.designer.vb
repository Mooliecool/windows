'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
'  This source code is intended only as a supplement to Microsoft
'  Development Tools and/or on-line documentation.  See these other
'  materials for detailed information regarding Microsoft code samples.
' 
'  THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
'  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
'  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
'  PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports System
Imports System.ComponentModel
Imports System.Workflow.Activities

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class CorrelatedLocalServiceWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim correlationtoken1 As System.Workflow.Runtime.CorrelationToken = New System.Workflow.Runtime.CorrelationToken
        Dim correlationtoken2 As System.Workflow.Runtime.CorrelationToken = New System.Workflow.Runtime.CorrelationToken
        Me.parallel1 = New System.Workflow.Activities.ParallelActivity
        Me.sequence1 = New System.Workflow.Activities.SequenceActivity
        Me.sequence2 = New System.Workflow.Activities.SequenceActivity
        Me.createTask1 = New CreateTask
        Me.taskCompleted1 = New TaskCompleted
        Me.createTask2 = New CreateTask
        Me.taskCompleted2 = New TaskCompleted
        '
        'parallel1
        '
        Me.parallel1.Activities.Add(Me.sequence1)
        Me.parallel1.Activities.Add(Me.sequence2)
        Me.parallel1.Name = "parallel1"
        '
        'sequence1
        '
        Me.sequence1.Activities.Add(Me.createTask1)
        Me.sequence1.Activities.Add(Me.taskCompleted1)
        Me.sequence1.Name = "sequence1"
        '
        'sequence2
        '
        Me.sequence2.Activities.Add(Me.createTask2)
        Me.sequence2.Activities.Add(Me.taskCompleted2)
        Me.sequence2.Name = "sequence2"
        '
        'createTask1
        '
        Me.createTask1.Assignee = "Joe"
        correlationtoken1.Name = "c1"
        correlationtoken1.OwnerActivityName = "sequence1"
        Me.createTask1.CorrelationToken = correlationtoken1
        Me.createTask1.Name = "createTask1"
        Me.createTask1.TaskId = "001"
        Me.createTask1.Text = "task 1"
        '
        'taskCompleted1
        '
        Me.taskCompleted1.CorrelationToken = correlationtoken1
        Me.taskCompleted1.EventArgs = Nothing
        Me.taskCompleted1.Name = "taskCompleted1"
        Me.taskCompleted1.Roles = Nothing
        Me.taskCompleted1.Sender = Nothing
        AddHandler Me.taskCompleted1.Invoked, AddressOf Me.OnTaskCompleted
        '
        'createTask2
        '
        Me.createTask2.Assignee = "Kim"
        correlationtoken2.Name = "c2"
        correlationtoken2.OwnerActivityName = "sequence2"
        Me.createTask2.CorrelationToken = correlationtoken2
        Me.createTask2.Name = "createTask2"
        Me.createTask2.TaskId = "002"
        Me.createTask2.Text = "task 2"
        '
        'taskCompleted2
        '
        Me.taskCompleted2.CorrelationToken = correlationtoken2
        Me.taskCompleted2.EventArgs = Nothing
        Me.taskCompleted2.Name = "taskCompleted2"
        Me.taskCompleted2.Roles = Nothing
        Me.taskCompleted2.Sender = Nothing
        AddHandler Me.taskCompleted2.Invoked, AddressOf Me.OnTaskCompleted
        '
        'CorrelatedLocalServiceWorkflow
        '
        Me.Activities.Add(Me.parallel1)
        Me.Name = "CorrelatedLocalServiceWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private parallel1 As System.Workflow.Activities.ParallelActivity
    Private sequence1 As System.Workflow.Activities.SequenceActivity
    Private createTask1 As CreateTask
    Private createTask2 As CreateTask
    Private taskCompleted1 As TaskCompleted
    Private taskCompleted2 As TaskCompleted
    Private sequence2 As System.Workflow.Activities.SequenceActivity

End Class
