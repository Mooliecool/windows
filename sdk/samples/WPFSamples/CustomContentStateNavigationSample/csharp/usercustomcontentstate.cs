using System;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace CustomContentStateNavigationSample
{
    [Serializable]
    // This class encapsulates the current user data - the list selected index
    class UserCustomContentState : CustomContentState
    {
        private User user;

        public UserCustomContentState(User user)
        {
            this.user = user;
        }

        // Override this property to customize the name that appears in 
        // navigation history for this custom content state object.
        public override string JournalEntryName
        {
            get
            {
                return this.user.Name;
            }
        }

        // MANDATORY:  Need to override this method to restore the required state.
        // Since the "navigation" is not user-initiated ie. set by the user selecting 
        // a new ListBoxItem, we set the flag to false.
        public override void Replay(NavigationService navigationService, NavigationMode mode)
        {
            StateNavigationPage page = (StateNavigationPage)navigationService.Content;
            ListBox userListBox = page.userListBox;

            page.userListBox.SelectionChanged -= page.userListBox_SelectionChanged;
            page.userListBox.SelectedItem = this.user;
            page.userListBox.SelectionChanged += page.userListBox_SelectionChanged;
        }
    }
}
