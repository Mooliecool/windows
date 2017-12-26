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
Imports System.Collections.Generic
Imports System.Text
Imports System.Threading
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared waitHandle As New AutoResetEvent(False)
    Shared readyHandle As New AutoResetEvent(False)
    Shared workflowInstance As WorkflowInstance
    Shared workflowRuntimeInstance As WorkflowRuntime

    Shared Sub Main(ByVal args As String())
        If args.Length < 2 Then
            Console.WriteLine("Usage WorkflowThreading.exe [Single | Multi] [Delay | WaitForMessage]")
            Return
        End If

        If Not args(0).Equals("Single", StringComparison.OrdinalIgnoreCase) And Not args(0).Equals("Multi", StringComparison.OrdinalIgnoreCase) Then
            Console.WriteLine("Specify Single or Multi as a first command line parameter")
            Return
        End If

        If Not args(1).Equals("Delay", StringComparison.OrdinalIgnoreCase) And Not args(1).Equals("WaitForMessage", StringComparison.OrdinalIgnoreCase) Then
            Console.WriteLine("Specify Delay or WaitForMessage as a second command line parameter")
            Return
        End If

        ThreadMonitor.Enlist(Thread.CurrentThread, "Host")
        Console.ForegroundColor = ConsoleColor.White

        ' Start the engine
        workflowRuntimeInstance = New WorkflowRuntime()
        Using workflowRuntimeInstance
            Dim scheduler As ManualWorkflowSchedulerService = Nothing
            If args(0).ToString().Equals("Single", StringComparison.OrdinalIgnoreCase) Then
                scheduler = New ManualWorkflowSchedulerService()
                workflowRuntimeInstance.AddService(scheduler)
            End If

            workflowRuntimeInstance.StartRuntime()

            ' Set up the workflow runtime event handlers
            AddHandler workflowRuntimeInstance.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler workflowRuntimeInstance.WorkflowTerminated, AddressOf OnWorkflowTerminated
            AddHandler workflowRuntimeInstance.WorkflowIdled, AddressOf OnWorkflowIdled
            AddHandler workflowRuntimeInstance.WorkflowCreated, AddressOf OnWorkflowCreated

            ' Load the workflow type
            Dim type As System.Type = GetType(ThreadingWorkflow)
            Dim workflowParameters As New Dictionary(Of String, Object)
            workflowParameters.Add("BranchFlag", args(1))

            Console.WriteLine(vbLf + "--- Before Starting Workflow ---" + vbLf)

            ' Create an instance of the workflow
            workflowInstance = workflowRuntimeInstance.CreateWorkflow(type, workflowParameters)
            workflowInstance.Start()

            Console.WriteLine(vbLf + "--- After Starting Workflow ---" + vbLf)

            If scheduler IsNot Nothing Then
                scheduler.RunWorkflow(workflowInstance.InstanceId)
            End If
            readyHandle.WaitOne()
            If args(1).ToString().Equals("WaitForMessage", StringComparison.OrdinalIgnoreCase) Then
                ' Send message to WaitForMessageActivity's queue
                workflowInstance.EnqueueItem("WaitForMessageQueue", "Hello", Nothing, Nothing)
            End If

            If scheduler IsNot Nothing Then
                scheduler.RunWorkflow(workflowInstance.InstanceId)
            End If
            waitHandle.WaitOne()

            workflowRuntimeInstance.StopRuntime()
        End Using
    End Sub

    Shared Sub OnWorkflowCreated(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "Host", "Host: Processed WorkflowCreated Event")
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        If workflowRuntimeInstance.GetService(Of ManualWorkflowSchedulerService)() IsNot Nothing Then
            ' Set a system timer to reload this workflow when its next timer expires
            SetReloadWorkflowTimer()
        Else
            readyHandle.Set()
        End If

        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "Host", "Host: Processed WorkflowIdle Event")
        Console.WriteLine(vbLf + "--- Workflow Idle ---" + vbLf)
    End Sub

    Shared Sub SetReloadWorkflowTimer()
        Dim reloadTime As DateTime = workflowInstance.GetWorkflowNextTimerExpiration()
        If reloadTime = DateTime.MaxValue Then
            ' WaitForMessage case
            readyHandle.Set()
        Else
            Dim timeDifference As TimeSpan = _
                reloadTime - _
                DateTime.UtcNow + New TimeSpan(0, 0, 0, 0, 1) 


            Dim timer As Timer = New System.Threading.Timer( _
                New TimerCallback(AddressOf ReloadWorkflow), _
                Nothing, _
                CType(IIf(timeDifference < TimeSpan.Zero, TimeSpan.Zero, timeDifference), TimeSpan), _
                New TimeSpan(-1))
        End If
    End Sub

    Shared Sub ReloadWorkflow(ByVal state As Object)
        If (workflowInstance.GetWorkflowNextTimerExpiration() > DateTime.UtcNow) Then
            SetReloadWorkflowTimer()
        Else
            readyHandle.Set()
        End If
    End Sub


    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        ThreadMonitor.WriteToConsole(Thread.CurrentThread, "Host", "Host: Processed WorkflowCompleted Event")

        waitHandle.Set()

        Console.WriteLine(vbLf + "--- Workflow Done ---" + vbLf)
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        waitHandle.Set()
    End Sub

End Class


