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
Partial Public Class SimpleInputWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
        Me.printInput = New System.Workflow.Activities.CodeActivity
        Me.dequeueInput = New InputActivity
        Me.sequenceActivity = New System.Workflow.Activities.SequenceActivity
        Me.whileActivity = New System.Workflow.Activities.WhileActivity
        '
        'printInput
        '
        Me.printInput.Name = "printInput"
        AddHandler Me.printInput.ExecuteCode, AddressOf Me.OnPrintInput
        '
        'dequeueInput
        '
        activitybind1.Name = "SimpleInputWorkflow"
        activitybind1.Path = "Data"
        Me.dequeueInput.Name = "dequeueInput"
        Me.dequeueInput.Queue = "Queue"
        Me.dequeueInput.SetBinding(InputActivity.DataValueProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        '
        'sequenceActivity
        '
        Me.sequenceActivity.Activities.Add(Me.dequeueInput)
        Me.sequenceActivity.Activities.Add(Me.printInput)
        Me.sequenceActivity.Name = "sequenceActivity"
        '
        'whileActivity
        '
        Me.whileActivity.Activities.Add(Me.sequenceActivity)
        AddHandler codecondition1.Condition, AddressOf Me.LoopCondition
        Me.whileActivity.Condition = codecondition1
        Me.whileActivity.Name = "whileActivity"
        '
        'SimpleInputWorkflow
        '
        Me.Activities.Add(Me.whileActivity)
        Me.Name = "SimpleInputWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private sequenceActivity As System.Workflow.Activities.SequenceActivity
    Private dequeueInput As InputActivity
    Private printInput As System.Workflow.Activities.CodeActivity
    Private whileActivity As System.Workflow.Activities.WhileActivity

End Class
