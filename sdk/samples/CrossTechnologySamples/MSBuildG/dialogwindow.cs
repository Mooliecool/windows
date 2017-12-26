// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Markup;

namespace Microsoft.Samples.MSBuildG
{
    // Summary:
    // A derivative of the Window class that creates a Dialog-like window, complete with 
    // reduced close button and a button bar on the bottom.
    public class DialogWindow : Window
    {
        public DialogWindow() 
        {
        }

        // Summary:
        // Adds a button (with the specified caption) to the bottom bar of the dialog window. 
        
        public Button AddButton(string caption)
        {
            if (caption == null)
            {
                throw new ArgumentNullException("caption");
            }

            Button dialogbutton = new Button();

            TextBlock dialogBlock = new TextBlock();
            dialogBlock.Text = caption;

            dialogbutton.Content = dialogBlock;

            AddButton(dialogbutton);

            return (dialogbutton);
        }

        // Summary:
        // Adds a button to the bottom bar of the dialog window. 
        
        public Button AddButton(Button dialogButton)
        {
            if (dialogButton == null)
            {
                throw new ArgumentNullException("dialogButton");
            }

            dialogButton.Style = (Style) Application.Current.FindResource("DialogButton");

            StackPanel container = (StackPanel) GetTemplateChild("ButtonBarPanel");

            container.Children.Add(dialogButton);

            return (dialogButton);
        }

        // Summary:
        // Adds a cancel button to the button bar, which returns a DialogResult of "Cancel" when clicked.        

        public void AddCancelButton()
        {
            Button CancelButton = AddButton("Cancel");
            CancelButton.Click += new RoutedEventHandler(CancelButton_Click);
        }

        void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = null;
        }
    }
}
