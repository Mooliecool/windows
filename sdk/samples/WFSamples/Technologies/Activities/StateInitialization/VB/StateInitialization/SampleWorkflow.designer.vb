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

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class SampleWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.SetCompletedState = New System.Workflow.Activities.SetStateActivity
        Me.ReportStateStart = New System.Workflow.Activities.CodeActivity
        Me.stateInitialization = New System.Workflow.Activities.StateInitializationActivity
        Me.CompletedState = New System.Workflow.Activities.StateActivity
        Me.StartState = New System.Workflow.Activities.StateActivity
        '
        'SetCompletedState
        '
        Me.SetCompletedState.Name = "SetCompletedState"
        Me.SetCompletedState.TargetStateName = "CompletedState"
        '
        'ReportStateStart
        '
        Me.ReportStateStart.Name = "ReportStateStart"
        AddHandler Me.ReportStateStart.ExecuteCode, AddressOf Me.ReportStateStart_ExecuteCode
        '
        'stateInitialization
        '
        Me.stateInitialization.Activities.Add(Me.ReportStateStart)
        Me.stateInitialization.Activities.Add(Me.SetCompletedState)
        Me.stateInitialization.Name = "stateInitialization"
        '
        'CompletedState
        '
        Me.CompletedState.Name = "CompletedState"
        '
        'StartState
        '
        Me.StartState.Activities.Add(Me.stateInitialization)
        Me.StartState.Name = "StartState"
        '
        'SampleWorkflow
        '
        Me.Activities.Add(Me.StartState)
        Me.Activities.Add(Me.CompletedState)
        Me.CompletedStateName = "CompletedState"
        Me.DynamicUpdateCondition = Nothing
        Me.InitialStateName = "StartState"
        Me.Name = "SampleWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents CompletedState As System.Workflow.Activities.StateActivity
    Private WithEvents stateInitialization As System.Workflow.Activities.StateInitializationActivity
    Private WithEvents ReportStateStart As System.Workflow.Activities.CodeActivity
    Private WithEvents SetCompletedState As System.Workflow.Activities.SetStateActivity
    Private WithEvents StartState As System.Workflow.Activities.StateActivity


End Class
