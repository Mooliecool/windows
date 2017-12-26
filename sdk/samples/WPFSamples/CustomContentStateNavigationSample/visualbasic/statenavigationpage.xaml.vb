Imports System.Windows.Navigation

Public Class StateNavigationPage
    Inherits Page

    Implements IProvideCustomContentState

    Private Sub removeBackEntryButton_Click(ByVal sender As Object, ByVal e As RoutedEventArgs)

        ' If there is an entry in back navigation history, remove it
        If MyBase.NavigationService.CanGoBack Then
            Dim entry As JournalEntry = MyBase.NavigationService.RemoveBackEntry
            Dim state As UserCustomContentState = DirectCast(entry.CustomContentState, UserCustomContentState)
            Me.logListBox.Items.Insert(0, "RemoveBackEntry: " & state.JournalEntryName)
        End If

    End Sub

    Function GetContentState() As CustomContentState Implements IProvideCustomContentState.GetContentState

        ' Once content state has been replayed, it cannot be replayed again.
        ' Consequently, when we navigate away from a custom content state navigation
        ' history item, we need to recreate it  and add it to navigation history again.
        Dim user As User = TryCast(Me.userListBox.SelectedItem, User)
        Me.logListBox.Items.Insert(0, "GetContentState: " & user.Name)
        Return New UserCustomContentState(user)

    End Function

    Friend Sub userListBox_SelectionChanged(ByVal sender As Object, ByVal e As SelectionChangedEventArgs)

        ' Only add custom state to navigation history if an item is selected
        If (e.RemovedItems.Count <> 0) Then

            ' SelectionChanged occurs after list selection changes, 
            ' hence we create custom content state for the previous user
            Dim user As User = TryCast(e.RemovedItems.Item(0), User)
            Me.logListBox.Items.Insert(0, "AddBackEntry: " & user.Name)

            ' Create custom content state and add a new back entry that contains it
            Dim state As New UserCustomContentState(user)
            MyBase.NavigationService.AddBackEntry(state)

        End If

    End Sub

End Class
