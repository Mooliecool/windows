/***************************************** Module Header *****************************\
* Module Name:  Connect.cs
* Project:      CSVSAddInCommandEvents
* Copyright (c) Microsoft Corporation.
* 
* This sample demonstrates how to close the document which is already open in another
* editor.
* 
* In Visual Studio, a specific editor cannot open the document which is already open by
* another editor.  For example, if a .cs file is already open with
* Code Editor, trying to open that .cs file with XML Editor will cause a dialog
* comes out saying "The document 'xxx.cs' is already open.  Do you want to close
* it?"
* Another example is, in VC++ project, the Resource View toolwindow will always trying
* to open the .rc file in a invisible editor(IVsInvisibleEditor) and track its change
* to update the information in its tree view.  So if we want to open the .rc file with
* Code Editor(right-click -> View Code) for seeing its source code, the same
* dialog will appear.
* 
* In this sample, it shows how to use the IVsRunningDocumentTable interface to
* access the RDT(Running Document Table), find out the opened document and lock it,
* then trying to close it, so that other editors can open the same document without the
* dialog prompt showing.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 01/28/2010 04:35 PM Wesley Yao Created
\*************************************************************************************/

using System;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.ComponentModel.Design;
using Microsoft.Win32;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.VisualStudio.OLE.Interop;
using Microsoft.VisualStudio.Shell;

namespace Microsoft.CSVSPackageCloseOpenedDocument
{
    /// <summary>
    /// This is the class that implements the package exposed by this assembly.
    ///
    /// The minimum requirement for a class to be considered a valid package for
    /// Visual Studio is to implement the IVsPackage interface and register itself with
    /// the shell.
    /// This package uses the helper classes defined inside the Managed Package
    /// Framework (MPF) to do it: it derives from the Package class that provides the
    /// implementation of the IVsPackage interface and uses the registration attributes
    /// defined in the framework to register itself and its components with the shell.
    /// </summary>
    // This attribute tells the registration utility (regpkg.exe) that this class needs
    // to be registered as package.
    [PackageRegistration(UseManagedResourcesOnly = true)]
    // A Visual Studio component can be registered under different regitry roots; for
    // instance when you debug your package you want to register it in the experimental
    // hive. This attribute specifies the registry root to use if no one is provided to
    // regpkg.exe with the /root switch.
    [DefaultRegistryRoot("Software\\Microsoft\\VisualStudio\\9.0")]
    // This attribute is used to register the informations needed to show the this
    // package in the Help/About dialog of Visual Studio.
    [InstalledProductRegistration(false, "#110", "#112", "1.0", IconResourceID = 400)]
    // In order be loaded inside Visual Studio in a machine that has not the VS SDK
    // installed, package needs to have a valid load key (it can be requested at 
    // http://msdn.microsoft.com/vstudio/extend/). This attributes tells the shell that
    // this package has a load key embedded in its resources.
    [ProvideLoadKey("Standard",
        "1.0",
        "CSVSPackageCloseOpenedDocument",
        "Microsoft",
        1)]
    // This attribute is needed to let the shell know that this package exposes
    // some menus.
    [ProvideMenuResource(1000, 1)]
    [Guid(GuidList.guidCSVSPackageCloseOpenedDocumentPkgString)]
    public sealed class CSVSPackageCloseOpenedDocumentPackage : Package
    {
        /// <summary>
        /// Default constructor of the package.
        /// Inside this method you can place any initialization code that does not
        /// require any Visual Studio service because at this point the package object
        /// is created but not sited yet inside Visual Studio environment. The place
        /// to do all the other initialization is the Initialize method.
        /// </summary>
        public CSVSPackageCloseOpenedDocumentPackage()
        {
            Trace.WriteLine(string.Format(CultureInfo.CurrentCulture,
                "Entering constructor for: {0}",
                this.ToString()));
        }



        /////////////////////////////////////////////////////////////////////////////
        // Overriden Package Implementation
        #region Package Members

