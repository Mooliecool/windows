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
Imports System.Workflow.ComponentModel
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            Try
                ' Fire up the engine
                currentWorkflowRuntime.StartRuntime()

                ' Subscribe to events
                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowIdled, AddressOf OnWorkflowIdled
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
                AddHandler currentWorkflowRuntime.ServicesExceptionNotHandled, AddressOf OnExceptionNotHandled

                ' Start PO approval workflow with purchase less than $1000
                Dim poAmount As System.Int32 = 750
                Dim type As System.Type = GetType(DynamicUpdateWorkflow)
                Dim inputParameters As Dictionary(Of String, Object) = New Dictionary(Of String, Object)
                inputParameters.Add("Amount", poAmount)
                currentWorkflowRuntime.CreateWorkflow(type, inputParameters).Start()
                WaitHandle.WaitOne()
            Catch ex As Exception
                Console.WriteLine("Exception Source: {0}" + vbCrLf + "Message: {1}", ex.Source, ex.Message)
            Finally
                currentWorkflowRuntime.StopRuntime()
                Console.WriteLine("Workflow runtime stopped, program exiting... " + vbLf)
            End Try
        End Using
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        WaitHandle.Set()
        Console.WriteLine("  Workflow instance " + e.WorkflowInstance.InstanceId.ToString() + " completed" + vbLf)
    End Sub

    Shared Sub OnWorkflowIdled(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Dim workflowInstance As WorkflowInstance = e.WorkflowInstance
        Dim wRoot As Activity = workflowInstance.GetWorkflowDefinition()
        '
        ' use WorkflowChanges class to author dynamic change
        '
        Dim changes As WorkflowChanges = New WorkflowChanges(wRoot)
        Console.WriteLine("  Host is denying all PO requests - Removing POCreated step")
        '
        ' remove POCreated activity
        '
        changes.TransientWorkflow.Activities.Remove(changes.TransientWorkflow.Activities("POCreated"))
        '
        ' apply transient changes to instance
        '
        workflowInstance.ApplyWorkflowChanges(changes)
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub OnExceptionNotHandled(ByVal sender As Object, ByVal e As ServicesExceptionNotHandledEventArgs)
        Console.WriteLine("Unhandled Workflow Exception ")
        Console.WriteLine("  Type: " + e.GetType().ToString())
        Console.WriteLine("  Message: " + e.Exception.Message)
    End Sub
End Class


