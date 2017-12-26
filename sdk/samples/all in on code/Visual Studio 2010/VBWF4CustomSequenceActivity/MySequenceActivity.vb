'****************************** Module Header ******************************\
' Module Name:  MySequenceActivity.vb
' Project:		VBWF4CustomSequenceActivity
' Copyright (c) Microsoft Corporation.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************/
Imports System.ComponentModel
Imports System.Activities
Imports System.Collections.ObjectModel

<Designer(GetType(MySequenceDesigner))>
Public Class MySequenceActivity
    Inherits NativeActivity

    Public Property Branches() As Collection(Of Activity)
        Get
            Return _Branches
        End Get
        Set(ByVal value As Collection(Of Activity))
            _Branches = value
        End Set
    End Property
    Private _Branches As Collection(Of Activity)

    Public Property Variables() As Collection(Of Variable)
        Get
            Return _Variables
        End Get
        Set(ByVal value As Collection(Of Variable))
            _Variables = value
        End Set
    End Property
    Private _Variables As Collection(Of Variable)

    Public Sub New()
        Branches = New Collection(Of Activity)()
        Variables = New Collection(Of Variable)()
    End Sub

    Dim activityCounter As Integer

    Protected Overrides Sub CacheMetadata(ByVal metadata As System.Activities.NativeActivityMetadata)

        metadata.SetChildrenCollection(Branches)
        metadata.SetVariablesCollection(Variables)

    End Sub

    Protected Overrides Sub Execute(ByVal context As System.Activities.NativeActivityContext)

        ScheduleActivities(context)

    End Sub

    Protected Sub ScheduleActivities(ByVal context As NativeActivityContext)

        If activityCounter < Branches.Count Then

            context.ScheduleActivity(Me.Branches(activityCounter), AddressOf OnActivityCompleted)
            activityCounter = activityCounter + 1

        End If

    End Sub

    Protected Sub OnActivityCompleted(ByVal context As NativeActivityContext, ByVal completedInstance As ActivityInstance)

        ScheduleActivities(context)

    End Sub

End Class
