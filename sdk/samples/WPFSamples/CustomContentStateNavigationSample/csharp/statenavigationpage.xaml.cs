using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace CustomContentStateNavigationSample
{
    public partial class StateNavigationPage : Page, IProvideCustomContentState
    {
        void removeBackEntryButton_Click(object sender, RoutedEventArgs e)
        {
            // If there is an entry in back navigation history, remove it
            if (this.NavigationService.CanGoBack)
            {
                JournalEntry entry = this.NavigationService.RemoveBackEntry();
                UserCustomContentState state = (UserCustomContentState)entry.CustomContentState;
                this.logListBox.Items.Insert(0, "RemoveBackEntry: " + state.JournalEntryName);
            }
        }

        internal void userListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Only add custom state to navigation history if an item is selected
            if( e.RemovedItems.Count == 0 ) return;

            // SelectionChanged occurs after list selection changes, 
            // hence we create custom content state for the previous user
            User previousUser = e.RemovedItems[0] as User;
            this.logListBox.Items.Insert(0, "AddBackEntry: " + previousUser.Name);

            // Create custom content state and add a new back entry that contains it
            UserCustomContentState userPageState = new UserCustomContentState(previousUser);
            this.NavigationService.AddBackEntry(userPageState);
        }

        CustomContentState IProvideCustomContentState.GetContentState()
        {
            // Once content state has been replayed, it cannot be replayed again.
            // Consequently, when we navigate away from a custom content state navigation
            // history item, we need to recreate it  and add it to navigation history again.
            User currentUser = this.userListBox.SelectedItem as User;
            this.logListBox.Items.Insert(0, "GetContentState: " + currentUser.Name);
            return new UserCustomContentState(currentUser);
        }
    }
}