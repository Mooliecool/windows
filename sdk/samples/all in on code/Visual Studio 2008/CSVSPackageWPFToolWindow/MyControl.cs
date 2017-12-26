using System.Security.Permissions;
using System.Windows.Forms;

namespace Company.VSPackageWPFToolWindow
{
    /// <summary>
    /// Summary description for MyControl.
    /// </summary>
    public partial class MyControl : UserControl
    {
        public MyControl()
        {
            InitializeComponent();
        }

        public WPFControl WPFControl
        {
            get
            {
                return wpfControl1;
            }
        }
    }
}
