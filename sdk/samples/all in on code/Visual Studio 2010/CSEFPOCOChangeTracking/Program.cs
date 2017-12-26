/****************************** Module Header ******************************\
Module Name:    Program.cs
Project:        CSEFPOCOChangeTracking
Copyright (c) Microsoft Corporation.

The CSEFPOCOChangeTracking example illustrates how to update POCO entity
properties and relationships with change tracking proxy and without change
tracking proxy.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Linq;
using System.Data.Objects;
using System.Data;
#endregion

namespace CSEFPOCOChangeTracking
{
    class Program
    {
        static void Main(string[] args)
        {
            // Update POCO entity properties with chang tracking proxy.
            UpdatePropertyWithChangeTrackingProxy();
            Console.WriteLine();

            // Update POCO entity properties without chang tracking proxy.
            UpdatePropertyWithoutChangeTrackingProxy();
            Console.WriteLine();

            // Update POCO entity relationship with chang tracking proxy.
            UpdateRelationshipWithChangeTrackingProxy();
            Console.WriteLine();

            // Update POCO entity relationship without chang tracking proxy.
            UpdateRelationshipWithoutChangeTrackingProxy();
            Console.WriteLine();

            Console.Write("Press [Enter] to exit...");
            Console.Read();
        }


        /// <summary>
        /// Demostrate how to update POCO entity properties with change
        /// tracking proxy.
        /// </summary>
        private static void UpdatePropertyWithChangeTrackingProxy()
        {
            Console.WriteLine("Update properties with change tracking proxy...");

            try
            {
                using (POCOChangeTrackingEntities context =
                    new POCOChangeTrackingEntities())
                {
                    
                    Department department = context.Departments.
                        Single(d => d.Name == "Chinese");

                    // Retrieve the entity state entry.
                    ObjectStateEntry entry = context.ObjectStateManager.
                        GetObjectStateEntry(department);

                    // It should be "Unchanged" now.
                    Console.WriteLine("Entity State before modification: {0}", 
                        entry.State);

                    department.Name = "New Department Name";

                    // It should be "Modified" now.
                    Console.WriteLine("Entity State after modification: {0}", 
                        entry.State);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }


        /// <summary>
        /// Demostrate how to update POCO entity properties without change
        /// tracking proxy.
        /// </summary>
        private static void UpdatePropertyWithoutChangeTrackingProxy()
        {
            Console.WriteLine("Update properties without change tracking proxy...");

            try
            {
                using (POCOChangeTrackingEntities context =
                    new POCOChangeTrackingEntities())
                {
                    // Turn off proxy creation.
                    context.ContextOptions.ProxyCreationEnabled = false;

                    Department department = context.Departments.
                        Single(d => d.Name == "Chinese");

                    // Retrieve the entity state entry.
                    ObjectStateEntry entry = context.ObjectStateManager.
                        GetObjectStateEntry(department);

                    // It should be "Unchanged" now.
                    Console.WriteLine("Entity State before modification: {0}", 
                        entry.State);

                    department.Name = "New Department Name";

                    // It should be "Unchanged" now.
                    Console.WriteLine("Entity State after modification: {0}",
                        entry.State);

                    // Detect the entity properties changes.
                    context.DetectChanges();

                    // It should be "Modified" now.
                    Console.WriteLine("Entity State after DetectChanges is called: " 
                        + "{0}", entry.State);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }


        /// <summary>
        /// Demostrate how to update POCO entity relationships with change
        /// tracking proxy.
        /// </summary>
        private static void UpdateRelationshipWithChangeTrackingProxy()
        {
            Console.WriteLine("Update relationships with change tracking proxy...");

            try
            {
                using (POCOChangeTrackingEntities context =
                    new POCOChangeTrackingEntities())
                {
                    Department department = context.Departments.
                        Single(d => d.Name == "Chinese");

                    // Create a proxy object.
                    Course newCourse = context.Courses.CreateObject();
                    newCourse.CourseID = 1234;
                    newCourse.Title = "Classical Chinese Literature";
                    newCourse.Credits = 4;
                    newCourse.StatusID = true;

                    // Set the relationship.
                    department.Courses.Add(newCourse);

                    Console.WriteLine("Is the newly created object a proxy? {0}", 
                        IsProxy(newCourse));

                    // Retrieve all the newly added entities' object state entry.
                    var added = context.ObjectStateManager.GetObjectStateEntries(
                        System.Data.EntityState.Added);

                    Console.WriteLine("{0} object(s) added", added.Count());
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }


        /// <summary>
        /// Demostrate how to update POCO entity relationships without change
        /// tracking proxy.
        /// </summary>
        private static void UpdateRelationshipWithoutChangeTrackingProxy()
        {
            Console.WriteLine("Update relationships without change tracking proxy...");

            try
            {
                using (POCOChangeTrackingEntities context =
                    new POCOChangeTrackingEntities())
                {
                    // Turn off proxy creation.
                    context.ContextOptions.ProxyCreationEnabled = false;

                    Department department = context.Departments.
                        Single(d => d.Name == "Chinese");

                    // Create a regular POCO entity.
                    Course newCourse = new Course()
                    {
                        CourseID = 1234,
                        Title = "Classical Chinese Literature",
                        Credits = 4,
                        StatusID = true
                    };

                    // Set the relationship.
                    department.Courses.Add(newCourse);

                    Console.WriteLine("Is the newly created object a proxy? {0}",
                         IsProxy(newCourse));

                    // Retrieve all the newly added entities' object state entry before 
                    // DetectChanges is called. 
                    var addedBeforeDetectChanges = context.ObjectStateManager.
                        GetObjectStateEntries(EntityState.Added);

                    // It should be no entity added.
                    Console.WriteLine("Before DetectChanges is called, " + 
                        "{0} object(s) added", addedBeforeDetectChanges.Count());

                    // Detect the entity relationships modification.
                    context.DetectChanges();

                    // Retrieve all the newly added entities' object state entry after 
                    // DetectChanges is called. 
                    var addedAftereDetectChanges = context.ObjectStateManager.
                        GetObjectStateEntries(EntityState.Added);

                    // It should be 1 entity added.
                    Console.WriteLine("After DetectChanges is called, " +
                        "{0} object(s) added", addedAftereDetectChanges.Count());
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }


        /// <summary>
        /// Check whether the certain object is a proxy entity object.
        /// <param name="type">The entity object</param>
        /// <returns>Return true if the certain object is a proxy entity object,
        /// otherwise return false</returns>
        /// </summary>
        private static bool IsProxy(object type)
        {
            // ObjectContext.GetObjectType returns the entity type of the POCO entity
            // associated with a proxy object of a specified type.  If it does not 
            // equal the entity type, it should be a proxy entity object.   
            return type != null && ObjectContext.GetObjectType(
                type.GetType()) != type.GetType();
        }
    }
}
