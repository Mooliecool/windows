using System;


namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Abstract class that supports shared functionality for 
    /// common file dialog controls.
    /// </summary>
    public abstract class CommonFileDialogControl : DialogControl
    {
        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        protected CommonFileDialogControl() {}
        /// <summary>
        /// Creates a new instance of this class using the specified name.
        /// </summary>
        /// <param name="name">The name for this instance.</param>
        protected CommonFileDialogControl(string name) : base(name) { }
    }
}
