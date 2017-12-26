Public Class Person

    Public Sub New(ByVal id As Integer, ByVal name As String, ByVal age As Integer)
        Me._id = id
        Me._name = name
        Me._age = age
    End Sub

    Private _id As Integer

    ''' <summary>
    ''' Person ID
    ''' </summary>
    Public Property PersonID() As Integer
        Get
            Return Me._id
        End Get
        Set(ByVal value As Integer)
            Me._id = value
        End Set
    End Property

    Private _name As String

    ''' <summary>
    ''' Person name
    ''' </summary>
    Public Property Name() As String
        Get
            Return Me._name
        End Get
        Set(ByVal value As String)
            Me._name = value
        End Set
    End Property

    Private _age As Integer

    ''' <summary>
    ''' Age that ranges from 1 to 100
    ''' </summary>
    Public Property Age() As Integer
        Get
            Return Me._age
        End Get
        Set(ByVal value As Integer)
            If ((value <= 0) OrElse (value > 100)) Then
                Throw New Exception("Age is out of scope [1,100]")
            End If
            Me._age = value
        End Set
    End Property

End Class
