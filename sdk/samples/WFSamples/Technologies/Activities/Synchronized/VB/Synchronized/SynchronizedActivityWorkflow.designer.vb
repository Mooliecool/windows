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
Imports System.Collections.Generic

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class SynchronizedActivityWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim stringcollection1 As System.Collections.Generic.List(Of String) = New System.Collections.Generic.List(Of String)
        Dim stringcollection2 As System.Collections.Generic.List(Of String) = New System.Collections.Generic.List(Of String)
        Me.decrement2 = New System.Workflow.Activities.CodeActivity
        Me.decrement1 = New System.Workflow.Activities.CodeActivity
        Me.increment2 = New System.Workflow.Activities.CodeActivity
        Me.increment1 = New System.Workflow.Activities.CodeActivity
        Me.synchronizedDecrement = New System.Workflow.ComponentModel.SynchronizationScopeActivity
        Me.synchronizedIncrement = New System.Workflow.ComponentModel.SynchronizationScopeActivity
        Me.decrementBranch = New System.Workflow.Activities.SequenceActivity
        Me.incrementBranch = New System.Workflow.Activities.SequenceActivity
        Me.endWorkflow = New System.Workflow.Activities.CodeActivity
        Me.parallelAccess = New System.Workflow.Activities.ParallelActivity
        Me.begin = New System.Workflow.Activities.CodeActivity
        '
        'decrement2
        '
        Me.decrement2.Name = "decrement2"
        AddHandler Me.decrement2.ExecuteCode, AddressOf Me.Decrement2_ExecuteCode
        '
        'decrement1
        '
        Me.decrement1.Name = "decrement1"
        AddHandler Me.decrement1.ExecuteCode, AddressOf Me.Decrement1_ExecuteCode
        '
        'increment2
        '
        Me.increment2.Name = "increment2"
        AddHandler Me.increment2.ExecuteCode, AddressOf Me.Increment2_ExecuteCode
        '
        'increment1
        '
        Me.increment1.Name = "increment1"
        AddHandler Me.increment1.ExecuteCode, AddressOf Me.Increment1_ExecuteCode
        '
        'synchronizedDecrement
        '
        Me.synchronizedDecrement.Activities.Add(Me.decrement1)
        Me.synchronizedDecrement.Activities.Add(Me.decrement2)
        Me.synchronizedDecrement.Name = "synchronizedDecrement"
        stringcollection1.Add("syncMutex")
        Me.synchronizedDecrement.SynchronizationHandles = stringcollection1
        '
        'synchronizedIncrement
        '
        Me.synchronizedIncrement.Activities.Add(Me.increment1)
        Me.synchronizedIncrement.Activities.Add(Me.increment2)
        Me.synchronizedIncrement.Name = "synchronizedIncrement"
        stringcollection2.Add("syncMutex")
        Me.synchronizedIncrement.SynchronizationHandles = stringcollection2
        '
        'decrementBranch
        '
        Me.decrementBranch.Activities.Add(Me.synchronizedDecrement)
        Me.decrementBranch.Name = "decrementBranch"
        '
        'incrementBranch
        '
        Me.incrementBranch.Activities.Add(Me.synchronizedIncrement)
        Me.incrementBranch.Name = "incrementBranch"
        '
        'end
        '
        Me.endWorkflow.Name = "endWorkflow"
        AddHandler Me.endWorkflow.ExecuteCode, AddressOf Me.End_ExecuteCode
        '
        'parallelAccess
        '
        Me.parallelAccess.Activities.Add(Me.incrementBranch)
        Me.parallelAccess.Activities.Add(Me.decrementBranch)
        Me.parallelAccess.Name = "parallelAccess"
        '
        'begin
        '
        Me.begin.Name = "begin"
        AddHandler Me.begin.ExecuteCode, AddressOf Me.Begin_ExecuteCode
        '
        'SynchronizedActivityWorkflow
        '
        Me.Activities.Add(Me.begin)
        Me.Activities.Add(Me.parallelAccess)
        Me.Activities.Add(Me.endWorkflow)
        Me.Name = "SynchronizedActivityWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents parallelAccess As System.Workflow.Activities.ParallelActivity
    Private WithEvents incrementBranch As System.Workflow.Activities.SequenceActivity
    Private WithEvents decrementBranch As System.Workflow.Activities.SequenceActivity
    Private WithEvents increment1 As System.Workflow.Activities.CodeActivity
    Private WithEvents increment2 As System.Workflow.Activities.CodeActivity
    Private WithEvents decrement1 As System.Workflow.Activities.CodeActivity
    Private WithEvents decrement2 As System.Workflow.Activities.CodeActivity
    Private WithEvents endWorkflow As System.Workflow.Activities.CodeActivity
    Private WithEvents synchronizedIncrement As System.Workflow.ComponentModel.SynchronizationScopeActivity
    Private WithEvents synchronizedDecrement As System.Workflow.ComponentModel.SynchronizationScopeActivity
    Private WithEvents begin As System.Workflow.Activities.CodeActivity

End Class
