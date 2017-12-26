using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// The event data for the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.TaskDialog.Tick"/> event.
    /// </summary>
    public class TaskDialogTickEventArgs : EventArgs
    {
        private int ticks;
        /// <summary>
        /// The data associated with the <see cref="Microsoft.SDK.Samples.VistaBridge.Library.TaskDialog.Tick"/> event.
        /// </summary>
        /// <param name="totalTicks">The total number of ticks since the control was activated.</param>
        public TaskDialogTickEventArgs(int totalTicks)
        {
            ticks = totalTicks;
        }
        /// <summary>
        /// Gets the current number of ticks.
        /// </summary>
        public int Ticks
        {
            get { return ticks; }
        }
    }
}
