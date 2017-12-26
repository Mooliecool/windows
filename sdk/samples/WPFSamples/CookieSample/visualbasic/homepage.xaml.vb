Imports System.ComponentModel
Imports System.Windows.Interop

Partial Public Class HomePage
    Inherits System.Windows.Controls.Page

    Public Sub New()
        InitializeComponent()
    End Sub

    ' * Cookie Without Expiration
    ' Format: NAME=VALUE
    ' Persistence: In-memory only (for same process that created it)
    ' Example: "name1=value1"

    ' * Cookie With Expiration
    ' Format: NAME=VALUE; expires=DAY, DD-MMM-YYYY HH:MM:SS GMT
    ' Persistence: Retained on disk, until expiration,
    '              in current Windows version's temporary internet files location
    ' Example: "name1=value1; expires=Sat, 01-Jan-2063 00:00:00 GMT"

    Private Sub setCookieButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        Try
            ' Write cookie value
            Application.SetCookie(BrowserInteropHelper.Source, Me.setCookieValueTextBox.Text)

        Catch ex As Win32Exception
            ' Error writing cookie
            MessageBox.Show(String.Concat(New Object() {ex.Message, " (Native Error Code=", ex.NativeErrorCode, ")"}))
        End Try

    End Sub

    Private Sub getCookieButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        Try

            ' Read cookie value
            Me.getCookieValueTextBox.Text = Application.GetCookie(BrowserInteropHelper.Source)

        Catch ex As Win32Exception
            ' Error reading cookie
            MessageBox.Show(String.Concat(New Object() {ex.Message, " (Native Error Code=", ex.NativeErrorCode, ")"}))
        End Try

    End Sub

End Class
