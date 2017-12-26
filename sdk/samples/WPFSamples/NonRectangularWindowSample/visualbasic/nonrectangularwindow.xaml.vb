Namespace NonRectangularWindowSample

    ' Interaction logic for NonRectangularWindow.xaml
    Partial Public Class NonRectangularWindow
        Inherits System.Windows.Window

        Public Sub New()
            InitializeComponent()
        End Sub

        Private Sub NonRectangularWindow_MouseLeftButtonDown(ByVal sender As Object, ByVal e As MouseButtonEventArgs)
            MyBase.DragMove()
        End Sub

        Private Sub closeButtonRectangle_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)
            MyBase.Close()
        End Sub

    End Class

End Namespace