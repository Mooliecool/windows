using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Abstract base class for all custom task dialog controls.
    /// </summary>
    public abstract class TaskDialogControl : DialogControl
    {
        /// <summary>
        /// Creates a new instance of a task dialog control.
        /// </summary>
        protected TaskDialogControl() {}
        /// <summary>
        /// Creates a new instance of a task dialog control with the specified name.
        /// </summary>
        /// <param name="name">The name for this control.</param>
        protected TaskDialogControl(string name) : base(name) {}
    }
}