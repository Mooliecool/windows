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
Imports System.ComponentModel
Imports System.Workflow.Activities

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class SuspendAndTerminateWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.suspend = New System.Workflow.ComponentModel.SuspendActivity
        Me.consoleMessage = New System.Workflow.Activities.CodeActivity
        Me.terminate = New System.Workflow.ComponentModel.TerminateActivity
        '
        'suspend
        '
        Me.suspend.Error = Nothing
        Me.suspend.Name = "suspend"
        '
        'consoleMessage
        '
        Me.consoleMessage.Name = "consoleMessage"
        AddHandler Me.consoleMessage.ExecuteCode, AddressOf Me.OnConsoleMessage
        '
        'terminate
        '
        Me.terminate.Error = Nothing
        Me.terminate.Name = "terminate"
        '
        'SuspendAndTerminateWorkflow
        '
        Me.Activities.Add(Me.suspend)
        Me.Activities.Add(Me.consoleMessage)
        Me.Activities.Add(Me.terminate)
        Me.Name = "SuspendAndTerminateWorkflow"
        Me.CanModifyActivities = False
    End Sub
    Private WithEvents suspend As System.Workflow.ComponentModel.SuspendActivity
    Private WithEvents terminate As System.Workflow.ComponentModel.TerminateActivity
    Private WithEvents consoleMessage As System.Workflow.Activities.CodeActivity

End Class
