/****************************** Module Header ******************************\
* Module Name:	TravelModelContainer.cs
* Project:		AzureBingMaps
* Copyright (c) Microsoft Corporation.
* 
* Partial class for the object context.
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
using System.Data;
using System.Data.Objects;
using System.Reflection;

namespace AzureBingMaps.DAL
{
    /// <summary>
    /// Partial class for the object context.
    /// </summary>
    public partial class TravelModelContainer : ObjectContext
    {
        public override int SaveChanges(SaveOptions options)
        {
            int returnValue = 0;
            // Since we do not call base.SaveChanges, we have to take care of connections manually.
            // Otherwise we may leave a lot of connections open, which results in connection throttle.
            // Entity Framework provides EnsureConnection and ReleaseConnection which base.SaveChanges uses internally.
            // They're internal methods, so we have to use reflection to invoke them.
            var EnsureConnectionMethod = typeof(ObjectContext).GetMethod(
                "EnsureConnection", BindingFlags.Instance | BindingFlags.NonPublic);
            EnsureConnectionMethod.Invoke(this, null);
            // Use ObjectStateManager.GetObjectStateEntries to obtain added, modified, and deleted entities.
            foreach (ObjectStateEntry ose in this.ObjectStateManager.GetObjectStateEntries(EntityState.Added))
            {
                Travel travel = ose.Entity as Travel;
                if (travel != null)
                {
                    RetryPolicy retryPolicy = new RetryPolicy();
                    retryPolicy.Task = new Action(() =>
                    {
                        this.InsertIntoTravel(travel.PartitionKey,
                            travel.Place, travel.GeoLocationText, travel.Time);
                    });
                    retryPolicy.Execute();
                    returnValue++;
                }
            }
            foreach (ObjectStateEntry ose in this.ObjectStateManager.GetObjectStateEntries(EntityState.Modified))
            {
                Travel travel = ose.Entity as Travel;
                if (travel != null)
                {
                    RetryPolicy retryPolicy = new RetryPolicy();
                    retryPolicy.Task = new Action(() =>
                    {
                        this.UpdateTravel(travel.PartitionKey,
                            travel.RowKey, travel.Place, travel.GeoLocationText, travel.Time);
                    });
                    retryPolicy.Execute();
                    returnValue++;
                }
            }
            foreach (ObjectStateEntry ose in this.ObjectStateManager.GetObjectStateEntries(EntityState.Deleted))
            {
                Travel travel = ose.Entity as Travel;
                if (travel != null)
                {
                    RetryPolicy retryPolicy = new RetryPolicy();
                    retryPolicy.Task = new Action(() =>
                    {
                        this.DeleteFromTravel(travel.PartitionKey, travel.RowKey);
                    });
                    retryPolicy.Execute();
                    returnValue++;
                }
            }
            var ReleaseConnectionMethod = typeof(ObjectContext).
                GetMethod("ReleaseConnection", BindingFlags.Instance | BindingFlags.NonPublic);
            ReleaseConnectionMethod.Invoke(this, null);
            return returnValue;
        }
    }
}
