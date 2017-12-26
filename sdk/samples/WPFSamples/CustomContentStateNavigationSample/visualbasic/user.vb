Public Class User

    Public Sub New()
    End Sub

    Public Sub New(ByVal name As String)
        Me._name = name
    End Sub

    Public Property Name() As String
        Get
            Return Me._name
        End Get
        Set(ByVal value As String)
            Me._name = value
        End Set
    End Property

    Private _name As String

End Class