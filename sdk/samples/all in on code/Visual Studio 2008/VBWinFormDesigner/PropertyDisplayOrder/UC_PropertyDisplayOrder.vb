'************************************* Module Header **************************************\
' Module Name:	UC_PropertyDisplayOrder.vb
' Project:		VBWinFormDesigner
' Copyright (c) Microsoft Corporation.
' 
' 
' The PropertyDisplayOrder sample demonstrates how to change the default display order on 
' Properties windows for properties, and how to expand a nested property on the Properties 
' window. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

Imports System.ComponentModel

Namespace PropertyDisplayOrder

    Public Class UC_PropertyDisplayOrder
        Private _cls As SubClass = New SubClass

        <TypeConverter(GetType(PropertyOrderConverter))> _
    Public Property Cls() As SubClass
            Get
                Return Me._cls
            End Get
            Set(ByVal value As SubClass)
                Me._cls = value
            End Set
        End Property
    End Class

    Public Class SubClass
        Private _prop1 As String
        Private _prop2 As Integer
        Private _prop3 As Single

        Public Property Prop1() As String
            Get
                Return Me._prop1
            End Get
            Set(ByVal value As String)
                Me._prop1 = value
            End Set
        End Property

        Public Property Prop2() As Integer
            Get
                Return Me._prop2
            End Get
            Set(ByVal value As Integer)
                Me._prop2 = value
            End Set
        End Property

        Public Property Prop3() As Single
            Get
                Return Me._prop3
            End Get
            Set(ByVal value As Single)
                Me._prop3 = value
            End Set
        End Property
    End Class


    Public Class PropertyOrderConverter
        Inherits ExpandableObjectConverter

        Public Overrides Function GetProperties(ByVal context As ITypeDescriptorContext, _
                    ByVal value As Object, ByVal attributes As Attribute()) As PropertyDescriptorCollection
            Dim propertyDescriptorCollection As PropertyDescriptorCollection = _
            TypeDescriptor.GetProperties(GetType(SubClass), attributes)
            Dim propNames As String() = New String() {"Prop2", "Prop3", "Prop1"}
            Return propertyDescriptorCollection.Sort(propNames)
        End Function

        Public Overrides Function GetPropertiesSupported(ByVal context As ITypeDescriptorContext) As Boolean
            Return True
        End Function

    End Class


End Namespace