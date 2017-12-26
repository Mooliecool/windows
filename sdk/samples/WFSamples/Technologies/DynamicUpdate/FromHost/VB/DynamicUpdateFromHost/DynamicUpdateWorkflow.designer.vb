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
Imports System.ComponentModel.Design
Imports System.Collections
Imports System.Drawing
Imports System.Reflection
Imports System.Workflow.ComponentModel.Compiler
Imports System.Workflow.ComponentModel.Serialization
Imports System.Workflow.ComponentModel
Imports System.Workflow.ComponentModel.Design
Imports System.Workflow.Runtime
Imports System.Workflow.Activities
Imports System.Workflow.Activities.Rules

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class DynamicUpdateWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.PORequest = New System.Workflow.Activities.CodeActivity
        Me.CreditCheck = New System.Workflow.Activities.DelayActivity
        Me.POCreated = New System.Workflow.Activities.CodeActivity
        '
        'PORequest
        '
        Me.PORequest.Name = "PORequest"
        AddHandler Me.PORequest.ExecuteCode, AddressOf Me.OnPORequest
        '
        'CreditCheck
        '
        Me.CreditCheck.Name = "CreditCheck"
        Me.CreditCheck.TimeoutDuration = System.TimeSpan.Parse("00:00:02")
        AddHandler Me.CreditCheck.InitializeTimeoutDuration, AddressOf Me.OnCreditCheckDelay
        '
        'POCreated
        '
        Me.POCreated.Name = "POCreated"
        AddHandler Me.POCreated.ExecuteCode, AddressOf Me.OnPOCreated
        '
        'Workflow1
        '
        Me.Activities.Add(Me.PORequest)
        Me.Activities.Add(Me.CreditCheck)
        Me.Activities.Add(Me.POCreated)
        Me.Name = "Workflow1"
        Me.CanModifyActivities = False
    End Sub
    Private WithEvents CreditCheck As System.Workflow.Activities.DelayActivity
    Private WithEvents POCreated As System.Workflow.Activities.CodeActivity
    Private WithEvents PORequest As System.Workflow.Activities.CodeActivity

End Class
