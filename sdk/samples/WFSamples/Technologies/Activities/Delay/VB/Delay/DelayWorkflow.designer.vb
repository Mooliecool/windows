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

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class DelayWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.logBeforeDelay = New System.Workflow.Activities.CodeActivity
        Me.delay = New System.Workflow.Activities.DelayActivity
        Me.logAfterDelay = New System.Workflow.Activities.CodeActivity
        '
        'logBeforeDelay
        '
        Me.logBeforeDelay.Name = "logBeforeDelay"
        AddHandler Me.logBeforeDelay.ExecuteCode, AddressOf Me.OnLogBeforeDelay
        '
        'delay
        '
        Me.delay.Name = "delay"
        Me.delay.TimeoutDuration = System.TimeSpan.Parse("00:00:02")
        '
        'logAfterDelay
        '
        Me.logAfterDelay.Name = "logAfterDelay"
        AddHandler Me.logAfterDelay.ExecuteCode, AddressOf Me.OnLogAfterDelay
        '
        'DelayWorkflow
        '
        Me.Activities.Add(Me.logBeforeDelay)
        Me.Activities.Add(Me.delay)
        Me.Activities.Add(Me.logAfterDelay)
        Me.Name = "DelayWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents delay As System.Workflow.Activities.DelayActivity
    Private WithEvents logAfterDelay As System.Workflow.Activities.CodeActivity
    Private WithEvents logBeforeDelay As System.Workflow.Activities.CodeActivity

End Class
