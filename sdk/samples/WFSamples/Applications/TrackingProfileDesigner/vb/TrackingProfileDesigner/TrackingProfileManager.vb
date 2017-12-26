'---------------------------------------------------------------------
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
'---------------------------------------------------------------------

Imports System.Diagnostics
Imports System.IO
Imports System.Text
Imports System.Workflow.Runtime.Tracking
Imports System.Workflow.ComponentModel
Imports System.Workflow.Runtime

Class TrackingProfileManager
    Dim trackingProfileValue As TrackingProfile

    ' Constructor
    Public Sub New()
        trackingProfileValue = New TrackingProfile()
        ' New profiles default to v1.0.0
        trackingProfileValue.Version = New Version("1.0.0")
    End Sub

    ' Constructor that takes a profile
    Public Sub New(ByVal profile As TrackingProfile)
        Me.trackingProfile = profile
    End Sub

    ' Gets the Workflow TrackPoint if one has been created, otherwise returns null.
    Public ReadOnly Property WorkflowTrackPoint() As WorkflowTrackPoint
        Get
            If TrackingProfile.WorkflowTrackPoints.Count > 0 Then
                Return TrackingProfile.WorkflowTrackPoints(0)
            Else
                Return Nothing
            End If
        End Get
    End Property

    ' Gets or sets the Tracking Profile
    Public Property TrackingProfile() As TrackingProfile
        Get
            Return trackingProfileValue
        End Get
        Set(ByVal value As TrackingProfile)
            trackingProfileValue = value
        End Set
    End Property

    ' Finds the associated ActivityTrackPoint for a given activity.
    ' Matches by activity type
    Public Function GetTrackPointForActivity(ByVal activity As Activity) As ActivityTrackPoint
        If activity IsNot Nothing Then
            For Each trackPoint As ActivityTrackPoint In TrackingProfile.ActivityTrackPoints
                If trackPoint.MatchingLocations(0).ActivityType Is activity.GetType() Then
                    Return trackPoint
                End If
            Next
        End If
        Return Nothing
    End Function

    ' Returns true if the activity has an associated track point
    Public Function IsTracked(ByVal activity As Activity) As Boolean
        Return GetTrackPointForActivity(activity) IsNot Nothing
    End Function

    ' Returns true if there exists a track point that matches the specified activity because the activity is derived from the track point's activity
    Public Function IsMatchedByDerivedTrackPoint(ByVal activity As Activity) As Boolean
        For Each trackPoint As ActivityTrackPoint In TrackingProfile.ActivityTrackPoints
            If (activity.GetType().IsSubclassOf(trackPoint.MatchingLocations(0).ActivityType) And trackPoint.MatchingLocations(0).MatchDerivedTypes) Then
                Return True
            End If
        Next
        Return False
    End Function

    ' Toggles (removes or adds) a trackpoint for a given activity
    Public Sub ToggleActivityTrackPoint(ByVal activity As Activity)
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        ' If a trackpoint is tracking this activity type, remove it
        If trackPoint IsNot Nothing Then
            TrackingProfile.ActivityTrackPoints.Remove(trackPoint)
            ' Otherwise, add a new one
        Else
            trackPoint = New ActivityTrackPoint()
            trackPoint.MatchingLocations.Add(New ActivityTrackingLocation(activity.GetType()))
            TrackingProfile.ActivityTrackPoints.Add(trackPoint)
        End If
    End Sub

#Region "Annotation"
    ' Gets the annotation for an activity.  If no annotation is present, null is returned.
    Public Function GetAnnotation(ByVal activity As Activity) As String
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing AndAlso trackPoint.Annotations.Count > 0 Then
            Return trackPoint.Annotations(0)
        Else
            Return Nothing
        End If
    End Function

    'Sets the annotaiton for an activity or workflow trackpoint

    Public Sub SetAnnotation(ByVal activity As Activity, ByVal annotation As String)
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing And trackPoint.Annotations.Count > 0 Then
            trackPoint.Annotations(0) = annotation
        Else
            trackPoint.Annotations.Add(annotation)
        End If
    End Sub
