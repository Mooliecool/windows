Namespace SDKSample

    Public Class Person

        Private _favoriteColor As Color
        Private _name As String

        Public Sub New()
        End Sub

        Public Sub New(ByVal name As String, ByVal favoriteColor As Color)
            _name = name
            _favoriteColor = favoriteColor
        End Sub

        Public Property Name() As String
            Get
                Return _name
            End Get
            Set(ByVal value As String)
                _name = value
            End Set
        End Property

        Public Property FavoriteColor() As Color
            Get
                Return _favoriteColor
            End Get
            Set(ByVal value As Color)
                _favoriteColor = value
            End Set
        End Property

    End Class

End Namespace