/****************************** Module Header ******************************\
* Module Name:	MainPage.xaml.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* Code behind for MainPage.
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
using System.Windows.Input;
using AzureBingMaps.DAL;
using GeocodeServiceReference;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Controls.Maps;

namespace WindowsPhoneClient
{
    public partial class MainPage : PhoneApplicationPage
    {
        // Your Bing Maps credential.
        private string _mapCredential = "[your credential]";

        private GeocodeServiceClient _geocodeClient = new GeocodeServiceClient();
        private Point clickedPoint;

        // Constructor
        public MainPage()
        {
            InitializeComponent();

            // Set the data context of the listbox control to the sample data
            DataContext = App.ViewModel;
            this.Loaded += new RoutedEventHandler(MainPage_Loaded);
            this.map.CredentialsProvider = new ApplicationIdCredentialsProvider(this._mapCredential);
            this._geocodeClient.ReverseGeocodeCompleted += new EventHandler<ReverseGeocodeCompletedEventArgs>(GeocodeClient_ReverseGeocodeCompleted);
        }

        // Load data for the ViewModel Items
        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            if (!App.ViewModel.IsDataLoaded)
            {
                App.ViewModel.LoadData();
            }
            this.mapItems.ItemsSource = App.DataSource.TravelItems;
        }

        private void Map_Loaded(object sender, RoutedEventArgs e)
        {
            App.DataSource.DataLoaded += new EventHandler(DataSource_DataLoaded);
        }

        void DataSource_DataLoaded(object sender, EventArgs e)
        {
            this.Dispatcher.BeginInvoke(new Action(() =>
            {
                this.mapItems.ItemsSource = App.DataSource.TravelItems;
            }));
        }

        /// <summary>
        /// Callback method for the Bing Maps Geocode service.
        /// </summary>
        private void GeocodeClient_ReverseGeocodeCompleted(object sender, ReverseGeocodeCompletedEventArgs e)
        {
            if (e.Error != null)
            {
                MessageBox.Show(e.Error.Message);
            }
            else if (e.Result.Results.Count > 0)
            {
                // We only care about the first result.
                var result = e.Result.Results[0];
                Travel travel = new Travel()
                {
                    // The PartitionKey represents the user.
                    // However, the client can send a fake identity as demonstrated below.
                    // To prevent clients faking the identity,
                    // our service always queries for the real identity on the service side.
                    PartitionKey = "fake@live.com",
                    RowKey = Guid.NewGuid(),
                    Place = result.DisplayName,
                    Time = DateTime.Now,
                    // Latitude/Longitude is obtained from the service,
                    // so it may not be the exact clicked position.
                    Latitude = result.Locations[0].Latitude,
                    Longitude = result.Locations[0].Longitude
                };
                // Add to the ObservableCollection.
                App.DataSource.AddToTravel(travel);
            }
        }

        /// <summary>
        /// Windows Phone map control doesn't support Click,
        /// so we have to handle MouseLeftButtonDown/Up.
        /// These events will fire when the user touches the screen.
        /// </summary>
        private void map_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            this.clickedPoint = e.GetPosition(this.map);
        }

        private void map_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            // Check to see if the finger has moved.
            Point clickedPoint = e.GetPosition(this.map);
            if (Math.Abs(clickedPoint.X - this.clickedPoint.X) < 5 && Math.Abs(clickedPoint.Y - this.clickedPoint.Y) < 5)
            {
                // Invoke Bing Maps Geocode service to obtain the nearest location.
                ReverseGeocodeRequest request = new ReverseGeocodeRequest() { Location = map.ViewportPointToLocation(e.GetPosition(this.map)) };
                request.Credentials = new Credentials() { ApplicationId = this._mapCredential };
                _geocodeClient.ReverseGeocodeAsync(request);
            }
        }

        /// <summary>
        /// Application bar event handler: Navigate to ListPage.
        /// </summary>
        private void ApplicationBarIconButton_Click(object sender, EventArgs e)
        {
            this.NavigationService.Navigate(new Uri("/ListPage.xaml", UriKind.Relative));
        }
    }
}