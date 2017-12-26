/****************************** Module Header ******************************\
* Module Name:  IDataCenterService.cs
* Project:      CSAzureXbap
* Copyright (c) Microsoft Corporation.
* 
* The service contract.
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
using System.ServiceModel;

namespace WCFServiceWebRole
{
    // NOTE: You can use the "Rename" command on the "Refactor" menu to change the interface name "IDataCenterService" in both code and config file together.
    [ServiceContract]
    public interface IDataCenterService
    {
        [OperationContract]
        List<DataCenter> GetDataCenters();
    }
}
