Imports System

    Public Class GreekGod
        ' Methods
        Public Sub New(ByVal name As String)
            Me.Name = name
        End Sub


        ' Properties
        Public Property Name As String
            Get
                Return Me._name
            End Get
            Set(ByVal value As String)
                Me._name = value
            End Set
        End Property


        ' Fields
        Private _name As String
    End Class


