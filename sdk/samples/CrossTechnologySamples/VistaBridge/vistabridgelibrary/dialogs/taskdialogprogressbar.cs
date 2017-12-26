using System;
using System.Collections.Generic;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Provides a visual representation of the progress of a long running operation.
    /// </summary>
    public class TaskDialogProgressBar : TaskDialogBar
    {
        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        public TaskDialogProgressBar() 
        {
        }
        /// <summary>
        /// Creates a new instance of this class with the specified name.
        /// </summary>
        /// <param name="name">The name of the control.</param>
        public TaskDialogProgressBar(string name) : base(name) { }
        /// <summary>
        /// Creates a new instance of this class with the specified 
        /// minimum, maximum and current values.
        /// </summary>
        /// <param name="minimum">The minimum value for this control.</param>
        /// <param name="maximum">The maximum value for this control.</param>
        /// <param name="value">The current value for this control.</param>
        public TaskDialogProgressBar(int minimum, int maximum, int value)
        {
            this.minimum = minimum;
            this.maximum = maximum;
            this.value = value;
        }

        private int value = TaskDialogDefaults.ProgressBarStartingValue;
        private int minimum = TaskDialogDefaults.ProgressBarMinimumValue;
        private int maximum = TaskDialogDefaults.ProgressBarMaximumValue;

        /// <summary>
        /// Gets or sets the minimum value for the control.
        /// </summary>
        public int Minimum
        {
            get { return minimum; }
            set 
            {
                CheckPropertyChangeAllowed("Minimum");
                minimum = value;
                ApplyPropertyChange("Minimum");
            }
        }
        /// <summary>
        /// Gets or sets the maximum value for the control.
        /// </summary>
        public int Maximum
        {
            get { return maximum; }
            set 
            {
                CheckPropertyChangeAllowed("Maximum");
                maximum = value;
                ApplyPropertyChange("Maximum");
            }
        }
        /// <summary>
        /// Gets or sets the current value for the control.
        /// </summary>
        public int Value
        {
            get { return this.value; }
            set 
            {
                CheckPropertyChangeAllowed("Value");
                this.value = value;
                ApplyPropertyChange("Value");
            }
        }

        internal bool HasValidValues
        {
            get { return (minimum <= value && value <= maximum); }
        }
        /// <summary>
        /// Resets the control to its minimum value.
        /// </summary>
        protected internal override void Reset()
        {
            base.Reset();
            value = minimum;
        }
    }
}
