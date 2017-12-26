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
using System.Diagnostics;
using System.IO;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for WizardPage1.xaml
    

    public partial class WizardPageDone : WizardPageFunction
    {
        private BuildProject m_Project;

        public override void Initialize(BuildProject currentObject)
        {
            if (currentObject == null)
            {
                throw new ArgumentNullException("currentObject");
            }

            m_Project = currentObject;

            this.InitializeComponent();
        }

        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            m_Project.Create();

            TextBlock wizardTitle = (TextBlock)this.GetTemplateChild("WizardTitle");

            wizardTitle.Text = "Project Created.";

            TextBlock wizardText = (TextBlock)this.GetTemplateChild("WizardText");

            wizardText.Text = "Your MS Build Project has been created and saved as: ";
            
            this.FileLink.Content = m_Project.Filename;

            this.ContentGrid.Visibility = Visibility.Visible;
        }

        private void OpenProjectClicked(object sender, RoutedEventArgs e)
        {
            base.Done(m_Project);
        }

        private void ProjectButtonClicked(object sender, RoutedEventArgs e)
        {
            //Open the folder in explorer and select the file
            String filepath = Path.GetDirectoryName(m_Project.Filename);

            try
            {
                Process p = new Process();
                p.StartInfo.FileName = "explorer.exe";
                p.StartInfo.WorkingDirectory = filepath;
                p.StartInfo.Arguments = "/select," + m_Project.Filename;
                if (!p.Start())
                {
                    ErrorDialog dialog = new ErrorDialog("An error occured while trying to create a new explorer window.");
                    dialog.ShowDialog();
                }
            }
            catch (System.ObjectDisposedException ex)
            {
                ErrorDialog dialog = new ErrorDialog("An error occured while trying to create a new explorer window. Error: " + ex.Message);
                dialog.ShowDialog();
            }
            catch (System.ComponentModel.Win32Exception ex) 
            {
                ErrorDialog dialog = new ErrorDialog("An error occured while trying to create a new explorer window. Error: " + ex.Message);
                dialog.ShowDialog();
            }
            catch (System.InvalidOperationException ex)
            {
                ErrorDialog dialog = new ErrorDialog("An error occured while trying to create a new explorer window. Error: " + ex.Message);
                dialog.ShowDialog();
            }
        }
    }
}