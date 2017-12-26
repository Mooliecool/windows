/****************************** Module Header ******************************\
* Module Name:  Program.cs
* Project:      CSADONETDataServiceClient
* Copyright (c) Microsoft Corporation.
* 
* CSADONETDataServiceClient example demonstrates an ADO.NET Data Service
* client application. It calls certain ADO.NET Data Services for ADO.NET
* Entity Data Model, Linq To SQL Data Classes, and non-relational in-memory
* data, and it demonstrates these ways (LINQ, ADO.NET query options, custom
* service operations) to update and query the data source. 
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
using System.Data.Services.Client;
using System.Data.Services.Common;
using CSADONETDataServiceClient.SamplesService;
using System.Xml.Linq;
#endregion


namespace CSADONETDataServiceClient
{
    class Program
    {
        #region ADO.NET Data Services URLs
        // The URLs need to be modified according to ADO.NET Data Services
        // settings
        private static string schoolLinqToEntitiesUri =
            "http://localhost:8888/SchoolLinqToEntities.svc";
        private static string schoolLinqToSQLUri =
            "http://localhost:8888/SchoolLinqToSQL.svc";
        private static string AIOUri =
            "http://localhost:8888/AIO.svc";
        #endregion

        static void Main(string[] args)
        {
            try
            {
                // Update and query relational database via LINQ to Entities
                UpdateQueryRelationalDataByLinqToEntities();

                // Update and query relational database via LINQ to SQL
                UpdateQueryRelationDataByLinqToSQL();

                // Query relational database via ADO.NET query options and
                // custom service operations
                QueryRelationDataByQueryOptionsAndServiceOperations();

                // Insert and query non-relational data
                UpdateQueryNonRelationalData();

            }
            catch (Exception ex)
            {
                // Retrieve the exception information if there is some
                // DataServiceException is thrown at the server side
                if (ex.InnerException is DataServiceClientException)
                {
                    // Parse the DataServieClientException
                    InnerDataServiceException innerException = 
                        ParseDataServiceClientException(ex.InnerException.
                        Message);

                    // Display the DataServiceClientException message
                    if (innerException != null)
                        Console.WriteLine("DataServiceException Message: " + 
                            innerException.Message);
                }
                else
                {
                    Console.WriteLine("The application throws the error: " +
                        ex.Message);
                }
            }
            finally
            {
                Console.ReadLine();
            }
        }

        #region Update and query methods
        /// <summary>
        /// Update and query relational database via LINQ to Entities
        /// </summary>
        private static void UpdateQueryRelationalDataByLinqToEntities()
        {
            Console.WriteLine("======================================");
            Console.WriteLine("Update and query data via LINQ to Entities:");
            Console.WriteLine("======================================");

            /////////////////////////////////////////////////////////////////
            // Initialize the DataService object for ADO.NET Entity Data 
            // Model
            //

            SchoolLinqToEntitiesService.SQLServer2005DBEntities svc = new 
                SchoolLinqToEntitiesService.SQLServer2005DBEntities(new Uri(
                    schoolLinqToEntitiesUri));


            /////////////////////////////////////////////////////////////////
            // Insert wrong data which the server side handles
            // 

            // Create a wrong instructor
            SchoolLinqToEntitiesService.Person wrongInstructor = new
                SchoolLinqToEntitiesService.Person()
            {
                FirstName = "Riquel",
                LastName = "Dong",
                HireDate = DateTime.Now,
                // Set invalid value for the PersonCategory
                PersonCategory = 3
            };

            Console.Write("Insert wrong Person information into database...");

            // Update the changes to the database
            if (UpdateData(svc))
                Console.WriteLine("Successfully!");
            else
            {
                Console.WriteLine("Failed!");
                try
                {
                    // Get the person update exception
                    svc.Execute<SchoolLinqToEntitiesService.Person>(new Uri(
                        "/GetPersonUpdateException", UriKind.Relative));
                }
                catch (Exception updateException)
                {
                    // Retrieve the exception information if there is some
                    // DataServiceException is thrown at the server side
                    if (updateException.InnerException is 
                        DataServiceClientException)
                    {
                        // Parse the DataServieClientException
                        InnerDataServiceException innerException =
                            ParseDataServiceClientException(
                            updateException.InnerException.Message);

                        // Display the DataServiceClientException message
                        if (innerException != null)
                            Console.WriteLine("DataServiceException Message: "
                                + innerException.Message);
                    }
                }

                // Detach the incorrect Person object
                svc.Detach(wrongInstructor);
            }


            /////////////////////////////////////////////////////////////////
            // Insert relational data into database 
            // 

            // Create a new instructor
            SchoolLinqToEntitiesService.Person newInstructor = new 
                SchoolLinqToEntitiesService.Person()
            {
                FirstName = "Riquel",
                LastName = "Dong",
                HireDate = DateTime.Now,
                PersonCategory = 2
            };

            // Create a new course
            SchoolLinqToEntitiesService.Course newCourse1 = new 
                SchoolLinqToEntitiesService.Course()
            {
                CourseID = 5010,
                Title = "Network",
                Credits = 4,
                DepartmentID = 1
            };

            // Create a new course
            SchoolLinqToEntitiesService.Course newCourse2 = new 
                SchoolLinqToEntitiesService.Course()
            {
                CourseID = 5020,
                Title = "Database",
                Credits = 3,
                DepartmentID = 1
            };

            // Add the newly-created instructor into context
            svc.AddToPerson(newInstructor);
            
            // Add the newly-created courses into context
            svc.AddToCourse(newCourse1);
            svc.AddToCourse(newCourse2);

            // Add relationships to the newly-created instructor and courses
            svc.AddLink(newCourse1, "Person", newInstructor);
            svc.AddLink(newCourse2, "Person", newInstructor);

            Console.Write("Insert related data into database...");

            // Update the changes to the database
            if (UpdateData(svc))
                Console.WriteLine("Successfully!");
            else
                Console.WriteLine("Faild!");

            Console.WriteLine();

            /////////////////////////////////////////////////////////////////
            // Query single data table via LINQ
            //

            // Get all the teachers whose first name is 'Roger'
            // LINQ operator and ADO.NET Data Service query option comparison
            // 'where'(LINQ) <==> 'filter'(Query Option)
            var teachers = from p in svc.Person
                           where p.PersonCategory == 2
                           && p.FirstName == "Roger"
                           select p;

            Console.WriteLine("All the teachers whose first name is 'Roger':");

            // Display the query results
            foreach (var t in teachers)
            {
                Console.WriteLine("{0} {1}", t.FirstName, t.LastName);
            }

            Console.WriteLine();


            // Get the third and fourth newly-enrolled students
            // LINQ operator and ADO.NET Data Service query option comparison
            // 'where'(LINQ) <==> 'filter'(Query Option)
            // 'orderby' (LINQ) <==> 'orderby' (Query Option)
            // 'Skip' (LINQ) <=> 'skip' (Query Option)
            // 'Take' (LINQ) <==> 'top' (Query Option)
            var students = (from p in svc.Person
                            where p.PersonCategory == 1
                            orderby p.EnrollmentDate descending
                            select p).Skip(2).Take(2);

            Console.WriteLine("The third and fourth newly-enrolled students:");

            // Display the query results
            foreach (var s in students)
            {
                Console.WriteLine("{0} {1}", s.FirstName, s.LastName);
            }

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Query relational data tables via LINQ and custom service 
            // operation
            //

            // Get the instructors whose name is 'Riquel Dong'
            var instructors = from p in svc.Person
                              where p.LastName == "Dong" &&
                              p.FirstName == "Riquel"
                              select p;

            // Get all the courses that 'Riquel Dong' owns
            foreach (var i in instructors)
            {
                // Call the service operation CoursesByPersonID to get the 
                // certain person's courses based on primary key PersonID
                Uri uri = new Uri(String.Format("/CoursesByPersonID?ID={0}",
                    i.PersonID), UriKind.Relative);

                // Exceute the URL to the retrieve the course list
                var courses = svc.Execute<SchoolLinqToEntitiesService.Course>
                    (uri);

                Console.WriteLine("The instructor {0}'s couses:", 
                    i.FirstName + " " + i.LastName);
                
                // Display the query results
                foreach (var c in courses)
                {
                    Console.WriteLine("Course Title: {0}, Credits: {1}", 
                        c.Title, c.Credits);
                }
            }

            Console.WriteLine();
        }


        /// <summary>
        /// Update and query relational database via LINQ to SQL
        /// </summary>
        private static void UpdateQueryRelationDataByLinqToSQL()
        {
            Console.WriteLine("======================================");
            Console.WriteLine("Update and query data via LINQ to SQL:");
            Console.WriteLine("======================================");

            /////////////////////////////////////////////////////////////////
            // Initialize the DataService object for LINQ to SQL Data Class
            //

            SchoolLinqToSQLService.SchoolLinqToSQLDataContext svc = new 
                SchoolLinqToSQLService.SchoolLinqToSQLDataContext(new Uri(
                    schoolLinqToSQLUri));


            /////////////////////////////////////////////////////////////////
            // Insert relational data into database 
            // 

            // Create a new student
            SchoolLinqToSQLService.Person newStudent = new 
                SchoolLinqToSQLService.Person()
            {
                FirstName = "Lingzhi",
                LastName = "Sun",
                HireDate = DateTime.Now,
                PersonCategory = 1,
            };

            // Create a new course
            SchoolLinqToSQLService.Course newCourse1 = new 
                SchoolLinqToSQLService.Course()
            {
                CourseID = 5030,
                Title = "Operation System",
                Credits = 4,
                DepartmentID = 1
            };

            // Create a new course
            SchoolLinqToSQLService.Course newCourse2 = new 
                SchoolLinqToSQLService.Course()
            {
                CourseID = 5040,
                Title = ".NET Framework",
                Credits = 3,
                DepartmentID = 1
            };

            // Add the newly-created instructor into context
            svc.AddToPersons(newStudent);

            // Add the newly-created courses into context
            svc.AddToCourses(newCourse1);
            svc.AddToCourses(newCourse2);

            Console.Write("Insert data into database...");

            // Update the changes to the database
            if (UpdateData(svc))
                Console.WriteLine("Successfully!");
            else
                Console.WriteLine("Failed!");

            // Create a CourseGrade object
            SchoolLinqToSQLService.CourseGrade newCourseGrade1 = new 
                SchoolLinqToSQLService.CourseGrade()
            {
                StudentID = newStudent.PersonID,
                CourseID = newCourse1.CourseID,
                Grade = new decimal(3.00)
            };

            // Create a CourseGrade object
            SchoolLinqToSQLService.CourseGrade newCourseGrade2 = new 
                SchoolLinqToSQLService.CourseGrade()
            {
                StudentID = newStudent.PersonID,
                CourseID = newCourse2.CourseID,
                Grade = new decimal(3.00)
            };

            // Add the newly-created CourseGrade objects into context 
            svc.AddToCourseGrades(newCourseGrade1);
            svc.AddToCourseGrades(newCourseGrade2);

            Console.Write("Insert data relationships into database...");

            // Update the changes to the database
            if (UpdateData(svc))
                Console.WriteLine("Successfully!");
            else
                Console.WriteLine("Failed!");

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Query single data table via LINQ
            //

            // Get all the teachers whose first name is 'Roger'
            // LINQ operator and ADO.NET Data Service query option comparison
            // 'where'(LINQ) <==> 'filter'(Query Option)
            var teachers = from p in svc.Persons
                           where p.PersonCategory == 2
                           && p.FirstName == "Roger"
                           select p;

            Console.WriteLine("All the teachers whose first name is 'Roger':");
            
            // Display the query results
            foreach (var t in teachers)
            {
                Console.WriteLine("{0} {1}", t.FirstName, t.LastName);
            }

            Console.WriteLine();


            // Get the third and fourth newly-enrolled students
            // LINQ operator and ADO.NET Data Service query option comparison
            // 'where'(LINQ) <==> 'filter'(Query Option)
            // 'orderby' (LINQ) <==> 'orderby' (Query Option)
            // 'Skip' (LINQ) <=> 'skip' (Query Option)
            // 'Take' (LINQ) <==> 'top' (Query Option)
            var students = (from p in svc.Persons
                            where p.PersonCategory == 1
                            orderby p.EnrollmentDate descending
                            select p).Skip(2).Take(2);

            Console.WriteLine("The third and fourth newly-enrolled students:");

            // Display the query results
            foreach (var s in students)
            {
                Console.WriteLine("{0} {1}", s.FirstName, s.LastName);
            }

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Query relational data tables via LINQ
            //

            // Get the students whose name is 'Lingzhi Sun'
            var studentsWithCourses = from p in svc.Persons
                                      where p.LastName == "Sun" &&
                                      p.FirstName == "Lingzhi"
                                      select p;

            // Get all the courses that' Lingzhi Sun' learns
            foreach (var s in studentsWithCourses)
            {
                // Create URL to get the certain person's CourseGrade list 
                // based on primary key PersonID
                Uri uri = new Uri(String.Format("/Persons({0})/CourseGrades", 
                    s.PersonID), UriKind.Relative);

                // Exceute the URL to the retrieve the CourseGarde list 
                var courseGrades = svc.Execute<SchoolLinqToSQLService.
                    CourseGrade>(uri);

                Console.WriteLine("The student {0}'s courses:", s.FirstName 
                    + " " + s.LastName);

                foreach (var cg in courseGrades)
                {
                    // Create URL to get the certain course list based on 
                    // primary key CourseID
                    uri = new Uri(String.Format("/Courses({0})", cg.CourseID),
                        UriKind.Relative);

                    // Exceute the URL to the retrieve the course list  
                    var course = svc.Execute<SchoolLinqToSQLService.Course>
                        (uri).First();

                    // Display the query results
                    Console.WriteLine("Course Title: {0}, Credits: {1}",
                       course.Title, course.Credits);
                }
            }

            Console.WriteLine();
        }


        /// <summary>
        /// Query relational database via ADO.NET query options and custom 
        /// service operations
        /// </summary>
        private static void QueryRelationDataByQueryOptionsAndServiceOperations()
        {
            Console.WriteLine("======================================");
            Console.WriteLine("Query data via ADO.NET query options\n" +
                "and custom service operations:");
            Console.WriteLine("======================================");

            /////////////////////////////////////////////////////////////////
            // Initialize the DataService object
            //

            DataServiceContext context = new DataServiceContext(new Uri(
                schoolLinqToEntitiesUri));


            /////////////////////////////////////////////////////////////////
            // Query single data table via ADO.NET query options
            //

            // Get all the teachers whose first name is 'Roger'
            // LINQ operator and ADO.NET Data Service query option comparison
            // 'where'(LINQ) <==> 'filter'(Query Option)
            var teachers = context.Execute<SchoolLinqToEntitiesService.
                Person>(new Uri("/Person?$filter=PersonCategory eq 2 and " + 
                    "FirstName eq 'Roger'", UriKind.Relative));

            Console.WriteLine("All the teachers whose first name is 'Roger':");

            // Display the query results
            foreach (var t in teachers)
            {
                Console.WriteLine("{0} {1}", t.FirstName, t.LastName);
            }

            Console.WriteLine();

            // Get the third and fourth newly-enrolled students
            // LINQ operator and ADO.NET Data Service query option comparison
            // 'where'(LINQ) <==> 'filter'(Query Option)
            // 'orderby' (LINQ) <==> 'orderby' (Query Option)
            // 'Skip' (LINQ) <=> 'skip' (Query Option)
            // 'Take' (LINQ) <==> 'top' (Query Option)
            var students = context.Execute<SchoolLinqToEntitiesService.Person>
                (new Uri("/Person?$filter=PersonCategory eq 1&$" + 
                    "orderby=EnrollmentDate desc&$skip=2&$top=2", 
                    UriKind.Relative));

            Console.WriteLine("The third and fourth newly-enrolled students:");

            // Display the query results
            foreach (var s in students)
            {
                Console.WriteLine("{0} {1}", s.FirstName, s.LastName);
            }

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Query single data table via service operations and the result 
            // entity class misses some properties
            //

            // Initialize a new DataServiceContext
            DataServiceContext contextSQL = new DataServiceContext(new Uri(
                schoolLinqToSQLUri));

            // Set tje IgnoreMissingProperties to ture to retrieve entities
            // that miss properties
            contextSQL.IgnoreMissingProperties = true;

            // SQL search command
            string searchText = "SELECT * FROM [Course] AS [c] WHERE " +
                "[c].[Credits] = 4";

            // Query the SQL commands at the server side
            var courses = contextSQL.Execute<TempCourse>(new Uri(
                string.Format("/SearchCourses?searchText='{0}'", searchText),
                UriKind.Relative));

            Console.WriteLine("All The Courses which credits is 4:");
            Console.WriteLine("Course ID        Title");

            // Display the query results
            foreach (var c in courses)
            {
                Console.WriteLine("  {0}          {1}", c.CourseID, 
                    c.Title);
            }

            Console.WriteLine();
        }


        /// <summary>
        /// Insert and query non-relational data
        /// </summary>
        private static void UpdateQueryNonRelationalData()
        {
            Console.WriteLine("======================================");
            Console.WriteLine("Insert and query non-relational data:");
            Console.WriteLine("======================================");

            /////////////////////////////////////////////////////////////////
            // Initialize the DataService object for non-relational data
            //

            DataServiceContext svc = new DataServiceContext(new Uri(
                AIOUri));


            /////////////////////////////////////////////////////////////////
            // Insert non-relational data
            //

            Console.Write("Insert non-relational data...");

            Category newCategory = new Category()
            {
                CategoryName = "Silverlight1",
            };

            svc.AddObject("Categories", newCategory);

            // Update the changes to the non-relational data
            if (UpdateData(svc))
                Console.WriteLine("Successfully!");
            else
                Console.WriteLine("Failed!");

            Console.WriteLine();

            // Get all the category objects
            var categories = svc.Execute<Category>(new Uri("/Categories",
                UriKind.Relative));

            Console.WriteLine("Categories after insert operation:");

            // Display the query results
            foreach (var c in categories)
            {
                Console.WriteLine(c.CategoryName);
            }

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Query non-relational data
            //

            // Get all the project objects to the local memory
            var projects = svc.Execute<Project>(new Uri("/Projects", 
                UriKind.Relative)).ToList();

            //var categories2 = svc.Execute<Project>(new Uri("/Categories",
            //    UriKind.Relative)).ToList();

            foreach (var p in projects)
            {
                // Create URL to get the project category information based
                // on primary key ProjectName 
                Uri uri = new Uri(String.Format("/Projects('{0}')/" + 
                    "ProjectCategory", p.ProjectName), UriKind.Relative);

                // Set the ProjectCategory property
                Category category = svc.Execute<Category>(uri).First();
                p.ProjectCategory = category;
            }

            // Group the projects by project category
            var projectsGroups = from p in projects
                                 group p by p.ProjectCategory;

            Console.WriteLine("Group the AIO projects by category name:");

            // Display the query results
            foreach (var g in projectsGroups)
            {
                Console.WriteLine("Projects in Category: {0}", 
                    g.Key.CategoryName);
                
                foreach (var p in g)
                {
                    Console.WriteLine("{0} owned by {1}", p.ProjectName, 
                        p.Owner);
                }

                Console.WriteLine();
            }
        }


        /// <summary>
        /// Update the changes to the data at the server side
        /// </summary>
        /// <param name="context">The DataServiceContext to be updated.
        /// </param>
        /// <returns>Whether the update is successful.</returns>
        private static bool UpdateData(DataServiceContext context)
        {
            try
            {
                // Get the response from the DataServiceContext update 
                // operation
                DataServiceResponse response = context.SaveChanges();

                bool isSucess = false;

                foreach (var r in response)
                {
                    // If response status code shows the update fails
                    // return false
                    if (!r.StatusCode.ToString().StartsWith("2"))
                        return false;
                    else
                        isSucess = true;
                }

                // The update is successful
                return isSucess;
            }
            catch (Exception ex)
            {
                // Retrieve the exception information if there is some
                // DataServiceException is thrown at the server side
                if (ex.InnerException is DataServiceClientException)
                {
                    // Parse the DataServieClientException
                    InnerDataServiceException innerException =
                        ParseDataServiceClientException(ex.InnerException.
                        Message);

                    // Display the DataServiceClientException message
                    if (innerException != null)
                        Console.WriteLine("DataServiceException Message: " +
                            innerException.Message);
                }
                else
                {
                    Console.WriteLine("The update operation throws the error: "
                        + ex.Message);
                }
                
                return false;
            }
        }


        /// <summary>
        /// Parse the DataServiceClientException to get the error code 
        /// and message
        /// </summary>
        /// <param name="exception">The DataServiceClientException message
        /// </param>
        /// <returns>The local InnerDataServiceException class</returns>
        private static InnerDataServiceException 
            ParseDataServiceClientException(string exception)
        {
            try
            {
                // The DataServiceClientException XML namespace
                XNamespace ns = 
                    "http://schemas.microsoft.com/ado/2007/08/dataservices/metadata";

                // Load the DataServiceClientException by XDocument
                XDocument doc = XDocument.Parse(exception);

                // Return the error code and message
                return new InnerDataServiceException 
                { 
                    Code = String.IsNullOrEmpty(
                        doc.Root.Element(ns + "code").Value) ? 400 : 
                        int.Parse(doc.Root.Element(ns + "code").Value), 
                    Message = doc.Root.Element(ns + "message").Value 
                };
            }
            catch (Exception ex)
            {
                Console.WriteLine(
                    "Exceptions when parsing the DataServiceClientException: " 
                    + ex.Message);
                return null;
            }
        }
        #endregion
    }

    #region Local entity classes
    // The local course entity class that misses some properties
    // It only contains CourseID, Title, and Credits properties
    public class TempCourse
    {
        public int CourseID { get; set; }
        public string Title { get; set; }
    }

    // The local InnerDataServiceException to hold the DataServiceException
    // data from the server side
    public class InnerDataServiceException
    {
        public int Code { get; set; }
        public string Message { get; set; }
    }
    #endregion
}
