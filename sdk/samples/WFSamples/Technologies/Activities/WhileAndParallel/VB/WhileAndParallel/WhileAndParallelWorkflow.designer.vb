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
Partial Public Class WhileAndParallelWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim codecondition1 As System.Workflow.Activities.CodeCondition = New System.Workflow.Activities.CodeCondition
        Me.WhileLoop = New System.Workflow.Activities.WhileActivity
        Me.Parallel1 = New System.Workflow.Activities.ParallelActivity
        Me.Sequence1 = New System.Workflow.Activities.SequenceActivity
        Me.Sequence2 = New System.Workflow.Activities.SequenceActivity
        Me.ConsoleMessage1 = New System.Workflow.Activities.CodeActivity
        Me.ConsoleMessage2 = New System.Workflow.Activities.CodeActivity
        '
        'WhileLoop
        '
        Me.WhileLoop.Activities.Add(Me.Parallel1)
        AddHandler codecondition1.Condition, AddressOf Me.WhileCondition
        Me.WhileLoop.Condition = codecondition1
        Me.WhileLoop.Name = "WhileLoop"
        '
        'Parallel1
        '
        Me.Parallel1.Activities.Add(Me.Sequence1)
        Me.Parallel1.Activities.Add(Me.Sequence2)
        Me.Parallel1.Name = "Parallel1"
        '
        'Sequence1
        '
        Me.Sequence1.Activities.Add(Me.ConsoleMessage1)
        Me.Sequence1.Name = "Sequence1"
        '
        'Sequence2
        '
        Me.Sequence2.Activities.Add(Me.ConsoleMessage2)
        Me.Sequence2.Name = "Sequence2"
        '
        'ConsoleMessage1
        '
        Me.ConsoleMessage1.Name = "ConsoleMessage1"
        AddHandler Me.ConsoleMessage1.ExecuteCode, AddressOf Me.OnConsoleMessage1
        '
        'ConsoleMessage2
        '
        Me.ConsoleMessage2.Name = "ConsoleMessage2"
        AddHandler Me.ConsoleMessage2.ExecuteCode, AddressOf Me.OnConsoleMessage2
        '
        'WhileAndParallelWorkflow
        '
        Me.Activities.Add(Me.WhileLoop)
        Me.Name = "WhileAndParallelWorkflow"
        Me.CanModifyActivities = False
    End Sub
    Private WithEvents WhileLoop As System.Workflow.Activities.WhileActivity
    Private WithEvents Parallel1 As System.Workflow.Activities.ParallelActivity
    Private WithEvents Sequence1 As System.Workflow.Activities.SequenceActivity
    Private WithEvents ConsoleMessage1 As System.Workflow.Activities.CodeActivity
    Private WithEvents ConsoleMessage2 As System.Workflow.Activities.CodeActivity
    Private WithEvents Sequence2 As System.Workflow.Activities.SequenceActivity

End Class
