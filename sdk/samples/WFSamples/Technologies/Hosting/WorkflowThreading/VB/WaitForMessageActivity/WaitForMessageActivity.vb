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
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime

Public Class WaitForMessageActivity
    Inherits System.Workflow.ComponentModel.Activity

    Dim workflowQueue As WorkflowQueue

    Protected Overrides Sub Initialize(ByVal provider As System.IServiceProvider)
        MyBase.Initialize(provider)

        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "WaitForMessageActivity", "WaitForMessageActivity: Processed Initialization")

        Dim queuingService As WorkflowQueuingService = CType(provider.GetService(GetType(WorkflowQueuingService)), WorkflowQueuingService)
        Me.workflowQueue = queuingService.CreateWorkflowQueue("WaitForMessageQueue", False)

        AddHandler Me.workflowQueue.QueueItemAvailable, AddressOf Me.HandleExternalEvent
    End Sub

    Protected Overrides Function Execute(ByVal context As System.Workflow.ComponentModel.ActivityExecutionContext) As System.Workflow.ComponentModel.ActivityExecutionStatus
        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "WaitForMessageActivity", "WaitForMessageActivity: Processed Waiting For External Event")

        Return ActivityExecutionStatus.Executing
    End Function

    Private Sub HandleExternalEvent(ByVal sender As Object, ByVal args As QueueEventArgs)
        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "WaitForMessageActivity", "WaitForMessageActivity: Processed External Event")

        Dim data As Object = Me.workflowQueue.Dequeue()

        Dim context As ActivityExecutionContext = CType(sender, ActivityExecutionContext)
        context.CloseActivity()
    End Sub
End Class
