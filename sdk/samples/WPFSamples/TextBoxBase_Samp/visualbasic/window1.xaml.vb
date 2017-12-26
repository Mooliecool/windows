Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Private Sub initValues(ByVal sender As Object, ByVal e As EventArgs)
            myTB1.Text = "ExtentHeight is currently " + myTextBox.ExtentHeight.ToString()
            myTB2.Text = "ExtentWidth is currently " + myTextBox.ExtentWidth.ToString()
            myTB3.Text = "HorizontalOffset is currently " + myTextBox.HorizontalOffset.ToString()
            myTB4.Text = "VerticalOffset is currently " + myTextBox.VerticalOffset.ToString()
            myTB5.Text = "ViewportHeight is currently " + myTextBox.ViewportHeight.ToString()
            myTB6.Text = "ViewportWidth is currently " + myTextBox.ViewportWidth.ToString()
            radiobtn1.IsChecked = True
        End Sub

        Private Sub copyText(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.Copy()
        End Sub

        Private Sub cutText(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.Cut()
        End Sub

        Private Sub pasteSelection(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.Paste()
        End Sub

        Private Sub selectAll(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.SelectAll()
        End Sub

        Private Sub undoAction(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If myTextBox.CanUndo = True Then
                myTextBox.Undo()
            Else : Return
            End If
        End Sub

        Private Sub redoAction(ByVal sender As Object, ByVal e As RoutedEventArgs)

            If myTextBox.CanRedo = True Then
                myTextBox.Redo()
            Else : Return
            End If
        End Sub

        Private Sub selectChanged(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.AppendText("Selection Changed event in myTextBox2 has just occurred.")
        End Sub

        Private Sub tChanged(ByVal sender As Object, ByVal e As TextChangedEventArgs)
            myTextBox.AppendText("Text content of myTextBox2 has changed.")
        End Sub

        Private Sub wrapOff(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.TextWrapping = TextWrapping.NoWrap
        End Sub

        Private Sub wrapOn(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.TextWrapping = TextWrapping.Wrap
        End Sub

        Private Sub clearTB1(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.Clear()
        End Sub

        Private Sub clearTB2(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox2.Clear()
        End Sub

        Private Sub lineDown(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.LineDown()
        End Sub

        Private Sub lineLeft(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.LineLeft()
        End Sub

        Private Sub lineRight(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.LineRight()
        End Sub

        Private Sub lineUp(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.LineUp()
        End Sub

        Private Sub pageDown(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.PageDown()
        End Sub

        Private Sub pageLeft(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.PageLeft()
        End Sub

        Private Sub pageRight(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.PageRight()
        End Sub

        Private Sub pageUp(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.PageUp()
        End Sub

        Private Sub scrollHome(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.ScrollToHome()
        End Sub

        Private Sub scrollEnd(ByVal sender As Object, ByVal e As RoutedEventArgs)
            myTextBox.ScrollToEnd()
        End Sub

    End Class
End Namespace
