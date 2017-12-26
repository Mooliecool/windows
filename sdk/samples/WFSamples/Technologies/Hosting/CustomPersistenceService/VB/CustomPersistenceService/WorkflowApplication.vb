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
Imports System.Workflow.Runtime
Imports System.Collections.Specialized
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            Try
                ' engine will unload workflow instance when it is idle
                currentWorkflowRuntime.AddService(New FilePersistenceService(True))

                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
                AddHandler currentWorkflowRuntime.WorkflowCreated, AddressOf OnWorkflowCreated
                AddHandler currentWorkflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled
                AddHandler currentWorkflowRuntime.WorkflowUnloaded, AddressOf OnWorkflowUnloaded
                AddHandler currentWorkflowRuntime.WorkflowLoaded, AddressOf OnWorkflowLoaded
                AddHandler currentWorkflowRuntime.ServicesExceptionNotHandled, AddressOf OnExceptionNotHandled

                currentWorkflowRuntime.CreateWorkflow(GetType(PersistenceServiceWorkflow)).Start()

                WaitHandle.WaitOne()
            Catch ex As Exception
                Console.WriteLine("Exception" + vbCrLf + vbTab + "Source: {0}" + vbCrLf + vbTab + "Message: {1}", ex.Source, ex.Message)
            Finally
                currentWorkflowRuntime.StopRuntime()
                Console.WriteLine("Workflow runtime stopped, program exiting... " + vbLf)
            End Try
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine("Workflow completed " + vbLf)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub OnExceptionNotHandled(ByVal sender As Object, ByVal e As System.Workflow.Runtime.ServicesExceptionNotHandledEventArgs)
        Console.WriteLine("Unhandled Workflow Exception ")
        Console.WriteLine("  Type: " + e.GetType().ToString())
        Console.WriteLine("  Message: " + e.Exception.Message)
    End Sub

    Shared Sub OnWorkflowCreated(ByVal sender As Object, ByVal e As System.Workflow.Runtime.WorkflowEventArgs)
        Console.WriteLine("Workflow created " + vbLf)
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As System.Workflow.Runtime.WorkflowEventArgs)
        Console.WriteLine("Workflow idling " + vbLf)
    End Sub

    Shared Sub OnWorkflowLoaded(ByVal sender As Object, ByVal e As System.Workflow.Runtime.WorkflowEventArgs)
        Console.WriteLine("Workflow loaded " + vbLf)
    End Sub

    Shared Sub OnWorkflowUnloaded(ByVal sender As Object, ByVal e As System.Workflow.Runtime.WorkflowEventArgs)
        Console.WriteLine("Workflow unloaded " + vbLf)
    End Sub
End Class


