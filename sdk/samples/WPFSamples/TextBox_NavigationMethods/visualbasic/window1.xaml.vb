Imports System
Imports System.Windows
Imports System.Windows.Controls
Imports System.Windows.Documents
Imports System.Windows.Navigation
Imports System.Windows.Shapes
Imports System.Windows.Data

Namespace SDKSample
    Partial Class Window1

        Public Sub New()
            InitializeComponent()
        End Sub

        Private Sub UpDownFunctions(ByVal sender As Object, ByVal args As RoutedEventArgs)

            Dim currentID As String
            Dim Button1 As Button
            Button1 = CType(sender, Button)
            currentID = Button1.Name

            Select Case currentID

                Case "btPageDown"
                    tbNavigate.PageDown()

                Case "btPageUp"
                    tbNavigate.PageUp()

                Case "btLineDown"
                    tbNavigate.LineDown()

                Case "btLineUp"
                    tbNavigate.LineUp()

                Case "btScrollToEnd"
                    tbNavigate.ScrollToEnd()

                Case "btScrollToHome"
                    tbNavigate.ScrollToHome()

            End Select

        End Sub

        Private Sub RLFunctions(ByVal sender As Object, ByVal args As RoutedEventArgs)


            Dim currentID As String
            Dim Button1 As Button
            Button1 = CType(sender, Button)
            currentID = Button1.Name

            Select Case currentID


                Case "btPageRight"
                    tbNavigate.PageRight()

                Case "btPageLeft"
                    tbNavigate.PageLeft()

                Case "btLineRight"
                    tbNavigate.LineRight()

                Case "btLineLeft"
                    tbNavigate.LineLeft()

            End Select

        End Sub

    End Class

End Namespace