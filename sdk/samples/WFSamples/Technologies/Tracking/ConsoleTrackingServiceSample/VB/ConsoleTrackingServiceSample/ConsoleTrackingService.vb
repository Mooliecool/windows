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
Imports System.IO
Imports System.Collections.Generic
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Tracking
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime.Hosting
Imports System.Globalization
Imports Microsoft.VisualBasic

Public Class TrackingChannelSample
    Inherits TrackingChannel

    Private parametersValue As TrackingParameters = Nothing

    Protected Sub New()
    End Sub

    Public Sub New(ByVal parameters As TrackingParameters)
        Me.parametersValue = parameters
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

    Private Shared Sub WriteTitle(ByVal title As String)
        Console.WriteLine(vbCrLf + "***********************************************************")
        Console.WriteLine(vbTab + title)
        Console.WriteLine("***********************************************************")
    End Sub

    Private Shared Sub WriteWorkflowTrackingRecord(ByVal workflowTrackingRecord As WorkflowTrackingRecord)
        WriteTitle("Workflow Tracking Record")
        Console.WriteLine("EventDateTime: " + workflowTrackingRecord.EventDateTime.ToString(CultureInfo.CurrentCulture))
        Console.WriteLine("Status: " + workflowTrackingRecord.TrackingWorkflowEvent.ToString())
    End Sub

    Private Shared Sub WriteActivityTrackingRecord(ByVal activityTrackingRecord As ActivityTrackingRecord)
        WriteTitle("Activity Tracking Record")
        Console.WriteLine("EventDateTime: " + activityTrackingRecord.EventDateTime.ToString(CultureInfo.CurrentCulture))
        Console.WriteLine("QualifiedName: " + activityTrackingRecord.QualifiedName.ToString())
        Console.WriteLine("Type: " + activityTrackingRecord.ActivityType.ToString())
        Console.WriteLine("Status: " + activityTrackingRecord.ExecutionStatus.ToString())
    End Sub

    Private Shared Sub WriteUserTrackingRecord(ByVal userTrackingRecord As UserTrackingRecord)
        WriteTitle("User Activity Record")
        Console.WriteLine("EventDateTime: " + userTrackingRecord.EventDateTime.ToString(CultureInfo.CurrentCulture))
        Console.WriteLine("QualifiedName: " + userTrackingRecord.QualifiedName.ToString())
        Console.WriteLine("ActivityType: " + userTrackingRecord.ActivityType.FullName.ToString())
        Console.WriteLine("Args: " + userTrackingRecord.UserData.ToString())
    End Sub
End Class

Public Class ConsoleTrackingService
    Inherits TrackingService

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
        ' Returning false to indicate there is no new profiles
        profile = Nothing
        Return False
    End Function

    Protected Overrides Function GetTrackingChannel(ByVal parameters As TrackingParameters) As TrackingChannel
        ' return a tracking channel to receive runtime events
        Return New TrackingChannelSample(parameters)
    End Function

    ' Profile creation
    Private Overloads Shared Function GetProfile() As TrackingProfile
        ' Create a Tracking Profile
        Dim profile As TrackingProfile = New TrackingProfile()
        profile.Version = New Version("3.0.0")

        ' Add a TrackPoint to cover all activity status events
        Dim activityTrackPoint As ActivityTrackPoint = New ActivityTrackPoint()
        Dim activityLocation As ActivityTrackingLocation = New ActivityTrackingLocation(GetType(Activity))
        activityLocation.MatchDerivedTypes = True
        Dim statuses As IEnumerable(Of ActivityExecutionStatus) = CType(System.Enum.GetValues(GetType(ActivityExecutionStatus)), IEnumerable(Of ActivityExecutionStatus))
        For Each status As ActivityExecutionStatus In statuses
            activityLocation.ExecutionStatusEvents.Add(status)
        Next
        activityTrackPoint.MatchingLocations.Add(activityLocation)
        profile.ActivityTrackPoints.Add(activityTrackPoint)

        ' Add a TrackPoint to cover all workflow status events
        Dim workflowTrackPoint As WorkflowTrackPoint = New WorkflowTrackPoint()
        workflowTrackPoint.MatchingLocation = New WorkflowTrackingLocation()
        For Each workflowEvent As TrackingWorkflowEvent In System.Enum.GetValues(GetType(TrackingWorkflowEvent))
            workflowTrackPoint.MatchingLocation.Events.Add(workflowEvent)
        Next
        profile.WorkflowTrackPoints.Add(workflowTrackPoint)

        ' Add a TrackPoint to cover all user track points
        Dim userTrackPoint As UserTrackPoint = New UserTrackPoint()
        Dim userLocation As UserTrackingLocation = New UserTrackingLocation()
        userLocation.ActivityType = GetType(Activity)
        userLocation.MatchDerivedActivityTypes = True
        userLocation.ArgumentType = GetType(Object)
        userLocation.MatchDerivedArgumentTypes = True
        userTrackPoint.MatchingLocations.Add(userLocation)
        profile.UserTrackPoints.Add(userTrackPoint)

        Return profile
    End Function
End Class