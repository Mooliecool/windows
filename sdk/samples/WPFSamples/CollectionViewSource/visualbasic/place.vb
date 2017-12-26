Imports System

    Public Class Place
        ' Methods
        Public Sub New()
            Me._name = ""
            Me._state = ""
        End Sub

        Public Sub New(ByVal name As String, ByVal state As String)
            Me._name = name
            Me._state = state
        End Sub


        ' Properties
        Public Property CityName As String
            Get
                Return Me._name
            End Get
            Set(ByVal value As String)
                Me._name = value
            End Set
        End Property

        Public Property State As String
            Get
                Return Me._state
            End Get
            Set(ByVal value As String)
                Me._state = value
            End Set
        End Property


        ' Fields
        Private _name As String
        Private _state As String
    End Class

