/****************************** Module Header ******************************\
* Module Name:  GeneralScenarios.xaml.cs
* Project:      CSSL3DataBinding
* Copyright (c) Microsoft Corporation.
* 
* This module demonstrates general scenarios of Data Binding in SL3
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* History:
* * 11/26/2009 06:00 PM Allen Chen Created
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
using CSSL3DataBinding.DAL;
using System.Windows.Data;
using System.Collections;
using System.Text;
using System.Collections.ObjectModel;
using System.ComponentModel.DataAnnotations;

namespace CSSL3DataBinding
{
    public partial class GeneralScenarios : UserControl
    {
        object _customer = CustomerGenerator.GetSingleCustomer();
        // _newcustomer caches the customer pending to insert
        object _newcustomer = new Customer() { ID = 4, Name = "Vince Xu" };
        IEnumerable customercollection = CustomerGenerator.GetCustomersList();

        public GeneralScenarios()
        {  
            InitializeComponent();

            // Init DataContext and ItemsSource
            this.BindingModeStackPanel.DataContext = _customer;
            this.ConverterScenarioDataGrid.ItemsSource = customercollection;
            this.InsertStackPanel.DataContext = _newcustomer;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // In TwoWay bindings, changes to the target automatically update the source, except when binding to the Text property of a TextBox. 
            // In this case, the update occurs when the TextBox loses focus. 
            // You can refer to http://forums.silverlight.net/forums/t/11547.aspx
            // for workarounds.
        }

        /// <summary>
        /// This is an event handler we can hook for notification of validation errors.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void BindingModeStackPanel_BindingValidationError(object sender, ValidationErrorEventArgs e)
        {
            if (e.Action == ValidationErrorEventAction.Added)
            {
                MessageBox.Show(e.Error.ErrorContent.ToString());
            }
            else
            {
                // Error has been fixed. You can do additional works here.
            }
        }
        /// <summary>
        /// Add a new Customer object to customercollection
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            try
            {
                int id = Int32.Parse(this.InsertIDTextBox.Text);
                string name = this.InsertNameTextBox.Text;
                // To notify collection elements' change, collection class should implement INotifyCollectionChanged.
                // It's recommended to use ObservableCollection rather than implementing INotifyCollectionChanged on your own.
                ObservableCollection<Customer> collection = this.customercollection as ObservableCollection<Customer>;
                if (collection != null)
                {
                    collection.Add(new Customer() { ID = id, Name = name });
                }
            }
            catch (Exception ex) { }
        }

    }

    /// <summary>
    /// This is a custom ValueConverter class. It converts int to Brush. 
    /// If ID is larger than 1 a redbrush will be returned.
    /// </summary>
    public class MyConverter : IValueConverter
    {
        Brush redbrush = new SolidColorBrush(Color.FromArgb(255, 255, 0, 0));
        public object Convert(object value,
                              Type targetType,
                              object parameter,
                              System.Globalization.CultureInfo culture)
        {

            if (value is int)
            {

                return (int)value>1?redbrush:null;
            }
            else 
            {
                return null; 
            }
        }

        public object ConvertBack(object value,
                                  Type targetType,
                                  object parameter,
                                  System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
