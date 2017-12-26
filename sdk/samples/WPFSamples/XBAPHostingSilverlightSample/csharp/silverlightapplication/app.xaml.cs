using System.Windows; // Application, StartupEventArgs

namespace SilverlightApplication
{
    public partial class App : Application
    {
        public App()
        {
            InitializeComponent();
        }

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            // Load the main control
            this.RootVisual = new Page();
        }
    }
}
