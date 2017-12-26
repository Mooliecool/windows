using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Provides standard combinations of standard buttons in the TaskDialog.
    /// </summary>
    public enum TaskDialogStandardButtons
    {
        /// <summary>
        /// No buttons.
        /// </summary>
        None        = TaskDialogStandardButton.None,
        /// <summary>
        /// A button labeled "Cancel".
        /// </summary>
        Cancel      = TaskDialogStandardButton.Cancel,
        /// <summary>
        /// Two  buttons labeled "Ok" and "Cancel".
        /// </summary>
        OkCancel    = TaskDialogStandardButton.Ok | TaskDialogStandardButton.Cancel,
        /// <summary>
        /// A button labeled "Yes".
        /// </summary>
        Yes         = TaskDialogStandardButton.Yes,
        /// <summary>
        /// Two buttons labeled "Yes" and "No".
        /// </summary>
        YesNo       = TaskDialogStandardButton.Yes | TaskDialogStandardButton.No,
        /// <summary>
        /// Three buttons labeled "Yes", "No", and "Cancel.
        /// </summary>
        YesNoCancel = TaskDialogStandardButton.Yes | TaskDialogStandardButton.No | TaskDialogStandardButton.Cancel,
        /// <summary>
        /// Two buttons labeled "Retry" and "Cancel".
        /// </summary>
        RetryCancel = TaskDialogStandardButton.Retry | TaskDialogStandardButton.Cancel,
        /// <summary>
        /// A single button labeled "Close".
        /// </summary>
        Close       = TaskDialogStandardButton.Close
    }
}
