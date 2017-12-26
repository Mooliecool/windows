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
Imports Microsoft.VisualBasic

Public Class WhileAndParallelWorkflow
    Inherits SequentialWorkflowActivity

    ' number of iterations for While loop
    Private count As Integer

    Private Sub WhileCondition(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
        count = count + 1
        e.Result = (count <= 2)
    End Sub

    Private Sub OnConsoleMessage1(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(vbLf + "In Sequence1 Activity: While loop # {0}", count)
    End Sub

    Private Sub OnConsoleMessage2(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(vbLf + "In Sequence2 Activity: While loop # {0}", count)
    End Sub
End Class
