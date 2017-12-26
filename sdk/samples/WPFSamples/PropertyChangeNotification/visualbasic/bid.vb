Imports System
Imports System.ComponentModel
Imports System.Runtime.CompilerServices

    Public Class Bid
        Implements INotifyPropertyChanged

        ' Events
    Public Event PropertyChanged As PropertyChangedEventHandler Implements INotifyPropertyChanged.PropertyChanged

        ' Methods
        Public Sub New(ByVal newBidItemName As String, ByVal newBidItemPrice As Decimal)
        Me._biditemname = newBidItemName
        Me._biditemprice = newBidItemPrice
    End Sub

        Private Sub OnPropertyChanged(ByVal propName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(propName))
        End Sub


        ' Properties
        Public Property BidItemName As String
            Get
                Return Me._biditemname
            End Get
            Set(ByVal value As String)
            If Not Me._biditemname.Equals(value) Then
                Me._biditemname = value
                Me.OnPropertyChanged("BidItemName")
            End If
            End Set
        End Property

        Public Property BidItemPrice As Decimal
            Get
            Return Me._biditemprice
            End Get
            Set(ByVal value As Decimal)
            If Not Me._biditemprice.Equals(value) Then
                Me._biditemprice = value
                Me.OnPropertyChanged("BidItemPrice")
            End If
            End Set
        End Property


        ' Fields
    Private _biditemname As String = "Unset"
    Private _biditemprice As Decimal = 0
    End Class


