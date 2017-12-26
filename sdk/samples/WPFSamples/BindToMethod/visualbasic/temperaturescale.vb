Imports System
Imports System.ComponentModel
Imports System.Runtime.CompilerServices

    Public Class TemperatureScale
        Implements INotifyPropertyChanged

        ' Events
    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

        ' Methods
        Public Sub New()
        End Sub

        Public Sub New(ByVal type As TempType)
            Me.type = type
        End Sub

        Public Function ConvertTemp(ByVal degree As Double, ByVal temptype As TempType) As String
        If (temptype = temptype.Celsius) Then
            degree = (((degree * 9) / 5) + 32)
            Return (degree.ToString & " Fahrenheit")
        End If
        If (temptype = temptype.Fahrenheit) Then
            degree = (((degree - 32) / 9) * 5)
            Return (degree.ToString & " Celsius")
        End If
        Return "Unknown Type"
    End Function

    Protected Sub OnPropertyChanged(ByVal name As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(name))
    End Sub


    ' Properties
    Public Property Type() As TempType
        Get
            Return Me._type
        End Get
        Set(ByVal value As TempType)
            Me._type = value
            Me.OnPropertyChanged("Type")
        End Set
    End Property


    ' Fields
    Private _type As TempType
End Class


