/****************************** Module Header ******************************\
Module Name:	MainWindow.xaml.cs
Project:	    CSWPFCascadeDataGridColumns
Copyright (c) Microsoft Corporation.

The CSWPFCascadeDataGridComboBoxColumns demonstrates how to implement
cascade DataGridComboBoxColumn in WPF DataGrid control.

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
using System.Data;

namespace CSWPFCascadeDataGridComboBoxColumns
{

    public partial class MainWindow : Window
    {

        public MainWindow()
        {
            InitializeComponent();
        }

        public enum Countries
        {
            China,
            UnitedStates
        }
        public enum ChinaCities
        {
            Beijing,
            Shanghai
        }
        public enum UnitedStatesCities
        {
            NewYork,
            Washington
        }

        DataTable table = null;
        string[] strChinaCities;
        string[] strUnitedStateCities;

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Copy all Country enumeration values to a string array.
            Array countries = Enum.GetValues(typeof(Countries));
            string[] strCountries = new string[countries.Length];
            for (int i = 0; i < countries.Length; i++)
            {
                strCountries[i] = (countries as Countries[])[i].ToString();
            }

            // Copy all ChinaCity enumeration values to a string array.
            Array chinaCities = Enum.GetValues(typeof(ChinaCities));
            strChinaCities = new string[chinaCities.Length];
            for (int i = 0; i < chinaCities.Length; i++)
            {
                strChinaCities[i] = (chinaCities as ChinaCities[])[i].ToString();
            }

            // Copy all UnitedStateCity enumeration values to a string array.
            Array unitedStateCities = Enum.GetValues(typeof(UnitedStatesCities));
            strUnitedStateCities = new string[unitedStateCities.Length];
            for (int i = 0; i < unitedStateCities.Length; i++)
            {
                strUnitedStateCities[i] = (unitedStateCities as UnitedStatesCities[])[i].ToString();
            }

            // Combine both the two cities enumeration value into one string array.
            string[] strAllCities = new string[strChinaCities.Length + strUnitedStateCities.Length];
            strChinaCities.CopyTo(strAllCities, 0);
            strUnitedStateCities.CopyTo(strAllCities, strChinaCities.Length);

            // Data bind the two DataGridComboBoxColumn's ItemsSource property respectively.
            BindingOperations.SetBinding(this.column1, DataGridComboBoxColumn.ItemsSourceProperty,
                new Binding() { Source = strCountries });
            BindingOperations.SetBinding(this.column2, DataGridComboBoxColumn.ItemsSourceProperty,
                new Binding() { Source = strAllCities });

            // Create a DataTable and add two DataColumn into it.
            table = new DataTable();
            table.Columns.Add("Country");
            table.Columns.Add("City");

            // Add a DataRow into this DataTable.
            table.Rows.Add(new object[] { "China", "Beijing" });

            // Set the DataContext property of the DataGrid to the DataTable.
            this.dataGrid.DataContext = table;

            // Set the Header of both DataGridComboBoxColumn and bind the
            // SelectedItemBinding property of both DataGridComboBoxColumn.
            this.column1.Header = "Country";
            this.column1.SelectedItemBinding = new Binding("Country")
            {
                UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged
            };
            this.column2.Header = "City";
            this.column2.SelectedItemBinding = new Binding("City")
            {
                UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged
            };

        }

        /// <summary>
        /// This PreparingCellForEdit event handler gets the hosted editing ComboBox control
        /// and bind its ItemsSource property according to the value of the Country.
        /// </summary>             
        private void datagrid_PreparingCellForEdit(object sender, DataGridPreparingCellForEditEventArgs e)
        {
            if (e.Column.Header.Equals("City"))
            {
                ComboBox cmbEditingElement = e.EditingElement as ComboBox;
                if ((e.Row.Item as DataRowView)["Country"].Equals("China"))
                {

                    // Bind the ItemsSource property of the cmbEditingElement to China cities
                    // string array if the selected country is China.
                    BindingOperations.SetBinding(cmbEditingElement, ComboBox.ItemsSourceProperty,
                        new Binding() { Source = strChinaCities });
                }
                else if ((e.Row.Item as DataRowView)["Country"].Equals("UnitedStates"))
                {

                    // Bind the ItemsSource property of the cmbEditingElement to United State
                    // cities string array if the selected country is United State.
                    BindingOperations.SetBinding(cmbEditingElement, ComboBox.ItemsSourceProperty,
                        new Binding() { Source = strUnitedStateCities });
                }
            }
        }
    }
}
