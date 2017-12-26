//<SnippetHostViewCode>
using System; // NotImplementedException
using System.Windows; // FrameworkElement
using System.Windows.Controls; // UserControl

namespace HostViews
{
    /// <summary>
    /// Defines the host's view of the add-in
    /// </summary>
    public class WPFAddInHostView : UserControl
    {
        // The view returns as a class that directly or indirectly derives from 
        // FrameworkElement and can subsequently be displayed by the host 
        // application by embedding it as content or sub-content of a UI that is 
        // implemented by the host application.
        public virtual FrameworkElement GetAddInStatusUI() {
            // The host-side adapter should override this method
            throw new NotImplementedException("Not Implemented");
        }
    }
}
//</SnippetHostViewCode>