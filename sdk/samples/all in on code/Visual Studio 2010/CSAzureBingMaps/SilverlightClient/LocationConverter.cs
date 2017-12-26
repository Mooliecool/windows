/****************************** Module Header ******************************\
* Module Name:	LocationConverter.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* A Silverlight converter.
* Converts Travel data to Bing Maps Location.
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
using System.Windows.Data;
using Microsoft.Maps.MapControl;
using SilverlightClient.TravelDataServiceReference;

namespace SilverlightClient
{
    /// <summary>
    /// A Silverlight converter.
    /// Converts Travel data to Bing Maps Location.
    /// </summary>
    public class LocationConverter : IValueConverter
    {
        public object Convert(object value, Type targetType,
            object parameter, System.Globalization.CultureInfo culture)
        {
            if (value is Travel)
            {
                Travel t = (Travel)value;
                return new Location(t.Latitude, t.Longitude);
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType,
            object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
