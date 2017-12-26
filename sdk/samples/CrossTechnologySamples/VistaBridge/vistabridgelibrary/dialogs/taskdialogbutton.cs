using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Implements a button that can be hosted in a task dialog.
    /// </summary>
    public class TaskDialogButton : TaskDialogButtonBase
    {
        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        public TaskDialogButton() { }

        /// <summary>
        /// Creates a new instance of this class with the specified property settings.
        /// </summary>
        /// <param name="name">The name of the button.</param>
        /// <param name="text">The button label.</param>
        public TaskDialogButton(string name, string text) : base(name, text) { }

        private bool showElevationIcon;
        /// <summary>
        /// Gets or sets a value that controls whether the elevation icon is displayed.
        /// </summary>
        public bool ShowElevationIcon
        {
            get { return showElevationIcon; }
            set 
            {
                CheckPropertyChangeAllowed("ShowElevationIcon");
                showElevationIcon = value;
                ApplyPropertyChange("ShowElevationIcon");
            }
        }
    }
}
