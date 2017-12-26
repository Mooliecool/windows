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
Imports System.Globalization
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Tracking
Imports System.Workflow.Activities
Imports System.Workflow.ComponentModel
Imports System.IO
Imports System.Data
Imports System.Data.SqlClient
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Private Const connectionString As String = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;"

    Private Shared waitHandle As AutoResetEvent = New AutoResetEvent(False)

    Shared Sub Main()
        Try
            ' Create the tracking profile to track user track points
            CreateAndInsertTrackingProfile()

            Using workflowRuntimeInstance As New WorkflowRuntime()

                ' Add the SQL tracking service to the run time
                workflowRuntimeInstance.AddService(New SqlTrackingService(connectionString))

                workflowRuntimeInstance.StartRuntime()

                AddHandler workflowRuntimeInstance.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler workflowRuntimeInstance.WorkflowTerminated, AddressOf OnWorkflowTerminated

                Dim workflowInstance As WorkflowInstance = workflowRuntimeInstance.CreateWorkflow(GetType(SimpleWorkflow))
                workflowInstance.Start()

                Dim workflowInstanceId As Guid = workflowInstance.InstanceId

                ' Wait for the workflow to complete
                waitHandle.WaitOne()

                workflowRuntimeInstance.StopRuntime()

                ' Get the tracking events from the database
                GetUserTrackingEvents(workflowInstanceId)
                Console.WriteLine("Done Running The workflow.")
            End Using
        Catch ex As Exception
            If Not ex.InnerException Is Nothing Then
                Console.WriteLine(ex.InnerException.Message)
            Else
                Console.WriteLine(ex.Message)
            End If
        End Try
    End Sub

    Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal e As WorkflowCompletedEventArgs)
        waitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        waitHandle.Set()
    End Sub

    Private Shared Sub CreateAndInsertTrackingProfile()
        Dim profile As TrackingProfile = New TrackingProfile()

        Dim trackPoint As ActivityTrackPoint = New ActivityTrackPoint()
        Dim location As ActivityTrackingLocation = New ActivityTrackingLocation(GetType(Activity))
        location.MatchDerivedTypes = True
        Dim statuses As IEnumerable(Of ActivityExecutionStatus) = CType(System.Enum.GetValues(GetType(ActivityExecutionStatus)), IEnumerable(Of ActivityExecutionStatus))
        For Each status As ActivityExecutionStatus In statuses
            location.ExecutionStatusEvents.Add(status)
        Next
        trackPoint.MatchingLocations.Add(location)
        profile.ActivityTrackPoints.Add(trackPoint)
        profile.Version = New Version("1.0.0.0")

        ' Adding a user track point to the tracking profile
        Dim utp As UserTrackPoint = New UserTrackPoint()

        ' Adding a user location to the track point 
        Dim ul As UserTrackingLocation = New UserTrackingLocation(GetType(String), GetType(CodeActivity))
        ul.MatchDerivedActivityTypes = True
        utp.MatchingLocations.Add(ul)
        profile.UserTrackPoints.Add(utp)

        ' Serialize the profile
        Dim serializer As TrackingProfileSerializer = New TrackingProfileSerializer()
        Dim writer As StringWriter = New StringWriter(New StringBuilder(), CultureInfo.InvariantCulture)
        serializer.Serialize(writer, profile)
        Dim trackingprofile As String = writer.ToString()
        InsertTrackingProfile(trackingprofile)
    End Sub

    Private Shared Sub InsertTrackingProfile(ByVal profile As String)
        Dim cmd As SqlCommand = New SqlCommand()

        cmd.CommandType = CommandType.StoredProcedure
        cmd.CommandText = "dbo.UpdateTrackingProfile"
        cmd.Connection = New SqlConnection(connectionString)
        Try
            cmd.Parameters.Clear()

            Dim param1 As SqlParameter = New SqlParameter()
            param1.ParameterName = "@TypeFullName"
            param1.SqlDbType = SqlDbType.NVarChar
            param1.SqlValue = GetType(SimpleWorkflow).ToString()
            cmd.Parameters.Add(param1)

            Dim param2 As SqlParameter = New SqlParameter()
            param2.ParameterName = "@AssemblyFullName"
            param2.SqlDbType = SqlDbType.NVarChar
            param2.SqlValue = GetType(SimpleWorkflow).Assembly.FullName
            cmd.Parameters.Add(param2)


            Dim param3 As SqlParameter = New SqlParameter()
            param3.ParameterName = "@Version"
            param3.SqlDbType = SqlDbType.VarChar
            param3.SqlValue = "1.0.0.0"
            cmd.Parameters.Add(param3)

            Dim param4 As SqlParameter = New SqlParameter()
            param4.ParameterName = "@TrackingProfileXml"
            param4.SqlDbType = SqlDbType.NText
            param4.SqlValue = profile
            cmd.Parameters.Add(param4)

                cmd.Connection.Open()
                cmd.ExecuteNonQuery()
            Catch e As Exception
                Console.WriteLine(e.Message)
                Console.WriteLine("The Tracking Profile Was not Inserted. If You want to add a new one then please increase the version Number" + vbLf)
            Finally
                If cmd IsNot Nothing AndAlso cmd.Connection IsNot Nothing AndAlso ConnectionState.Closed <> cmd.Connection.State Then
                    cmd.Connection.Close()
                End If
            End Try
        End Sub



    Private Shared Sub GetUserTrackingEvents(ByVal workflowInstanceId As Guid)
        Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(connectionString)


            Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
            If sqlTrackingQuery.TryGetWorkflow(workflowInstanceId, sqlTrackingWorkflowInstance) Then
                Dim userTrackingRecord As UserTrackingRecord
                For Each userTrackingRecord In sqlTrackingWorkflowInstance.UserEvents
                    Console.WriteLine(vbLf + "User Tracking Event : Event Date Time : {0}, Event Data : {1}" + vbLf, userTrackingRecord.EventDateTime.ToString(CultureInfo.CurrentCulture), userTrackingRecord.UserData.ToString())
                Next
            Else
                Throw New Exception("\nFailed to retrieve workflow instance\n")
            End If
    End Sub
End Class




