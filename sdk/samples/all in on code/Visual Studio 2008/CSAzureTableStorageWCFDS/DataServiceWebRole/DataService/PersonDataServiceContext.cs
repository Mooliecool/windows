/****************************** Module Header ******************************\
* Module Name:	PersonDataServiceContext.cs
* Project:		CSAzureTableStorageWCFDS
* Copyright (c) Microsoft Corporation.
* 
* This is the WCF Data Service object context.
* Do not confuse it with PersonTableStorageContext (the table storage context).
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
using System.Data.Services;
using System.Linq;
using WebRole.Entities;

namespace WebRole.DataService
{
	/// <summary>
	/// This is the WCF Data Service object context.
	/// Do not confuse it with PersonTableStorageContext (the table storage context).
	/// Table storage does not use EDM or LINQ to SQL.
	/// So we must create a custom object context that implements IUpdatable.
	/// This sample only demonstrates simple CRUD operations. So not all methods are implemented.
	/// </summary>
	public class PersonDataServiceContext : IUpdatable
	{
		private PersonTableStorageContext _ctx = new PersonTableStorageContext();

		public IQueryable<Person> Person
		{
			get { return this._ctx.Person; }
		}

		public void AddReferenceToCollection(object targetResource, string propertyName, object resourceToBeAdded)
		{
			throw new NotImplementedException();
		}

		public void ClearChanges()
		{
			throw new NotImplementedException();
		}

		public object CreateResource(string containerName, string fullTypeName)
		{
			Person entity = new Person();
			// Add the entity to table context.
			this._ctx.AddObject(PersonTableStorageContext.TableName, entity);
			return entity;
		}

		/// <summary>
		/// Delete the person entity from table storage.
		/// </summary>
		public void DeleteResource(object targetResource)
		{
			Person person = targetResource as Person;
			if (person == null)
			{
				throw new DataServiceException(400, "Invalid object. Object must be a Person");
			}
			this._ctx.DeleteObject(person);
			this._ctx.SaveChanges();
		}

		/// <summary>
		/// GetResource is invoked before updating and deletion.
		/// </summary>
		public object GetResource(IQueryable query, string fullTypeName)
		{
			IQueryable<Person> tableQuery = query as IQueryable<Person>;
			if (tableQuery == null)
			{
				throw new DataServiceException(400, "Invalid query.");
			}
			return tableQuery.First();
		}

		public object GetValue(object targetResource, string propertyName)
		{
			throw new NotImplementedException();
		}

		public void RemoveReferenceFromCollection(object targetResource, string propertyName, object resourceToBeRemoved)
		{
			throw new NotImplementedException();
		}

		public object ResetResource(object resource)
		{
			throw new NotImplementedException();
		}

		public object ResolveResource(object resource)
		{
			return resource;
		}

		public void SaveChanges()
		{
			this._ctx.SaveChanges();
		}

		public void SetReference(object targetResource, string propertyName, object propertyValue)
		{
			throw new NotImplementedException();
		}

		/// <summary>
		/// SetValue is invoked before updating.
		/// </summary>
		public void SetValue(object targetResource, string propertyName, object propertyValue)
		{
			// The Partition/RowKey should not be modified.
			if (propertyValue != null && propertyName != "PartitionKey" && propertyName != "RowKey")
			{
				Person person = (Person)targetResource;
				typeof(Person).GetProperty(propertyName).SetValue(person, propertyValue, null);
				this._ctx.UpdateObject(person);
			}
		}
	}
}
