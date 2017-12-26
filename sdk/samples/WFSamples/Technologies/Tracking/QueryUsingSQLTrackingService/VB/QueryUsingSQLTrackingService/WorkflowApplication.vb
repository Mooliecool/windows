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
Imports System.Workflow.Runtime.Tracking
Imports System.Workflow.ComponentModel
Imports System.IO
Imports System.Data
Imports System.Data.SqlClient
Imports System.Globalization
Imports Microsoft.VisualBasic

Class WorkflowApplication
    Const connectionString As String = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI;"
    Shared WaitHandle As New AutoResetEvent(False)
    Shared version As Version

    Shared Sub Main()
        Try
            version = GetTrackingProfileVersion(New Version("3.0.0.0"))
            CreateAndInsertTrackingProfile()

            Using currentWorkflowRuntime As New WorkflowRuntime()
                currentWorkflowRuntime.AddService(New SqlTrackingService(connectionString))
                currentWorkflowRuntime.StartRuntime()

                AddHandler currentWorkflowRuntime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                AddHandler currentWorkflowRuntime.WorkflowTerminated, AddressOf OnWorkflowTerminated

                Dim type As System.Type = GetType(SimpleWorkflow)
                Dim instance As WorkflowInstance = currentWorkflowRuntime.CreateWorkflow(type)
                instance.Start()

                WaitHandle.WaitOne()

                currentWorkflowRuntime.StopRuntime()

                OutputWorkflowTrackingEvents(instance.InstanceId)
                OutputActivityTrackingEvents(instance.InstanceId)

                Console.WriteLine(vbLf + "Done Running The workflow.")
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
        WaitHandle.Set()
    End Sub

    Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
        Console.WriteLine(e.Exception.Message)
        WaitHandle.Set()
    End Sub

    Shared Sub CreateAndInsertTrackingProfile()
        Dim profile As TrackingProfile = New TrackingProfile()

        Dim activityTrack As ActivityTrackPoint = New ActivityTrackPoint()
        Dim activityLocation As ActivityTrackingLocation = New ActivityTrackingLocation(GetType(Activity))
        activityLocation.MatchDerivedTypes = True
        Dim statuses As IEnumerable(Of ActivityExecutionStatus) = CType(System.Enum.GetValues(GetType(ActivityExecutionStatus)), IEnumerable(Of ActivityExecutionStatus))
        For Each status As ActivityExecutionStatus In statuses
            activityLocation.ExecutionStatusEvents.Add(status)
        Next

        activityTrack.MatchingLocations.Add(activityLocation)
        profile.ActivityTrackPoints.Add(activityTrack)
        profile.Version = version

        Dim workflowTrack As WorkflowTrackPoint = New WorkflowTrackPoint()
        Dim workflowLocation As WorkflowTrackingLocation = New WorkflowTrackingLocation()
        Dim eventStatuses As IEnumerable(Of TrackingWorkflowEvent) = CType(System.Enum.GetValues(GetType(TrackingWorkflowEvent)), IEnumerable(Of TrackingWorkflowEvent))
        For Each status As TrackingWorkflowEvent In eventStatuses
            workflowLocation.Events.Add(status)
        Next

        workflowTrack.MatchingLocation = workflowLocation
        profile.WorkflowTrackPoints.Add(workflowTrack)

        Dim serializer As TrackingProfileSerializer = New TrackingProfileSerializer()
        Dim writer As StringWriter = New StringWriter(New StringBuilder(), CultureInfo.InvariantCulture)
        serializer.Serialize(writer, profile)
        Dim trackingProfile As String = writer.ToString()
        InsertTrackingProfile(trackingProfile)
    End Sub


        Shared Sub InsertTrackingProfile(ByVal profile As String)
            Using command As New SqlCommand()


                command.CommandType = CommandType.StoredProcedure
                command.CommandText = "dbo.UpdateTrackingProfile"
                command.Connection = New SqlConnection(connectionString)



                Try
                    Dim typeFullName As SqlParameter = New SqlParameter()
                    typeFullName.ParameterName = "@TypeFullName"
                    typeFullName.SqlDbType = SqlDbType.NVarChar
                    typeFullName.SqlValue = GetType(SimpleWorkflow).ToString()
                    command.Parameters.Add(typeFullName)



                    Dim assemblyFullName As SqlParameter = New SqlParameter()
                    assemblyFullName.ParameterName = "@AssemblyFullName"
                    assemblyFullName.SqlDbType = SqlDbType.NVarChar
                    assemblyFullName.SqlValue = GetType(SimpleWorkflow).Assembly.FullName
                    command.Parameters.Add(assemblyFullName)



                    Dim versionId As SqlParameter = New SqlParameter()
                    versionId.ParameterName = "@Version"
                    versionId.SqlDbType = SqlDbType.VarChar
                    versionId.SqlValue = version.ToString()
                    command.Parameters.Add(versionId)



                    Dim trackingProfile As SqlParameter = New SqlParameter()
                    trackingProfile.ParameterName = "@TrackingProfileXml"
                    trackingProfile.SqlDbType = SqlDbType.NVarChar
                    trackingProfile.SqlValue = profile
                    command.Parameters.Add(trackingProfile)



                    command.Connection.Open()
                    command.ExecuteNonQuery()
                Catch e As SqlException
                    Console.WriteLine(e.Message)
                    Console.WriteLine("The tracking profile was not inserted. If you wish to add a new tracking profile, please increase the tracking profile's version number.")
                End Try

            End Using

        End Sub



        Private Shared Function GetTrackingProfileVersion(ByVal version As Version) As Version
            Dim profile As TrackingProfile = Nothing
            Dim reader As SqlDataReader = Nothing
            Using command As New SqlCommand()
                command.CommandType = CommandType.StoredProcedure
                command.CommandText = "dbo.GetTrackingProfile"
                command.Connection = New SqlConnection(connectionString)

                Dim typeFullName As SqlParameter = New SqlParameter()
                typeFullName.ParameterName = "@TypeFullName"
                typeFullName.SqlDbType = SqlDbType.NVarChar
                typeFullName.SqlValue = GetType(SimpleWorkflow).FullName
                command.Parameters.Add(typeFullName)

            Dim assemblyFullName As SqlParameter = New SqlParameter()
            assemblyFullName.ParameterName = "@AssemblyFullName"
            assemblyFullName.SqlDbType = SqlDbType.NVarChar
            assemblyFullName.SqlValue = GetType(SimpleWorkflow).Assembly.FullName
            command.Parameters.Add(assemblyFullName)

            Dim versionId As SqlParameter = New SqlParameter()
            versionId.ParameterName = "@Version"
            versionId.SqlDbType = SqlDbType.VarChar
            command.Parameters.Add(versionId)

            Dim createDefault As SqlParameter = New SqlParameter()
            createDefault.ParameterName = "@CreateDefault"
            createDefault.SqlDbType = SqlDbType.Bit
            createDefault.SqlValue = 0
            command.Parameters.Add(createDefault)


                Try
                    command.Connection.Open()
                    reader = command.ExecuteReader()
                    If reader.Read() Then
                        Dim profileXml As String = CType(reader(0), String)
                        If Nothing <> profileXml Then
                            Dim serializer As TrackingProfileSerializer = New TrackingProfileSerializer()
                            Dim stringReader As StringReader = Nothing
                            Try
                                stringReader = New StringReader(profileXml)
                                profile = serializer.Deserialize(stringReader)
                            Finally
                                If stringReader IsNot Nothing Then
                                    stringReader.Close()
                                End If
                            End Try
                        End If
                    End If
                Finally
                    If reader IsNot Nothing AndAlso Not reader.IsClosed Then
                        reader.Close()
                    End If



                End Try


            End Using

            If Not profile Is Nothing Then
                Return New Version(profile.Version.Major, profile.Version.MajorRevision, profile.Version.Minor, profile.Version.MinorRevision + 1)
            Else
                Return New Version(version.Major, version.MajorRevision, version.Minor, version.MinorRevision + 1)
            End If
        End Function


        Shared Sub OutputWorkflowTrackingEvents(ByVal instanceId As Guid)
            Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(connectionString)


        Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
        sqlTrackingQuery.TryGetWorkflow(instanceId, sqlTrackingWorkflowInstance)

        Console.WriteLine(vbCrLf + "Instance Level Events:" + vbCrLf)

        Dim workflowTrackingRecord As WorkflowTrackingRecord
        For Each workflowTrackingRecord In sqlTrackingWorkflowInstance.WorkflowEvents
            Console.WriteLine("EventDescription : {0}  DateTime : {1}", workflowTrackingRecord.TrackingWorkflowEvent, workflowTrackingRecord.EventDateTime)
        Next
    End Sub


        Shared Sub OutputActivityTrackingEvents(ByVal instanceId As Guid)
            Dim sqlTrackingQuery As SqlTrackingQuery = New SqlTrackingQuery(connectionString)


        Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
        sqlTrackingQuery.TryGetWorkflow(instanceId, sqlTrackingWorkflowInstance)

        Console.WriteLine(vbCrLf + "Activity Tracking Events:" + vbCrLf)


            Dim activityTrackingRecord As ActivityTrackingRecord
            For Each activityTrackingRecord In sqlTrackingWorkflowInstance.ActivityEvents
                Console.WriteLine("StatusDescription : {0}  DateTime : {1} Activity Qualified Name : {2}", activityTrackingRecord.ExecutionStatus, activityTrackingRecord.EventDateTime, activityTrackingRecord.QualifiedName)
            Next
        End Sub
End Class



