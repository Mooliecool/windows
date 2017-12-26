'-----------------------------------------------------------------------
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
'
'=====================================================================
'
'  File:      Serialize.vb
'  Summary:   Demonstrates how to use runtime serialization.
'
'=====================================================================

Option Explicit On 
Option Strict On


Imports System
Imports System.Text
Imports System.IO
Imports System.Runtime.Serialization
Imports System.Runtime.Serialization.Formatters.Binary
Imports System.Runtime.Serialization.Formatters.Soap
Imports System.Xml
Imports Microsoft.VisualBasic


Namespace Microsoft.Samples

Public Class App

        Private Shared Sub Usage()
            Console.WriteLine("Usage:")
            Console.WriteLine("   -sb [filename] [nodecount]" & ControlChars.Tab & "Serialize a linked list with [nodecount]")
            Console.WriteLine(ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & _
                "nodes to [filename] using binary formatter")
            Console.WriteLine()
            Console.WriteLine("   -sx [filename] [nodecount]" & ControlChars.Tab & "Serialize a linked list with [nodecount]")
            Console.WriteLine(ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & _
                "nodes to [filename] using XML formatter")
            Console.WriteLine()
            Console.WriteLine("   -db [filename] " & ControlChars.Tab & ControlChars.Tab & "De-Serialize and display a linked list from")
            Console.WriteLine(ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & _
                "[filename] using binary formatter")
            Console.WriteLine()
            Console.WriteLine("   -dx [filename] " & ControlChars.Tab & ControlChars.Tab & "De-Serialize and display a linked list from")
            Console.WriteLine(ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & ControlChars.Tab & "[filename] using XML formatter")
            Console.WriteLine()
        End Sub 'Usage


        Public Shared Sub Main(ByVal args() As String)
            ' Display usage text
            Try
                If args.Length = 0 Then
                    Usage()
                    Return
                End If

                ' Parse command line args
                Dim useBinary As Boolean = False
                Dim serialize As Boolean = False
                Dim shiftArgs As Integer = 0

                If args(0).ToCharArray()(0) = "-"c Or args(0).ToCharArray()(0) = "/"c Then
                    If args(0).ToCharArray()(1) = "?"c Then
                        Usage()
                        Return
                    End If
                    useBinary = args(0).ToCharArray()(2) = "b"c
                    serialize = args(0).ToCharArray()(1) = "s"c
                    shiftArgs = 1
                End If

                ' Serialize or Deserialize an object graph
                If serialize Then
                    SerializeGraph(useBinary, args((shiftArgs)), Integer.Parse(args((1 + shiftArgs))))
                Else
                    DeserializeGraph(useBinary, args((shiftArgs)))
                End If
            Catch e As IndexOutOfRangeException
                Usage()
            End Try
        End Sub 'Main


        Private Shared Sub SerializeGraph(ByVal useBinary As Boolean, ByVal fileName As String, ByVal nodeCount As Integer)
            ' Create a filestream object
            Console.WriteLine(ControlChars.CrLf & "Serializing LinkedList to file: {0} .." & ControlChars.CrLf, fileName)
            Dim fileStr As Stream = File.Open(fileName, FileMode.Create)

            ' Create a linked list object and populate it with random nodes
            Dim list As New LinkedList
            list.PopulateRandom(nodeCount)
            Console.WriteLine(list)

            ' Create a formatter object based on command line arguments
            Dim formatter As IFormatter
            If useBinary Then
                formatter = CType(New BinaryFormatter, IFormatter)
            Else
                formatter = CType(New SoapFormatter, IFormatter)
            End If

            ' Serialize the object graph to stream
            formatter.Serialize(fileStr, list)

            ' All done
            fileStr.Close()
        End Sub 'SerializeGraph


        Private Shared Sub DeserializeGraph(ByVal useBinary As Boolean, ByVal fileName As String)
            ' Verify that the input file exists
            If (Not File.Exists(fileName)) Then
                Console.WriteLine("Input file not found: {0}" & ControlChars.CrLf, fileName)
                Usage()
                Return
            End If

            ' Open the requested file to a stream object
            Console.WriteLine(ControlChars.CrLf + "Deserializing LinkedList from file: {0} .." + ControlChars.CrLf, fileName)
            Dim fileStr As Stream = File.Open(fileName, FileMode.Open)

            ' Create a formatter object based on command line arguments
            Dim formatter As IFormatter
            If useBinary Then
                formatter = CType(New BinaryFormatter, IFormatter)
            Else
                formatter = CType(New SoapFormatter, IFormatter)
            End If

            ' Deserialize the object graph from stream
            Try
                Dim list As LinkedList = CType(formatter.Deserialize(fileStr), LinkedList)

                ' Tests like this are relevant in projects which 
                ' serialize multiple object types
                If Not (list Is Nothing) Then
                    Console.WriteLine(list)
                Else
                    Console.WriteLine(("The deserialized object graph" + " is not a LinkedList."))
                End If
                ' Handler for BinaryFormatter exceptions
            Catch ex As SerializationException
                Console.WriteLine("Could not deserialize file.  Check that the input file is valid and the requested serialization format is correct.")
                ' Handler for SoapFormatter exceptions
            Catch ex As ArgumentException
                Console.WriteLine("Could not deserialize file.  Check that the input file is valid and the requested serialization format is correct.")
                ' Handler for XmlFormatter exceptions
            Catch ex As XmlException
                Console.WriteLine("Could not deserialize file.  Check that the input file is valid and the requested serialization format is correct.")
            End Try

            ' All done
            fileStr.Close()
        End Sub 'DeserializeGraph 
    End Class 'App


