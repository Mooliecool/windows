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
Partial Public Class StateMachineWorkflow

    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.StartState = New System.Workflow.Activities.StateActivity
        Me.state1 = New System.Workflow.Activities.StateActivity
        Me.CompletedState = New System.Workflow.Activities.StateActivity
        Me.eventDriven1 = New System.Workflow.Activities.EventDrivenActivity
        Me.eventDriven2 = New System.Workflow.Activities.EventDrivenActivity
        Me.startStateDelay = New System.Workflow.Activities.DelayActivity
        Me.code1 = New System.Workflow.Activities.CodeActivity
        Me.setState1 = New System.Workflow.Activities.SetStateActivity
        Me.state1Delay = New System.Workflow.Activities.DelayActivity
        Me.code2 = New System.Workflow.Activities.CodeActivity
        Me.setCompletedState = New System.Workflow.Activities.SetStateActivity
        '
        'StartState
        '
        Me.StartState.Activities.Add(Me.eventDriven1)
        Me.StartState.Name = "StartState"
        '
        'state1
        '
        Me.state1.Activities.Add(Me.eventDriven2)
        Me.state1.Name = "state1"
        '
        'CompletedState
        '
        Me.CompletedState.Name = "CompletedState"
        '
        'eventDriven1
        '
        Me.eventDriven1.Activities.Add(Me.startStateDelay)
        Me.eventDriven1.Activities.Add(Me.code1)
        Me.eventDriven1.Activities.Add(Me.setState1)
        Me.eventDriven1.Name = "eventDriven1"
        '
        'eventDriven2
        '
        Me.eventDriven2.Activities.Add(Me.state1Delay)
        Me.eventDriven2.Activities.Add(Me.code2)
        Me.eventDriven2.Activities.Add(Me.setCompletedState)
        Me.eventDriven2.Name = "eventDriven2"
        '
        'delay1
        '
        Me.startStateDelay.Name = "startStateDelay"
        Me.startStateDelay.TimeoutDuration = System.TimeSpan.Parse("00:00:05")
        '
        'code1
        '
        Me.code1.Name = "code1"
        AddHandler Me.code1.ExecuteCode, AddressOf Me.Code1Handler
        '
        'setState1
        '
        Me.setState1.Name = "setState1"
        Me.setState1.TargetStateName = "state1"
        '
        'delay2
        '
        Me.state1Delay.Name = "state1Delay"
        Me.state1Delay.TimeoutDuration = System.TimeSpan.Parse("00:00:02")
        '
        'code2
        '
        Me.code2.Name = "code2"
        AddHandler Me.code2.ExecuteCode, AddressOf Me.Code2Handler
        '
        'setCompletedState
        '
        Me.setCompletedState.Name = "setCompletedState"
        Me.setCompletedState.TargetStateName = "CompletedState"
        '
        'StateMachineWorkflow
        '
        Me.Activities.Add(Me.StartState)
        Me.Activities.Add(Me.state1)
        Me.Activities.Add(Me.CompletedState)
        Me.CompletedStateName = "CompletedState"
        Me.DynamicUpdateCondition = Nothing
        Me.InitialStateName = "StartState"
        Me.Name = "StateMachineWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private state1 As System.Workflow.Activities.StateActivity
    Private CompletedState As System.Workflow.Activities.StateActivity
    Private eventDriven1 As System.Workflow.Activities.EventDrivenActivity
    Private eventDriven2 As System.Workflow.Activities.EventDrivenActivity
    Private startStateDelay As System.Workflow.Activities.DelayActivity
    Private code1 As System.Workflow.Activities.CodeActivity
    Private setState1 As System.Workflow.Activities.SetStateActivity
    Private state1Delay As System.Workflow.Activities.DelayActivity
    Private code2 As System.Workflow.Activities.CodeActivity
    Private setCompletedState As System.Workflow.Activities.SetStateActivity
    Private StartState As System.Workflow.Activities.StateActivity
End Class
