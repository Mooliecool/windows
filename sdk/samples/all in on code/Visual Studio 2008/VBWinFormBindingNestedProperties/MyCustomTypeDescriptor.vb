'********************************* Module Header *********************************'
' Module Name:  MyCustomTypeDescriptor.vb
' Project:      VBWinFormBindToNestedProperties
' Copyright (c) Microsoft Corporation.
' 
' The sample demonstrates how to bind a DataGridView column to a nested property 
' in the data source.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'\********************************************************************************'

Imports System.ComponentModel


Public Class MyCustomTypeDescriptor
    Inherits CustomTypeDescriptor

    Public Sub New(ByVal parent As ICustomTypeDescriptor)
        MyBase.New(parent)

    End Sub

    Public Overrides Function GetProperties(ByVal attributes() As Attribute) As PropertyDescriptorCollection
        ' Get original PropertyDescriptorCollection from the type
        Dim originalPds As PropertyDescriptorCollection = MyBase.GetProperties(attributes)
        Dim subPds As New List(Of PropertyDescriptor)

        Dim i As Integer = 0
        Do While (i < originalPds.Count)
            ' Get the chid properties of each original PropertyDescriptor
            Dim tempPds As PropertyDescriptorCollection = originalPds(i).GetChildProperties
            ' If the child properties' count is greater than 0, create a new 
            ' PropertyDescriptor for each of the child property and add it to the subPds
            If (tempPds.Count > 0) Then
                Dim j As Integer = 0
                Do While (j < tempPds.Count)
                    subPds.Add(New SubPropertyDescriptor(originalPds(i), tempPds(j), _
                        (originalPds(i).Name + ("_" + tempPds(j).Name))))
                    j = (j + 1)
                Loop
            End If
            i = (i + 1)
        Loop

        Dim array(originalPds.Count + subPds.Count) As PropertyDescriptor

        ' Copy the original PropertyDescriptorCollection to the array
        originalPds.CopyTo(array, 0)

        ' Copy all the PropertyDescriptor representing the child properties to the array
        subPds.CopyTo(array, originalPds.Count)
        Dim newPds As PropertyDescriptorCollection = New PropertyDescriptorCollection(array)

        ' Return the new PropertyDescriptorCollection containing both the PropertyDescriptor 
        ' for original properties and 
        ' child properties
        Return newPds
    End Function

    Public Overrides Function GetProperties() As PropertyDescriptorCollection
        ' Get original PropertyDescriptorCollection from the type
        Dim originalPds As PropertyDescriptorCollection = MyBase.GetProperties()
        Dim subPds As New List(Of PropertyDescriptor)

        Dim i As Integer = 0
        Do While (i < originalPds.Count)
            ' Get the chid properties of each original PropertyDescriptor
            Dim tempPds As PropertyDescriptorCollection = originalPds(i).GetChildProperties
            ' If the child properties' count is greater than 0, create a new PropertyDescriptor for
            ' each of the child property and add it to the subPds
            If (tempPds.Count > 0) Then
                Dim j As Integer = 0
                Do While (j < tempPds.Count)
                    subPds.Add(New SubPropertyDescriptor(originalPds(i), tempPds(j), _
                        (originalPds(i).Name + ("_" + tempPds(j).Name))))
                    j = (j + 1)
                Loop
            End If
            i = (i + 1)
        Loop

        Dim array(originalPds.Count + subPds.Count) As PropertyDescriptor

        ' Copy the original PropertyDescriptorCollection to the array
        originalPds.CopyTo(array, 0)

        ' Copy all the PropertyDescriptor representing the child properties to the array
        subPds.CopyTo(array, originalPds.Count)
        Dim newPds As PropertyDescriptorCollection = New PropertyDescriptorCollection(array)

        ' Return the new PropertyDescriptorCollection containing both the PropertyDescriptor 
        ' for original properties and child properties
        Return newPds
    End Function
End Class

Public Class MyTypeDescriptionProvider
    Inherits TypeDescriptionProvider

    Private td As ICustomTypeDescriptor

    Public Sub New()
        Me.New(TypeDescriptor.GetProvider(GetType(Person)))

    End Sub

    Public Sub New(ByVal parent As TypeDescriptionProvider)
        MyBase.New(parent)

    End Sub

    Public Overrides Function GetTypeDescriptor(ByVal objectType As Type, ByVal instance As Object) As ICustomTypeDescriptor
        If (td Is Nothing) Then
            td = MyBase.GetTypeDescriptor(objectType, instance)
            td = New MyCustomTypeDescriptor(td)
        End If
        Return td
    End Function
End Class

