Imports System

Namespace SharedPages
    Public Class TaskData

        Public Property DataItem As String
            Get
                Return Me._dataItem
            End Get
            Set(ByVal value As String)
                Me._dataItem = value
            End Set
        End Property

        Private _dataItem As String

    End Class
End Namespace


