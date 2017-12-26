// DocViewerStyling SDK Sample - Window1.xaml.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Input;
using System.Windows.Xps.Packaging;
using System.Security.Cryptography.X509Certificates;
using System.Xml;
using System.IO;
using System.Collections.Generic;


namespace SdkSample
{
    // ----------------------------- Class Window1 ----------------------------
    /// <summary>
    ///   Demonstrates the opening of an XPS document with DocumentViewer. </summary>
    public partial class Window1 : Window
    {
        #region Constructors
        public Window1()
            : base()
        {
            InitializeComponent();

            // Bind the Open Command
            AddCommandBindings(ApplicationCommands.Open, OpenCommandHandler);
            // Bind the Close Command (which is invoked by the Quit menu item)
            AddCommandBindings(ApplicationCommands.Close, CloseCommandHandler);

            // Add menu handlers to set the styles
            menuItemClose.Click += CloseMenuHandler;
            menuItemPlainStyle.Click += PlainStyleMenuHandler;
            menuItemVerticalStyle.Click += VerticalStyleMenuHandler;
            menuItemDefaultStyle.Click += DefaultStyleMenuHandler;
        }
        #endregion Constructors


        #region Private Methods
        // ----------------------- OpenCommandHandler -------------------------
        /// <summary>
        ///   Opens an existing XPS document and displays
        ///   it with a DocumentViewer. </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OpenCommandHandler(
            object sender, ExecutedRoutedEventArgs e)
        {
            //Display a file open dialog to find and existing document
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Xps Documents (*.xps)|*.xps";
            dlg.FilterIndex = 1;
            if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                if (_xpsDocument != null)
                {
                    _xpsDocument.Close();
                }
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
                menuItemClose.IsEnabled = true;
                menuStyle.IsEnabled = true;
            }
        }// end:OpenCommandHandler()


        // ------------------------ CloseCommandHandler -----------------------
        /// <summary>
        ///   File|Quit handler - Exits the application.</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CloseCommandHandler(object sender, ExecutedRoutedEventArgs e)
        {
            this.Close();
        }// end:CloseCommandHandler()


        // ------------------------- CloseMenuHandler ------------------------
        /// <summary>
        ///   File|Close handler - Closes current XPS document.</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void CloseMenuHandler(object sender, EventArgs e)
        {
            if (_xpsDocument != null)
            {
                _xpsDocument.Close();
                _xpsDocument = null;
            }
            docViewer.Document = null;
            menuItemClose.IsEnabled = false;
        }// end:CloseMenuHandler()


        // ------------------------ PlainStyleMenuHandler ---------------------
        /// <summary>
        ///   Style|Plain handler - Demonstrates restyling the DocumentViewer
        ///     by applying the plain style.</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PlainStyleMenuHandler(object sender, EventArgs e)
        {
            docViewer.Style = FindResource("plainStyleKey") as Style;
            menuItemDefaultStyle.IsEnabled = true;
            menuItemVerticalStyle.IsEnabled = true;
            menuItemPlainStyle.IsEnabled = false;
        }// end:PlainStyleMenuHandler()


        // ------------------------ VerticalStyleMenuHandler ---------------------
        /// <summary>
        ///   Style|Vertical handler - Demonstrates restyling the DocumentViewer
        ///     by applying the default style.</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void VerticalStyleMenuHandler(object sender, EventArgs e)
        {
            docViewer.Style = FindResource("verticalStyleKey") as Style;
            menuItemDefaultStyle.IsEnabled = true;
            menuItemVerticalStyle.IsEnabled = false;
            menuItemPlainStyle.IsEnabled = true;
        }// end:VerticalStyleMenuHandler()


        // ------------------------ DefaultStyleMenuHandler ---------------------
        /// <summary>
        ///   Style|Default handler - Restores the default style.</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DefaultStyleMenuHandler(object sender, EventArgs e)
        {
            docViewer.Style = FindResource(typeof(DocumentViewer)) as Style;
            menuItemDefaultStyle.IsEnabled = false;
            menuItemVerticalStyle.IsEnabled = true;
            menuItemPlainStyle.IsEnabled = true;
        }// end:DefaultStyleMenuHandler()


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

        #endregion Private Methods


        #region Private Members
        private XpsDocument             _xpsDocument;
        private string                  _fileName;
        #endregion Private Members

    }// end:partial class Window1

}// end:namespace SdkSample