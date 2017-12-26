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
Imports System.Threading
Imports System.Workflow.Activities

Public Class ThreadingWorkflow
    Inherits SequentialWorkflowActivity

    Private branchFlagValue As String

    Public Sub New()
        MyBase.New()
        InitializeComponent()

        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "ThreadingWorkflow: Processed Constructor")
    End Sub

    Public Property BranchFlag() As System.String
        Get
            Return Me.branchFlagValue
        End Get
        Set(ByVal value As System.String)
            Me.branchFlagValue = value
        End Set
    End Property

    Private Sub OnCodeActivity1ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "CodeActivity1: Processed ExecuteCode Event")
    End Sub

    Private Sub OnCodeActivity2ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "CodeActivity2: Processed ExecuteCode Event")
    End Sub

    Private Sub OnCodeActivity3ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "ThreadingWorkflow", "CodeActivity3: Processed ExecuteCode Event")
    End Sub

    Private Sub IfElseBranchActivityCodeCondition(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
        e.Result = Not Me.BranchFlag.Equals("Delay", StringComparison.OrdinalIgnoreCase)
    End Sub
End Class
