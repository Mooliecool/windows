'---------------------------------------------------------------------
'  This file is part of the Microsoft .NET Framework SDK Code Samples.
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
Imports System.Xml.Serialization

Public Class UnorderedClass

    Private _field1 As Integer
    Private _field2 As String
    Private _field3 As String

    Public Property Field3() As String
        Get
            Return _field3
        End Get
        Set(ByVal value As String)
            _field3 = value
        End Set
    End Property

    Public Property Field1() As String
        Get
            Return _field1
        End Get
        Set(ByVal value As String)
            _field1 = value
        End Set
    End Property

    Public Property Field2() As String
        Get
            Return _field2
        End Get
        Set(ByVal value As String)
            _field2 = value
        End Set
    End Property


    Public Sub New()
        _field1 = 1
        _field2 = "String1"
        _field3 = "String2"
    End Sub
End Class

Public Class OrderedClass
    Private _field1 As Integer
    Private _field2 As String
    Private _field3 As String

    <XmlElement(Order:=3)> _
    Public Property Field3() As String
        Get
            Return _field3
        End Get
        Set(ByVal value As String)
            _field3 = value
        End Set
    End Property

    <XmlElement(Order:=1)> _
    Public Property Field1() As String
        Get
            Return _field1
        End Get
        Set(ByVal value As String)
            _field1 = value
        End Set
    End Property

    <XmlElement(Order:=2)> _
    Public Property Field2() As String
        Get
            Return _field2
        End Get
        Set(ByVal value As String)
            _field2 = value
        End Set
    End Property


    Public Sub New()
        _field1 = 1
        _field2 = "String1"
        _field3 = "String2"
    End Sub

End Class
Module Module1

    Sub Main()
        Dim ordered As OrderedClass = New OrderedClass()
        Dim unordered As UnorderedClass = New UnorderedClass()

        Dim orderedSerializer As XmlSerializer = New XmlSerializer(GetType(OrderedClass))
        Dim unorderedSerializer As XmlSerializer = New XmlSerializer(GetType(UnorderedClass))

        Console.WriteLine("Unordered serialization:" + vbCrLf)
        unorderedSerializer.Serialize(Console.Out, unordered)
        Console.WriteLine(vbCrLf + vbCrLf + "Ordered serialization:" + vbCrLf)
        orderedSerializer.Serialize(Console.Out, ordered)

        Console.ReadLine()
    End Sub

End Module
