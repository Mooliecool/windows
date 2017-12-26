//<SnippetAddInViewCode>
using System; // NotImplementedException
using System.AddIn.Pipeline; // AddInBaseAttribute
using System.Windows; // FrameworkElement
using System.Windows.Controls; // UserControl

namespace AddInViews
{
    /// <summary>
    /// Defines the add-in's view of the contract.
    /// </summary>
    [AddInBase]
    public class WPFAddInView : UserControl
    {
        // The add-in's override of this method will return
        // a UI type that directly or indirectly derives from 
        // FrameworkElement.
        public virtual FrameworkElement GetAddInStatusUI() {
            // The add-in's should override this method
            throw new NotImplementedException("Not Implemented");
        }
    }
}
//</SnippetAddInViewCode>