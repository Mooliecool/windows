using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Specifies the initial display location for a task dialog. 
    /// </summary>
    public enum TaskDialogStartupLocation
    {
        /// <summary>
        /// Place the window in the center of the screen.
        /// </summary>
        CenterScreen,
        /// <summary>
        /// Center the window based on the window that launched the dialog.
        /// </summary>
        CenterOwner
    }
}
