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
using System.Windows.Shapes;


namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for AddTargetDialog.xaml
    

    public partial class AddTargetDialog : DialogWindow
    {
        private string m_Condition;
        private string m_DependsOn="";
        private string m_TargetName;

        private BuildProject m_Project;

        public AddTargetDialog()
        {
        }

        public AddTargetDialog(BuildProject project)
        {
            if (project == null)
            {
                throw new ArgumentNullException("project");
            }
            m_Project = project;

            InitializeComponent();

            this.DataContext = m_Project;
        }

        void WindowLoaded(object sender, RoutedEventArgs e)
        {
           //Create the buttons I need for this dialog
           Button okayButton = base.AddButton("Okay");
           okayButton.Click += new RoutedEventHandler(okayButton_Click);

           base.AddCancelButton();

           okayButton.Focus();
        }

        void okayButton_Click(object sender, RoutedEventArgs e)
        {
            m_TargetName = NameBox.Text;

            if (m_TargetName.Trim().Length == 0)
            {
                ErrorDialog dialog = new ErrorDialog("You must enter a target name", "Enter target name", ErrorDialog.ErrorType.Warning);
                dialog.ShowDialog();
                return;
            }

            m_Condition = ConditionBox.Text;

            if (DependsBox.SelectedItem != null)
            {
                BuildAction currentTarget = DependsBox.SelectedItem as BuildAction;
                m_DependsOn = currentTarget.TargetName;
            }

            this.DialogResult = true;
        }

        void CheckTarget(object sender, RoutedEventArgs e)
        {
            bool found = false;

            foreach (BuildAction action in m_Project.Targets)
            {
                if (action.TargetName == NameBox.Text)
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                TargetConflict.Visibility = Visibility.Visible;
            }
            else
            {
                TargetConflict.Visibility = Visibility.Hidden;
            }
        }

        public string DependsOn
        {
            get { return m_DependsOn; }
        }
        public string TargetName
        {
            get { return m_TargetName; }
        }
        public string Condition
        {
            get { return m_Condition; }
        }
    }
}