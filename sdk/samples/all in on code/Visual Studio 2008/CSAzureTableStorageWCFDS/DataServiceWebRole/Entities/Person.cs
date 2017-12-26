/****************************** Module Header ******************************\
* Module Name:	Person.cs
* Project:		CSAzureTableStorageWCFDS
* Copyright (c) Microsoft Corporation.
* 
* This is the table storage entity.
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
using Microsoft.WindowsAzure.StorageClient;

namespace WebRole.Entities
{
	public class Person : TableServiceEntity
	{
		public string Name { get; set; }
		public int Age { get; set; }

		// By default, when creating a new entity, the PartitionKey is set to the current year, and the RowKey is a GUID. Insert the ticks in the beginning of RowKey because the result returned by a query is ordered by PartitionKey and then RowKey.
		public Person()
			: base(DateTime.UtcNow.ToString("yyyy"), string.Format("{0:10}_{1}", DateTime.MaxValue.Ticks - DateTime.Now.Ticks, Guid.NewGuid()))
		{ }

		public Person(string partitionKey, string rowKey)
			: base(partitionKey, rowKey)
		{ }
	}
}