        /// <summary>
        /// Initialization of the package; this method is called right after the package is sited, so this is the place
        /// where you can put all the initilaization code that rely on services provided by VisualStudio.
        /// </summary>
        protected override void Initialize()
        {
            Trace.WriteLine (string.Format(CultureInfo.CurrentCulture, "Entering Initialize() of: {0}", this.ToString()));
            base.Initialize();

            // Add our command handlers for menu (commands must exist in the .vsct file)
            OleMenuCommandService mcs = GetService(typeof(IMenuCommandService)) as OleMenuCommandService;
            if ( null != mcs )
            {
                // Create the command for the menu item.
                CommandID menuCommandID = new CommandID(GuidList.guidCSVSPackageCloseOpenedDocumentCmdSet, (int)PkgCmdIDList.cmdidCSVSPackageCloseOpenedDocument);
                MenuCommand menuItem = new MenuCommand(MenuItemCallback, menuCommandID );
                mcs.AddCommand( menuItem );
            }
        }
        #endregion

        /// <summary>
        /// This function is the callback used to execute a command when the a menu
        /// item is clicked. See the Initialize method to see how the menu item is
        /// associated to this function using the OleMenuCommandService service and
        /// the MenuCommand class.
        /// </summary>
        private void MenuItemCallback(object sender, EventArgs e)
        {
            // Call CloseAndOpenInXMLEditor method for a project file, if it's already
            // in the RDT, close it then open it with XML Editor again.
            CloseAndOpenInXMLEditor(@"E:\Projects\TestProject\TestProject\TestProject.cs");
        }

        private void CloseAndOpenInXMLEditor(string filePath)
        {
            IVsHierarchy ppHier = null;
            uint pitemid = Microsoft.VisualStudio.VSConstants.VSITEMID_NIL;
            IntPtr ppunkDocData = IntPtr.Zero;
            uint pdwCookie = Microsoft.VisualStudio.VSConstants.VSITEMID_NIL;

            try
            {
                // Get the IVsRunningDocumentTable interface and cast it to
                // IVsRunningDocumentTable2 interface.
                IVsRunningDocumentTable rdt =
                    GetService(typeof(SVsRunningDocumentTable))
                    as IVsRunningDocumentTable;
                IVsRunningDocumentTable2 rdt2 = rdt as IVsRunningDocumentTable2;

                // Find the opened document(.rc file) from the RDT.
                rdt.FindAndLockDocument((uint)_VSRDTFLAGS.RDT_NoLock, filePath,
                    out ppHier, out pitemid, out ppunkDocData, out pdwCookie);
                if (ppunkDocData != IntPtr.Zero)
                {
                    // Close the opened document.
                    Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(
                        rdt2.CloseDocuments((uint)__FRAMECLOSE.FRAMECLOSE_SaveIfDirty,
                        null, pdwCookie));
                    ppunkDocData = IntPtr.Zero;
                }

                IVsInvisibleEditorManager spIEM;
                IVsInvisibleEditor invisibleEditor = null;
                IVsWindowFrame winFrame = null;
                Guid logicalView = Microsoft.VisualStudio.VSConstants.LOGVIEWID_Primary;

                // Get the IVsInvisibleEditorManager interface.
                spIEM = (IVsInvisibleEditorManager)
                    GetService(typeof(IVsInvisibleEditorManager));
                // Register a invisible editor, then the specific document will be
                // loaded into the RDT.
                spIEM.RegisterInvisibleEditor(filePath, null,
                    (uint)_EDITORREGFLAGS.RIEF_ENABLECACHING,
                    null, out invisibleEditor);

                // Get the IVsUIShellOpenDocument interface.
                IVsUIShellOpenDocument uiShellOpenDocument =
                    GetService(typeof(SVsUIShellOpenDocument))
                    as IVsUIShellOpenDocument;
                // Guid of the Microsoft XML editor
                Guid guidXMLEditor = new Guid("{FA3CD31E-987B-443A-9B81-186104E8DAC1}");
                rdt.FindAndLockDocument((uint)_VSRDTFLAGS.RDT_NoLock, filePath,
                    out ppHier, out pitemid, out ppunkDocData, out pdwCookie);
                // Open the document in XML editor.
                Microsoft.VisualStudio.ErrorHandler.ThrowOnFailure(
                    uiShellOpenDocument.OpenSpecificEditor((uint)0, filePath,
                    ref guidXMLEditor, "", ref logicalView, "XML Editor",
                    ppHier as IVsUIHierarchy, pitemid,
                    ppunkDocData, this, out winFrame));
                // Show the editor window.
                winFrame.Show();
            }
            catch (Exception e)
            {
                System.Windows.Forms.MessageBox.Show(e.Message);
            }
        }
    }
}