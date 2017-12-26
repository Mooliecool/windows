/****************************** Module Header ******************************\
* Module Name:	FKAssociationClass.cs
* Project:		CSEFForeignKeyAssociation
* Copyright (c) Microsoft Corporation.
* 
* This file demostrates how to insert new related entities, insert by 
* existing entities and update existing entities with the Foreign Key
* Association.
* 
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
* 
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#region Using directive
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
#endregion

namespace CSEFForeignKeyAssociation.FKAssociation
{
    public static class FKAssociationClass
    {
        /// <summary>
        /// Test method to run the insert and update methods with the 
        /// Foreign Key Association
        /// </summary>
        public static void Test()
        {
            Console.WriteLine("Inserting new related Course and Department" +
                " entities by Foreign Key Association...");

            // Insert new related entities by Foreign Key Association.
            InsertNewRelatedEntities();

            // Query the database.
            Query();

            Console.WriteLine("Inserting a new Course related to an existing"
                + " Department entity by Foreign Key Association...");

            // Insert a new entity related to an existing entity by
            // Foreign Key Association.
            InsertByExistingEntities();

            // Query the database.
            Query();

            Console.WriteLine("Updating an existing Course entity along with"
                + " its related Department entity...");

            // Update an existing entity with its relationship by
            // Foreign Key Association.
            UpdateExistingEntities(new Course()
            {
                CourseID = 5002,
                Title = "Basic Data Structure",
                Credits = 4,
                StatusID = true,
                DepartmentID = 7
            });

            // Query the database.
            Query();
        }


        /// <summary>
        /// Insert a new Course and its Department entity by Foreign Key 
        /// Association
        /// </summary>
        private static void InsertNewRelatedEntities()
        {
            using (FKAssociationEntities context = 
                new FKAssociationEntities())
            {
                // Create a new Department entity.
                Department department = new Department()
                {
                    DepartmentID = 5, 
                    Name = "Computer Science",
                    Budget = 400000,
                    StartDate = DateTime.Now
                };

                // Create a new Course entity.
                Course course = new Course()
                {
                    CourseID = 5001,
                    Title = "Operation System",
                    Credits = 4,
                    StatusID = true,
                    // Set its foreign key property.
                    DepartmentID = department.DepartmentID
                };

                try
                {
                    // Add the Department and Course entities into the 
                    // ObjectContext.
                    context.AddToDepartments(department);
                    context.AddToCourses(course);
                    
                    // Note: The navigation properties between the 
                    // Department and Course entities won't map to each 
                    // other until after SaveChanges is called.
                    context.SaveChanges();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        /// <summary>
        /// Insert a new Course and set it belong to an existing Department 
        /// by Foreign Key Association
        /// </summary>
        private static void InsertByExistingEntities()
        {
            using (FKAssociationEntities context = 
                new FKAssociationEntities())
            {
                // Create a new Course entity.
                Course course = new Course()
                {
                    CourseID = 5002,
                    Title = "Data Structure",
                    Credits = 3,
                    StatusID = true,
                    // Set the foreign key property to an existing 
                    // Department's DepartmentID.
                    DepartmentID = 5
                };

                try
                {
                    // Add the Course entity into the ObjectContext.
                    context.AddToCourses(course);

                    // Note: The navigation property of the Course
                    // won't map to the Department entity until 
                    // after SaveChanges is called.
                    context.SaveChanges();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        /// <summary>
        /// Update an existing Course entity as well as its relationship
        /// </summary>
        /// <param name="updatedCourse">An existing Course entity with
        /// updated data</param>
        private static void UpdateExistingEntities(Course updatedCourse)
        {
            using (FKAssociationEntities context = 
                new FKAssociationEntities())
            {
                try
                {
                    // Attach a new Course entity by the primary key of the
                    // updated Course entity.
                    context.Courses.Attach(new Course() 
                    { CourseID = updatedCourse.CourseID });

                    // Apply the updated values to the attached Course entity
                    // including the foreign key property.
                    // We don't need to provide the existing Department entity
                    // here.
                    context.Courses.ApplyCurrentValues(updatedCourse);

                    // Save the modifications.
                    context.SaveChanges();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        /// <summary>
        /// Query the Course entities and the corresponding Department 
        /// entities
        /// </summary>
        private static void Query()
        {
            using (FKAssociationEntities context = 
                new FKAssociationEntities())
            {
                foreach (var c in context.Courses)
                {
                    Console.WriteLine("Course ID:{0}\nTitle:{1}\n" + 
                        "Department:{2}", c.CourseID, c.Title, 
                        c.Department.Name);
                }
            }

            Console.WriteLine();
        }
    }
}
