#region Using directives

using System;
using System.Collections;
using System.Text;

#endregion

namespace Microsoft.Samples.WPFNotepad
{
    /// <summary>
    /// define tool tip message for font dialog
    /// </summary>
    class ToolTipMessage
    {
        /// <summary>
        /// hashtable to hold all the message.
        /// </summary>
        static Hashtable table = new Hashtable();
        /// <summary>
        /// method to fill up the hastable.
        /// </summary>
        static ToolTipMessage()
        {
            table.Add("OKButton", "Close the dialog and save any changes you have made");
            table.Add("CancelButton", "Close the dialog without saving any changes you have made");
            
            table.Add("StrikeCheckBox", "Check/uncheck to draw/undraw StrikeLine");
            table.Add("UnderLineCheckBox", "Check/uncheck to draw/undraw UnderLine");
            table.Add("BaseLineCheckBox", "Check/uncheck to draw/undraw BaseLine");
            table.Add("OverLineCheckBox", "Check/uncheck to draw/undraw OverLine");
            table.Add("TestRichTextBox", "Sample font, size, weight, color, etc");
            table.Add("TextColorListBox", "List popular colors for text");
            table.Add("TextColorTextBox", "Input to find an available color");
            table.Add("FontSizeListBox", "List available sizes in point for specified font");
            table.Add("FontStretchListBox", "List available stretchs for specified font");
            table.Add("FontWeightListBox", "List available weights for specified font");
            table.Add("FontStyleListBox", "List available styles for specified font");
            table.Add("FontFamilyListBox", "List available System fonts");
            table.Add("FontSizeTextBox", "Input to find an available font Size");
            table.Add("FontStretchTextBox", "Input to find an available font Stretch");
            table.Add("FontWeightTextBox", "Input to find an available font Weight");
            table.Add("FontStyleTextBox", "Input to find an available font Style");
            table.Add("FontFamilyTextBox", "Input to find an available System font");
        }
        /// <summary>
        /// return the hastable containing all the tooltip messages. using the Control Name as key to retrieve the message.
        /// </summary>
        /// <param name="ControlID">the key to return a specific message.</param>
        /// <returns>return a string that describle the usage of each control in the Font dialog</returns>
        public static string GetToolTipMessage(string ControlID)
        {
            return (string)table[ControlID];

        }
    }
}