#End Region

    ' Toggles whether the specified member is extracted
    Friend Sub ToggleExtract(ByVal activity As Activity, ByVal activityExtract As Boolean, ByVal member As String)
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing Then
            Dim removed As Boolean = False
            For i As Integer = trackPoint.Extracts.Count - 1 To 1 Step -1
                If trackPoint.Extracts(i).Member = member Then
                    trackPoint.Extracts.Remove(trackPoint.Extracts(i))
                    removed = True
                End If

                If Not removed Then
                    If activityExtract Then

                        trackPoint.Extracts.Add(New ActivityDataTrackingExtract(member))
                    Else

                        trackPoint.Extracts.Add(New WorkflowDataTrackingExtract(member))
                    End If
                End If
            Next
        End If
    End Sub

    ' Returns true if the associated trackpoint for the activity matches derived types
    Public Function MatchesDerivedTypes(ByVal activity As Activity) As Boolean
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        Return trackPoint IsNot Nothing AndAlso trackPoint.MatchingLocations(0).MatchDerivedTypes
    End Function

    ' Toggles whether the specified activity status is tracked
    Friend Sub ToggleEventStatus(ByVal activity As Activity, ByVal status As ActivityExecutionStatus)
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing Then
            If (trackPoint.MatchingLocations(0).ExecutionStatusEvents.Contains(status)) Then
                trackPoint.MatchingLocations(0).ExecutionStatusEvents.Remove(status)
            Else
                trackPoint.MatchingLocations(0).ExecutionStatusEvents.Add(status)
            End If
        End If
    End Sub

    ' Returns an error message if the specified activity's track point is not correctly configured
    Friend Function IsActivityValid(ByVal activity As Activity, ByRef errorMessage As String) As Boolean
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing Then

            If (trackPoint.MatchingLocations.Count = 0) Or _
                (trackPoint.MatchingLocations(0).ExecutionStatusEvents.Count = 0) Then
                errorMessage = "Need at least one status event"
                Return True
            End If
        End If
        errorMessage = String.Empty
        Return False
    End Function

    ' Toggles a tracked workflow event for the workflow trackpoint
    Friend Sub ToggleEventStatus(ByVal workflowEvent As TrackingWorkflowEvent)
        If WorkflowTrackPoint IsNot Nothing AndAlso _
            WorkflowTrackPoint.MatchingLocation.Events.Contains(workflowEvent) Then
            WorkflowTrackPoint.MatchingLocation.Events.Remove(workflowEvent)
            If TrackingProfile.WorkflowTrackPoints(0).MatchingLocation.Events.Count = 0 Then
                TrackingProfile.WorkflowTrackPoints.Clear()
            Else
                If WorkflowTrackPoint Is Nothing Then
                    TrackingProfile.WorkflowTrackPoints.Add(New WorkflowTrackPoint())
                End If
                WorkflowTrackPoint.MatchingLocation.Events.Add(workflowEvent)
            End If
        End If
    End Sub


    ' Deletes a tracking condition for an activity      
    Friend Sub DeleteTrackingCondition(ByVal activity As Activity, ByVal condition As ActivityTrackingCondition)
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing Then
            trackPoint.MatchingLocations(0).Conditions.Remove(condition)
        End If
    End Sub

    ' Saves a tracking condition for an activity
    Friend Sub SaveTrackingCondition(ByVal activity As Activity, ByRef key As ActivityTrackingCondition, ByVal member As String, ByVal op As ComparisonOperator, ByVal value As String)
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing Then
            If (key Is Nothing) Then
                key = New ActivityTrackingCondition()
                trackPoint.MatchingLocations(0).Conditions.Add(key)
            End If
            key.Member = member
            key.Value = value
            key.Operator = op
        End If
    End Sub

    ' Gets the conditions for an activity
    Friend Function GetTrackingConditions(ByVal activity As Activity) As TrackingConditionCollection
        Dim trackPoint As ActivityTrackPoint = GetTrackPointForActivity(activity)
        If trackPoint IsNot Nothing Then
            Return trackPoint.MatchingLocations(0).Conditions
        Else
            Return Nothing
        End If
    End Function


#Region "Serialization"
    ' Read profile from path
    Public Sub ReadProfile(ByVal path As String)
        Using reader As StreamReader = New StreamReader(path)
            Dim serializer As New TrackingProfileSerializer()
            TrackingProfile = serializer.Deserialize(reader)
        End Using
    End Sub

    ' Serialize a profile to a string

    Public Function SerializeProfile() As String
        Dim profileDoc As New StringBuilder()
        Using writer As New StringWriter(profileDoc)
            SerializeProfile(writer)
            Return writer.GetStringBuilder().ToString()
        End Using
    End Function

    ' Serialize the profile to a text writer

    Public Sub SerializeProfile(ByVal writer As TextWriter)
        Dim serializer As New TrackingProfileSerializer()
        serializer.Serialize(writer, TrackingProfile)
    End Sub
#End Region

End Class
