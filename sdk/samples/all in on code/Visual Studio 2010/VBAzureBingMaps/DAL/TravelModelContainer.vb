'***************************** Module Header ******************************\
'* Module Name:	TravelModelContainer.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* Partial class for the object context.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Data
Imports System.Data.Objects
Imports System.Reflection

''' <summary>
''' Partial class for the object context.
''' </summary>
Partial Public Class TravelModelContainer
    Inherits ObjectContext

    Public Overrides Function SaveChanges(ByVal options As SaveOptions) As Integer
        Dim returnValue As Integer = 0
        ' Since we do not call base.SaveChanges, we have to take care of connections manually.
        ' Otherwise we may leave a lot of connections open, which results in connection throttle.
        ' Entity Framework provides EnsureConnection and ReleaseConnection which base.SaveChanges uses internally.
        ' They're internal methods, so we have to use reflection to invoke them.
        Dim EnsureConnectionMethod = GetType(ObjectContext).GetMethod("EnsureConnection", BindingFlags.Instance Or BindingFlags.NonPublic)
        EnsureConnectionMethod.Invoke(Me, Nothing)
        ' Use ObjectStateManager.GetObjectStateEntries to obtain added, modified, and deleted entities.
        For Each ose As ObjectStateEntry In Me.ObjectStateManager.GetObjectStateEntries(EntityState.Added)
            Dim travel As Travel = TryCast(ose.Entity, Travel)
            If travel IsNot Nothing Then
                Dim retryPolicy As New RetryPolicy()
                retryPolicy.Task = New Action(Function()
                                                  Me.InsertIntoTravel(travel.PartitionKey, travel.Place, travel.GeoLocationText, travel.Time)
                                                  Return Nothing
                                              End Function)
                retryPolicy.Execute()
                returnValue += 1
            End If
        Next
        For Each ose As ObjectStateEntry In Me.ObjectStateManager.GetObjectStateEntries(EntityState.Modified)
            Dim travel As Travel = TryCast(ose.Entity, Travel)
            If travel IsNot Nothing Then
                Dim retryPolicy As New RetryPolicy()
                retryPolicy.Task = New Action(Function()
                                                  Me.UpdateTravel(travel.PartitionKey, travel.RowKey, travel.Place, travel.GeoLocationText, travel.Time)
                                                  Return Nothing
                                              End Function)
                retryPolicy.Execute()
                returnValue += 1
            End If
        Next
        For Each ose As ObjectStateEntry In Me.ObjectStateManager.GetObjectStateEntries(EntityState.Deleted)
            Dim travel As Travel = TryCast(ose.Entity, Travel)
            If travel IsNot Nothing Then
                Dim retryPolicy As New RetryPolicy()
                retryPolicy.Task = New Action(Function()
                                                  Me.DeleteFromTravel(travel.PartitionKey, travel.RowKey)
                                                  Return Nothing
                                              End Function)
                retryPolicy.Execute()
                returnValue += 1
            End If
        Next
        Dim ReleaseConnectionMethod = GetType(ObjectContext).GetMethod("ReleaseConnection", BindingFlags.Instance Or BindingFlags.NonPublic)
        ReleaseConnectionMethod.Invoke(Me, Nothing)
        Return returnValue
    End Function
End Class