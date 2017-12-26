Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Input
Imports System.Windows.Media

namespace document1

    Partial Public Class DockPanel1
        Inherits DockPanel


        Public Sub colWidest(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.ColumnWidth = 150
        End Sub
        Public Sub colMore(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.ColumnWidth = 250
        End Sub
        Public Sub colAverage(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.ColumnWidth = 350
        End Sub
        Public Sub colFewer(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.ColumnWidth = 450
        End Sub
        Public Sub colLeast(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.ColumnWidth = 550
        End Sub

        Public Sub textLargest(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.FontSize = 18
        End Sub
        Public Sub textLarge(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.FontSize = 15
        End Sub
        Public Sub textAverage(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.FontSize = 12
        End Sub
        Public Sub textSmall(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.FontSize = 10
        End Sub
        Public Sub textSmallest(ByVal Sender As Object, ByVal e As RoutedEventArgs)

            fd1.FontSize = 8
        End Sub
    End Class
End Namespace
