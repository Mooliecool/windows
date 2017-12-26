Imports System
Imports System.Windows     
Imports System.Windows.Controls     
Imports System.Windows.Documents     

Namespace SDKSample

    '@ <summary>
    '@ Interaction logic for Window1.xaml
    '@ </summary>

    Partial Public Class Window1
        Inherits Window

        Dim myDouble1 As New Double
        Dim myDouble2 As New Double

        Public Sub initVal(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myDouble1 = 1
            myDouble2 = 2
        End Sub

        Public Sub setCol(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)
            Dim lbi As ListBoxItem = CType(CType(sender, ListBox).SelectedItem, ListBoxItem)
            txt3.Text = "Column currently selected is " + lbi.Content.ToString()
            If (lbi.Content.ToString() = "colDef1") Then
                myDouble1 = 1
            ElseIf (lbi.Content.ToString() = "colDef2") Then
                myDouble1 = 2
            ElseIf (lbi.Content.ToString() = "colDef3") Then
                myDouble1 = 3
            End If
        End Sub

        Public Sub setRow(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)
            Dim lbi2 As ListBoxItem = CType(CType(sender, ListBox).SelectedItem, ListBoxItem)
            txt3.Text = "Column currently selected is " + lbi2.Content.ToString()
            If (lbi2.Content.ToString() = "rowDef1") Then
                myDouble2 = 1
            ElseIf (lbi2.Content.ToString() = "rowDef2") Then
                myDouble2 = 2
            End If
        End Sub
        Public Sub colOneStar(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If (myDouble1 = 1) Then
                colDef1.Width = New GridLength(1, GridUnitType.Star)
                txt1.Text = "colDef1.Width is currently set to " + colDef1.Width.ToString()
            ElseIf (myDouble1 = 2) Then
                colDef2.Width = New GridLength(1, GridUnitType.Star)
                txt1.Text = "colDef2.Width is currently set to " + colDef2.Width.ToString()
            ElseIf (myDouble1 = 3) Then
                colDef3.Width = New GridLength(1, GridUnitType.Star)
                txt1.Text = "colDef3.Width is currently set to " + colDef3.Width.ToString()
            End If
        End Sub
        Public Sub colTwoStar(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If (myDouble1 = 1) Then
                colDef1.Width = New GridLength(2, GridUnitType.Star)
                txt1.Text = "colDef1.Width is currently set to " + colDef1.Width.ToString()
            ElseIf (myDouble1 = 2) Then
                colDef2.Width = New GridLength(2, GridUnitType.Star)
                txt1.Text = "colDef2.Width is currently set to " + colDef2.Width.ToString()
            ElseIf (myDouble1 = 3) Then
                colDef3.Width = New GridLength(2, GridUnitType.Star)
                txt1.Text = "colDef3.Width is currently set to " + colDef3.Width.ToString()
            End If
        End Sub

        Public Sub colThreeStar(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If (myDouble1 = 1) Then
                colDef1.Width = New GridLength(3, GridUnitType.Star)
                txt1.Text = "colDef1.Width is currently set to " + colDef1.Width.ToString()
            ElseIf (myDouble1 = 2) Then
                colDef2.Width = New GridLength(3, GridUnitType.Star)
                txt1.Text = "colDef2.Width is currently set to " + colDef2.Width.ToString()
            ElseIf (myDouble1 = 3) Then
                colDef3.Width = New GridLength(3, GridUnitType.Star)
                txt1.Text = "colDef3.Width is currently set to " + colDef3.Width.ToString()
            End If
        End Sub
        Public Sub rowOneStar(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If (myDouble2 = 1) Then
                rowDef1.Height = New GridLength(1, GridUnitType.Star)
                txt2.Text = "rowDef1.Height is currently set to " + rowDef1.Height.ToString()
            ElseIf (myDouble2 = 2) Then
                rowDef2.Height = New GridLength(1, GridUnitType.Star)
                txt2.Text = "rowDef2.Height is currently set to " + rowDef2.Height.ToString()
            End If
        End Sub
        Public Sub rowTwoStar(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If (myDouble2 = 1) Then
                rowDef1.Height = New GridLength(2, GridUnitType.Star)
                txt2.Text = "rowDef1.Height is currently set to " + rowDef1.Height.ToString()
            ElseIf (myDouble2 = 2) Then
                rowDef2.Height = New GridLength(2, GridUnitType.Star)
                txt2.Text = "rowDef2.Height is currently set to " + rowDef2.Height.ToString()
            End If
        End Sub

        Public Sub rowThreeStar(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If (myDouble2 = 1) Then
                rowDef1.Height = New GridLength(3, GridUnitType.Star)
                txt2.Text = "rowDef1.Height is currently set to " + rowDef1.Height.ToString()
            ElseIf (myDouble2 = 2) Then
                rowDef2.Height = New GridLength(3, GridUnitType.Star)
                txt2.Text = "rowDef2.Height is currently set to " + rowDef2.Height.ToString()
            End If
        End Sub
        Public Sub resetSample(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myDouble1 = 1
            myDouble2 = 1
            rowDef1.Height = New GridLength(1, GridUnitType.Auto)
            rowDef2.Height = New GridLength(1, GridUnitType.Auto)
            colDef1.Width = New GridLength(1, GridUnitType.Auto)
            colDef2.Width = New GridLength(1, GridUnitType.Auto)
            colDef3.Width = New GridLength(1, GridUnitType.Auto)
            txt1.Text = ""
            txt2.Text = ""
            txt3.Text = ""
            txt4.Text = ""
        End Sub

    End Class
End Namespace
