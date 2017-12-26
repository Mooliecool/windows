/****************************** Module Header ******************************\
* Module Name:    Program.cs
* Project:        CSEFCodeOnly
* Copyright (c) Microsoft Corporation.
*
* The CSEFCodeOnly example illustrates how to use the new feature Code Only
* in the Entity Framework 4.0 to use create the Entity Data Model metadata
* and the corresponding .edmx file with POCO entity classes and ObjectContext
* class at runtime.  It also demostrates some insert and query operations to 
* test the created the Entity Data Model metadata. 
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
using Microsoft.Data.Objects;
using System.Data.SqlClient;
using System.Data.Objects;
using System.Xml;
#endregion

namespace CSEFCodeOnly
{
    class Program
    {
        // Const connection string
        const string DB_CONN = @"Data Source=.\SQLEXPRESS;" + 
            "Initial Catalog=CodeOnlyDB;Integrated Security=SSPI;";

        static void Main(string[] args)
        {
            // Create a ContextBuilder to create the Entity Data Model 
            // metadata.
            ContextBuilder<CodeOnlyContainer> builder = 
                new ContextBuilder<CodeOnlyContainer>();

            // Create a SqlConnection to use the const connection string to 
            // connect the SQL Server Express
            SqlConnection conn = new SqlConnection(DB_CONN);
            
            // Register the entity configurations to create the Entity Data
            // Model metadata
            RegisterConfigurations(builder);

            // Create the .edmx file based on the EDM metadata
            CreateEDMX(builder, conn);
            Console.WriteLine();

            // Create the database based on EDM metadata, then insert and 
            // query relational entities
            CreateDatabaseAndInsertQueryEntities(builder, conn);
            Console.WriteLine();

            // Query the Type-per-Hierarchy inheriance entities
            QueryTPHData(builder, conn);
            Console.WriteLine();
        }


        /// <summary>
        /// Create the database based on EDM metadata, then insert and query
        /// relational entities
        /// </summary>
        /// <param name="builder">The certain ContextBuilder to create certain
        /// ObjectContext</param>
        /// <param name="conn">The certain SqlConnection to connect the 
        /// SQL Server Express</param>
        static void CreateDatabaseAndInsertQueryEntities(
            ContextBuilder<CodeOnlyContainer> builder, SqlConnection conn)
        {
            // Create the ObjectContext object by the ContextBuilder and 
            // SqlConnection
            using (CodeOnlyContainer context = builder.Create(conn))
            {
                // Check if the database exists
                if (context.DatabaseExists())
                {
                    // Delete it if it exists
                    context.DeleteDatabase();
                }
                // Create the database
                context.CreateDatabase();

                // Create a new Type-per-Table(TPT) inheritance Admin entity
                AdminTPT admin = new AdminTPT()
                {
                    // PK value
                    PersonID = 1,
                    // Two Complex Types properties, Name and Address
                    Name = new Name() 
                    { 
                        FirstName = "Jialiang", 
                        LastName = "Ge" 
                    },
                    Address = new Address() 
                    {
                        City = "Shanghai", 
                        Country = "China", 
                        Zipcode = "200030" 
                    },
                    AdminDate = DateTime.Now
                };

                // Add the newly created Admin entity into the ObjectContext
                context.PeopleTPT.AddObject(admin);

                // Create a new Deparment entity
                Department department = new Department()
                {
                    // PK value
                    DepartmentID = 1,
                    Name = "Computer Science",
                    Budget = 400000,
                    StartDate = DateTime.Now
                };

                // Create a new Course entity
                Course course = new Course()
                {
                    // PK value
                    CourseID = 1001,
                    Title = ".NET Framework",
                    Credits = 3,
                    // Set the relational entity by navigation property
                    Department = department,
                    // Initialize the Instructors navigation property
                    Instructors = new List<InstructorTPT>()
                };

                // Create a new TPT inheritance Instructor entity
                InstructorTPT instructor = new InstructorTPT()
                {
                    // PK value
                    PersonID = 2,
                    // Two Complex Types properties, Name and Address
                    Name = new Name() 
                    { 
                        FirstName = "Colbert", 
                        LastName = "Zhou" 
                    },
                    Address = new Address() 
                    { 
                        City = "Shanghai", 
                        Country = "China", 
                        Zipcode = "200030" 
                    },
                    HireDate = DateTime.Now
                };

                // Add the newly created Instructor entity into the 
                // Course's Instructor collection
                course.Instructors.Add(instructor);

                // Create a new TPT inheritance Student entity
                StudentTPT student = new StudentTPT()
                {
                    // PK value
                    PersonID = 3,
                    // Two Complex Types properties, Name and Address
                    Name = new Name() 
                    { 
                        FirstName = "Lingzhi", 
                        LastName = "Sun" 
                    },
                    Address = new Address() 
                    { 
                        City = "Shanghai", 
                        Country = "China", 
                        Zipcode = "200032" 
                    },
                    EnrollmentDate = DateTime.Now
                };

                // Create a new CourseStudent relationship entity
                CourseStudent courseStudent = new CourseStudent()
                {
                    // Set the navigation properties
                    Student = student,
                    Course = course,
                    Score = 90
                };

                // Add the CourseStudent relationship entity into the 
                // ObjectContext
                context.CourseStudents.AddObject(courseStudent);


                // Create a new Type-per-Hierarchy(TPH) inheritance Admin 
                // entity
                AdminTPH adminTPH = new AdminTPH()
                {
                    // PK value
                    PersonID = 1,
                    // Two Complex Types properties, Name and Address
                    Name = new Name() 
                    { 
                        FirstName = "Jialiang", 
                        LastName = "Ge" 
                    },
                    Address = new Address() 
                    { 
                        City = "Shanghai", 
                        Country = "China", 
                        Zipcode = "200032" 
                    },
                    AdminDate = DateTime.Now
                };

                // Create a new TPH inheritance Instructor entity
                InstructorTPH instructorTPH = new InstructorTPH()
                {
                    // PK value
                    PersonID = 2,
                    // Two Complex Types properties, Name and Address
                    Name = new Name() 
                    { 
                        FirstName = "Colbert", 
                        LastName = "Zhou" 
                    },
                    Address = new Address() 
                    { 
                        City = "Shanghai", 
                        Country = "China", 
                        Zipcode = "200030" 
                    },
                    HireDate = DateTime.Now
                };

                // Create a new TPH inheritance Student entity
                StudentTPH studentTPH = new StudentTPH()
                {
                    // PK value
                    PersonID = 3,
                    // Two Complex Types properties, Name and Address
                    Name = new Name() 
                    { 
                        FirstName = "Lingzhi", 
                        LastName = "Sun" 
                    },
                    Address = new Address() 
                    { 
                        City = "Shanghai", 
                        Country = "China", 
                        Zipcode = "200032" 
                    },
                    EnrollmentDate = DateTime.Now
                };

                // Add the TPH inheritance entities into the ObjectContext
                context.PeopleTPH.AddObject(adminTPH);
                context.PeopleTPH.AddObject(instructorTPH);
                context.PeopleTPH.AddObject(studentTPH);

                try
                {
                    // Saving the relational entities
                    Console.Write("Saving the relational entities...");
                    context.SaveChanges();
                    Console.WriteLine("Successfully!");
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Failed!");
                    Console.WriteLine(ex.ToString());
                }


                // Query the single Course entitiy from the ObjectContext
                var queryCourse = context.Courses.Single();

                // Display the Course informaion
                Console.WriteLine("Course: {0} under Department: {1}", 
                    queryCourse.Title, queryCourse.Department.Name);
                Console.WriteLine();

                // Query and display the Course's Instructors information
                Console.WriteLine("Course Instructors:");
                foreach (var i in queryCourse.Instructors)
                {
                    Console.WriteLine(i.Name);
                }
                Console.WriteLine();
                
                // Query and display the Course's Students information
                Console.WriteLine("Course Students:");
                foreach (var cs in queryCourse.CourseStudents)
                {
                    Console.WriteLine(cs.Student.Name);
                }
            }
        }


        /// <summary>
        /// Create the .edmx file based on the EDM metadata
        /// </summary>
        /// <param name="builder">The certain ContextBuilder to create the 
        /// .edmx file</param>
        /// <param name="conn">The certain SqlConnection to connect the 
        /// SQL Server Express</param>
        static void CreateEDMX(ContextBuilder<CodeOnlyContainer> builder, 
            SqlConnection conn)
        {
            // Create a XmlWriter object
            XmlWriter writer = new XmlTextWriter("CodeOnly.edmx", 
                Encoding.UTF8);
            try
            {
                // Write the .edmx file by the ContextBuilder
                Console.Write("Creating the .edmx file...");
                builder.WriteEdmx(conn, writer);
                Console.WriteLine("Successfully!");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Failed!");
                Console.WriteLine(ex.ToString());
            }
        }


        /// <summary>
        /// Query the TPH inheritance entities
        /// </summary>
        /// <param name="builder">The certain ContextBuilder to create the 
        /// ObjectContext</param>
        /// <param name="conn">The certain SqlConnection to connect the 
        /// SQL Server Express</param>
        static void QueryTPHData(ContextBuilder<CodeOnlyContainer> builder, 
            SqlConnection conn)
        {
            // Create the ObjectContext object by the ContextBuilder and 
            // SqlConnection
            using (CodeOnlyContainer context = builder.Create(conn))
            {
                // Query and display the TPH inheritance entities
                Console.WriteLine("Display People (TPH):");
                foreach (var peopleTPH in context.PeopleTPH)
                {
                    // Check if it is the Admin type
                    if (peopleTPH is AdminTPH)
                    {
                        var admin = (AdminTPH)peopleTPH;
                        Console.WriteLine("Admin:");
                        Console.WriteLine(
                            "ID: {0}, Name: {1}, AdminDate: {2}", 
                            admin.PersonID, admin.Name, admin.AdminDate);
                    }
                    // Check if it is the Student type
                    else if (peopleTPH is StudentTPH)
                    {
                        var student = (StudentTPH)peopleTPH;
                        Console.WriteLine("Student:");
                        Console.WriteLine(
                            "ID: {0}, Name: {1}, EnrollmentDate: {2}", 
                            student.PersonID, student.Name, 
                            student.EnrollmentDate);
                    }
                    // Check if it is the Instructor type
                    else if (peopleTPH is InstructorTPH)
                    {
                        var instructor = (InstructorTPH)peopleTPH;
                        Console.WriteLine("Instructor:");
                        Console.WriteLine(
                            "ID: {0}, Name: {1}, HireDate: {2}", 
                            instructor.PersonID, instructor.Name, 
                            instructor.HireDate);
                    }
                }
            }
        }


        /// <summary>
        /// Use the ContextBuilder to create the EDM metadata
        /// </summary>
        /// <param name="builder">The certain ContextBuilder to create the 
        /// EDM metadata</param>
        static void RegisterConfigurations(
            ContextBuilder<CodeOnlyContainer> builder)
        {
            // Config the Complex Types, Name and Address
            builder.Configurations.Add(new ComplexTypeNameConfiguration());
            builder.Configurations.Add(new ComplexTypeAddressConfiguration());

            // Config the TPT inheritance entities
            builder.Configurations.Add(new PersonTPTConfiguration());
            builder.Configurations.Add(new StudentTPTConfiguration());
            builder.Configurations.Add(new InstructorTPTConfiguration());
            builder.Configurations.Add(new AdminTPTConfiguration());
            builder.Configurations.Add(new BusinessStudentTPTConfiguration());

            // Config the TPH inheritance entities
            builder.Configurations.Add(new PersonTPHConfiguration());

            // Config other relational entities
            builder.Configurations.Add(new CourseStudentConfiguration());
            builder.Configurations.Add(new CourseConfiguration());
            builder.Configurations.Add(new DepartmentConfiguration());
        }
    }
}
