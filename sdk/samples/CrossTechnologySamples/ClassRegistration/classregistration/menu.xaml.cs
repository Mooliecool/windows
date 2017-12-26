using System;
using System.Windows;

namespace Microsoft.Samples.ClassRegistration
{
    /// <summary>
    /// Interaction logic for Menu.xaml
    /// </summary>

    public partial class Menu : Window
    {
        private void RegisterForClasses(object sender, EventArgs args)
        {
            Register registerWindow = new Register();
            registerWindow.ShowDialog();
        }

        private void ViewSchedule(object sender, EventArgs args)
        {
            // TODO: View Shedule
        }
    }
}