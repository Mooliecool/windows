// DocViewerXps SDK Sample - Window1.xaml.cs
// Copyright (c) Microsoft Corporation. All rights reserved. 

using System;
using System.IO;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Xps.Packaging;


namespace SDKSample
{
    // ----------------------------- Class Window1 ----------------------------
    /// <summary>
    ///   Demonstrates the opening of an XPS document with DocumentViewer.</summary>
    public partial class Window1 : Window
    {
        #region Constructors
        public Window1() : base()
        {
            InitializeComponent();

            // Add the File|Open Command
            AddCommandBindings(ApplicationCommands.Open, OpenCommandHandler);
            // Add the File|Exit Command
            AddCommandBindings(ApplicationCommands.Close, CloseCommandHandler);
        }
        #endregion Constructors


        #region Private Methods
        // ----------------------- OpenCommandHandler -------------------------
        /// <summary>
        ///   Opens an existing XPS document and displays
        ///   it with a DocumentViewer./// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OpenCommandHandler(
            object sender, ExecutedRoutedEventArgs e)
        {
            //Display a file open dialog to find and existing document
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.InitialDirectory = GetContentFolder();
            dlg.Filter = "Xps Documents (*.xps)|*.xps";
            dlg.FilterIndex = 1;
            if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                // If there's an existing document open, close it.
                if (_xpsDocument != null)
                    _xpsDocument.Close();

                // Create an XpsDocument for the file specified by the user.
                try
                {
                    _xpsDocument = new
                        XpsDocument(dlg.FileName, System.IO.FileAccess.Read);
                }
                catch (UnauthorizedAccessException)
                {
                    System.Windows.MessageBox.Show( 
                        String.Format("Unable to access {0}", dlg.FileName ) );
                    return;
                }
                // For optimal performance the XPS document should be remain
                // open while its FixedDocumentSequence is active in the
                // DocumentViewer control.  When the XPS document is opened
                // with the XpsDocument constructor ("new XpsDocument" above) a
                // reference to it is automatically added to the PackageStore.
                // The PackStore is a static application collection that contains
                // a reference to each open package along the package's URI as
                // a key.  Adding a reference of the XPS package to the
                // PackageStore keeps the package open and avoids repeated opens
                // and closes while the document content is being accessed by
                // DocumentViewer control.  The XpsDocument.Dispose() method 
                // automatically removes the package from the PackageStore after
                // the document is removed from the DocumentViewer control and
                // is no longer in use.
                docViewer.Document = _xpsDocument.GetFixedDocumentSequence();
                _fileName = dlg.FileName;
             }
        }// end:OpenCommandHandler()
     

        // ------------------------ CloseCommandHandler -----------------------
        /// <summary>
        ///   File|Exit handler - Closes current XPS document.</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CloseCommandHandler(object sender, ExecutedRoutedEventArgs e)
        {
            this.Close();
        }// end:CloseCommandHandler()


        // ------------------------ AddCommandBindings ------------------------
        /// <summary>
        ///     Registers menu commands (helper method).</summary>
        /// <param name="command"></param>
        /// <param name="handler"></param>
        private void AddCommandBindings(ICommand command, ExecutedRoutedEventHandler handler)
        {
            CommandBinding cmdBindings = new CommandBinding(command);
            cmdBindings.Executed += handler;
            CommandBindings.Add(cmdBindings);
        }// end:AddCommandBindings()


        // ------------------------- GetContentFolder -------------------------
        /// <summary>
        ///   Locates and returns the path to the "Content\" folder
        ///   containing the fixed document for the sample.</summary>
        /// <returns>
        ///   The path to the fixed document "Content\" folder.</returns>
        private string GetContentFolder()
        {
            // Get the path to the current directory and its length.
            string contentDir = Directory.GetCurrentDirectory();
            int dirLength = contentDir.Length;

            // If we're in "...\bin\debug", move up to the root.
            if (contentDir.ToLower().EndsWith(@"\bin\debug"))
                contentDir = contentDir.Remove(dirLength - 10, 10);

            // If we're in "...\bin\release", move up to the root.
            else if (contentDir.ToLower().EndsWith(@"\bin\release"))
                contentDir = contentDir.Remove(dirLength - 12, 12);

            // If there's a "Content" subfolder, that's what we want.
            if (Directory.Exists(contentDir + @"\Content"))
                contentDir = contentDir + @"\Content";

            // Return the "Content\" folder (or the "current"
            // directory if we're executing somewhere else).
            return contentDir;
        }// end:GetContentFolder()
        #endregion Private Methods


        #region Private Members
        private XpsDocument             _xpsDocument;
        private string                  _fileName;
        #endregion  Private Members

    }// end:partial class Window1

}// end:namespace SDKSample
