using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.Shell;
using System.ComponentModel.Design;

namespace AllInOne.CSVSToolWindow
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
    [Guid("8c72909f-1d26-41e6-8e17-381f3ef34c01")]
    public class MyToolWindow : ToolWindowPane
    {
        // This is the user control hosted by the tool window; it is exposed to the base class 
        // using the Window property. Note that, even if this class implements IDispose, we are
        // not calling Dispose on this object. This is because ToolWindowPane calls Dispose on 
        // the object returned by the Window property.
        private MyControl control;

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

            // Create the toolbar.
            this.ToolBar = new CommandID(GuidList.guidCSVSToolWindowCmdSet,
                PkgCmdIDList.ToolbarID);
            this.ToolBarLocation = (int)VSTWT_LOCATION.VSTWT_TOP;

            // Create the handlers for the toolbar commands.
            var mcs = GetService(typeof(IMenuCommandService))
                as OleMenuCommandService;
            if (null != mcs)
            {
                var toolbarbtnCmdID = new CommandID(
                    GuidList.guidCSVSToolWindowCmdSet,
                    PkgCmdIDList.cmdidWindowsMediaOpen);
                var menuItem = new MenuCommand(new EventHandler(
                    ButtonHandler), toolbarbtnCmdID);
                mcs.AddCommand(menuItem);
            }

            control = new MyControl();
        }

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


        private void ButtonHandler(object sender, EventArgs arguments)
        {
            OpenFileDialog fd = new OpenFileDialog();
            fd.Filter = "media files |*.wmv;*.asf";
            fd.Multiselect = false;
            if (DialogResult.OK == fd.ShowDialog())
            {
                control.MediaPlayer.URL = fd.FileName;
            }
        }

    }
}
