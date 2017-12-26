Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents

namespace Typography_Samp


    Partial Class Page1
        Inherits Page

        Public Sub changeArial(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            tf1.FontFamily = New System.Windows.Media.FontFamily("Arial")
        End Sub

        Public Sub changePalatino(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            tf1.FontFamily = New System.Windows.Media.FontFamily("Palatino Linotype")
        End Sub

        Public Sub changeTimes(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            tf1.FontFamily = New System.Windows.Media.FontFamily("Times New Roman")
        End Sub

        Public Sub changeVerdana(ByVal Sender As Object, ByVal e As RoutedEventArgs)
            tf1.FontFamily = New System.Windows.Media.FontFamily("Verdana")
        End Sub

    End Class
End Namespace
