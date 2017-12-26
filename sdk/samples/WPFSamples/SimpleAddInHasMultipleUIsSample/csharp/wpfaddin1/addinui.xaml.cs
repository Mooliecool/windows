//<SnippetAddInUICodeBehind>
using System.AddIn; // AddInAttribute
using System.ComponentModel; // INotifyPropertyChanged
using System.Windows; // MessageBox, RoutedEventArgs
using System.Windows.Controls; // UserControl

using AddInViews; // WPFAddInView

namespace WPFAddIn1
{
    /// <summary>
    /// Implements the add-in UI.
    /// Note that the add-in status UI binds to the ClickCount property of the
    /// add-in UI which, consequently, implements INotifyPropertyChanged to
    /// broadcast changes to the value of the ClickCount property. In this
    /// example, the ClickCount property is incremented with each click of
    /// the button that is displayed by the add-in UI.
    /// </summary>
    [AddIn("WPF Add-In 1")]
    public partial class AddInUI : WPFAddInView, INotifyPropertyChanged
    {
        static AddInStatusUI AddInStatusUI;

        private int clickCount = 0;

        public AddInUI()
        {
            InitializeComponent();
        }

        // Provide singleton ("once-per-instance") access to the add-in status UI
        public override FrameworkElement GetAddInStatusUI()
        {
            // Return add-in status UI
            if (AddInStatusUI == null) AddInStatusUI = new AddInStatusUI(this);

            return AddInStatusUI;
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public int ClickCount
        {
            get { return this.clickCount; }
            set 
            { 
                this.clickCount = value;
                PropertyChangedEventHandler handler = this.PropertyChanged;
                if (handler != null)
                {
                    handler(this, new PropertyChangedEventArgs("ClickCount"));
                }
            }
        }

        void clickMeButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Hello from WPFAddIn1");
            ++ClickCount;
        }
    }
}
//</SnippetAddInUICodeBehind>