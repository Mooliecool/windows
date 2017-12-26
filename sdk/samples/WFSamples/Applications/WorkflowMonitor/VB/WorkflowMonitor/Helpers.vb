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
Imports System.ComponentModel.Design
Imports System.Workflow.ComponentModel

Friend Class Helpers

    Private Sub New()
        ' Static invocation only
    End Sub

    Friend Shared Function GetNestedActivities(ByVal compositeActivity As CompositeActivity) As Activity()
        If compositeActivity Is Nothing Then
            Throw New ArgumentNullException("compositeActivity")
        End If

        Dim childActivities As IList(Of Activity) = Nothing
        Dim nestedActivities As ArrayList = New ArrayList()
        Dim compositeActivities As Queue = New Queue()
        compositeActivities.Enqueue(compositeActivity)
        While compositeActivities.Count > 0
            Dim compositeActivity2 As CompositeActivity = CType(compositeActivities.Dequeue(), CompositeActivity)
            childActivities = compositeActivity2.Activities

            For Each activity As Activity In childActivities
                nestedActivities.Add(activity)
                If TypeOf activity Is CompositeActivity Then
                    compositeActivities.Enqueue(activity)
                End If
            Next
        End While
        Return CType(nestedActivities.ToArray(GetType(Activity)), Activity())
    End Function

    Friend Shared Sub AddObjectGraphToDesignerHost(ByVal designerHost As IDesignerHost, ByVal activity As Activity)
        If designerHost Is Nothing Then
            Throw New ArgumentNullException("designerHost")
        End If

        If activity Is Nothing Then
            Throw New ArgumentNullException("activity")
        End If

        Dim rootSiteName As String = activity.Name
        designerHost.Container.Add(activity, activity.Name)

        If TypeOf activity Is CompositeActivity Then
            For Each activity2 As Activity In GetNestedActivities(activity)
                designerHost.Container.Add(activity2, activity2.QualifiedName)
            Next
        End If
    End Sub
End Class


' ListView sorter that sorts alphanumeric or numeric
Friend Class ListViewItemComparer
    Implements IComparer

    Private column As Integer
    Private isAlphaSort As Boolean

    Friend Sub New()
        Me.column = 0
    End Sub

    Friend Sub New(ByVal column As Integer, ByVal isAlphaSort As Boolean)
        Me.isAlphaSort = isAlphaSort
        Me.column = column
    End Sub

    Public Function Compare(ByVal x As Object, ByVal y As Object) As Integer Implements System.Collections.IComparer.Compare
        If (isAlphaSort) Then
            Return String.Compare(CType(x, ListViewItem).SubItems(column).Text, CType(y, ListViewItem).SubItems(column).Text)
        Else
            Dim result As Integer = 0
            Try
                result = (Convert.ToInt32(CType(x, ListViewItem).SubItems(column).Text)).CompareTo(Convert.ToInt32(CType(y, ListViewItem).SubItems(column).Text))
            Catch
                result = 0
            End Try
            Return result
        End If
    End Function
End Class
