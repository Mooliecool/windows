using System.Windows.Controls; // UserControl

using AddInViews; // IPersonView

namespace WPFAddIn3
{
    public partial class AddInUI : UserControl
    {
        public AddInUI(IPersonView person)
        {
            InitializeComponent();

            // Bind to person object that was provided by the host
            this.DataContext = person;
        }
    }
}
