using System.Windows.Controls; // UserControl

using HostViews; // PersonView

namespace Host
{
    public partial class HostUI : UserControl
    {
        public HostUI(Person person)
        {
            InitializeComponent();

            // Bind to person object that was provided by the host
            this.DataContext = person;
        }
    }
}
