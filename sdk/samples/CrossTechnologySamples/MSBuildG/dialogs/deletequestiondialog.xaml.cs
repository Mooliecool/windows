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
    // Interaction logic for SaveQuestionDialog.xaml


    public partial class DeleteQuestionDialog : DialogWindow
    {
        public DeleteQuestionDialog()
        {
            InitializeComponent();
        }

        void WindowLoaded(object sender, RoutedEventArgs e)
        {
            //Create the buttons for the dialog here
            Button yesButton = base.AddButton("Yes");
            yesButton.Click += new RoutedEventHandler(yesButton_Click);

            Button noButton = base.AddButton("No");
            noButton.Click += new RoutedEventHandler(noButton_Click);

            yesButton.Focus();

        }

        void yesButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }
        void noButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
        }

    }
}