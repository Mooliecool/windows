Imports System.Windows.Navigation

Partial Public Class SampleDetail
    Inherits Page
    Public Sub New()
        InitializeComponent()
    End Sub

    ' Executes when the user navigates to this page.
    Protected Overloads Overrides Sub OnNavigatedTo(ByVal e As NavigationEventArgs)
        ' Retrieve the sample entity according to 'sname' querystring
        ' value, then bind to view.
        If NavigationContext.QueryString.ContainsKey("sname") Then
            Me.DataContext = DirectCast(App.Current, App).SContext.Samples.FirstOrDefault(Function(s) s.Name = NavigationContext.QueryString("sname"))
        End If
    End Sub

End Class