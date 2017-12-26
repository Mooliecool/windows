using System;
using System.ComponentModel;
using System.Text;

namespace Microsoft.SDK.Samples.VistaBridge.Library
{
    /// <summary>
    /// The event data associated with <see cref="Microsoft.SDK.Samples.VistaBridge.Library.CommonFileDialog.FolderChanging"/> event.
    /// </summary>
    /// 
    public class CommonFileDialogFolderChangeEventArgs : CancelEventArgs
    {
        /// <summary>
        /// Creates a new instance of this class.
        /// </summary>
        /// <param name="folder">The name of the folder.</param>
        public CommonFileDialogFolderChangeEventArgs(string folder)
        {
            this.folder = folder;
        }

        private string folder;
        /// <summary>
        /// Gets or sets the name of the folder.
        /// </summary>
        public string Folder
        {
            get { return folder; }
            set { folder = value; }
        }

    }
}
