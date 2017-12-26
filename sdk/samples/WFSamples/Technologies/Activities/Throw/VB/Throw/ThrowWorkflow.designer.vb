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
Partial Public Class ThrowWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Dim activitybind1 As System.Workflow.ComponentModel.ActivityBind = New System.Workflow.ComponentModel.ActivityBind
        Me.throwActivity1 = New System.Workflow.ComponentModel.ThrowActivity
        activitybind1.Name = "ThrowWorkflow"
        activitybind1.Path = "ThrownException"
        '
        'throwActivity1
        '
        Me.throwActivity1.FaultType = GetType(System.Exception)
        Me.throwActivity1.Name = "throwActivity1"
        Me.throwActivity1.SetBinding(System.Workflow.ComponentModel.ThrowActivity.FaultProperty, CType(activitybind1, System.Workflow.ComponentModel.ActivityBind))
        '
        'ThrowWorkflow
        '
        Me.Activities.Add(Me.throwActivity1)
        Me.Name = "ThrowWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private throwActivity1 As System.Workflow.ComponentModel.ThrowActivity

End Class
