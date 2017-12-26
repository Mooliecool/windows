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

Imports System
Imports System.Workflow.Activities
Imports Microsoft.VisualBasic

<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Public Class SimpleWorkflow
    Inherits SequentialWorkflowActivity

    Private WithEvents codeConsoleNotify As System.Workflow.Activities.CodeActivity

    <System.Diagnostics.DebuggerNonUserCode()> _
    Private Sub InitializeComponent()
        Me.CanModifyActivities = True
        Me.codeConsoleNotify = New System.Workflow.Activities.CodeActivity
        '
        ' codeConsoleNotify
        '
        Me.codeConsoleNotify.Name = "codeConsoleNotify"
        AddHandler Me.codeConsoleNotify.ExecuteCode, AddressOf Me.Code1Handler
        '
        ' SimpleWorkflow
        '
        Me.Activities.Add(Me.codeConsoleNotify)
        Me.Name = "SimpleWorkflow"
        Me.CanModifyActivities = False
    End Sub

    Private Sub Code1Handler(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Executing the workflow")
    End Sub
End Class
