// DocumentMerge SDK Sample - Window1.xaml.cs
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Xps;
using System.Windows.Xps.Packaging;
using System.IO.Packaging;
using System.Printing;
using System.Collections;
using System.Collections.Generic;
using System.Windows.Media.Animation;


namespace SdkSample
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>

    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
            // Add the Open Command
            AddCommandBindings(ApplicationCommands.Open, OpenCommandHandler);
            // Add the Save Command
            //AddCommandBindings(ApplicationCommands.Save, SaveCommandHandler);
            AddCommandBindings(ApplicationCommands.Save, SaveCommandHandler);
            // Add the Exit/Close Command
            AddCommandBindings(ApplicationCommands.Close, CloseCommandHandler);

            _documentRollUp = new RollUpDocument();
            _documentCache = new Dictionary<string, XpsDocument>();
        }


        // ------------------------ OpenCommandHandler ------------------------
        /// <summary>
        ///   Opens an existing XPS document and displays it
        ///   in a DocumentViewer control.</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OpenCommandHandler(object sender, RoutedEventArgs e)
        {
            //Display a file open dialog to select an existing document
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.Filter = "XPS Document (*.xps)|*.xps";
            dlg.InitialDirectory = GetContentFolder();
            dlg.CheckFileExists = true;
            dlg.Multiselect = true;
            if (dlg.ShowDialog() ==  true)
            {
                foreach (string file in dlg.FileNames)
                {
                    AddDocumentsToSource(file);
                }
            }
            menuFileSave.IsEnabled = true;
        }


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


        private void AddDocumentsToSource(string fileName)
        {
            if (!_documentCache.ContainsKey(fileName))
            {
                XpsDocument xpsDocument = new XpsDocument(fileName, FileAccess.Read);
                _documentCache[fileName] = xpsDocument;
                FixedDocumentSequence docSeq = xpsDocument.GetFixedDocumentSequence();
                foreach (DocumentReference docRef in docSeq.References)
                {
                    DocumentItem item = new DocumentItem(fileName, docRef);
                    source.Items.Add(item);
                }
            }
        }


        /// <summary>
        /// Closes current Xps Document
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SaveCommandHandler(object sender, RoutedEventArgs e)
        {
            if (dest.Items.Count < 1)
            {
                MessageBox.Show("Please add document to list before saving",
                "No document to be combined", MessageBoxButton.OK, MessageBoxImage.Exclamation);
                return;
            }
            Microsoft.Win32.SaveFileDialog saveFileDialog = new Microsoft.Win32.SaveFileDialog();
            saveFileDialog.Filter = "Xps Documents (*.xps)|*.xps";
            saveFileDialog.FilterIndex = 1;

            if (saveFileDialog.ShowDialog() == true)
            {
                string destFile = saveFileDialog.FileName;
                if (File.Exists(destFile))
                {
                    File.Delete(destFile);
                }
                _documentRollUp.Uri = new Uri(destFile);
                _documentRollUp.Save();
            }

        }


        /// <summary>
        ///   Closes current XPS document</summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        ///!!!private void CloseCommandHandler(object sender, ExecutedRoutedEventArgs e)
        private void CloseCommandHandler(object sender, RoutedEventArgs e)
        {
            this.Close();
        }


        void SourceSelected(object sender, SelectionChangedEventArgs args)
        {
            sourcePage.Items.Clear();
            foreach (object item in args.AddedItems)
            {
                if (item is DocumentItem)
                {
                    DocumentItem docSrc = item as DocumentItem;
                    foreach (PageContent pageContent in docSrc.FixedDocument.Pages)
                    {
                        sourcePage.Items.Add(new PageItem(pageContent));
                    }
                }
            }
        }

        void DestSelected(object sender, SelectionChangedEventArgs args)
        {
            destPage.Items.Clear();
            int selectedIndex = dest.SelectedIndex;
            if (selectedIndex != -1)
            {
                int pageCount = _documentRollUp.GetPageCount(selectedIndex);
                for (int i = 0; i < pageCount; i++)
                {
                    PageContent pageContent = _documentRollUp.GetPage(selectedIndex, i);
                    destPage.Items.Add(new PageItem(pageContent));
                }
            }
        }

        void Window_DragOver(object sender, System.Windows.DragEventArgs e)
        {
            if (e.Data != null && e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                e.Effects = DragDropEffects.All;
            }
            else
            {
                e.Effects = DragDropEffects.None;
            }
        }

        void Window_OnDrop(object sender, System.Windows.DragEventArgs e)
        {
            if (e.Data != null && e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                foreach (string file in files)
                {
                    AddDocumentsToSource(file);

                }
            }
        }

        /// <summary>
        /// Helper function to find commands to handlers and register them
        /// </summary>
        /// <param name="command"></param>
        /// <param name="handler"></param>
        private void AddCommandBindings(ICommand command, ExecutedRoutedEventHandler handler)
        {
            CommandBinding cmdBindings = new CommandBinding(command);
            cmdBindings.Executed += handler;
            CommandBindings.Add(cmdBindings);
        }

        protected void DestDocList_OnDrop(object sender, System.Windows.DragEventArgs e)
        {
            if (e.Data != null && e.Data.GetDataPresent(typeof(List<DocumentItem>)))
            {
                List<DocumentItem> items = (List<DocumentItem>)e.Data.GetData(typeof(List<DocumentItem>));
                dest.BeginInit();
                dest.SelectedItems.Clear();
                DocumentItem firstItem = null;
                foreach (DocumentItem item in items)
                {
                    DocumentItem newItem = new DocumentItem(item);
                    dest.Items.Add(newItem);
                    // only select fist item
                    if (firstItem == null)
                    {
                        firstItem = newItem;
                    }
                    _documentRollUp.AddDocument(item.DocumentReference.Source,(item.DocumentReference as IUriContext).BaseUri);
                }
                dest.SelectedItem = firstItem;
                dest.EndInit();
            }
            e.Handled = true;
        }

        protected void DestPageList_OnDrop(object sender, System.Windows.DragEventArgs e)
        {
            if (e.Data != null && e.Data.GetDataPresent(typeof(List<PageItem>)))
            {
                List<PageItem> items = (List<PageItem>)e.Data.GetData(typeof(List<PageItem>));
                if (items.Count > 0)
                {
                    int selectedDoc = dest.SelectedIndex;
                    if (selectedDoc == -1)
                    {
                        selectedDoc = _documentRollUp.AddDocument();
                        DocumentItem docItem = new DocumentItem(items[0].FixedPage);
                        dest.Items.Add(docItem);
                        dest.SelectedItem = docItem;
                    }
                    foreach (PageItem item in items)
                    {
                        destPage.Items.Add(new PageItem(item));
                        _documentRollUp.AddPage(selectedDoc, item.PageContent.Source, (item.PageContent as IUriContext).BaseUri);
                    }
                }
            }
            e.Handled = true;
        }

        protected void DestItem_Drop(object sender, System.Windows.DragEventArgs e)
        {
            if (e.Data != null && e.Data.GetDataPresent(typeof(List<DocumentItem>)))
            {
                List<DocumentItem> items = (List<DocumentItem>)e.Data.GetData(typeof(List<DocumentItem>));
                ListBoxItem currItem = (ListBoxItem)sender;
                int currItemIndex = dest.Items.IndexOf(currItem.Content);
                int insertionIndex = currItemIndex;

                Point currentDropPosition = e.GetPosition(dest);
                Point itemPosition = currItem.TranslatePoint(new Point(0, 0), dest);
                if (currentDropPosition.X < (itemPosition.X + (currItem.ActualWidth / 2)))
                {

                }
                else
                {
                    insertionIndex++;
                }
                foreach (DocumentItem item in items)
                {
                    dest.Items.Insert(insertionIndex, new DocumentItem(item));
                    _documentRollUp.InsertDocument(insertionIndex, item.DocumentReference.Source, (item.DocumentReference as IUriContext).BaseUri);
                }
            }
            e.Handled = true;
        }

        protected void DestPageItem_Drop(object sender, System.Windows.DragEventArgs e)
        {
            if (e.Data != null && e.Data.GetDataPresent(typeof(List<PageItem>)))
            {
                List<PageItem> items = (List<PageItem>)e.Data.GetData(typeof(List<PageItem>));
                ListBoxItem currItem = (ListBoxItem)sender;
                int currItemIndex = destPage.Items.IndexOf(currItem.Content);
                int insertionIndex = currItemIndex;

                Point currentDropPosition = e.GetPosition(dest);
                Point itemPosition = currItem.TranslatePoint(new Point(0, 0), destPage);
                if (currentDropPosition.X < (itemPosition.X + (currItem.ActualWidth / 2)))
                {

                }
                else
                {
                    insertionIndex++;
                }
                foreach (PageItem item in items)
                {
                    destPage.Items.Insert(insertionIndex, new PageItem(item));
                    _documentRollUp.InsertPage(dest.SelectedIndex, insertionIndex, item.PageContent.Source, (item.PageContent as IUriContext).BaseUri);
                }
            }
            e.Handled = true;
        }

        void DestList_PreviewKeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
            {
                int index = -1;

                while ((index = dest.SelectedIndex) != -1)
                {
                    dest.Items.RemoveAt(index);
                    _documentRollUp.RemoveDocument(index);
                }
            }
        }

        void DestPageList_PreviewKeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
            {
                int index = -1;

                while ((index = destPage.SelectedIndex) != -1)
                {
                    destPage.Items.RemoveAt(index);
                    _documentRollUp.RemovePage(dest.SelectedIndex, index);
                }
                // if we deleted all the pages, we will remove the parent document too
                if (destPage.Items.Count == 0)
                {
                    while ((index = dest.SelectedIndex) != -1)
                    {
                        dest.Items.RemoveAt(index);
                        _documentRollUp.RemoveDocument(index);
                    }
                }
            }
        }

        void rect_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount == 1 && !Keyboard.IsKeyDown(Key.RightCtrl) && !Keyboard.IsKeyDown(Key.LeftCtrl))
            {
                List<DocumentItem> items = new List<DocumentItem>();
                bool clearAndSelect = false;

                ListBoxItem clikedItem = sender as ListBoxItem;
                 if (Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift))
                {
                    foreach (DocumentItem item in source.SelectedItems)
                    {
                        items.Add(item);
                    }
                }
                else
                {
                    clearAndSelect = true;
                    items.Add(clikedItem.Content as DocumentItem);
                }

                DragDrop.DoDragDrop(source, items, DragDropEffects.All);
                if (clearAndSelect)
                {
                    source.SelectedItems.Clear();
                }
                clikedItem.IsSelected = true;
                e.Handled = true;
            }
        }

        void page_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount == 1 && !Keyboard.IsKeyDown(Key.RightCtrl) && !Keyboard.IsKeyDown(Key.LeftCtrl))
            {
                List<PageItem> items = new List<PageItem>();
                bool clearAndSelect = false;

                ListBoxItem clikedItem = sender as ListBoxItem;
                if (Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift))
                {
                    foreach (PageItem item in sourcePage.SelectedItems)
                    {
                        items.Add(item);
                    }
                }
                else
                {
                    clearAndSelect = true;
                    items.Add(clikedItem.Content as PageItem);
                }

                DragDrop.DoDragDrop(source, items, DragDropEffects.All);
                if (clearAndSelect)
                {
                    sourcePage.SelectedItems.Clear();
                }
                clikedItem.IsSelected = true;
                e.Handled = true;
            }
        }

        void dest_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
        }

        RollUpDocument _documentRollUp;
        Dictionary<string, XpsDocument> _documentCache;
    }

}