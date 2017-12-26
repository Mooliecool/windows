using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Event data associated with a HyperlinkClick event.
    /// </summary>
    public class TaskDialogHyperlinkClickedEventArgs : EventArgs
    {
        /// <summary>
        /// Creates a new instance of this class with the specified link text.
        /// </summary>
        /// <param name="link">The text of the hyperlink that was clicked.</param>
        public TaskDialogHyperlinkClickedEventArgs(string link)
        {
            linkText = link;
        }
        
        private string linkText;
        /// <summary>
        /// The text of the hyperlink that was clicked.
        /// </summary>
        public string LinkText
        {
            get { return linkText; }
            set { linkText = value; }
        }
    }
}
