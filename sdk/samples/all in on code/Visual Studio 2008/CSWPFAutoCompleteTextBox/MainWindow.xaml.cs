/************************************* Module Header **************************************\
* Module Name:  MainWindow.xaml.cs
* Project:      CSWPFAutoCompleteTextBox
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to achieve AutoComplete TextBox in WPF Application.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 10/20/2009 3:00 PM Bruce Zhou Created
 * 
\******************************************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace CSWPFAutoCompleteTextBox
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="MainWindow"/> class.
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
            // set dataSource for AutoComplete TextBox
            ConstructAutoCompletionSource();
        }

        /// <summary>
        /// Constructs the auto completion source.
        /// </summary>
        private void ConstructAutoCompletionSource()
        {
            
            this.textBox.AutoSuggestionList.Add("Hello world");
            this.textBox.AutoSuggestionList.Add("Hey buddy");
            this.textBox.AutoSuggestionList.Add("Halo world");
            this.textBox.AutoSuggestionList.Add("apple");
            this.textBox.AutoSuggestionList.Add("apple tree");
            this.textBox.AutoSuggestionList.Add("appaaaaa");
            this.textBox.AutoSuggestionList.Add("arrange");
            for (int i = 0; i < 100; i++)
            {
                this.textBox.AutoSuggestionList.Add("AA" + i);
            }
        }
    }
}
