'***************************** Module Header ******************************\
'* Module Name:	Travel.vb
'* Project:		AzureBingMaps
'* Copyright (c) Microsoft Corporation.
'* 
'* Partial class for the Travel EF entity.
'* 
'* This source is subject to the Microsoft Public License.
'* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
'* All other rights reserved.
'* 
'* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
'* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
'* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\**************************************************************************


Imports System.Data.Objects.DataClasses
Imports System.Data.Services
Imports System.Data.Services.Common
Imports System.IO
Imports Microsoft.SqlServer.Types

''' <summary>
''' The partial class for the Travel EF entity.
''' Both PartitionKey and RowKey are part of data service key.
''' Properties such as EntityState and EntityKey should not be passed to the client.
''' The binary representation GeoLocation does not need to be passed to the client as well.
''' </summary>
<DataServiceKey(New String() {"PartitionKey", "RowKey"})> _
<IgnoreProperties(New String() {"EntityState", "EntityKey", "GeoLocation"})> _
Partial Public Class Travel
    Inherits EntityObject
    Private _geoLocationText As String

    ''' <summary>
    ''' The text representation of the geo location, which is more user friendly.
    ''' When Latitude and Longitude are modified, GeoLocationText will be modified as well.
    ''' Client may upload an entity with Latitude/Longitude, but without GeoLocationText, so its value could be null.
    ''' To avoid unintentionally setting GeoLocaionText to null, let's check the value in setter.
    ''' </summary>
    Public Property GeoLocationText() As String
        Get
            Return Me._geoLocationText
        End Get
        Set(ByVal value As String)
            If Not String.IsNullOrEmpty(value) Then
                Me._geoLocationText = value
            End If
        End Set
    End Property

    ' When either latitude or longitude changes, GeoLocationText must be changed as well.
    ' The binary GeoLocation does not need to be changed, as it is only known by the database.
    Private _latitude As Double
    Public Property Latitude() As Double
        Get
            Return Me._latitude
        End Get
        Set(ByVal value As Double)
            Me._latitude = value
            Me.GeoLocationText = Me.LatLongToWKT(Me.Latitude, Me.Longitude)
        End Set
    End Property

    Private _longitude As Double
    Public Property Longitude() As Double
        Get
            Return Me._longitude
        End Get
        Set(ByVal value As Double)
            Me._longitude = value
            Me.GeoLocationText = Me.LatLongToWKT(Me.Latitude, Me.Longitude)
        End Set
    End Property

    ''' <summary>
    ''' Convert latitude and longitude to WKT.
    ''' </summary>
    Private Function LatLongToWKT(ByVal latitude As Double, ByVal longitude As Double) As String
        Dim sqlGeography__1 As SqlGeography = SqlGeography.Point(latitude, longitude, 4326)
        Return sqlGeography__1.ToString()
    End Function

    ''' <summary>
    ''' GeoLocationText, Latitude, Longitude do not correspond to any column in the database.
    ''' Geolocation (binary) corresponds to the GeoLocation column in the TravelView.
    ''' If the binary GeoLocation changes, those values should be modified as well.
    ''' This could happen when querying the entity.
    ''' </summary>
    Private Sub OnGeoLocationChanging(ByVal value As Global.System.Byte())
        If value IsNot Nothing Then
            Using ms As New MemoryStream(value)
                Using reader As New BinaryReader(ms)
                    Dim sqlGeography As New SqlGeography()
                    sqlGeography.Read(reader)
                    Me.GeoLocationText = New String(sqlGeography.STAsText().Value)
                    Me.Latitude = sqlGeography.Lat.Value
                    Me.Longitude = sqlGeography.[Long].Value
                End Using
            End Using
        End If
    End Sub
End Class