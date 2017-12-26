/************************************* Module Header **************************************\
* Module Name:  MainWindow.xaml.cs
* Project:      CSWPFAnimationWhenDataChanged
* Copyright (c) Microsoft Corporation.
* 
* This example demonstrates how to trigger animation when the value of the datagrid cell is
* changed.
* 
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/30/2009 3:00 PM Bruce Zhou Created
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
using System.ComponentModel;
using System.Collections.ObjectModel;
using Microsoft.Windows.Controls;
using System.Windows.Threading;
namespace CSWPFAnimationWhenDataChanged
{
    public partial class MainWindow : Window
    {
        NameList list = new NameList();
        public MainWindow()
        {
            InitializeComponent();
            //hook up AutoGeneratingColumn event
            this.dataGrid1.AutoGeneratingColumn += 
                new EventHandler<
                    Microsoft.Windows.Controls.DataGridAutoGeneratingColumnEventArgs>(
                    dataGrid1_AutoGeneratingColumn);
            this.DataContext = list;    
            
        }
        //set NotifyOnTargetUpdated property of each binding to true.
        void dataGrid1_AutoGeneratingColumn(object sender, 
            Microsoft.Windows.Controls.DataGridAutoGeneratingColumnEventArgs e)
        {

            Binding binding = (e.Column as DataGridTextColumn).Binding as Binding;
            binding.NotifyOnTargetUpdated = true;
            
        }


        /// <summary>
        /// Handles the Click event of the btnHookupAnimation control, 
        /// apply the style for each column here.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">The <see cref="System.Windows.RoutedEventArgs"/> 
        /// instance containing the event data.</param>
        private void btnHookupAnimation_Click(object sender, RoutedEventArgs e)
        {
            foreach (var col in dataGrid1.Columns)
            {
                DataGridTextColumn textCol = col as DataGridTextColumn;
                textCol.CellStyle = this.FindResource("cellStyle") as Style;
            }
        }
    }
}
