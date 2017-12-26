using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Interop;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Sets the state of a task dialog progress bar.
    /// </summary>
    public enum TaskDialogProgressBarState
    {
        /// <summary>
        /// Normal state.
        /// </summary>
        Normal      = SafeNativeMethods.PBST.PBST_NORMAL,
        /// <summary>
        /// An error occurred.
        /// </summary>
        Error       = SafeNativeMethods.PBST.PBST_ERROR,
        /// <summary>
        /// The progress is paused.
        /// </summary>
        Paused      = SafeNativeMethods.PBST.PBST_PAUSED
    }
}
