/****************************** Module Header ******************************\
* Module Name:	TravelDataServiceContext.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* Implements a reflection provider for WCF Data Services.
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
using System.Data.Objects;
using System.Data.Services;
using System.Linq;
using System.Web;
using AzureBingMaps.DAL;

namespace AzureBingMaps.WebRole.DataService
{    
    /// <summary>
    /// Implements a reflection provider for WCF Data Services.
    /// </summary>
    public class TravelDataServiceContext : IUpdatable
    {
        private TravelModelContainer _entityFrameworkContext;

        public TravelDataServiceContext()
        {
            // Obtain the connection string for the partition.
            // PartitionKey represents the current user.
            this._entityFrameworkContext = new TravelModelContainer(
                this.GetConnectionString(this.SetPartitionKey()));
        }

        /// <summary>
        /// Standard query for data services.
        /// </summary>
        public IQueryable<Travel> Travels
        {
            get
            {
                // Only query data for the specific user.
                string partitionKey = this.SetPartitionKey();
                return this._entityFrameworkContext.Travels.Where(e => e.PartitionKey == partitionKey);
            }
        }

        public void AddReferenceToCollection(object targetResource, string propertyName, object resourceToBeAdded)
        {
            throw new NotImplementedException();
        }

        public void ClearChanges()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Create a new entity. This step doesn't set properties.
        /// </summary>
        public object CreateResource(string containerName, string fullTypeName)
        {
            try
            {
                Type t = Type.GetType(fullTypeName + ", AzureBingMaps.DAL", true);
                object resource = Activator.CreateInstance(t);
                if (resource is Travel)
                {
                    this._entityFrameworkContext.Travels.AddObject((Travel)resource);
                }
                return resource;
            }
            catch (Exception ex)
            {
                throw new InvalidOperationException("Failed to create resource. See the inner exception for more details.", ex);
            }
        }

        /// <summary>
        ///  Delete an entity.
        /// </summary>
        public void DeleteResource(object targetResource)
        {
            if (targetResource is Travel)
            {
                this._entityFrameworkContext.Travels.DeleteObject((Travel)targetResource);
            }
        }

        /// <summary>
        /// Get a single entity. Used in update and delete.
        /// </summary>
        public object GetResource(IQueryable query, string fullTypeName)
        {
            ObjectQuery<Travel> q = query as ObjectQuery<Travel>;
            var enumerator = query.GetEnumerator();
            if (!enumerator.MoveNext())
            {
                throw new ApplicationException("Could not locate the resource.");
            }
            if (enumerator.Current == null)
            {
                throw new ApplicationException("Could not locate the resource.");
            }
            return enumerator.Current;
        }

        public object GetValue(object targetResource, string propertyName)
        {
            throw new NotImplementedException();
        }

        public void RemoveReferenceFromCollection(object targetResource, string propertyName, object resourceToBeRemoved)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Update the entity.
        /// </summary>
        public object ResetResource(object resource)
        {
            if (resource is Travel)
            {
                Travel updated = (Travel)resource;
                var original = this._entityFrameworkContext.Travels.Where(
                    t => t.PartitionKey == updated.PartitionKey && t.RowKey == updated.RowKey).FirstOrDefault();
                original.GeoLocationText = updated.GeoLocationText;
                original.Place = updated.Place;
                original.Time = updated.Time;
            }
            return resource;
        }

        public object ResolveResource(object resource)
        {
            return resource;
        }

        public void SaveChanges()
        {
            this._entityFrameworkContext.SaveChanges();
        }

        public void SetReference(object targetResource, string propertyName, object propertyValue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Set property values.
        /// </summary>
        public void SetValue(object targetResource, string propertyName, object propertyValue)
        {
            try
            {

                var property = targetResource.GetType().GetProperty(propertyName);
                if (property == null)
                {
                    throw new InvalidOperationException("Invalid property: " + propertyName);
                }
                // PartitionKey represents the user identity,
                // which must be obtained on the server side,
                // otherwise client may send false identities.
                if (property.Name == "PartitionKey")
                {
                    string partitionKey = this.SetPartitionKey();
                    property.SetValue(targetResource, partitionKey, null);
                }
                else
                {
                    property.SetValue(targetResource, propertyValue, null);
                }
            }
            catch (Exception ex)
            {
                throw new InvalidOperationException("Failed to set value. See the inner exception for more details.", ex);
            }
        }

        /// <summary>
        /// If the user hasn't signed in, using default partition.
        /// Otherwise the partition key is the user's email address.
        /// </summary>
        private string SetPartitionKey()
        {
            string partitionKey = "defaultuser@live.com";
            string user = HttpContext.Current.Session["User"] as string;
            if (user != null)
            {
                partitionKey = user;
            }
            return partitionKey;
        }

        /// <summary>
        /// Obtain the connection string for the partition.
        /// For now, all partitions are stored in the same database.
        /// But as data and users grows,
        /// we can move partitions to other databases for better scaling.
        /// In the future, we can also take advantage of SQL Azure federation.
        /// </summary>
        private string GetConnectionString(string partitionKey)
        {
            return "name=TravelModelContainer";
        }
    }
}