Imports System
Imports System.Collections.ObjectModel
Imports System.Timers

    Public Class BidCollection
        Inherits ObservableCollection(Of Bid)

        ' Methods
        Public Sub New()
            Me.item1 = New Bid("Perseus Vase", New Decimal(2495, 0, 0, False, 2))
            Me.item2 = New Bid("Hercules Statue", New Decimal(1605, 0, 0, False, 2))
            Me.item3 = New Bid("Odysseus Painting", New Decimal(100))
            MyBase.Add(Me.item1)
            MyBase.Add(Me.item2)
            MyBase.Add(Me.item3)
            Me.CreateTimer
        End Sub

        Private Sub CreateTimer()
            Dim timer1 As New Timer
            timer1.Enabled = True
            timer1.Interval = 2000
            AddHandler timer1.Elapsed, New ElapsedEventHandler(AddressOf Me.Timer1_Elapsed)
        End Sub

        Private Sub Timer1_Elapsed(ByVal sender As Object, ByVal e As ElapsedEventArgs)
            Me.item1.BidItemPrice = (Me.item1.BidItemPrice + New Decimal(125, 0, 0, False, 2))
            Me.item2.BidItemPrice = (Me.item2.BidItemPrice + New Decimal(245, 0, 0, False, 2))
            Me.item3.BidItemPrice = (Me.item3.BidItemPrice + New Decimal(1055, 0, 0, False, 2))
        End Sub


        ' Fields
        Private item1 As Bid
        Private item2 As Bid
        Private item3 As Bid
    End Class

