/****************************** Module Header ******************************\
* Module Name:	IndependentAssociationClass.cs
* Project:		CSEFForeignKeyAssociation
* Copyright (c) Microsoft Corporation.
* 
* This file demostrates how to insert new related entities, insert by 
* existing entities and update existing entities with the Independent 
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

namespace CSEFForeignKeyAssociation.IndependentAssociation
{
    public class IndependentAssociationClass
    {
        /// <summary>
        /// Test method to run the insert and update methods with the 
        /// Independent Association
        /// </summary>
        public static void Test()
        {
            Console.WriteLine("Inserting new related Course and Department" +
                " entities by Independent Association...");

            // Insert new related entities by Independent Association.
            InsertNewRelatedEntities();

            // Query the database.
            Query();

            Console.WriteLine("Inserting a new Course related to an existing"
                + " Department entity by Independent Association...");

            // Insert a new entity related to an existing entity by
            // Independent Association.
            InsertByExistingEntities();

            // Query the database.
            Query();

            Console.WriteLine("Updating an existing Course entity " + 
                "(only its regular properties)...");

            Course course = null;
            using (IndependentAssociationEntities context = 
                new IndependentAssociationEntities())
            {
                // Retrieve an existing Course entity.
                // Note: The Single method is new in EF 4.0.
                course = context.Courses.Single(c => c.CourseID == 5002);

                // Modify the Title property of the Course entity.
                course.Title = "Data Structures";

                // Set the relationship by Independent Association.
                course.Department = context.Departments.Single(
                    d => d.DepartmentID == 5);
            }

            // Update an existing Course entity.
            UpdateExistingEntities(course);

            // Query the database.
            Query();
        }


        /// <summary>
        /// Insert a new Course and its Department entity by Independent 
        /// Association
        /// </summary>
        private static void InsertNewRelatedEntities()
        {
            using (IndependentAssociationEntities context = 
                new IndependentAssociationEntities())
            {
                // Create a new Department entity.
                Department department = new Department()
                {
                    DepartmentID = 6,
                    Name = "Software Engineering",
                    Budget = 300000,
                    StartDate = DateTime.Now
                };

                // Create a new Course entity.
                Course course = new Course()
                {
                    CourseID = 6001,
                    Title = "Object Oriented",
                    Credits = 4,
                    StatusID = true,
                    // Set the navigation property.
                    Department = department
                };

                try
                {
                    // Note: Only need to add one entity because the 
                    // relationship and related entity will be added
                    // automatically.
                    context.AddToCourses(course);

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
        /// by Independent Association
        /// </summary>
        private static void InsertByExistingEntities()
        {
            using (IndependentAssociationEntities context = 
                new IndependentAssociationEntities())
            {
                // Create a new Course entity.
                Course course = new Course()
                {
                    CourseID = 6002,
                    Title = ".NET Framework",
                    Credits = 4,
                    StatusID = true,
                    // Set the navigation property to an existing Department 
                    // entity.
                    Department = context.Departments.Single(
                        d => d.DepartmentID == 6)
                };

                try
                {
                    // Note: No need to add the Course entity into the 
                    // ObjectContext because it is automatically done by
                    // relating to an existing Department entity.
                    context.SaveChanges();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        /// <summary>
        /// Update an existing Course entity (only its regular properties, 
        /// not for the relationship)
        /// </summary>
        /// <param name="updatedCourse">An existing Course entity with
        /// updated data</param>
        private static void UpdateExistingEntities(Course updatedCourse)
        {
            using (IndependentAssociationEntities context = 
                new IndependentAssociationEntities())
            {
                try
                {
                    // Attach a new Course entity by the primary key of the
                    // updated Course entity.
                    context.Courses.Attach(new Course() 
                    { CourseID = updatedCourse.CourseID });

                    // Apply the updated regular properties to the attached 
                    // Course entity.
                    // Note: the navigation property is not updated here 
                    // even if it is modified.
                    context.Courses.ApplyCurrentValues(updatedCourse);
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
            using (IndependentAssociationEntities context = 
                new IndependentAssociationEntities())
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
