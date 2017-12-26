' Copyright (c) Microsoft Corporation. All rights reserved.

Imports System.Windows.Threading
Imports Microsoft.ServiceModel.Samples

Namespace AlbumClient

    Public Class Window1
        Inherits System.Windows.Window

        Private WithEvents client As New AlbumServiceClient()

        Public Sub New()

            InitializeComponent()

        End Sub

        Protected Overloads Overrides Sub OnInitialized(ByVal e As EventArgs)

            MyBase.OnInitialized(e)

            ' Bind the data returned from the service to the gridPanel UI element
            client.GetAlbumListAsync()

        End Sub

        Protected Overloads Overrides Sub OnClosed(ByVal e As EventArgs)

            MyBase.OnClosed(e)

            ' Clean up client when Window closes
            ' Closing the client gracefully closes the connection and cleans up resources
            client.Close()

        End Sub

        Private Sub OnAddNew(ByVal sender As Object, ByVal e As RoutedEventArgs)

            Dim value As String = newAlbumName.Text
            client.AddAlbumAsync(value)

        End Sub

        'Event handler for AddAlbum
        Private Sub client_AddAlbumCompleted(ByVal sender As Object, ByVal e As System.ComponentModel.AsyncCompletedEventArgs) Handles client.AddAlbumCompleted
            client.GetAlbumListAsync()
        End Sub
        'Event handler for GetAlbumList
        Private Sub client_GetAlbumListCompleted(ByVal sender As Object, ByVal e As GetAlbumListCompletedEventArgs) Handles client.GetAlbumListCompleted
            ' This is on the UI thread, gridPanel can be accessed directly
            gridPanel.DataContext = e.Result
        End Sub

    End Class

    Public Class TextLen2Bool
        Implements IValueConverter

        Public Function Convert(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.Convert

            Dim text As String = DirectCast(value, String)
            Return (text.Length > 0)

        End Function

        Public Function ConvertBack(ByVal value As Object, ByVal targetType As Type, ByVal parameter As Object, ByVal culture As System.Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack

            Return Nothing

        End Function

    End Class

End Namespace