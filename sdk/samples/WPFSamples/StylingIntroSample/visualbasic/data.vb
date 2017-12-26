Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.IO
Imports System.Collections.ObjectModel
Imports System.ComponentModel

Public Class Photo
    Public Sub New(ByVal path As String)
        _source = path
    End Sub

    Public Overrides Function ToString() As String
        Return Source
    End Function

    Private _source As String
    Public ReadOnly Property Source() As String
        Get
            Return _source
        End Get
    End Property

End Class

Public Class PhotoList
    Inherits ObservableCollection(Of Photo)
    Public Sub New()

    End Sub

    Public Sub New(ByVal path As String)
        Me.New(New DirectoryInfo(path))
    End Sub

    Public Sub New(ByVal directory As DirectoryInfo)
        _directory = directory
        Update()
    End Sub

    Public Property Path() As String
        Get
            Return _directory.FullName
        End Get
        Set(ByVal Value As String)
            _directory = New DirectoryInfo(Value)
            Update()
        End Set
    End Property

    Public Property Directory() As DirectoryInfo
        Get
            Return _directory
        End Get
        Set(ByVal Value As DirectoryInfo)
            _directory = Value
            Update()
        End Set
    End Property
    Private Sub Update()
        Dim f As FileInfo
        For Each f In _directory.GetFiles("*.jpg")
            Add(New Photo(f.FullName))
        Next
    End Sub

    Dim _directory As DirectoryInfo
End Class