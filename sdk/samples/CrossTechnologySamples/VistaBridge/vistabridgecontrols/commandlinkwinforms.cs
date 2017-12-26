using System;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.ComponentModel;
using Microsoft.SDK.Samples.VistaBridge.Library;

namespace Microsoft.SDK.Samples.VistaBridge.Controls
{
    /// <summary>
    /// Implements a CommandLink button that can be used in 
    /// WinForms user interfaces.
    /// </summary>
    public class CommandLinkWinForms : Button
    {
        // Add BS_COMMANDLINK style before control creation.
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;

                cp.Style = AddCommandLinkStyle(cp.Style);

                return (cp);
            }
        }

        // Let Windows handle the rendering.
        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        public CommandLinkWinForms()
        {
            FlatStyle = FlatStyle.System;
        }

        // Add Design-Time Support.

        // Increase default width.
        protected override System.Drawing.Size DefaultSize
        {
            get { return new System.Drawing.Size(180, 60); }
        }

        // Enable note text to be set at design-time.
        [Category("Appearance")]
        [Description("Specifies the supporting note text.")]
        [BrowsableAttribute(true)]
        [DefaultValue("(Note Text)")]
        public string NoteText
        {
            get { return (GetNote(this)); }
            set
            {
                SetNote(this, value);
            }
        }
        
        // Enable shield icon to be set at design-time.
        [Category("Appearance")]
        [Description("Indicates whether the button should be decorated with the security shield icon (Windows Vista only).")]
        [BrowsableAttribute(true)]
        [DefaultValue(false)]
        public bool ShieldIcon
        {
            get { return (shieldIconDisplayed); }
            set
            {
                shieldIconDisplayed = value;
                SetShieldIcon(this, this.shieldIconDisplayed);
            }
        }
        private bool shieldIconDisplayed;


        #region Interop helpers

        private static int AddCommandLinkStyle(int Style)
        {
            int newStyle = Style;

            // Only add BS_COMMANDLINK style on Windows Vista or above.
            // Otherwise, button creation will fail.
            if (Environment.OSVersion.Version.Major >= 6)
            {
                newStyle |= SafeNativeMethods.BS_COMMANDLINK;
            }

            return (newStyle);
        }

        private static string GetNote(System.Windows.Forms.Button Button)
        {
            IntPtr retVal = InteropHelper.SendMessage(
                Button.Handle, 
                SafeNativeMethods.BCM_GETNOTELENGTH,
                IntPtr.Zero, 
                IntPtr.Zero);

            // Add 1 for null terminator, to get the entire string back.
            int len = ((int)retVal) + 1;
            StringBuilder strBld = new StringBuilder(len);

            retVal = InteropHelper.SendMessage(
                Button.Handle, 
                SafeNativeMethods.BCM_GETNOTE, 
                ref len, 
                strBld);
            return (strBld.ToString());
        }

        private static void SetNote(System.Windows.Forms.Button button, string text)
        {
            // This call will be ignored on versions earlier than 
            // Windows Vista.
            InteropHelper.SendMessage(
               button.Handle, 
               SafeNativeMethods.BCM_SETNOTE, 
               0, 
               text);
        }

        static internal void SetShieldIcon(
         System.Windows.Forms.Button Button, bool Show)
        {
            IntPtr fRequired = new IntPtr(Show ? 1 : 0);
            InteropHelper.SendMessage(
               Button.Handle,
                SafeNativeMethods.BCM_SETSHIELD, 
                IntPtr.Zero, 
                fRequired);
        }

        #endregion
    }
}
