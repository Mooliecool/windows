Imports System.Collections.ObjectModel

Public Class TabItemData

    Private _header As String
    Private _content As String

    Public Sub New(ByVal header As String, ByVal content As String)

        _header = header
        _content = content

    End Sub

    Public ReadOnly Property Header() As String
        Get
            Return _header
        End Get
    End Property



    Public ReadOnly Property Content() As String
        Get
            Return _content
        End Get
    End Property


End Class



Public Class TabList
    Inherits ObservableCollection(Of TabItemData)
    Public Sub New()

        MyBase.New()

        Add(New TabItemData("Header 1", "Content 1"))
        Add(New TabItemData("Header 2", "Content 2"))
        Add(New TabItemData("Header 3", "Content 3"))

    End Sub
End Class