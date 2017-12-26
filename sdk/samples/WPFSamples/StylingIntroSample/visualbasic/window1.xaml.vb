' Interaction logic for Window1.xaml
Partial Public Class Window1
    Inherits System.Windows.Window

    Public Sub New()
        InitializeComponent()
    End Sub

    Dim Photos As PhotoList

    Private Sub WindowLoaded(ByVal sender As Object, ByVal e As RoutedEventArgs)
        Photos = CType((CType(Me.Resources("MyPhotos"), ObjectDataProvider)).Data, PhotoList)
        Photos.Path = "...\\Images"
    End Sub


End Class
