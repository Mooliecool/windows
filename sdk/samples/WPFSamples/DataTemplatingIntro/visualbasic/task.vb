Imports System
Imports System.ComponentModel
Imports System.Runtime.CompilerServices

    Public Class Task
        Implements INotifyPropertyChanged

        ' Events
        Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

        ' Methods
        Public Sub New()
        End Sub

        Public Sub New(ByVal name As String, ByVal description As String, ByVal priority As Integer, ByVal type As TaskType)
            Me._name = name
            Me._description = description
            Me._priority = priority
            Me._type = type
        End Sub

        Protected Sub OnPropertyChanged(ByVal info As String)
            RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(info))
        End Sub

        Public Overrides Function ToString() As String
            Return Me._name.ToString
        End Function


        ' Properties
        Public Property Description As String
            Get
                Return Me._description
            End Get
            Set(ByVal value As String)
                Me._description = value
                Me.OnPropertyChanged("Description")
            End Set
        End Property

        Public Property Priority As Integer
            Get
                Return Me._priority
            End Get
            Set(ByVal value As Integer)
                Me._priority = value
                Me.OnPropertyChanged("Priority")
            End Set
        End Property

        Public Property TaskName As String
            Get
                Return Me._name
            End Get
            Set(ByVal value As String)
                Me._name = value
                Me.OnPropertyChanged("TaskName")
            End Set
        End Property

        Public Property TaskType As TaskType
            Get
                Return Me._type
            End Get
            Set(ByVal value As TaskType)
                Me._type = value
                Me.OnPropertyChanged("TaskType")
            End Set
        End Property


        ' Fields
        Private _description As String
        Private _name As String
        Private _priority As Integer
        Private _type As TaskType
    End Class


