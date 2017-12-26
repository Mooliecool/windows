'************************** Module Header ******************************'
' Module Name:  PropertyControlTable.vb
' Project:      VBVSXProjectSubType
' Copyright (c) Microsoft Corporation.
' 
' The PropertyControlTable class stores the Control / Property Name KeyValuePairs. 
' A KeyValuePair contains a Control of a PageView object, and a Property Name of
' PropertyPage object.
'  
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'************************************************************************'

Imports System.Windows.Forms

Namespace PropertyPageBase
    Public Class PropertyControlTable

        ' With these two dictionaries, it is more quick to find a Control or Property Name. 
        Private _controlNameIndex As New Dictionary(Of Control, String)()
        Private _propertyNameIndex As New Dictionary(Of String, Control)()

        ''' <summary>
        ''' Add a Key Value Pair to the dictionaries.
        ''' </summary>
        Public Sub Add(ByVal propertyName As String, ByVal control As Control)
            Me._controlNameIndex.Add(control, propertyName)
            Me._propertyNameIndex.Add(propertyName, control)
        End Sub

        ''' <summary>
        ''' Get the Control which is mapped to a Property.
        ''' </summary>
        Public Function GetControlFromPropertyName(ByVal propertyName As String) _
            As Control

            Dim _control As Control = Nothing
            If Me._propertyNameIndex.TryGetValue(propertyName, _control) Then
                Return _control
            End If
            Return Nothing
        End Function

        ''' <summary>
        ''' Get all Controls.
        ''' </summary>
        Public Function GetControls() As List(Of Control)
            Dim controlArray(Me._controlNameIndex.Count - 1) As Control
            Me._controlNameIndex.Keys.CopyTo(controlArray, 0)
            Return New List(Of Control)(controlArray)
        End Function

        ''' <summary>
        ''' Get the Property Name which is mapped to a Control.
        ''' </summary>
        Public Function GetPropertyNameFromControl(ByVal control As Control) As String
            Dim str As String = Nothing
            If Me._controlNameIndex.TryGetValue(control, str) Then
                Return str
            End If
            Return Nothing
        End Function

        ''' <summary>
        ''' Get all Property Names.
        ''' </summary>
        Public Function GetPropertyNames() As List(Of String)
            Dim strArray(Me._propertyNameIndex.Count - 1) As String
            Me._propertyNameIndex.Keys.CopyTo(strArray, 0)
            Return New List(Of String)(strArray)
        End Function

        ''' <summary>
        ''' Remove a Key Value Pair from the dictionaries.
        ''' </summary>
        Public Sub Remove(ByVal propertyName As String, ByVal control As Control)
            Me._controlNameIndex.Remove(control)
            Me._propertyNameIndex.Remove(propertyName)
        End Sub

    End Class
End Namespace
