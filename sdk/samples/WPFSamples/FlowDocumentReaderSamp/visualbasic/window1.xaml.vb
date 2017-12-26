Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Input
Imports System.Windows.Controls.Primitives

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Public Sub nextPageClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
            NavigationCommands.NextPage.Execute(Nothing, flowDocRdr)
        End Sub
        Public Sub prevPageClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
            NavigationCommands.PreviousPage.Execute(Nothing, flowDocRdr)
        End Sub
        Public Sub findClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDocRdr.Find()
        End Sub
        Public Sub IsFindEnabled_t(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDocRdr.IsFindEnabled = True
        End Sub
        Public Sub IsFindEnabled_f(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDocRdr.IsFindEnabled = False
        End Sub
        Public Sub printClick(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDocRdr.Print()
        End Sub
        Public Sub IsPageViewEnabled_t(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDocRdr.IsPageViewEnabled = True
        End Sub
        Public Sub IsScrollViewEnabled_t(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDocRdr.IsScrollViewEnabled = True
        End Sub
        Public Sub IsTwoPageViewEnabled_t(ByVal sender As Object, ByVal e As RoutedEventArgs)
            flowDocRdr.IsTwoPageViewEnabled = True
        End Sub
        Public Sub IsPageViewEnabled_f(ByVal sender As Object, ByVal e As RoutedEventArgs)

            If flowDocRdr.ViewingMode = FlowDocumentReaderViewingMode.Page Then
                MessageBox.Show("Select a different viewing mode before disabling this one.")
                rbPageView_t.IsChecked = True
                Return
            ElseIf flowDocRdr.IsScrollViewEnabled = False AndAlso flowDocRdr.IsTwoPageViewEnabled = False Then
                MessageBox.Show("At least one viewing mode must be enabled.")
                rbPageView_t.IsChecked = True
                Return
            Else
                flowDocRdr.IsPageViewEnabled = False
            End If
        End Sub
        Public Sub IsScrollViewEnabled_f(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If flowDocRdr.ViewingMode = FlowDocumentReaderViewingMode.Scroll Then
                MessageBox.Show("Select a different viewing mode before disabling this one.")
                rbScrollView_t.IsChecked = True
                Return
            ElseIf flowDocRdr.IsPageViewEnabled = False AndAlso flowDocRdr.IsTwoPageViewEnabled = False Then
                MessageBox.Show("At least one viewing mode must be enabled.")
                rbScrollView_t.IsChecked = True
                Return
            Else
                flowDocRdr.IsScrollViewEnabled = False
            End If
        End Sub
        Public Sub IsTwoPageViewEnabled_f(ByVal sender As Object, ByVal e As RoutedEventArgs)
            If flowDocRdr.ViewingMode = FlowDocumentReaderViewingMode.TwoPage Then

                MessageBox.Show("Select a different viewing mode before disabling this one.")
                rbTwoPageView_t.IsChecked = True
                Return
            ElseIf flowDocRdr.IsScrollViewEnabled = False AndAlso flowDocRdr.IsPageViewEnabled = False Then
                MessageBox.Show("At least one viewing mode must be enabled.")
                rbTwoPageView_t.IsChecked = True
                Return
            Else
                flowDocRdr.IsTwoPageViewEnabled = False
            End If
        End Sub

    End Class
End Namespace
