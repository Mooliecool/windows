/****************************** Module Header ******************************\
* Module Name:	DataSource.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* The data source used by both MainPage and ListPage.
* Wraps calls to WCF Data Services.
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
using AzureBingMaps.DAL;

namespace WindowsPhoneClient
{
    /// <summary>
    /// The data source used by both MainPage and ListPage.
    /// Wraps calls to WCF Data Services.
    /// </summary>
    public class DataSource
    {
        // Windows Phone applications cannot be hosted in Windows Azure.
        // So we have to use absolute address.
        // Use http://127.0.0.1:81/DataService/TravelDataService.svc/ if you're using Compute Emulator. Otherwise, use your own Windows Azure service address.
        private TravelDataServiceContext _dataServiceContext = new TravelDataServiceContext(new Uri("http://127.0.0.1:81/DataService/TravelDataService.svc/"));

        private ObservableCollection<Travel> _travelItems = new ObservableCollection<Travel>();
        public event EventHandler DataLoaded;

        public ObservableCollection<Travel> TravelItems
        {
            get { return this._travelItems; }
        }

        /// <summary>
        /// Query the data.
        /// </summary>
        public void LoadDataAsync()
        {
            this._dataServiceContext.BeginExecute<Travel>(new Uri(this._dataServiceContext.BaseUri, "Travels"), result =>
            {
                var results = this._dataServiceContext.EndExecute<Travel>(result).ToList().OrderBy(t => t.Time);
                this._travelItems = new ObservableCollection<Travel>();
                foreach (var item in results)
                {
                    this._travelItems.Add(item);
                }
                if (this.DataLoaded != null)
                {
                    this.DataLoaded(this, EventArgs.Empty);
                }
            }, null);
        }

        public void AddToTravel(Travel travel)
        {
            this._travelItems.Add(travel);
            this._dataServiceContext.AddObject("Travels", travel);
        }

        public void UpdateTravel(Travel travel)
        {
            this._dataServiceContext.UpdateObject(travel);
        }

        public void RemoveFromTravel(Travel travel)
        {
            this._travelItems.Remove(travel);
            this._dataServiceContext.DeleteObject(travel);
        }

        public void SaveChanges()
        {
            // Our data service provider implementation doesn't support MERGE, so let's do a full update (PUT).
            this._dataServiceContext.BeginSaveChanges(SaveChangesOptions.ReplaceOnUpdate, new AsyncCallback((result) =>
            {
                var response = this._dataServiceContext.EndSaveChanges(result);
            }), null);
        }
    }
}
