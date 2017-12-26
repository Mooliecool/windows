'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class SuspendedWorkflow

    'NOTE: The following procedure is required by the Workflow Designer
    'It can be modified using the Workflow Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.faultHandlersActivity1 = New System.Workflow.ComponentModel.FaultHandlersActivity
        Me.suspend1 = New System.Workflow.ComponentModel.SuspendActivity
        '
        'faultHandlersActivity1
        '
        Me.faultHandlersActivity1.Name = "faultHandlersActivity1"
        '
        'suspend1
        '
        Me.suspend1.Error = "Suspending the workflow"
        Me.suspend1.Name = "suspend1"
        '
        'SuspendedWorkflow
        '
        Me.Activities.Add(Me.suspend1)
        Me.Activities.Add(Me.faultHandlersActivity1)
        Me.Name = "SuspendedWorkflow"
        Me.CanModifyActivities = False

    End Sub
    Private WithEvents faultHandlersActivity1 As System.Workflow.ComponentModel.FaultHandlersActivity
    Private WithEvents suspend1 As System.Workflow.ComponentModel.SuspendActivity

End Class
