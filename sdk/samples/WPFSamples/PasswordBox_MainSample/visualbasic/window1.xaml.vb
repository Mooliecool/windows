Imports System     
Imports System.Windows     
Imports System.Windows.Controls
Imports System.Windows.Documents

Namespace SDKSample

    Partial Public Class Window1
        Inherits Window

        Dim pwChanges As Integer = 0

        Public Sub WindowLoaded(ByVal sender As Object, ByVal args As RoutedEventArgs)
            'Set the starting value of the masking char combobox to the 
            'default masking character for the passwordbox.            
            listMaskChar.Text = pwdBox.PasswordChar.ToString()

            Dim x As Integer
            For x = 6 To 256
                selectMaxLen.Items.Add(x.ToString())
                selectMaxLen.SelectedIndex = 0
            Next


        End Sub
        Public Sub NewMaskChar(ByVal sender As Object, ByVal args As SelectionChangedEventArgs)
            pwdBox.PasswordChar = CType(sender, ComboBox).SelectedItem.Content.ToString().ToCharArray()(0)
        End Sub
        Public Sub PasswordChanged(ByVal sender As Object, ByVal args As RoutedEventArgs)
            pwChangesLabel.Content = pwdBox.Password.Length.ToString()
        End Sub

        Public Sub CopyContents(ByVal sender As Object, ByVal args As RoutedEventArgs)
            scratchTextBox.SelectAll()
            scratchTextBox.Copy()
        End Sub
        Public Sub PwbClear(ByVal sender As Object, ByVal args As RoutedEventArgs)
            pwdBox.Clear()
        End Sub
        Public Sub PwbPaste(ByVal sender As Object, ByVal args As RoutedEventArgs)
            pwdBox.Paste()
        End Sub
        Public Sub MaxSelected(ByVal sender As Object, ByVal args As SelectionChangedEventArgs)
            pwdBox.MaxLength = selectMaxLen.SelectedIndex + 5
            currentMaxLen.Content = pwdBox.MaxLength.ToString()
        End Sub

    End Class
End Namespace
