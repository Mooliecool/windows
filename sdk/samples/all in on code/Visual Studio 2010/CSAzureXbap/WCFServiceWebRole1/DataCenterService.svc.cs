/****************************** Module Header ******************************\
* Module Name:  DataCenterService.cs
* Project:      CSAzureXbap
* Copyright (c) Microsoft Corporation.
* 
* The service implementation.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Collections.Generic;

namespace WCFServiceWebRole
{
    // NOTE: You can use the "Rename" command on the "Refactor" menu to change the class name "DataCenterService" in code, svc and config file together.
    public class DataCenterService : IDataCenterService
    {
        public List<DataCenter> GetDataCenters()
        {
            List<DataCenter>  dataCenters = new List<DataCenter>(6);
            dataCenters.Add(new DataCenter() { Name = "Chicago", Bound = new Rect(328, 790, 20, 20) });
            dataCenters.Add(new DataCenter() { Name = "San Antonio", Bound = new Rect(285, 873, 20, 20) });
            dataCenters.Add(new DataCenter() { Name = "Amsterdam", Bound = new Rect(856, 711, 20, 20) });
            dataCenters.Add(new DataCenter() { Name = "Dublin", Bound = new Rect(796, 703, 20, 20) });
            dataCenters.Add(new DataCenter() { Name = "Hong Kong", Bound = new Rect(1454, 923, 20, 20) });
            dataCenters.Add(new DataCenter() { Name = "Singapore", Bound = new Rect(1406, 1040, 20, 20) });
            return dataCenters;
        }
    }
}
