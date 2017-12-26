/****************************** Module Header ******************************\
* Module Name:    One2ManyClass.cs
* Project:        CSEFEntityDataModel
* Copyright (c) Microsoft Corporation.
*
* This example illustrates how to dinsert, update and query the two entities 
* with one to many association.
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
using System.Collections.Generic;
using System.Linq;
using System.Text;
#endregion


namespace CSEFEntityDataModel.One2Many
{
    public static class One2ManyClass
    {
        // Test all the methods in One2ManyClass
        public static void One2ManyTest()
        {
            InsertDepartmentWithCourse();

            InsertCourse();

            UpdateCourse();

            UpdateDepartment();
        }


        // Insert new department with new course
        public static void InsertDepartmentWithCourse()
        {
            using (EFO2MEntities context = new EFO2MEntities())
            {
                Department department = new Department() 
                { 
                    DepartmentID = 4, 
                    Name = "Software Engineering" 
                };

                Course course = new Course() 
                { 
                    CourseID = 2202, 
                    Title = "ADO.NET" 
                };

                department.Course.Add(course);

                context.AddToDepartment(department);

                try
                {
                    Console.WriteLine("Inserting department {0} with course "
                        + "{1}", department.Name, course.Title);

                    context.SaveChanges();

                    Query();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        // Insert new course to existing department
        public static void InsertCourse()
        {
            using (EFO2MEntities context = new EFO2MEntities())
            {
                Course course = new Course() 
                { 
                    CourseID = 2203, 
                    Title = "Object Oriented Programming" 
                };

                course.Department = (
                    from p in context.Department 
                    where p.DepartmentID == 7 
                    select p).First();

                context.AddToCourse(course);

                try
                {
                    Console.WriteLine("Inserting course {0} to department "
                        + "{1}", course.Title, course.Department.Name);

                    context.SaveChanges();

                    Query();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        // Get all the departments with courses
        public static void Query()
        {
            using (EFO2MEntities context = new EFO2MEntities())
            {
                var query = from p in context.Department.Include("Course") 
                            select p;

                Console.WriteLine("Deparments with their courses:");
                foreach (Department d in query)
                {
                    Console.WriteLine("{0} {1}", d.DepartmentID, d.Name);

                    foreach (Course c in d.Course)
                    {
                        Console.WriteLine("   {0}", c.Title);
                    }
                }

                Console.WriteLine();
            }
        }


        // Update one existing course
        public static void UpdateCourse()
        {
            using (EFO2MEntities context = new EFO2MEntities())
            {
                Course course = new Course();

                course.CourseID = 2203;

                context.AttachTo("Course", course);

                course.Title = "OOP";

                try
                {
                    Console.WriteLine("Modifying Course 2203's Title to {0}", 
                        course.Title);

                    context.SaveChanges();

                    Query();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        // Update one existing department 
        public static void UpdateDepartment()
        {
            using (EFO2MEntities context = new EFO2MEntities())
            {
                Department department = new Department();

                department.DepartmentID = 1;

                context.AttachTo("Department", department);

                department.Name = "Computer Engineering";

                department.Course.Add(new Course() 
                { 
                    CourseID = 2204, 
                    Title = "Arithmetic" 
                });

                try
                {
                    Console.WriteLine("Modifying Department 1's Title to {0}"
                        + ", and insert a new Course 2204 into the " + 
                        "Department 1", department.Name);

                    context.SaveChanges();

                    Query();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }
    }
}
