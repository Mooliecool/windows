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
Partial Public Class ThreadingWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
        Me.codeActivity1 = New System.Workflow.Activities.CodeActivity
        Me.ifElseActivity = New System.Workflow.Activities.IfElseActivity
        Me.codeActivity2 = New System.Workflow.Activities.CodeActivity
        Me.codeActivity3 = New System.Workflow.Activities.CodeActivity
        Me.ifElseBranchActivity = New System.Workflow.Activities.IfElseBranchActivity
        Me.ifElseBranchActivity1 = New System.Workflow.Activities.IfElseBranchActivity
        Me.WaitForMessageActivity = New WaitForMessageActivity
        Me.delayActivity = New System.Workflow.Activities.DelayActivity
        '
        'codeActivity1
        '
        Me.codeActivity1.Name = "codeActivity1"
        AddHandler Me.codeActivity1.ExecuteCode, AddressOf Me.OnCodeActivity1ExecuteCode
        '
        'ifElseActivity
        '
        Me.ifElseActivity.Activities.Add(Me.ifElseBranchActivity)
        Me.ifElseActivity.Activities.Add(Me.ifElseBranchActivity1)
        Me.ifElseActivity.Name = "ifElseActivity"
        '
        'codeActivity2
        '
        Me.codeActivity2.Name = "codeActivity2"
        AddHandler Me.codeActivity2.ExecuteCode, AddressOf Me.OnCodeActivity2ExecuteCode
        '
        'codeActivity3
        '
        Me.codeActivity3.Name = "codeActivity3"
        AddHandler Me.codeActivity3.ExecuteCode, AddressOf Me.OnCodeActivity3ExecuteCode
        '
        'ifElseBranchActivity
        '
        Me.ifElseBranchActivity.Activities.Add(Me.WaitForMessageActivity)
        AddHandler codecondition1.Condition, AddressOf Me.IfElseBranchActivityCodeCondition
        Me.ifElseBranchActivity.Condition = codecondition1
        Me.ifElseBranchActivity.Name = "ifElseBranchActivity"
        '
        'ifElseBranchActivity1
        '
        Me.ifElseBranchActivity1.Activities.Add(Me.delayActivity)
        Me.ifElseBranchActivity1.Condition = Nothing
        Me.ifElseBranchActivity1.Name = "ifElseBranchActivity1"
        '
        'WaitForMessageActivity
        '
        Me.WaitForMessageActivity.Name = "WaitForMessageActivity"
        '
        'delayActivity
        '
        Me.delayActivity.Name = "delayActivity"
        Me.delayActivity.TimeoutDuration = System.TimeSpan.Parse("00:00:02")
        '
        'ThreadingWorkflow
        '
        Me.Activities.Add(Me.codeActivity1)
        Me.Activities.Add(Me.ifElseActivity)
        Me.Activities.Add(Me.codeActivity2)
        Me.Activities.Add(Me.codeActivity3)
        Me.Name = "ThreadingWorkflow"
        Me.CanModifyActivities = False
    End Sub

    Private codeActivity1 As System.Workflow.Activities.CodeActivity
    Private ifElseActivity As System.Workflow.Activities.IfElseActivity
    Private ifElseBranchActivity As System.Workflow.Activities.IfElseBranchActivity
    Private WaitForMessageActivity As WaitForMessageActivity
    Private ifElseBranchActivity1 As System.Workflow.Activities.IfElseBranchActivity
    Private delayActivity As System.Workflow.Activities.DelayActivity
    Private codeActivity2 As System.Workflow.Activities.CodeActivity
    Private codeActivity3 As System.Workflow.Activities.CodeActivity

End Class
