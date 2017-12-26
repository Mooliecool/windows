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
Imports System.Workflow.Runtime.Tracking
Imports System.Workflow.ComponentModel
Imports System.Workflow.Activities.Rules

Public Class TrackingChannelSample
    Inherits TrackingChannel

    Dim parameters As TrackingParameters = Nothing

    Protected Sub New()
    End Sub
        

    Public Sub New(ByVal parameters As TrackingParameters)
        Me.parameters = parameters
    End Sub

    ' Send() is called by Tracking runtime to send various tracking records
    Protected Overrides Sub Send(ByVal record As TrackingRecord)
        'filter on record type
        If (TypeOf record Is UserTrackingRecord) Then
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

    Private Shared Sub WriteUserTrackingRecord(ByVal userTrackingRecord As UserTrackingRecord)
        WriteTitle("User Activity Record")
        Console.WriteLine("EventDataTime: " + userTrackingRecord.EventDateTime.ToString())
        Console.WriteLine("QualifiedId: " + userTrackingRecord.QualifiedName.ToString())
        Console.WriteLine("ActivityType: " + userTrackingRecord.ActivityType.FullName.ToString())
        If TypeOf userTrackingRecord.UserData Is RuleActionTrackingEvent Then
            WriteRuleActionTrackingEvent(CType(userTrackingRecord.UserData, RuleActionTrackingEvent))
        End If
    End Sub

    Private Shared Sub WriteRuleActionTrackingEvent(ByVal ruleActionTrackingEvent As RuleActionTrackingEvent)
        Console.WriteLine("RuleActionTrackingEvent")
        Console.WriteLine("***********************")
        Console.WriteLine("RuleName: " + ruleActionTrackingEvent.RuleName.ToString())
        Console.WriteLine("ConditionResult: " + ruleActionTrackingEvent.ConditionResult.ToString())
    End Sub
End Class


Public Class ConsoleTrackingService
    Inherits TrackingService

    Public Sub New()
    End Sub


    Protected Overrides Function TryGetProfile(ByVal workflowType As Type, ByRef profile As TrackingProfile) As Boolean

        'Retrieves the tracking profile for the specified workflow type if one is available.
        'Depending on the workflowType, service can return different tracking profiles.
        'In this sample, the same profile is returned for all running types.
        profile = GetProfile()
        Return True
    End Function


    Protected Overrides Function GetProfile(ByVal workflowInstanceId As Guid) As TrackingProfile
        ' Returns the tracking profile for the specified workflow instance. 
        ' This sample does not support reloading/instance profiles.
        Throw New NotImplementedException("The method or operation is not implemented.")
    End Function

    Protected Overrides Function GetProfile(ByVal workflowType As Type, ByVal profileVersionId As Version) As TrackingProfile
        'Returns the tracking profile for the specified workflow type, qualified by version.
        'In this sample, profiles are not modified.
        Return GetProfile()
    End Function

    Protected Overrides Function TryReloadProfile(ByVal workflowType As Type, ByVal workflowInstanceId As Guid, ByRef profile As TrackingProfile) As Boolean
        ' Retrieves a new tracking profile for the specified workflow instance 
        ' if the tracking profile has changed since it was last loaded. 
        ' Returning false to indicate there are no new profiles.
        profile = Nothing
        Return False
    End Function


    Protected Overrides Function GetTrackingChannel(ByVal parameters As TrackingParameters) As TrackingChannel
        ' Returns the channel that the runtime tracking infrastructure uses 
        ' to send tracking records to the tracking service.
        ' For this sample, a channel is returned that dumps the tracking record
        ' to the command window using Console.WriteLine().
        Return New TrackingChannelSample(parameters)
    End Function

    ' Profile creation
    Private Overloads Shared Function GetProfile() As TrackingProfile
        ' Create a Tracking Profile that covers all user track points
        Dim profile As New TrackingProfile()
        profile.Version = New Version("1.0.0")

        ' Add a TrackPoint to cover all user track points.
        ' We want to receive user events generated by any Activity, with any argument type.
        ' We could restrict the ActivityType to be PolicyActivity and 
        ' ArgumentType to be RuleActionTrackingEvent if we wanted to only get 
        ' tracking events from policy execution.
        Dim UserTrackPoint As New UserTrackPoint()
        Dim userLocation As New UserTrackingLocation()
        userLocation.ActivityType = GetType(Activity)
        userLocation.MatchDerivedActivityTypes = True
        userLocation.ArgumentType = GetType(Object)
        userLocation.MatchDerivedArgumentTypes = True
        UserTrackPoint.MatchingLocations.Add(userLocation)
        profile.UserTrackPoints.Add(UserTrackPoint)

        Return profile
    End Function
End Class
