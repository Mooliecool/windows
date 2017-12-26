' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System
Imports System.Collections.Generic
Imports System.Collections.ObjectModel
Imports System.Text
Imports System.Runtime.CompilerServices

Namespace Microsoft.Rules.Samples
    ' The container class of the extension method
    Public Module Extensions
        ' The definition of the extension method for the class OrderErrorCollection
            <Extension()> _
            Public Sub PrintOrderErrors(ByVal orderErrorCollection As OrderErrorCollection)
            Console.WriteLine()

            For Each orderError As OrderError In orderErrorCollection
                Console.WriteLine(orderError.ErrorText)
            Next
        End Sub
    End Module

' The error object that will get created when an invalid input is entered
Public Class OrderError
    Private m_errorText As String ' ie. Error: Zip code is invalid
    Private m_customerName As String ' ie. "John Customer"
    Private m_itemNum As Integer ' ie. 1 => for Vista Ultimate DVD
    Private m_zipCode As String ' ie. "00999"
    Public Sub New()
    End Sub

    Public Sub New(ByVal invalidItemNum As Integer)
        Me.ItemNum = invalidItemNum
    End Sub

    Public Sub New(ByVal invalidZipCode As String)
        Me.ZipCode = invalidZipCode
    End Sub

    Public Property ErrorText() As String
        Get
            Return Me.m_errorText
        End Get
        Set(ByVal value As String)
            Me.m_errorText = value
        End Set
    End Property

    Public Property CustomerName() As String
        Get
            Return Me.m_customerName
        End Get
        Set(ByVal value As String)
            Me.m_customerName = value
        End Set
    End Property

    Public Property ItemNum() As Integer
        Get
            Return Me.m_itemNum
        End Get
        Set(ByVal value As Integer)
            Me.m_itemNum = value
        End Set
    End Property

    Public Property ZipCode() As String
        Get
            Return Me.m_zipCode
        End Get
        Set(ByVal value As String)
            Me.m_zipCode = value
        End Set
    End Property
End Class

Public Class OrderErrorCollection
    Inherits Collection(Of OrderError)
    Public Sub New()
    End Sub

    Public Sub AddError(ByVal orderError As OrderError)
        Me.Add(orderError)
    End Sub

    ' Overload the operator + for two OrderErrorCollection objects
    Public Shared Operator +(ByVal orderErrorCollection1 As OrderErrorCollection, ByVal orderErrorCollection2 As OrderErrorCollection) As OrderErrorCollection
        Dim orderErrorCollection As New OrderErrorCollection()
        If orderErrorCollection1 IsNot Nothing Then
            For Each orderError As OrderError In orderErrorCollection1
                orderErrorCollection.Add(orderError)
            Next
        End If

        If orderErrorCollection2 IsNot Nothing Then
            For Each orderError As OrderError In orderErrorCollection2
                orderErrorCollection.Add(orderError)
            Next
        End If

        Return orderErrorCollection
    End Operator
End Class
End Namespace
