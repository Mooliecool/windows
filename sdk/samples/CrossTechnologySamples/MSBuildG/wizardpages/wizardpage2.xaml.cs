// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using System.Windows.Navigation;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Text;
using System.Collections.Generic;
using System.IO;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for WizardPage1.xaml
    

    public partial class WizardPage2 : WizardPageFunction
    {
        public override void Initialize(BuildProject currentObject)
        {
            this.InitializeComponent();
        }

        void BrowseForFolder(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog folderDialog = new System.Windows.Forms.FolderBrowserDialog();
            folderDialog.Description = "Please Select a Folder that contains the files for this project.";

            System.Windows.Forms.DialogResult df = folderDialog.ShowDialog();

            if (df == System.Windows.Forms.DialogResult.OK)
            {
                checkPath(folderDialog.SelectedPath);
            }
        }

        // Summary:
        // Checks a given path for source files. If source files exist, this displays all files in the file list.
        
        private void checkPath(string filePath)
        {
            if (filePath == null)
            {
                throw new ArgumentNullException("filePath");
            }

            //Valid source extensions
            List<string> extensions = new List<string>();

            extensions.Add("cs");
            extensions.Add("vb");
            extensions.Add("xaml");

            //Check if this is a valid path
            if (Directory.Exists(filePath))
            {
                this.FileList.Items.Clear();

                int validCount = 0;

                bool isValid = false;

                //Location files
                foreach (string fileName in Directory.GetFiles(filePath, "*"))
                {
                    isValid = false;

                    string fileExtension = Path.GetExtension(fileName);

                    foreach (string ext in extensions)
                    {
                        if (fileExtension == "." + ext)
                        {
                            isValid = true;
                        }
                    }

                    if (isValid)
                    {
                        validCount++;
                    }

                    if (fileExtension != ".resx")
                    {
                        FileListItem fli = new FileListItem(fileName, isValid);
                        this.FileList.Items.Add(fli);
                    }
                }

                if (validCount == 0)
                {
                    ErrorDialog dialog = new ErrorDialog("No source files found in the target directory.","No Source Files Found",ErrorDialog.ErrorType.Message);
                    dialog.ShowDialog();

                    this.ProjectFilesLabel.Visibility = Visibility.Hidden;
                    this.FileList.Visibility = Visibility.Hidden;

                }
                else
                {
                    this.ProjectFilesLabel.Visibility = Visibility.Visible;
                    this.FileList.Visibility = Visibility.Visible;

                    this.FilePath.Text = filePath;
                }
            }
            else
            {
                ErrorDialog dialog = new ErrorDialog("Invalid Path Entered.", "Invalid Path", ErrorDialog.ErrorType.Warning);
                dialog.ShowDialog();

                this.FilePath.Focus();
                this.FilePath.Text = "";

                this.ProjectFilesLabel.Visibility = Visibility.Hidden;
                this.FileList.Visibility = Visibility.Hidden;
            }
        }

        public override void Done(BuildProject currentObject)
        {
            if (currentObject == null)
            {
                throw new ArgumentNullException("currentObject");
            }

            List<string> filePathList = new List<string>();

            //Make sure we have some checked files before continuing
            foreach (FileListItem cItem in FileList.Items)
            {
                if (cItem.Checked)
                {
                    filePathList.Add(cItem.FilePath);
                }
            }

            if ((this.FilePath.Text.Trim().Length == 0) || (filePathList.Count == 0))
            {
                ShowErrorBubble(new WizardErrorMessage(FilePath, "Project Files Required", "You must choose files for this project to continue"));
                return;
            }

            //Create the template for this project
            currentObject.CreateTemplate(filePathList.ToArray());

            //Done
            base.Done(currentObject);
        }

        private void FilePathKeyDown(object sender, RoutedEventArgs e)
        {
            KeyEventArgs kea = (KeyEventArgs)e;

            if (kea.Key == Key.Enter)
            {
                checkPath(this.FilePath.Text);
            }
        }

        void CancelSelect(object sender, RoutedEventArgs e)
        {
            ListBox senderBox = (ListBox)sender;
            senderBox.SelectedIndex = -1;
        }

        // Summary:
        // UI Wrapper Class for files in the file list
        
        private class FileListItem : ItemsControl
        {
            private string m_FilePath = "";
            private string m_Caption = "";
            private CheckBox m_CheckBox;

            public FileListItem(string filePath, bool sourceFile)
            {

                if (filePath == null)
                {
                    throw new ArgumentNullException("filePath");
                }

                //Save a reference to the file path
                m_FilePath = filePath;

                string shortFileName = Path.GetFileName(filePath);

                m_Caption = shortFileName;

                TextBlock captionBlock = new TextBlock();
                captionBlock.Text = m_Caption;

                string fileExt = Path.GetExtension(shortFileName);

                //Setup the icon based on the file type
                string iconType = "Generic.ico";

                if (fileExt == ".cs")
                {
                    iconType = "Code_CodeFileCS.ico";
                }

                if (fileExt == ".vb")
                {
                    iconType = "Code_CodeFile.ico";
                }

                if (fileExt==".xaml")
                {
                    iconType = "Web_XML.ico";
                }

                //Add the information to the layout of the file item
                BitmapImage fileIcon = new BitmapImage(SettingsSystem.GetIcon(iconType));

                Image fileIconImage = new Image();
                fileIconImage.Source = fileIcon;

                m_CheckBox = new CheckBox();
                m_CheckBox.Height = 14;
                m_CheckBox.IsChecked = sourceFile;

                StackPanel layout = new StackPanel();
                layout.Orientation = Orientation.Horizontal;
                layout.VerticalAlignment = VerticalAlignment.Center;

                layout.Children.Add(m_CheckBox);
                layout.Children.Add(fileIconImage);
                layout.Children.Add(captionBlock);

                this.AddChild(layout);
            }

            public string FilePath
            {
                get { return m_FilePath; }
                set { m_FilePath = value; }
            }

            public bool Checked
            {
                get { return m_CheckBox.IsChecked.Value; }
                set { m_CheckBox.IsChecked = value; }
            }
        }
    }
}