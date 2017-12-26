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

'This sample demonstrates a canonical purchase order workflow that
' contains a single IfElse activity with two branches.  Each branch
' contains a single code activity.  Since the condition always
' evaluates to true the left branch is always taken and the order
' is approved.
Public Class SequentialWorkflow
    Inherits SequentialWorkflowActivity

    ' Code condition to evaluate whether to take the first branch, YesIfElseBranch
    ' Since it always returns true, the first branch is always taken.
    Private Sub IsUnderLimit(ByVal sender As System.Object, ByVal e As System.Workflow.Activities.ConditionalEventArgs)
        e.Result = True
    End Sub

    ' The event handler that executes on ExecuteCode event of the ApprovePO activity
    Private Sub OnApproved(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Purchase Order Approved.")
    End Sub

    ' The event handler that executes on ExecuteCode event of the RejectPO activity
    Private Sub OnRejected(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Purchase Order Rejected.")
    End Sub
End Class
