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
Partial Public Class PersistenceServicesWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.code1 = New System.Workflow.Activities.CodeActivity
        Me.delay1 = New System.Workflow.Activities.DelayActivity
        Me.code2 = New System.Workflow.Activities.CodeActivity
        '
        'code1
        '
        Me.code1.Name = "code1"
        AddHandler Me.code1.ExecuteCode, AddressOf Me.OnCode1ExecuteCode
        '
        'delay1
        '
        Me.delay1.Name = "delay1"
        Me.delay1.TimeoutDuration = System.TimeSpan.Parse("00:00:05")
        '
        'code2
        '
        Me.code2.Name = "code2"
        AddHandler Me.code2.ExecuteCode, AddressOf Me.OnCode2ExecuteCode
        '
        'PersistenceServices
        '
        Me.Activities.Add(Me.code1)
        Me.Activities.Add(Me.delay1)
        Me.Activities.Add(Me.code2)
        Me.Name = "PersistenceServicesWorkflow"
        Me.CanModifyActivities = False
    End Sub
    Private WithEvents code1 As System.Workflow.Activities.CodeActivity
    Private WithEvents delay1 As System.Workflow.Activities.DelayActivity
    Private WithEvents code2 As System.Workflow.Activities.CodeActivity

End Class
