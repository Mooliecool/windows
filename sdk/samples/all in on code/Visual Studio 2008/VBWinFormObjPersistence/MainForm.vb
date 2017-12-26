'************************************* Module Header **************************************\
' Module Name:  MainForm.vb
' Project:      VBWinFormObjPersistence
' Copyright (c) Microsoft Corporation.
' 
' The Object Persistance sample demonstrates how to persist an object's data between 
' instances, so that the data can be restored the next time the object is instantiated. 
' 
' This source is subject to the Microsoft Public License.
' See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
' All other rights reserved.
'
' THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
' EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
'***************************************************************************'


#Region "Import Namespace"
Imports System.Runtime.Serialization.Formatters.Binary
Imports System.IO
Imports System.Globalization
#End Region

Public Class MainForm
    Private customer As Customer

    Private Sub MainForm_Load(ByVal sender As System.Object, _
                 ByVal e As System.EventArgs) Handles MyBase.Load

        ' If the file exists, restore the data from the file 
        If File.Exists("Customer.bin") Then
            Using s As Stream = File.OpenRead("Customer.bin")
                Dim deserializer As New BinaryFormatter
                Me.customer = DirectCast(deserializer.Deserialize(s), Customer)
            End Using
        Else

            ' If the file does not exist,create a new instance of Customer class
            Me.customer = New Customer
        End If

        Me.textBox1.Text = Me.customer.Name
        Me.textBox2.Text = Me.customer.Age.ToString(New NumberFormatInfo())
        Me.textBox3.Text = Me.customer.Address

    End Sub

    Private Sub MainForm_FormClosing(ByVal sender As System.Object, _
            ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing

        ' When closing the form, save the data from the TextBoxes to a file
        Me.customer.Name = Me.textBox1.Text
        Me.customer.Age = Convert.ToInt32(Me.textBox2.Text, New NumberFormatInfo())
        Me.customer.Address = Me.textBox3.Text

        Using s As Stream = File.Create("Customer.bin")
            Dim serializer As BinaryFormatter = New BinaryFormatter
            serializer.Serialize(s, Me.customer)
        End Using

    End Sub

End Class


' The Serializable attribute tells the compiler that everything 
' within the class can be persisted to a file.
<Serializable()> _
Public Class Customer

    Private _addr As String
    Private _age As Integer
    Private _name As String

    ' If you do not want to persist a field, mark it as NonSerialized.
    <NonSerialized()> _
    Private _other As Single

    Public Property Address() As String
        Get
            Return Me._addr
        End Get
        Set(ByVal value As String)
            Me._addr = value
        End Set
    End Property

    Public Property Age() As Integer
        Get
            Return Me._age
        End Get
        Set(ByVal value As Integer)
            Me._age = value
        End Set
    End Property

    Public Property Name() As String
        Get
            Return Me._name
        End Get
        Set(ByVal value As String)
            Me._name = value
        End Set
    End Property

    Public Property Other() As Single
        Get
            Return Me._other
        End Get
        Set(ByVal value As Single)
            Me._other = value
        End Set
    End Property

End Class


