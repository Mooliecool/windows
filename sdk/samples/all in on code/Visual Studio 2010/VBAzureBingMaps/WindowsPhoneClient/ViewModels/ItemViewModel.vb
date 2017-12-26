Imports System.ComponentModel
Imports System.Diagnostics
Imports System.Net
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Ink
Imports System.Windows.Input
Imports System.Windows.Media
Imports System.Windows.Media.Animation
Imports System.Windows.Shapes

Public Class ItemViewModel
    Implements INotifyPropertyChanged
    Private _lineOne As String
    ''' <summary>
    ''' Sample ViewModel property; this property is used in the view to display its value using a Binding.
    ''' </summary>
    ''' <returns></returns>
    Public Property LineOne() As String
        Get
            Return _lineOne
        End Get
        Set(ByVal value As String)
            If value <> _lineOne Then
                _lineOne = value
                NotifyPropertyChanged("LineOne")
            End If
        End Set
    End Property

    Private _lineTwo As String
    ''' <summary>
    ''' Sample ViewModel property; this property is used in the view to display its value using a Binding.
    ''' </summary>
    ''' <returns></returns>
    Public Property LineTwo() As String
        Get
            Return _lineTwo
        End Get
        Set(ByVal value As String)
            If value <> _lineTwo Then
                _lineTwo = value
                NotifyPropertyChanged("LineTwo")
            End If
        End Set
    End Property

    Private _lineThree As String
    ''' <summary>
    ''' Sample ViewModel property; this property is used in the view to display its value using a Binding.
    ''' </summary>
    ''' <returns></returns>
    Public Property LineThree() As String
        Get
            Return _lineThree
        End Get
        Set(ByVal value As String)
            If value <> _lineThree Then
                _lineThree = value
                NotifyPropertyChanged("LineThree")
            End If
        End Set
    End Property

    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged
    Private Sub NotifyPropertyChanged(ByVal propertyName As [String])
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propertyName))
    End Sub
End Class