Imports System     
Imports System.Windows     
Imports System.Windows.Controls     
Imports System.Windows.Controls.Primitives     
Imports System.Windows.Documents     
Imports System.Windows.Navigation     
Imports System.Text     

namespace ScrollViewer_Methods

    Partial Public Class Window1
        Inherits Window
        Private Sub onLoad(ByVal sender As Object, ByVal args As RoutedEventArgs)
            CType(sp1, IScrollInfo).CanVerticallyScroll = True
            CType(sp1, IScrollInfo).CanHorizontallyScroll = True
            CType(sp1, IScrollInfo).ScrollOwner = sv1
        End Sub

        Private Sub spLineUp(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).LineUp()
        End Sub
        Private Sub spLineDown(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).LineDown()
        End Sub

        Private Sub spLineRight(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).LineRight()
        End Sub

        Private Sub spLineLeft(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).LineLeft()
        End Sub

        Private Sub spPageUp(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).PageUp()
        End Sub

        Private Sub spPageDown(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).PageDown()
        End Sub

        Private Sub spPageRight(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).PageRight()
        End Sub

        Private Sub spPageLeft(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).PageLeft()
        End Sub

        Private Sub spMouseWheelDown(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).MouseWheelDown()
        End Sub

        Private Sub spMouseWheelUp(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).MouseWheelUp()
        End Sub

        Private Sub spMouseWheelLeft(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).MouseWheelLeft()
        End Sub

        Private Sub spMouseWheelRight(ByVal sender As Object, ByVal args As RoutedEventArgs)

            CType(sp1, IScrollInfo).MouseWheelRight()
        End Sub
    End Class
    End Namespace
