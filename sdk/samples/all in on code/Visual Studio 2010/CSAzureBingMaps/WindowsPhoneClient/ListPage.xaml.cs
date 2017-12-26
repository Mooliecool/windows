/****************************** Module Header ******************************\
* Module Name:	ListPage.xaml.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* Code behind for ListPage.
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
using System.Windows;
using System.Windows.Controls;
using AzureBingMaps.DAL;
using Microsoft.Phone.Controls;

namespace WindowsPhoneClient
{
    public partial class ListPage : PhoneApplicationPage
    {
        public ListPage()
        {
            InitializeComponent();
        }

        private void PhoneApplicationPage_Loaded(object sender, RoutedEventArgs e)
        {
            this.placeList.ItemsSource = App.DataSource.TravelItems;
        }

        /// <summary>
        /// When the DatePicker's value is changed, update the data source.
        /// </summary>
        private void DatePicker_ValueChanged(object sender, DateTimeValueChangedEventArgs e)
        {
            DatePicker datePicker = (DatePicker)sender;
            Travel travel = datePicker.DataContext as Travel;
            if (travel != null && travel.Time != datePicker.Value)
            {
                travel.Time = datePicker.Value.Value;
                App.DataSource.UpdateTravel(travel);
            }
        }

        /// <summary>
        /// Delete the item from data source.
        /// </summary>
        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            HyperlinkButton button = (HyperlinkButton)sender;
            Travel travel = button.DataContext as Travel;
            if (travel != null)
            {
                App.DataSource.RemoveFromTravel(travel);
            }
        }

        /// <summary>
        /// Save changes.
        /// </summary>
        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            App.DataSource.SaveChanges();
        }

        /// <summary>
        /// Application bar event handler: Navigate to MainPage.
        /// </summary>
        private void ApplicationBarIconButton_Click(object sender, EventArgs e)
        {
            this.NavigationService.Navigate(new Uri("/MainPage.xaml", UriKind.Relative));
        }
    }
}