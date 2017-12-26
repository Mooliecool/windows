/****************************** Module Header ******************************\
* Module Name:	PersonTableStorageContext.cs
* Project:		CSAzureTableStorageWCFDS
* Copyright (c) Microsoft Corporation.
* 
* This is the table storage context.
* Do not confuse it with PersonDataServiceContext (the WCF Data Service object context).
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

using System.Linq;
using Microsoft.WindowsAzure;
using Microsoft.WindowsAzure.StorageClient;

namespace WebRole.Entities
{
	/// <summary>
	/// This is the table storage context.
	/// Do not confuse it with PersonDataServiceContext (the WCF Data Service object context).
	/// Table storage contexts should derive from TableServiceContext.
	/// </summary>
	public class PersonTableStorageContext : TableServiceContext
	{
		private static CloudStorageAccount account;
		public static string TableName = "Person";

		static PersonTableStorageContext()
		{
			account = CloudStorageAccount.FromConfigurationSetting("DataConnectionString");
			CloudTableClient tableClient = new CloudTableClient(account.TableEndpoint.AbsoluteUri, account.Credentials);
			tableClient.CreateTableIfNotExist(TableName);
		}

		public PersonTableStorageContext()
			: base(account.TableEndpoint.AbsoluteUri, account.Credentials)
		{
		}

		public IQueryable<Person> Person
		{
			get { return this.CreateQuery<Person>(TableName); }
		}
	}
}
