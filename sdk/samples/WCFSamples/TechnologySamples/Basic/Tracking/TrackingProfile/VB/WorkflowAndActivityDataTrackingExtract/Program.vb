' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.Threading
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Tracking
Imports System.Text
Imports System.IO
Imports System.Data
Imports System.Data.SqlClient
Imports System.Globalization
Imports Microsoft.VisualBasic

Namespace Microsoft.Samples.Workflow.WorkflowAndActivityDataTrackingExtract
    Class Program
        Private Shared waitHandle As AutoResetEvent
        Private Const connectionString As String = "Initial Catalog=Tracking;Data Source=localhost\SQLEXPRESS;Integrated Security=SSPI;"

        Shared Sub Main()
            Try
                waitHandle = New AutoResetEvent(False)
                CreateAndInsertTrackingProfile()
                Using runtime As New WorkflowRuntime()
                    Dim trackingService As New SqlTrackingService(connectionString)
                    runtime.AddService(trackingService)
                    runtime.StartRuntime()
                    AddHandler runtime.WorkflowCompleted, AddressOf OnWorkflowCompleted
                    AddHandler runtime.WorkflowTerminated, AddressOf OnWorkflowTerminated
                    AddHandler runtime.WorkflowAborted, AddressOf OnWorkflowAborted

                    Dim instance As WorkflowInstance = runtime.CreateWorkflow(GetType(BankMachineWorkflow))
                    instance.Start()
                    waitHandle.WaitOne()

                    runtime.StopRuntime()

                    OutputTrackedData(instance.InstanceId)
                End Using
            Catch ex As Exception
                If ex.InnerException IsNot Nothing Then
                    Console.WriteLine(ex.InnerException.Message)
                Else
                    Console.WriteLine(ex.Message)
                End If
            End Try
        End Sub

        Private Shared Sub OnWorkflowTerminated(ByVal sender As Object, ByVal e As WorkflowTerminatedEventArgs)
            Console.WriteLine(e.Exception.Message)
            waitHandle.[Set]()
        End Sub


        Private Shared Sub OnWorkflowAborted(ByVal sender As Object, ByVal e As WorkflowEventArgs)
            Console.WriteLine("Workflow has been aborted.")
            waitHandle.[Set]()
        End Sub

        Private Shared Sub OnWorkflowCompleted(ByVal sender As Object, ByVal instance As WorkflowCompletedEventArgs)
            Console.WriteLine("" & vbCrLf & "Your transaction has finished. Thank you for your service.")
            waitHandle.[Set]()
        End Sub

        Private Shared Sub CreateAndInsertTrackingProfile()
            Dim profile As New TrackingProfile()

            ' Create an activity track point, used for tracking data from Code Activities.
            Dim codeActivityTrackPoint As New ActivityTrackPoint()

            ' Create an ActivityTrackingLocation to be added to the track point.
            Dim codeActivityTrackingLocation As New ActivityTrackingLocation("CodeActivity")
            codeActivityTrackingLocation.MatchDerivedTypes = True
            ' Add the location "Closed" event to track.
            codeActivityTrackingLocation.ExecutionStatusEvents.Add(ActivityExecutionStatus.Closed)
            codeActivityTrackPoint.MatchingLocations.Add(codeActivityTrackingLocation)

            ' Create a WorkflowDataTrackingExtract for extracting data from the Balance property.
            Dim balanceWorkflowTrackingExtract As New WorkflowDataTrackingExtract()
            balanceWorkflowTrackingExtract.Member = "Balance"

            ' Create an activity track point, used for tracking data in the custom activity "ServiceCharge".
            Dim customActivityTrackPoint As New ActivityTrackPoint()
            Dim customActivityTrackingLocation As New ActivityTrackingLocation("ServiceCharge")
            ' Create an ActivityTrackingLocation to be added to the track point
            customActivityTrackingLocation.ExecutionStatusEvents.Add(ActivityExecutionStatus.Closed)
            customActivityTrackPoint.MatchingLocations.Add(customActivityTrackingLocation)

            ' Create an ActivityDataTrackingExtract for extracting Fee property data from the ServiceCharge activity.
            Dim feeActivityTrackingExtract As New ActivityDataTrackingExtract()
            feeActivityTrackingExtract.Member = "Fee"

            ' Add extracts to the activity tracking points.
            codeActivityTrackPoint.Extracts.Add(balanceWorkflowTrackingExtract)
            customActivityTrackPoint.Extracts.Add(feeActivityTrackingExtract)

            profile.ActivityTrackPoints.Add(codeActivityTrackPoint)
            profile.ActivityTrackPoints.Add(customActivityTrackPoint)
            profile.Version = New Version("3.0.0.0")

            ' Serialize the profile.
            Dim serializer As New TrackingProfileSerializer()
            Dim writer As New StringWriter(New StringBuilder(), CultureInfo.InvariantCulture)
            serializer.Serialize(writer, profile)
            Dim trackingprofile As String = writer.ToString()
            InsertTrackingProfile(trackingprofile)
        End Sub

        Private Shared Sub InsertTrackingProfile(ByVal profile As String)
            Using command As New SqlCommand()

                command.CommandType = CommandType.StoredProcedure
                command.CommandText = "dbo.UpdateTrackingProfile"
                command.Connection = New SqlConnection(connectionString)

                Using command.Connection
                    Try
                        Dim typeFullName As New SqlParameter()
                        typeFullName.ParameterName = "@TypeFullName"
                        typeFullName.SqlDbType = SqlDbType.NVarChar
                        typeFullName.SqlValue = GetType(BankMachineWorkflow).ToString()
                        command.Parameters.Add(typeFullName)

                        Dim assemblyFullName As New SqlParameter()
                        assemblyFullName.ParameterName = "@AssemblyFullName"
                        assemblyFullName.SqlDbType = SqlDbType.NVarChar
                        assemblyFullName.SqlValue = GetType(BankMachineWorkflow).Assembly.FullName
                        command.Parameters.Add(assemblyFullName)

                        Dim versionId As New SqlParameter()
                        versionId.ParameterName = "@Version"
                        versionId.SqlDbType = SqlDbType.VarChar
                        versionId.SqlValue = "3.0.0.0"
                        command.Parameters.Add(versionId)

                        Dim trackingProfile As New SqlParameter()
                        trackingProfile.ParameterName = "@TrackingProfileXml"
                        trackingProfile.SqlDbType = SqlDbType.NVarChar
                        trackingProfile.SqlValue = profile
                        command.Parameters.Add(trackingProfile)

                        command.Connection.Open()
                        command.ExecuteNonQuery()
                        command.Connection.Close()
                    Catch e As SqlException
                        Console.WriteLine(e.Message)
                        Console.WriteLine("The tracking profile was not inserted. If you wish to add a new tracking profile, please increase the tracking profile's version number.")
                    End Try
                End Using
            End Using
        End Sub


        Private Shared Sub OutputTrackedData(ByVal instanceId As Guid)
            Dim sqlDataTrackingQuery As New SqlTrackingQuery(connectionString)

            Dim sqlTrackingWorkflowInstance As SqlTrackingWorkflowInstance = Nothing
            If sqlDataTrackingQuery.TryGetWorkflow(instanceId, sqlTrackingWorkflowInstance) Then
                Console.WriteLine("" & vbCrLf & "Outputting data tracked from the workflow:" & vbCrLf & "")

                For Each activityDataTrackingRecord As ActivityTrackingRecord In sqlTrackingWorkflowInstance.ActivityEvents
                    For Each dataItem As TrackingDataItem In activityDataTrackingRecord.Body
                        ' Output data queried from TrackingDataItem table in the database.
                        Console.WriteLine("At the {0} event for the {1}:" & vbCrLf & "{2} = {3}", activityDataTrackingRecord.ExecutionStatus, activityDataTrackingRecord.QualifiedName, dataItem.FieldName, dataItem.Data)
                    Next
                Next
            Else
                Console.WriteLine("Could not retrieve data for workflow with instance id {0}", instanceId)
            End If
        End Sub
    End Class
End Namespace
