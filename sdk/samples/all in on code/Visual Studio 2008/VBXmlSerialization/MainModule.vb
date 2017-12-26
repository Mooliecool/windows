'****************************** Module Header ******************************'
' Module Name:  MainModule.vb
' Project:      VBXmlSerialization
' Copyright (c) Microsoft Corporation.
' 
' This sample shows how to serialize an in-memory object to local xml file 
' and how to deserialize the xml file back to an in-memory object using 
' VB.NET. The designed MySerializableType includes int, string, generic, as 
' well as customized type field and property.
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
' 
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'

#Region "Imports Directives"

Imports System.Xml
Imports System.Xml.Serialization
Imports System.IO
Imports System.Collections
Imports System.Collections.Generic
Imports System

#End Region


Module MainModule

    Sub Main()

        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Serialize the object to an XML file.
        ' 

        ' Create and initialize a MySerializableType instance.
        Dim instance As New MySerializableType
        instance.BoolValue = True
        instance.IntValue = 1
        instance.StringValue = "Test String"
        instance.ListValue.Add("List Item 1")
        instance.ListValue.Add("List Item 2")
        instance.ListValue.Add("List Item 3")
        instance.AnotherTypeValue = New AnotherType
        instance.AnotherTypeValue.IntValue = 2
        instance.AnotherTypeValue.StringValue = "Inner Test String"

        ' Create the serializer
        Dim serializer As New XmlSerializer(GetType(MySerializableType))

        ' Serialize the object to an XML file
        Using streamWriter As StreamWriter = File.CreateText( _
        "VBXmlSerialization.xml")
            serializer.Serialize(streamWriter, instance)
        End Using


        '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
        ' Deserialize from a XML file to an object instance.
        ' 

        ' Deserialize the object
        Dim deserializedInstance As MySerializableType
        Using streamReader As StreamReader = File.OpenText( _
               "VBXmlSerialization.xml")
            deserializedInstance = TryCast(serializer.Deserialize( _
            streamReader), MySerializableType)
        End Using

        ' Dump the object
        Console.WriteLine("BoolValue: {0}", deserializedInstance.BoolValue)
        Console.WriteLine("IntValue: {0}", deserializedInstance.IntValue)
        Console.WriteLine("StringValue: {0}", deserializedInstance.StringValue)
        Console.WriteLine("AnotherTypeValue.IntValue: {0}", _
                          deserializedInstance.AnotherTypeValue.IntValue)
        Console.WriteLine("AnotherTypeValue.StringValue: {0}", _
                          deserializedInstance.AnotherTypeValue.StringValue)
        Console.WriteLine("ListValue: ")
        For Each obj As Object In deserializedInstance.ListValue
            Console.WriteLine(obj.ToString())
        Next

    End Sub

End Module


''' <summary>
''' Serializable Type Declaration
''' </summary>
''' <remarks></remarks>
<Serializable()> _
Public Class MySerializableType

    ' Properties
    Public Property BoolValue() As Boolean
        Get
            Return Me.boolValueField
        End Get
        Set(ByVal value As Boolean)
            Me.boolValueField = value
        End Set
    End Property

    Public Property AnotherTypeValue() As AnotherType
        Get
            Return Me.anotherTypeValueField
        End Get
        Set(ByVal value As AnotherType)
            Me.anotherTypeValueField = value
        End Set
    End Property

    Public Property IntValue() As Integer
        Get
            Return Me.intValueField
        End Get
        Set(ByVal value As Integer)
            Me.intValueField = value
        End Set
    End Property

    Public Property ListValue() As List(Of String)
        Get
            Return Me.listValueField
        End Get
        Set(ByVal value As List(Of String))
            Me.listValueField = value
        End Set
    End Property

    Public Property StringValue() As String
        Get
            Return Me.stringValueField
        End Get
        Set(ByVal value As String)
            Me.stringValueField = value
        End Set
    End Property


    ' Fields
    Private boolValueField As Boolean
    Private anotherTypeValueField As AnotherType
    <NonSerialized()> _
    Private ignoredField As Integer = 1
    Private intValueField As Integer
    Private listValueField As List(Of String) = New List(Of String)
    Private stringValueField As String

End Class


''' <summary>
''' Another Type Declaration
''' </summary>
''' <remarks></remarks>
<Serializable()> _
Public Class AnotherType

    ' Properties
    Public Property IntValue() As Integer
        Get
            Return Me.intValueField
        End Get
        Set(ByVal value As Integer)
            Me.intValueField = value
        End Set
    End Property

    Public Property StringValue() As String
        Get
            Return Me.stringValueField
        End Get
        Set(ByVal value As String)
            Me.stringValueField = value
        End Set
    End Property


    ' Fields
    Private intValueField As Integer
    Private stringValueField As String

End Class