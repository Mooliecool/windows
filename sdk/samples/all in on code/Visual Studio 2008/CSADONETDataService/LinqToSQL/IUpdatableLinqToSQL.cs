/****************************** Module Header ******************************\
* Module Name:	IUpdatableLinqToSQL.cs
* Project:		CSADONETDataService
* Copyright (c) Microsoft Corporation.
* 
* IUpdatableLinqToSQL.cs is the implementation of ADO.NET Data Service
* IUpdatable interface for Linq to SQL data sources.  It also contains
* partial Linq to SQL entity classes to set the DataServiceKey fields. 
* The source code of the implementation of ADO.NET Data Service IUpdatable
* interface for Linq to SQL is downloaded from: 
* http://code.msdn.microsoft.com/IUpdateableLinqToSql
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directives
using System;
using System.Data.Linq;
using System.Data.Linq.Mapping;
using System.Data.Services;
using System.Linq;
using System.Reflection;
using System.Collections;
using System.Data.Services.Common;
#endregion


namespace CSADONETDataService.LinqToSQL
{
    #region DataContext partial class
    public partial class SchoolLinqToSQLDataContext : IUpdatable
    {
        /// <summary>
        /// Creates the resource of the given type and belonging to the 
        /// given container
        /// </summary>
        /// <param name="containerName">container name to which the resource 
        /// needs to be added</param>
        /// <param name="fullTypeName">full type name i.e. Namespace 
        /// qualified type name of the resource</param>
        /// <returns>object representing a resource of given type and 
        /// belonging to the given container</returns>
        public object CreateResource(string containerName, 
            string fullTypeName)
        {
            Type t = Type.GetType(fullTypeName, true);
            ITable table = GetTable(t);
            object resource = Activator.CreateInstance(t);
            table.InsertOnSubmit(resource);
            return resource;
        }

        /// <summary>
        /// Gets the resource of the given type that the query points to
        /// </summary>
        /// <param name="query">query pointing to a particular resource
        /// </param>
        /// <param name="fullTypeName">full type name i.e. Namespace 
        /// qualified type name of the resource</param>
        /// <returns>object representing a resource of given type and as 
        /// referenced by the query</returns>
        public object GetResource(IQueryable query, string fullTypeName)
        {
            object resource = query.Cast<object>().SingleOrDefault();

            // fullTypeName can be null for deletes
            if (fullTypeName != null && resource.GetType().FullName != 
                fullTypeName)
                throw new Exception("Unexpected type for resource");
            return resource;
        }


        /// <summary>
        /// Resets the value of the given resource to its default value
        /// </summary>
        /// <param name="resource">resource whose value needs to be reset
        /// </param>
        /// <returns>same resource with its value reset</returns>
        public object ResetResource(object resource)
        {
            Type t = resource.GetType();
            MetaTable table = Mapping.GetTable(t);
            object dummyResource = Activator.CreateInstance(t);
            foreach (var member in table.RowType.DataMembers)
            {
                if (!member.IsPrimaryKey && !member.IsDeferred &&
                    !member.IsAssociation && !member.IsDbGenerated)
                {
                    object defaultValue = member.MemberAccessor.
                        GetBoxedValue(dummyResource);
                    member.MemberAccessor.SetBoxedValue(ref resource, 
                        defaultValue);
                }
            }
            return resource;
        }

        /// <summary>
        /// Sets the value of the given property on the target object
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="propertyValue">value of the property</param>
        public void SetValue(object targetResource, string propertyName, 
            object propertyValue)
        {
            MetaTable table = Mapping.GetTable(targetResource.GetType());
            MetaDataMember member = table.RowType.DataMembers.Single(
                x => x.Name == propertyName);
            member.MemberAccessor.SetBoxedValue(ref targetResource, 
                propertyValue);
        }

        /// <summary>
        /// Gets the value of the given property on the target object
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <returns>the value of the property for the given target resource
        /// </returns>
        public object GetValue(object targetResource, string propertyName)
        {
            MetaTable table = Mapping.GetTable(targetResource.GetType());
            MetaDataMember member = table.RowType.DataMembers.Single(
                x => x.Name == propertyName);
            return member.MemberAccessor.GetBoxedValue(targetResource);
        }

        /// <summary>
        /// Sets the value of the given reference property on the target 
        /// object
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="propertyValue">value of the property</param>
        public void SetReference(object targetResource, string propertyName, 
            object propertyValue)
        {
            ((IUpdatable)this).SetValue(targetResource, propertyName, 
                propertyValue);
        }

        /// <summary>
        /// Adds the given value to the collection
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="resourceToBeAdded">value of the property which needs
        /// to be added</param>
        public void AddReferenceToCollection(object targetResource, string 
            propertyName, object resourceToBeAdded)
        {
            PropertyInfo pi = targetResource.GetType().GetProperty(
                propertyName);
            if (pi == null)
                throw new Exception("Can't find property");
            IList collection = (IList)pi.GetValue(targetResource, null);
            collection.Add(resourceToBeAdded);
        }

        /// <summary>
        /// Removes the given value from the collection
        /// </summary>
        /// <param name="targetResource">target object which defines the 
        /// property</param>
        /// <param name="propertyName">name of the property whose value needs
        /// to be updated</param>
        /// <param name="resourceToBeRemoved">value of the property which 
        /// needs to be removed</param>
        public void RemoveReferenceFromCollection(object targetResource, 
            string propertyName, object resourceToBeRemoved)
        {
            PropertyInfo pi = targetResource.GetType().GetProperty(
                propertyName);
            if (pi == null)
                throw new Exception("Can't find property");
            IList collection = (IList)pi.GetValue(targetResource, null);
            collection.Remove(resourceToBeRemoved);
        }

        /// <summary>
        /// Delete the given resource
        /// </summary>
        /// <param name="targetResource">resource that needs to be deleted
        /// </param>
        public void DeleteResource(object targetResource)
        {
            ITable table = GetTable(targetResource.GetType());
            table.DeleteOnSubmit(targetResource);
        }

        /// <summary>
        /// Saves all the pending changes made till now
        /// </summary>
        public void SaveChanges()
        {
            SubmitChanges();
        }

        /// <summary>
        /// Returns the actual instance of the resource represented by the 
        /// given resource object
        /// </summary>
        /// <param name="resource">object representing the resource whose 
        /// instance needs to be fetched</param>
        /// <returns>The actual instance of the resource represented by the
        /// given resource object</returns>
        public object ResolveResource(object resource)
        {
            return resource;
        }

        /// <summary>
        /// Revert all the pending changes.
        /// </summary>
        public void ClearChanges()
        {
            // see issue #2 in Code Gallery
            // No clear way how to do this with LtoS?

            // Comment out the following line if you'd prefer a silent 
            // failure
            throw new NotSupportedException();
        }
    }
    #endregion

    #region Linq to SQL entity partial classes
    // Set key filed of the Course class to 'CourseID'
    [DataServiceKey("CourseID")]
    public partial class Course
    { }

    // Set key filed of the CourseGrade class to 'EnrollmentID'
    [DataServiceKey("EnrollmentID")]
    public partial class CourseGrade
    { }

    // Set key filed of the CourseInstructor class to 'CourseID' and 'PersonID'
    [DataServiceKey("CourseID", "PersonID")]
    public partial class CourseInstructor
    { }

    // Set key filed of the Person class to 'PersonID'
    [DataServiceKey("PersonID")]
    public partial class Person
    { }
    #endregion
}