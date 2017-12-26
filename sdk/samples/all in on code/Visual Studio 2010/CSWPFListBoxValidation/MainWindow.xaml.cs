/****************************** Module Header ******************************\
 * Module Name:  MainWindow.xaml.cs
 * Project: CSWPFListBoxValidation
 * Copyright (c) Microsoft Corporation.
 * 
 * This file contains the C# code for the CSWPFListBoxValidation.
 * The sample demonstrates how to add validation to a ListBox, overriding the control
 * to contain a ValidationListener property, which can be bound to provide validation
 * using built in validation UI features in WPF.
 * 
 * This source is subject to the Microsoft Public License.
 * See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 * All other rights reserved.
 * 
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;

namespace CSWPFListBoxValidation
{
    public partial class MainWindow : Window
    {
        // This is a property that is used to provide the validation information.  
        // In the sample, it is part of the Window control hosting the overridden ListBox.
        // It could be anyplace, and is linked to the overridden ValidatingListBox 
        // via the ValidationListener property's binding, specified in the page XAML.
        ObservableCollection<string> i = new ObservableCollection<string>();

        public static readonly DependencyProperty ValidationProperty = 
            DependencyProperty.Register("Validation", typeof(string), typeof(Window),
            new PropertyMetadata(""));

        public MainWindow()
        {
            InitializeComponent();
            
            // For the sample, we just add 2 items to a list and set the Window's DataContext to the list.
            i.Add("Item 1");
            i.Add("Item 2");
            this.DataContext = i;

            // After initializing the list, we want to validate right away, since our validation
            // in this case will fail if no items are selected in the list.
            Validate(validatingListBox1);
        }

        private void ValidatingListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // We also want to force validation anytime the list selection changes, since the 
            // validation criteria involve selection status of list items
            Validate(sender as ValidatingListBox);
        }

        private void Validate(ValidatingListBox sender)
        {
            // We simply validate here if any items are selected, otherwise, we invalidate.
            // Our validation criteria is based on whether the Validation property has text or not,
            // and its text is used to provide feedback about the validation failure.
            if (sender.SelectedItems.Count == 0)
            {
                SetValue(ValidationProperty, "You must select at least one item!");
            }
            else
            {
                SetValue(ValidationProperty, "");
            }
        }
    }

    // This class is simply a ListBox with one additional property, the ValidationListener property, added.  
    // One could also use an existing property like Tag, but since this property is often required for other purposes,
    // it is better to add a specific property for the purpose of the validation.
    public class ValidatingListBox : ListBox 
    {
        // This is the validation property listener for the overridden version of the ListBox.
        // The property is bound to any property, and is used to determine validation state for the ListBox itself.
        // Note the XAML where this property is bound, and the associated ValidationRule, which uses 
        // the ListBoxValidationRule class below.
        public static readonly DependencyProperty ValidationListenerProperty = 
            DependencyProperty.Register("ValidationListener", typeof(string),  typeof(ValidatingListBox), 
            new PropertyMetadata(""));

        public string ValidationListener
        { 
            get 
            {
                return (string)GetValue(ValidationListenerProperty); 
            }
            set 
            { 
                SetValue(ValidationListenerProperty, value);
            }
        }
    }

    // The ListBoxValidationRule is where we actually validate.
    public class ListBoxValidationRule : ValidationRule
    {
        // In this case, we have a very simple validation.  If there is text in the Validation property,
        // we consider the validation failed.  If the Validation property is empty, then it succeeds.
        // The text of the property is set when we call the Validate() function in the Window class, 
        // which uses its own logic to decide whether to put an error message in the Validation property or not.
        public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
        {
            string s = value as String;

            if (String.IsNullOrEmpty(s))
            {
                return ValidationResult.ValidResult;
            }
            else
            {
                return new ValidationResult(false, value as string);
            }
        }
    }
}
