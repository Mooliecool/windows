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
Imports System.IO
Imports System.Threading
Imports System.Data
Imports System.Data.SqlClient
Imports System.Runtime.Serialization.Formatters.Binary
Imports System.Globalization
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Hosting
Imports System.Workflow.Runtime.Tracking
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Const ConnectionString As String = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;"
    Shared WaitHandle As New AutoResetEvent(False)

    Shared Sub Main()
        ' Create WorkflowRuntime
        Using currentWorkflowRuntime As WorkflowRuntime = New WorkflowRuntime()
            Try
                ' Add SqlTrackingService
                Dim sqlTrackingService As SqlTrackingService = New SqlTrackingService(ConnectionString)
                sqlTrackingService.IsTransactional = False
                currentWorkflowRuntime.AddService(sqlTrackingService)

                ' Subscribe to Workflow Suspended WorkflowRuntime Event
                AddHandler currentWorkflowRuntime.WorkflowSuspended, AddressOf OnWorkflowSuspended
                ' Subscribe to Workflow Terminated WorkflowRuntime Event
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                ' Start WorkflowRuntime
                currentWorkflowRuntime.StartRuntime()

                ' Execute the ExceptionWorkflow Workflow
                WriteTitle("Executing The Exception Workflow")
                Dim exceptionWorkflowInstance As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(GetType(ExceptionWorkflow))
                exceptionWorkflowInstance.Start()
                WaitHandle.WaitOne()
                QueryAndWriteTrackingInformationToConsole(exceptionWorkflowInstance.InstanceId, TrackingWorkflowEvent.Exception)
                QueryAndWriteTrackingInformationToConsole(exceptionWorkflowInstance.InstanceId, TrackingWorkflowEvent.Terminated)

                ' Execute the SuspendedWorkflow Workflow
                WriteTitle("Executing The Suspended Workflow")
                Dim suspendedWorkflowInstance As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(GetType(SuspendedWorkflow))
                suspendedWorkflowInstance.Start()
                WaitHandle.WaitOne()
                QueryAndWriteTrackingInformationToConsole(suspendedWorkflowInstance.InstanceId, TrackingWorkflowEvent.Suspended)

            Catch ex As Exception
                Console.WriteLine("Encountered an exception. Exception Source: {0}, Exception Message: {1} ", ex.Source, ex.Message)
            Finally
                ' Stop Runtime
                currentWorkflowRuntime.StopRuntime()
            End Try
        End Using
    End Sub

    Shared Sub OnWorkflowSuspended(ByVal sender As Object, ByVal instance As WorkflowSuspendedEventArgs)
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    ' Reading Data from Tracking Database
    Shared Sub QueryAndWriteTrackingInformationToConsole(ByVal instanceId As Guid, ByVal workflowEventToFind As TrackingWorkflowEvent)
        Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(ConnectionString)

        Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
        sqlTrackingQuery.TryGetWorkflow(instanceId, sqlTrackingWorkflowInstance)

        Dim workflowTrackingRecord As WorkflowTrackingRecord
        For Each workflowTrackingRecord In sqlTrackingWorkflowInstance.WorkflowEvents
            If workflowTrackingRecord.TrackingWorkflowEvent = workflowEventToFind Then
                WriteEventDescriptionAndArgs(workflowTrackingRecord.TrackingWorkflowEvent.ToString(), workflowTrackingRecord.EventArgs, workflowTrackingRecord.EventDateTime)
                Exit For
            End If
        Next
    End Sub

    ' Manipulating and Writing Information to Console
    Shared Sub WriteEventDescriptionAndArgs(ByVal eventDescription As String, ByVal argData As Object, ByVal eventDateTime As DateTime)
        ' Checking the type and the corresponding event
        If TypeOf argData Is TrackingWorkflowSuspendedEventArgs Then
            WriteSuspendedEventArgs(eventDescription, CType(argData, TrackingWorkflowSuspendedEventArgs), eventDateTime)
        End If
        If TypeOf argData Is TrackingWorkflowTerminatedEventArgs Then
            WriteTerminatedEventArgs(eventDescription, CType(argData, TrackingWorkflowTerminatedEventArgs), eventDateTime)
        End If
        If TypeOf argData Is TrackingWorkflowExceptionEventArgs Then
            WriteExceptionEventArgs(eventDescription, CType(argData, TrackingWorkflowExceptionEventArgs), eventDateTime)
        End If
    End Sub

    Shared Sub WriteSuspendedEventArgs(ByVal eventDescription As String, ByVal suspendedEventArgs As TrackingWorkflowSuspendedEventArgs, ByVal eventDataTime As DateTime)
        Console.WriteLine(vbCrLf + "Suspended Event Arguments Read From Tracking Database:")
        Console.WriteLine("EventDataTime: " + eventDataTime.ToString(CultureInfo.CurrentCulture))
        Console.WriteLine("EventDescription: " + eventDescription)
        Console.WriteLine("SuspendedEventArgs Info: " + suspendedEventArgs.Error)
    End Sub

    Shared Sub WriteTerminatedEventArgs(ByVal eventDescription As String, ByVal terminatedEventArgs As TrackingWorkflowTerminatedEventArgs, ByVal eventDataTime As DateTime)
        Console.WriteLine(vbCrLf + "Terminated Event Arguments Read From Tracking Database:")
        Console.WriteLine("EventDataTime: " + eventDataTime.ToString(CultureInfo.CurrentCulture))
        Console.WriteLine("EventDescription: " + eventDescription)
        If terminatedEventArgs.Exception IsNot Nothing Then
            Console.WriteLine("TerminatedEventArgs Exception Message: " + terminatedEventArgs.Exception.Message.ToString())
        End If
    End Sub

    Shared Sub WriteExceptionEventArgs(ByVal eventDescription As String, ByVal exceptionEventArgs As TrackingWorkflowExceptionEventArgs, ByVal eventDataTime As DateTime)
        Console.WriteLine(vbCrLf + "Exception Event Arguments Read From Tracking Database:")
        Console.WriteLine("EventDataTime: " + eventDataTime.ToString(CultureInfo.CurrentCulture))
        Console.WriteLine("EventDescription: " + eventDescription)
        If exceptionEventArgs.Exception IsNot Nothing Then
            Console.WriteLine("ExceptionEventArgs Exception Message: " + exceptionEventArgs.Exception.Message.ToString())
        End If
        Console.WriteLine("ExceptionEventArgs Original Activity Path: " + exceptionEventArgs.OriginalActivityPath.ToString())
    End Sub

    Shared Sub WriteTitle(ByVal title As String)
        Console.WriteLine(vbCrLf + "***********************************************************")
        Console.WriteLine(vbTab + title)
        Console.WriteLine("***********************************************************")
    End Sub
End Class

