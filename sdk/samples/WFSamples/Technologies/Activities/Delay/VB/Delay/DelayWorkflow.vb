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
Imports System.Globalization
Imports System.Workflow.Activities
Imports Microsoft.VisualBasic

Public Class DelayWorkflow
    Inherits SequentialWorkflowActivity

    Dim before As DateTime
    Dim after As DateTime
    Dim delta As TimeSpan

    ' The event handler that executes on ExecuteCode event of the logBeforeDelay activity
    Private Sub OnLogBeforeDelay(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(vbLf + "Executing code activity before the delay")
        before = DateTime.Now
        Console.WriteLine("Current Time: {0}", before.ToString(CultureInfo.CurrentCulture))
    End Sub

    ' The event handler that executes on ExecuteCode event of the logAfterDelay activity
    Private Sub OnLogAfterDelay(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine(vbLf + "Executing code activity after the delay")
        after = DateTime.Now
        Console.WriteLine("Current Time: {0}", after.ToString(CultureInfo.CurrentCulture))
        delta = after.Subtract(before)
        Console.WriteLine(vbLf + "Workflow completed after the following delay: {0}", delta.ToString())
    End Sub
End Class
