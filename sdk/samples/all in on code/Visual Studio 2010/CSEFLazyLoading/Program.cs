/****************************** Module Header ******************************\
* Module Name:    Program.cs
* Project:        CSEFLazyLoading
* Copyright (c) Microsoft Corporation.
*
* The CSEFLazyLoading example illustrates how to work with the Lazy Loading
* which is new in Entity Framework 4.0.  It also shows how to use the eager
* loading and explicit loading which is already implemented in the first
* version of Entity Framework.   
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

namespace CSEFLazyLoading
{
    class Program
    {
        static void Main(string[] args)
        {
            LazyLoadingTest();
            EagerLoadingTest();
            ExplicitLoadingTest();

            Console.Write("Press [Enter] to exit...");
            Console.Read();
        }


        /// <summary>
        /// Demostrates how to use lazy loading for related entities 
        /// </summary>
        static void LazyLoadingTest()
        {
            Console.WriteLine("---Lazy Loading---");

            using (LazyLoadingEntities context = new LazyLoadingEntities())
            {
                // Query the department entities which Budget is not NULL
                var departments = from d in context.Departments
                                  where d.Budget != null
                                  select d;

                foreach (var department in departments)
                {
                    Console.WriteLine("Department: {0}", department.Name);

                    Console.WriteLine("Courses:");

                    // With Lazy Loading enabled, directly access the 
                    // Courses property of the Department will load the
                    // related course entities automatically
                    // Note: here for each department, there will be a 
                    // seperate database call to load the course entities
                    foreach (var course in department.Courses)
                    {
                        Console.WriteLine(course.Title);
                    }

                    Console.WriteLine();
                }
            }
        }


        /// <summary>
        /// Demostrates how to use eager loading for related entities 
        /// </summary>
        static void EagerLoadingTest()
        {
            Console.WriteLine("---Eager Loading---");

            using (LazyLoadingEntities context = new LazyLoadingEntities())
            {
                // Lazy loading is enabled dy default in EF4, so turn it off 
                // to use eager loading later
                context.ContextOptions.LazyLoadingEnabled = false;

                // Query the department entities which Budget is not NULL
                // Here we use the .Include() method to eager load the related
                // course entities
                var departments = from d in context.Departments.Include("Courses")
                                  where d.Budget != null
                                  select d;

                foreach (var department in departments)
                {
                    Console.WriteLine("Department: {0}", department.Name);

                    Console.WriteLine("Courses:");

                    // The related course entities has been already loaded
                    // Note: for eager loading, there will be only one
                    // database call to load the department and corresponding
                    // course entities
                    foreach (var course in department.Courses)
                    {
                        Console.WriteLine(course.Title);
                    }

                    Console.WriteLine();
                }
            }
        }


        /// <summary>
        /// Demostrates how to use explicit loading for related entities 
        /// </summary>
        static void ExplicitLoadingTest()
        {
            Console.WriteLine("---Explicit Loading---");

            using (LazyLoadingEntities context = new LazyLoadingEntities())
            {
                // Lazy loading is enabled dy default in EF4, so turn it off 
                // to use explicit loading later
                context.ContextOptions.LazyLoadingEnabled = false;

                // Query the department entities which Budget is not NULL
                var departments = from d in context.Departments
                                  where d.Budget != null
                                  select d;

                foreach (var department in departments)
                {
                    Console.WriteLine("Department: {0}", department.Name);

                    Console.WriteLine("Courses:");

                    // Explicit load the related courses entities if they
                    // are not loaded yet
                    // Note: here for each department, there will be a 
                    // seperate database call to load the course entities
                    if (!department.Courses.IsLoaded)
                    {
                        department.Courses.Load();
                    }

                    foreach (var course in department.Courses)
                    {
                        Console.WriteLine(course.Title);
                    }

                    Console.WriteLine();
                }
            }
        }
    }
}
