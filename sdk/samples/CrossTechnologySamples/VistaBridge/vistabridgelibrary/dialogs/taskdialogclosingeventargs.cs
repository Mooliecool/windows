using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Data associated with <see cref="E:Microsoft.SDK.Samples.VistaBridge.Library.TaskDialog.Closing"/> event.
    /// </summary>
    public class TaskDialogClosingEventArgs : CancelEventArgs
    {
        private TaskDialogStandardButton standardButton;
        /// <summary>
        /// Gets or sets the standard button that was clicked.
        /// </summary>
        public TaskDialogStandardButton StandardButton
        {
            get { return standardButton; }
            set { standardButton = value; }
        }

        private string customButton;
        /// <summary>
        /// Gets or sets the text of the custom button that was clicked.
        /// </summary>
        public string CustomButton
        {
            get { return customButton; }
            set { customButton = value; }
        }

        
    }
}
