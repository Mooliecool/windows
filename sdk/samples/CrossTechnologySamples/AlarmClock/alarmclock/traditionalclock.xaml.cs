using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;


namespace Microsoft.Samples.WinFX.AlarmClock
{
    /// <summary>
    /// Interaction logic for TraditionalClock.xaml
    /// </summary>

    public partial class TraditionalClock : Window
    {
        public TraditionalClock()
        {
            InitializeComponent();
        }

        // Set the time to the current time - note that this must be triggered 
        // AFTER the animations have actually been started - in this case, after
        // the Canvas has loaded
        private void SetTime(object sender, RoutedEventArgs e)
        {
            // Fetch the storyboard and advance time 
            Storyboard clockHandStoryboard = (Storyboard)clockWindow.Resources["clockHandStoryboard"];
            clockHandStoryboard.Seek(clockCanvas, DateTime.Now.TimeOfDay, TimeSeekOrigin.BeginTime);   
        }

        private void LeftButtonDown(object sender, RoutedEventArgs e)
        {
            this.DragMove();
        }
    }
}