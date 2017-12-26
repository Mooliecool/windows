Imports System.Windows.Navigation

Partial Public Class AuthorDetail
    Inherits Page
    Public Sub New()
        InitializeComponent()
    End Sub

    ' Executes when the user navigates to this page.
    Protected Overloads Overrides Sub OnNavigatedTo(ByVal e As NavigationEventArgs)
        ' Retrieve the Auther entity according to 'aname' querystring
        ' value, then bind to view.
        If NavigationContext.QueryString.ContainsKey("aname") Then
            Me.DataContext = DirectCast(App.Current, App).SContext.AuthorInfos.FirstOrDefault(Function(a) a.Name = NavigationContext.QueryString("aname"))
        End If
    End Sub

End Class