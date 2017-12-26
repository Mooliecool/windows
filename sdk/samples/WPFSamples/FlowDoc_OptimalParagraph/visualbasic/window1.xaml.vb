Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window
        Public Sub ToggleHyphenation(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDoc.IsHyphenationEnabled = (CType(sender, CheckBox).IsChecked.Value)
        End Sub
        Public Sub ToggleOptimalParagraph(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDoc.IsOptimalParagraphEnabled = (CType(sender, CheckBox).IsChecked.Value)
        End Sub
        Public Sub ToggleColumnFlex(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDoc.IsColumnWidthFlexible = (CType(sender, CheckBox).IsChecked.Value)
        End Sub
        Public Sub ChangeColumnWidth(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))
            If myGrid.Children.Contains(flowReader) Then
                If columnWidthSlider.Value = 0 Then
                    flowDoc.ColumnWidth = 100
                ElseIf columnWidthSlider.Value = 1 Then
                    flowDoc.ColumnWidth = 200
                ElseIf columnWidthSlider.Value = 2 Then
                    flowDoc.ColumnWidth = 300
                ElseIf columnWidthSlider.Value = 3 Then
                    flowDoc.ColumnWidth = 400
                ElseIf columnWidthSlider.Value = 4 Then
                    flowDoc.ColumnWidth = 500
                End If
            End If
        End Sub
        Public Sub ChangeColumnGap(ByVal sender As Object, ByVal e As RoutedPropertyChangedEventArgs(Of Double))
            If myGrid.Children.Contains(flowReader) Then
                If columnGapSlider.Value = 0 Then
                    flowDoc.ColumnGap = 5
                ElseIf columnGapSlider.Value = 1 Then
                    flowDoc.ColumnGap = 10
                ElseIf columnGapSlider.Value = 2 Then
                    flowDoc.ColumnGap = 15
                ElseIf columnGapSlider.Value = 3 Then
                    flowDoc.ColumnGap = 20
                ElseIf columnGapSlider.Value = 4 Then
                    flowDoc.ColumnGap = 25
                End If
            End If
        End Sub

    End Class
End Namespace
