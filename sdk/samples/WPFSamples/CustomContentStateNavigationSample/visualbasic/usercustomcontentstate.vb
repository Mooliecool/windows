' This class encapsulates the current user data - the list selected index
<Serializable()> _
Friend Class UserCustomContentState
    Inherits CustomContentState

    Public Sub New(ByVal user As User)
        Me.user = user
    End Sub

    Public Overrides Sub Replay(ByVal navigationService As NavigationService, ByVal mode As NavigationMode)

        ' MANDATORY:  Need to override this method to restore the required state.
        ' Since the "navigation" is not user-initiated ie. set by the user selecting 
        ' a new ListBoxItem, we set the flag to false.
        Dim page As StateNavigationPage = DirectCast(navigationService.Content, StateNavigationPage)
        Dim listBox As ListBox = page.userListBox
        RemoveHandler page.userListBox.SelectionChanged, New SelectionChangedEventHandler(AddressOf page.userListBox_SelectionChanged)
        page.userListBox.SelectedItem = Me.user
        AddHandler page.userListBox.SelectionChanged, New SelectionChangedEventHandler(AddressOf page.userListBox_SelectionChanged)
    End Sub

    ' Override this property to customize the name that appears in 
    ' navigation history for this custom content state object.
    Public Overrides ReadOnly Property JournalEntryName() As String
        Get
            Return Me.user.Name
        End Get
    End Property

    Private user As User

End Class