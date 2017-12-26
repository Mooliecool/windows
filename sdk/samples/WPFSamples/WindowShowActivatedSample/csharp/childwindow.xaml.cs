using System; // EventArgs
using System.Windows; // Window

namespace WindowShowActivatedSample
{
    public partial class ChildWindow : Window
    {
        public ChildWindow()
        {
            InitializeComponent();
        }

        private void Window_Activated(object sender, EventArgs e)
        {
            this.activationStatusTextBlock.Text = "Activated";
        }

        private void Window_Deactivated(object sender, EventArgs e)
        {
            this.activationStatusTextBlock.Text = "Deactivated";
        }
    }
}
