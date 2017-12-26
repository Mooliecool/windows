using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using Microsoft.SDK.Samples.VistaBridge.Library;
using Microsoft.SDK.Samples.VistaBridge.Interop;
using System.Security.Permissions;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// Provides access to a Vista Common File Dialog, which allows the user
    /// to select one or more files.
    /// </summary>
    /// 
    [FileDialogPermissionAttribute(SecurityAction.LinkDemand, Open = true)]
    public sealed class CommonOpenFileDialog : CommonFileDialog
    {
        private NativeFileOpenDialog openDialogCoClass;

        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        public CommonOpenFileDialog() : base() { }
        /// <summary>
        /// Creates a new instance of this class with the specified name.
        /// </summary>
        /// <param name="name">The name for this dialog.</param>
        public CommonOpenFileDialog(string name) : base(name) { }

        #region Public API specific to Open

        /// <summary>
        /// gets a collection of the selected file names.
        /// </summary>
        /// <remarks>This property should only be used when the
        /// <see cref="Microsoft.SDK.Samples.VistaBridge.Library.CommonOpenFileDialog.MultiSelect"/>
        /// property is <b>true</b>.</remarks>
        public Collection<string> FileNames
        {
            get 
            {
                CheckFileNamesAvailable();
                return fileNames; 
            }
        }

        private bool multiselect;
        /// <summary>
        /// Gets or sets a value that controls whether the user can
        /// select more than one file.
        /// </summary>
        public bool MultiSelect
        {
            get { return multiselect; }
            set { multiselect = value; }
        }

        private bool foldersOnly;
        /// <summary>
        /// Gets or sets a value that controls whether the user can select
        /// folders or files.
        /// </summary>
        public bool FoldersOnly
        {
            get { return foldersOnly; }
            set { foldersOnly = value; }
        }

        #endregion

        internal override IFileDialog GetNativeFileDialog()
        {
            Debug.Assert(openDialogCoClass != null,
                "Must call Initialize() before fetching dialog interface");
            return (IFileDialog)openDialogCoClass;
        }

        internal override void InitializeNativeFileDialog()
        {
            openDialogCoClass = new NativeFileOpenDialog();
        }

        internal override void CleanUpNativeFileDialog()
        {
            if (openDialogCoClass != null)
                Marshal.ReleaseComObject(openDialogCoClass);
        }

        internal override void PopulateWithFileNames(Collection<string> names)
        {
            IShellItemArray resultsArray;
            uint count;
            
            openDialogCoClass.GetResults(out resultsArray);
            resultsArray.GetCount(out count);
            for (int i = 0; i < count; i++)
                names.Add(GetFileNameFromShellItem(GetShellItemAt(resultsArray, i)));
        }

        internal override SafeNativeMethods.FOS GetDerivedOptionFlags(SafeNativeMethods.FOS flags)
        {
            if (multiselect)
                flags |= SafeNativeMethods.FOS.FOS_ALLOWMULTISELECT;
            if (foldersOnly)
                flags |= SafeNativeMethods.FOS.FOS_PICKFOLDERS;

            return flags;
        }
    }
}
