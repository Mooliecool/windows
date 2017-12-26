using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.Shell;
using System.IO;
using System.Windows.Controls;
using Microsoft.VisualStudio;
using EnvDTE;

namespace Company.VSPackageWPFToolWindow
{
    /// <summary>
    /// This class implements the tool window exposed by this package and hosts a user control.
    ///
    /// In Visual Studio tool windows are composed of a frame (implemented by the shell) and a pane, 
    /// usually implemented by the package implementer.
    ///
    /// This class derives from the ToolWindowPane class provided from the MPF in order to use its 
    /// implementation of the IVsWindowPane interface.
    /// </summary>
    [Guid("1c0ed058-d9bf-44f0-97fa-18ae474a30f5")]
    public class MyToolWindow : ToolWindowPane, Microsoft.VisualStudio.OLE.Interop.IServiceProvider
    {
        // This is the user control hosted by the tool window; it is exposed to the base class 
        // using the Window property. Note that, even if this class implements IDispose, we are
        // not calling Dispose on this object. This is because ToolWindowPane calls Dispose on 
        // the object returned by the Window property.
        private MyControl control;
        private WPFControl wpfControl;

        /// <summary>
        /// Standard constructor for the tool window.
        /// </summary>
        public MyToolWindow() :
            base(null)
        {
            // Set the window title reading it from the resources.
            this.Caption = Resources.ToolWindowTitle;
            // Set the image that will appear on the tab of the window frame
            // when docked with an other window
            // The resource ID correspond to the one defined in the resx file
            // while the Index is the offset in the bitmap strip. Each image in
            // the strip being 16x16.
            this.BitmapResourceID = 301;
            this.BitmapIndex = 1;


            control = new MyControl();
            wpfControl = control.WPFControl;
        }

        public override void OnToolWindowCreated()
        {
            base.OnToolWindowCreated();
            InitializeTreeViewContent();

        }

        private void InitializeTreeViewContent()
        {
            DirectoryInfo myDocInfo =
                new DirectoryInfo(
                    Environment.GetFolderPath(
                        Environment.SpecialFolder.MyDocuments
                    )
                );
            IntializeTreeViewContentRecursively(myDocInfo, wpfControl.WPFTreeView.Items);
        }

        private void IntializeTreeViewContentRecursively(
            DirectoryInfo myDocInfo, ItemCollection itemCollection)
        {            
            if (myDocInfo == null)
                return;

            try
            {
                TreeViewItem item = new TreeViewItem();
                item.Header = myDocInfo.Name;
                itemCollection.Add(item);

                DirectoryInfo[] subDirs = myDocInfo.GetDirectories();
                if (subDirs != null)
                {
                    foreach (DirectoryInfo dir in subDirs)
                        IntializeTreeViewContentRecursively(dir, item.Items);
                }

                FileInfo[] files = myDocInfo.GetFiles();
                if (files != null)
                {
                    foreach (FileInfo file in files)
                    {
                        TreeViewItem fileItem = new TreeViewItem();
                        fileItem.Header = file.Name;
                        fileItem.Tag = file.FullName;
                        // fileItem.MouseDoubleClick += new System.Windows.Input.MouseButtonEventHandler(fileItem_MouseDoubleClick);
                        item.Items.Add(fileItem);
                    }
                }
            }
            catch
            {
                return;
            }
        }

        //void fileItem_MouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        //{
        //    TreeViewItem item = sender as TreeViewItem;
        //    DTE dte = GetService(typeof(DTE)) as DTE;

        //    dte.ExecuteCommand("File.OpenFile", item.Tag as string);
            
        //}

        /// <summary>
        /// This property returns the handle to the user control that should
        /// be hosted in the Tool Window.
        /// </summary>
        override public IWin32Window Window
        {
            get
            {
                return (IWin32Window)control;
            }
        }


        #region IServiceProvider Members

        public int QueryService(ref Guid guidService, ref Guid riid, out IntPtr ppvObject)
        {
            ppvObject = IntPtr.Zero;
            return VSConstants.S_OK;
        }

        #endregion
    }
}
