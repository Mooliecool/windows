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
Partial Public Class NestedExceptionsWorkflow
    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.synchronizationScopeActivity = New System.Workflow.ComponentModel.SynchronizationScopeActivity
        Me.faultHandlersActivity = New System.Workflow.ComponentModel.FaultHandlersActivity
        Me.throwsException = New System.Workflow.Activities.CodeActivity
        Me.faultHandlerActivity = New System.Workflow.ComponentModel.FaultHandlerActivity
        Me.processThrownException = New System.Workflow.Activities.CodeActivity
        '
        'synchronizationScopeActivity
        '
        Me.synchronizationScopeActivity.Activities.Add(Me.throwsException)
        Me.synchronizationScopeActivity.Name = "synchronizationScopeActivity"
        Me.synchronizationScopeActivity.SynchronizationHandles = Nothing
        '
        'faultHandlersActivity
        '
        Me.faultHandlersActivity.Activities.Add(Me.faultHandlerActivity)
        Me.faultHandlersActivity.Name = "faultHandlersActivity"
        '
        'throwsException
        '
        Me.throwsException.Name = "throwsException"
        AddHandler Me.throwsException.ExecuteCode, AddressOf Me.throwsException_ExecuteCode
        '
        'faultHandlerActivity
        '
        Me.faultHandlerActivity.Activities.Add(Me.processThrownException)
        Me.faultHandlerActivity.FaultType = GetType(System.Exception)
        Me.faultHandlerActivity.Name = "faultHandlerActivity"
        '
        'processThrownException
        '
        Me.processThrownException.Name = "processThrownException"
        AddHandler Me.processThrownException.ExecuteCode, AddressOf Me.processThrownException_ExecuteCode
        '
        'NestedExceptionsWorkflow
        '
        Me.Activities.Add(Me.synchronizationScopeActivity)
        Me.Activities.Add(Me.faultHandlersActivity)
        Me.Name = "NestedExceptionsWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents synchronizationScopeActivity As System.Workflow.ComponentModel.SynchronizationScopeActivity
    Private WithEvents faultHandlersActivity As System.Workflow.ComponentModel.FaultHandlersActivity
    Private WithEvents faultHandlerActivity As System.Workflow.ComponentModel.FaultHandlerActivity
    Private WithEvents processThrownException As System.Workflow.Activities.CodeActivity
    Private WithEvents throwsException As System.Workflow.Activities.CodeActivity

End Class
