'----------------------------------------------------------------------
'   This file is part of the Windows Workflow Foundation SDK Code Samples.
'  
'   Copyright (C) Microsoft Corporation.  All rights reserved.
'  
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
'  
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'----------------------------------------------------------------------

Imports System
Imports System.IO
Imports System.Workflow.Runtime.Tracking
Imports System.Xml.Schema

Public Class SimpleTrackingChannel
    Inherits TrackingChannel

    Public Shared ReadOnly TrackingDataFilePrefix As String = Environment.GetEnvironmentVariable("TEMP") + "\Tracking_"

    Private trackingParameters As TrackingParameters = Nothing
    Private Shared trackingDataFile As String = ""
    Private streamWriter As StreamWriter = Nothing

    Protected Sub New()
    End Sub

    Public Sub New(ByVal parameters As TrackingParameters)
        trackingParameters = parameters
        trackingDataFile = TrackingDataFilePrefix + parameters.InstanceId.ToString() + ".txt"
        Console.WriteLine("Saving tracking information to " + trackingDataFile)
        streamWriter = File.CreateText(trackingDataFile)
        streamWriter.AutoFlush = True
    End Sub

    ' Send() is called by Tracking runtime to send various tracking records
    Protected Overrides Sub Send(ByVal record As TrackingRecord)
        'filter on record type
        If TypeOf record Is WorkflowTrackingRecord Then
            WriteWorkflowTrackingRecord(CType(record, WorkflowTrackingRecord))
        End If
        If TypeOf record Is ActivityTrackingRecord Then
            WriteActivityTrackingRecord(CType(record, ActivityTrackingRecord))
        End If
        If TypeOf record Is UserTrackingRecord Then
            WriteUserTrackingRecord(CType(record, UserTrackingRecord))
        End If
    End Sub

    ' InstanceCompletedOrTerminated() is called by Tracking runtime to indicate that the Workflow instance finished running
    Protected Overrides Sub InstanceCompletedOrTerminated()
        WriteTitle("Workflow Instance Completed or Terminated")
    End Sub


    Private Sub WriteTitle(ByVal title As String)
        WriteToFile("**** " + title + " ****")
    End Sub


    Private Sub WriteWorkflowTrackingRecord(ByVal workflowTrackingRecord As WorkflowTrackingRecord)
        WriteToFile("Workflow: " + workflowTrackingRecord.TrackingWorkflowEvent.ToString())
    End Sub

    Private Sub WriteActivityTrackingRecord(ByVal activityTrackingRecord As ActivityTrackingRecord)
        WriteToFile("Activity: " + activityTrackingRecord.QualifiedName.ToString() + " " + activityTrackingRecord.ExecutionStatus.ToString())
    End Sub

    Private Sub WriteUserTrackingRecord(ByVal userTrackingRecord As UserTrackingRecord)
        WriteToFile("User Data: " + userTrackingRecord.UserData.ToString())
    End Sub

    Private Sub WriteToFile(ByVal toWrite As String)
        Try
            If streamWriter IsNot Nothing Then
                streamWriter.WriteLine(toWrite)
            Else
                Throw New Exception("trackingDataFile " + trackingDataFile + " doesn't exist")
            End If
        Catch e As Exception
            Console.WriteLine("Encountered an exception. Exception Source: {0}, Exception Message: {1} ", e.Source, e.Message)
        End Try
    End Sub
End Class


Public Class SimpleFileTrackingService
    Inherits TrackingService
    Public Sub New()

    End Sub

    Protected Overrides Function TryGetProfile(ByVal workflowType As Type, ByRef profile As TrackingProfile) As Boolean
        'Depending on the workflowType, service can return different tracking profiles
        'In this sample we're returning the same profile for all running types
        profile = GetProfile()
        Return True
    End Function

    Protected Overrides Function GetProfile(ByVal workflowInstanceId As Guid) As TrackingProfile
        ' Does not support reloading/instance profiles
        Throw New NotImplementedException("The method or operation is not implemented.")
    End Function

    Protected Overrides Function GetProfile(ByVal workflowType As Type, ByVal profileVersionId As Version) As TrackingProfile
        ' Return the version of the tracking profile that runtime requests (profileVersionId)
        Return GetProfile()
    End Function

    Protected Overrides Function TryReloadProfile(ByVal workflowType As Type, ByVal workflowInstanceId As Guid, ByRef profile As TrackingProfile) As Boolean      
        ' Returning false to indicate there are no new profiles
        profile = Nothing
        Return False
    End Function

    Protected Overrides Function GetTrackingChannel(ByVal parameters As TrackingParameters) As TrackingChannel
        Return New SimpleTrackingChannel(parameters)
    End Function

#Region "Tracking Profile Creation"

    ' Reads a file containing an XML representation of a Tracking Profile
    Private Overloads Shared Function GetProfile() As TrackingProfile
        Dim fileStream As FileStream = Nothing
        Try
            Dim trackingProfileFile As String = Environment.CurrentDirectory + "\profile.xml"
            Console.WriteLine("trackingProfileFile is {0}", trackingProfileFile)

            If File.Exists(trackingProfileFile) Then
                Console.WriteLine("Reading trackingProfile from {0}", trackingProfileFile)
                fileStream = File.OpenRead(trackingProfileFile)
                If fileStream Is Nothing Then
                    Console.WriteLine("fileStream is null")
                    Return Nothing
                End If

                Dim reader As New StreamReader(fileStream)
                Dim profile As TrackingProfile
                Dim trackingProfileSerializer As New TrackingProfileSerializer()
                profile = trackingProfileSerializer.Deserialize(reader)
                Return profile
            Else
                Console.WriteLine("trackingProfileFile {0} doesn't exist", trackingProfileFile)
                Return Nothing
            End If

        Catch tpex As TrackingProfileDeserializationException
            Console.WriteLine("Encountered a deserialization exception.")

            For Each validationError As ValidationEventArgs In tpex.ValidationEventArgs
                Console.WriteLine("Exception Message: {0}", validationError.Message)
            Next

            Return Nothing
        Catch ex As Exception
            Console.WriteLine("Encountered an exception. Exception Source: {0}, Exception Message: {1}", ex.Source, ex.Message)
            Return Nothing
        Finally

            If fileStream IsNot Nothing Then
                fileStream.Close()
            End If

        End Try

    End Function
#End Region

End Class
