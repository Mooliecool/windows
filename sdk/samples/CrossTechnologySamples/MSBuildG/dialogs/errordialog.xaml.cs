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
using System.Windows.Media.Imaging;


namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // Interaction logic for SaveQuestionDialog.xaml


    public partial class ErrorDialog : DialogWindow
    {
        public enum ErrorType { Error, Warning, Message };

        public ErrorDialog()
        {
            InitializeComponent();
        }

        public ErrorDialog(string errorMessage)
        {
            InitializeComponent();

            this.ErrorMessage.Text = errorMessage;
        }

        public ErrorDialog(string errorMessage, string title, ErrorType type)
        {
            InitializeComponent();

            this.Title = title;
            this.ErrorMessage.Text = errorMessage;
            this.ErrorImage.Source = new BitmapImage(new Uri("pack://application:,,,/Icons/" + type.ToString() + ".ico"));
        }

        void WindowLoaded(object sender, RoutedEventArgs e)
        {
            Button okayButton = base.AddButton("Okay");
            okayButton.Click += new RoutedEventHandler(okayButton_Click);

            okayButton.Focus();
        }

        void okayButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
        }
    }
}