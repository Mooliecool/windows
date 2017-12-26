/****************************** Module Header ******************************\
* Module Name:	PersonDataService.cs
* Project:		CSAzureTableStorageWCFDS
* Copyright (c) Microsoft Corporation.
* 
* This is the WCF Data Service.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Data.Services;

namespace WebRole.DataService
{
	public class PersonDataService : DataService<PersonDataServiceContext>
    {
        // This method is called only once to initialize service-wide policies.
        public static void InitializeService(IDataServiceConfiguration config)
        {
             config.SetEntitySetAccessRule("*", EntitySetRights.All);
             config.SetServiceOperationAccessRule("*", ServiceOperationRights.All);
        }
    }
}
