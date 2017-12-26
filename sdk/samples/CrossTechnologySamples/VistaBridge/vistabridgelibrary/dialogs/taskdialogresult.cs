using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Library;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Diagnostics;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Indicates the various buttons and options clicked by the user on the task dialog.
    /// </summary>
    public class TaskDialogResult
    {
        private TaskDialogStandardButton standardButtonClicked = TaskDialogStandardButton.None;
        private string customButtonClicked;
        private string radioButtonClicked;
        private bool checkboxChecked;

        internal TaskDialogResult(TaskDialogStandardButton stdButton,
                                  string custButton,
                                  string rButton,
                                  bool cbChecked)
        {
            standardButtonClicked = stdButton;
            customButtonClicked = custButton;
            radioButtonClicked = rButton;
            checkboxChecked = cbChecked;
        }

        /// <summary>
        /// Indicates which standard button was selected by the user, 
        /// if they selected a custom button.
        /// </summary>
        /// <remarks>
        /// TaskDialogStandardButton.None will be returned 
        /// if some other option was selected, or the
        /// dialog was cancelled.
        /// </remarks>
        public TaskDialogStandardButton StandardButtonClicked
        {
            get { return standardButtonClicked; }
        }
        
        /// <summary>
        /// Indicates which named custom button was 
        /// selected by the user. It will be null
        /// if user cancelled the dialog or selected a standard button.
        /// </summary>
        public string CustomButtonClicked
        {
            get { return customButtonClicked; }
        }
       
        /// <summary>
        /// Indicates which radio button was selected when 
        /// the user closed the dialog. It will be null
        /// if the user cancelled the dialog.
        /// </summary>
        public string  RadioButtonClicked
        {
            get { return radioButtonClicked; }
        }
        
        /// <summary>
        /// Indicates whether the checkbox was 
        /// checked or unchecked when the dialog was closed by the user.
        /// </summary>
        public bool CheckBoxChecked
        {
            get { return checkboxChecked; }
        }

        // Helper, for debugging purposes.
        /// <summary>
        /// Returns a string representation of this object.
        /// </summary>
        /// <returns>A <see cref="System.String"/>.</returns>
        public override string ToString()
        {
            string tempCustomButton = customButtonClicked;
            if (tempCustomButton == null)
                tempCustomButton = "<none>";
            string tempRadioButton = radioButtonClicked;
            if (tempRadioButton == null)
                tempRadioButton = "<none>";
            return String.Format(
                "TaskDialogResult - standard button: {0}, custom button: {1}, radio button: {2}, checked: {3}, " +
                "cancelled: {4}",
                standardButtonClicked,
                tempCustomButton,
                tempRadioButton,
                checkboxChecked,
                standardButtonClicked==TaskDialogStandardButton.Cancel);
        }
    }
}
