using System;


namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Used to store the result of displaying a dialog.
    /// </summary>
    public class CommonFileDialogResult
    {
        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        /// <param name="canceled">Specifies the starting value for the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.CommonFileDialogResult.Canceled"/> property.</param>
        public CommonFileDialogResult(bool canceled) 
        {
            this.canceled = canceled;
        }

        private bool canceled;
        /// <summary>
        /// Gets a value that indicates whether the end user canceled the dialog.
        /// </summary>
        public bool Canceled
        {
            get { return canceled; }
        }

    }
}
