Imports System.Windows.Navigation

Partial Public Class SampleList
    Inherits Page
    Public Sub New()
        InitializeComponent()
    End Sub

    ' Executes when the user navigates to this page.
    Protected Overloads Overrides Sub OnNavigatedTo(ByVal e As NavigationEventArgs)
        ' Bind sample list to view.
        SampleDataGrid.ItemsSource = DirectCast(App.Current, App).SContext.Samples
    End Sub

End Class