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
Imports System.Workflow.Activities
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Shared WaitHandle As New AutoResetEvent(False)

    <STAThread()> _
    Shared Sub Main()
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()

            ' Create our local service and add it to the workflow runtime's list of services
            Dim dataService As ExternalDataExchangeService = New ExternalDataExchangeService()
            currentWorkflowRuntime.AddService(dataService)

            Dim votingService As VotingServiceImpl = New VotingServiceImpl()
            dataService.AddService(votingService)

            ' Start up the runtime and hook the creation and completion events
            currentWorkflowRuntime.StartRuntime()

            AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
            AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated
            AddHandler currentWorkflowRuntime.WorkflowStarted, AddressOf OnWorkflowStarted

            ' Create the workflow's parameters collection
            Dim parameters As New Dictionary(Of String, Object)
            parameters.Add("VoterName", "Jim")
            ' Create and start the workflow
            Dim type As System.Type = GetType(VotingServiceWorkflow)
            currentWorkflowRuntime.CreateWorkflow(type, parameters).Start()

            WaitHandle.WaitOne()

            ' Cleanly stop the runtime and all services
            currentWorkflowRuntime.StopRuntime()
        End Using
    End Sub

    ' Called when a workflow is started
    Shared Sub OnWorkflowStarted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
        Console.WriteLine("Workflow started." + vbCrLf)
    End Sub

    ' Called when a workflow is completed - sets the waitHandle event which signals the main thread
    ' it can continue and exit the host application
    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        Console.WriteLine(vbCrLf + "Workflow completed.")
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

End Class


