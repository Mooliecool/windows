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

Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.Data
Imports System.Data.SqlClient
Imports System.IO
Imports System.Globalization
Imports System.Text
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime.Tracking


Public NotInheritable Class DataAccess

    Private Shared version As Version = GetTrackingProfileVersion(New Version("3.0.0.0"))
    Friend Const connectionString As String = "Initial Catalog=Tracking;Data Source=localhost;Integrated Security=SSPI"

    Private Sub New()
        ' Static invocation only
    End Sub

    Friend Shared Sub CreateAndInsertTrackingProfile()
        Dim profile As New TrackingProfile()

        Dim activityTrack As New ActivityTrackPoint()
        Dim activityLocation As New ActivityTrackingLocation(GetType(Activity))

        activityLocation.MatchDerivedTypes = True
        Dim statuses As IEnumerable(Of ActivityExecutionStatus) = TryCast(System.Enum.GetValues(GetType(ActivityExecutionStatus)), IEnumerable(Of ActivityExecutionStatus))
        For Each status As ActivityExecutionStatus In statuses
            activityLocation.ExecutionStatusEvents.Add(status)
        Next



        activityTrack.MatchingLocations.Add(activityLocation)
        profile.ActivityTrackPoints.Add(activityTrack)
        profile.Version = version

        Dim workflowTrack As New WorkflowTrackPoint()
        Dim workflowLocation As New WorkflowTrackingLocation()
        Dim eventStatuses As IEnumerable(Of TrackingWorkflowEvent) = TryCast(System.Enum.GetValues(GetType(TrackingWorkflowEvent)), IEnumerable(Of TrackingWorkflowEvent))

        For Each status As TrackingWorkflowEvent In eventStatuses
            workflowLocation.Events.Add(status)
        Next

        workflowTrack.MatchingLocation = workflowLocation
        profile.WorkflowTrackPoints.Add(workflowTrack)

        Dim serializer As New TrackingProfileSerializer()
        Dim writer As New StringWriter(New StringBuilder(), CultureInfo.InvariantCulture)
        serializer.Serialize(writer, profile)
        Dim trackingprofile As String = writer.ToString()
        InsertTrackingProfile(trackingprofile)
    End Sub


    Private Shared Sub InsertTrackingProfile(ByVal profile As String)

        Using Command As New SqlCommand()

            Command.CommandType = CommandType.StoredProcedure
            Command.CommandText = "dbo.UpdateTrackingProfile"
            Command.Connection = New SqlConnection(connectionString)
            Try
                Dim typeFullName As New SqlParameter()
                typeFullName.ParameterName = "@TypeFullName"
                typeFullName.SqlDbType = SqlDbType.NVarChar
                typeFullName.SqlValue = GetType(SimpleWorkflow).ToString()
                Command.Parameters.Add(typeFullName)

                Dim assemblyFullName As New SqlParameter()
                assemblyFullName.ParameterName = "@AssemblyFullName"
                assemblyFullName.SqlDbType = SqlDbType.NVarChar
                assemblyFullName.SqlValue = GetType(SimpleWorkflow).Assembly.FullName
                Command.Parameters.Add(assemblyFullName)

                Dim versionId As New SqlParameter()
                versionId.ParameterName = "@Version"
                versionId.SqlDbType = SqlDbType.VarChar
                versionId.SqlValue = version.ToString()
                Command.Parameters.Add(versionId)

                Dim TrackingProfile As New SqlParameter()
                TrackingProfile.ParameterName = "@TrackingProfileXml"
                TrackingProfile.SqlDbType = SqlDbType.NVarChar
                TrackingProfile.SqlValue = profile
                Command.Parameters.Add(TrackingProfile)

                Command.Connection.Open()
                Command.ExecuteNonQuery()

            Catch e As SqlException

                Console.WriteLine(e.Message)
                Console.WriteLine("The tracking profile was not inserted. If you wish to add a new tracking profile, please increase the tracking profile's version number.")
            End Try

        End Using

    End Sub


    Private Shared Function GetTrackingProfileVersion(ByVal version As Version) As Version

        Dim profile As TrackingProfile = Nothing
        Dim reader As SqlDataReader = Nothing
        Using Command As New SqlCommand()
            Command.CommandType = CommandType.StoredProcedure
            Command.CommandText = "dbo.GetTrackingProfile"
            Command.Connection = New SqlConnection(connectionString)

            Dim typeFullName As New SqlParameter()
            typeFullName.ParameterName = "@TypeFullName"
            typeFullName.SqlDbType = SqlDbType.NVarChar
            typeFullName.SqlValue = GetType(SimpleWorkflow).FullName
            Command.Parameters.Add(typeFullName)

            Dim assemblyFullName As New SqlParameter()
            assemblyFullName.ParameterName = "@AssemblyFullName"
            assemblyFullName.SqlDbType = SqlDbType.NVarChar
            assemblyFullName.SqlValue = GetType(SimpleWorkflow).Assembly.FullName
            Command.Parameters.Add(assemblyFullName)

            Dim versionId As New SqlParameter()
            versionId.ParameterName = "@Version"
            versionId.SqlDbType = SqlDbType.VarChar
            Command.Parameters.Add(versionId)

            Dim createDefault As New SqlParameter()
            createDefault.ParameterName = "@CreateDefault"
            createDefault.SqlDbType = SqlDbType.Bit
            createDefault.SqlValue = 0
            Command.Parameters.Add(createDefault)

            Command.Connection.Open()
            reader = Command.ExecuteReader()
            If reader.Read() Then

                Dim profileXml As String = TryCast(reader(0), String)
                If profileXml IsNot Nothing Then

                    Dim serializer As New TrackingProfileSerializer()
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
        End Using

        If (profile IsNot Nothing) Then
            Return New Version(profile.Version.Major, profile.Version.MajorRevision, profile.Version.Minor, profile.Version.MinorRevision + 1)
        Else
            Return New Version(version.Major, version.MajorRevision, version.Minor, version.MinorRevision + 1)
        End If
    End Function

    Friend Shared Sub GetWorkflowTrackingEvents(ByVal instanceId As Guid)

        Dim SqlTrackingQuery As New SqlTrackingQuery(connectionString)

        Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
        If SqlTrackingQuery.TryGetWorkflow(instanceId, sqlTrackingWorkflowInstance) Then
            Console.WriteLine(vbCrLf + "Instance Level Events:" + vbCrLf)

            For Each workflowTrackingRecord As WorkflowTrackingRecord In sqlTrackingWorkflowInstance.WorkflowEvents
                Console.WriteLine("EventDescription : {0}  DateTime : {1}", workflowTrackingRecord.TrackingWorkflowEvent, workflowTrackingRecord.EventDateTime)
            Next
        End If
    End Sub



    Friend Shared Sub SetPartitionInterval(ByVal interval As Char)
        ' Valid values are 'd' (daily), 'm' (monthly), and 'y' (yearly).  The default is 'm'.
        Using Command As New SqlCommand("dbo.SetPartitionInterval")
            Command.CommandType = CommandType.StoredProcedure
            Command.Connection = New SqlConnection(connectionString)

            Dim intervalParameter As New SqlParameter("@Interval", SqlDbType.Char)
            intervalParameter.SqlValue = interval
            Command.Parameters.Add(intervalParameter)

            Command.Connection.Open()
            Command.ExecuteNonQuery()

        End Using

    End Sub


    Friend Shared Sub ShowTrackingPartitionInformation()

        'Show the contents of the TrackingPartitionName table
        Using command As New SqlCommand("SELECT * FROM vw_TrackingPartitionSetName")
            Dim reader As SqlDataReader = Nothing
            command.CommandType = CommandType.Text
            command.Connection = New SqlConnection(connectionString)

            Try
                command.Connection.Open()
                reader = command.ExecuteReader()
                If (reader.Read()) Then
                    Console.WriteLine()
                    Console.WriteLine("***************************")
                    Console.WriteLine("Partition information: ")
                    Console.WriteLine("PartitionId: {0}", reader(0))
                    Console.WriteLine("Name: {0}", reader(1))
                    Console.WriteLine("Created: {0}", reader(2))
                    Console.WriteLine("End: {0}", IIf(TypeOf (reader(3)) Is System.DBNull, "NULL", reader(3)))
                    Console.WriteLine("Partition Interval: {0}", reader(4))
                    Console.WriteLine("***************************")
                Else
                    Console.WriteLine()
                    Console.WriteLine("No partition information present.")
                End If
            Finally
                If reader IsNot Nothing And Not reader.IsClosed Then
                    reader.Close()
                End If
            End Try
        End Using

    End Sub

    Friend Shared Sub ShowPartitionTableInformation()

        'List the names of the partition tables created

        Using command As New SqlCommand( _
            "declare @trackingName varchar(255) select @trackingName = Name from TrackingPartitionSetName " + _
            "select name from sysobjects where name like '%' + @trackingName")

            Console.WriteLine()
            Console.WriteLine("***************************")
            Console.WriteLine("Partition tables: ")
            Dim reader As SqlDataReader = Nothing
            command.CommandType = CommandType.Text
            command.Connection = New SqlConnection(connectionString)

            Try

                command.Connection.Open()
                reader = command.ExecuteReader()
                While reader.Read()
                    Console.WriteLine(reader(0))
                End While
            Finally
                If reader IsNot Nothing And Not reader.IsClosed Then
                    reader.Close()
                End If
                Console.WriteLine("***************************")
            End Try
        End Using

    End Sub
End Class







