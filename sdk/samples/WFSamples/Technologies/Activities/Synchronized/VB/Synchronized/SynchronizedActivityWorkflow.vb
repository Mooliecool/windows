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

Public Class SynchronizedActivityWorkflow
    Inherits SequentialWorkflowActivity

    Private count As Integer

    Private Sub Begin_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Begin i = " + Me.count.ToString())
    End Sub

    Private Sub Increment1_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.count = Me.count + 1
        Console.WriteLine("Increment1 i = " + Me.count.ToString())
    End Sub

    Private Sub Increment2_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.count = Me.count + 1
        Console.WriteLine("Increment2 i = " + Me.count.ToString())
    End Sub

    Private Sub Decrement1_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.count = Me.count - 1
        Console.WriteLine("Decrement1 i = " + Me.count.ToString())
    End Sub

    Private Sub Decrement2_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.count = Me.count - 1
        Console.WriteLine("Decrement2 i = " + Me.count.ToString())
    End Sub

    Private Sub End_ExecuteCode(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("End i = " + Me.count.ToString())
    End Sub
End Class
