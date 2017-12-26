/****************************** Module Header ******************************\
* Module Name:    Program.cs
* Project:        CSLinqToEntities
* Copyright (c) Microsoft Corporation.
*
* The CSLinqToEntities example demonstrates the Microsoft Language-Integrated 
* Query (LINQ) technology to access ADO.NET Entity Data Model using Visual C#. 
* It shows the basic structure of connecting and querying the data source and
* inserting data into the database with LINQ.  It especially demonstrates the
* new features of LINQ to Entities comparing with LINQ to SQL, Inheritance
* and Many-to-Many Relationship (without payload).
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* History:
* * 4/7/2009 09:00 PM Lingzhi Sun Created
\***************************************************************************/

#region Using directives
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CSLinqToEntities;
using System.IO;
#endregion


class Program
{
    static void Main(string[] args)
    {
        try
        {
            /////////////////////////////////////////////////////////////////
            // Construct the object context
            // 

            // Create Entity ObectContext for School database
            SchoolEntities entities = new SchoolEntities();


            /////////////////////////////////////////////////////////////////
            // Perform the Insert operation
            // 

            Console.Write("Insert data into database...");

            // Create a Student object which inherits the Person object
            Student newStudent = new Student()
            {
                FirstName = "Lingzhi",
                LastName = "Sun",
                EnrollmentDate = DateTime.Now,
                Picture = ReadImage(@"MSDN.jpg")
            };

            // Add the Student object into the Person list
            entities.AddToPerson(newStudent);

            // Create a Instructor object which inherits the Person object
            Instructor newInstructor = new Instructor()
            {
                FirstName = "Jialiang",
                LastName = "Ge",
                HireDate = DateTime.Now,
                Picture = ReadImage(@"MSDN.jpg")
            };

            // Create a Course object
            Course newCourse1 = new Course()
            {
                CourseID = 5011,
                Title = "Computer",
                Credits = 4,
                DepartmentID = 1
            };

            // Create a Course object
            Course newCourse2 = new Course()
            {
                CourseID = 5023,
                Title = "Database",
                Credits = 3,
                DepartmentID = 1
            };

            // Add the new Course objects into the Instructor object's
            // Course list (Many-to-Many Relationship)
            newInstructor.Course.Add(newCourse1);
            newInstructor.Course.Add(newCourse2);

            // Add the Instructor object into the Person list
            // This command will also add the corresponding related Course
            // objects into the Course list
            // We don't need to call entities.AddToCourse method here
            entities.AddToPerson(newInstructor);

            // Update the changes to the database
            int result = entities.SaveChanges();

            if (result > 0)
                Console.WriteLine("SUCCESS\n");

            /////////////////////////////////////////////////////////////////
            // Perform the query operation in one data table
            // 

            Console.WriteLine("\nQuery students whose first name is "
                + "Roger:\n");

            // Perform the query
            var query = from p in entities.Person
                        where p.FirstName == "Roger"
                        select p;

            // Display the query results
            foreach (var p in query)
            {
                Console.WriteLine("ID = {0}, Name = {1} {2}", p.PersonID,
                    p.FirstName, p.LastName);
            }

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Perform the query operation across multiple data tables
            // 

            Console.WriteLine("\nQuery the max grade of each course:\n");

            // Perform the query and get an anonymous type collection
            var courses = from grade in entities.CourseGrade
                          group grade by grade.Course.CourseID into g
                          join cur in entities.Course
                          on g.Key equals cur.CourseID
                          select new
                          {
                              CourseID = g.Key,
                              Title = cur.Title,
                              TopGrade = g.Max(gra => gra.Grade == null ? 
                                  decimal.Zero : gra.Grade)
                          };

            // Display the query results
            foreach (var c in courses)
            {
                Console.WriteLine("Course = {0}, TopGrade = {1}", c.Title,
                    c.TopGrade);
            }

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Perform the query operation across multiple related data 
            // tables
            // 

            Console.WriteLine("\nQuery all the grades that Nino got:\n");

            // Perform the query between related data tables and get an 
            // anonymous type collection
            var grades = from per in entities.Person
                         from gra in per.CourseGrade
                         join cur in entities.Course
                         on gra.Course equals cur
                         where per.FirstName == "Nino"
                         select new
                         {
                             Person = per,
                             Course = cur,
                             CourseGrade = gra
                         };

            // Display the query results
            foreach (var grade in grades)
            {
                Console.WriteLine("FirstName = {0}, Course = {1}, Grade = "
                    + "{2}", grade.Person.FirstName, grade.Course.Title,
                    grade.CourseGrade.Grade);
            }

            Console.WriteLine();


            /////////////////////////////////////////////////////////////////
            // Perform the query operation accross Many-to-Many related 
            // data tables
            // 

            Console.WriteLine("\nQuery all the courses that Jialiang Ge " + 
                "owns:\n");

            // Perform the query across Many-to-Many related data tables and 
            // get an anonymous type collection
            var instructors = from i in entities.Person.OfType<Instructor>()
                              where i.LastName == "Ge" && i.FirstName == 
                              "Jialiang"
                              select new
                              {
                                  Name = i.FirstName + " " + i.LastName,
                                  Courses = i.Course
                              };

            // Display the query results
            foreach (var i in instructors)
            {
                Console.WriteLine("The instructors {0}'s couses:", i.Name);

                foreach (var c in i.Courses)
                {
                    Console.WriteLine("Course Title: {0}, Credits: {1}", 
                        c.Title, c.Credits);
                }
            }

            Console.Read();
        }
        catch (Exception e)
        {
            Console.WriteLine("The application throws the error: {0}", 
                e.Message);

            Console.Read();
        }
    }


    /// <summary>
    /// Read an image file to an array of bytes.
    /// </summary>
    /// <param name="path">The path of the image file.</param>
    /// <returns>The output of the array.</returns>
    private static byte[] ReadImage(string path)
    {
        try
        {
            // Open the image file
            using (FileStream fs = new FileStream(path, FileMode.Open,
                FileAccess.Read))
            {
                // Create an array of bytes
                byte[] bPicture = new byte[fs.Length];

                // Read the image file
                fs.Read(bPicture, 0, Convert.ToInt32(fs.Length));

                return bPicture;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine(
                "The application throws the error: {0}", ex.Message);
            return null;
        }
    }
}