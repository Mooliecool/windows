using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Defines a radio button that can be hosted in by a 
    /// <see cref="Microsoft.SDK.Samples.VistaBridge.Library.TaskDialog"/> object.
    /// </summary>
    public class TaskDialogRadioButton : TaskDialogButtonBase
    {
        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        public TaskDialogRadioButton() { }
        /// <summary>
        /// Creates a new instance of this class with
        /// the specified name and text.
        /// </summary>
        /// <param name="name">The name for this control.</param>
        /// <param name="text">The value for this controls 
        /// <see cref="P:Microsoft.SDK.Samples.VistaBridge.Library.TaskDialogButtonBase.Text"/> property.</param>
        public TaskDialogRadioButton(string name, string text) : base(name, text) { }
    }
}
