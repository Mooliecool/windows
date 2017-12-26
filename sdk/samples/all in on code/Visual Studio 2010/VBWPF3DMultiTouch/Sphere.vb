'***************************** Module Header ******************************'
' Module Name: Sphere.cs
' Project: CSWpf3DMultiTouch
' Copyright (c) Microsoft Corporation.
' 
' Creates a reusable sphere.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System
Imports System.Windows
Imports System.Windows.Media
Imports System.Windows.Media.Media3D


Public Class Sphere
    Inherits UIElement3D
    ''' <summary>
    ''' The 3D model.
    ''' </summary>
    Private Shared ReadOnly ModelProperty As DependencyProperty = DependencyProperty.Register("Model", GetType(GeometryModel3D), GetType(Sphere), New PropertyMetadata(AddressOf ModelPropertyChanged))
    Private Property Model() As GeometryModel3D
        Get
            Return DirectCast(GetValue(ModelProperty), GeometryModel3D)
        End Get

        Set(ByVal value As GeometryModel3D)
            SetValue(ModelProperty, value)
        End Set
    End Property

    Private Shared Sub ModelPropertyChanged(ByVal d As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim s As Sphere = DirectCast(d, Sphere)
        s.Visual3DModel = s.Model
    End Sub

    ''' <summary>
    ''' Longitudes: Controls how many triangles should be created.
    ''' The larger the value, the more accurate the sphere is, but also more resources is used, and takes longer to calculate.
    ''' </summary>
    Public Shared ReadOnly LongitudesProperty As DependencyProperty = DependencyProperty.Register("Longitudes", GetType(Integer), GetType(Sphere), New PropertyMetadata(36, New PropertyChangedCallback(AddressOf LongitudesChanged)))
    Public Property Longitudes() As Integer
        Get
            Return CInt(GetValue(LongitudesProperty))
        End Get
        Set(ByVal value As Integer)
            SetValue(LongitudesProperty, value)
        End Set
    End Property

    Private Shared Sub LongitudesChanged(ByVal d As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim s As Sphere = DirectCast(d, Sphere)
        s.InvalidateModel()
    End Sub

    ''' <summary>
    ''' Latitudes: Controls how many triangles should be created.
    ''' The larger the value, the more accurate the sphere is, but also more resources is used, and takes longer to calculate.
    ''' </summary>
    Public Shared ReadOnly LatitudesProperty As DependencyProperty = DependencyProperty.Register("Latitudes", GetType(Integer), GetType(Sphere), New PropertyMetadata(18, New PropertyChangedCallback(AddressOf LongitudesChanged)))
    Public Property Latitudes() As Integer
        Get
            Return CInt(GetValue(LatitudesProperty))
        End Get
        Set(ByVal value As Integer)
            SetValue(LatitudesProperty, value)
        End Set
    End Property

    Private Shared Sub LatitudesChanged(ByVal d As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim s As Sphere = DirectCast(d, Sphere)
        s.InvalidateModel()
    End Sub

    ''' <summary>
    ''' Radius: Controls the x, y, z raduises. The larger the value, the flatter the sphere becomes.
    ''' The default value 1 indicates x, y, z values are the same, which renders a ball.
    ''' </summary>
    Public Shared ReadOnly RadiusProperty As DependencyProperty = DependencyProperty.Register("Radius", GetType(Integer), GetType(Sphere), New PropertyMetadata(1, New PropertyChangedCallback(AddressOf RadiusChanged)))
    Public Property Radius() As Integer
        Get
            Return CInt(GetValue(RadiusProperty))
        End Get
        Set(ByVal value As Integer)
            SetValue(RadiusProperty, value)
        End Set
    End Property

    Private Shared Sub RadiusChanged(ByVal d As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim s As Sphere = DirectCast(d, Sphere)
        s.InvalidateModel()
    End Sub

    ''' <summary>
    ''' The 3D material.
    ''' </summary>
    Public Shared ReadOnly MaterialProperty As DependencyProperty = DependencyProperty.Register("Material", GetType(Material), GetType(Sphere), New PropertyMetadata(New DiffuseMaterial(Brushes.Aqua), New PropertyChangedCallback(AddressOf MaterialChanged)))
    Public Property Material() As Integer
        Get
            Return CInt(GetValue(MaterialProperty))
        End Get
        Set(ByVal value As Integer)
            SetValue(MaterialProperty, value)
        End Set
    End Property

    Private Shared Sub MaterialChanged(ByVal d As DependencyObject, ByVal e As DependencyPropertyChangedEventArgs)
        Dim s As Sphere = DirectCast(d, Sphere)
        If s.Model IsNot Nothing Then
            s.Model.Material = DirectCast(e.NewValue, Material)
        End If
    End Sub

    Protected Overloads Overrides Sub OnUpdateModel()
        Dim model__1 As New GeometryModel3D()
        model__1.Geometry = Calculate(Latitudes, Longitudes, Radius)
        model__1.Material = DirectCast(GetValue(MaterialProperty), Material)
        Model = model__1
    End Sub

    ''' <summary>
    ''' Generates the geometries that compose the sphere.
    ''' </summary>
    Private Function Calculate(ByVal Latitudes As Integer, ByVal Longitudes As Integer, ByVal Radius__1 As Integer) As Geometry3D
        Dim geometry As New MeshGeometry3D()
        For latitude As Integer = 0 To Latitudes
            Dim phi As Double = Math.PI / 2 - latitude * Math.PI / Latitudes
            Dim y As Double = Math.Sin(phi)
            Dim radius__2 As Double = -Math.Cos(phi)
            For longitude As Integer = 0 To Longitudes
                Dim theta As Double = longitude * 2 * Math.PI / Longitudes
                Dim x As Double = radius__2 * Math.Sin(theta)
                Dim z As Double = radius__2 * Math.Cos(theta)
                geometry.Positions.Add(New Point3D(x, y, z))
                geometry.Normals.Add(New Vector3D(x, y, z))
                geometry.TextureCoordinates.Add(New Point(CDbl(longitude) / Longitudes, CDbl(latitude) / Latitudes))
            Next
        Next
        For latitude As Integer = 0 To Latitudes - 1
            For longitude As Integer = 0 To Longitudes - 1
                geometry.TriangleIndices.Add(latitude * (Longitudes + 1) + longitude)
                geometry.TriangleIndices.Add((latitude + 1) * (Longitudes + 1) + longitude)
                geometry.TriangleIndices.Add(latitude * (Longitudes + 1) + longitude + 1)
                geometry.TriangleIndices.Add(latitude * (Longitudes + 1) + longitude + 1)
                geometry.TriangleIndices.Add((latitude + 1) * (Longitudes + 1) + longitude)
                geometry.TriangleIndices.Add((latitude + 1) * (Longitudes + 1) + longitude + 1)
            Next
        Next
        geometry.Freeze()
        Return geometry
    End Function
End Class
