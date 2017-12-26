Imports System
<Serializable()> _
Public Class Person

    Public Sub New()
    End Sub 'New '

    Private _name As String
    Private _address As String '
    Private _birthDate As DateTime

    Public Property Name() As String
        Get
            Return _name
        End Get
        Set(ByVal value As String)
            _name = value
        End Set
    End Property

    Public Property Address() As String
        Get
            Return _address
        End Get
        Set(ByVal value As String)
            _address = value
        End Set
    End Property

    Public Property BirthDate() As DateTime
        Get
            Return _birthDate
        End Get
        Set(ByVal value As DateTime)
            _birthDate = value
        End Set
    End Property '

End Class 'Person
