/****************************** Module Header ******************************\
Module Name:	MainWindow.xaml.cs
Project:	    CSWPFDataGridCustomSort
Copyright (c) Microsoft Corporation.

The CSWPFDataGridCustomSort demonstrates how to implement
a custom sort for one or severalcolumns in WPF DataGrid control.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Collections;
using System.ComponentModel;

namespace CSWPFDataGridCustomSort
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Declare a custom sort class which implements IComparer interface 
        /// </summary>
        internal class JobSort : IComparer
        {
            private bool _direction;
            public JobSort(bool direction)
            {
                _direction = direction;
            }

            /// <summary>
            /// Implement the IComparer.Compare method to compare two objects
            /// </summary>
            /// <param name="x"></param>
            /// <param name="y"></param>
            /// <returns></returns>
            public int Compare(object x, object y)
            {
                int iXIndex = GetJobIndex(((Employee)x).Job);
                int iYIndex = GetJobIndex(((Employee)y).Job);

                // Based on the index of this kind of job for a sort column
                return (_direction ? 1 : -1) * iXIndex.CompareTo(iYIndex);
            }

            /// <summary>
            /// Get the index value of the Job property based on the custom order
            /// "CTO" > "Manager" > "Leader" > "Member"
            /// </summary>
            /// <param name="job"></param>
            /// <returns></returns>
            private int GetJobIndex(string job)
            {
                switch (job)
                {
                    case "CTO":
                        return 1;
                    case "Manager":
                        return 2;
                    case "Leader":
                        return 3;
                    case "Member":
                        return 4;
                    default:
                        return 0;
                }
            }
        }

        /// <summary>
        /// This Sorting event handler when you click the Column Header named "Job"
        /// and sort "Job" column content by Job kind 
        /// </summary>  
        private void WPF_DataGrid_Sorting(object sender, DataGridSortingEventArgs e)
        {
            // Check the sorted column which needs to implement the custom sort
            if (e.Column.Header.Equals("Job"))
            {
                // Get the ListCollectionView from the DataGrid
                ListCollectionView view = (ListCollectionView)(CollectionViewSource.GetDefaultView(datagrid.ItemsSource));

                if (e.Column != null && e.Column.CanUserSort == true)
                {
                    // Create an object to implement the custom sort 
                    if (e.Column.SortDirection == ListSortDirection.Ascending)
                    {
                        view.CustomSort = new JobSort(false);
                        e.Column.SortDirection = ListSortDirection.Descending;
                    }
                    else
                    {
                        view.CustomSort = new JobSort(true);
                        e.Column.SortDirection = ListSortDirection.Ascending;
                    }

                    e.Handled = true;
                }
            }
        }
    }
}