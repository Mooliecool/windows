//<SnippetStatusAddInUICodeBehind>
using System.Windows.Controls.Primitives; // StatusBarItem

namespace WPFAddIn1
{
    public partial class AddInStatusUI : StatusBarItem
    {
        public AddInStatusUI(AddInUI AddInUI)
        {
            InitializeComponent();

            // Enable Content property to be bound to AddInUI.ClickCount property
            this.DataContext = AddInUI;
        }
    }
}
//</SnippetStatusAddInUICodeBehind>