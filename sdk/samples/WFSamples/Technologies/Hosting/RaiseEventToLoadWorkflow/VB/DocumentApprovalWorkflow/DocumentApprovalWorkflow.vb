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

Public Class DocumentApprovalWorkflow
    Inherits SequentialWorkflowActivity

    Private Sub OnMethodInvoking(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Console.WriteLine("Workflow: Request document approval" + vbLf)
    End Sub

    Private Sub OnInvoked(ByVal sender As System.Object, ByVal e As ExternalDataEventArgs)
        Console.WriteLine("Workflow: Received document approval" + vbLf)
    End Sub
End Class
