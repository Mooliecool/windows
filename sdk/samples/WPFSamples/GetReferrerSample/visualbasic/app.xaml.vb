Public Class App
    Inherits Application

    Protected Overrides Sub OnNavigating(ByVal e As NavigatingCancelEventArgs)
        MyBase.OnNavigating(e)
        App._referrer = Nothing
        Dim navigator As Object = e.Navigator
        If TypeOf navigator Is NavigationWindow Then
            ' Not a frame
            App._referrer = DirectCast(navigator, NavigationWindow).CurrentSource
        ElseIf TypeOf navigator Is Frame Then
            ' A frame
            App._referrer = DirectCast(navigator, Frame).CurrentSource
        End If
    End Sub

    Protected Overrides Sub OnNavigationStopped(ByVal e As NavigationEventArgs)
        MyBase.OnNavigationStopped(e)
        App._referrer = Nothing
    End Sub

    Public Shared ReadOnly Property Referrer() As Uri
        Get
            Return App._referrer
        End Get
    End Property

    Private Shared _referrer As Uri

End Class
