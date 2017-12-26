using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Markup;
using Microsoft.SDK.Samples.VistaBridge.Library;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Security.Permissions;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Encapsulates a new-to-Vista Win32 TaskDialog window 
    /// - a powerful successor to the MessageBox available
    /// in previous versions of Windows.
    /// </summary>
    [SecurityPermissionAttribute(SecurityAction.InheritanceDemand, Flags = SecurityPermissionFlag.UnmanagedCode)]
    [ContentProperty("Controls")]
    public class TaskDialog : IDialogControlHost, IDisposable
    {
        // Global instance of TaskDialog, to be used by static Show() method.
        // As most parameters of a dialog created via static Show() will have
        // identical parameters, we'll create one TaskDialog and treat it
        // as a NativeTaskDialog generator for all static Show() calls.
        private static TaskDialog staticDialog;

        // Main current native dialog.
        private NativeTaskDialog nativeDialog;

        private List<TaskDialogButtonBase> buttons;
        private List<TaskDialogButtonBase> radioButtons;
        private List<TaskDialogButtonBase> commandLinks;
        private Window ownerWindow;

        #region Public Properties

        public event EventHandler<TaskDialogTickEventArgs> Tick;
        public event EventHandler<TaskDialogHyperlinkClickedEventArgs> HyperlinkClick;
        public event EventHandler<TaskDialogClosingEventArgs> Closing;
        public event EventHandler HelpInvoked;
        public event EventHandler Opened;

        public Window Owner
        {
            get { return ownerWindow; }
            set
            {
                ThrowIfDialogShowing("Dialog owner cannot be modified while dialog is showing.");
                ownerWindow = value;
            }
        }

        // Main content (maps to MessageBox's "message"). 
        private string content;
        public string Content
        {
            get { return content; }
            set
            {
                // Set local value, then update native dialog if showing.
                content = value;
                if (NativeDialogShowing)
                    nativeDialog.UpdateContent(content);
            }
        }

        private string instruction;
        public string Instruction
        {
            get { return instruction; }
            set
            {
                // Set local value, then update native dialog if showing.
                instruction = value;
                if (NativeDialogShowing)
                    nativeDialog.UpdateInstruction(instruction);
            }
        }

        private string caption;
        public string Caption
        {
            get { return caption; }
            set
            {
                ThrowIfDialogShowing("Dialog caption can't be set while dialog is showing.");
                caption = value;
            }
        }

        private string footerText;
        public string FooterText
        {
            get { return footerText; }
            set
            {
                // Set local value, then update native dialog if showing.
                footerText = value;
                if (NativeDialogShowing)
                    nativeDialog.UpdateFooterText(footerText);
            }
        }

        private string checkBoxText;
        public string CheckBoxText
        {
            get { return checkBoxText; }
            set
            {
                ThrowIfDialogShowing("Checkbox text can't be set while dialog is showing.");
                checkBoxText = value;
            }
        }

        private string expandedText;
        public string ExpandedText
        {
            get { return expandedText; }
            set
            {
                // Set local value, then update native dialog if showing.
                expandedText = value;
                if (NativeDialogShowing)
                    nativeDialog.UpdateExpandedText(expandedText);
            }
        }

        private bool expanded;
        public bool Expanded
        {
            get { return expanded; }
            set
            {
                ThrowIfDialogShowing("Expanded state of the dialog can't be modified while dialog is showing.");
                expanded = value;
            }
        }

        private string expandedControlText;
        public string ExpandedControlText
        {
            get { return expandedControlText; }
            set
            {
                ThrowIfDialogShowing("Expanded control text can't be set while dialog is showing.");
                expandedControlText = value;
            }
        }

        private string collapsedControlText;
        public string CollapsedControlText
        {
            get { return collapsedControlText; }
            set
            {
                ThrowIfDialogShowing("Collapsed control text can't be set while dialog is showing.");
                collapsedControlText = value;
            }
        }

        private bool cancelable;
        public bool Cancelable
        {
            get { return cancelable; }
            set
            {
                ThrowIfDialogShowing("Cancelable can't be set while dialog is showing.");
                cancelable = value;
            }
        }

        private TaskDialogStandardIcon mainIcon;
        public TaskDialogStandardIcon MainIcon
        {
            get { return mainIcon; }
            set
            {
                // Set local value, then update native dialog if showing.
                mainIcon = value;
                if (NativeDialogShowing)
                    nativeDialog.UpdateMainIcon(mainIcon);
            }
        }

        private TaskDialogStandardIcon footerIcon;
        public TaskDialogStandardIcon FooterIcon
        {
            get { return footerIcon; }
            set
            {
                // Set local value, then update native dialog if showing.
                footerIcon = value;
                if (NativeDialogShowing)
                    nativeDialog.UpdateFooterIcon(footerIcon);
            }
        }

        private TaskDialogStandardButtons standardButtons = TaskDialogStandardButtons.None;
        public TaskDialogStandardButtons StandardButtons
        {
            get { return standardButtons; }
            set
            {
                ThrowIfDialogShowing("Standard buttons can't be set while dialog is showing.");
                standardButtons = value;
            }
        }

        private DialogControlCollection<TaskDialogControl> controls;
        public DialogControlCollection<TaskDialogControl> Controls
        {
            // "Show protection" provided by collection itself, 
            // as well as individual controls.
            get { return controls; }
        }

        private bool hyperlinksEnabled;
        public bool HyperlinksEnabled
        {
            get { return hyperlinksEnabled; }
            set
            {
                ThrowIfDialogShowing("Hyperlinks can't be enabled/disabled while dialog is showing.");
                hyperlinksEnabled = value;
            }
        }

        private bool? checkBoxChecked = null;
        public bool? CheckBoxChecked
        {
            get 
            {
                if (!checkBoxChecked.HasValue)
                    return false;
                else
                    return checkBoxChecked; 
            }
            set
            {
                // Set local value, then update native dialog if showing.
                checkBoxChecked = value;
                if (NativeDialogShowing)
                    nativeDialog.UpdateCheckBoxChecked(checkBoxChecked.Value);
            }
        }

        private TaskDialogExpandedInformationLocation expansionMode;
        public TaskDialogExpandedInformationLocation ExpansionMode
        {
            get { return expansionMode; }
            set
            {
                ThrowIfDialogShowing("Expanded information mode can't be set while dialog is showing.");
                expansionMode = value;
            }
        }

        private TaskDialogStartupLocation startupLocation;
        public TaskDialogStartupLocation StartupLocation
        {
            get { return startupLocation; }
            set
            {
                ThrowIfDialogShowing("Startup location can't be changed while dialog is showing.");
                startupLocation = value;
            }
        }
        private TaskDialogProgressBar progressBar;
        public TaskDialogProgressBar ProgressBar
        {
            get { return progressBar; }
            set
            {
                ThrowIfDialogShowing("Progress bar can't be changed while dialog is showing");
                if (value != null)
                {
                    if (value.HostingDialog != null) 
                        throw new InvalidOperationException("Progress bar cannot be hosted in multiple dialogs.");

                    // Cannot have progress bar and marquee in same dialog.
                    if (marquee != null)
                    {
                        throw new NotSupportedException("Dialog cannot contain marquee progress bar and a regular progress bar.");
                    }
                    value.HostingDialog = this;
                }
                progressBar = value;
            }
        }
        private TaskDialogMarquee marquee;
        public TaskDialogMarquee Marquee
        {
            get { return marquee; }
            set
            {
                ThrowIfDialogShowing("Marquee can't be changed while dialog is showing.");
                if (value != null)
                {
                    if (value.HostingDialog != null)
                        throw new InvalidOperationException("Marquee cannot be hosted in multiple dialogs.");

                    // Cannot have progress bar and marquee in same dialog.
                    if (progressBar != null)
                    {
                        throw new NotSupportedException("Dialog cannot contain marquee progress bar and a regular progress bar.");
                    }
                    value.HostingDialog = this;
                }
                marquee = value;
            }
        }
        #endregion

        #region Constructors

        // Constructors.
 
        
        public TaskDialog()
        {
            if (!Helpers.RunningOnVista)
                throw new PlatformNotSupportedException("Task Dialog requires Windows Vista or later.");

            // Initialize various data structs.
            controls = new DialogControlCollection<TaskDialogControl>(this);
            buttons = new List<TaskDialogButtonBase>();
            radioButtons = new List<TaskDialogButtonBase>();
            commandLinks = new List<TaskDialogButtonBase>();
        }



        #endregion

        #region Static Show Methods

        // STATIC SHOW() METHODS -- these all forward to ShowCoreStatic()
        public static TaskDialogResult Show(string msg)
        {
            return ShowCoreStatic(
                msg, 
                TaskDialogDefaults.MainInstruction, 
                TaskDialogDefaults.Caption);
        }

        public static TaskDialogResult Show(string msg, string mainInstruction)
        {
            return ShowCoreStatic(
                msg, mainInstruction, 
                TaskDialogDefaults.Caption);
        }

        public static TaskDialogResult Show(string msg, string mainInstruction, string caption)
        {
            return ShowCoreStatic(msg, mainInstruction, caption);
        }
        #endregion

        #region Instance Show Methods
        // INSTANCE SHOW() METHODS -- these all forward to ShowCore()

        public TaskDialogResult Show()
        {
            return ShowCore();
        }
        #endregion

        #region Core Show Logic

        // CORE SHOW METHODS:
        // All static Show() calls forward here - 
        // it is responsible for retrieving
        // or creating our cached TaskDialog instance, getting it configured,
        // and in turn calling the appropriate instance Show.

        private static TaskDialogResult ShowCoreStatic(
            string msg, 
            string mainInstruction, 
            string caption)
        {
            // If no instance cached yet, create it.
            if (staticDialog == null)
            {
                // New TaskDialog will automatically pick up defaults when 
                // a new config structure is created as part of ShowCore().
                staticDialog = new TaskDialog();
            }

            // Set the few relevant properties, 
            // and go with the defaults for the others.
            staticDialog.content = msg;
            staticDialog.instruction = mainInstruction;
            staticDialog.caption = caption;

            return staticDialog.Show();
        }

        private TaskDialogResult ShowCore()
        {
            TaskDialogResult result;

            try
            {
                // Populate control lists, based on current 
                // contents - note we are somewhat late-bound 
                // on our control lists, to support XAML scenarios.
                SortDialogControls();

                // First, let's make sure it even makes 
                // sense to try a show.
                ValidateCurrentDialogSettings();

                // Create settings object for new dialog, 
                // based on current state.
                NativeTaskDialogSettings settings = 
                    new NativeTaskDialogSettings();
                ApplyCoreSettings(settings);
                ApplySupplementalSettings(settings);

                // Show the dialog.
                // NOTE: this is a BLOCKING call; the dialog proc callbacks
                // will be executed by the same thread as the 
                // Show() call before the thread of execution 
                // contines to the end of this method.
                nativeDialog = new NativeTaskDialog(settings, this);
                nativeDialog.NativeShow();

                // Build and return dialog result to public API - leaving it
                // null after an exception is thrown is fine in this case
                result = ConstructDialogResult(nativeDialog);
                checkBoxChecked = result.CheckBoxChecked;
            }
            finally
            {
                CleanUp();
                nativeDialog = null;
            }

            return result;
        }

        // Helper that looks at the current state of the TaskDialog and verifies
        // that there aren't any abberant combinations of properties.
        // NOTE that this method is designed to throw 
        // rather than return a bool.
        private void ValidateCurrentDialogSettings()
        {
            if (checkBoxChecked.HasValue &&
                checkBoxChecked.Value == true &&
                String.IsNullOrEmpty(checkBoxText))
                throw new InvalidOperationException(
                    "Checkbox text must be provided to enable the dialog checkbox.");

            // Progress bar validation.
            // Make sure we don't have both a progress bar and marquee, 
            // and that the progress bar values are valid.
            // the Win32 API will valiantly try to rationalize 
            // bizarre min/max/value combinations, but we'll save
            // it the trouble by validating.
            if (progressBar != null && marquee != null)
                throw new NotSupportedException(
                    "Can't display both a progress bar and a marquee in same dialog.");
            if (progressBar != null)
                if (!progressBar.HasValidValues)
                    throw new ArgumentException( 
                        "Progress bar must have a value between the minimum and maxium values.");

            // Validate Buttons collection.
            // Make sure we don't have buttons AND 
            // command-links - the Win32 API treats them as different
            // flavors of a single button struct.
            if (buttons.Count > 0 && commandLinks.Count > 0)
                throw new NotSupportedException(
                    "Dialog cannot display both non-standard buttons and command links.");
            if (buttons.Count > 0 && standardButtons != TaskDialogStandardButtons.None)
                throw new NotSupportedException(
                    "Dialog cannot display both non-standard buttons and standard buttons.");
        }

        // Analyzes the final state of the NativeTaskDialog instance and creates the 
        // final TaskDialogResult that will be returned from the public API
        private TaskDialogResult ConstructDialogResult(NativeTaskDialog native)
        {
            Debug.Assert(native.ShowState == NativeDialogShowState.Closed, "dialog result being constructed for unshown dialog.");

            string customButton = null;
            string radioButton = null;
            bool isCheckBoxChecked = false;
            TaskDialogButtonBase button;

            TaskDialogStandardButton standardButton = MapButtonIdToStandardButton(native.SelectedButtonID);

            // If returned ID isn't a standard button, let's fetch 
            if (standardButton == TaskDialogStandardButton.None)
            {
                button = GetButtonForId(native.SelectedButtonID);
                if (button == null)
                    throw new InvalidOperationException("Received bad control ID from Win32 callback.");
                customButton = button.Name;
            }

            // If there were radio buttons and one was selected, figure out which one
            if (radioButtons.Count > 0 && native.SelectedRadioButtonID != SafeNativeMethods.NO_DEFAULT_BUTTON_SPECIFIED)
            {
                button = GetButtonForId(native.SelectedRadioButtonID);
                if (button == null)
                    throw new InvalidOperationException("Received bad control ID from Win32 callback.");
                radioButton = button.Name;
            }
            isCheckBoxChecked = native.CheckBoxChecked;

            return new TaskDialogResult(
                standardButton,
                customButton,
                radioButton,
                isCheckBoxChecked);
        }

        public void Close()
        {
            if (!NativeDialogShowing)
                throw new InvalidOperationException(
                    "Attempting to close a non-showing dialog.");

            nativeDialog.NativeClose();
            // TaskDialog's own cleanup code - 
            // which runs post show - will handle disposal of native dialog.
        }

        #endregion

        #region Configuration Construction

        private void ApplyCoreSettings(NativeTaskDialogSettings settings)
        {
            ApplyGeneralNativeConfiguration(settings.NativeConfiguration);
            ApplyTextConfiguration(settings.NativeConfiguration);
            ApplyOptionConfiguration(settings.NativeConfiguration);
            ApplyControlConfiguration(settings);
        }

        private void ApplyGeneralNativeConfiguration(SafeNativeMethods.TASKDIALOGCONFIG dialogConfig)
        {
            // If an owner wasn't specifically specified, 
            // we'll use the app's main window.
            Window currentOwner = ownerWindow;
            if (currentOwner == null)
                currentOwner = Helpers.GetDefaultOwnerWindow();

            if (currentOwner != null)
                dialogConfig.hwndParent = (new WindowInteropHelper(currentOwner)).Handle;

            // Other miscellaneous sets.
            dialogConfig.MainIcon =
                new SafeNativeMethods.TASKDIALOGCONFIG_ICON_UNION((int)mainIcon);
            dialogConfig.FooterIcon =
                new SafeNativeMethods.TASKDIALOGCONFIG_ICON_UNION((int)footerIcon);
            dialogConfig.dwCommonButtons =
                (SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_FLAGS)standardButtons;
        }

        private void ApplyTextConfiguration(SafeNativeMethods.TASKDIALOGCONFIG dialogConfig)
        {
            // Set important text properties - 
            // note that nulls or empty strings are fine here.
            // TODO: Rationalize default handling - 
            // do we even need to set defaults first?
            dialogConfig.pszContent = content;
            dialogConfig.pszWindowTitle = caption;
            dialogConfig.pszMainInstruction = instruction;
            dialogConfig.pszExpandedInformation = expandedText;
            dialogConfig.pszExpandedControlText = expandedControlText;
            dialogConfig.pszCollapsedControlText = collapsedControlText;
            dialogConfig.pszFooter = footerText;
            dialogConfig.pszVerificationText = checkBoxText;

        }

        private void ApplyOptionConfiguration(SafeNativeMethods.TASKDIALOGCONFIG dialogConfig)
        {
            // Handle options - start with no options set.
            SafeNativeMethods.TASKDIALOG_FLAGS options = SafeNativeMethods.TASKDIALOG_FLAGS.NONE;
            if (cancelable)
                options |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_ALLOW_DIALOG_CANCELLATION;
            if (checkBoxChecked.HasValue && checkBoxChecked.Value)
                options |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_VERIFICATION_FLAG_CHECKED;
            if (hyperlinksEnabled)
                options |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_ENABLE_HYPERLINKS;
            if (expanded)
                options |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_EXPANDED_BY_DEFAULT;
            if (Tick != null)
                options |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_CALLBACK_TIMER;
            if (startupLocation == TaskDialogStartupLocation.CenterOwner)
                options |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_POSITION_RELATIVE_TO_WINDOW;

            // Note: no validation required, as we allow this to 
            // be set even if there is no expanded information 
            // text because that could be added later.
            // Default for Win32 API is to expand into (and after) 
            // the content area.
            if (expansionMode == TaskDialogExpandedInformationLocation.ExpandFooter)
                options |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_EXPAND_FOOTER_AREA;

            // Finally, apply options to config.
            dialogConfig.dwFlags = options;
        }

        // Builds the actual configuration 
        // that the NativeTaskDialog (and underlying Win32 API)
        // expects, by parsing the various control 
        // lists, marshalling to the unmanaged heap, etc.

        private void ApplyControlConfiguration(NativeTaskDialogSettings settings)
        {
            // Deal with progress bars/marquees.
            if (marquee != null)
                settings.NativeConfiguration.dwFlags |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_SHOW_MARQUEE_PROGRESS_BAR;
            else if (progressBar != null)
                settings.NativeConfiguration.dwFlags |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_SHOW_PROGRESS_BAR;

            // Build the native struct arrays that NativeTaskDialog 
            // needs - though NTD will handle
            // the heavy lifting marshalling to make sure 
            // all the cleanup is centralized there.
            if (buttons.Count > 0 || commandLinks.Count > 0)
            {
                // These are the actual arrays/lists of 
                // the structs that we'll copy to the 
                // unmanaged heap.
                List<TaskDialogButtonBase> sourceList = (
                    buttons.Count > 0 ? buttons : commandLinks);
                settings.Buttons = BuildButtonStructArray(sourceList);

                // Apply option flag that forces all 
                // custom buttons to render as command links.
                if (commandLinks.Count > 0)
                    settings.NativeConfiguration.dwFlags |= 
                      SafeNativeMethods.TASKDIALOG_FLAGS.TDF_USE_COMMAND_LINKS;

                // Set default button and add elevation icons 
                // to appropriate buttons.
                settings.NativeConfiguration.nDefaultButton = 
                    FindDefaultButtonId(sourceList);

                ApplyElevatedIcons(settings, sourceList);
            }
            if (radioButtons.Count > 0)
            {
                settings.RadioButtons = BuildButtonStructArray(radioButtons);

                // Set default radio button - radio buttons don't support.
                int defaultRadioButton = FindDefaultButtonId(radioButtons);
                settings.NativeConfiguration.nDefaultRadioButton = 
                    defaultRadioButton;

                if (defaultRadioButton == 
                    SafeNativeMethods.NO_DEFAULT_BUTTON_SPECIFIED)
                    settings.NativeConfiguration.dwFlags |= SafeNativeMethods.TASKDIALOG_FLAGS.TDF_NO_DEFAULT_RADIO_BUTTON;
            }
        }

        private static SafeNativeMethods.TASKDIALOG_BUTTON[] BuildButtonStructArray(List<TaskDialogButtonBase> controls)
        {
            SafeNativeMethods.TASKDIALOG_BUTTON[] buttonStructs;
            TaskDialogButtonBase button;

            int totalButtons = controls.Count;
            buttonStructs = new SafeNativeMethods.TASKDIALOG_BUTTON[totalButtons];
            for (int i = 0; i < totalButtons; i++)
            {
                button = controls[i];
                buttonStructs[i] = new SafeNativeMethods.TASKDIALOG_BUTTON(button.Id, button.ToString());
            }
            return buttonStructs;
        }

        // Searches list of controls and returns the ID of 
        // the default control, or null if no default was specified.
        private static int FindDefaultButtonId(List<TaskDialogButtonBase> controls)
        {
            int found = SafeNativeMethods.NO_DEFAULT_BUTTON_SPECIFIED;
            foreach (TaskDialogButtonBase control in controls)
            {
                if (control.Default)
                {
                    // Check if we've found a default in this list already.
                    if (found != SafeNativeMethods.NO_DEFAULT_BUTTON_SPECIFIED)
                        throw new InvalidOperationException("Can't have more than one default button of a given type.");
                    return control.Id;
                }
            }
            return found;
        }

        private static void ApplyElevatedIcons(NativeTaskDialogSettings settings, List<TaskDialogButtonBase> controls)
        {
            foreach (TaskDialogButton control in controls)
            {
                if (control.ShowElevationIcon)
                {
                    if (settings.ElevatedButtons == null)
                        settings.ElevatedButtons = new List<int>();
                    settings.ElevatedButtons.Add(control.Id);
                }
            }
        }

        private void ApplySupplementalSettings(NativeTaskDialogSettings settings)
        {
            if (progressBar != null)
            {
                settings.ProgressBarMinimum = progressBar.Minimum;
                settings.ProgressBarMaximum = progressBar.Maximum;
                settings.ProgressBarState = progressBar.State;
                settings.ProgressBarValue = progressBar.Value;
            }
            else if (marquee != null)
                settings.ProgressBarState = marquee.State;

            if (HelpInvoked != null)
                settings.InvokeHelp = true;
        }

        // Here we walk our controls collection and 
        // sort the various controls by type. 
        private void SortDialogControls()
        {
            foreach (TaskDialogControl control in controls)
            {
                if (control is TaskDialogButtonBase && String.IsNullOrEmpty(((TaskDialogButtonBase)control).Text))
                {
                    if (control is TaskDialogCommandLink && String.IsNullOrEmpty(((TaskDialogCommandLink)control).Instruction))
                        throw new InvalidOperationException(
                            "Button text must be non-empty");
                }

                // Loop through child controls 
                // and sort the controls based on type.
                if (control is TaskDialogCommandLink)
                {
                    commandLinks.Add((TaskDialogCommandLink)control);
                }
                else if (control is TaskDialogRadioButton)
                {
                    if (radioButtons == null)
                        radioButtons = new List<TaskDialogButtonBase>();
                    radioButtons.Add((TaskDialogRadioButton)control);
                }
                else if (control is TaskDialogButtonBase)
                {
                    if (buttons == null)
                        buttons = new List<TaskDialogButtonBase>();
                    buttons.Add((TaskDialogButtonBase)control);
                }
                else if (control is TaskDialogProgressBar)
                {
                    if (progressBar != null)
                        throw new InvalidOperationException(
                           "Can't have more than one progress bar in dialog.");
                    progressBar = (TaskDialogProgressBar)control;
                }
                else if (control is TaskDialogMarquee)
                {
                    if (marquee != null)
                        throw new InvalidOperationException(
                            "Can't have more than one marquee in dialog.");
                    marquee = (TaskDialogMarquee)control;
                }
                else
                {
                    throw new ArgumentException("Unknown dialog control type.");
                }
            }
        }

        #endregion

        #region Helpers

        // Helper to map the standard button IDs returned by 
        // TaskDialogIndirect to the standard button ID enum - 
        // note that we can't just cast, as the Win32
        // typedefs differ incoming and outgoing.

        private static TaskDialogStandardButton MapButtonIdToStandardButton(int id)
        {
            switch ((SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID)id)
            {
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDOK:
                    return TaskDialogStandardButton.Ok;
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDCANCEL:
                    return TaskDialogStandardButton.Cancel;
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDABORT:
                    // Included for completeness in API - 
                    // we can't pass in an Abort standard button.
                    return TaskDialogStandardButton.None;
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDRETRY:
                    return TaskDialogStandardButton.Retry;
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDIGNORE:
                    // Included for completeness in API - 
                    // we can't pass in an Ignore standard button.
                    return TaskDialogStandardButton.None;
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDYES:
                    return TaskDialogStandardButton.Yes;
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDNO:
                    return TaskDialogStandardButton.No;
                case SafeNativeMethods.TASKDIALOG_COMMON_BUTTON_RETURN_ID.IDCLOSE:
                    return TaskDialogStandardButton.Close;
                default:
                    return TaskDialogStandardButton.None;
            }
        }

        private void ThrowIfDialogShowing(string message)
        {
            if (NativeDialogShowing)
                throw new NotSupportedException(message);
        }

        private bool NativeDialogShowing
        {
            get
            {
                return (nativeDialog != null)
                    && (nativeDialog.ShowState == NativeDialogShowState.Showing ||
                    nativeDialog.ShowState == NativeDialogShowState.Closing);
            }
        }

        // NOTE: we are going to require names be unique 
        // across both buttons and radio buttons,
        // even though the Win32 API allows them to be separate.
        private TaskDialogButtonBase GetButtonForId(int id)
        {
            return (TaskDialogButtonBase)controls.GetControlbyId(id);
        }

        #endregion

        #region IDialogControlHost Members

        // We're explicitly implementing this interface 
        // as the user will never need to know about it
        // or use it directly - it is only for the internal 
        // implementation of "pseudo controls" within 
        // the dialogs.

        // Called whenever controls are being added 
        // to or removed from the dialog control collection.
        bool IDialogControlHost.IsCollectionChangeAllowed()
        {
            // Only allow additions to collection if dialog is NOT showing.
            return !NativeDialogShowing;
        }

        // Called whenever controls have been added or removed.
        void IDialogControlHost.ApplyCollectionChanged()
        {
            // If we're showing, we should never get here - 
            // the changing notification would have thrown and the 
            // property would not have been changed.
            Debug.Assert(!NativeDialogShowing,
                "Collection changed notification received despite show state of dialog");
        }

        // Called when a control currently in the collection 
        // has a property changing - this is 
        // basically to screen out property changes that 
        // cannot occur while the dialog is showing
        // because the Win32 API has no way for us to 
        // propagate the changes until we re-invoke the Win32 call.
        bool IDialogControlHost.IsControlPropertyChangeAllowed(string propertyName, DialogControl control)
        {
            Debug.Assert(control is TaskDialogControl, 
                "Property changing for a control that is not a TaskDialogControl-derived type");
            Debug.Assert(propertyName != "Name", 
                "Name changes at any time are not supported - public API should have blocked this");

            bool canChange = false;

            if (!NativeDialogShowing)
                canChange = true;
            else
            {
                // If the dialog is showing, we can only 
                // allow some properties to change.
                switch (propertyName)
                {
                    // Properties that CAN'T be changed while dialog is showing.
                    case "Text":
                    case "Default":
                        canChange = false;
                        break;

                    // Properties that CAN be changed while dialog is showing.
                    case "ShowElevationIcon":
                    case "Enabled":
                        canChange = true;
                        break;
                    default:
                        Debug.Assert(true, "Unknown property name coming through property changing handler");
                        break;
                }
            }
            return canChange;
        }

        // Called when a control currently in the collection 
        // has a property changed - this handles propagating
        // the new property values to the Win32 API. 
        // If there isn't a way to change the Win32 value, then we
        // should have already screened out the property set 
        // in NotifyControlPropertyChanging.
        void IDialogControlHost.ApplyControlPropertyChange(string propertyName, DialogControl control)
        {
            // We only need to apply changes to the 
            // native dialog when it actually exists.
            if (NativeDialogShowing)
            {
                // One of the progress bar flavors?
                if (control is TaskDialogMarquee)
                {
                    if (propertyName == "State")
                        nativeDialog.UpdateProgressBarState(marquee.State);
                    else
                        Debug.Assert(true, "Unknown property being set");
                }
                else if (control is TaskDialogProgressBar)
                {
                    if (!progressBar.HasValidValues)
                        throw new ArgumentException(
                            "Progress bar must have a value between Minimum and Maximum.");
                    switch (propertyName)
                    {
                        case "State":
                            nativeDialog.UpdateProgressBarState(progressBar.State);
                            break;
                        case "Value":
                            nativeDialog.UpdateProgressBarValue(progressBar.Value);
                            break;
                        case "Minimum":
                        case "Maximum":
                            nativeDialog.UpdateProgressBarRange();
                            break;
                        default:
                            Debug.Assert(true, "Unknown property being set");
                            break;
                    }
                }
                else if (control is TaskDialogButton)
                {
                    TaskDialogButton button = (TaskDialogButton)control;
                    switch (propertyName)
                    {
                        case "ShowElevationIcon":
                            nativeDialog.UpdateElevationIcon(button.Id, button.ShowElevationIcon);
                            break;
                        case "Enabled":
                            if (control is TaskDialogRadioButton)
                            {
                                nativeDialog.UpdateRadioButtonEnabled(button.Id, button.Enabled);
                            }
                            else
                            {
                                nativeDialog.UpdateButtonEnabled(button.Id, button.Enabled);
                            }
                            break;
                        default:
                            Debug.Assert(true, "Unknown property being set");
                            break;
                    }
                }
                else
                {
                    // Do nothing with property change - 
                    // note that this shouldn't ever happen, we should have
                    // either thrown on the changing event, or we handle above.
                    Debug.Assert(true, "Control property changed notification not handled properly - being ignored");
                }
            }
            return;
        }

        #endregion

        #region Event Percolation Methods

        // All Raise*() methods are called by the 
        // NativeTaskDialog when various pseudo-controls
        // are triggered.
        internal void RaiseButtonClickEvent(int id)
        {
            // First check to see if the ID matches a custom button.
            TaskDialogButtonBase button = GetButtonForId(id);

            // If a custom button was found, 
            // raise the event - if not, it's a standard button, and
            // we don't support custom event handling for the standard buttons
            if (button != null)
                button.RaiseClickEvent();
        }

        internal void RaiseHyperlinkClickEvent(string link)
        {
            EventHandler<TaskDialogHyperlinkClickedEventArgs> handler = HyperlinkClick;
            if (handler != null)
            {
                handler(this, new TaskDialogHyperlinkClickedEventArgs(link));
            }
        }

        // Gives event subscriber a chance to prevent 
        // the dialog from closing, based on 
        // the current state of the app and the button 
        // used to commit. Note that we don't 
        // have full access at this stage to 
        // the full dialog state.
        internal int RaiseClosingEvent(int id)
        {
            EventHandler<TaskDialogClosingEventArgs> handler = Closing;
            if (handler != null)
            {

                TaskDialogButtonBase customButton = null;
                TaskDialogClosingEventArgs e = new TaskDialogClosingEventArgs();

                // Try to identify the button - is it a standard one?
                e.StandardButton = MapButtonIdToStandardButton(id);

                // If not, it had better be a custom button...
                if (e.StandardButton == TaskDialogStandardButton.None)
                {
                    customButton = GetButtonForId(id);

                    // ... or we have a problem.
                    if (customButton == null)
                        throw new InvalidOperationException("Bad button ID in closing event.");
                    e.CustomButton = customButton.Name;
                }

                // Raise the event and determine how to proceed.
                handler(this, e);
                if (e.Cancel)
                    return (int)HRESULT.S_FALSE;
            }
            // It's okay to let the dialog close.
            return (int)HRESULT.S_OK;
        }

        internal void RaiseHelpInvokedEvent()
        {
            EventHandler handler = HelpInvoked;
            if (handler != null)
                handler(this, EventArgs.Empty);
        }

        internal void RaiseOpenedEvent()
        {
            EventHandler handler = Opened;
            if (handler != null)
                handler(this, EventArgs.Empty);
        }

        internal void RaiseTickEvent(int ticks)
        {
            EventHandler<TaskDialogTickEventArgs> handler = Tick;
            if (handler != null)
                handler(this, new TaskDialogTickEventArgs(ticks));
        }

        #endregion

        #region Cleanup Code

        // Cleans up data and structs from a single 
        // native dialog Show() invocation.
        private void CleanUp()
        {
            // Reset values that would be considered 
            // 'volatile' in a given instance.
            if (progressBar != null)
            {
                progressBar.Reset();
            }
            if (marquee != null)
            {
                marquee.Reset();
            }

            // Clean out sorted control lists - 
            // though we don't of course clear the main controls collection,
            // so the controls are still around; we'll 
            // resort on next show, since the collection may have changed.
            if (buttons != null)
                buttons.Clear();
            if (commandLinks != null)
                commandLinks.Clear();
            if (radioButtons != null)
                radioButtons.Clear();
            progressBar = null;
            marquee = null;

            // Have the native dialog clean up the rest.
            if (nativeDialog != null)
                nativeDialog.Dispose();
        }


        // Dispose pattern - cleans up data and structs for 
        // a) any native dialog currently showing, and
        // b) anything else that the outer TaskDialog has.
        private bool disposed;

        public void Dispose() 
        { 
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        ~TaskDialog() 
        { 
            Dispose(false); 
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                disposed = true;

                if (disposing)
                {
                    // Clean up managed resources.
                    if (nativeDialog.ShowState == 
                        NativeDialogShowState.Showing)
                        nativeDialog.NativeClose();
                }

                // Clean up unmanaged resources SECOND, NTD counts on 
                // being closed before being disposed.
                if (nativeDialog != null)
                    nativeDialog.Dispose();
            }
        }

        #endregion
    }
}
