using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Microsoft.SDK.Samples.VistaBridge.Library;

namespace Microsoft.SDK.Samples.VistaBridge.Interop
{
    ///<summary>
    /// Encapsulates additional configuration needed by NativeTaskDialog
    /// that it can't get from the TASKDIALOGCONFIG struct.
    ///</summary>
    internal class NativeTaskDialogSettings
    {
        internal NativeTaskDialogSettings()
        {
            nativeConfiguration = new SafeNativeMethods.TASKDIALOGCONFIG();

            // Apply standard settings.
            nativeConfiguration.cbSize = (uint)Marshal.SizeOf(nativeConfiguration);
            nativeConfiguration.hwndParent = SafeNativeMethods.NO_PARENT;
            nativeConfiguration.hInstance = IntPtr.Zero;
            nativeConfiguration.dwFlags = SafeNativeMethods.TASKDIALOG_FLAGS.TDF_ALLOW_DIALOG_CANCELLATION;
            nativeConfiguration.dwCommonButtons = SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_FLAGS.TDCBF_OK_BUTTON;
            nativeConfiguration.MainIcon = new SafeNativeMethods.TASKDIALOGCONFIG_ICON_UNION(0);
            nativeConfiguration.FooterIcon = new SafeNativeMethods.TASKDIALOGCONFIG_ICON_UNION(0);
            nativeConfiguration.cxWidth = TaskDialogDefaults.IdealWidth;

            // Zero out all the custom button fields.
            nativeConfiguration.cButtons = 0;
            nativeConfiguration.cRadioButtons = 0;
            nativeConfiguration.pButtons = IntPtr.Zero;
            nativeConfiguration.pRadioButtons = IntPtr.Zero;
            nativeConfiguration.nDefaultButton = 0;
            nativeConfiguration.nDefaultRadioButton = 0;

            // Various text defaults.
            nativeConfiguration.pszWindowTitle = TaskDialogDefaults.Caption;
            nativeConfiguration.pszMainInstruction = TaskDialogDefaults.MainInstruction;
            nativeConfiguration.pszContent = TaskDialogDefaults.Content;
            nativeConfiguration.pszVerificationText = null;
            nativeConfiguration.pszExpandedInformation = null;
            nativeConfiguration.pszExpandedControlText = null;
            nativeConfiguration.pszCollapsedControlText = null;
            nativeConfiguration.pszFooter = null;
        }

        private int progressBarMinimum;
        public int ProgressBarMinimum
        {
            get { return progressBarMinimum; }
            set { progressBarMinimum = value; }
        }

        private int progressBarMaximum;
        public int ProgressBarMaximum
        {
            get { return progressBarMaximum; }
            set { progressBarMaximum = value; }
        }

        private int progressBarValue;
        public int ProgressBarValue
        {
            get { return progressBarValue; }
            set { this.progressBarValue = value; }
        }

        private TaskDialogProgressBarState progressBarState;
        public TaskDialogProgressBarState ProgressBarState
        {
            get { return progressBarState; }
            set { progressBarState = value; }
        }

        private bool invokeHelp;
        public bool InvokeHelp
        {
            get { return invokeHelp; }
            set { invokeHelp = value; }
        }

        private SafeNativeMethods.TASKDIALOGCONFIG nativeConfiguration;
        public SafeNativeMethods.TASKDIALOGCONFIG NativeConfiguration
        {
            get { return nativeConfiguration; }
            set { nativeConfiguration = value; }
        }

        private SafeNativeMethods.TASKDIALOG_BUTTON[] buttons;
        public SafeNativeMethods.TASKDIALOG_BUTTON[] Buttons
        {
            get { return buttons; }
            set { buttons = value; }
        }

        private SafeNativeMethods.TASKDIALOG_BUTTON[] radioButtons;
        public SafeNativeMethods.TASKDIALOG_BUTTON[] RadioButtons
        {
            get { return radioButtons; }
            set { radioButtons = value; }
        }

        private List<int> elevatedButtons;
        public List<int> ElevatedButtons
        {
            get { return elevatedButtons; }
            set { elevatedButtons = value; }
        }
    }
}
