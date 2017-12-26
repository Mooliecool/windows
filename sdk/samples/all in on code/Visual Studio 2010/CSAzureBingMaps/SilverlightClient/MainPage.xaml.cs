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
using System.Collections.ObjectModel;
using System.Data.Services.Client;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using Microsoft.Maps.MapControl;
using SilverlightClient.GeocodeServiceReference;
using SilverlightClient.TravelDataServiceReference;

namespace SilverlightClient
{
    public partial class MainPage : UserControl
    {
        // Your Bing Maps credential.
        private string _mapCredential = "[your credential]";
        private GeocodeServiceClient _geocodeClient = new GeocodeServiceClient("CustomBinding_IGeocodeService");
        // Since the Silverlight client is hosted in the same Web Role as the service,
        // we'll use relative address.
        private TravelDataServiceContext _dataServiceContext =
            new TravelDataServiceContext(new Uri("../DataService/TravelDataService.svc", UriKind.Relative));
        private ObservableCollection<Travel> _travelItems = new ObservableCollection<Travel>();

        public MainPage()
        {
            InitializeComponent();
            // Display sign in link or welcome message based on identity.
            if (App.IsAuthenticated)
            {
                this.LoginLink.Visibility = System.Windows.Visibility.Collapsed;
                this.WelcomeTextBlock.Visibility = System.Windows.Visibility.Visible;
                this.WelcomeTextBlock.Text = App.WelcomeMessage;
            }
            else
            {
                this.LoginLink.Visibility = System.Windows.Visibility.Visible;
                this.WelcomeTextBlock.Visibility = System.Windows.Visibility.Collapsed;
            }
            this.map.CredentialsProvider = new ApplicationIdCredentialsProvider(this._mapCredential);
            this._geocodeClient.ReverseGeocodeCompleted +=
                new EventHandler<ReverseGeocodeCompletedEventArgs>(GeocodeClient_ReverseGeocodeCompleted);
            this.LoginLink.NavigateUri =
                new Uri(Application.Current.Host.Source, "../LoginPage.aspx?returnpage=SilverlightClient.aspx");
        }

        private void Map_Loaded(object sender, RoutedEventArgs e)
        {
            // Query the data.
            this._dataServiceContext.Travels.BeginExecute(result =>
            {
                this._travelItems = new ObservableCollection<Travel>
                    (this._dataServiceContext.Travels.EndExecute(result).ToList().OrderBy(t => t.Time));
                this.Dispatcher.BeginInvoke(new Action(() =>
                {
                    this.mapItems.ItemsSource = this._travelItems;
                    this.placeList.ItemsSource = this._travelItems;
                }));
            }, null);
        }

        private void map_MouseClick(object sender, Microsoft.Maps.MapControl.MapMouseEventArgs e)
        {
            // Invoke Bing Maps Geocode service to obtain the nearest location.
            ReverseGeocodeRequest request = new ReverseGeocodeRequest()
            {
                Location = map.ViewportPointToLocation(e.ViewportPoint)
            };
            request.Credentials = new Credentials() { Token = this._mapCredential };
            _geocodeClient.ReverseGeocodeAsync(request);
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
                this._travelItems.Add(travel);
                this._dataServiceContext.AddObject("Travels", travel);
            }
        }

        private void DatePicker_SelectedDateChanged(object sender, SelectionChangedEventArgs e)
        {
            DatePicker datePicker = (DatePicker)sender;
            Travel travel = datePicker.DataContext as Travel;
            if (travel != null && travel.Time != datePicker.SelectedDate.Value)
            {
                travel.Time = datePicker.SelectedDate.Value;
                this._dataServiceContext.UpdateObject(travel);
            }
        }

        /// <summary>
        /// Save changes.
        /// </summary>
        private void SaveButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            // Our data service provider implementation doesn't support MERGE, so let's do a full update (PUT).
            this._dataServiceContext.BeginSaveChanges
                (SaveChangesOptions.ReplaceOnUpdate, new AsyncCallback((result) =>
            {
                var response = this._dataServiceContext.EndSaveChanges(result);
            }), null);
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
                this._travelItems.Remove(travel);
                this._dataServiceContext.DeleteObject(travel);
            }
        }
    }
}
