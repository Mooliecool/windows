/****************************** Module Header ******************************\
* Module Name:	Travel.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* Partial class for the Travel EF entity.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Data.Objects.DataClasses;
using System.Data.Services;
using System.Data.Services.Common;
using System.IO;
using Microsoft.SqlServer.Types;

namespace AzureBingMaps.DAL
{
    /// <summary>
    /// The partial class for the Travel EF entity.
    /// Both PartitionKey and RowKey are part of data service key.
    /// Properties such as EntityState and EntityKey should not be passed to the client.
    /// The binary representation GeoLocation does not need to be passed to the client as well.
    /// </summary>
    [DataServiceKey(new string[] { "PartitionKey", "RowKey" })]
    [IgnoreProperties(new string[] { "EntityState", "EntityKey", "GeoLocation" })]
    public partial class Travel : EntityObject
    {
        private string _geoLocationText;

        /// <summary>
        /// The text representation of the geo location, which is more user friendly.
        /// When Latitude and Longitude are modified, GeoLocationText will be modified as well.
        /// Client may upload an entity with Latitude/Longitude, but without GeoLocationText, so its value could be null.
        /// To avoid unintentionally setting GeoLocaionText to null, let's check the value in setter.
        /// </summary>
        public string GeoLocationText
        {
            get { return this._geoLocationText; }
            set
            {
                if (!string.IsNullOrEmpty(value))
                {
                    this._geoLocationText = value;
                }
            }
        }

        // When either latitude or longitude changes, GeoLocationText must be changed as well.
        // The binary GeoLocation does not need to be changed, as it is only known by the database.
        private double _latitude;
        public double Latitude
        {
            get { return this._latitude; }
            set
            {
                this._latitude = value;
                this.GeoLocationText = this.LatLongToWKT(this.Latitude, this.Longitude);
            }
        }

        private double _longitude;
        public double Longitude
        {
            get { return this._longitude; }
            set
            {
                this._longitude = value;
                this.GeoLocationText = this.LatLongToWKT(this.Latitude, this.Longitude);
            }
        }

        /// <summary>
        /// Convert latitude and longitude to WKT.
        /// </summary>
        private string LatLongToWKT(double latitude, double longitude)
        {
            SqlGeography sqlGeography = SqlGeography.Point(latitude, longitude, 4326);            
            return sqlGeography.ToString();
        }

        /// <summary>
        /// GeoLocationText, Latitude, Longitude do not correspond to any column in the database.
        /// Geolocation (binary) corresponds to the GeoLocation column in the TravelView.
        /// If the binary GeoLocation changes, those values should be modified as well.
        /// This could happen when querying the entity.
        /// </summary>
        partial void OnGeoLocationChanging(global::System.Byte[] value)
        {
            if (value != null)
            {
                using (MemoryStream ms = new MemoryStream(value))
                {
                    using (BinaryReader reader = new BinaryReader(ms))
                    {
                        SqlGeography sqlGeography = new SqlGeography();
                        sqlGeography.Read(reader);
                        this.GeoLocationText = new string(sqlGeography.STAsText().Value);
                        this.Latitude = sqlGeography.Lat.Value;
                        this.Longitude = sqlGeography.Long.Value;
                    }
                }
            }
        }
    }
}
