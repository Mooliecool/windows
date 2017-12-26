'---------------------------------------------------------------------
'  This file is part of the Windows Workflow Foundation SDK Code Samples.
' 
'  Copyright (C) Microsoft Corporation.  All rights reserved.
' 
' This source code is intended only as a supplement to Microsoft
' Development Tools and/or on-line documentation.  See these other
' materials for detailed information regarding Microsoft code samples.
' 
' THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
' KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
' IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
' PARTICULAR PURPOSE.
'---------------------------------------------------------------------

Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.Globalization
Imports System.IO
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime
Imports System.Workflow.Runtime.Tracking
Imports System.Text

Class Program
    Shared Sub Main()
        WriteTitle("Extract Name off all events of activityName except compensating event and annotate them")
        SerializeTrackingProfileAndWriteToConsole(GetProfileWithWorkflowDataExtract())

        WriteTitle("Extract all user track points")
        SerializeTrackingProfileAndWriteToConsole(GetProfileWithUserTrackPoint())

        WriteTitle("Extract all workflow events")
        SerializeTrackingProfileAndWriteToConsole(GetProfileAllWorkflowEvents())

        WriteTitle("Deserialize TrackingProfile String")
        DeserializeTrackingProfileString()
    End Sub

    Shared Function GetProfileWithWorkflowDataExtract() As TrackingProfile
        ' Create Tracking Profile
        Dim trackingProfile As New TrackingProfile()
        trackingProfile.Version = New Version("1.0.0")

        ' Create Activity Track Point
        Dim activityTrackPoint As New ActivityTrackPoint()

        ' Create matchingActivityTrackingLocation 
        Dim matchingActivityTrackingLocation As New ActivityTrackingLocation()
        ' Set ActivityTypeName = "activityName"
        matchingActivityTrackingLocation.ActivityTypeName = "activityName"
        ' Add all possible ActivityExecutionStatus to the matchingActivityTrackingLocation Events
        Dim statuses As IEnumerable(Of ActivityExecutionStatus) = CType(System.Enum.GetValues(GetType(ActivityExecutionStatus)), IEnumerable(Of ActivityExecutionStatus))
        For Each status As ActivityExecutionStatus In statuses
            matchingActivityTrackingLocation.ExecutionStatusEvents.Add(status)
        Next
        'Create matchingActivityTrackingCondition where memberName = memberValue
        Dim matchingActivityTrackingCondition As New ActivityTrackingCondition("memberName", "memberValue")
        matchingActivityTrackingCondition.Operator = ComparisonOperator.Equals

        ' Add matchingActivityTrackingCondition to activityTrackingLocation
        matchingActivityTrackingLocation.Conditions.Add(matchingActivityTrackingCondition)

        ' Add matchingActivityTrackingCondition to the matching locations for activityTrackPoint
        activityTrackPoint.MatchingLocations.Add(matchingActivityTrackingLocation)

        ' Create excludedActivityTrackingLocation 
        Dim excludedActivityTrackingLocation As New ActivityTrackingLocation()
        ' Set ActivityTypeName = "activityName"
        excludedActivityTrackingLocation.ActivityTypeName = "activityName"
        ' Add Compensating ActivityExecutionStatus to the excludedActivityTrackingLocation Events
        excludedActivityTrackingLocation.ExecutionStatusEvents.Add(ActivityExecutionStatus.Compensating)

        ' Add excludedActivityTrackingCondition to the excluded locations for activityTrackPoint
        activityTrackPoint.ExcludedLocations.Add(excludedActivityTrackingLocation)

        ' Create workflowDataTrackingExtract to extract Workflow data "Name"
        Dim workflowDataTrackingExtract As New WorkflowDataTrackingExtract()
        workflowDataTrackingExtract.Member = "Name"

        ' Add workflowDataTrackingExtract to activityTrackPoint
        activityTrackPoint.Extracts.Add(workflowDataTrackingExtract)

        ' Annotate activityTrackPoint with 
        activityTrackPoint.Annotations.Add("Track Point Annotations")

        ' Add ActivityTrackPoints to trackingProfile 
        trackingProfile.ActivityTrackPoints.Add(activityTrackPoint)

        Return trackingProfile
    End Function

    Shared Function GetProfileWithUserTrackPoint() As TrackingProfile

        Dim trackingProfile As New TrackingProfile()
        trackingProfile.Version = New Version("1.0.0")

        ' Add a TrackPoint to cover all user track points
        Dim userTrackPoint As New UserTrackPoint()
        Dim userLocation As New UserTrackingLocation()
        userLocation.ActivityType = GetType(Activity)
        userLocation.MatchDerivedActivityTypes = True
        userLocation.ArgumentType = GetType(Object)
        userLocation.MatchDerivedArgumentTypes = True
        userTrackPoint.MatchingLocations.Add(userLocation)
        trackingProfile.UserTrackPoints.Add(userTrackPoint)

        Return trackingProfile
    End Function

    Shared Function GetProfileAllWorkflowEvents() As TrackingProfile
        Dim trackingProfile As New TrackingProfile()
        trackingProfile.Version = New Version("1.0.0")

        ' Add a TrackPoint to cover all user track points
        Dim workflowTrackPoint As New WorkflowTrackPoint()
        Dim statuses As IEnumerable(Of TrackingWorkflowEvent) = CType(System.Enum.GetValues(GetType(TrackingWorkflowEvent)), IEnumerable(Of TrackingWorkflowEvent))
        For Each status As TrackingWorkflowEvent In statuses
            workflowTrackPoint.MatchingLocation.Events.Add(status)
        Next
        trackingProfile.WorkflowTrackPoints.Add(workflowTrackPoint)

        Return trackingProfile
    End Function

    Shared Sub SerializeTrackingProfileAndWriteToConsole(ByVal trackingProfile As TrackingProfile)
        Dim trackingProfileSerializer As New TrackingProfileSerializer()
        Dim trackingProfileString As New StringBuilder()
        Using writer As New StringWriter(trackingProfileString, CultureInfo.InvariantCulture)
            trackingProfileSerializer.Serialize(writer, trackingProfile)
            Console.WriteLine(writer.ToString())
        End Using
    End Sub

    Shared Sub DeserializeTrackingProfileString()
        Dim workflowTerminatedTrackingProfile As String = _
        "<?xml version=""1.0"" encoding=""utf-8"" standalone=""yes""?>" + vbCrLf + _
        "<TrackingProfile xmlns=""http://schemas.microsoft.com/winfx/2006/workflow/trackingprofile"" version=""2.0.0"">" + vbCrLf + _
        "   <TrackPoints>" + vbCrLf + _
        "        <WorkflowTrackPoint>" + vbCrLf + _
        "            <MatchingLocation>" + vbCrLf + _
        "                <WorkflowTrackingLocation>" + vbCrLf + _
        "                    <TrackingWorkflowEvents>" + vbCrLf + _
        "                        <TrackingWorkflowEvent>Terminated</TrackingWorkflowEvent>" + vbCrLf + _
        "                    </TrackingWorkflowEvents>" + vbCrLf + _
        "                </WorkflowTrackingLocation>" + vbCrLf + _
        "            </MatchingLocation>" + vbCrLf + _
        "        </WorkflowTrackPoint>" + vbCrLf + _
        "    </TrackPoints>" + vbCrLf + _
        "</TrackingProfile>"
        Dim trackingProfileSerializer As TrackingProfileSerializer = New TrackingProfileSerializer()
        Using stringReader As StringReader = New StringReader(workflowTerminatedTrackingProfile)
            Dim trackingProfile As TrackingProfile = trackingProfileSerializer.Deserialize(stringReader)
            Console.WriteLine("Tracking Profile Version " + trackingProfile.Version.ToString())
        End Using
    End Sub

    Shared Sub WriteTitle(ByVal title As String)
        Console.WriteLine(vbCrLf + "*************************************************************************")
        Console.WriteLine(title)
        Console.WriteLine("*************************************************************************")
    End Sub


End Class
