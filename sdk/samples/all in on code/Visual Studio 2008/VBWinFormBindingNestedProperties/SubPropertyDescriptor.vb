'********************************* Module Header *********************************'
' Module Name:  SubPropertyDescriptor.vb
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
'*********************************************************************************'

Imports System.ComponentModel

Class SubPropertyDescriptor
    Inherits PropertyDescriptor

    Private _subPD As PropertyDescriptor
    Private _parentPD As PropertyDescriptor

    Public Sub New(ByVal parentPD As PropertyDescriptor, ByVal subPD As PropertyDescriptor, ByVal pdname As String)
        MyBase.New(pdname, Nothing)
        _subPD = subPD
        _parentPD = parentPD
    End Sub

    Public Overrides ReadOnly Property IsReadOnly() As Boolean
        Get
            Return False
        End Get
    End Property
    Public Overrides Sub ResetValue(ByVal component As Object)

    End Sub
    Public Overrides Function CanResetValue(ByVal component As Object) As Boolean
        Return False
    End Function
    Public Overrides Function ShouldSerializeValue(ByVal component As Object) As Boolean
        Return True
    End Function

    Public Overrides ReadOnly Property ComponentType() As Type
        Get
            Return _parentPD.ComponentType
        End Get
    End Property

    Public Overrides ReadOnly Property PropertyType() As Type
        Get
            Return _subPD.PropertyType
        End Get
    End Property

    Public Overrides Function GetValue(ByVal component As Object) As Object
        Return _subPD.GetValue(_parentPD.GetValue(component))
    End Function

    Public Overrides Sub SetValue(ByVal component As Object, ByVal value As Object)
        _subPD.SetValue(_parentPD.GetValue(component), value)
        OnValueChanged(component, EventArgs.Empty)
    End Sub

End Class


