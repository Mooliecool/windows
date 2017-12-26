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


    public partial class AddTaskDialog : DialogWindow
    {
        private string m_TaskParam;
        private string m_TaskName;
        private string m_Condition;

        public AddTaskDialog()
        {
            InitializeComponent();
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
            m_TaskName = NameBox.Text;

            if (m_TaskName.Trim().Length == 0)
            {
                ErrorDialog dialog = new ErrorDialog("You must enter a task name", "Enter task name", ErrorDialog.ErrorType.Warning);
                dialog.ShowDialog();
                return;
            }

            m_TaskParam = ParamBox.Text;
            m_Condition = ConditionBox.Text;

            this.DialogResult = true;
        }

        public string TaskName
        {
            get
            {
                return (m_TaskName);
            }
        }

        public string Condition
        {
            get
            {
                return (m_Condition);
            }
        }


        public string TaskParam
        {
            get
            {
                return (m_TaskParam);
            }
        }

    }
}