Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Controls.Primitives
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Shapes
Imports System.Windows.Data
Imports System.Collections.ObjectModel
Imports System.Collections.Generic


Namespace SDKSample

    '@ <summary>
    '@ Interaction logic for Window1_xaml.xaml
    '@ </summary>
    Partial Class Window1




        Private Sub KeyboardIncrementChanged(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            If (KeyboardIncrementAuto.IsChecked) Then
                myGridSplitter.KeyboardIncrement = 1
            ElseIf (KeyboardIncrementCols.IsChecked) Then
                myGridSplitter.KeyboardIncrement = 20
            ElseIf (KeyboardIncrementRows.IsChecked) Then
                myGridSplitter.KeyboardIncrement = 50
            ElseIf (KeyboardIncrementBoth.IsChecked) Then
                myGridSplitter.KeyboardIncrement = 100
            End If

        End Sub

        Private Sub DragIncrementChanged(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            If (DragIncrementAuto.IsChecked) Then
                myGridSplitter.DragIncrement = 1
            ElseIf (DragIncrementCols.IsChecked) Then
                myGridSplitter.DragIncrement = 20
            ElseIf (DragIncrementRows.IsChecked) Then
                myGridSplitter.DragIncrement = 50
            End If
        End Sub
        Private Sub GridResizeDirectionChanged(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            If (GridResizeDirectionAuto.IsChecked) Then
                myGridSplitter.ResizeDirection = GridResizeDirection.Auto
            ElseIf (GridResizeDirectionCols.IsChecked) Then
                myGridSplitter.ResizeDirection = GridResizeDirection.Columns
            ElseIf (GridResizeDirectionRows.IsChecked) Then
                myGridSplitter.ResizeDirection = GridResizeDirection.Rows
            End If
        End Sub
        Private Sub ResizeBehaviorChanged(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            If (BehaviorBasedOnAlignment.IsChecked) Then
                myGridSplitter.ResizeBehavior = GridResizeBehavior.BasedOnAlignment
            ElseIf (BehaviorCurrentAndNext.IsChecked) Then
                myGridSplitter.ResizeBehavior = GridResizeBehavior.CurrentAndNext
            ElseIf (BehaviorPreviousAndCurrent.IsChecked) Then
                myGridSplitter.ResizeBehavior = GridResizeBehavior.PreviousAndCurrent
            ElseIf (BehaviorPreviousAndNext.IsChecked) Then
                myGridSplitter.ResizeBehavior = GridResizeBehavior.PreviousAndNext
            End If
        End Sub
        Private Sub ShowsPreviewChanged(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            If (ShowsPreviewFalse.IsChecked) Then
                myGridSplitter.ShowsPreview = False
            ElseIf (ShowsPreviewTrue.IsChecked) Then
                myGridSplitter.ShowsPreview = True
            End If

        End Sub

        Private Sub VerticalAlignmentChanged(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            If (VerticalAlignmentTop.IsChecked) Then
                myGridSplitter.VerticalAlignment = VerticalAlignment.Top
            ElseIf (VerticalAlignmentBottom.IsChecked) Then
                myGridSplitter.VerticalAlignment = VerticalAlignment.Bottom
            ElseIf (VerticalAlignmentCenter.IsChecked) Then
                myGridSplitter.VerticalAlignment = VerticalAlignment.Center
            ElseIf (VerticalAlignmentStretch.IsChecked) Then
                myGridSplitter.VerticalAlignment = VerticalAlignment.Stretch
            End If

        End Sub
        Private Sub HorizontalAlignmentChanged(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            If (HorizontalAlignmentLeft.IsChecked) Then
                myGridSplitter.HorizontalAlignment = HorizontalAlignment.Left
            ElseIf (HorizontalAlignmentRight.IsChecked) Then
                myGridSplitter.HorizontalAlignment = HorizontalAlignment.Right
            ElseIf (HorizontalAlignmentCenter.IsChecked) Then
                myGridSplitter.HorizontalAlignment = HorizontalAlignment.Center
            ElseIf (HorizontalAlignmentStretch.IsChecked) Then
                myGridSplitter.HorizontalAlignment = HorizontalAlignment.Stretch
            End If

        End Sub
    End Class

End Namespace

