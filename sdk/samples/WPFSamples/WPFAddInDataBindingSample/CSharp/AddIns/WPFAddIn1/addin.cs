using System.AddIn; // AddInAttribute
using System.Windows; // FrameworkElement

using AddInViews; // IAddInView, IPersonView

namespace WPFAddIn1
{
    [AddIn("WPF Add-In - LightSalmon Read-Only UI")]
    public class AddIn : IAddInView
    {
        public FrameworkElement GetAddInUI(IPersonView person)
        {
            return new AddInUI(person);
        }
    }    
}