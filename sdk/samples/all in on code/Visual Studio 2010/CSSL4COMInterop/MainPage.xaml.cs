/****************************** Module Header ******************************\
* Module Name:                MainPage.xaml.cs
* Project:                    CSSL4COMInterop
* Copyright (c) Microsoft Corporation.
* 
* Silverlight COM interoperate sample codebehind file.
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
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Runtime.InteropServices.Automation;
using System.Collections;
using System.Threading;

namespace CSSL4COMInterop
{
    public partial class MainPage : UserControl
    {
        public MainPage()
        {
            InitializeComponent();
            Loaded += new RoutedEventHandler(MainPage_Loaded);
        }

        void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            // Create entity list
            var list = new List<PersonEntity>();
            var rand = new Random();
            for (int i = 0; i < 9; i++)
                list.Add(new PersonEntity
                {
                    Name = "Person:" + i,
                    Age = rand.Next(20),
                    Gender = (i % 2 == 0 ? "Male" : "Female"),
                });

            // Bind entity list to datagrid.
            dataGrid1.ItemsSource = list;
        }


        bool _isprint;
        // Update "print directly" state.
        private void CheckBox_StateChanged(object sender, RoutedEventArgs e)
        {
            var state = ((CheckBox)sender).IsChecked;
            if (state.HasValue && state.Value)
                _isprint = true;
            else
                _isprint = false;
        }

        // Export data to notepad.
        private void TextExport_Click(object sender, RoutedEventArgs e)
        {
            // Check if using AutomationFactory is allowed.
            if (!AutomationFactory.IsAvailable)
            {
                MessageBox.Show("This function need the silverlight application running at evaluated OOB mode.");
            }
            else
            {
                // Use shell to open notepad application.
                using (dynamic shell = AutomationFactory.CreateObject("WScript.Shell"))
                {
                    shell.Run(@"%windir%\notepad", 5);
                    Thread.Sleep(100);

                    shell.SendKeys("Name{Tab}Age{Tab}Gender{Enter}");
                    foreach (PersonEntity item in dataGrid1.ItemsSource as IEnumerable)
                        shell.SendKeys(item.Name + "{Tab}" + item.Age + "{Tab}" + item.Gender + "{Enter}");
                }
            }
        }

        // Export data to word.
        private void WordExport_Click(object sender, RoutedEventArgs e)
        {
            // Check if using AutomationFactory is allowed.
            if (!AutomationFactory.IsAvailable)
            {
                MessageBox.Show("This function need the silverlight application running at evaluated OOB mode.");
            }
            else
            {
                // Create Word automation object.
                dynamic word = AutomationFactory.CreateObject("Word.Application");
                word.Visible = true;

                // Create a new word document.
                dynamic doc = word.Documents.Add();

                // Write title
                dynamic range1 = doc.Paragraphs[1].Range;
                range1.Text = "Silverlight4 Word Automation Sample\n";
                range1.Font.Size = 24;
                range1.Font.Bold = true;

                var list = dataGrid1.ItemsSource as List<PersonEntity>;

                dynamic range2 = doc.Paragraphs[2].Range;
                range2.Font.Size = 12;
                range2.Font.Bold = false;

                // Create table
                doc.Tables.Add(range2, list.Count+1, 3, null, null);

                dynamic cell = doc.Tables[1].Cell(1, 1);
                cell.Range.Text = "Name";
                cell.Range.Font.Bold = true;

                cell = doc.Tables[1].Cell(1, 2);
                cell.Range.Text = "Age";
                cell.Range.Font.Bold = true;

                cell = doc.Tables[1].Cell(1, 3);
                cell.Range.Text = "Gender";
                cell.Range.Font.Bold = true;

                // Fill data to table cells
                for (int i = 0; i < list.Count; i++)
                {
                    cell = doc.Tables[1].Cell(i + 2, 1);
                    cell.Range.Text = list[i].Name;

                    cell = doc.Tables[1].Cell(i + 2, 2);
                    cell.Range.Text = list[i].Age;

                    cell = doc.Tables[1].Cell(i + 2, 3);
                    cell.Range.Text = list[i].Gender;
                }

                if (_isprint)
                {
                    // Print the word directly without preview.
                    doc.PrintOut();
                }

            }
        }
    }
    
}
