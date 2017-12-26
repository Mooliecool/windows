using System;
using System.ComponentModel;
using System.Windows;

public partial class CustomMediaPlayerWindow : Window
{
    public CustomMediaPlayerWindow()
    {
        InitializeComponent();
    }

    bool isMediaElementPlaying;
    
    void playButton_Click(object sender, RoutedEventArgs e) {
        // Start media player
        this.mediaElement.Play();
        this.isMediaElementPlaying = true;
    }

    void stopButton_Click(object sender, RoutedEventArgs e)
    {
        // Stop media player
        this.mediaElement.Stop();
        this.isMediaElementPlaying = false;
    }

    void window_Activated(object sender, EventArgs e)
    {
        // Recommence playing media if window is activated
        if( this.isMediaElementPlaying ) this.mediaElement.Play();
    }

    void window_Deactivated(object sender, EventArgs e)
    {
        // Pause playing if media is being played and window is deactivated
        if (this.isMediaElementPlaying) this.mediaElement.Pause();
    }

    void exitMenu_Click(object sender, RoutedEventArgs e)
    {
        // Close the window
        this.Close();
    }

    void window_Closing(object sender, CancelEventArgs e)
    {
        // Ask user if they want to close the window
        if (this.isMediaElementPlaying)
        {
            string msg = "Media is playing. Really close?";
            string title = "Custom Media Player?";
            MessageBoxButton buttons = MessageBoxButton.YesNo;
            MessageBoxImage icon = MessageBoxImage.Warning;

            // Show message box and get user's answer
            MessageBoxResult result =
                MessageBox.Show(msg, title, buttons, icon);

            // Don't close window if user clicked No
            e.Cancel = (result == MessageBoxResult.No);
        }
    }
}