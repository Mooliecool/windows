using System; // Environment
using System.Collections.ObjectModel; // Collection<T>
using System.AddIn.Hosting; // AddInStore, AddInToken, AddInSecurityLevel, AddInController
using System.Windows; // Window, RoutedEventArgs

using HostViews; // IAddInView, IPersonView

namespace Host
{
    public partial class MainWindow : Window
    {
        Collection<IAddInView> addInViews = new Collection<IAddInView>();
        Person person = new Person("Bob", 21);

        public MainWindow()
        {
            InitializeComponent();

            this.addInUIStackPanel.Children.Add(new HostUI(this.person));
        }

        void fileExitMenuItem_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        void loadUIAddInsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            // Get add-in pipeline folder (the folder in which this application was launched from)
            string appPath = Environment.CurrentDirectory;

            // Rebuild visual add-in pipeline
            string[] warnings = AddInStore.Rebuild(appPath);
            if (warnings.Length > 0)
            {
                string msg = "Could not rebuild pipeline:";
                foreach (string warning in warnings) msg += "\n" + warning;
                MessageBox.Show(msg);
                return;
            }

            // Load add-in with Internet zone security isolation
            Collection<AddInToken> addInTokens = AddInStore.FindAddIns(typeof(IAddInView), appPath);
            foreach (AddInToken addInToken in addInTokens)
            {
                // Get view of add-in
                IAddInView addInView = addInToken.Activate<IAddInView>(AddInSecurityLevel.Internet);

                // Store a reference to it so we can unload it later
                this.addInViews.Add(addInView);

                // Get add-in UI (passing person data to bind to
                FrameworkElement addInUI = addInView.GetAddInUI(this.person);

                // Host add-in
                this.addInUIStackPanel.Children.Add(addInUI);
            }
        }
        
        void unloadUIAddInsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            // Unload add-ins
            foreach (IAddInView addInView in this.addInViews)
            {
                AddInController addInController = AddInController.GetAddInController(addInView);
                addInController.Shutdown();
            }
        }
    }
}
