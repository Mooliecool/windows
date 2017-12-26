using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Library;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Security.Permissions;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Provides access to a Vista Common File Dialog, which allows the user
    /// to select the filename and location for a saved file.
    /// </summary>
    /// <permission cref="System.Security.Permissions.FileDialogPermission">
    /// to save a file. Associated enumeration: <see cref="System.Security.Permissions.SecurityAction.LinkDemand"/>.
    /// </permission>
    [FileDialogPermissionAttribute(SecurityAction.LinkDemand, Save = true)]
    public sealed class CommonSaveFileDialog : CommonFileDialog
    {
        private NativeFileSaveDialog saveDialogCoClass;

        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        public CommonSaveFileDialog() : base() { }
        /// <summary>
        /// Creates a new instance of this class with the specified name.
        /// </summary>
        /// <param name="name">The name for this dialog.</param>
        public CommonSaveFileDialog(string name) : base(name) { }

        #region Public API specific to Save

        private bool overwritePrompt;

        /// <summary>
        /// Gets or sets a value that controls whether to prompt before 
        /// overwriting an existing file of the same name. 
        /// </summary>
        /// <permission cref="System.InvalidOperationException">
        /// This property cannot be changed when the dialog is showing.
        /// </permission>
        public bool OverwritePrompt
        {
            get { return overwritePrompt; }
            set 
            {
                ThrowIfDialogShowing("OverwritePrompt" + IllegalPropertyChangeString);
                overwritePrompt = value; 
            }
        }

        private bool createPrompt;
        /// <summary>
        /// Gets or sets a value that controls whether to prompt for creation 
        /// if the item returned in the save dialog does not exist. 
        /// </summary>
        /// <remarks>Note that this does not actually create the item.</remarks>
        /// <permission cref="System.InvalidOperationException">
        /// This property cannot be changed when the dialog is showing.
        /// </permission>
        public bool CreatePrompt
        {
            get { return createPrompt; }
            set 
            {
                ThrowIfDialogShowing("CreatePrompt" + IllegalPropertyChangeString);
                createPrompt = value; 
            }
        }

        private bool enableMiniMode;
        /// <summary>
        /// Gets or sets a value that controls whether to the save dialog 
        /// displays in expanded mode. 
        /// </summary>
        /// <remarks>Expanded mode controls whether the dialog
        /// shows folders for browsing or hides them.</remarks>
        /// <permission cref="System.InvalidOperationException">
        /// This property cannot be changed when the dialog is showing.
        /// </permission>
        public bool EnableMiniMode
        {
            get { return enableMiniMode; }
            set 
            {
                ThrowIfDialogShowing("EnableMiniMode" + IllegalPropertyChangeString);
                enableMiniMode = value; 
            }
        }

        private bool strictExtensions;
        /// <summary>
        /// Gets or sets a value that controls whether to ensure that the 
        /// returned file name has a file extension that matches the 
        /// currently selected file type. The dialog appends the correct 
        /// file extension if necessary.
        /// </summary>
        /// <permission cref="System.InvalidOperationException">
        /// This property cannot be changed when the dialog is showing.
        /// </permission>
        public bool StrictExtensions
        {
            get { return strictExtensions; }
            set
            {
                ThrowIfDialogShowing("StrictExtensions" + IllegalPropertyChangeString);
                strictExtensions = value;
            }
        }

        #endregion

        internal override void InitializeNativeFileDialog()
        {
            saveDialogCoClass = new NativeFileSaveDialog();
        }
 
        internal override IFileDialog GetNativeFileDialog()
        {
            Debug.Assert(saveDialogCoClass != null,
                "Must call Initialize() before fetching dialog interface");
            return (IFileDialog)saveDialogCoClass;
        }

        internal override void PopulateWithFileNames(
            System.Collections.ObjectModel.Collection<string> names)
        {
            IShellItem item;
            saveDialogCoClass.GetResult(out item);

            if (item == null)
                throw new InvalidOperationException(
                    "Retrieved a null shell item from dialog");
            names.Add(GetFileNameFromShellItem(item));
        }

        internal override void CleanUpNativeFileDialog()
        {
            if (saveDialogCoClass != null)
                Marshal.ReleaseComObject(saveDialogCoClass);
        }

        internal override SafeNativeMethods.FOS GetDerivedOptionFlags(SafeNativeMethods.FOS flags)
        {
            if (overwritePrompt)
                flags |= SafeNativeMethods.FOS.FOS_OVERWRITEPROMPT;
            if (createPrompt)
                flags |= SafeNativeMethods.FOS.FOS_CREATEPROMPT;
            if (!enableMiniMode)
                flags |= SafeNativeMethods.FOS.FOS_DEFAULTNOMINIMODE;
            if (strictExtensions)
                flags |= SafeNativeMethods.FOS.FOS_STRICTFILETYPES;
            return flags;
        }
    }
}
