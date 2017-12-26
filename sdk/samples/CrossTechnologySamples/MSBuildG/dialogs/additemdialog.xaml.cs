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
    // Interaction logic for AddPropertyGroupDialog.xaml


    public partial class AddItemDialog : DialogWindow
    {
        private string m_Name = "";

        public AddItemDialog()
        {
            InitializeComponent();
        }

        public AddItemDialog(string type)
        {
            InitializeComponent();

            TitleBox.Text = type;
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
            m_Name = TitleBox.Text;

            if (m_Name.Trim().Length < 2)
            {
                ErrorDialog dialog = new ErrorDialog("You must enter a valid title to create this item.","Enter Title",ErrorDialog.ErrorType.Message);
                dialog.ShowDialog();
                return;
            }

            this.DialogResult = true;
        }

        public string ItemName
        {
            get { return m_Name; }
        }
    }
}