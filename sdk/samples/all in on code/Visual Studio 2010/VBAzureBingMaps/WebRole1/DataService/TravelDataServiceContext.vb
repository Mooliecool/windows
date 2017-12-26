'***************************** Module Header ******************************\
'* Module Name:	TravelDataServiceContext.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* Implements a reflection provider for WCF Data Services.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Data.Objects
Imports System.Data.Services
Imports System.Linq
Imports System.Web
Imports AzureBingMaps.DAL

Namespace AzureBingMaps.WebRole.DataService
    ''' <summary>
    ''' Implements a reflection provider for WCF Data Services.
    ''' </summary>
    Public Class TravelDataServiceContext
        Implements IUpdatable
        Private _entityFrameworkContext As TravelModelContainer

        Public Sub New()
            ' Obtain the connection string for the partition.
            ' PartitionKey represents the current user.
            Me._entityFrameworkContext = New TravelModelContainer(Me.GetConnectionString(Me.SetPartitionKey()))
        End Sub

        ''' <summary>
        ''' Standard query for data services.
        ''' </summary>
        Public ReadOnly Property Travels() As IQueryable(Of Travel)
            Get
                ' Only query data for the specific user.
                Dim partitionKey As String = Me.SetPartitionKey()
                Return Me._entityFrameworkContext.Travels.Where(Function(e) e.PartitionKey = partitionKey)
            End Get
        End Property

        Public Sub AddReferenceToCollection( _
                                           ByVal targetResource As Object, _
                                           ByVal propertyName As String, _
                                           ByVal resourceToBeAdded As Object _
                                           ) Implements IUpdatable.AddReferenceToCollection
            Throw New NotImplementedException()
        End Sub

        Public Sub ClearChanges() Implements IUpdatable.ClearChanges
            Throw New NotImplementedException()
        End Sub

        ''' <summary>
        ''' Create a new entity. This step doesn't set properties.
        ''' </summary>
        Public Function CreateResource( _
                                      ByVal containerName As String, _
                                      ByVal fullTypeName As String _
                                      ) As Object Implements IUpdatable.CreateResource
            Try
                Dim t As Type = Type.[GetType](fullTypeName & ", AzureBingMaps.DAL", True)
                Dim resource As Object = Activator.CreateInstance(t)
                If TypeOf resource Is Travel Then
                    Me._entityFrameworkContext.Travels.AddObject(DirectCast(resource, Travel))
                End If
                Return resource
            Catch ex As Exception
                Throw New InvalidOperationException(
                    "Failed to create resource. See the inner exception for more details.", ex)
            End Try
        End Function

        ''' <summary>
        '''  Delete an entity.
        ''' </summary>
        Public Sub DeleteResource(ByVal targetResource As Object) Implements IUpdatable.DeleteResource
            If TypeOf targetResource Is Travel Then
                Me._entityFrameworkContext.Travels.DeleteObject(DirectCast(targetResource, Travel))
            End If
        End Sub

        ''' <summary>
        ''' Get a single entity. Used in update and delete.
        ''' </summary>
        Public Function GetResource( _
                                   ByVal query As IQueryable, _
                                   ByVal fullTypeName As String _
                                   ) As Object Implements IUpdatable.GetResource
            Dim q As ObjectQuery(Of Travel) = TryCast(query, ObjectQuery(Of Travel))
            Dim enumerator = query.GetEnumerator()
            If Not enumerator.MoveNext() Then
                Throw New ApplicationException("Could not locate the resource.")
            End If
            If enumerator.Current Is Nothing Then
                Throw New ApplicationException("Could not locate the resource.")
            End If
            Return enumerator.Current
        End Function

        Public Function GetValue( _
                                ByVal targetResource As Object, _
                                ByVal propertyName As String _
                                ) As Object Implements IUpdatable.GetValue
            Throw New NotImplementedException()
        End Function

        Public Sub RemoveReferenceFromCollection( _
                                                ByVal targetResource As Object, _
                                                ByVal propertyName As String, _
                                                ByVal resourceToBeRemoved As Object _
                                                ) Implements IUpdatable.RemoveReferenceFromCollection
            Throw New NotImplementedException()
        End Sub

        ''' <summary>
        ''' Update the entity.
        ''' </summary> 
        Public Function ResetResource( _
                                     ByVal resource As Object _
                                     ) As Object Implements IUpdatable.ResetResource
            If TypeOf resource Is Travel Then
                Dim updated As Travel = DirectCast(resource, Travel)
                Dim original = Me._entityFrameworkContext.Travels.Where( _
                    Function(t) t.PartitionKey = updated.PartitionKey AndAlso t.RowKey = updated.RowKey).FirstOrDefault()
                original.GeoLocationText = updated.GeoLocationText
                original.Place = updated.Place
                original.Time = updated.Time
            End If
            Return resource
        End Function

        Public Function ResolveResource( _
                                       ByVal resource As Object _
                                       ) As Object Implements IUpdatable.ResolveResource
            Return resource
        End Function

        Public Sub SaveChanges() Implements IUpdatable.SaveChanges
            Me._entityFrameworkContext.SaveChanges()
        End Sub

        Public Sub SetReference( _
                               ByVal targetResource As Object, _
                               ByVal propertyName As String, _
                               ByVal propertyValue As Object _
                               ) Implements IUpdatable.SetReference
            Throw New NotImplementedException()
        End Sub

        ''' <summary>
        ''' Set property values.
        ''' </summary>
        Public Sub SetValue( _
                           ByVal targetResource As Object, _
                           ByVal propertyName As String, _
                           ByVal propertyValue As Object _
                           ) Implements IUpdatable.SetValue
            Try

                Dim [property] = targetResource.[GetType]().GetProperty(propertyName)
                If [property] Is Nothing Then
                    Throw New InvalidOperationException("Invalid property: " & propertyName)
                End If

                ' PartitionKey represents the user identity,
                ' which must be obtained on the server side,
                ' otherwise client may send false identities.
                If [property].Name = "PartitionKey" Then
                    Dim partitionKey As String = Me.SetPartitionKey()
                    [property].SetValue(targetResource, partitionKey, Nothing)
                Else
                    [property].SetValue(targetResource, propertyValue, Nothing)
                End If
            Catch ex As Exception
                Throw New InvalidOperationException("Failed to set value. See the inner exception for more details.", ex)
            End Try
        End Sub

        ''' <summary>
        ''' If the user hasn't signed in, using default partition.
        ''' Otherwise the partition key is the user's email address.
        ''' </summary>
        Private Function SetPartitionKey() As String
            Dim partitionKey As String = "defaultuser@live.com"
            Dim user As String = TryCast(HttpContext.Current.Session("User"), String)
            If user IsNot Nothing Then
                partitionKey = user
            End If
            Return partitionKey
        End Function

        ''' <summary>
        ''' Obtain the connection string for the partition.
        ''' For now, all partitions are stored in the same database.
        ''' But as data and users grows,
        ''' we can move partitions to other databases for better scaling.
        ''' In the future, we can also take advantage of SQL Azure federation.
        ''' </summary>
        Private Function GetConnectionString(ByVal partitionKey As String) As String
            Return "name=TravelModelContainer"
        End Function
    End Class
End Namespace