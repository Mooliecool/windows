using System.AddIn; // AddInAttribute
using System.Windows; // FrameworkElement

using AddInViews; // IAddInView, IPersonView

namespace WPFAddIn3
{
    [AddIn("WPF Add-In - LightSeaGreen Editable UI")]
    public class AddIn : IAddInView
    {
        public FrameworkElement GetAddInUI(IPersonView person)
        {
            return new AddInUI(person);
        }
    }
}