' Note that this type is attributed as serializable
<Serializable()> Class LinkedList
    
    
    ' Construct an empty LinkedList
    Public Sub New()
        nodeHead = New Node()
    End Sub 'New
    
    
    ' Represent the LinkedList as a string
    Public Overrides Function ToString() As String        
        Dim list As New StringBuilder("List:" & ControlChars.CrLf)
        
        Dim index As Integer = 0
        Dim iterator As Node = nodeHead.Next
        
        While Not (iterator Is Nothing)
            list.Append(("Node #" & index.ToString() & ControlChars.CrLf & iterator.ToString() & ControlChars.CrLf))
            index = index + 1
            iterator = iterator.Next
        End While
        Return list.ToString()
    End Function 'ToString
    
    
    ' Populate the list with an arbitrary number of nodes with random data
    Public Sub PopulateRandom(numNodes As Integer)
        Dim rand As New Random()
        
        While numNodes <> 0
            numNodes = numNodes - 1
            Add("Semi-Random String: " & rand.Next(1001).ToString(), rand.Next(1001))
        End While
    End Sub 'PopulateRandom
    
    
    ' Add a node
    Public Sub Add([text] As [String], number As Integer)
        Dim node As New Node()
        
        node.TextData = [text]
        node.NumberData = number
        
        node.Add(nodeHead)
    End Sub 'Add
    
    ' Reference to the empty head node
    Private nodeHead As Node
    
    
    ' This nested type is also attributed as serializable
<Serializable()> Public Class Node
        
        ' Construct a Node object
        Public Sub New()
            nextField = Nothing
        End Sub 'New
        
        
        ' Add a node object to a list
        Public Sub Add(nodeHead As Node)
            Dim iterator As Node = nodeHead
            
            While Not (iterator.nextField Is Nothing)
                iterator = iterator.Next
            End While 
            iterator.nextField = Me
            nextField = Nothing
        End Sub 'Add
        
        ' Accessor property for textData private field
        
        Public Property TextData() As String
            Get
                Return textDataField
            End Get
            Set
                textDataField = value
            End Set
        End Property 
        ' Accessor property for numberData private field
        
        Public Property NumberData() As Integer
            Get
                Return numberDataField
            End Get
            Set
                numberDataField = value
            End Set
        End Property 
        ' Read-only property for next private field
        
        Public ReadOnly Property [Next] As Node
            Get
                Return nextField
            End Get
        End Property
         
        ' Represent the node as a string
        Public Overrides Function ToString() As String
            Return ControlChars.Tab & "TextData   " & ChrW(61) & " """ & TextData & ControlChars.CrLf & ControlChars.Tab & "NumberData " & ChrW(61) & " " & NumberData
        End Function 'ToString
        
        ' Private field referencing the next node in the list
        Private nextField As Node
        
        ' Private fields containing node data
        Private textDataField As String
        Private numberDataField As Integer
    End Class 'Node
End Class 'LinkedList
End Namespace
