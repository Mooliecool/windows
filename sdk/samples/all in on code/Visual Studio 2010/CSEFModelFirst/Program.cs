/****************************** Module Header ******************************\
* Module Name:    Program.cs
* Project:        CSEFModelFirst
* Copyright (c) Microsoft Corporation.
*
* The CSEFModelFirst example illustrates how to use the new feature Model
* First in the Entity Framework 4.0 to generate DDL that will create a 
* database according to the Entity Data Model.  It also demostrates some 
* insert and query operations to test the generated database. 
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

namespace CSEFModelFirst
{
    class Program
    {
        static void Main(string[] args)
        {
            InsertEntities();
            QueryEntities();
        }


        /// <summary>
        /// Insert new related entities into the database that is created by
        /// the DDL
        /// </summary>
        private static void InsertEntities()
        {
            using (ModelFirstContainer context = new ModelFirstContainer())
            {
                // Create a new Department entity.
                Department department = new Department
                {
                    DepartmentID = 1,
                    Name = "Computer Science",
                    Budget = 400000,
                    StartDate = DateTime.Now
                };

                // Create a new Course entity.
                // Note: Here we only use the entity reference or entity set
                // to set the relationship.  In EF 4.0, we can also use the 
                // Foreign Key properties. For detail, please see examples 
                // CSEFForeignKeyAssociation.
                Course course1 = new Course
                {
                    CourseID = 1001,
                    Title = "Operation System",
                    Credits = 4,
                    // Set the relationship to the Department entity.
                    Department = department
                };

                // Create another new Course entity.
                Course course2 = new Course
                {
                    CourseID = 1002,
                    Title = ".NET Framework",
                    Credits = 3,
                    // Set the relationship to the Department entity.
                    Department = department
                };

                // Create a new Instructor.
                Instructor instructor1 = new Instructor
                {
                    PersonID = 1,
                    // Set Complex Properties.
                    Name = new Name
                    {
                        FirstName = "Jialiang",
                        LastName = "Ge"
                    },
                    Address = new Address
                    {
                        Country = "China",
                        City = "Shanghai",
                        Zipcode = "200000"
                    },
                    HireDate = DateTime.Now,
                };

                // Add the relationship.
                instructor1.Courses.Add(course1);

                // Create another new Instructor.
                Instructor instructor2 = new Instructor
                {
                    PersonID = 2,
                    // Set Complex Properties.
                    Name = new Name
                    {
                        FirstName = "Colbert",
                        LastName = "Zhou"
                    },
                    Address = new Address
                    {
                        Country = "China",
                        City = "Shanghai",
                        Zipcode = "200000"
                    },
                    HireDate = DateTime.Now
                };

                // Add the relationship.
                instructor2.Courses.Add(course2);

                // Create a new Student entity.
                Student student = new Student
                {
                    PersonID = 3,
                    // Set Complex Properties.
                    Name = new Name
                    {
                        FirstName = "Lingzhi",
                        LastName = "Sun"
                    },
                    Address = new Address
                    {
                        Country = "China",
                        City = "Shanghai",
                        Zipcode = "200000"
                    },
                    EnrollmentDate = DateTime.Now,
                    Degree = 1
                };

                // Add the relationships.
                student.CourseStudents.Add(new CourseStudent
                {
                    Course = course1,
                    Score = 90
                });
                student.CourseStudents.Add(new CourseStudent
                {
                    Course = course2,
                    Score = 95
                });

                try
                {
                    // Note: We can only add the Department entity because
                    // all the other related entities will be added as well.
                    context.AddToDepartments(department);
                    context.SaveChanges();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }
            }
        }


        /// <summary>
        /// Query the related entities from the database that is created by
        /// the DDL
        /// </summary>
        private static void QueryEntities()
        {
            using (ModelFirstContainer context = new ModelFirstContainer())
            {
                // Display all the Instructor and Student typed People 
                // entities.
                foreach (var p in context.People)
                {
                    // Check if it is an Instructor entity.
                    if (p is Instructor)
                    {
                        // Display the Instructor Name and Course information.
                        // Note: The ToStrin() methods of the Complex Types
                        // Name and Address have been overriden. 
                        Console.WriteLine("Instructor {0}'s courses:", p.Name);
                        foreach (var c in ((Instructor)p).Courses)
                        {
                            Console.WriteLine(c.Title);
                        }
                    }

                    // Check if it is a Student entity.
                    if (p is Student)
                    {
                        // Display the Student Name and Course information.
                        Console.WriteLine("Student {0}'s courses and scores:", 
                            p.Name);
                        foreach (var cs in ((Student)p).CourseStudents)
                        {
                            Console.WriteLine("{0} (Score:{1})", 
                                cs.Course.Title, cs.Score);
                        }
                    }

                    // Display each Person's Address information.
                    Console.WriteLine("Address:{0}", p.Address);
                    Console.WriteLine();
                }
            }
        }
    }


    // Complex Type Name partial class. 
    public partial class Name
    {
        // Override the ToString() method.
        public override string ToString()
        {
            return this.FirstName + " " + this.LastName;
        }
    }


    // Complex Type Address partial class.
    public partial class Address
    {
        // Override the ToString() method.
        public override string ToString()
        {
            return string.Format("{0}, {1} (Zipcode:{2})", this.City, 
                this.Country, this.Zipcode);
        }
    }
}
