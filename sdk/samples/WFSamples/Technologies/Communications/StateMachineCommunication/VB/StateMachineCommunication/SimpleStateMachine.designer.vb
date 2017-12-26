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

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class SimpleStateMachine

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.setCompletedState = New System.Workflow.Activities.SetStateActivity
        Me.handleExternalEventActivity1 = New System.Workflow.Activities.HandleExternalEventActivity
        Me.startStateEventDriven = New System.Workflow.Activities.EventDrivenActivity
        Me.CompletedState = New System.Workflow.Activities.StateActivity
        Me.StartState = New System.Workflow.Activities.StateActivity
        '
        'setCompletedState
        '
        Me.setCompletedState.Name = "setCompletedState"
        Me.setCompletedState.TargetStateName = "CompletedState"
        '
        'handleExternalEventActivity1
        '
        Me.handleExternalEventActivity1.EventName = "SetState"
        Me.handleExternalEventActivity1.InterfaceType = GetType(IEventService)
        Me.handleExternalEventActivity1.Name = "handleExternalEventActivity1"
        '
        'startStateEventDriven
        '
        Me.startStateEventDriven.Activities.Add(Me.handleExternalEventActivity1)
        Me.startStateEventDriven.Activities.Add(Me.setCompletedState)
        Me.startStateEventDriven.Name = "startStateEventDriven"
        '
        'CompletedState
        '
        Me.CompletedState.Name = "CompletedState"
        '
        'StartState
        '
        Me.StartState.Activities.Add(Me.startStateEventDriven)
        Me.StartState.Name = "StartState"
        '
        'SimpleStateMachine
        '
        Me.Activities.Add(Me.StartState)
        Me.Activities.Add(Me.CompletedState)
        Me.CompletedStateName = "CompletedState"
        Me.DynamicUpdateCondition = Nothing
        Me.InitialStateName = "StartState"
        Me.Name = "SimpleStateMachine"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents CompletedState As System.Workflow.Activities.StateActivity
    Private WithEvents startStateEventDriven As System.Workflow.Activities.EventDrivenActivity
    Private WithEvents handleExternalEventActivity1 As System.Workflow.Activities.HandleExternalEventActivity
    Private WithEvents setCompletedState As System.Workflow.Activities.SetStateActivity
    Private WithEvents StartState As System.Workflow.Activities.StateActivity


End Class
